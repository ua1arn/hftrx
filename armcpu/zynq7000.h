/*
 * zynq7000.h
 *
 *  Created on: Dec 23, 2020
 *      Author: gena
 */

#ifndef ARMCPU_ZYNQ7000_H_
#define ARMCPU_ZYNQ7000_H_

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/******  SGI Interrupts Numbers                 ****************************************/
  SGI0_IRQn           =  0,                                       //!< SGI0_IRQn
  SGI1_IRQn           =  1,                                       //!< SGI1_IRQn
  SGI2_IRQn           =  2,                                       //!< SGI2_IRQn
  SGI3_IRQn           =  3,                                       //!< SGI3_IRQn
  SGI4_IRQn           =  4,                                       //!< SGI4_IRQn
  SGI5_IRQn           =  5,                                       //!< SGI5_IRQn
  SGI6_IRQn           =  6,                                       //!< SGI6_IRQn
  SGI7_IRQn           =  7,                                       //!< SGI7_IRQn
  SGI8_IRQn           =  8,                                       //!< SGI8_IRQn
  SGI9_IRQn           =  9,                                       //!< SGI9_IRQn
  SGI10_IRQn          = 10,                                       //!< SGI10_IRQn
  SGI11_IRQn          = 11,                                       //!< SGI11_IRQn
  SGI12_IRQn          = 12,                                       //!< SGI12_IRQn
  SGI13_IRQn          = 13,                                       //!< SGI13_IRQn
  SGI14_IRQn          = 14,                                       //!< SGI14_IRQn
  SGI15_IRQn          = 15,                                       //!< SGI15_IRQn

  /* Private Peripheral Interrupts                                                                                     */
  //VirtualMaintenanceInterrupt_IRQn = 25,     /*!< Virtual Maintenance Interrupt                                        */
 // HypervisorTimer_IRQn             = 26,     /*!< Hypervisor Timer Interrupt                                           */
  GlobalTimer_IRQn                 = 27,     /*!< Global Timer Interrupt                                              */
  Legacy_nFIQ_IRQn                 = 28,     /*!< Legacy nFIQ Interrupt                                                */
  PrivTimer_IRQn        	   	   = 29,     /*!< Private Timer Interrupt                                      */
  AwdtTimer_IRQn      			   = 30,     /*!< Private watchdog timer for each CPU Interrupt                                  */
  Legacy_nIRQ_IRQn                 = 31,     /*!< Legacy nIRQ Interrupt                                                */

  /******  ZYNQ  specific Interrupt Numbers ****************************************************************************/

	/* interrupts */
	TTC0_A_IRQn 		= 42,
	TTC0_B_IRQn 		= 43,
	TTC0_C_IRQn			= 44,
	GPIO_IRQn 			= 52,
	USB0_IRQn 			= 53,
	ETH0_IRQn 			= 54,
	ETH0_WAKE_IRQn 		= 55,
	SDIO0_IRQn 			= 56,
	I2C0_IRQn 			= 57,
	SPI0_IRQn 			= 58,
	UART0_IRQn 			= 59,
	UART1_IRQn 			= 82,
	TTC1_A_IRQn 		= 69,
	TTC2_B_IRQn 		= 70,
	TTC3_C_IRQn 		= 71,
	ETH1_IRQn 			= 77,
	ETH1_WAKE_IRQn		 = 78,

	/* Perhipheral IRQs from fabric */
	F2P0_IRQn = 61,
	F2P1_IRQn = 62,
	F2P2_IRQn = 63,
	F2P3_IRQn = 64,
	F2P4_IRQn = 65,
	F2P5_IRQn = 66,
	F2P6_IRQn = 67,
	F2P7_IRQn = 68,

	F2P8_IRQn = 84,
	F2P9_IRQn = 85,
	F2P10_IRQn = 86,
	F2P11_IRQn = 87,
	F2P12_IRQn = 88,
	F2P13_IRQn = 89,
	F2P14_IRQn = 90,
	F2P15_IRQn = 91,

	Force_IRQn_enum_size             = 1048    /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */

} IRQn_Type;

//Info: Found Cortex-A9 r3p0
//Info: 6 code breakpoints, 4 data breakpoints
//Info: Debug architecture ARMv7.0
//Info: Data endian: little
//Info: Main ID register: 0x413FC090
//Info: I-Cache L1: 32 KB, 256 Sets, 32 Bytes/Line, 4-Way
//Info: D-Cache L1: 32 KB, 256 Sets, 32 Bytes/Line, 4-Way

/******************************************************************************/
/*                Device Specific Peripheral Section                          */
/******************************************************************************/

