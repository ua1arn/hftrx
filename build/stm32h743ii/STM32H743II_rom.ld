/* $Id: STM32H743II_rom.ld 1830 2015-03-26 11:27:24Z gena $ */
/* STM32H743II memory layout */
/* Linker script to configure memory regions. */
MEMORY
{
	ITCMRAM (rwx) : ORIGIN = 0x00000000, LENGTH = 64k	/* ITCMRAM-RAM */
	FLASH (rx)    : ORIGIN = 0x08000000, LENGTH = 2048k	/* FLASH on AXIM interface */
	DTCMRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128k	/* DTCMRAM Data DTCMRAM-RAM */
	RAM_D1 (xrw)  : ORIGIN = 0x24000000, LENGTH = 512k	/* AXI SRAM - LTDC frame buffer */
	RAM_D2 (xrw)  : ORIGIN = 0x30000000, LENGTH = 288k	/* SRAM1, SRAM2, SRAM3 */
	RAM_D3 (xrw)  : ORIGIN = 0x38000000, LENGTH = 64k	/* SRAM4 */
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
		
		LONG (__etext4)
		LONG (__data4_start__)
		LONG ((__data4_end__ - __data4_start__) / 4)
		
		LONG (__etext5)
		LONG (__data5_start__)
		LONG ((__data5_end__ - __data5_start__) / 4)
		
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

	.data :
	{
		__data_start__ = .;
		*(vtable*)
		. = ALIGN(8);
		*(.data*)
		. = ALIGN(8);
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

		/* All data end */
		__data_end__ = .;

	} > DTCMRAM AT > FLASH

	. = __etext + __data_end__ - __data_start__;

	__etext2 = .;

	.itcm :
	{
		__data2_start__ = .;
		*(.itcm*)
		*(.text.__ieee754_logf)
		*(.text.__ieee754_powf)
		*(.text.__ieee754_rem_pio2f)
		*(.text.__ieee754_sqrtf)
		*(.text.__kernel_rem_pio2f)

		. = ALIGN(8);
		__data2_end__ = .;
	} > ITCMRAM AT > FLASH

	. = __etext2 + __data2_end__ - __data2_start__;

	__etext3 = .;

	.ram_d1 :
	{
		__data3_start__ = .;
		*(.ram_d1)
		*(.ram_d1.*)
		. = ALIGN(8);
		__data3_end__ = .;
	} > RAM_D1 AT > FLASH

	. = __etext3 + __data3_end__ - __data3_start__;
	
	__etext4 = .;

	.ram_d2 :
	{
		__data4_start__ = .;
		*(.ram_d2)
		*(.ram_d2.*)
		. = ALIGN(8);
		__data4_end__ = .;
	} > RAM_D2 AT > FLASH

	. = __etext4 + __data4_end__ - __data4_start__;
	
	__etext5 = .;

	.ram_d3 :
	{
		__data5_start__ = .;
		*(.ram_d3)
		*(.ram_d3.*)
		. = ALIGN(8);
		__data5_end__ = .;
	} > RAM_D3 AT > FLASH

	. = __etext5 + __data5_end__ - __data5_start__;

	.bss (NOLOAD):
	{
		. = ALIGN(32);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(32);
		__bss_end__ = .;
	} > RAM_D1

	.heap (NOLOAD):
	{
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		KEEP(*(.heap*))
		. = ALIGN(8);
		__HeapLimit = .;
	} > RAM_D3

	.noinit (NOLOAD):
	{
		. = ALIGN(32);
		*(.noinit*)
		. = ALIGN(32);
	} > RAM_D1

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (NOLOAD):
	{
		KEEP(*(.stack*))
		. = ALIGN(8);
	} > DTCMRAM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(DTCMRAM) + LENGTH(DTCMRAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
	/*ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack") */
}
