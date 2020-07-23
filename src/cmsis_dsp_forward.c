/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHINTEGRATEDDSP

	#define ARM_MATH_LOOPUNROLL 1	// выставляется в hardware.h
	//#define ARM_MATH_NEON 1

	#include "BasicMathFunctions/BasicMathFunctions.c"
	#include "CommonTables/CommonTables.c"
	//#include "DistanceFunctions/DistanceFunctions.c"
	//#include "MatrixFunctions/MatrixFunctions.c"
	#include "SupportFunctions/SupportFunctions.c"
	//#include "BayesFunctions/BayesFunctions.c"
	#include "ComplexMathFunctions/ComplexMathFunctions.c"
	#include "FastMathFunctions/FastMathFunctions.c"
	//#include "SVMFunctions/SVMFunctions.c"
	#include "TransformFunctions/TransformFunctions.c"
	//#include "ControllerFunctions/ControllerFunctions.c"
	#include "FilteringFunctions/FilteringFunctions.c"
#if WITHTOUCHGUI
	#include "StatisticsFunctions/StatisticsFunctions.c"
#endif

#endif  /* WITHINTEGRATEDDSP */

