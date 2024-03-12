/* $Id$ */
/* Allwinner F133 layout */
/* Linker script to configure memory regions. */

MEMORY
{
   RAM (rwx)	: ORIGIN = 0x00020000 + 256, LENGTH = 160k - 256		/*  */
   bootRAM (rwx)	: ORIGIN = 0x00028000 + 256, LENGTH = 160k - 32k - 256		/*  */
   DDR3 (rwx) : ORIGIN = 0x40000000 + 16M, LENGTH = 64M - 16M	/*  */
}

/* Library configurations */
/* GROUP(libgcc.a libc.a libm.a libnosys.a) */

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
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
 */
	 ENTRY(Reset_Handler)

SECTIONS
{
	.text :
	{
		. = ALIGN(32);
		KEEP(*(.startup0))

		*(.text*)
		KEEP(*(.init))
		KEEP(*(.fini))
		
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

		*(.rodata*  .srodata*)

		KEEP(*(.eh_frame*))
		
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		__Vectors_End = .;
		__Vectors_Size = __Vectors_End - __Vectors;
		
	} > RAM

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > RAM

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > RAM
	__exidx_end = .;

	/* To copy multiple ROM to RAM sections,
	 * uncomment .copy.table section and,
	 * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
	.copy.table :
	{
		. = ALIGN(8);
		__copy_table_start64__ = .;
	/*
		QUAD (__etext)
		QUAD (__data_start__)
		QUAD ((__data_end__ - __data_start__) / 4)
	*/
	/*
		QUAD (__etext2)
		QUAD (__data2_start__)
		QUAD ((__data2_end__ - __data2_start__) / 4)
	*/
		__copy_table_end64__ = .;
	} > RAM

	/* To clear multiple BSS sections,
	 * uncomment .zero.table section and,
	 * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
	.zero.table :
	{
		. = ALIGN(8);
		__zero_table_start64__ = .;
		QUAD (__bss_start__)
		QUAD ((__bss_end__ - __bss_start__) / 4)
	/*
		QUAD (__bss2_start__)
		QUAD ((__bss2_end__ - __bss2_start__) / 4)
	*/
		__zero_table_end64__ = .;
	} > RAM

	/* __etext = .; */

	.data : /*AT (__etext)*/
	{
		__data_start__ = .;
		. = ALIGN(64);

		*(.data* .sdata*)
		*(.itcm*)
		*(.dtcm*)

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
		/* All data end */

		__data_end__ = .;

	} > RAM

	/*
	__etext2 = .;

	.sysram : AT (__etext2)
	{
		__data2_start__ = .;

		*(.itcm*)
		*(.dtcm*)

		__data2_end__ = .;
	} > RAM

	. = __etext2 + __data2_end__ - __data2_start__;

   */
	
	.bss (NOLOAD):
	{
		__bss_start__ = .;
		. = ALIGN(64);
		*(.bss*  .sbss*)
		*(COMMON)

		__bss_end__ = .;
	} > RAM

	.noinit (NOLOAD):
	{
		. = ALIGN(64);
		*(.noinit*)
		*(.framebuff*)
	} > DDR3

	.heap (NOLOAD):
	{
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		KEEP(*(.heap*))
		__HeapLimit = .;
	} > RAM

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (NOLOAD):
	{
		KEEP(*(.stack*))
	} > RAM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(RAM) + LENGTH(RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* PROVIDE(__TTB_BASE = ORIGIN(TTB)); */
	/* PROVIDE(__ttb_end__ = ORIGIN(TTB) + LENGTH(TTB)); */

	/* Check if data + heap + stack exceeds RAM limit */
	/*ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")*/
}
