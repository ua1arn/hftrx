/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if ! LINUX_SUBSYSTEM

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "formats.h"	/* Отладочная печать */

#include <sys/stat.h>
#include <sys/unistd.h>
#include <sys/reent.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>

#if (CPUSTYLE_RISCV || defined(__aarch64__))

/**
  \brief   Initializes data and bss sections
  \details This default implementations initialized all data and additional bss
           sections relying on .copy.table and .zero.table specified properly
           in the used linker script.

 */
/* fill .bss and prepare constant data */
void addr64_preparedata(void)
{
  typedef struct {
    volatile uint32_t const* src;
    volatile uint32_t* dest;
    ptrdiff_t  wlen;
  } __copy_table_t;

  typedef struct {
	volatile uint32_t* dest;
    ptrdiff_t  wlen;
  } __zero_table_t;

  extern const __copy_table_t __copy_table_start64__;
  extern const __copy_table_t __copy_table_end64__;
  extern const __zero_table_t __zero_table_start64__;
  extern const __zero_table_t __zero_table_end64__;

  for (__copy_table_t const* pTable = &__copy_table_start64__; pTable < &__copy_table_end64__; ++pTable) {
    for(ptrdiff_t i=0u; i<pTable->wlen; ++i) {
      pTable->dest[i] = pTable->src[i];
    }
  }

  for (__zero_table_t const* pTable = &__zero_table_start64__; pTable < &__zero_table_end64__; ++pTable) {
    for(ptrdiff_t i=0u; i<pTable->wlen; ++i) {
      pTable->dest[i] = 0u;
    }
  }
}
#endif /* CPUSTYLE_RISCV || defined(__aarch64__) */

// Используется в случае наличия ключа ld -nostartfiles
// Так же смотреть вокруг software_init_hook
// see https://github.com/gmarkall/newlib/blob/de33102df7309e77441dfc76aa8cdef6d26da209/newlib/libc/sys/arm/crt0.S#L1

#if ! WITHRTTHREAD

extern int main(void);
extern void __libc_init_array(void);

void __NO_RETURN _start(void)
{
	__libc_init_array();	// invoke constructors
    /* Branch to main function */
    main();
     /* Infinite loop */
	for (;;)
		;
}
#endif /* ! WITHRTTHREAD */

// call after __preinit_array_xxx and before __init_array_xxx passing
void _init(void)
{
}

void * __dso_handle;

void _fini(void)
{
	for (;;)
		;
}

void _exit(int code)
{
	for (;;)
		;
}

// See
// https://github.com/littlekernel/newlib/blob/master/libgloss/arm/crt0.S

// User mode only:           This routine makes default target specific Stack

void _stack_init(void)
{

}

static int SER_GetChar(void)
{

	return (-1);
}

int __attribute__((used)) (_open)(const char * path, int flags, ...)
{
	return (-1);
}

int __attribute__((used)) (_close)(int fd) {
	return (-1);
}

int __attribute__((used)) (_unlink)(const char * path) {
	return (-1);
}

int __attribute__((used)) (_lseek)(int fd, int ptr, int dir)
{
	return (0);
}

int __attribute__((used)) (_stat)(const char *__restrict path, struct stat *__restrict st)
{
	memset(st, 0, sizeof(*st));
	st->st_mode = S_IFCHR;
	return (0);
}

int __attribute__((used)) (_fstat)(int fd, struct stat *st )
{
	memset(st, 0, sizeof(*st));
	st->st_mode = S_IFCHR;
	return (0);
}

int __attribute__((used)) (_isatty)(int fd)
{
	return (1);
}

int __attribute__((used)) (_kill)(int id)
{
	return (-1);
}

int __attribute__((used)) (_getpid)(int id)
{
	return (-1);
}

_READ_WRITE_RETURN_TYPE __attribute__((used)) (_read)(int fd, char * ptr, size_t len) {
	char c;
	int i;

	for (i = 0; i < len; i++) {
		c = SER_GetChar();
		if (c == 0x0D)
			break;
		((char *) ptr) [i] = c;
		while (dbg_putchar(c) == 0)
			;
	}
	return (len - i);
}

_READ_WRITE_RETURN_TYPE __attribute__((used)) (_write)(int fd, const void * ptr, size_t len)
{
	int i;

	for (i = 0; i < len; i++)
	{
		const char c = ((const char *) ptr) [i];
		dbg_writechar(c);
	}
	return (i);
}


#if defined(__aarch64__) && 0

// Fix dc zva, xx opcode usage
void * memset(void * dst, int v, size_t n)
{
	volatile uint8_t * d = (volatile uint8_t *) dst;
	while (n --)
		* d ++ = v;
	return dst;
}

void * memcpy(void * dst, const void * src, size_t n)
{
	volatile uint8_t * d = (volatile uint8_t *) dst;
	const volatile uint8_t * s = (const volatile uint8_t *) src;
	while (n --)
		* d ++ = * s ++;
	return dst;
}

void * memmove(void * dst, const void * src, size_t n)
{
	volatile uint8_t * d = (volatile uint8_t *) dst;
	const volatile uint8_t * s = (const volatile uint8_t *) src;
	if (s >= d && s < (d + n))
	{
		// If overlaps
		s += n;
		d += n;
		while (n --)
			* -- d = * -- s;
	}
	else
	{
		while (n --)
			* d ++ = * s ++;
	}
	return dst;
}

