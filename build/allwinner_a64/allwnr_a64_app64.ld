/* $Id$ */
/* Allwinner A64-H (Banana Pi BPI-M64) layout */
/* Linker script to configure memory regions. */

MEMORY
{
  RAM (rwx)  : ORIGIN = 0x40000000 + 256, 			LENGTH = 2048M - 256	/* DDR internal memory - 128 MB - 16 kB - 256 B*/
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

		*(.rodata*)
		
		*(.itcm*)
		*(.dtcm*)

		KEEP(*(.eh_frame*))

		. = ALIGN(2048);
		KEEP(*(.isr_vector))
		__Vectors_End = .;
		__Vectors_Size = __Vectors_End - __Vectors64;


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
		QUAD (__data_end__ - __data_start__)
	*/
		/*
		QUAD (__etext2)
		QUAD (__data2_start__)
		QUAD (__data2_end__ - __data2_start__)
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
		QUAD (__bss_end__ - __bss_start__)
		QUAD (__bss2_start__)
		QUAD (__bss2_end__ - __bss2_start__)
		__zero_table_end64__ = .;
	} > RAM

	/* __etext = .; */

	.data : /*AT (__etext)*/
	{
		__data_start__ = .;
		. = ALIGN(64);

		*(.data*)

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
		
		*(.got) *(.igot)
		*(.got.plt)  *(.igot.plt)
		*(.plt) *(.iplt)
		*(.plt.got)
		*(.plt.bnd)
		
		/* All data end */
		
		__data_end__ = .;

	} > RAM

	.bss (NOLOAD):
	{
		__bss_start__ = .;
		. = ALIGN(64);
		*(.bss*)
		*(COMMON)

		. = ALIGN(64);
		__bss_end__ = .;
	} > RAM

	.noinit (NOLOAD):
	{
		. = ALIGN(64);
		*(.noinit*)
		*(.framebuff*)
	} > RAM

	.ttb BLOCK(16k) (NOLOAD):
	{
		PROVIDE(__TTB_BASE = .);
		LONG(0)	/* dummy object */
		. = ALIGN(16k);
	} > RAM

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
	} > RAM

	__StackTop = ORIGIN(RAM) + LENGTH(RAM);
	PROVIDE(__stack = __StackTop);
}
