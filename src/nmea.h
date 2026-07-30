/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef SRC_NMEA_H_
#define SRC_NMEA_H_


#include <stdint.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum nmeaparser_states
{
	NMEAST_INITIALIZED,
	NMEAST_OPENED,	// встретился символ '$'
	NMEAST_CHSHI,	// прём старшего символа контрольной суммы
	NMEAST_CHSLO,	// приём младшего символа контрольной суммы


	//
	NMEAST_COUNTSTATES

};

struct nmeaparser
{

	uint_fast8_t state;// = NMEAST_INITIALIZED;
	uint_fast8_t checksum;
	uint_fast8_t chsval;
	uint_fast8_t param;		// номер принимаемого параметра в строке
	uint_fast8_t chars;		// количество символов, помещённых в буфер

	uint_fast8_t nmea_params;
	uint_fast8_t nmea_charssmall;
	char  * buff;
};

void nmeaparser_initialize(struct nmeaparser * np, unsigned nparams, unsigned fieldsize, char * buff);
uint_fast8_t nmeaparser_onrxchar(struct nmeaparser * np, uint_fast8_t c);
char * nmeaparser_get_buff(const struct nmeaparser * np, uint_fast8_t field);
void nmeaparser_reset(struct nmeaparser * np);
size_t nmeaparser_rebuild(const struct nmeaparser * np, char * buff, size_t len);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SRC_NMEA_H_ */
