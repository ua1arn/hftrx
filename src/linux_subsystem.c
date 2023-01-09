/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints
#include "audio.h"

#if LINUX_SUBSYSTEM

#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <sys/time.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/gpio.h>

void * get_highmem_ptr (uint32_t addr)
{
	int fd;
	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
	{
		printf("open /dev/mem failure\n");
		return NULL;
	}

	void *map_base, *virt_addr;
	unsigned page_size, mapped_size, offset_in_page;
	mapped_size = page_size = 4096;
	offset_in_page = (unsigned)addr & (page_size - 1);

	map_base = (unsigned*) mmap(0, mapped_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, addr & ~(off_t)(page_size - 1));
	close(fd);

	if (map_base == NULL)
	{
		printf("mmap %x failure\n", addr);
		return NULL;
	}

	virt_addr = (char*) map_base + offset_in_page;
	return virt_addr;
}

void process_linux_timer_spool(void)
{
	while(1)
	{
		spool_systimerbundle1();
		spool_systimerbundle2();
		usleep(5000);
	}
}

void linux_encoder_spool(void)
{
	while(1)
	{
		spool_encinterrupt();
		usleep(100);
	}
}

/************************ Framebuffer *****************************/

static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
volatile uint32_t * fbp = 0;
static uint32_t screensize = 0;

uint32_t * linux_get_fb(uint32_t * size)
{
	* size = screensize;
	return (uint32_t *)fbp;
}

uint32_t * fbmem_at(uint_fast16_t x, uint_fast16_t y)
{
	uint32_t location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 32) +
	                   (y + vinfo.yoffset) * finfo.line_length / 4;
	return (uint32_t *) (fbp + location);
}

int linux_framebuffer_init(void)
{
	int fbfd, ttyd;

	// Open the file for reading and writing
	fbfd = open("/dev/fb0", O_RDWR);
	if (fbfd == -1) {
		PRINTF("Error: cannot open framebuffer device");
		exit(1);
	}

	ttyd = open("/dev/tty0", O_RDWR);
	if (ttyd)
		ioctl(ttyd, KDSETMODE, KD_GRAPHICS);
	else
	{
		PRINTF("Error: cannot open tty device");
		exit(1);
	}

	// Get fixed screen information
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
		PRINTF("Error reading fixed information");
		exit(2);
	}

	// Get variable screen information
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		PRINTF("Error reading variable information");
		exit(3);
	}

	// Figure out the size of the screen in bytes
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	// Map the device to memory
	fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (* fbp == -1) {
		PRINTF("Error: failed to map framebuffer device to memory");
		exit(4);
	}

	PRINTF("linux drm fb: %dx%d, %dbpp, %d bytes\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, screensize);

	close(fbfd);
	close(ttyd);
	return 0;
}

void framebuffer_close(void)
{
	int ttyd = open("/dev/tty0", O_RDWR);
    ioctl(ttyd, KDSETMODE, KD_TEXT);
    munmap((uint32_t *) fbp, screensize);
    close(ttyd);
}

/********************** EMIO ************************/

volatile uint32_t * xgpo, * xgpi;

void linux_xgpio_init(void)
{
	int fd;

	if((fd = open("/dev/mem", O_RDWR)) < 0)
	{
		PRINTF("open /dev/mem failed \n");
		return;
	}

	xgpo = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, XGPO_ADDR);
	xgpi = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, XGPI_ADDR);

	ASSERT(xgpo != NULL);
	ASSERT(xgpi != NULL);

	close(fd);
}

uint8_t linux_xgpi_read_pin(uint8_t pin)
{
	uint32_t v = * xgpi;
	return (v >> pin) & 1;
}

void linux_xgpo_write_pin(uint8_t pin, uint8_t val)
{
	uint32_t mask = 1 << pin;
	uint32_t rw = * xgpo;

	if (val)
		rw |= mask;
	else
		rw &= ~mask;

	* xgpo = rw;
}

void xc7z_writepin(uint8_t pin, uint8_t val)
{
	linux_xgpo_write_pin(pin, val);
}

uint_fast8_t xc7z_readpin(uint8_t pin)
{
	return linux_xgpi_read_pin(pin);
}

