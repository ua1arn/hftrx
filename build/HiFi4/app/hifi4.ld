ROM_SIZE   = 64K - 2K        ; /* Size of "Read-Only" Data  */
RAM_SIZE   = 32K             ; /* Size of Static R/W Data   */
HEAP_SIZE  = 16K             ; /* Heap Size                 */
STACK_SIZE = 32K - HEAP_SIZE ; /* Stack Size for CPU        */

LIT_BASE   = 0x00028000            ; /* IRAM  */
VEC_BASE   = 0x00028400            ; /* IRAM  */
ROM_BASE   = 0x00028800            ; /* IRAM  */

RAM_BASE   = 0x00038000            ; /* DRAM0 */
HEAP_BASE  = 0x00040000            ; /* DRAM1 */

STACK_BASE = HEAP_BASE + HEAP_SIZE ;

ENTRY(Start)

MEMORY
{
 LIT   (RX) : ORIGIN = LIT_BASE   , LENGTH =   1K
 VEC   (RX) : ORIGIN = VEC_BASE   , LENGTH =   1K
 ROM   (RX) : ORIGIN = ROM_BASE   , LENGTH =   ROM_SIZE

 RAM   (RW) : ORIGIN = RAM_BASE   , LENGTH =   RAM_SIZE
 HEAP  (RW) : ORIGIN = HEAP_BASE  , LENGTH =  HEAP_SIZE
 STACK (RW) : ORIGIN = STACK_BASE , LENGTH = STACK_SIZE
}

SECTIONS
{

 .entry.text : {
                KEEP (*(.entry.text))
                KEEP (*(.literal)) 
               } > LIT

 .Vectors.text : {
                  KEEP (*(.Vectors.text))
                 } > VEC

 .text     : { 
              KEEP (*(.text))
             } > ROM

_sidata = LOADADDR(.data);

.data : {
	 . = ALIGN(4);
	 _sdata = .;
         KEEP( *(.data) )
	 KEEP( *(.data.*) )
         KEEP( *(.rodata) )
	 KEEP( *(.rodata.*) )
	 . = ALIGN(4);
	 _edata = .;
	} > RAM AT > ROM

.bss : {
	. = ALIGN(4);
        _sbss = .;
        *(.bss*). = . ;
	. = ALIGN(4);
        _ebss = .;
       } > RAM

.heap : {
	 . = ALIGN(4);
         __heap_start = . ;
         *(.heap*). = . ;
	 . = ALIGN(4);
         __heap_end = . + HEAP_SIZE ;
        } > HEAP 

.stack : {
          . = ALIGN(16);
          *(.stack*). = . ;
          . = ALIGN(16);
          __stack = . + STACK_SIZE ; /* Set stack pointer to the end of region */
         } > STACK

/* remove the debugging information from the standard libraries */
DISCARD : {
           libc.a ( * )
           libm.a ( * )
           libgcc.a ( * )
          }

}
