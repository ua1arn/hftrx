#ifndef XPARAMETERS_H   /* prevent circular inclusions */
#define XPARAMETERS_H   /* by using protection macros */

/* Definition for CPU ID */
#define XPAR_CPU_ID 0U

/* Definitions for peripheral PS7_CORTEXA9_0 */
#define XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ 766666687


/******************************************************************/

/* Canonical definitions for peripheral PS7_CORTEXA9_0 */
#define XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ 766666687


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
/* Definitions for driver AXIDMA */
#define XPAR_XAXIDMA_NUM_INSTANCES 2

/* Definitions for peripheral AXI_DMA_FIR_RELOAD */
#define XPAR_AXI_DMA_FIR_RELOAD_DEVICE_ID 0
#define XPAR_AXI_DMA_FIR_RELOAD_BASEADDR 0x40400000
#define XPAR_AXI_DMA_FIR_RELOAD_HIGHADDR 0x4040FFFF
#define XPAR_AXI_DMA_FIR_RELOAD_SG_INCLUDE_STSCNTRL_STRM 0
#define XPAR_AXI_DMA_FIR_RELOAD_INCLUDE_MM2S_DRE 1
#define XPAR_AXI_DMA_FIR_RELOAD_INCLUDE_S2MM_DRE 0
#define XPAR_AXI_DMA_FIR_RELOAD_INCLUDE_MM2S 1
#define XPAR_AXI_DMA_FIR_RELOAD_INCLUDE_S2MM 0
#define XPAR_AXI_DMA_FIR_RELOAD_M_AXI_MM2S_DATA_WIDTH 32
#define XPAR_AXI_DMA_FIR_RELOAD_M_AXI_S2MM_DATA_WIDTH 32
#define XPAR_AXI_DMA_FIR_RELOAD_INCLUDE_SG 0
#define XPAR_AXI_DMA_FIR_RELOAD_ENABLE_MULTI_CHANNEL 0
#define XPAR_AXI_DMA_FIR_RELOAD_NUM_MM2S_CHANNELS 1
#define XPAR_AXI_DMA_FIR_RELOAD_NUM_S2MM_CHANNELS 1
#define XPAR_AXI_DMA_FIR_RELOAD_MM2S_BURST_SIZE 16
#define XPAR_AXI_DMA_FIR_RELOAD_S2MM_BURST_SIZE 16
#define XPAR_AXI_DMA_FIR_RELOAD_MICRO_DMA 0
#define XPAR_AXI_DMA_FIR_RELOAD_ADDR_WIDTH 32
#define XPAR_AXI_DMA_FIR_RELOAD_SG_LENGTH_WIDTH 23


/* Definitions for peripheral AXI_DMA_IF_TX */
#define XPAR_AXI_DMA_IF_TX_DEVICE_ID 1
#define XPAR_AXI_DMA_IF_TX_BASEADDR 0x40410000
#define XPAR_AXI_DMA_IF_TX_HIGHADDR 0x4041FFFF
#define XPAR_AXI_DMA_IF_TX_SG_INCLUDE_STSCNTRL_STRM 0
#define XPAR_AXI_DMA_IF_TX_INCLUDE_MM2S_DRE 1
#define XPAR_AXI_DMA_IF_TX_INCLUDE_S2MM_DRE 0
#define XPAR_AXI_DMA_IF_TX_INCLUDE_MM2S 1
#define XPAR_AXI_DMA_IF_TX_INCLUDE_S2MM 0
#define XPAR_AXI_DMA_IF_TX_M_AXI_MM2S_DATA_WIDTH 64
#define XPAR_AXI_DMA_IF_TX_M_AXI_S2MM_DATA_WIDTH 32
#define XPAR_AXI_DMA_IF_TX_INCLUDE_SG 0
#define XPAR_AXI_DMA_IF_TX_ENABLE_MULTI_CHANNEL 0
#define XPAR_AXI_DMA_IF_TX_NUM_MM2S_CHANNELS 1
#define XPAR_AXI_DMA_IF_TX_NUM_S2MM_CHANNELS 1
#define XPAR_AXI_DMA_IF_TX_MM2S_BURST_SIZE 32
#define XPAR_AXI_DMA_IF_TX_S2MM_BURST_SIZE 16
#define XPAR_AXI_DMA_IF_TX_MICRO_DMA 0
#define XPAR_AXI_DMA_IF_TX_ADDR_WIDTH 32
#define XPAR_AXI_DMA_IF_TX_SG_LENGTH_WIDTH 14


