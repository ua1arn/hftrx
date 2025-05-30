/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include <arm_math.h>

#pragma once

#if LINUX_SUBSYSTEM && WITHAD936XIIO

#define NUM_FIR_TAPS	256
#define NUM_FIR_TAPS_TX	240

q31_t fir_coeffs[NUM_FIR_TAPS] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	-1,
	1,
	-1,
	2,
	-2,
	3,
	-3,
	4,
	-4,
	4,
	-5,
	5,
	-5,
	5,
	-5,
	5,
	-4,
	4,
	-3,
	2,
	-1,
	0,
	1,
	-2,
	4,
	-6,
	8,
	-10,
	12,
	-15,
	17,
	-19,
	21,
	-23,
	25,
	-27,
	28,
	-28,
	29,
	-29,
	28,
	-26,
	24,
	-21,
	17,
	-13,
	7,
	-1,
	-5,
	13,
	-22,
	31,
	-41,
	51,
	-61,
	72,
	-83,
	94,
	-104,
	113,
	-122,
	129,
	-136,
	140,
	-142,
	143,
	-140,
	135,
	-127,
	116,
	-101,
	82,
	-60,
	34,
	-4,
	-29,
	67,
	-109,
	154,
	-203,
	255,
	-311,
	368,
	-428,
	489,
	-552,
	615,
	-678,
	740,
	-802,
	861,
	-918,
	971,
	-1021,
	1066,
	-1107,
	1142,
	-1172,
	1195,
	-1212,
	1222,
	31542,
	1222,
	-1212,
	1195,
	-1172,
	1142,
	-1107,
	1066,
	-1021,
	971,
	-918,
	861,
	-802,
	740,
	-678,
	615,
	-552,
	489,
	-428,
	368,
	-311,
	255,
	-203,
	154,
	-109,
	67,
	-29,
	-4,
	34,
	-60,
	82,
	-101,
	116,
	-127,
	135,
	-140,
	143,
	-142,
	140,
	-136,
	129,
	-122,
	113,
	-104,
	94,
	-83,
	72,
	-61,
	51,
	-41,
	31,
	-22,
	13,
	-5,
	-1,
	7,
	-13,
	17,
	-21,
	24,
	-26,
	28,
	-29,
	29,
	-28,
	28,
	-27,
	25,
	-23,
	21,
	-19,
	17,
	-15,
	12,
	-10,
	8,
	-6,
	4,
	-2,
	1,
	0,
	-1,
	2,
	-3,
	4,
	-4,
	5,
	-5,
	5,
	-5,
	5,
	-5,
	4,
	-4,
	4,
	-3,
	3,
	-2,
	2,
	-1,
	1,
	-1,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

q31_t fir_coeffs_tx[NUM_FIR_TAPS_TX] =
{
	234,
	262,
	291,
	321,
	351,
	381,
	411,
	440,
	469,
	496,
	522,
	546,
	568,
	586,
	602,
	614,
	622,
	625,
	624,
	617,
	605,
	587,
	562,
	531,
	493,
	448,
	395,
	335,
	267,
	192,
	109,
	18,
	-79,
	-184,
	-297,
	-416,
	-541,
	-672,
	-809,
	-950,
	-1095,
	-1243,
	-1394,
	-1546,
	-1699,
	-1851,
	-2001,
	-2149,
	-2293,
	-2431,
	-2563,
	-2687,
	-2803,
	-2907,
	-3000,
	-3080,
	-3144,
	-3193,
	-3225,
	-3237,
	-3230,
	-3202,
	-3151,
	-3076,
	-2977,
	-2853,
	-2701,
	-2523,
	-2316,
	-2081,
	-1817,
	-1523,
	-1200,
	-847,
	-464,
	-51,
	389,
	859,
	1358,
	1884,
	2436,
	3014,
	3615,
	4240,
	4886,
	5552,
	6235,
	6935,
	7649,
	8375,
	9111,
	9854,
	10603,
	11354,
	12106,
	12856,
	13600,
	14338,
	15065,
	15780,
	16479,
	17161,
	17822,
	18461,
	19075,
	19661,
	20217,
	20741,
	21232,
	21686,
	22103,
	22481,
	22817,
	23112,
	23363,
	23570,
	23732,
	23848,
	23918,
	23942,
	23918,
	23848,
	23732,
	23570,
	23363,
	23112,
	22817,
	22481,
	22103,
	21686,
	21232,
	20741,
	20217,
	19661,
	19075,
	18461,
	17822,
	17161,
	16479,
	15780,
	15065,
	14338,
	13600,
	12856,
	12106,
	11354,
	10603,
	9854,
	9111,
	8375,
	7649,
	6935,
	6235,
	5552,
	4886,
	4240,
	3615,
	3014,
	2436,
	1884,
	1358,
	859,
	389,
	-51,
	-464,
	-847,
	-1200,
	-1523,
	-1817,
	-2081,
	-2316,
	-2523,
	-2701,
	-2853,
	-2977,
	-3076,
	-3151,
	-3202,
	-3230,
	-3237,
	-3225,
	-3193,
	-3144,
	-3080,
	-3000,
	-2907,
	-2803,
	-2687,
	-2563,
	-2431,
	-2293,
	-2149,
	-2001,
	-1851,
	-1699,
	-1546,
	-1394,
	-1243,
	-1095,
	-950,
	-809,
	-672,
	-541,
	-416,
	-297,
	-184,
	-79,
	18,
	109,
	192,
	267,
	335,
	395,
	448,
	493,
	531,
	562,
	587,
	605,
	617,
	624,
	625,
	622,
	614,
	602,
	586,
	568,
	546,
	522,
	496,
	469,
	440,
	411,
	381,
	351,
	321,
	291,
	262,
	234,
	207
};

#endif /* LINUX_SUBSYSTEM && WITHAD936XIIO */
