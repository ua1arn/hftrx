#ifndef XPARAMETERS_H   /* prevent circular inclusions */
#define XPARAMETERS_H   /* by using protection macros */

/* Definition for CPU ID */
#define XPAR_CPU_ID 0U

/* Definitions for peripheral PS7_CORTEXA9_0 */
#define XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ 666666687


/******************************************************************/

/* Canonical definitions for peripheral PS7_CORTEXA9_0 */
#define XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ 666666687


/******************************************************************/

#include "xparameters_ps.h"

#define STDIN_BASEADDRESS 0xE0001000
#define STDOUT_BASEADDRESS 0xE0001000

/******************************************************************/

/* Platform specific definitions */
#define PLATFORM_ZYNQ
 
/* Definitions for sleep timer configuration */
#define XSLEEP_TIMER_IS_DEFAULT_TIMER
 
 
/******************************************************************/
/* Definitions for driver AXIVDMA */
#define XPAR_XAXIVDMA_NUM_INSTANCES 1U

/* Definitions for peripheral VIDEO_AXI_VDMA_0 */
#define XPAR_VIDEO_AXI_VDMA_0_DEVICE_ID 0U
#define XPAR_VIDEO_AXI_VDMA_0_BASEADDR 0x43C00000U
#define XPAR_VIDEO_AXI_VDMA_0_HIGHADDR 0x43C0FFFFU
#define XPAR_VIDEO_AXI_VDMA_0_NUM_FSTORES 3U
#define XPAR_VIDEO_AXI_VDMA_0_INCLUDE_MM2S 1U
#define XPAR_VIDEO_AXI_VDMA_0_INCLUDE_MM2S_DRE 0U
#define XPAR_VIDEO_AXI_VDMA_0_M_AXI_MM2S_DATA_WIDTH 32U
#define XPAR_VIDEO_AXI_VDMA_0_INCLUDE_S2MM 0U
#define XPAR_VIDEO_AXI_VDMA_0_INCLUDE_S2MM_DRE 0U
#define XPAR_VIDEO_AXI_VDMA_0_M_AXI_S2MM_DATA_WIDTH 64U
#define XPAR_VIDEO_AXI_VDMA_0_AXI_MM2S_ACLK_FREQ_HZ 0U
#define XPAR_VIDEO_AXI_VDMA_0_AXI_S2MM_ACLK_FREQ_HZ 0U
#define XPAR_VIDEO_AXI_VDMA_0_MM2S_GENLOCK_MODE 3U
#define XPAR_VIDEO_AXI_VDMA_0_MM2S_GENLOCK_NUM_MASTERS 1U
#define XPAR_VIDEO_AXI_VDMA_0_S2MM_GENLOCK_MODE 0U
#define XPAR_VIDEO_AXI_VDMA_0_S2MM_GENLOCK_NUM_MASTERS 1U
#define XPAR_VIDEO_AXI_VDMA_0_INCLUDE_SG 0U
#define XPAR_VIDEO_AXI_VDMA_0_ENABLE_VIDPRMTR_READS 1U
#define XPAR_VIDEO_AXI_VDMA_0_USE_FSYNC 1U
#define XPAR_VIDEO_AXI_VDMA_0_FLUSH_ON_FSYNC 1U
#define XPAR_VIDEO_AXI_VDMA_0_MM2S_LINEBUFFER_DEPTH 2048U
#define XPAR_VIDEO_AXI_VDMA_0_S2MM_LINEBUFFER_DEPTH 512U
#define XPAR_VIDEO_AXI_VDMA_0_INCLUDE_INTERNAL_GENLOCK 1U
#define XPAR_VIDEO_AXI_VDMA_0_S2MM_SOF_ENABLE 1U
#define XPAR_VIDEO_AXI_VDMA_0_M_AXIS_MM2S_TDATA_WIDTH 32U
#define XPAR_VIDEO_AXI_VDMA_0_S_AXIS_S2MM_TDATA_WIDTH 32U
#define XPAR_VIDEO_AXI_VDMA_0_ENABLE_DEBUG_INFO_1 0U
#define XPAR_VIDEO_AXI_VDMA_0_ENABLE_DEBUG_INFO_5 0U
#define XPAR_VIDEO_AXI_VDMA_0_ENABLE_DEBUG_INFO_6 1U
#define XPAR_VIDEO_AXI_VDMA_0_ENABLE_DEBUG_INFO_7 1U
#define XPAR_VIDEO_AXI_VDMA_0_ENABLE_DEBUG_INFO_9 0U
#define XPAR_VIDEO_AXI_VDMA_0_ENABLE_DEBUG_INFO_13 0U
#define XPAR_VIDEO_AXI_VDMA_0_ENABLE_DEBUG_INFO_14 1U
#define XPAR_VIDEO_AXI_VDMA_0_ENABLE_DEBUG_INFO_15 1U
#define XPAR_VIDEO_AXI_VDMA_0_ENABLE_DEBUG_ALL 0U
#define XPAR_VIDEO_AXI_VDMA_0_ADDR_WIDTH 32U
#define XPAR_VIDEO_AXI_VDMA_0_ENABLE_VERT_FLIP 0U


