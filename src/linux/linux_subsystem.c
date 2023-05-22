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
#include <sys/stat.h>
#include <termios.h>

void xcz_resetn_modem_state(uint8_t val);
void ft8_thread(void);

enum {
	rx_fir_shift_pos 	= 0,
	tx_shift_pos 		= 8,
	rx_cic_shift_pos 	= 16,
	tx_state_pos 		= 24,
	resetn_modem_pos 	= 25,
	hw_vfo_sel_pos		= 26,
};

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
		usleep(500);
	}
}

#if WITHNMEA && WITHLFM

void linux_nmea_spool(void)
{
	const char * argv [5] = { "/bin/stty", "-F", LINUX_NMEA_FILE, "115200", NULL, };
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
				nmea_parsechar(buf[i]);
		}
	}
}

void linux_pps_thread(void)
{
	uint32_t uio_key = 1;
	uint32_t uio_value = 0;

	int fd_pps = open(LINUX_PPS_INT_FILE, O_RDWR);
    if (fd_pps < 0) {
        PRINTF("%s open failed\n", LINUX_PPS_INT_FILE);
        return;
    }

    while(1)
    {
        //Acknowledge IRQ
        if (write(fd_pps, &uio_key, sizeof(uio_key)) < 0) {
            PRINTF("Failed to acknowledge IRQ: %s\n", strerror(errno));
            return;
        }

        //Wait for next IRQ
        if (read(fd_pps, &uio_value, sizeof(uio_value)) < 0) {
            PRINTF("Failed to wait for IRQ: %s\n", strerror(errno));
            return;
        }

        spool_nmeapps();
    }
}

#endif /* WITHNMEA && WITHLFM */

/******************************************************************/

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
	fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
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

/********************** EMIO ************************/

volatile uint32_t * xgpo, * xgpi;

#if CPUSTYLE_XC7Z
	#include "./gpiops/xgpiops.h"
	static XGpioPs xc7z_gpio;
	uint32_t * gpiops_ptr;
#endif /* CPUSTYLE_XC7Z */

void linux_xgpio_init(void)
{
#if CPUSTYLE_XCZU
	xgpo = get_highmem_ptr(AXI_XGPO_ADDR);
	xgpi = get_highmem_ptr(AXI_XGPI_ADDR);
#elif CPUSTYLE_XC7Z
	int ff = open("/sys/class/gpio/export", O_WRONLY);
	if (! ff)
		perror("Unable to open /sys/class/gpio/export");

	write(ff, "906", 3);
	close(ff);

	XGpioPs_Config * gpiocfg = XGpioPs_LookupConfig(0);
	gpiops_ptr = get_highmem_ptr(gpiocfg->BaseAddr);
	XGpioPs_CfgInitialize(& xc7z_gpio, gpiocfg, (uintptr_t) gpiops_ptr);
#endif
}

uint8_t linux_xgpi_read_pin(uint8_t pin)
{
#if CPUSTYLE_XCZU
	uint32_t v = * xgpi;
	return (v >> pin) & 1;
#elif CPUSTYLE_XC7Z
	return XGpioPs_ReadPin(& xc7z_gpio, pin);
#endif
}

void linux_xgpo_write_pin(uint8_t pin, uint8_t val)
{
#if CPUSTYLE_XCZU
	uint32_t mask = 1 << pin;
	uint32_t rw = * xgpo;

	if (val)
		rw |= mask;
	else
		rw &= ~mask;

	* xgpo = rw;
#elif CPUSTYLE_XC7Z
	XGpioPs_WritePin(& xc7z_gpio, pin, val);
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
#if WITHPS7BOARD_EBAZ_7020
	XGpioPs_SetDirectionPin(& xc7z_gpio, pin, 0);
#endif
}

