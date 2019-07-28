/*
 *  $Id$
 *  Register definitions for the general purpose IO pins (GPIO).
 *
 *  This header file was originally taken from DSP2833x_Gpio.h in the 
 *  2833x examples
 */

#ifndef TMS2833x_H_INCLUDED
#define TMS2833x_H_INCLUDED

#include <stdint.h>



#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t Uint16;
typedef uint32_t Uint32;

#define EALLOW do { asm(" eallow"); } while (0)
#define EDIS do { asm(" edis"); } while (0)


#if 1
	// OLD STYLE port definitions
	#define REG16(val) (*(volatile Uint16 *)(val))
	#define REG32(val) (*(volatile Uint32 *)(val))

	// GPIOA
	#define GPACTRL 		REG32(0x6F80)
	#define GPAQSEL1 		REG32(0x6F82)
	#define GPAQSEL2 		REG32(0x6F84)
	#define GPAMUX1 		REG32(0x6F86)
	#define GPAMUX2 		REG32(0x6F88)
	#define GPADIR 			REG32(0x6F8A)
	#define GPAPUD 			REG32(0x6F8C)
	// GPIOA - data register
	#define GPADAT 			REG32(0x6FC0)
	#define GPASET 			REG32(0x6FC2)
	#define GPACLEAR 		REG32(0x6FC4)
	#define GPATOGGLE 		REG32(0x6FC6)
	// GPIOB
	#define GPBCTRL 		REG32(0x6F90)
	#define GPBQSEL1 		REG32(0x6F92)
	#define GPBQSEL2 		REG32(0x6F94)
	#define GPBMUX1 		REG32(0x6F96)
	#define GPBMUX2 		REG32(0x6F98)
	#define GPBDIR 			REG32(0x6F9A)
	#define GPBPUD 			REG32(0x6F9C)
	// GPIOB - data register
	#define GPBDAT 			REG32(0x6FC8)
	#define GPBSET 			REG32(0x6FCA)
	#define GPBCLEAR 		REG32(0x6FCC)
	#define GPBTOGGLE 		REG32(0x6FCE)
	// GPIOC
	#define GPCMUX1 		REG32(0x6FA6)
	#define GPCMUX2 		REG32(0x6FA8)
	#define GPCDIR 			REG32(0x6FAA)
	#define GPCPUD 			REG32(0x6FAC)
	// GPIOC - data register
	#define GPCDAT 			REG32(0x6FD0)
	#define GPCSET 			REG32(0x6FD2)
	#define GPCCLEAR 		REG32(0x6FD4)
	#define GPCTOGGLE 		REG32(0x6FD6)

	// PLL
	#define PLLCR			REG16(0x7021)
	#define PLLSTS		 	REG16(0x7011)
	#define PLLSTS_DIVSEL_BITS      0x0180
	#define PLLSTS_DIVSEL_SHIFTBITS 7
	#define PLLSTS_MCLKOFF_BIT      0x0040
	#define PLLSTS_MCLKSTS_BIT      0x0008
	#define PLLSTS_PLLLOCKS_BIT     0x0001

	//  SCI-A
	#define	SCIACCR		REG16(0x7050) // SCI-A Communications Control Register
	#define	SCIACTL1	REG16(0x7051) // SCI-A Control Register 1
	#define	SCIAHBAUD	REG16(0x7052) // SCI-A Baud Register, High Bits
	#define	SCIALBAUD	REG16(0x7053) // SCI-A Baud Register, Low Bits
	#define	SCIACTL2	REG16(0x7054) // SCI-A Control Register 2
	#define	SCIARXST	REG16(0x7055) // SCI-A Receive Status Register
	#define	SCIARXEMU	REG16(0x7056) // SCI-A Receive Emulation Data Buffer Register
	#define	SCIARXBUF	REG16(0x7057) // SCI-A Receive Data Buffer Register
	#define	SCIATXBUF	REG16(0x7059) // SCI-A Transmit Data Buffer Register
	#define	SCIAFFTX	REG16(0x705A) // SCI-A FIFO Transmit Register
	#define	SCIAFFRX	REG16(0x705B) // SCI-A FIFO Receive Register
	#define	SCIAFFCT	REG16(0x705C) // SCI-A FIFO Control Register
	#define	SCIAPRI		REG16(0x705F) // SCI-A Priority Control Register

	//  SCI-B
	#define	SCIBCCR		REG16(0x7750) // SCI-B Communications Control Register
	#define	SCIBCTL1 	REG16(0x7751) // SCI-B Control Register 1
	#define	SCIBHBAUD 	REG16(0x7752) // SCI-B Baud Register, High Bits
	#define	SCIBLBAUD 	REG16(0x7753) // SCI-B Baud Register, Low Bits
	#define	SCIBCTL2 	REG16(0x7754) // SCI-B Control Register 2
	#define	SCIBRXST 	REG16(0x7755) // SCI-B Receive Status Register
	#define	SCIBRXEMU 	REG16(0x7756) // SCI-B Receive Emulation Data Buffer Register
	#define	SCIBRXBUF 	REG16(0x7757) // SCI-B Receive Data Buffer Register
	#define	SCIBTXBUF 	REG16(0x7759) // SCI-B Transmit Data Buffer Register
	#define	SCIBFFTX 	REG16(0x775A) // SCI-B FIFO Transmit Register
	#define	SCIBFFRX 	REG16(0x775B) // SCI-B FIFO Receive Register
	#define	SCIBFFCT 	REG16(0x775C) // SCI-B FIFO Control Register
	#define	SCIBPRI 	REG16(0x775F) // SCI-B Priority Control Register

	// PLL, Clocking, Watchdog, and Low-Power Mode Registers
	#define	PLLSTS 		REG16(0x7011)	// 1 PLL Status Register Figure 24
	#define	HISPCP 		REG16(0x701A)	// 1 High-Speed Peripheral Clock (HSPCLK) Prescaler Register Figure 17
	#define	LOSPCP 		REG16(0x701B)	// 1 Low-Speed Peripheral Clock (LSPCLK) Prescaler Register Figure 18
	#define	PCLKCR0 	REG16(0x701C)	// 1 Peripheral Clock Control Register 0 Figure 14
	#define	PCLKCR1 	REG16(0x701D)	// 1 Peripheral Clock Control Register 1 Figure 15
	#define	LPMCR0 		REG16(0x701E)	// 1 Low Power Mode Control Register 0 Figure 18
	#define	PCLKCR3 	REG16(0x7020)	// 1 Peripheral Clock Control Register 3 Figure 16
	#define	PLLCR 		REG16(0x7021)	// 1 PLL Control Register Figure 23
	#define	SCSR 		REG16(0x7022)	// 1 System Control & Status Register Figure 27
	#define	WDCNTR 		REG16(0x7023)	// 1 Watchdog Counter Register. Figure 28
	#define	WDKEY 		REG16(0x7025)	// 1 Watchdog Reset Key Register Figure 29
	#define	WDCR 		REG16(0x7029)	// 1 Watchdog Control Register Figure 30#else

	// EALLOW-Protected Flash/OTP Configuration Registers
	#define FOPT 			REG16(0x0A80)	// Flash Option Register
	#define FPWR			REG16(0x0A82)	// Flash Power Modes Register
	#define FSTATUS			REG16(0x0A83)	// Status Register
	#define FSTDBYWAIT		REG16(0x0A84)	// Flash Sleep To Standby Wait State Register
	#define FACTIVEWAIT 	REG16(0x0A85)	// Flash Standby To Active Wait State Register
	#define FBANKWAIT 		REG16(0x0A86)	// Flash Read Access Wait State Register
	#define FOTPWAIT 		REG16(0x0A87)	// OTP Read Access Wait State Register