#define CPUPRIV_BASE      (0xF8F00000uL)
#define SCU_CONTROL_BASE  (CPUPRIV_BASE + 0x0000uL)
#define GIC_PROC_BASE     (CPUPRIV_BASE + 0x0100uL)
#define GLOBAL_TIMER_BASE (CPUPRIV_BASE + 0x0200uL)
#define PRIV_TIMER_BASE   (CPUPRIV_BASE + 0x0600uL)
#define GIC_DISTRIB_BASE  (CPUPRIV_BASE + 0x1000uL)
#define L2CACHE_BASE      (CPUPRIV_BASE + 0x2000uL)

#define __CORTEX_A                    9U      /*!< Cortex-A# Core                              */
//#define __CA_REV                 0x0005U      /*!< Core revision r0p0                          */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 1U      /*!< Set to 1 if L2C is present                  */

#define GIC_DISTRIBUTOR_BASE         GIC_DISTRIB_BASE                        /*!< (GIC DIST  ) Base Address */
#define GIC_INTERFACE_BASE           GIC_PROC_BASE                        /*!< (GIC CPU IF) Base Address */
#define L2C_310_BASE                 L2CACHE_BASE                        /*!< (PL310     ) Base Address */
#define TIMER_BASE				PRIV_TIMER_BASE

/* --------  Configuration of the Cortex-A9 Processor and Core Peripherals  ------- */
#define __CA_REV         0x0000    /*!< Core revision r0       */

#include "core_ca.h"
#include "system_zynq7000.h"

// See
// https://github.com/mtk-watch/android_trusty/blob/320a422109b955a03443ae92b011ade929a3f812/external/lk/platform/zynq/swdt.c
// https://github.com/mtk-watch/android_trusty/blob/320a422109b955a03443ae92b011ade929a3f812/external/lk/platform/zynq/include/platform/zynq.h

/** @name Register Map
 *
 * Registers of the UART.
 * @{
 */
typedef struct
{
    volatile uint32_t CR;            /**< Control Register */
    volatile uint32_t MR;            /**< Mode Register */
    volatile uint32_t IER;           /**< Interrupt Enable */
    volatile uint32_t IDR;           /**< Interrupt Disable */
    volatile uint32_t IMR;           /**< Interrupt Mask */
    volatile uint32_t ISR;           /**< Interrupt Status */
    volatile uint32_t BAUDGEN;       /**< Baud Rate Generator "CD" */
    volatile uint32_t RXTOUT;        /**< RX Timeout */
    volatile uint32_t RXWM;          /**< RX FIFO Trigger Level */
    volatile uint32_t MODEMCR;       /**< Modem Control */
    volatile uint32_t MODEMSR;       /**< Modem Status */
    volatile uint32_t SR;            /**< Channel Status */
    volatile uint32_t FIFO;          /**< FIFO */
    volatile uint32_t BAUDDIV;       /**< Baud Rate Divider "DIV" */
    volatile uint32_t FLOWDEL;       /**< Flow Delay */
    volatile uint32_t RESERVED1;
    volatile uint32_t RESERVED2;
    volatile uint32_t TXWM;            /* TX FIFO Trigger Level */
} UART_Registers;

/* system watchdog timer */
typedef struct swdt_regs {
	volatile uint32_t MODE;
	volatile uint32_t CONTROL;
	volatile uint32_t RESTART;
	volatile uint32_t STATUS;
} SWDT_Registers;


