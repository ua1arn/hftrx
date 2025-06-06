/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "hardware.h"
#include "formats.h"
#include "display2.h"

#if WITHLVGL && LINUX_SUBSYSTEM

#include "lvgl.h"

void linux_lvgl_gui(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	// первоначальная инициализация через display2_initialize,
	// а дальше коллбеки
}


#endif /* WITHLVGL && LINUX_SUBSYSTEM */

