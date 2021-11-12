/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#if defined (BOARD_BITIMAGE_NAME)
	#include BOARD_BITIMAGE_NAME
#elif CTLSTYLE_STORCH_V1 && ! WITHUSEDUALWATCH && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v7_1ch.h"	//
#elif CTLSTYLE_STORCH_V1 && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v7a_2ch.h"	// same as CTLSTYLE_RAVENDSP_V7
#elif CTLSTYLE_STORCH_V4 && ! WITHUSEDUALWATCH && (DDS1_CLK_MUL == 1)	// modem v2
	#include "rbf/rbfimage_v7_1ch.h"	//
#else
	#error Missing FPGA image file BOARD_BITIMAGE_NAME
	0,
#endif
