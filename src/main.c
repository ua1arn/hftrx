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
	board_beep_initialize();
	//hardware_cw_diagnostics_noirq(1, 0, 1);	// 'K'
#if WITHDEBUG

	//HARDWARE_DEBUG_INITIALIZE();
	//HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);


	PRINTF("Version " __DATE__ " " __TIME__ " 1 debug session starts.\n");
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
	// Инициализация списка регистрирумых обработчиков
	tickers_initialize();

	board_initialize();		/* инициализация чипселектов и SPI, I2C, загрузка FPGA */
	cpu_initdone();			/* секция init (в которой лежит образ для загрузки в FPGA) больше не нужна */
}

/* Главная функция программы */
int 
//__attribute__ ((used))
main(void)
{
	sysinit_gpio_initialize();

	lowtests();		/* функции тестирования, работающие до инициализации периферии */

	global_disableIRQ();
	cpu_initialize();		// в случае ARM - инициализация прерываний и контроллеров, AVR - запрет JTAG
	lowinitialize();	/* вызывается при запрещённых прерываниях. */
	applowinitialize();	/* вызывается при запрещённых прерываниях. */
	task_scheduler_start();
	global_enableIRQ();
	cpump_runuser();	/* остальным ядрам разрешаем выполнять прерывания */
	// Инициализация таймера
	hardware_timer_initialize(TICKS_FREQUENCY);
	midtests();
	application_initialize();
	hightests();		/* подпрограммы для тестирования аппаратуры */

	hamradio_main_start();
	for (;;)
	{
		board_dpc_processing();		// обработка отложенного вызова user mode функций
	}
	return 0;
}
