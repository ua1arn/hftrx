/*
 * zynquscale.h
 *
 */

#ifndef ARMCPU_ZYNQUSCALE_H_
#define ARMCPU_ZYNQUSCALE_H_

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

//#define CPUPRIV_BASE      (0xF8F00000uL)
//#define SCU_CONTROL_BASE  (CPUPRIV_BASE + 0x0000uL)
//#define GLOBAL_TIMER_BASE (CPUPRIV_BASE + 0x0200uL)
//#define PRIV_TIMER_BASE   (CPUPRIV_BASE + 0x0600uL)
//#define L2CACHE_BASE      (CPUPRIV_BASE + 0x2000uL)
//#define GLOBAL_TIMER_BASE 0

/* Verify the entries match the TRM offset to validate the struct */
//STATIC_ASSERT(offsetof(XEMACPS_Registers, TIMER_STROBE_S) == 0x01C8);
//STATIC_ASSERT(offsetof(XEMACPS_Registers, DESIGN_CFG5) == 0x0290);

/* configuration for the PL310 L2 cache controller */
#define PL310_BASE L2CACHE_BASE
#define PL310_TAG_RAM_LATENCY ((1uL << 8) | (1uL << 4) | (1uL << 0))
#define PL310_DATA_RAM_LATENCY ((1uL << 8) | (2uL << 4) | (1uL << 0))

/* Verify the entries match the TRM offset to validate the struct */
//STATIC_ASSERT(offsetof(struct slcr_regs, SCL) == 0x0);
//STATIC_ASSERT(offsetof(struct slcr_regs, DDRIOB_DCI_STATUS) == 0xb74);

#define DDRC_CTRL                       0xF8006000L
#define DDRC_MODE_STATUS                0xF8006054L

/* memory addresses */
/* assumes sram is mapped at 0 the first MB of sdram is covered by it */
#define SDRAM_BASE          (0x00100000)
#define SDRAM_APERTURE_SIZE (0x3fF00000)
#define SRAM_BASE           (0x0)
#define SRAM_BASE_HIGH      (0xfffc0000)
#define SRAM_APERTURE_SIZE  (0x00040000)
#define SRAM_SIZE           (0x00040000)

/* hardware base addresses */
#define UART0_BASE 	(0xFF000000uL)
#define UART1_BASE 	(0xFF010000uL)
#define USB0_BASE  	(0xFF9D0000uL)
#define USB1_BASE  	(0xFF9E0000uL)
#define I2C0_BASE  	(0xFF020000uL)
#define I2C1_BASE  	(0xFF030000uL)
#define SPI0_BASE  	(0xFF040000uL)
#define SPI1_BASE  	(0xFF050000uL)
#define CAN0_BASE  	(0xFF060000uL)
#define CAN1_BASE  	(0xFF070000uL)
#define GPIO_BASE  	(0xFF0A0000uL)
#define GEM0_BASE  	(0xFF0B0000uL) // gigabit eth controller
#define GEM1_BASE  	(0xFF0C0000uL) // ""
#define GEM2_BASE  	(0xFF0D0000uL) // ""
#define GEM3_BASE  	(0xFF0E0000uL) // ""
#define QSPI_BASE  	(0xFF0F0000uL)
#define SD0_BASE   	(0xFF160000uL)
#define SD1_BASE   	(0xFF170000uL)
#define RTC_BASE  	(0xFFA60000uL)
#define XPPU_BASE  	(0xFF980000uL)
#define XPPUsink_BASE  	(0xFF980000uL)
#define CSU_BASE  	(0xFFCA0000uL)
#define CSU_ROM_BASE  	(0xFFC40000uL)
#define CSU_RAM_BASE  	(0xFFC00000uL)
#define PMU_ROM_BASE  	(0xFFD00000uL)
#define PMU_RAM_BASE  	(0xFFDC0000uL)
//
//#define SLCR_BASE  		(0xF8000000uL)
//#define TTC0_BASE  		(0xF8001000uL)
//#define TTC1_BASE  		(0xF8002000uL)
//#define DMAC0_NS_BASE 	(0xF8004000uL)
//#define DMAC0_S_BASE 	(0xF8003000uL)
//#define SWDT_BASE  		(0xF8005000uL)
//#define XDCFG_BASE  	(0xF8007000uL)	// Device configuraion Interface

#define QSPI_LINEAR_BASE  (0xfC000000uL)


#define GTC 			((GTC_Registers *) GLOBAL_TIMER_BASE)
#define SCLR 			((SLCR_Registers *) SLCR_BASE)
#define SWDT            ((SWDT_Registers *) SWDT_BASE)
#define UART0           ((XUARTPS_Registers *) UART0_BASE)
#define UART1           ((XUARTPS_Registers *) UART1_BASE)
#define SPI0            ((SPI_Registers *) SPI0_BASE)
#define SPI1            ((SPI_Registers *) SPI1_BASE)
#define SD0				((SD_Registers *) SD0_BASE)
#define SD1				((SD_Registers *) SD1_BASE)
#define XDCFG			((XDCFG_Registers *) XDCFG_BASE)
#define XQSPIPS			((XQSPIPS_Registers *) QSPI_BASE)
#define GEM0			((XEMACPS_Registers *) GEM0_BASE)
#define GEM1			((XEMACPS_Registers *) GEM1_BASE)

