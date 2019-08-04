/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef ENCODER_H_INCLUDED
#define ENCODER_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ENCODER_SLOW_STEPS 48	/* шагов на один оборот валкодера на минимальной скорости вращения  */
#define ENCODER_MENU_STEPS 24	/* количество изменений настраиваемого параметра на один оборот валкодера */


int_least16_t getRotateHiRes(uint_fast8_t * jumpsize, uint_fast8_t hiresdiv);	/* получение накопленных значений прерываний от валкодера. накопитель сбрасывается */
int_least16_t getRotateHiRes2(uint_fast8_t * jumpsize);	/* получение накопленных значений прерываний от валкодера. накопитель сбрасывается */
void encoder_clear(void);	/* накопитель сбрасывается */
int getRotateLoRes(uint_fast8_t hiresdiv); /* получение "редуцированного" количества прерываний от валкодера. */
void encoder_initialize(void);
void encoder_pushback(int outsteps, uint_fast8_t hiresdiv);
void encoder_kbdctl(
	uint_fast8_t code, 		// код клавиши
	uint_fast8_t accel		// 0 - одиночное нажатие на клавишу, иначе автоповтор
	);

int encoder_get_snapshot(unsigned * speed, const uint_fast8_t derate);
int encoder2_get_snapshot(unsigned * speed, const uint_fast8_t derate);

void encoder_set_resolution(uint_fast8_t resolution, uint_fast8_t dynamic);	// параметр - делённое на ENCRESSCALE значение.

#define ENCODER_NORMALIZED_RESOLUTION (1440)	// виртуальных импульсов за оборот в секунду - нормализованная скорость
//#define ENCODER_NORMALIZED_RESOLUTION (144)	// виртуальных импульсов за оборот в секунду - нормализованная скорость

#define ENCRESSCALE 4UL


#define ENC_DYNA_MAX 4


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ENCODER_H_INCLUDED */