#else

/* GPIO A control register bit definitions */
struct GPACTRL_BITS {        /* bits   description */
   Uint16 QUALPRD0:8;        /* 7:0    Qual period */
   Uint16 QUALPRD1:8;        /* 15:8   Qual period */
   Uint16 QUALPRD2:8;        /* 23:16  Qual period */
   Uint16 QUALPRD3:8;        /* 31:24  Qual period */
};

union GPACTRL_REG {
   Uint32              all;
   struct GPACTRL_BITS bit;
};

/* GPIO B control register bit definitions */
struct GPBCTRL_BITS {        /* bits   description */
   Uint16 QUALPRD0:8;        /* 7:0    Qual period */
   Uint16 QUALPRD1:8;        /* 15:8   Qual period */
   Uint16 QUALPRD2:8;        /* 23:16  Qual period */
   Uint16 QUALPRD3:8;        /* 31:24              */
};

union GPBCTRL_REG {
   Uint32              all;
   struct GPBCTRL_BITS bit;
};

/* GPIO A Qual/MUX select register bit definitions */
struct GPA1_BITS {            /* bits   description */
   Uint16 GPIO0:2;            /* 1:0    GPIO0 */
   Uint16 GPIO1:2;            /* 3:2    GPIO1 */
   Uint16 GPIO2:2;            /* 5:4    GPIO2 */
   Uint16 GPIO3:2;            /* 7:6    GPIO3 */
   Uint16 GPIO4:2;            /* 9:8    GPIO4 */
   Uint16 GPIO5:2;            /* 11:10  GPIO5 */
   Uint16 GPIO6:2;            /* 13:12  GPIO6 */
   Uint16 GPIO7:2;            /* 15:14  GPIO7 */
   Uint16 GPIO8:2;            /* 17:16  GPIO8 */
   Uint16 GPIO9:2;            /* 19:18  GPIO9 */
   Uint16 GPIO10:2;           /* 21:20  GPIO10 */
   Uint16 GPIO11:2;           /* 23:22  GPIO11 */
   Uint16 GPIO12:2;           /* 25:24  GPIO12 */
   Uint16 GPIO13:2;           /* 27:26  GPIO13 */
   Uint16 GPIO14:2;           /* 29:28  GPIO14 */
   Uint16 GPIO15:2;           /* 31:30  GPIO15 */
};

struct GPA2_BITS {            /* bits   description */
   Uint16 GPIO16:2;           /* 1:0    GPIO16 */
   Uint16 GPIO17:2;           /* 3:2    GPIO17 */
   Uint16 GPIO18:2;           /* 5:4    GPIO18 */
   Uint16 GPIO19:2;           /* 7:6    GPIO19 */
   Uint16 GPIO20:2;           /* 9:8    GPIO20 */
   Uint16 GPIO21:2;           /* 11:10  GPIO21 */
   Uint16 GPIO22:2;           /* 13:12  GPIO22 */
   Uint16 GPIO23:2;           /* 15:14  GPIO23 */
   Uint16 GPIO24:2;           /* 17:16  GPIO24 */
   Uint16 GPIO25:2;           /* 19:18  GPIO25 */
   Uint16 GPIO26:2;           /* 21:20  GPIO26 */
   Uint16 GPIO27:2;           /* 23:22  GPIO27 */
   Uint16 GPIO28:2;           /* 25:24  GPIO28 */
   Uint16 GPIO29:2;           /* 27:26  GPIO29 */
   Uint16 GPIO30:2;           /* 29:28  GPIO30 */
   Uint16 GPIO31:2;           /* 31:30  GPIO31 */
};