#define GIC_DISTRIBUTOR_BASE         0xF9000000                        /*!< (GIC DIST  ) Base Address */
#define GIC_INTERFACE_BASE           0xF9002000                        /*!< (GIC CPU IF) Base Address */
//#define L2C_310_BASE                 L2CACHE_BASE                        /*!< (PL310     ) Base Address */
//#define TIMER_BASE				PRIV_TIMER_BASE

/* --------  Configuration of the Cortex-A9 Processor and Core Peripherals  ------- */
#define __CA_REV         		    0x0000    /*!< Core revision r0p0       */

#define __CORTEX_A                    53U      /*!< Cortex-A# Core                              */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

//#include "core_ca.h"

#include <stdint.h>
#define __IO volatile

#include "system_zynqultrascale.h"

// See
// https://github.com/mtk-watch/android_trusty/blob/320a422109b955a03443ae92b011ade929a3f812/external/lk/platform/zynq/swdt.c
// https://github.com/mtk-watch/android_trusty/blob/320a422109b955a03443ae92b011ade929a3f812/external/lk/platform/zynq/include/platform/zynq.h

/** @name Register Map
 *
 * UART Controller (UART)
 * Registers of the UART.
 * @{
 */
typedef struct xuartps_regs {
    __IO uint32_t CR;            /**< Control Register */
    __IO uint32_t MR;            /**< Mode Register */
    __IO uint32_t IER;           /**< Interrupt Enable */
    __IO uint32_t IDR;           /**< Interrupt Disable */
    __IO uint32_t IMR;           /**< Interrupt Mask */
    __IO uint32_t ISR;           /**< Interrupt Status */
    __IO uint32_t BAUDGEN;       /**< Baud Rate Generator "CD" */
    __IO uint32_t RXTOUT;        /**< RX Timeout */
    __IO uint32_t RXWM;          /**< RX FIFO Trigger Level */
    __IO uint32_t MODEMCR;       /**< Modem Control */
    __IO uint32_t MODEMSR;       /**< Modem Status */
    __IO uint32_t SR;            /**< Channel Status */
    __IO uint32_t FIFO;          /**< FIFO */
    __IO uint32_t BAUDDIV;       /**< Baud Rate Divider "DIV" */
    __IO uint32_t FLOWDEL;       /**< Flow Delay */
    uint32_t RESERVED1;
    uint32_t RESERVED2;
    __IO uint32_t TXWM;            /* TX FIFO Trigger Level */
} XUARTPS_Registers;

#define XUARTPS_CR_TXRST	0x00000002uL  /**< TX logic reset */
#define XUARTPS_CR_RXRST	0x00000001uL  /**< RX logic reset */

#define XUARTPS_MR_CHMODE_NORM		0x00000000uL /**< Normal mode */
#define XUARTPS_MR_STOPMODE_1_BIT	0x00000000uL /**< 1 stop bit */
#define XUARTPS_MR_PARITY_NONE		0x00000020uL /**< No parity mode */
#define XUARTPS_MR_CHARLEN_8_BIT	0x00000000uL /**< 8 bits data */
#define XUARTPS_MR_CLKSEL			0x00000001uL /**< Input clock selection */

#define XUARTPS_SR_TNFUL	0x00004000uL /**< TX FIFO Nearly Full Status */
#define XUARTPS_SR_TACTIVE	0x00000800uL /**< TX active */
#define XUARTPS_SR_RXEMPTY	0x00000002uL /**< RX FIFO empty */

#define XUARTPS_CR_TX_DIS	0x00000020uL  /**< TX disabled. */
#define XUARTPS_CR_TX_EN	0x00000010uL  /**< TX enabled */
#define XUARTPS_CR_RX_DIS	0x00000008uL  /**< RX disabled. */
#define XUARTPS_CR_RX_EN	0x00000004uL  /**< RX enabled */

/* system watchdog timer */
typedef struct swdt_regs {
	__IO uint32_t MODE;
	__IO uint32_t CONTROL;
	__IO uint32_t RESTART;
	__IO uint32_t STATUS;
} SWDT_Registers;


