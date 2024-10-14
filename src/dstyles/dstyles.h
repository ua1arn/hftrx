/* $Id$ */
//
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef DSTYLES_H_INCLUDED
#define DSTYLES_H_INCLUDED 1

#if defined (BOARD_DSTYLE)
    #include BOARD_DSTYLE
#elif DSTYLE_G_X320_Y240
    #include "g_x320_y240.h"
#elif DSTYLE_G_X480_Y272 && WITHSPECTRUMWF && ! WITHTOUCHGUI
    #include "g_x480_y272_spectrum_notouch.h"
#elif DSTYLE_G_X480_Y272
    #include "g_x480_y272.h"
#elif DSTYLE_G_X800_Y480 && LINUX_SUBSYSTEM
    #include <src/dstyles/g_x800_y480_linux.h>
#elif DSTYLE_G_X800_Y480 && WITHTOUCHGUI && WITHALTERNATIVELAYOUT
    #include "g_x800_y480_touch_alternative.h"
#elif DSTYLE_G_X800_Y480 && WITHTOUCHGUI //&& WITHSPECTRUMWF
    #include "g_x800_y480_touch.h"
#elif DSTYLE_G_X1024_Y600
	#include "g_x1024_y600.h"
#elif DSTYLE_G_X800_Y480
	#include "g_x800_y480.h"
#elif DSTYLE_G_X800_Y480 && 1	//&& WITHSPECTRUMWF
    #include "g_x800_y480_mini.h"
#elif DSTYLE_G_DUMMY
	#include "g_dummy.h"
#else
	#error BOARD_DSTYLE not defined
#endif /* LCDMODE_LS020 */

#endif /* DSTYLES_H_INCLUDED */
