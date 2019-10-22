/* $Id$ */
//
// Сиквенсор переключения приём-передача и назад.
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#include "hardware.h"
#include "board.h"
#include "formats.h"
#include "inc/spi.h"

static ticker_t seqticker;

#if WITHTX

enum {
	SEQST_INITIALIZE,			// ничего не передается - начальное состояние сиквенсора
	// electronic key modes and SSB modes
	SEQST_PUSHED_WAIT_ACK_TX,	// Ждём, пока в SW-2011-RDX выключится сигнал TX1 и будем переходить на передачу.
	SEQST_WAIT_ACK_TX,			// ждем перехода на передачу от пользовательской программы
	SEQST_PUSHED_HOLDTX,		// ждём, когда переключится антенное реле на передачу
	SEQST_HOLDTX,				// ждём пока не исчезнет запрос на передачу
	SEQST_RGBEEP,				// перед переходом на приём, после отпускания тангенты - формирование звукового сигнала (roger beep)
	SEQST_PUSHED_SWITCHTORX,	// ждем перехода на на приём от пользовательской программы
	SEQST_SWITCHTORX,			// ожидание перепрограммирования переиферии на приём - пока переключаем SPI регистры
	//
	SEQST_MAX		// не используется в сиквенсоре.- количество состояний
};

// Значения для выбора из таблицы управления трактом.
// Последняя размерность в данной таблице: 0: keyup, 1: keydown
static const FLASHMEM uint_fast8_t seqtxgfi [SEQST_MAX] [2] =
{
	{ TXGFI_RX,			TXGFI_RX,		},	// SEQST_INITIALIZE,		// ничего не передается - начальное состояние сиквенсора
	{ TXGFI_TRANSIENT,	TXGFI_TRANSIENT, }, // SEQST_PUSHED_WAIT_ACK_TX,// Ждём, пока в SW-2011-RDX выключится сигнал TX1 и будем переходить на передачу.
	{ TXGFI_TRANSIENT,	TXGFI_TRANSIENT, }, // SEQST_WAIT_ACK_TX,		// ждем перехода на передачу от пользовательской программы
	{ TXGFI_TRANSIENT,	TXGFI_TRANSIENT, }, // SEQST_PUSHED_HOLDTX,		// ждём, когда переключится антенное реле на передачу
	{ TXGFI_TX_KEYUP,	TXGFI_TX_KEYDOWN, }, // SEQST_HOLDTX,			// ждём пока не исчезнет запрос на передачу
	{ TXGFI_TX_KEYUP,	TXGFI_TX_KEYDOWN, }, // SEQST_RGBEEP,			// перед переходом на приём, после отпускания тангенты - формирование звукового сигнала (roger beep)
	{ TXGFI_TRANSIENT,	TXGFI_TRANSIENT, },	// SEQST_PUSHED_SWITCHTORX,	// ждем перехода на на приём от пользовательской программы
	{ TXGFI_TRANSIENT,	TXGFI_TRANSIENT, },	// SEQST_SWITCHTORX,		// ожидание перепрограммирования переиферии на приём - пока переключаем SPI регистры
};

// Значения для выбора из таблицы управления трактом.
// Последняя размерность в данной таблице: 0: keyup, 1: keydown
static const FLASHMEM uint_fast8_t seqtxstate [SEQST_MAX] =
{
	0,	// SEQST_INITIALIZE,			// ничего не передается - начальное состояние сиквенсора
	0,	// SEQST_PUSHED_WAIT_ACK_TX,	// Ждём, пока в SW-2011-RDX выключится сигнал TX1 и будем переходить на передачу.
	1,	// SEQST_WAIT_ACK_TX,			// ждем перехода на передачу от пользовательской программы
	1,	// SEQST_PUSHED_HOLDTX,			// ждём, когда переключится антенное реле на передачу
	1,	// SEQST_HOLDTX,				// ждём пока не исчезнет запрос на передачу
	1,	// SEQST_RGBEEP,				// перед переходом на приём, после отпускания тангенты - формирование звукового сигнала (roger beep)
	1,	// SEQST_PUSHED_SWITCHTORX,		// ждем перехода на на приём от пользовательской программы
	0,	// SEQST_SWITCHTORX,			// ожидание перепрограммирования переиферии на приём - пока переключаем SPI регистры
};

#if WITHDEBUG

