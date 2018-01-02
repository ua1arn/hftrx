/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#if CPUSTYLE_R7S721

#include "hardware.h"
#include "formats.h"
#include "hardware_r7s721.h"

/* ==== Interrupt detection ==== */

static void Userdef_INTC_Dummy_Interrupt(void)
{
	debug_printf_P(PSTR("Userdef_INTC_Dummy_Interrupt()\n"));
	for (;;)
		;
}

/* Interrupt handler table */
static void (* intc_func_table [IRQ_GIC_LINE_COUNT])(void);

/* Вызывается из crt_r7s721.s со сброшенным флагом прерываний */
void IRQHandlerSafe(void)
{
	/* const uint32_t icchpir = */ (void) GICC_HPPIR;	/* GICC_HPPIR */
	const uint32_t icciar = GICC_IAR;				/* GICC_IAR */
	const uint16_t int_id = icciar & INTC_ICCIAR_ACKINTID;

	// See R01UH0437EJ0200 Rev.2.00 7.8.3 Reading Interrupt ID Values from Interrupt Acknowledge Register (ICCIAR)
	// IHI0048B_b_gic_architecture_specification.pdf
	// See ARM IHI 0048B.b 3.4.2 Special interrupt numbers when a GIC supports interrupt grouping

	if (int_id >= 1020)
	{
		GICD_IPRIORITYRn(0) = GICD_IPRIORITYRn(0);
	}
	else if (int_id != 0 || (INTC.ICDABR0 & 0x0001) != 0)
	{
	#if WITHNESTEDINTERRUPTS

		__enable_irq();						/* modify I bit in CPSR */
		(* intc_func_table [int_id])();	    /* Call interrupt handler */
		__disable_irq();					/* modify I bit in CPSR */

	#else /* WITHNESTEDINTERRUPTS */

		(* intc_func_table [int_id])();	    /* Call interrupt handler */

	#endif /* WITHNESTEDINTERRUPTS */
		INTC.ICCEOIR = int_id;				/* GICC_EOIR */
	}
	else
	{
		GICD_IPRIORITYRn(0) = GICD_IPRIORITYRn(0);
	}
}