/* SLCR registers */
typedef struct slcr_regs {
	__IO uint32_t SCL;                             // Secure Configuration Lock
	__IO uint32_t SLCR_LOCK;                       // SLCR Write Protection Lock
	__IO uint32_t SLCR_UNLOCK;                     // SLCR Write Protection Unlock
	__IO uint32_t SLCR_LOCKSTA;                    // SLCR Write Protection Status
    uint32_t reserved0[60];
    __IO uint32_t ARM_PLL_CTRL;                    // ARM PLL Control
    __IO uint32_t DDR_PLL_CTRL;                    // DDR PLL Control
    __IO  uint32_t IO_PLL_CTRL;                     // IO PLL Control
    __IO uint32_t PLL_STATUS;                      // PLL Status
    __IO uint32_t ARM_PLL_CFG;                     // ARM PLL Configuration
    __IO uint32_t DDR_PLL_CFG;                     // DDR PLL Configuration
    __IO uint32_t IO_PLL_CFG;                      // IO PLL Configuration
    uint32_t reserved1[1];
    __IO uint32_t ARM_CLK_CTRL;                    // CPU Clock Control
    __IO uint32_t DDR_CLK_CTRL;                    // DDR Clock Control
    __IO uint32_t DCI_CLK_CTRL;                    // DCI clock control
    __IO uint32_t APER_CLK_CTRL;                   // AMBA Peripheral Clock Control
    __IO uint32_t USB0_CLK_CTRL;                   // USB 0 ULPI Clock Control
    __IO uint32_t USB1_CLK_CTRL;                   // USB 1 ULPI Clock Control
    __IO uint32_t GEM0_RCLK_CTRL;                  // GigE 0 Rx Clock and Rx Signals Select
    __IO uint32_t GEM1_RCLK_CTRL;                  // GigE 1 Rx Clock and Rx Signals Select
    __IO uint32_t GEM0_CLK_CTRL;                   // GigE 0 Ref Clock Control
    __IO uint32_t GEM1_CLK_CTRL;                   // GigE 1 Ref Clock Control
    __IO uint32_t SMC_CLK_CTRL;                    // SMC Ref Clock Control
    __IO uint32_t LQSPI_CLK_CTRL;                  // Quad SPI Ref Clock Control
    __IO uint32_t SDIO_CLK_CTRL;                   // SDIO Ref Clock Control
    __IO uint32_t UART_CLK_CTRL;                   // UART Ref Clock Control
    __IO uint32_t SPI_CLK_CTRL;                    // SPI Ref Clock Control
    __IO uint32_t CAN_CLK_CTRL;                    // CAN Ref Clock Control
    __IO uint32_t CAN_MIOCLK_CTRL;                 // CAN MIO Clock Control
    __IO uint32_t DBG_CLK_CTRL;                    // SoC Debug Clock Control
    __IO uint32_t PCAP_CLK_CTRL;                   // PCAP Clock Control
    __IO uint32_t TOPSW_CLK_CTRL;                  // Central Interconnect Clock Control
    __IO uint32_t FPGA0_CLK_CTRL;                  // PL Clock 0 Output control
    __IO uint32_t FPGA0_THR_CTRL;                  // PL Clock 0 Throttle control
    __IO uint32_t FPGA0_THR_CNT;                   // PL Clock 0 Throttle Count control
    __IO uint32_t FPGA0_THR_STA;                   // PL Clock 0 Throttle Status read
    __IO uint32_t FPGA1_CLK_CTRL;                  // PL Clock 1 Output control
    __IO uint32_t FPGA1_THR_CTRL;                  // PL Clock 1 Throttle control
    __IO uint32_t FPGA1_THR_CNT;                   // PL Clock 1 Throttle Count
    __IO uint32_t FPGA1_THR_STA;                   // PL Clock 1 Throttle Status control
    __IO uint32_t FPGA2_CLK_CTRL;                  // PL Clock 2 output control
    __IO uint32_t FPGA2_THR_CTRL;                  // PL Clock 2 Throttle Control
    __IO uint32_t FPGA2_THR_CNT;                   // PL Clock 2 Throttle Count
    __IO uint32_t FPGA2_THR_STA;                   // PL Clock 2 Throttle Status
    __IO uint32_t FPGA3_CLK_CTRL;                  // PL Clock 3 output control
    __IO uint32_t FPGA3_THR_CTRL;                  // PL Clock 3 Throttle Control
    __IO uint32_t FPGA3_THR_CNT;                   // PL Clock 3 Throttle Count
    __IO uint32_t FPGA3_THR_STA;                   // PL Clock 3 Throttle Status
    uint32_t reserved2[5];
    __IO uint32_t CLK_621_TRUE;                    // CPU Clock Ratio Mode select
    uint32_t reserved3[14];
    __IO uint32_t PSS_RST_CTRL;                    // PS Software Reset Control
    __IO uint32_t DDR_RST_CTRL;                    // DDR Software Reset Control
    __IO uint32_t TOPSW_RST_CTRL;                  // Central Interconnect Reset Control
    __IO uint32_t DMAC_RST_CTRL;                   // DMAC Software Reset Control
    __IO uint32_t USB_RST_CTRL;                    // USB Software Reset Control
    __IO uint32_t GEM_RST_CTRL;                    // Gigabit Ethernet SW Reset Control
    __IO uint32_t SDIO_RST_CTRL;                   // SDIO Software Reset Control
    __IO uint32_t SPI_RST_CTRL;                    // SPI Software Reset Control
    __IO uint32_t CAN_RST_CTRL;                    // CAN Software Reset Control
    __IO uint32_t I2C_RST_CTRL;                    // I2C Software Reset Control
    __IO uint32_t UART_RST_CTRL;                   // UART Software Reset Control
    __IO uint32_t GPIO_RST_CTRL;                   // GPIO Software Reset Control
    __IO uint32_t LQSPI_RST_CTRL;                  // Quad SPI Software Reset Control
    __IO uint32_t SMC_RST_CTRL;                    // SMC Software Reset Control
    __IO uint32_t OCM_RST_CTRL;                    // OCM Software Reset Control
    uint32_t reserved4[1];
    __IO uint32_t FPGA_RST_CTRL;                   // FPGA Software Reset Control
    __IO uint32_t A9_CPU_RST_CTRL;                 // CPU Reset and Clock control
    uint32_t reserved5[1];
    __IO uint32_t RS_AWDT_CTRL;                    // Watchdog Timer Reset Control
    uint32_t reserved6[2];
    __IO uint32_t REBOOT_STATUS;                   // Reboot Status, persistent
    __IO uint32_t BOOT_MODE;                       // Boot Mode Strapping Pins
    uint32_t reserved7[40];
    __IO uint32_t APU_CTRL;                        // APU Control
    __IO uint32_t WDT_CLK_SEL;                     // SWDT clock source select
    uint32_t reserved8[78];
    __IO uint32_t TZ_DMA_NS;                       // DMAC TrustZone Config
    __IO uint32_t TZ_DMA_IRQ_NS;                   // DMAC TrustZone Config for Interrupts
    __IO uint32_t TZ_DMA_PERIPH_NS;                // DMAC TrustZone Config for Peripherals
    uint32_t reserved9[57];
    __IO uint32_t PSS_IDCODE;                      // PS IDCODE
    uint32_t reserved10[51];
    __IO uint32_t DDR_URGENT;                      // DDR Urgent Control
    uint32_t reserved11[2];
    __IO uint32_t DDR_CAL_START;                   // DDR Calibration Start Triggers
    uint32_t reserved12[1];
    __IO uint32_t DDR_REF_START;                   // DDR Refresh Start Triggers
    __IO uint32_t DDR_CMD_STA;                     // DDR Command Store Status
    __IO uint32_t DDR_URGENT_SEL;                  // DDR Urgent Select
    __IO uint32_t DDR_DFI_STATUS;                  // DDR DFI status
    uint32_t reserved13[55];
    __IO uint32_t MIO_PIN [54];                      // MIO Pin 0 Control
    uint32_t reserved14[11];
    __IO uint32_t MIO_LOOPBACK;                    // Loopback function within MIO
    uint32_t reserved15[1];
    __IO uint32_t MIO_MST_TRI0;                    // MIO pin Tri-state Enables, 31:0
    __IO uint32_t MIO_MST_TRI1;                    // MIO pin Tri-state Enables, 53:32
    uint32_t reserved16[7];
    __IO uint32_t SD0_WP_CD_SEL;                   // SDIO 0 WP CD select
    __IO uint32_t SD1_WP_CD_SEL;                   // SDIO 1 WP CD select
    uint32_t reserved17[50];
    __IO uint32_t LVL_SHFTR_EN;                    // Level Shifters Enable
    uint32_t reserved18[3];
    __IO uint32_t OCM_CFG;                         // OCM Address Mapping
    uint32_t reserved19[66];
    __IO uint32_t RESERVED;                        // Reserved
    uint32_t reserved20[56];
    __IO uint32_t GPIOB_CTRL;                      // PS IO Buffer Control
    __IO uint32_t GPIOB_CFG_CMOS18;                // MIO GPIOB CMOS 1.8V config
    __IO uint32_t GPIOB_CFG_CMOS25;                // MIO GPIOB CMOS 2.5V config
    __IO uint32_t GPIOB_CFG_CMOS33;                // MIO GPIOB CMOS 3.3V config
    uint32_t reserved21[1];
    __IO uint32_t GPIOB_CFG_HSTL;                  // MIO GPIOB HSTL config
    __IO uint32_t GPIOB_DRVR_BIAS_CTRL;            // MIO GPIOB Driver Bias Control
    uint32_t reserved22[9];
    __IO uint32_t DDRIOB_ADDR0;                    // DDR IOB Config for A[14:0], CKE and DRST_B
    __IO uint32_t DDRIOB_ADDR1;                    // DDR IOB Config for BA[2:0], ODT, CS_B, WE_B, RAS_B and CAS_B
    __IO uint32_t DDRIOB_DATA0;                    // DDR IOB Config for Data 15:0
    __IO uint32_t DDRIOB_DATA1;                    // DDR IOB Config for Data 31:16
    __IO uint32_t DDRIOB_DIFF0;                    // DDR IOB Config for DQS 1:0
    __IO uint32_t DDRIOB_DIFF1;                    // DDR IOB Config for DQS 3:2
    __IO uint32_t DDRIOB_CLOCK;                    // DDR IOB Config for Clock Output
    __IO uint32_t DDRIOB_DRIVE_SLEW_ADDR;          // Drive and Slew controls for Address and Command pins of the DDR Interface
    __IO uint32_t DDRIOB_DRIVE_SLEW_DATA;          // Drive and Slew controls for DQ pins of the DDR Interface
    __IO uint32_t DDRIOB_DRIVE_SLEW_DIFF;          // Drive and Slew controls for DQS pins of the DDR Interface
    __IO uint32_t DDRIOB_DRIVE_SLEW_CLOCK;         // Drive and Slew controls for Clock pins of the DDR Interface
    __IO uint32_t DDRIOB_DDR_CTRL;                 // DDR IOB Buffer Control
    __IO uint32_t DDRIOB_DCI_CTRL;                 // DDR IOB DCI Config
    __IO uint32_t DDRIOB_DCI_STATUS;               // DDR IO Buffer DCI Status
} SLCR_Registers;

