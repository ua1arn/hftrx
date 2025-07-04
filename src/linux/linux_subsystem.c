/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "radio.h"
#include "encoder.h"

#if LINUX_SUBSYSTEM

#include "formats.h"	// for debug prints
#include "board.h"
#include "buffers.h"
#include "audio.h"
#include "ft8.h"
#include "gui/gui.h"
#include "display2.h"

#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/gpio.h>
#include <linux/spi/spidev.h>
#include <sys/stat.h>
#include <termios.h>
#include <fcntl.h>
#include <linux/input.h>
#include <dirent.h>
#include "lvgl/lvgl.h"
#include "pcie_dev.h"

void linux_create_thread(pthread_t * tid, void * (* process)(void * args), int priority, int cpuid);
void linux_cancel_thread(pthread_t tid);
void ft8_thread(void);
void lvgl_init(void);
void lvgl_test(void);

#define PIDFILE 		"/var/run/hftrx.pid"
#define MAX_WAIT_TIME 	5

#if defined (AXI_LITE_UARTLITE) && defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)

int uartlite_rx_ready(void)
{
    uint32_t status = xdma_read_user(AXI_LITE_UARTLITE + UARTLITE_STATUS);
    return (status & STATUS_RXVALID) != 0;
}

void uartlite_write_byte(uint8_t data)
{
    while ((xdma_read_user(AXI_LITE_UARTLITE + UARTLITE_STATUS) & STATUS_TXFULL)) {
        usleep(100);
    }
    xdma_write_user(AXI_LITE_UARTLITE + UARTLITE_TX_FIFO, data);
}

void uartlite_write_string(const char * str)
{
    while (* str) {
        uartlite_write_byte(* str ++);
    }
}

uint8_t uartlite_read_byte(void)
{
    uint32_t val = xdma_read_user(AXI_LITE_UARTLITE + UARTLITE_RX_FIFO);
    return (uint8_t)(val & 0xFF);
}

void uartlite_reset(void)
{
	xdma_write_user(AXI_LITE_UARTLITE + UARTLITE_CONTROL, CONTROL_RESET_FIFO);
	usleep(100);
	xdma_write_user(AXI_LITE_UARTLITE + UARTLITE_CONTROL, CONTROL_RX_ENABLE | CONTROL_TX_ENABLE | CONTROL_INTR_ENABLE);
}

#endif /* defined (AXI_LITE_UARTLITE) && defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA) */

pthread_t timer_spool_t, iq_interrupt_t, ft8t_t, nmea_t, pps_t, disp_t, audio_interrupt_t;
static struct cond_thread ct_iq;
int pcie_status = 0;

void linux_wait_iq(void)
{
	if (linux_verify_cond(& ct_iq))
		safe_cond_wait(& ct_iq);
}

void iq_mutex_unlock(void)
{
	if (linux_verify_cond(& ct_iq))
		safe_cond_signal(& ct_iq);
}

enum {
	rx_fir_shift_pos 	= 0,
	tx_shift_pos 		= 8,
	rx_cic_shift_pos 	= 16,
	tx_state_pos 		= 24,
	resetn_modem_pos 	= 25,
	hw_vfo_sel_pos		= 26,
	adc_rand_pos 		= 27,
	iq_test_pos			= 28,
	wnb_pos				= 29,
	stream_reset_pos 	= 30,
	fir_load_reset_pos 	= 31,
};

enum {
	CNT16TX = DMABUFFSIZE16TX / DMABUFFSTEP16TX,
	CNT32RX = DMABUFFSIZE32RX / DMABUFFSTEP32RX,
	SIZERX8 = DMABUFFSIZE32RX * 4,
};

enum {
#if CPUSTYLE_XC7Z
	stream_core = 1,
	alsa_thread_core = 1,
	iq_thread_core = 1,
	iio_thread_core = 1,
	spool_thread_core = 0,
	nmea_thread_core = 1,
#elif CPUSTYLE_RK356X
	stream_thread_core = 3,
	alsa_thread_core = 3,
	iq_thread_core = 2,
	iio_thread_core = 2,
	spool_thread_core = 0,
	nmea_thread_core = 1,
#endif
};

#if defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_FPGAV1)

void linux_rxtx_state(uint8_t tx)
{

}

#endif /* defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_FPGAV1) */

#if WITHAUDIOSAMPLESREC && WITHTOUCHGUI

enum {
	as_max_length = 10,
	as_sample_rate = ARMI2SRATE,
	as_buf_size = as_max_length * as_sample_rate,
	as_buf_step = DMABUFFSIZE16TX,
	as_gui_upd_pr = as_buf_size / 50,
};

static pthread_mutex_t mutex_as;
uint32_t as_buf [as_buf_size];
uint32_t as_idx = 0, as_idx_stop = 0;
uint8_t as_state = AS_IDLE, stop = 0;

void as_rx(uint32_t * buf)
{
	pthread_mutex_lock(& mutex_as);

	if (as_state == AS_RECORDING)
	{
		ASSERT(as_idx < as_buf_size);
		memcpy(& as_buf [as_idx], buf, as_buf_step * 4);
		as_idx += as_buf_step;
		if (as_idx % as_gui_upd_pr == 0) gui_as_update();

		if (as_idx >= as_buf_size || stop)
		{
			as_idx_stop = as_idx;
			as_idx = 0;
			stop = 0;
			as_state = AS_READY;
			gui_as_update();
		}
	}
	else if (as_state == AS_PLAYING)
	{
		ASSERT(as_idx < as_buf_size);
		memcpy(buf, & as_buf [as_idx], as_buf_step * 4);
		as_idx += as_buf_step;
		if (as_idx % as_gui_upd_pr == 0) gui_as_update();

		if (as_idx >= as_idx_stop || stop)
		{
			as_idx = 0;
			stop = 0;
			as_state = AS_READY;
			gui_as_update();
		}
	}

	pthread_mutex_unlock(& mutex_as);
}

void as_tx(uint32_t * buf)
{
	if (as_state != AS_TX)
		return;

	pthread_mutex_lock(& mutex_as);

	ASSERT(as_idx < as_buf_size);
	memcpy(buf, & as_buf [as_idx], as_buf_step * 4);
	as_idx += as_buf_step;
	if (as_idx % as_gui_upd_pr == 0) gui_as_update();

	if (as_idx >= as_idx_stop || stop)
	{
		as_idx = 0;
		stop = 0;
		as_state = AS_READY;
		hamradio_set_moxmode(0);
		gui_as_update();
	}

	pthread_mutex_unlock(& mutex_as);
}

uint8_t as_get_state(void)
{
	return as_state;
}

uint8_t as_get_progress(void)	// 1 ... 100
{
	if (as_state == AS_PLAYING || as_state == AS_TX)
		return (int) (as_idx * 100 / as_idx_stop);
	else
		return (int) (as_idx * 100 / as_buf_size);
}

void as_toggle_record(void)
{
	pthread_mutex_lock(& mutex_as);

	if (as_state == AS_IDLE || as_state == AS_READY)
		as_state = AS_RECORDING;
	else if (as_state == AS_RECORDING)
		stop = 1;

	pthread_mutex_unlock(& mutex_as);
}

void as_toggle_play(void)
{
	pthread_mutex_lock(& mutex_as);

	if (as_state == AS_READY)
		as_state = AS_PLAYING;
	else if (as_state == AS_PLAYING)
		stop = 1;

	pthread_mutex_unlock(& mutex_as);
}

void as_toggle_trx(void)
{
	static uint8_t agc_backup = 0, agc_gain_backup = 0;

	pthread_mutex_lock(& mutex_as);

	if (as_state == AS_READY)
	{
		agc_backup = hamradio_get_gmikeagc();
		agc_gain_backup = hamradio_get_gmikeagcgain();
		hamradio_set_gmikeagc(0);
		hamradio_set_gmikeagcgain(50);
		as_state = AS_TX;
		hamradio_set_moxmode(1);
	}
	else if (as_state == AS_TX)
	{
		stop = 1;
		hamradio_set_gmikeagc(agc_backup);
		hamradio_set_gmikeagcgain(agc_gain_backup);
	}

	pthread_mutex_unlock(& mutex_as);
}

void as_draw_spectrogram(COLORPIP_T * d, uint16_t len, uint16_t lim)
{
	const uint16_t step = as_idx_stop / len;
	int32_t d_max = 0;
	arm_max_no_idx_q31((q31_t *) as_buf, as_idx_stop, & d_max);

	for (int i = 0; i < len; i ++)
		d [i] = normalize(abs(as_buf [i * step]), 0, d_max, lim);
}

#endif /* WITHAUDIOSAMPLESREC && WITHTOUCHGUI*/

#if CPUSTYLE_XC7Z
void * get_highmem_ptr(uint32_t addr)
{
	int fd;
	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
	{
		printf("open /dev/mem failure\n");
		return NULL;
	}

	void *map_base, *virt_addr;
	unsigned page_size, mapped_size, offset_in_page;
	mapped_size = page_size = sysconf(_SC_PAGESIZE);
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

void * get_blockmem_ptr(uint32_t addr, uint8_t pages)
{
	int fd = open("/dev/mem", O_RDWR);
	void * blkptr = mmap(NULL, pages * sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, addr);
	close(fd);
	ASSERT(blkptr);
	return blkptr;
}

void reg_write(uint32_t addr, uint32_t val)
{
	void * ptr = get_highmem_ptr(addr);

	* (uint32_t *)(ptr) = val;

	//reg_read(addr);
	//printf("reg_write: 0x%08x, 0x%08x\n", addr, val);
}

uint32_t reg_read(uint32_t addr)
{
	void * ptr = get_highmem_ptr(addr);

	uint32_t res = * (uint32_t *) ptr;

//	printf("reg_read: 0x%08X, 0x%08X\n", addr, res);
	return res;
}

#elif defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)

void reg_write(uint32_t addr, uint32_t val)
{
	xdma_write_user(addr, val);
}

uint32_t reg_read(uint32_t addr)
{
	return xdma_read_user(addr);
}

#endif /* CPUSTYLE_XC7Z */

void * process_linux_timer_spool(void * args)
{
	const int delay = (1000 / TICKS_FREQUENCY) * 1000;

	while(1)
	{
		spool_systimerbundle();
		usleep(delay);
	}
}

#if WITHNMEA && WITHLFM && CPUSTYLE_XC7Z

void * linux_nmea_spool(void * args)
{
	const char * argv [] = { "/bin/stty", "-F", LINUX_NMEA_FILE, "115200", NULL, };
	linux_run_shell_cmd(argv);

	int num_read;
	int fid = open(LINUX_NMEA_FILE, O_RDONLY);
	tcflush(fid, TCIFLUSH);
	const int bufsize = 256;
	char buf[bufsize];

	while(1)
	{
		usleep(5000);
		memset(buf, 0, bufsize);
		num_read = read(fid, buf, bufsize);
		if (num_read > 1)
		{
			for (int i = 0; i < num_read; i ++)
				nmeagnss_parsechar(buf[i]);		/* USER MODE или SYSTEM-MODE обработчик надо вызывать ? */
		}
	}
}

void * linux_pps_thread(void * args)
{
	uint32_t uio_key = 1;
	uint32_t uio_value = 0;

	int fd_pps = open(LINUX_PPS_INT_FILE, O_RDWR);
    if (fd_pps < 0) {
        PRINTF("%s open failed\n", LINUX_PPS_INT_FILE);
        return 0;
    }

    while(1)
    {
        //Acknowledge IRQ
        if (write(fd_pps, &uio_key, sizeof(uio_key)) < 0) {
            PRINTF("Failed to acknowledge IRQ: %s\n", strerror(errno));
            return 0;
        }

        //Wait for next IRQ
        if (read(fd_pps, &uio_value, sizeof(uio_value)) < 0) {
            PRINTF("Failed to wait for IRQ: %s\n", strerror(errno));
            return 0;
        }

        spool_nmeapps(NULL);
    }
}

#endif /* WITHNMEA && WITHLFM && CPUSTYLE_XC7Z */

/******************************************************************/

#if WITHFBDEV && ! WITHLVGL

static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
volatile uint32_t * fbp = 0;
static uint32_t screensize = 0;

uint32_t * linux_get_fb(uint32_t * size)
{
	ASSERT(fbp != NULL);

	* size = screensize;
	return (uint32_t *) fbp;
}

uint32_t * fbmem_at(uint_fast16_t x, uint_fast16_t y)
{
	ASSERT(fbp != NULL);

	uint32_t location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 32) +
	                   (y + vinfo.yoffset) * finfo.line_length / 4;
	return (uint32_t *) (fbp + location);
}