/******************************************************************/

/* Canonical definitions for peripheral AXI_DMA_FIR_RELOAD */
#define XPAR_AXIDMA_0_DEVICE_ID XPAR_AXI_DMA_FIR_RELOAD_DEVICE_ID
#define XPAR_AXIDMA_0_BASEADDR 0x40400000
#define XPAR_AXIDMA_0_SG_INCLUDE_STSCNTRL_STRM 0
#define XPAR_AXIDMA_0_INCLUDE_MM2S 1
#define XPAR_AXIDMA_0_INCLUDE_MM2S_DRE 1
#define XPAR_AXIDMA_0_M_AXI_MM2S_DATA_WIDTH 32
#define XPAR_AXIDMA_0_INCLUDE_S2MM 0
#define XPAR_AXIDMA_0_INCLUDE_S2MM_DRE 0
#define XPAR_AXIDMA_0_M_AXI_S2MM_DATA_WIDTH 32
#define XPAR_AXIDMA_0_INCLUDE_SG 0
#define XPAR_AXIDMA_0_ENABLE_MULTI_CHANNEL 0
#define XPAR_AXIDMA_0_NUM_MM2S_CHANNELS 1
#define XPAR_AXIDMA_0_NUM_S2MM_CHANNELS 1
#define XPAR_AXIDMA_0_MM2S_BURST_SIZE 16
#define XPAR_AXIDMA_0_S2MM_BURST_SIZE 16
#define XPAR_AXIDMA_0_MICRO_DMA 0
#define XPAR_AXIDMA_0_c_addr_width 32
#define XPAR_AXIDMA_0_c_sg_length_width 23

/* Canonical definitions for peripheral AXI_DMA_IF_TX */
#define XPAR_AXIDMA_1_DEVICE_ID XPAR_AXI_DMA_IF_TX_DEVICE_ID
#define XPAR_AXIDMA_1_BASEADDR 0x40410000
#define XPAR_AXIDMA_1_SG_INCLUDE_STSCNTRL_STRM 0
#define XPAR_AXIDMA_1_INCLUDE_MM2S 1
#define XPAR_AXIDMA_1_INCLUDE_MM2S_DRE 1
#define XPAR_AXIDMA_1_M_AXI_MM2S_DATA_WIDTH 64
#define XPAR_AXIDMA_1_INCLUDE_S2MM 0
#define XPAR_AXIDMA_1_INCLUDE_S2MM_DRE 0
#define XPAR_AXIDMA_1_M_AXI_S2MM_DATA_WIDTH 32
#define XPAR_AXIDMA_1_INCLUDE_SG 0
#define XPAR_AXIDMA_1_ENABLE_MULTI_CHANNEL 0
#define XPAR_AXIDMA_1_NUM_MM2S_CHANNELS 1
#define XPAR_AXIDMA_1_NUM_S2MM_CHANNELS 1
#define XPAR_AXIDMA_1_MM2S_BURST_SIZE 32
#define XPAR_AXIDMA_1_S2MM_BURST_SIZE 16
#define XPAR_AXIDMA_1_MICRO_DMA 0
#define XPAR_AXIDMA_1_c_addr_width 32
#define XPAR_AXIDMA_1_c_sg_length_width 14


/******************************************************************/

/* Definitions for driver AXIVDMA */
#define XPAR_XAXIVDMA_NUM_INSTANCES 1U

