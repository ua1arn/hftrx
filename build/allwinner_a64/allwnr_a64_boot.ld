/* $Id$ */
/* Allwinner A64-H (Banana Pi BPI-M64) layout */
/* Linker script to configure memory regions. */

/* https://linux-sunxi.org/A64/Memory_map say about 108k 0x00018000-0x00032FFF, wee use 96k */
MEMORY
{
  RAM (rwx)	 : ORIGIN = 0x00010000 + 256, 			LENGTH = 32k + 96k - 256		/* SRAM A1 + SRAM C: SD card boot */
  bootltestRAM (rwx) : ORIGIN = 0x00018000 + 256, 			LENGTH = 96k - 256		/* bootloader tests / DDR initialier base */
  STACK (rwx): ORIGIN = 0x00044000,					LENGTH = 64k
  DDR3 (rwx) : ORIGIN = 0x40000000, 				LENGTH = 2048M		/*  */
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
	.startup :
	{
		KEEP(*(.startup0))
	} > RAM
	
	.text :
	{
		KEEP(*(.isr_vector))
		. = ALIGN(64);
		*(.text*)
		. = ALIGN(64);
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
		. = ALIGN(8);
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
		. = ALIGN(8);
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

		. = ALIGN(8);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(8);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);


		. = ALIGN(8);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))

        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
	} > RAM

	.bss :
	{
		__bss_start__ = .;
		. = ALIGN(64);
		*(.sbss*)
		*(.gnu.linkonce.sb.*)
		*(.bss .bss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)
		. = ALIGN(64);
		__bss_end__ = .;
		LONG(0xDEADBEEF)
	} > RAM

	.noinit (NOLOAD):
	{
		. = ALIGN(64);
		*(.noinit*)

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
