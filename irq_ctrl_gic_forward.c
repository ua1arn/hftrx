/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if CPUSTYLE_ARM_CA7 || CPUSTYLE_ARM_CA7

	// Закомментировать #include CMSIS_device_header

	#include "irq_ctrl_gic.c"

#endif /* CPUSTYLE_ARM_CA7 || CPUSTYLE_ARM_CA7 */