#define ZYNQ_MIO_CNT    54


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

#define SLCR_A9_CPU_CLKSTOP	0x10
#define SLCR_A9_CPU_RST		0x01

#define ZYNQ_SLCR_BOOT_MODE_ARM_PLL_BYPASS 		0x00000010
#define ZYNQ_SLCR_BOOT_MODE_IO_PLL_BYPASS		0x00000010
#define ZYNQ_SLCR_BOOT_MODE_BOOT_MODE 			0x0000000F

/* SPI */
typedef struct spi_regs {
	__IO uint32_t CR; 		/*	SPI Configuration.	 */
	__IO uint32_t SR; 		/*	SPI Interrupt Status	 */
	__IO uint32_t IER; 		/*	Interrupt Enable. 	 */
	__IO uint32_t IDR; 		/*	Interrupt disable. 	 */
	__IO uint32_t IMR; 		/*	Interrupt mask. 	 */
	__IO uint32_t ER; 		/*	SPI Controller Enable.	 */
	__IO uint32_t DR; 		/*	Delay Control 	 */
	__IO uint32_t TXD; 		/*	Transmit Data. 	 */
	__IO uint32_t RXD; 		/*	Receive Data. 	 */
	__IO uint32_t SICR; 	/*	Slave Idle Count.	 */
	__IO uint32_t TXWR; 	/*	TX_FIFO Threshold. 	 */
	__IO uint32_t RXWR;		/* 0x0000002C	RX FIFO Threshold.	 */
	uint32_t reserved1 [(0xfc - 0x30) / 4];
	__IO uint32_t Mod_id_reg0;	/* 0x000000FC	Module ID. 	 */
} SPI_Registers;