// Названия состояний сиквенсора - для отладочной печати
static const char * FLASHMEM const seqnames [SEQST_MAX] =
{
	"SEQST_INITIALIZE",			// ничего не передается - начальное состояние сиквенсора
	"SEQST_PUSHED_WAIT_ACK_TX",	// Ждём, пока в SW-2011-RDX выключится сигнал TX1 и будем переходить на передачу.
	"SEQST_WAIT_ACK_TX",		// ждем перехода на передачу от пользовательской программы
	"SEQST_PUSHED_HOLDTX",		// ждём, когда переключится антенное реле на передачу
	"SEQST_HOLDTX[4]",			// ждём пока не исчезнет запрос на передачу
	"SEQST_RGBEEP",				// перед переходом на приём, после отпускания тангенты - формирование звукового сигнала (roger beep)
	"SEQST_PUSHED_SWITCHTORX",	// ждем перехода на на приём от пользовательской программы
	"SEQST_SWITCHTORX",			// ожидание перепрограммирования переиферии на приём - пока переключаем SPI регистры
};

#endif /* WITHDEBUG */

// vox and mox parameters
// здесь используются 16-битные переменные
// так как максимальная задержка VOX - 2.5 секунды (500 тиков),
// и максимальная задержка QSK - 1.6 секунды (320 тиков).

static uint_fast16_t bkin_delay;	/* задержка отпускания qsk в тиках (смотри TICKS_FREQUENCY) */
static uint_fast16_t vox_delay;	/* задержка отпускания vox в тиках (смотри TICKS_FREQUENCY) */


static uint_fast16_t bkin_count;
static uint_fast16_t vox_count;

#if WITHVOX
static uint_fast8_t	vox_level;	/* уровень срабатывания VOX */
static uint_fast8_t	avox_level;	/* уровень срабатывания anti-VOX */
#endif /* WITHVOX */

static uint_fast8_t	seq_cwenable;		/* */
static uint_fast8_t	seq_voxenable;		/* */
static uint_fast8_t	seq_bkinenable;		/* */
static uint_fast8_t	seq_rgbeep;			/* разрешение формирование roger beep */
//static uint_fast8_t	seq_rgbeeptype;		/* вид roger beep */

// sequenser parameers
static uint_fast8_t rxtxticks;
static uint_fast8_t txrxticks;
static uint_fast8_t pretxticks;	// время выключения тракта RX в слуаче совмещённого тракта перед коммутацией на передачу.
static uint_fast16_t rgbeepticks;	// время формирование roger beep

static /* volatile */ uint_fast8_t exttunereq;	// запрос на tune от пользовательской программы
static /* volatile */ uint_fast8_t ptt;	// запрос на передачу от пользовательской программы
static /* volatile */ uint_fast8_t usertxstate;	/* 0 - периферия находимся в состоянии приёма, иначе - в состоянии передачи */

static volatile uint_fast8_t seqstate;
static volatile uint_fast8_t seqpushtime;	// Возможные количства "тиков" на передачу и на приём


static uint_fast8_t
getstablev8(volatile uint_fast8_t * p)
{
	uint_fast8_t v1 = * p;
	uint_fast8_t v2;
	do
	{
		v2 = v1;
		v1 = * p;
	} while (v2 != v1);
	return v1;

}

/* процедура возвращает из сиквенсора запрос на переключение на передачу в основную программу */
uint_fast8_t seq_get_txstate(void)
{
	return seqtxstate [getstablev8(& seqstate)];
}

#if WITHVOX


/* установить уровень срабатывания vox и anti-vox */
/* TODO:	Скорее всего, увеличение чувствительности VOX должно снижать порог срабатывания, с которым сравнивает vox_probe().
*/
void vox_set_levels(uint_fast8_t vlevel, uint_fast8_t alevel)
{
	//disableIRQ();
	
	vox_level = vlevel;	// 0..100
	avox_level = alevel;	// 0..100

	//enableIRQ();
}

static int vscale(uint_fast8_t v, uint_fast8_t mag)
{
	return v * mag / 100;
}
 
/* предъявить для проверки детектированный уровень сигнала и anti-vox */
// Вызывается при запрещённых прерываниях.
void vox_probe(uint_fast8_t vlevel, uint_fast8_t alevel)
{
	// vlevel, alevel - значение 0..UINT8_MAX
	if ((vscale(vlevel, vox_level) - vscale(alevel, avox_level) >= 16))
	{
		vox_count = vox_delay;
	}
}