struct GPB1_BITS {            /* bits   description */
   Uint16 GPIO32:2;           /* 1:0    GPIO32 */
   Uint16 GPIO33:2;           /* 3:2    GPIO33 */
   Uint16 GPIO34:2;           /* 5:4    GPIO34 */
   Uint16 GPIO35:2;           /* 7:6    GPIO35 */
   Uint16 GPIO36:2;           /* 9:8    GPIO36 */
   Uint16 GPIO37:2;           /* 11:10  GPIO37 */
   Uint16 GPIO38:2;           /* 13:12  GPIO38 */
   Uint16 GPIO39:2;           /* 15:14  GPIO39 */
   Uint16 GPIO40:2;           /* 17:16  GPIO40 */
   Uint16 GPIO41:2;           /* 19:16  GPIO41 */
   Uint16 GPIO42:2;           /* 21:20  GPIO42 */
   Uint16 GPIO43:2;           /* 23:22  GPIO43 */
   Uint16 GPIO44:2;           /* 25:24  GPIO44 */
   Uint16 GPIO45:2;           /* 27:26  GPIO45 */
   Uint16 GPIO46:2;           /* 29:28  GPIO46 */
   Uint16 GPIO47:2;           /* 31:30  GPIO47 */
};                                        

struct GPB2_BITS  {           /* bits   description */
   Uint16 GPIO48:2;           /* 1:0    GPIO48 */
   Uint16 GPIO49:2;           /* 3:2    GPIO49 */
   Uint16 GPIO50:2;           /* 5:4    GPIO50 */
   Uint16 GPIO51:2;           /* 7:6    GPIO51 */
   Uint16 GPIO52:2;           /* 9:8    GPIO52 */
   Uint16 GPIO53:2;           /* 11:10  GPIO53 */
   Uint16 GPIO54:2;           /* 13:12  GPIO54 */
   Uint16 GPIO55:2;           /* 15:14  GPIO55 */
   Uint16 GPIO56:2;           /* 17:16  GPIO56 */
   Uint16 GPIO57:2;           /* 19:18  GPIO57 */
   Uint16 GPIO58:2;           /* 21:20  GPIO58 */
   Uint16 GPIO59:2;           /* 23:22  GPIO59 */
   Uint16 GPIO60:2;           /* 25:24  GPIO60 */
   Uint16 GPIO61:2;           /* 27:26  GPIO61 */
   Uint16 GPIO62:2;           /* 29:28  GPIO62 */
   Uint16 GPIO63:2;           /* 31:30  GPIO63 */
};                                        

struct GPC1_BITS {            /* bits   description */
   Uint16 GPIO64:2;           /* 1:0    GPIO64 */
   Uint16 GPIO65:2;           /* 3:2    GPIO65 */
   Uint16 GPIO66:2;           /* 5:4    GPIO66 */
   Uint16 GPIO67:2;           /* 7:6    GPIO67 */
   Uint16 GPIO68:2;           /* 9:8    GPIO68 */
   Uint16 GPIO69:2;           /* 11:10  GPIO69 */
   Uint16 GPIO70:2;           /* 13:12  GPIO70 */
   Uint16 GPIO71:2;           /* 15:14  GPIO71 */
   Uint16 GPIO72:2;           /* 17:16  GPIO72 */
   Uint16 GPIO73:2;           /* 19:18  GPIO73 */
   Uint16 GPIO74:2;           /* 21:20  GPIO74 */
   Uint16 GPIO75:2;           /* 23:22  GPIO75 */
   Uint16 GPIO76:2;           /* 25:24  GPIO76 */
   Uint16 GPIO77:2;           /* 27:26  GPIO77 */
   Uint16 GPIO78:2;           /* 29:28  GPIO78 */
   Uint16 GPIO79:2;           /* 31:30  GPIO79 */
};                                        


struct GPC2_BITS {            /* bits   description */
   Uint16 GPIO80:2;           /* 1:0    GPIO80 */
   Uint16 GPIO81:2;           /* 3:2    GPIO81 */
   Uint16 GPIO82:2;           /* 5:4    GPIO82 */
   Uint16 GPIO83:2;           /* 7:6    GPIO83 */
   Uint16 GPIO84:2;           /* 9:8    GPIO84 */
   Uint16 GPIO85:2;           /* 11:10  GPIO85 */
   Uint16 GPIO86:2;           /* 13:12  GPIO86 */
   Uint16 GPIO87:2;           /* 15:14  GPIO87 */
   Uint16 rsvd:16;            /* 31:16  reserved */
};                                        


union GPA1_REG {
   Uint32              all;
   struct GPA1_BITS    bit;
};

union GPA2_REG {
   Uint32              all;
   struct GPA2_BITS    bit;
};

union GPB1_REG {
   Uint32              all;
   struct GPB1_BITS    bit;
};

union GPB2_REG {
   Uint32              all;
   struct GPB2_BITS    bit;
};

union GPC1_REG {
   Uint32              all;
   struct GPC1_BITS    bit;
};

union GPC2_REG {
   Uint32              all;
   struct GPC2_BITS    bit;
};

