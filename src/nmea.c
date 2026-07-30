/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#include "hardware.h"
#include "formats.h"
#include "nmea.h"

void nmeaparser_initialize(struct nmeaparser * np, unsigned nparams, unsigned fieldsize, char * buff)
{
	np->nmea_params = nparams;
	np->nmea_charssmall = fieldsize;
	np->buff = buff;
	nmeaparser_reset(np);
}

void nmeaparser_reset(struct nmeaparser * np)
{
	np->state = NMEAST_INITIALIZED;

}

static unsigned nmeaparser_get_buffsize(struct nmeaparser * np, uint_fast8_t field)
{
	ASSERT(field < np->nmea_params);
	return np->nmea_charssmall;
}

char * nmeaparser_get_buff(const struct nmeaparser * np, uint_fast8_t field)
{
	ASSERT(field < np->nmea_params);
	return & np->buff [field * np->nmea_charssmall];
}

static uint_fast8_t hex2int(uint_fast8_t c)
{
	if (isdigit((unsigned char) c))
		return c - '0';
	if (isupper((unsigned char) c))
		return c - 'A' + 10;
	if (islower((unsigned char) c))
		return c - 'a' + 10;
	return 0;
}

/* вызывается из обработчика прерываний */
// принятый символ с последовательного порта
// возврат не-0 после принятого сообщения с правильной контрольной суммой.
uint_fast8_t nmeaparser_onrxchar(struct nmeaparser * np, uint_fast8_t c)
{
	switch (np->state)
	{
	case NMEAST_INITIALIZED:
		if (c == '$')
		{
			np->checksum = '*';
			np->state = NMEAST_OPENED;
			np->param = 0;		// номер принимаемого параметра в строке
			np->chars = 0;		// количество символов, помещённых в буфер
		}
		break;

	case NMEAST_OPENED:
		np->checksum ^= c;
		if (c == ',')
		{
			// закрываем буфер параметра, переходим к следующему параметру
			nmeaparser_get_buff(np, np->param) [np->chars] = '\0';
			np->param += 1;
			np->chars = 0;
		}
		else if (c == '*')
		{
			// закрываем буфер параметра, переходим к следующему параметру
			nmeaparser_get_buff(np, np->param) [np->chars] = '\0';
			np->param += 1;
			// переходим к приёму контрольной суммы
			np->state = NMEAST_CHSHI;
		}
		else if (np->param < np->nmea_params && np->chars < (nmeaparser_get_buffsize(np, np->param) - 1))
		{
			nmeaparser_get_buff(np, np->param) [np->chars] = c;
			np->chars += 1;
			//stat_l1 = stat_l1 > np->chars ? stat_l1 : np->chars;
		}
		else
			np->state = NMEAST_INITIALIZED;	// при ошибках формата строки
		break;

	case NMEAST_CHSHI:
		np->chsval = hex2int(c) * 16;
		np->state = NMEAST_CHSLO;
		break;

	case NMEAST_CHSLO:
		np->state = NMEAST_INITIALIZED;	// не совпало - продолжаем приём
		if (np->checksum == (np->chsval + hex2int(c)))
		{
			return 1;	// принятое сообщение
		}
		break;

	default:
		break;
	}
	return 0;
}

static uint_fast8_t npeaparser_calcxorv(
	const char * s,
	size_t len
	)
{
	unsigned char r = '*';
	while (len --)
		r ^= (unsigned char) * s ++;
	return r & 0xff;
}

size_t nmeaparser_rebuild(const struct nmeaparser * np, char * buff, size_t len)
{
	//static const char hex [] = "0123456789ABCDEF";
	size_t n = 0;
	unsigned field;
	if (np->param == 0)
		return 0;
	n += local_snprintf_P(buff + n, len - n, "$%s", nmeaparser_get_buff(np, 0));
	for (field = 1; field < np->param; ++ field)
	{
		n += local_snprintf_P(buff + n, len - n, ",%s", nmeaparser_get_buff(np, field));
	}
	const unsigned xorv = npeaparser_calcxorv(buff + 1, n - 1);
	n += local_snprintf_P(buff + n, len - n, "*%02X\r\n", xorv);

	return n;
}