/******************************************************************/

/* Canonical definitions for peripheral VIDEO_AXI_VDMA_0 */
#define XPAR_AXIVDMA_0_DEVICE_ID XPAR_VIDEO_AXI_VDMA_0_DEVICE_ID
#define XPAR_AXIVDMA_0_BASEADDR 0x43C00000U
#define XPAR_AXIVDMA_0_HIGHADDR 0x43C0FFFFU
#define XPAR_AXIVDMA_0_NUM_FSTORES 3U
#define XPAR_AXIVDMA_0_INCLUDE_MM2S 1U
#define XPAR_AXIVDMA_0_INCLUDE_MM2S_DRE 0U
#define XPAR_AXIVDMA_0_M_AXI_MM2S_DATA_WIDTH 32U
#define XPAR_AXIVDMA_0_INCLUDE_S2MM 0U
#define XPAR_AXIVDMA_0_INCLUDE_S2MM_DRE 0U
#define XPAR_AXIVDMA_0_M_AXI_S2MM_DATA_WIDTH 64U
#define XPAR_AXIVDMA_0_AXI_MM2S_ACLK_FREQ_HZ 0U
#define XPAR_AXIVDMA_0_AXI_S2MM_ACLK_FREQ_HZ 0U
#define XPAR_AXIVDMA_0_MM2S_GENLOCK_MODE 3U
#define XPAR_AXIVDMA_0_MM2S_GENLOCK_NUM_MASTERS 1U
#define XPAR_AXIVDMA_0_S2MM_GENLOCK_MODE 0U
#define XPAR_AXIVDMA_0_S2MM_GENLOCK_NUM_MASTERS 1U
#define XPAR_AXIVDMA_0_INCLUDE_SG 0U
#define XPAR_AXIVDMA_0_ENABLE_VIDPRMTR_READS 1U
#define XPAR_AXIVDMA_0_USE_FSYNC 1U
#define XPAR_AXIVDMA_0_FLUSH_ON_FSYNC 1U
#define XPAR_AXIVDMA_0_MM2S_LINEBUFFER_DEPTH 2048U
#define XPAR_AXIVDMA_0_S2MM_LINEBUFFER_DEPTH 512U
#define XPAR_AXIVDMA_0_INCLUDE_INTERNAL_GENLOCK 1U
#define XPAR_AXIVDMA_0_S2MM_SOF_ENABLE 1U
#define XPAR_AXIVDMA_0_M_AXIS_MM2S_TDATA_WIDTH 32U
#define XPAR_AXIVDMA_0_S_AXIS_S2MM_TDATA_WIDTH 32U
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_1 0U
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_5 0U
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_6 1U
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_7 1U
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_9 0U
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_13 0U
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_14 1U
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_15 1U
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_ALL 0U
#define XPAR_AXIVDMA_0_c_addr_width 32U
#define XPAR_AXIVDMA_0_c_enable_vert_flip 0U


/******************************************************************/


/* Definitions for peripheral PS7_DDR_0 */
#define XPAR_PS7_DDR_0_S_AXI_BASEADDR 0x00100000
#define XPAR_PS7_DDR_0_S_AXI_HIGHADDR 0x1FFFFFFF


/******************************************************************/

/* Definitions for driver DEVCFG */
#define XPAR_XDCFG_NUM_INSTANCES 1U

/* Definitions for peripheral PS7_DEV_CFG_0 */
#define XPAR_PS7_DEV_CFG_0_DEVICE_ID 0U
#define XPAR_PS7_DEV_CFG_0_BASEADDR 0xF8007000U
#define XPAR_PS7_DEV_CFG_0_HIGHADDR 0xF80070FFU


/******************************************************************/

/* Canonical definitions for peripheral PS7_DEV_CFG_0 */
#define XPAR_XDCFG_0_DEVICE_ID XPAR_PS7_DEV_CFG_0_DEVICE_ID
#define XPAR_XDCFG_0_BASEADDR 0xF8007000U
#define XPAR_XDCFG_0_HIGHADDR 0xF80070FFU