/******************************************************************************
* Function Name: r7s721_intc_initialize
* Description  : Executes initial setting for the INTC.
*              : The interrupt mask level is set to 31 to receive interrupts 
*              : with the interrupt priority level 0 to 30.
* Arguments    : none
* Return Value : none
******************************************************************************/
void r7s721_intc_initialize(void)
{

	/* ==== Total number of registers ==== */
	enum { INTC_ICDISR_REG_TOTAL   = (IRQ_GIC_LINE_COUNT + 31) / 32 };	// 19 == INTC_ICDISR0_COUNT
	enum { INTC_ICDICFR_REG_TOTAL  = (IRQ_GIC_LINE_COUNT + 15) / 16 };	// 37 == INTC_ICDICFR0_COUNT
	enum { INTC_ICDIPR_REG_TOTAL   = (IRQ_GIC_LINE_COUNT + 3) /  4 };	// 147 == INTC_ICDIPR0_COUNT
	enum { INTC_ICDIPTR_REG_TOTAL  = (IRQ_GIC_LINE_COUNT + 3) /  4 };	// 147 == INTC_ICDIPTR0_COUNT
	//enum { INTC_ICDISER_REG_TOTAL  = (IRQ_GIC_LINE_COUNT + 31) / 32 };	// 19 == INTC_ICDISER0_COUNT
	enum { INTC_ICDICER_REG_TOTAL  = (IRQ_GIC_LINE_COUNT + 31) / 32 };	// 19 == INTC_ICDICER0_COUNT

	/* Initial value table of Interrupt Configuration Registers */
	// Table 4-19 GICD_ICFGR Int_config[0] encoding in some early GIC implementations
	// каждая пара бит кодирует:

	// [0] == 0: Corresponding interrupt is handled using the N-N model.
	// [0] == 1: Corresponding interrupt is handled using the 1-N model.
	// [1] == 0: Corresponding interrupt is level-sensitive.
	// [1] == 1: Corresponding interrupt is edge-triggered.
	static const uint32_t intc_icdicfrn_table [INTC_ICDICFR_REG_TOTAL] =
	{                          /*           Interrupt ID */
		0xAAAAAAAA,            /* ICDICFR0  :  15 to   0 */
		0x00000055,            /* ICDICFR1  :  19 to  16 */
		0xFFFD5555,            /* ICDICFR2  :  47 to  32 */
		0x555FFFFF,            /* ICDICFR3  :  63 to  48 */
		0x55555555,            /* ICDICFR4  :  79 to  64 */
		0x55555555,            /* ICDICFR5  :  95 to  80 */
		0x55555555,            /* ICDICFR6  : 111 to  96 */
		0x55555555,            /* ICDICFR7  : 127 to 112 */
		0x5555F555,            /* ICDICFR8  : 143 to 128 */
		0x55555555,            /* ICDICFR9  : 159 to 144 */
		0x55555555,            /* ICDICFR10 : 175 to 160 */
		0xF5555555,            /* ICDICFR11 : 191 to 176 */
		0xF555F555,            /* ICDICFR12 : 207 to 192 */
		0x5555F555,            /* ICDICFR13 : 223 to 208 */
		0x55555555,            /* ICDICFR14 : 239 to 224 */
		0x55555555,            /* ICDICFR15 : 255 to 240 */
		0x55555555,            /* ICDICFR16 : 271 to 256 */
		0xFD555555,            /* ICDICFR17 : 287 to 272 */
		0x55555557,            /* ICDICFR18 : 303 to 288 */
		0x55555555,            /* ICDICFR19 : 319 to 304 */
		0x55555555,            /* ICDICFR20 : 335 to 320 */
		0x5F555555,            /* ICDICFR21 : 351 to 336 */
		0xFD55555F,            /* ICDICFR22 : 367 to 352 */
		0x55555557,            /* ICDICFR23 : 383 to 368 */
		0x55555555,            /* ICDICFR24 : 399 to 384 */
		0x55555555,            /* ICDICFR25 : 415 to 400 */
		0x55555555,            /* ICDICFR26 : 431 to 416 */
		0x55555555,            /* ICDICFR27 : 447 to 432 */
		0x55555555,            /* ICDICFR28 : 463 to 448 */
		0x55555555,            /* ICDICFR29 : 479 to 464 */
		0x55555555,            /* ICDICFR30 : 495 to 480 */
		0x55555555,            /* ICDICFR31 : 511 to 496 */
		0x55555555,            /* ICDICFR32 : 527 to 512 */
		0x55555555,            /* ICDICFR33 : 543 to 528 */
		0x55555555,            /* ICDICFR34 : 559 to 544 */
		0x55555555,            /* ICDICFR35 : 575 to 560 */
		0x00155555             /* ICDICFR36 : 586 to 576 */
	};

    uint16_t offset;
    volatile uint32_t * addr;

	/* default interrut handlers setup */
    for (offset = 0; offset < IRQ_GIC_LINE_COUNT; ++ offset)
    {
        intc_func_table [offset] = Userdef_INTC_Dummy_Interrupt;    /* Set all interrupts default handlers */
    }
	
    /* ==== Initial setting 1 to receive GIC interrupt request ==== */
    /* Interrupt Security Registers setting */
    addr = (volatile uint32_t *) & INTC.ICDISR0;
    for (offset = 0; offset < INTC_ICDISR_REG_TOTAL; ++ offset)
    {
        * (addr + offset) = 0x00000000uL;    /* Set all interrupts to be secured */
    }

    /* Interrupt Configuration Registers setting */
    addr = (volatile uint32_t *) & INTC.ICDICFR0;
    for (offset = 0; offset < INTC_ICDICFR_REG_TOTAL; ++ offset)
    {
        * (addr + offset) = intc_icdicfrn_table [offset];
    }

    /* Interrupt Priority Registers setting */
    addr = (volatile uint32_t *) & GICD_IPRIORITYRn(0);
    for (offset = 0; offset < INTC_ICDIPR_REG_TOTAL; ++ offset)
    {
        /* Set the priority for all interrupts to 31 */
        * (addr + offset) = (31 << ARM_CA9_PRIORITYSHIFT) * 0x01010101uL;
    }

    /* Interrupt Processor Targets Registers setting */
    /* Initialise ICDIPTR8 to ICDIPTRn                     */
    /* (n = The number of interrupt sources / 4)           */
    /*   - ICDIPTR0 to ICDIPTR4 are dedicated for main CPU */
    /*   - ICDIPTR5 is dedicated for sub CPU               */
    /*   - ICDIPTR6 to 7 are reserved                      */
    addr = (volatile uint32_t *) & INTC.ICDIPTR0;
    for (offset = 8; offset < INTC_ICDIPTR_REG_TOTAL; ++ offset)
    {
    	/* Set the target for all interrupts to main CPU */
        * (addr + offset) = 0x01010101uL;
    }

    /* Interrupt Clear-Enable Registers setting */
    addr = (volatile uint32_t *) & INTC.ICDICER0;
    for (offset = 0; offset < INTC_ICDICER_REG_TOTAL; ++ offset)
    {
    	 /* Set all interrupts to be disabled */
    	* (addr + offset) = 0xFFFFFFFFuL;
    }

    /* Interrupt Priority Mask Register setting */
    /* Enable priorities for all interrupts */
    GIC_SetInterfacePriorityMask(ARM_CA9_ENCODE_PRIORITY(31));	// GICC_PMR

    /* Binary Point Register setting */
    /* Group priority field [7:3], Subpriority field [2:0](Do not use) */
    //INTC.ICCBPR = 0x00000002uL; // Binary Point Register, GICC_BPR, may be ARM_CA9_PRIORITYSHIFT - 1
	GIC_SetBinaryPoint(2);
    /* CPU Interface Control Register setting */
    //INTC.ICCICR = 0x00000003uL;	// GICC_CTLR
	GIC_EnableInterface();	// cjeck GICInterface->CTLR a same for INTC.ICCICR

    /* Initial setting 2 to receive GIC interrupt request */
    /* Distributor Control Register setting */
    //INTC.ICDDCR = 0x00000001uL;
	GIC_EnableDistributor();	// check GICDistributor->CTLR a same for INTC.ICDDCR

}

