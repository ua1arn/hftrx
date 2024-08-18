/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "bootloader.h"
#include "formats.h"


/* вызывается при запрещённых прерываниях. */
void
lowinitialize(void)
{
#if ! WITHRTOS
	board_beep_initialize();
	//hardware_cw_diagnostics_noirq(1, 0, 1);	// 'K'
#if WITHDEBUG

	//HARDWARE_DEBUG_INITIALIZE();
	//HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);


	dbg_puts_impl_P(PSTR("Version " __DATE__ " " __TIME__ " 1 debug session starts.\n"));
	// выдача повторяющегося символа для тестирования скорости передачи, если ошибочная инициализация
	//for (;;)
	//	hardware_putchar(0xff);
	// тестирование приёма и передачи символов
	for (;0;)
	{
		char c;
		if (dbg_getchar(& c))
		{
			if (c == 0x1b)
				break;
			dbg_putchar(c);
		}
	}
#endif /* WITHDEBUG */

	//hardware_cw_diagnostics_noirq(1, 0, 0);	// 'D'
	// Инициализация таймера и списка регистрирумых обработчиков
	hardware_timer_initialize(TICKS_FREQUENCY);

	board_initialize();		/* инициализация чипселектов и SPI, I2C, загрузка FPGA */
	cpu_initdone();			/* секция init (в которой лежит образ для загрузки в FPGA) больше не нужна */
#endif /* ! WITHRTOS */

	board_dpc_initialize();		/* инициализация списка user-mode опросных функций */
}

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
	local_delay_initialize();
#endif /* (CPUSTYLE_ARM || CPUSTYLE_RISCV) && ! LINUX_SUBSYSTEM */
#if WITHDEBUG && (! (CPUSTYLE_ARM || CPUSTYLE_RISCV) /* || WITHISBOOTLOADER */)

	HARDWARE_DEBUG_INITIALIZE();
	HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);

#endif /* WITHDEBUG && (! (CPUSTYLE_ARM || CPUSTYLE_RISCV) */

	lowtests();		/* функции тестирования, работающие до инициализации периферии */

	global_disableIRQ();
	cpu_initialize();		// в случае ARM - инициализация прерываний и контроллеров, AVR - запрет JTAG
	lowinitialize();	/* вызывается при запрещённых прерываниях. */
	applowinitialize();	/* вызывается при запрещённых прерываниях. */
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

#if WITHISBOOTLOADER && WITHISBOOTLOADERFATFS
	bootloader_fatfs_mainloop();
#elif WITHISBOOTLOADER0
	bootloader0_mainloop();
#elif WITHCTRLBOARDT507
	ctlboardt507_mainloop();
#elif WITHISBOOTLOADER
	bootloader_mainloop();
#elif 0
	siggen_mainloop();
#elif 0
	hamradio_mainloop_beacon();
#else
	application_mainloop();
#endif
	return 0;
}
