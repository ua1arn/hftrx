/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#if defined (BOARD_BITIMAGE_NAME)
	#include BOARD_BITIMAGE_NAME
#elif (CTLSTYLE_RAVENDSP_V4 && ! WITHUSEDUALWATCH) && (DDS1_CLK_MUL == 10)
	#include "rbf/rbfimage_v4_pll.h"
#elif (CTLSTYLE_RAVENDSP_V4 && ! WITHUSEDUALWATCH) && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v4.h"
#elif CTLSTYLE_RAVENDSP_V5 && (DDS1_CLK_MUL == 10)
	#include "rbf/rbfimage_v5_2ch_pll.h"	// CTLSTYLE_RAVENDSP_V5 with 12.288 osc
#elif CTLSTYLE_RAVENDSP_V5 && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v5_2ch.h"	// CTLSTYLE_RAVENDSP_V5
#elif CTLSTYLE_RAVENDSP_V6 && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v6_2ch.h"	// CTLSTYLE_RAVENDSP_V6
#elif CTLSTYLE_RAVENDSP_V7 && ! WITHUSEDUALWATCH && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v7_1ch.h"	//
#elif CTLSTYLE_RAVENDSP_V7 && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v7a_2ch.h"	// CTLSTYLE_RAVENDSP_V7
#elif CTLSTYLE_STORCH_V1 && ! WITHUSEDUALWATCH && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v7_1ch.h"	//
#elif CTLSTYLE_STORCH_V1 && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v7a_2ch.h"	// same as CTLSTYLE_RAVENDSP_V7
#elif CTLSTYLE_STORCH_V2 && ! WITHUSEDUALWATCH && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v7a_2ch.h"	//
#elif CTLSTYLE_STORCH_V2 && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v7a_2ch.h"	// same as CTLSTYLE_RAVENDSP_V7
#elif CTLSTYLE_STORCH_V3 && ! WITHUSEDUALWATCH && (DDS1_CLK_MUL == 1)
	//#include "rbf/rbfimage_v7_1ch.h"	//
	#include "rbf/rbfimage_v7a_2ch.h"	// same as CTLSTYLE_RAVENDSP_V7
#elif CTLSTYLE_STORCH_V3 && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v7a_2ch.h"	// same as CTLSTYLE_RAVENDSP_V7
#elif CTLSTYLE_STORCH_V4 && ! WITHUSEDUALWATCH && (DDS1_CLK_MUL == 1)	// modem v2
	#include "rbf/rbfimage_v7_1ch.h"	//
//#elif CTLSTYLE_STORCH_V4 && (DDS1_CLK_MUL == 1)	// modem v2
//	#include "rbf/rbfimage_v7_2ch.h"	// same as CTLSTYLE_RAVENDSP_V7
#elif CTLSTYLE_STORCH_V6 && ! WITHUSEDUALWATCH && (DDS1_CLK_MUL == 1)		// mini STM32H7 with TFT plug on board
	#include "rbf/rbfimage_v7h_2ch.h"	// - 32BIT audio
#elif CTLSTYLE_STORCH_V6 && (DDS1_CLK_MUL == 1)		// mini STM32H7 with TFT plug on board
	#include "rbf/rbfimage_v7h_2ch.h"	// same as CTLSTYLE_RAVENDSP_V7 - 32BIT audio
#elif CTLSTYLE_STORCH_V7 && ! WITHUSEDUALWATCH && (DDS1_CLK_MUL == 1)		// normal size STM32H7, TFT plug on board
	#include "rbf/rbfimage_v7h_2ch.h"	// - 32BIT audio
#elif CTLSTYLE_STORCH_V7 && (DDS1_CLK_MUL == 1)		// normal size STM32H7, TFT plug on board
	#include "rbf/rbfimage_v7h_2ch.h"	// same as CTLSTYLE_RAVENDSP_V7 - 32BIT audio
#elif CTLSTYLE_OLEG4Z_V1 && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_oleg4z.h"	// same as CTLSTYLE_RAVENDSP_V7, 1 RX & WFM
#else
	#error Missing FPGA image file BOARD_BITIMAGE_NAME
	0,
#endif