/******************************************************************/

/* Definitions for driver DMAPS */
#define XPAR_XDMAPS_NUM_INSTANCES 2

/* Definitions for peripheral PS7_DMA_NS */
#define XPAR_PS7_DMA_NS_DEVICE_ID 0
#define XPAR_PS7_DMA_NS_BASEADDR 0xF8004000
#define XPAR_PS7_DMA_NS_HIGHADDR 0xF8004FFF


/* Definitions for peripheral PS7_DMA_S */
#define XPAR_PS7_DMA_S_DEVICE_ID 1
#define XPAR_PS7_DMA_S_BASEADDR 0xF8003000
#define XPAR_PS7_DMA_S_HIGHADDR 0xF8003FFF


/******************************************************************/

/* Canonical definitions for peripheral PS7_DMA_NS */
#define XPAR_XDMAPS_0_DEVICE_ID XPAR_PS7_DMA_NS_DEVICE_ID
#define XPAR_XDMAPS_0_BASEADDR 0xF8004000
#define XPAR_XDMAPS_0_HIGHADDR 0xF8004FFF

/* Canonical definitions for peripheral PS7_DMA_S */
#define XPAR_XDMAPS_1_DEVICE_ID XPAR_PS7_DMA_S_DEVICE_ID
#define XPAR_XDMAPS_1_BASEADDR 0xF8003000
#define XPAR_XDMAPS_1_HIGHADDR 0xF8003FFF


/******************************************************************/

/* Definitions for driver EMACPS */
#define XPAR_XEMACPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_ETHERNET_0 */
#define XPAR_PS7_ETHERNET_0_DEVICE_ID 0
#define XPAR_PS7_ETHERNET_0_BASEADDR 0xE000B000
#define XPAR_PS7_ETHERNET_0_HIGHADDR 0xE000BFFF
#define XPAR_PS7_ETHERNET_0_ENET_CLK_FREQ_HZ 25000000
#define XPAR_PS7_ETHERNET_0_ENET_SLCR_1000MBPS_DIV0 8
#define XPAR_PS7_ETHERNET_0_ENET_SLCR_1000MBPS_DIV1 1
#define XPAR_PS7_ETHERNET_0_ENET_SLCR_100MBPS_DIV0 8
#define XPAR_PS7_ETHERNET_0_ENET_SLCR_100MBPS_DIV1 5
#define XPAR_PS7_ETHERNET_0_ENET_SLCR_10MBPS_DIV0 8
#define XPAR_PS7_ETHERNET_0_ENET_SLCR_10MBPS_DIV1 50
#define XPAR_PS7_ETHERNET_0_ENET_TSU_CLK_FREQ_HZ 0


/******************************************************************/

#define XPAR_PS7_ETHERNET_0_IS_CACHE_COHERENT 0
#define XPAR_XEMACPS_0_IS_CACHE_COHERENT 0
/* Canonical definitions for peripheral PS7_ETHERNET_0 */
#define XPAR_XEMACPS_0_DEVICE_ID XPAR_PS7_ETHERNET_0_DEVICE_ID
#define XPAR_XEMACPS_0_BASEADDR 0xE000B000
#define XPAR_XEMACPS_0_HIGHADDR 0xE000BFFF
#define XPAR_XEMACPS_0_ENET_CLK_FREQ_HZ 25000000
#define XPAR_XEMACPS_0_ENET_SLCR_1000Mbps_DIV0 8
#define XPAR_XEMACPS_0_ENET_SLCR_1000Mbps_DIV1 1
#define XPAR_XEMACPS_0_ENET_SLCR_100Mbps_DIV0 8
#define XPAR_XEMACPS_0_ENET_SLCR_100Mbps_DIV1 5
#define XPAR_XEMACPS_0_ENET_SLCR_10Mbps_DIV0 8
#define XPAR_XEMACPS_0_ENET_SLCR_10Mbps_DIV1 50
#define XPAR_XEMACPS_0_ENET_TSU_CLK_FREQ_HZ 0


/******************************************************************/


/* Peripheral Definitions for peripheral AUDIO_AXI_I2S_ADI_0 */
#define XPAR_AUDIO_AXI_I2S_ADI_0_BASEADDR 0x43C20000
#define XPAR_AUDIO_AXI_I2S_ADI_0_HIGHADDR 0x43C2FFFF