/* SLCR registers */
typedef struct slcr_regs {
	volatile uint32_t SCL;                             // Secure Configuration Lock
	volatile uint32_t SLCR_LOCK;                       // SLCR Write Protection Lock
	volatile uint32_t SLCR_UNLOCK;                     // SLCR Write Protection Unlock
	volatile uint32_t SLCR_LOCKSTA;                    // SLCR Write Protection Status
    uint32_t reserved0[60];
    volatile uint32_t ARM_PLL_CTRL;                    // ARM PLL Control
    volatile uint32_t DDR_PLL_CTRL;                    // DDR PLL Control
    volatile  uint32_t IO_PLL_CTRL;                     // IO PLL Control
    volatile uint32_t PLL_STATUS;                      // PLL Status
    volatile uint32_t ARM_PLL_CFG;                     // ARM PLL Configuration
    volatile uint32_t DDR_PLL_CFG;                     // DDR PLL Configuration
    volatile uint32_t IO_PLL_CFG;                      // IO PLL Configuration
    uint32_t reserved1[1];
    volatile uint32_t ARM_CLK_CTRL;                    // CPU Clock Control
    volatile uint32_t DDR_CLK_CTRL;                    // DDR Clock Control
    volatile uint32_t DCI_CLK_CTRL;                    // DCI clock control
    volatile uint32_t APER_CLK_CTRL;                   // AMBA Peripheral Clock Control
    volatile uint32_t USB0_CLK_CTRL;                   // USB 0 ULPI Clock Control
    volatile uint32_t USB1_CLK_CTRL;                   // USB 1 ULPI Clock Control
    volatile uint32_t GEM0_RCLK_CTRL;                  // GigE 0 Rx Clock and Rx Signals Select
    volatile uint32_t GEM1_RCLK_CTRL;                  // GigE 1 Rx Clock and Rx Signals Select
    volatile uint32_t GEM0_CLK_CTRL;                   // GigE 0 Ref Clock Control
    volatile uint32_t GEM1_CLK_CTRL;                   // GigE 1 Ref Clock Control
    volatile uint32_t SMC_CLK_CTRL;                    // SMC Ref Clock Control
    volatile uint32_t LQSPI_CLK_CTRL;                  // Quad SPI Ref Clock Control
    volatile uint32_t SDIO_CLK_CTRL;                   // SDIO Ref Clock Control
    volatile uint32_t UART_CLK_CTRL;                   // UART Ref Clock Control
    volatile uint32_t SPI_CLK_CTRL;                    // SPI Ref Clock Control
    volatile uint32_t CAN_CLK_CTRL;                    // CAN Ref Clock Control
    volatile uint32_t CAN_MIOCLK_CTRL;                 // CAN MIO Clock Control
    volatile uint32_t DBG_CLK_CTRL;                    // SoC Debug Clock Control
    volatile uint32_t PCAP_CLK_CTRL;                   // PCAP Clock Control
    volatile uint32_t TOPSW_CLK_CTRL;                  // Central Interconnect Clock Control
    volatile uint32_t FPGA0_CLK_CTRL;                  // PL Clock 0 Output control
    volatile uint32_t FPGA0_THR_CTRL;                  // PL Clock 0 Throttle control
    volatile uint32_t FPGA0_THR_CNT;                   // PL Clock 0 Throttle Count control
    volatile uint32_t FPGA0_THR_STA;                   // PL Clock 0 Throttle Status read
    volatile uint32_t FPGA1_CLK_CTRL;                  // PL Clock 1 Output control
    volatile uint32_t FPGA1_THR_CTRL;                  // PL Clock 1 Throttle control
    volatile uint32_t FPGA1_THR_CNT;                   // PL Clock 1 Throttle Count
    volatile uint32_t FPGA1_THR_STA;                   // PL Clock 1 Throttle Status control
    volatile uint32_t FPGA2_CLK_CTRL;                  // PL Clock 2 output control
    volatile uint32_t FPGA2_THR_CTRL;                  // PL Clock 2 Throttle Control
    volatile uint32_t FPGA2_THR_CNT;                   // PL Clock 2 Throttle Count
    volatile uint32_t FPGA2_THR_STA;                   // PL Clock 2 Throttle Status
    volatile uint32_t FPGA3_CLK_CTRL;                  // PL Clock 3 output control
    volatile uint32_t FPGA3_THR_CTRL;                  // PL Clock 3 Throttle Control
    volatile uint32_t FPGA3_THR_CNT;                   // PL Clock 3 Throttle Count
    volatile uint32_t FPGA3_THR_STA;                   // PL Clock 3 Throttle Status
    uint32_t reserved2[5];
    volatile uint32_t CLK_621_TRUE;                    // CPU Clock Ratio Mode select
    uint32_t reserved3[14];
    volatile uint32_t PSS_RST_CTRL;                    // PS Software Reset Control
    volatile uint32_t DDR_RST_CTRL;                    // DDR Software Reset Control
    volatile uint32_t TOPSW_RST_CTRL;                  // Central Interconnect Reset Control
    volatile uint32_t DMAC_RST_CTRL;                   // DMAC Software Reset Control
    volatile uint32_t USB_RST_CTRL;                    // USB Software Reset Control
    volatile uint32_t GEM_RST_CTRL;                    // Gigabit Ethernet SW Reset Control
    volatile uint32_t SDIO_RST_CTRL;                   // SDIO Software Reset Control
    volatile uint32_t SPI_RST_CTRL;                    // SPI Software Reset Control
    volatile uint32_t CAN_RST_CTRL;                    // CAN Software Reset Control
    volatile uint32_t I2C_RST_CTRL;                    // I2C Software Reset Control
    volatile uint32_t UART_RST_CTRL;                   // UART Software Reset Control
    volatile uint32_t GPIO_RST_CTRL;                   // GPIO Software Reset Control
    volatile uint32_t LQSPI_RST_CTRL;                  // Quad SPI Software Reset Control
    volatile uint32_t SMC_RST_CTRL;                    // SMC Software Reset Control
    volatile uint32_t OCM_RST_CTRL;                    // OCM Software Reset Control
    uint32_t reserved4[1];
    volatile uint32_t FPGA_RST_CTRL;                   // FPGA Software Reset Control
    volatile uint32_t A9_CPU_RST_CTRL;                 // CPU Reset and Clock control
    uint32_t reserved5[1];
    volatile uint32_t RS_AWDT_CTRL;                    // Watchdog Timer Reset Control
    uint32_t reserved6[2];
    volatile uint32_t REBOOT_STATUS;                   // Reboot Status, persistent
    volatile uint32_t BOOT_MODE;                       // Boot Mode Strapping Pins
    uint32_t reserved7[40];
    volatile uint32_t APU_CTRL;                        // APU Control
    volatile uint32_t WDT_CLK_SEL;                     // SWDT clock source select
    uint32_t reserved8[78];
    volatile uint32_t TZ_DMA_NS;                       // DMAC TrustZone Config
    volatile uint32_t TZ_DMA_IRQ_NS;                   // DMAC TrustZone Config for Interrupts
    volatile uint32_t TZ_DMA_PERIPH_NS;                // DMAC TrustZone Config for Peripherals
    uint32_t reserved9[57];
    volatile uint32_t PSS_IDCODE;                      // PS IDCODE
    uint32_t reserved10[51];
    volatile uint32_t DDR_URGENT;                      // DDR Urgent Control
    uint32_t reserved11[2];
    volatile uint32_t DDR_CAL_START;                   // DDR Calibration Start Triggers
    uint32_t reserved12[1];
    volatile uint32_t DDR_REF_START;                   // DDR Refresh Start Triggers
    volatile uint32_t DDR_CMD_STA;                     // DDR Command Store Status
    volatile uint32_t DDR_URGENT_SEL;                  // DDR Urgent Select
    volatile uint32_t DDR_DFI_STATUS;                  // DDR DFI status
    uint32_t reserved13[55];
    volatile uint32_t MIO_PIN_00;                      // MIO Pin 0 Control
    volatile uint32_t MIO_PIN_01;                      // MIO Pin 1 Control
    volatile uint32_t MIO_PIN_02;                      // MIO Pin 2 Control
    volatile uint32_t MIO_PIN_03;                      // MIO Pin 3 Control
    volatile uint32_t MIO_PIN_04;                      // MIO Pin 4 Control
    volatile uint32_t MIO_PIN_05;                      // MIO Pin 5 Control
    volatile uint32_t MIO_PIN_06;                      // MIO Pin 6 Control
    volatile uint32_t MIO_PIN_07;                      // MIO Pin 7 Control
    volatile uint32_t MIO_PIN_08;                      // MIO Pin 8 Control
    volatile uint32_t MIO_PIN_09;                      // MIO Pin 9 Control
    volatile uint32_t MIO_PIN_10;                      // MIO Pin 10 Control
    volatile uint32_t MIO_PIN_11;                      // MIO Pin 11 Control
    volatile uint32_t MIO_PIN_12;                      // MIO Pin 12 Control
    volatile uint32_t MIO_PIN_13;                      // MIO Pin 13 Control
    volatile uint32_t MIO_PIN_14;                      // MIO Pin 14 Control
    volatile uint32_t MIO_PIN_15;                      // MIO Pin 15 Control
    volatile uint32_t MIO_PIN_16;                      // MIO Pin 16 Control
    volatile uint32_t MIO_PIN_17;                      // MIO Pin 17 Control
    volatile uint32_t MIO_PIN_18;                      // MIO Pin 18 Control
    volatile uint32_t MIO_PIN_19;                      // MIO Pin 19 Control
    volatile uint32_t MIO_PIN_20;                      // MIO Pin 20 Control
    volatile uint32_t MIO_PIN_21;                      // MIO Pin 21 Control
    volatile uint32_t MIO_PIN_22;                      // MIO Pin 22 Control
    volatile uint32_t MIO_PIN_23;                      // MIO Pin 23 Control
    volatile uint32_t MIO_PIN_24;                      // MIO Pin 24 Control
    volatile uint32_t MIO_PIN_25;                      // MIO Pin 25 Control
    volatile uint32_t MIO_PIN_26;                      // MIO Pin 26 Control
    volatile uint32_t MIO_PIN_27;                      // MIO Pin 27 Control
    volatile uint32_t MIO_PIN_28;                      // MIO Pin 28 Control
    volatile uint32_t MIO_PIN_29;                      // MIO Pin 29 Control
    volatile uint32_t MIO_PIN_30;                      // MIO Pin 30 Control
    volatile uint32_t MIO_PIN_31;                      // MIO Pin 31 Control
    volatile uint32_t MIO_PIN_32;                      // MIO Pin 32 Control
    volatile uint32_t MIO_PIN_33;                      // MIO Pin 33 Control
    volatile uint32_t MIO_PIN_34;                      // MIO Pin 34 Control
    volatile uint32_t MIO_PIN_35;                      // MIO Pin 35 Control
    volatile uint32_t MIO_PIN_36;                      // MIO Pin 36 Control
    volatile uint32_t MIO_PIN_37;                      // MIO Pin 37 Control
    volatile uint32_t MIO_PIN_38;                      // MIO Pin 38 Control
    volatile uint32_t MIO_PIN_39;                      // MIO Pin 39 Control
    volatile uint32_t MIO_PIN_40;                      // MIO Pin 40 Control
    volatile uint32_t MIO_PIN_41;                      // MIO Pin 41 Control
    volatile uint32_t MIO_PIN_42;                      // MIO Pin 42 Control
    volatile uint32_t MIO_PIN_43;                      // MIO Pin 43 Control
    volatile uint32_t MIO_PIN_44;                      // MIO Pin 44 Control
    volatile uint32_t MIO_PIN_45;                      // MIO Pin 45 Control
    volatile uint32_t MIO_PIN_46;                      // MIO Pin 46 Control
    volatile uint32_t MIO_PIN_47;                      // MIO Pin 47 Control
    volatile uint32_t MIO_PIN_48;                      // MIO Pin 48 Control
    volatile uint32_t MIO_PIN_49;                      // MIO Pin 49 Control
    volatile uint32_t MIO_PIN_50;                      // MIO Pin 50 Control
    volatile uint32_t MIO_PIN_51;                      // MIO Pin 51 Control
    volatile uint32_t MIO_PIN_52;                      // MIO Pin 52 Control
    volatile uint32_t MIO_PIN_53;                      // MIO Pin 53 Control
    uint32_t reserved14[11];
    volatile uint32_t MIO_LOOPBACK;                    // Loopback function within MIO
    uint32_t reserved15[1];
    volatile uint32_t MIO_MST_TRI0;                    // MIO pin Tri-state Enables, 31:0
    volatile uint32_t MIO_MST_TRI1;                    // MIO pin Tri-state Enables, 53:32
    uint32_t reserved16[7];
    volatile uint32_t SD0_WP_CD_SEL;                   // SDIO 0 WP CD select
    volatile uint32_t SD1_WP_CD_SEL;                   // SDIO 1 WP CD select
    uint32_t reserved17[50];
    volatile uint32_t LVL_SHFTR_EN;                    // Level Shifters Enable
    uint32_t reserved18[3];
    volatile uint32_t OCM_CFG;                         // OCM Address Mapping
    uint32_t reserved19[66];
    volatile uint32_t RESERVED;                        // Reserved
    uint32_t reserved20[56];
    volatile uint32_t GPIOB_CTRL;                      // PS IO Buffer Control
    volatile uint32_t GPIOB_CFG_CMOS18;                // MIO GPIOB CMOS 1.8V config
    volatile uint32_t GPIOB_CFG_CMOS25;                // MIO GPIOB CMOS 2.5V config
    volatile uint32_t GPIOB_CFG_CMOS33;                // MIO GPIOB CMOS 3.3V config
    uint32_t reserved21[1];
    volatile uint32_t GPIOB_CFG_HSTL;                  // MIO GPIOB HSTL config
    volatile uint32_t GPIOB_DRVR_BIAS_CTRL;            // MIO GPIOB Driver Bias Control
    uint32_t reserved22[9];
    volatile uint32_t DDRIOB_ADDR0;                    // DDR IOB Config for A[14:0], CKE and DRST_B
    volatile uint32_t DDRIOB_ADDR1;                    // DDR IOB Config for BA[2:0], ODT, CS_B, WE_B, RAS_B and CAS_B
    volatile uint32_t DDRIOB_DATA0;                    // DDR IOB Config for Data 15:0
    volatile uint32_t DDRIOB_DATA1;                    // DDR IOB Config for Data 31:16
    volatile uint32_t DDRIOB_DIFF0;                    // DDR IOB Config for DQS 1:0
    volatile uint32_t DDRIOB_DIFF1;                    // DDR IOB Config for DQS 3:2
    volatile uint32_t DDRIOB_CLOCK;                    // DDR IOB Config for Clock Output
    volatile uint32_t DDRIOB_DRIVE_SLEW_ADDR;          // Drive and Slew controls for Address and Command pins of the DDR Interface
    volatile uint32_t DDRIOB_DRIVE_SLEW_DATA;          // Drive and Slew controls for DQ pins of the DDR Interface
    volatile uint32_t DDRIOB_DRIVE_SLEW_DIFF;          // Drive and Slew controls for DQS pins of the DDR Interface
    volatile uint32_t DDRIOB_DRIVE_SLEW_CLOCK;         // Drive and Slew controls for Clock pins of the DDR Interface
    volatile uint32_t DDRIOB_DDR_CTRL;                 // DDR IOB Buffer Control
    volatile uint32_t DDRIOB_DCI_CTRL;                 // DDR IOB DCI Config
    volatile uint32_t DDRIOB_DCI_STATUS;               // DDR IO Buffer DCI Status
} SLCR_Registers;

