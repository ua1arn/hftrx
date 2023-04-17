/*
 * dspdefines.h
 *
 *  Created on: 17 апр. 2023 г.
 *      Author: User
 */

#ifndef SRC_DSPDEFINES_H_
#define SRC_DSPDEFINES_H_

#if WITHINTEGRATEDDSP

	#if __ARM_NEON
		//#define ARM_MATH_NEON 1
		//#define ARM_MATH_NEON_EXPERIMENTAL 1
	#endif /* __ARM_NEON */
	//#define ARM_MATH_AUTOVECTORIZE 1
	#define ARM_MATH_LOOPUNROLL 1	// выставляется в hardware.h
	#define ARM_FAST_ALLOW_TABLES	1
	#define ARM_MATH_LOOPUNROLL 1
	#define DISABLEFLOAT16 1

	#include "arm_math.h"
	#include "arm_const_structs.h"


#if WITHAFEQUALIZER

enum {
	AF_EQUALIZER_BANDS = 3,		// число полос
	AF_EQUALIZER_BASE = 8,		// предел регулировки
	AF_EQUALIZER_LOW = 400,		// частота нижней полосы
	AF_EQUALIZER_MID = 1500,	// частота средней полосы
	AF_EQUALIZER_HIGH = 2700	// частота верхней полосы
};

int_fast32_t hamradio_get_af_equalizer_base(void);
int_fast32_t hamradio_get_af_equalizer_gain_rx(uint_fast8_t v);
void hamradio_set_af_equalizer_gain_rx(uint_fast8_t index, uint_fast8_t gain);
void board_set_equalizer_rx(uint_fast8_t n);
void board_set_equalizer_tx(uint_fast8_t n);
void board_set_equalizer_rx_gains(const uint_fast8_t * p);
void board_set_equalizer_tx_gains(const uint_fast8_t * p);
uint_fast8_t hamradio_get_geqrx(void);
void hamradio_set_geqrx(uint_fast8_t v);

void audio_rx_equalizer(float32_t *buffer, uint_fast16_t size);

#endif /* WITHAFEQUALIZER */

#endif /* WITHINTEGRATEDDSP */


#if (((__ARM_FP & 0x08) && defined(__aarch64__)) || (__riscv_d)) && 0

	typedef double FLOAT_t;

	#define ARM_MORPH(name) name ## _f64
	#define FLOORF	floor
	#define LOG10F	local_log10 //log10
	#define LOGF	log
	#define POWF	pow
	#define LOG2F	log2
//		#define LOGF	local_log
//		#define POWF	local_pow
//		#define LOG2F	local_log2
	#define SINF	sin
	#define COSF	cos
	#define ATAN2F	atan2
	#define ATANF	atan
	#define TANF	tan
	//#define EXPF	exp
	#define EXPF	local_exp
	#define FABSF	fabs
	#define SQRTF	sqrt
	#define FMAXF	fmax
	#define FMINF	fmin
	#define LDEXPF	ldexp
	#define FREXPF	frexp

	#if defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMA)
		#define FMAF	fma
	#endif /* defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMA) */
	#define DSP_FLOAT_BITSMANTISSA 54

#elif 1 //(__ARM_FP & 0x04)

	typedef float FLOAT_t;

	#define ARM_MORPH(name) name ## _f32
	#define FLOORF	floorf
	#define LOG10F	local_log10 //log10f
	#define LOGF	logf
	#define POWF	powf
	#define LOG2F	log2f
//		#define LOGF	local_log
//		#define POWF	local_pow
//		#define LOG2F	local_log2
	#define SINF	sinf
	#define COSF	cosf
	#define ATAN2F	atan2f
	#define ATANF	atanf
	#define TANF	tanf
	//#define EXPF	expf
	#define EXPF	local_exp
	#define FABSF	fabsf
	#define SQRTF	sqrtf
	#define FMAXF	fmaxf
	#define FMINF	fminf
	#define LDEXPF	ldexpf
	#define FREXPF	frexpf

	#if defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMAF)
		#define FMAF	fmaf
	#endif /* defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMAF) */
	#define DSP_FLOAT_BITSMANTISSA 24

#else

	#error This CPU not support floating point

#endif


/* from "C Language Algorithms for Digital Signal Processing"
by Paul M. Embree and Bruce Kimble, Prentice Hall, 1991 */


FLOAT_t local_exp(FLOAT_t x);
FLOAT_t local_pow(FLOAT_t x, FLOAT_t y);
FLOAT_t local_log(FLOAT_t x);
FLOAT_t local_log10(FLOAT_t X);

/* для возможности работы с функциями сопроцессора NEON - vld1_f32 например */
#define IV ivqv [0]
#define QV ivqv [1]

typedef struct
{
	FLOAT_t ivqv [2];
} FLOAT32P_t;

typedef struct
{
	int_fast32_t ivqv [2];
} INT32P_t;



#endif /* SRC_DSPDEFINES_H_ */