/* Peripheral Definitions for peripheral AUDIO_FIFO_MIC */
#define XPAR_AUDIO_FIFO_MIC_BASEADDR 0x43C50000
#define XPAR_AUDIO_FIFO_MIC_HIGHADDR 0x43C50FFF


/* Peripheral Definitions for peripheral AUDIO_FIFO_PHONES */
#define XPAR_AUDIO_FIFO_PHONES_BASEADDR 0x43C30000
#define XPAR_AUDIO_FIFO_PHONES_HIGHADDR 0x43C30FFF


/* Peripheral Definitions for peripheral IQ_MODEM_FIFO_IQ_TX */
#define XPAR_IQ_MODEM_FIFO_IQ_TX_BASEADDR 0x43C60000
#define XPAR_IQ_MODEM_FIFO_IQ_TX_HIGHADDR 0x43C60FFF


/* Peripheral Definitions for peripheral IQ_MODEM_FIR_RELOAD_RX */
#define XPAR_IQ_MODEM_FIR_RELOAD_RX_BASEADDR 0x43C80000
#define XPAR_IQ_MODEM_FIR_RELOAD_RX_HIGHADDR 0x43C80FFF


/* Peripheral Definitions for peripheral IQ_MODEM_AXI_DDS_FTW */
#define XPAR_IQ_MODEM_AXI_DDS_FTW_BASEADDR 0x43CB0000
#define XPAR_IQ_MODEM_AXI_DDS_FTW_HIGHADDR 0x43CB0FFF


/* Peripheral Definitions for peripheral IQ_MODEM_AXI_DDS_FTW_SUB */
#define XPAR_IQ_MODEM_AXI_DDS_FTW_SUB_BASEADDR 0x43CC0000
#define XPAR_IQ_MODEM_AXI_DDS_FTW_SUB_HIGHADDR 0x43CC0FFF


/* Peripheral Definitions for peripheral IQ_MODEM_AXI_DDS_RTS */
#define XPAR_IQ_MODEM_AXI_DDS_RTS_BASEADDR 0x43CD0000
#define XPAR_IQ_MODEM_AXI_DDS_RTS_HIGHADDR 0x43CD0FFF


/* Peripheral Definitions for peripheral IQ_MODEM_BLKMEM_CNT */
#define XPAR_IQ_MODEM_BLKMEM_CNT_BASEADDR 0x43C90000
#define XPAR_IQ_MODEM_BLKMEM_CNT_HIGHADDR 0x43C90FFF


/* Peripheral Definitions for peripheral IQ_MODEM_BLKMEM_READER */
#define XPAR_IQ_MODEM_BLKMEM_READER_BASEADDR 0x43CA0000
#define XPAR_IQ_MODEM_BLKMEM_READER_HIGHADDR 0x43CA0FFF


/* Peripheral Definitions for peripheral IQ_MODEM_MODEM_CONTROL */
#define XPAR_IQ_MODEM_MODEM_CONTROL_BASEADDR 0x43C40000
#define XPAR_IQ_MODEM_MODEM_CONTROL_HIGHADDR 0x43C40FFF


/* Peripheral Definitions for peripheral PS7_AFI_0 */
#define XPAR_PS7_AFI_0_S_AXI_BASEADDR 0xF8008000
#define XPAR_PS7_AFI_0_S_AXI_HIGHADDR 0xF8008FFF


/* Peripheral Definitions for peripheral PS7_AFI_1 */
#define XPAR_PS7_AFI_1_S_AXI_BASEADDR 0xF8009000
#define XPAR_PS7_AFI_1_S_AXI_HIGHADDR 0xF8009FFF


/* Peripheral Definitions for peripheral PS7_AFI_2 */
#define XPAR_PS7_AFI_2_S_AXI_BASEADDR 0xF800A000
#define XPAR_PS7_AFI_2_S_AXI_HIGHADDR 0xF800AFFF


/* Peripheral Definitions for peripheral PS7_AFI_3 */
#define XPAR_PS7_AFI_3_S_AXI_BASEADDR 0xF800B000
#define XPAR_PS7_AFI_3_S_AXI_HIGHADDR 0xF800BFFF


/* Peripheral Definitions for peripheral PS7_DDRC_0 */
#define XPAR_PS7_DDRC_0_S_AXI_BASEADDR 0xF8006000
#define XPAR_PS7_DDRC_0_S_AXI_HIGHADDR 0xF8006FFF


/* Peripheral Definitions for peripheral PS7_GLOBALTIMER_0 */
#define XPAR_PS7_GLOBALTIMER_0_S_AXI_BASEADDR 0xF8F00200
#define XPAR_PS7_GLOBALTIMER_0_S_AXI_HIGHADDR 0xF8F002FF


