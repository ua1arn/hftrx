/* $Id: STM32F767ZI_rom.ld 1830 2015-03-26 11:27:24Z gena $ */
/* STM32F767ZI memory layout */
/* Linker script to configure memory regions. */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2048k	/* FLASH on AXIM interface */
  RAM (rwx)  : ORIGIN = 0x20020000, LENGTH = 384k	/* SRAM1 + SRAM2 */
  DTCM (rwx) : ORIGIN = 0x20000000, LENGTH = 128k	/* DTCM Data DTCM-RAM */
  ITCM (rwx) : ORIGIN = 0x00000000, LENGTH = 16k	/* ITCM-RAM */
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
	.text :
	{
		KEEP(*(.vectors))
		__Vectors_End = .;
		__Vectors_Size = __Vectors_End - __Vectors;
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
		. = ALIGN(8);
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
		. = ALIGN(8);
		__copy_table_start__ = .;
		LONG (__etext)
		LONG (__data_start__)
		LONG ((__data_end__ - __data_start__) / 4)
		LONG (__etext2)
		LONG (__data2_start__)
		LONG ((__data2_end__ - __data2_start__) / 4)
		LONG (__etext3)
		LONG (__data3_start__)
		LONG ((__data3_end__ - __data3_start__) / 4)
		__copy_table_end__ = .;
	} > FLASH


	/* To clear multiple BSS sections,
	 * uncomment .zero.table section and,
	 * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */

	.zero.table :
	{
		. = ALIGN(8);
		__zero_table_start__ = .;
		LONG (__bss_start__)
		LONG ((__bss_end__ - __bss_start__) / 4)
		__zero_table_end__ = .;
	} > FLASH


	__etext = .;

	.dtcm : AT (__etext)
	{
		__data_start__ = .;
		*(vtable)
		. = ALIGN(8);
		*(.dtcm*)

		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;
	} > DTCM

	. = __etext + __data_end__ - __data_start__;

	__etext2 = .;

	.itcm : AT (__etext2)
	{
		__data2_start__ = .;
		*(.itcm*)
		*(.text.__ieee754_logf)
		*(.text.__ieee754_powf)
		*(.text.__ieee754_rem_pio2f)
		*(.text.__ieee754_sqrtf)
		*(.text.__kernel_rem_pio2f)

		. = ALIGN(8);	/* may be 32 */
		__data2_end__ = .;
	} > ITCM

	. = __etext2 + __data2_end__ - __data2_start__;

	__etext3 = .;

	.data : AT (__etext3)
	{
		__data3_start__ = .;
		*(.data*)
		. = ALIGN(8);	/* may be 32 */
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(8);	/* may be 32 */
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);


		. = ALIGN(8);	/* may be 32 */
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))
		. = ALIGN(8);	/* may be 32 */
		__data3_end__ = .;
	} > RAM

	. = __etext3 + __data3_end__ - __data3_start__;

	.bss :
	{
		. = ALIGN(32);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(32);
		__bss_end__ = .;
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
	} > DTCM /*RAM*/

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(DTCM) + LENGTH(DTCM);	/* was: RAM */
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
	/*ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack") */
}
