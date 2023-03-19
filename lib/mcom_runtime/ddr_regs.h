/*
 * regs.h
 *
 *  Created on: Aug 29, 2013
 *      Author: dzagrebin
 */

#ifndef REGS_H
#define REGS_H

#define DDRMC0_BASE 0x37204000
#define DDRPHY0_BASE 0x37205000
#define DDRMC1_BASE 0x37206000
#define DDRPHY1_BASE 0x37207000
#define NFCM_BASE 0x38007000
#define SDMMC0_BASE 0x3800B000
#define SDMMC1_BASE 0x3800D000
#define TIMERS_BASE 0x38026000
#define RTC_BASE 0x38027000
//#define UART0_BASE 0x38028000
#define BIST_BRIDGE_BASE 0x38035000
#define MFBSP0_BASE 0x38086000
#define MFBSP1_BASE 0x38088000
#define CMCTR_BASE 0x38094000
#define PMCTR_BASE 0x38095000
#define SMCTR_BASE 0x38096000

typedef struct {
    // CMCTR_MPU
    volatile unsigned int RESERVED0;
    volatile unsigned int DIV_MPU_CTR;
    volatile unsigned int DIV_ATB_CTR;
    volatile unsigned int DIV_APB_CTR;
    volatile unsigned int RESERVED1;
    volatile unsigned int GATE_MPU_CTR;
    volatile unsigned int RESERVED2[2];
    /* CMCTR_CORE */
    volatile unsigned int RESERVED3;
    volatile unsigned int RESERVED4;
    volatile unsigned int RESERVED5;
    volatile unsigned int DIV_GPU_CTR;
    volatile unsigned int DIV_DDR0_CTR;
    volatile unsigned int DIV_DDR1_CTR;
    volatile unsigned int DIV_NFC_CTR;
    volatile unsigned int DIV_NOR_CTR;
    volatile unsigned int DIV_SYS0_CTR;
    volatile unsigned int DIV_SYS1_CTR;
    volatile unsigned int GATE_CORE_CTR;
    volatile unsigned int GATE_SYS_CTR;
    volatile unsigned int RESERVED6[4];
    /* CMCTR_DSP */
    volatile unsigned int RESERVED7;
    volatile unsigned int RESERVED8;
    volatile unsigned int GATE_DSP_CTR;
    volatile unsigned int RESERVED9[5];
    /* CLKOUT */
    volatile unsigned int RESERVED10;
    volatile unsigned int DIV_CLKOUT;
    volatile unsigned int GATE_CLKOUT;
    volatile unsigned int RESERVED11;
    /* LS_ENABLE */
    volatile unsigned int LS_ENABLE;
    volatile unsigned int RESERVED12[27];
    /* PLL */
    volatile unsigned int SEL_APLL;
    volatile unsigned int SEL_CPLL;
    volatile unsigned int SEL_DPLL;
    volatile unsigned int SEL_SPLL;
    volatile unsigned int SEL_VPLL;
    volatile unsigned int SEL_UPLL;
} cmctr_t;