/* разрешение (не-0) или запрещение (0) работы vox. */
void 
vox_enable(
	uint_fast8_t voxstate, 			/* разрешение (не-0) или запрещение (0) работы vox. */
	uint_fast8_t vox_delay_tens	/* задержка отпускания vox в 1/100 секундных интервалах */
	)
{
	disableIRQ();
	vox_delay = NTICKS(10 * vox_delay_tens);
	seq_voxenable = voxstate;
	enableIRQ();
}

#endif

/* разрешение (не-0) или запрещение (0) работы BREAK-IN. */
void 
seq_set_bkin_enable(
	uint_fast8_t bkinstate, 			/* разрешение (не-0) или запрещение (0) работы BREAK-IN. */
	uint_fast8_t bkin_delay_tens	/* задержка отпускания break-in в 1/100 секундных интервалах */
	)
{
	disableIRQ();
	bkin_delay = NTICKS(10 * bkin_delay_tens);
	seq_bkinenable = bkinstate;
	enableIRQ();
}

/* разрешение работы CW */
void 
seq_set_cw_enable(
	uint_fast8_t state		/* разрешение (не-0) или запрещение (0) работы CW. */
	)	
{
	disableIRQ();
	seq_cwenable = state;
	enableIRQ();
}

/* разрешение формирования roger beep */
void 
seq_set_rgbeep(
	uint_fast8_t state		/* разрешение (не-0) или запрещение (0). */
	)	
{
	disableIRQ();
	seq_rgbeep = state;
	enableIRQ();
}


/* подготовка работы задержек переключения приём-передача. Вызывается при запрещённых прерываниях. */
void 
vox_initialize(void)
{
	//bkin_count = vox_count = vox_count_stk = 0;
	bkin_delay = NTICKS(250);
	vox_delay = NTICKS(1000); /* Одна секунда */
	//vox_level = avox_level = 0;
	//seq_voxenable = 0;
	//seq_cwenable = 0;
}

////////////////////////////////

enum { QUEUEUESIZE = 2 * (NTICKS(WITHMAXRXTXDELAY) +  NTICKS(WITHMAXTXRXDELAY) * 2 + 4) };

// очередь организована как массив битов
static uint8_t keyqueuebuff [(QUEUEUESIZE + 7) / 8];
static uint_fast8_t keyqueueput;
static uint_fast8_t keyqueueget;
//static uint_fast8_t keyqueuecnt;

static void keyqueuein(uint_fast8_t v)
{
	const uint_fast8_t next = (keyqueueput + 1) % QUEUEUESIZE;
	if (next != keyqueueget)
	{
		// очередь организована как массив битов
		if (v != 0)
			keyqueuebuff [keyqueueput / 8] |= rbvalues [keyqueueput % 8];
		else
			keyqueuebuff [keyqueueput / 8] &= ~ rbvalues [keyqueueput % 8];
		keyqueueput = next;
		//keyqueuecnt += v;
	}	
	else
	{
#if WITHDEBUG
		debug_printf_P(PSTR("keyqueuein(%d) to full buffer, seqstate=%s\n"), v, seqnames [seqstate]);
#endif /* WITHDEBUG */
	}
}

// вывести бтт из очереди
// функция должна возвращать 0 или 1 и никак не другие значения
static uint_fast8_t keyqueueout(void)
{
	if (keyqueueget != keyqueueput)
	{
		// очередь организована как массив битов
		const uint_fast8_t v = (keyqueuebuff [keyqueueget / 8] & rbvalues [keyqueueget % 8]) != 0;
		keyqueueget = (keyqueueget + 1) % QUEUEUESIZE;
		//keyqueuecnt -= v;
		return v;
	}
	return 0;
}

#if 0
// количество ненулевых битов в очереди.
// функция должна возвращать 0 или н-ноль
static uint_fast8_t keyqueueact(void)
{
	return keyqueuecnt != 0;
}
#endif

// очистить очередь
static void keyqueueclear(void)
{
	//keyqueuecnt = 0;
	keyqueueget = keyqueueput = 0;
	//dbg_putchar('x');
}

/* Начальное значение параметров управления трактом - для исключения использования неинициализированных значений. */
static const FLASHMEM 
	portholder_t txgfva0 [TXGFI_SZIE] =	// усостояния выходов для разных режимов
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB }; // для SSB
static const FLASHMEM 
	uint_fast8_t sdtnva0 [TXGFI_SZIE] =	// признаки включения самоконтроля для разных режимов
		{ 0, 0, 0, 0 };	// для SSB