#define ZYNQ_MIO_CNT    54

/* SPI */
typedef struct spi_regs {
	volatile uint32_t CR; 		/*	SPI Configuration.	 */
	volatile uint32_t SR; 		/*	SPI Interrupt Status	 */
	volatile uint32_t IER; 		/*	Interrupt Enable. 	 */
	volatile uint32_t IDR; 		/*	Interrupt disable. 	 */
	volatile uint32_t IMR; 		/*	Interrupt mask. 	 */
	volatile uint32_t ER; 		/*	SPI Controller Enable.	 */
	volatile uint32_t DR; 		/*	Delay Control 	 */
	volatile uint32_t TXD; 		/*	Transmit Data. 	 */
	volatile uint32_t RXD; 		/*	Receive Data. 	 */
	volatile uint32_t SICR; 	/*	Slave Idle Count.	 */
	volatile uint32_t TXWR; 	/*	TX_FIFO Threshold. 	 */
	volatile uint32_t RXWR;		/* 0x0000002C	RX FIFO Threshold.	 */
	uint32_t reserved1 [(0xfc - 0x30) / 4];
	volatile uint32_t Mod_id_reg0;	/* 0x000000FC	Module ID. 	 */
} SPI_Registers;

// Global Timer Registers offsets
typedef struct gtc_regs {
	volatile uint32_t GTCTRL; 	// Global_Timer_Counter_Register0
	volatile uint32_t GTCTRH; 	// Global_Timer_Counter_Register1
	volatile uint32_t GTCLR; 	// Control
	volatile uint32_t GTISR; 	// Interrupt Status
	volatile uint32_t GTCOMPL;	// Comparator_Value_Register0
	volatile uint32_t GTCOMPH; 	// Comparator_Value_Register1
	volatile uint32_t GTCAIR; 	// Auto_increment_Register
} GTC_Registers;