typedef struct {
    volatile unsigned int RESERVED0;
    volatile unsigned int SYS_PWR_DOWN;
    volatile unsigned int RESERVED1;
    volatile unsigned int SYS_PWR_STATUS;
    volatile unsigned int SYS_PWR_IMASK;
    volatile unsigned int SYS_PWR_IRSTAT;
    volatile unsigned int SYS_PWR_ISTAT;
    volatile unsigned int SYS_PWR_ICLR;
    volatile unsigned int SYS_PWR_DELAY;
    volatile unsigned int DDR_PIN_RET;
    volatile unsigned int DDR_INIT_END;
    volatile unsigned int WARM_RST_EN;
    volatile unsigned int WKP_IMASK;
    volatile unsigned int WKP_IRSTAT;
    volatile unsigned int WKP_ISTAT;
    volatile unsigned int WKP_ICLR;
    volatile unsigned int SW_RST;
    volatile unsigned int WARM_RST_STATUS;
    volatile unsigned int PDM_RST_STATUS;
    volatile unsigned int VMODE;
    volatile unsigned char CPU0_WKP_MASK[16];
    volatile unsigned char CPU1_WKP_MASK[16];
    volatile unsigned int ALWAYS_MISC0;
    volatile unsigned int ALWAYS_MISC1;
    volatile unsigned int WARM_BOOT_OVRD;
    volatile unsigned int CORE_PWR_UP;
    volatile unsigned int CORE_PWR_DOWN;
    volatile unsigned int RESERVED2;
    volatile unsigned int CORE_PWR_STATUS;
    volatile unsigned int CORE_PWR_IMASK;
    volatile unsigned int CORE_PWR_IRSTAT;
    volatile unsigned int CORE_PWR_ISTAT;
    volatile unsigned int CORE_PWR_ICLR;
    volatile unsigned int CORE_PWR_DELAY;
} pmctr_t;

typedef struct {
    volatile unsigned int BOOT;
    volatile unsigned int BOOT_REMAP;
    volatile unsigned int MFU_CFGNMFI;
    volatile unsigned int DDR_REMAP;
    volatile unsigned int CPU_SECURE_CTR;
    volatile unsigned int ACP_CTL;
    volatile unsigned int MIPI_MUX;
    volatile unsigned int CHIP_ID;
    volatile unsigned int CHIP_CONFIG;
    volatile unsigned int EMA_ARM;
    volatile unsigned int EMA_L2;
    volatile unsigned int EMA_DSP;
    volatile unsigned int EMA_CORE;
    volatile unsigned int IOPULL_CTR;
    volatile unsigned int COMM_DLOCK;
} smctr_t;

typedef struct {

    volatile unsigned int MSTR;
    volatile unsigned int STAT;
    volatile unsigned char RESERVED0[12];
    volatile unsigned int MRCTRL1;
    volatile unsigned char RESERVED1[12];
    volatile unsigned int DERATEINT;
    volatile unsigned char RESERVED2[8];
    volatile unsigned int PWRCTL;
    volatile unsigned char RESERVED3[28];
    volatile unsigned int RFSHCTL0;
    volatile unsigned char RESERVED4[12];
    volatile unsigned int RFSHCTL3;
    volatile unsigned char RESERVED5[108];
    volatile unsigned int INIT0;
    volatile unsigned int INIT1;
    volatile unsigned int INIT2;
    volatile unsigned int INIT3;
    volatile unsigned int INIT4;
    volatile unsigned int INIT5;
    volatile unsigned char RESERVED6[24];
    volatile unsigned int DRAMTMG0;
    volatile unsigned int DRAMTMG1;
    volatile unsigned int DRAMTMG2;
    volatile unsigned int DRAMTMG3;
    volatile unsigned int DRAMTMG4;
    volatile unsigned char RESERVED7[108];
    volatile unsigned int ZQCTL0;
    volatile unsigned char RESERVED8[12];
    volatile unsigned int DFITMG0;
    volatile unsigned int DFITMG1;
    volatile unsigned char RESERVED9[8];
    volatile unsigned int DFIUPD0;
    volatile unsigned int DFIUPD1;
    volatile unsigned char RESERVED10[8];
    volatile unsigned int DFIMISC;
    volatile unsigned char RESERVED11[76];
    volatile unsigned int ADDRMAP0;
    volatile unsigned int ADDRMAP1;
    volatile unsigned int ADDRMAP2;
    volatile unsigned int ADDRMAP3;
    volatile unsigned int ADDRMAP4;
    volatile unsigned int ADDRMAP5;
    volatile unsigned int ADDRMAP6;
    volatile unsigned char RESERVED12[628];
    volatile unsigned int PCTRL_0;
    volatile unsigned char RESERVED13[172];
    volatile unsigned int PCTRL1;
    volatile unsigned char RESERVED14[172];
    volatile unsigned int PCTRL2;
    volatile unsigned char RESERVED15[12];

} dwc_umctl2_regs_t;

