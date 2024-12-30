/* $Id$ */
/* ST STM32MP157AAB3/STM32MP157DAB1 memory layout */
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
  DDR3 (rwx) 	: ORIGIN = 0xC0000000 + 64M, LENGTH = 256M - 64M 		/* lower 64MB reserved for image, DDR3 single chip NT5CC128M16IP-DI */
  RAM (rwx)		: ORIGIN = 0x2FFC0000 + 9k + 0x100, LENGTH = 256k - 16k - 9k - 0x100	/* SYSRAM - 256k - 16k - 9k - 256 B */
  SRAM (rwx)  	: ORIGIN = 0x20000000, LENGTH = 384k		/* Cortex-M4 memories */
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
	 ENTRY(Reset_Handler)

SECTIONS
{
	.text :
	{
		. = ALIGN(64);
		KEEP(*(.isr_vector))
		__Vectors_End = .;
		__Vectors_Size = __Vectors_End - __Vectors;
		
		. = ALIGN(64);
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

		*(.rodata*)

		KEEP(*(.eh_frame*))
		
		*(.itcm*)
		
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
		LONG (__bss2_start__)
		LONG (__bss2_end__ - __bss2_start__)
		__zero_table_end__ = .;
	} > RAM

	__etext = .;

	.data : /*AT (__etext)*/
	{
		__data_start__ = .;

		. = ALIGN(64);

		*(.data*)
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

	.noinit (NOLOAD):
	{
		. = ALIGN(64);
		*(.noinit*)

	} > RAM

	.bss (NOLOAD):
	{
		__bss_start__ = .;
		. = ALIGN(64);
		*(.bss*)
		*(COMMON)

		__bss_end__ = .;
	} > RAM 

	.vram (NOLOAD):
	{
		. = ALIGN(64);
		*(.framebuff*)

	} > DDR3

	.ramnc BLOCK(1M) (NOLOAD):
	{
		PROVIDE(__RAMNC_BASE = .);
		__bss2_start__ = .;
		*(.ramnc*)
		__bss2_end__ = .;
		. = ALIGN(1M);
		PROVIDE(__RAMNC_TOP = .);
		
		. = ALIGN(64);
		__end__ = .;
		PROVIDE(end = __end__);
	} > DDR3

	__StackTop = ORIGIN(DDR3) + LENGTH(DDR3);
	PROVIDE(__stack = __StackTop);
}