/******************************************************************************
* Function Name: R_INTC_RegistIntFunc
* Description  : Registers the function specified by the func to the element 
*              : specified by the int_id in the INTC interrupt handler function
*              : table.
* Arguments    : uint16_t int_id         : Interrupt ID
*              : void (* func)(uint32_t) : Function to be registered to INTC
*              :                         : interrupt hander table
* Return Value : DEVDRV_SUCCESS          : Success of registration of INTC 
*              :                         : interrupt handler function
*              : DEVDRV_ERROR            : Failure of registration of INTC 
*              :                         : interrupt handler function
******************************************************************************/
void r7s721_intc_registintfunc(uint_fast16_t int_id, void (* func)(void))
{

    intc_func_table [int_id] = func;     /* Register specified interrupt functions */
}

/******************************************************************************/

// TTB initialize

// SAM9XE512 bits
//#define TLB_NCNB 0x0DF2 // Noncachable, Nonbufferable 11 0 1111 1 00 10
//#define TLB_WT 0x0DFA // Write-through 11 0 1111 1 10 10
//#define TLB_WB 0x0DFE // Write-back 11 0 1111 1 11 10
//#define TLB_INVALID 0x0000 // Write-back 11 0 1111 1 11 10


/*

; ---- Parameter setting to level1 descriptor (bits 19:0) ----
; setting for Strongly-ordered memory
TTB_PARA_STRGLY             EQU     2_00000000000000000000110111100010
; setting for Outer and inner not cache normal memory
TTB_PARA_NORMAL_NOT_CACHE   EQU     2_00000000000000000001110111100010
; setting for Outer and inner write back, write allocate normal memory (Cacheable)
TTB_PARA_NORMAL_CACHE       EQU     2_00000000000000000001110111101110

; ---- Memory area size (MB) ----
M_SIZE_NOR      EQU     128             ; [Area00] CS0, CS1 area (for NOR flash)
M_SIZE_SDRAM    EQU     128             ; [Area01] CS2, CS3 area (for SDRAM)
M_SIZE_CS45     EQU     128             ; [Area02] CS4, CS5 area
M_SIZE_SPI      EQU     128             ; [Area03] SPI, SP2 area (for Serial flash)
M_SIZE_RAM      EQU     10              ; [Area04] Internal RAM
M_SIZE_IO_1     EQU     502             ; [Area05] I/O area 1
M_SIZE_NOR_M    EQU     128             ; [Area06] CS0, CS1 area (for NOR flash) (mirror)
M_SIZE_SDRAM_M  EQU     128             ; [Area07] CS2, CS3 area (for SDRAM) (mirror)
M_SIZE_CS45_M   EQU     128             ; [Area08] CS4, CS5 area (mirror)
M_SIZE_SPI_M    EQU     128             ; [Area09] SPI, SP2 area (for Serial flash) (mirror)
M_SIZE_RAM_M    EQU     10              ; [Area10] Internal RAM (mirror)
M_SIZE_IO_2     EQU     2550            ; [Area11] I/O area 2
;===================================================================
; Cortex-A9 MMU Configuration
; Set translation table base
;===================================================================
    ;;; Cortex-A9 supports two translation tables
    ;;; Configure translation table base (TTB) control register cp15,c2
    ;;; to a value of all zeros, indicates we are using TTB register 0.
    MOV  r0,#0x0
    MCR  p15, 0, r0, c2, c0, 2      ;;; TTBCR

    ;;; write the address of our page table base to TTB register 0
    LDR  r0,=||Image$$TTB$$ZI$$Base||
    MOV  r1, #0x08                  ;;; RGN=b01  (outer cacheable write-back cached, write allocate)
                                    ;;; S=0      (translation table walk to non-shared memory)
    ORR  r1,r1,#0x40                ;;; IRGN=b01 (inner cacheability for the translation table walk is Write-back Write-allocate)
    ORR  r0,r0,r1
    MCR  p15, 0, r0, c2, c0, 0      ;;; TTBR0

;===================================================================
; PAGE TABLE generation 
; Generate the page tables
; Build a flat translation table for the whole address space.
; ie: Create 4096 1MB sections from 0x000xxxxx to 0xFFFxxxxx
; 31                 20 19  18  17  16 15  14   12 11 10  9  8     5   4    3 2   1 0
; |section base address| 0  0  |nG| S |AP2|  TEX  |  AP | P | Domain | XN | C B | 1 0|
;
; Bits[31:20]   - Top 12 bits of VA is pointer into table
; nG[17]=0      - Non global, enables matching against ASID in the TLB when set.
; S[16]=0       - Indicates normal memory is shared when set.
; AP2[15]=0  
; AP[11:10]=11  - Configure for full read/write access in all modes
; TEX[14:12]=000
; CB[3:2]= 00   - Set attributes to Strongly-ordered memory.
;                 (except for the descriptor where code segment is based, see below)
; IMPP[9]=0     - Ignored
; Domain[5:8]=1111   - Set all pages to use domain 15
; XN[4]=0       - Execute never disabled
; Bits[1:0]=10  - Indicate entry is a 1MB section
;===================================================================

  */