/* Definitions for peripheral AXI_VDMA_0 */
#define XPAR_AXI_VDMA_0_DEVICE_ID 0U
#define XPAR_AXI_VDMA_0_BASEADDR 0x43000000U
#define XPAR_AXI_VDMA_0_HIGHADDR 0x4300FFFFU
#define XPAR_AXI_VDMA_0_NUM_FSTORES 3U
#define XPAR_AXI_VDMA_0_INCLUDE_MM2S 1U
#define XPAR_AXI_VDMA_0_INCLUDE_MM2S_DRE 0U
#define XPAR_AXI_VDMA_0_M_AXI_MM2S_DATA_WIDTH 32U
#define XPAR_AXI_VDMA_0_INCLUDE_S2MM 0U
#define XPAR_AXI_VDMA_0_INCLUDE_S2MM_DRE 0U
#define XPAR_AXI_VDMA_0_M_AXI_S2MM_DATA_WIDTH 64U
#define XPAR_AXI_VDMA_0_AXI_MM2S_ACLK_FREQ_HZ 0U
#define XPAR_AXI_VDMA_0_AXI_S2MM_ACLK_FREQ_HZ 0U
#define XPAR_AXI_VDMA_0_MM2S_GENLOCK_MODE 3U
#define XPAR_AXI_VDMA_0_MM2S_GENLOCK_NUM_MASTERS 1U
#define XPAR_AXI_VDMA_0_S2MM_GENLOCK_MODE 0U
#define XPAR_AXI_VDMA_0_S2MM_GENLOCK_NUM_MASTERS 1U
#define XPAR_AXI_VDMA_0_INCLUDE_SG 0U
#define XPAR_AXI_VDMA_0_ENABLE_VIDPRMTR_READS 1U
#define XPAR_AXI_VDMA_0_USE_FSYNC 1U
#define XPAR_AXI_VDMA_0_FLUSH_ON_FSYNC 1U
#define XPAR_AXI_VDMA_0_MM2S_LINEBUFFER_DEPTH 2048U
#define XPAR_AXI_VDMA_0_S2MM_LINEBUFFER_DEPTH 512U
#define XPAR_AXI_VDMA_0_INCLUDE_INTERNAL_GENLOCK 1U
#define XPAR_AXI_VDMA_0_S2MM_SOF_ENABLE 1U
#define XPAR_AXI_VDMA_0_M_AXIS_MM2S_TDATA_WIDTH 32U
#define XPAR_AXI_VDMA_0_S_AXIS_S2MM_TDATA_WIDTH 32U
#define XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_1 0U
#define XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_5 0U
#define XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_6 1U
#define XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_7 1U
#define XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_9 0U
#define XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_13 0U
#define XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_14 1U
#define XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_15 1U
#define XPAR_AXI_VDMA_0_ENABLE_DEBUG_ALL 0U
#define XPAR_AXI_VDMA_0_ADDR_WIDTH 32U
#define XPAR_AXI_VDMA_0_ENABLE_VERT_FLIP 0U


/******************************************************************/

/* Canonical definitions for peripheral AXI_VDMA_0 */
#define XPAR_AXIVDMA_0_DEVICE_ID XPAR_AXI_VDMA_0_DEVICE_ID
#define XPAR_AXIVDMA_0_BASEADDR 0x43000000U
#define XPAR_AXIVDMA_0_HIGHADDR 0x4300FFFFU
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
#define XPAR_PS7_ETHERNET_0_ENET_CLK_FREQ_HZ 125000000
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
#define XPAR_XEMACPS_0_ENET_CLK_FREQ_HZ 125000000
#define XPAR_XEMACPS_0_ENET_SLCR_1000Mbps_DIV0 8
#define XPAR_XEMACPS_0_ENET_SLCR_1000Mbps_DIV1 1
#define XPAR_XEMACPS_0_ENET_SLCR_100Mbps_DIV0 8
#define XPAR_XEMACPS_0_ENET_SLCR_100Mbps_DIV1 5
#define XPAR_XEMACPS_0_ENET_SLCR_10Mbps_DIV0 8
#define XPAR_XEMACPS_0_ENET_SLCR_10Mbps_DIV1 50
#define XPAR_XEMACPS_0_ENET_TSU_CLK_FREQ_HZ 0


/******************************************************************/