static const portholder_t FLASHMEM * txgfp = txgfva0;	// параметры управления трактом
static const uint_fast8_t FLASHMEM * sdtnp = sdtnva0;	// параметры управления самоконтролем

/* как включать тракт в данном режиме работы из прерываний */
void seq_set_txgate_P(
	const portholder_t FLASHMEM * atxgfp, 
	const uint_fast8_t FLASHMEM * asdtnp
	)
{
	disableIRQ();
	txgfp = atxgfp;	// параметры управления трактом
	sdtnp = asdtnp;	// параметры управления самоконтролем
	enableIRQ();
}	

/* в зависимсти от текущего режима возвращает признак для перехода (или невыхода) из
   состояния передачи. Формирует сигналы управления трактом передачи и самоконтроля.

   Вызывается из seq_spool_ticks().
*/
static uint_fast8_t 
//NOINLINEAT
seqhastxrequest(void)
{
	
	if (exttunereq)	
	{
		// режим "настройка - включить несущую - или включить режим AM
		return 1;
	}
	if (ptt)	
	{
		// педаль или через CAT
		return 1;
	}
	
	if (seq_cwenable)
	{
		// CW
		return (bkin_count != 0);
	}
	else
	{
		// SSB, AM, FM
		return seq_voxenable && vox_count != 0;
	}

	// причин оставаться в режиме передачи больше нет.
	// выключаем если надо всё и переходим к приёму.
	return 0;
}

static void
seq_txpath_set(portholder_t txpathstate)
{
#if WITHINTEGRATEDDSP
	dsp_txpath_set(txpathstate);
#endif
	hardware_txpath_set(txpathstate);
}

// 
// сиквенсор переключения приём-передача и назад.
// переключение на передачу и приём осуществляется из user-mode программ
// (через spi-управляемые компоненты). после переключения на передачу
// отмеряется задержка до следующего тика (или больше) rxtxticks и выдаётся арраратный
// сигнал на разрешение формирование телеграфного или ssb сигнала.
// при переходе на приём сперва снимается аппаратный сигнал,
// затем после задержки rxtxticks выдаётся команда
// в программно управляемые компоненты приёмопередатчика.
//
// hardware_txsignal_enable(txgate) - аппаратное управление выдачей несущей
// board_sidetone_enable(tx) - выдача сигнала самоконтроля
//


// вызывается из обработчика прерываний. Желательно вызывать самым первым для уменьшения
// паразитного дрейфа по времени

