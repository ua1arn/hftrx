/* $Id$ */
/* ST STM32MP157AAB3 memory layout */
/* Linker script to configure memory regions. */
MEMORY
{
  FLASH (rx) 	: ORIGIN = 0x70000000, LENGTH = 4M		/* W25Q32FVZPIG */
  RAM (rwx) 	: ORIGIN = 0xC0000000, LENGTH = 256M 	/* DDR3 single chip NT5CC128M16IP-DI */
  SYSRAM (rwx)	: ORIGIN = 0x2FFC0000, LENGTH = 240k	/* SYSRAM - 256 kB */
  TTB (rwx)  	: ORIGIN = 0x2FFFC000, LENGTH = 16k		/* last 16 kB SYSRAM for TTB */
  SRAM (rwx)  	: ORIGIN = 0x20000000, LENGTH = 384k	/* Cortex-M4 memories */
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
/* ENTRY(Reset_Handler) */

SECTIONS
{
	.text :
	{
		KEEP(*(.vectors))
		__Vectors_End = .;
		__Vectors_Size = __Vectors_End - __Vectors;
		KEEP(*(.init))
		KEEP(*(.initfunc))
		KEEP(*(.initdata))
		KEEP(*(.fini))
		*(.text*)
		*(.itcm*)
		*(.dtcm*)
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
	} > RAM

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
	} > RAM

	__etext = .;

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

		*(.data*)
	   . = ALIGN(4);	/* one line added by ua1arn */
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
		. = ALIGN(32);
		/* All data end */
		__data_end__ = .;

	} > RAM

	.bss :
	{
		. = ALIGN(6);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(64);
		*(.noinit)
		. = ALIGN(64);
		*(.framebuff)
		. = ALIGN(64);
		__bss_end__ = .;
	} > RAM

	.noinit :
	{
	} > RAM

	.vram :
	{
		. = ALIGN(64);
	} > RAM

	.heap (COPY):
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
	.stack_dummy (COPY):
	{
		KEEP(*(.stack*))
	} > SYSRAM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(SYSRAM) + LENGTH(SYSRAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	PROVIDE(__TTB_BASE = ORIGIN(TTB));
	/* PROVIDE(__ttb_end__ = ORIGIN(TTB) + LENGTH(TTB)); */

	/* Check if data + heap + stack exceeds RAM limit */
	/* ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack") */
}
