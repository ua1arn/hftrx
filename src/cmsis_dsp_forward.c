/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHINTEGRATEDDSP

	#include "BasicMathFunctions/BasicMathFunctions.c"
	//#include "BayesFunctions/BayesFunctions.c"
	#include "CommonTables/CommonTables.c"
	#include "ComplexMathFunctions/ComplexMathFunctions.c"
	//#include "ControllerFunctions/ControllerFunctions.c"

	//#include "DistanceFunctions/DistanceFunctions.c"
	#include "FastMathFunctions/FastMathFunctions.c"
	#include "FilteringFunctions/FilteringFunctions.c"
	#include "WindowFunctions/WindowFunctions.c"
	//#include "InterpolationFunctions/InterpolationFunctions.c"
	//#include "MatrixFunctions/MatrixFunctions.c"

	//#include "QuaternionMathFunctions/QuaternionMathFunctions.c"
	#include "StatisticsFunctions/StatisticsFunctions.c"
	#include "SupportFunctions/SupportFunctions.c"
	//#include "SVMFunctions/SVMFunctions.c"
	#include "TransformFunctions/TransformFunctions.c"

#endif  /* WITHINTEGRATEDDSP */