void gpio_writepin(uint8_t pin, uint8_t val)
{
	linux_xgpo_write_pin(pin, val);
}

uint_fast8_t gpio_readpin(uint8_t pin)
{
	return linux_xgpi_read_pin(pin);
}

void reg_write(uint32_t addr, uint32_t val)
{
	int fd;
	void *ptr;
	uint32_t page_addr, page_offset;
	uint32_t page_size=sysconf(_SC_PAGESIZE);

	fd=open("/dev/mem",O_RDWR);
	if(fd<1)
		err(errno, "Failed to open\n");

	page_addr=(addr & ~(page_size-1));
	page_offset=addr-page_addr;

	ptr=mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(addr & ~(page_size-1)));
	if((int)ptr==-1)
		err(errno, "Failed to mmap\n");

	*((unsigned *)(ptr+page_offset))=val;

	close(fd);

	//reg_read(addr);
	//printf("reg_write: 0x%08x, 0x%08x\n", addr, val);
}

uint32_t reg_read(uint32_t addr)
{
	int fd;
	void *ptr;
	uint32_t page_addr, page_offset;
	uint32_t page_size=sysconf(_SC_PAGESIZE);

	fd=open("/dev/mem", O_RDWR);
	if(fd<1)
		err(errno, "Failed to open\n");

	page_addr=(addr & ~(page_size-1));
	page_offset=addr-page_addr;

	ptr=mmap(NULL,page_size,PROT_READ,MAP_SHARED,fd,(addr & ~(page_size-1)));
	if((int)ptr==-1)
		err(errno, "Failed to mmap\n");

	uint32_t res = *((unsigned *)(ptr+page_offset));

	close(fd);

	printf("reg_read: 0x%08X, 0x%08X\n", addr, res);
	return res;
}

/************** SPI *********************/

#if WITHSPISW && WITHPS7BOARD_MYC_Y7Z020 // sv9 only
uint32_t * readreg, * writereg;
SPINLOCK_t lock_sclk, lock_miso, lock_mosi;

void linux_spi_init(void)
{
	uint32_t gpiobase = 0xE000A000;

	uint32_t * dirreg = get_highmem_ptr(gpiobase + 0x244);
	uint32_t * oenreg = get_highmem_ptr(gpiobase + 0x248);
	readreg = get_highmem_ptr(gpiobase + 0x64);
	writereg = get_highmem_ptr(gpiobase + 0x8);

	* oenreg |= ((1 << (SPI_MOSI_MIO - 32)) | (1 << (SPI_SCLK_MIO - 32)));
	* dirreg |= ((1 << (SPI_MOSI_MIO - 32)) | (1 << (SPI_SCLK_MIO - 32)));
	* dirreg &= ~(1 << (SPI_MISO_MIO - 32));

	SPINLOCK_INITIALIZE(& lock_sclk);
	SPINLOCK_INITIALIZE(& lock_miso);
	SPINLOCK_INITIALIZE(& lock_mosi);
}


void spi_sclk_set(void)
{
	* writereg |= (1 << (SPI_SCLK_MIO - 32));
}

void spi_sclk_clear(void)
{
	* writereg &= ~(1 << (SPI_SCLK_MIO - 32));
}

void spi_mosi_set(void)
{
	* writereg |= (1 << (SPI_MOSI_MIO - 32));
}

void spi_mosi_clear(void)
{
	* writereg &= ~(1 << (SPI_MOSI_MIO - 32));
}

uint8_t spi_miso_get(void)
{
	uint8_t r = (* readreg >> (SPI_MISO_MIO - 32)) & 1;
	return r;
}
#endif /* WITHSPISW */

/************* I2C ************************/
static int fd_i2c = 0;

void i2c_initialize(void)
{
	const char *path = "/dev/i2c-0";

	fd_i2c = open(path, O_RDWR);
	if (fd_i2c < 0)
		PRINTF("%s open failed: %d\n", path, fd_i2c);
	else
		PRINTF("linux i2c started\n");
}

