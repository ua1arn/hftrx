 0x00, 0x00, 0x50, 0x02, 0x50, 0x00, 0x40, 0x00,
 0x36, 0x41, 0x00, 0x66, 0x92, 0x05, 0xA2, 0xA0,
 0x0D, 0x65, 0xFF, 0xFF, 0x81, 0xFB, 0xFF, 0x0C,
 0x29, 0xC0, 0x20, 0x00, 0xA2, 0x28, 0x1F, 0x97,
 0x0A, 0xF6, 0xC0, 0x20, 0x00, 0x29, 0x08, 0x1D,
 0xF0, 0x00, 0x00, 0x00, 0x36, 0x41, 0x00, 0x46,
 0x01, 0x00, 0x00, 0x00, 0x00, 0x25, 0xFD, 0xFF,
 0x1B, 0x22, 0x0B, 0x82, 0xA2, 0x08, 0x00, 0x66,
 0x9A, 0xF2, 0x1D, 0xF0, 0x36, 0x41, 0x00, 0xA1,
 0xEF, 0xFF, 0x25, 0xFE, 0xFF, 0x06, 0xFF, 0xFF,
 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x57,
 0x6F, 0x72, 0x64, 0x20, 0x28, 0x48, 0x69, 0x46,
 0x69, 0x34, 0x20, 0x44, 0x53, 0x50, 0x29, 0x21,
 0x0A, 0x00, 0x00, 0x00, 0x78, 0x00, 0x40, 0x00,
 0x78, 0x00, 0x40, 0x00,

// crt1.S

// Sets up the C calling environment (sets up stack, clears BSS sections,
// calls __clibrary_init) and then calls main(). Control arrives here from
// the reset handler.

// Copyright (c) 1998-2018 Cadence Design Systems, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#if defined (SIMULATOR)
#include <xtensa/simboard.h>
#include <xtensa/simcall.h>
#endif

#include <xtensa/coreasm.h>
#include "xtos-internal.h"


	// Exports
	.global	_start
	.type	_start,@function

	// External Functions
	//
	// __clibrary_init		from C library
	// _init			from C library
	// _fini			from C library
	// _init_reent_bss		from C library
	// main				from user code
	// exit				from C library
	// board_init			from BSP/user code
	// xthal_dcache_all_writeback	from HAL
	// __memmap_init		(optional) user code
	// __bss_init			(optional) user code
	// __stack_init			(optional) user code

	// External Symbols/Variables
	//
	// __stack			from linker script or user code
	// _bss_table_start		from linker script
	// _bss_table_end		from linker script
	// __xtos_simc			from linker script or user code


// Macros to abstract away ABI differences

#if (defined __XTENSA_CALL0_ABI__)
# define CALL	call0
# define CALLX	callx0
# define ARG1	a2	/* 1st outgoing call argument */
# define ARG2	a3	/* 2nd outgoing call argument */
# define ARG3	a4	/* 3rd outgoing call argument */
# define ARG4	a5	/* 4th outgoing call argument */
# define ARG5	a6	/* 5th outgoing call argument */
#elif XCHAL_HAVE_XEA3
# define CALL	call8
# define CALLX	callx8
# define ARG1	a10	/* 1st outgoing call argument */
# define ARG2	a11	/* 2nd outgoing call argument */
# define ARG3	a12	/* 3rd outgoing call argument */
# define ARG4	a13	/* 4th outgoing call argument */
# define ARG5	a14	/* 5th outgoing call argument */
#else
# define CALL	call4
# define CALLX	callx4
# define ARG1	a6	/* 1st outgoing call argument */
# define ARG2	a7	/* 2nd outgoing call argument */
# define ARG3	a8	/* 3rd outgoing call argument */
# define ARG4	a9	/* 4th outgoing call argument */
# define ARG5	a10	/* 5th outgoing call argument */
#endif

	.data
#if !defined (SIMULATOR)
	.weak	_start_argc
	.weak	_start_argv

	.align	4
_start_argc:
	.word	1		// one argument (program name)
_start_argv:
	.word	_start_null	// empty program name
#endif

	.weak	_start_envp

	.align	4
_start_envp:
_start_null:
	.word	0		// end of argv array, empty string


	.text
	.align	4

