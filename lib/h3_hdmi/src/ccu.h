#ifdef __cplusplus
extern "C" {
#endif

// The CCU registers base address.
//#define CCU_BASE 0x01C20000

// Structure of CCU registers.
#define PLL_CPUX_CTRL         *(volatile uint32_t *)(CCU_BASE + 0x000)
#define PLL_AUDIO_CTRL        *(volatile uint32_t *)(CCU_BASE + 0x008)
#define PLL_VIDEO_CTRL        *(volatile uint32_t *)(CCU_BASE + 0x010)
#define PLL_VE_CTRL           *(volatile uint32_t *)(CCU_BASE + 0x018)
#define PLL_DDR_CTRL          *(volatile uint32_t *)(CCU_BASE + 0x020)
#define PLL_PERIPH0_CTRL      *(volatile uint32_t *)(CCU_BASE + 0x028)
#define PLL_GPU_CTRL          *(volatile uint32_t *)(CCU_BASE + 0x038)
#define PLL_PERIPH1_CTRL      *(volatile uint32_t *)(CCU_BASE + 0x044)
#define PLL_DE_CTRL           *(volatile uint32_t *)(CCU_BASE + 0x048)
#define CPUX_AXI_CFG          *(volatile uint32_t *)(CCU_BASE + 0x050)
#define AHB1_APB1_CFG         *(volatile uint32_t *)(CCU_BASE + 0x054)
#define APB2_CFG              *(volatile uint32_t *)(CCU_BASE + 0x058)
#define AHB2_CFG              *(volatile uint32_t *)(CCU_BASE + 0x05C)
#define BUS_CLK_GATING0       *(volatile uint32_t *)(CCU_BASE + 0x060)
#define BUS_CLK_GATING1       *(volatile uint32_t *)(CCU_BASE + 0x064)
#define BUS_CLK_GATING2       *(volatile uint32_t *)(CCU_BASE + 0x068)
#define BUS_CLK_GATING3       *(volatile uint32_t *)(CCU_BASE + 0x06C)
#define BUS_CLK_GATING4       *(volatile uint32_t *)(CCU_BASE + 0x070)
#define THS_CLK               *(volatile uint32_t *)(CCU_BASE + 0x074)
#define NAND_CLK              *(volatile uint32_t *)(CCU_BASE + 0x080)
#define SDMMC0_CLK            *(volatile uint32_t *)(CCU_BASE + 0x088)
#define SDMMC1_CLK            *(volatile uint32_t *)(CCU_BASE + 0x08C)
#define SDMMC2_CLK            *(volatile uint32_t *)(CCU_BASE + 0x090)
#define CE_CLK                *(volatile uint32_t *)(CCU_BASE + 0x09C)
#define SPI0_CLK              *(volatile uint32_t *)(CCU_BASE + 0x0A0)
#define SPI1_CLK              *(volatile uint32_t *)(CCU_BASE + 0x0A4)
#define I2S_PCM0_CLK          *(volatile uint32_t *)(CCU_BASE + 0x0B0)
#define I2S_PCM1_CLK          *(volatile uint32_t *)(CCU_BASE + 0x0B4)
#define I2S_PCM2_CLK          *(volatile uint32_t *)(CCU_BASE + 0x0B8)
#define OWA_CLK               *(volatile uint32_t *)(CCU_BASE + 0x0C0)
#define USBPHY_CFG            *(volatile uint32_t *)(CCU_BASE + 0x0CC)
#define DRAM_CFG              *(volatile uint32_t *)(CCU_BASE + 0x0F4)
#define MBUS_RST              *(volatile uint32_t *)(CCU_BASE + 0x0FC)
#define DRAM_CLK_GATING       *(volatile uint32_t *)(CCU_BASE + 0x100)
#define DE_CLK                *(volatile uint32_t *)(CCU_BASE + 0x104)
#define TCON0_CLK             *(volatile uint32_t *)(CCU_BASE + 0x118)
#define TVE_CLK               *(volatile uint32_t *)(CCU_BASE + 0x120)
#define DEINTERLACE_CLK       *(volatile uint32_t *)(CCU_BASE + 0x124)
#define CSI_MISC_CLK          *(volatile uint32_t *)(CCU_BASE + 0x130)
#define CSI_CLK               *(volatile uint32_t *)(CCU_BASE + 0x134)
#define VE_CLK                *(volatile uint32_t *)(CCU_BASE + 0x13C)
#define AC_DIG_CLK            *(volatile uint32_t *)(CCU_BASE + 0x140)
#define AVS_CLK               *(volatile uint32_t *)(CCU_BASE + 0x144)
#define HDMI_CLK              *(volatile uint32_t *)(CCU_BASE + 0x150)
#define HDMI_SLOW_CLK         *(volatile uint32_t *)(CCU_BASE + 0x154)
#define MBUS_CLK              *(volatile uint32_t *)(CCU_BASE + 0x15C)
#define GPU_CLK               *(volatile uint32_t *)(CCU_BASE + 0x1A0)
#define PLL_STABLE_TIME0      *(volatile uint32_t *)(CCU_BASE + 0x200)
#define PLL_STABLE_TIME1      *(volatile uint32_t *)(CCU_BASE + 0x204)
#define PLL_CPUX_BIAS         *(volatile uint32_t *)(CCU_BASE + 0x220)
#define PLL_AUDIO_BIAS        *(volatile uint32_t *)(CCU_BASE + 0x224)
#define PLL_VIDEO_BIAS        *(volatile uint32_t *)(CCU_BASE + 0x228)
#define PLL_VE_BIAS           *(volatile uint32_t *)(CCU_BASE + 0x22C)
#define PLL_DDR_BIAS          *(volatile uint32_t *)(CCU_BASE + 0x230)
#define PLL_PERIPH0_BIAS      *(volatile uint32_t *)(CCU_BASE + 0x234)
#define PLL_GPU_BIAS          *(volatile uint32_t *)(CCU_BASE + 0x23C)
#define PLL_PERIPH1_BIAS      *(volatile uint32_t *)(CCU_BASE + 0x244)
#define PLL_DE_BIAS           *(volatile uint32_t *)(CCU_BASE + 0x248)
#define PLL_CPUX_TUN          *(volatile uint32_t *)(CCU_BASE + 0x250)
#define PLL_DDR_TUN           *(volatile uint32_t *)(CCU_BASE + 0x260)
#define PLL_CPUX_PAT_CTRL     *(volatile uint32_t *)(CCU_BASE + 0x280)
#define PLL_AUDIO_PAT_CTRL0   *(volatile uint32_t *)(CCU_BASE + 0x284)
#define PLL_VIDEO_PAT_CTRL0   *(volatile uint32_t *)(CCU_BASE + 0x288)
#define PLL_VE_PAT_CTRL       *(volatile uint32_t *)(CCU_BASE + 0x28C)
#define PLL_DDR_PAT_CTRL0     *(volatile uint32_t *)(CCU_BASE + 0x290)
#define PLL_GPU_PAT_CTRL      *(volatile uint32_t *)(CCU_BASE + 0x29C)
#define PLL_PERIPH1_PAT_CTRL1 *(volatile uint32_t *)(CCU_BASE + 0x2A4)
#define PLL_DE_PAT_CTRL       *(volatile uint32_t *)(CCU_BASE + 0x2A8)
#define BUS_SOFT_RST0         *(volatile uint32_t *)(CCU_BASE + 0x2C0)
#define BUS_SOFT_RST1         *(volatile uint32_t *)(CCU_BASE + 0x2C4)
#define BUS_SOFT_RST2         *(volatile uint32_t *)(CCU_BASE + 0x2C8)
#define BUS_SOFT_RST3         *(volatile uint32_t *)(CCU_BASE + 0x2D0)
#define BUS_SOFT_RST4         *(volatile uint32_t *)(CCU_BASE + 0x2D8)
#define CCU_SEC_SWITCH        *(volatile uint32_t *)(CCU_BASE + 0x2F0)
#define PS_CTRL               *(volatile uint32_t *)(CCU_BASE + 0x300)
#define PS_CNT                *(volatile uint32_t *)(CCU_BASE + 0x304)

//#define R_PRCM_BASE 0x01F01400
#define APB0_CLK_GATING       *(volatile uint32_t *)(R_PRCM_BASE + 0x28)

#define PLL_CPUX_FACTOR_K_SHIFT 4
#define PLL_CPUX_FACTOR_N_SHIFT 8

#define PLL_CPUX_FACTOR_K_MASK	0x00000030UL
#define PLL_CPUX_FACTOR_N_MASK  0x00001f00UL

#ifdef __cplusplus
}
#endif