int linux_framebuffer_init(void)
{
	int ttyd = open(LINUX_TTY_FILE, O_RDWR);
	if (ttyd)
		ioctl(ttyd, KDSETMODE, KD_GRAPHICS);
	else
	{
		PRINTF("Error: cannot open tty device");
		exit(1);
	}

	close(ttyd);

	// Open the file for reading and writing
	int fbfd = open(LINUX_FB_FILE, O_RDWR);
	if (fbfd == -1) {
		PRINTF("Error: cannot open framebuffer device");
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
	fbp = (uint32_t *) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (* fbp == -1) {
		PRINTF("Error: failed to map framebuffer device to memory");
		exit(4);
	}

	PRINTF("linux drm fb: %dx%d, %dbpp, %d bytes %p\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, screensize, fbp);

	close(fbfd);
	return 0;
}

void framebuffer_close(void)
{
	int ttyd = open(LINUX_TTY_FILE, O_RDWR);
    ioctl(ttyd, KDSETMODE, KD_TEXT);
    munmap((uint32_t *) fbp, screensize);
    close(ttyd);
}

#endif /* WITHFBDEV && ! WITHLVGL */

/********************** EMIO ************************/

#if CPUSTYLE_XC7Z
	#include "./gpiops/xgpiops.h"
	static XGpioPs xc7z_gpio;
	uint32_t * gpiops_ptr;
#elif 0
	uint32_t * xgpo, * xgpi;
	pthread_mutex_t gpiolock;
#elif CPUSTYLE_RK356X
	#include <gpiod.h>
	struct gpiod_chip * gpiochip3 = NULL;
	struct gpiod_chip * gpiochip4 = NULL;
#endif
void linux_xgpio_init(void)
{
#if 0
	xgpo = (uint32_t *) get_highmem_ptr(AXI_XGPO_ADDR);
	xgpi = (uint32_t *) get_highmem_ptr(AXI_XGPI_ADDR);
	pthread_mutex_init(& gpiolock, NULL);
#elif CPUSTYLE_XC7Z
	int ff = open("/sys/class/gpio/export", O_WRONLY);
	if (! ff)
		perror("Unable to open /sys/class/gpio/export");

	write(ff, "906", 3);
	close(ff);

	XGpioPs_Config * gpiocfg = XGpioPs_LookupConfig(0);
	gpiops_ptr = (uint32_t *) get_highmem_ptr(gpiocfg->BaseAddr);
	XGpioPs_CfgInitialize(& xc7z_gpio, gpiocfg, (uintptr_t) gpiops_ptr);
#elif CPUSTYLE_RK356X
	gpiochip3 = gpiod_chip_open_by_name("gpiochip3");
//	gpiochip4 = gpiod_chip_open_by_name("gpiochip4");
	ASSERT(gpiochip3);
//	ASSERT(gpiochip4);
#endif
}

#if CPUSTYLE_RK356X
void rk356x_gpio3_set(uint8_t pin, uint8_t val)
{
	struct gpiod_line *line = gpiod_chip_get_line(gpiochip3, pin);
    if (! line) {
        perror("gpiod_chip_get_line");
        ASSERT(0);
    }

    gpiod_line_request_output(line, "", !val);
    gpiod_line_set_value(line, !val);
}
#endif /* CPUSTYLE_RK356X */

uint8_t linux_xgpi_read_pin(uint8_t pin)
{
#if 0
	pthread_mutex_lock(& gpiolock);
	uint32_t v = * xgpi;
	pthread_mutex_unlock(& gpiolock);
	return (v >> pin) & 1;
#elif CPUSTYLE_XC7Z
	return XGpioPs_ReadPin(& xc7z_gpio, pin);
#elif CPUSTYLE_RK356X
	return 0;
#endif
}

void linux_xgpo_write_pin(uint8_t pin, uint8_t val)
{
#if 0
	pthread_mutex_lock(& gpiolock);

	uint32_t mask = 1 << pin;
	uint32_t rw = * xgpo;

	if (val)
		rw |= mask;
	else
		rw &= ~mask;

	* xgpo = rw;

	pthread_mutex_unlock(& gpiolock);
#elif CPUSTYLE_XC7Z
	XGpioPs_WritePin(& xc7z_gpio, pin, val);
#elif CPUSTYLE_RK356X

#endif
}

void xc7z_writepin(uint8_t pin, uint8_t val)
{
	linux_xgpo_write_pin(pin, val);
}

uint_fast8_t xc7z_readpin(uint8_t pin)
{
	return linux_xgpi_read_pin(pin);
}

void xc7z_gpio_input(uint8_t pin)
{
#if CPUSTYLE_XC7Z
	XGpioPs_SetDirectionPin(& xc7z_gpio, pin, 0);
#elif CPUSTYLE_RK356X

#endif
}

void xc7z_gpio_output(uint8_t pin)
{
#if CPUSTYLE_XC7Z
	XGpioPs_SetDirectionPin(& xc7z_gpio, pin, 1);
	XGpioPs_SetOutputEnablePin(& xc7z_gpio, pin, 1);
#elif CPUSTYLE_RK356X

#endif
}

void gpio_writepin(uint8_t pin, uint8_t val)
{
	linux_xgpo_write_pin(pin, val);
}

uint_fast8_t gpio_readpin(uint8_t pin)
{
	return linux_xgpi_read_pin(pin);
}

/************* I2C ************************/
static int fd_i2c = 0;

void i2c_initialize(void)
{
	fd_i2c = open(LINUX_I2C_FILE, O_RDWR);

	if (fd_i2c < 0)
		PRINTF("%s open failed: %d\n", LINUX_I2C_FILE, fd_i2c);
	else
		PRINTF("linux i2c started\n");
}

/* return non-zero then error */
// LSB of slave_address8b ignored */
int i2chw_write(uint16_t slave_address8b, const uint8_t * buf, uint32_t size)
{
	int rc = - 1;

	if (fd_i2c)
	{
		if (ioctl(fd_i2c, I2C_SLAVE, slave_address8b >> 1) < 0)
			perror("i2chw_write:");

		rc = write(fd_i2c, buf, size);
		if (rc < 0)
			PRINTF("Tried to write to address '0x%02x'\n", slave_address8b);
	}

	return rc < 0;
}

/* return non-zero then error */
// LSB of slave_address8b ignored */
int i2chw_read(uint16_t slave_address8b, uint8_t * buf, uint32_t size)
{
	int rc = - 1;

	if (fd_i2c)
	{
		if (ioctl(fd_i2c, I2C_SLAVE, slave_address8b >> 1) < 0)
			perror("i2chw_read:");

		rc = read(fd_i2c, buf, size);
		if (rc < 0)
			PRINTF("Tried to read from address '0x%02x'\n", slave_address8b);
	}

	return rc < 0;
}

/* return non-zero then error */
// LSB of slave_address8b ignored */
// TODO: Use restart for read
int i2chw_exchange(uint16_t slave_address8b, const uint8_t * wbuf, uint32_t wsize, uint8_t * rbuf, uint32_t rsize)
{
	int rc = - 1;

	if (fd_i2c)
	{
		if (ioctl(fd_i2c, I2C_SLAVE, slave_address8b >> 1) < 0)
			perror("i2chw_write:");

		rc = write(fd_i2c, wbuf, wsize);
		if (rc < 0)
			PRINTF("Tried to write to address '0x%02x'\n", slave_address8b);

		rc = read(fd_i2c, rbuf, rsize);
		if (rc < 0)
			PRINTF("Tried to read from address '0x%02x'\n", slave_address8b);
	}

	return rc < 0;
}

/*************************************************************/

#if WITHSPIDEV

uint8_t spidev_mode = SPI_MODE_3;
uint8_t spidev_bits = 0;
uint32_t spidev_speed = SPIC_SPEED25M;
static int * spidev_fd;

static pthread_mutex_t mutex_spidev;

void spidev_init(void)
{
	spidev_fd = (int *) malloc(cs_cnt * sizeof(int));
	ASSERT(spidev_fd);

	pthread_mutex_init(& mutex_spidev, NULL);

	for (int i = 0; i < cs_cnt; i ++)
	{
		char path[20];

		snprintf(path, ARRAY_SIZE(path), "%s.%d", SPIDEV_PATH, i);
		printf("%s start\n", path);

		int ret = 0;
		spidev_fd[i] = open(path, O_RDWR);
		if (!spidev_fd[i])
			perror("can't open device");

		ret = ioctl(spidev_fd[i], SPI_IOC_WR_MODE, & spidev_mode);
		if (ret == -1)
			perror("can't set spi mode");

		ret = ioctl(spidev_fd[i], SPI_IOC_RD_MODE, & spidev_mode);
		if (ret == -1)
			perror("can't get spi mode");

		ret = ioctl(spidev_fd[i], SPI_IOC_WR_BITS_PER_WORD, & spidev_bits);
		if (ret == -1)
			perror("can't set bits per word");

		ret = ioctl(spidev_fd[i], SPI_IOC_RD_BITS_PER_WORD, & spidev_bits);
		if (ret == -1)
			perror("can't get bits per word");

		ret = ioctl(spidev_fd[i], SPI_IOC_WR_MAX_SPEED_HZ, & spidev_speed);
		if (ret == -1)
			perror("can't set max speed hz");

		ret = ioctl(spidev_fd[i], SPI_IOC_RD_MAX_SPEED_HZ, & spidev_speed);
		if (ret == -1)
			perror("can't get max speed hz");
	}
}

void prog_spi_io(
	spitarget_t target, spi_speeds_t spispeedindex, spi_modes_t spimode,
	const uint8_t * txbuff1, unsigned int txsize1,
	const uint8_t * txbuff2, unsigned int txsize2,
	uint8_t * rxbuff, unsigned int rxsize
	)
{
	struct spi_ioc_transfer tr[3] = { 0 };
	uint8_t i = 0;

	pthread_mutex_lock(& mutex_spidev);

	ASSERT(target < cs_cnt);

	if (spimode != spidev_mode)
	{
		int ret = ioctl(spidev_fd[target], SPI_IOC_WR_MODE, & spimode);
		if (ret == -1)
			perror("can't set spi mode");
	}

	if (spispeedindex != spidev_speed)
	{
		int ret = ioctl(spidev_fd[target], SPI_IOC_WR_MAX_SPEED_HZ, & spispeedindex);
		if (ret == -1)
			perror("can't set max speed hz");
	}

	if (txsize1)
	{
		tr[i].tx_buf = (__u64) txbuff1;
		tr[i].rx_buf = 0;
		tr[i].len = txsize1;
		tr[i].speed_hz = spispeedindex;
		tr[i].bits_per_word = 8;
		tr[i].delay_usecs = 10;
		i ++;
	}

	if (txsize2)
	{
		tr[i].tx_buf = (__u64) txbuff2;
		tr[i].rx_buf = 0;
		tr[i].len = txsize2;
		tr[i].speed_hz = spispeedindex;
		tr[i].bits_per_word = 8;
		tr[i].delay_usecs = 10;
		i ++;
	}

	if (rxsize)
	{
		tr[i].tx_buf = 0;
		tr[i].rx_buf = (__u64) rxbuff;
		tr[i].len = rxsize;
		tr[i].speed_hz = spispeedindex;
		tr[i].bits_per_word = 8;
		tr[i].delay_usecs = 10;
		i ++;
	}

	int ret = ioctl(spidev_fd[target], SPI_IOC_MESSAGE(i), tr);
	if (ret < 1)
		perror("can't send spi message");

	pthread_mutex_unlock(& mutex_spidev);
}

void prog_spi_exchange(
	spitarget_t target, spi_speeds_t spispeedindex, spi_modes_t spimode,
	const uint8_t * txbuff,
	uint8_t * rxbuff,
	unsigned int size
	)
{
	ASSERT(target < cs_cnt);

	pthread_mutex_lock(& mutex_spidev);

	if (spimode != spidev_mode)
	{
		int ret = ioctl(spidev_fd[target], SPI_IOC_WR_MODE, & spimode);
		if (ret == -1)
			perror("can't set spi mode");
	}

	if (spispeedindex != spidev_speed)
	{
		int ret = ioctl(spidev_fd[target], SPI_IOC_WR_MAX_SPEED_HZ, & spispeedindex);
		if (ret == -1)
			perror("can't set max speed hz");
	}

	/* full-duplex transfer */
	struct spi_ioc_transfer tr = {
		.tx_buf = (__u64) txbuff,
		.rx_buf = (__u64) rxbuff,
		.len = size,
	};

	int ret = ioctl(spidev_fd[target], SPI_IOC_MESSAGE(1), & tr);
	if (ret < 1)
		perror("can't send spi message");

	pthread_mutex_unlock(& mutex_spidev);
}

#endif /* WITHSPIDEV */

/*************************************************************/

#if (DDS1_TYPE == DDS_TYPE_ZYNQ_PL || DDS1_TYPE == DDS_TYPE_XDMA)

void * iq_rx_blkmem;
volatile uint32_t * ftw, * ftw_sub, * rts, * modem_ctrl, * ph_fifo, * iq_count_rx, * iq_fifo_rx, * iq_fifo_tx, * mic_fifo;
volatile static uint8_t rx_fir_shift = 0, rx_cic_shift = 0, tx_shift = 0, tx_state = 0, resetn_modem = 1, hw_vfo_sel = 0;
volatile static uint8_t fir_load_rst = 0, iq_test = 0, wnb_state = 0, resetn_stream = 0;
const uint8_t rx_cic_shift_min = 32, rx_cic_shift_max = 64, rx_fir_shift_min = 32, rx_fir_shift_max = 56, tx_shift_min = 16, tx_shift_max = 32;
int fd_int = 0;

uint8_t rxbuf[SIZERX8] = { 0 };

void update_modem_ctrl(void)
{
	uint32_t v = ((rx_fir_shift & 0xFF) << rx_fir_shift_pos) 	| ((tx_shift & 0xFF) << tx_shift_pos)
			| ((rx_cic_shift & 0xFF) << rx_cic_shift_pos) 		| (!! tx_state << tx_state_pos)
			| (!! resetn_modem << resetn_modem_pos) 			| (!! hw_vfo_sel << hw_vfo_sel_pos)
			| (!! hamradio_get_gadcrand() << adc_rand_pos) 		| (!! iq_test << iq_test_pos)
			| (!! wnb_state << wnb_pos)							| (!! resetn_stream << stream_reset_pos)
			| (!! fir_load_rst << fir_load_reset_pos)			| 0;

#if DDS1_TYPE == DDS_TYPE_ZYNQ_PL
	* modem_ctrl = v;
#elif defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)
	xdma_write_user(AXI_LITE_MODEM_CONTROL, v);
#endif
}

void fir_load_reset(uint8_t val)
{
	fir_load_rst = val != 0;
	update_modem_ctrl();
}

void xcz_resetn_modem(uint8_t val)
{
	resetn_modem = val != 0;
	update_modem_ctrl();
}

void xcz_dds_rts(const uint_least64_t * val)
{
	uint32_t v = * val;
    mirror_ncorts = v;

#if DDS1_TYPE == DDS_TYPE_ZYNQ_PL
	* rts = v;
#elif defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)
	xdma_write_user(AXI_LITE_DDS_RTS, v);
#endif
}

void xcz_dds_ftw(const uint_least64_t * val)
{
	uint32_t v = * val;
    mirror_nco1 = v;

#if DDS1_TYPE == DDS_TYPE_ZYNQ_PL
    * ftw = v;
#elif defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)
    xdma_write_user(AXI_LITE_DDS_FTW, v);
#endif
}

void xcz_dds_ftw_sub(const uint_least64_t * val)
{
	uint32_t v = * val;
	mirror_nco2 = v;

#if DDS1_TYPE == DDS_TYPE_ZYNQ_PL && WITHUSEDUALWATCH
	* ftw_sub = v;
#elif defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA) && WITHUSEDUALWATCH
	xdma_write_user(AXI_LITE_DDS_FTW_SUB, v);
#endif
}

uint8_t iq_shift_fir_rx(uint8_t val) // 52
{
	if (val > 0)
	{
		if (val >= rx_fir_shift_min && val <= rx_fir_shift_max)
			rx_fir_shift = val & 0xFF;

		update_modem_ctrl();
	}
	return rx_fir_shift;
}

uint8_t iq_shift_cic_rx(uint8_t val)
{
	if (val > 0)
	{
		if (val >= rx_cic_shift_min && val <= rx_cic_shift_max)
			rx_cic_shift = val & 0xFF;

		update_modem_ctrl();
	}
	return rx_cic_shift;
}

uint8_t iq_shift_tx(uint8_t val)
{
	if (val > 0)
	{
		if (val >= tx_shift_min && val <= tx_shift_max)
			tx_shift = val & 0xFF;

		update_modem_ctrl();
	}
	return tx_shift;
}

uint8_t wnb_state_switch(void)
{
	wnb_state = wnb_state ? 0 : 1;
	update_modem_ctrl();

	return wnb_state;
}

uint32_t iq_cic_test_process(void)
{
	return 0;
}

void iq_cic_test(uint32_t val)
{
	iq_test = val != 0;
	update_modem_ctrl();
}

void linux_rxtx_state(uint8_t tx)
{
	tx_state = tx != 0;
	update_modem_ctrl();
}

#if WITHWNB		// Simple noise blanker в PL

const uint16_t threshold_min = 16, threshold_max = 31;
static uint32_t threshold = 30;

static void wnb_update(void)
{
#if DDS1_TYPE == DDS_TYPE_ZYNQ_PL
	reg_write(XPAR_IQ_MODEM_WNB_CONFIG, threshold);
#elif defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)
	xdma_write_user(AXI_LITE_WNB_CONFIG, threshold);
#endif
}

void wnb_set_threshold(uint16_t v)
{
	if (v >= threshold_min && v <= threshold_max)
	{
		threshold = v;
		wnb_update();
	}
}

uint16_t wnb_get_threshold(void)
{
	return threshold;
}

#endif /* WITHWNB */

#if WITHEXTIO_LAN

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

void stream_log(char * str);

#define TCP_PORT 1001

struct cond_thread ct_rts;
pthread_t eth_main_t, eth_send_t, eth_int_t;
int sockServer, sockClient;
uint8_t streambuf[4096] = { 0 };
char strbuf[128];
static uint8_t stream_state = STREAM_IDLE, auto_restart = 0;
struct sockaddr_in local_addr;

enum {
	STREAM_RATE_192K = REFERENCE_FREQ / 192000 / 2,
	STREAM_RATE_384K = REFERENCE_FREQ / 384000 / 2,
	STREAM_RATE_768K = REFERENCE_FREQ / 768000 / 2,
};

#if DDS1_TYPE == DDS_TYPE_ZYNQ_PL
volatile uint32_t * stream_rate, * stream_data, * stream_pos;
#endif /* DDS1_TYPE == DDS_TYPE_ZYNQ_PL */

void set_stream_rate(uint32_t v)
{
#if DDS1_TYPE == DDS_TYPE_ZYNQ_PL
	* stream_rate = v;
#elif defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)
	xdma_write_user(AXI_LITE_STREAM_RATE, v);
#endif
}

uint32_t get_stream_pos(void)
{
#if DDS1_TYPE == DDS_TYPE_ZYNQ_PL
	return * stream_pos;
#elif defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)
	return xdma_read_user(AXI_LITE_STREAM_POS);
#endif
}

void stream_receive(uint8_t * buf, uint16_t offset)
{
#if DDS1_TYPE == DDS_TYPE_ZYNQ_PL
	memcpy(buf, (uint8_t *) stream_data + offset, 4096);
#elif defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)
	xdma_c2h_transfer(AXI_IQ_STREAM_BRAM + offset, 4096, buf);
#endif
}

void server_kill(void)
{
	close(sockClient);
	close(sockServer);
}

void stream_event_handler(void)
{
	if (stream_state == STREAM_CONNECTED)
	{
		uint32_t pos = get_stream_pos();
		uint16_t offset = pos >= 512 ? 0 : 4096;
		stream_receive(streambuf, offset);
		safe_cond_signal(& ct_rts);
	}
}

void * eth_stream_interrupt_thread(void * args)
{
	ssize_t nb;

	int fd = open(LINUX_STREAM_INT_FILE, O_RDWR);
    if (fd < 0) {
        PRINTF("%s open failed\n", LINUX_STREAM_INT_FILE);
        return NULL;
    }

	struct pollfd fds = {
		.fd = fd,
		.events = POLLIN,
	};

	resetn_stream = 1;
	update_modem_ctrl();

    while(1)
    {
    	uint32_t intr = 1; /* unmask */

#if DDS1_TYPE != DDS_TYPE_XDMA
		nb = write(fd, & intr, sizeof(intr));
		if (nb != (ssize_t) sizeof(intr)) {
			perror("write");
			close(fd);
			exit(EXIT_FAILURE);
		}
#endif /* DDS1_TYPE != DDS_TYPE_XDMA */

		int ret = poll(& fds, 1, -1);
		if (ret >= 1) {
			nb = read(fd, & intr, sizeof(intr));
			if (nb == (ssize_t) sizeof(intr) && stream_state == STREAM_CONNECTED)
			{
				uint32_t pos = get_stream_pos();
				uint16_t offset = pos >= 512 ? 0 : 4096;
				stream_receive(streambuf, offset);
				safe_cond_signal(& ct_rts);
			}
		} else {
			perror("poll()");
			close(fd);
			exit(EXIT_FAILURE);
		}
    }
}

void * eth_main_thread(void * args)
{
	struct sockaddr_in addr, addrClient;
	int yes = 1;
	uint32_t command;
	socklen_t size = sizeof(addrClient);
	stream_state = STREAM_IDLE;

	if((sockServer = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return NULL;
	}

	setsockopt(sockServer, SOL_SOCKET, SO_REUSEADDR, (void *) & yes , sizeof(yes));

	/* setup listening address */
	memset(& local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(TCP_PORT);
	bind(sockServer, (struct sockaddr *) & local_addr, sizeof(local_addr));

	local_snprintf_P(strbuf, ARRAY_SIZE(strbuf), "Start listening on port %d", TCP_PORT);
	stream_log(strbuf);
	stream_state = STREAM_LISTEN;
	listen(sockServer, 1024);

    if((sockClient = accept(sockServer, (struct sockaddr *) & addrClient, & size)) < 0)
    {
      perror("accept");
      return NULL;
    }

    stream_state = STREAM_CONNECTED;
	local_snprintf_P(strbuf, ARRAY_SIZE(strbuf), "Accepted TCP connection from %s:%d", inet_ntoa(addrClient.sin_addr), TCP_PORT);
	stream_log(strbuf);

	while(1)
	{
		if(ioctl(sockClient, FIONREAD, & size) < 0) perror("FIONREAD");
		if(size >= 4)
		{
			if(recv(sockClient, (char *) & command, 4, MSG_WAITALL) < 0) perror("recv");

			switch (command >> 31) {
			case 0:
				// Set frequency
				hamradio_set_freq(command);
				break;
			case 1:
				// Set sample rate
				switch (command & 3) {
				case 0:
					set_stream_rate(STREAM_RATE_192K);
					break;
				case 1:
					set_stream_rate(STREAM_RATE_384K);
					break;
				case 2:
					set_stream_rate(STREAM_RATE_768K);
					break;
				}
				break;
			}
		}

		safe_cond_wait(& ct_rts);

#if 0				// Инжекция импульсных помех
		{
			uint32_t * b = (uint32_t *) streambuf;

			for (int i = 0; i < 1024; i ++)
			{
				if (i % 20 == 0)
					b[i] = 0x7FFFFFFF;
				if (i % 50 == 0)
					b[i] = 0x1FFFFFF;
				if (i % 100 == 0)
					b[i] = 0x2FFFFFF;
			}
		}
#endif

#if WITHWNB && 0		// Программный simple noise blanker
		if (wnb_state)
		{
			int32_t * const b = (int32_t *) streambuf;

			for (int i = 0; i < 1024; i ++)
			{
				uint32_t absv = b[i] >> 31 ? (~ b[i] + 1) : b[i];
				if (absv > 1 << threshold)
					b[i] = 0x0;
			}
		}
#endif /* WITHWNB */

		if (send(sockClient, streambuf, 4096, MSG_NOSIGNAL) < 0)
		{
			local_snprintf_P(strbuf, ARRAY_SIZE(strbuf), "TCP connection from %s is closed", inet_ntoa(addrClient.sin_addr));
			stream_log(strbuf);
			server_kill();
			stream_state = STREAM_IDLE;
			if (auto_restart) server_restart();
			return NULL;
		}

	}

	return NULL;
}

void server_restart(void)
{
	linux_cancel_thread(eth_main_t);
	linux_create_thread(& eth_main_t, eth_main_thread, 50, stream_thread_core);
}

uint8_t stream_get_state(void)
{
	return stream_state;
}

void server_stop(void)
{
	if (stream_state != STREAM_IDLE)
	{
		local_snprintf_P(strbuf, ARRAY_SIZE(strbuf), "Stream server stopped");
		stream_log(strbuf);
		server_kill();
		linux_cancel_thread(eth_main_t);
		stream_state = STREAM_IDLE;
	}
}

void server_start(void)
{
	if (stream_state == STREAM_IDLE)
		linux_create_thread(& eth_main_t, eth_main_thread, 50, stream_thread_core);
}

#endif /* WITHEXTIO_LAN */

/*************************************************************/

#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_ALSA)

#include <alsa/asoundlib.h>

pthread_t alsa_t;
snd_pcm_t * pcm_ph = NULL;
snd_pcm_uframes_t frames = DMABUFFSIZE16TX / DMABUFFSTEP16TX;
snd_pcm_uframes_t alsa_buffer_size = ARMI2SRATE * 50 / 1000; // 50 мс
unsigned int actual_sample_rate = ARMI2SRATE;
snd_pcm_sframes_t error;
uint8_t vol_shift = 0;

void * alsa_thread(void * args)
{
	const snd_pcm_uframes_t half_buf = alsa_buffer_size / 2;

	while(1)
	{
		snd_pcm_sframes_t avail = snd_pcm_avail_update(pcm_ph);

		if (avail == -EPIPE)
		{
			printf("Buffer overrun, recovering...");
			snd_pcm_recover(pcm_ph, avail, 0);
		}
		else if (avail == -ESTRPIPE)
		{
			printf("Threat was freesing, resuming...");
			snd_pcm_resume(pcm_ph);
		}
		else if (avail > 0 && avail < half_buf)
			usleep(1000);
		else
		{
			const uintptr_t addr_ph = getfilled_dmabuffer16tx();
			int32_t * b = (int32_t *) addr_ph;
#if WITHAUDIOSAMPLESREC && WITHTOUCHGUI
			as_rx(b);
#endif /* WITHAUDIOSAMPLESREC && WITHTOUCHGUI*/

			arm_shift_q31(b, - vol_shift, b, DMABUFFSIZE16TX);

		    if ((error = snd_pcm_writei(pcm_ph, b, frames)) != frames) {
		    	printf("Write to PCM device failed: %s\n", snd_strerror(error));
		        if (error == -EPIPE)
		            snd_pcm_prepare(pcm_ph);
		        else if (error == -ESTRPIPE)
		        	snd_pcm_resume(pcm_ph);
		    }

			release_dmabuffer16tx(addr_ph);
		}
	}
}

int alsa_init(const char * card_name)
{
	if ((error = snd_pcm_open(& pcm_ph, card_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC)) < 0) {
		printf("Cannot open PCM device: %s\n", snd_strerror(error));
		return 1;
	}

	snd_pcm_hw_params_t *params;
	snd_pcm_hw_params_alloca(&params);

	if ((error = snd_pcm_hw_params_any(pcm_ph, params)) < 0) {
		printf("Cannot initialize hardware parameter structure: %s\n", snd_strerror(error));
		return 1;
	}

	if ((error = snd_pcm_hw_params_set_access(pcm_ph, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		printf("Cannot set access type: %s\n", snd_strerror(error));
		return 1;
	}

	if ((error = snd_pcm_hw_params_set_format(pcm_ph, params, SND_PCM_FORMAT_S24_LE)) < 0) {
		printf("Cannot set sample format: %s\n", snd_strerror(error));
		return 1;
	}

	if ((error = snd_pcm_hw_params_set_rate_near(pcm_ph, params, & actual_sample_rate, 0)) < 0) {
		printf("Cannot set sample rate: %s\n", snd_strerror(error));
		return 1;
	}

	if ((error = snd_pcm_hw_params_set_channels(pcm_ph, params, DMABUFFSTEP16TX)) < 0) {
		printf("Cannot set channel count: %s\n", snd_strerror(error));
		return 1;
	}

	if ((error = snd_pcm_hw_params_set_buffer_size_near(pcm_ph, params, & alsa_buffer_size)) < 0) {
		printf("Cannot set channel count: %s\n", snd_strerror(error));
		return 1;
	}

	if ((error = snd_pcm_hw_params(pcm_ph, params)) < 0) {
		printf("Cannot set parameters: %s\n", snd_strerror(error));
		return 1;
	}

	int32_t * s = calloc(alsa_buffer_size, sizeof(int32_t));
	snd_pcm_writei(pcm_ph, s, alsa_buffer_size);
	free(s);

	linux_create_thread(& alsa_t, alsa_thread, 50, alsa_thread_core);

	return 0;
}

void alsa_close(void)
{
	linux_cancel_thread(alsa_t);
	snd_pcm_drain(pcm_ph);
	snd_pcm_close(pcm_ph);
}

#if BLUETOOTH_ALSA

char alsa_cards[2][10] = { "default", "bluealsa" };
char names[2][10] = { "speaker", "bluetooth" };
uint8_t card_id = 0;

char * get_alsa_out(void)
{
	ASSERT(card_id < 2);
	return names[card_id];
}

void alsa_switch_out(void)
{
	card_id = (card_id + 1) % 2;

	alsa_close();
	if(alsa_init(alsa_cards[card_id]))
	{
		// если bluetooth audio не доступно, повторить инициализацию по умолчанию
		card_id = 0;
		alsa_init(alsa_cards[card_id]);
	}
}

#endif /* BLUETOOTH_ALSA */

static void dummy_stop (void) {}
uint_fast8_t dummy_clocksneed(void) {}
static void dummy_setprocparams (uint_fast8_t procenable, const uint_fast8_t * gains) {}
static void dummy_lineinput(uint_fast8_t v, uint_fast8_t mikeboost20db, uint_fast16_t mikegain, uint_fast16_t linegain) {}
static void dummy_setlineinput (uint_fast8_t linein, uint_fast8_t mikeboost20db, uint_fast16_t mikegain, uint_fast16_t linegain) {}

static void alsa_setvolume (uint_fast16_t gainL, uint_fast16_t gainR, uint_fast8_t mute, uint_fast8_t mutespk)
{
	vol_shift = mutespk ? BOARD_AFGAIN_MAX : BOARD_AFGAIN_MAX - gainL;
}

static void alsa_initialize(void (* io_control)(uint_fast8_t on), uint_fast8_t master)
{
	ASSERT(! alsa_init("default"));
}

const codec1if_t *
board_getaudiocodecif(void)
{

	static const char codecname [] = "ALSA";

	/* Интерфейс управления кодеком */
	static const codec1if_t ifc =
	{
		dummy_clocksneed,
		dummy_stop,
		alsa_initialize,
		alsa_setvolume,
		dummy_lineinput,
		dummy_setprocparams,
		codecname
	};

	return & ifc;
}

#endif /* (CODEC1_TYPE == CODEC_TYPE_ALSA) */

/*************************************************************/

static void iq_proccessing(uint8_t * buf, uint32_t len)
{
	static int rx_stage = 0;
	uintptr_t addr32rx = allocate_dmabuffer32rx();
	memcpy((uint8_t *) addr32rx, buf, len);
	save_dmabuffer32rx(addr32rx);

	rx_stage += CNT32RX;

	while (rx_stage >= CNT16TX)
	{
#if DMABUFSCALE == 2
		const uintptr_t addr_ph = getfilled_dmabuffer16tx();
		uint32_t * b = (uint32_t *) addr_ph;

		for (int i = 0; i < DMABUFFSIZE16TX; i ++)
			* ph_fifo = b[i];

		release_dmabuffer16tx(addr_ph);
#endif /* DMABUFSCALE == 2 */

		const uintptr_t addr = getfilled_dmabuffer32tx();
		uint32_t * t = (uint32_t *) addr;

		for (uint16_t i = 0; i < DMABUFFSIZE32TX / 2; i ++)				// 16 bit
			* iq_fifo_tx = t[i];

		release_dmabuffer32tx(addr);

		rx_stage -= CNT16TX;
	}

#if WITHFT8
	if (! ft8_get_state())
#endif /* WITHFT8 */
	{
#if DMABUFSCALE == 1
		const uintptr_t addr_ph = getfilled_dmabuffer16tx();
		uint32_t * b = (uint32_t *) addr_ph;

#if WITHAUDIOSAMPLESREC
#if WITHAD936XIIO
		if (! get_ad936x_stream_status())
#endif /* WITHAD936XIIO */
			as_rx(b);
#endif /* WITHAUDIOSAMPLESREC */

		for (int i = 0; i < DMABUFFSIZE16TX; i ++)
			* ph_fifo = b[i];

		release_dmabuffer16tx(addr_ph);
#endif /* DMABUFSCALE == 1 */

		uintptr_t addr_mic = allocate_dmabuffer16rx();
		uint32_t * mic = (uint32_t *) addr_mic;

		for (int i = 0; i < DMABUFFSIZE16RX; i ++)
			mic[i] = * mic_fifo;

#if WITHAUDIOSAMPLESREC
		as_tx(mic);
#endif /* WITHAUDIOSAMPLESREC */

		save_dmabuffer16rx(addr_mic);
	}

	iq_mutex_unlock();
}

void linux_iq_init(void)
{
#if DDS1_TYPE == DDS_TYPE_ZYNQ_PL
	ftw = 			(uint32_t *) get_highmem_ptr(XPAR_IQ_MODEM_AXI_DDS_FTW_BASEADDR);
	ftw_sub = 		(uint32_t *) get_highmem_ptr(XPAR_IQ_MODEM_AXI_DDS_FTW_SUB_BASEADDR);
	rts = 			(uint32_t *) get_highmem_ptr(XPAR_IQ_MODEM_AXI_DDS_RTS_BASEADDR);
	ph_fifo = 		(uint32_t *) get_highmem_ptr(XPAR_AUDIO_FIFO_PHONES_BASEADDR);
	iq_fifo_tx = 	(uint32_t *) get_highmem_ptr(XPAR_IQ_MODEM_FIFO_IQ_TX_BASEADDR);
	mic_fifo = 		(uint32_t *) get_highmem_ptr(XPAR_AUDIO_FIFO_MIC_BASEADDR);
	modem_ctrl = 	(uint32_t *) get_highmem_ptr(XPAR_IQ_MODEM_MODEM_CONTROL_BASEADDR);
	iq_count_rx = 	(uint32_t *) get_highmem_ptr(XPAR_IQ_MODEM_BLKMEM_CNT_BASEADDR);
	iq_rx_blkmem =  (uint32_t *) get_blockmem_ptr(XPAR_IQ_MODEM_BLKMEM_READER_BASEADDR, 1);
#endif /* DDS1_TYPE == DDS_TYPE_ZYNQ_PL */
#if defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)
	pcie_init();
	pcie_status = pcie_open();
	if (pcie_status < 0)
		perror("pcie init");
#endif /* DDS1_TYPE == DDS_TYPE_XDMA */
#if WITHEXTIO_LAN
#if DDS1_TYPE == DDS_TYPE_ZYNQ_PL
	stream_rate = (uint32_t *) get_highmem_ptr(XPAR_IQ_MODEM_STREAM_RATE);
	stream_pos = (uint32_t *) get_highmem_ptr(XPAR_IQ_MODEM_STREAM_COUNT);
	stream_data = (uint32_t *) get_blockmem_ptr(XPAR_IQ_MODEM_STREAM_DATA, 2);
#endif /* DDS1_TYPE == DDS_TYPE_ZYNQ_PL */
	resetn_stream = 0;
	update_modem_ctrl();

	set_stream_rate(STREAM_RATE_192K);
	linux_init_cond(& ct_rts);
	linux_create_thread(& eth_int_t, eth_stream_interrupt_thread, 90, stream_thread_core);
#endif /* WITHEXTIO_LAN */
#if defined (XPAR_AUDIO_AXI_I2S_ADI_0_BASEADDR)
	reg_write(XPAR_AUDIO_AXI_I2S_ADI_0_BASEADDR + AUDIO_REG_I2S_CLK_CTRL, (64 / 2 - 1) << 16 | (4 / 2 - 1));
	reg_write(XPAR_AUDIO_AXI_I2S_ADI_0_BASEADDR + AUDIO_REG_I2S_PERIOD, DMABUFFSIZE16TX);
	reg_write(XPAR_AUDIO_AXI_I2S_ADI_0_BASEADDR + AUDIO_REG_I2S_CTRL, TX_ENABLE_MASK | RX_ENABLE_MASK);
#endif /* defined (XPAR_AUDIO_AXI_I2S_ADI_0_BASEADDR) */
#if WITHWNB
	wnb_update();
#endif /* WITHWNB */
#if WITHIQSHIFT
	iq_shift_fir_rx(CALIBRATION_IQ_FIR_RX_SHIFT);
	iq_shift_cic_rx(CALIBRATION_IQ_CIC_RX_SHIFT);
	iq_shift_tx(CALIBRATION_TX_SHIFT);
#endif /* WITHIQSHIFT */
}

#if defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)

pthread_t xdma_t;

void xdma_iq_event_handler(void)
{
#if WITHAD936XIIO
	if (get_ad936x_stream_status()) return;
#endif /* WITHAD936XIIO */

	uint16_t position = xdma_read_user(AXI_LITE_IQ_RX_BRAM_CNT);
	uint16_t offset = position >= DMABUFFSIZE32RX ? 0 : SIZERX8;
	xdma_c2h_transfer(AXI_IQ_RX_BRAM + offset, SIZERX8, rxbuf);

	uintptr_t addr32rx = allocate_dmabuffer32rx();
	memcpy((uint8_t *) addr32rx, rxbuf, SIZERX8);
	save_dmabuffer32rx(addr32rx);

	const uintptr_t addr32tx = getfilled_dmabuffer32tx();
	uint32_t * t = (uint32_t *) addr32tx;

	for (uint16_t i = 0; i < DMABUFFSIZE32TX / 2; i ++)				// 16 bit
		xdma_write_user(AXI_LITE_IQ_FX_FIFO, t[i]);

	release_dmabuffer32tx(addr32tx);

	iq_mutex_unlock();
}

void codec_event_handler(void)
{
#if CODEC1_FPGA
	enum { SIZEMIC8 = DMABUFFSIZE16RX * 4 };
	uint8_t bufm[SIZEMIC8] = { 0 };
	uint16_t mic_pos = xdma_read_user(AXI_LITE_MIC_POS);
	uint16_t offs = mic_pos >= DMABUFFSIZE16RX ? 0 : SIZEMIC8;
	xdma_c2h_transfer(AXI_BRAM_MIC + offs, SIZEMIC8, bufm);

	uintptr_t addrm = allocate_dmabuffer16rx();
	memcpy((uint8_t *) addrm, bufm, SIZEMIC8);
#if WITHAUDIOSAMPLESREC && WITHTOUCHGUI
	as_tx((uint32_t *) bufm);
#endif /* WITHAUDIOSAMPLESREC && WITHTOUCHGUI */
	save_dmabuffer16rx(addrm);

#if WITHFT8
	if (! ft8_get_state())
#endif /* WITHFT8 */
	{
		const uintptr_t addr_ph = getfilled_dmabuffer16tx();
		uint32_t * b = (uint32_t *) addr_ph;

#if WITHAUDIOSAMPLESREC && WITHTOUCHGUI
#if WITHAD936XIIO
		if (! get_ad936x_stream_status())
#endif /* WITHAD936XIIO */
			as_rx(b);
#endif /* WITHAUDIOSAMPLESREC && WITHTOUCHGUI */

		for (int i = 0; i < DMABUFFSIZE16TX; i ++)
			xdma_write_user(AXI_LITE_FIFO_PHONES, b[i]);

		release_dmabuffer16tx(addr_ph);
	}
#endif /* CODEC1_FPGA */
}

#if defined (LINUX_XDMA_PPS_FILE)
void pps_event_handler(void)
{
	spool_nmeapps(NULL);
}
#endif /* defined (LINUX_XDMA_PPS_FILE) */

#if defined (LINUX_XDMA_UART_FILE)
void uartlite_event_handler(void)
{
	enum { bufsize = 256 };
	static char buf[bufsize] = { 0 };
	static int rx_index = 0;

	while(uartlite_rx_ready() && rx_index < bufsize)
	{
		buf[rx_index ++] = uartlite_read_byte();
	}

	if (rx_index > 1)
	{
		for (int i = 0; i < rx_index; i ++)
			nmeagnss_parsechar(buf[i]);

		rx_index = 0;
		memset(buf, 0, bufsize);
	}
}
#endif /* defined (LINUX_XDMA_UART_FILE) */

typedef void (* event_handler_t)(void);
typedef struct {
	const char * device_file;
	event_handler_t handler;
	int fd;
} irq_channel_t;

irq_channel_t channels[] = {
#if defined (LINUX_XDMA_IQ_EVENT_FILE)
		{ LINUX_XDMA_IQ_EVENT_FILE, xdma_iq_event_handler, -1 },
#endif /* defined (LINUX_XDMA_IQ_EVENT_FILE) */
#if CODEC1_FPGA
		{ LINUX_XDMA_MIC_EVENT_FILE, codec_event_handler, -1 },
#endif /* CODEC1_FPGA */
#if WITHEXTIO_LAN
		{ LINUX_STREAM_INT_FILE, stream_event_handler, -1 },
#endif /* WITHEXTIO_LAN */
#if defined (LINUX_XDMA_PPS_FILE)
		{ LINUX_XDMA_PPS_FILE, pps_event_handler, -1 },
#endif /* defined (LINUX_XDMA_PPS_FILE) */
#if defined (LINUX_XDMA_UART_FILE)
		{ LINUX_XDMA_UART_FILE, uartlite_event_handler, -1 },
#endif /* defined (LINUX_XDMA_UART_FILE) */
};

#define NUM_CHANNELS (sizeof(channels) / sizeof(channels[0]))

void * xdma_event_thread(void * args)
{
	if (! NUM_CHANNELS)
		return NULL;

	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		perror("epoll_create1");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUM_CHANNELS; i ++) {
		channels[i].fd = open(channels[i].device_file, O_RDONLY);
		if (channels[i].fd < 0) {
			perror("open");
			fprintf(stderr, "Open %s error\n", channels[i].device_file);
			continue;
		}

		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;
		ev.data.ptr = & channels[i];

		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, channels[i].fd, & ev) == -1) {
			perror("epoll_ctl ADD");
			close(channels[i].fd);
			channels[i].fd = -1;
		}
	}

	#define MAX_EVENTS 10
	struct epoll_event events[MAX_EVENTS];

	while (1) {
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_events == -1) {
			if (errno == EINTR)
				continue;
			perror("epoll_wait");
			break;
		}

		for (int i = 0; i < num_events; i++) {
			irq_channel_t * channel = (irq_channel_t *) events[i].data.ptr;
			uint32_t event;

			ssize_t bytes_read = pread(channel->fd, & event, sizeof(event), 0);
			if (bytes_read < 0) {
				perror("read");
				continue;
			}

			if (bytes_read != sizeof(event)) {
				fprintf(stderr, "Partial read event\n");
				continue;
			}

			channel->handler();
		}
	}
}

void xdma_iq_init(void)
{
	if(pcie_status < 0)
		return;

	xcz_resetn_modem(0);

	linux_create_thread(& xdma_t, xdma_event_thread, 95, iq_thread_core);

	xcz_resetn_modem(1);
	linux_init_cond(& ct_iq);
}

void xdma_close(void)
{
	linux_cancel_thread(xdma_t);
	pcie_close();
}

#endif /* DDS1_TYPE == DDS_TYPE_XDMA */

#if DDS1_TYPE == DDS_TYPE_ZYNQ_PL

void linux_iq_thread(void)
{
	uint32_t pos = * iq_count_rx;
	uint16_t offset = pos >= DMABUFFSIZE32RX ? 0 : SIZERX8;
	memcpy(rxbuf, (uint8_t *) iq_rx_blkmem + offset, SIZERX8);

#if WITHWNB && 0		// Программный simple noise blanker
	if (wnb_state)
	{
		int32_t * const b = (int32_t *) rxbuf;

		for (int i = 0; i < DMABUFFSIZE32RX; i ++)
		{
			uint32_t absv = b[i] >> 31 ? (~ b[i] + 1) : b[i];
			if (absv > 1 << threshold)
				b[i] = 0x0;
		}
	}
#endif /* WITHWNB */

	iq_proccessing(rxbuf, SIZERX8);
}

void * linux_iq_interrupt_thread(void * args)
{
	fd_int = open(LINUX_IQ_INT_FILE, O_RDWR);
    if (fd_int < 0) {
        PRINTF("%s open failed\n", LINUX_IQ_INT_FILE);
        return NULL;
    }

    while(1)
    {
#if 1
    	uint32_t intr = 1; /* unmask */

		ssize_t nb = write(fd_int, & intr, sizeof(intr));
		if (nb != (ssize_t) sizeof(intr)) {
			perror("write");
			close(fd_int);
			exit(EXIT_FAILURE);
		}

		struct pollfd fds = {
			.fd = fd_int,
			.events = POLLIN,
		};

		int ret = poll(& fds, 1, -1);
		if (ret >= 1) {
			nb = read(fd_int, & intr, sizeof(intr));
			if (nb == (ssize_t) sizeof(intr)) {
				linux_iq_thread();
			}
		} else {
			perror("poll()");
			close(fd_int);
			exit(EXIT_FAILURE);
		}
#else
        linux_iq_thread();
        usleep(100);
#endif
    }
}

void * audio_interrupt_thread(void * args)
{
	int fd = open(LINUX_AUDIO_INT_FILE, O_RDWR);
    if (fd < 0) {
        PRINTF("%s open failed\n", LINUX_AUDIO_INT_FILE);
        return NULL;
    }

	// пнуть fifo
	for (int i = 0; i < DMABUFFSIZE16TX; i ++)
		* ph_fifo = 1;

    while(1)
    {
    	uint32_t intr = 1; /* unmask */

		ssize_t nb = write(fd, & intr, sizeof(intr));
		if (nb != (ssize_t) sizeof(intr)) {
			perror("write");
			close(fd);
			exit(EXIT_FAILURE);
		}

		struct pollfd fds = {
			.fd = fd,
			.events = POLLIN,
		};

		int ret = poll(& fds, 1, -1);
		if (ret >= 1) {
			nb = read(fd, & intr, sizeof(intr));
			if (nb == (ssize_t) sizeof(intr))
			{
				const uintptr_t addr_ph = getfilled_dmabuffer16tx();
				uint32_t * ph = (uint32_t *) addr_ph;

				for (int i = 0; i < DMABUFFSIZE16TX; i ++)
					* ph_fifo = ph[i];

				release_dmabuffer16tx(addr_ph);

				uintptr_t addr_mic = allocate_dmabuffer16rx();
				uint32_t * mic = (uint32_t *) addr_mic;

				for (int i = 0; i < DMABUFFSIZE16RX; i ++)
					mic[i] = * mic_fifo;

				save_dmabuffer16rx(addr_mic);
			}
		} else {
			perror("poll()");
			close(fd);
			exit(EXIT_FAILURE);
		}
    }
}

void zynq_pl_init(void)
{
	xcz_resetn_modem(0);

#if WITHAUDIOSAMPLESREC
	pthread_mutex_init(& mutex_as, NULL);
#endif /* WITHAUDIOSAMPLESREC */

	linux_create_thread(& iq_interrupt_t, linux_iq_interrupt_thread, 95, iq_thread_core);
//	linux_create_thread(& audio_interrupt_t, audio_interrupt_thread, 50, 1);

#if defined AXI_DCDC_PWM_ADDR
	const float FS = powf(2, 32);
	uint32_t dcdcpwm_period = 25000 * FS / REFERENCE_FREQ;
	reg_write(AXI_DCDC_PWM_ADDR + 0, dcdc_pwm_period);

	uint32_t dcdc_pwm_duty = FS * (1.0f - 0.8f) - 1;
	reg_write(AXI_DCDC_PWM_ADDR + 4, dcdc_pwm_duty);
#endif /* defined AXI_DCDC_PWM_ADDR */

#if WITHCPUFANPWM
	{
		const float FS = powf(2, 32);
		uint32_t fan_pwm_period = 25000 * FS / REFERENCE_FREQ;
		reg_write(XPAR_FAN_PWM_RX_BASEADDR + 0, fan_pwm_period);

		uint32_t fan_pwm_duty = FS * (1.0f - 0.6f) - 1;
		reg_write(XPAR_FAN_PWM_RX_BASEADDR + 4, fan_pwm_duty);
	}
#endif /* WITHCPUFANPWM */

	xcz_resetn_modem(1);

	linux_init_cond(& ct_iq);
}

#endif /* DDS1_TYPE  == DDS_TYPE_ZYNQ_PL */
#endif /* DDS1_TYPE  == DDS_TYPE_ZYNQ_PL */

/*************************************************************/

int check_and_terminate_existing_instance(void)
{

    FILE *pidfile = fopen(PIDFILE, "r");
    if (pidfile == NULL) {
        return 0;
    }

    pid_t pid;
    if (fscanf(pidfile, "%d", &pid) != 1) {
        fprintf(stderr, "Invalid PID in file %s\n", PIDFILE);
        fclose(pidfile);
        unlink(PIDFILE);
        return -1;
    }
    fclose(pidfile);

    if (kill(pid, 0) == 0) {
        printf("HFTRX is already running with PID %d. Terminating it...\n", pid);

        if (kill(pid, SIGTERM) != 0) {
            perror("Failed to send SIGTERM");
            return -1;
        }

        int wait_time = 0;
        while (wait_time < MAX_WAIT_TIME && kill(pid, 0) == 0) {
            sleep(1);
            wait_time++;
        }

        if (kill(pid, 0) == 0) {
            printf("Process did not terminate gracefully. Sending SIGKILL...\n");
            if (kill(pid, SIGKILL) != 0) {
                perror("Failed to send SIGKILL");
                return -1;
            }
        }
    }

    unlink(PIDFILE);
    return 0;
}

void linux_run_shell_cmd(const char * argv [])
{
	if (access(argv[0], F_OK) != 0)
	{
		printf("%s not found\n", argv[0]);
		return;
	}

	extern char ** environ;
	int pid = getpid();

	if (0 == (pid = fork()))
	{
		if (execve(argv[0], (char **) argv, environ) < 0)
			perror(argv[0]);
	}
}

void linux_init_cond(struct cond_thread * ct)
{
	ASSERT(ct != NULL);
	pthread_cond_init(& ct->ready_cond, NULL);
	pthread_mutex_init(& ct->ready_mutex, NULL);
	ct->tag = ct;
}

int linux_verify_cond(struct cond_thread * ct)
{
	return (ct->tag == ct) ? 1 : 0;
}

void linux_destroy_cond(struct cond_thread * ct)
{
	ASSERT(ct != NULL);
	ASSERT(ct->tag == ct);
	pthread_cond_destroy(& ct->ready_cond);
	pthread_mutex_destroy(& ct->ready_mutex);
	ct->tag = NULL;
}

void safe_cond_signal(struct cond_thread * ct)
{
	ASSERT(ct != NULL);
	ASSERT(ct->tag == ct);
	pthread_mutex_lock(& ct->ready_mutex);
	pthread_cond_signal(& ct->ready_cond);
	pthread_mutex_unlock(& ct->ready_mutex);
}

void safe_cond_wait(struct cond_thread * ct)
{
	ASSERT(ct != NULL);
	ASSERT(ct->tag == ct);
	pthread_mutex_lock(& ct->ready_mutex);
	pthread_cond_wait(& ct->ready_cond, & ct->ready_mutex);
	pthread_mutex_unlock(& ct->ready_mutex);
}

void linux_create_thread(pthread_t * tid, void * (* process)(void * args), int priority, int cpuid)
{
	pthread_attr_t attr;
	struct sched_param param;
	cpu_set_t mask;

	ASSERT(cpuid < sysconf(_SC_NPROCESSORS_ONLN));

	pthread_attr_init(& attr);
	pthread_attr_setschedpolicy(& attr, SCHED_RR);
	param.sched_priority = priority;
	pthread_attr_setschedparam(& attr, & param);
	CPU_ZERO(& mask);
	CPU_SET(cpuid, & mask);
	pthread_attr_setaffinity_np(& attr, sizeof(mask), & mask);

    if(pthread_create(tid, & attr, process, NULL) < 0)
      perror("pthread_create");

    pthread_detach(* tid);
}

void linux_cancel_thread(pthread_t tid)
{
    if(pthread_cancel (tid) < 0)
      perror("pthread_cancel");

    if(pthread_join(tid, NULL) < 0)
    	perror("pthread_join");
}

static void handle_sig(int sig)
{
	printf("Waiting for process to finish... Got signal %d\n", sig);
#if WITHAD936XIIO
	iio_stop_stream();
#endif /* WITHAD936XIIO */
	linux_exit();
}

/*************************************************************/

#if WITHCPUTEMPERATURE

float linux_get_cpu_temp(void)
{
	int cpu_temp;
	const char tpath[] = "/sys/class/thermal/thermal_zone0/temp";

	FILE * tf = fopen(tpath, "r");
	if (! tf)
		perror("fopen");

	fscanf(tf, "%d", &cpu_temp);
	fclose(tf);

#if WITHFANTIMER

	const int t_on = 63, t_gyst = 3;
	static int fanState = 0;
	int temp = cpu_temp / 1000;

	if (fanState == 0 && temp >= t_on)
		fanState = 1;
	else if (fanState == 1 && temp < (t_on - t_gyst))
		fanState = 0;

	board_setfanflag(fanState);
	board_update();

#endif /* WITHFANTIMER */

	return cpu_temp / 1000.0;
}

#endif /* WITHCPUTEMPERATURE */

void linux_subsystem_init(void)
{
    if (check_and_terminate_existing_instance() != 0) {
        fprintf(stderr, "Failed to terminate existing instance.\n");
        return;
    }

    FILE *pidfile = fopen(PIDFILE, "w");
    if (pidfile == NULL) {
        perror("Cannot create PID file");
        return;
    }
	fprintf(pidfile, "%d", getpid());
	fclose(pidfile);

	char spid[6];
	local_snprintf_P(spid, ARRAY_SIZE(spid), "%d", getpid());
	const char * argv [] = { "/usr/bin/taskset", "-p", "1", spid, NULL, };
	linux_run_shell_cmd(argv);

	signal(SIGINT, handle_sig);
	signal(SIGTERM, handle_sig);
	linux_xgpio_init();
#if WITHSPIDEV
	spidev_init();
#endif /* WITHSPIDEV */
#if (DDS1_TYPE == DDS_TYPE_ZYNQ_PL || DDS1_TYPE == DDS_TYPE_XDMA)
	linux_iq_init();
#endif /* (DDS1_TYPE == DDS_TYPE_ZYNQ_PL || DDS1_TYPE == DDS_TYPE_XDMA) */
#if WITHLVGL
	lvgl_dev_init();	// linux-specific LVGL initialize - lv_deinit and lv_init include
	lvgl_init();
#endif /* WITHLVGL */
#if WITHIQSHIFT
	iq_shift_cic_rx(CALIBRATION_IQ_CIC_RX_SHIFT);
	iq_shift_fir_rx(CALIBRATION_IQ_FIR_RX_SHIFT);
	iq_shift_tx(CALIBRATION_TX_SHIFT);
#endif /* WITHIQSHIFT */
}

void linux_user_init(void)
{
	linux_create_thread(& timer_spool_t, process_linux_timer_spool, 50, spool_thread_core);

#if (DDS1_TYPE == DDS_TYPE_ZYNQ_PL)
	zynq_pl_init();
#elif (DDS1_TYPE == DDS_TYPE_XDMA)
	xdma_iq_init();
#if defined(AXI_LITE_UARTLITE)
	uartlite_reset();
#endif /* defined(AXI_LITE_UARTLITE) */
#if defined(HARDWARE_NMEA_INITIALIZE) && WITHNMEA
	HARDWARE_NMEA_INITIALIZE();
#endif /* defined(HARDWARE_NMEA_INITIALIZE()) && WITHNMEA*/
#endif /* (DDS1_TYPE == DDS_TYPE_XDMA) */
#if WITHNMEA && WITHLFM && CPUSTYLE_XC7Z
	linux_create_thread(& nmea_t, linux_nmea_spool, 50, nmea_thread_core);
	linux_create_thread(& pps_t, linux_pps_thread, 90, nmea_thread_core);
#endif /* WITHNMEA && WITHLFM && CPUSTYLE_XC7Z*/
#if WITHLVGL
	lvgl_test();
#endif /* WITHLVGL */
	evdev_initialize();
}

/****************************************************************/

pthread_mutex_t linux_md = PTHREAD_MUTEX_INITIALIZER;

void lclspin_lock(lclspinlock_t * __restrict p, const char * file, int line)
{
	pthread_mutex_lock(p);
}

void lclspin_unlock(lclspinlock_t * __restrict p)
{
	pthread_mutex_unlock(p);
}

#if WITHDSPEXTFIR && defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_ZYNQ_PL)
volatile uint32_t * fir_reload = NULL;
static adapter_t plfircoefsout;		/* параметры преобразования к PL */

void board_fpga_fir_initialize(void)
{
	adpt_initialize(& plfircoefsout, HARDWARE_COEFWIDTH, 0, "fpgafircoefsout");
	fir_reload = (uint32_t *) get_highmem_ptr(XPAR_IQ_MODEM_FIR_RELOAD_RX_BASEADDR);
}

void board_reload_fir(uint_fast8_t ifir, const int32_t * const k, const FLOAT_t * const kf, unsigned Ntap, unsigned CWidth)
{
	if (fir_reload)
	{
		const int iHalfLen = (Ntap - 1) / 2;
		int i = 0, m = 0, bits = 0;

		// Приведение разрядности значений коэффициентов к CWidth
		for (; i <= iHalfLen; ++ i)
		{
			int32_t coeff = adpt_output(& plfircoefsout, kf [i]);
			m = coeff > m ? coeff : m;
		}

		while(m > 0)
		{
			m  = m >> 1;
			bits ++;
		}

		bits = CWidth - bits - 1;

		for (i = 0; i <= iHalfLen; ++ i)
		{
			int32_t coeff = adpt_output(& plfircoefsout, kf [i]);
			* fir_reload = coeff << bits;
		}

		i -= 1;
		for (; -- i >= 0;)
		{
			int32_t coeff = adpt_output(& plfircoefsout, kf [i]);
			* fir_reload = coeff << bits;
		}
	}
}
#elif WITHDSPEXTFIR && defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)
static adapter_t plfircoefsout;		/* параметры преобразования к PL */

void board_fpga_fir_initialize(void)
{
	adpt_initialize(& plfircoefsout, HARDWARE_COEFWIDTH, 0, "fpgafircoefsout");
}

void board_reload_fir(uint_fast8_t ifir, const int32_t * const k, const FLOAT_t * const kf, unsigned Ntap, unsigned CWidth)
{
	const int iHalfLen = (Ntap - 1) / 2;
	int i = 0, m = 0, bits = 0;

	fir_load_reset(0);

	// Приведение разрядности значений коэффициентов к CWidth
	for (; i <= iHalfLen; ++ i)
	{
		int32_t coeff = adpt_output(& plfircoefsout, kf [i]);
		m = coeff > m ? coeff : m;
	}

	while(m > 0)
	{
		m  = m >> 1;
		bits ++;
	}

	bits = CWidth - bits - 1;
	fir_load_reset(1);

	for (i = 0; i <= iHalfLen; ++ i)
	{
		int32_t coeff = adpt_output(& plfircoefsout, kf [i]);
		xdma_write_user(AXI_LITE_FIR_COEFFS, coeff << bits);
	}

	i -= 1;
	for (; -- i >= 0;)
	{
		int32_t coeff = adpt_output(& plfircoefsout, kf [i]);
		xdma_write_user(AXI_LITE_FIR_COEFFS, coeff << bits);
	}
}
#endif /* WITHDSPEXTFIR && (DDS1_TYPE == DDS_TYPE_ZYNQ_PL) */

#if RTC1_TYPE == RTC_TYPE_LINUX
void board_rtc_getdate(
	uint_fast16_t * year,
	uint_fast8_t * month,
	uint_fast8_t * dayofmonth
	)
{
	struct tm * tm;

	time_t lt = time(NULL);
	tm = localtime (& lt);

	* dayofmonth = tm->tm_mday;
	* month = 1 + tm->tm_mon;
	* year = 1900 + tm->tm_year;
}

void board_rtc_gettime(
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * seconds
	)
{
	struct tm * tm;

	time_t lt = time(NULL);
	tm = localtime (& lt);

	* seconds = tm->tm_sec;
	* minute = tm->tm_min;
	* hour = tm->tm_hour;
}

void board_rtc_getdatetime(
	uint_fast16_t * year,
	uint_fast8_t * month,	// 01-12
	uint_fast8_t * dayofmonth,
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * seconds
	)
{
	struct tm * tm;

	time_t lt = time(NULL);
	tm = localtime (& lt);

	* seconds = tm->tm_sec;
	* minute = tm->tm_min;
	* hour = tm->tm_hour;
	* dayofmonth = tm->tm_mday;
	* month = 1 + tm->tm_mon;
	* year = 1900 + tm->tm_year;
}

void board_rtc_setdatetime(
	uint_fast16_t year,
	uint_fast8_t month,
	uint_fast8_t dayofmonth,
	uint_fast8_t hours,
	uint_fast8_t minutes,
	uint_fast8_t seconds
	)
{
	struct tm * tm;
	struct timeval tv;

	time_t lt = time(NULL);
	tm = localtime (& lt);

	tm->tm_sec = seconds;
	tm->tm_min = minutes;
	tm->tm_hour = hours;
	tm->tm_mday = dayofmonth;
	tm->tm_mon = month - 1;
	tm->tm_year = year - 1900;

	tv.tv_sec = mktime(tm);
	tv.tv_usec = 0;

	if (settimeofday(& tv, NULL) == -1)
		perror("settimeofday");
}

void board_rtc_settime(
	uint_fast8_t hour,
	uint_fast8_t minute,
	uint_fast8_t seconds
	)
{
	struct tm * tm;
	struct timeval tv;

	time_t lt = time(NULL);
	tm = localtime (& lt);

	tm->tm_sec = seconds;
	tm->tm_min = minute;
	tm->tm_hour = hour;

	tv.tv_sec = mktime(tm);
	tv.tv_usec = 0;

	if (settimeofday(& tv, NULL) == -1)
		perror("settimeofday");
}

#endif /* RTC1_TYPE == RTC_TYPE_LINUX */

uint_fast8_t board_rtc_chip_initialize(void)
{
	return 0;
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

// ********************** EVDEV Events ******************************

#if WITHENCODER2 && ENCODER2_EVDEV

int evdev_enc2_fd = -1;
int mouse_wheel_rotate = 0;

int get_enc2_rotate(void)
{
	struct input_event in;
	int step = 0;

	if (! evdev_enc2_fd) return 0;

	while (read(evdev_enc2_fd, &in, sizeof(struct input_event)) > 0)
	{
		if(in.type == EV_REL && in.code == REL_X)
		{
			step += in.value;
		}
	}

	return step;
}

int linux_get_enc2(void)
{
	int s = 0;
	s += get_enc2_rotate();
#if MOUSE_EVDEV
	s += mouse_wheel_rotate;
	mouse_wheel_rotate = 0;
#endif /* MOUSE_EVDEV */
	return s;
}

#endif /* WITHENCODER2 && ENCODER2_EVDEV */

static int_fast16_t xx = DIM_X / 2, yy = DIM_Y / 2, pr = 0;

#if WITHKEYBOARD && KEYBOARD_EVDEV

int evdev_keyboard_fd = -1;

#endif /* WITHKEYBOARD && KEYBOARD_EVDEV */

#if MOUSE_EVDEV

int evdev_mouse_fd = -1;

void get_cursor_pos(uint16_t * x, uint16_t * y)
{
	* x = xx;
	* y = yy;
}

uint8_t check_is_mouse_present(void)
{
	return evdev_mouse_fd > 0;
}

void get_mouse_events(uint_fast16_t * xr, uint_fast16_t * yr)
{
	struct input_event in;

	if (! evdev_mouse_fd) return;

	while (read(evdev_mouse_fd, & in, sizeof(struct input_event)) > 0)
	{
		if(in.type == EV_REL && in.code == REL_X)
		{
			xx += in.value;
			if (xx < 0) xx = 0;
			if (xx > DIM_X) xx = DIM_X;
		}
		else if (in.type == EV_REL && in.code == REL_Y)
		{
			yy += in.value;
			if (yy < 0) yy = 0;
			if (yy > DIM_Y) yy = DIM_Y;
		}
		else if(in.type == EV_KEY && in.code == BTN_LEFT)
			pr = in.value;
#if ENCODER2_EVDEV
		else if(in.type == EV_REL && in.code == REL_WHEEL)
			mouse_wheel_rotate = in.value;
#endif /* ENCODER2_EVDEV */
	}

	* xr = xx;
	* yr = yy;
}

#endif /* MOUSE_EVDEV */

#if (TSC1_TYPE == TSC_TYPE_EVDEV) && ! WITHLVGL

int evdev_touch_fd = -1;

void get_touch_events(uint_fast16_t * xr, uint_fast16_t * yr)
{
	struct input_event in;

	if (! evdev_touch_fd) return;

	while (read(evdev_touch_fd, & in, sizeof(struct input_event)) > 0)
	{
		if(in.type == EV_ABS && (in.code == ABS_X || in.code == ABS_MT_POSITION_X))
		{
			xx = in.value;
#if defined (TSC_EVDEV_RAWX)
			xx = normalize(xx, 0, TSC_EVDEV_RAWX, DIM_X - 1);
#endif /* defined (TSC_EVDEV_RAWX)*/
		}
		else if (in.type == EV_ABS && (in.code == ABS_Y || in.code == ABS_MT_POSITION_Y))
		{
			yy = in.value;
#if defined (TSC_EVDEV_RAWY)
			yy = normalize(yy, 0, TSC_EVDEV_RAWY, DIM_Y - 1);
#endif /* defined (TSC_EVDEV_RAWY) */
		}
		else if(in.type == EV_KEY && in.code == BTN_TOUCH)
			pr = in.value;
	}

	* xr = xx;
	* yr = yy;
}

uint_fast8_t board_tsc_getxy(uint_fast16_t * xr, uint_fast16_t * yr)
{
	get_touch_events(xr, yr);
#if MOUSE_EVDEV
	get_mouse_events(xr, yr);
#endif /* MOUSE_EVDEV */

	return pr;
}

#endif /* (TSC1_TYPE == TSC_TYPE_EVDEV) && ! WITHLVGL*/

static int is_event_device(const struct dirent * dir) {
	return strncmp("event", dir->d_name, 5) == 0;
}

static void check_event(int * fd, char * name, char * fname, const char * event_name)
{
	if (* fd < 0 && strstr(name, event_name)) {
		printf("Use %s for %s events\n", fname, event_name);

		* fd = open(fname, O_RDWR | O_NOCTTY | O_NDELAY);
		if (* fd == -1) {
			perror("unable open evdev interface:");
		}

		fcntl(* fd, F_SETFL, O_ASYNC | O_NONBLOCK);
	}
}

void evdev_initialize(void)
{
	struct dirent ** namelist;
	char * filename;

#if CPUSTYLE_XC7Z
	const char * argv [] = { "/sbin/modprobe", "gt911", NULL, };
	linux_run_shell_cmd(argv);
	usleep(500000);
#endif /* CPUSTYLE_XC7Z */

	int ndev = scandir("/dev/input", & namelist, is_event_device, alphasort);
	if (ndev <= 0)
		return;

	for (int i = 0; i < ndev; i++)
	{
		char fname[4096];
		int fd = -1;
		char name[256] = "???";

		snprintf(fname, sizeof(fname), "%s/%s", "/dev/input", namelist[i]->d_name);
		fd = open(fname, O_RDONLY);
		if (fd < 0)
			continue;
		ioctl(fd, EVIOCGNAME(sizeof(name)), name);

		close(fd);
		free(namelist[i]);

#if TSC1_TYPE == TSC_TYPE_EVDEV
		check_event(& evdev_touch_fd, name, fname, TOUCH_EVENT_NAME);
#endif /* TSC1_TYPE == TSC_TYPE_EVDEV */
#if ENCODER2_EVDEV
		check_event(& evdev_enc2_fd, name, fname, ENCODER2_EVENT_NAME);
#endif /* ENCODER2_EVDEV */
#if KEYBOARD_EVDEV
		check_event(& evdev_keyboard_fd, name, fname, KEYBOARD_EVENT_NAME);
#endif /* KEYBOARD_EVDEV */
#if MOUSE_EVDEV
		check_event(& evdev_mouse_fd, name, fname, MOUSE_EVENT_NAME);
#endif /* MOUSE_EVDEV */
	}

#if TSC1_TYPE == TSC_TYPE_EVDEV
	if (evdev_touch_fd < 0) printf("Not found %s event devices\n", TOUCH_EVENT_NAME);
#endif /* TSC1_TYPE == TSC_TYPE_EVDEV */
#if ENCODER2_EVDEV
	if (evdev_enc2_fd < 0) printf("Not found %s event devices\n", ENCODER2_EVENT_NAME);
#endif /* ENCODER2_EVDEV */
#if KEYBOARD_EVDEV
	if (evdev_keyboard_fd < 0) printf("Not found %s event devices\n", KEYBOARD_EVENT_NAME);
#endif /* KEYBOARD_EVDEV */
#if MOUSE_EVDEV
	if (evdev_mouse_fd < 0) printf("Not found %s event devices\n", MOUSE_EVENT_NAME);
#endif /* MOUSE_EVDEV */
	return;
}

// *****************************************************************

void arm_hardware_set_handler_overrealtime(uint_fast16_t int_id, void (* handler)(void)) 	{}
void arm_hardware_set_handler_realtime(uint_fast16_t int_id, void (* handler)(void)) 		{}
void arm_hardware_set_handler_system(uint_fast16_t int_id, void (* handler)(void)) 			{}

void linux_exit(void)
{
#if WITHFBDEV && ! WITHLVGL
	framebuffer_close();
#elif WITHSDL2VIDEO
	sdl2_render_close();
#endif /* WITHFBDEV && ! WITHLVGL */

#if 0
	linux_cancel_thread(timer_spool_t);
	linux_cancel_thread(iq_interrupt_t);
#if WITHNMEA
	linux_cancel_thread(nmea_t);
	linux_cancel_thread(pps_t);
#endif /* WITHNMEA */
#if WITHFT8
	linux_cancel_thread(ft8t_t);
#endif /* WITHFT8 */

	close(fd_int);
	close(fd_i2c);

	munmap((void *) ftw, sysconf(_SC_PAGESIZE));
	munmap((void *) ftw_sub, sysconf(_SC_PAGESIZE));
	munmap((void *) rts, sysconf(_SC_PAGESIZE));
	munmap((void *) modem_ctrl, sysconf(_SC_PAGESIZE));
	munmap((void *) ph_fifo, sysconf(_SC_PAGESIZE));
	munmap((void *) iq_count_rx, sysconf(_SC_PAGESIZE));
	munmap((void *) iq_count_tx, sysconf(_SC_PAGESIZE));
	munmap((void *) iq_fifo_tx, sysconf(_SC_PAGESIZE));
	munmap((void *) xgpo, sysconf(_SC_PAGESIZE));
	munmap((void *) xgpi, sysconf(_SC_PAGESIZE));

#if WITHDSPEXTFIR
	munmap((void *) fir_reload, sysconf(_SC_PAGESIZE));
#endif /* WITHDSPEXTFIR */
#endif

#if defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)
	xdma_close();
#endif /* defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA) */

#if defined(CODEC1_TYPE) && CODEC1_TYPE == CODEC_TYPE_ALSA
	alsa_close();
#endif /* CODEC1_TYPE == CODEC_TYPE_ALSA */

	unlink(PIDFILE);
	exit(EXIT_SUCCESS);
}

#if CPUSTYLE_XC7Z
unsigned long xc7z_get_arm_freq(void)
{
	return 766000000uL;
}
#endif

#if WITHAD936XIIO

void iio_start_stream(void);

pthread_t iio_t;
static char iio_uri[30];

void * iio_stream_thread(void * args)
{
	ad9363_iio_start(iio_uri);
	return NULL;
}

void iq_stream_start(void)
{
#if defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)
	linux_create_thread(& xdma_t, xdma_event_thread, 95, iq_thread_core);
#elif defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_ZYNQ_PL)
	linux_create_thread(& iq_interrupt_t, linux_iq_interrupt_thread, 95, iq_thread_core);
#endif
}

void iq_stream_stop(void)
{
#if defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_XDMA)
	linux_cancel_thread(xdma_t);
#elif DDS1_TYPE == DDS_TYPE_ZYNQ_PL
	linux_cancel_thread(iq_interrupt_t);
#endif
}

uint8_t iio_ad936x_start(const char * uri)
{
	strncpy(iio_uri, uri, 30);

	iq_stream_stop();
	if (! get_status_iio())
	{
		linux_create_thread(& iio_t, iio_stream_thread, 50, iio_thread_core);
		while(! get_ad936x_stream_status()) ;
	}
	iio_start_stream();
	hamradio_set_freq(433000000);

	return 2;
}

uint8_t iio_ad936x_stop(void)
{
	//while(get_ad936x_stream_status()) ;
	//linux_cancel_thread(iio_t);
	iq_stream_start();
	iio_stop_stream();
	hamradio_set_freq(7012000);

	return 0;
}

#endif /* WITHAD936XIIO */

#endif /* LINUX_SUBSYSTEM */