size_t strlen(const char * s1)
{
	volatile const char * s = s1;
	size_t n = 0;
	while (* s ++ != '\0')
		++ n;
	return n;
}
#endif /* defined(__aarch64__) */

#if 0

static LCLSPINLOCK_t lockmalloc = LCLSPINLOCK_INIT;

void __malloc_lock(struct _reent * reent)
{
	PRINTF("%s: %p core%u\n", __func__, reent, board_dpc_coreid());
	//LCLSPIN_LOCK(& lockmalloc);
}

void __malloc_unlock(struct _reent * reent)
{
	PRINTF("%s: %p core%u\n", __func__, reent, board_dpc_coreid());
	//LCLSPIN_UNLOCK(& lockmalloc);
}

#endif

#if 0
#if defined __SINGLE_THREAD__
	#warning Have __SINGLE_THREAD__
#endif
#if defined __DYNAMIC_REENT__
	#warning Have __DYNAMIC_REENT__
#endif


// Check __DYNAMIC_REENT__ and __SINGLE_THREAD__
struct _reent * __getreent(void)
{
    static struct _reent r [16];
    PRINTF("__getreent: CPU%u\n", arm_hardware_cpuid());
    return r + arm_hardware_cpuid();
}

/* Make sure that Newlib was compiled with retargetable locking support. */
#ifndef _RETARGETABLE_LOCKING
#error "Newlib must be compiled with retargetable locking support"
#endif

struct custom_lock
{
	LCLSPINLOCK_t lock;
	IRQL_t oldIrql;
};

/* Static locks */
struct custom_lock __lock___malloc_recursive_mutex;		// Используется ld и при работе программы
struct custom_lock __lock___env_recursive_mutex;		// Используется ld
struct custom_lock __lock___sfp_recursive_mutex;		// Используется ld
struct custom_lock __lock___sinit_recursive_mutex;
struct custom_lock __lock___atexit_recursive_mutex;

struct custom_lock __lock___at_quick_exit_mutex;
struct custom_lock __lock___tz_mutex;
struct custom_lock __lock___dd_hash_mutex;
struct custom_lock __lock___arc4random_mutex;

// 	Allocate lock related resources.
void __retarget_lock_init(_LOCK_T *lock)
{
	struct custom_lock * * const lpp = (struct custom_lock * *) lock;
	PRINTF("%s: %p core%u\n", __func__, lpp, board_dpc_coreid());
	LCLSPINLOCK_INITIALIZE(& (* lpp)->lock);
}

// Allocate recursive lock related resources.
void __retarget_lock_init_recursive(_LOCK_T *lock)
{
	struct custom_lock * * const lpp = (struct custom_lock * *) lock;
	PRINTF("%s: %p core%u\n", __func__, lpp, board_dpc_coreid());
	LCLSPINLOCK_INITIALIZE(& (* lpp)->lock);
}

// Free lock related resources.
void __retarget_lock_close(_LOCK_T lock)
{
	struct custom_lock * const lp = (struct custom_lock *) lock;
	PRINTF("%s: %p core%u\n", __func__, lp, board_dpc_coreid());
	LCLSPINLOCK_UNINITIALIZE((* lpp)->lock);
}

// Free recursive lock related resources.
void __retarget_lock_close_recursive(_LOCK_T lock)
{
	struct custom_lock * const lp = (struct custom_lock *) lock;
	PRINTF("%s: %p core%u\n", __func__, lp, board_dpc_coreid());
	LCLSPINLOCK_UNINITIALIZE((* lpp)->lock);
}

// Acquire lock immediately after the lock object is available.
void __retarget_lock_acquire(_LOCK_T lock)
{
	struct custom_lock * const lp = (struct custom_lock *) lock;
	PRINTF("%s: %p core%u\n", __func__, lp, board_dpc_coreid());
	LCLSPIN_LOCK(& lp->lock);
}

// Acquire recursive lock immediately after the lock object is available.
void __retarget_lock_acquire_recursive(_LOCK_T lock)
{
	struct custom_lock * const lp = (struct custom_lock *) lock;
	PRINTF("%s: %p core%u\n", __func__, lp, board_dpc_coreid());
	//LCLSPIN_LOCK(& lp->lock);
}

// Acquire lock if the lock object is available.
int __retarget_lock_try_acquire(_LOCK_T lock)
{
	struct custom_lock * const lp = (struct custom_lock *) lock;
	PRINTF("%s: %p core%u\n", __func__, lp, board_dpc_coreid());
	return 0;
}

// Acquire recursive lock if the lock object is available.
int __retarget_lock_try_acquire_recursive(_LOCK_T lock)
{
	struct custom_lock * const lp = (struct custom_lock *) lock;
	PRINTF("%s: %p core%u\n", __func__, lp, board_dpc_coreid());
	return 0;
}

// Relinquish the lock ownership.
void __retarget_lock_release(_LOCK_T lock)
{
	struct custom_lock * const lp = (struct custom_lock *) lock;
	PRINTF("%s: %p core%u\n", __func__, lp, board_dpc_coreid());
	LCLSPIN_UNLOCK(& lp->lock);
}

// Relinquish the recursive lock ownership.
void __retarget_lock_release_recursive(_LOCK_T lock)
{
	struct custom_lock * const lp = (struct custom_lock *) lock;
	PRINTF("%s: %p core%u\n", __func__, lp, board_dpc_coreid());
	//LCLSPIN_UNLOCK(& lp->lock);
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ! LINUX_SUBSYSTEM */

