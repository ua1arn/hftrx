#include "pll.h"
#include "mcom02.h"

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
/********MCOM-02 REGMAP DEFINE*******************************************/
/***************************System Registers*****************************/
#define CMCTR_BASE 0x38094000
#define GATE_SYS_CTR (*(volatile unsigned int *)(CMCTR_BASE + 0x04c))
#define CLK_I2C2_EN (1 << 18)
#define CLK_I2C1_EN (1 << 17)
#define CLK_I2C0_EN (1 << 16)
#define GATE_DSP_CTR (*(volatile unsigned int *)(CMCTR_BASE + 0x068))
#define DSPENC_EN (1 << 3)
#define DSPEXT_EN (1 << 2)
#define DSP1_EN (1 << 1)
#define DSP0_EN (1 << 0)
#define SEL_APLL (*(volatile unsigned int *)(CMCTR_BASE + 0x100))
#define SEL_CPLL (*(volatile unsigned int *)(CMCTR_BASE + 0x104))
#define SEL_DPLL (*(volatile unsigned int *)(CMCTR_BASE + 0x108))
#define SEL_SPLL (*(volatile unsigned int *)(CMCTR_BASE + 0x10c))
#define SEL_VPLL (*(volatile unsigned int *)(CMCTR_BASE + 0x110))
#define PLL_LOCK_BIT (1 << 31)

/***************************GPIO******************************************/
#define GPIO0_BASE 0x38034000
#define GPIO0(a) (*(volatile unsigned int *)(GPIO0_BASE + (a)))
#define SWPORTA_DR 0x00
#define SWPORTA_DDR 0x04
#define SWPORTA_CTL 0x08
#define SWPORTB_DR 0x0c
#define SWPORTB_DDR 0x10
#define SWPORTB_CTL 0x14
#define SWPORTC_DR 0x18
#define SWPORTC_DDR 0x1c
#define SWPORTC_CTL 0x20
#define SWPORTD_DR 0x24
#define SWPORTD_DDR 0x28
#define SWPORTD_CTL 0x2c

#define GPIOA29_I2C0_SDA (1 << 29)
#define GPIOA29_I2C0_SCL (1 << 30)
#define GPIOD22_I2C1_SDA (1 << 22)
#define GPIOD23_I2C1_SCL (1 << 23)
#define GPIOD24_I2C2_SDA (1 << 24)
#define GPIOD25_I2C2_SCL (1 << 25)

/***FAN53555**************************************************************/
/***5 A, 2.4MHz, Digitally Programmable TinyBuck Regulator****************/

/***CPU FREQ TABLE********************************************************/
#define CPU_FREQ_VOLTAGE_SIZE 3
const unsigned int CPU_FREQ_VOLTAGE[CPU_FREQ_VOLTAGE_SIZE][2]
    __attribute__((aligned(8))) = {
        //   MHz,   mV
        {816, 1040},
        {912, 1103},
        {1008, 1205}};

/***DSP FREQ TABLE********************************************************/
#define DSP_FREQ_VOLTAGE_SIZE 3
const unsigned int DSP_FREQ_VOLTAGE[CPU_FREQ_VOLTAGE_SIZE][2]
    __attribute__((aligned(8))) = {
        //   MHz,   mV
        {696, 1040},
        {768, 1103},
        {888, 1205}};


int setCPUFreq(unsigned int MHz)
{
    unsigned int last_freq;

    if (MHz < MIN_CPU_FREQ_MHZ || MHz > MAX_CPU_FREQ_MHZ )
    {
        return 1;
    }
    // Save current CPU freq
    last_freq = getCurrentCPUFreq();
    // Set default CPU freq
    setCPUPLLFreq(DEFAULT_XTI_CLOCK);

    // Set CPU freq
    setCPUPLLFreq(MHz);

    return 0;
}
int setDSPFreq(unsigned int MHz)
{
    unsigned int last_freq;

    if (MHz < MIN_DSP_FREQ_MHZ || MHz > MAX_DSP_FREQ_MHZ) 
    {
        return 1;
    }
    // Enable DSP_CLK
    GATE_DSP_CTR |= DSP0_EN | DSP1_EN | DSPEXT_EN | DSPENC_EN;

    // Save current DSP freq
    last_freq = getCurrentDSPFreq();
    // Set default DSP freq
    setDSPPLLFreq(DEFAULT_XTI_CLOCK);

    // Set DSP freq
    setDSPPLLFreq(MHz);

    return 0;
}

unsigned int getCurrentCPUFreq()
{
    int sel = (SEL_APLL & 0xFF);
    if (sel > 0x3D)
        sel = 0x3D;
    return (sel + 1) * DEFAULT_XTI_CLOCK;
}
unsigned int getCurrentDSPFreq()
{
    int sel = (SEL_DPLL & 0xFF);
    if (sel > 0x3D)
        sel = 0x3D;
    return (sel + 1) * DEFAULT_XTI_CLOCK;
}
void setCPUPLLFreq(unsigned int MHz)
{
    int sel = (MHz / DEFAULT_XTI_CLOCK) - 1;
    if (sel < 0)
        sel = 0;
    SEL_APLL = sel;
    while (!(SEL_APLL & PLL_LOCK_BIT))
        ;
}
void setDSPPLLFreq(unsigned int MHz)
{
    int sel = (MHz / DEFAULT_XTI_CLOCK) - 1;
    if (sel < 0)
        sel = 0;
    SEL_DPLL = sel;
    while (!(SEL_DPLL & PLL_LOCK_BIT))
        ;
}

int setSystemFreq(unsigned int MHz)
{
    int sel = (MHz / DEFAULT_XTI_CLOCK) - 1;
    if (sel < 0)
        sel = 0;

    if (sel > 5) {
        /* L3_PCLK can't be above 144 MHz */
        DIV_SYS1_CTR = 1;
    }

    if ((SEL_SPLL & 0xFF) != sel) {
        SEL_SPLL = sel;
        while (!(SEL_SPLL & PLL_LOCK_BIT))
            ;
    }

    if (sel <= 5) {
        DIV_SYS1_CTR = 0;
    }

    return 0;
}