// Global Timer Registers offsets
typedef struct gtc_regs {
	__IO uint32_t GTCTRL; 	// Global_Timer_Counter_Register0
	__IO uint32_t GTCTRH; 	// Global_Timer_Counter_Register1
	__IO uint32_t GTCLR; 	// Control
	__IO uint32_t GTISR; 	// Interrupt Status
	__IO uint32_t GTCOMPL;	// Comparator_Value_Register0
	__IO uint32_t GTCOMPH; 	// Comparator_Value_Register1
	__IO uint32_t GTCAIR; 	// Auto_increment_Register
} GTC_Registers;

// https://github.com/yuhuidream/crane/blob/bc4d2b830839c0678ee5ae249378d5cf606dfa9e/arch/asr/include/SDH_v3_1.h
// SD Controller (sdio)
typedef struct sd_regs {
    __IO uint32_t SYS_DMA_ADDR                   ; /* 0x0000 System DMA Address Register */
    __IO uint32_t Block_Size_Block_Count         ; /* 0x0004 Block Size Register */
    __IO uint32_t ARG                     		 ; /* 0x0008 Argument Register */
    __IO uint32_t CMD_TRANSFER_MODE              ; /* 0x000C Command Register, Transfer Mode Register */
    __IO uint32_t RESP_0                         ; /* 0x0010 Response Register 0 */
    __IO uint32_t RESP_1                         ; /* 0x0014 Response Register 1 */
    __IO uint32_t RESP_2                         ; /* 0x0018 Response Register 2 */
    __IO uint32_t RESP_3                         ; /* 0x001C Response Register 3 */
    __IO uint32_t BUFFER_DATA_PORT               ; /* 0x0020 Buffer Data Port Register */
    __IO uint32_t PRESENT_STATE                  ; /* 0x0024 Present State Register 1 */
    __IO uint32_t HOST_CTRL_BLOCK_GAP_CTRL       ; /* 0x0028 Host Control Register */
    __IO uint32_t TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL; /* 0x002C Timeout Control/Software Reset Register, Clock Control Register */
    __IO uint32_t INT_STATUS            		 ; /* 0x0030 Interrupt Status Register */
    __IO uint32_t INT_STATUS_EN           		 ; /* 0x0034 Interrupt Status Enable Register */
    __IO uint32_t INT_STATUS_INT_EN       		 ; /* 0x0038 Interrupt Status Interrupt Enable Register */
    __IO uint8_t AUTO_CMD12_ERROR_STATUS        ; /* 0x003C Auto CMD12 Error Status Register */
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
    uint8_t reserved5 [0x00FC - 0x00F1];
    __IO uint32_t Vendor_Version_Number          ; /* 0x00FC Shared Bus Control Register */
} SD_Registers;