// See B3.5.2 in DDI0406C_C_arm_architecture_reference_manual.pdf

//; setting for Strongly-ordered memory
//#define	TTB_PARA_STRGLY             0b_0000_0000_1101_1110_0010
#define	TTB_PARA_STRGLY             0b00000000110111100010UL	// 0x00DE2

//; setting for Outer and inner not cache normal memory
//#define	TTB_PARA_NORMAL_NOT_CACHE   0b_0000_0001_1101_1110_0010
#define	TTB_PARA_NORMAL_NOT_CACHE   0b00000001110111100010UL	// 0x01DE2

//; setting for Outer and inner write back, write allocate normal memory (Cacheable)
//#define	TTB_PARA_NORMAL_CACHE       0b_0000_0001_1101_1110_1110
#define	TTB_PARA_NORMAL_CACHE       0b00000001110111101110UL	// 01DEE

static uint32_t 
r7s721_accessbits(uint32_t a)
{
	const uint32_t addrbase = a & 0xFFF00000uL;

	if (a >= 0x00000000uL && a < 0x00A00000uL)			// up to 10 MB
		return addrbase | TTB_PARA_NORMAL_CACHE;
	if (a >= 0x20000000uL && a < 0x20A00000uL)			// up to 10 MB
		return addrbase | TTB_PARA_NORMAL_CACHE;
	return addrbase | TTB_PARA_STRGLY;
}

void r7s721_ttb_initialize(void)
{
	extern uint32_t __ttb_start__;		// получено из скрипта линкера
	uint32_t * const tlbbase = & __ttb_start__;
	unsigned i;

	for (i = 0; i < 4096; ++ i)
	{
		const uint32_t address = (uint32_t) i << 20;
		tlbbase [i] =  r7s721_accessbits(address);
	}	

	//CP15_writeTTBCR(0);
	__set_TTBR0((unsigned int) tlbbase | 0x48);	// TTBR0
	//CP15_writeTTB1((unsigned int) tlbbase | 0x48);	// TTBR1

	// Program the domain access register
	//__set_DACR(0x55555555); // domain 15: access are not checked
	__set_DACR(0xFFFFFFFF); // domain 15: access are not checked
}

// с точностью до 1 мегабайта
void r7s721_ttb_map(
	uint32_t va,	/* virtual address */
	uint32_t la	/* linear (physical) address */
	)
{
	extern uint32_t __ttb_start__;		// получено из скрипта линкера
	uint32_t * const tlbbase = & __ttb_start__;
	unsigned i = va >> 20;
	tlbbase [i] =  r7s721_accessbits(la);
}

void arm_hardware_sdram_initialize(void)
{

}

void arm_hardware_ltdc_initialize(void)
{
}

#endif /* CPUSTYLE_R7S721 */
