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
#define ENCTICKSMAX ENCNTICKS(125)
#define ENCNTICKS(v) ((v + (ENC_TICKS_PERIOD - 1)) / ENC_TICKS_PERIOD)
#define ENCTICKS_FREQUENCY (1000 / ENC_TICKS_PERIOD)

typedef struct encoder_tag
{
	int_least16_t position;	// обновляется по прерыванию
	uint8_t old_val;
	uint_fast8_t (* getpins)(void);
	IRQLSPINLOCK_t enclock;

	/* перенесено из глобальной области видимости */
	IRQLSPINLOCK_t encspeedlock;
	unsigned enchist [HISTLEN];
	uint_fast8_t tichist;	// Должно поместиться число от 0 до TICKSMAX включительно
	uint_fast8_t enchistindex;
	int rotate_kbd;
} encoder_t;

void encoder_initialize(encoder_t * e, uint_fast8_t (* agetpins)(void));

int_least16_t encoder_get_delta(encoder_t * e);
void encoder_pushback(encoder_t * const e, int outsteps);

int_least16_t
encoder_getrotatehires(
	encoder_t * const e,
	uint_fast8_t * jumpsize	/* jumpsize - во сколько раз увеличивается скорость перестройки */
	);

void encoders_clear(void);	/* накопитель сбрасывается */

void encoders_initialize(void);

void encoder_kbdctl(
	uint_fast8_t code, 		// код клавиши
	uint_fast8_t accel		// 0 - одиночное нажатие на клавишу, иначе автоповтор
	);

void encoder_set_resolution(encoder_t * e, uint_fast8_t resolution, uint_fast8_t dynamic);	// параметр - делённое на ENCRESSCALE значение.
unsigned encoder_get_actualresolution(encoder_t * e);	// возвращает количество инкрементов на оборот

#define ENCODER_NORMALIZED_RESOLUTION (1440)	// виртуальных импульсов за оборот в секунду - нормализованная скорость
//#define ENCODER_NORMALIZED_RESOLUTION (144)	// виртуальных импульсов за оборот в секунду - нормализованная скорость

#define ENCRESSCALE 4U

#define ENC_DYNA_MAX 4

extern encoder_t encoder1;	// Main RX tuning knob
#if WITHENCODER_SUB
extern encoder_t encoder_sub;	// Sub RX tuning knob
#endif /* WITHENCODER_SUB */
#if WITHENCODER2
extern encoder_t encoder2;	// FN knob or Sub RX tuning knob
#endif /* WITHENCODER2 */
extern encoder_t encoder_ENC1F;
extern encoder_t encoder_ENC2F;
extern encoder_t encoder_ENC3F;
extern encoder_t encoder_ENC4F;
extern encoder_t encoder_kbd;


void spool_encinterrupts(void * ctx);	/* прерывание по изменению сигнала на входах от валкодера */
void spool_encinterrupts_ccw(void * ctx);	/* прерывание по изменению сигнала на входах от валкодера */
void spool_encinterrupts4_dirA_ccw(void * ctx);	/* прерывание по одному перепаду сигнала на входе B от валкодера - направление по A */
void spool_encinterrupts4_dirB_ccw(void * ctx);	/* прерывание по одному перепаду сигнала на входе A от валкодера - направление по B */
void spool_encinterrupts4_dirA_cw(void * ctx);	/* прерывание по одному перепаду сигнала на входе B от валкодера - направление по A */
void spool_encinterrupts4_dirB_cw(void * ctx);	/* прерывание по одному перепаду сигнала на входе A от валкодера - направление по B */

void hardware_encoders_initialize(void);

#define GETENCBIT_A 0x02
#define GETENCBIT_B 0x01

uint_fast8_t hardware_get_encoder_bits(void);	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
uint_fast8_t hardware_get_encoder_sub_bits(void);	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
uint_fast8_t hardware_get_encoder2_bits(void);	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
uint_fast8_t hardware_get_encoder3_bits(void);	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
uint_fast8_t hardware_get_encoder4_bits(void);	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
uint_fast8_t hardware_get_encoder5_bits(void);	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
uint_fast8_t hardware_get_encoder6_bits(void);	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

#define ENCODER_IRQL IRQL_OVERREALTIME
#define ENCODER_PRIORITY ARM_OVERREALTIME_PRIORITY
#define ENCODER_TARGETCPU TARGETCPU_OVRT

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ENCODER_H_INCLUDED */