// See also
// https://github.com/jameswalmsley/bitthunder/blob/master/arch/arm/mach/zynq/devcfg.c
// Device Configuration Interface (devcfg)
typedef struct devcfg_regs {
	__IO uint32_t CTRL;				/* 0x0000 Control Register */
	__IO uint32_t LOCK;				/* 0x0004 Locks for the Control Register */
	__IO uint32_t CFG;				/* 0x0008 Configuration Register */
	__IO uint32_t INT_STS;			/* 0x000C Interrupt Status */
	__IO uint32_t INT_MASK;			/* 0x0010 Interrupt Mask */
	__IO uint32_t STATUS;			/* 0x0014 Miscellaneous Status */
	__IO uint32_t DMA_SRC_ADDR;		/* 0x0018 DMA Source Address */
	__IO uint32_t DMA_DST_ADDR;		/* 0x001C DMA Destination Address */
	__IO uint32_t DMA_SRC_LEN;		/* 0x0020 DMA Source Transfer Length */
	__IO uint32_t DMA_DST_LEN;		/* 0x0024 DMA Destination Transfer Length */
	uint32_t reserved1;
	__IO uint32_t MULTIBOOT_ADDR;	/* 0x002C Multi-Boot Address Pointer */
	uint32_t reserved2;
	__IO uint32_t UNLOCK;			/* 0x0034 Unlock Control */
	uint8_t reserved3 [0x0080 - 0x0038];
	__IO uint32_t MCTRL;			/* 0x0080 Miscellaneous Control */
	uint8_t reserved4 [0x0100 - 0x0084];
	__IO uint32_t XADCIF_CFG;		/* 0x0100 XADC Interface Configuration */
	__IO uint32_t XADCIF_INT_STS;	/* 0x0114 XADC Interface Interrupt Status */
	__IO uint32_t XADCIF_INT_MASK;	/* 0x0118 XADC Interface Interrupt Mask */
	__IO uint32_t XADCIF_MSTS;		/* 0x010C XADC Interface Miscellaneous Status */
	__IO uint32_t XADCIF_CMDFIFO;	/* 0x0110 XADC Interface Command FIFO Data Port */
	__IO uint32_t XADCIF_RDFIFO;	/* 0x0114 XADC Interface Data FIFO Data Port */
	__IO uint32_t XADCIF_MCTL;		/* 0x0118 XADC Interface Miscellaneous Control */
} XDCFG_Registers;

#define XDCFG_CTRL_DAP_EN				0x00000007
#define XDCFG_CTRL_DBGEN				0x00000008
#define XDCFG_CTRL_NIDEN				0x00000010
#define XDCFG_CTRL_SPIDEN				0x00000020
#define XDCFG_CTRL_SPNIDEN				0x00000040
#define XDCFG_CTRL_SEC_EN				0x00000080
#define XDCFG_CTRL_SEU_EN				0x00000100
#define XDCFG_CTRL_PCFG_AES_EN			0x00000E00
#define XDCFG_CTRL_PCFG_AES_FUSE 		0x00001000
#define XDCFG_CTRL_USER_MODE 			0x00008000
#define XDCFG_CTRL_JTAG_CHAIN_DIS 		0x00800000
#define XDCFG_CTRL_MULTIBOOT_EN 		0x01000000
#define XDCFG_CTRL_QUARTER_PCAP_RATE 	0x02000000
#define XDCFG_CTRL_PCAP_MODE 			0x04000000
#define XDCFG_CTRL_PCAP_PR 				0x08000000
#define XDCFG_CTRL_PCFG_POR_CNT_4K		0x20000000
#define XDCFG_CTRL_PCFG_PROG_B 			0x40000000
#define XDCFG_CTRL_FORCE_RESET 			0x80000000
#define XDCFG_INT_STS_DMA_DONE_INT  	0x00002000
#define XDCFG_INT_STS_DMA_PCAP_DONE		0x00001000
#define XDCFG_INT_STS_PCFG_DONE			0x00000004
#define XDCFG_STATUS_PCFG_INIT			0x00000010
#define XDCFG_MCTRL_PCAP_LPBK			0x00000010

// Quad-SPI Flash Controller (qspi)
typedef struct qspi_regs {
	__IO uint32_t CR;				/* 0x0000 QSPI configuration register */
	__IO uint32_t SR;				/* 0x0004 QSPI interrupt status register */
	__IO uint32_t IER;				/* 0x0008 Interrupt Enable register */
	__IO uint32_t IDR;				/* 0x000C Interrupt disable register */
	__IO uint32_t IMR;				/* 0x0010 Interrupt mask register */
} XQSPIPS_Registers;