typedef struct {
    volatile unsigned int MSTR;
    volatile unsigned int STAT;
    volatile unsigned char RESERVED0[12];
    volatile unsigned int MRCTRL1;
    volatile unsigned char RESERVED1[12];
    volatile unsigned int DERATEINT;
    volatile unsigned char RESERVED2[8];
    volatile unsigned int PWRCTL;
    volatile unsigned char RESERVED3[28];
    volatile unsigned int RFSHCTL0;
    volatile unsigned char RESERVED4[12];
    volatile unsigned int RFSHCTL3;
    volatile unsigned int RFSHTMG;
    volatile unsigned char RESERVED5[104];
    volatile unsigned int INIT0;
    volatile unsigned int INIT1;
    volatile unsigned int INIT2;
    volatile unsigned int INIT3;
    volatile unsigned int INIT4;
    volatile unsigned int INIT5;
    volatile unsigned char RESERVED6[24];
    volatile unsigned int DRAMTMG0;
    volatile unsigned int DRAMTMG1;
    volatile unsigned int DRAMTMG2;
    volatile unsigned int DRAMTMG3;
    volatile unsigned int DRAMTMG4;
    volatile unsigned char RESERVED7[108];
    volatile unsigned int ZQCTL0;
    volatile unsigned char RESERVED8[12];
    volatile unsigned int DFITMG0;
    volatile unsigned int DFITMG1;
    volatile unsigned char RESERVED9[8];
    volatile unsigned int DFIUPD0;
    volatile unsigned int DFIUPD1;
    volatile unsigned char RESERVED10[8];
    volatile unsigned int DFIMISC;
    volatile unsigned char RESERVED11[76];
    volatile unsigned int ADDRMAP0;
    volatile unsigned int ADDRMAP1;
    volatile unsigned int ADDRMAP2;
    volatile unsigned int ADDRMAP3;
    volatile unsigned int ADDRMAP4;
    volatile unsigned int ADDRMAP5;
    volatile unsigned int ADDRMAP6;
    volatile unsigned char RESERVED12[628];
    volatile unsigned int PCTRL_0;
    volatile unsigned char RESERVED13[172];
    volatile unsigned int PCTRL1;
    volatile unsigned char RESERVED14[172];
    volatile unsigned int PCTRL2;
    volatile unsigned char RESERVED15[12];
} ddrmc_t;