/* Peripheral Definitions for peripheral PS7_GPV_0 */
#define XPAR_PS7_GPV_0_S_AXI_BASEADDR 0xF8900000
#define XPAR_PS7_GPV_0_S_AXI_HIGHADDR 0xF89FFFFF


/* Peripheral Definitions for peripheral PS7_INTC_DIST_0 */
#define XPAR_PS7_INTC_DIST_0_S_AXI_BASEADDR 0xF8F01000
#define XPAR_PS7_INTC_DIST_0_S_AXI_HIGHADDR 0xF8F01FFF


/* Peripheral Definitions for peripheral PS7_IOP_BUS_CONFIG_0 */
#define XPAR_PS7_IOP_BUS_CONFIG_0_S_AXI_BASEADDR 0xE0200000
#define XPAR_PS7_IOP_BUS_CONFIG_0_S_AXI_HIGHADDR 0xE0200FFF


/* Peripheral Definitions for peripheral PS7_L2CACHEC_0 */
#define XPAR_PS7_L2CACHEC_0_S_AXI_BASEADDR 0xF8F02000
#define XPAR_PS7_L2CACHEC_0_S_AXI_HIGHADDR 0xF8F02FFF


/* Peripheral Definitions for peripheral PS7_OCMC_0 */
#define XPAR_PS7_OCMC_0_S_AXI_BASEADDR 0xF800C000
#define XPAR_PS7_OCMC_0_S_AXI_HIGHADDR 0xF800CFFF


/* Peripheral Definitions for peripheral PS7_PL310_0 */
#define XPAR_PS7_PL310_0_S_AXI_BASEADDR 0xF8F02000
#define XPAR_PS7_PL310_0_S_AXI_HIGHADDR 0xF8F02FFF


/* Peripheral Definitions for peripheral PS7_PMU_0 */
#define XPAR_PS7_PMU_0_S_AXI_BASEADDR 0xF8891000
#define XPAR_PS7_PMU_0_S_AXI_HIGHADDR 0xF8891FFF
#define XPAR_PS7_PMU_0_PMU1_S_AXI_BASEADDR 0xF8893000
#define XPAR_PS7_PMU_0_PMU1_S_AXI_HIGHADDR 0xF8893FFF


/* Peripheral Definitions for peripheral PS7_RAM_0 */
#define XPAR_PS7_RAM_0_S_AXI_BASEADDR 0x00000000
#define XPAR_PS7_RAM_0_S_AXI_HIGHADDR 0x0003FFFF


/* Peripheral Definitions for peripheral PS7_RAM_1 */
#define XPAR_PS7_RAM_1_S_AXI_BASEADDR 0xFFFC0000
#define XPAR_PS7_RAM_1_S_AXI_HIGHADDR 0xFFFFFFFF


/* Peripheral Definitions for peripheral PS7_SCUC_0 */
#define XPAR_PS7_SCUC_0_S_AXI_BASEADDR 0xF8F00000
#define XPAR_PS7_SCUC_0_S_AXI_HIGHADDR 0xF8F000FC


/* Peripheral Definitions for peripheral PS7_SLCR_0 */
#define XPAR_PS7_SLCR_0_S_AXI_BASEADDR 0xF8000000
#define XPAR_PS7_SLCR_0_S_AXI_HIGHADDR 0xF8000FFF


/******************************************************************/


/* Canonical Definitions for peripheral AUDIO_AXI_I2S_ADI_0 */
#define XPAR_AXI_I2S_ADI_0_BASEADDR 0x43C20000
#define XPAR_AXI_I2S_ADI_0_HIGHADDR 0x43C2FFFF


/* Canonical Definitions for peripheral AUDIO_FIFO_MIC */
#define XPAR_AXI_AXIS_READER_0_BASEADDR 0x43C50000
#define XPAR_AXI_AXIS_READER_0_HIGHADDR 0x43C50FFF


/* Canonical Definitions for peripheral AUDIO_FIFO_PHONES */
#define XPAR_AXI_AXIS_WRITER_0_BASEADDR 0x43C30000
#define XPAR_AXI_AXIS_WRITER_0_HIGHADDR 0x43C30FFF


/* Canonical Definitions for peripheral IQ_MODEM_FIFO_IQ_TX */
#define XPAR_AXI_AXIS_WRITER_1_BASEADDR 0x43C60000
#define XPAR_AXI_AXIS_WRITER_1_HIGHADDR 0x43C60FFF