/* GPIO A DIR/TOGGLE/SET/CLEAR register bit definitions */
struct GPADAT_BITS {          /* bits   description */
   Uint16 GPIO0:1;            /* 0      GPIO0 */
   Uint16 GPIO1:1;            /* 1      GPIO1 */
   Uint16 GPIO2:1;            /* 2      GPIO2 */
   Uint16 GPIO3:1;            /* 3      GPIO3 */
   Uint16 GPIO4:1;            /* 4      GPIO4 */
   Uint16 GPIO5:1;            /* 5      GPIO5 */
   Uint16 GPIO6:1;            /* 6      GPIO6 */
   Uint16 GPIO7:1;            /* 7      GPIO7 */
   Uint16 GPIO8:1;            /* 8      GPIO8 */
   Uint16 GPIO9:1;            /* 9      GPIO9 */
   Uint16 GPIO10:1;           /* 10     GPIO10 */
   Uint16 GPIO11:1;           /* 11     GPIO11 */
   Uint16 GPIO12:1;           /* 12     GPIO12 */
   Uint16 GPIO13:1;           /* 13     GPIO13 */
   Uint16 GPIO14:1;           /* 14     GPIO14 */
   Uint16 GPIO15:1;           /* 15     GPIO15 */
   Uint16 GPIO16:1;           /* 16     GPIO16 */
   Uint16 GPIO17:1;           /* 17     GPIO17 */
   Uint16 GPIO18:1;           /* 18     GPIO18 */
   Uint16 GPIO19:1;           /* 19     GPIO19 */
   Uint16 GPIO20:1;           /* 20     GPIO20 */
   Uint16 GPIO21:1;           /* 21     GPIO21 */
   Uint16 GPIO22:1;           /* 22     GPIO22 */
   Uint16 GPIO23:1;           /* 23     GPIO23 */
   Uint16 GPIO24:1;           /* 24     GPIO24 */
   Uint16 GPIO25:1;           /* 25     GPIO25 */
   Uint16 GPIO26:1;           /* 26     GPIO26 */
   Uint16 GPIO27:1;           /* 27     GPIO27 */
   Uint16 GPIO28:1;           /* 28     GPIO28 */
   Uint16 GPIO29:1;           /* 29     GPIO29 */
   Uint16 GPIO30:1;           /* 30     GPIO30 */
   Uint16 GPIO31:1;           /* 31     GPIO31 */
};
                                         
struct GPBDAT_BITS {          /* bits   description */
   Uint16 GPIO32:1;           /* 0      GPIO32 */
   Uint16 GPIO33:1;           /* 1      GPIO33 */
   Uint16 GPIO34:1;           /* 2      GPIO34 */
   Uint16 GPIO35:1;           /* 3      GPIO35 */
   Uint16 GPIO36:1;           /* 4      GPIO36 */
   Uint16 GPIO37:1;           /* 5      GPIO37 */
   Uint16 GPIO38:1;           /* 6      GPIO38 */
   Uint16 GPIO39:1;           /* 7      GPIO39 */
   Uint16 GPIO40:1;           /* 8      GPIO40 */
   Uint16 GPIO41:1;           /* 9      GPIO41 */
   Uint16 GPIO42:1;           /* 10     GPIO42 */
   Uint16 GPIO43:1;           /* 11     GPIO43 */
   Uint16 GPIO44:1;           /* 12     GPIO44 */
   Uint16 GPIO45:1;           /* 13     GPIO45 */
   Uint16 GPIO46:1;           /* 14     GPIO46 */
   Uint16 GPIO47:1;           /* 15     GPIO47 */
   Uint16 GPIO48:1;           /* 16     GPIO48 */
   Uint16 GPIO49:1;           /* 17     GPIO49 */
   Uint16 GPIO50:1;           /* 18     GPIO50 */
   Uint16 GPIO51:1;           /* 19     GPIO51 */
   Uint16 GPIO52:1;           /* 20     GPIO52 */
   Uint16 GPIO53:1;           /* 21     GPIO53 */
   Uint16 GPIO54:1;           /* 22     GPIO54 */
   Uint16 GPIO55:1;           /* 23     GPIO55 */
   Uint16 GPIO56:1;           /* 24     GPIO56 */
   Uint16 GPIO57:1;           /* 25     GPIO57 */
   Uint16 GPIO58:1;           /* 26     GPIO58 */
   Uint16 GPIO59:1;           /* 27     GPIO59 */
   Uint16 GPIO60:1;           /* 28     GPIO60 */
   Uint16 GPIO61:1;           /* 29     GPIO61 */
   Uint16 GPIO62:1;           /* 30     GPIO62 */
   Uint16 GPIO63:1;           /* 31     GPIO63 */
};
                                         