// See https://github.com/grub4android/lk/blob/579832fe57eeb616cefd82b93d991141f0db91ce/platform/zynq/include/platform/gem.h
// Gigabit Ethernet Controller (GEM)
typedef struct xemacps_regs {
	__IO uint32_t NET_CTRL;
	__IO uint32_t NET_CFG;
	__IO uint32_t NET_STATUS;
	uint32_t ___RESERVED1;
	__IO uint32_t DMA_CFG;
	__IO uint32_t TX_STATUS;
	__IO uint32_t RX_QBAR;
	__IO uint32_t TX_QBAR;
	__IO uint32_t RX_STATUS;
	__IO uint32_t INTR_STATUS;
	__IO uint32_t INTR_EN;
	__IO uint32_t INTR_DIS;
	__IO uint32_t INTR_MASK;
	__IO uint32_t PHY_MAINT;
	__IO uint32_t RX_PAUSEQ;
	__IO uint32_t TX_PAUSEQ;
	uint32_t ___RESERVED2[16];
	__IO uint32_t HASH_BOT;
	__IO uint32_t HASH_TOP;
	__IO uint32_t SPEC_ADDR1_BOT;
	__IO uint32_t SPEC_ADDR1_TOP;
	__IO uint32_t SPEC_ADDR2_BOT;
	__IO uint32_t SPEC_ADDR2_TOP;
	__IO uint32_t SPEC_ADDR3_BOT;
	__IO uint32_t SPEC_ADDR3_TOP;
	__IO uint32_t SPEC_ADDR4_BOT;
	__IO uint32_t SPEC_ADDR4_TOP;
	__IO uint32_t TYPE_ID_MATCH1;
	__IO uint32_t TYPE_ID_MATCH2;
	__IO uint32_t TYPE_ID_MATCH3;
	__IO uint32_t TYPE_ID_MATCH4;
	__IO uint32_t WAKE_ON_LAN;
	__IO uint32_t IPG_STRETCH;
	__IO uint32_t STACKED_VLAN;
	__IO uint32_t TX_PFC_PAUSE;
	__IO uint32_t SPEC_ADDR1_MASK_BOT;
	__IO uint32_t SPEC_ADDR1_MASK_TOP;
	uint32_t ___RESERVED3[11];
	__IO uint32_t MODULE_ID;					/**< 0x00FC	xxx */
	__IO uint32_t OCTETS_TX_BOT;
	__IO uint32_t OCTETS_TX_TOP;
	__IO uint32_t FRAMES_TX;
	__IO uint32_t BROADCAST_FRAMES_TX;
	__IO uint32_t MULTI_FRAMES_TX;
	__IO uint32_t PAUSE_FRAMES_TX;
	__IO uint32_t FRAMES_64B_TX;				/**< 0x0118	xxx */
	__IO uint32_t FRAMES_65TO127B_TX;
	__IO uint32_t FRAMES_128TO255B_TX;
	__IO uint32_t FRAMES_256TO511B_TX;
	__IO uint32_t FRAMES_512TO1023B_TX;
	__IO uint32_t FRAMES_1024TO1518B_TX;		/**< 0x012C	xxx */
	uint32_t ___RESERVED4;
	__IO uint32_t TX_UNDER_RUNS;
	__IO uint32_t SINGLE_COLLISN_FRAMES;
	__IO uint32_t MULTI_COLLISN_FRAMES;
	__IO uint32_t EXCESSIVE_COLLISNS;
	__IO uint32_t LATE_COLLISNS;
	__IO uint32_t DEFERRED_TX_FRAMES;
	__IO uint32_t CARRIER_SENSE_ERRS;
	__IO uint32_t OCTETS_RX_BOT;
	__IO uint32_t OCTETS_RX_TOP;
	__IO uint32_t FRAMES_RX;
	__IO uint32_t BDCAST_FAMES_RX;
	__IO uint32_t MULTI_FRAMES_RX;
	__IO uint32_t PAUSE_RX;
	__IO uint32_t FRAMES_64B_RX;			/**< 0x0168	xxx */
	__IO uint32_t FRAMES_65TO127B_RX;
	__IO uint32_t FRAMES_128TO255B_RX;
	__IO uint32_t FRAMES_256TO511B_RX;
	__IO uint32_t FRAMES_512TO1023B_RX;
	__IO uint32_t FRAMES_1024TO1518B_RX;	/**< 0x017C	xxx */
	uint32_t ___RESERVED5;
	__IO uint32_t UNDERSZ_RX;				/**< 0x0184	xxx */
	__IO uint32_t OVERSZ_RX;
	__IO uint32_t JAB_RX;					/**< 0x018C	Jabbers received */
	__IO uint32_t FCS_ERRORS;				/**< 0x0190	xxx */
	__IO uint32_t LENGTH_FIELD_ERRORS;		/**< 0x0194	xxx */
	__IO uint32_t RX_SYMBOL_ERRORS;			/**< 0x0198	xxx */
	__IO uint32_t ALIGN_ERRORS;				/**< 0x019C	xxx */
	__IO uint32_t RX_RESOURCE_ERRORS;		/**< 0x01A0	xxx */
	__IO uint32_t RX_OVERRUN_ERRORS;		/**< 0x01A4	xxx */
	__IO uint32_t IP_HDR_CSUM_ERRORS;		/**< 0x01A8	xxx */
	__IO uint32_t TCP_CSUM_ERRORS;			/**< 0x01AC	xxx */
	__IO uint32_t UDP_CSUM_ERRORS;			/**< 0x01B0	xxx */
	uint8_t ___RESERVED6 [0x01C8 - 0x01B4];
	__IO uint32_t TIMER_STROBE_S;			/**< 0x01C8	xxx */
	__IO uint32_t TIMER_STROBE_NS;
	__IO uint32_t TIMER_S;
	__IO uint32_t TIMER_NS;
	__IO uint32_t TIMER_ADJUST;
	__IO uint32_t TIMER_INCR;
	__IO uint32_t PTP_TX_S;
	__IO uint32_t PTP_TX_NS;
	__IO uint32_t PTP_RX_S;
	__IO uint32_t PTP_RX_NS;
	__IO uint32_t PTP_PEER_TX_S;
	__IO uint32_t PTP_PEER_TX_NS;
	__IO uint32_t PTP_PEER_RX_S;
	__IO uint32_t PTP_PEER_RX_NS;	/**< 0x01FC	PTP peer event frame received nanoseconds */
	uint32_t ___RESERVED7 [33];		// changed from 22 in original text
	__IO uint32_t DESIGN_CFG2;		/**< 0x0284	Design Configuration 2 */
	__IO uint32_t DESIGN_CFG3;		/**< 0x0288	Design Configuration 3 */
	__IO uint32_t DESIGN_CFG4;		/**< 0x028C	Design Configuration 4 */
	__IO uint32_t DESIGN_CFG5;		/**< 0x0290	Design Configuration 5 */
} XEMACPS_Registers;



