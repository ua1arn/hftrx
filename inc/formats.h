/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>	// format specifiers for printing

#include "hardware.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint_fast8_t local_snprintf_P( char * __restrict buffer, uint_fast8_t count, const FLASHMEM char * __restrict format, ... );
// Отладочная печать
void debug_printf_P(const FLASHMEM char * __restrict format, ... );

char * safestrcpy(char * dst, size_t blen, const char * src);

void printhex(unsigned long voffs, const unsigned char * buff, unsigned length);


#if WITHDEBUG
	#define TP() \
		do { \
			static const char this_file [] = __FILE__; \
			PRINTF(PSTR("At %d in %s.\n"), __LINE__, this_file); \
		} while(0)
#else /* WITHDEBUG */
	#define TP() do { } while(0)
#endif /* WITHDEBUG */

#if WITHDEBUG

	#define PRINTF	debug_printf_P
	#define ASSERT(v) do { if ((v) == 0) { \
		PRINTF(PSTR("%s(%d): Assert '%s'\n"), __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

	#define VERIFY(v) do { if ((v) == 0) { \
		PRINTF(PSTR("%s(%d): Verify '%s'\n"), __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

	#define TRACE0(f)		do { PRINTF( PSTR(f)); } while (0)
	#define TRACE1(f,a1)		do { PRINTF( PSTR(f),(a1)); } while (0)
	#define TRACE2(f,a1,a2)		do { PRINTF( PSTR(f),(a1),(a2)); } while (0)
	#define TRACE3(f,a1,a2,a3)	do { PRINTF( PSTR(f),(a1),(a2),(a3)); } while (0)
	#define TRACE4(f,a1,a2,a3,a4)	do { PRINTF( PSTR(f),(a1),(a2),(a3),(a4)); } while (0)
	#define TRACE5(f,a1,a2,a3,a4,a5) do { PRINTF( PSTR(f),(a1),(a2),(a3),(a4),(a5)); } while (0)

#else /* WITHDEBUG */

	#define PRINTF(...)	do {} while (0)
	#define ASSERT(v) ((void) (0))
	#define VERIFY(v) ((void) (v))

	#define TRACE0(f)			do {} while (0)
	#define TRACE1(f,a1)			do {} while (0)
	#define TRACE2(f,a1,a2)			do {} while (0)
	#define TRACE3(f,a1,a2,a3)		do {} while (0)
	#define TRACE4(f,a1,a2,a3,a4)		do {} while (0)
	#define TRACE5(f,a1,a2,a3,a4,a5)		do {} while (0)

#endif /* WITHDEBUG */

#ifdef __cplusplus
}
#endif /* __cplusplus */
