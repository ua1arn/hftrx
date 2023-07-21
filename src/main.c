/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "bootloader.h"
#include "formats.h"


/* Главная функция программы */
int 
//__attribute__ ((used))
main(void)
{
#if LINUX_SUBSYSTEM
	linux_subsystem_init();
#endif /* LINUX_SUBSYSTEM */
#if (CPUSTYLE_ARM || CPUSTYLE_RISCV) && ! LINUX_SUBSYSTEM
	sysinit_gpio_initialize();
#endif /* (CPUSTYLE_ARM || CPUSTYLE_RISCV) && ! LINUX_SUBSYSTEM */
#if WITHDEBUG && (! CPUSTYLE_ARM /* || WITHISBOOTLOADER */)

	HARDWARE_DEBUG_INITIALIZE();
	HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);

#endif /* WITHDEBUG && ! CPUSTYLE_ARM */

	lowtests();		/* функции тестирования, работающие до инициализации периферии */

	global_disableIRQ();
	cpu_initialize();		// в случае ARM - инициализация прерываний и контроллеров, AVR - запрет JTAG
	lowinitialize();	/* вызывается при запрещённых прерываниях. */
	global_enableIRQ();
	cpump_runuser();	/* остальным ядрам разрешаем выполнять прерывания */
	midtests();

	initialize2();	/* вызывается при разрешённых прерываниях. */
#if WITHLWIP
	//network_initialize();
#endif /* WITHLWIP */
	application_initialize();
	hightests();		/* подпрограммы для тестирования аппаратуры */

#if LINUX_SUBSYSTEM
	linux_user_init();
#endif /* LINUX_SUBSYSTEM */
#if WITHNMEA && CPUSTYLE_XC7Z && ! LINUX_SUBSYSTEM
	HARDWARE_NMEA_INITIALIZE();
	HARDWARE_NMEA_SET_SPEED(115200L);
	HARDWARE_NMEA_ENABLERX(1);
	nmea_parser_init(); // пока тут
#endif /* WITHNMEA && CPUSTYLE_XC7Z && ! LINUX_SUBSYSTEM */

#if WITHISBOOTLOADER && WITHISBOOTLOADERFATFS
	bootloader_fatfs_mainloop();
#elif WITHISBOOTLOADER
	bootloader_mainloop();
#elif 0
	siggen_mainloop();
#elif 0
	hamradio_mainloop_beacon();
#elif WITHSPISLAVE
	dspcontrol_mainloop();
#else /* WITHSPISLAVE */
	application_mainloop();
#endif /* WITHSPISLAVE */
	return 0;
}