static void 
seq_spool_ticks(void * ctc)
{
	// Поместить в линию задержки данные со стороны источника манипуляции
	//
	const uint_fast8_t keydown = elkey_get_output();	// а так же состояния ручной манипуляции, манипуляции от CAT...
	if (keydown && seq_bkinenable && seq_cwenable)
	{
		bkin_count = bkin_delay;
	}

	// Выдача сигнала самоконтроля в зависимости от состояния сиквенсора.
	// самоконтроль от ключа передается всегда, может добавится тон от состояния TUNE.
	board_sidetone_enable(sdtnp [seqtxgfi [seqstate] [keydown]] || keydown); // - выдача сигнала самоконтроля
	// использование данных с выхода линии задержки
	/* часть сиквенсора */
	// обработка таймера mox
	if (bkin_count != 0)
		-- bkin_count;
	// обработка таймера vox
	if (vox_count != 0)
		-- vox_count;

	/* В очередь всегда должно помещаться что-либо */
	if (bkin_count && seq_cwenable)
		keyqueuein(keydown);	// В режиме break-in: поместить нажатие в очередь FIFO
	else if (seq_cwenable && seq_get_txstate())
		keyqueuein(keydown);	// В режиме ручного перехода на передачу: поместить нажатие в очередь FIFO
	else
		keyqueuein(0);

	const uint_fast8_t keydowndly = keyqueueout();
	/* переходы по состояниям сиквенсора, в которых отсчитываются тики системного таймера */
	switch (seqstate)
	{
	case SEQST_INITIALIZE:
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [0]]);	// - аппаратное управление выдачей несущей
		// начальное состояние - проверяются источники запросов на переход в режим передачи
		if (seqhastxrequest() != 0)
		{
			// Если запрос на передачу появился - выполняем всю последовательность действий до перехода на передачу.
			// Начинаем переключаться на передачу
			if ((seqpushtime = pretxticks) == 0)
			{
				// обычная работа
				seqstate = SEQST_WAIT_ACK_TX;
			}
			else
			{
				// Ждём, пока в SW-2011-RDX выключится сигнал TX1 и будем переходить на передачу.
				seqstate = SEQST_PUSHED_WAIT_ACK_TX;	
			}
		}
		break;

	case SEQST_PUSHED_SWITCHTORX:	
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [0]]);	// - аппаратное управление выдачей несущей
		// ждем перехода на на приём от пользовательской программы
		// обработка ожиданий времени при переходах между состояниями.
		if (-- seqpushtime == 0)
		{
			seqstate = SEQST_SWITCHTORX;	// на следующем такте будет выполняться запомненное состояние.
		}
		break;


	case SEQST_PUSHED_HOLDTX:
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [0]]);	// - аппаратное управление выдачей несущей
		// обработка ожиданий времени при переходах между состояниями.
		// ждём, когда переключится антенное реле на передачу
		if (-- seqpushtime == 0)
		{
			seqstate = SEQST_HOLDTX;	// на следующем такте будет выполняться запомненное состояние.
		}
		break;

	// перед переходом на приём, после отпускания тангенты - формирование звукового сигнала (roger beep)
	case SEQST_RGBEEP:
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [keydowndly]]);	// - аппаратное управление выдачей несущей
		if (-- seqpushtime != 0)
		{
		}
		else if ((seqpushtime = txrxticks) == 0)
		{
			seqstate = SEQST_SWITCHTORX;
		}
		else
		{
			/* ждём указанное время и перепрограммируем (через SPI) на приём */
			seqstate = SEQST_PUSHED_SWITCHTORX;	// ждем перехода на на приём от пользовательской программы
		}
	break;

	case SEQST_PUSHED_WAIT_ACK_TX:
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [0]]);	// - аппаратное управление выдачей несущей
		// Ждём, пока в SW-2011-RDX выключится сигнал TX1 и будем переходить на передачу.
		if (-- seqpushtime == 0)
		{
			seqstate = SEQST_WAIT_ACK_TX;	// на следующем такте будет выполняться запомненное состояние.
		}
		break;

	// Режм передачи. Поскольку режим работы может поменяться
	// до перехода в режим приёма, комбинация условий режима работы проверяется
	// на каждом "тике" таймера.
	case SEQST_HOLDTX:
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [keydowndly]]);	// - аппаратное управление выдачей несущей
		if (seqhastxrequest() == 0)
		{
			// Если запрос на передачу исчез - выполняем всю последовательность действий до перехода на приём.
			// Проверяем, не надо ли формировать roger beep
			if (seq_rgbeep != 0 && (seqpushtime == rgbeepticks) != 0)
			{
				seqstate = SEQST_RGBEEP;
			}
			else if ((seqpushtime = txrxticks) == 0)
			{
				seqstate = SEQST_SWITCHTORX;
			}
			else
			{
				/* ждём указанное время и перепрограммируем (через SPI) на приём */
				seqstate = SEQST_PUSHED_SWITCHTORX;	// ждем перехода на на приём от пользовательской программы
			}
		}
		break;

	// ожидание перепрограммирования переиферии на передачу - пока переключаем SPI регистры
	// ждём перехода user mode программы на передачу
	case SEQST_WAIT_ACK_TX:
		if (usertxstate != 0)
		{
			// дождались ответа от user-mode программы - переключение на передачу произошло
			/* ждём и включаем несущую */
			if ((seqpushtime = rxtxticks) == 0)
			{
				seqstate = SEQST_HOLDTX;
			}
			else
			{
				seqstate = SEQST_PUSHED_HOLDTX;	// ждём, когда переключится антенное реле на передачу
			}
		}
		break;

	case SEQST_SWITCHTORX:
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [0]]);	// - аппаратное управление выдачей несущей
		// ожидание перепрограммирования переиферии на приём - пока переключаем SPI регистры
	// ожидание перепрограммирования переиферии на приём - пока переключаем SPI регистры
	// ждём перехода user mode программы на приём
		if (usertxstate == 0)
		{
			// уже всё перепрограммировалось на приём
			// переход в состояние ожидания
			seqstate = SEQST_INITIALIZE;
		}
		break;
	}
}