// https://github.com/yuhuidream/crane/blob/bc4d2b830839c0678ee5ae249378d5cf606dfa9e/arch/asr/include/SDH_v3_1.h
// SD Controller (sdio)
typedef struct sd_regs {
    __IO uint32_t SYS_ADDR                       ; /* 0x0000 System Address Low Register */
    __IO uint16_t BLOCK_SIZE                     ; /* 0x0004 Block Size Register */
    __IO uint16_t BLOCK_COUNT                    ; /* 0x0006 Block Count Register */
    __IO uint32_t ARG                     		 ; /* 0x0008 Argument Low Register */
    __IO uint16_t TRANSFER_MODE                  ; /* 0x000C Transfer Mode Register */
    __IO uint16_t CMD                            ; /* 0x000E Command Register */
    __IO uint32_t RESP_0                         ; /* 0x0010 Response Register 0 */
    __IO uint32_t RESP_1                         ; /* 0x0014 Response Register 1 */
    __IO uint32_t RESP_2                         ; /* 0x0018 Response Register 2 */
    __IO uint32_t RESP_3                         ; /* 0x001C Response Register 3 */
    __IO uint32_t BUFFER_DATA_PORT               ; /* 0x0020 Buffer Data Port Register */
    __IO uint32_t PRESENT_STATE                  ; /* 0x0024 Present State Register 1 */
    __IO uint16_t HOST_CTRL                      ; /* 0x0028 Host Control Register */
    __IO uint16_t BLOCK_GAP_CTRL                 ; /* 0x002A Block Gap Control Register */
    __IO uint16_t CLOCK_CTRL                     ; /* 0x002C Clock Control Register */
    __IO uint16_t TIMEOUT_CTRL_SW_RESET          ; /* 0x002E Timeout Control/Software Reset Register */
    __IO uint16_t NORMAL_INT_STATUS              ; /* 0x0030 Normal Interrupt Status Register */
    __IO uint16_t ERROR_INT_STATUS               ; /* 0x0032 Error Interrupt Status Register */
    __IO uint16_t NORMAL_INT_STATUS_EN           ; /* 0x0034 Normal Interrupt Status Enable Register */
    __IO uint16_t ERROR_INT_STATUS_EN            ; /* 0x0036 Error Interrupt Status Enable Register */
    __IO uint16_t NORMAL_INT_STATUS_INT_EN       ; /* 0x0038 Normal Interrupt Status Interrupt Enable Register */
    __IO uint16_t ERROR_INT_STATUS_INT_EN        ; /* 0x003A Error Interrupt Status Interrupt Enable Register */
    __IO uint8_t AUTO_CMD12_ERROR_STATUS         ; /* 0x003C Auto CMD12 Error Status Register */
    uint8_t reserved0 [3];
    __IO uint32_t CAPABILITIES                   ; /* 0x0040 Capabilities Register 1 */
    uint32_t reserved1 [1];
    __IO uint32_t MAX_CURRENT                    ; /* 0x0048 Maximum Current Register 1 */
    uint32_t reserved2 [1];
    __IO uint16_t FORCE_EVENT_AUTO_CMD12_ERROR   ; /* 0x0050 Force Event Auto cmd12 Error Register */
    __IO uint16_t FORCE_EVENT_FOR_ERROR_STATUS   ; /* 0x0052 Force Event for Error Status Register */
    __IO uint32_t ADMA_ERROR_STATUS              ; /* 0x0054 ADMA Error Status Register */
    __IO uint32_t ADMA_SYS_ADDR                  ; /* 0x0058 ADMA System Address Register */
    uint32_t reserved3 [1];
    __IO uint32_t Boot_Timeout_control           ; /* 0x0060 Boot Timeout control register */
    __IO uint32_t Debug_Selection            	 ; /* 0x0064 Debug Selection Register */
    uint8_t reserved4 [0x00F0 - 0x68];
    __IO uint8_t SPI_interrupt_support           ; /* 0x00F0 SPI interrupt support register */
    uint8_t reserved5 [0x00F1 - 0x68];
    __IO uint32_t Vendor_Version_Number          ; /* 0x00FC Shared Bus Control Register */
} SD_Registers;