/* Peripheral Definitions for peripheral AXI_I2S_ADI_0 */
#define XPAR_AXI_I2S_ADI_0_BASEADDR 0x43C30000
#define XPAR_AXI_I2S_ADI_0_HIGHADDR 0x43C3FFFF


/* Peripheral Definitions for peripheral CPU_FAN_PWM_0 */
#define XPAR_CPU_FAN_PWM_0_S00_AXI_BASEADDR 0x43C20000
#define XPAR_CPU_FAN_PWM_0_S00_AXI_HIGHADDR 0x43C2FFFF


/* Peripheral Definitions for peripheral DCDC_PWM_1 */
#define XPAR_DCDC_PWM_1_S00_AXI_BASEADDR 0x43C70000
#define XPAR_DCDC_PWM_1_S00_AXI_HIGHADDR 0x43C7FFFF


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


/* Peripheral Definitions for peripheral PS7_QSPI_LINEAR_0 */
#define XPAR_PS7_QSPI_LINEAR_0_S_AXI_BASEADDR 0xFC000000
#define XPAR_PS7_QSPI_LINEAR_0_S_AXI_HIGHADDR 0xFDFFFFFF


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


/* Peripheral Definitions for peripheral TRX_CONTROL2_0 */
#define XPAR_TRX_CONTROL2_0_S00_AXI_BASEADDR 0x43C40000
#define XPAR_TRX_CONTROL2_0_S00_AXI_HIGHADDR 0x43C4FFFF


/******************************************************************/




/* Canonical Definitions for peripheral CPU_FAN_PWM_0 */
#define XPAR_AX_PWM_0_S00_AXI_BASEADDR 0x43C20000
#define XPAR_AX_PWM_0_S00_AXI_HIGHADDR 0x43C2FFFF


/* Canonical Definitions for peripheral DCDC_PWM_1 */
#define XPAR_AX_PWM_1_S00_AXI_BASEADDR 0x43C70000
#define XPAR_AX_PWM_1_S00_AXI_HIGHADDR 0x43C7FFFF








































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
#define XPAR_PS7_I2C_0_I2C_CLK_FREQ_HZ 127777779


/******************************************************************/

/* Canonical definitions for peripheral PS7_I2C_0 */
#define XPAR_XIICPS_0_DEVICE_ID XPAR_PS7_I2C_0_DEVICE_ID
#define XPAR_XIICPS_0_BASEADDR 0xE0004000
#define XPAR_XIICPS_0_HIGHADDR 0xE0004FFF
#define XPAR_XIICPS_0_I2C_CLK_FREQ_HZ 127777779


/******************************************************************/

/* Definition for input Clock */
/* Definitions for driver LLFIFO */
#define XPAR_XLLFIFO_NUM_INSTANCES 3U

/* Definitions for peripheral AXI_FIFO_IQ_RX */
#define XPAR_AXI_FIFO_IQ_RX_DEVICE_ID 0U
#define XPAR_AXI_FIFO_IQ_RX_BASEADDR 0x43C00000U
#define XPAR_AXI_FIFO_IQ_RX_HIGHADDR 0x43C0FFFFU
#define XPAR_AXI_FIFO_IQ_RX_AXI4_BASEADDR 0U
#define XPAR_AXI_FIFO_IQ_RX_AXI4_HIGHADDR 0U
#define XPAR_AXI_FIFO_IQ_RX_DATA_INTERFACE_TYPE 0U

/* Canonical definitions for peripheral AXI_FIFO_IQ_RX */
#define XPAR_AXI_FIFO_0_DEVICE_ID 0U
#define XPAR_AXI_FIFO_0_BASEADDR 0x43C00000U
#define XPAR_AXI_FIFO_0_HIGHADDR 0x43C0FFFFU
#define XPAR_AXI_FIFO_0_AXI4_BASEADDR 0U
#define XPAR_AXI_FIFO_0_AXI4_HIGHADDR 0U
#define XPAR_AXI_FIFO_0_DATA_INTERFACE_TYPE 0U



