/*
 * nmea.h
 *
 *  Created on: Jul 30, 2026
 *      Author: Gena
 */

#ifndef SRC_NMEA_H_
#define SRC_NMEA_H_


#include <stdint.h>
#include <ctype.h>

enum nmeaparser_states
{
	NMEAST_INITIALIZED,
	NMEAST_OPENED,	// встретился символ '$'
	NMEAST_CHSHI,	// прём старшего символа контрольной суммы
	NMEAST_CHSLO,	// приём младшего символа контрольной суммы


	//
	NMEAST_COUNTSTATES

};

#define NMEA_CHARSSMALL		24
//#define NMEA_CHARSBIG		257
//#define NMEA_BIGFIELD		255	// номер большого поля

#if WITHAUTOTUNER_UA1CEI
enum
{
	//	ответ:
	NMF_CODE, //	$ANSW,

	NMF_STATE, //	состояние устройства
	NMF_FWD, //	V_FWD, //ADC датчик апрямой волны
	NMF_REF, //	V_REF, //ADC датчика отраженной волны
	NMF_C_SENS, //	C_SENS, //ADC датчика тока ACS712
	NMF_12V_SENS, //	U_SENS, //ADC входного напряжения питания 12V
	NMF_T_SENS, //	T_SENS, //ADC датчика температуры LM235

	NMEA_PARAMS
};
#else
enum
{
	//	ответ:
	NMF_CODE, //	$ANSW,

	P_POS,
	P_STATE,

	NMEA_PARAMS
};
#endif

struct nmeaparser
{

	uint_fast8_t state;// = NMEAST_INITIALIZED;
	uint_fast8_t checksum;
	uint_fast8_t chsval;
	uint_fast8_t param;		// номер принимаемого параметра в строке
	uint_fast8_t chars;		// количество символов, помещённых в буфер

	char buffsmall [NMEA_PARAMS] [NMEA_CHARSSMALL];
//	char buffbig [NMEA_CHARSBIG];

};

void nmeaparser_initialize(struct nmeaparser * np);
uint_fast8_t nmeaparser_onrxchar(struct nmeaparser * np, uint_fast8_t c);
char * nmeaparser_get_buff(struct nmeaparser * np, uint_fast8_t field);


#endif /* SRC_NMEA_H_ */