/* Canonical Definitions for peripheral IQ_MODEM_FIR_RELOAD_RX */
#define XPAR_AXI_AXIS_WRITER_2_BASEADDR 0x43C80000
#define XPAR_AXI_AXIS_WRITER_2_HIGHADDR 0x43C80FFF


/* Canonical Definitions for peripheral IQ_MODEM_AXI_DDS_FTW */
#define XPAR_AXI_AXIS_WRITER_3_BASEADDR 0x43CB0000
#define XPAR_AXI_AXIS_WRITER_3_HIGHADDR 0x43CB0FFF


/* Canonical Definitions for peripheral IQ_MODEM_AXI_DDS_FTW_SUB */
#define XPAR_AXI_AXIS_WRITER_4_BASEADDR 0x43CC0000
#define XPAR_AXI_AXIS_WRITER_4_HIGHADDR 0x43CC0FFF


/* Canonical Definitions for peripheral IQ_MODEM_AXI_DDS_RTS */
#define XPAR_AXI_AXIS_WRITER_5_BASEADDR 0x43CD0000
#define XPAR_AXI_AXIS_WRITER_5_HIGHADDR 0x43CD0FFF


/* Canonical Definitions for peripheral IQ_MODEM_BLKMEM_CNT */
#define XPAR_AXI_STS_REGISTER_0_BASEADDR 0x43C90000
#define XPAR_AXI_STS_REGISTER_0_HIGHADDR 0x43C90FFF


/* Canonical Definitions for peripheral IQ_MODEM_BLKMEM_READER */
#define XPAR_AXI_BRAM_READER_0_BASEADDR 0x43CA0000
#define XPAR_AXI_BRAM_READER_0_HIGHADDR 0x43CA0FFF


/* Canonical Definitions for peripheral IQ_MODEM_MODEM_CONTROL */
#define XPAR_AXI_CFG_REGISTER_0_BASEADDR 0x43C40000
#define XPAR_AXI_CFG_REGISTER_0_HIGHADDR 0x43C40FFF




































/******************************************************************/

/* Definitions for driver GPIOPS */
#define XPAR_XGPIOPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_GPIO_0 */
#define XPAR_PS7_GPIO_0_DEVICE_ID 0
#define XPAR_PS7_GPIO_0_BASEADDR 0xE000A000
#define XPAR_PS7_GPIO_0_HIGHADDR 0xE000AFFF


/******************************************************************/

/* Canonical definitions for peripheral PS7_GPIO_0 */
#define XPAR_XGPIOPS_0_DEVICE_ID XPAR_PS7_GPIO_0_DEVICE_ID
#define XPAR_XGPIOPS_0_BASEADDR 0xE000A000
#define XPAR_XGPIOPS_0_HIGHADDR 0xE000AFFF


/******************************************************************/

/* Definitions for driver IICPS */
#define XPAR_XIICPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_I2C_0 */
#define XPAR_PS7_I2C_0_DEVICE_ID 0
#define XPAR_PS7_I2C_0_BASEADDR 0xE0004000
#define XPAR_PS7_I2C_0_HIGHADDR 0xE0004FFF
#define XPAR_PS7_I2C_0_I2C_CLK_FREQ_HZ 111111115


/******************************************************************/

/* Canonical definitions for peripheral PS7_I2C_0 */
#define XPAR_XIICPS_0_DEVICE_ID XPAR_PS7_I2C_0_DEVICE_ID
#define XPAR_XIICPS_0_BASEADDR 0xE0004000
#define XPAR_XIICPS_0_HIGHADDR 0xE0004FFF
#define XPAR_XIICPS_0_I2C_CLK_FREQ_HZ 111111115


/******************************************************************/

/* Definition for input Clock */
/* Definitions for Fabric interrupts connected to ps7_scugic_0 */
#define XPAR_FABRIC_VIDEO_V_TC_0_IRQ_INTR 62U
#define XPAR_FABRIC_VIDEO_AXI_VDMA_0_MM2S_INTROUT_INTR 66U

/******************************************************************/

/* Canonical definitions for Fabric interrupts connected to ps7_scugic_0 */
#define XPAR_FABRIC_VTC_0_VEC_ID XPAR_FABRIC_VIDEO_V_TC_0_IRQ_INTR
#define XPAR_FABRIC_AXIVDMA_0_VEC_ID XPAR_FABRIC_VIDEO_AXI_VDMA_0_MM2S_INTROUT_INTR

/******************************************************************/

/* Definitions for driver SCUGIC */
#define XPAR_XSCUGIC_NUM_INSTANCES 1U

