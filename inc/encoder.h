/* $Id$ */
//
// Rotary encoders support
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

#define HISTLEN 4		// кое-где дальше есть код, в неявном виде использующий это значение
#define TICKSMAX NTICKS(125)

typedef struct encoder_tag
{
	int_least16_t position;	// обновляется по прерыванию
	int_least16_t backup_position;	// остаток после деления разрашения
	uint8_t old_val;
	uint_fast8_t (* getpins)(void);
	IRQLSPINLOCK_t enclock;
	int reverse;
	/* locked by encspeedlock */
	int_least16_t rotate;
	int_least16_t backup_rotate;	// остаток после деления разрашения
} encoder_t;

void encoder_initialize(encoder_t * e, uint_fast8_t (* agetpins)(void));
int_least16_t
encoder_get_snapshot(
	encoder_t * e,
	uint_fast8_t derate
	);
int_least16_t
encoder_get_delta(
	encoder_t * e,
	const uint_fast8_t derate
	);
int_least16_t
encoder_get_snapshotproportional(
	encoder_t * e,
	unsigned * speed,
	const uint_fast8_t derate
	);

int_least16_t getRotateHiRes_A(uint_fast8_t * jumpsize, uint_fast8_t hiresdiv);	/* получение накопленных значений прерываний от валкодера. накопитель сбрасывается */
int_least16_t getRotateHiRes_B(uint_fast8_t * jumpsize, uint_fast8_t loresdiv);	/* получение накопленных значений прерываний от валкодера. накопитель сбрасывается */

void encoders_clear(void);	/* накопитель сбрасывается */

int_least16_t getRotateLoRes_A(uint_fast8_t hiresdiv); /* получение "редуцированного" количества прерываний от валкодера #1. */
int_least16_t getRotateLoRes_B(uint_fast8_t hiresdiv); /* получение "редуцированного" количества прерываний от валкодера #2. */

void encoders_initialize(void);
void encoderA_pushback(int outsteps, uint_fast8_t hiresdiv);
void encoder_kbdctl(
	uint_fast8_t code, 		// код клавиши
	uint_fast8_t accel		// 0 - одиночное нажатие на клавишу, иначе автоповтор
	);

int_least16_t encoderA_get_snapshot(unsigned * speed, const uint_fast8_t derate);
int_least16_t encoderB_get_snapshot(const uint_fast8_t derate);

void encoderA_set_resolution(uint_fast8_t resolution, uint_fast8_t dynamic);	// параметр - делённое на ENCRESSCALE значение.
void encoderB_set_resolution(uint_fast8_t resolution, uint_fast8_t dynamic);	// параметр - делённое на ENCRESSCALE значение.

#define ENCODER_NORMALIZED_RESOLUTION (1440)	// виртуальных импульсов за оборот в секунду - нормализованная скорость
//#define ENCODER_NORMALIZED_RESOLUTION (144)	// виртуальных импульсов за оборот в секунду - нормализованная скорость

#define ENCRESSCALE 4U

#define ENC_DYNA_MAX 4

extern encoder_t encoder1;	// Main RX tuning knob
extern encoder_t encoder2;	// Sub RX tuning knob
extern encoder_t encoder_ENC1F;
extern encoder_t encoder_ENC2F;
extern encoder_t encoder_ENC3F;
extern encoder_t encoder_ENC4F;
extern encoder_t encoder_kbd;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ENCODER_H_INCLUDED */