/** @addtogroup Exported_types
  * @{
  */
typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0,
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum
{
  ERROR = 0,
  SUCCESS = !ERROR
} ErrorStatus;

/**
  * @}
  */


/** @addtogroup Exported_macros
  * @{
  */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))


/**
  * @}
  */

#if defined (USE_HAL_DRIVER)
 #include "zynquscale_hal.h"
#endif /* USE_HAL_DRIVER */

/**
  \brief   Signed Saturate
  \details Saturates a signed value.
  \param [in]  value  Value to be saturated
  \param [in]    sat  Bit position to saturate to (1..32)
  \return             Saturated value
 */
__attribute__((always_inline)) static inline int32_t __SSAT(int32_t val, uint32_t sat)
{
  if ((sat >= 1U) && (sat <= 32U))
  {
    const int32_t max = (int32_t)((1U << (sat - 1U)) - 1U);
    const int32_t min = -1 - max ;
    if (val > max)
    {
      return max;
    }
    else if (val < min)
    {
      return min;
    }
  }
  return val;
}

/**
  \brief   Unsigned Saturate
  \details Saturates an unsigned value.
  \param [in]  value  Value to be saturated
  \param [in]    sat  Bit position to saturate to (0..31)
  \return             Saturated value
 */
__attribute__((always_inline)) static inline uint32_t __USAT(int32_t val, uint32_t sat)
{
  if (sat <= 31U)
  {
    const uint32_t max = ((1U << sat) - 1U);
    if (val > (int32_t)max)
    {
      return max;
    }
    else if (val < 0)
    {
      return 0U;
    }
  }
  return (uint32_t)val;
}

/**
  \brief   Count leading zeros
  \details Counts the number of leading zeros of a data value.
  \param [in]  value  Value to count the leading zeros
  \return             number of leading zeros in value
 */
__attribute__((always_inline)) static inline uint8_t __CLZ(uint32_t value)
{
  /* Even though __builtin_clz produces a CLZ instruction on ARM, formally
     __builtin_clz(0) is undefined behaviour, so handle this case specially.
     This guarantees ARM-compatible results if happening to compile on a non-ARM
     target, and ensures the compiler doesn't decide to activate any
     optimisations using the logic "value was passed to __builtin_clz, so it
     is non-zero".
     ARM Compiler 6.10 and possibly earlier will optimise this test away, leaving a
     single CLZ instruction.
   */
  if (value == 0U)
  {
    return 32U;
  }
  return __builtin_clz(value);
}


/**
  \brief   Rotate Right in unsigned value (32 bit)
  \details Rotate Right (immediate) provides the value of the contents of a register rotated by a variable number of bits.
  \param [in]    op1  Value to rotate
  \param [in]    op2  Number of Bits to rotate
  \return               Rotated value
 */
__attribute__((always_inline)) static inline uint32_t __ROR(uint32_t op1, uint32_t op2)
{
  op2 %= 32U;
  if (op2 == 0U)
  {
    return op1;
  }
  return (op1 >> op2) | (op1 << (32U - op2));
}

#ifdef __cplusplus
}
#endif


#endif /* ARMCPU_ZYNQUSCALE_H_ */