struct GPCDAT_BITS {          /* bits   description */
   Uint16 GPIO64:1;           /* 0      GPIO64 */
   Uint16 GPIO65:1;           /* 1      GPIO65 */
   Uint16 GPIO66:1;           /* 2      GPIO66 */
   Uint16 GPIO67:1;           /* 3      GPIO67 */
   Uint16 GPIO68:1;           /* 4      GPIO68 */
   Uint16 GPIO69:1;           /* 5      GPIO69 */
   Uint16 GPIO70:1;           /* 6      GPIO70 */
   Uint16 GPIO71:1;           /* 7      GPIO71 */
   Uint16 GPIO72:1;           /* 8      GPIO72 */
   Uint16 GPIO73:1;           /* 9      GPIO73 */
   Uint16 GPIO74:1;           /* 10     GPIO74 */
   Uint16 GPIO75:1;           /* 11     GPIO75 */
   Uint16 GPIO76:1;           /* 12     GPIO76 */
   Uint16 GPIO77:1;           /* 13     GPIO77 */
   Uint16 GPIO78:1;           /* 14     GPIO78 */
   Uint16 GPIO79:1;           /* 15     GPIO79 */
   Uint16 GPIO80:1;           /* 16     GPIO80 */
   Uint16 GPIO81:1;           /* 17     GPIO81 */
   Uint16 GPIO82:1;           /* 18     GPIO82 */
   Uint16 GPIO83:1;           /* 19     GPIO83 */
   Uint16 GPIO84:1;           /* 20     GPIO84 */
   Uint16 GPIO85:1;           /* 21     GPIO85 */
   Uint16 GPIO86:1;           /* 22     GPIO86 */
   Uint16 GPIO87:1;           /* 23     GPIO87 */
   Uint16 rsvd1:8;            /* 31:24  reserved */
};

union GPADAT_REG {
   Uint32              all;
   struct GPADAT_BITS  bit;
};                                            

union GPBDAT_REG {
   Uint32              all;
   struct GPBDAT_BITS  bit;
};

union GPCDAT_REG {
   Uint32              all;
   struct GPCDAT_BITS  bit;
};

/* GPIO Xint1/XINT2/XNMI select register bit definitions */
struct GPIOXINT_BITS {        /* bits   description */
    Uint16 GPIOSEL:5;         /* 4:0    Select GPIO interrupt input source */
    Uint16 rsvd1:11;          /* 15:5   reserved */
};

union GPIOXINT_REG {
   Uint16                all;
   struct GPIOXINT_BITS  bit;
};


struct GPIO_CTRL_REGS {
   union  GPACTRL_REG  GPACTRL;   /* GPIO A Control Register (GPIO0 to 31) */
   union  GPA1_REG     GPAQSEL1;  /* GPIO A Qualifier Select 1 Register (GPIO0 to 15) */
   union  GPA2_REG     GPAQSEL2;  /* GPIO A Qualifier Select 2 Register (GPIO16 to 31) */
   union  GPA1_REG     GPAMUX1;   /* GPIO A Mux 1 Register (GPIO0 to 15) */
   union  GPA2_REG     GPAMUX2;   /* GPIO A Mux 2 Register (GPIO16 to 31) */
   union  GPADAT_REG   GPADIR;    /* GPIO A Direction Register (GPIO0 to 31) */
   union  GPADAT_REG   GPAPUD;    /* GPIO A Pull Up Disable Register (GPIO0 to 31) */
   Uint32              rsvd1;
   union  GPBCTRL_REG  GPBCTRL;   /* GPIO B Control Register (GPIO32 to 63) */
   union  GPB1_REG     GPBQSEL1;  /* GPIO B Qualifier Select 1 Register (GPIO32 to 47) */
   union  GPB2_REG     GPBQSEL2;  /* GPIO B Qualifier Select 2 Register (GPIO48 to 63) */
   union  GPB1_REG     GPBMUX1;   /* GPIO B Mux 1 Register (GPIO32 to 47) */
   union  GPB2_REG     GPBMUX2;   /* GPIO B Mux 2 Register (GPIO48 to 63) */
   union  GPBDAT_REG   GPBDIR;    /* GPIO B Direction Register (GPIO32 to 63) */
   union  GPBDAT_REG   GPBPUD;    /* GPIO B Pull Up Disable Register (GPIO32 to 63) */
   Uint16              rsvd2[8];
   union  GPC1_REG     GPCMUX1;   /* GPIO C Mux 1 Register (GPIO64 to 79) */
   union  GPC2_REG     GPCMUX2;   /* GPIO C Mux 2 Register (GPIO80 to 95) */
   union  GPCDAT_REG   GPCDIR;    /* GPIO C Direction Register (GPIO64 to 95) */
   union  GPCDAT_REG   GPCPUD;    /* GPIO C Pull Up Disable Register (GPIO64 to 95) */
};

struct GPIO_DATA_REGS {
   union  GPADAT_REG       GPADAT;       /* GPIO Data Register (GPIO0 to 31) */
   union  GPADAT_REG       GPASET;       /* GPIO Data Set Register (GPIO0 to 31) */
   union  GPADAT_REG       GPACLEAR;     /* GPIO Data Clear Register (GPIO0 to 31) */
   union  GPADAT_REG       GPATOGGLE;    /* GPIO Data Toggle Register (GPIO0 to 31) */
   union  GPBDAT_REG       GPBDAT;       /* GPIO Data Register (GPIO32 to 63) */
   union  GPBDAT_REG       GPBSET;       /* GPIO Data Set Register (GPIO32 to 63) */
   union  GPBDAT_REG       GPBCLEAR;     /* GPIO Data Clear Register (GPIO32 to 63) */
   union  GPBDAT_REG       GPBTOGGLE;    /* GPIO Data Toggle Register (GPIO32 to 63) */
   union  GPCDAT_REG       GPCDAT;       /* GPIO Data Register (GPIO64 to 95) */
   union  GPCDAT_REG       GPCSET;       /* GPIO Data Set Register (GPIO64 to 95) */
   union  GPCDAT_REG       GPCCLEAR;     /* GPIO Data Clear Register (GPIO64 to 95) */
   union  GPCDAT_REG       GPCTOGGLE;    /* GPIO Data Toggle Register (GPIO64 to 95) */
   Uint16                  rsvd1[8];
};

