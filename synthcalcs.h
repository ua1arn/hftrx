/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef SYNTHCACS_H_INCLUDED
#define SYNTHCACS_H_INCLUDED

#include "hardware.h"

#include <stdbool.h>
#include <stdint.h>

extern const phase_t r1_ph; // = SYNTH_R1;

void synth_setreference(
	uint_fast32_t refclock 	/* частота опорника */
);
void synthcalc_init(void);

#define phase_fromulong(o, v) { * (o) = (v); } while (0)
#define phase_fromuint(o, v) { * (o) = (v); } while (0)

void synth_lo4_setfreq(
	uint_fast8_t pathi,	/* номер тракта - 0/1: main/sub */
	int_fast32_t f,		/* частота, которую хотим получить на выходе DDS */
	uint_fast8_t od,	/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	int_fast8_t enable	/* не=0 = разрешение работы гетеродина */
	);
void synth_lo3_setfreq(
	uint_fast8_t pathi,		/* номер тракта - 0/1: main/sub */
	int_fast32_t f,	/* частота, которую хотим получить на выходе DDS */
	uint_fast8_t od	/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	);
void synth_lo2_setfreq(
	uint_fast8_t pathi,		/* номер тракта - 0/1: main/sub */
	int_fast32_t f,	/* частота, которую хотим получить на выходе DDS */
	uint_fast8_t od	/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	);
void synth_lo1_setfreq(
	uint_fast8_t pathi,		/* номер тракта - 0/1: main/sub */
	int_fast32_t f,	/* частота на выходе первого гетеродина */
	uint_fast8_t od	/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	);
// Версия функции, получающая гарантированно положительное значение частоты
void synth_lo1_setfrequ(
	uint_fast8_t pathi,		/* номер тракта - 0/1: main/sub */
	uint_fast32_t f,	/* частота на выходе первого гетеродина (беззнаковое число) */
	uint_fast8_t od	/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	);
// Установка центральной частоты панорамного индикатора
void synth_rts1_setfreq(
	uint_fast8_t pathi,		/* номер тракта - 0/1: main/sub */
	int_fast32_t f	/* частота, которую хотим получить в центре панорамы */
	);
// установка частоты конвертора
void synth_lo0_setfreq(
	int_fast32_t f,	/* частота, которую хотим получить на выходе DDS */
	uint_fast8_t enable		/* номер тракта - 0/1: main/sub */
	);

void synth_lfm_setparams(uint_fast32_t astart, uint_fast32_t astop, uint_fast32_t aspeed, uint_fast8_t od);

/* Получить частоту lo1 из частоты настройки */
uint_fast32_t synth_freq2lo1(
	uint_fast32_t freq,	/* частота на индикаторе устройства */
	uint_fast8_t fi		/* номер тракта: 0/1: main/sub */
	);


#endif /* SYNTHCACS_H_INCLUDED */
