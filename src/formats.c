/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "formats.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// использование библиотечной функции форматного вывода вместо самописной
//#define FORMATFROMLIBRARY (CPUSTYLE_ARM_CM7 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM0 /* || (__CORTEX_A != 0) */)

#if ! FORMATFROMLIBRARY

//#if CPUSTYLE_ATMEGA
//#define FETCH(c, fmt) ((c) = pgm_read_byte((fmt) ++))
//#else
#define FETCH(c, fmt) ((c) = * (fmt) ++)
//#endif

static	char *
uconvert(long unsigned n, uint_fast8_t base, char * s, const FLASHMEM char * dg)
{	do
	{
		//const ldiv_t v = ldiv(n, base);	// AVR GCC: нельзя применять ldiv - при переполнении (числа вроде -1L) неправильно считает

		* -- s = dg[(int) (n % base)];
		n = n / base;
	} while (n != 0);
	return (s);
}

static size_t
safestrlen(register const char * s, size_t len)
{
	size_t n;

	for (n = 0; n < len && * s != '\0'; ++ n, ++ s)
		;
	return n;
}


struct fmt_param
{
	size_t pos;
	size_t count;
	char * buffer;
};

static uint_fast8_t local_isdigit(char c)
{
	//return isdigit((unsigned char) c) != 0;
	return c >= '0' && c <= '9';
}

static int
local_format(void * param, int (* putsub)(void *, int), const FLASHMEM char * pfmt, va_list args)
{
	enum { TMP_S_SIZE = 14 };
	int	rj, altern;
	char c, signc, fillc;
	int maxwidth, width, i;
	union
	{
#ifdef	__NOLONG__
		int	lval;
#else
		signed long 	lval;
		//unsigned long 	ulval;
#endif
		void * pval;
		/* double dval;	*/
	} u;
	char	 	* cp;
	char		s [TMP_S_SIZE + 1];
	int		len;
	static const FLASHMEM char	lcase[17] = { "0123456789abcdef" };
	static const FLASHMEM char	ucase[17] = { "0123456789ABCDEF" };
	static char null_s[] = "(null)";

	u.lval = 0;	/* Supress GCC warning. */
	len = 0;	/* Counter of written characters.	*/
	while (FETCH(c, pfmt) != '\0')
	{	if (c != '%')
		{	if ((* putsub)(param, c) == EOF)
				return -1;
			++ len;
			continue;
		}
		s [TMP_S_SIZE] = '\0';
		rj = 1;
		fillc = ' ';
		signc = '\0';
		altern = 0;
		maxwidth = 255; //10000;

		/*	Parse flags.	*/
		for (FETCH(c, pfmt);; FETCH(c, pfmt))
		{
			switch (c)
			{
			default:
				break;
			case '-':
				rj = 0, fillc = ' '; continue;
			case '0':
				rj = 1, fillc = '0'; continue;
			case '+':
			case ' ':
				signc = c;    continue;
			case '#':
				altern = 1;     continue;
			}
			break;
		}

		/* Parse field width.	*/
		if (c == '*')
		{	width = va_arg(args, int);
			FETCH(c, pfmt);
			if (width < 0)
				rj = 0, width = - width; /* fillc ? */
		}
		else
			for (width = 0; local_isdigit((unsigned char) c); FETCH(c, pfmt))
				width = width * 10 + c - '0';
		/*************************************/

		/*	Has prec.	*/
		if ( c == '.' )
		{       fillc = ' ';
			if ((FETCH(c, pfmt)) == '*')
			{	maxwidth = va_arg(args, int);
				FETCH(c, pfmt);
			}
			else
			{	for (maxwidth = 0 ; local_isdigit((unsigned char) c) ; FETCH(c, pfmt))
					maxwidth = maxwidth*10 + c - '0';
			}
		}
		/*	Parse 'l' and 'h' modifiers.	*/
		switch (c)
		{
		case 'l':
			FETCH(c, pfmt);
			u.lval = va_arg(args, long);
			break;
		case 'h':
			FETCH(c, pfmt);
			u.lval = va_arg(args, /*short*/int);
			break;

		case 'e':
		case 'f':
		case 'g':
#if 0
			u.dval = va_arg(args, double);
			dflag = 1;
#else
			(void) va_arg(args, double);
#endif
			break;
		case 's':
			if ((cp = va_arg(args, char *)) == NULL)
				cp = null_s;
			i = safestrlen(cp, maxwidth);
			/*i = strlen(cp);*/
			goto havelen;
		case 'x':
		case 'X':
		case 'u':
		case 'o':
			u.lval = va_arg(args, unsigned int);
			break;
		case 'p':
			u.pval = (void *) va_arg(args, void *);
			break;
		default:
			u.lval = va_arg(args, int); /* d,o,i,c specs. */
			break;
		}

		switch (c)
		{
		case 'o':
			cp = uconvert(u.lval, 8, s + TMP_S_SIZE, lcase);
			if (u.lval != 0 && altern)
				* -- cp = '0';
			break;
		case 'd':
		case 'i':
			if (u.lval < 0)
			{	cp = uconvert((unsigned long) (- u.lval), 10, s + TMP_S_SIZE, lcase);
				* -- cp = '-';
				break;
			}
		case 'u':
			cp = uconvert(u.lval, 10, s + TMP_S_SIZE, lcase);
			if (signc != '\0')
				* -- cp = signc;
			break;
		case 'x':
			cp = uconvert(u.lval, 16, s + TMP_S_SIZE, lcase);
			if (u.lval != 0 && altern)
			{	* -- cp = 'x';
				* -- cp = '0';
			}
			break;
		case 'X':
			cp = uconvert(u.lval, 16, s + TMP_S_SIZE, ucase);
			if (u.lval != 0 && altern)
			{	* -- cp = 'X';
				* -- cp = '0';
			}
			break;
		case 'p':	/* Pointer print.	*/
			cp = uconvert((unsigned) u.pval, 16, s + TMP_S_SIZE, ucase);
			fillc = '0';
			width = (sizeof (void *)) * 2;
			break;
		case 'e': case 'f': case 'E': case 'g':
			/* .... */
			cp = & s [TMP_S_SIZE];
			continue;
		case 'c':
			c = (char) u.lval;
		default:
			* (cp = s + TMP_S_SIZE - 1) = c;
			break;
		}
		i = (s + TMP_S_SIZE) - cp;

		/* Variables 'i' and 'cp' are setup.	*/
havelen:
		if (i > maxwidth)
			i = maxwidth;
		if (rj)
			while (width -- > i)
			{	if ((* putsub)(param, fillc) == EOF)
					return -1;
				++ len;
			}
		{
			int k;
			for (k = 0; * cp != '\0' && k < (int) maxwidth; ++ k)
			{	if ((* putsub)(param, (unsigned char)(* cp ++)) == EOF)
					return -1;
				++ len;
			}
		}
		if (!rj)
		{
			while (width -- > i)
			{	if ((* putsub)(param, ' ') == EOF)
					return -1;
				++ len;
			}
		}
	}
	return (len);
}