/* configuration for the PL310 L2 cache controller */
#define PL310_BASE L2CACHE_BASE
#define PL310_TAG_RAM_LATENCY ((1 << 8) | (1 << 4) | (1 << 0))
#define PL310_DATA_RAM_LATENCY ((1 << 8) | (2 << 4) | (1 << 0))

/* Verify the entries match the TRM offset to validate the struct */
//STATIC_ASSERT(offsetof(struct slcr_regs, SCL) == 0x0);
//STATIC_ASSERT(offsetof(struct slcr_regs, DDRIOB_DCI_STATUS) == 0xb74);

#define DDRC_CTRL                       0xF8006000L
#define DDRC_MODE_STATUS                0xF8006054L

/* GPIO registers are not indexed in a particularly convenient manner, but can be calculated
 * via the GPIO bank */

#define GPIO_MASK_DATA_BASE         (GPIO_BASE + 0x0)
#define GPIO_MASK_DATA_LSW(bank)    (GPIO_MASK_DATA_BASE + (8 * (bank)))
#define GPIO_MASK_DATA_MSW(bank)    (GPIO_MASK_DATA_BASE + 4 + (8 * (bank)))

#define GPIO_DATA_BASE              (GPIO_BASE + 0x40)
#define GPIO_DATA(bank)             (GPIO_DATA_BASE + (4 * (bank)))

