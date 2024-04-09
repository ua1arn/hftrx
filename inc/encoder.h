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
	int position;	// обновляется по прерыванию
	uint8_t old_val;
	uint_fast8_t (* getpins)(void);
	IRQLSPINLOCK_t enclock;
	int reverse;
	/* locked by encspeedlock */
	int rotate;
	int backup_rotate;
} encoder_t;

void encoder_initialize(encoder_t * e, uint_fast8_t (* agetpins)(void));
int
encoder_get_snapshot(
	encoder_t * e,
	unsigned * speed,
	const uint_fast8_t derate
	);
int
encoder_get_snapshotproportional(
	encoder_t * e,
	unsigned * speed,
	const uint_fast8_t derate
	);

int_least16_t getRotateHiRes(uint_fast8_t * jumpsize, uint_fast8_t hiresdiv);	/* получение накопленных значений прерываний от валкодера. накопитель сбрасывается */
int_least16_t getRotateHiRes2(uint_fast8_t * jumpsize, uint_fast8_t loresdiv);	/* получение накопленных значений прерываний от валкодера. накопитель сбрасывается */
int_least16_t getRotateHiRes3(uint_fast8_t * jumpsize, uint_fast8_t loresdiv);	/* получение накопленных значений прерываний от валкодера. накопитель сбрасывается */
int_least16_t getRotateHiRes4(uint_fast8_t * jumpsize, uint_fast8_t loresdiv);	/* получение накопленных значений прерываний от валкодера. накопитель сбрасывается */
int_least16_t getRotateHiRes5(uint_fast8_t * jumpsize, uint_fast8_t loresdiv);	/* получение накопленных значений прерываний от валкодера. накопитель сбрасывается */

void encoders_clear(void);	/* накопитель сбрасывается */

int getRotateLoRes(uint_fast8_t hiresdiv); /* получение "редуцированного" количества прерываний от валкодера #1. */
int getRotateLoRes2(uint_fast8_t hiresdiv); /* получение "редуцированного" количества прерываний от валкодера #2. */
int getRotateLoRes3(uint_fast8_t hiresdiv); /* получение "редуцированного" количества прерываний от валкодера #3. */
int getRotateLoRes4(uint_fast8_t hiresdiv); /* получение "редуцированного" количества прерываний от валкодера #4. */
int getRotateLoRes5(uint_fast8_t hiresdiv); /* получение "редуцированного" количества прерываний от валкодера #15. */

void encoders_initialize(void);
void encoder1_pushback(int outsteps, uint_fast8_t hiresdiv);
void encoder_kbdctl(
	uint_fast8_t code, 		// код клавиши
	uint_fast8_t accel		// 0 - одиночное нажатие на клавишу, иначе автоповтор
	);

int encoder1_get_snapshot(unsigned * speed, const uint_fast8_t derate);
int encoder2_get_snapshot(unsigned * speed, const uint_fast8_t derate);

void encoder_set_resolution(uint_fast8_t resolution, uint_fast8_t dynamic);	// параметр - делённое на ENCRESSCALE значение.

#define ENCODER_NORMALIZED_RESOLUTION (1440)	// виртуальных импульсов за оборот в секунду - нормализованная скорость
//#define ENCODER_NORMALIZED_RESOLUTION (144)	// виртуальных импульсов за оборот в секунду - нормализованная скорость

#define ENCRESSCALE 4U


#define ENC_DYNA_MAX 4

extern encoder_t encoder1;
extern encoder_t encoder2;
extern encoder_t encoder3;
extern encoder_t encoder4;
extern encoder_t encoder5;
extern encoder_t encoder6;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ENCODER_H_INCLUDED */