uint16_t i2chw_write(uint16_t slave_address, uint8_t * buf, uint32_t size)
{
	int rc;

	if (fd_i2c)
	{
		if (ioctl(fd_i2c, I2C_SLAVE, slave_address >> 1) < 0)
			PRINTF("Failed to set slave\n");

		rc = write(fd_i2c, buf, size);
		if (rc < 0)
			PRINTF("Tried to write to address '0x%02x'\n", slave_address);
	}

	return rc;
}

uint16_t i2chw_read(uint16_t slave_address, uint8_t * buf, uint32_t size)
{
	int rc;

	if (fd_i2c)
	{
//		if (ioctl(fd_i2c, I2C_SLAVE, slave_address >> 1) < 0)
//			PRINTF("Failed to set slave\n");

		rc = read(fd_i2c, buf, size);
		if (rc < 0)
			PRINTF("Tried to read from address '0x%02x'\n", slave_address);
	}

	return rc;
}

uint16_t linux_i2c_read(uint16_t slave_address, uint16_t reg, uint8_t * buf, const uint8_t size)
{
	int rc;

	uint8_t regbuf[2] = { (reg >> 8), (reg & 0xFF), };

	if (fd_i2c)
	{
		if (ioctl(fd_i2c, I2C_SLAVE, slave_address >> 1) < 0)
			PRINTF("Failed to set slave\n");

		rc = write(fd_i2c, regbuf, 2);
		if (rc < 0)
			PRINTF("Tried to write to address '0x%02x'\n", slave_address);


		rc = read(fd_i2c, buf, size);
		if (rc < 0)
			PRINTF("Tried to read from address '0x%02x'\n", slave_address);
	}

	return rc;
}

/*************************************************************/

volatile uint32_t *ftw, *rts, *iq_shift, *iq_fifo, *ph_fifo, *fir_reload, *iq_count;
uint32_t sinbuf32[DMABUFFSIZE16TX];

void linux_iq_init(void)
{
	int fd;

	if((fd = open("/dev/mem", O_RDWR)) < 0)
	{
		PRINTF("open /dev/mem failed \n");
		return;
	}

	ftw = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x8004a000);
	ASSERT(ftw);
	rts = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x80042000);
	ASSERT(rts);
	iq_shift = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x80049000);
	ASSERT(iq_shift);
	iq_count = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, AXI_IQ_COUNT_ADDR);
	ASSERT(iq_count);
	iq_fifo = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, AXI_IQ_FIFO_ADDR);
	ASSERT(iq_fifo);
	ph_fifo = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, AXI_FIFO_PHONES_ADDR);
	ASSERT(ph_fifo);
#if WITHDSPEXTFIR
	fir_reload = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, AXI_FIR_RELOAD_ADDR);
	ASSERT(fir_reload);
#endif
	close(fd);

	reg_write(AXI_ADI_ADDR + AUDIO_REG_I2S_CLK_CTRL, (64 / 2 - 1) << 16 | (4 / 2 - 1));
	reg_write(AXI_ADI_ADDR + AUDIO_REG_I2S_PERIOD, DMABUFFSIZE16TX);
	reg_write(AXI_ADI_ADDR + AUDIO_REG_I2S_CTRL, TX_ENABLE_MASK);

	xcz_rx_iq_shift(52);

	uint32_t amp = 16384, ss = DMABUFFSIZE16TX;
    for(int i = 0; i < ss; i+=2)
    {
    	sinbuf32[i + 0] = cos(i / ss * 2 * M_PI) * amp;
    	sinbuf32[i + 1] = sin(i / ss * 2 * M_PI) * amp;
    }
}