/* Definitions for peripheral PS7_SCUGIC_0 */
#define XPAR_PS7_SCUGIC_0_DEVICE_ID 0U
#define XPAR_PS7_SCUGIC_0_BASEADDR 0xF8F00100U
#define XPAR_PS7_SCUGIC_0_HIGHADDR 0xF8F001FFU
#define XPAR_PS7_SCUGIC_0_DIST_BASEADDR 0xF8F01000U


/******************************************************************/

/* Canonical definitions for peripheral PS7_SCUGIC_0 */
#define XPAR_SCUGIC_0_DEVICE_ID 0U
#define XPAR_SCUGIC_0_CPU_BASEADDR 0xF8F00100U
#define XPAR_SCUGIC_0_CPU_HIGHADDR 0xF8F001FFU
#define XPAR_SCUGIC_0_DIST_BASEADDR 0xF8F01000U


/******************************************************************/

/* Definitions for driver SCUTIMER */
#define XPAR_XSCUTIMER_NUM_INSTANCES 1

/* Definitions for peripheral PS7_SCUTIMER_0 */
#define XPAR_PS7_SCUTIMER_0_DEVICE_ID 0
#define XPAR_PS7_SCUTIMER_0_BASEADDR 0xF8F00600
#define XPAR_PS7_SCUTIMER_0_HIGHADDR 0xF8F0061F


/******************************************************************/

/* Canonical definitions for peripheral PS7_SCUTIMER_0 */
#define XPAR_XSCUTIMER_0_DEVICE_ID XPAR_PS7_SCUTIMER_0_DEVICE_ID
#define XPAR_XSCUTIMER_0_BASEADDR 0xF8F00600
#define XPAR_XSCUTIMER_0_HIGHADDR 0xF8F0061F


/******************************************************************/

/* Definitions for driver SCUWDT */
#define XPAR_XSCUWDT_NUM_INSTANCES 1

/* Definitions for peripheral PS7_SCUWDT_0 */
#define XPAR_PS7_SCUWDT_0_DEVICE_ID 0
#define XPAR_PS7_SCUWDT_0_BASEADDR 0xF8F00620
#define XPAR_PS7_SCUWDT_0_HIGHADDR 0xF8F006FF


/******************************************************************/

/* Canonical definitions for peripheral PS7_SCUWDT_0 */
#define XPAR_SCUWDT_0_DEVICE_ID XPAR_PS7_SCUWDT_0_DEVICE_ID
#define XPAR_SCUWDT_0_BASEADDR 0xF8F00620
#define XPAR_SCUWDT_0_HIGHADDR 0xF8F006FF


/******************************************************************/

/* Definitions for driver SDPS */
#define XPAR_XSDPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_SD_0 */
#define XPAR_PS7_SD_0_DEVICE_ID 0
#define XPAR_PS7_SD_0_BASEADDR 0xE0100000
#define XPAR_PS7_SD_0_HIGHADDR 0xE0100FFF
#define XPAR_PS7_SD_0_SDIO_CLK_FREQ_HZ 25000000
#define XPAR_PS7_SD_0_HAS_CD 0
#define XPAR_PS7_SD_0_HAS_WP 0
#define XPAR_PS7_SD_0_BUS_WIDTH 0
#define XPAR_PS7_SD_0_MIO_BANK 0
#define XPAR_PS7_SD_0_HAS_EMIO 0


/******************************************************************/

#define XPAR_PS7_SD_0_IS_CACHE_COHERENT 0
/* Canonical definitions for peripheral PS7_SD_0 */
#define XPAR_XSDPS_0_DEVICE_ID XPAR_PS7_SD_0_DEVICE_ID
#define XPAR_XSDPS_0_BASEADDR 0xE0100000
#define XPAR_XSDPS_0_HIGHADDR 0xE0100FFF
#define XPAR_XSDPS_0_SDIO_CLK_FREQ_HZ 25000000
#define XPAR_XSDPS_0_HAS_CD 0
#define XPAR_XSDPS_0_HAS_WP 0
#define XPAR_XSDPS_0_BUS_WIDTH 0
#define XPAR_XSDPS_0_MIO_BANK 0
#define XPAR_XSDPS_0_HAS_EMIO 0
#define XPAR_XSDPS_0_IS_CACHE_COHERENT 0


/******************************************************************/

/* Definitions for driver UARTPS */
#define XPAR_XUARTPS_NUM_INSTANCES 2

