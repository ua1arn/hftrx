/* $Id$ */
/* RENESAS RZ/A1L R7S721020 memory layout */
/* Linker script to configure memory regions. */
MEMORY
{
	RAM (rwx)  : ORIGIN = 0x20240000 + 256, LENGTH = 256K - 16k - 256
	TTB (rwx)  : ORIGIN = 0x20240000 + 256K - 16k, LENGTH = 16k
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
		KEEP(*(.isr_vector))
		
		. = ALIGN(8);
		KEEP(*(.startup0))
		KEEP(*(.init))
		KEEP(*(.fini))
		*(.text*)
		*(.rodata*)
		. = ALIGN(8);
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

		KEEP(*(.eh_frame*))

		KEEP(*(vtable))
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
		. = ALIGN(64);
		*(.framebuff*)
		. = ALIGN(64);
		
		. = ALIGN(64);
		__end__ = .;
		PROVIDE(end = __end__);
	} > RAM

	__StackTop = ORIGIN(RAM) + LENGTH(RAM);
	PROVIDE(__stack = __StackTop);
}