#define GPIO_DATA_RO_BASE           (GPIO_BASE + 0x60)
#define GPIO_DATA_RO(bank)          (GPIO_DATA_RO_BASE + (4 * (bank)))

#define GPIO_REGS(bank)             (GPIO_BASE + 0x204 + (0x40 * (bank)))
#define GPIO_DIRM(bank)             (GPIO_REGS(bank) + 0x0)
#define GPIO_OEN(bank)              (GPIO_REGS(bank) + 0x4)
#define GPIO_INT_MASK(bank)         (GPIO_REGS(bank) + 0x8)
#define GPIO_INT_EN(bank)           (GPIO_REGS(bank) + 0xC)
#define GPIO_INT_DIS(bank)          (GPIO_REGS(bank) + 0x10)
#define GPIO_INT_STAT(bank)         (GPIO_REGS(bank) + 0x14)
#define GPIO_INT_TYPE(bank)         (GPIO_REGS(bank) + 0x18)
#define GPIO_INT_POLARITY(bank)     (GPIO_REGS(bank) + 0x1C)
#define GPIO_INT_ANY(bank)          (GPIO_REGS(bank) + 0x20)


#define XUARTPS_CR_TXRST	0x00000002U  /**< TX logic reset */
#define XUARTPS_CR_RXRST	0x00000001U  /**< RX logic reset */

