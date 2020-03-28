/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#if (CTLSTYLE_RAVENDSP_V3 && ! WITHUSEDUALWATCH) && (DDS1_CLK_MUL == 10)
	#include "rbf/rbfimage_v3_pll.h"
#elif (CTLSTYLE_RAVENDSP_V3 && ! WITHUSEDUALWATCH) && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v3.h"
#elif (CTLSTYLE_RAVENDSP_V4 && ! WITHUSEDUALWATCH) && (DDS1_CLK_MUL == 10)
	#include "rbf/rbfimage_v4_pll.h"
#elif (CTLSTYLE_RAVENDSP_V4 && ! WITHUSEDUALWATCH) && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_v4.h"
#elif CTLSTYLE_RAVENDSP_V5 && (DDS1_CLK_MUL == 10)
	#include "rbf/rbfimage_v5_2ch_pll.h"	// CTLSTYLE_RAVENDSP_V5 with 12.288 osc
#elif CTLSTYLE_RAVENDSP_V5 && (DDS1_CLK_MUL == 1) && WITHOPERA4BEACON
	#include "rbf/rbfimage_v5_2ch_opera4.h"	// CTLSTYLE_RAVENDSP_V5
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
#elif CTLSTYLE_STORCH_V5 && ! WITHUSEDUALWATCH && (DDS1_CLK_MUL == 1)		// mini
	#include "rbf/rbfimage_v7_1ch.h"	//
#elif CTLSTYLE_STORCH_V5 && (DDS1_CLK_MUL == 1)		// mini
	#include "rbf/rbfimage_v7a_2ch.h"	// same as CTLSTYLE_RAVENDSP_V7
#elif CTLSTYLE_STORCH_V6 && ! WITHUSEDUALWATCH && (DDS1_CLK_MUL == 1)		// mini
	#include "rbf/rbfimage_v7a_2ch.h"	//
#elif CTLSTYLE_STORCH_V6 && (DDS1_CLK_MUL == 1)		// mini
	#include "rbf/rbfimage_v7a_2ch.h"	// same as CTLSTYLE_RAVENDSP_V7
#elif CTLSTYLE_STORCH_V7 && ! WITHUSEDUALWATCH && (DDS1_CLK_MUL == 1)		// TFT plug on board
	#include "rbf/rbfimage_v7a_2ch.h"	//
#elif CTLSTYLE_STORCH_V7 && (DDS1_CLK_MUL == 1)		// TFT plug on board
	#include "rbf/rbfimage_v7a_2ch.h"	// same as CTLSTYLE_RAVENDSP_V7
#elif CTLSTYLE_OLEG4Z_V1 && (DDS1_CLK_MUL == 1)
	#include "rbf/rbfimage_oleg4z.h"	// same as CTLSTYLE_RAVENDSP_V7, 1 RX & WFM
#elif CTLSTYLE_STORCH_V8 && (DDS1_CLK_MUL == 1) && WITHRTS192	// renesas & TFT panel on CPU
	#include "rbf/rbfimage_v8t_192k.h"
#elif CTLSTYLE_STORCH_V8 && (DDS1_CLK_MUL == 1)	// renesas & TFT panel on CPU
	#include "rbf/rbfimage_v8t_96k.h"
#elif CTLSTYLE_STORCH_V9 && (DDS1_CLK_MUL == 1) && WITHRTS192	// renesas & TFT panel on CPU
	#include "rbf/rbfimage_v8t_192k.h"
#elif CTLSTYLE_STORCH_V9 && (DDS1_CLK_MUL == 1)	// renesas & TFT panel on CPU
	#include "rbf/rbfimage_v8t_96k.h"
#else
	#error Missing FPGA image file
	0,
#endif