struct GPIO_INT_REGS {
   union  GPIOXINT_REG     GPIOXINT1SEL; /* XINT1 GPIO Input Selection */
   union  GPIOXINT_REG     GPIOXINT2SEL; /* XINT2 GPIO Input Selection */
   union  GPIOXINT_REG     GPIOXNMISEL;  /* XNMI_Xint13 GPIO Input Selection */
   union  GPIOXINT_REG     GPIOXINT3SEL; /* XINT3 GPIO Input Selection */
   union  GPIOXINT_REG     GPIOXINT4SEL; /* XINT4 GPIO Input Selection */
   union  GPIOXINT_REG     GPIOXINT5SEL; /* XINT5 GPIO Input Selection */
   union  GPIOXINT_REG     GPIOXINT6SEL; /* XINT6 GPIO Input Selection */
   union  GPIOXINT_REG     GPIOXINT7SEL; /* XINT7 GPIO Input Selection */
   union  GPADAT_REG       GPIOLPMSEL;   /* Low power modes GP I/O input select */
};

//---------------------------------------------------------------------------
// System Control Individual Register Bit Definitions:
//


// PLL Status Register
struct PLLSTS_BITS   {    // bits  description
   Uint16 PLLLOCKS:1;     // 0     PLL lock status
   Uint16 rsvd1:1;        // 1     reserved
   Uint16 PLLOFF:1;       // 2     PLL off bit
   Uint16 MCLKSTS:1;      // 3     Missing clock status bit
   Uint16 MCLKCLR:1;      // 4     Missing clock clear bit
   Uint16 OSCOFF:1;       // 5     Oscillator clock off
   Uint16 MCLKOFF:1;      // 6     Missing clock detect
   Uint16 DIVSEL:2;       // 7     Divide Select
   Uint16 rsvd2:7;        // 15:7  reserved
};

union PLLSTS_REG {
   Uint16              all;
   struct PLLSTS_BITS  bit;
};

// High speed peripheral clock register bit definitions:
struct HISPCP_BITS  {   // bits  description
   Uint16 HSPCLK:3;     // 2:0   Rate relative to SYSCLKOUT
   Uint16 rsvd1:13;     // 15:3  reserved
};

union HISPCP_REG {
   Uint16              all;
   struct HISPCP_BITS  bit;
};

// Low speed peripheral clock register bit definitions:
struct LOSPCP_BITS  {   // bits  description
   Uint16 LSPCLK:3;     // 2:0   Rate relative to SYSCLKOUT
   Uint16 rsvd1:13;     // 15:3  reserved
};

union LOSPCP_REG {
   Uint16              all;
   struct LOSPCP_BITS  bit;
};

// Peripheral clock control register 0 bit definitions:
struct PCLKCR0_BITS  {   // bits  description
   Uint16 rsvd1:2;       // 1:0   reserved
   Uint16 TBCLKSYNC:1;   // 2     EWPM Module TBCLK enable/sync
   Uint16 ADCENCLK:1;    // 3     Enable high speed clk to ADC
   Uint16 I2CAENCLK:1;   // 4     Enable SYSCLKOUT to I2C-A
   Uint16 SCICENCLK:1;   // 5     Enalbe low speed clk to SCI-C
   Uint16 rsvd2:2;       // 7:6   reserved
   Uint16 SPIAENCLK:1;   // 8     Enable low speed clk to SPI-A
   Uint16 rsvd3:1;       // 9     reserved
   Uint16 SCIAENCLK:1;   // 10    Enable low speed clk to SCI-A
   Uint16 SCIBENCLK:1;   // 11    Enable low speed clk to SCI-B
   Uint16 MCBSPAENCLK:1; // 12    Enable low speed clk to McBSP-A
   Uint16 MCBSPBENCLK:1; // 13    Enable low speed clk to McBSP-B
   Uint16 ECANAENCLK:1;  // 14    Enable system clk to eCAN-A
   Uint16 ECANBENCLK:1;  // 15    Enable system clk to eCAN-B
};

union PCLKCR0_REG {
   Uint16              all;
   struct PCLKCR0_BITS bit;
};

// Peripheral clock control register 1 bit definitions:
struct PCLKCR1_BITS  {    // bits  description
   Uint16 EPWM1ENCLK:1;   // 0     Enable SYSCLKOUT to EPWM1
   Uint16 EPWM2ENCLK:1;   // 1     Enable SYSCLKOUT to EPWM2
   Uint16 EPWM3ENCLK:1;   // 2     Enable SYSCLKOUT to EPWM3
   Uint16 EPWM4ENCLK:1;   // 3     Enable SYSCLKOUT to EPWM4
   Uint16 EPWM5ENCLK:1;   // 4     Enable SYSCLKOUT to EPWM5
   Uint16 EPWM6ENCLK:1;   // 5     Enable SYSCLKOUT to EPWM6
   Uint16 rsvd1:2;        // 7:6   reserved
   Uint16 ECAP1ENCLK:1;   // 8     Enable SYSCLKOUT to ECAP1
   Uint16 ECAP2ENCLK:1;   // 9     Enable SYSCLKOUT to ECAP2
   Uint16 ECAP3ENCLK:1;   // 10    Enable SYSCLKOUT to ECAP3
   Uint16 ECAP4ENCLK:1;   // 11    Enable SYSCLKOUT to ECAP4
   Uint16 ECAP5ENCLK:1;   // 12    Enable SYSCLKOUT to ECAP5
   Uint16 ECAP6ENCLK:1;   // 13    Enable SYSCLKOUT to ECAP6
   Uint16 EQEP1ENCLK:1;   // 14    Enable SYSCLKOUT to EQEP1
   Uint16 EQEP2ENCLK:1;   // 15    Enable SYSCLKOUT to EQEP2
};

