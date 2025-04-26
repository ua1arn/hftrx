/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if LINUX_SUBSYSTEM && (DDS1_TYPE == DDS_TYPE_XDMA)

#include "pcie_dev.h"

#define ONE_MB (1024UL * 1024UL)

/* ltoh: little to host */
/* htol: little to host */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define ltohl(x)       (x)
#define ltohs(x)       (x)
#define htoll(x)       (x)
#define htols(x)       (x)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define ltohl(x)     __bswap_32(x)
#define ltohs(x)     __bswap_16(x)
#define htoll(x)     __bswap_32(x)
#define htols(x)     __bswap_16(x)
#endif

#define MAP_SIZE (64*1024UL)

/* helper struct to remember the Xdma device names */
typedef struct {
    char * base_path;  /* path to first found Xdma device */
	char * c2h0_path;     /* card to host DMA 0 */
    char * h2c0_path;     /* host to card DMA 0 */
	char * user_path;     /* XDMA AXI Lite interface */
    char * buffer_c2h;   /* pointer to the allocated buffer card to host*/
    char * buffer_h2c;   /* pointer to the allocated buffer host to card*/
    void * user_map_base; /* XDMA AXI Lite interface map base address */
    u_long buf_c2h_size; /* size of the buffer c2h in bytes */
    u_long buf_h2c_size; /* size of the buffer h2c in bytes */
    int c2h0;
    int h2c0;
    int user;
} xdma_device;

xdma_device xdma;

int pcie_init()
{
    int status = 1;

    xdma.h2c0_path = "/dev/xdma0_h2c_0";
    xdma.c2h0_path = "/dev/xdma0_c2h_0";
	xdma.user_path = "/dev/xdma0_user";

    /* allocate buffer */
    xdma.buf_c2h_size = 8 * ONE_MB;     // buffer card to host size
    if(posix_memalign((void **)&xdma.buffer_c2h, 1024, xdma.buf_c2h_size)) //512 | 1024
        if (xdma.buffer_c2h) {
            status = -1;
            free(xdma.buffer_c2h);
            fprintf(stderr, "Error allocate buffer , OOM--error code: %d\n", ENOMEM);
        }
	
    return status;
}

int pcie_open()
{
    int status = 1;

    /* open XDMA Host-to-Card 0 device */
    xdma.h2c0 = open(xdma.h2c0_path, O_RDWR);
    if (xdma.h2c0 < 0){
        printf("FAILURE: Could not open %s. Make sure xdma device driver is loaded and you have access rights (maybe use sudo?).\n", xdma.h2c0_path);
        status = -1;
        goto cleanup_handles;
    }

    /* open XDMA Card-to-Host 0 device */
    xdma.c2h0 = open(xdma.c2h0_path, O_RDWR);
    if (xdma.c2h0 < 0){
        fprintf(stderr, "unable to open device %s.\n", xdma.c2h0_path);
        status = -1;
        goto cleanup_handles;
    }

    /* open user device */
    if(access(xdma.user_path, F_OK) ==0){
        xdma.user = open(xdma.user_path, O_RDWR | O_SYNC);
        if (xdma.user < 0) {
            fprintf(stderr, "unable to open device %s.\n", xdma.user_path);
            status = -1;
            goto cleanup_handles;
        }
        xdma.user_map_base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, xdma.user, 0);
        if (xdma.user_map_base == MAP_FAILED){
            perror("mmap user error");
            status = -2;
            goto cleanup_handles;
        }
    }
    return status;

cleanup_handles:
    if (xdma.c2h0) close(xdma.c2h0);
    if (xdma.h2c0) close(xdma.h2c0);
    if (xdma.user) close(xdma.user);
    return status;
}

void pcie_close()
{
    if (xdma.c2h0) close(xdma.c2h0);
    if (xdma.h2c0) close(xdma.h2c0);
    if (xdma.user) close(xdma.user);
    if (xdma.buffer_c2h) free(xdma.buffer_c2h);
    if (xdma.buffer_h2c) free(xdma.buffer_h2c);
    if (xdma.user_map_base) munmap(xdma.user_map_base,MAP_SIZE);
}

static long write_device(int device, long offset, unsigned long size, char* snd_content)
{
    uint64_t count = 0;

    if (-1 == lseek(device, offset, SEEK_SET)){
        fprintf(stderr, "%s, seek off failed.\n", "xdma device write");
        return -EIO;
    }

	// write to device from allocated buffer
	count = write(device, snd_content, size);
    if (count != size) {
        fprintf(stderr, "WriteFile to XDMA failed.\n");
        perror("write file");
        return -EIO;
    }

    return count;
}

static long read_device(int device, long offset, unsigned long size, char* rcv_content)
{
    uint64_t count=0;

    if (-1 == lseek(device, offset, SEEK_SET)){
        fprintf(stderr, "%s, seek off failed.\n", "xdma device read");
        return -EIO;
    }

    // read from device into allocated buffer
    count = read(device, rcv_content, size);
    if (count != size) {
        fprintf(stderr, "ReadFile from XDMA failed\n");
		perror("read file");
        return -EIO;
    }
	
    return count;
}

//transfer data from Host PC to FPGA Card
long xdma_h2c_transfer(long offset, unsigned long size, char* snd_content)
{
    return write_device(xdma.h2c0, offset, size, snd_content);
}

//transfer data from FPGA Card to Host PC
long xdma_c2h_transfer(long offset, unsigned long size, char* rcv_content)
{
    if(size < 5){
        read_device(xdma.c2h0, offset, 10, xdma.buffer_c2h);
        memcpy(rcv_content, xdma.buffer_c2h, size);
        return size;
    }
    else
        return read_device(xdma.c2h0, offset, size, rcv_content);
}

void xdma_write_user(long offset, uint32_t value)
{
    /* swap 32-bit endianess if host is not little-endian */
	value = htoll(value);
    *((uint32_t *) (xdma.user_map_base + offset)) = value;
}

uint32_t xdma_read_user(long offset)
{
	uint32_t v = *((uint32_t *) (xdma.user_map_base + offset));
	/* swap 32-bit endianess if host is not little-endian */
	v = ltohl(v);
	return v;
}

#endif /* LINUX_SUBSYSTEM  && (DDS1_TYPE == DDS_TYPE_XDMA) */
