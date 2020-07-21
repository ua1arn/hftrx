/* $Id$ */
/* STM32F746ZG memory layout */
/* Linker script to configure memory regions. */
MEMORY
{
	ITCM (rwx) :		ORIGIN = 0x00000000, LENGTH = 16k	/* ITCM = RAMITCM_BASE */
	FLASH (rx) :		ORIGIN = 0x08000000, LENGTH = 1024k	/* FLASH on AXIM interface = FLASHAXI_BASE */
	FLASHitcm (rx) :	ORIGIN = 0x00200000, LENGTH = 1024k	/* FLASH on AXIM interface = FLASHITCM_BASE */
	DTCM (rwx) :		ORIGIN = 0x20000000, LENGTH = 64k	/* DTCM = RAMDTCM_BASE */
	RAM (rwx)  :		ORIGIN = 0x20010000, LENGTH = 256k	/* was: 240k SRAM = SRAM1_BASE (240k) */
	/*RAM2 (rwx)  :		ORIGIN = 0x2004C000, LENGTH = 64k	 SRAM = SRAM2_BASE (64k) */
  SDRAM  (rwx)  :		ORIGIN = 0xC0000000, LENGTH = 8192k	/* MT48LC4M32B2 */
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __Vectors_End
 *   __Vectors_Size
 */
ENTRY(Reset_Handler)

SECTIONS
{
  .sdram (NOLOAD):
	{
		. = ALIGN(32);
		*(.framebuff*)
    . = ALIGN(32);
	} > SDRAM
  
	.text :
	{
		. = ALIGN(32);
		KEEP(*(.vectors))
		__Vectors_End = .;
		__Vectors_Size = __Vectors_End - __Vectors;
		
		. = ALIGN(4);
		KEEP(*(.init))
		KEEP(*(.initfunc))
		KEEP(*(.initdata))
		KEEP(*(.fini))
		*(.text*)
	} > FLASH

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASH

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > FLASH
	__exidx_end = .;

	/* To copy multiple ROM to RAM sections,
	 * uncomment .copy.table section and,
	 * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */

	.copy.table :
	{
		. = ALIGN(4);
		__copy_table_start__ = .;
  /*
		LONG (__etext)
		LONG (__data_start__)
		LONG (__data_end__ - __data_start__)
		LONG (__etext2)
		LONG (__data2_start__)
		LONG (__data2_end__ - __data2_start__)
	*/		
		__copy_table_end__ = .;
	} > FLASH

	/* To clear multiple BSS sections,
	 * uncomment .zero.table section and,
	 * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */

	.zero.table :
	{
		. = ALIGN(4);
		__zero_table_start__ = .;
		LONG (__bss_start__)
		LONG (__bss_end__ - __bss_start__)
	/*
		LONG (__bss2_start__)
		LONG (__bss2_end__ - __bss2_start__)
	*/
		__zero_table_end__ = .;
	} > FLASH

	.dtcm :
	{
		*(vtable)
		. = ALIGN(4);
		*(.dtcm*)
		. = ALIGN(4);	/* may be 32 */
		/* All data end */
	} > DTCM


	.itcm :
	{
		*(.itcm*)
		*(.text.__ieee754_logf)
		*(.text.__ieee754_powf)
		*(.text.__ieee754_rem_pio2f)
		*(.text.__ieee754_sqrtf)
		*(.text.__kernel_rem_pio2f)

		. = ALIGN(4);	/* may be 32 */
	} > ITCM
  
	.data :
	{
		__data_start__ = .;

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
 		*crtbegin.o(.dtors)
 		*crtbegin?.o(.dtors)
 		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
 		*(SORT(.dtors.*))
 		*(.dtors)

		*(.rodata*)

		KEEP(*(.eh_frame*))

		*(vtable)
		*(.data*)
	   . = ALIGN(4);	/* one line added by ua1arn */

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);


		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))
		. = ALIGN(32);
		/* All data end */
		__data_end__ = .;
	} > RAM

	.bss (NOLOAD):
	{
		. = ALIGN(32);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(32);
		__bss_end__ = .;
	} > SDRAM

	.heap (NOLOAD):
	{
		__HeapBase = .;
		KEEP(*(.heap*))
		__HeapLimit = .;
	} > SDRAM

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (COPY):
	{
		KEEP(*(.stack*))
	} > DTCM /*RAM*/

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(DTCM) + LENGTH(DTCM);	/* was: RAM */
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
	/*ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack") */
}