void linux_iq_receive(void)
{
//	static int offset, position, limit, qq = 0;
//	static unsigned rx_stage = 0;
//
//	enum { CNT16TX = DMABUFFSIZE16TX / DMABUFFSTEP16TX };
//	enum { CNT32RX = DMABUFFSIZE32RX / DMABUFFSTEP32RX };
//
//	position = *((uint32_t *) sts);
//	offset = position >= DMABUFFSIZE32RX ? 0 : (DMABUFFSIZE32RX * 4);
//
//	uintptr_t addr = allocate_dmabuffer32rx();
//	memcpy((uint32_t *) addr, (uint32_t *) (ram + offset), DMABUFFSIZE32RX * 4);
//
////	qq ++;
////	if (qq > 5000)
////	{
////		qq = 0;
////		uint32_t * q = (uint32_t *) addr;
////		for (int i = 4; i < DMABUFFSIZE32RX; i += 8)
////		{
////			PRINTF("%03d: %08X ", i + 0, q[i + 0]);
////			PRINTF("%03d: %08X ", i + 1, q[i + 1]);
////			PRINTF("%03d: %08X ", i + 2, q[i + 2]);
////			PRINTF("%03d: %08X\n", i + 3, q[i + 3]);
////		}
////		PRINTF("\n****\n");
////	}
//
//
//	processing_dmabuffer32rx(addr);
//	release_dmabuffer32rx(addr);
//	rx_stage += CNT32RX;
//
//	while (rx_stage >= CNT16TX)
//	{
//		const uintptr_t addr = getfilled_dmabuffer16txphones();
//		uint32_t * b = (uint32_t *) addr;
//
//		for (int i = 0; i < DMABUFFSIZE16TX; i ++)
//			*((uint32_t *) ph_fifo) = b[i];
//
//		release_dmabuffer16tx(addr);
//		rx_stage -= CNT16TX;
//	}

	enum { CNT16TX = DMABUFFSIZE16TX / DMABUFFSTEP16TX };
	enum { CNT32RX = DMABUFFSIZE32RX / DMABUFFSTEP32RX };
	static int rx_stage = 0, qq = 0;;

	//printf("%d\n", * iq_count);

	if (* iq_count >= DMABUFFSIZE32RX)
	{
		const uintptr_t addr = allocate_dmabuffer32rx();
		uint32_t * r = (uint32_t *) addr;

		for (int i = 0; i < DMABUFFSIZE32RX; i ++)
			r[i] = * iq_fifo;

		processing_dmabuffer32rx(addr);
		release_dmabuffer32rx(addr);

		rx_stage += CNT32RX;

		while (rx_stage >= CNT16TX)
		{
			const uintptr_t addr2 = getfilled_dmabuffer16txphones();
			uint64_t * b = (uint64_t *) addr2;

			for (int i = 0; i < DMABUFFSIZE16TX; i ++)
				* ph_fifo = sinbuf32[i]; //b[i];

//			qq ++;
//			if (qq > 5000)
//			{
//				qq = 0;
//				for (int i = 4; i < DMABUFFSIZE16TX; i += 8)
//				{
//					PRINTF("%03d: %08X ", i + 0, b[i + 0]);
//					PRINTF("%03d: %08X ", i + 1, b[i + 1]);
//					PRINTF("%03d: %08X ", i + 2, b[i + 2]);
//					PRINTF("%03d: %08X\n", i + 3, b[i + 3]);
//				}
//				PRINTF("\n****\n");
//			}

			release_dmabuffer16tx(addr2);
			rx_stage -= CNT16TX;
		}
	}
//	else
//		printf("%d\n", * iq_count);
}

void linux_iq_interrupt_thread(void)
{
//	while(1)
//	{
//		linux_iq_receive();
//	}

	uint32_t uio_key = 1;
	uint32_t uio_value = 0;

	int fd_int = open("/dev/uio0", O_RDWR);
    if (fd_int < 0) {
        PRINTF("/dev/uio0 open failed\n");
        return;
    }

	* (uint32_t *) iq_shift &= ~(1 << 8);
	* (uint32_t *) iq_shift |= (1 << 8);

    while(1)
    {
        //Acknowledge IRQ
        if (write(fd_int, &uio_key, sizeof(uio_key)) < 0) {
            PRINTF("Failed to acknowledge IRQ: %s\n", strerror(errno));
            return;
        }

        linux_iq_receive();

        //Wait for next IRQ
        if (read(fd_int, &uio_value, sizeof(uio_value)) < 0) {
            PRINTF("Failed to wait for IRQ: %s\n", strerror(errno));
            return;
        }
    }
}

/*************************************************************/