void xc7z_gpio_output(uint8_t pin)
{
#if CPUSTYLE_XC7Z
	XGpioPs_SetDirectionPin(& xc7z_gpio, pin, 1);
	XGpioPs_SetOutputEnablePin(& xc7z_gpio, pin, 1);
#endif /* CPUSTYLE_XC7Z */
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

uint16_t i2chw_write(uint16_t slave_address, const uint8_t * buf, uint32_t size)
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

volatile uint32_t * ftw, * ftw_sub, * rts, * modem_ctrl,  * ph_fifo, * iq_count_rx, * iq_count_tx, * iq_fifo_rx, * iq_fifo_tx;
static uint8_t rx_fir_shift = 0, rx_cic_shift = 0, tx_shift = 0, tx_state = 0, resetn_modem = 1, hw_vfo_sel = 0;
const uint8_t rx_cic_shift_min = 32, rx_cic_shift_max = 64, rx_fir_shift_min = 32, rx_fir_shift_max = 56, tx_shift_min = 16, tx_shift_max = 30;
int fd_int = 0;

void linux_iq_init(void)
{
	ftw = 			get_highmem_ptr(AXI_IQ_FTW_ADDR);
	ftw_sub = 		get_highmem_ptr(AXI_IQ_FTW_SUB_ADDR);
	rts = 			get_highmem_ptr(AXI_IQ_RTS_ADDR);
	ph_fifo = 		get_highmem_ptr(AXI_FIFO_PHONES_ADDR);
	iq_fifo_rx = 	get_highmem_ptr(AXI_IQ_FIFO_RX_ADDR);
	iq_fifo_tx = 	get_highmem_ptr(AXI_IQ_FIFO_TX_ADDR);
	modem_ctrl = 	get_highmem_ptr(AXI_MODEM_CTRL_ADDR);
	iq_count_rx = 	get_highmem_ptr(AXI_IQ_RX_COUNT_ADDR);
	iq_count_tx = 	get_highmem_ptr(AXI_IQ_TX_COUNT_ADDR);

	reg_write(AXI_ADI_ADDR + AUDIO_REG_I2S_CLK_CTRL, (64 / 2 - 1) << 16 | (4 / 2 - 1));
	reg_write(AXI_ADI_ADDR + AUDIO_REG_I2S_PERIOD, DMABUFFSIZE16TX);
	reg_write(AXI_ADI_ADDR + AUDIO_REG_I2S_CTRL, TX_ENABLE_MASK);

	xcz_rx_iq_shift(CALIBRATION_IQ_FIR_RX_SHIFT);
	xcz_rx_cic_shift(CALIBRATION_IQ_CIC_RX_SHIFT);
	xcz_tx_shift(CALIBRATION_TX_SHIFT);
}

void linux_iq_thread(void)
{
	enum { CNT16TX = DMABUFFSIZE16TX / DMABUFFSTEP16TX };
	enum { CNT32RX = DMABUFFSIZE32RX / DMABUFFSTEP32RX };
	static int rx_stage = 0;

	uint32_t iqcnt = * iq_count_rx;

	if (iqcnt >= DMABUFFSIZE32RX)
	{
		uintptr_t addr32rx = allocate_dmabuffer32rx();
		uint32_t * r = (uint32_t *) addr32rx;

		for (int i = 0; i < DMABUFFSIZE32RX; i ++)
			r[i] = * iq_fifo_rx;

		processing_dmabuffer32rx(addr32rx);
		processing_dmabuffer32rts(addr32rx);
		release_dmabuffer32rx(addr32rx);

		rx_stage += CNT32RX;

		while (rx_stage >= CNT16TX)
		{
			const uintptr_t addr2 = getfilled_dmabuffer16txphones();
			uint32_t * b = (uint32_t *) addr2;

			for (int i = 0; i < DMABUFFSIZE16TX; i ++)
				* ph_fifo = b[i];


			release_dmabuffer16tx(addr2);
			rx_stage -= CNT16TX;
		}
	}

	if (* iq_count_tx < DMABUFFSIZE32TX)
	{
		uintptr_t addr_mic = allocate_dmabuffer16rx();
		// todo: fill buffer from mic FIFO
		processing_dmabuffer16rx(addr_mic);

		const uintptr_t addr = getfilled_dmabuffer32tx_main();
		uint32_t * r = (uint32_t *) addr;

		for (uint16_t i = 0; i < DMABUFFSIZE32TX / 2; i ++)				// 16 bit
			* iq_fifo_tx = r[i];

		release_dmabuffer32tx(addr);
	}
}

void linux_iq_interrupt_thread(void)
{
	uint32_t uio_key = 1;
	uint32_t uio_value = 0;

	fd_int = open(LINUX_IQ_INT_FILE, O_RDWR);
    if (fd_int < 0) {
        PRINTF("%s open failed\n", LINUX_IQ_INT_FILE);
        return;
    }

    while(1)
    {
#if 1
        //Acknowledge IRQ
        if (write(fd_int, &uio_key, sizeof(uio_key)) < 0) {
            PRINTF("Failed to acknowledge IRQ: %s\n", strerror(errno));
            return;
        }

        //Wait for next IRQ
        if (read(fd_int, &uio_value, sizeof(uio_value)) < 0) {
            PRINTF("Failed to wait for IRQ: %s\n", strerror(errno));
            return;
        }

        linux_iq_thread();
#else
        linux_iq_thread();
        usleep(100);
#endif
    }
}

/*************************************************************/

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

void linux_create_thread(pthread_t * tid, void * process, int priority, int cpuid)
{
	pthread_attr_t attr;
	struct sched_param param;
	cpu_set_t mask;

	ASSERT(cpuid < sysconf(_SC_NPROCESSORS_ONLN));

	pthread_attr_init(& attr);
	pthread_attr_setinheritsched(& attr, PTHREAD_EXPLICIT_SCHED);
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

void linux_subsystem_init(void)
{
#if 0 //CPUSTYLE_XCZU
	char spid[6];
	local_snprintf_P(spid, ARRAY_SIZE(spid), "%d", getpid());
	const char * argv [5] = { "/usr/bin/taskset", "-p", "1", spid, NULL, };
	linux_run_shell_cmd(argv);
#endif /* CPUSTYLE_XCZU */

	linux_xgpio_init();
	linux_iq_init();
}

pthread_t timer_spool_t, encoder_spool_t, iq_interrupt_t, ft8_t, nmea_t, pps_t, disp_t;

#if WITHCPUTEMPERATURE && CPUSTYLE_XCZU
#include "../sysmon/xsysmonpsu.h"
static XSysMonPsu xczu_sysmon;

float xczu_get_cpu_temperature(void)
{
	u32 TempRawData = XSysMonPsu_GetAdcData(& xczu_sysmon, XSM_CH_TEMP, XSYSMON_PS);
	return XSysMonPsu_RawToTemperature_OnChip(TempRawData);
}
#endif /* WITHCPUTEMPERATURE && CPUSTYLE_XCZU */

#if WITHLINUXKERNELINT
static void signal_handler(int n, siginfo_t * info, void * unused)
{
	linux_iq_thread();
}
#endif /* #if WITHLINUXKERNELINT */

void linux_user_init(void)
{
	xcz_resetn_modem_state(0);
	usleep(5);
	xcz_resetn_modem_state(1);

	linux_create_thread(& timer_spool_t, process_linux_timer_spool, 50, 0);
	linux_create_thread(& encoder_spool_t, linux_encoder_spool, 50, 0);

#if WITHLINUXKERNELINT
	struct sigaction sig;
	sig.sa_sigaction = signal_handler;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, & sig, NULL);

	const char * argv [] = { "/sbin/modprobe", "inttest", NULL, };
	linux_run_shell_cmd(argv);
	usleep(200000);

	int fs = open("/dev/iq_irq", O_RDONLY);
	ioctl(fs, 0, getpid());
	close(fs);
#else
	linux_create_thread(& iq_interrupt_t, linux_iq_interrupt_thread, 80, 1);
#endif /* ! WITHLINUXKERNELINT */

#if WITHFT8
	linux_create_thread(& ft8_t, ft8_thread, 50, 0);
#endif /* WITHFT8 */

#if defined AXI_DCDC_PWM_ADDR
	const float FS = powf(2, 32);
	uint32_t fan_pwm_period = 25000 * FS / REFERENCE_FREQ;
	reg_write(AXI_DCDC_PWM_ADDR + 0, fan_pwm_period);

	uint32_t fan_pwm_duty = FS * (1.0f - 0.8f) - 1;
	reg_write(AXI_DCDC_PWM_ADDR + 4, fan_pwm_duty);
#endif /* defined AXI_DCDC_PWM_ADDR */

#if WITHCPUTEMPERATURE && CPUSTYLE_XCZU
	XSysMonPsu_Config * ConfigPtr = XSysMonPsu_LookupConfig(0);
	XSysMonPsu_CfgInitialize(& xczu_sysmon, ConfigPtr, ConfigPtr->BaseAddress);
	int Status = XSysMonPsu_SelfTest(& xczu_sysmon);
	if (Status != XST_SUCCESS) {
		PRINTF("sysmon init error %d\n", Status);
		ASSERT(0);
	}
	XSysMonPsu_SetSequencerMode(& xczu_sysmon, XSM_SEQ_MODE_SAFE, XSYSMON_PS);
	XSysMonPsu_SetAvg(& xczu_sysmon, XSM_AVG_256_SAMPLES, XSYSMON_PS);
#endif /* WITHCPUTEMPERATURE && CPUSTYLE_XCZU */

#if WITHNMEA && WITHLFM
	linux_create_thread(& nmea_t, linux_nmea_spool, 20, 0);
	linux_create_thread(& pps_t, linux_pps_thread, 90, 1);
#endif /* WITHNMEA && WITHLFM */

}

/****************************************************************/

pthread_mutex_t md = PTHREAD_MUTEX_INITIALIZER;

void lclspin_lock(lclspinlock_t * __restrict p, const char * file, int line)
{
	pthread_mutex_lock(p);
}

void lclspin_unlock(lclspinlock_t * __restrict p)
{
	pthread_mutex_unlock(p);
}

void update_modem_ctrl(void)
{
	uint32_t v = ((rx_fir_shift & 0xFF) << rx_fir_shift_pos) | ((tx_shift & 0xFF) << tx_shift_pos)
			| ((rx_cic_shift & 0xFF) << rx_cic_shift_pos) | (!!tx_state << tx_state_pos)
			| (!!resetn_modem << resetn_modem_pos) | (!!hw_vfo_sel << hw_vfo_sel_pos);

	* modem_ctrl = v;
}

void xcz_resetn_modem_state(uint8_t val)
{
	resetn_modem = val != 0;
	update_modem_ctrl();
}

void xcz_rxtx_state(uint8_t tx)
{
	tx_state = tx != 0;
	update_modem_ctrl();
}

void xcz_dds_ftw(const uint_least64_t * val)
{
	uint32_t v = * val;
    * ftw = v;
    mirror_nco1 = v;
}

void xcz_dds_rts(const uint_least64_t * val)
{
	uint32_t v = * val;
    * rts = v;
    mirror_ncorts = v;
}

uint32_t xcz_rx_iq_shift(uint8_t val) // 52
{
	if (val > 0)
	{
		if (val >= rx_fir_shift_min && val <= rx_fir_shift_max)
			rx_fir_shift = val & 0xFF;

		update_modem_ctrl();
	}
	return rx_fir_shift;
}

void xcz_dds_ftw_sub(const uint_least64_t * val)
{
	uint32_t v = * val;
	* ftw_sub = v;
	mirror_nco2 = v;
}

uint32_t xcz_rx_cic_shift(uint32_t val)
{
	if (val > 0)
	{
		if (val >= rx_cic_shift_min && val <= rx_cic_shift_max)
			rx_cic_shift = val & 0xFF;

		update_modem_ctrl();
	}
	return rx_cic_shift;
}

uint32_t xcz_tx_shift(uint32_t val)
{
	if (val > 0)
	{
		if (val >= tx_shift_min && val <= tx_shift_max)
			tx_shift = val & 0xFF;

		update_modem_ctrl();
	}
	return tx_shift;
}

uint32_t xcz_cic_test_process(void)
{
	return 0;
}

void xcz_cic_test(uint32_t val) {}

#if WITHHWDUALVFO

void hamradio_set_hw_vfo(uint_fast8_t v)
{
	hw_vfo_sel = v != 0;
	update_modem_ctrl();
}

#endif /* WITHHWDUALVFO */

#if WITHDSPEXTFIR
volatile uint32_t * fir_reload = NULL;
static adapter_t plfircoefsout;		/* параметры прербразования к PL */

void board_fpga_fir_initialize(void)
{
	adpt_initialize(& plfircoefsout, HARDWARE_COEFWIDTH, 0, "fpgafircoefsout");
	fir_reload = get_highmem_ptr(AXI_FIR_RELOAD_ADDR);
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
#endif /* WITHDSPEXTFIR */

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
	struct tm tm;
	struct timeval tv;

	tm.tm_sec = seconds;
	tm.tm_min = minutes;
	tm.tm_hour = hours;
	tm.tm_mday = dayofmonth;
	tm.tm_mon = month - 1;
	tm.tm_year = year + 2000 - 1900;

	tv.tv_sec = mktime(& tm);;
	tv.tv_usec = 0;

	if (settimeofday(& tv, NULL) == -1)
		perror("settimeofday");
}

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

void arm_hardware_set_handler_overrealtime(uint_fast16_t int_id, void (* handler)(void)) 	{}
void arm_hardware_set_handler_realtime(uint_fast16_t int_id, void (* handler)(void)) 		{}
void arm_hardware_set_handler_system(uint_fast16_t int_id, void (* handler)(void)) 			{}

void linux_exit(void)
{
	linux_cancel_thread(timer_spool_t);
	linux_cancel_thread(encoder_spool_t);
	linux_cancel_thread(iq_interrupt_t);
#if WITHNMEA
	linux_cancel_thread(nmea_t);
	linux_cancel_thread(pps_t);
#endif /* WITHNMEA */
#if WITHFT8
	linux_cancel_thread(ft8_t);
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

	framebuffer_close();
	exit(EXIT_SUCCESS);
}

#if CPUSTYLE_XC7Z
unsigned long xc7z_get_arm_freq(void)
{
	return 766000000uL;
}
#endif

void RiseIrql_DEBUG(IRQL_t newIRQL, IRQL_t * oldIrql, const char * file, int line) {}
void LowerIrql(IRQL_t newIRQL) {}

#if CS_BY_I2C

static uint8_t cs_nvram = 1;
static uint8_t cs_ext1 = 1;
static uint8_t cs_ext2 = 1;

void pcf8575_write_pins(uint8_t buf[2])
{
#if WITHTWIHW
		i2chw_write(PCF8575CTS_ADDR, buf, 2);
#elif WITHTWISW
		i2c_start(PCF8575CTS_ADDR);
		i2c_write(buf[0]);
		i2c_write(buf[1]);
#endif /* WITHTWIHW */
}

void cs_i2c_assert(spitarget_t target)
{
	if (target < 256)
	{
		gpio_writepin((target), 0);
	}
	else
	{
		uint8_t buf[2] = { 0, 0, };

		cs_nvram = target == targetnvram ? 0 : 1;
		cs_ext1 = target == targetext1 ? 0 : 1;
		cs_ext2 = target == targetext2 ? 0 : 1;

		buf[1] = (cs_nvram << 0) | (cs_ext1 << 1) | (cs_ext1 << 2);
		pcf8575_write_pins(buf);
	}
}

void cs_i2c_deassert(spitarget_t target)
{
	if (target < 256)
	{
		gpio_writepin(targetctl1, 1);
	}
	else
	{
		uint8_t buf[2] = { 0, 0, };
		cs_nvram = 1;
		cs_ext1 = 1;
		cs_ext2 = 1;

		buf[1] = (cs_nvram << 0) | (cs_ext1 << 1) | (cs_ext1 << 2);
		pcf8575_write_pins(buf);
	}
}

void cs_i2c_disable(void)
{
	gpio_writepin(targetctl1, 1);

	uint8_t buf[2] = { 0, 0, };
	cs_nvram = 1;
	cs_ext1 = 1;
	cs_ext2 = 1;

	buf[1] = (cs_nvram << 0) | (cs_ext1 << 1) | (cs_ext1 << 2);
	pcf8575_write_pins(buf);
}

#endif /* CS_BY_I2C */

#endif /* LINUX_SUBSYSTEM */