typedef struct {
    volatile unsigned int RIDR;
    volatile unsigned int PIR;
    volatile unsigned int PGCR;
    volatile unsigned int PGSR;
    volatile unsigned char RESERVED0[8];
    volatile unsigned int PTR0;
    volatile unsigned int PTR1;
    volatile unsigned int PTR2;
    volatile unsigned char RESERVED1[4];
    volatile unsigned int DXCCR;
    volatile unsigned int DSGCR;
    volatile unsigned int DCR;
    volatile unsigned int DTPR0;
    volatile unsigned int DTPR1;
    volatile unsigned int DTPR2;
    volatile unsigned int MR0;
    volatile unsigned int MR1_LPDDR2;
    volatile unsigned int MR2;
    volatile unsigned int MR3;
    volatile unsigned char RESERVED2[112];
    volatile unsigned int DCUAR; // 0x30
    volatile unsigned char RESERVED3[16];
    volatile unsigned int DCUTPR;
    volatile unsigned int DCUSR0;
    volatile unsigned int DCUSR1;
    volatile unsigned char RESERVED4[64];
    volatile unsigned int BISTUDPR; // 0x48
    volatile unsigned char RESERVED5[92];
    volatile unsigned int ZQ0CR0;
    volatile unsigned int ZQ0CR1;
    volatile unsigned int ZQ0SR0;
    volatile unsigned int ZQ0SR1;
    volatile unsigned int ZQ1CR0;
    volatile unsigned int ZQ1CR1;
    volatile unsigned int ZQ1SR0;
    volatile unsigned int ZQ1SR1;
    volatile unsigned int ZQ2CR0;
    volatile unsigned int ZQ2CR1;
    volatile unsigned int ZQ2SR0;
    volatile unsigned int ZQ2SR1;
    volatile unsigned int ZQ3CR0;
    volatile unsigned int ZQ3CR1;
    volatile unsigned int ZQ3SR0;
    volatile unsigned int ZQ3SR1;
    volatile unsigned int DX0GCR;
    volatile unsigned int DX0GSR0;
    volatile unsigned int DX0GSR1;
    volatile unsigned int DX0DLLCR;
    volatile unsigned int DX0DQTR;
    volatile unsigned int DX0DQSTR;
    volatile unsigned char RESERVED9[36];
    volatile unsigned int DX1GCR;
    volatile unsigned int DX1GSR0;
    volatile unsigned int DX1GSR1;
    volatile unsigned int DX1DLLCR;
    volatile unsigned int DX1DQTR;
    volatile unsigned int DX1DQSTR;
    volatile unsigned char RESERVED10[36];
    volatile unsigned int DX2GCR;
    volatile unsigned int DX2GSR0;
    volatile unsigned int DX2GSR1;
    volatile unsigned int DX2DLLCR;
    volatile unsigned int DX2DQTR;
    volatile unsigned int DX2DQSTR;
    volatile unsigned char RESERVED11[36];
    volatile unsigned int DX3GCR;
    volatile unsigned int DX3GSR0;
    volatile unsigned int DX3GSR1;
    volatile unsigned int DX3DLLCR;
    volatile unsigned int DX3DQTR;
    volatile unsigned int DX3DQSTR;
    volatile unsigned char RESERVED12[36];
    volatile unsigned int DX4GCR;
    volatile unsigned int DX4GSR0;
    volatile unsigned int DX4GSR1;
    volatile unsigned int DX4DLLCR;
    volatile unsigned int DX4DQTR;
    volatile unsigned int DX4DQSTR;
    volatile unsigned char RESERVED13[36];
    volatile unsigned int DX5GCR;
    volatile unsigned int DX5GSR0;
    volatile unsigned int DX5GSR1;
    volatile unsigned int DX5DLLCR;
    volatile unsigned int DX5DQTR;
    volatile unsigned int DX5DQSTR;
    volatile unsigned char RESERVED14[36];
    volatile unsigned int DX6GCR;
    volatile unsigned int DX6GSR0;
    volatile unsigned int DX6GSR1;
    volatile unsigned int DX6DLLCR;
    volatile unsigned int DX6DQTR;
    volatile unsigned int DX6DQSTR;
    volatile unsigned char RESERVED15[36];
    volatile unsigned int DX7GCR;
    volatile unsigned int DX7GSR0;
    volatile unsigned int DX7GSR1;
    volatile unsigned int DX7DLLCR;
    volatile unsigned int DX7DQTR;
    volatile unsigned int DX7DQSTR;
    volatile unsigned char RESERVED16[36];
    volatile unsigned int DX8GCR;
    volatile unsigned int DX8GSR0;
    volatile unsigned int DX8GSR1;
    volatile unsigned int DX8DLLCR;
    volatile unsigned int DX8DQTR;
    volatile unsigned int DX8DQSTR;
    volatile unsigned char RESERVED17[36];
} ddrphy_t;

typedef struct {
    ddrmc_t *DDRMC0;
    ddrphy_t *DDRPHY0;
    ddrmc_t *DDRMC1;
    ddrphy_t *DDRPHY1;
    cmctr_t *CMCTR;
    pmctr_t *PMCTR;
    smctr_t *SMCTR;
    unsigned int *DDR_REMAP;
    unsigned int BASE_ADR[2];
} sys_t;

#endif /* REGS_H */