union PCLKCR1_REG {
   Uint16              all;
   struct PCLKCR1_BITS bit;
};


// Peripheral clock control register 2 bit definitions:
struct PCLKCR3_BITS  {        // bits  description
   Uint16 rsvd1:8;            // 7:0   reserved
   Uint16 CPUTIMER0ENCLK:1;   // 8     Enable SYSCLKOUT to CPU-Timer 0
   Uint16 CPUTIMER1ENCLK:1;   // 9     Enable SYSCLKOUT to CPU-Timer 1
   Uint16 CPUTIMER2ENCLK:1;   // 10    Enable SYSCLKOUT to CPU-Timer 2
   Uint16 DMAENCLK:1;         // 11    Enable the DMA clock
   Uint16 XINTFENCLK:1;       // 12    Enable SYSCLKOUT to XINTF
   Uint16 GPIOINENCLK:1;      //     Enable GPIO input clock
   Uint16 rsvd2:2;            // 15:14 reserved
};

union PCLKCR3_REG {
   Uint16              all;
   struct PCLKCR3_BITS bit;
};



// PLL control register bit definitions:
struct PLLCR_BITS {      // bits  description
   Uint16 DIV:4;         // 3:0   Set clock ratio for the PLL
   Uint16 rsvd1:12;      // 15:4  reserved
};

union PLLCR_REG {
   Uint16             all;
   struct PLLCR_BITS  bit;
};

// Low Power Mode 0 control register bit definitions:
struct LPMCR0_BITS {     // bits  description
   Uint16 LPM:2;         // 1:0   Set the low power mode
   Uint16 QUALSTDBY:6;   // 7:2   Qualification
   Uint16 rsvd1:7;       // 14:8  reserved
   Uint16 WDINTE:1;      // 15    Enables WD to wake the device from STANDBY
};

union LPMCR0_REG {
   Uint16              all;
   struct LPMCR0_BITS  bit;
};

//---------------------------------------------------------------------------
// System Control Register File:
//
struct SYS_CTRL_REGS {
   Uint16              rsvd7;     // 0
   union   PLLSTS_REG  PLLSTS;    // 1
   Uint16              rsvd1[8];  // 2-9
   union   HISPCP_REG  HISPCP;    // 10: High-speed peripheral clock pre-scaler
   union   LOSPCP_REG  LOSPCP;    // 11: Low-speed peripheral clock pre-scaler
   union   PCLKCR0_REG PCLKCR0;   // 12: Peripheral clock control register
   union   PCLKCR1_REG PCLKCR1;   // 13: Peripheral clock control register
   union   LPMCR0_REG  LPMCR0;    // 14: Low-power mode control register 0
   Uint16              rsvd2;     // 15: reserved
   union   PCLKCR3_REG PCLKCR3;   // 16: Peripheral clock control register
   union   PLLCR_REG   PLLCR;     // 17: PLL control register
   // No bit definitions are defined for SCSR because
   // a read-modify-write instruction can clear the WDOVERRIDE bit
   Uint16              SCSR;      // 18: System control and status register
   Uint16              WDCNTR;    // 19: WD counter register
   Uint16              rsvd4;     // 20
   Uint16              WDKEY;     // 21: WD reset key register
   Uint16              rsvd5[3];  // 22-24
   // No bit definitions are defined for WDCR because
   // the proper value must be written to the WDCHK field
   // whenever writing to this register.
   Uint16              WDCR;      // 25: WD timer control register
   Uint16              rsvd6[6];  // 26-31
};


/* --------------------------------------------------- */
/* CSM Registers                                       */
/*                                                     */
/* ----------------------------------------------------*/

/* CSM Status & Control register bit definitions */
struct  CSMSCR_BITS {      // bit   description
   Uint16     SECURE:1;    // 0     Secure flag
   Uint16     rsvd1:14;    // 14-1  reserved
   Uint16     FORCESEC:1;  // 15    Force Secure control bit

};

/* Allow access to the bit fields or entire register */
union CSMSCR_REG {
   Uint16             all;
   struct CSMSCR_BITS bit;
};

/* CSM Register File */
struct  CSM_REGS {
   Uint16           KEY0;    // KEY reg bits 15-0
   Uint16           KEY1;    // KEY reg bits 31-16
   Uint16           KEY2;    // KEY reg bits 47-32
   Uint16           KEY3;    // KEY reg bits 63-48
   Uint16           KEY4;    // KEY reg bits 79-64
   Uint16           KEY5;    // KEY reg bits 95-80
   Uint16           KEY6;    // KEY reg bits 111-96
   Uint16           KEY7;    // KEY reg bits 127-112
   Uint16           rsvd1;   // reserved
   Uint16           rsvd2;   // reserved
   Uint16           rsvd3;   // reserved
   Uint16           rsvd4;   // reserved
   Uint16           rsvd5;   // reserved
   Uint16           rsvd6;   // reserved
   Uint16           rsvd7;   // reserved
   union CSMSCR_REG CSMSCR;  // CSM Status & Control register
};