/*	Formatted output to standart output stream.	*/
/*	User-side of console output.			*/

static int 
vsputchar(void * param, int ch)
{
	struct fmt_param * const p = (struct fmt_param *) param;

	if (p->pos + 1 < p->count)
	{	return p->buffer [p->pos ++] = (char) ch;
	}
	else
	{	p->buffer [p->pos] = '\0';
		return EOF;
	}
}

#endif /* ! FORMATFROMLIBRARY */

	// Для архитектуры ATMega определена только эта функция -
	// с расположением форматной строкии в памяти программ.
uint_fast8_t local_snprintf_P( char * __restrict buffer, uint_fast8_t count, const FLASHMEM char * __restrict format, ... )
{
	va_list	ap;
	int n;

#if FORMATFROMLIBRARY
	va_start(ap, format);
	n = vsnprintf(buffer, count, format, ap);
	va_end(ap);
#else /* FORMATFROMLIBRARY */

	struct fmt_param pr;

	pr.buffer = buffer;
	pr.pos = 0;
	pr.count = count;

	va_start(ap, format);
	// использование самописной функции
	n = local_format(& pr, vsputchar, format, ap);		// никогда не возвращается ошибка
	va_end(ap);
	vsputchar(& pr, '\0');
#endif /* FORMATFROMLIBRARY */
	return n == -1 ? count - 1 : n;	// изменено от стандартного поведения = всегда длинну возвращаем.
}

char *
safestrcpy(char * dst, size_t blen, const char * src)
{
	ASSERT(dst != NULL);
	ASSERT(src != NULL);
	ASSERT(strlen(src) < blen);
	return strcpy(dst, src);
}

#if WITHDEBUG

#if FORMATFROMLIBRARY

/*	Formatted output to standart output stream.	*/
/*	User-side of console output.			*/
// использование библиотечной функции (поддержка печати чисел с плавающей точкой).

void debug_printf_P(const FLASHMEM char *__restrict format, ... )
{
	char b [256];
	va_list	ap;
	va_start(ap, format);

	vsnprintf(b, sizeof b / sizeof b [0], format, ap);
	dbg_puts_impl(b);

	va_end(ap);
}

#else /* FORMATFROMLIBRARY */


static int 
dbg_local_putchar(void * param, int ch)
{
	return dbg_putchar(ch);
}

/*	Formatted output to standart output stream.	*/
/*	User-side of console output.			*/
// использование самописной функции

void debug_printf_P(const FLASHMEM char *format, ... )
{
	va_list	ap;
	va_start(ap, format);

	local_format(NULL, dbg_local_putchar, format, ap);		// никогда не возвращается ошибка

	va_end(ap);
}

#endif /* FORMATFROMLIBRARY */



static int
toprintc(int c)
{
	if (c < 0x20 || c >= 0x7f)
		return '.';
	return c;
}

void
printhex(unsigned long voffs, const unsigned char * buff, unsigned length)
{
	unsigned i, j;
	unsigned rows = (length + 15) / 16;

	for (i = 0; i < rows; ++ i)
	{
		const int trl = ((length - 1) - i * 16) % 16 + 1;
		debug_printf_P(PSTR("%08lX "), voffs + i * 16);
		for (j = 0; j < trl; ++ j)
			debug_printf_P(PSTR(" %02X"), buff [i * 16 + j]);

		debug_printf_P(PSTR("%*s"), (16 - trl) * 3, "");

		debug_printf_P(PSTR("  "));
		for (j = 0; j < trl; ++ j)
			debug_printf_P(PSTR("%c"), toprintc(buff [i * 16 + j]));

		debug_printf_P(PSTR("\n"));
	}
}

#else /* WITHDEBUG */

void debug_printf_P(const FLASHMEM char *format, ... )
{
	(void) format;
}

void
printhex(unsigned long voffs, const unsigned char * buff, unsigned length)
{
}
#endif /* WITHDEBUG */