/* обработка меню - установить задержку пре переходе на передачу и обратно. */
void seq_set_rxtxdelay(
	uint_fast8_t rxtxdelay, 
	uint_fast8_t txrxdelay,
	uint_fast8_t pretxdelay
	)
{
	const uint_fast8_t arxtxticks = NTICKS(rxtxdelay); // задержка пре переходе на передачу
	const uint_fast8_t atxrxticks = NTICKS(txrxdelay); // и обратно
	const uint_fast8_t apretxticks = NTICKS(pretxdelay); // задержка перед переходом на передачу
	if (arxtxticks != rxtxticks || atxrxticks != txrxticks || apretxticks != pretxticks)
	{
		disableIRQ();
		rxtxticks = arxtxticks;
		txrxticks = atxrxticks;
		pretxticks = apretxticks;
		keyqueueclear();
		uint_fast8_t n = (pretxticks + rxtxticks) * 2;
		while (n --)
			keyqueuein(0);
		enableIRQ();
	}
}


/* инициализация сиквенсора и телеграфного ключа. Выполняется при запрещённых прерываниях. */
void seq_initialize(void)
{
	ticker_initialize(& seqticker, 1, seq_spool_ticks, NULL);

	hardware_ptt_port_initialize();		// инициализация входов управления режимом передачи и запрета передачи

	hardware_txpath_initialize();
	//seq_set_txgate_P(txgfva0, sdtnva0);	// Сделано статической инициализацией
	seq_txpath_set(TXGFV_RX);	// - аппаратное управление выдачей несущей - в состояние приём
	board_sidetone_enable(0); // - остановить выдачу сигнала самоконтроля


	rxtxticks = NTICKS(WITHMAXRXTXDELAY);	// 15 ms задержка пре переходе на передачу
	txrxticks = NTICKS(WITHMAXTXRXDELAY);	// 15 ms задержка пре переходе на приём
	pretxticks = NTICKS(WITHMAXTXRXDELAY);

	keyqueueclear();
	uint_fast8_t n = (pretxticks + rxtxticks) * 2;	// Умножение на 2 просто "от балды", в некоторых состояниях основного цикла задержка больше той, что в скобках.
	while (n --)
		keyqueuein(0);
	
	rgbeepticks = NTICKS(200);	// длительность roger beep
	////exttunereq =
	////usertxreq =
	////usertxstate = 0;

	seqstate = SEQST_INITIALIZE;

	// compile-time check
	if (QUEUEUESIZE > 256)
	{
		extern void wrong_QUEUEUESIZE_holder_type(void);
		wrong_QUEUEUESIZE_holder_type();
		for (;;)
			;
	}
}

/* очистка запомненных нажатий до этого момента. Вызывается из user-mode программы */
void seq_purge(void)
{
	//disableIRQ();
	//enableIRQ();
}

// запрос из user-mode части программы на переход на передачу для tune.
void seq_txrequest(uint_fast8_t tune, uint_fast8_t aptt)
{
	disableIRQ();
	exttunereq = tune;
	ptt = aptt;
	enableIRQ();
}

/* подтверждение от user-mode программы о том, что смена режима приём-передача осуществлена */
void seq_ask_txstate(
	uint_fast8_t tx)	/* 0 - периферия находимся в состоянии приёма, иначе - в состоянии передачи */
{
	disableIRQ();

	usertxstate = tx;

	enableIRQ();
}



#else	/* WITHTX */

static void 
seq_spool_ticks(void * ctc)
{
	const uint_fast8_t keydown = elkey_get_output();	// а так же состояния ручной манипуляции, манипуляции от CAT...

	// Выдача сигнала самоконтроля в зависимости от состояния сиквенсора.
	// самоконтроль от ключа передается всегда, может добавится тон от состояния TUNE.
	board_sidetone_enable(keydown); // - выдача сигнала самоконтроля
}


/* заглушки функций для работы в случае только приёмника. */
void seq_txrequest(uint_fast8_t tune, uint_fast8_t ptt)
{
}

uint_fast8_t seq_get_txstate(void)
{
	return 0;
}

void seq_ask_txstate(uint_fast8_t tx)
{
}

/* инициализация сиквенсора и телеграфного ключа. Выполняется при запрещённых прерываниях. */
void seq_initialize(void)
{
	ticker_initialize(& seqticker, 1, seq_spool_ticks, NULL);
}

/* очистка запомненных нажатий до этого момента. Вызывается из user-mode программы */
void seq_purge(void)
{
}

void 
vox_initialize(void)
{
}

void 
seq_set_bkin_enable(
	uint_fast8_t bkinstate, 			/* разрешение (не-0) или запрещение (0) работы BREAK-IN. */
	uint_fast8_t bkin_delay_tens	/* задержка отпускания break-in в 1/100 секундных интервалах */
	)
{
}


#endif	/* WITHTX */