/* Definitions for peripheral PS7_UART_0 */
#define XPAR_PS7_UART_0_DEVICE_ID 0
#define XPAR_PS7_UART_0_BASEADDR 0xE0000000
#define XPAR_PS7_UART_0_HIGHADDR 0xE0000FFF
#define XPAR_PS7_UART_0_UART_CLK_FREQ_HZ 100000000
#define XPAR_PS7_UART_0_HAS_MODEM 0


/* Definitions for peripheral PS7_UART_1 */
#define XPAR_PS7_UART_1_DEVICE_ID 1
#define XPAR_PS7_UART_1_BASEADDR 0xE0001000
#define XPAR_PS7_UART_1_HIGHADDR 0xE0001FFF
#define XPAR_PS7_UART_1_UART_CLK_FREQ_HZ 100000000
#define XPAR_PS7_UART_1_HAS_MODEM 0


/******************************************************************/

/* Canonical definitions for peripheral PS7_UART_0 */
#define XPAR_XUARTPS_0_DEVICE_ID XPAR_PS7_UART_0_DEVICE_ID
#define XPAR_XUARTPS_0_BASEADDR 0xE0000000
#define XPAR_XUARTPS_0_HIGHADDR 0xE0000FFF
#define XPAR_XUARTPS_0_UART_CLK_FREQ_HZ 100000000
#define XPAR_XUARTPS_0_HAS_MODEM 0

/* Canonical definitions for peripheral PS7_UART_1 */
#define XPAR_XUARTPS_1_DEVICE_ID XPAR_PS7_UART_1_DEVICE_ID
#define XPAR_XUARTPS_1_BASEADDR 0xE0001000
#define XPAR_XUARTPS_1_HIGHADDR 0xE0001FFF
#define XPAR_XUARTPS_1_UART_CLK_FREQ_HZ 100000000
#define XPAR_XUARTPS_1_HAS_MODEM 0


/******************************************************************/

/* Definition for input Clock */
/* Definition for input Clock */
/* Definitions for driver VTC */
#define XPAR_XVTC_NUM_INSTANCES 1

/* Definitions for peripheral VIDEO_V_TC_0 */
#define XPAR_VIDEO_V_TC_0_DEVICE_ID 0
#define XPAR_VIDEO_V_TC_0_BASEADDR 0x43C10000
#define XPAR_VIDEO_V_TC_0_HIGHADDR 0x43C1FFFF
#define XPAR_VIDEO_V_TC_0_GENERATE_EN 1
#define XPAR_VIDEO_V_TC_0_DETECT_EN 0
#define XPAR_VIDEO_V_TC_0_DET_HSYNC_EN 1
#define XPAR_VIDEO_V_TC_0_DET_VSYNC_EN 1
#define XPAR_VIDEO_V_TC_0_DET_HBLANK_EN 1
#define XPAR_VIDEO_V_TC_0_DET_VBLANK_EN 1
#define XPAR_VIDEO_V_TC_0_DET_AVIDEO_EN 1
#define XPAR_VIDEO_V_TC_0_DET_ACHROMA_EN 0


/******************************************************************/

/* Canonical definitions for peripheral VIDEO_V_TC_0 */
#define XPAR_VTC_0_DEVICE_ID XPAR_VIDEO_V_TC_0_DEVICE_ID
#define XPAR_VTC_0_BASEADDR 0x43C10000
#define XPAR_VTC_0_HIGHADDR 0x43C1FFFF
#define XPAR_VTC_0_GENERATE_EN 1
#define XPAR_VTC_0_DETECT_EN 0
#define XPAR_VTC_0_DET_HSYNC_EN 1
#define XPAR_VTC_0_DET_VSYNC_EN 1
#define XPAR_VTC_0_DET_HBLANK_EN 1
#define XPAR_VTC_0_DET_VBLANK_EN 1
#define XPAR_VTC_0_DET_AVIDEO_EN 1
#define XPAR_VTC_0_DET_ACHROMA_EN 0


/******************************************************************/

/* Definitions for driver XADCPS */
#define XPAR_XADCPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_XADC_0 */
#define XPAR_PS7_XADC_0_DEVICE_ID 0
#define XPAR_PS7_XADC_0_BASEADDR 0xF8007100
#define XPAR_PS7_XADC_0_HIGHADDR 0xF8007120


/******************************************************************/

/* Canonical definitions for peripheral PS7_XADC_0 */
#define XPAR_XADCPS_0_DEVICE_ID XPAR_PS7_XADC_0_DEVICE_ID
#define XPAR_XADCPS_0_BASEADDR 0xF8007100
#define XPAR_XADCPS_0_HIGHADDR 0xF8007120


/******************************************************************/

#endif  /* end of protection macro */