/* Password locations */
struct  CSM_PWL {
   Uint16   PSWD0;  // PSWD bits 15-0
   Uint16   PSWD1;  // PSWD bits 31-16
   Uint16   PSWD2;  // PSWD bits 47-32
   Uint16   PSWD3;  // PSWD bits 63-48
   Uint16   PSWD4;  // PSWD bits 79-64
   Uint16   PSWD5;  // PSWD bits 95-80
   Uint16   PSWD6;  // PSWD bits 111-96
   Uint16   PSWD7;  // PSWD bits 127-112
};



/* Flash Registers */

#define FLASH_SLEEP   0x0000;
#define FLASH_STANDBY 0x0001;
#define FLASH_ACTIVE  0x0003;


/* Flash Option Register bit definitions */
struct  FOPT_BITS {       // bit   description
   Uint16     ENPIPE:1;   // 0     Enable Pipeline Mode
   Uint16     rsvd:15;    // 1-15  reserved
};

/* Allow access to the bit fields or entire register */
union FOPT_REG {
   Uint16           all;
   struct FOPT_BITS bit;
};

/* Flash Power Modes Register bit definitions */
struct  FPWR_BITS {       // bit   description
   Uint16     PWR:2;      // 0-1   Power Mode bits
   Uint16     rsvd:14;    // 2-15  reserved
};

/* Allow access to the bit fields or entire register */
union FPWR_REG {
   Uint16           all;
   struct FPWR_BITS bit;
};


/* Flash Status Register bit definitions */
struct  FSTATUS_BITS {       // bit   description
   Uint16     PWRS:2;        // 0-1   Power Mode Status bits
   Uint16     STDBYWAITS:1;  // 2     Bank/Pump Sleep to Standby Wait Counter Status bits
   Uint16     ACTIVEWAITS:1; // 3     Bank/Pump Standby to Active Wait Counter Status bits
   Uint16     rsvd1:4;       // 4-7   reserved
   Uint16     V3STAT:1;      // 8     VDD3V Status Latch bit
   Uint16     rsvd2:7;       // 9-15  reserved
};

/* Allow access to the bit fields or entire register */
union FSTATUS_REG {
   Uint16              all;
   struct FSTATUS_BITS bit;
};

/* Flash Sleep to Standby Wait Counter Register bit definitions */
struct  FSTDBYWAIT_BITS {    // bit   description
   Uint16     STDBYWAIT:9;   // 0-8   Bank/Pump Sleep to Standby Wait Count bits
   Uint16     rsvd:7;        // 9-15  reserved
};

/* Allow access to the bit fields or entire register */
union FSTDBYWAIT_REG {
   Uint16                 all;
   struct FSTDBYWAIT_BITS bit;
};

/* Flash Standby to Active Wait Counter Register bit definitions */
struct  FACTIVEWAIT_BITS {   // bit   description
   Uint16     ACTIVEWAIT:9;  // 0-8   Bank/Pump Standby to Active Wait Count bits
   Uint16     rsvd:7;        // 9-15  reserved
};

/* Allow access to the bit fields or entire register */
union FACTIVEWAIT_REG {
   Uint16                  all;
   struct FACTIVEWAIT_BITS bit;
};

/* Bank Read Access Wait State Register bit definitions */
struct  FBANKWAIT_BITS {     // bit   description
   Uint16     RANDWAIT:4;    // 0-3   Flash Random Read Wait State bits
   Uint16     rsvd1:4;       // 4-7   reserved
   Uint16     PAGEWAIT:4;    // 8-11  Flash Paged Read Wait State bits
   Uint16     rsvd2:4;       // 12-15 reserved
};

/* Allow access to the bit fields or entire register */
union FBANKWAIT_REG {
   Uint16                all;
   struct FBANKWAIT_BITS bit;
};

/* OTP Read Access Wait State Register bit definitions */
struct  FOTPWAIT_BITS {      // bit   description
   Uint16     OTPWAIT:5;     // 0-4   OTP Read Wait State bits
   Uint16     rsvd:11;       // 5-15  reserved
};

/* Allow access to the bit fields or entire register */
union FOTPWAIT_REG {
   Uint16               all;
   struct FOTPWAIT_BITS bit;
};


struct FLASH_REGS {
   union FOPT_REG        FOPT;        // Option Register
   Uint16                rsvd1;       // reserved
   union FPWR_REG        FPWR;        // Power Modes Register
   union FSTATUS_REG     FSTATUS;     // Status Register
   union FSTDBYWAIT_REG  FSTDBYWAIT;  // Pump/Bank Sleep to Standby Wait State Register
   union FACTIVEWAIT_REG FACTIVEWAIT; // Pump/Bank Standby to Active Wait State Register
   union FBANKWAIT_REG   FBANKWAIT;   // Bank Read Access Wait State Register
   union FOTPWAIT_REG    FOTPWAIT;    // OTP Read Access Wait State Register
};


/* GPI/O External References & Function Declarations: */
#define GPIO_CTRL ((struct GPIO_CTRL_REGS *) 0x0000x)
#define GPIO_DATA ((struct GPIO_DATA_REGS *) 0x0000x)
#define GPIO_INT ((struct GPIO_INT_REGS *) 0x0000x)

// System Control External References & Function Declarations:

#define SYS_CTRL ((struct SYS_CTRL_REGS *) 00007010)
extern volatile struct CSM_REGS CsmRegs;
extern volatile struct CSM_PWL CsmPwl;
extern volatile struct FLASH_REGS FlashRegs;

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */


#endif  /* end of TMS2833x_H_INCLUDED definition */