/* Definitions for peripheral AXI_FIFO_MIC */
#define XPAR_AXI_FIFO_MIC_DEVICE_ID 1U
#define XPAR_AXI_FIFO_MIC_BASEADDR 0x43C10000U
#define XPAR_AXI_FIFO_MIC_HIGHADDR 0x43C1FFFFU
#define XPAR_AXI_FIFO_MIC_AXI4_BASEADDR 0U
#define XPAR_AXI_FIFO_MIC_AXI4_HIGHADDR 0U
#define XPAR_AXI_FIFO_MIC_DATA_INTERFACE_TYPE 0U

/* Canonical definitions for peripheral AXI_FIFO_MIC */
#define XPAR_AXI_FIFO_1_DEVICE_ID 1U
#define XPAR_AXI_FIFO_1_BASEADDR 0x43C10000U
#define XPAR_AXI_FIFO_1_HIGHADDR 0x43C1FFFFU
#define XPAR_AXI_FIFO_1_AXI4_BASEADDR 0U
#define XPAR_AXI_FIFO_1_AXI4_HIGHADDR 0U
#define XPAR_AXI_FIFO_1_DATA_INTERFACE_TYPE 0U



/* Definitions for peripheral AXI_FIFO_PHONES */
#define XPAR_AXI_FIFO_PHONES_DEVICE_ID 2U
#define XPAR_AXI_FIFO_PHONES_BASEADDR 0x43C60000U
#define XPAR_AXI_FIFO_PHONES_HIGHADDR 0x43C6FFFFU
#define XPAR_AXI_FIFO_PHONES_AXI4_BASEADDR 0U
#define XPAR_AXI_FIFO_PHONES_AXI4_HIGHADDR 0U
#define XPAR_AXI_FIFO_PHONES_DATA_INTERFACE_TYPE 0U

/* Canonical definitions for peripheral AXI_FIFO_PHONES */
#define XPAR_AXI_FIFO_2_DEVICE_ID 2U
#define XPAR_AXI_FIFO_2_BASEADDR 0x43C60000U
#define XPAR_AXI_FIFO_2_HIGHADDR 0x43C6FFFFU
#define XPAR_AXI_FIFO_2_AXI4_BASEADDR 0U
#define XPAR_AXI_FIFO_2_AXI4_HIGHADDR 0U
#define XPAR_AXI_FIFO_2_DATA_INTERFACE_TYPE 0U



/******************************************************************/

/* Definitions for driver QSPIPS */
#define XPAR_XQSPIPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_QSPI_0 */
#define XPAR_PS7_QSPI_0_DEVICE_ID 0
#define XPAR_PS7_QSPI_0_BASEADDR 0xE000D000
#define XPAR_PS7_QSPI_0_HIGHADDR 0xE000DFFF
#define XPAR_PS7_QSPI_0_QSPI_CLK_FREQ_HZ 200000000
#define XPAR_PS7_QSPI_0_QSPI_MODE 0
#define XPAR_PS7_QSPI_0_QSPI_BUS_WIDTH 2


/******************************************************************/

/* Canonical definitions for peripheral PS7_QSPI_0 */
#define XPAR_XQSPIPS_0_DEVICE_ID XPAR_PS7_QSPI_0_DEVICE_ID
#define XPAR_XQSPIPS_0_BASEADDR 0xE000D000
#define XPAR_XQSPIPS_0_HIGHADDR 0xE000DFFF
#define XPAR_XQSPIPS_0_QSPI_CLK_FREQ_HZ 200000000
#define XPAR_XQSPIPS_0_QSPI_MODE 0
#define XPAR_XQSPIPS_0_QSPI_BUS_WIDTH 2


/******************************************************************/

/* Definitions for Fabric interrupts connected to ps7_scugic_0 */
#define XPAR_FABRIC_V_TC_0_IRQ_INTR 61U
#define XPAR_FABRIC_AXI_DMA_IF_TX_MM2S_INTROUT_INTR 62U
#define XPAR_FABRIC_AXI_FIFO_IQ_RX_INTERRUPT_INTR 63U
#define XPAR_FABRIC_AXI_FIFO_PHONES_INTERRUPT_INTR 64U
#define XPAR_FABRIC_AXI_FIFO_MIC_INTERRUPT_INTR 65U