_start:
	//  _start is typically NOT at the beginning of the text segment --
	//  it is always called from either the reset vector or other code
	//  that does equivalent initialization (such as crt0-app.S).
	//
	//  Assumptions on entry to _start:
	//	- (XEA1/XEA2) low (level-one) and medium priority interrupts are
	//	  disabled via PS.INTLEVEL and/or INTENABLE (PS.INTLEVEL is expected
	//	  to be zeroed, to potentially enable them, before calling main)
	//	- (XEA3) all interrupts are disabled via PS.DI. PS.DI will be cleared
	//	  before calling main.
	//	- C calling context not initialized:
	//	  - PS not initialized
	//	  - SP not initialized
	//	- the following are initialized:
	//	  - LITBASE, cache attributes, WindowBase, WindowStart,
	//	    CPENABLE, FP's FCR and FSR, EXCSAVE[n]

	// Keep a0 zero.  It is used to initialize a few things.
	// It is also the return address, where zero indicates
	// that the frame used by _start is the bottommost frame.
      
		
	movi	a0, 0			// keep this register zero.
#if 0
        movi a3, 0x11221111
	movi a7, 0x020008
	s32i  a3, a7, 0
	memw
	j .
#endif
	        
// This is never needed in normal operation. If for some reason you want to reset
// the extra state, include this file. Note reset-unneeded.S is not certified.
// parasoft-begin-suppress MISRA2012-DIR-4_4 "Not code".
//  #if XTOS_RESET_UNNEEDED
//  #include "reset-unneeded.S"
//  #endif
// parasoft-end-suppress MISRA2012-DIR-4_4 "Not code".

	// Set up the stack pointer. It is possible to override the default
	// stack location by providing a user hook __stack_init. This hook
	// must not rely on stack and BSS availability.

	.weak	__stack_init
	movi	a2, __stack_init	// optional user hook
	movi	a1, __stack		// default SP
	movi	sp, __stack
#if 0
        movi a3, 0x11221111
	movi a7, 0x020004
	s32i  a3, a7, 0
	memw
	j .
#endif
	beqz	a2, 1f
	callx0	a2
	movi	a0, 0			// keep this register zero
#if 0
        movi a3, 0x11224444
	movi a7, 0x020000
	s32i  a3, a7, 0
	memw
	j .
#endif
1:
        
#if XCHAL_HAVE_ISL
	// If the Interrupt Stack Limit register exists, then set up stack limit
	// for XTOS. This can be overridden later by applications if needed.
	compute_stack_limit	a3 a4 a5
	wsr.isl	a3
#endif
        
#if defined (SIMULATOR)
	// Reserve stack space for
	//    - argv array
	//    - argument strings

	movi	a2, SYS_iss_argv_size
	simcall				// returns size of argv[] + its strings in a2

	// The stack only needs 16-byte alignment.
	// However, here we round up the argv size further to 128 byte multiples
	// so that in most cases, variations in argv[0]'s path do not result in
	// different stack allocation.  Otherwise, such variations can impact
	// execution timing (eg. due to cache effects etc) for the same code and data.
	// If we have an outbound bus, it's more likely there's enough memory.

# if XCHAL_HAVE_PIF
	addi	a2, a2, 127
	srli	a2, a2, 7
	slli	a2, a2, 7
# else
	// Keep stack 16-byte aligned.
	addi	a2, a2, 15
	srli	a2, a2, 4
	slli	a2, a2, 4
# endif

	// No need to use MOVSP because we have no caller (we're the base caller).
	// In fact it's better not to use MOVSP in this context, to avoid unnecessary
	// ALLOCA exceptions and copying from undefined memory.

	sub	a1, a1, a2
#endif	// SIMULATOR

	// Now that the stack is set, we can set PS for application execution.
	// (User vector mode, enable interrupts, enable window exceptions).

#if XCHAL_HAVE_EXCEPTIONS
# if XCHAL_HAVE_XEA3
	movi	a3, PS_STACK_FIRSTINT	// PS.STK = FirstInt, PS.DI = 0
	wsr.ps	a3
# else
	movi	a3, PS_UM|PS_WOE_ABI	// PS.WOE = 0|1, PS.UM = 1, PS.EXCM = 0, PS.INTLEVEL = 0
	wsr.ps	a3
