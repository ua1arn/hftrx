/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if (__CORTEX_A != 0) && CPUSTYLE_ARM && (! defined(__aarch64__))

	#include "irq_ctrl_gic.c"

#endif /* (__CORTEX_A != 0) && CPUSTYLE_ARM && (! defined(__aarch64__)) */