/******************************************************************/

/* Canonical definitions for Fabric interrupts connected to ps7_scugic_0 */
#define XPAR_FABRIC_VTC_0_VEC_ID XPAR_FABRIC_V_TC_0_IRQ_INTR
#define XPAR_FABRIC_AXIDMA_1_VEC_ID XPAR_FABRIC_AXI_DMA_IF_TX_MM2S_INTROUT_INTR
#define XPAR_FABRIC_LLFIFO_0_VEC_ID XPAR_FABRIC_AXI_FIFO_IQ_RX_INTERRUPT_INTR
#define XPAR_FABRIC_LLFIFO_2_VEC_ID XPAR_FABRIC_AXI_FIFO_PHONES_INTERRUPT_INTR
#define XPAR_FABRIC_LLFIFO_1_VEC_ID XPAR_FABRIC_AXI_FIFO_MIC_INTERRUPT_INTR

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

/* Definitions for driver UARTPS */
#define XPAR_XUARTPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_UART_1 */
#define XPAR_PS7_UART_1_DEVICE_ID 0
#define XPAR_PS7_UART_1_BASEADDR 0xE0001000
#define XPAR_PS7_UART_1_HIGHADDR 0xE0001FFF
#define XPAR_PS7_UART_1_UART_CLK_FREQ_HZ 100000000
#define XPAR_PS7_UART_1_HAS_MODEM 0


/******************************************************************/

/* Canonical definitions for peripheral PS7_UART_1 */
#define XPAR_XUARTPS_0_DEVICE_ID XPAR_PS7_UART_1_DEVICE_ID
#define XPAR_XUARTPS_0_BASEADDR 0xE0001000
#define XPAR_XUARTPS_0_HIGHADDR 0xE0001FFF
#define XPAR_XUARTPS_0_UART_CLK_FREQ_HZ 100000000
#define XPAR_XUARTPS_0_HAS_MODEM 0


/******************************************************************/

/* Definition for input Clock */
/* Definitions for driver USBPS */
#define XPAR_XUSBPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_USB_0 */
#define XPAR_PS7_USB_0_DEVICE_ID 0
#define XPAR_PS7_USB_0_BASEADDR 0xE0002000
#define XPAR_PS7_USB_0_HIGHADDR 0xE0002FFF


/******************************************************************/

/* Canonical definitions for peripheral PS7_USB_0 */
#define XPAR_XUSBPS_0_DEVICE_ID XPAR_PS7_USB_0_DEVICE_ID
#define XPAR_XUSBPS_0_BASEADDR 0xE0002000
#define XPAR_XUSBPS_0_HIGHADDR 0xE0002FFF


/******************************************************************/

/* Definitions for driver VTC */
#define XPAR_XVTC_NUM_INSTANCES 1

/* Definitions for peripheral V_TC_0 */
#define XPAR_V_TC_0_DEVICE_ID 0
#define XPAR_V_TC_0_BASEADDR 0x43C50000
#define XPAR_V_TC_0_HIGHADDR 0x43C5FFFF
#define XPAR_V_TC_0_GENERATE_EN 1
#define XPAR_V_TC_0_DETECT_EN 0
#define XPAR_V_TC_0_DET_HSYNC_EN 1
#define XPAR_V_TC_0_DET_VSYNC_EN 1
#define XPAR_V_TC_0_DET_HBLANK_EN 1
#define XPAR_V_TC_0_DET_VBLANK_EN 1
#define XPAR_V_TC_0_DET_AVIDEO_EN 1
#define XPAR_V_TC_0_DET_ACHROMA_EN 0


/******************************************************************/

/* Canonical definitions for peripheral V_TC_0 */
#define XPAR_VTC_0_DEVICE_ID XPAR_V_TC_0_DEVICE_ID
#define XPAR_VTC_0_BASEADDR 0x43C50000
#define XPAR_VTC_0_HIGHADDR 0x43C5FFFF
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