# endif
	rsync
#endif

	// Do any initialization that affects the memory map, such as setting
	// up TLB entries, that needs to be done before we can successfully
	// clear BSS (e.g. if some BSS segments are in remapped areas).
	//
	// NOTE: This hook works where the reset vector does not unpack segments
	// (see "ROM packing" in the LSP manual), or where unpacking of segments
	// is not affected by memory remapping. If ROM unpacking is affected,
	// TLB setup must be done in assembler from the reset handler.
	//
	// The __memmap_init() routine can be a C function, however it does not
	// have BSS initialized! In particular, __memmap_init() cannot set BSS
	// variables, i.e. uninitialized global variables (they'll be wiped out
	// by the following BSS clear), nor can it assume they are yet initialized
	// to zero.

	.weak	__memmap_init
	movi	a4, __memmap_init	// optional memory map init hook
	beqz	a4, 1f
	CALLX	a4
	movi	a0, 0
1:
      
#if !XCHAL_HAVE_BOOTLOADER

	// Clear the BSS segments. This code supports multiple BSS areas.
         
	movi	a6, _bss_table_start
	movi	a7, _bss_table_end
	bgeu	a6, a7, .Lnobss		// No BSS areas if a6 >= a7

	// If there is an optional BSS init routine provided, we will call
	// it instead of clearing BSS here. This routine can be a C function,
	// but it must be written to be able to work with BSS not initialized.

	.weak	__bss_init
	movi	a4, __bss_init
	beqz	a4, 2f
	movi	ARG1, _bss_table_start
	movi	ARG2, _bss_table_end
	CALLX	a4
	movi	a0, 0
#if 0	
	movi a3, _bss_table_start
	movi a7, 0x020010
	s32i  a3, a7, 0
	memw

	movi a3, _bss_table_end
	movi a7, 0x020014
	s32i  a3, a7, 0
	memw

	movi a3, __bss_init
	movi a7, 0x020018
	s32i  a3, a7, 0
	memw
        
        j .
#endif
	j	.Lnobss			// skip default BSS init code
2:
        
        
	// If running on simulator and there is simcall support for memset,
	// use it. Else clear memory the hard way.

#if defined (SIMULATOR) && (XCHAL_HW_MAX_VERSION > XTENSA_HWVERSION_RB_2007_2)
.Lbssloop:
	movi	a2, SYS_memset
	l32i	a3, a6, 0		// arg1 = fill start address
	movi	a4, 0			// arg2 = fill pattern
	l32i	a5, a6, 4		// get end address
	addi	a6, a6, 8		// next bss table entry
	sub	a5, a5, a3		// arg3 = fill size in bytes
	simcall				// memset(a3,a4,a5)
	bltu	a6, a7, .Lbssloop	// loop until end of bss table
#else
.L0zte:
	l32i	a8, a6, 0		// get start address, assumed multiple of 4
	l32i	a9, a6, 4		// get end address, assumed multiple of 4
	addi	a6, a6, 8		// next entry
	sub	a10, a9, a8		// a10 = length, assumed a multiple of 4
	bbci.l	a10, 2, .L1zte
	s32i	a0, a8, 0		// clear 4 bytes to make length multiple of 8
	addi	a8, a8, 4
.L1zte: bbci.l	a10, 3, .L2zte
	s32i	a0, a8, 0		// clear 8 bytes to make length multiple of 16
	s32i	a0, a8, 4
	addi	a8, a8, 8
.L2zte: srli	a10, a10, 4		// length is now multiple of 16, divide by 16
	floopnez	a10, clearzte
	s32i	a0, a8,  0		// clear 16 bytes at a time...
	s32i	a0, a8,  4
	s32i	a0, a8,  8
	s32i	a0, a8, 12
	addi	a8, a8, 16
	floopend	a10, clearzte
	bltu	a6, a7, .L0zte		// loop until end of table of *.bss sections
.L3zte:
#endif	// SIMULATOR

.Lnobss:

#endif	// XCHAL_HAVE_BOOTLOADER
       