void linux_create_thread(void * process, int priority, int cpuid)
{
	pthread_attr_t attr;
	struct sched_param param;
	pthread_t thread;
	cpu_set_t mask;

	ASSERT(cpuid < sysconf(_SC_NPROCESSORS_ONLN));

	pthread_attr_init(& attr);
	pthread_attr_setinheritsched(& attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(& attr, SCHED_FIFO);
	param.sched_priority = priority;
	pthread_attr_setschedparam(& attr, & param);
	CPU_ZERO(& mask);
	CPU_SET(cpuid, & mask);
	pthread_attr_setaffinity_np(& attr, sizeof(mask), & mask);

    if(pthread_create(& thread, & attr, process, NULL) < 0)
    {
      perror("pthread_create");
    }

    pthread_detach(thread);
}

void linux_subsystem_init(void)
{
	linux_framebuffer_init();
	linux_xgpio_init();
	linux_iq_init();
}

void linux_user_init(void)
{
	linux_create_thread(process_linux_timer_spool, 50, 0);
	linux_create_thread(linux_iq_interrupt_thread, 90, 1);
	linux_create_thread(linux_encoder_spool, 50, 1);

	const float FS = powf(2, 32);
	uint32_t fan_pwm_period = 25000 * FS / 61440000;
	reg_write(0x80048000 + 0, fan_pwm_period);

	uint32_t fan_pwm_duty = FS * (1.0f - 0.7f) - 1;
	reg_write(0x80048000 + 4, fan_pwm_duty);
}

/****************************************************************/

pthread_mutex_t md = PTHREAD_MUTEX_INITIALIZER;

void spin_lock(spinlock_t * __restrict p, const char * file, int line)
{
	pthread_mutex_lock(p);
}

void spin_unlock(spinlock_t * __restrict p)
{
	pthread_mutex_unlock(p);
}


void system_disableIRQ(void)
{

}

void system_enableIRQ(void)
{

}

void xcz_rxtx_state(uint8_t tx)
{

}

void xcz_dds_ftw(const uint_least64_t * val)
{
	uint32_t v = * val;
    * ftw = v;
}

void xcz_dds_rts(const uint_least64_t * val)
{
	uint32_t v = * val;
    * rts = v;
}

void xcz_rx_iq_shift(uint8_t val) // 52
{
	 * (uint32_t *) iq_shift |= (val & 0xff);
}

void xcz_dds_ftw_sub(const uint_least64_t * val)
{

}

void xcz_rx_cic_shift(uint32_t val)
{

}

void xcz_tx_shift(uint32_t val)
{

}

#if WITHDSPEXTFIR
void board_fpga_fir_initialize(void)
{

}

void board_reload_fir(uint_fast8_t ifir, const int_fast32_t * const k, unsigned Ntap, unsigned CWidth)
{
	if (fir_reload)
	{
		const int iHalfLen = (Ntap - 1) / 2;
		int i, j = 0;

		for (i = 0; i <= iHalfLen; ++ i)
			*((uint32_t *) fir_reload) = k[i];

		i -= 1;
		for (; -- i >= 0;)
			*((uint32_t *) fir_reload) = k[i];
	}
}
#endif /* WITHDSPEXTFIR */

void board_rtc_getdate(
	uint_fast16_t * year,
	uint_fast8_t * month,
	uint_fast8_t * dayofmonth
	)
{
	* dayofmonth = 0;
	* month = 0;
	* year = 0;
}

void board_rtc_gettime(
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * secounds
	)
{
	* secounds = 0;
	* minute = 0;
	* hour = 0;
}

uint_fast8_t dummy_putchar(uint_fast8_t c)
{
	printf("%c", c);
	return 1;
}

uint_fast8_t dummy_getchar(char * cp)
{
	* cp = 0;
	return 1;
}

void arm_hardware_set_handler_overrealtime(uint_fast16_t int_id, void (* handler)(void)) 	{}
void arm_hardware_set_handler_realtime(uint_fast16_t int_id, void (* handler)(void)) 		{}
void arm_hardware_set_handler_system(uint_fast16_t int_id, void (* handler)(void)) 			{}

#endif /* LINUX_SUBSYSTEM */
