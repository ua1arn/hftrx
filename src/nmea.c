/*
 * nmea.c
 *
 *  Created on: Jul 30, 2026
 *      Author: Gena
 */


#include "hardware.h"
#include "formats.h"
#include "nmea.h"

void nmeaparser_initialize(struct nmeaparser * np)
{
	np->state = NMEAST_INITIALIZED;
}

static unsigned nmeaparser_get_buffsize(struct nmeaparser * np, uint_fast8_t field)
{
	ASSERT(field < NMEA_PARAMS);
//	switch (field)
//	{
//	case NMEA_BIGFIELD:
//		return NMEA_CHARSBIG;
//	default:
//		return NMEA_CHARSSMALL;
//	}
	return NMEA_CHARSSMALL;
}

char * nmeaparser_get_buff(struct nmeaparser * np, uint_fast8_t field)
{
	ASSERT(field < NMEA_PARAMS);
//	switch (field)
//	{
//	case NMEA_BIGFIELD:
//		return np->buffbig;
//	default:
//		return np->buffsmall [field];
//	}
	return np->buffsmall [field];
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
// для дальнейшего приёма надо опять вызывать nmeaparser_initialize
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
		else if (np->param < NMEA_PARAMS && np->chars < (nmeaparser_get_buffsize(np, np->param) - 1))
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
		if (np->checksum == (np->chsval + hex2int(c)))
		{
			return 1;	// принятое сообщение
		}
		np->state = NMEAST_INITIALIZED;	// не совпало - продолжаем приём
		break;

	default:
		break;
	}
	return 0;
}

