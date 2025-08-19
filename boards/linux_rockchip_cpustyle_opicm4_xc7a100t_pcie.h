/* $Id$ */
/* board-specific CPU attached signals */
/*
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
 *
 * Комплект из двух плат: процессорная на основе модуля Orange Pi CM4 (Rockchip RK3566 + 2Гб DDR4)
 * и RF frontend FPGA Artix-7 XC7A100T (AD9361, LTC2208 и DAC904E), интерфейс связи - PCI Express x1
 * by RA4ASN
 *
 */

#ifndef LINUX_ROCKCHIP_CPUSTYLE_OPICM4_XC7A100T_PCIE_H_INCLUDED
#define LINUX_ROCKCHIP_CPUSTYLE_OPICM4_XC7A100T_PCIE_H_INCLUDED 1

#define WITHLTDCHW			1	/* Наличие контроллера дисплея с framebuffer-ом */
#define WITHLCDBACKLIGHTMAX	0
//#define WITHSPIDEV		1	/* Linux SPI userspace API */
#define WITHTWIHW 			1	/* Использование аппаратного контроллера TWI (I2C) */
#define WITHSDL2VIDEO		1	/* Вывод графики посредством Linux Simple DirectMedia Layer v2 */
//#define WITHFBDEV			1

#define	AXI_IQ_RX_BRAM				0xC0000000
#define	AXI_IQ_STREAM_BRAM			0xC0010000
#define	AXI_LITE_IQ_RX_BRAM_CNT		0x00001000
#define	AXI_LITE_DDS_FTW			0x00002000
#define	AXI_LITE_DDS_RTS			0x00003000
#define	AXI_LITE_MODEM_CONTROL		0x00004000
#define	AXI_LITE_WNB_CONFIG			0x00005000
//#define	AXI_LITE_DDS_FTW_SUB		0x00006000
#define	AXI_LITE_IQ_TX_FIFO			0x00007000
#define	AXI_LITE_STREAM_RATE		0x00008000
#define	AXI_LITE_STREAM_POS			0x00009000
#define	AXI_LITE_FIR_COEFFS			0x0000C000

#define CALIBRATION_IQ_FIR_RX_SHIFT		56	// 56 - sw FIR, 50 - hw FIR
#define CALIBRATION_IQ_CIC_RX_SHIFT		62
#define CALIBRATION_TX_SHIFT			27

#define LINUX_XDMA_IQ_EVENT_FILE	"/dev/xdma0_events_0"
//#define LINUX_STREAM_INT_FILE		"/dev/xdma0_events_1"
#define LINUX_AD936X_INT_FILE		"/dev/xdma0_events_2"
#define LINUX_I2C_FILE				"/dev/i2c-3"
#define LINUX_FB_FILE				"/dev/fb0"
#define LINUX_TTY_FILE				"/dev/tty0"
#define TOUCH_EVENT_NAME			"fts_ts"
#define MOUSE_EVENT_NAME			"Mouse"
#define MEMORYCELLS_DB_FILE			"/usr/hftrx_data/memory_cells.db"
#define NVRAM_DB_FILE				"/usr/hftrx_data/nvram.db"

#if WITHCPUTEMPERATURE
	#define GET_CPU_TEMPERATURE() (linux_get_cpu_temp())
#endif /* WITHCPUTEMPERATURE */

#if WITHTX

	#define HARDWARE_GET_TXDISABLE() 	(0)
	#define HARDWARE_GET_PTT() 			(0)
	#define HARDWARE_GET_TUNE() 		(0)

#endif /* WITHTX */

#if WITHSPIDEV

	#define SPIDEV_PATH	"/dev/spidev3"

	// CS lines
	enum {
		cs0,
		cs1,
//		cs2,

		cs_cnt
	};

#define targetctl1	cs1

#endif /* WITHSPIDEV */

/* макроопределение, которое должно включить в себя все инициализации */
#define	HARDWARE_INITIALIZE()   do {} while (0)
#define HARDWARE_DEBUG_FLUSH()	do {} while (0)
#define ENCODER_INITIALIZE()	do {} while (0)

#endif /* LINUX_ROCKCHIP_CPUSTYLE_OPICM4_XC7A100T_PCIE_H_INCLUDED */