#if XCHAL_HAVE_HALT

	// Assume minimalist environment for memory-constrained cores.
	// No C library or board initialization, no parameters passed to main
	// (assume declared as "void main(void)") and no call to exit().

	CALL	main
	halt

#else

	// Call __clibrary_init to initialize the C library:
	//
	// void __clibrary_init(int argc, char ** argv, char ** environ,
	//	void(*init_func)(void), void(*fini_func)(void));

#if defined (SIMULATOR)
	// Get argv with the arguments from the ISS
	movi	a2, SYS_iss_set_argv
	mov	a3, a1			// tell simcall where to write argv[]
	simcall				// write argv[] array at a3
	movi	a2, SYS_iss_argc
	simcall				// put argc in a2
# if defined (__XTENSA_CALL0_ABI__)
	mov	a12, a2			// save argc (a2 is ARG1)
# else
	mov	ARG1, a2		// argc
# endif
	mov	ARG2, a1		// argv
#else
	// Call board-specific BSP or user provided init function
	
	CALL	board_init
	movi	ARG1, _start_argc
	movi	ARG2, _start_argv	// argv = ["", 0]
	l32i	ARG1, ARG1, 0		// argc = 1
#endif	// SIMULATOR
        
#if XCHAL_HAVE_PRID && (XSHAL_CLIB == XTHAL_CLIB_XCLIB)
	.weak	__xtos_simc

	movi	a4, __xtos_simc		// externally defined symbol
	rsr.prid  a5
#if 0
	movi a3, __xtos_simc
	movi a7, 0x020010
	s32i  a3, a7, 0
	memw
	movi a7, 0x020014
	s32i  a5, a7, 0
        movi a3, _init_reent_bss
	movi a7, 0x020018
	s32i  a3, a7, 0
	j .
#endif
	beqz	a4, 1f
	extui	a5, a5, PRID_ID_SHIFT, PRID_ID_BITS
	
	beqz	a5, 1f			// core 0, skip ahead
	CALL	_init_reent_bss		// limited init for core != core0
	
	movi 	ARG3, _start_envp	// envp
	movi 	ARG4, _init		// _init
	movi	ARG5, _fini		// _fini
	
	CALL	__clibrary_init

	j	.Lmain
1:
#endif
        j	.Lmain
	movi 	ARG3, _start_envp	// envp
	movi 	ARG4, _init		// _init
	movi	ARG5, _fini		// _fini
#if 1
        movi a3, _start_envp
	movi a7, 0x020010
	s32i  a3, a7, 0
	memw
	movi a3, _init
	movi a7, 0x020014
	s32i  a3, a7, 0
	memw
	movi a3, _fini
	movi a7, 0x020018
	s32i  a3, a7, 0
	memw
	
#endif
	CALL	__clibrary_init
#if 1	
        movi a3, 0x11223344
	movi a7, 0x020020
	s32i  a3, a7, 0
	memw
	j .
#endif
.Lmain:
	// Call: int main(int argc, char ** argv, char ** environ);

#if defined (SIMULATOR)
# if defined (__XTENSA_CALL0_ABI__)
	mov 	ARG1, a12		// argc
# else
	mov	ARG1, a2		// argc
# endif
	mov	ARG2, a1		// argv
#else
	movi	ARG1, _start_argc
	movi	ARG2, _start_argv	// argv = ["", 0]
	l32i	ARG1, ARG1, 0		// argc = 1
#endif	// SIMULATOR

	movi	ARG3, _start_envp	// envp
#if 0
	movi a3, 0x11223355
	movi a7, 0x020010
	s32i  a3, a7, 0
	memw
	j   .
#endif
	CALL	main

#if XCHAL_HAVE_PRID && (XSHAL_CLIB == XTHAL_CLIB_XCLIB)
	movi	a4, __xtos_simc		// externally defined symbol
	rsr.prid  a5
	beqz	a4, 1f
	extui	a5, a5, PRID_ID_SHIFT, PRID_ID_BITS
	beqz	a5, 1f			// core 0, skip ahead
	CALL	_exit			// skip exit processing for core != core0
1:
#endif
	// The return value is the same register as the first outgoing argument.

	CALL	exit			// exit with main's return value

	// Does not return here.

#endif	// XCHAL_HAVE_HALT

	.size	_start, . - _start

