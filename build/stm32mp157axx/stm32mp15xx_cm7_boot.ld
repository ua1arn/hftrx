/* $Id$ */
/* RENESAS RZ/A1L R7S721020 memory layout */
/* Linker script to configure memory regions. */

/* Note: due to ROM code RAM needs, FSBL payload is limited to 247 KB.  */
/* The ROM code loads the image into SYSRAM internal memory at address 0x2ffc2400.  */
/*
	https://community.st.com/s/question/0D50X0000BiDo0fSQC/stm32mp157aab-failed-to-boot-from-nor-qspi-baremetal?t=1575552683743
	
	Load address is ALWAYS 0x2FFC2400 for FSBL, whatever you put in the header (it is ignored for FSBL).
	Entry point in the header could be anywhere after the header (i.e. 0x2FFC2500) and the end of the SYSRAM
	I recommend to compile your FSBL SW to fit with a region starting from 0x2FFC2500 (with a size of max 247KB minus 256 for header).
	Entry point will depend on your SW startup.s construction, it is not always at the very first address.
*/
MEMORY
{
  FLASH (rx) 	: ORIGIN = 0x70000000, LENGTH = 4M		/* W25Q32FVZPIG */
  DDR3 (rwx) 	: ORIGIN = 0xC0000000, LENGTH = 256M 	/* DDR3 single chip NT5CC128M16IP-DI */
  RAM (rwx)		: ORIGIN = 0x2FFC2500, LENGTH = 0x00039B00	/* SYSRAM - 256k - 16k - 9k - 256 B */
  TTB (rwx)  	: ORIGIN = 0x2FFFC000, LENGTH = 16k		/* last 16 kB SYSRAM for TTB */
  SRAM (rwx)  	: ORIGIN = 0x20000000, LENGTH = 384k	/* Cortex-M4 memories */
  SRAM2 (rwx)  	: ORIGIN = 0x30000000, LENGTH = 96k		/* Cortex-M4 memories visible for Cortex-A7*/
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
	 ENTRY(Reset_Handler7)

SECTIONS
{
	.text :
	{
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

	.data : /*AT (__etext)*/
	{
		__data_start__ = .;
		. = ALIGN(32);
		KEEP(*(.vectors))
		__Vectors_End = .;
		__Vectors_Size = __Vectors_End - __Vectors;

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
		. = ALIGN(32);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(32);
		__bss_end__ = .;
	} > RAM

	.noinit :
	{
		. = ALIGN(32);
		*(.noinit)
		. = ALIGN(32);
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
	} > RAM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(RAM) + LENGTH(RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	PROVIDE(__TTB_BASE = ORIGIN(TTB));
	/* PROVIDE(__ttb_end__ = ORIGIN(TTB) + LENGTH(TTB)); */

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}