#define XUARTPS_MR_CHMODE_NORM		0x00000000UL /**< Normal mode */
#define XUARTPS_MR_STOPMODE_1_BIT	0x00000000UL /**< 1 stop bit */
#define XUARTPS_MR_PARITY_NONE		0x00000020UL /**< No parity mode */
#define XUARTPS_MR_CHARLEN_8_BIT	0x00000000UL /**< 8 bits data */
#define XUARTPS_MR_CLKSEL			0x00000001UL /**< Input clock selection */

#define XUARTPS_SR_TNFUL	0x00004000UL /**< TX FIFO Nearly Full Status */
#define XUARTPS_SR_TACTIVE	0x00000800UL /**< TX active */
#define XUARTPS_SR_RXEMPTY	0x00000002UL /**< RX FIFO empty */

#define XUARTPS_CR_TX_DIS	0x00000020UL  /**< TX disabled. */
#define XUARTPS_CR_TX_EN	0x00000010UL  /**< TX enabled */
#define XUARTPS_CR_RX_DIS	0x00000008UL  /**< RX disabled. */
#define XUARTPS_CR_RX_EN	0x00000004UL  /**< RX enabled */

/* memory addresses */
/* assumes sram is mapped at 0 the first MB of sdram is covered by it */
#define SDRAM_BASE          (0x00100000)
#define SDRAM_APERTURE_SIZE (0x3fF00000)
#define SRAM_BASE           (0x0)
#define SRAM_BASE_HIGH      (0xfffc0000)
#define SRAM_APERTURE_SIZE  (0x00040000)
#define SRAM_SIZE           (0x00040000)

/* hardware base addresses */
#define UART0_BASE (0xE0000000uL)
#define UART1_BASE (0xE0001000uL)
#define USB0_BASE  (0xE0002000uL)
#define USB1_BASE  (0xE0003000uL)
#define I2C0_BASE  (0xE0004000uL)
#define I2C1_BASE  (0xE0005000uL)
#define SPI0_BASE  (0xE0006000uL)
#define SPI1_BASE  (0xE0007000uL)
#define CAN0_BASE  (0xE0008000uL)
#define CAN1_BASE  (0xE0009000uL)
#define GPIO_BASE  (0xE000a000uL)
#define GEM0_BASE  (0xE000b000uL) // gigabit eth controller
#define GEM1_BASE  (0xE000x000uL) // ""
#define QSPI_BASE  (0xE000d000uL)
#define SMCC_BASE  (0xE000e000uL) // PL353 shared memory controller

#define SD0_BASE   (0xE0100000uL)
#define SD1_BASE   (0xE0101000uL)

#define SLCR_BASE  		(0xF8000000uL)
#define TTC0_BASE  		(0xF8001000uL)
#define TTC1_BASE  		(0xF8002000uL)
#define DMAC0_NS_BASE 	(0xF8004000uL)
#define DMAC0_S_BASE 	(0xF8003000uL)
#define SWDT_BASE  		(0xF8005000uL)

#define QSPI_LINEAR_BASE  (0xfc000000uL)


#define GTC 						((GTC_Registers *) GLOBAL_TIMER_BASE)
#define SCLR 						((SLCR_Registers *) SLCR_BASE)
#define SWDT                       	((SWDT_Registers *) SWDT_BASE)
#define UART0                       ((UART_Registers *) UART0_BASE)
#define UART1                       ((UART_Registers *) UART1_BASE)
#define SPI0                       	((SPI_Registers *) SPI0_BASE)
#define SPI1                       	((SPI_Registers *) SPI1_BASE)
#define SD0							((SD_Registers *) SD0_BASE)
#define SD1							((SD_Registers *) SD1_BASE)

#ifdef __cplusplus
}
#endif


#endif /* ARMCPU_ZYNQ7000_H_ */
