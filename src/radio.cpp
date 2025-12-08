/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "buffers.h"
#include "gui/framework/gui.h"
#include "synthcalcs.h"
#include "board.h"
#include "keyboard.h"
#include "encoder.h"
#include "display2.h"
#include "clocks.h"
#include "radio.h"

#include "spi.h"	// bootloader_readimage definition
#include "formats.h"

#include "audio.h"
#include "codecs.h"
#include "bootloader.h"

#include "dspdefines.h"
#include <atomic>

//#define WITHRPTOFFSET 1

#define UI_TICKS_PERIOD 50	// ms
#define UINTICKS(v) (((v) + (UI_TICKS_PERIOD - 1)) / UI_TICKS_PERIOD)

#if WITHUSEUSBBT
#include "btstack.h"
#endif /* WITHUSEUSBBT */

#if WITHFT8
	#include "ft8.h"
#endif /* WITHFT8 */

#if XVTR_R820T2
	extern uint8_t ad936x_active;
#endif /* XVTR_R820T2 */

#if WITHUSEFATFS
	#include "fatfs/ff.h"
#endif /* WITHUSEFATFS */
#if WITHUSEFATFS
	#include "sdcard.h"
#endif /* WITHUSEFATFS */

#include <string.h>
#include <ctype.h>
#include <math.h>

#include "src/speex/arch.h"
#include "src/speex/speex_preprocess.h"

static uint_fast8_t board_wakeup(void);
static uint_fast8_t
processcatmsg(uint_fast8_t catcommand1,
	uint_fast8_t catcommand2,
	uint_fast8_t cathasparam,
	uint_fast8_t catpcount,	// количество символов за кодом команды
	const uint8_t * catp	// массив символов
	);

typedef struct keyevent_tag
{
	uint_fast8_t kbready;
	uint_fast16_t kbch;
} keyevent_t;

void keyevent_initialize(keyevent_t * e)
{
	e->kbready = 0;
}

typedef struct knobevent_tag
{
	encoder_t * enc;
	int16_t delta;
	uint_fast8_t jumpsize;
} knobevent_t;

void knobevent_initialize(knobevent_t * e, encoder_t * aenc)
{
	e->delta = 0;
	e->enc = aenc;
}

typedef struct mouseevent_tag
{
	int8_t pressed;
	uint16_t x;
	uint16_t y;
} mouseevent_t;

void mouseevent_initialize(mouseevent_t * e)
{
	e->pressed = 0;
}

typedef struct inputevent_tag
{
	keyevent_t frontkeyevent;
	keyevent_t dtmfkeyevent;
#if WITHENCODER
	knobevent_t encMAIN;
#if WITHENCODER_SUB
	knobevent_t encSUB;
#endif /* WITHENCODER_SUB */
#if WITHENCODER2
	knobevent_t encFN;
#endif /* WITHENCODER2 */
#if WITHENCODER_1F
	knobevent_t encF1;
#endif /* WITHENCODER_1F */
#if WITHENCODER_2F
	knobevent_t encF2;
#endif /* WITHENCODER_2F */
#if WITHENCODER_3F
	knobevent_t encF3;
#endif /* WITHENCODER_3F */
#if WITHENCODER_4F
	knobevent_t encF4;
#endif /* WITHENCODER_4F */
#endif /* WITHENCODER */
	mouseevent_t mouse;
} inputevent_t;

void inputevent_initialize(inputevent_t * e)
{
	keyevent_initialize(& e->frontkeyevent);
	keyevent_initialize(& e->dtmfkeyevent);
#if WITHENCODER
	knobevent_initialize(& e->encMAIN, & encoder1);
#if WITHENCODER_SUB
	knobevent_initialize(& e->encSUB, & encoder_sub);
#endif /* WITHENCODER_SUB */
#if WITHENCODER2
	knobevent_initialize(& e->encFN, & encoder2);
#endif /* WITHENCODER2 */
#if WITHENCODER_1F
	knobevent_initialize(& e->encF1, & encoder_ENC1F);
#endif /* WITHENCODER_1F */
#if WITHENCODER_2F
	knobevent_initialize(& e->encF2, & encoder_ENC2F);
#endif /* WITHENCODER_2F */
#if WITHENCODER_3F
	knobevent_initialize(& e->encF3, & encoder_ENC3F);
#endif /* WITHENCODER_3F */
#if WITHENCODER_4F
	knobevent_initialize(& e->encF4, & encoder_ENC4F);
#endif /* WITHENCODER_4F */
#endif /* WITHENCODER */
	mouseevent_initialize(& e->mouse);
}

static uint_fast16_t gstep_ENC_MAIN;
static uint_fast16_t gstep_ENC2;	/* шаг для второго валкодера в режимие подстройки частоты */
static uint_fast16_t gencderate = 1;

#if WITHENCODER

#if defined (ENCDIV_DEFAULT)
	static uint_fast8_t genc1div = ENCDIV_DEFAULT;	/* во сколько раз уменьшаем разрешение валкодера. */
#else /* defined (ENCDIV_DEFAULT) */
	static uint_fast8_t genc1div = 1;	/* во сколько раз уменьшаем разрешение валкодера. */
#endif /* defined (ENCDIV_DEFAULT) */
#if defined (ENCDYNAMIC_DEFAULT)
	static uint_fast8_t genc1dynamic = ENCDYNAMIC_DEFAULT;
#else /* defined (ENCDYNAMIC_DEFAULT) */
	static uint_fast8_t genc1dynamic = 1;
#endif /* defined (ENCDYNAMIC_DEFAULT) */

#if defined (BOARD_ENCODER2_DIVIDE)
	static uint_fast8_t genc2div = BOARD_ENCODER2_DIVIDE;
#else /* defined (BOARD_ENCODER2_DIVIDE) */
	static uint_fast8_t genc2div = 2;	/* значение для валкодера PEC16-4220F-n0024 (с трещёткой") */
#endif /* defined (BOARD_ENCODER2_DIVIDE) */
static uint_fast8_t genc2dynamic = 0;



static int_least16_t event_getRotateHiRes(knobevent_t * e, uint_fast8_t * jsize, unsigned derate)
{
	div_t d;
	d = div(e->delta, derate);
	encoder_pushback(e->enc, d.rem);
	* jsize = e->jumpsize;
	e->delta = 0;
	return d.quot;
}


/* получение "редуцированного" количества прерываний от валкодера.
 * То что осталось после деления на scale, остается в накопителе
 */
static int_least16_t event_getRotate_Menu(knobevent_t * e)
{
	const int derate = encoder_get_actualresolution(e->enc) * genc1div / ENCODER_MENU_STEPS;
	div_t d;
	d = div(e->delta, derate);
	encoder_pushback(e->enc, d.rem);
	e->delta = 0;
	return d.quot;
}

static int_least16_t event_getRotate_LoRes(knobevent_t * e, int derate)
{
	div_t d;
	d = div(e->delta, derate);
	encoder_pushback(e->enc, d.rem);
	e->delta = 0;
	return d.quot;
}

static void event_pushback_LoRes(knobevent_t * e, int_least16_t delta, int derate)
{
	encoder_pushback(e->enc, (int) delta * derate);
}

#else /* WITHENCODER */

static uint_fast8_t genc1div = 1;	/* во сколько раз уменьшаем разрешение валкодера. */

#endif /* WITHENCODER */



/* система отказа от передачи при аварийных ситуациях */
typedef struct edgepin_tag
{
	LIST_ENTRY item;
	//uint8_t outstate;	/* результирующее состояние */
	uint8_t prevstate;
	uint8_t posedge;
	uint8_t negedge;
	uint8_t req;
	uint_fast8_t (* getpin)(void);
} edgepin_t;

void edgepin_initialize(LIST_ENTRY * list, edgepin_t * egp, uint_fast8_t (* fn)(void));

typedef enum txreqst_values
{
	TXREQST_RX,	// приём
	TXREQST_TX,	// передача
	TXREQST_TXDATA,	// передача с USB
	TXREQST_TXTONE,	// передача тестового сигнала
	TXREQST_TXAUTOTUNE,	// автонастройка тюнера
	//
	TXREQ_count
} txreqst_t;

typedef struct txreq_tag
{
	LIST_ENTRY edgepins;	/* писок входов, для которых отслеживается состояние */
	edgepin_t edgphandptt;	// тангента/педаль
	edgepin_t edgpcathwptt;	// CAT rts/dtr
	edgepin_t edgpelkeyptt;	// ELKEY activity
	edgepin_t edgpexttune;	// внешний запрос на выдачу несущей

	txreqst_t state;
} txreq_t;

void txreq_initialize(txreq_t * txreqp);
void txreq_process(void);		/* Установка сиквенсору запроса на передачу и работа с аппаратурой	*/

void txreq_reqautotune(txreq_t * txreqp, uint_fast8_t v);	// Выход из режима - txreq_rx
uint_fast8_t txreq_getreqautotune(const txreq_t * txreqp);
uint_fast8_t txreq_gethint(const txreq_t * txreqp);
void txreq_txtone(txreq_t * txreqp);
uint_fast8_t txreq_gettxtone(const txreq_t * txreqp);	/* возвращаем не-0, если есть запрос на tune от пользователя или CAT */
void txreq_mox(txreq_t * txreqp);
uint_fast8_t txreq_get_tx(const txreq_t * txreqp);
void txreq_rx(txreq_t * txreqp, const char * label);	/* переход на приём (сброс всех запросов) */
uint_fast8_t txreq_gettxdata(const txreq_t * txreqp);
void txreq_txdata(txreq_t * txreqp);	// передача с USB

static txreq_t txreqst0;

static uint_fast8_t gtx;	/* текущее состояние прием или передача */

/* обработка сообщений от уровня обработчиков прерываний к user-level функциям. */
void
processmessages(
	uint_fast16_t * kbch,
	uint_fast8_t * kbready
	)
{
	if (hardware_getshutdown())	// признак провала питания
	{
		display_uninitialize();	// выключаем дисплей
		txreq_rx(& txreqst0, NULL);	// переходим на приём
		for (;;)				// вешаемся...
			;
	}

	board_dpc_processing();		// обработка отложенного вызова user mode функций
	watchdog_ping();

#if WITHLVGL && WITHLVGLINDEV

	* kbch = KBD_CODE_MAX;
	* kbready = 0;

#else /* WITHLVGL && WITHLVGLINDEV */

	if ((* kbready = kbd_scan(kbch)) != 0)
	{
		if (board_wakeup() && * kbch != KBD_CODE_POWEROFF)
			* kbch = KBD_CODE_MAX;	// первое нажатие в спящем режиме игнорируеся и используется только для пробуждения
	}
	else
		* kbch = KBD_CODE_MAX;

#endif /* WITHLVGL && WITHLVGLINDEV */

	uint8_t * buff;
	switch (takemsgready(& buff))
	{
	case MSGT_EMPTY:
		return;

	case MSGT_CAT:
		board_wakeup();
#if WITHCAT
		{
			// check MSGBUFFERSIZE8 valie
			// 12 bytes as parameter
			//PRINTF(PSTR("processmessages: MSGT_CAT\n"));
			if (processcatmsg(buff [0], buff [1], buff [2], buff [8], buff + 9))
			{
				//display2_needupdate();			/* Обновление дисплея - всё, включая частоту */
			}
		}
#endif /* WITHCAT */
		break;

	default:
		break;
	}
	releasemsgbuffer(buff);
}

void inputevent_fill(inputevent_t * e)
{
	processmessages(& e->frontkeyevent.kbch, & e->frontkeyevent.kbready);
	e->dtmfkeyevent.kbready = dtmf_scan(& e->dtmfkeyevent.kbch);
	if (e->dtmfkeyevent.kbready)
	{
		PRINTF("dtmfkey=%02X\n", (unsigned char) e->dtmfkeyevent.kbch);
	}

#if WITHENCODER
	// main encoder

	e->encMAIN.delta = encoder_getrotatehires(e->encMAIN.enc, & e->encMAIN.jumpsize);
#if WITHENCODER_SUB
	e->encSUB.delta = encoder_getrotatehires(e->encSUB.enc, & e->encSUB.jumpsize);
#endif /* WITHENCODER_SUB */
#if WITHENCODER2
	e->encFN.delta = encoder_get_delta(e->encFN.enc);
	e->encFN.jumpsize = 1;
#endif /* WITHENCODER2 */
#if WITHENCODER_1F
	e->encF1.delta = encoder_get_delta(e->encF1.enc);
	e->encF1.jumpsize = 0;
#endif /* WITHENCODER_1F */
#if WITHENCODER_2F
	e->encF2.delta = encoder_get_delta(e->encF2.enc);
	e->encF2.jumpsize = 0;
#endif /* WITHENCODER_2F */
#if WITHENCODER_3F
	e->encF3.delta = encoder_get_delta(e->encF3.enc);
	e->encF3.jumpsize = 0;
#endif /* WITHENCODER_3F */
#if WITHENCODER_4F
	e->encF4.delta = encoder_get_delta(e->encF4.enc);
	e->encF4.jumpsize = 0;
#endif /* WITHENCODER_4F */
#endif /* WITHENCODER */
}

// Определения для работ по оптимизации быстродействия
#if WITHDEBUG && 0

	// stm32f746, no dualwatch:
	//	dtcount=0, dtmax=0, dtlast=0, dtcount2=41807716, dtmax2=1244, dtlast2=739, dtcount3=41806755, dtmax3=1446, dtlast3=916
	// R7S721xxx, Neon, dualwatch:
	//	dtcount=0, dtmax=0, dtlast=0, dtcount2=15890107, dtmax2=1119, dtlast2=590, dtcount3=31778668, dtmax3=1169, dtlast3=723
	// R7S721xxx, no Neon, dualwatch:
	// dtcount=0, dtmax=0, dtlast=0, dtcount2=184728, dtmax2=1461, dtlast2=911, dtcount3=367872, dtmax3=1169, dtlast3=713

	static volatile uint_fast32_t dtmax = 0, dtlast = 0, dtcount = 0;
	static volatile uint_fast32_t dtmax2 = 0, dtlast2 = 0, dtcount2 = 0;
	static volatile uint_fast32_t dtmax3 = 0, dtlast3 = 0, dtcount3 = 0;
	static uint_fast32_t perft = 0;
	static uint_fast32_t perft2 = 0;
	static uint_fast32_t perft3 = 0;

	static void debug_cleardtmax(void)
	{
		dtmax = 0;
		dtmax2 = 0;
		dtmax3 = 0;
	}

	#define BEGIN_STAMP() do { \
			perft = cpu_getdebugticks(); \
		} while (0)

	#define END_STAMP() do { \
			const uint_fast32_t t2 = cpu_getdebugticks(); \
			if (perft < t2) \
			{ \
				const uint_fast32_t vdt = t2 - perft; \
				dtlast = vdt; /* текущее значение длительности */ \
				if (vdt > dtmax) \
					dtmax = vdt; /* максимальное значение длительности */ \
				++ dtcount; \
			} \
		} while (0)

	#define BEGIN_STAMP2() do { \
			perft2 = cpu_getdebugticks(); \
		} while (0)

	#define END_STAMP2() do { \
			const uint_fast32_t t2 = cpu_getdebugticks(); \
			if (perft2 < t2) \
			{ \
				const uint_fast32_t vdt = t2 - perft2; \
				dtlast2 = vdt; /* текущее значение длительности */ \
				if (vdt > dtmax2) \
					dtmax2 = vdt; /* максимальное значение длительности */ \
				++ dtcount2; \
			} \
		} while (0)

	#define BEGIN_STAMP3() do { \
			perft3 = cpu_getdebugticks(); \
		} while (0)

	#define END_STAMP3() do { \
			const uint_fast32_t t2 = cpu_getdebugticks(); \
			if (perft3 < t2) \
			{ \
				const uint_fast32_t vdt = t2 - perft3; \
				dtlast3 = vdt; /* текущее значение длительности */ \
				if (vdt > dtmax3) \
					dtmax3 = vdt; /* максимальное значение длительности */ \
				++ dtcount3; \
			} \
		} while (0)


	//static uint32_t dd [4];
	/* DSP speed test */
	void main_speed_diagnostics(void)
	{
		//PRINTF(PSTR("data=%08lX,%08lX,%08lX,%08lX\n"), dd [0], dd [1], dd [2], dd [3]);
		PRINTF(PSTR("dtcount=%" PRIuFAST32 ", dtmax=%" PRIuFAST32 ", dtlast=%" PRIuFAST32 ", "), dtcount, dtmax, dtlast);
		PRINTF(PSTR("dtcount2=%" PRIuFAST32 ", dtmax2=%" PRIuFAST32 ", dtlast2=%" PRIuFAST32 ", "), dtcount2, dtmax2, dtlast2);
		PRINTF(PSTR("dtcount3=%" PRIuFAST32 ", dtmax3=%" PRIuFAST32 ", dtlast3=%" PRIuFAST32 "\n"), dtcount3, dtmax3, dtlast3);
	}

#else /* WITHDEBUG */

	#define BEGIN_STAMP() do { \
		} while (0)

	#define END_STAMP() do { \
		} while (0)

	#define BEGIN_STAMP2() do { \
		} while (0)

	#define END_STAMP2() do { \
		} while (0)

	#define BEGIN_STAMP3() do { \
		} while (0)

	#define END_STAMP3() do { \
		} while (0)

	static void debug_cleardtmax(void)
	{
	}

	void main_speed_diagnostics(void)
	{
	}

#endif /* WITHDEBUG */

#if WITHRFSG
	#error WITHRFSG now not supported
#endif /* WITHRFSG */

#define NUMLPFADJ 16	/* Коррекция мощности по ФНЧ передачика - количество ФНЧ. */

/* на плату/dsp идут значения в диапазоне BOARDPOWERMIN..BOARDPOWERMAX */
#define WITHPOWERTRIMMIN    5    	// Нижний предел регулировки (показываемый на дисплее)
#define WITHPOWERTRIMMAX    100    	// Верхний предел регулировки (показываемый на дисплее)
#define WITHPOWERTRIMATU    10    	// Значение для работы автотюнера

#if WITHTOUCHGUI
static uint_fast8_t keyboard_redirect = 0;	// перенаправление кодов кнопок в менеджер gui
static enc2_menu_t enc2_menu;
static uint_fast8_t encoder2_redirect = 0;

#endif /* WITHTOUCHGUI */

static uint_fast32_t
nextfreq(uint_fast32_t oldfreq, uint_fast32_t freq,
							   uint_fast32_t step, uint_fast32_t top);
static uint_fast32_t
prevfreq(uint_fast32_t oldfreq, uint_fast32_t freq,
							   uint_fast32_t step, uint_fast32_t bottom);

static void tuner_eventrestart(void);

static uint_fast8_t getdefantenna(uint_fast32_t f);
static uint_fast8_t geteffantenna(uint_fast32_t f);	/* действительно выбранная антенна с учетом ручного или автоматического переключения */
static uint_fast8_t geteffrxantenna(uint_fast32_t f);	/* действительно выбранная антенна с учетом ручного или автоматического переключения */

typedef struct dualctl8_tag
{
	uint_fast8_t value;		/* результирующее знаяение для формирования управляющего воздействия и инфопмирования по CAT */
	uint_fast8_t potvalue;	/* значение после функции гистерезиса от потенциометра */
} dualctl8_t;

typedef struct dualctl16_tag
{
	uint_fast16_t value;	/* результирующее знаяение для формирования управляющего воздействия и инфопмирования по CAT */
	uint_fast16_t potvalue;	/* значение после функции гистерезиса от потенциометра */
} dualctl16_t;

typedef struct dualctl32_tag
{
	uint_fast32_t value;	/* результирующее знаяение для формирования управляющего воздействия и инфопмирования по CAT */
	uint_fast32_t potvalue;	/* значение после функции гистерезиса от потенциометра */
} dualctl32_t;

#define BRSCALE 1200U

/* скорость 115200 не добавлена из соображений невозможностти точного формирования на atmega
   при частоте генератора 8 МГц
   */
static const uint_fast8_t catbr2int [] =
{
	1200u / BRSCALE,	// 1200
	2400u / BRSCALE,	// 2400
	4800u / BRSCALE,	// 4800
	9600u / BRSCALE,	// 9600
	19200u / BRSCALE,	// 19200
	38400u / BRSCALE,	// 38400
	57600u / BRSCALE,	// 57600
	115200u / BRSCALE,	// 115200
};

#if WITHLFM

	#define LFMFREQBIAS 20000

#if defined WITHLFMTOFFSET
	static uint_fast16_t lfmtoffset = WITHLFMTOFFSET;
#else
	static uint_fast16_t lfmtoffset = 0;
#endif /* WITHLFMTOFFSET */
	static uint_fast16_t lfmtinterval = 5 * 60;
	static uint_fast8_t lfmmode = 1;
	static uint_fast16_t lfmstart100k = 80;
	static uint_fast16_t lfmstop100k = 350;
	static uint_fast16_t lfmspeed1k = 100;
	static uint_fast16_t lfmfreqbias = LFMFREQBIAS;


// Используются параметры
// lfmtoffset - Секунды от начала часа до запуска
// lfmtinterval - Интервал в секундах между запусками в пределах часа
// возврат не-0 в случае подходящего времени для запуска.
uint_fast8_t
islfmstart(unsigned now)
{
	unsigned s;
	for (s = lfmtoffset; s < 60 * 60; s += lfmtinterval)
	{
		if (s == now)
			return 1;
	}
	return 0;
}

uint_fast16_t hamradio_get_lfmtinterval(void)
{
	return lfmtinterval;
}

void hamradio_set_lfmtinterval(uint_fast16_t v)
{
	if (lfmtinterval < 60 * 60)
		lfmtinterval = v;
}

uint_fast8_t hamradio_get_lfmmode(void)
{
	return lfmmode;
}

void hamradio_set_lfmmode(uint_fast8_t v)
{
	lfmmode = v != 0;

	updateboard();
}

uint_fast16_t hamradio_get_lfmstop100k(void)
{
	return lfmstop100k;
}

void hamradio_set_lfmstop100k(uint_fast16_t v)
{
	if (v > 80 && v <= 350)
		lfmstop100k = v;

	updateboard();
}

uint_fast16_t hamradio_get_lfmtoffset(void)
{
	return lfmtoffset;
}

void hamradio_set_lfmtoffset(uint_fast16_t v)
{
	if (v < 60)
		lfmtoffset = v;

	updateboard();
}

void hamradio_lfm_disable(void)
{
	lfm_disable();
	updateboard();
}

#endif /* WITHLFM */


static uint_fast8_t gcwpitch10 = 700 / CWPITCHSCALE;	/* тон при приеме телеграфа или самоконтроль (в десятках герц) */

#if WITHRPTOFFSET

	// For instance, a 2-meter repeater might be described as "147.36 with a plus offset",
	// meaning that the repeater transmits on 147.36 MHz and receives on 147.96 MHz,
	// 600 kHz above the output frequency.

	enum { RPTOFFSMIN = 0, RPTOFFSHALF = UINT16_MAX / 2, RPTOFFSMAX = UINT16_MAX };
	static uint_fast16_t rptroffshf1k = (+ 300) + RPTOFFSHALF;		/* Repeater offset HF */
	static uint_fast16_t rptroffsvhf1k = (+ 600) + RPTOFFSHALF;		/* Repeater offset UHF */
	static uint_fast8_t rptrhfenable;		/* Repeater offset HF enable flag */
	static uint_fast8_t rptrvhfenable;		/* Repeater offset VHF enable flag */

	static int_fast32_t getrptoffsbase(void)
	{
		return - RPTOFFSHALF;
	}

#endif /* WITHRPTOFFSET */

#if WITHDSPEXTDDC	/* "Воронёнок" с DSP и FPGA */

#endif /* WITHDSPEXTDDC */

//#define DEBUGEXT 1

static uint_fast8_t getbankindex_raw(uint_fast8_t pathi);
static uint_fast8_t getbankindex_ab(uint_fast8_t ab);
static uint_fast8_t getbankindex_pathi(uint_fast8_t pathi);
static uint_fast8_t getbankindex_tx(uint_fast8_t tx);
static uint_fast8_t getbankindex_ab_fordisplay(uint_fast8_t ab);
static uint_fast8_t getsubmode(uint_fast8_t bi);		/* bi: vfo bank index */
static uint_fast8_t getactualmainsubrx(void);
static uint_fast8_t getfreqbandgroup(const uint_fast32_t freq);


const char * pd_getlonglabel(const struct paramdefdef * pd)
{
	return pd->label;
}


const char * pd_getshortlabel(const struct paramdefdef * pd)
{
	return pd->qlabel;
}

static const struct paramdefdef * getmiddlemenu(uint_fast8_t section, uint_fast8_t * active);

// Интерфейсные функции доступа к NVRAM
static uint_fast8_t
//NOINLINEAT
loadvfy8up(
	nvramaddress_t place,
	uint_fast8_t bottom, uint_fast8_t upper, uint_fast8_t def)	// upper - inclusive limit
{
#if HARDWARE_IGNORENONVRAM
	return def;
#endif /* HARDWARE_IGNORENONVRAM */

	if (place == MENUNONVRAM)
		return def;

	const uint_fast8_t v = restore_i8(place);

	// pre-chechk default value added for mode row switching with same column as default
	if (def > upper || def < bottom)
		def = bottom;

	if (v > upper || v < bottom)
	{
		save_i8(place, def);
		return def;
	}
	return v;
}

// Интерфейсные функции доступа к NVRAM

static uint_fast16_t
//NOINLINEAT
loadvfy16up(
	nvramaddress_t place,
	uint_fast16_t bottom, uint_fast16_t upper, uint_fast16_t def)	// upper - inclusive limit
{
#if HARDWARE_IGNORENONVRAM
	return def;
#endif /* HARDWARE_IGNORENONVRAM */

	if (place == MENUNONVRAM)
		return def;

	const uint_fast16_t v = restore_i16(place);

	if (def > upper || def < bottom)
		def = bottom;

	if (v > upper || v < bottom)
	{
		save_i16(place, def);
		return def;
	}
	return v;
}

/* входит ли данный пункт меню в группу разрешённых для показа */
static uint_fast8_t
ismenukinddp(
	const struct paramdefdef * pd,
	uint_fast8_t itemmask
	)
{
	return (pd->qspecial & itemmask) != 0;
}

/* пункт меню для подстройки частот фильтра ПЧ (высокочастотный скат) */
static uint_fast8_t
ismenufilterusb(
	const struct paramdefdef * pd
	)
{
	return ismenukinddp(pd, ITEM_FILTERU);
}

/* пункт меню для подстройки частот фильтра ПЧ (низкочастотный скат) */
static uint_fast8_t
ismenufilterlsb(
	const struct paramdefdef * pd
	)
{
	return ismenukinddp(pd, ITEM_FILTERL);
}


/* Сохранить параметр после редактирования */
static void
savemenuvalue(
	const struct paramdefdef * pd
	)
{
	if (ismenukinddp(pd, ITEM_VALUE))
	{
		unsigned nvalues;
		const unsigned sel = pd->qselector(& nvalues); // индекс параметра в массиве
		const nvramaddress_t nvram = pd->qnvramoffs(pd->qnvram, sel);
		const ptrdiff_t offs = pd->valoffs(sel);
		const uint_fast16_t * const pv16 = pd->apval16 ? pd->apval16 + offs : NULL;
		const uint_fast8_t * const pv8 = pd->apval8 ? pd->apval8 + offs : NULL;

		if (nvram == MENUNONVRAM)
			return;
		if (pv16 != NULL)
		{
			ASSERT3(* pv16 <= pd->qupper, __FILE__, __LINE__, pd->label);
			ASSERT3(* pv16 >= pd->qbottom, __FILE__, __LINE__, pd->label);
			save_i16(nvram, * pv16);		/* сохраняем отредактированное значение */
		}
		else if (pv8 != NULL)
		{
			ASSERT3(* pv8 <= pd->qupper, __FILE__, __LINE__, pd->label);
			ASSERT3(* pv8 >= pd->qbottom, __FILE__, __LINE__, pd->label);
			save_i8(nvram, * pv8);		/* сохраняем отредактированное значение */
		}
	}
}

// Установить значение параметра и сохранить в nvram
void
param_setvalue(
	const struct paramdefdef * pd,
	int_fast32_t v
	)
{
	if (ismenukinddp(pd, ITEM_VALUE))
	{
		unsigned nvalues;
		const unsigned sel = pd->qselector(& nvalues); // индекс параметра в массиве
		const nvramaddress_t nvram = pd->qnvramoffs(pd->qnvram, sel);
		const ptrdiff_t offs = pd->valoffs(sel);
		uint_fast16_t * const pv16 = pd->apval16 ? pd->apval16 + offs : NULL;
		uint_fast8_t * const pv8 = pd->apval8 ? pd->apval8 + offs : NULL;
		// new value validation
		v = v - pd->funcoffs();
		if (v < pd->qbottom)
			v = pd->qbottom;
		else if (v > pd->qupper)
			v = pd->qupper;

		if (pv16 != NULL)
		{
			* pv16 = v;
		}
		else if (pv8 != NULL)
		{
			* pv8 = v;
		}
		savemenuvalue(pd);
	}
}

int_fast32_t
param_getvalue(
	const struct paramdefdef * pd
	)
{
	if (ismenukinddp(pd, ITEM_VALUE))
	{
		unsigned nvalues;
		const unsigned sel = pd->qselector(& nvalues); // индекс параметра в массиве
		const nvramaddress_t nvram = pd->qnvramoffs(pd->qnvram, sel);
		const ptrdiff_t offs = pd->valoffs(sel);
		const uint_fast16_t * const pv16 = pd->apval16 ? pd->apval16 + offs : NULL;
		const uint_fast8_t * const pv8 = pd->apval8 ? pd->apval8 + offs : NULL;

		// получение значения для отображения
		if (ismenufilterlsb(pd))
		{
			const filter_t * const filter = CONTAINING_RECORD(pv16, filter_t, low_or_center);
			return getlo4baseflt(filter) + * pv16;
		}
		if (ismenufilterusb(pd))
		{
			const filter_t * const filter = CONTAINING_RECORD(pv16, filter_t, high);
			return getlo4baseflt(filter) + * pv16;
		}
		if (pv16 != NULL)
		{
			return (int_fast32_t) * pv16 + pd->funcoffs();
		}
		if (pv8 != NULL)
		{
			return (int_fast32_t) * pv8 + pd->funcoffs();
		}
		return pd->qbottom;
	}
	return 0;
}

// Считать значение параметра из nvram
void
param_load(
	const struct paramdefdef * pd
	)
{
	if (ismenukinddp(pd, ITEM_VALUE))
	{
		unsigned nvalues;
		const unsigned sel = pd->qselector(& nvalues); // индекс параметра в массиве
		const nvramaddress_t nvram = pd->qnvramoffs(pd->qnvram, sel);
		const ptrdiff_t offs = pd->valoffs(sel);
		uint_fast16_t * const pv16 = pd->apval16 ? pd->apval16 + offs : NULL;
		uint_fast8_t * const pv8 = pd->apval8 ? pd->apval8 + offs : NULL;
		if (pv16 != NULL)
		{
			// проверка на MENUNONVRAM уже внутри есть
			* pv16 = loadvfy16up(nvram, pd->qbottom, pd->qupper, * pv16);
		}
		else if (pv8 != NULL)
		{
			// проверка на MENUNONVRAM уже внутри есть
			* pv8 = loadvfy8up(nvram, pd->qbottom, pd->qupper, * pv8);
		}
	}
}

/* выравнивание после перехода на следующую частоту, кратную указаному шагу */
/* freq - новая частота, step - шаг */
static uint_fast32_t
nextfreq(uint_fast32_t oldfreq, uint_fast32_t freq,
							   uint_fast32_t step, uint_fast32_t top)
{
	//const ldiv_t r = ldiv(freq, step);
	//freq -= r.rem;
	freq -= (freq % step);

	if (oldfreq > freq || freq >= top)
		return top - step;

	return freq;
}

/* выравнивание после перехода на предидущую частоту, кратную указаному шагу */
/* freq - новая частота, step - шаг */
static uint_fast32_t
prevfreq(uint_fast32_t oldfreq, uint_fast32_t freq,
							   uint_fast32_t step, uint_fast32_t bottom)
{
	const uint_fast32_t rem = (freq % step);
	if (rem != 0)
		freq += (step - rem);
	//const ldiv_t r = ldiv(freq, step);

	//if (r.rem != 0)
	//	freq += (step - r.rem);

	if (oldfreq < freq || freq < bottom)
		return bottom;


	return freq;
}

/* получение следующего числа в диапазоне low..high с "заворотом" */
/* используется при переборе режимов кнопками */
uint_fast16_t
calc_next(uint_fast16_t v, uint_fast16_t low, uint_fast16_t high)
{
	return (v < low || v >= high) ? low : (v + 1);
}

/* получение предыдущего числа в диапазоне low..high с "заворотом" */
/* используется при переборе режимов кнопками */
static uint_fast16_t
calc_prev(uint_fast16_t v, uint_fast16_t low, uint_fast16_t high)
{
	return (v <= low || v > high) ? high : (v - 1);
}


/* получение предыдущего или следующего числа в диапазоне low..high с "заворотом" */
/* используется при переборе режимов кнопками */
static uint_fast16_t
calc_dir(uint_fast8_t reverse, uint_fast16_t v, uint_fast16_t low, uint_fast16_t high)
{
	return reverse ? calc_prev(v, low, high) : calc_next(v, low, high);
}

static uint_fast16_t
calc_delta(uint_fast16_t v, uint_fast16_t low, uint_fast16_t high, int delta)
{
	while (delta < 0)
	{
		v = calc_prev(v, low, high);
		++ delta;
	}
	while (delta > 0)
	{
		v = calc_next(v, low, high);
		-- delta;
	}
	return v;
}

/* модификация паметра по нажатиям - выбор следующего значения
 * из допустимых (с "заворотом" через границы)
 * - возврат не-0  в случае модификации
 * */
static uint_fast8_t
param_keyclick(const struct paramdefdef * pd)
{
	unsigned nvalues;
	const unsigned sel = pd->qselector(& nvalues); // индекс параметра в массиве
	const nvramaddress_t nvram = pd->qnvramoffs(pd->qnvram, sel);
	const ptrdiff_t offs = pd->valoffs(sel);
	uint_fast16_t * const pv16 = pd->apval16 ? pd->apval16 + offs : NULL;
	uint_fast8_t * const pv8 = pd->apval8 ? pd->apval8 + offs : NULL;
	const uint_fast16_t step = pd->qistep;

	if (! ismenukinddp(pd, ITEM_VALUE) || step == ISTEP_RO)
		return 0;

	if (pv16)
		* pv16 = calc_next(* pv16, pd->qbottom, pd->qupper);
	else if (pv8)
		* pv8 = calc_next(* pv8, pd->qbottom, pd->qupper);

	savemenuvalue(pd);
	return 1;
}

/* модификация и сохранение параметра по валкодеру
 * - возврат не-0  в случае модификации */
static uint_fast8_t
param_rotate(const struct paramdefdef * pd, int_least16_t nrotate)
{
	/* редактирование паратметра */
	unsigned nvalues;
	const unsigned sel = pd->qselector(& nvalues); // индекс параметра в массиве
	const nvramaddress_t nvram = pd->qnvramoffs(pd->qnvram, sel);
	const ptrdiff_t offs = pd->valoffs(sel);
	uint_fast16_t * const pv16 = pd->apval16 ? pd->apval16 + offs : NULL;
	uint_fast8_t * const pv8 = pd->apval8 ? pd->apval8 + offs : NULL;
	const uint_fast16_t step = pd->qistep;

	if (! ismenukinddp(pd, ITEM_VALUE) || step == ISTEP_RO)
		return 0;

	if (nrotate == 0)
		return 0;
	if (nrotate < 0)
	{
		// negative change value
		const uint_fast32_t bottom = pd->qbottom;
		if (pv16 != NULL)
		{
			* pv16 =
				prevfreq(* pv16, * pv16 - (- nrotate * step), step, bottom);
		}
		else
		{
			* pv8 =
				prevfreq(* pv8, * pv8 - (- nrotate * step), step, bottom);
		}
	}
	else
	{
		// positive change value
		const uint_fast32_t upper = pd->qupper;
		if (pv16 != NULL)
		{
			* pv16 =
				nextfreq(* pv16, * pv16 + (nrotate * step), step, upper + (uint_fast32_t) step);
		}
		else
		{
			* pv8 =
				nextfreq(* pv8, * pv8 + (nrotate * step), step, upper + (uint_fast32_t) step);
		}
	}
	savemenuvalue(pd);
	return 1;
}

static uint_fast16_t gzero;

static ptrdiff_t valueoffs0(unsigned sel)
{
	(void) sel;
	return 0;
}

static unsigned getselector0(unsigned * count)
{
	* count = 1;
	return 0;
}

static nvramaddress_t nvramoffs0(nvramaddress_t base, unsigned sel)
{
	(void) sel;
	return base;
}

static int_fast32_t getzerobase(void)
{
	return 0;
}

static const struct paramdefdef xgdummy =
{
	QLABEL(""), 7, 0, RJ_UNSIGNED, 	ISTEP_RO,	// тип процессора
	ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
	0, 0,
	MENUNONVRAM,
	getselector0, nvramoffs0, valueoffs0,
	& gzero,
	NULL,
	getzerobase,
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};

#if WITHIF4DSP
struct rxaproc_tag;
static FLOAT_t * afpnoproc(uint_fast8_t pathi, struct rxaproc_tag *, FLOAT_t * p);
static FLOAT_t * afpcw(uint_fast8_t pathi, struct rxaproc_tag *, FLOAT_t * p);
static FLOAT_t * afpcwtx(uint_fast8_t pathi, struct rxaproc_tag *, FLOAT_t * p);
static FLOAT_t * afpssb(uint_fast8_t pathi, struct rxaproc_tag *, FLOAT_t * p);
static FLOAT_t * afpssbtx(uint_fast8_t pathi, struct rxaproc_tag *, FLOAT_t * p);
static FLOAT_t * afprtty(uint_fast8_t pathi, struct rxaproc_tag *, FLOAT_t * p);
#endif /* WITHIF4DSP */

#if WITHCAT

static void processcat_enable(uint_fast8_t catenable);
static void NOINLINEAT cat_answer_request(uint_fast8_t catindex);	// call from user-mode

static uint_fast8_t aistate;		/* autoinformation state flag */
#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)
static uint_fast8_t rmstate;		/* RM answer state type (1..3) */
#endif /* WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) */

// add/remove codes: modify also catanswers table.
enum
{
#if WITHELKEY
	CAT_KY0_INDEX,		// ky0answer()	// обрабатыаем первой для ускорения реакции
	CAT_KY1_INDEX,		// ky1answer()	// обрабатыаем первой для ускорения реакции
	CAT_KS_INDEX,		// ksanswer()
#endif /* WITHELKEY */
	CAT_PS_INDEX,		// psanswer()
	CAT_SM0_INDEX,		// sm0answer()
	CAT_SM9_INDEX,		// sm1answer()
	CAT_RA_INDEX,		// raanswer()
	CAT_PA_INDEX,		// paanswer()
#if WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2
	CAT_AN_INDEX,		// ananswer()
#endif /* WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2 */
#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)
	CAT_RM1_INDEX,		// rm1answer()
	CAT_RM2_INDEX,		// rm2answer()
	CAT_RM3_INDEX,		// rm3answer()
#endif /* WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) */
#if WITHTX && WITHAUTOTUNER
	CAT_AC_INDEX,		// acanswer()
#endif /* WITHTX && WITHAUTOTUNER */
	CAT_ID_INDEX,		// idanswer()
	CAT_FV_INDEX,		// fvanswer()
	CAT_DA_INDEX,		// daanswer()
	CAT_FA_INDEX,		// faanswer()
	CAT_FB_INDEX,		// fbanswer()
	CAT_SP_INDEX,		// spanswer()
	CAT_FR_INDEX,		// franswer()
	CAT_FT_INDEX,		// ftanswer()
	CAT_MD_INDEX,		// mdanswer()
	CAT_TX_INDEX,		// txanswer()
	CAT_RX_INDEX,		// rxanswer()
	CAT_AI_INDEX,		// aianswer()
	CAT_PT_INDEX,		// ptanswer()
	CAT_IF_INDEX,		// ifanswer()
	CAT_FW_INDEX,		// fwanswer()
#if WITHIF4DSP
	CAT_ZY_INDEX,		// zyanswer()
	CAT_ZZ_INDEX,		// zzanswer()
#endif
#if WITHIF4DSP
	CAT_RG_INDEX,		// rganswer()
	CAT_AG_INDEX,		// aganswer()
	CAT_SQ_INDEX,		// sqanswer()
#endif /* WITHIF4DSP */
#if WITHPOWERTRIM && WITHTX
	CAT_PC_INDEX,		// pcanswer()
#endif /* WITHPOWERTRIM && WITHTX */
#if WITHIF4DSP
	CAT_NR_INDEX,		// nranswer()
#endif /* WITHIF4DSP */
	CAT_BADCOMMAND_INDEX,		// badcommandanswer()
	//
	CAT_MAX_INDEX
};

static uint8_t cat_answer_map [CAT_MAX_INDEX];
static uint8_t cat_answerparam_map [CAT_MAX_INDEX];


static uint_fast32_t savedbaudrate;	// Скорость, на которую в последний раз был запрограммирован порт.

static void
cat_set_speed(uint_fast32_t baudrate)
{
	/* На ATMega и ATXMega перепрограммирование вызывает сбой при приёме если идёт передача из контроллера. */
	if (savedbaudrate == baudrate)
		return;
	savedbaudrate = baudrate;
	HARDWARE_CAT_SET_SPEED(baudrate);
}

#endif /* WITHCAT */


enum {
	VFOMODES_VFOINIT,
	VFOMODES_VFOSPLIT,
	//
	VFOMODES_COUNT
};

#if WITHNOATTNOPREAMP

	/* строки, выводимые на индикатор для обозначения режимов.
	 */
	static const struct {
		uint8_t codeatt;	// признак включения аттенюатора
		uint8_t codepre;	// признак включения предусилителя
		int16_t atten10;	// результирующее затухание
		const char * label;
	}  attmodes [] =
	{
		{ 0, 0, 0, "", },
	};

	/* строки, выводимые на индикатор для обозначения режимов.
	 */
	static const struct {
		uint8_t code;
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  pampmodes [] =
	{
		{ 0, "", 0 },	// три символа нужны для стирания надписи OVF если используется индикация в одном месте с PRE
	};

#elif WITHONEATTONEAMP
	/* строки, выводимые на индикатор для обозначения режимов.
	 */
	static const struct {
		uint8_t codeatt;	// признак включения аттенюатора
		uint8_t codepre;	// признак включения предусилителя
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  attmodes [] =
	{
		{ 0, 0, "", 0  },
		{ 0, 1, "PRE", 0  },
		{ 0, 0, "", 0  },
		{ 1, 0, "ATT", 0  },
	};

	/* строки, выводимые на индикатор для обозначения режимов.
	 */
	static const struct {
		uint8_t code;
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  pampmodes [] =
	{
		{ 0, "", 0 },	// три символа нужны для стирания надписи OVF если используется индикация в одном месте с PRE
	};

#elif WITHPREAMPATT2_6DB
	/* Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB */

	/* строки, выводимые на индикатор для обозначения режимов.
	 */

	static const struct {
		uint8_t code;
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  attmodes [] =
	{
		{ 0, "", 0 },
		{ 1, "6dB", 60 },
		{ 2, "12dB", 120 },
		{ 3, "18dB", 180 },
	};

	/* строки, выводимые на индикатор для обозначения режимов.
	 */
	static const struct {
		uint8_t code;
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  pampmodes [] =
	{
		{ 0, "", 0 },
		{ 1, "PRE", -120 },
	};

#elif WITHPREAMPATT2_10DB
	/* Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 10 - 20 - 30 dB */

	/* строки, выводимые на индикатор для обозначения режимов.
	 */

	static const struct {
		uint8_t code;
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  attmodes [] =
	{
		{ 0, "", 0  },
		{ 1, "10dB", 100  },
		{ 2, "20dB", 200  },
		{ 3, "30dB", 300  },
	};

	/* строки, выводимые на индикатор для обозначения режимов.
	 */
	static const struct {
		uint8_t code;
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  pampmodes [] =
	{
		{ 0, "", 0 },
		{ 1, "PRE", -120 },
	};

#elif WITHATT2_6DB
	/* Управление двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB без УВЧ */
	/* строки, выводимые на индикатор для обозначения режимов.
	 */

	static const struct {
		uint8_t code;
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  attmodes [] =
	{
		{ 0, "", 0 },
		{ 1, "6dB", 60 },
		{ 2, "12dB", 120 },
		{ 3, "18dB", 180 },
	};

	/* строки, выводимые на индикатор для обозначения режимов.
	 */
	static const struct {
		uint8_t code;
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  pampmodes [] =
	{
		{ 0, "", 0 },	// три символа нужны для стирания надписи OVF если используется индикация в одном месте с PRE
	};
#elif WITHATT2_10DB
	/* Управление двухкаскадным аттенюатором с затуханиями 0 - 10 - 20 - 30 dB без УВЧ */

	/* строки, выводимые на индикатор для обозначения режимов.
	 */

	static const struct {
		uint8_t code;
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  attmodes [] =
	{
		{ 0, "", 0 },
		{ 1, "10dB", 100  },
		{ 2, "20dB", 200  },
		{ 3, "30dB", 300  },
	};

	/* строки, выводимые на индикатор для обозначения режимов.
	 */
	static const struct {
		uint8_t code;
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  pampmodes [] =
	{
		{ 0, "", 0 },	// три символа нужны для стирания надписи OVF если используется индикация в одном месте с PRE
	};
#elif WITHATT1PRE1

	static const struct {
		uint8_t code;
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  attmodes [] =
	{
		{ 0, "", 0 },
		{ 1, "ATT", 120 },
	};


	/* строки, выводимые на индикатор для обозначения режимов.
	 */
	static const struct {
		uint8_t code;
		const char * label;
		int16_t atten10;	// результирующее затухание
	}  pampmodes [] =
	{
		{ 0, "", 0 },
		{ 1, "PRE", - 120 },
	};
#else
	#error ATT/PREAMP mode undefined
#endif

// парамер меню
static uint_fast8_t gattpresh;	/* корректировка показаний с-метра по включенному аттенюатору и предусилителю */

// вернуть положительное значение в случае необходимости коррекции С-метра на величину аттенюатора
// и отрицательное значение в случае коррекции на величину усиления.
// Возвращаем с точностью 0.1 дБ

static int_fast16_t gerflossdb10(uint_fast8_t xvrtr, uint_fast8_t att, uint_fast8_t pre)
{
	if (gattpresh && ! xvrtr)
	{
		// если не трансвертор и не отключено - корректируем S-meter
		return pampmodes [pre].atten10 + attmodes [att].atten10;
	}
	else
	{
		// без коррекции
		return 0;
	}
}

/* строки, выводимые на индикатор для обозначения режимов.
 */

#if WITHAGCMODEONOFF

	/* перечисление всех возможных режимов АРУ
	 */
	enum {
		AGCMODE_MED = 0,
		AGCMODE_FAST = 0,
		//
		AGCMODE_OFF
		//
		};
	static const struct {
		uint_fast8_t code;
		char label4 [5];
		char label3 [4];
	}  agcmodes [] =
	{
		{ BOARD_AGCCODE_ON,  "AGC", "AGC" },
		{ BOARD_AGCCODE_OFF, "OFF", "OFF" },
	};

#else
	#define WITHAGCMODENONE 1
	/* перечисление всех возможных режимов АРУ
	 */
	enum {
		AGCMODE_SLOW = 0,
		AGCMODE_MED = 0,
		AGCMODE_FAST = 0
		};
//	static const struct {
//		uint_fast8_t code;
//		char label4 [5];
//		char label3 [4];
//	}  agcmodes [] =
//	{
//		{ BOARD_AGCCODE_ON, "", "" },
//	};
#endif

#if WITHANTSELECT1RX
	enum { NANTENNAS = 1 };		// одна антенна с возможным подключением приемной
#elif WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT2
	enum { NANTENNAS = 2 };		// выбираем одну из двух антенн
#else /* WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2 */
	enum { NANTENNAS = 1 };		// eдинственная антенна
#endif /* WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2 */

#if WITHANTSELECT1RX

static const struct {
	uint8_t code;
	const char * label;
}  antmodes [] =
{
	{	0,	"A1" },
};

static const struct {
	uint8_t code;
	const char * label;
}  rxantmodes [] =
{
	{	0,	"" },
	{	1,	"RA" },
};

#elif WITHANTSELECTRX

static const struct {
	uint8_t code;
	const char * label;
}  antmodes [] =
{
	{	0,	"A1" },
	{	1,	"A2" },
};

static const struct {
	uint8_t code;
	const char * label;
}  rxantmodes [] =
{
	{	0,	"" },
	{	1,	"RA" },
};

#elif WITHANTSELECT

static const struct {
	uint8_t code [2];	// RX/TX antenna
	char label5 [6];
}  antmodes [] =
{
#if WITHTX && 0
	{	{ 0, 0, },	"1R-1T" },
	{	{ 1, 1, },	"2R-2T" },
	{	{ 0, 1, },	"1R-2T" },
	{	{ 1, 0, },	"2R-1T" },
#else /* WITHTX */
	{	{ 0, 0, },	"ANT1" },
	{	{ 1, 1, },	"ANT2" },
#endif /* WITHTX */
};

#elif WITHANTSELECT2

static const struct {
	uint8_t code;
	const char * label;
}  antmodes [] =
{
	{	0,	"A1" },
	{	1,	"A2" },
};
#endif /* WITHANTSELECT || WITHANTSELECTRX */

#if WITHPOWERLPHP

static const struct {
	uint8_t code;
	const char * label;
}  pwrmodes [] =
{
	{ BOARDPOWERMIN, "LP" },
	{ BOARDPOWERMAX, "HP" },
};

// HP/LP
static size_t getvaltextpwrmode(char * buff, size_t count, int_fast32_t value)
{
	return local_snprintf_P(buff, count, "%s", pwrmodes [value]);
}

#endif /* WITHPOWERLPHP */

#if WITHNOTCHONOFF || WITHNOTCHFREQ

static const struct {
	uint8_t code;
	const char * label;
}  notchmodes [] =
{
//	{ BOARD_NOTCH_OFF, 		"" },
#if WITHLMSAUTONOTCH
	{ BOARD_NOTCH_AUTO, 	"ANTCH" },
#endif /* WITHLMSAUTONOTCH */
	{ BOARD_NOTCH_MANUAL, 	"NOTCH" },
};

static size_t getvaltextnotchmode(char * buff, size_t count, int_fast32_t value)
{
	switch (notchmodes [value].code)
	{
	default:
	case BOARD_NOTCH_OFF:
		return local_snprintf_P(buff, count, "%s", "OFF");
	case BOARD_NOTCH_MANUAL:
		return local_snprintf_P(buff, count, "%s", "FREQ");
	case BOARD_NOTCH_AUTO:
		return local_snprintf_P(buff, count, "%s", "AUTO");
	}
}

#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */

#if WITHUSEDUALWATCH

static const struct {
	uint8_t code;
	const char * label;
}  mainsubrxmodes [] =
{
	{ BOARD_RXMAINSUB_A_B, "A/B", },	// Левый/правый, A - main RX, B - sub RX
	{ BOARD_RXMAINSUB_B_A, "B/A", },
	{ BOARD_RXMAINSUB_B_B, "B/B", },
	{ BOARD_RXMAINSUB_A_A, "A/A", },
	{ BOARD_RXMAINSUB_TWO, "A&B", },	// в оба аудиоканала поступает сумма выходов приемников.
};

static uint_fast8_t mainsubrxmode;		// Левый/правый, A - main RX, B - sub RX
#endif /* WITHUSEDUALWATCH */

#if 1//WITHTX && WITHIF4DSP

static const struct {
	uint8_t code;
	const char * label;
}  txaudiosrcs [] =
{
	{ BOARD_TXAUDIO_MIKE, 	"MIKE", },
#if WITHAFCODEC1HAVELINEINLEVEL	/* кодек имеет управление усилением с линейного входа */
	{ BOARD_TXAUDIO_LINE, 	"LINE", },
#endif /* WITHAFCODEC1HAVELINEINLEVEL */
#if WITHUSBHW && WITHUSBUACOUT
	{ BOARD_TXAUDIO_USB, 	"USB", },
#endif /* WITHUSBHW && WITHUSBUACOUT */
#if WITHUSBHW && WITHUSEUSBBT
	{ BOARD_TXAUDIO_BT, 	"BT", },
#endif /* WITHUSBHW && WITHUSEUSBBT */
	{ BOARD_TXAUDIO_2TONE, 	"2TONE", },
	{ BOARD_TXAUDIO_NOISE, 	"NOISE", },
	{ BOARD_TXAUDIO_1TONE, 	"1TONE", },
	{ BOARD_TXAUDIO_MUTE, 	"MUTE", },
};

#define TXAUDIOSRC_COUNT (ARRAY_SIZE(txaudiosrcs))

static uint_fast8_t findtxaudioindex(uint_fast8_t code)
{
	uint_fast8_t i;

	for (i = 0; i < TXAUDIOSRC_COUNT; ++ i)
	{
		if (txaudiosrcs [i].code == code)
			return i;
	}
	return 0;
}

static size_t getvaltexttxaudio(char * buff, size_t count, int_fast32_t value)
{
	ASSERT(value >= 0 && value < (int) TXAUDIOSRC_COUNT);
	return local_snprintf_P(buff, count, "%s", txaudiosrcs [value].label);
}

#endif /* WITHTX && WITHIF4DSP */

#define PWRMODE_COUNT (sizeof pwrmodes / sizeof pwrmodes [0])
#define NOTCHMODE_COUNT (sizeof notchmodes / sizeof notchmodes [0])
#define PAMPMODE_COUNT (sizeof pampmodes / sizeof pampmodes [0])
#define ATTMODE_COUNT (sizeof attmodes / sizeof attmodes [0])
#define AGCMODE_COUNT (sizeof agcmodes / sizeof agcmodes [0])
#define MAINSUBRXMODE_COUNT (sizeof mainsubrxmodes / sizeof mainsubrxmodes [0])

#define RXANTMODE_COUNT (sizeof rxantmodes / sizeof rxantmodes [0])
#if WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2
	#define ANTMODE_COUNT (sizeof antmodes / sizeof antmodes [0])
#else /* WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2 */
	#define ANTMODE_COUNT 1
#endif /* WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2 */

// проверка и приведение в допустимый диапазон значений, считанных из eeprom или принятых по CAT. Или при autosplit
static uint_fast32_t
//NOINLINEAT
vfy32up(
	uint_fast32_t v,
	uint_fast32_t bottom, uint_fast32_t upper, uint_fast32_t def)
{

	if (def > upper || def < bottom)
		def = bottom;
	if (v > upper || v < bottom)
		v = def;
	return v;
}

static uint_fast32_t
//NOINLINEAT
loadvfy32(
	nvramaddress_t place,
	uint_fast32_t bottom, uint_fast32_t top, uint_fast32_t def)
{
#if HARDWARE_IGNORENONVRAM
	return def;
#endif /* HARDWARE_IGNORENONVRAM */

	if (place == MENUNONVRAM)
		return def;

	const uint_fast32_t v = restore_i32(place);

	if (def >= top || def < bottom)
		def = bottom;

	if (v >= top || v < bottom)
	{
		save_i32(place, def);
		return def;
	}
	return v;
}

#if WITHIF4DSP

// профиль ацдиопроцессора

typedef struct mikproc_tag
{
	uint_fast8_t comp;
	uint_fast8_t complevel;
} mikproc_t;

/* Хранимые в NVRAM параметры обработки сигнала перед модулятором */
struct micproc
{
	uint8_t comp;
	uint8_t complevel;
} ATTRPACKED;	// аттрибут GCC, исключает "дыры" в структуре. Так как в ОЗУ нет копии этой структуры, see also NVRAM_TYPE_BKPSRAM

struct micprof_cell
{
	uint_fast8_t level;
	uint_fast8_t clip;
	uint_fast8_t agc;
	uint_fast8_t agcgain;
#if WITHAFCODEC1HAVEPROC
	uint_fast8_t mikeboost20db;
	uint_fast8_t eq_enable;
	uint8_t eq_params [HARDWARE_CODEC1_NPROCPARAMS];
#endif /* WITHAFCODEC1HAVEPROC */
	uint_fast8_t cell_saved;
};

typedef struct micprof_cell	micprof_t;

#if WITHTOUCHGUI
static micprof_t micprof_cells [NMICPROFCELLS];
#endif /* WITHTOUCHGUI */

static mikproc_t micprofiles [] =
{
	{
		0,
		0,
	},
	{
		0,
		0,
	},
};

#define NMICPROFILES (sizeof micprofiles / sizeof micprofiles [0])

// индекс номера банка параметров обработки звука для данного режима
enum
{
	TXAPROFIG_CW,
	TXAPROFIG_SSB,
	TXAPROFIG_DIGI,
	TXAPROFIG_AM,
	TXAPROFIG_NFM,
	TXAPROFIG_DRM,
	TXAPROFIG_WFM,
	//
	TXAPROFIG_count
};


static uint8_t gtxaprofiles [TXAPROFIG_count];	// индекс профиля для группы режимов передачи - значения 0..NMICPROFILES-1


// параметры фильтра на приеме

#define AFRESPONCESHIFT 192
#define AFRESPONCEMIN 0
#define AFRESPONCEMAX (AFRESPONCESHIFT + 18)	// -96..+18

static int_fast32_t getafresponcebase(void)
{
	return - AFRESPONCESHIFT;
}

enum
{
	BWSET_SINGLE,	// параметры полосы пропускания - это одиночные значения полосы пропускания
	BWSET_PAIR		// параметры полосы пропускания - пара нижний срез/верхний срез
};


enum
{
	BWSET_WIDTHS = 3	// максимальное количество полос пропускания в одном bwsetsc_t
};

// индекс банка полос пропускания и коррекции АЧХ для данного режима
enum
{
	BWSETI_CW,
	BWSETI_SSB,
	BWSETI_SSBTX,
	BWSETI_DIGI,
	BWSETI_AM,
	BWSETI_NFM,
	BWSETI_DRM,
	BWSETI_WFM,
	//
	BWSETI_count
};

static const char
	strFlashWFM [] = "WFM",
	strFlashWide [] = "WID",
	strFlashMedium [] = "MED",
	strFlashNarrow [] = "NAR",
	strFlashNormal [] = "NOR";


// Частоты перестройки границ полосы пропускания
typedef struct
{
	uint16_t granulationleft;	// дискретность изменения параметра через CAT или меню - used if WITHAMHIGHKBDADJ enabled
	uint16_t granulationright;	// дискретность изменения параметра через CAT или меню - used if WITHAMHIGHKBDADJ enabled
	uint8_t left10_width10_low, left10_width10_high;	// пределы изменения параметров
	uint8_t right100_low, right100_high;	// пределы изменения параметров
} bwlimits_t;

typedef struct
{
	/* константные параметры */
	const bwlimits_t * limits;
	uint8_t bwpropi;	// BWPROPI_xxxx - used if WITHAMHIGHKBDADJ enabled
	uint8_t type;		// BWSET_SINGLE/BWSET_PAIR
	/* параметры, изменяемые через меню */
	uint_fast8_t left10_width10, right100;	/* left выполняет роль width для телеграфных (BWSET_SINGLE) фильтров */
	uint_fast8_t afresponce;	/* наклон АЧХ - на Samplerate/2 АЧХ становится на столько децибел  */
	uint_fast8_t fltsofter;	/* Код управления сглаживанием скатов фильтра основной селекции на приёме */
	uint8_t rxbw100;	/* полоса пропускания радиотракта (или 0 если вычисляется) в сотнях герц */
	uint8_t txbw100;	/* полоса пропускания радиотракта (или 0 если вычисляется), удвоенная девиация для NFM в сотнях герц*/
} bwprop_t;

// Частоты границ полосы пропускания
typedef struct
{
	uint8_t last;		// 0..BWSET_WIDTHS-1
	bwprop_t  * prop [BWSET_WIDTHS];				// частоты и другие свойства фильтров
	const char * labels [BWSET_WIDTHS];	// названия фильтров
} bwsetsc_t;

// Пределы изменения полосы пропускания для меню

#define BWGRANLOW	10		// низкочастотный скат фильтров подстраивается и меню с шагом 10 герц
#define BWGRANHIGH	100		// высокочастотный скат фильтров подстраивается с шагом 100 герц

#define BWCWLEFTMIN (100 / BWGRANLOW)	// 100 Hz..1800 Hz
#define BWCWLEFTMAX (1800 / BWGRANLOW)

#define BWLEFTMIN (50 / BWGRANLOW)	// 50 Hz..700 Hz
#define BWLEFTMAX (700 / BWGRANLOW)

#define BWRIGHTMIN (800 / BWGRANHIGH)	// 0.8 kHz-18 kHz
#define BWRIGHTMAX (18000 / BWGRANHIGH)

// полосы пропускания радиотракта для NFM
#define NBFMWIDTHRX 10000	// полоса тракта для bwprop_nfmnarrow
#define WBFMWIDTHRX 20000	// полоса тракта для bwprop_nfmwide
#define NBFMWIDTHTX 5000	// +/- 2.5 кГц удвоенная девиация для bwprop_nfmnarrow
#define WBFMWIDTHTX 10000	// +/- 5 кГц удвоенная девиация для bwprop_nfmwide

// bwlimits_nfm - left10_width10_low фиксированно 300 герц для работы CTCSS
static const bwlimits_t bwlimits_cw = { 100 / BWGRANLOW, 100 / BWGRANHIGH, BWCWLEFTMIN, BWCWLEFTMAX, 0, 0,  };
static const bwlimits_t bwlimits_am = { 50 / BWGRANLOW, 100 / BWGRANHIGH, BWLEFTMIN, BWLEFTMAX, BWRIGHTMIN, BWRIGHTMAX,  };
static const bwlimits_t bwlimits_ssb = { 50 / BWGRANLOW, 100 / BWGRANHIGH, BWLEFTMIN, BWLEFTMAX, BWRIGHTMIN, BWRIGHTMAX, };
static const bwlimits_t bwlimits_nfm = { 300 / BWGRANLOW, 100 / BWGRANHIGH, 300 / BWGRANLOW, 300 / BWGRANLOW, BWRIGHTMIN, BWRIGHTMAX, };
static const bwlimits_t bwlimits_wfm = { 50 / BWGRANLOW, 100 / BWGRANHIGH, BWLEFTMIN, BWLEFTMAX, BWRIGHTMIN, BWRIGHTMAX, };

// индекс банка полос пропускания для данного режима
enum
{
	BWPROPI_CWNARROW,
	BWPROPI_CWWIDE,
	BWPROPI_SSBWIDE,
	BWPROPI_SSBMEDIUM,
	BWPROPI_SSBNARROW,
	BWPROPI_SSBTX,
	BWPROPI_AMWIDE,
	BWPROPI_AMNARROW,
	BWPROPI_DIGIWIDE,
	BWPROPI_NFMNARROW,
	BWPROPI_NFMWIDE,
	BWPROPI_WFM,
	//
	BWPROPI_count
};

#define AFRESPONCEDEFAULT (- 72)	// наклон АЧХ для голосовых режимов
#define AFRESPONCEWFM (+ 18)	// наклон АЧХ для WFM
#define AFRESPONCEFLAT (0)

// Частоты границ полосы пропускания
// эти значения могут модифицироваться через меню
static bwprop_t bwprop_cwnarrow = { & bwlimits_cw, BWPROPI_CWNARROW, BWSET_SINGLE, 200 / BWGRANLOW, 0, AFRESPONCEFLAT + AFRESPONCESHIFT, WITHFILTSOFTMIN, 0, 0, };
static bwprop_t bwprop_cwwide = { & bwlimits_cw, BWPROPI_CWWIDE, BWSET_SINGLE, 500 / BWGRANLOW, 0, AFRESPONCEFLAT + AFRESPONCESHIFT, WITHFILTSOFTMIN, 0, 0, };
static bwprop_t bwprop_ssbwide = { & bwlimits_ssb, BWPROPI_SSBWIDE, BWSET_PAIR, 300 / BWGRANLOW, 3400 / BWGRANHIGH, AFRESPONCEDEFAULT + AFRESPONCESHIFT, WITHFILTSOFTMIN, 0, 0, };
static bwprop_t bwprop_ssbmedium = { & bwlimits_ssb, BWPROPI_SSBMEDIUM, BWSET_PAIR, 300 / BWGRANLOW, 2700 / BWGRANHIGH, AFRESPONCEDEFAULT + AFRESPONCESHIFT, WITHFILTSOFTMIN, 0, 0, };
static bwprop_t bwprop_ssbnarrow = { & bwlimits_ssb, BWPROPI_SSBNARROW, BWSET_PAIR, 300 / BWGRANLOW, 2200 / BWGRANHIGH, AFRESPONCEDEFAULT + AFRESPONCESHIFT, WITHFILTSOFTMIN, 0, 0, };
static bwprop_t bwprop_ssbtx = { & bwlimits_ssb, BWPROPI_SSBTX, BWSET_PAIR, 300 / BWGRANLOW, 3400 / BWGRANHIGH, AFRESPONCEFLAT + AFRESPONCESHIFT, WITHFILTSOFTMIN, 0, 0, };
static bwprop_t bwprop_amwide = { & bwlimits_am, BWPROPI_AMWIDE, BWSET_PAIR, 100 / BWGRANLOW, 9000 / BWGRANHIGH, AFRESPONCEDEFAULT + AFRESPONCESHIFT, WITHFILTSOFTMIN, 0, 0, };
static bwprop_t bwprop_amnarrow = { & bwlimits_am, BWPROPI_AMNARROW, BWSET_PAIR, 100 / BWGRANLOW, 4500 / BWGRANHIGH, AFRESPONCEDEFAULT + AFRESPONCESHIFT, WITHFILTSOFTMIN, 0, 0, };
static bwprop_t bwprop_digiwide = { & bwlimits_ssb, BWPROPI_DIGIWIDE, BWSET_PAIR, 50 / BWGRANLOW, 5500 / BWGRANHIGH, AFRESPONCEFLAT + AFRESPONCESHIFT, WITHFILTSOFTMIN, 0, 0, };
static bwprop_t bwprop_nfmnarrow = { & bwlimits_nfm, BWPROPI_NFMNARROW, BWSET_PAIR, 300 / BWGRANLOW, 3400 / BWGRANHIGH, AFRESPONCEFLAT + AFRESPONCESHIFT, WITHFILTSOFTMIN, NBFMWIDTHRX / BWGRANHIGH, NBFMWIDTHTX / BWGRANHIGH, };
static bwprop_t bwprop_nfmwide = { & bwlimits_nfm, BWPROPI_NFMWIDE, BWSET_PAIR, 300 / BWGRANLOW, 4000 / BWGRANHIGH, AFRESPONCEFLAT + AFRESPONCESHIFT, WITHFILTSOFTMIN, WBFMWIDTHRX / BWGRANHIGH, WBFMWIDTHTX / BWGRANHIGH, };
static bwprop_t bwprop_wfm = { & bwlimits_wfm, BWPROPI_WFM, BWSET_PAIR, 100 / BWGRANLOW, 12000 / BWGRANHIGH, AFRESPONCEWFM + AFRESPONCESHIFT, WITHFILTSOFTMIN, 0, 0, };

// Способ представления частот и количество профилей полосы пропускания,
// а так же названия полос пропускания для отображения
static const bwsetsc_t bwsetsc [BWSETI_count] =
{
	{ 2, { & bwprop_cwwide, & bwprop_cwnarrow, & bwprop_ssbwide, }, { strFlashWide, strFlashNarrow, strFlashNormal, }, },	// BWSETI_CW
	{ 2, { & bwprop_ssbwide, & bwprop_ssbmedium, & bwprop_ssbnarrow, }, { strFlashWide, strFlashMedium, strFlashNarrow, }, },	// BWSETI_SSB
	{ 0, { & bwprop_ssbtx, }, { strFlashNormal, }, },	// BWSETI_SSBTX
	{ 0, { & bwprop_digiwide, }, { strFlashNormal, }, },	// BWSETI_DIGI
	{ 1, { & bwprop_amwide, & bwprop_amnarrow, }, { strFlashWide, strFlashNarrow, }, },	// BWSETI_AM
	{ 1, { & bwprop_nfmnarrow, & bwprop_nfmwide, }, { strFlashWide, strFlashNarrow, }, },	// BWSETI_NFM
	{ 0, { & bwprop_amwide, }, { strFlashNormal, }, },	// BWSETI_DRM
	{ 0, { & bwprop_wfm, }, { strFlashWFM, }, },	// BWSETI_WFM
};

// выбранная полоса пропускания в каждом режиме
static uint8_t bwsetpos [BWSETI_count];

// Используется для обмена с NVRAN параметрами фильтров
static bwprop_t * const bwprops [BWPROPI_count] =
{
	& bwprop_cwnarrow,	// BWPROPI_CWNARROW,
	& bwprop_cwwide,	// BWPROPI_CWWIDE,
	& bwprop_ssbwide,	// BWPROPI_SSBWIDE,
	& bwprop_ssbmedium,	// BWPROPI_SSBMEDIUM,
	& bwprop_ssbnarrow,	// BWPROPI_SSBNARROW
	& bwprop_ssbtx,		// BWPROPI_SSBTX
	& bwprop_amwide,	// BWPROPI_AMWIDE,
	& bwprop_amnarrow,	// BWPROPI_AMNARROW,
	& bwprop_digiwide,	// BWPROPI_DIGIWIDE,
	& bwprop_nfmnarrow,	// BWPROPI_NFMNARROW,
	& bwprop_nfmwide,	// BWPROPI_NFMWIDE,
	& bwprop_wfm,		// BWPROPI_WFM,
};

// получить тип фильтра
static int_fast16_t
bwseti_getwide(
	uint_fast8_t bwseti
	)
{
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	switch (p->type)
	{
	case BWSET_SINGLE:
		return 0;

	default:
		return 1;
	}
}

// получить наклон АЧХ фильтра
static int_fast8_t
bwseti_getafresponce(
	uint_fast8_t bwseti
	)
{
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	switch (p->type)
	{
	case BWSET_SINGLE:
		return 0;

	default:
		return p->afresponce - AFRESPONCESHIFT;
	}
}

// получить полосу пропускания фильтра
static int_fast16_t
bwseti_getwidth(
	uint_fast8_t bwseti
	)
{
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	switch (p->type)
	{
	case BWSET_SINGLE:
		{
			const int_fast16_t width = p->left10_width10 * BWGRANLOW;
			const int_fast16_t width_2 = width / 2;
			const int_fast16_t center = gcwpitch10 * CWPITCHSCALE;
			return (center > width_2) ? width : (center * 2);
		}

	default:
	case BWSET_PAIR:
		return p->right100 * BWGRANHIGH - p->left10_width10 * BWGRANLOW;
	}
}

// получить полосу пропускания NFM редима
static int_fast16_t
bwseti_getnfmbw(
	uint_fast8_t bwseti,
	uint_fast8_t tx
	)
{
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	return (tx ? p->txbw100 : p->rxbw100) * BWGRANHIGH;
}

// получить нижнюю частоту среза фильтра
static int_fast16_t
bwseti_getlow(
	uint_fast8_t bwseti
	)
{
	// gssbtxlowcut10 * BWGRANLOW
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	switch (p->type)
	{
	case BWSET_PAIR:
		return p->left10_width10 * BWGRANLOW;

	default:
	case BWSET_SINGLE:
		{
			const int_fast16_t width = p->left10_width10 * BWGRANLOW;
			const int_fast16_t width2 = width / 2;
			const int_fast16_t center = gcwpitch10 * CWPITCHSCALE;
			return (center > width2) ? (center - width2) : 0;
		}
	}
}

// получить верхнюю частоту среза фильтра
static int_fast16_t
bwseti_gethigh(
	uint_fast8_t bwseti
	)
{
	// gssbtxhighcut100 * BWGRANHIGH
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	switch (p->type)
	{
	case BWSET_PAIR:
		return p->right100 * BWGRANHIGH;

	default:
	case BWSET_SINGLE:
		{
			const int_fast16_t width = p->left10_width10 * BWGRANLOW;
			const int_fast16_t width2 = width / 2;
			const int_fast16_t center = gcwpitch10 * CWPITCHSCALE;
			return (center > width2) ? (center + width2) : (center * 2);
		}
	}
}

// получить код управления сглаживанием скатов фильтра
static int_fast8_t
bwseti_getfltsofter(
	uint_fast8_t bwseti
	)
{
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	return p->fltsofter;
}

// индекс набора параметров слухового приема
enum
{
	AGCSETI_SSB,
	AGCSETI_CW,
	AGCSETI_FLAT,	// NFM
	AGCSETI_AM,		// AM, SAM
	AGCSETI_DRM,
	AGCSETI_DIGI,
	//
	AGCSETI_COUNT
};

/* структура хранения параметров в NVRAM */
struct agcseti_tag
{
	/* параметры АРУ по режимам работы */
	uint8_t rate;
	uint8_t t0;
	uint8_t t1;
	uint8_t release10;
	uint8_t t4;
	uint8_t thung10;
} ATTRPACKED;	// аттрибут GCC, исключает "дыры" в структуре. Так как в ОЗУ нет копии этой структуры, see also NVRAM_TYPE_BKPSRAM

struct afsetitempl
{
	/* начальные значения параметров АРУ */
	uint8_t rate; 		// = 10;	// на agc_rate дБ изменения входного сигнала 1 дБ выходного
	uint8_t t0; 		// = 0;	// in 1 mS steps. 0=0 mS	charge fast
	uint8_t t1; 		// = 120;	// in 1 mS steps. 120=120 mS	charge slow
	uint8_t release10; 	// = 5;		// in 0.1 S steps. 0.5 S discharge slow
	uint8_t t4; 		// = 50;	// in 1 mS steps. 35=35 mS discharge fast
	uint8_t thung10; 	// = 3;	// 0.1 S hung time (0.3 S recomennded).
	uint8_t scale; 		// = 100;	// 100% - требуемый выход от АРУ
};

#define AGC_RATE_FLAT	192	//(UINT8_MAX - 1)
#define AGC_RATE_SSB	10
#define AGC_RATE_DIGI	3
#define AGC_RATE_DRM	3

static const struct afsetitempl aft [AGCSETI_COUNT] =
{
	//AGCSETI_SSB,
	{
		AGC_RATE_SSB,		// agc_rate
		0,		// agc_t0
		120,	// agc_t1
		5,		// agc_release10
		50,		// agc_t4
		3,		// agc_thung10
		100,	// agc_scale
	},
	//AGCSETI_CW,
	{
		AGC_RATE_SSB,		// agc_rate
		0,		// agc_t0
		120,	// agc_t1
		1,		// agc_release10
		50,		// agc_t4
		1,		// agc_thung10
		100,	// agc_scale
	},
	//AGCSETI_FLAT,
	{
		AGC_RATE_FLAT,		// agc_rate
		0,		// agc_t0
		120,	// agc_t1
		1,		// agc_release10
		50,		// agc_t4
		1,		// agc_thung10
		100,	// agc_scale
	},
	//AGCSETI_AM,
	{
#if 0
		AGC_RATE_FLAT,		// agc_rate
		100,	// agc_t0
		100,	// agc_t1
		1,		// agc_release10
		100,	// agc_t4
		0,		// agc_thung10
		25,		// agc_scale
#else
		AGC_RATE_FLAT,		// agc_rate
		0,		// agc_t0
		120,	// agc_t1
		1,		// agc_release10
		50,		// agc_t4
		1,		// agc_thung10
		100,	// agc_scale
#endif
	},
	//AGCSETI_DRM,
	{
		AGC_RATE_DRM,		// agc_rate
		0,		// agc_t0
		120,	// agc_t1
		1,		// agc_release10
		50,		// agc_t4
		1,		// agc_thung10
		100,	// agc_scale
	},
	//AGCSETI_DIGI,
	{
		AGC_RATE_DIGI,		// agc_rate
		0,		// agc_t0
		120,	// agc_t1
		1,		// agc_release10
		50,		// agc_t4
		1,		// agc_thung10
		100,	// agc_scale
	},
};


typedef struct agcp_tag
{
	uint_fast8_t rate; 		// = 10;	// на gagc_rate дБ изменения входного сигнала 1 дБ выходного
	uint_fast8_t t0; 		// = 0;		// in 1 mS steps. 0=0 mS	charge fast
	uint_fast8_t t1; 		// = 120;	// in 1 mS steps. 120=120 mS	charge slow
	uint_fast8_t release10; // = 5;		// in 0.1 S steps. 0.5 S discharge slow - время разряда медленной цепи АРУ
	uint_fast8_t t4; 		// = 50;	// in 1 mS steps. 35=35 mS discharge fast - время разряда быстрой цепи АРУ
	uint_fast8_t thung10; 	// = 3;	// 0.1 S hung time (0.3 S recomennded).
	uint_fast8_t scale; 	// = 100
} agcp_t;

static agcp_t gagc [AGCSETI_COUNT];

#endif /* WITHIF4DSP */

const struct paramdefdef * const * getmiddlemenu_cw(unsigned * size);
const struct paramdefdef * const * getmiddlemenu_ssb(unsigned * size);
const struct paramdefdef * const * getmiddlemenu_am(unsigned * size);
const struct paramdefdef * const * getmiddlemenu_nfm(unsigned * size);
const struct paramdefdef * const * getmiddlemenu_digi(unsigned * size);
const struct paramdefdef * const * getmiddlemenu_wfm(unsigned * size);


#define	DEFAULT_DRM_PITCH	12000	/* тон DRM - 12 кГц*/
// The standard mark and space tones are 2125 hz and 2295 hz respectively
#define	DEFAULT_RTTY_PITCH	1275	/* mark тон DIGI modes - 2.125 кГц (1275 2125) */

#if WITHIF4DSP
	#if (LO4_SIDE == LOCODE_LOWER) || (LO4_SIDE == LOCODE_UPPER) || (LO4_SIDE == LOCODE_INVALID)
		#define DEFAULT_DSP_LO5SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
	#else
		#define DEFAULT_DSP_LO5SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#endif
	// "Дятел"
	// Комментарий к "Пиону": Чтоб принимать usb надо чтобы приемник был настроен на 4012 Гц ниже, чтоб нбп - ниже на 7012Гц

	/* XX_PROCESSING_PITCH_XX: В случае наличия DSP процессора за смесительным детектором - на какую частоту переносим (в герцах). */
	/* Режимы, использующие смесительный детектор для слухового приёма, так же относятся к требующим включённого последнего гетеродина. */
	#define	SSB_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	SSB_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	DIGI_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	DIGI_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	BPSK_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	BPSK_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	CW_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	CW_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	CWZ_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	CWZ_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	AM_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	AM_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	SAM_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	SAM_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	NFM_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	NFM_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	DRM_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	DRM_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	WFM_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	WFM_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

#else /* WITHIF4DSP */
	/* XX_PROCESSING_PITCH_XX: В случае наличия DSP процессора за смесительным детектором - на какую частоту переносим (в герцах). */
	/* Режимы, использующие смесительный детектор для слухового приёма, так же относятся к требующим включённого последнего гетеродина. */
	#define	CW_LO5_SIDE_RX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define	CW_LO5_SIDE_TX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */

	#define	CWZ_LO5_SIDE_RX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define	CWZ_LO5_SIDE_TX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */

	#if ATMEGA_CTLSTYLE_V9SF_RX3QSP_H_INCLUDED
		#define	AM_PROCESSING_PITCH_RX		(455000L)	/* переносим на 455 кГц */
		#define	AM_PROCESSING_PITCH_TX		0
		#define	AM_LO5_SIDE_RX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#define	AM_LO5_SIDE_TX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#else
		#define	AM_PROCESSING_PITCH_RX		0
		#define	AM_PROCESSING_PITCH_TX		0
		#define	AM_LO5_SIDE_RX		LOCODE_INVALID	/* Этого гетеродина нет. */
		#define	AM_LO5_SIDE_TX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#endif

	#define	SSB_LO5_SIDE_RX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define	SSB_LO5_SIDE_TX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */

	#define	DIGI_LO5_SIDE_RX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define	DIGI_LO5_SIDE_TX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */

	#define	BPSK_LO5_SIDE_RX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define	BPSK_LO5_SIDE_TX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */

	#define	NFM_LO5_SIDE_RX		LOCODE_INVALID	/* Этого гетеродина нет. */
	#define	NFM_LO5_SIDE_TX		LOCODE_LOWER	/* Не выключать третий гетеродин - при NFM используется тот же тракт. */

	#define	DRM_LO5_SIDE_RX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define	DRM_LO5_SIDE_TX		LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */

	#define	WFM_LO5_SIDE_RX		LOCODE_INVALID	/* Этого гетеродина нет. */
	#define	WFM_LO5_SIDE_TX		LOCODE_INVALID	/* Этого гетеродина нет. */
#endif /* WITHIF4DSP */
/* шаг настройки для разных режимов приема.
 * После деления на 10 значения должны влезть в 8 бит беззнаковое число.
 */

#define STEP_MINIMAL	10		/* минимальный шаг перестройки */
#define STEP_SSB_HIGH	10		/* шаг для SSB (USB/LSB/CW/CWR) при применении валкдера с большим к-вом шагов */
#define STEP_SSB_LOW	50		/* шаг для SSB (USB/LSB/CW/CWR) при применении валкдера с небольшим к-вом шагов */
#define	STEP_AM_HIGH	100		/* шаг для AM */
#define	STEP_AM_LOW		200		/* шаг для AM */
#define STEP_DRM	1000	/* шаг для DRM */
#define STEP_FM		250		/* шаг для FM */
#define	STEP_CWZ	10		/* шаг для CWZ (калибровка гетеродинов)  */
#define	STEP_WFM_LOW		1000		/* шаг для WFM */
#define	STEP_WFM_HIGH		2500		/* шаг для WFM */

//#define	STEP_GENERAL	100000		/* приращение при перестройке в режиме general coverage */
struct modetempl
{
	uint_fast8_t step10 [2];		/* индексинуется gbigstep */
	int_fast16_t autosplitK;		/* смещение в килогерцах для включения режима autosplit */
	uint_fast8_t nar : 1;			/* разрешение включения НЧ фильтра в этом режиме */
	uint_fast8_t txcw : 1;			/* Включение передачи в обход балансного модулятора */
	uint_fast8_t vox : 1;			/* в этом режиме возможно использование VOX */
	uint_fast8_t wbkin : 1;			/* в этом режиме возможно использование BREAK-IN */
	uint_fast8_t abkin : 1;			/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
	uint_fast8_t mute : 1;			/* в этом режиме запрещён микрофонный усилитель */
	uint_fast8_t wifshift : 1;		/* в этом режиме разрешено использовать параметры IF SHIFT */
	uint_fast8_t wpbt : 1;			/* в этом режиме разрешено использовать параметры PBT */
	uint_fast8_t subtone: 1;		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
	uint_fast8_t lo5side [2];		/* в этом режиме не выключать последний гетеродин (RX и TX)  */
#endif /* ! defined (LO5_SIDE) */
	uint_fast8_t defagcmode;		/* начальный режим АРУ */
#if WITHTX
	portholder_t txgfva [TXGFI_SIZE];	// состояния выходов для разных режимов
	uint_fast8_t sdtnva [TXGFI_SIZE];	// признаки включения самоконтроля для разных режимов
#endif /* WITHTX */
#if WITHIF4DSP
	uint_fast8_t dspmode [2];		// Управление для DSP в режиме приёма и передачи
	uint_fast8_t bwsetis [2];			// индекс банка полос пропускания для данного режима в режиме приёма и передачи
	int_fast16_t bw6s [2];			// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
	uint_fast8_t txaudiocode;			// источник звукового сигнала для данного режима (код BOARD_TXAUDIO_xxx)
	uint_fast8_t txaprofgp;		// группа профилей обработки звука
	uint_fast8_t agcseti;			// параметры слухового приема
	FLOAT_t * (* afproc)(uint_fast8_t pathi, struct rxaproc_tag *, FLOAT_t * p);	// функция обработки звука в user mode в режиме приёма и передачи
#else /* WITHIF4DSP */
	uint_fast8_t detector [2];		/* код детектора RX и TX */
#endif /* WITHIF4DSP */
	const struct paramdefdef * const * (* middlemenu)(unsigned * size);
	const char * label;					// для контроля правильности инициализации структуры
};

// modes
// Порядок элементов в этом массиве должен совпадать с элементми enum,
// в котором определён MODE_COUNT
static const struct modetempl mdt [MODE_COUNT] =
{
	/* MODE_CW */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_CW
		1,					/*  смещение в килогерцах для включения режима autosplit */
		1,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		1,		/* TXCW Включение передачи в обход балансного модулятора */
		0,		/* в этом режиме возможно использование VOX */
		1,		/* в этом режиме возможно использование BREAK-IN */
		1, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		1,		/* в этом режиме запрещён микрофонный усилитель */
		0,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		1,		/* в этом режиме разрешено использовать параметры PBT */
		0,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ CW_LO5_SIDE_RX, CW_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_FAST,	/* начальный режим АРУ для MODE_CW */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TRANS, TXGFV_TX_CW, }, // txgfva для CW
		{ 0, 0, 0, 1 },	// признаки включения самоконтроля для CW
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_NARROW, DSPCTL_MODE_TX_CW, },	// Управление для DSP в режиме приёма и передачи - режим узкого фильтра
		{ BWSETI_CW, BWSETI_CW },				// индекс банка полос пропускания для данного режима
		{ 0, INT16_MAX, },	// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
		BOARD_TXAUDIO_MUTE,		// источник звукового сигнала для данного режима
		TXAPROFIG_CW,				// группа профилей обработки звука
		AGCSETI_CW,
		afpcw, // afproc
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, },		/* ssb detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_cw,
		"CW",
	},
	/* MODE_SSB */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_SSB
		5,					/*  смещение в килогерцах для включения режима autosplit */
		0,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		0,		/* TXCW Включение передачи в обход балансного модулятора */
		1,		/* в этом режиме возможно использование VOX */
		0,		/* в этом режиме возможно использование BREAK-IN */
		1, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		0,		/* в этом режиме запрещён микрофонный усилитель */
		1,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		1,		/* в этом режиме разрешено использовать параметры PBT */
		0,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ SSB_LO5_SIDE_RX, SSB_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* начальный режим АРУ для MODE_SSB */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva для SSB
		{ 0, 0, 0, 0 },	// признаки включения самоконтроля для SSB
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_WIDE, DSPCTL_MODE_TX_SSB, },	// Управление для DSP в режиме приёма и передачи - режим широкого фильтра
		{ BWSETI_SSB, BWSETI_SSBTX, },				// индекс банка полос пропускания для данного режима
		{ 0, 0, },	// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
		BOARD_TXAUDIO_MIKE,		// источник звукового сигнала для данного режима
		TXAPROFIG_SSB,				// группа профилей обработки звука
		AGCSETI_SSB,
		afpcw, // afproc
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, },		/* ssb detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_ssb,
		"SSB",
	},
	/* MODE_AM */
	{
		{ STEP_AM_HIGH / 10, STEP_AM_LOW / 10, },	// step for MODE_AM
		5,					/*  смещение в килогерцах для включения режима autosplit */
		0,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		1,		/* TXCW Включение передачи в обход балансного модулятора */
		1,		/* в этом режиме возможно использование VOX */
		0,		/* в этом режиме возможно использование BREAK-IN */
		1, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		0,		/* в этом режиме запрещён микрофонный усилитель */
		0,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		1,		/* в этом режиме разрешено использовать параметры PBT */
		0,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ AM_LO5_SIDE_RX, AM_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* начальный режим АРУ для MODE_AM */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_AM, TXGFV_TX_AM, }, // txgfva для AM
		{ 0, 0, 0, 0 },	// признаки включения самоконтроля для AM
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_AM, DSPCTL_MODE_TX_AM, },	// Управление для DSP в режиме приёма и передачи - режим широкого фильтра
		{ BWSETI_AM, BWSETI_SSBTX, },				// индекс банка полос пропускания для данного режима
		{ 0, INT16_MAX, },	// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
		BOARD_TXAUDIO_MIKE,		// источник звукового сигнала для данного режима
		TXAPROFIG_AM,				// группа профилей обработки звука
		AGCSETI_AM,
		afpcw, // afproc
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_AM, BOARD_DETECTOR_AM, }, 		/* AM detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_am,
		"AM",
	},
#if WITHSAM
	/* MODE_SAM  synchronous AM demodulation */
	{
		{ STEP_AM_HIGH / 10, STEP_AM_LOW / 10, },	// step for MODE_SAM
		5,					/*  смещение в килогерцах для включения режима autosplit */
		0,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		1,		/* TXCW Включение передачи в обход балансного модулятора */
		1,		/* в этом режиме возможно использование VOX */
		0,		/* в этом режиме возможно использование BREAK-IN */
		0, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		0,		/* в этом режиме запрещён микрофонный усилитель */
		0,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		1,		/* в этом режиме разрешено использовать параметры PBT */
		0,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ SAM_LO5_SIDE_RX, SAM_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* начальный режим АРУ для MODE_AM */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_AM, TXGFV_TX_AM, }, // txgfva для AM
		{ 0, 0, 0, 0 },	// признаки включения самоконтроля для AM
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_SAM, DSPCTL_MODE_TX_AM, },	// Управление для DSP в режиме приёма и передачи - режим широкого фильтра
		{ BWSETI_AM, BWSETI_SSBTX, },				// индекс банка полос пропускания для данного режима
		{ 0, INT16_MAX, },	// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
		BOARD_TXAUDIO_MIKE,		// источник звукового сигнала для данного режима
		TXAPROFIG_AM,				// группа профилей обработки звука
		AGCSETI_AM,
		afpcw, // afproc
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_AM, BOARD_DETECTOR_AM, }, 		/* AM detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_am,
		"SAM",
	},
#endif /* WITHSAM */
	/* MODE_NFM */
	{
		{ STEP_FM / 10, STEP_FM / 10, },	// step for MODE_NFM
		- 600,	/*  смещение в килогерцах для включения режима autosplit */
		0,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		1,		/* TXCW Включение передачи в обход балансного модулятора */
		1,		/* в этом режиме возможно использование VOX */
		0,		/* в этом режиме возможно использование BREAK-IN */
		1, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		0,		/* в этом режиме запрещён микрофонный усилитель */
		0,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		1,		/* в этом режиме разрешено использовать параметры PBT */
		1,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ NFM_LO5_SIDE_RX, NFM_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_FAST,	/* начальный режим АРУ для MODE_NFM */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_NFM, TXGFV_TX_NFM, }, // txgfva для NFM
		{ 0, 0, 0, 0 },	// признаки включения самоконтроля для NFM
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_NFM, DSPCTL_MODE_TX_NFM, },	// Управление для DSP в режиме приёма и передачи - режим широкого фильтра
		{ BWSETI_NFM, BWSETI_SSBTX, },				// индекс банка полос пропускания для данного режима
		{ 0, 0, },	// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
		BOARD_TXAUDIO_MIKE,		// источник звукового сигнала для данного режима
		TXAPROFIG_NFM,				// группа профилей обработки звука
		AGCSETI_FLAT,
		afpcw, // afproc
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_FM, BOARD_DETECTOR_FM, }, 		/* FM detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_nfm,
		"NFM",
	},
	/* MODE_DRM */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_DRM
		0,					/*  смещение в килогерцах для включения режима autosplit */
		0,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		0,		/* TXCW Включение передачи в обход балансного модулятора */
		1,		/* в этом режиме возможно использование VOX */
		0,		/* в этом режиме возможно использование BREAK-IN */
		0, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		1,		/* в этом режиме запрещён микрофонный усилитель */
		0,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		1,		/* в этом режиме разрешено использовать параметры PBT */
		0,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ DRM_LO5_SIDE_RX, DRM_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* начальный режим АРУ для MODE_DRM */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva для DRM
		{ 0, 0, 0, 0 },	// признаки включения самоконтроля для DRM
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_DRM, DSPCTL_MODE_TX_SSB, },	// Управление для DSP в режиме приёма и передачи - режим узкого фильтра
		{ BWSETI_DRM, BWSETI_DRM, },				// индекс банка полос пропускания для данного режима
		{ 12000, INT16_MAX, },	// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
		BOARD_TXAUDIO_MUTE,		// источник звукового сигнала для данного режима
		TXAPROFIG_AM,				// группа профилей обработки звука
		AGCSETI_DRM,
		afpnoproc, // afproc
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_MUTE, BOARD_DETECTOR_MUTE, },		/* ssb detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_am,
		"DRM",
	},
	/* MODE_CWZ - этот режим при передаче используется во время TUNE. */
	{
		{ STEP_CWZ / 10, STEP_CWZ / 10, },	// step for MODE_CWZ
		0,					/*  смещение в килогерцах для включения режима autosplit */
		0,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		1,		/* TXCW Включение передачи в обход балансного модулятора */
		1,		/* в этом режиме возможно использование VOX */
		0,		/* в этом режиме возможно использование BREAK-IN */
		0, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		1,		/* в этом режиме запрещён микрофонный усилитель */
		0,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		0,		/* в этом режиме разрешено использовать параметры PBT */
		0,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ CWZ_LO5_SIDE_RX, CWZ_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_MED,	/* начальный режим АРУ для MODE_CWZ */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_CW, TXGFV_TX_CW, }, // txgfva для CWZ (TUNE)
		{ 0, 0, 1, 1 },	// признаки включения самоконтроля для CWZ (TUNE)
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_DSB, DSPCTL_MODE_TX_CW, },	// Управление для DSP в режиме приёма и передачи - режим узкого фильтра
		{ BWSETI_SSB, BWSETI_CW, },				// индекс банка полос пропускания для данного режима
		{ 0, INT16_MAX, },	// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
		BOARD_TXAUDIO_MUTE,		// источник звукового сигнала для данного режима
		TXAPROFIG_SSB,				// группа профилей обработки звука
		AGCSETI_SSB,
		afpnoproc, // afproc
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_TUNE, },		/* ssb detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_am,
		"CWZ",
	},
#if WITHWFM || WITHMODESETFULLNFMWFM
	/* WFM mode if=10.7 MHz */
	/* MODE_WFM */
	{
		{ STEP_WFM_HIGH / 10, STEP_WFM_LOW / 10, },	// step for MODE_CW
		0,			/*  смещение в килогерцах для включения режима autosplit */
		0,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		1,		/* Включение передачи в обход балансного модулятора */
		1,		/* в этом режиме возможно использование VOX */
		0,		/* в этом режиме возможно использование BREAK-IN */
		0, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		0,		/* в этом режиме запрещён микрофонный усилитель */
		0,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		0,		/* в этом режиме разрешено использовать параметры PBT */
		0,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ WFM_LO5_SIDE_RX, WFM_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_FAST,	/* начальный режим АРУ для MODE_NFM */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_CW, TXGFV_TX_CW, }, // txgfva для AM
		{ 0, 0, 0, 0 },	// признаки включения самоконтроля для AM
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_WFM, DSPCTL_MODE_TX_SSB, },	// Управление для DSP в режиме приёма и передачи - режим широкого фильтра
		{ BWSETI_WFM, BWSETI_WFM, },				// индекс банка полос пропускания для данного режима
		{ INT16_MAX, INT16_MAX, },	// bypass, фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
		BOARD_TXAUDIO_MUTE,		// источник звукового сигнала для данного режима
		TXAPROFIG_SSB,				// группа профилей обработки звука
		AGCSETI_SSB,
		afpcw, // afproc
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_WFM, BOARD_DETECTOR_WFM, },		/* WFM detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_wfm,
		"WFM",
	},
#endif /* WITHWFM || WITHMODESETFULLNFMWFM */
	/* MODE_DIGI */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_SSB
		5,		/*  смещение в килогерцах для включения режима autosplit */
		0,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		0,		/* TXCW Включение передачи в обход балансного модулятора */
		1,		/* в этом режиме возможно использование VOX */
		0,		/* в этом режиме возможно использование BREAK-IN */
		0, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		0,		/* в этом режиме запрещён микрофонный усилитель */
		1,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		1,		/* в этом режиме разрешено использовать параметры PBT */
		0,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ SSB_LO5_SIDE_RX, SSB_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* начальный режим АРУ для MODE_SSB */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva для SSB
		{ 0, 0, 0, 0 },	// признаки включения самоконтроля для SSB
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_WIDE, DSPCTL_MODE_TX_DIGI, },	// Управление для DSP в режиме приёма и передачи - режим широкого фильтра
		{ BWSETI_DIGI, BWSETI_DIGI, },				// индекс банка полос пропускания для данного режима
		{ 0, 0, },	// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
	#if WITHUSBHW && WITHUSBUACOUT
		BOARD_TXAUDIO_USB,		// источник звукового сигнала для данного режима
	#else /* WITHUSBHW && WITHUSBUACOUT */
		BOARD_TXAUDIO_MIKE,		// источник звукового сигнала для данного режима
	#endif /* WITHUSBHW && WITHUSBUACOUT */
		TXAPROFIG_DIGI,				// группа профилей обработки звука
		AGCSETI_DIGI,
		afpnoproc, // afproc - сигнал не обрабатывается
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, },		/* ssb detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_digi,
		"DIG",
	},
	/* MODE_RTTY */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_CW
		5,			/*  смещение в килогерцах для включения режима autosplit */
		1,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		0,		/* Включение передачи в обход балансного модулятора */
		1,		/* в этом режиме возможно использование VOX */
		0,		/* в этом режиме возможно использование BREAK-IN */
		0, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		1,		/* в этом режиме запрещён микрофонный усилитель */
		0,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		1,		/* в этом режиме разрешено использовать параметры PBT */
		0,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ DIGI_LO5_SIDE_RX, DIGI_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* начальный режим АРУ для MODE_SSB */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva для DIGI
		{ 0, 0, 0, 0 },	// признаки включения самоконтроля для DIGI
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_WIDE, DSPCTL_MODE_TX_SSB, },	// Управление для DSP в режиме приёма и передачи - режим широкого фильтра
		{ BWSETI_DIGI, BWSETI_DIGI, },				// индекс банка полос пропускания для данного режима
		{ 0, INT16_MAX, },	// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
	#if WITHUSBHW && WITHUSBUACOUT
		BOARD_TXAUDIO_USB,		// источник звукового сигнала для данного режима
	#else /* WITHUSBHW && WITHUSBUACOUT */
		BOARD_TXAUDIO_MIKE,		// источник звукового сигнала для данного режима
	#endif /* WITHUSBHW && WITHUSBUACOUT */
		TXAPROFIG_DIGI,				// группа профилей обработки звука
		AGCSETI_DIGI,
		afprtty, // afproc
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, }, 		/* ssb detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_digi,
		"TTY",
	},
#if WITHMODEM
	/* MODE_MODEM */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_MODEM
		5,			/*  смещение в килогерцах для включения режима autosplit */
		1,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		0,		/* Включение передачи в обход балансного модулятора */
		1,		/* в этом режиме возможно использование VOX */
		0,		/* в этом режиме возможно использование BREAK-IN */
		0, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		1,		/* в этом режиме запрещён микрофонный усилитель */
		0,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		0,		/* в этом режиме разрешено использовать параметры PBT */
		0,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ BPSK_LO5_SIDE_RX, BPSK_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* начальный режим АРУ для MODE_SSB */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva для DIGI
		{ 0, 0, 0, 0 },	// признаки включения самоконтроля для DIGI
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_BPSK, DSPCTL_MODE_TX_BPSK, },	// Управление для DSP в режиме приёма и передачи - режим широкого фильтра
		{ BWSETI_DIGI, BWSETI_DIGI, },				// индекс банка полос пропускания для данного режима
		{ 0, INT16_MAX, },	// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
		BOARD_TXAUDIO_MUTE,		// источник звукового сигнала для данного режима
		TXAPROFIG_SSB,				// группа профилей обработки звука
		AGCSETI_SSB,
		afpnoproc, // afproc
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, }, 		/* ssb detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_digi,
		"MDM",
	},
#endif /* WITHMODEM */
	/* MODE_ISB */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_SSB
		5,					/*  смещение в килогерцах для включения режима autosplit */
		0,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		0,		/* TXCW Включение передачи в обход балансного модулятора */
		1,		/* в этом режиме возможно использование VOX */
		0,		/* в этом режиме возможно использование BREAK-IN */
		0, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		0,		/* в этом режиме запрещён микрофонный усилитель */
		1,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		1,		/* в этом режиме разрешено использовать параметры PBT */
		0,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ SSB_LO5_SIDE_RX, SSB_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* начальный режим АРУ для MODE_SSB */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva для SSB
		{ 0, 0, 0, 0 },	// признаки включения самоконтроля для SSB
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_ISB, DSPCTL_MODE_TX_ISB, },	// Управление для DSP в режиме приёма и передачи - режим широкого фильтра
		{ BWSETI_SSB, BWSETI_SSBTX, },				// индекс банка полос пропускания для данного режима
		{ 0, 0, },	// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
	#if WITHUSBHW && WITHUSBUACOUT
		BOARD_TXAUDIO_USB,		// источник звукового сигнала для данного режима
	#else /* WITHUSBHW && WITHUSBUACOUT */
		BOARD_TXAUDIO_MIKE,		// источник звукового сигнала для данного режима
	#endif /* WITHUSBHW && WITHUSBUACOUT */
		TXAPROFIG_SSB,				// группа профилей обработки звука
		AGCSETI_SSB,
		afpcw, // afproc
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, },		/* ssb detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_ssb,
		"ISB",
	},
#if WITHFREEDV
	/* MODE_FREEDV */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_FREEDV
		5,			/*  смещение в килогерцах для включения режима autosplit */
		1,		/* nar: разрешение включения НЧ фильтра в этом режиме */
		0,		/* Включение передачи в обход балансного модулятора */
		1,		/* в этом режиме возможно использование VOX */
		0,		/* в этом режиме возможно использование BREAK-IN */
		1, 		/* в этом режиме возможно использование BREAK-IN если включён режим SSB TXCW */
		1,		/* в этом режиме запрещён микрофонный усилитель */
		0,		/* в этом режиме разрешено использовать параметры IF SHIFT */
		0,		/* в этом режиме разрешено использовать параметры PBT */
		0,		/* в этом режиме разрешено использовать subtones */
#if ! defined (LO5_SIDE)
		{ BPSK_LO5_SIDE_RX, BPSK_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* начальный режим АРУ для MODE_SSB */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva для DIGI
		{ 0, 0, 0, 0 },	// признаки включения самоконтроля для DIGI
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_FREEDV, DSPCTL_MODE_TX_FREEDV, },	// Управление для DSP в режиме приёма и передачи - режим широкого фильтра
		{ BWSETI_SSB, BWSETI_SSBTX, },				// индекс банка полос пропускания для данного режима
		{ 0, 0, },	// фиксированная полоса пропускания в DSP (if6) для данного режима (если не ноль).
		BOARD_TXAUDIO_MIKE,		// источник звукового сигнала для данного режима
		TXAPROFIG_SSB,				// группа профилей обработки звука
		AGCSETI_SSB,
		afpcw, // afproc
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, }, 		/* ssb detector used */
#endif /* WITHIF4DSP */
		getmiddlemenu_ssb,
		"FDV",
	},
#endif /* WITHFREEDV */
};


#define ENCRES_24	0	/* значение по умолчанию для индекса при использовании енкодера на 24 позиции */
#define ENCRES_32	1	/* значение по умолчанию для индекса при использовании енкодера на 64 позиций */
#define ENCRES_64	2	/* значение по умолчанию для индекса при использовании енкодера на 64 позиций */
#define ENCRES_100	4	/* значение по умолчанию для индекса при использовании енкодера на 100 позиций */
#define ENCRES_128	5	/* значение по умолчанию для индекса при использовании енкодера на 128 позиций */
#define ENCRES_256	7	/* значение по умолчанию для индекса при использовании енкодера на 256 позиций */
#define ENCRES_360	9	/* значение по умолчанию для индекса при использовании енкодера на 360 позиций */
#define ENCRES_400	10	/* значение по умолчанию для индекса при использовании енкодера на 400 позиций */
#define ENCRES_600	11	/* значение по умолчанию для индекса при использовании енкодера на 600 позиций */

/* скорость 115200 не добавлена из соображений невозможностти точного формирования на atmega
   при частоте генератора 8 МГц
   */
static const uint_fast8_t encresols [] =
{
	24 / ENCRESSCALE,	// 0
	32 / ENCRESSCALE,	// 1
	64 / ENCRESSCALE,	// 2
	96 / ENCRESSCALE,	// 3
	100 / ENCRESSCALE,	// 4
	128 / ENCRESSCALE,	// 5
	144 / ENCRESSCALE,	// 6
	256 / ENCRESSCALE,	// 7
	300 / ENCRESSCALE,	// 8
	360 / ENCRESSCALE,	// 9
	400 / ENCRESSCALE,	// 10
	600 / ENCRESSCALE,	// 11
};

static size_t getvaltextencres(char * buff, size_t count, int_fast32_t value)
{
	ASSERT(value >= 0 && value < (int) ARRAY_SIZE(encresols));
	return local_snprintf_P(buff, count, "%u", encresols [value] * ENCRESSCALE);
}

#if WITHTOUCHGUI
	#define BANDPAD	0
#else
	#define BANDPAD 15000UL	/* 15 kHz - запас по сторонам от любительского диапазона */
#endif

#if (FIXSCALE_48M0_X1_DIV256 || (defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570))) && WITHMODESETSMART
	#define BANDFUSBFREQ	13800000L	/* Выше этой частоты по умолчанию используется USB */
#else
	#define BANDFUSBFREQ	9000000L	/* Выше этой частоты по умолчанию используется USB */
#endif
/* BANDMIDDLE - граница, по которой происходит разделение двух обзорных диапазонов */
#if defined (BANDMIDDLE)

#elif TUNE_TOP >= 54000000L

	#define BANDMIDDLE  32000000L //(29700000 + BANDPAD)
	#define UPPER_DEF	40000000L

#elif TUNE_TOP >= 30000000L

	#define BANDMIDDLE	20000000L
	#define UPPER_DEF	24000000L

#elif TUNE_TOP >= 16000000L

	// Р-143 "Багульник"
	#define BANDMIDDLE	15000000L
	#define UPPER_DEF	17000000L

#else
	// Р-143 "Багульник"
	#define BANDMIDDLE	12000000L
	#define UPPER_DEF	13000000L

#endif

#define TUNE_6MBAND	((TUNE_TOP) >= (54000000 + BANDPAD))		// наличие в аппарате диапазона 6 метров
#define TUNE_4MBAND	((TUNE_TOP) >= (70500000 + BANDPAD))		// наличие в аппарате диапазона 4 метра
#define TUNE_2MBAND	((TUNE_TOP) >= (146000000 + BANDPAD))		// наличие в аппарате диапазона 2 метра
#define TUNE_07MBAND	((TUNE_TOP) >= (440000000 + BANDPAD))		// наличие в аппарате диапазона 70 сантиметров

enum
{
	BANDSETF_HAM = 0x00,
	BANDSETF_2M = 0x10,
	BANDSETF_6M = 0x20,
	BANDSETF_4M = 0x30,
	BANDSETF_07M = 0x40,
	BANDSETF_CB = 0x50,
	BANDSETF_ALL = 0x60,	/* диапазон присутствует во всех наборах диапазонов */
	BANDSETF_BCAST = 0x70,	/* вещательный диапазон */
	BANDSETF_HAMWARC = 0x80,
	//
	BANDSET_MASK = 0xf0,
	BANDSET_SUBMODE = 0x0f
};

enum
{
	BANDGROUP_LW,
	BANDGROUP_MW,
	BANDGROUP_SWLOW,
	BANDGROUP_SWHIGH,
	BANDGROUP_1p8MHz,
	BANDGROUP_3p5MHz,
	BANDGROUP_4p44MHz,
	BANDGROUP_5p24MHz,
	BANDGROUP_5p35MHz,
	BANDGROUP_7MHz,
	BANDGROUP_10p1MHz,
	BANDGROUP_14MHz,
	BANDGROUP_18MHz,
	BANDGROUP_21MHz,
	BANDGROUP_24p8MHz,
	BANDGROUP_CB,
	BANDGROUP_28MHz,
	BANDGROUP_45MHz,
	BANDGROUP_50MHz,
	BANDGROUP_70MHz,
	BANDGROUP_144MHz,
	BANDGROUP_430MHz,
	BANDGROUP_LO,
	BANDGROUP_HI,
	//
	BANDGROUP_COUNT			// Значение, используемое как признак отсутствия группировки диапазонов
};


static const char * const bandlabels [BANDGROUP_COUNT] =
{
	"LW",
	"MW",
	"SW1",
	"SW2",
	"1.8",
	"3.5",
	"5.3",
	"7",
	"10",
	"14",
	"18",
	"21",
	"24",
	"CB",
	"28",
	"45",
	"50",
	"70",
	"144",
	"430",
	"lo",
	"hi",
};

#if (FLASHEND > 0x7FFF)
#else
#endif

#if (FLASHEND > 0x3FFF)	|| CPUSTYLE_ARM || CPUSTYLE_RISCV || (TUNE_TOP >= 65535000L)

	/* частоты хранятся с точностью до герца в 32-х битных переменных */
	struct bandrange {
		uint32_t bottom, top;
		uint32_t init;
		uint8_t defsubmode_bandset;
		uint8_t bandgroup;
		const char * label;
	};

	#define BMF(a) (a)		/* получение инициализационного элемента */
	#define PEEK_BMF(v) (v)	/* выборка значения для использования */

#else
	/* иначе, частоты хранятся с точностью до килогерца в 16-ти битных переменных */
	struct bandrange {
		uint16_t bottom, top;
		uint16_t init;
		uint8_t defsubmode_bandset;
		uint8_t	bandgroup;
		char label[9];
	};

	#define BMF_SCALE	1000UL
	#define BMF(a) ((a) / BMF_SCALE )	/* получение инициализационного элемента */
	#define PEEK_BMF(v) ((v) * BMF_SCALE)	/* выборка значения для использования */

#endif

#if WITHMODESETSMART
	#define BANDMAPSUBMODE_LSB	SUBMODE_SSBSMART
	#define BANDMAPSUBMODE_USB	SUBMODE_SSBSMART
	#define BANDMAPSUBMODE_CW	SUBMODE_CWSMART
	#define BANDMAPSUBMODE_CWR	SUBMODE_CWSMART
	#define BANDMAPSUBMODE_AM	SUBMODE_SSBSMART
	#define BANDMAPSUBMODE_NFM	SUBMODE_NFM
#else /* WITHMODESETSMART */
	#define BANDMAPSUBMODE_LSB	SUBMODE_LSB
	#define BANDMAPSUBMODE_USB	SUBMODE_USB
	#define BANDMAPSUBMODE_CW	SUBMODE_CW
	#define BANDMAPSUBMODE_CWR	SUBMODE_CWR
	#define BANDMAPSUBMODE_AM	SUBMODE_AM
	#define BANDMAPSUBMODE_WFM	SUBMODE_WFM
	#define BANDMAPSUBMODE_NFM	SUBMODE_NFM
#endif /* WITHMODESETSMART */

/*
	60-meter band (all modes - USB):
	5330.5, 5346.5, 5366.5, 5371.5 and 5403.5 kHz

	UK: ~5298..~5408

*/

static struct bandrange  const bandsmap [] =
{
#if FQMODEL_FMRADIO	// 87..108.5 MHz
	{ BMF(89000000), 			BMF(89500000), 			BMF(79000000), 	BANDMAPSUBMODE_WFM | BANDSETF_ALL, BANDGROUP_COUNT, "", },				/*  */
	{ BMF(90000000), 			BMF(90500000), 			BMF(90000000), 	BANDMAPSUBMODE_WFM | BANDSETF_ALL, BANDGROUP_COUNT, "", },				/*  */
	{ BMF(102000000), 			BMF(102500000), 			BMF(102000000), 	BANDMAPSUBMODE_WFM | BANDSETF_ALL, BANDGROUP_COUNT, "", },				/*  */
	/* Далее никаких диапазонов добавлять нельзя - это служебные элементы и их порядок зависит от других частей пронграммы (band_up и band_down). */
	{ BMF(TUNE_BOTTOM), 		BMF(90000000), 			BMF(TUNE_BOTTOM),	BANDMAPSUBMODE_USB | BANDSETF_ALL, BANDGROUP_COUNT, "", },				/* обзорный диапазон - HBANDS_COUNT should equal to this index */
	{ BMF(90000000), 			BMF(TUNE_TOP), 				BMF(101400000), 	BANDMAPSUBMODE_USB | BANDSETF_ALL, BANDGROUP_COUNT, },					/* обзорный диапазон */
	/* VFOS */
	{ BMF(TUNE_BOTTOM), 		BMF(TUNE_TOP), 				BMF(101400000), 	BANDMAPSUBMODE_USB | BANDSETF_ALL, BANDGROUP_COUNT, "", },				/* VFO A - VFOS_BASE should equal to this index */
	{ BMF(TUNE_BOTTOM), 		BMF(TUNE_TOP), 				BMF(101400000), 	BANDMAPSUBMODE_USB | BANDSETF_ALL, BANDGROUP_COUNT, "", },				/* VFO B */
#else /* FQMODEL_FMRADIO */
//	{ BMF(135000 - 0), 			BMF(138000 + 0), 			BMF(136000), 		SUBMODE_USB, 						BANDGROUP_COUNT, "", },				/* 135),7-137),8 кГц */
	#if TUNE_BOTTOM <= (153000 - BANDPAD)
	{ BMF(153000 - BANDPAD), 	BMF(279000 + BANDPAD), 	BMF(225000), 		BANDMAPSUBMODE_AM | BANDSETF_ALL, BANDGROUP_LW, "LW", },				/*  */
	{ BMF(530000 - BANDPAD), 	BMF(1611000 + BANDPAD), 	BMF(1440000), 		BANDMAPSUBMODE_AM | BANDSETF_ALL, BANDGROUP_MW, "MW", },				/*  */
	#endif
	{ BMF(1810000 - BANDPAD), 	BMF(2000000 + BANDPAD), 	BMF(1810000), 		BANDMAPSUBMODE_LSB | BANDSETF_HAM, BANDGROUP_1p8MHz, "1.8M"},			/* Ukrainian band from freq 1715 kHz */
	{ BMF(3500000 - BANDPAD), 	BMF(3800000 + BANDPAD), 	BMF(3500000),		BANDMAPSUBMODE_LSB | BANDSETF_HAM, BANDGROUP_3p5MHz, "3.5M", },			/*  */
#if 0
	{ BMF(2535000 - BANDPAD), 	BMF(2900000 + BANDPAD), 	BMF(2535000), 		BANDMAPSUBMODE_USB | BANDSETF_HAM, BANDGROUP_SWLOW, "", },				/* Old NEDRA stations band */
#endif
	{ BMF(3900000), 			BMF(4000000), 				BMF(3900000), 		BANDMAPSUBMODE_AM | BANDSETF_BCAST, BANDGROUP_SWLOW, "75m", },			/*  */
#if WITHBANDR1BBU
	{ BMF(4455000 - BANDPAD), 	BMF(4455000 + BANDPAD), 	BMF(4455000), 		BANDMAPSUBMODE_USB | BANDSETF_HAM, BANDGROUP_4p44MHz, "4.4M"},
#endif /* WITHBANDR1BBU */
	{ BMF(4750000), 			BMF(5060000), 				BMF(4750000), 		BANDMAPSUBMODE_AM | BANDSETF_BCAST, BANDGROUP_SWLOW, "", },				/*  */
#if WITHBANDR1BBU
	{ BMF(5245000 - BANDPAD), 	BMF(5245000 + BANDPAD), 	BMF(5245000), 		BANDMAPSUBMODE_USB | BANDSETF_HAM, BANDGROUP_5p24MHz, "5.2M"},
#endif /* WITHBANDR1BBU */
	/*
		Частотный план диапазона 5 MHz ( 60m )
		Диапазон: 5351.5-5.366.5 khz
		1. 5.351.5- CW -полоса 200 hz .
		2. 5.354.0, 5.357.0, 5.360.0, 5.363.0 - ALL MODE полоса 2700 hz.
		3. 5366.0 - weak signal полоса 20 hz.
		На самом деле "цифра" около 5,357,0, CW 5.351.5 и выше SSB около 5.360.0
	*/
	{ BMF(5298000), 			BMF(5408000), 				BMF(5351500), 		BANDMAPSUBMODE_CW | BANDSETF_HAMWARC, 	BANDGROUP_5p35MHz, "", },		/* 60-meters band */
	{ BMF(5730000), 			BMF(6295000), 				BMF(5730000), 		BANDMAPSUBMODE_AM | BANDSETF_BCAST, 	BANDGROUP_SWLOW, "49m", },		/*  */
	{ BMF(6890000), 			BMF(6990000), 				BMF(6890000), 		BANDMAPSUBMODE_AM | BANDSETF_BCAST,  	BANDGROUP_COUNT, "", },			/*  */
	{ BMF(7000000 - BANDPAD), 	BMF(7200000 + BANDPAD), 	BMF(7000000), 		BANDMAPSUBMODE_LSB | BANDSETF_HAM, 		BANDGROUP_7MHz, "7M", },		/* top freq - 7300 in region-2 */
	{ BMF(7200000), 			BMF(7600000), 				BMF(7200000), 		BANDMAPSUBMODE_AM | BANDSETF_BCAST,		BANDGROUP_SWLOW, "41m", },		/*  */
	{ BMF(9250000), 			BMF(9900000), 				BMF(9250000), 		BANDMAPSUBMODE_AM | BANDSETF_BCAST, 	BANDGROUP_SWHIGH, "31m", },		/*  */
	{ BMF(10100000 - BANDPAD), 	BMF(10150000 + BANDPAD), 	BMF(10100000), 	BANDMAPSUBMODE_CW | BANDSETF_HAMWARC, 	BANDGROUP_10p1MHz, "10.1M", },			/*  */
#if (TUNE_TOP) >= (19020000)
	{ BMF(11500000), 			BMF(12160000), 				BMF(11500000), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  	BANDGROUP_SWHIGH, "25m", },		/*  */
	{ BMF(13570000), 			BMF(13870000), 				BMF(13570000), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  	BANDGROUP_SWHIGH, "22m", },		/*  */

	{ BMF(14000000 - BANDPAD), 	BMF(14105000), 			BMF(14000000), 	BANDMAPSUBMODE_CW | BANDSETF_HAM, 		BANDGROUP_14MHz, "14M CW", },	/*  */
	{ BMF(14105000),			BMF(14350000 + BANDPAD),	BMF(14130000), 	BANDMAPSUBMODE_USB | BANDSETF_HAM, 		BANDGROUP_14MHz, "14M SSB", },	/*  */

	{ BMF(15030000), 			BMF(15800000), 				BMF(15030000), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  	BANDGROUP_SWHIGH, "19m", },		/*  */
	{ BMF(17480000), 			BMF(17900000), 				BMF(17480000), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  	BANDGROUP_SWHIGH, "16m", },		/*  */
	{ BMF(18068000 - BANDPAD), 	BMF(18168000 + BANDPAD), 	BMF(18068000), 	BANDMAPSUBMODE_USB | BANDSETF_HAMWARC, 	BANDGROUP_18MHz, "18M", },			/*  */
	{ BMF(18900000), 			BMF(19020000), 				BMF(18900000), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  	BANDGROUP_SWHIGH, "15m", },		/*  */
#endif
#if (TUNE_TOP) >= (21450000 + BANDPAD)
	{ BMF(21000000 - BANDPAD), 	BMF(21155000), 			BMF(21000000), 	BANDMAPSUBMODE_CW | BANDSETF_HAM, 		BANDGROUP_21MHz, "21M CW", },		/*  */
	{ BMF(21155000), 			BMF(21450000 + BANDPAD), 	BMF(21160000), 	BANDMAPSUBMODE_USB | BANDSETF_HAM, 		BANDGROUP_21MHz, "21M SSB", },		/*  */
#endif
#if (TUNE_TOP) >= (21850000)
	{ BMF(21450000), 			BMF(21850000), 				BMF(21450000), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  	BANDGROUP_SWHIGH, "13m", },		/*  */
#endif /* (TUNE_TOP) >= (21850000) */

#if (TUNE_TOP) >= (29700000 + BANDPAD)
	{ BMF(24890000 - BANDPAD), 	BMF(24990000 + BANDPAD), 	BMF(24890000), 	BANDMAPSUBMODE_USB | BANDSETF_HAMWARC, 	BANDGROUP_24p8MHz, "24M", },			/*  */
	{ BMF(25670000), 			BMF(26100000), 				BMF(25670000), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  	BANDGROUP_SWHIGH, "11m", },		/*  */
	{ BMF(26965000 - BANDPAD), 	BMF(27405000 + BANDPAD), 	BMF(27120000), 	BANDMAPSUBMODE_USB | BANDSETF_CB, 		BANDGROUP_CB, "CB", },		/* Citizens Band 26.9650 MHz to 27.4050 MHz (40 channels) */

	/* next three sections - one band - "ten". */
	{ BMF(28000000 - BANDPAD), 	BMF(28320000), 			BMF(28000000), 	BANDMAPSUBMODE_CW | BANDSETF_HAM, 		BANDGROUP_28MHz, "28M CW", },	/* CW */
	{ BMF(28320000), 			BMF(29200000), 			BMF(28500000), 	BANDMAPSUBMODE_USB | BANDSETF_HAM, 		BANDGROUP_28MHz, "28M SSB", },	/* SSB */
	{ BMF(29200000), 			BMF(29700000 + BANDPAD),	BMF(29600000), 	BANDMAPSUBMODE_USB | BANDSETF_HAM, 		BANDGROUP_28MHz, "28M FM", },	/* FM */
#endif

#if WITHBANDR1BBU
	{ BMF(41000000 - BANDPAD), 	BMF(49000000 + BANDPAD), 	BMF(44880000), 		BANDMAPSUBMODE_NFM | BANDSETF_HAM, BANDGROUP_45MHz, "LowBand"},
#endif /* WITHBANDR1BBU */
#if TUNE_6MBAND
	{ BMF(50000000 - BANDPAD), 	BMF(54000000 + BANDPAD), 	BMF(50100000), 	BANDMAPSUBMODE_USB | BANDSETF_6M, 		BANDGROUP_50MHz, "50M SSB", },			/* 6 meters HAM band */
#endif /* TUNE_6MBAND */

#if TUNE_4MBAND
	{ BMF(70000000 - BANDPAD),	BMF(70050000), 			BMF(70000000), 	BANDMAPSUBMODE_CW | BANDSETF_4M, 		BANDGROUP_70MHz, "70M CW", },			/* CW */
	{ BMF(70050000), 			BMF(70300000), 			BMF(70050000), 	BANDMAPSUBMODE_USB | BANDSETF_4M, 		BANDGROUP_70MHz, "70M SSB", },			/* SSB */
	{ BMF(70300000), 			BMF(70500000 + BANDPAD),	BMF(70300000), 	BANDMAPSUBMODE_USB | BANDSETF_4M, 		BANDGROUP_70MHz, "70M FM", },			/* FM */
#endif /* TUNE_4MBAND */

#if TUNE_2MBAND
	/* next three sections - one band - "2 meter". */
	{ BMF(144000000 - BANDPAD),BMF(144200000), 			BMF(144050000), 	BANDMAPSUBMODE_CW | BANDSETF_2M, 		BANDGROUP_144MHz, "144M CW", },		/* CW */
	{ BMF(144200000), 			BMF(145000000), 			BMF(144300000), 	BANDMAPSUBMODE_USB | BANDSETF_2M, 		BANDGROUP_144MHz, "144M SSB", },		/* SSB */
	{ BMF(144500000), 			BMF(146000000 + BANDPAD),	BMF(145500000), 	BANDMAPSUBMODE_USB | BANDSETF_2M, 		BANDGROUP_144MHz, "144M FM", },		/* FM */
#endif /* TUNE_2MBAND */

#if TUNE_07MBAND
	/* next three sections - one band - "0.7 meter". */
	{ BMF(430000000 - BANDPAD),	BMF(432100000), 			BMF(430050000), 	BANDMAPSUBMODE_CW | BANDSETF_07M, 		BANDGROUP_430MHz, "", },		/* CW */
	{ BMF(432100000),			BMF(440000000 + BANDPAD), 	BMF(432500000), 	BANDMAPSUBMODE_USB | BANDSETF_07M, 		BANDGROUP_430MHz, "", },		/* CW */
#endif /* TUNE_2MBAND */
	/* далее никаких диапазонов добавлять нельзя - это служебные элементы и их порядок зависит от других частей пронграммы (band_up и band_down). */
	{ BMF(TUNE_BOTTOM), 		BMF(BANDMIDDLE), 			BMF(9995000), 		BANDMAPSUBMODE_USB | BANDSETF_ALL, 		BANDGROUP_LO, "", },			/* обзорный диапазон - HBANDS_COUNT should equal to this index */
	{ BMF(BANDMIDDLE), 			BMF(TUNE_TOP), 				BMF(UPPER_DEF), 	BANDMAPSUBMODE_USB | BANDSETF_ALL, 		BANDGROUP_HI, "", },			/* обзорный диапазон */
	/* VFOS */
	{ BMF(TUNE_BOTTOM), 		BMF(TUNE_TOP), 				BMF(DEFAULTDIALFREQ), BANDMAPSUBMODE_USB | BANDSETF_ALL, 	BANDGROUP_COUNT, "", },			/* VFO A - VFOS_BASE should equal to this index */
	{ BMF(TUNE_BOTTOM), 		BMF(TUNE_TOP), 				BMF(DEFAULTDIALFREQ), BANDMAPSUBMODE_USB | BANDSETF_ALL, 	BANDGROUP_COUNT, "", },			/* VFO B */
#endif /* FQMODEL_FMRADIO */
};

#define VFOS_COUNT 2	/* два VFO - A и B */
#define XBANDS_COUNT 2	/* два обзорных диапазона */
#define HBANDS_COUNT ((sizeof bandsmap / sizeof bandsmap [0]) - XBANDS_COUNT - VFOS_COUNT)
#define VFOS_BASE ((sizeof bandsmap / sizeof bandsmap [0]) - VFOS_COUNT)
#define XBANDS_BASE0	(HBANDS_COUNT + 0)	/* первая из двух ячеек с обзорными диапазонами */
#define XBANDS_BASE1	(HBANDS_COUNT + 1)	/* вторая из двух ячеек с обзорными диапазонами */
#define MBANDS_BASE (HBANDS_COUNT + XBANDS_COUNT + VFOS_COUNT)	/* первая ячейка с фиксированными настройками */


#if WITHSWLMODE

	#if ! defined (NVRAM_TYPE) || ! defined (NVRAM_END)
		#error NVRAM_TYPE or NVRAM_END not defined here
	#endif

	#if NVRAM_END <= 511
		#define MBANDS_COUNT	21	/* количество ячеек фиксированных частот */
		typedef uint_fast8_t vindex_t;
	#elif NVRAM_END <= 4095
		#define MBANDS_COUNT	100 //150	/* количество ячеек фиксированных частот */
		typedef uint_fast8_t vindex_t;
	#else
		#define MBANDS_COUNT	1000 // (254 - MBANDS_BASE)	/* количество ячеек фиксированных частот */
		typedef uint_fast16_t vindex_t;
	#endif

#elif WITHTOUCHGUI

	#define MBANDS_COUNT	memory_cells_count	/* количество ячеек фиксированных частот */
		typedef unsigned vindex_t;

#else

	#define MBANDS_COUNT	0	/* количество ячеек фиксированных частот */
	typedef uint_fast8_t vindex_t;

#endif	/* WITHSWLMODE */

static vindex_t getfreqband(const uint_fast32_t freq, uint_fast8_t bandset_no_check);

/* получение индекса хранения VFO в памяти в зависимости от текущего режима расстройки
   - в режиме приема
*/
static vindex_t
getvfoindex(uint_fast8_t bi)
{
	ASSERT(bi < 2);
	return VFOS_BASE + bi;
}

/* получение человекопонятного названия диапазона */
const char *
get_band_label3(unsigned b)	/* b: диапазон в таблице bandsmap */
{
	ASSERT(b != ((vindex_t) - 1));
	const uint_fast8_t bandgroup = bandsmap [b].bandgroup;
	if (bandgroup >= ARRAY_SIZE(bandlabels))
		return "ERR";
	if (bandlabels [bandgroup] == NULL)
		return "NUL";	// недостаточно правильно заполненная таблица
	return bandlabels [bandgroup];
}

/* интерфейсная функция доступа к параметра диапазона */
static uint_fast32_t
//NOINLINEAT
get_band_bottom(vindex_t b)	/* b: диапазон в таблице bandsmap */
{
	ASSERT(b != ((vindex_t) - 1));
	return PEEK_BMF(bandsmap [b].bottom);
}
/* интерфейсная функция доступа к параметра диапазона */
static uint_fast32_t
//NOINLINEAT
get_band_top(vindex_t b)	/* b: диапазон в таблице bandsmap */
{
	ASSERT(b != ((vindex_t) - 1));
	return PEEK_BMF(bandsmap [b].top);
}
/* интерфейсная функция доступа к параметра диапазона */
static uint_fast32_t
//NOINLINEAT
get_band_init(vindex_t b)	/* b: диапазон в таблице bandsmap */
{
	ASSERT(b != ((vindex_t) - 1));
	return PEEK_BMF(bandsmap [b].init);
}
/* интерфейсная функция доступа к параметра диапазона */
static uint_fast8_t
//NOINLINEAT
get_band_defsubmode(vindex_t b)	/* b: диапазон в таблице bandsmap */
{
	ASSERT(b != ((vindex_t) - 1));
	return bandsmap [b].defsubmode_bandset & BANDSET_SUBMODE;
}

/* интерфейсная функция доступа к параметра диапазона */
static uint_fast8_t
//NOINLINEAT
get_band_bandset(vindex_t b)	/* b: диапазон в таблице bandsmap */
{
	ASSERT(b != ((vindex_t) - 1));
	return bandsmap [b].defsubmode_bandset & BANDSET_MASK;
}

static const char * get_band_label(vindex_t b)	/* b: диапазон в таблице bandsmap */
{
	ASSERT(b != ((vindex_t) - 1));
	return bandsmap [b].label;
}

/* "карта" режимов,
 * Переход по строкам идет по короткому нажатию кнопки "режим",
 * переход в пределах сроки идет по длинному нажатию кнопки "режим".
 */
#if WITHBBOX && defined (WITHBBOXSUBMODE)
	static const uint_fast8_t modes [][2] =
	{
		{ 1, WITHBBOXSUBMODE, },
	};
#elif WITHMODESETSMART
	static const uint_fast8_t modes [][2] =
	{
		{ 1, SUBMODE_SSBSMART, },
		{ 1, SUBMODE_CWSMART, },
		{ 1, SUBMODE_DIGISMART, },
	};
#elif WITHMODESETMIXONLY	// Use only product detector
	static const uint_fast8_t modes [][3] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 2, SUBMODE_CWR, SUBMODE_CW, },
	};
#elif WITHMODESETMIXONLY3
	static const uint_fast8_t modes [][4] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 2, SUBMODE_CWR, SUBMODE_CW, },
		{ 3, SUBMODE_DGU, SUBMODE_DGL, SUBMODE_CWZ, },
	};
#elif WITHMODESAMONLY
	static const uint_fast8_t modes [][2] =
	{
		{ 1, SUBMODE_AM, },
	};
#elif WITHMODESETMIXONLY3AM
	static const uint_fast8_t modes [][4] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 2, SUBMODE_CWR, SUBMODE_CW, },
		{ 2, SUBMODE_AM, SUBMODE_CWZ, },
		{ 2, SUBMODE_DGU, SUBMODE_DGL, },
	};
#elif WITHMODESETMIXONLY3NFM // SW2014FM modes set
	static const uint_fast8_t modes [][3] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 2, SUBMODE_CWR, SUBMODE_CW, },
		{ 2, SUBMODE_NFM, SUBMODE_CWZ, },
		{ 2, SUBMODE_DGU, SUBMODE_DGL, },
	};
#elif WITHMODESETMIXONLY2
	static const uint_fast8_t modes [][4] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 3, SUBMODE_CWR, SUBMODE_CW, SUBMODE_CWZ },
	};
#elif (WITHMODESETFULLNFM && WITHWFM)
	static const uint_fast8_t modes [][5] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 2, SUBMODE_CWR, SUBMODE_CW, },
		{ 3, SUBMODE_AM, SUBMODE_CWZ, SUBMODE_DRM, },
		{ 4, SUBMODE_NFM, SUBMODE_WFM, SUBMODE_DGU, SUBMODE_DGL, },
	};
#elif WITHMODESETFULLNFM
	#if KEYB_FPANEL30_V3
	static const uint_fast8_t modes [][4] =
		{
			{ 2, SUBMODE_LSB, SUBMODE_USB, },				// ROW 0
			{ 2, SUBMODE_CWR, SUBMODE_CW, },				// ROW 1
			{ 3, SUBMODE_AM, SUBMODE_CWZ, SUBMODE_DRM, },	// ROW 2
			{ 3, SUBMODE_NFM, SUBMODE_DGU, SUBMODE_DGL, },	// ROW 3
		};
	#elif WITHMODEM
		static const uint_fast8_t modes [][5] =
		{
			{ 1, SUBMODE_BPSK, },
		};
	#elif WITHSAM
	static const uint_fast8_t modes [][5] =
		{
			{ 2, SUBMODE_LSB, SUBMODE_USB, },
			{ 2, SUBMODE_CWR, SUBMODE_CW, },
			{ 4, SUBMODE_AM, SUBMODE_SAM, SUBMODE_CWZ, SUBMODE_DRM, },
			{ 3, SUBMODE_NFM, SUBMODE_DGU, SUBMODE_DGL, },
		#if WITHRTTY
			{ 1, SUBMODE_RTTY, },
		#endif /* WITHRTTY */
		};
	#else /* WITHMODEM */
	static const uint_fast8_t modes [][4] =
		{
			{ 2, SUBMODE_LSB, SUBMODE_USB, },
			{ 2, SUBMODE_CWR, SUBMODE_CW, },
			{ 3, SUBMODE_AM, SUBMODE_CWZ, SUBMODE_DRM, },
			{ 3, SUBMODE_NFM, SUBMODE_DGU, SUBMODE_DGL, },
		};
	#endif /* WITHMODEM */
#elif WITHMODESETFULLNFMWFM
	static const uint_fast8_t modes [][5] =
		{
			{ 2, SUBMODE_LSB, SUBMODE_USB, },
			{ 2, SUBMODE_CWR, SUBMODE_CW, },
			{ 4, SUBMODE_AM, SUBMODE_SAM, SUBMODE_CWZ, SUBMODE_DRM, },
			{ 4, SUBMODE_NFM, SUBMODE_WFM, SUBMODE_DGU, SUBMODE_DGL, },
		};
#elif WITHMODESETFULL
	static const uint_fast8_t modes [][4] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 2, SUBMODE_CWR, SUBMODE_CW, },
		{ 3, SUBMODE_AM, SUBMODE_CWZ, SUBMODE_DRM, },
		{ 2, SUBMODE_DGU, SUBMODE_DGL, },
	};
#elif WITHMODESET_IGOR
	static const uint_fast8_t modes [][4] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
	};
#else
	#error WITHMODESETxxx not defined
#endif /*  */


#define MODEROW_COUNT (sizeof modes / sizeof modes [0])

/* поиск координаты режима в карте режимов.
   код возврвта функции - колонка
   Если не найден - возврат 0 и строки 0.
   TODO: при попытке поиска режима FM, если его нет в списке, ставится LSB (0-й режим в 0-й строке. Не очень красиво.
*/
static uint_fast8_t
//NOINLINEAT
locatesubmode(
	const uint_fast8_t submode,		/* код режима */
	uint_fast8_t * const xrow		/* найденные координаты */
	)
{
	uint_fast8_t row;

	for (row = 0; row < MODEROW_COUNT; ++ row)
	{
		const uint_fast8_t n = modes [row] [0];
		uint_fast8_t col;

		for (col = 0; col < n && col < (sizeof modes [row] / sizeof modes [row][0] - 1); ++ col)
		{
			if (modes [row][col + 1] == submode)
			{
				* xrow = row;
				return col;
			}
		}
	}
	* xrow = 0;
	return 0;
}

#if 0
/* проверка наличия режима в карте режимов.
   Если не найден - возврат 0
*/
static uint_fast8_t
validatesubmode(
	const uint_fast8_t submode		/* код режима */
	)
{
	uint_fast8_t row;

	for (row = 0; row < MODEROW_COUNT; ++ row)
	{
		const uint_fast8_t n = modes [row] [0];
		uint_fast8_t col;

		for (col = 0; col < n && col < (sizeof modes [row] / sizeof modes [row][0] - 1); ++ col)
		{
			if (modes [row][col + 1] == submode)
			{
				return 1;
			}
		}
	}
	return 0;
}
#endif

/* текст (любой), используемый как сигнатура содержимого NVRAM */
/* последний байт этого массива в NVRAM не запоминается и не сравнивается. */
static const char nvramsign [] =
#if WITHKEEPNVRAM
		/* ослабить проверку совпадения версий прошивок для стирания NVRAM */
		{ 0, 0, 0, 0, 1, 1, 1, 1, };
#else /* WITHKEEPNVRAM */

#if NVRAM_END > 511
	__DATE__
#endif
__TIME__;
#endif /* WITHKEEPNVRAM */

/* Шаблон данных для тестирования доступа к NVRAM */
static const char nvrampattern [sizeof nvramsign / sizeof nvramsign [0]] =
{
	'D', 'E', 'A', 'D', 'B', 'E', 'E', 'F',
};

/* структура - расположение байтов в конфигурационной памяти.
   bitfields нельзя использовать, так как всё это - только обозначения смещений
	 переменных в конфигурационной памяти.
 	параметры "семейства" режимов - CW&CWR, LSB&USB.
	*/
struct modeprops
{
#if ! WITHAGCMODENONE
	uint8_t agc;	/* режим АРУ для данного семейства режимов */
#endif /* ! WITHAGCMODENONE */
	uint8_t filter;	/* индекс фильтра в общей таблице фильтров */
	//uint16_t step;	/* шаг валкодера в данном режиме */

#if WITHIF4DSP
#if WITHTX
	uint8_t txaudioindex;	/* источник звука для передачи (индекс) */
#endif /* WITHTX */
	uint8_t noisereduct;	/* включение NR для данного режима */
#endif /* WITHIF4DSP */
	uint8_t	gmidmenupos;	/* активный пункт в middlemenu */

} ATTRPACKED;// аттрибут GCC, исключает "дыры" в структуре. Так как в ОЗУ нет копии этой структуры, see also NVRAM_TYPE_BKPSRAM

/* структура - расположение байтов в конфигурационном ОЗУ.
   bitfields нельзя использовать, так как всё это - только обозначения смещений
	 переменных в конфигурационном ОЗУ.
	Информация, сохраняемая для каждого диапазона */

struct bandinfo
{
	uint32_t freq;		/* рабочая частота */
	uint8_t modecols [MODEROW_COUNT];	/* массив режимов работы - каждый байт указывают номер позиции в каждой строке) */
	uint8_t moderow;		/* номер режима работы в маске (номер тройки бит) */
	uint8_t glock;			/* блокировка валкодера */
} ATTRPACKED;// аттрибут GCC, исключает "дыры" в структуре. Так как в ОЗУ нет копии этой структуры, see also NVRAM_TYPE_BKPSRAM

/* структура - расположение байтов в конфигурационном ОЗУ.
   bitfields нельзя использовать, так как всё это - только обозначения смещений
	 переменных в конфигурационном ОЗУ.
 	 Информация, сохраняемая для каждой антенны */

struct onerxant_tag {
	uint8_t pamp;		/* режим УВЧ */
	uint8_t att;		/* режим аттенюатора */
} ATTRPACKED;	// аттрибут GCC, исключает "дыры" в структуре. Так как в ОЗУ нет копии этой структуры, see also NVRAM_TYPE_BKPSRAM

struct onetxant_tag {
#if WITHAUTOTUNER
	uint8_t tunercap;
	uint8_t tunerind;
	uint8_t tunertype;
	uint8_t tunerwork;
#else /* WITHAUTOTUNER */
	uint8_t dummy_paramm;
#endif /* WITHAUTOTUNER */
} ATTRPACKED;	// аттрибут GCC, исключает "дыры" в структуре. Так как в ОЗУ нет копии этой структуры, see also NVRAM_TYPE_BKPSRAM

/* структура - расположение байтов в конфигурационном ОЗУ.
   bitfields нельзя использовать, так как всё это - только обозначения смещений
	 переменных в конфигурационном ОЗУ.
 	 Информация, сохраняемая для группы диапазонов */

struct bandgroup_tag {
	uint8_t	band;		/* последний диапазон в группе, куда был переход по кнопке диапазона (индекс в bands). */
#if WITHANTSELECTRX || WITHANTSELECT1RX
	uint8_t rxant;		/* признак включения приемной антенны */
	uint8_t ant;		/* код выбора антенны (0/1) */
#elif WITHANTSELECT2
	uint8_t ant;		/* антенна при ручном выборе */
#elif WITHANTSELECT
	uint8_t ant;		/* код выбора антенны (1T+1R, 2T+1R и так далее, не код антенны (0/1) */
#endif /* WITHANTSELECT || WITHANTSELECTRX */
#if WITHSPECTRUMWF
	uint8_t gzoomxpow2;	/* уменьшение отображаемого участка спектра */
	uint8_t gtopdb;		/* нижний предел FFT */
	uint8_t gbottomdb;	/* верхний предел FFT */
#endif /* WITHSPECTRUMWF */
#if WITHANTSELECTRX || WITHANTSELECT1RX
	struct onerxant_tag orxants [ANTMODE_COUNT + 1];	// Параметры, связанные с антенами, которые могут использоваться на приём (TODO: добавить для приёмной антенны)
#else
	struct onerxant_tag orxants [ANTMODE_COUNT];	// Параметры, связанные с антенами, которые могут использоваться на приём (TODO: добавить для приёмной антенны)
#endif
	struct onetxant_tag otxants [ANTMODE_COUNT];	// Параметры, связанные с антенами, которые могут использоваться на передачу (параметры тюнера)
} ATTRPACKED;	// аттрибут GCC, исключает "дыры" в структуре. Так как в ОЗУ нет копии этой структуры, see also NVRAM_TYPE_BKPSRAM

/* структура - расположение байтов в конфигурационном ОЗУ.
   bitfields нельзя использовать, так как всё это - только обозначения смещений
	 переменных в конфигурационном ОЗУ.
*/
struct nvmap
{
#if WITHUSEUSBBT
	uint8_t tlvbt [TLVBT_SIZE];
#endif /* WITHUSEUSBBT */
#if WITHENCODER2
	uint8_t enc2state;
	uint8_t enc2pos;			// выбраный пунки меню (второй валкодер)
#endif /* WITHENCODER2 */
#if WITHUSEFAST
	uint8_t gusefast;			/* переключение в режим крупного шага */
#endif /* WITHUSEFAST */
	uint8_t gmenuset;		/* набор функций кнопок и режим отображения на дисплее */

	/* группы */
	uint16_t ggroup;			/* последняя группа в менюю, с которой работали */
	uint16_t	ggrpdisplay;	// последний посещённый пункт группы
	uint16_t	ggrptxparam;		// последний посещённый пункт группы
	uint16_t	ggrptxadj;		// последний посещённый пункт группы
	uint16_t	ggrpsecial;		// последний посещённый пункт группы
	uint16_t	ggrpaudio;		// последний посещённый пункт группы
	uint16_t	ggrpmike;		// последний посещённый пункт группы
#if WITHSUBTONES
	uint16_t ggrpctcss;		// последний посещённый пункт группы
#endif /* WITHSUBTONES */
#if defined (RTC1_TYPE)
	uint16_t	ggrpclock; // последний посещённый пункт группы
#endif /* defined (RTC1_TYPE) */
	uint16_t	ggrpabout;		// последний посещённый пункт группы

#if LO1MODE_HYBRID
	uint8_t alignmode;			/* режимы для настройки аппаратной части (0-нормальная работа) */
#endif /* LO1MODE_HYBRID */
#if WITHUSEDUALWATCH
	uint8_t mainsubrxmode;		// Левый/правый, A - main RX, B - sub RX
#endif /* WITHUSEDUALWATCH */
#if WITHENCODER
	uint8_t genc1pulses;		/* индекс в таблице разрешений валкодера */
	uint8_t genc1dynamic;	/* включение динамического енкодера */
#endif /* WITHENCODER */

#if WITHLCDBACKLIGHT
	uint8_t gbglight;
#endif /* WITHLCDBACKLIGHT */
#if WITHDCDCFREQCTL
	//uint16_t dcdcrefdiv;
#endif /* WITHDCDCFREQCTL */
#if WITHKBDBACKLIGHT
	uint8_t gkblight;
#endif /* WITHKBDBACKLIGHT */
#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT
	uint8_t gdimmtime;
#endif /* WITHLCDBACKLIGHT || WITHKBDBACKLIGHT */
#if WITHFANTIMER
	uint8_t gfanpatime;
#if (WITHTHERMOLEVEL || WITHTHERMOLEVEL2)
	uint8_t gfanpatempflag;
	uint8_t gfanpaofftemp;
	uint8_t gfanpaontemp;
#endif /* (WITHTHERMOLEVEL || WITHTHERMOLEVEL2) */
	#if WITHFANPWM
		uint16_t gfanpapwm;
	#endif /* WITHFANPWM */
#endif /* WITHFANTIMER */
#if WITHSLEEPTIMER
	uint8_t gsleeptime;
#endif /* WITHSLEEPTIMER */
#if LCDMODE_COLORED
	//uint8_t gbluebgnd;
#endif /* LCDMODE_COLORED */

#if WITHMIC1LEVEL
	uint16_t gmik1level;
#endif /* WITHMIC1LEVEL */
#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_NAU8822L)
	uint8_t ALCNEN;// = 0;	// ALC noise gate function control bit
	uint8_t ALCNTH;// = 0;	// ALC noise gate threshold level
	uint8_t ALCEN;// = 1;	// only left channel ALC enabled
	uint8_t ALCMXGAIN;// = 7;	// Set maximum gain limit for PGA volume setting changes under ALC control
	uint8_t ALCMNGAIN;// = 0;	// Set minimum gain value limit for PGA volume setting changes under ALC control
#endif /* defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_NAU8822L) */
#if WITHTX
	uint16_t	ggrptxparams; // последний посещённый пункт группы
	//uint8_t gfitx;		/* номер используемого фильтра на передачу */
	uint8_t gbandf2adj_a [NUMLPFADJ];	/* коррекция мощности по ФНЧ передачика */
	uint8_t gbandf2adj_b [NUMLPFADJ];	/* коррекция мощности по ФНЧ передачика */
#if WITHPACLASSA
	uint8_t gbandf2adj_classa [NUMLPFADJ];	/* коррекция мощности по ФНЧ передачика в режиме CLASSA*/
#endif /* WITHPACLASSA */
	#if WITHPOWERLPHP
		uint8_t gpwri;		// индекс в pwrmodes - мощность при обычной работе
		uint8_t gpwratunei;	// индекс в pwrmodes - моность при работе автотюнера или по внешнему запросу
	#elif WITHPOWERTRIM
		uint8_t gnormalpower;/* мощность WITHPOWERTRIMMIN..WITHPOWERTRIMMAX */
		uint8_t gclassamode;	/* использование режима клвсс А при передаче */
		uint8_t gtunepower;/* мощность при работе автоматического согласующего устройства WITHPOWERTRIMMIN..WITHPOWERTRIMMAX */
	#endif /* WITHPOWERLPHP, WITHPOWERTRIM */
#endif /* WITHTX */

#if WITHNOTCHONOFF
	uint16_t	ggrpnotch; // последний посещённый пункт группы
	uint8_t gnotch;
#elif WITHNOTCHFREQ
	uint16_t	ggrpnotch; // последний посещённый пункт группы
	uint8_t gnotch;		// on/off - кнопкой, не через меню
	uint8_t gnotchtype;
	uint16_t gnotchfreq;
	uint16_t gnotchwidth;
#endif /* WITHNOTCHONOFF, WITHNOTCHFREQ */

#if WITHRFSG
	uint8_t userfsg;
#endif /* WITHRFSG */

	uint8_t gdisplayfreqsfps;		/* скорость обновления индикатора частоты */
	uint8_t glatchfps;	/* скорость обновления S-метра */
#if WITHSPECTRUMWF
	uint8_t gviewstyle;		/* стиль отображения спектра и панорамы */
	uint8_t gview3dss_mark;	/* Для VIEW_3DSS - индикация полосы пропускания на спектре */
	uint8_t gtxloopback;		 /* включение спектроанализатора сигнала передачи */
	uint8_t gspecbeta100;	/* beta - парамеры видеофильтра спектра */
	uint8_t gwflbeta100;	/* beta - парамеры видеофильтра водопада */
	uint8_t glvlgridstep;	/* Шаг сетки уровней в децибелах */
	uint8_t grxbwsatu;		/* 0..100 - насыщнность цвета заполнения "шторки" - индикатор полосы пропускания примника на спкктре. */
	uint8_t gspectrumpart;	/* Часть отведенной под спектр высоты экрана 0..100 */
#endif /* WITHSPECTRUMWF */
#if WITHHDMITVHW
	uint8_t ghdmiformat;	/* Видеорежим внешнего HDMI монитора */
#endif /* WITHHDMITVHW */

	uint8_t gshowdbm;	/* Отображение уровня сигнала в dBm или S-memter */
#if WITHBCBANDS
	uint8_t gbandsetbcast;	/* Broadcasting radio bands */
#endif /* WITHBCBANDS */
	uint8_t bandset11m;	/* CB radio band */
#if TUNE_6MBAND
	uint8_t bandset6m;	/* используется ли диапазон 6 метров */
#endif /* TUNE_6MBAND */
#if TUNE_4MBAND
	uint8_t bandset4m;	/* используется ли диапазон 6 метров */
#endif /* TUNE_4MBAND */
#if TUNE_2MBAND
	uint8_t bandset2m;	/* используется ли диапазон 2 метра */
#endif /* TUNE_2MBAND */

#if WITHANTSELECT2
	uint8_t gantmanual;		/* 0 - выбор антенны автоматический */
#endif /* WITHANTSELECT2 */
#if WITHSPLIT
	uint8_t splitmode;		/* не-0, если работа с фиксированными ячейками (vfo/vfoa/vfob/mem) */
	uint8_t vfoab;		/* 1, если работа с VFO B, 0 - с VFO A */
#elif WITHSPLITEX
	uint8_t splitmode;		/* не-0, если работа с фиксированными ячейками (vfo/vfoa/vfob/mem) */
	uint8_t vfoab;		/* 1, если работа с VFO B, 0 - с VFO A */
#endif /* WITHSPLIT */
#if WITHRPTOFFSET
	uint8_t rptrhfenable;		/* Repeater offset HF */
	uint16_t rptroffshf1k;		/* Repeater offset HF */
	uint8_t rptrvhfenable;		/* Repeater offset VHF */
	uint16_t rptroffsvhf1k;		/* Repeater offset VHF */
#endif /* WITHRPTOFFSET */
	uint8_t gcwpitch10;	/* тон в CW/CWR режиме */
	uint8_t gkeybeep10;	/* тон озвучки нажатий клавиш */
	uint8_t stayfreq;	/* при изменении режимов кнопками - не меняем частоту */

#if  WITHUSBHW && (WITHUSBUACOUT || WITHUSBUACIN || WITHUSEUSBBT)
	uint16_t	ggrpusb; // последний посещённый пункт группы
#endif

#if WITHIF4DSP
	uint16_t	ggrpagc; // последний посещённый пункт группы
	uint16_t	ggrpagcssb; // последний посещённый пункт группы
	uint16_t	ggrpagccw; // последний посещённый пункт группы
	uint16_t	ggrpagcdigi; // последний посещённый пункт группы

	uint8_t gnoisereductvl;	// noise reduction level
	uint8_t bwsetpos [BWSETI_count];	/* выбор одной из полос пропускания */

	uint8_t bwpropsleft [BWPROPI_count];	/* значения границ полосы пропускания */
	uint8_t bwpropsright [BWPROPI_count];	/* значения границ полосы пропускания */
	uint8_t bwpropsfltsofter [BWPROPI_count];	/* Код управления сглаживанием скатов фильтра основной селекции на приёме */
	uint8_t bwpropsafresponce [BWPROPI_count];	/* Наклон АЧХ */

	struct agcseti_tag afsets [AGCSETI_COUNT];	/* режимы приема */

	uint8_t gagcoff;
	uint8_t gamdepth;		/* Глубина модуляции в АМ - 0..100% */
	uint16_t gtxtot;			/* разрешённое время передачи */
	uint8_t ggainnfmrx10;		/* дополнительное усиление по НЧ в режиме приёма NFM 100..1000% */
	uint8_t gdacscale;		/* Использование амплитуды сигнала с ЦАП передатчика - 0..100% */
	uint16_t ggaindigitx;		/* Увеличение усиления при передаче в цифровых режимах 100..300% */
	uint16_t ggaincwtx;		/* Увеличение усиления при передаче в CW режимах 30..150% */
	uint16_t gdesignscale;	/* используется при калибровке параметров интерполятора */
#if WITHELKEY
	uint8_t	gcwedgetime;			/* Время нарастания/спада огибающей телеграфа при передаче - в 1 мс */
	uint8_t gcwssbtx;		/* разрешение самопрослушивания */
#endif /* WITHELKEY */
	uint8_t	gsidetonelevel;	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
	uint8_t gmoniflag;		/* разрешение самопрослушивания */
	uint8_t	gsubtonelevel;	/* Уровень сигнала CTCSS в процентах - 0%..100% */
#if WITHWAVPLAYER || WITHSENDWAV
	uint8_t gloopmsg, gloopsec;
#endif /* WITHWAVPLAYER || WITHSENDWAV */
	uint8_t gdigigainmax;	/* диапазон ручной регулировки цифрового усиления - максимальное значение */
	uint8_t gsquelch;		/* уровень открытия шумоподавителя */
	uint8_t gsquelchNFM;	/* sуровень открытия шумоподавителя для NFM */
	uint16_t gfsadcpower10 [2];	/*	Мощность, соответствующая full scale от IF ADC (с тояностью 0.1 дБмВт */
	#if ! WITHPOTAFGAIN
		uint16_t afgain1;	// Параметр для регулировки уровня на выходе аудио-ЦАП
	#endif /* ! WITHPOTAFGAIN */
	#if ! WITHPOTIFGAIN
		uint16_t rfgain1;	// Параметр для регулировки усиления по ПЧ
	#endif /* ! WITHPOTIFGAIN */
	uint16_t glineamp;	// усиление с LINE IN
	uint8_t gmikeboost20db;	// предусилитель микрофона
	uint8_t gmikeagc;	/* Включение программной АРУ перед модулятором */
	uint8_t gmikeagcgain;	/* Максимальное усидение АРУ микрофона */
	uint8_t gmikehclip;		/* Ограничитель */
	#if WITHCOMPRESSOR
		uint8_t	gcompressor_attack;		/* Компрессор - время атаки */
		uint8_t gcompressor_release;	/* Компрессор - время освобождения */
		uint8_t gcompressor_hold;		/* Компрессор - время удержания */
		uint8_t gcompressor_gain;		/* Компрессор - отношение компрессии */
		uint8_t gcompressor_threshold;	/* Компрессор - порог срабатывания */
	#endif /* WITHCOMPRESSOR */
	#if WITHREVERB
		uint8_t greverb;		/* ревербератор */
		uint8_t greverbdelay;		/* ревербератор - задержка */
		uint8_t greverbloss;		/* ревербератор - ослабление на возврате */
	#endif /* WITHREVERB */
	#if WITHUSBHW && WITHUSBUAC
		uint8_t gdatatx;	/* автоматическое изменение источника при появлении звука со стороны компьютера */
		uint8_t gdatamode;	/* передача звука с USB вместо обычного источника */
		uint8_t guacplayer;	/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
		uint8_t gusb_hs;	/* Использование USB HS dvtcn USB FS */
		uint8_t gbtaudioplayer;	/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
		#if WITHRTS96 || WITHRTS192
			uint8_t gswapiq;		/* Поменять местами I и Q сэмплы в потоке RTS96 */
		#endif /* WITHRTS96 || WITHRTS192 */
		uint8_t	gusb_ft8cn;	/* совместимость VID/PID для работы с программой FT8CN */
	#endif /* WITHUSBHW && WITHUSBUAC */
	#if WITHAFCODEC1HAVEPROC
		uint8_t gmikeequalizer;	// включение обработки сигнала с микрофона (эффекты, эквалайзер, ...)
		uint8_t gmikeequalizerparams [HARDWARE_CODEC1_NPROCPARAMS];	// Эквалайзер 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
	#endif /* WITHAFCODEC1HAVEPROC */
	#if WITHAFEQUALIZER
		uint16_t	ggrpafeq;	// последний посещённый пункт группы
		uint8_t geqtx;	// эквалайзер в режиме передачи
		uint8_t geqrx;	// эквалайзер в режиме приема
		uint8_t geqtxparams [AF_EQUALIZER_BANDS];
		uint8_t geqrxparams [AF_EQUALIZER_BANDS];
	#endif /* #if WITHAFEQUALIZER */
	struct micproc gmicprocs [NMICPROFILES];
	uint8_t txaprofile [TXAPROFIG_count];	/* параметры обработки звука перед модулятором */
#endif /* WITHIF4DSP */

#if WITHDSPEXTDDC	/* "Воронёнок" с DSP и FPGA */
	uint16_t	ggrprfadc; // последний посещённый пункт группы
	uint8_t gdither;	/* управление зашумлением в LTC2208 */
	uint8_t gdactest;
	uint8_t gshowovf;				/* Показ индикатора переполнения АЦП */
#endif /* WITHDSPEXTDDC */

#if WITHMODEM
	uint16_t	ggrpmodem; // последний посещённый пункт группы
	uint8_t gmodemspeed;	// индекс в таблице скоростей передачи
	uint8_t gmodemmode;		// применяемая модуляция
#endif /* WITHMODEM */

#if WITHLO1LEVELADJ
	uint8_t lo1level;	/* уровень (амплитуда) LO1 в процентах */
#endif /* WITHLO1LEVELADJ */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_DCRX)

#elif WITHDUALFLTR		/* Переворот боковых за счёт переключения фильтра верхней или нижней боковой полосы */
	uint16_t lo4offset;			/* частота (без базы) третьего гетеродина */
#elif WITHFIXEDBFO
	uint8_t glo4lsb;			/* признак LSB на последней ПЧ */
	uint16_t lo4offset;			/* частота (без базы) третьего гетеродина */
#elif WITHDUALBFO
	uint16_t lo4offsets [2];		/* частота (без базы) третьего гетеродина в USB и LSB */
#endif

#if WITHIFSHIFT
	uint16_t ifshifoffset;	/* смещение частоты пропускания в режиме If SHIFT */
#endif /* WITHIFSHIFT */

	uint16_t	ggrpfilters; // последний посещённый пункт группы

#if (defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX) && (IF3_MODEL != IF3_TYPE_BYPASS))

	uint8_t dcrxmode;	/* settings menu option - RX acts as direct conversion */
	uint8_t dctxmodessb, dctxmodecw;	/* settings menu option - TX acts as direct conversion */


#if WITHPBT //&& (LO3_SIDE != LOCODE_INVALID)
	uint16_t	ggrppbts; // последний посещённый пункт группы
	uint16_t pbtoffset;	/* смещение частоты пропускания в режиме PBT */
#endif /* WITHPBT && (LO3_SIDE != LOCODE_INVALID) */

	/* параметры подстройки центральной частоты узких фильтров */
#if (IF3_FMASK & IF3_FMASK_0P3)
	uint8_t hascw0p3;			/* наличие в плате фильтра на 0.3 кГц */
	uint16_t carr0p3;	/* settings menu option */
#endif
#if (IF3_FMASK & IF3_FMASK_0P5)
	uint8_t hascw0p5;			/* наличие в плате фильтра на 0.5 кГц */
	uint16_t carr0p5;	/* settings menu option */
#endif

	/* параметры подстройки частот скатов широких фильтров */
#if (IF3_FMASK & IF3_FMASK_1P8)
	uint8_t hascw1p8;			/* наличие в плате фильтра на 1.8 кГц */
	uint16_t usbe1p8;	/* settings menu option */
	uint16_t lsbe1p8;	/* settings menu option */
#endif
#if (IF3_FMASK & IF3_FMASK_2P4)
	uint8_t hascw2p4;			/* наличие в плате фильтра на 2.4 кГц */
	uint16_t usbe2p4;	/* settings menu option */
	uint16_t lsbe2p4;	/* settings menu option */
	#if WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P4) != 0
		uint16_t usbe2p4tx;	/* settings menu option */
		uint16_t lsbe2p4tx;	/* settings menu option */
	#endif
#endif
#if (IF3_FMASK & IF3_FMASK_2P7)
	uint8_t hascw2p7;			/* наличие в плате фильтра на 2.7 кГц */
	uint16_t usbe2p7;	/* settings menu option */
	uint16_t lsbe2p7;	/* settings menu option */
	#if WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) != 0
		uint16_t usbe2p7tx;	/* settings menu option */
		uint16_t lsbe2p7tx;	/* settings menu option */
	#endif
#endif
#if (IF3_FMASK & IF3_FMASK_3P1)
	uint16_t usbe3p1;	/* settings menu option */
	uint16_t lsbe3p1;	/* settings menu option */
	#if WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_3P1) != 0
		uint16_t usbe3p1tx;	/* settings menu option */
		uint16_t lsbe3p1tx;	/* settings menu option */
	#endif
#endif

	/* параметры подстройки центральной частоты широких фильтров */
#if (IF3_FMASK & IF3_FMASK_6P0)
	uint8_t hascw6p0;			/* наличие в плате фильтра на 6.0 кГц */
	uint16_t cfreq6k;
#endif
#if (IF3_FMASK & IF3_FMASK_7P8)
	uint16_t cfreq7p8k;
#endif
#if (IF3_FMASK & IF3_FMASK_8P0)
	uint16_t cfreq8k;
#endif
#if (IF3_FMASK & IF3_FMASK_9P0)
	uint16_t cfreq9k;
#endif
#if (IF3_FMASK & IF3_FMASK_15P0)
	uint16_t cfreq15k;
	uint16_t cfreq15k_nfm;
#endif
#if (IF3_FMASK & IF3_FMASK_17P0)
	uint16_t cfreq17k;
#endif
#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P4)
	uint8_t hascw2p4_tx;
#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P4) */
#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7)
	uint8_t hascw2p7_tx;
#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) */
#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_3P1)
	uint8_t hascw3p1_tx;
#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) */

#endif

#if defined(REFERENCE_FREQ)
#if defined (DAC1_TYPE)
	uint8_t dac1level;	/* напряжение на управлении опорным генератором */
#endif /* defined (DAC1_TYPE) */
	uint16_t refbias;	/* подстройка опорника */

#endif /* defined(REFERENCE_FREQ) */
#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
	uint16_t si570_xtall_offset;
#endif


#if WITHCAT
	uint16_t	ggrpcat; // последний посещённый пункт группы
	uint8_t catenable;	/* удаленное управление разрешено */
	uint8_t catbaudrate;	/* номер скорости работы по CAT */
	#if WITHTX
		uint8_t catsigptt;	/* Выбраный сигнал для перехода на передачу по CAT */
	#endif /* WITHTX */
	uint8_t catsigkey;	/* Выбраный сигнал для манипуляции по CAT */
#if WITHCAT_MUX
	uint8_t gcatmux;	/* удаленное управление разрешено */
#endif /* WITHCAT_MUX */
#endif /* WITHCAT */

#if WITHAUTOTUNER
	uint16_t	ggrptuner; // последний посещённый пункт группы
	uint8_t gtunerdelay;
#if WITHAUTOTUNER_N7DDCALGO
	uint8_t gn7ddclinearC;
	uint8_t gn7ddclinearL;
#endif /* WITHAUTOTUNER_N7DDCALGO */
#endif /* WITHAUTOTUNER */

#if WITHTX
	#if WITHMUTEALL
		uint8_t gmuteall;	/* Отключить микрофон всегда. */
	#endif /* WITHMUTEALL */
	#if WITHVOX
		uint16_t	ggrpvox; // последний посещённый пункт группы
		uint8_t gvoxenable;	/* автоматическое управление передатчиком (от голоса) */
		uint8_t gvoxlevel;	/* уровень срабатывания VOX */
		uint8_t gavoxlevel;	/* уровень anti-VOX */
		uint8_t voxdelay;	/* задержка отпускания VOX */
	#endif /* WITHVOX */

	#if WITHELKEY
		uint8_t gbkinenable;	/* автоматическое управление передатчиком (от телеграфного манипулятора) */
	#endif /* WITHELKEY */

	uint8_t gtxgate;	/* разрешение предусилителя */
	uint8_t bkindelay;	/* задержка отпускания BREAK-IN */
	uint8_t grgbeep;	/* разрешение (не-0) или запрещение (0) формирования roger beep */
	uint8_t rxtxdelay;	/* приём-передача */

	uint8_t txrxdelay;	/* передача-приём */


	uint8_t swrcalibr;	/* калибровочный параметр SWR-метра */
	uint8_t maxpwrcali;	/* калибровочный параметр PWR-метра */
	uint16_t minforward; // = 10;
	#if WITHSWRMTR && ! WITHSHOWSWRPWR
		uint8_t swrmode;	/* 1 - показ SWR шкалы, 0 - мощность */
	#endif

#if (WITHTHERMOLEVEL || WITHTHERMOLEVEL2)
	uint8_t gtempvmax;
	uint8_t gheatprot;	/* защита от перегрева */
#endif /* (WITHTHERMOLEVEL || WITHTHERMOLEVEL2) */
#if (WITHSWRMTR || WITHSHOWSWRPWR)
	uint8_t gswrprot;	/* защита от превышения КСВ */
#endif /* (WITHSWRMTR || WITHSHOWSWRPWR) */
	uint8_t gdownatcwtune;	/* снижаем мощность до "тюнерной" при нажатии TUNE */
#endif /* WITHTX */
#if WITHSUBTONES
	uint8_t gsubtoneitx;	// номер subtone
	uint8_t gsubtoneirx;	// номер subtone
#endif /* WITHSUBTONES */

#if WITHVOLTLEVEL && ! WITHREFSENSOR
	uint8_t voltcalibr100mV;	/* калибровочный параметр измерителя напряжения АКБ - Напряжение fullscale = VREF * 5.3 = 3.3 * 5.3 = 17.5 вольта */
#endif /* WITHVOLTLEVEL && ! WITHREFSENSOR */
	uint16_t gipacali;
#if WITHELKEY
	uint16_t	ggrpelkey; // последний посещённый пункт группы
	uint8_t elkeywpm;	/* скорость электронного ключа */
	uint8_t elkeymode;	/* режим электронного ключа - 0 - asf, 1 - paddle, 2 - keyer */
	uint8_t dashratio;	/* отношение длителности тире к точке в десятках процентов */
	uint8_t spaceratio;	/* отношение длителности паузы к точке в десятках процентов */
	uint8_t elkeyreverse;
#if WITHVIBROPLEX
	uint8_t elkeyslope;	/* скорость уменьшения длительности точки и паузы - имитация виброплекса */
#endif /* WITHVIBROPLEX */
#endif /* WITHELKEY */


	uint8_t gbigstep;		/* больщой шаг валкодера */
	uint8_t genc1div;		/* во сколько раз уменьшаем разрешение валкодера. */
	uint8_t genc2div;
#if WITHSPKMUTE
	uint8_t gmutespkr;		/* выключение динамика */
#endif /* WITHSPKMUTE */

#if LO1FDIV_ADJ
	uint8_t lo1powrx;		/* на сколько раз по 2 делим выходную частоту синтезатора первого гетеродина */
	uint8_t lo1powtx;		/* на сколько раз по 2 делим выходную частоту синтезатора первого гетеродина */
#endif /* LO1FDIV_ADJ */
#if LO4FDIV_ADJ
	uint8_t lo4powrx;		/* на сколько раз по 2 делим выходную частоту синтезатора четивертого гетеродина */
	uint8_t lo4powtx;		/* на сколько раз по 2 делим выходную частоту синтезатора четивертого гетеродина */
#endif /* LO1FDIV_ADJ */

	uint8_t gattpresh;	/* корректировка показаний с-метра по включенному аттенюатору и предусилителю */

#if (WITHSWRMTR || WITHSHOWSWRPWR)
	uint8_t gsmetertype;		/* выбор внешнего вида прибора - стрелочный или градусник */
#endif /* (WITHSWRMTR || WITHSHOWSWRPWR) */

#if LO1PHASES
	uint16_t phaserx, phasetx;
#endif /* LO1PHASES */

#if WITHLFM
	uint16_t	ggrplfm; // последний посещённый пункт группы
	uint16_t lfmtoffset;
	uint16_t lfmtinterval;
	uint8_t lfmmode;
	uint16_t lfmstart100k;
	uint16_t lfmstop100k;
	uint16_t lfmspeed1k;
	uint16_t lfmfreqbias;
#endif /* WITHLFM */

#if WITHUSEAUDIOREC
	uint8_t recmode;	/* автоматически начинаем запись на SD CARD при включении */
#endif /* WITHUSEAUDIOREC */

#if (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ	/* подстройка частоты гетеродина через меню. */
	uint16_t lo3offset;
#endif	/* (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ */

	struct modeprops modes [MODE_COUNT];

	struct bandinfo bands [HBANDS_COUNT + XBANDS_COUNT + VFOS_COUNT + MBANDS_COUNT];
	struct bandgroup_tag bandgroups [BANDGROUP_COUNT + 1];	/* один элемент для не относящихся к группам диапазонов */

#if WITHANTSELECT2
	uint8_t hffreqswitch; /* выше этой частоты (МГц) выбирается вторая (ВЧ) антенна */
#endif /* WITHANTSELECT2 */

#if WITHTOUCHGUI
	struct gui_nvram_t gui_nvram;
	struct micprof_cell micprof_cells [NMICPROFCELLS];	/* ячейки памяти профилей микрофона */
#endif /*  WITHTOUCHGUI */

#if 1//WITHDEBUG
	uint8_t gforcexvrtr;	/* принудительно включить коммутацию трансвертора */
#endif /* WITHDEBUG */

	uint8_t signature [sizeof nvramsign - 1];	/* сигнатура соответствия версии программы и содержимого NVRAM */
} ATTRPACKED;	// аттрибут GCC, исключает "дыры" в структуре. Так как в ОЗУ нет копии этой структуры, see also NVRAM_TYPE_BKPSRAM

/* константы, определяющие расположение параметров в FRAM */

#define RMT_MENUSET_BASE OFFSETOF(struct nvmap, gmenuset)		/* набор функций кнопок и режим отображения на дисплее */
#define RMT_GROUP_BASE OFFSETOF(struct nvmap, ggroup)		/* байт - последняя группа меню, с которой работали */
#define RMT_SIGNATURE_BASE(i) OFFSETOF(struct nvmap, signature [(i)])			/* расположение сигнатуры */
#define RMT_LOCKMODE_BASE(b) OFFSETOF(struct nvmap, bands [(b)].glock)		/* признак блокировки валкодера */
#define RMT_USEFAST_BASE OFFSETOF(struct nvmap, gusefast)		/* переключение в режим крупного шага */

#define RMT_SPLITMODE_BASE OFFSETOF(struct nvmap, splitmode)		/* (vfo/vfoa/vfob/mem) */
#define RMT_VFOAB_BASE OFFSETOF(struct nvmap, vfoab)		/* (vfoa/vfob) */
#define RMT_MBAND_BASE OFFSETOF(struct nvmap, gmband)		/* ячейка памяти фиксированных частот */
#define RMT_ANTMANUAL_BASE OFFSETOF(struct nvmap, gantmanual)		/* 0 - выбор антенны автоматический */

#define RMT_MAINSUBRXMODE_BASE	OFFSETOF(struct nvmap, mainsubrxmode)
#define RMT_DATAMODE_BASE	OFFSETOF(struct nvmap, gdatamode)


#define RMT_NR_BASE(i)	OFFSETOF(struct nvmap, modes [(i)].noisereduct)
#define RMT_AGC_BASE(i)	OFFSETOF(struct nvmap, modes [(i)].agc)
#define RMT_FILTER_BASE(i)	OFFSETOF(struct nvmap, modes [(i)].filter)
#define RMT_STEP_BASE(i)	OFFSETOF(struct nvmap, modes [(i)].step)

#define RMT_TXPOWER_BASE(i)	OFFSETOF(struct nvmap, modes [(i)].txpower)
#define RMT_TXCOMPR_BASE(i)	OFFSETOF(struct nvmap, modes [(i)].txcompr)
#define RMT_TXAUDIOINDEX_BASE(i) OFFSETOF(struct nvmap, modes [(i)].txaudioindex)
#define RMT_MIDDLEMENUPOS_BASE(i) OFFSETOF(struct nvmap, modes [(i)].gmidmenupos)
#define RMT_TXAPROFIGLE_BASE(i) OFFSETOF(struct nvmap, txaprofile[(i)])

#define RMT_BFREQ_BASE(b) OFFSETOF(struct nvmap, bands [(b)].freq)			/* последняя частота, на которую настроились (4 байта) */

#define RMT_BANDPOS(bg) OFFSETOF(struct nvmap, bandgroups [(bg)].band)	/* последний диапазон в группе, куда был переход по кнопке диапазона (индекс в bands). */
#define RMT_PAMPBG3_BASE(bg, ant, rxant) OFFSETOF(struct nvmap, bandgroups [(bg)].orxants [(rxant) ? ANTMODE_COUNT : (ant)].pamp)	/* признак включения аттенюатора (1 байт) */
#define RMT_ATTBG3_BASE(bg, ant, rxant) OFFSETOF(struct nvmap, bandgroups [(bg)].orxants [(rxant) ? ANTMODE_COUNT : (ant)].att)		/* признак включения аттенюатора (1 байт) */
#define RMT_RXANTENNABG_BASE(bg) OFFSETOF(struct nvmap, bandgroups [(bg)].rxant)			/* код включённой антенны (1 байт) */
#define RMT_ANTENNABG_BASE(bg) OFFSETOF(struct nvmap, bandgroups [(bg)].ant)			/* код включённой антенны (1 байт) */
#define RMT_MODEROW_BASE(b)	OFFSETOF(struct nvmap, bands [(b)].moderow)			/* номер строки в массиве режимов. */
#define RMT_MODECOLS_BASE(b, j)	OFFSETOF(struct nvmap, bands [(b)].modecols [(j)])	/* выбранный столбец в каждой строке режимов. */
#define RMT_PWR_BASE OFFSETOF(struct nvmap, gpwri)								/* большая мощность sw2012sf */
#define RMT_NOTCH_BASE OFFSETOF(struct nvmap, gnotch)							/* NOTCH on/off */
#define RMT_NOTCHTYPE_BASE OFFSETOF(struct nvmap, gnotchtype)					/* NOTCH filter type */
//#define RMT_NOTCHFREQ_BASE OFFSETOF(struct nvmap, gnotchfreq)					/* Manual NOTCH filter frequency */

#define RMT_BWSETPOS_BASE(i) OFFSETOF(struct nvmap, bwsetpos [(i)])

#define RMT_BWPROPSLEFT_BASE(i) OFFSETOF(struct nvmap, bwpropsleft [(i)])
#define RMT_BWPROPSRIGHT_BASE(i) OFFSETOF(struct nvmap, bwpropsright [(i)])
#define RMT_BWPROPSFLTSOFTER_BASE(i) OFFSETOF(struct nvmap, bwpropsfltsofter [(i)])
#define RMT_BWPROPSAFRESPONCE_BASE(i) OFFSETOF(struct nvmap, bwpropsafresponce [(i)])

#define RMT_MICLEVEL_BASE(c) OFFSETOF(struct nvmap, micprof_cells [(c)].level)
#define RMT_MICCLIP_BASE(c) OFFSETOF(struct nvmap, micprof_cells [(c)].clip)
#define RMT_MICAGC_BASE(c) OFFSETOF(struct nvmap, micprof_cells [(c)].agc)
#define RMT_MICAGCGAIN_BASE(c) OFFSETOF(struct nvmap, micprof_cells [(c)].agcgain)
#if WITHAFCODEC1HAVEPROC
	#define RMT_MICBOOST_BASE(c) OFFSETOF(struct nvmap, micprof_cells [(c)].mikeboost20db)
	#define RMT_MICEQ_BASE(c) OFFSETOF(struct nvmap, micprof_cells [(c)].eq_enable)
	#define RMT_MICEQPARAMS_BASE(c, j) OFFSETOF(struct nvmap, micprof_cells [(c)].eq_params[(j)])
#endif /* WITHAFCODEC1HAVEPROC */
#define RMT_MICPSAVE_BASE(c) OFFSETOF(struct nvmap, micprof_cells [(c)].cell_saved)

#if WITHUSEUSBBT

unsigned nvram_tlv_getparam(unsigned * base)
{
	* base = OFFSETOF(struct nvmap, tlvbt);
	return TLVBT_SIZE;
}
#endif /* WITHUSEUSBBT */


/* переменные, вынесенные из главной функции - определяют текущий тежим рботы
   и частоту настройки
   */

/* параметры диапазона, переключаемые при смене VFO */
static uint_fast32_t gfreqs [VFOS_COUNT] = { DEFAULTDIALFREQ, DEFAULTDIALFREQ };		/* отображаемая на дисплее частота работы */
#if ! WITHONEATTONEAMP
static uint_fast8_t gpamp;
#endif /* ! WITHONEATTONEAMP */
static uint_fast8_t gatt;
#if WITHANTSELECTRX || WITHANTSELECT1RX
static uint_fast8_t grxantenna;
static uint_fast8_t gantenna;
#elif WITHANTSELECT2
static uint_fast8_t gantennabym;	/* manualy selected antenna */
static uint_fast8_t hffreqswitch = 14; /* выше этой частоты (МГц) выбирается вторая (ВЧ) антенна */
static uint_fast8_t gantmanual;		/* 0 - выбор антенны автоматический */
enum { grxantenna = 0 };
#elif WITHANTSELECT
static uint_fast8_t gantenna;
enum { grxantenna = 0 };
#else
enum { gantenna = 0 };
enum { grxantenna = 0 };
#endif /* WITHANTSELECT || WITHANTSELECTRX */
static uint_fast8_t gvfosplit [VFOS_COUNT];	// At index 0: RX VFO A or B, at index 1: TX VFO A or B
// Параметры, выставляемые в update board
// кэш установленных параметров.
// На эти параметры ориентируемся при работе кнопками управления, переклбчения фильттров и так далее.
static uint_fast8_t gsubmode;		/* код текущего режима */
static uint_fast8_t gmode;		/* текущий код группы режимов */
static uint_fast8_t gfi;			/* номер фильтра (сквозной) для текущего режима */

static unsigned nvramoffs_selector(unsigned * count)
{
	const uint_fast8_t mode = gmode;
	* count = MODE_COUNT;
	return mode;
}

static nvramaddress_t nvramoffs_mode(nvramaddress_t base, unsigned mode)
{
	ASSERT(base != MENUNONVRAM);

	if (base == MENUNONVRAM)
		return MENUNONVRAM;

	//
	// для диапазонов - вычисляем шаг увеличения индекса по массиву хранения в диапазонах
	return base + RMT_FILTER_BASE(mode) - RMT_FILTER_BASE(mode);
}

static ptrdiff_t valueoffs_mode(unsigned mode)
{
	return mode;
}

#if ! WITHAGCMODENONE
static uint_fast8_t gagcmode;
#endif /* ! WITHAGCMODENONE */


#if WITHIF4DSP
static uint_fast8_t gnoisereducts [MODE_COUNT];	// noise reduction level
static uint_fast8_t gnoisereductvl = 25;	// noise reduction

static const struct paramdefdef xgnoisereduct =
{
	QLABEL3("NR", "NR", "NR"), 8, 3, RJ_ON,	ISTEP1,		/* управление режимом NOTCH */
	ITEM_VALUE,
	0, 1,
	RMT_NR_BASE(0),							/* управление режимом NOTCH */
	nvramoffs_selector, nvramoffs_mode, valueoffs_mode,
	NULL,	// uint_fast16_t value pointer
	& gnoisereducts [0],	// uint_fast8_t value pointer
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif /* WITHIF4DSP */
#if WITHUSEDUALWATCH


static size_t getvaltextmaisubrxmode(char * buff, size_t count, int_fast32_t value)
{
	ASSERT(value >= 0 && value < (int) MAINSUBRXMODE_COUNT);
	return local_snprintf_P(buff, count, "%s", mainsubrxmodes [value].label);
}

// Левый/правый, A - main RX, B - sub RX
static const struct paramdefdef xmainsubrxmode =
{
	QLABEL3("DUAL", "Dual RX", "DUAL"), 7, 0, RJ_CB,	ISTEP1,
	ITEM_VALUE | ITEM_LISTSELECT,
	0, MAINSUBRXMODE_COUNT - 1,
	OFFSETOF(struct nvmap, mainsubrxmode),
	getselector0, nvramoffs0, valueoffs0,
	NULL,	// uint_fast16_t value pointer
	& mainsubrxmode,	// uint_fast8_t value pointer
	getzerobase, /* складывается со смещением и отображается */
	getvaltextmaisubrxmode, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif /* WITHUSEDUALWATCH */

#if WITHIF4DSP

static dualctl8_t gsquelch = { 0, 0 };	/* squelch level */
static const struct paramdefdef xgsquelch =
{
	QLABEL3("SQUELCH", "Squelech", "SQUELCH"), 7, 0, RJ_UNSIGNED, ISTEP1,		/* squelch level */
	ITEM_VALUE,
	0, SQUELCHMAX,
	OFFSETOF(struct nvmap, gsquelch),	/* уровень сигнала болше которого открывается шумодав */
	getselector0, nvramoffs0, valueoffs0,
	NULL,	// uint_fast16_t value pointer
	& gsquelch.value,	// uint_fast8_t value pointer
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};

static uint_fast8_t gsquelchNFM;	/* squelch level for NFM */
static const struct paramdefdef xgsquelchNFM =
{
	QLABEL3("SQLC NFM", "Squelech NFM", "SQUELCHFM"), 0, 0, RJ_UNSIGNED, ISTEP1,		/* squelch level for NFM */
	ITEM_VALUE,
	0, SQUELCHMAX,
	OFFSETOF(struct nvmap, gsquelchNFM),	/* уровень сигнала болше которого открывается шумодав */
	getselector0, nvramoffs0, valueoffs0,
	NULL,	// uint_fast16_t value pointer
	& gsquelchNFM,	// uint_fast8_t value pointer
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};

static uint_fast8_t ggainnfmrx10 = 30;	/* дополнительное усиление по НЧ в режиме приёма NFM 100..1000% */
static const struct paramdefdef xggainnfmrx10 =
{
	QLABEL3("NFM GAIN", "NFM Gain", "NFM GAIN"), 7, 1, RJ_UNSIGNED, ISTEP1,		/* дополнительное усиление по НЧ в режиме приёма NFM 100..1000% */
	ITEM_VALUE,
	10, 100,
	OFFSETOF(struct nvmap, ggainnfmrx10),	/* дополнительное усиление по НЧ в режиме приёма NFM 100..1000% */
	getselector0, nvramoffs0, valueoffs0,
	NULL,	// uint_fast16_t value pointer
	& ggainnfmrx10,	// uint_fast8_t value pointer
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif /* WITHIF4DSP */

#if WITHIFSHIFT
	enum { IFSHIFTTMIN = 0, IFSHIFTHALF = 3000, IFSHIFTMAX = 2 * IFSHIFTHALF };
	#if WITHIFSHIFTOFFSET
		/* есть начальный сдвиг полосы пропускания */
		static dualctl16_t ifshifoffset =
		{
				IFSHIFTHALF + WITHIFSHIFTOFFSET,		/* if shift offset value */
				IFSHIFTHALF + WITHIFSHIFTOFFSET,		/* if shift offset value */
		};
	#else
		/* Без начального сдвига полосы пропускания */
		static dualctl16_t ifshifoffset =
		{
				IFSHIFTHALF,		/* if shift offset value */
				IFSHIFTHALF,		/* if shift offset value */
		};
	#endif
	static int_fast32_t getifshiftbase(void)
	{
		return 0 - IFSHIFTHALF;
	}
#endif /* WITHIFSHIFT */

#if WITHPBT // && (LO3_SIDE != LOCODE_INVALID)
	enum { PBTMIN = 0, PBTHALF = 2560, PBTMAX = 5100 };	// Значения для управления потенциометром
	static dualctl16_t gpbtoffset =
	{
			PBTHALF;		/* pbt offset value */
			PBTHALF;		/* pbt offset value */
	};
	static int_fast32_t getpbtbase(void)
	{
		return 0L - PBTHALF;
	}
	// Для отображения на дисплее
	int_fast32_t hamradio_get_pbtvalue(void)
	{
		return gpbtoffset.value + getpbtbase();
	}
#endif /* WITHPBT */

#if (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ	/* подстройка частоты гетеродина через меню. */
	enum { LO2AMIN = 0, LO2AHALF = 15000, LO2AMAX = 2 * LO2AHALF };
	static uint_fast16_t lo3offset = LO2AHALF;
	static int_fast32_t lo3base = (int_fast32_t) ((int_fast64_t) REFERENCE_FREQ * LO3_PLL_N / LO3_PLL_R) - LO2AHALF;
	static int_fast32_t getlo3base(void)
	{
		return lo3base;
	}
#endif	/* (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ */

static const struct paramdefdef xgcwpitch10 =
{
	QLABEL3("CW PITCH", "CW Pitch", "CW PITCH"), 7, 2, RJ_UNSIGNED, 	ISTEP1,
	ITEM_VALUE,
	CWPITCHMIN10, CWPITCHMAX10,	// 40, 190,			/* 400 Hz..1900, Hz in 10 Hz steps */
	OFFSETOF(struct nvmap, gcwpitch10),
	getselector0, nvramoffs0, valueoffs0,
	NULL,	// uint_fast16_t value pointer
	& gcwpitch10,	// uint_fast8_t value pointer
	getzerobase,
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};

#if WITHENCODER
	static uint_fast8_t gbigstep = (ENCRES_24 >= ENCRES_DEFAULT);	/* модифицируется через меню. */
	static uint_fast8_t genc1pulses = ENCRES_DEFAULT;		/* 5: 128 индекс в таблице разрешений валкодера */

#else
	static const uint_fast8_t gbigstep = 0;

#endif

#if WITHOVFHIDE
	static uint_fast8_t gshowovf = 0;		/* Показ индикатора переполнения АЦП */
#else /* WITHOVFHIDE */
	static uint_fast8_t gshowovf = 1;		/* Показ индикатора переполнения АЦП */
#endif /* WITHOVFHIDE */

static uint_fast8_t glock;
#if WITHLCDBACKLIGHTOFF
	// Имеется управление включением/выключением подсветки дисплея
	static uint_fast8_t dimmmode;
#else /* WITHLCDBACKLIGHTOFF */
	enum { dimmmode = 0 };
#endif /* WITHLCDBACKLIGHTOFF */
static uint_fast8_t gusefast;


#if WITHNOTCHONOFF
	static uint_fast8_t gnotchtype;
	static uint_fast8_t gnotch;	// on/off
#elif WITHNOTCHFREQ
	static uint_fast8_t gnotchtype;
	static uint_fast8_t gnotch;	// on/off
	static dualctl16_t gnotchfreq = { 1000, 1000 };
	static dualctl16_t gnotchwidth = { 500, 500 };
#endif /* WITHNOTCHFREQ */

#if WITHNOTCHONOFF || WITHNOTCHFREQ
static const struct paramdefdef xgnotch =
{
	QLABEL3("NOTCH", "Notch", "NOTCH"), 8, 3, RJ_ON,	ISTEP1,		/* управление режимом NOTCH */
	ITEM_VALUE,
	0, NOTCHMODE_COUNT - 1,
	RMT_NOTCH_BASE,							/* управление режимом NOTCH */
	getselector0, nvramoffs0, valueoffs0,
	NULL,	// uint_fast16_t value pointer
	& gnotch,	// uint_fast8_t value pointer
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */

#if WITHSPLIT
	static uint_fast8_t gvfoab;	/* (vfoa/vfob) */
	static uint_fast8_t gsplitmode = VFOMODES_VFOINIT;	/* (vfo/vfoa/vfob/mem) */
#elif WITHSPLITEX
	static uint_fast8_t gvfoab;	/* 1: vfoa/vfob swapped */
	static uint_fast8_t gsplitmode = VFOMODES_VFOINIT;	/* (vfo/vfoa/vfob/mem) */
#else /* WITHSPLIT */
	static const uint_fast8_t gvfoab = 0;	/* (vfoa/vfob) */
	static const uint_fast8_t gsplitmode = VFOMODES_VFOINIT;	/* (vfo/vfoa/vfob/mem) */
#endif /* WITHSPLIT */

#if WITHWARCBANDS
	enum { bandsethamwarc = 1 };	/* WARC HAM radio bands */
#else /* WITHWARCBANDS */
	enum { bandsethamwarc = 0 };	/* WARC HAM radio bands */
#endif /* WITHWARCBANDS */


#if WITHBCBANDS
static uint_fast8_t gbandsetbcast = 0;	/* Broadcast radio bands */
static const struct paramdefdef xgbandsetbcast =
{
	QLABEL3("BAND BC", "Bands BC", "BANDS BC"), 7, 3, RJ_YES,	ISTEP1,
	ITEM_VALUE,
	0, 1,
	OFFSETOF(struct nvmap, gbandsetbcast),
	getselector0, nvramoffs0, valueoffs0,
	NULL,	// uint_fast16_t value pointer
	& gbandsetbcast,	// uint_fast8_t value pointer
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#else /* WITHBCBANDS */
enum { gbandsetbcast = 0 };
#endif /* WITHBCBANDS */

static uint_fast8_t bandset11m;
static const struct paramdefdef xgbandset11m =
{
	QLABEL3("BAND 27", "Band 27", "BAND 27"), 7, 3, RJ_YES,	ISTEP1,
	ITEM_VALUE,
	0, 1,
	OFFSETOF(struct nvmap, bandset11m),
	getselector0, nvramoffs0, valueoffs0,
	NULL,	// uint_fast16_t value pointer
	& bandset11m,	// uint_fast8_t value pointer
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};

#if TUNE_6MBAND
static uint_fast8_t bandset6m = 1;	/* используется ли диапазон 6 метров */
static const struct paramdefdef xgbandset6m =
{
	QLABEL3("BAND 50", "Band 50", "BAND 50"), 8, 3, RJ_ON,	ISTEP1,
	ITEM_VALUE,
	0, 1,
	OFFSETOF(struct nvmap, bandset6m),
	getselector0, nvramoffs0, valueoffs0,
	NULL,	// uint_fast16_t value pointer
	& bandset6m,	// uint_fast8_t value pointer
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif /* TUNE_6MBAND */
#if TUNE_4MBAND
static uint_fast8_t bandset4m = 1;	/* используется ли диапазон 4 метров */
static const struct paramdefdef xgbandset4m =
{
	QLABEL3("BAND 70", "Band 70", "BAND 70"), 8, 3, RJ_ON,	ISTEP1,
	ITEM_VALUE,
	0, 1,
	OFFSETOF(struct nvmap, bandset4m),
	getselector0, nvramoffs0, valueoffs0,
	NULL,	// uint_fast16_t value pointer
	& bandset4m,	// uint_fast8_t value pointer
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif /* TUNE_4MBAND */
#if TUNE_2MBAND
static uint_fast8_t bandset2m = 1;	/* используется ли диапазон 2 метра */
static const struct paramdefdef xgbandset2m =
{
	QLABEL3("BAND 144", "Band 144", "BAND 144"), 8, 3, RJ_ON,	ISTEP1,
	ITEM_VALUE,
	0, 1,
	OFFSETOF(struct nvmap, bandset2m),
	getselector0, nvramoffs0, valueoffs0,
	NULL,	// uint_fast16_t value pointer
	& bandset2m,	// uint_fast8_t value pointer
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif /* TUNE_2MBAND */

#if WITHCAT
	static uint_fast8_t catprocenable;	/* разрешена работа подсистемы CAT */
#endif /* WITHCAT */

static uint_fast8_t alignmode;		/* режимы для настройки аппаратной части (0-нормальная работа) */

#if 1//WITHDEBUG
static uint_fast8_t gforcexvrtr;	/* принудительно включить коммутацию трансвертора */
#else
	enum { gforcexvrtr = 0 };
#endif /* WITHDEBUG */

#if WITHUSEAUDIOREC
	#if defined (WITHBBOX) && defined (WITHBBOXREC)
		static uint_fast8_t recmode = WITHBBOXREC;	/* автоматически начинаем запись на SD CARD при включении */
	#else /* defined (WITHBBOX) && defined (WITHBBOXREC) */
		static uint_fast8_t recmode;	/* автоматически начинаем запись на SD CARD при включении */
	#endif /* defined (WITHBBOX) && defined (WITHBBOXREC) */
#endif /* WITHUSEAUDIOREC */

#if WITHHDMITVHW

static uint_fast8_t ghdmiformat;
/* Видеорежим внешнего HDMI монитора */
static const struct paramdefdef xhdmiformat =
{
	QLABEL3("HDMI FMT", "HDMI Format", "HDMI FMT"), 7, 5, RJ_CB, ISTEP1,
	ITEM_VALUE | ITEM_LISTSELECT,
	0, HDMIFORMATS_count - 1,
	OFFSETOF(struct nvmap, ghdmiformat),
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& ghdmiformat,
	getzerobase, /* складывается со смещением и отображается */
	getvaltexthdmiformat, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif /* WITHHDMITVHW */

#if WITHLO1LEVELADJ
	static uint_fast8_t lo1level = WITHLO1LEVELADJINITIAL; //100;	/* уровень (амплитуда) LO1 в процентах */
#endif /* WITHLO1LEVELADJ */

#if defined (WITHDISPLAY_FPS)
	static uint_fast8_t gdisplayfreqsfps = WITHDISPLAY_FPS;
#else /* defined (WITHDISPLAY_FPS) */
	static uint_fast8_t gdisplayfreqsfps = DISPLAY_FPS;
#endif /* defined (WITHDISPLAY_FPS) */

#if defined (WITHDISPLAYSWR_FPS)
	static uint_fast8_t glatchfps = WITHDISPLAYSWR_FPS;	// частота latch
#else /* defined (WITHDISPLAYSWR_FPS) */
	static uint_fast8_t glatchfps = DISPLAYSWR_FPS;	// частота latch
#endif /* defined (WITHDISPLAYSWR_FPS) */
#if WITHSPECTRUMWF
#if defined (WITHDEFAULTVIEW)		/* стиль отображения спектра и панорамы */
	static uint_fast8_t gviewstyle = WITHDEFAULTVIEW;
#else /* defined (WITHDEFAULTVIEW) */
	static uint_fast8_t gviewstyle = VIEW_COLOR;
#endif /* defined (WITHDEFAULTVIEW) */

#if defined (WITHVIEW_3DSS_MARK)	/* Для VIEW_3DSS - индикация полосы пропускания на спектре */
	static uint_fast8_t gview3dss_mark = WITHVIEW_3DSS_MARK;
#else /* defined (WITHVIEW_3DSS_MARK) */
	static uint_fast8_t gview3dss_mark = 0;
#endif /* defined (WITHVIEW_3DSS_MARK) */

	/* складывается со смещением и отображается */
	static int_fast32_t getrfdbbase(void)
	{
		return DBVALOFFSET_BASE;
	}

	static uint_fast8_t gtopdbtx = WITHTOPDBDEFAULTTX;	/* верхний предел FFT для показа в режиме TX */
	static uint_fast8_t gbottomdbtx = WITHBOTTOMDBDEFAULTTX;	/* нижний предел FFT для показа в режиме TX */
	static uint_fast8_t gtopdb = WITHTOPDBDEFAULT;	/* верхний предел FFT */
	static uint_fast8_t gbottomdb = WITHBOTTOMDBDEFAULT;	/* нижний предел FFT */
	static uint_fast8_t gzoomxpow2;		/* степень двойки - состояние растягиваия спектра (уменьшение наблюдаемой полосы частот) */
	static uint_fast8_t gtxloopback = 1;	/* включение спектроанализатора сигнала передачи */
	static int_fast16_t gafspeclow = 100;	// нижняя частота отображения спектроанализатора
	static int_fast16_t gafspechigh = 4000;	// верхняя частота отображения спектроанализатора
	static uint_fast8_t glvlgridstep = 12;	/* Шаг сетки уровней в децибелах */
	static uint_fast8_t grxbwsatu = 50;	/* 0..100 - насыщнность цвета заполнения "шторки" - индикатор полосы пропускания примника на спкктре. */
#if defined (WITHSPECBETA_DEFAULT)
	static uint_fast8_t gspecbeta100 = WITHSPECBETA_DEFAULT;
#else /* defined (WITHSPECBETA_DEFAULT) */
	static uint_fast8_t gspecbeta100 = 50;	/* beta = 0.1 .. 1.0 */
#endif /* defined (WITHSPECBETA_DEFAULT) */
#if defined (WITHWFLBETA_DEFAULT)
	static uint_fast8_t gwflbeta100 = WITHWFLBETA_DEFAULT;
#else /* defined (WITHWFLBETA_DEFAULT) */
	static uint_fast8_t gwflbeta100 = 50;	/* beta = 0.1 .. 1.0 */
#endif /* defined (WITHWFLBETA_DEFAULT) */

static unsigned getselector_bandgroup(unsigned * count)
{
	const uint_fast8_t bi = getbankindex_ab_fordisplay(0);	/* VFO A modifications */
	const uint_fast8_t bg = getfreqbandgroup(gfreqs [bi]);
	* count = BANDGROUP_COUNT;

	return bg;
}

static nvramaddress_t nvramoffs_bandgroup(nvramaddress_t base, unsigned bg)
{
	ASSERT(base != MENUNONVRAM);

	if (base == MENUNONVRAM)
		return MENUNONVRAM;
	//
	// для диапазонов - вычисляем шаг увеличения индекса по массиву хранения в диапазонах
	return base + RMT_BANDPOS(bg) - RMT_BANDPOS(0);
}


static const char * const view_types [VIEW_count] =
{
	"LINE",
	"FILL",
	"COLOR",
	"DOTS",
#if WITHVIEW_3DSS
	"3DSS",
#endif /* WITHVIEW_3DSS */
};

static size_t getvaltextviewstyle(char * buff, size_t count, int_fast32_t value)
{
	/* стиль отображения спектра и панорамы */
	return local_snprintf_P(buff, count, "%s", view_types [value]);
}
/* стиль отображения спектра и панорамы */
static const struct paramdefdef xgviewstyle =
{
	QLABEL3("VIEW STL", "View style", "VIEW STLE"), 7, 5, RJ_CB, ISTEP1,
	ITEM_VALUE | ITEM_LISTSELECT,
	0, VIEW_count - 1,
	OFFSETOF(struct nvmap, gviewstyle),
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& gviewstyle,
	getzerobase, /* складывается со смещением и отображается */
	getvaltextviewstyle, /* getvaltext получить текст значения параметра - see RJ_CB */
};

/* Часть отведенной под спектр высоты экрана 0..100 */
static uint_fast8_t gspectrumpart = 75;	/* Часть отведенной под спектр высоты экрана 0..100 */
static const struct paramdefdef xgspectrumpart =
{
	QLABEL2("SPEC PRT", "Spectrum part"), 7, 0, RJ_UNSIGNED, ISTEP1,
	ITEM_VALUE,
	WITHSPPARTMIN, WITHSPPARTMAX,			/* Часть отведенной под спектр высоты экрана 0..100 */
	OFFSETOF(struct nvmap, gspectrumpart),
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& gspectrumpart,
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};

#if WITHSPECTRUMWF

#if BOARD_FFTZOOM_POW2MAX > 0
/* уменьшение отображаемого участка спектра */
static const struct paramdefdef xgzoomxpow2 =
{
	QLABEL3("ZOOM PAN", "ZOOM PAN", "ZOOM PAN"), 7, 0, RJ_POW2,	ISTEP1,
	ITEM_VALUE,
	0, BOARD_FFTZOOM_POW2MAX,							/* уменьшение отображаемого участка спектра */
	OFFSETOF(struct nvmap, bandgroups [0].gzoomxpow2),
	getselector_bandgroup, nvramoffs_bandgroup, valueoffs0,
	NULL,
	& gzoomxpow2,
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif

/* нижний предел FFT */
static const struct paramdefdef xgtopdb =
{
	QLABEL("TOP DB"), 7, 0, RJ_SIGNED,	ISTEP1,
	ITEM_VALUE,
	WITHTOPDBMIN, WITHTOPDBMAX,							/* сколько не показывать сверху */
	OFFSETOF(struct nvmap, bandgroups [0].gtopdb),
	getselector_bandgroup, nvramoffs_bandgroup, valueoffs0,
	NULL,
	& gtopdb,
	getrfdbbase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
/* верхний предел FFT */
static const struct paramdefdef xgbottomdb =
{
	QLABEL2("BOTTM DB", "BOTTOM DB"), 7, 0, RJ_SIGNED,	ISTEP1,
	ITEM_VALUE,
	WITHBOTTOMDBMIN, WITHBOTTOMDBMAX,							/* диапазон отображаемых значений */
	OFFSETOF(struct nvmap, bandgroups [0].gbottomdb),
	getselector_bandgroup, nvramoffs_bandgroup, valueoffs0,
	NULL,
	& gbottomdb,
	getrfdbbase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
/* нижний предел FFT для показа в режиме TX */
static const struct paramdefdef xgtopdbtx =
{
	QLABEL("TOP DB TX"), 7, 0, RJ_SIGNED,	ISTEP_RO,
	ITEM_VALUE,
	WITHTOPDBMIN, WITHTOPDBMAX,							/* сколько не показывать сверху */
	MENUNONVRAM,
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& gtopdbtx,
	getrfdbbase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
/* верхний предел FFT для показа в режиме TX */
static const struct paramdefdef xgbottomdbtx =
{
	QLABEL2("BOTTM DB TX", "BOTTOM DB"), 7, 0, RJ_SIGNED,	ISTEP_RO,
	ITEM_VALUE,
	WITHBOTTOMDBMIN, WITHBOTTOMDBMAX,							/* диапазон отображаемых значений */
	MENUNONVRAM,
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& gbottomdbtx,
	getrfdbbase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};

#endif /* WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0 */

#endif /* WITHSPECTRUMWF */

#if WITHLCDBACKLIGHT
	#if WITHISBOOTLOADER
		static uint_fast8_t gbglight = WITHLCDBACKLIGHTMIN;
	#elif defined (WITHLCDBACKLIGHTDEF)
		static uint_fast8_t gbglight = WITHLCDBACKLIGHTDEF;
	#else /* WITHISBOOTLOADER */
		static uint_fast8_t gbglight = WITHLCDBACKLIGHTMIN;
	#endif /* WITHISBOOTLOADER */

	static const struct paramdefdef xgbglight =
	{
		QLABEL2("LCD LIGH", "TFT Backlight"), 7, 0, RJ_UNSIGNED, ISTEP1,
		ITEM_VALUE,
		WITHLCDBACKLIGHTMIN, WITHLCDBACKLIGHTMAX,
		OFFSETOF(struct nvmap, gbglight),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbglight,
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};

#else /* WITHLCDBACKLIGHT */
	enum { gbglight = 0 };
#endif /* WITHLCDBACKLIGHT */

#if WITHKBDBACKLIGHT
	static uint_fast8_t gkblight /* = 1 */;

	static const struct paramdefdef xgkblight =
	{
		QLABEL2("KBD LIGH", "KBD Backlight"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gkblight),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gkblight,
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};

#else /* WITHKBDBACKLIGHT */
	enum { gkblight = 0 };
#endif /* WITHKBDBACKLIGHT */

static uint_fast8_t gpoweronhold = 1;	/* выдать "1" на выход удержания питания включенным */

#if LCDMODE_COLORED
	//static uint_fast8_t gbluebgnd;
	enum { gbluebgnd = 0 };
#else
	enum { gbluebgnd = 0 };
#endif /* LCDMODE_COLORED */

static uint_fast8_t gshowdbm = 1;	// Отображение уровня сигнала в dBm или S-memter

#if WITHAUTOTUNER

enum
{
 KSCH_TOHIGH = 0,
 KSCH_TOLOW,
 //
 KSCH_COUNT
};


#if FULLSET_7L8C
	/* 7 indictors, 8 capacitors */
	#define CMAX	254    //максимальное значение емкости конденсатора контура
	#define CMIN  0      //минимальное значение емкости конденсатора контура
	#define LMAX	127    //максимальное значение индуктивности катушки контура
	#define LMIN  0      //минимальное значение индуктивности катушки контура
#elif FULLSET_8L7C
	/* 8 indictors, 7 capacitors */
	#define CMAX	127    //максимальное значение емкости конденсатора контура
	#define CMIN  0      //минимальное значение емкости конденсатора контура
	#define LMAX	254    //максимальное значение индуктивности катушки контура
	#define LMIN  0      //минимальное значение индуктивности катушки контура
#elif FULLSET7
	/* 7 indictors, 7 capacitors */
	#define CMAX	127    //максимальное значение емкости конденсатора контура
	#define CMIN  0      //минимальное значение емкости конденсатора контура
	#define LMAX	127    //максимальное значение индуктивности катушки контура
	#define LMIN  0      //минимальное значение индуктивности катушки контура
#elif FULLSET8
	/* 8 indictors, 8 capacitors */
	#define CMAX	254    //максимальное значение емкости конденсатора контура
	#define CMIN  0      //минимальное значение емкости конденсатора контура
	#define LMAX	254    //максимальное значение индуктивности катушки контура
	#define LMIN  0      //минимальное значение индуктивности катушки контура
#endif

#endif /* WITHAUTOTUNER */

#if WITHCAT

	static uint_fast8_t catenable = 1;	/* модифицируется через меню. */
	static uint_fast8_t catbaudrate = 3;	/* 3 is a 9600 */ /* модифицируется через меню. - номер скорости при работе по CAT */

	static const struct paramdefdef xcatenable =
	{
		QLABEL("CAT ENAB"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, catenable),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& catenable,
		getzerobase,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};

	#if WITHCAT_MUX

		static const struct catmuxmode_tag
		{
			uint8_t code;
			const char * label;
		} catmuxmodes [] =
		{
			{	BOARD_CATMUX_USBCDC, "USB", },
		#if WITHUSEUSBBT
			{	BOARD_CATMUX_BTSPP, "BT", },
		#endif /* WITHUSEUSBBT */
			{	BOARD_CATMUX_DIN8, "DIN8", },
		};

		static size_t getvaltextcatspeed(char * buff, size_t count, int_fast32_t value)
		{
			return local_snprintf_P(buff, count, "%u", catbr2int [value] * BRSCALE);
		}

		#if WITHCAT_MUX
		static size_t getvaltextcatmux(char * buff, size_t count, int_fast32_t value)
		{
			return local_snprintf_P(buff, count, "%s", catmuxmodes [value].label);
		}
		#endif /* WITHCAT_MUX */

		enum { nopttsig = BOARD_CATSIG_NONE };
		enum { nokeysig = BOARD_CATSIG_NONE };
		static uint_fast8_t gcatmux;
		static const struct paramdefdef xgcatmux =
		{
			QLABEL("CAT SEL"), 8, 3, RJ_CB,	ISTEP1,
			ITEM_VALUE | ITEM_LISTSELECT,
			0, ARRAY_SIZE(catmuxmodes) - 1,
			OFFSETOF(struct nvmap, gcatmux),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gcatmux,
			getzerobase,
			getvaltextcatmux, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
		static const struct paramdefdef xcatbaudrate =
		{
			QLABEL("CAT SPD"), 7, 0, RJ_CB,	ISTEP1,
			ITEM_VALUE | ITEM_LISTSELECT,
			0, ARRAY_SIZE(catbr2int) - 1,
			OFFSETOF(struct nvmap, catbaudrate),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& catbaudrate,
			getzerobase,
			getvaltextcatspeed, /* getvaltext получить текст значения параметра - see RJ_CB */
		};

	#elif WITHCAT_CDC
		#if LCDMODE_DUMMY || ! WITHKEYBOARD
			enum { nopttsig = BOARD_CATSIG_NONE /*BOARD_CATSIG_SER1_DTR */};		// устройство без органов управления и индикации
			enum { nokeysig = BOARD_CATSIG_NONE };		// устройство без органов управления и индикации
		#else /* LCDMODE_DUMMY || ! WITHKEYBOARD */
			enum { nopttsig = BOARD_CATSIG_NONE };
			enum { nokeysig = BOARD_CATSIG_NONE };
		#endif /* LCDMODE_DUMMY || ! WITHKEYBOARD */
	#else /* WITHCAT_CDC */
		enum { nopttsig = BOARD_CATSIG_NONE };
		enum { nokeysig = BOARD_CATSIG_NONE };
	#endif /* WITHCAT_CDC */

	static const char catsiglabels [BOARD_CATSIG_count] [9] =
	{
		"NONE",
		"SER1 DTR",
		"SER1 RTS",
	#if WITHUSBHW && WITHUSBCDCACM && WITHUSBCDCACM_N > 1
		"SER2 DTR",
		"SER2 RTS",
	#endif /* WITHUSBHW && WITHUSBCDCACM && WITHUSBCDCACM_N > 1 */
	};

	static size_t getvaltextcatsig(char * buff, size_t count, int_fast32_t value)
	{
		return local_snprintf_P(buff, count, "%s", catsiglabels [value]);
	}

#if WITHTX
	static uint_fast8_t catsigptt = nopttsig;	/* Выбраный сигнал для перехода на передачу по CAT */
	static const struct paramdefdef xcatsigptt =
	{
		QLABEL("CAT PTT"), 8, 8, RJ_CB,	ISTEP1,
		ITEM_VALUE | ITEM_LISTSELECT,
		0, BOARD_CATSIG_count - 1,
		OFFSETOF(struct nvmap, catsigptt),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& catsigptt,
		getzerobase,
		getvaltextcatsig, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
#endif /* WITHTX */
	static uint_fast8_t catsigkey = nokeysig;	/* Выбраный сигнал для манипуляции по CAT */
	static const struct paramdefdef xcatsigkey =
	{
		QLABEL("CAT KEY"), 8, 8, RJ_CB,	ISTEP1,
		ITEM_VALUE | ITEM_LISTSELECT,
		0, BOARD_CATSIG_count - 1,
		OFFSETOF(struct nvmap, catsigkey),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& catsigkey,
		getzerobase,
		getvaltextcatsig, /* getvaltext получить текст значения параметра - see RJ_CB */
	};

#else /* WITHCAT */

	enum { catenable = 0 };

#endif /* WITHCAT */

#if WITHIF4DSP
#if defined WITHAFGAINDEFAULT
	static dualctl16_t afgain1 = { WITHAFGAINDEFAULT, WITHAFGAINDEFAULT };
#elif defined WITHPOTAFGAIN
	static dualctl16_t afgain1 = { BOARD_AFGAIN_MIN, BOARD_AFGAIN_MIN };	// Усиление НЧ на максимуме
#else
	static dualctl16_t afgain1 = { BOARD_AFGAIN_MAX, BOARD_AFGAIN_MAX };	// Усиление НЧ на максимуме
#endif /* defined WITHAFGAINDEFAULT */
	static dualctl16_t rfgain1 = { BOARD_IFGAIN_MAX, BOARD_IFGAIN_MAX };	// Усиление ПЧ на максимуме
	// Громкость в процентах
	static const struct paramdefdef xafgain1 =
	{
		QLABEL3("AF Gain", "Volume", "VOLUME"), 7, 0, RJ_UNSIGNED, ISTEP1,
		ITEM_VALUE,
		BOARD_AFGAIN_MIN, BOARD_AFGAIN_MAX, 					// Громкость в процентах
#if WITHPOTAFGAIN
		MENUNONVRAM,
#else /* WITHPOTAFGAIN */
		OFFSETOF(struct nvmap, afgain1),
#endif /* WITHPOTAFGAIN */
		getselector0, nvramoffs0, valueoffs0,
		& afgain1.value,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	// Усиление ПЧ/ВЧ в процентах
	static const struct paramdefdef xrfgain1 =
	{
		QLABEL3("RF GAIN", "RF Gain", "RF GAIN"), 7, 0, RJ_UNSIGNED, ISTEP1,
		ITEM_VALUE,
		BOARD_IFGAIN_MIN, BOARD_IFGAIN_MAX, 					// Усиление ПЧ/ВЧ в процентах
#if WITHPOTIFGAIN
		MENUNONVRAM,
#else /* WITHPOTIFGAIN */
		OFFSETOF(struct nvmap, rfgain1),
#endif /* WITHPOTIFGAIN */
		getselector0, nvramoffs0, valueoffs0,
		& rfgain1.value,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	static uint_fast16_t glineamp = WITHLINEINGAINMAX;	// усиление с LINE IN
	/* подстройка усиления с линейного входа через меню. */
	static const struct paramdefdef xglineamp =
	{
		QLABEL("LINE LVL"), 7, 0, RJ_UNSIGNED, ISTEP1,		/* подстройка усиления с линейного входа через меню. */
		ITEM_VALUE,
		WITHLINEINGAINMIN, WITHLINEINGAINMAX,
		OFFSETOF(struct nvmap, glineamp),	/* усиление с линейного входа */
		getselector0, nvramoffs0, valueoffs0,
		& glineamp,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	static uint_fast8_t gmikeboost20db;	// предусилитель микрофона
	static uint_fast8_t gmikeagc = 1;	/* Включение программной АРУ перед модулятором */
	static uint_fast8_t gmikeagcgain = 30;	/* Максимальное усидение АРУ микрофона */
#if WITHNOAUDIPROC
	static uint_fast8_t gmikehclip = 0;		/* Ограничитель (0 - не действует, 90 – ограничение наступает на 10 процентах от полной амплитуды) */
#else /* WITHNOAUDIPROC */
	static uint_fast8_t gmikehclip = 25;		/* Ограничитель */
#endif /* WITHNOAUDIPROC */

	#if WITHCOMPRESSOR
		static uint_fast8_t gcompressor_attack = 30;
		static uint_fast8_t gcompressor_release = 20;
		static uint_fast8_t gcompressor_hold = 10;
		static uint_fast8_t gcompressor_gain = 6;
		static uint_fast8_t gcompressor_threshold = 20;
	#endif /* WITHCOMPRESSOR */
	#if WITHREVERB
		static uint_fast8_t greverb;		/* ревербератор */
		static uint_fast8_t greverbdelay = 100;		/* ревербератор - задержка (ms) */
		static uint_fast8_t greverbloss = WITHREVERBLOSSMAX;		/* ревербератор - ослабление на возврате */
	#endif /* WITHREVERB */

	#if WITHUSBHW && WITHUSBUAC
		static uint_fast8_t gdatamode;	/* передача звука с USB вместо обычного источника */
		static uint_fast8_t	gusb_ft8cn;	/* совместимость VID/PID для работы с программой FT8CN */
		static uint_fast8_t gdatatx;	/* автоматическое изменение источника при появлении звука со стороны компьютера */
		static uint_fast8_t guacplayer = 0;	/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
#if WITHUSBDEV_HSDESC
		static uint_fast8_t gusb_hs = 1;	/* Использование USB HS dvtcn USB FS */
#else /* WITHUSBDEV_HSDESC */
		static uint_fast8_t gusb_hs = 0;	/* Использование USB HS dvtcn USB FS */
#endif /* WITHUSBDEV_HSDESC */
		static uint_fast8_t gbtaudioplayer = 0;
		static uint_fast8_t gswapiq;	/* Поменять местами I и Q сэмплы в потоке RTS96 */
		/* передача звука с USB вместо обычного источника */
		static const struct paramdefdef xgdatamode =
		{
			QLABEL2("DATA MDE", "DATA MODE"), 8, 3, RJ_ON,	ISTEP1,
			ITEM_VALUE,
			0, 1,
			OFFSETOF(struct nvmap, gdatamode),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gdatamode,
			getzerobase, /* складывается со смещением и отображается */
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
		uint_fast8_t hamradio_get_datamode(void) { return param_getvalue(& xgdatamode); }
		/* совместимость VID/PID для работы с программой FT8CN */
		static const struct paramdefdef xgusb_ft8cn =
		{
			QLABEL2("FT8CN", "FT8CN compat"), 7, 3, RJ_YES,	ISTEP1,
			ITEM_VALUE,
			0, 1, 					/* совместимость VID/PID для работы с программой FT8CN */
			OFFSETOF(struct nvmap, gusb_ft8cn),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gusb_ft8cn,
			getzerobase, /* складывается со смещением и отображается */
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
		uint_fast8_t hamradio_get_ft8cn(void) { return param_getvalue(& xgusb_ft8cn); }
		/* автоматическое изменение источника при появлении звука со стороны компьютера */
		static const struct paramdefdef xgdatatx =
		{
			QLABEL3("USB DATA", "USB Data", "USB DATA"), 8, 3, RJ_ON,	ISTEP1,		/* автоматическое изменение источника при появлении звука со стороны компьютера */
			ITEM_VALUE,
			0, 1,
			OFFSETOF(struct nvmap, gdatatx),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gdatatx,
			getzerobase, /* складывается со смещением и отображается */
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
		/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
		static const struct paramdefdef xguacplayer =
		{
			QLABEL3("PLAY USB", "Play USB", "PLAY USB"), 7, 3, RJ_YES,	ISTEP1,
			ITEM_VALUE,
			0, 1, 					/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
			OFFSETOF(struct nvmap, guacplayer),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& guacplayer,
			getzerobase, /* складывается со смещением и отображается */
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
		/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
		static const struct paramdefdef xgbtaudioplayer =
		{
			QLABEL3("PLAY BT", "Play BT", "PLAY BT"), 7, 3, RJ_YES,	ISTEP1,
			ITEM_VALUE,
			0, 1, 					/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
			OFFSETOF(struct nvmap, gbtaudioplayer),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gbtaudioplayer,
			getzerobase, /* складывается со смещением и отображается */
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
		/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
		static const struct paramdefdef xgusb_hs =
		{
			QLABEL("HS USB"), 7, 3, RJ_YES,	ISTEP1,
			ITEM_VALUE,
			0, 1, 					/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
			OFFSETOF(struct nvmap, gusb_hs),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gusb_hs,
			getzerobase, /* складывается со смещением и отображается */
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
	#if WITHRTS96 || WITHRTS192
		/* Поменять местами I и Q сэмплы в потоке RTS96 */
		static const struct paramdefdef xgswapiq =
		{
			QLABEL3("SWAP I/Q", "Swap I/Q", "SWAP I/Q"), 7, 3, RJ_YES,	ISTEP1,
			ITEM_VALUE,
			0, 1, 					/* Поменять местами I и Q сэмплы в потоке RTS96 */
			OFFSETOF(struct nvmap, gswapiq),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gswapiq,
			getzerobase, /* складывается со смещением и отображается */
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
	#endif /* WITHRTS96 || WITHRTS192 */
	#else /* WITHUSBHW && WITHUSBUAC */
		enum { gdatamode = 0 };	/* передача звука с USB вместо обычного источника */
		enum { guacplayer = 0 };
		enum { gusb_hs = 0 };
		uint_fast8_t hamradio_get_datamode(void) { return gdatamode; }
		uint_fast8_t hamradio_get_ft8cn(void) { return 0; }
	#endif /* WITHUSBHW && WITHUSBUAC */
	#if WITHAFCODEC1HAVEPROC
		static int_fast32_t getequalizerbase(void)
		{
			return - EQUALIZERBASE;
		}
		static uint_fast8_t gmikeequalizer;	// включение обработки сигнала с микрофона (эффекты, эквалайзер, ...)
		static uint_fast8_t gmikeequalizerparams [HARDWARE_CODEC1_NPROCPARAMS] =
		{
			// Эквалайзер 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
			EQUALIZERBASE, EQUALIZERBASE, EQUALIZERBASE, EQUALIZERBASE, EQUALIZERBASE
		};
		// включение обработки сигнала с микрофона (эффекты, эквалайзер, ...)
		static const struct paramdefdef xgmikeequalizer =
		{
			QLABEL("MIC EQUA"), 8, 3, RJ_ON,	ISTEP1,
			ITEM_VALUE,
			0, 1,
			OFFSETOF(struct nvmap, gmikeequalizer),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gmikeequalizer,
			getzerobase, /* складывается со смещением и отображается */
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
	#endif /* WITHAFCODEC1HAVEPROC */
#if WITHAFEQUALIZER
		static uint_fast8_t geqtx;
		static uint_fast8_t geqrx;
		static uint_fast8_t geqtxparams [AF_EQUALIZER_BANDS] =
		{
			AF_EQUALIZER_BASE, AF_EQUALIZER_BASE, AF_EQUALIZER_BASE
		};
		static uint_fast8_t geqrxparams [AF_EQUALIZER_BANDS] =
		{
			AF_EQUALIZER_BASE, AF_EQUALIZER_BASE, AF_EQUALIZER_BASE
		};

		int_fast32_t hamradio_get_af_equalizer_base(void)
		{
			ASSERT(3 == AF_EQUALIZER_BANDS);	// вылетит если увеличат количество полос (инициализаторы добавить!)
			return - AF_EQUALIZER_BASE;
		}

		int_fast32_t hamradio_get_af_equalizer_gain_rx(uint_fast8_t v)
		{
			ASSERT(3 == AF_EQUALIZER_BANDS);	// вылетит если увеличат количество полос (инициализаторы добавить!)
			ASSERT(v < AF_EQUALIZER_BANDS);
			return geqrxparams [v];
		}

		void hamradio_set_af_equalizer_gain_rx(uint_fast8_t index, uint_fast8_t gain)
		{
			ASSERT(3 == AF_EQUALIZER_BANDS);	// вылетит если увеличат количество полос (инициализаторы добавить!)
			ASSERT(index < AF_EQUALIZER_BANDS);
			ASSERT(gain <= AF_EQUALIZER_BASE * 2);
			geqrxparams [index] = gain;
			save_i8(OFFSETOF(struct nvmap, geqrxparams [index]), geqrxparams [index]);
			updateboard();
		}

		uint_fast8_t hamradio_get_geqrx(void)
		{
			return geqrx;
		}

		void hamradio_set_geqrx(uint_fast8_t v)
		{
			geqrx = v != 0;
			save_i8(OFFSETOF(struct nvmap, geqrx), geqrx);
			updateboard();
		}

	#endif /* WITHAFEQUALIZER */
	static uint_fast8_t gagcoff;
#else /* WITHIF4DSP */
	static const uint_fast8_t gagcoff = 0;
	static const uint_fast8_t gdatamode = 0;	/* передача звука с USB вместо обычного источника */
	uint_fast8_t hamradio_get_ft8cn(void) { return 0; }
#endif /* WITHIF4DSP */


#if WITHAUTOTUNER
	static uint_fast16_t tunercap;// = (CMAX - CMIN) / 2 + CMIN;
	static uint_fast16_t tunerind;// = (LMAX - LMIN) / 2 + LMIN;
	static uint_fast8_t tunertype;
	static uint_fast8_t tunerwork;	/* начинаем работу с выключенным тюнером */
	static uint_fast8_t gtunerdelay = 25;
#if WITHAUTOTUNER_N7DDCALGO
	static uint_fast8_t gn7ddclinearC = 1;
	static uint_fast8_t gn7ddclinearL;
#endif /* WITHAUTOTUNER_N7DDCALGO */

#endif /* WITHAUTOTUNER */

#if WITHSUBTONES && WITHIF4DSP
	// частоты  Continuous Tone-Coded Squelch System or CTCSS с точностью 0.1 герца.
	// https://en.wikipedia.org/wiki/Continuous_Tone-Coded_Squelch_System#List_of_tones
	static const uint_least16_t gsubtones [] =
	{
		0,	/* Off */
		670,	/* 67.0 герц */
		693,	/* 69.3 герц */
		719,	/* 71.9 герц */
		744,	/* 74.4 герц */
		770,	/* 77.0 герц */
		797,	/* 79.7 герц */
		825,	/* 82.5 герц */
		854,	/* 85.4 герц */
		885,	/* 88.5 герц */
		915,	/* 91.5 герц */
		948,	/* 94.8 герц */
		974,	/* 97.4 герц */
		1000,	/* 100.0 герц */
		1035,	/* 103.5 герц */
		1072,	/* 107.2 герц */
		1109,	/* 110.9 герц */
		1148,	/* 114.8 герц */
		1188,	/* 118.8 герц */
		1230,	/* 123.0 герц */
		1273,	/* 127.3 герц */
		1318,	/* 131.8 герц - also used in Digital-Coded Squelch - DCS as the digital bit rate is 134.4 bits per second */
		1365,	/* 136.5 герц - also used in DCS */
		1413,	/* 141.3 герц */
		1462,	/* 146.2 герц */
		1514,	/* 151.4 герц */
		1567,	/* 156.7 герц */
		1598,	/* 159.8 герц */
		1622,	/* 162.2 герц */
		1655,	/* 165.5 герц */
		1679,	/* 167.9 герц */
		1713,	/* 171.3 герц */
		1738,	/* 173.8 герц */
		1773,	/* 177.3 герц */
		1799,	/* 179.9 герц */
		1835,	/* 183.5 герц */
		1862,	/* 186.2 герц */
		1899,	/* 189.9 герц */
		1928,	/* 192.8 герц */
		1966,	/* 196.6 герц */
		1995,	/* 199.5 герц */
		2035,	/* 203.5 герц */
		2065,	/* 206.5 герц */
		2107,	/* 210.7 герц */
		2181,	/* 218.1 герц */
		2257,	/* 225.7 герц */
		2291,	/* 229.1 герц */
		2336,	/* 233.6 герц */
		2418,	/* 241.8 герц */
		2503,	/* 250.3 герц */
		2541,	/* 254.1 герц */
	};
	#define CTCSS_NFREQUES ARRAY_SIZE(gsubtones)
	static size_t getvaltextsubtone(char * buff, size_t count, int_fast32_t value)
	{
		if (gsubtones [value] == 0)
			return local_snprintf_P(buff, count, "%s", "OFF");
		else
			return local_snprintf_P(buff, count, "%u.%1u", gsubtones [value] / 10, gsubtones [value] % 10);
	}
	static uint_fast8_t gsubtoneitx;	// частота subtone
	static uint_fast8_t gsubtoneirx;	// частота subtone
	//  Continuous Tone-Coded Squelch System or CTCSS settings group
	static const struct paramdefdef xgctssgroup =
	{
		QLABEL("CTCSS"), 0, 0, 0, 0,
		ITEM_GROUP,
		0, 0,
		OFFSETOF(struct nvmap, ggrpctcss),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	//  Continuous Tone-Coded Squelch System or CTCSS freq
	static const struct paramdefdef xgsubtoneitx =
	{
		QLABEL2("TCTCSS FQ", "T-CTCSS FREQ"), 7, 1, RJ_CB,	ISTEP1,
		ITEM_VALUE | ITEM_LISTSELECT,
		0, CTCSS_NFREQUES - 1,
		OFFSETOF(struct nvmap, gsubtoneitx),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gsubtoneitx,
		getzerobase,
		getvaltextsubtone, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	//  Continuous Tone-Coded Squelch System or CTCSS freq
	static const struct paramdefdef xgsubtoneirx =
	{
		QLABEL2("RCTCSS FQ", "R-CTCSS FREQ"), 7, 1, RJ_CB,	ISTEP1,
		ITEM_VALUE | ITEM_LISTSELECT,
		0, CTCSS_NFREQUES - 1,
		OFFSETOF(struct nvmap, gsubtoneirx),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gsubtoneirx,
		getzerobase,
		getvaltextsubtone, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	static uint_fast8_t gsubtonelevel = 10;	/* Уровень сигнала CTCSS в процентах - 0%..100% */
	//  Continuous Tone-Coded Squelch System or CTCSS control
	/* Select the CTCSS transmit level. */
	static const struct paramdefdef xgctsslevel =
	{
		QLABEL2("CTCSSLVL", "CTCSS LEVEL"), 7, 0, RJ_UNSIGNED, ISTEP1,		/* Select the CTCSS transmit level. */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gsubtonelevel),	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gsubtonelevel,
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
#endif /* WITHTX && WITHSUBTONES && WITHIF4DSP */

#if WITHTX

#if WITHRPTOFFSET
		static const struct paramdefdef xrptroffshf1k =
		{
			QLABEL("OFFS HF"), 0, 0, RJ_SIGNED, ISTEP1,
			ITEM_VALUE,
			RPTOFFSMIN, RPTOFFSMAX,		/* repeater offset */
			OFFSETOF(struct nvmap, rptroffshf1k),
			getselector0, nvramoffs0, valueoffs0,
			& rptroffshf1k,
			NULL,
			getrptoffsbase,
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
		static const struct paramdefdef xrptroffsvhf1k =
		{
			QLABEL("OFFS VHF"), 0, 0, RJ_SIGNED, ISTEP1,
			ITEM_VALUE,
			RPTOFFSMIN, RPTOFFSMAX,		/* repeater offset */
			OFFSETOF(struct nvmap, rptroffsvhf1k),
			getselector0, nvramoffs0, valueoffs0,
			& rptroffsvhf1k,
			NULL,
			getrptoffsbase,
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
		static const struct paramdefdef xrptrhfenable =
		{
			QLABEL("REPT HF"), 0, 0, RJ_ON, ISTEP1,
			ITEM_VALUE,
			0, 1,		/* repeater offset */
			OFFSETOF(struct nvmap, rptrhfenable),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& rptrhfenable,
			getzerobase,
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
		static const struct paramdefdef xrptrvhfenable =
		{
			QLABEL("REPT VHF"), 0, 0, RJ_ON, ISTEP1,
			ITEM_VALUE,
			0, 1,		/* repeater offset */
			OFFSETOF(struct nvmap, rptrvhfenable),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& rptrvhfenable,
			getzerobase,
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
#endif /* WITHRPTOFFSET */

		/* пара значений для 10% выходной мощности и 100% выходной мощности */
		struct pwradj
		{
			uint_fast8_t adj_a;	/* 10%	*/
			uint_fast8_t adj_b;	/* 100% */
#if WITHPACLASSA
			uint_fast8_t adj_classa;	/* 100% */
#endif /* WITHPACLASSA */
		};
		static struct pwradj gbandf2adj [NUMLPFADJ]; /* коррекция мощности по ФНЧ передачика */

		/* запись значений по умолчанию для корректировок мощности в завивимости от диапазона ФНЧ УМ */
		static void
		bandf2adjust_initialize(void)
		{
			uint_fast8_t i;

			for (i = 0; i < ARRAY_SIZE(gbandf2adj); ++ i)
			{
				gbandf2adj [i].adj_a = 20;
				gbandf2adj [i].adj_b = 100;
#if WITHPACLASSA
				gbandf2adj [i].adj_classa = 100;
#endif /* WITHPACLASSA */
			}
		}

	#if WITHPOWERTRIM
		#if WITHPACLASSA
			static uint_fast8_t gclassamode;	/* использование режима клвсс А при передаче */
			/* усилитель мощности поддерживает переключение в класс А */
			static const struct paramdefdef xgclassamode =
			{
				QLABEL2("CLASSA", "Class A"), 7, 0, RJ_ON,	ISTEP1,		/* использование режима клвсс А при передаче */
				ITEM_VALUE,
				0, 1,
				OFFSETOF(struct nvmap, gclassamode),
				getselector0, nvramoffs0, valueoffs0,
				NULL,
				& gclassamode,
				getzerobase,
				NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
			};
		#else /* WITHPACLASSA */
		#endif /* WITHPACLASSA */
			static dualctl8_t gnormalpower = { WITHPOWERTRIMMAX, WITHPOWERTRIMMAX };
		#if WITHLOWPOWEREXTTUNE
			static uint_fast8_t gtunepower = WITHPOWERTRIMATU; /* мощность при работе автоматического согласующего устройства */
		#else /* WITHLOWPOWEREXTTUNE */
			enum { gtunepower = WITHPOWERTRIMMAX }; /* мощность при работе автоматического согласующего устройства */
		#endif /* WITHLOWPOWEREXTTUNE */
	#elif WITHPOWERLPHP
		static uint_fast8_t gpwri = 1;	// индекс нормальной мощности
		#if WITHLOWPOWEREXTTUNE
			static uint_fast8_t gpwratunei = 0; // индекс мощность при работе автоматического согласующего устройства
		#else /* WITHLOWPOWEREXTTUNE */
			enum { gpwratunei = 1 }; // индекс нормальной мощности
		#endif /* WITHLOWPOWEREXTTUNE */
		#if WITHLOWPOWEREXTTUNE
			static uint_fast8_t gtunepower = WITHPOWERTRIMATU; /* мощность при работе автоматического согласующего устройства */
		#else /* WITHLOWPOWEREXTTUNE */
			enum { gtunepower = WITHPOWERTRIMMAX }; /* мощность при работе автоматического согласующего устройства */
		#endif /* WITHLOWPOWEREXTTUNE */
		static dualctl8_t gnormalpower = { WITHPOWERTRIMMAX, WITHPOWERTRIMMAX };
	#else
		static dualctl8_t gnormalpower = { WITHPOWERTRIMMAX, WITHPOWERTRIMMAX };
		enum { gtunepower = WITHPOWERTRIMMAX }; /* мощность при работе автоматического согласующего устройства */
	#endif /* WITHPOWERTRIM, WITHPOWERLPHP */

	#if WITHVOX
		static uint_fast8_t gvoxenable;	/* модифицируется через меню - автоматическое управление передатчиком (от голоса) */
		static uint_fast8_t gvoxlevel = 10;	/* модифицируется через меню - усиление VOX */
		static uint_fast8_t gavoxlevel = 50;	/* модифицируется через меню - усиление anti-VOX */
		static uint_fast8_t voxdelay = 70;	/* модифицируется через меню - задержка отпускания VOX */

		static const struct paramdefdef xgvoxenable =
		{
			QLABEL2("VOX EN", "VOX ENABLE"), 8, 3, RJ_ON,	ISTEP1,
			ITEM_VALUE,
			0, 1,
			OFFSETOF(struct nvmap, gvoxenable),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gvoxenable,
			getzerobase,
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
		static const struct paramdefdef xgvoxlevel =
		{
			QLABEL2("VOX LEVL", "VOX LEVEL"), 7, 0, RJ_UNSIGNED, ISTEP1,
			ITEM_VALUE,
			WITHVOXLEVELMIN, WITHVOXLEVELMAX,
			OFFSETOF(struct nvmap, gvoxlevel),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gvoxlevel,
			getzerobase,
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
		static const struct paramdefdef xgavoxlevel =
		{
			QLABEL2("AVOX LEV", "AVOX LEVEL"), 7, 0, RJ_UNSIGNED, ISTEP1,
			ITEM_VALUE,
			WITHAVOXLEVELMIN, WITHAVOXLEVELMAX,
			OFFSETOF(struct nvmap, gavoxlevel),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gavoxlevel,
			getzerobase,
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};
		static const struct paramdefdef xgvoxdelay =
		{
			QLABEL2("VOXDELAY", "VOX DELAY"), 7, 2, RJ_UNSIGNED, ISTEP5,	/* 50 mS step of changing value */
			ITEM_VALUE,
			WITHVOXDELAYMIN, WITHVOXDELAYMAX,						/* 0.1..2.5 seconds delay */
			OFFSETOF(struct nvmap, voxdelay),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& voxdelay,
			getzerobase,
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
		};


	#else /* WITHVOX */
		enum { gvoxenable = 0 };	/* автоматическое управление передатчиком (от голоса) */
	#endif /* WITHVOX */

	#if WITHMUTEALL
		static uint_fast8_t gmuteall;	/* Отключить микрофон всегда. */
	#else /* WITHMUTEALL */
		enum { gmuteall = 0 };
	#endif /* WITHMUTEALL */

	#if ! WITHPACLASSA
		enum { gclassamode = 0 };	/* использование режима клвсс А при передаче */
	#endif /* WITHPACLASSA */

	#if WITHELKEY
		static uint_fast8_t gbkinenable = 1;	/* модифицируется через меню - автоматическое управление передатчиком (от телеграфного манипулятора) */
		static uint_fast8_t bkindelay = 20;	/* в десятках mS. модифицируется через меню - задержка отпускания BREAK-IN */
	#else /* WITHELKEY */
		enum { gbkinenable = 0 };
	#endif /* WITHELKEY */

#if TXPATH_BIT_GATE_RX && CTLSTYLE_SW2011ALL
	enum { pretxdelay = 1 };	/* признак того, что требуется снятие питания со второго смесителя */
#else
	enum { pretxdelay = 0 };
#endif

	static uint_fast8_t grgbeep;	/* разрешение (не-0) или запрещение (0) формирования roger beep */
	#if (CTLREGMODE_STORCH_V4)
		// modem
		static uint_fast8_t rxtxdelay = 75;	/* в единицах mS. модифицируется через меню - задержка перехода прём-передача */
		static uint_fast8_t txrxdelay = 25;	/* в единицах mS. модифицируется через меню - задержка перехода передача-прём */
	#else
		static uint_fast8_t rxtxdelay = 25;	/* в единицах mS. модифицируется через меню - задержка перехода прём-передача */
		static uint_fast8_t txrxdelay = 25;	/* в единицах mS. модифицируется через меню - задержка перехода передача-прём */
	#endif

#else /* WITHTX */
	static const uint_fast8_t gbkinenable = 0;	/* модифицируется через меню - автоматическое управление передатчиком (от телеграфного манипулятора) */
	static const uint_fast8_t gvoxenable = 0;	/* модифицируется через меню - автоматическое управление передатчиком (от голоса) */
	static const uint_fast8_t bkindelay = 80;	/* в десятках mS. модифицируется через меню - задержка отпускания BREAK-IN */
#endif /* WITHTX */

#if WITHELKEY

	/* режим электронного ключа - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
	/* строки, выводимые на индикатор для обозначения режимов.
	 */
	static const struct {
		uint8_t code;
		const char * label;
	}  elkeymodes [] =
	{
		{ ELKEY_MODE_ACS, "ACS", },
		{ ELKEY_MODE_ELE, "ELE", },
		{ ELKEY_MODE_OFF, "OFF", },
		{ ELKEY_MODE_BUG, "BUG", },
	};

	static dualctl8_t elkeywpm = { 20, 20 };	/* скорость электронного ключа */
	static uint_fast8_t dashratio = 30;	/* отношение тире к длительности точки - в десятках процентов */
	static uint_fast8_t spaceratio = 10;	/* отношение паузы к длительности точки - в десятках процентов */
	static uint_fast8_t elkeyreverse;
	static uint_fast8_t elkeymode = 1;		/* режим электронного ключа - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
	static uint_fast8_t elkeyslope;		/* скорость уменьшения длительности точки и паузы - имитация виброплекса */

	static const struct paramdefdef xgelkeywpm =
	{
		QLABEL2("CW SPEED", "CW SPEED"), 7, 0, RJ_UNSIGNED, ISTEP1,
		ITEM_VALUE,
		CWWPMMIN, CWWPMMAX,		// minimal WPM = 10, maximal = 60 (also changed by command KS).
#if WITHPOTWPM
		MENUNONVRAM,
#else /* WITHPOTWPM */
		OFFSETOF(struct nvmap, elkeywpm),
#endif /* WITHPOTWPM */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& elkeywpm.value,
		getzerobase,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	static const struct paramdefdef xgdashratio =
	{
		QLABEL("DASH LEN"), 7, 1, RJ_UNSIGNED, ISTEP1,
		ITEM_VALUE,
		23, 45,
		OFFSETOF(struct nvmap, dashratio),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& dashratio,
		getzerobase,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	static const struct paramdefdef xgspaceratio =
	{
		QLABEL("DOT LEN"), 7, 1, RJ_UNSIGNED, ISTEP1,
		ITEM_VALUE,
		7, 13,
		OFFSETOF(struct nvmap, spaceratio),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& spaceratio,
		getzerobase,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	static const struct paramdefdef xgelkeyreverse =
	{
		QLABEL("KEY REV"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1,	/* режим электронного ключа - поменять местами точки с тире или нет. */
		OFFSETOF(struct nvmap, elkeyreverse),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& elkeyreverse,
		getzerobase,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	/* получить текст значения параметра */
	static size_t getelkeymodetext(char * buff, size_t count, int_fast32_t value)
	{
		/* режим электронного ключа - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
		return local_snprintf_P(buff, count, "%s", elkeymodes [value].label);
	}
	static const struct paramdefdef xgelkeymode =
	{
		QLABEL("KEYER"), 6, 0, RJ_CB,	ISTEP1,
		ITEM_VALUE | ITEM_LISTSELECT,
		0, ARRAY_SIZE(elkeymodes) - 1,	/* режим электронного ключа */
		OFFSETOF(struct nvmap, elkeymode),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& elkeymode,
		getzerobase,
		getelkeymodetext
	};
#if WITHVIBROPLEX
	static const struct paramdefdef xgelkeyslope =
	{
		QLABEL("VIBROPLX"), 7, 0, RJ_UNSIGNED, ISTEP1,		/* скорость уменьшения длительности точки и паузы - имитация виброплекса */
		ITEM_VALUE,
		0, 5,		// minimal 0 - без эффекта Виброплекса
		OFFSETOF(struct nvmap, elkeyslope),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& elkeyslope,
		getzerobase,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
#endif /* WITHVIBROPLEX */
#if WITHTX
	static const struct paramdefdef xgbkinenable =
	{
		QLABEL("BREAK-IN"), 8, 3, RJ_ON,	ISTEP1,	/* автоматическое управление передатчиком (от телеграфного манипулятора) */
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gbkinenable),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbkinenable,
		getzerobase,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	static const struct paramdefdef xgbkindelay =
	{
		QLABEL("CW DELAY"), 7, 2, RJ_UNSIGNED, ISTEP1,	/* задержка в десятках ms */
		ITEM_VALUE,
		5, 160,						/* 0.05..1.6 секунды */
		OFFSETOF(struct nvmap, bkindelay),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bkindelay,
		getzerobase,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
#endif /* WITHTX */
#if WITHIF4DSP
	static uint_fast8_t gcwedgetime = 5;	/* Время нарастания/спада огибающей телеграфа при передаче - в 1 мс */
	static const struct paramdefdef xgcwedgetime =
	{
		QLABEL2("EDGE TIM", "EDGE TIME"), 7, 0, RJ_UNSIGNED, ISTEP1,		/* Set the rise time of the transmitted CW envelope. */
		ITEM_VALUE,
		2, 16,
		OFFSETOF(struct nvmap, gcwedgetime),	/* Время нарастания/спада огибающей телеграфа при передаче - в 1 мс */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gcwedgetime,
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
#endif /* WITHIF4DSP */
#if WITHTX && WITHIF4DSP
	static uint_fast8_t gcwssbtx;			/* разрешение передачи телеграфа как тона в режиме SSB */
	static const struct paramdefdef xgcwssbtx =
	{
		QLABEL("SSB TXCW"), 8, 3, RJ_ON, ISTEP1,		/*  */
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gcwssbtx),	/* разрешение передачи телеграфа как тона в режиме SSB */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gcwssbtx,
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
#else /* WITHTX && WITHIF4DSP */
	enum { gcwssbtx = 0 };
#endif /* WITHTX && WITHIF4DSP */

#else /* WITHELKEY */
	enum { gcwssbtx = 0 };
	//static const uint_fast8_t elkeymode = 2;		/* режим электронного ключа - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
	//static const uint_fast8_t elkeyslope = 0;		/* скорость уменьшения длительности точки и паузы - имитация виброплекса */
#endif /* WITHELKEY */

static uint_fast8_t stayfreq;			/* при изменении режимов кнопками - не меняем частоту */

#if defined (DAC1_TYPE)
	#if defined (WITHDAC1VALDEF)
		static uint_fast8_t dac1level = WITHDAC1VALDEF;
	#else /* defined (WITHDAC1VALDEF) */
		static uint_fast8_t dac1level = (WITHDAC1VALMAX + 1 - WITHDAC1VALMIN) / 2 + WITHDAC1VALMIN;	/* модифицируется через меню. */
	#endif /* defined (WITHDAC1VALDEF) */
#endif /* defined (DAC1_TYPE) */


#if LO1FDIV_ADJ
	static uint_fast8_t lo1powmap [2] = { LO1_POWER2, LO1_POWER2 };		/* на сколько делим выходную частоту синтезатора первого гетеродина */
#endif
#if LO2FDIV_ADJ
	static uint_fast8_t lo2powmap [2] = { LO2_POWER2, LO2_POWER2 };		/* на сколько делим выходную частоту синтезатора 2-го гетеродина */
#endif
#if LO3FDIV_ADJ
	static uint_fast8_t lo3powmap [2] = { LO3_POWER2, LO3_POWER2 };		/* на сколько делим выходную частоту синтезатора 3-го гетеродина */
#endif
#if LO4FDIV_ADJ
	static uint_fast8_t lo4powmap [2] = { LO4_POWER2, LO4_POWER2 };		/* на сколько делим выходную частоту синтезатора 4-го гетеродина */
#endif

#if LO1PHASES
	static uint_fast16_t phasesmap [2]; /* приемник [0] и передатчик [1] - коррекция фазы - в nvram phaserx и phasetx */
#endif /* LO1PHASES */

static uint_fast8_t dctxmodecw;	/* при передаче предполагается, что частоты if2 и далее равны 0 - формирование телеграфа на передачу DDS */

#if 1//WITHBARS

	uint_fast16_t minforward = (1U << HARDWARE_ADCBITS) / 32;
	uint_fast8_t swrcalibr = 100;	/* калибровочный параметр SWR-метра 1.00 */

	#if WITHSWRMTR
		#if WITHMAXPWRCALI
			uint_fast8_t maxpwrcali = WITHMAXPWRCALI;	/* калибровочный параметр PWR-метра */
		#else
			uint_fast8_t maxpwrcali = 255;	/* калибровочный параметр PWR-метра */
		#endif
	#else
		uint_fast8_t maxpwrcali = 255;	/* калибровочный параметр PWR-метра */
	#endif /* WITHSWRMTR */

	#if WITHSWRMTR && ! WITHSHOWSWRPWR
		uint_fast8_t swrmode = 1;
	#else
		//static const uint_fast8_t swrmode = 0;
	#endif
#else /* WITHBARS */
	uint_fast8_t swrmode = 0;
#endif /* WITHBARS */

#if WITHVOLTLEVEL && ! WITHREFSENSOR

	// Напряжение fullscale = VREF * 5.3 = 3.3 * 5.3 = 17.5 вольта: сверху 4.3 килоом, синзу 1.0 килоом
	// в схеме датчика делитель: сверху 4.3 килоома, снизу 1 килоом.
	// ADCVREF_CPU - в сотнях милливольт.

	uint_fast8_t voltcalibr100mV = (ADCVREF_CPU * (VOLTLEVEL_UPPER + VOLTLEVEL_LOWER) + VOLTLEVEL_LOWER / 2) / VOLTLEVEL_LOWER;		// Напряжение fullscale - что показать при ADCVREF_CPU вольт на входе АЦП

#endif /* WITHVOLTLEVEL && ! WITHREFSENSOR */
#if (WITHCURRLEVEL || WITHCURRLEVEL2)

	// Корректировка показаний измерителя тока оконечного каскада
	#define IPACALI_RANGE 500
	#define IPACALI_BASE (IPACALI_RANGE / 2)
	static int_fast32_t getipacalibase(void)
	{
		return - IPACALI_BASE;
	}
	static uint_fast16_t gipacali = IPACALI_BASE;
#endif /* (WITHCURRLEVEL || WITHCURRLEVEL2) */
#if WITHDIRECTFREQENER
	static uint_fast8_t editfreqmode;		/* Режим прямого ввода частоты */
	static uint_fast32_t editfreq;		/* значение частоты, которое редактируем */
	static uint_fast8_t blinkpos;		/* позиция редактирования (степень 10) */
	static uint_fast8_t blinkstate = 1;		/* В позиции редактирования показ подчёркивания (0 - пробел) */
#endif /* WITHDIRECTFREQENER */

#if WITHRFSG
	static uint_fast8_t userfsg;		/* включено управление ВЧ сигнал-генератором */
#else /* WITHRFSG */
	enum { userfsg = 0 };
#endif /* WITHRFSG */

static uint_fast8_t gmoderows [2];		/* индексом используется результат функции getbankindex_xxx(tx) */
										/* номер режима работы в маске (номер тройки бит) */
static uint_fast8_t gmodecolmaps [2] [MODEROW_COUNT];	/* индексом 1-й размерности используется результат функции getbankindex_xxx(tx) */
#if WITHSPKMUTE
	static uint_fast8_t gmutespkr;		/*  выключение динамика */
	static const struct paramdefdef xgmutespkr =
	{
		QLABEL2("SPK MUTE", "SPK MUTE"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gmutespkr),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmutespkr,
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
#endif /* WITHSPKMUTE */
										/* маска режимов работы (тройки бит, указывают номер позиции в каждой строке) */
#if WITHTX
#if (WITHTHERMOLEVEL || WITHTHERMOLEVEL2)
	static uint_fast8_t gtempvmax = 55;		/* порог срабатывания защиты по температуре */
#if defined (WITHHEATPROT)
	static uint_fast8_t gheatprot = WITHHEATPROT;	/* защита от перегрева */
#else /* defined (WITHHEATPROT) */
	static uint_fast8_t gheatprot = 1;	/* защита от перегрева */
#endif /* defined (WITHHEATPROT) */
#endif /* (WITHTHERMOLEVEL || WITHTHERMOLEVEL2) */
#if (WITHSWRMTR || WITHSHOWSWRPWR)
#if defined (WITHSWRPROT)
	static uint_fast8_t gswrprot = WITHSWRPROT;
#else /* defined (WITHSWRPROT) */
	static uint_fast8_t gswrprot = 1;	/* защита от превышения КСВ */
#endif /* defined (WITHSWRPROT) */
#endif /* (WITHSWRMTR || WITHSHOWSWRPWR) */
	static uint_fast8_t gdownatcwtune;	/* снижаем мощность до "тюнерной" при нажатии TUNE */

	static const struct paramdefdef xgdownatcwtune =
	{
		QLABEL("TUNE LP"), 8, 3, RJ_ON,	ISTEP1,	/* снижаем мощность до "тюнерной" при нажатии TUNE */
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gdownatcwtune),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gdownatcwtune,
		getzerobase,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};

#if (WITHSWRMTR || WITHSHOWSWRPWR)
	static const struct paramdefdef xgswrprot =
	{
		QLABEL2("SWR PROT", "SWR Protect"), 7, 0, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,						/* защита от превышения КСВ */
		OFFSETOF(struct nvmap, gswrprot),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gswrprot,
		getzerobase,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	static const struct paramdefdef xminforward =
	{
		QLABEL("FWD LOWR"), 7, 0, RJ_UNSIGNED, ISTEP1,		/* нечувствительность SWR-метра */
		ITEM_VALUE,
		1, (1U << HARDWARE_ADCBITS) - 1,
		OFFSETOF(struct nvmap, minforward),
		getselector0, nvramoffs0, valueoffs0,
		& minforward,
		NULL,
		getzerobase,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	static const struct paramdefdef xmaxpwrcali =
	{
		QLABEL("PWR CALI"), 7, 0, RJ_UNSIGNED, ISTEP1,		/* калибровка PWR-метра */
		ITEM_VALUE,
		1, 255,
		OFFSETOF(struct nvmap, maxpwrcali),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& maxpwrcali,
		getzerobase,
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
#endif /* */

#else /* WITHTX */

	enum { gdownatcwtune = 0 };
	enum { gheatprot = 0, gtempvmax = 99 };

#endif /* WITHTX */


#if (WITHSWRMTR || WITHSHOWSWRPWR)
#if WITHAFSPECTRE
	static uint_fast8_t gsmetertype = SMETER_TYPE_BARS;
#else
	static uint_fast8_t gsmetertype = SMETER_TYPE_DIAL;	/* выбор внешнего вида прибора - стрелочный или градусник */
#endif /* WITHAFSPECTRE */

static size_t getvaltextsmetertype(char * buff, size_t count, int_fast32_t value)
{
	static const char msg_dial [] = "DIAL";
	static const char msg_bars [] = "BARS";
	return local_snprintf_P(buff, count, "%s", value ? msg_dial : msg_bars);
}

static const struct paramdefdef xgsmetertype =
{
	QLABEL2("SMETER", "S-meter Type"), 7, 3, RJ_CB,	ISTEP1,
	ITEM_VALUE | ITEM_LISTSELECT,
	0, 1,							/* выбор внешнего вида прибора - стрелочный или градусник */
	OFFSETOF(struct nvmap, gsmetertype),
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& gsmetertype,
	getzerobase, /* складывается со смещением и отображается */
	getvaltextsmetertype, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif /* (WITHSWRMTR || WITHSHOWSWRPWR) */

static uint_fast8_t gmenuset; 	/* номер комплекта функций на кнопках (переключается кнопкой MENU) */
static uint_fast8_t dimmflag;	/* не-0: притушить дисплей. */
static uint_fast8_t sleepflag;	/* не-0: выключить дисплей и звук. */
static uint_fast8_t gblinkphase;

uint_fast8_t actpageix(void)
{
	if ((dimmflag || sleepflag || dimmmode))
		return display_getpagesleep();
	return gmenuset;
}

/* состояние для мерцающих индикаторов на диспле */
uint_fast8_t hamradio_get_blinkphase(void)
{
	return gblinkphase;
}

uint_fast8_t hamradio_get_classa(void)
{
#if WITHPACLASSA && WITHTX
	return param_getvalue(& xgclassamode);
#else /* WITHPACLASSA */
	return 0;
#endif /* WITHPACLASSA */
}
// текущее состояние LOCK
uint_fast8_t
hamradio_get_lockvalue(void)
{
	return glock;
}

// текущее состояние FAST
uint_fast8_t
hamradio_get_usefastvalue(void)
{
	return gusefast;
}

/* поддержка ABOUT: частота процессора */
static int_fast32_t getcpufreqbase(void)
{
	return CPU_FREQ / 1000000;
}

static const char months [13] [4] =
{
	"---",
	"JAN",
	"FEB",
	"MAR",
	"APR",
	"MAY",
	"JUN",
	"JUL",
	"AUG",
	"SEP",
	"OCT",
	"NOV",
	"DEC",
};

static size_t getvaltextmonth(char * buff, size_t count, int_fast32_t value)
{
	return local_snprintf_P(buff, count, "%s", months [value]);
}

static size_t getvaltextcompiled(char * buff, size_t count, int_fast32_t value)
{
	uint_fast16_t year;
	uint_fast8_t month, day;
	uint_fast8_t hour, minute, seconds;

	board_get_compile_datetime(& year, & month, & day, & hour, & minute, & seconds);
	return local_snprintf_P(buff, count, "%02d-%3.3s-%02d", day, months [month], year % 100);
}

static size_t getvaltextserialnum(char * buff, size_t count, int_fast32_t value)
{
	uint_fast32_t serialnr;

	board_get_serialnr(& serialnr);
	return local_snprintf_P(buff, count, "%04X:%04X", (serialnr >> 16) & 0xFFFF, (serialnr >> 0) & 0xFFFF);
}

static size_t getvaltextcputype(char * buff, size_t count, int_fast32_t value)
{
	const char * msg;
#if defined (WITHCPUNAME)
	msg = PSTR(WITHCPUNAME);
#elif CPUSTYLE_STM32MP1
	RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_BSECEN;
	(void) RCC->MP_APB5ENSETR;
	RCC->MP_APB5LPENSETR = RCC_MP_APB5LPENSETR_BSECLPEN;
	(void) RCC->MP_APB5LPENSETR;

	const unsigned rpn = ((* (volatile uint32_t *) RPN_BASE) & RPN_ID_Msk) >> RPN_ID_Pos;
	switch (rpn)
	{
	case 0x24: 	msg = PSTR("STM32MP153Cx"); break;
	case 0x25: 	msg = PSTR("STM32MP153Ax"); break;
	case 0xA4: 	msg = PSTR("STM32MP153Fx"); break;
	case 0xA5: 	msg = PSTR("STM32MP153Dx"); break;
	case 0x00: 	msg = PSTR("STM32MP157Cx"); break;
	case 0x01: 	msg = PSTR("STM32MP157Ax"); break;
	case 0x80: 	msg = PSTR("STM32MP157Fx"); break;
	case 0x81:	msg = PSTR("STM32MP157Dx"); break;
	default: 	msg = PSTR("STM32MP15xxx"); break;
	}
#elif CPUSTYLE_XC7Z
	msg = PSTR("ZYNQ 7020");
#elif CPUSTYLE_R7S721
	msg = PSTR("RENESAS");
#else
	msg = PSTR("CPUxxx");
#endif
	return local_snprintf_P(buff, count, "%s", msg);
}

static const struct paramdefdef xgcpufreq =
{
	QLABEL("CPU FREQ"), 7, 0, 0, 	ISTEP_RO,	// частота процессора
	ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
	0, 0,
	MENUNONVRAM,
	getselector0, nvramoffs0, valueoffs0,
	& gzero,
	NULL,
	getcpufreqbase,
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};

static const struct paramdefdef xgcputype =
{
	QLABEL("CPU TYPE"), 7, 0, RJ_CB, 	ISTEP_RO,	// тип процессора
	ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
	0, 0,
	MENUNONVRAM,
	getselector0, nvramoffs0, valueoffs0,
	& gzero,
	NULL,
	getzerobase,
	getvaltextcputype, /* getvaltext получить текст значения параметра - see RJ_CB */
};

#ifdef DDR_FREQ
/* поддержка ABOUT: частота памяти */
static int_fast32_t getddrfreqbase(void)
{
	return DDR_FREQ / 1000000;
}
#endif /* DDR_FREQ */

#ifdef AXISS_FREQ
/* поддержка ABOUT: частота шины */
static int_fast32_t getaxissfreqbase(void)
{
	return AXISS_FREQ / 1000000;
}
#endif /* AXISS_FREQ */

#if WITHLFM

static int_fast32_t getlfmbias(void)
{
	return - LFMFREQBIAS;
}

#endif /* WITHLFM */
//
//#define ADCOFFSETMID (512 / 2)
//static int_fast32_t getadcoffsbase(void)
//{
//	return - ADCOFFSETMID;
//}


#if defined (REFERENCE_FREQ)

	static const int_fast32_t refbase = REFERENCE_FREQ - OSCSHIFT;

#if defined (REALREFERENCE_FREQ)
	static uint_fast16_t refbias = OSCSHIFT - (REFERENCE_FREQ - REALREFERENCE_FREQ);
#else /* defined (REALREFERENCE_FREQ) */
	static uint_fast16_t refbias = OSCSHIFT;
#endif /* defined (REALREFERENCE_FREQ) */

	int_fast32_t getrefbase(void)
	{
		return refbase;
	}
#endif /* defined (REFERENCE_FREQ) */

static uint_fast8_t gkeybeep10 = 880 / 10;	/* озвучка нажатий клавиш - 880 Гц - нота ля второй октавы (A5) (аналогично FT1000) */


#if WITHMIC1LEVEL
	static uint_fast16_t gmik1level = (WITHMIKEINGAINMAX - WITHMIKEINGAINMIN) / 4 + WITHMIKEINGAINMIN;
	/* подстройка усиления микрофонного усилителя через меню. */
	static const struct paramdefdef xgmik1level =
	{
		QLABEL2("MIC LEVL", "MIC Level"), 7, 0, RJ_UNSIGNED, ISTEP1,
		ITEM_VALUE,
		WITHMIKEINGAINMIN, WITHMIKEINGAINMAX,
		OFFSETOF(struct nvmap, gmik1level),	/* усиление микрофонного усилителя */
		getselector0, nvramoffs0, valueoffs0,
		& gmik1level,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
#endif /* WITHMIC1LEVEL */
#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_NAU8822L)
	uint_fast8_t ALCNEN = 0;	// ALC noise gate function control bit
	uint_fast8_t ALCNTH = 0;	// ALC noise gate threshold level
	uint_fast8_t ALCEN = 0;	// only left channel ALC enabled
	uint_fast8_t ALCMXGAIN = 7;	// Set maximum gain limit for PGA volume setting changes under ALC control
	uint_fast8_t ALCMNGAIN = 0;	// Set minimum gain value limit for PGA volume setting changes under ALC control
#endif /* defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_NAU8822L) */
#if WITHIF4DSP
#if WITHTX
	// See HARDWARE_DACSCALE
	static uint_fast16_t gdesignscale = 1000;		/* используется при калибровке параметров интерполятора */
	#if WITHTXCPATHCALIBRATE
	const struct paramdefdef xgdesignscale = {
		QLABELENC2("TX CALIBR"), 0, 3, RJ_UNSIGNED, ISTEP1,
		ITEM_VALUE,
		0, 2500,		/* используется при калибровке параметров интерполятора */
		OFFSETOF(struct nvmap, gdesignscale),
		getselector0, nvramoffs0, valueoffs0,
		& gdesignscale,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	};
	#endif /* WITHTXCPATHCALIBRATE */
	#if WITHTXCPATHCALIBRATE
		static uint_fast16_t ggaincwtx = 100;		/* Увеличение усиления при передаче в CW режимах 100..300% */
		static uint_fast16_t ggaindigitx = 100;		/* Увеличение усиления при передаче в цифровых режимах 100..300% */
	#elif 1//WITHTXCWREDUCE
		static uint_fast16_t ggaincwtx = 60;		/* Увеличение усиления при передаче в CW режимах 100..300% */
		static uint_fast16_t ggaindigitx = 160;		/* Увеличение усиления при передаче в цифровых режимах 100..300% */
	#else /* WITHTXCWREDUCE */
		static uint_fast16_t ggaincwtx = 100;		/* Увеличение усиления при передаче в CW режимах 100..300% */
		static uint_fast16_t ggaindigitx = 160;		/* Увеличение усиления при передаче в цифровых режимах 100..300% */
	#endif /* WITHTXCWREDUCE */
	static uint_fast8_t gamdepth = 30;		/* Глубина модуляции в АМ - 0..100% */
	/* Увеличение усиления при передаче в цифровых режимах 90..300% */
	static const struct paramdefdef xggaindigitx =
	{
		QLABEL2("FT8BOOST", "FT8 Boost"),	7, 2, RJ_UNSIGNED, ISTEP1,		/* Увеличение усиления при передаче в цифровых режимах 90..300% */
		ITEM_VALUE,
		90, 300,
		OFFSETOF(struct nvmap, ggaindigitx),
		getselector0, nvramoffs0, valueoffs0,
		& ggaindigitx,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	static uint_fast16_t gtxtimer;
	static uint_fast16_t gtxtot = 300;		/* разрешённое время передачи */
	/* разрешённое время передачи */
	static const struct paramdefdef xgtxtot =
	{
		QLABEL2("TX TOUT", "TX Timeout"), 7, 0, RJ_UNSIGNED, ISTEP5,		/* разрешённое время передачи */
		ITEM_VALUE,
		0, 300,
		OFFSETOF(struct nvmap, gtxtot),	/* разрешённое время передачи */
		getselector0, nvramoffs0, valueoffs0,
		& gtxtot,	// uint_fast16_t value pointer
		NULL,	// uint_fast8_t value pointer
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	/* Глубина модуляции в АМ - 0..100% */
	static const struct paramdefdef xgamdepth =
	{
		QLABEL2("AM DEPTH", "AM Depth"), 7, 0, RJ_UNSIGNED, ISTEP1,		/* Подстройка глубины модуляции в АМ */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gamdepth),	/* Глубина модуляции в АМ - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,	// uint_fast16_t value pointer
		& gamdepth,	// uint_fast8_t value pointer
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	/* Увеличение усиления при передаче в цифровых режимах 100..300% */
	static const struct paramdefdef xggaincwtx =
	{
		QLABEL2("CW BOOST", "CW Boost"),	7, 2, RJ_UNSIGNED, ISTEP1,		/* Увеличение усиления при передаче в CW режимах 30..150% */
		ITEM_VALUE,
		30, 150,
		OFFSETOF(struct nvmap, ggaincwtx),
		getselector0, nvramoffs0, valueoffs0,
		& ggaincwtx,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	/*  Использование амплитуды сигнала с ЦАП передатчика - 0..100% */
	#if defined (WITHDEFDACSCALE)
		static uint_fast8_t gdacscale = WITHDEFDACSCALE;	/* настраивается под прегруз драйвера. */
	#else /* defined (WITHDEFDACSCALE) */
//#warning Limited WITHDEFDACSCALE value used
		static uint_fast8_t gdacscale = 100;	/* настраивается под прегруз драйвера. */
	#endif /* defined (WITHDEFDACSCALE) */

#endif /* WITHTX */

	#define FSADCPOWEROFFSET10 700
	static int_fast32_t getfsasdcbase10(void)
	{
		return - FSADCPOWEROFFSET10;
	}

#if WITHWAVPLAYER || WITHSENDWAV
	static uint_fast8_t gloopmsg, gloopsec = 15;
	static uint_fast8_t loopticks;
	static const char * const loopnames [] =
	{
			"none",
			"1.wav",
			"2.wav",
			"3.wav",
			"4.wav",
			"5.wav",
	};
	void playhandler(uint8_t code);
#endif /* WITHWAVPLAYER || WITHSENDWAV */

#if  defined (ADC1_TYPE) && (ADC1_TYPE == ADC_TYPE_AD9246)
	// 14 bit AD9246 + LTC6401-20
	static uint_fast8_t gsidetonelevel = 15;	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
	static uint_fast8_t gdigigainmax = 86;	/* диапазон ручной регулировки цифрового усиления - максимальное значение */
	static uint_fast16_t gfsadcpower10 [2] =
	{
		(-  30) + FSADCPOWEROFFSET10,	// для соответствия HDSDR мощность, соответствующая full scale от IF ADC
		(- 330) + FSADCPOWEROFFSET10,	// с конвертором
	};
#else /*  */
	// 16 bit LTC2208 + LTC6401-20
	static uint_fast8_t gsidetonelevel = 15;	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
	static uint_fast8_t gdigigainmax = 120;	/* диапазон ручной регулировки цифрового усиления - максимальное значение */
	static uint_fast16_t gfsadcpower10 [2] =
	{
		(- 30) + FSADCPOWEROFFSET10,	// для соответствия HDSDR мощность, соответствующая full scale от IF ADC
		(- 230) + FSADCPOWEROFFSET10,	// с конвертором
	};

#endif /*  */
	static const struct paramdefdef xgfsadcpower10 =
	{
		QLABEL("ADC FS"), 3 + WSIGNFLAG, 1, RJ_SIGNED,	ISTEP1,		/* Калиьровка S-метра - момент перегрузки */
		ITEM_VALUE,
		0, FSADCPOWEROFFSET10 * 2, 		// -50..+50 dBm
		OFFSETOF(struct nvmap, gfsadcpower10 [0]),
		getselector0, nvramoffs0, valueoffs0,
		& gfsadcpower10 [0],	// 16 bit
		NULL,
		getfsasdcbase10, /* складывается со смещением и отображается */
		NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};
	static const struct paramdefdef xgfsadcpower10xv =
	{
			QLABEL("ADC FSXV"), 3 + WSIGNFLAG, 1, RJ_SIGNED,	ISTEP1,		/* с колнвертором Калиьровка S-метра - момент перегрузки */
			ITEM_VALUE,
			0, FSADCPOWEROFFSET10 * 2, 		// -50..+50 dBm
			OFFSETOF(struct nvmap, gfsadcpower10 [1]),
			getselector0, nvramoffs0, valueoffs0,
			& gfsadcpower10 [1],	// 16 bit
			NULL,
			getfsasdcbase10, /* складывается со смещением и отображается */
			NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
	};

	static uint_fast8_t gmoniflag;		/* разрешение самопрослушивания */

	#if WITHDSPEXTDDC	/* "Воронёнок" с DSP и FPGA */
		static uint_fast8_t gdither;		/* управление зашумлением в LTC2208 */
		#if (ADC1_TYPE == ADC_TYPE_AD9246)
			static const uint_fast8_t gadcrand = 0;		/* управление интерфейсом в LTC2208 */
		#else /* ADC1_TYPE == ADC_TYPE_AD9246 */
			static const uint_fast8_t gadcrand = 1;		/* управление интерфейсом в LTC2208 */
		#endif /* ADC1_TYPE == ADC_TYPE_AD9246 */

		static uint_fast8_t gdactest;
		#if WITHDACSTRAIGHT
			static const uint_fast8_t gdacstraight = 1;	/* Требуется формирование кода для ЦАП в режиме беззнакового кода */
		#else /* WITHDACSTRAIGHT */
			static const uint_fast8_t gdacstraight = 0;	/* Требуется формирование кода для ЦАП в режиме знакового кода */
		#endif /* WITHDACSTRAIGHT */
		#if WITHTXINHDISABLE
			static const uint_fast8_t gtxinhenable = 0;	/* запрещение реакции на вход tx_inh */
		#else /* WITHTXINHDISABLE */
			static const uint_fast8_t gtxinhenable = 1;	/* разрешение реакции на вход tx_inh */
		#endif /* WITHTXINHDISABLE */
	#endif /* WITHDSPEXTDDC */

#endif /* WITHIF4DSP */

#if WITHMODEM
	static const uint_fast32_t modembr2int100 [] =	// Индекс в этой таблице хранится в gmodemspeed
	{
		3125,			// 31.25 baud		#0
		6250,			// 62.5 baud		#1
		125 * 100uL,	// 125 baud			#2
		150 * 100uL,	// 150 baud			#3
		250 * 100uL,	// 125 baud			#4
		300 * 100uL,	// 300 baud			#5
		500 * 100uL,	// 500 baud			#6
		600 * 100uL,	// 600 baud			#7
		750 * 100uL,	// 750 baud			$8
		1000 * 100uL,	// 1000 (1k) baud	#9
		1200 * 100uL,	// 1200 (1.2k) baud	#10
		2000 * 100uL,	// 2k baud			#11
		2400 * 100uL,	// 2,4k baud		#12
		3600 * 100uL,	// 3,6k baud		#13
		4000 * 100uL,	// 4,0k baud		#14
		4800 * 100uL,	// 4,8k baud		#15
		5000 * 100uL,	// 5,0k baud		#16
		5600 * 100uL,	// 5,6k baud		#17
		6000 * 100uL,	// 6,0k baud		#18
		6400 * 100uL,	// 6,4k baud		#19
		8000 * 100uL,	// 8,0k baud		#20
		9200 * 100uL,	// 9,2k baud		#21
		9600 * 100uL,	// 9,6k baud		#22
	};

	static uint_fast8_t gmodemspeed = 20;	// индекс в таблице скоростей передачи - #5: 300 baud, #7: 600 baud, #9: 1k baud
	static uint_fast8_t gmodemmode;		// применяемая модуляция

	uint_fast32_t hamradio_get_modem_baudrate100(void)
	{
		return modembr2int100 [gmodemspeed];
	}

#endif /* WITHMODEM */

static const uint_fast16_t actbring_time = 10;	// 1 second
static const uint_fast16_t swrbring_time = BOARD_ERRBEEP_LENGTH / 100;	// 0.6 second

static uint_fast16_t actbring_afvolume;
// Начать отображение текущего положения регулировки AF
static void bring_afvolume(void)
{
	actbring_afvolume = actbring_time;
}

static uint_fast16_t actbring_rfvolume;

// Начать отображение текущего положения регулировки RF
static void bring_rfvolume(void)
{
	actbring_rfvolume = actbring_time;
}

static const char * bring_swr_text = "---";
static uint_fast16_t actbring_swr;

// Начать отображение перегруза по SWR
static void bring_swr(const char * label)
{
	PRINTF("bring_swr: %s\n", label);
	bring_swr_text = label;
	actbring_swr = swrbring_time;
	board_errbeep_enable(1);
}

///


static uint_fast16_t actbring_ENC1F;
static uint_fast16_t actbring_ENC2F;
static uint_fast16_t actbring_ENC3F;
static uint_fast16_t actbring_ENC4F;

static void bring_enc1f(void)
{
	actbring_ENC1F = actbring_time;
}

static void bring_enc2f(void)
{
	actbring_ENC2F = actbring_time;
}

static void bring_enc3f(void)
{
	actbring_ENC3F = actbring_time;
}

static void bring_enc4f(void)
{
	actbring_ENC4F = actbring_time;
}

// Вращали 3-й валкодер (ENC1F)
uint_fast8_t hamradio_get_bringENC1F(void)
{
	return actbring_ENC1F != 0;
}
// Вращали 3-й валкодер (ENC2F)
uint_fast8_t hamradio_get_bringENC2F(void)
{
	return actbring_ENC2F != 0;
}
// Вращали 3-й валкодер (ENC3F)
uint_fast8_t hamradio_get_bringENC3F(void)
{
	return actbring_ENC3F != 0;
}
// Вращали 3-й валкодер (ENC4F)
uint_fast8_t hamradio_get_bringENC4F(void)
{
	return actbring_ENC4F != 0;
}
// Была ошибка SWR ли что-то другое
uint_fast8_t hamradio_get_bringSWR(const char * * label)
{
	* label = bring_swr_text;
	return actbring_swr != 0;
}

///


typedef struct encfnitem_tag
{
	int v;
	const char * label;
} encfnitem_t;

static const encfnitem_t enclabelsENC1FN [] =
{
		{
			0,
			"AF gain"
		},
		{
			0,
			"RF gain"
		},
};

static const encfnitem_t enclabelsENC2FN [] =
{
		{
			0,
			"ENC2F"
		},
};

static const encfnitem_t enclabelsENC3FN [] =
{
		{
			0,
			"ENC3F"
		},
};

static const encfnitem_t enclabelsENC4FN [] =
{
		{
			0,
			"ENC4F"
		},
};

static uint_fast8_t enc1f_sel;
static uint_fast8_t enc2f_sel;
static uint_fast8_t enc3f_sel;
static uint_fast8_t enc4f_sel;

/* получить надпись для отображения состояние ENC1F */
void hamradio_get_label_ENC1F(uint_fast8_t active, char * buff, size_t count)
{
	local_snprintf_P(buff, count, "%s", enclabelsENC1FN [enc1f_sel].label);
}

/* получить надпись для отображения состояние ENC2F */
void hamradio_get_label_ENC2F(uint_fast8_t active, char * buff, size_t count)
{
	local_snprintf_P(buff, count, "%s", enclabelsENC2FN [enc2f_sel].label);
}

/* получить надпись для отображения состояние ENC3F */
void hamradio_get_label_ENC3F(uint_fast8_t active, char * buff, size_t count)
{
	local_snprintf_P(buff, count, "%s", enclabelsENC3FN [enc3f_sel].label);
}

/* получить надпись для отображения состояние ENC4F */
void hamradio_get_label_ENC4F(uint_fast8_t active, char * buff, size_t count)
{
	local_snprintf_P(buff, count, "%s", enclabelsENC4FN [enc4f_sel].label);
}

///
static uint_fast16_t actbring_tuneA;

// Начать отображение текущей частоты на водопаде
static void bring_tuneA(void)
{
	actbring_tuneA = actbring_time;
}

// Разрешить отображение текущей частоты на водопаде
uint_fast8_t hamradio_get_bringtuneA(void)
{
	return actbring_tuneA != 0;
}

///
static uint_fast16_t actbring_tuneB;

// Начать отображение текущей частоты на водопаде
static void bring_tuneB(void)
{
	actbring_tuneB = actbring_time;
}

// Разрешить отображение текущей частоты на водопаде
uint_fast8_t hamradio_get_bringtuneB(void)
{
	return actbring_tuneB != 0;
}

static void bringtimers(void)
{
	actbring_afvolume = actbring_afvolume ? (actbring_afvolume - 1) : 0;
	actbring_rfvolume = actbring_rfvolume ? (actbring_rfvolume - 1) : 0;
	actbring_tuneA = actbring_tuneA ? (actbring_tuneA - 1) : 0;
	actbring_tuneB = actbring_tuneB ? (actbring_tuneB - 1) : 0;
#if WITHTX
	actbring_swr = actbring_swr ? (actbring_swr - 1) : 0;
	board_errbeep_enable(actbring_swr != 0);
#endif /* WITHTX */
	actbring_ENC1F = actbring_ENC1F ? (actbring_ENC1F - 1) : 0;	// енкодер ENC1F
	actbring_ENC2F = actbring_ENC2F ? (actbring_ENC2F - 1) : 0;	// енкодер ENC2F
	actbring_ENC3F = actbring_ENC3F ? (actbring_ENC3F - 1) : 0;	// енкодер ENC3F
	actbring_ENC4F = actbring_ENC4F ? (actbring_ENC4F - 1) : 0;	// енкодер ENC4F

}

#if WITHTX

// SWR=1 = озвращаем 0
uint_fast16_t tuner_get_swr0(uint_fast16_t fullscale, adcvalholder_t * pr, adcvalholder_t * pf)
{
	const uint_fast8_t fs = fullscale - TUS_SWRMIN;
	adcvalholder_t r;
	const adcvalholder_t f = board_getswrpair_filtered_tuner(& r, swrcalibr);

#if WITHSWRMTR
	// обновить кеш данных для дисплея
	board_adc_store_data(PWRMRRIX, f);
	board_adc_store_data(FWDMRRIX, f);
	board_adc_store_data(REFMRRIX, r);
#endif /* WITHSWRMTR */

	* pr = r;
	* pf = f;

	if (f < minforward)
		return 0;	// SWR=1
	else if (f <= r)
		return fs;		// SWR is infinite

	const uint_fast16_t swr10 = (uint_fast32_t) (f + r) * TUS_SWRMIN / (f - r) - TUS_SWRMIN;
	return swr10 > fs ? fs : swr10;
}

// Used with WITHMGLOOP
unsigned get_swr_cached(unsigned rangemax)
{
	uint_fast16_t swr10; 		// swr10 = 0..30 for swr 1..4
	adcvalholder_t forward, reflected;

	ASSERT(rangemax > SWRMIN);
	forward = board_getswrmeter_cached(& reflected, swrcalibr);

	// рассчитанное  значение
	if (forward < minforward)
		swr10 = SWRMIN;				// SWR=1
	else if (forward <= reflected)
		swr10 = rangemax;		// SWR is infinite
	else
		swr10 = (forward + reflected) * SWRMIN / (forward - reflected);
	return swr10;
}

// Used with WITHMGLOOP
unsigned hamradio_get_pwr(void)
{
	uint_fast8_t pwrtrace;
	uint_fast8_t pwr = board_getpwrmeter(& pwrtrace);

	if (pwrtrace > maxpwrcali)
		pwrtrace = maxpwrcali;

	return pwrtrace * 100 / maxpwrcali;
}
#endif /* WITHTX */

#if WITHAUTOTUNER

enum phases
{
	PHASE_ABORT,
	PHASE_DONE,
	PHASE_CONTINUE
};

static uint_fast8_t tuner_bg;
static uint_fast8_t tuner_ant;

static void board_set_tuner_group(void)
{
	//PRINTF(PSTR("tuner: CAP=%-3d, IND=%-3d, TYP=%d\n"), tunercap, tunerind, tunertype);
	// todo: добавить учет включенной антенны
	board_set_tuner_C(tunercap);
	board_set_tuner_L(tunerind);
	board_set_tuner_type(tunertype);
	board_set_tuner_bypass(! tunerwork);
}

// выдача параметров на тюнер
static void updateboard_tuner(void)
{
	//PRINTF(PSTR("updateboard_tuner: CAP=%-3d, IND=%-3d, TYP=%d\n"), tunercap, tunerind, tunertype);
	board_set_tuner_group();
	board_update();		/* вывести забуферированные изменения в регистры */
}

// ожидание требуемого времени после выдачи параметров на тюнер.
static void tuner_waitadc(void)
{
	uint_fast8_t n = (gtunerdelay + 4) / 5;
	while (n --)
		local_delay_ms(5);
}


// SWR=1: return 100
// no power: return 0
unsigned n7ddc_get_swr(void)
{
	const uint_fast8_t fs = 900;
	adcvalholder_t r;
	const adcvalholder_t f = board_getswrpair_filtered_tuner(& r, swrcalibr);

	// обновить кеш данных для дисплея
	{
		board_adc_store_data(PWRMRRIX, f);
		board_adc_store_data(FWDMRRIX, f);
		board_adc_store_data(REFMRRIX, r);
	}

	if (f < minforward)
	{
		PRINTF("n7ddc_get_swr: No forward power (f=%u,r=%u,mf=%d),L=%u,C=%u,T=%u\n", f, r, minforward, tunerind, tunercap, tunertype);
		return 0;	// алгоритм тюнера рассматривает эту ситуацию как "нет сигнала"
	}
	else if (f <= r)
		return fs;		// SWR is infinite

	const uint_fast16_t swr10 = (uint_fast32_t) (f + r) * 100 / (f - r);
	unsigned result = swr10 > fs ? fs : swr10;
	PRINTF("n7ddc_get_swr: swr=%u (f=%u,r=%u),L=%u,C=%u,T=%u\n", result, f, r, tunerind, tunercap, tunertype);
	return result;
}

static void printtunerstate(const char * title, uint_fast16_t swr, adcvalholder_t r, adcvalholder_t f)
{
	PRINTF("%s: L=%u,C=%u,ty=%u,fw=%u,ref=%u,swr=%u.%02u\n",
		title,
		(unsigned) tunerind, (unsigned) tunercap, (unsigned) tunertype,
		(unsigned) f,
		(unsigned) r,
		(unsigned) (swr + TUS_SWRMIN) / TUS_SWRMIN,
		(unsigned) (swr + TUS_SWRMIN) % TUS_SWRMIN);
}

uint_fast16_t tuner_get_swr(const char * title, uint_fast16_t fullscale, adcvalholder_t * pr, adcvalholder_t * pf)
{
	adcvalholder_t r;
	adcvalholder_t f;
	const uint_fast16_t swr = tuner_get_swr0(fullscale, & r, & f);

	* pr = r;
	* pf = f;
	printtunerstate(title, swr, r, f);
	return swr;
}

// Если прервана настройка - возврат не-0
static uint_fast8_t tuneabort(void)
{
	uint_fast16_t kbch;
	uint_fast8_t kbready;

	// todo: не работает на дисплеях с off screen composition.
	// счетчик перебора сбрасывается в 0 - и до обновления экрана дело не доходит.

	txreq_process();	/* обработка запросов */
	processmessages(& kbch, & kbready);
	if (! txreq_getreqautotune(& txreqst0))
		return 1;
	if (kbready != 0)
	{
		switch (kbch)
		{
		case KBD_CODE_ATUBYPASS:
		case KBD_CODE_ATUSTART:
			return 1;
		}
	}
	return 0;
}

static void storetuner(uint_fast8_t bg, uint_fast8_t ant)
{
	PRINTF("storetuner: L=%u,C=%u,T=%u\n", tunerind, tunercap, tunertype);
	save_i8(OFFSETOF(struct nvmap, bandgroups [bg].otxants [ant].tunercap), tunercap);
	save_i8(OFFSETOF(struct nvmap, bandgroups [bg].otxants [ant].tunerind), tunerind);
	save_i8(OFFSETOF(struct nvmap, bandgroups [bg].otxants [ant].tunertype), tunertype);
	save_i8(OFFSETOF(struct nvmap, bandgroups [bg].otxants [ant].tunerwork), tunerwork);
}

static void loadtuner(uint_fast8_t bg, uint_fast8_t ant)
{
	tunercap = loadvfy8up(OFFSETOF(struct nvmap, bandgroups [bg].otxants [ant].tunercap), CMIN, CMAX, tunercap);
	tunerind = loadvfy8up(OFFSETOF(struct nvmap, bandgroups [bg].otxants [ant].tunerind), LMIN, LMAX, tunerind);
	tunertype = loadvfy8up(OFFSETOF(struct nvmap, bandgroups [bg].otxants [ant].tunertype), 0, KSCH_COUNT - 1, tunertype);
	tunerwork = loadvfy8up(OFFSETOF(struct nvmap, bandgroups [bg].otxants [ant].tunerwork), 0, 1, 0);	// в новых диапазонах - тоюнер не включаем по умолчанию
}

#if WITHAUTOTUNER_N7DDCALGO

void n7ddc_settuner(unsigned inductors, unsigned capcitors, unsigned type)
{
	tunerind = inductors;
	tunercap = capcitors;
	tunertype = type;

	updateboard_tuner();
	local_delay_ms(gtunerdelay);
}


static void auto_tune0_init(void)
{
	const uint_fast8_t tx = 1;
	const uint_fast8_t bi = getbankindex_tx(tx);
	const uint_fast32_t freq = gfreqs [bi];
	tuner_bg = getfreqbandgroup(freq);
	tuner_ant = geteffantenna(freq);
}

// Обновление изображения в процессе выполнения согласования
// non-zero for cancel tuning process
static int n7ddc_display(void * ctx)
{
	return tuneabort();
}

static enum phases auto_tune0(void)
{
#if WITHTX
	switch (n7ddc_tune(gn7ddclinearC, gn7ddclinearL, n7ddc_display, NULL))
	{
	default:
	case N7DDCTUNE_ABORT:
		txreq_rx(& txreqst0, "ABT");
		return PHASE_ABORT; // восстановление будет в auto_tune3
	case N7DDCTUNE_ERROR:
		txreq_rx(& txreqst0, "ERR");
		return PHASE_ABORT; // восстановление будет в auto_tune3
	case N7DDCTUNE_OK:
		return PHASE_DONE; // сохранение будет в auto_tune2
	}
#else /* WITHTX */
	txreq_rx(& txreqst0, "ABT");
	return PHASE_ABORT; // восстановление будет в auto_tune3
#endif /* WITHTX */
}

static void auto_tune1_init(void)
{
}

static enum phases auto_tune1(void)
{
	return PHASE_DONE;
}

static void auto_tune2_init(void)
{
}

// save to nvram
static void auto_tune2(void)
{
	storetuner(tuner_bg, tuner_ant);
}

#else /* WITHAUTOTUNER_N7DDCALGO */


// что удалось достичь в результате перебора
typedef struct tunerstate
{
	uint8_t tunercap, tunerind, tunertype;
	uint16_t swr;	// values 0..190: SWR = 1..20
	adcvalholder_t f, r;
} tus_t;

static tus_t tunerstatuses [KSCH_COUNT];

static void scanminLk_init(void)
{

}


// Перебор значений L в поиске минимума SWR
// Если прервана настройка - возврат не-0
static uint_fast8_t scanminLk(tus_t * tus)
{
	uint_fast8_t bestswrvalid = 0;

	PRINTF("scanminLk start ****************\n");
	for (tunerind = LMIN; tunerind <= LMAX; ++ tunerind)
	{
		if (tuneabort())
			return 1;
		updateboard_tuner();
		tuner_waitadc();
		adcvalholder_t r;
		adcvalholder_t f;
		const uint_fast16_t swr = tuner_get_swr("scanminLk", TUS_SWRMAX, & r, & f);

		if ((bestswrvalid == 0) || (tus->swr > swr))
		{
			// Измерений ещё небыло или это полоэение обеспечивает лучше КСВ
			tus->swr = swr;
			tus->tunerind = tunerind;
			tus->r = r;
			tus->f = f;
			bestswrvalid = 1;
			PRINTF("scanminLk: best ty=%u, L=%u, C=%u\n", tunertype, tunerind, tunercap);
		}
	}
	tunerind = tus->tunerind;	// лучшее запомненное
	PRINTF("scanminLk done ****************\n");
	return 0;
}

static void scanminCk_init(void)
{

}

// Перебор значений C в поиске минимума SWR
// Если прервана настройка - возврат не-0
static uint_fast8_t scanminCk(tus_t * tus)
{
	uint_fast8_t bestswrvalid = 0;

	PRINTF("scanminCk start ****************\n");
	for (tunercap = CMIN; tunercap <= CMAX; ++ tunercap)
	{
		if (tuneabort())
			return 1;
		updateboard_tuner();
		tuner_waitadc();
		adcvalholder_t r;
		adcvalholder_t f;
		const uint_fast16_t swr = tuner_get_swr("scanminCk", TUS_SWRMAX, & r, & f);

		if ((bestswrvalid == 0) || (tus->swr > swr))
		{
			// Измерений ещё небыло или это полоэение обеспечивает лучше КСВ
			tus->swr = swr;
			tus->tunercap = tunercap;
			tus->r = r;
			tus->f = f;
			bestswrvalid = 1;
			PRINTF("scanminCk: best ty=%u, L=%u, C=%u\n", tunertype, tunerind, tunercap);
		}
	}
	tunercap = tus->tunercap;	// лучшее запомненное
	PRINTF("scanminCk done ****************\n");
	return 0;
}

// Выбираем наилучший результат согласования
static uint_fast8_t findbestswr(const tus_t * v, uint_fast8_t n)
{
	uint_fast8_t i;
	uint_fast8_t best = 0;
	if (n < 2)
		return best;
	for (i = 1; i < n; ++ i)
	{
		if (v [i].swr < v [best].swr)
			best = i;
	}
	return best;
}

static void auto_tune0_init(void)
{
}
// 1 - aborted
static enum phases auto_tune0(void)
{
	unsigned ndummies;
	const uint_fast8_t tx = 1;
	const uint_fast8_t bi = getbankindex_tx(tx);
	const uint_fast32_t freq = gfreqs [bi];
	tuner_bg = getfreqbandgroup(freq);
	tuner_ant = geteffantenna(freq);

	PRINTF(PSTR("auto_tune start\n"));
	for (ndummies = 5; ndummies --; )
	{
		if (tuneabort())
			return PHASE_ABORT; //goto aborted;
		local_delay_ms(50);
		adcvalholder_t r;
		adcvalholder_t f;
		const uint_fast16_t swr = tuner_get_swr("dummy", TUS_SWRMAX, & r, & f);
		tuner_waitadc();
	}
	return PHASE_DONE;
}


static void auto_tune1_init(void)
{
	tunertype = 0;
}

// 1 - aborted
static enum phases auto_tune1(void)
{

	// Попытка согласовать двумя схемами

	tunerstatuses [tunertype].tunertype = tunertype;
	tunerind = LMIN;
	if (tunertype == 0)
	{
		PRINTF("tuner: ty=%u, scan capacitors\n", (unsigned) tunertype);
		scanminCk_init();
		if (scanminCk(& tunerstatuses [tunertype]) != 0)
			return PHASE_ABORT; //goto aborted;
		PRINTF("scanminCk finish: C=%u\n", tunercap);
		updateboard_tuner();
	}
	else
	{
		tunerstatuses [1] = tunerstatuses [0];
		tunerstatuses [1].tunertype = tunertype;
	}

	// проверка - а может уже нашли подходяшее согласование?
	////if (tunerstatuses [tunertype].swr <= TUS_SWR1p1)
	////	goto NoMoreTune;

	PRINTF("tuner: ty=%u, scan inductors\n", (unsigned) tunertype);
	scanminLk_init();
	if (scanminLk(& tunerstatuses [tunertype]) != 0)
		return PHASE_ABORT; //goto aborted;
	PRINTF("scanminLk finish: L=%u\n", tunerind);
	updateboard_tuner();

	// проверка - а может уже нашли подходяшее согласование?
	////if (tunerstatuses [tunertype].swr <= TUS_SWR1p1)
	////	goto NoMoreTune;

	return (++ tunertype >= KSCH_COUNT) ? PHASE_DONE : PHASE_CONTINUE;
}

static unsigned tuner_cshindex;

static void auto_tune2_init(void)
{
	// Выбираем наилучший результат согласования
	tuner_cshindex = findbestswr(tunerstatuses, sizeof tunerstatuses / sizeof tunerstatuses [0]);
	PRINTF(PSTR("auto_tune loop done\n"));
	printtunerstate("Selected 1", tunerstatuses [tuner_cshindex].swr, tunerstatuses [tuner_cshindex].r, tunerstatuses [tuner_cshindex].f);
	tunertype = tunerstatuses [tuner_cshindex].tunertype;
	tunerind = tunerstatuses [tuner_cshindex].tunerind;
	tunercap = tunerstatuses [tuner_cshindex].tunercap;
	scanminCk_init();
}

// save to nvram
static void auto_tune2(void)
{
	if (scanminCk(& tunerstatuses [tuner_cshindex]) != 0)
		return;// PHASE_ABORT; //goto aborted;
	printtunerstate("Selected 2", tunerstatuses [tuner_cshindex].swr, tunerstatuses [tuner_cshindex].r, tunerstatuses [tuner_cshindex].f);
	// Устанавливаем аппаратуру в состояние при лучшем результате
	tunertype = tunerstatuses [tuner_cshindex].tunertype;
	tunerind = tunerstatuses [tuner_cshindex].tunerind;
	tunercap = tunerstatuses [tuner_cshindex].tunercap;
	updateboard_tuner();
	PRINTF(PSTR("auto_tune stop\n"));
////NoMoreTune:
	storetuner(tuner_bg, tuner_ant);
}

#endif /* WITHAUTOTUNER_N7DDCALGO */

// aborting - restore tuner state from saved
static void auto_tune3(void)
{
	tunerwork = 1;	// всегда единица (сохранилось в начале настройки)
	tunercap = loadvfy8up(OFFSETOF(struct nvmap, bandgroups [tuner_bg].otxants [tuner_ant].tunercap), CMIN, CMAX, tunercap);
	tunerind = loadvfy8up(OFFSETOF(struct nvmap, bandgroups [tuner_bg].otxants [tuner_ant].tunerind), LMIN, LMAX, tunerind);
	tunertype = loadvfy8up(OFFSETOF(struct nvmap, bandgroups [tuner_bg].otxants [tuner_ant].tunertype), 0, KSCH_COUNT - 1, tunertype);
	updateboard_tuner();
}

#endif /* WITHAUTOTUNER */

#if WITHAUTOTUNER

static ticker_t ticker_tuner;

/* user-mode function */
static void dpc_tunertimer_fn(void * arg)
{
}

/* закончили установку нового состояния тюнера - запускаем новый период таймера */
static void tuner_eventrestart(void)
{
	ticker_setperiod(& ticker_tuner, NTICKS(gtunerdelay));
}

#endif /* WITHAUTOTUNER */

#if defined(NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING)

static void fillrelaxedsign(uint8_t * tsign)
{
	const uint_fast32_t key = sizeof (struct nvmap);
	ASSERT(sizeof nvramsign == 8);

	tsign [0] = (uint8_t) (key >> 24);
	tsign [1] = (uint8_t) (key >> 16);
	tsign [2] = (uint8_t) (key >> 8);
	tsign [3] = (uint8_t) (key >> 0);
	tsign [4] = (uint8_t) ~ (key >> 24);
	tsign [5] = (uint8_t) ~ (key >> 16);
	tsign [6] = (uint8_t) ~ (key >> 8);
	tsign [7] = (uint8_t) ~ (key >> 0);
}

/* проверка совпадения сигнатуры в энергонезависимой памяти.
   0 - совпадает,
	 не-0 - отличается
	 */
static uint_fast8_t
//NOINLINEAT
verifynvramsignature(void)
{
#if WITHKEEPNVRAM

	/* ослабить проверку совпадения версий прошивок для стирания NVRAM */
	uint8_t tsign [sizeof nvramsign];
	uint_fast8_t i;

	fillrelaxedsign(tsign);
	for (i = 0; i < (sizeof nvramsign - 1); ++ i)
	{
		const char c = restore_i8(RMT_SIGNATURE_BASE(i));
		if (c != tsign [i])
		{
			return 1;	/* есть отличие */
		}
	}
	return 0;	/* сигнатура совпадает */

#else /* WITHKEEPNVRAM */

	uint_fast8_t i;
	for (i = 0; i < (sizeof nvramsign - 1); ++ i)
	{
		const char c = restore_i8(RMT_SIGNATURE_BASE(i));
		if (c != nvramsign [i])
		{
			return 1;	/* есть отличие */
		}
	}
	return 0;	/* сигнатура совпадает */

#endif /* WITHKEEPNVRAM */
}

static void
//NOINLINEAT
initnvramsignature(void)
{
#if WITHKEEPNVRAM
	/* ослабить проверку совпадения версий прошивок для стирания NVRAM */
	uint8_t tsign [sizeof nvramsign];
	uint_fast8_t i;

	fillrelaxedsign(tsign);

	for (i = 0; i < sizeof nvramsign - 1; ++ i)
	{
		save_i8(RMT_SIGNATURE_BASE(i), tsign [i]);
	}
#else /* WITHKEEPNVRAM */
	uint_fast8_t i;

	for (i = 0; i < sizeof nvramsign - 1; ++ i)
	{
		save_i8(RMT_SIGNATURE_BASE(i), nvramsign [i]);
	}
#endif /* WITHKEEPNVRAM */
}

/* проверка совпадения тестовой сигнатуры в энергонезависимой памяти.
   0 - совпадает,
	 не-0 - отличается
	 */
static uint_fast8_t
//NOINLINEAT
verifynvrampattern(void)
{
	//const uint_fast32_t c32a = restore_i32(RMT_SIGNATURE_BASE(0));
	//const uint_fast32_t c32b = restore_i32(RMT_SIGNATURE_BASE(4));
	//PRINTF(PSTR("verifynvrampattern: c32a=%08lX c32b=%08lX\n"), c32a, c32b);
	uint_fast8_t i;
	for (i = 0; i < (sizeof nvramsign - 1); ++ i)
	{
		const char c = restore_i8(RMT_SIGNATURE_BASE(i));
		//PRINTF(PSTR("verifynvrampattern: pattern[%u]=%02X, mem=%02X\n"), i, (unsigned char) nvrampattern [i], (unsigned char) c);
		if (c != nvrampattern [i])
		{
			return 1;	/* есть отличие */
		}
	}
	return 0;	/* сигнатура совпадает */
}

static void
//NOINLINEAT
initnvrampattern(void)
{
	uint_fast8_t i;

	for (i = 0; i < sizeof nvramsign - 1; ++ i)
	{
		save_i8(RMT_SIGNATURE_BASE(i), nvrampattern [i]);
	}
}

#endif /* defined(NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING) */

/* описатели режимов. код режима - индекс в этом массиве
*/
typedef struct submodeprops_tag
{
	uint_least8_t qflags;		/* флаги подрежима */
	uint_least8_t mode;			/* код семейства режимов */
	uint_least8_t qcatmodecode;		/* Kenwood cat mode code */
	uint_fast16_t qcatwidth;		/* полоса режима, рапортуемая через CAT */
									// The FW command cannot be used in SSB or AM mode (an error tone will sound).

	const char * qlabel;
} submodeprops_t;

/* флаги подрежима */
#define QMODEF_HARDLSB	0x01		// для режимов приема на нижней боковой	 (CWR, LSB)
#define QMODEF_SMARTLSB	0x02	// для режимов приема на нижней боковой	 (CWR, LSB) в зависимости от частоты

// Порядок элементов в этом массиве должен совпадать с элементми enum,
// в котором определён SUBMODE_COUNT
static const submodeprops_t submodes [SUBMODE_COUNT] =
{
#if WITHMODESETSMART
	/* SUBMODE_SSBSMART */
	{
		QMODEF_SMARTLSB,
		MODE_SSB, 	/* индекс семейства режимов */
		2,			/* Kenwood cat mode code */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"SSB",
	},
	/* SUBMODE_CWSMART */
	{
		QMODEF_SMARTLSB,
		MODE_CW, 	/* индекс семейства режимов */
		3,			/* Kenwood cat mode code */
		500,		/* полоса фильтра режима, возвращаемая через CAT */
		"CW",
	},
	/* SUBMODE_CWZSMART */
	{
		QMODEF_SMARTLSB,
		MODE_CWZ, 	/* индекс семейства режимов */
		3,			/* Kenwood cat mode code */
		500,		/* полоса фильтра режима, возвращаемая через CAT */
		"CWZ",
	},
	/* SUBMODE_DIGISMART */
	{
		QMODEF_SMARTLSB,
		MODE_DIGI, 	/* индекс семейства режимов */
		9,			/* Kenwood cat mode code */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"DIG",
	},
#else /* WITHMODESETSMART */
	/* SUBMODE_USB */
	{
		0,
		MODE_SSB, 	/* индекс семейства режимов */
		2,			/* Kenwood cat mode code */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"USB",
	},
	/* SUBMODE_LSB */
	{
		QMODEF_HARDLSB,
		MODE_SSB, 	/* индекс семейства режимов */
		1,			/* Kenwood cat mode code */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"LSB",
	},
	/* SUBMODE_CW */
	{
		0,
		MODE_CW, 	/* индекс семейства режимов */
		3,			/* Kenwood cat mode code */
		500,		/* полоса фильтра режима, возвращаемая через CAT */
		"CW",
	},
	/* SUBMODE_CWR */
	{
		QMODEF_HARDLSB,
		MODE_CW, 	/* индекс семейства режимов */
		7,			/* Kenwood cat mode code */
		500,		/* полоса фильтра режима, возвращаемая через CAT */
		"CWR",
	},
	/* SUBMODE_CWZ */
	{
		QMODEF_SMARTLSB,
		MODE_CWZ, 	/* индекс семейства режимов */
		UINT8_MAX,	/* Kenwood cat mode code - на этот режим через CAT не переключиться */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
	  	"CWZ",
	},
	/* SUBMODE_AM */
	{
		QMODEF_SMARTLSB,
		MODE_AM, 	/* индекс семейства режимов */
		5,			/* Kenwood cat mode code */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"AM",
	},
	#if WITHSAM
	/* SUBMODE_SAM */
	{
		QMODEF_SMARTLSB,
		MODE_SAM, 	/* индекс семейства режимов */
		UINT8_MAX,	/* Kenwood cat mode code */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"SAM",
	},
	#endif /* WITHSAM */
	/* SUBMODE_DRM */
	{
		QMODEF_SMARTLSB,
		MODE_DRM, 	/* индекс семейства режимов */
		UINT8_MAX,	/* Kenwood cat mode code - на этот режим через CAT не переключиться */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"DRM",
	},
	/* SUBMODE_DGL */
	{
		QMODEF_HARDLSB,
		MODE_DIGI, 	/* индекс семейства режимов */
		9,			/* Kenwood cat mode code */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"DGL",
	},
	/* SUBMODE_DGU */
	{
		QMODEF_SMARTLSB,
		MODE_DIGI, 	/* индекс семейства режимов */
		6,			/* Kenwood cat mode code */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"DGU",
	},
#if WITHMODESETFULLNFM || WITHMODESETMIXONLY3NFM || WITHMODESETFULLNFMWFM	// чтобы через CAT не включился этот режтим - если нет - в комментарии.
	/* SUBMODE_NFM */
	{
		QMODEF_SMARTLSB,
		MODE_NFM, 	/* индекс семейства режимов */
		4,			/* Kenwood cat mode code */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"NFM",
	},
#endif /* WITHMODESETFULLNFM || WITHMODESETMIXONLY3NFM || WITHMODESETFULLNFMWFM  */
#if WITHWFM || WITHMODESETFULLNFMWFM
	/* SUBMODE_WFM */
	{
		QMODEF_SMARTLSB,
		MODE_WFM, 	/* индекс семейства режимов */
		8,			/* Kenwood cat mode code - придуман мною. В описаниях такого нет. */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"WFM",
	},
#endif /* WITHWFM || WITHMODESETFULLNFMWFM */
#if WITHMODEM
	/* SUBMODE_PSK */
	{
		0,
		MODE_MODEM, /* индекс семейства режимов */
		UINT8_MAX,	/* Kenwood cat mode code */
		500,		/* полоса фильтра режима, возвращаемая через CAT */
		"PSK",
	},
#endif /* WITHMODEM */
	/* SUBMODE_ISB */
	{
		0,
		MODE_ISB, 	/* индекс семейства режимов */
		UINT8_MAX,	/* Kenwood cat mode code - на этот режим через CAT не переключиться */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
	  	"ISB",
	},
#if WITHFREEDV
	/* SUBMODE_FDV */
	{
		0,
		MODE_FREEDV,/* индекс семейства режимов */
		UINT8_MAX,	/* Kenwood cat mode code */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"FDV",
	},
#endif /* WITHFREEDV */
#if WITHRTTY
	/* SUBMODE_RTTY */
	{
		0,
		MODE_RTTY,/* индекс семейства режимов */
		UINT8_MAX,	/* Kenwood cat mode code */
		0,			/* полоса фильтра режима, возвращаемая через CAT */
		"TTY",
	},
#endif /* WITHFREEDV */
#endif /* WITHMODESETSMART */
};

// Возвращает признак работы в LSB данного режима.
static uint_fast8_t
getsubmodelsb(
	uint_fast8_t submode,
	uint_fast8_t forcelsb
	)
{
	const uint_fast8_t flags = submodes [submode].qflags;
#if WITHMODESETSMART
	if ((flags & QMODEF_SMARTLSB) != 0)
		return forcelsb;
#endif /* WITHMODESETSMART */
	return (flags & QMODEF_HARDLSB) != 0;
}

#if WITHCAT
enum
{
	CATSTATE_HALTED,
	CATSTATE_WAITPARAM,		/* состояние ожидания приёма параметра (кончается по приёму ';' */
#if WITHELKEY
	CATSTATE_WAITMORSE,		/* состояние ожидания приёма символа за KY */
#endif /* WITHELKEY */
	CATSTATE_WAITCOMMAND1,	/* состояние ожидания приёма первого байта команды */
	CATSTATE_WAITCOMMAND2	/* состояние ожидания приёма второго байта команды */
};

enum
{
	CATSTATEO_HALTED,
	CATSTATEO_SENDREADY,
	CATSTATEO_WAITSENDREPLAY	/* обработчик поставил ответ на передачу - пока не закончится */
};

#endif /* WITHCAT */

#if WITHCAT

/* поиск по кенвудовскому номеру */
static uint_fast8_t
findkenwoodsubmode(uint_fast8_t catmode, uint_fast8_t defsubmode)
{
	uint_fast8_t submode;

	for (submode = 0; submode < (sizeof submodes / sizeof submodes [0]); submode ++)
	{
		const uint_fast8_t v = submodes [submode].qcatmodecode;
		if (v == UINT8_MAX)
			continue;		/* на этот режим через CAT не переключиться */
		if (v == catmode)
			return submode;
	}
	return defsubmode;
}

// Эта команда протокола CAT не возвращает полосу пропускания для SSB режимов.
// TODO: доделать для реальной работы
static uint_fast16_t
getkenwoodfw(
	uint_fast8_t submode,	// current submode
	uint_fast8_t fi)	// filter index
{
	return submodes [submode].qcatwidth;
}

static uint_fast8_t cat_getstateout(void);
static void cat_answer_forming(void);

#endif /* WITHCAT */

#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT

static uint_fast8_t gdimmtime;	/* количество секунд до гашения индикатора, 0 - не гасим. Регулируется из меню. */
static uint_fast8_t dimmcount;
static uint_fast8_t dimmflagch;	/* не-0: изменилось состояние dimmflag */

static const struct paramdefdef xgdimmtime =
{
	QLABEL2("DIMM TIM", "Dimmer Time"), 7, 0, RJ_UNSIGNED, ISTEP5,
	ITEM_VALUE,
	0, 240,
	OFFSETOF(struct nvmap, gdimmtime),
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& gdimmtime,
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif /* WITHLCDBACKLIGHT || WITHKBDBACKLIGHT */

#if WITHFANTIMER

#define FANPATIMEMAX	240
#if (WITHTHERMOLEVEL || WITHTHERMOLEVEL2)
static uint_fast8_t gfanpatempflag = 1;	/* */
static uint_fast8_t gfanpaontemp = 45;	/* температура включения вентилятора */
static uint_fast8_t gfanpaofftemp = 35;	/* температура выключения вентилятора */
#endif /* (WITHTHERMOLEVEL || WITHTHERMOLEVEL2) */

static uint_fast8_t gfanpatime = 25;	/* количество секунд до выключения вентилятора после передачи, 0 - не гасим. Регулируется из меню. */
static uint_fast8_t fanpacount = FANPATIMEMAX;
static uint_fast8_t fanpaflag = 1;	/* не-0: выключить вентилятор. */
static uint_fast8_t fanpaflagch;	/* не-0: изменилось состояние fanpaflag */
	#if WITHFANPWM
		static uint_fast16_t gfanpapwm = WITHFANPWMMAX;
	#endif /* WITHFANPWM */

#endif /* WITHFANTIMER */


#if WITHSLEEPTIMER

static uint_fast8_t gsleeptime;	/* количество минут до выключения, 0 - не выключаем. Регулируется из меню. */
static uint_fast16_t sleepcount;	/* счетчик в секундах */
static uint_fast8_t sleepflagch;	/* не-0: изменилось состояние sleepflag */

static const struct paramdefdef xgsleeptime =
{
	QLABEL2("SLEEPTIM", "Sleep Time"), 7, 0, RJ_UNSIGNED, ISTEP5,
	ITEM_VALUE,
	0, 240,
	OFFSETOF(struct nvmap, gsleeptime),
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& gsleeptime,
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};

#else
	//enum { sleepflag = 0 };

#endif /* WITHSLEEPTIMER */


/* Произошла какая-то активность со стороны пользователя, зажигаем подсветку.
	если было состояние "сна" - возвращаем 1 */
static uint_fast8_t
board_wakeup(void)
{
	uint_fast8_t r = 0;
#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT
	dimmcount = 0;		/* счётчик времени неактивности */
	if (dimmflag != 0)
	{
		dimmflag = 0;
		dimmflagch = 1;
		r = 1;
	}
#endif /* WITHLCDBACKLIGHT || WITHKBDBACKLIGHT */
#if WITHSLEEPTIMER
	sleepcount = 0;		/* счётчик времени неактивности */
	if (sleepflag != 0)
	{
		sleepflag = 0;
		sleepflagch = 1;
		r = 1;
	}
#endif /* WITHSLEEPTIMER */
	return r;
}

/* получаем PBT offset для текущего режима работы */
/* TODO: сделать зависимым от текущего фильтра */
static int_fast16_t
getpbt(
	const filter_t * workfilter,
	uint_fast8_t mode,		/* код режима работы */
	uint_fast8_t tx				/* признак передачи */
	)
{
#if WITHPBT // && (LO3_SIDE != LOCODE_INVALID)
	if (tx || mdt [mode].wpbt == 0)
		return 0;
	return gpbtoffset.value + getpbtbase();	/* из индицируемого значения получить знаковое */

#else /* WITHPBT */
	(void) mode;
	(void) tx;
	return 0;
#endif /* WITHPBT */
}
/* получаем IF SHIFT offset для текущего режима работы */
/* TODO: сделать зависимым от текущего фильтра */
// Увеличение значения параметра смещает слышимую часть спектра в более высокие частоты
static int_fast16_t
getifshift(
	const filter_t * workfilter,
	uint_fast8_t mode,		/* код режима работы */
	uint_fast8_t tx				/* признак передачи */
	)
{
#if WITHIFSHIFT
	if (tx || mdt [mode].wifshift == 0)
		return 0;
	return ifshifoffset.value + getifshiftbase();	/* из индицируемого значения получить знаковое */

#else /* WITHIFSHIFT */

	(void) workfilter;
	(void) mode;
	(void) tx;
	return 0;

#endif /* WITHIFSHIFT */
}

static uint_fast32_t
loadvfy32freq(
	vindex_t b		// band
	)
{
	const vindex_t b0 = (b >= MBANDS_BASE) ? VFOS_BASE : b;
	return loadvfy32(RMT_BFREQ_BASE(b), get_band_bottom(b0), get_band_top(b0), get_band_init(b0));
}

#if 0
static const uint_fast8_t ssb_steps10 [] =
{
	1,	/* 10 Hz */
	5,	/* 50 Hz */
	10,	/* 100 Hz */
};
static const uint_fast8_t am_steps10 [] =
{
	10,	/* 100 Hz */
	50,	/* 500 Hz */
	100,/* 1 kHz */
	125,/* 1.25 kHz */
	150,/* 1.5 kHz - for 9 kHz implementation */
	250,/* 2.5 kHz */
};
#endif

/* функция работает с кэшем параметров режима работы */
static const struct modetempl *
NOINLINEAT
getmodetempl(uint_fast8_t submode)
{
	return & mdt [submodes [submode].mode];
}

#if WITHAMHIGHKBDADJ


/* Изменение верхнего среза полосы пропускания в установленном режиме */
static void
uif_key_click_amfmbandpassup(void)
{
	const struct modetempl * const pmodet = getmodetempl(gsubmode);
	const uint_fast8_t bwseti = pmodet->bwsetis [0];	// индекс банка полос пропускания для данного режима на приеме
	const uint_fast8_t pos = bwsetpos [bwseti];
	bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	//if (p->type != BWSET_PAIR)
	//	return;

	switch (p->type)
	{
	case BWSET_SINGLE:
		p->left10_width10 = nextfreq(p->left10_width10, p->left10_width10 + p->limits->granulationleft, p->limits->granulationleft, p->limits->left10_width10_high + 1);
		save_i8(RMT_BWPROPSLEFT_BASE(p->bwpropi), p->left10_width10);	// верхний срез фильтра НЧ в сотнях герц
		updateboard();
		break;

	default:
	case BWSET_PAIR:
		p->right100 = nextfreq(p->right100, p->right100 + p->limits->granulationright, p->limits->granulationright, p->limits->right100_high + 1);
		save_i8(RMT_BWPROPSRIGHT_BASE(p->bwpropi), p->right100);	// верхний срез фильтра НЧ в сотнях герц
		updateboard();
		break;
	}

}

/* Изменение верхнего среза полосы пропускания в установленном режиме */
static void
uif_key_click_amfmbandpassdown(void)
{
	const struct modetempl * const pmodet = getmodetempl(gsubmode);
	const uint_fast8_t bwseti = pmodet->bwsetis [0];	// индекс банка полос пропускания для данного режима на приеме
	const uint_fast8_t pos = bwsetpos [bwseti];
	bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	//if (p->type != BWSET_PAIR)
	//	return;

	switch (p->type)
	{
	case BWSET_SINGLE:
		p->left10_width10 = prevfreq(p->left10_width10, p->left10_width10 - p->limits->granulationleft, p->limits->granulationleft, p->limits->left10_width10_low);
		save_i8(RMT_BWPROPSLEFT_BASE(p->bwpropi), p->left10_width10);	// верхний срез фильтра НЧ в сотнях герц
		updateboard();
		break;

	default:
	case BWSET_PAIR:
		p->right100 = prevfreq(p->right100, p->right100 - 1, p->limits->granulationright, p->limits->right100_low);
		save_i8(RMT_BWPROPSRIGHT_BASE(p->bwpropi), p->right100);	// верхний срез фильтра НЧ в сотнях герц
		updateboard();
		break;
	}

}

/* текущее значение верхнего среза полосы пропускания в установленном режиме (в десятках герц) */
uint_fast8_t hamradio_get_amfm_highcut10_value(uint_fast8_t * flag)
{
	const struct modetempl * const pmodet = getmodetempl(gsubmode);
	const uint_fast8_t bwseti = pmodet->bwsetis [0];	// индекс банка полос пропускания для данного режима на приеме
	const uint_fast8_t pos = bwsetpos [bwseti];
	bwprop_t * const p = bwsetsc [bwseti].prop [pos];

	switch (p->type)
	{
	case BWSET_SINGLE:
		* flag = 1;//p->type == BWSET_PAIR;
		return p->left10_width10;
		break;
	default:
	case BWSET_PAIR:
		* flag = 1;//p->type == BWSET_PAIR;
		return p->right100 * 10;
		break;
	}
}
#endif /* WITHAMHIGHKBDADJ */

static void
uif_pwbutton_press(void)
{
	txreq_rx(& txreqst0, NULL);	// переходим на приём
	gpoweronhold = 0;
	updateboard();
}

// проверка, используется ли описатель диапазона с данным кодом в текущей конфигурации.
// Возврат 0 - не используется
static uint_fast8_t
//NOINLINEAT
existingband(
	uint_fast8_t b,	// код диапазона
	uint_fast8_t bandsetbcast,
	uint_fast8_t bandset_no_check
	)
{
	const uint_fast8_t bandset = get_band_bandset(b);

	if (get_band_bottom(b) >= TUNE_TOP || get_band_top(b) < TUNE_BOTTOM)
		return 0;

	if (bandset_no_check)
		return 1;

	switch (bandset)
	{
	default:
	case BANDSETF_ALL:
		return 1;		// используется всегда
	case BANDSETF_HAM:
		return ! bandsetbcast;
	case BANDSETF_HAMWARC:
		return bandsethamwarc && ! bandsetbcast;
	case BANDSETF_BCAST:
		return bandsetbcast;
	case BANDSETF_CB:
		return bandset11m;
#if TUNE_6MBAND
	case BANDSETF_6M:
		return bandset6m && ! bandsetbcast;		// используется или нет - определяется параметром
#endif /* TUNE_6MBAND */
#if TUNE_4MBAND
	case BANDSETF_4M:
		return bandset4m && ! bandsetbcast;		// используется или нет - определяется параметром
#endif /* TUNE_4MBAND */

	// 144 и 430 разрешаются одним пунктом в меню.
#if TUNE_2MBAND
	case BANDSETF_2M:
		return bandset2m && ! bandsetbcast;		// используется или нет - определяется параметром
#endif /* TUNE_2MBAND */
#if TUNE_07MBAND
	case BANDSETF_07M:
		return bandset2m && ! bandsetbcast;		// используется или нет - определяется параметром
#endif /* TUNE_2MBAND */
	}
}

static uint_fast8_t
//NOINLINEAT
existingbandsingle(
	uint_fast8_t b,	// код диапазона
	uint_fast8_t bandsetbcast
	)
{
	const uint_fast8_t bandset_no_check = 0;
	if (existingband(b, 0, bandset_no_check) && existingband(b, 1, bandset_no_check))
		return bandsetbcast;	// BANDSETF_ALL
	return existingband(b, bandsetbcast, bandset_no_check);
}

static void
//NOINLINEAT
verifyband(const vindex_t b)
{
#if DEBUGEXT
	if (b == ((uint_fast8_t) - 1))
	{
		ASSERT(0);
		for (;;)
			;
	}
#endif /* DEBUGEXT */
}


#if XVTR_NYQ1
// Частота из лююой зоны найквиста преобразуется в первую
static uint_fast32_t FQMODEL_TUNING_TO_NYQ1(uint_fast32_t f)
{
	uint_fast32_t f1 = (f % DUCDDC_FREQ);
	return f1 >= (DUCDDC_FREQ / 2) ? (DUCDDC_FREQ - f1) : f1;
}
#endif /* XVTR_NYQ1 */

// tuning frequency to band pass frequency adjust
static uint_fast32_t
nyquistadj(uint_fast32_t f)
{
#if 0//XVTR_R820T2
	reuturn R820T_IFFREQ;
#elif 0//XVTR_NYQ1
	return FQMODEL_TUNING_TO_NYQ1(f);
#else /* XVTR_NYQ1 */
	return f;
#endif /* XVTR_NYQ1 */
}

// tuning frequency to PA low pass frequency adjust
static uint_fast32_t
nyquistadj2(uint_fast32_t f)
{
#if 0//XVTR_R820T2
	reuturn R820T_IFFREQ;
#elif XVTR_NYQ1
	return FQMODEL_TUNING_TO_NYQ1(f);
#else /* XVTR_NYQ1 */
	return f;
#endif /* XVTR_NYQ1 */
}

// tuning frequency to external PA (ACC socket) frequency adjust - no correction
static uint_fast32_t
nyquistadj3(uint_fast32_t f)
{
	return f;
}

/* получить номер диапазона, в который попадает отображающаяся частота
  Если не принадлежит ни одному диапазону, возврат ((uint8_t)-1) */
static vindex_t
getfreqband(const uint_fast32_t freq, uint_fast8_t bandset_no_check)
{
	vindex_t i;

	for (i = 0; i < (sizeof bandsmap / sizeof bandsmap [0]); ++ i)
	{
		if (! existingband(i, gbandsetbcast, bandset_no_check))	// диапазон в данной конфигурации не используется
			continue;
		if (get_band_bottom(i) <= freq && get_band_top(i) > freq)
			return i;
	}
	return ((vindex_t) - 1);
}

/* получить номер диапазона с большей частотой, на который переходить.
  Если нет подходящих, возврат low */
static vindex_t
getnexthband(const uint_fast32_t freq)
{
	enum { LOW = 0, HIGH = HBANDS_COUNT - 1 };
	const uint_fast8_t bandset_no_check = 0;
	vindex_t i;

	for (i = 0; i < HBANDS_COUNT; ++ i)
	{
		if (! existingband(i, gbandsetbcast, bandset_no_check))	// диапазон в данной конфигурации не используется
			continue;
		if (get_band_top(i) > freq)
			return i;
	}
	return LOW;
}

/* получить номер любительского диспазона, следующего в группе. Если в группе больше нет ни одного диапазона,
 вернуть номер текущего.
 */
static vindex_t
getnextbandingroup(const vindex_t b, const uint_fast8_t bandgroup)
{
	const uint_fast8_t bandset_no_check = 0;
	enum { LOW = 0, HIGH = HBANDS_COUNT - 1 };

	vindex_t i = b;		// начальный диапазон
	do
	{
		i = i == HIGH ? LOW : (i + 1);	// переход к следующему диапазону
		if (! existingband(i, gbandsetbcast, bandset_no_check))	// диапазон в данной конфигурации не используется
			continue;
		if (bandsmap [i].bandgroup == bandgroup)
			break;			// диапазон той же группы
	} while (i != b);
	return i;
}

/* получить номер диапазона с меньшей частотой, на который переходить.
  Если нет подходящих, возврат high */
static vindex_t
getprevhband(const uint_fast32_t freq)
{
	const uint_fast8_t bandset_no_check = 0;
	vindex_t i;

	for (i = 0; i < HBANDS_COUNT; ++ i)
	{
		if (! existingband(i, gbandsetbcast, bandset_no_check))	// диапазон в данной конфигурации не используется
			continue;
		if (get_band_bottom(i) > freq)
		{
			i = (i == 0) ? HBANDS_COUNT : i;
			break;
		}
	}
	// возврат только допустимых диапазонов.
	do
		i = calc_prev(i, 0, HBANDS_COUNT - 1);
	while (! existingband(i, gbandsetbcast, bandset_no_check));
	return i;
}


static vindex_t
getnext_ham_band(
	vindex_t b,		// текущий диапазон
	const uint_fast32_t freq
	)
{
	const uint_fast8_t bandset_no_check = 0;
	uint_fast8_t xi;
	vindex_t xsel [XBANDS_COUNT];
	vindex_t xnext [XBANDS_COUNT];
	vindex_t xprev [XBANDS_COUNT];

	for (xi = 0; xi < XBANDS_COUNT; ++ xi)
	{
		const uint_fast32_t f = loadvfy32freq(XBANDS_BASE0 + xi);	// частота в обзорном диапазоне
		xsel [xi] = getfreqband(f, bandset_no_check);			// не принадлежит ли частота какому-то диапазону
		xnext [xi] = getnexthband(f);		// получить номер диапазона с большей частотой
		xprev [xi] = getprevhband(f);		// получить номер диапазона с меньшей частотой
	}
//	PRINTF("getnext_ham_band: b=%d(%d), xsel[0]=%d, xsel[1]=%d, xnext[0]=%d, xnext[1]=%d, xprev[0]=%d, xprev[1]=%d\n",
//			b, XBANDS_BASE0, xsel[0], xsel[1], xnext[0], xnext[1], xprev[0], xprev[1]);

	do
	{
		// 147M - 120 kHz
		// getnext_ham_band: b=37(36), xsel[0]=36, xsel[1]=37, xnext[0]=0, xnext[1]=0, xprev[0]=35, xprev[1]=35
		// getnext_ham_band: b=35(36), xsel[0]=36, xsel[1]=37, xnext[0]=0, xnext[1]=0, xprev[0]=35, xprev[1]=35
		//
		if (
			b == XBANDS_BASE1 &&
			xsel [0] == XBANDS_BASE0 &&
			xsel [1] == XBANDS_BASE1 &&
			xnext [0] == 0 &&
			xnext [1] == 0 &&
			xprev [0] == (HBANDS_COUNT - 1) &&
			xprev [1] == (HBANDS_COUNT - 1) &&
			1)
		{
			/* обработка ситуацию "из обзорного - в обзорный диапазон",
			если запомненная частота нового обзорного диапазона не попадает на выделенный диапазон */
			b = XBANDS_BASE0;
			continue;
		}
		// 120 kHz -> 225 kHz
		// getnext_ham_band: b=36(36,36), xsel[0]=36, xsel[1]=37, xnext[0]=0, xnext[1]=0, xprev[0]=35, xprev[1]=35
		//
		if (
			b == XBANDS_BASE0 &&
			xsel [0] == XBANDS_BASE0 &&
			xsel [1] == XBANDS_BASE1 &&
			xnext [0] == 0 &&
			xnext [1] == 0 &&
			xprev [0] == (HBANDS_COUNT - 1) &&
			xprev [1] == (HBANDS_COUNT - 1) &&
			1)
		{
			/* обработка ситуацию "из обзорного - в обзорный диапазон",
			если запомненная частота нового обзорного диапазона не попадает на выделенный диапазон */
			b = HBANDS_COUNT - 1;
			do
				b = calc_next(b, 0, HBANDS_COUNT - 1);
			while (! existingband(b, gbandsetbcast, bandset_no_check));
			continue;
		}
		if (
			b == XBANDS_BASE0 &&
			xprev [0] == xprev [1] &&
			xnext [0] == xnext [1] &&
			xsel [1] >= HBANDS_COUNT)
		{
			/* обработка ситуацию "из обзорного - в выделенный диапазон",
			если запомненная частота нового обзорного диапазона не попадает на выделенный диапазон */
			b = XBANDS_BASE1;
			continue;
		}
		if (b == xprev [0] && xsel [0] >= HBANDS_COUNT)
		{
			// текущая является предшествующей для xfreq [0]
			/* переходим в обзорный диапазон 0 */
			b = XBANDS_BASE0;
			continue;
		}
		if (b == xprev [1] && xsel [1] >= HBANDS_COUNT)
		{
			// текущая является предшествующей для xfreq [1]
			/* переходим в обзорный диапазон 1 */
			b = XBANDS_BASE1;
			continue;
		}
		if (b < HBANDS_COUNT)
		{
			/* текущая частота относится к любительским диапазонам */
			do
				b = calc_next(b, 0, HBANDS_COUNT - 1);
			while (! existingband(b, gbandsetbcast, bandset_no_check));
			continue;
		}
		if (b == XBANDS_BASE0)
		{
			// текущая частота - обзорный 0
			b = xnext [0];
			continue;
		}
		if (b == XBANDS_BASE1)
		{
			// текущая частота - обзорный 1
			b = xnext [1];
			continue;
		}

		b = getnexthband(freq);
	} while (0);
	//PRINTF("exit b=%d\n", b);
	return b;
}


static vindex_t
getprev_ham_band(
	vindex_t b,		// текущий диапазон
	const uint_fast32_t freq
	)
{
	const uint_fast8_t bandset_no_check = 0;
	uint_fast8_t xi;
	vindex_t xsel [XBANDS_COUNT];
	vindex_t xnext [XBANDS_COUNT];
	vindex_t xprev [XBANDS_COUNT];

	for (xi = 0; xi < XBANDS_COUNT; ++ xi)
	{
		const uint_fast32_t f = loadvfy32freq(XBANDS_BASE0 + xi);	// частота в обзорном диапазоне
		xsel [xi] = getfreqband(f, bandset_no_check);			// не принадлежит ли частота какому-то диапазону
		xnext [xi] = getnexthband(f);		// получить номер диапазона с большей частотой
		xprev [xi] = getprevhband(f);		// получить номер диапазона с меньшей частотой
	}
//	PRINTF("getprev_ham_band: b=%d(%d), xsel[0]=%d, xsel[1]=%d, xnext[0]=%d, xnext[1]=%d, xprev[0]=%d, xprev[1]=%d\n",
//			b, XBANDS_BASE0, xsel[0], xsel[1], xnext[0], xnext[1], xprev[0], xprev[1]);

	do
	{
		// 120 kHz -> 147 MHz
		// getprev_ham_band: b=36(36), xsel[0]=36, xsel[1]=37, xnext[0]=0, xnext[1]=0, xprev[0]=35, xprev[1]=35
		//
		if (
			b == XBANDS_BASE0 &&
			xsel [0] == XBANDS_BASE0 &&
			xsel [1] == XBANDS_BASE1 &&
			xnext [0] == 0 &&
			xnext [1] == 0 &&
			xprev [0] == (HBANDS_COUNT - 1) &&
			xprev [1] == (HBANDS_COUNT - 1) &&
			1)
		{
			/* обработка ситуацию "из обзорного - в обзорный диапазон",
			если запомненная частота нового обзорного диапазона не попадает на выделенный диапазон */
			b = XBANDS_BASE1;
			continue;
		}
		// 147 MHz -> 145 MHz
		// getnext_ham_band: b=36(36,36), xsel[0]=36, xsel[1]=37, xnext[0]=0, xnext[1]=0, xprev[0]=35, xprev[1]=35
		//
		if (
			b == XBANDS_BASE1 &&
			xsel [0] == XBANDS_BASE0 &&
			xsel [1] == XBANDS_BASE1 &&
			xnext [0] == 0 &&
			xnext [1] == 0 &&
			xprev [0] == (HBANDS_COUNT - 1) &&
			xprev [1] == (HBANDS_COUNT - 1) &&
			1)
		{
			/* обработка ситуацию "из обзорного - в выделенный диапазон",
			если запомненная частота нового обзорного диапазона не попадает на выделенный диапазон */
			b = 0;
			do
				b = calc_prev(b, 0, HBANDS_COUNT - 1);
			while (! existingband(b, gbandsetbcast, bandset_no_check));
			continue;
			continue;
		}
		if (
			b == XBANDS_BASE1 &&
			xprev [0] == xprev [1] &&
			xnext [0] == xnext [1] &&
			xsel [0] >= HBANDS_COUNT
			)
		{
			/* обработка ситуацию "из обзорного - в обзорный диапазон",
			если запомненная частота нового обзорного диапазона не попадает на выделенный диапазон */
			b = XBANDS_BASE0;
			continue;

		}
		if (b == xnext [0] && xsel [0] >= HBANDS_COUNT)
		{
			/* переходим в обзорный диапазон 0 */
			b = XBANDS_BASE0;
			continue;
		}
		if (b == xnext [1] && xsel [1] >= HBANDS_COUNT)
		{
			/* переходим в обзорный диапазон 1 */
			b = XBANDS_BASE1;
			continue;
		}
		if (b < HBANDS_COUNT)
		{
			/* текущая частота относится к любительским диапазонам */
			do
				b = calc_prev(b, 0, HBANDS_COUNT - 1);
			while (! existingband(b, gbandsetbcast, bandset_no_check));
			continue;
		}
		if (b == (XBANDS_BASE0))
		{
			// текущая частота - обзорный 0
			b = xprev [0];
			continue;
		}
		if (b == (XBANDS_BASE1))
		{
			// текущая частота - обзорный 1
			b = xprev [1];
			continue;
		}

		b = getprevhband(freq);
	} while (0);
	//PRINTF("exit b=%d\n", b);
	return b;
}

/* определяем по частоте, в какой группе диапазонов находимся */
static uint_fast8_t
getfreqbandgroup(const uint_fast32_t freq)
{
	const uint_fast8_t bandset_no_check = 0;
	const vindex_t b = getfreqband(freq, bandset_no_check);
	ASSERT(b != ((vindex_t) - 1));
	const uint_fast8_t bandgroup = bandsmap [b].bandgroup;
	return bandgroup;
}

static uint_fast8_t
getbankindex_raw(const uint_fast8_t n)
{
	ASSERT(n < 2);
	return (gvfoab + n) % 2;
}

// программирование трактов для двойного приема
static uint_fast8_t
getbankindex_pathi(const uint_fast8_t pathi)
{
	ASSERT(pathi < 2);

#if WITHSPLIT || WITHSPLITEX

	switch (gsplitmode)	/* (vfo/vfoa/vfob/mem) */
	{
	case VFOMODES_VFOINIT:
		return getbankindex_raw(0);

	case VFOMODES_VFOSPLIT:
		if (gtx == 0)
			return getbankindex_raw(pathi);
		else
			return getbankindex_tx(1);
	default:
		ASSERT(0);
		return 0;
	}

#else /* WITHSPLIT || WITHSPLITEX */
	return getbankindex_raw(0);
#endif /* WITHSPLIT || WITHSPLITEX */
}

static uint_fast8_t
getbankindex_ab(
	const uint_fast8_t ab	// 0: A, 1: B
	)
{
	ASSERT(ab < 2);
#if WITHSPLIT || WITHSPLITEX

	switch (gsplitmode)	/* (vfo/vfoa/vfob/mem) */
	{
	case VFOMODES_VFOINIT:
		return getbankindex_raw(0);

	case VFOMODES_VFOSPLIT:
		return getbankindex_raw(ab);
	default:
		ASSERT(0);
		return getbankindex_raw(0);
	}

#else /* WITHSPLIT || WITHSPLITEX */
	return getbankindex_raw(0);
#endif /* WITHSPLIT || WITHSPLITEX */
}

static uint_fast8_t
getbankindex_tx(const uint_fast8_t tx)
{
	ASSERT(tx < 2);
#if WITHSPLIT || WITHSPLITEX

	switch (gsplitmode)	/* (vfo/vfoa/vfob/mem) */
	{
	case VFOMODES_VFOINIT:
		return getbankindex_raw(0);

	case VFOMODES_VFOSPLIT:
		return getbankindex_raw(tx);
	default:
		ASSERT(0);
		return getbankindex_raw(0);
	}

#else /* WITHSPLIT || WITHSPLITEX */
	return getbankindex_raw(0);
#endif /* WITHSPLIT || WITHSPLITEX */
}

// получить bankindex для показа частоты или режима работы тракта на дисплее в полях A (0) и B (1)
static uint_fast8_t
//NOINLINEAT
getbankindex_ab_fordisplay(const uint_fast8_t ab)
{
	ASSERT(ab < 2);
#if WITHSPLIT
	return getbankindex_ab(gtx != ab);
#elif WITHSPLITEX
	return getbankindex_raw(gtx != ab);
#else /* WITHSPLIT */
	return getbankindex_raw(0);
#endif /* WITHSPLIT */
}

static uint_fast8_t
getbankindexmain(void)
{
#if 1//WITHSPLIT
    const uint_fast8_t bi_main = getbankindex_ab_fordisplay(0);        /* состояние выбора банков может измениться */
    const uint_fast8_t bi_sub = getbankindex_ab_fordisplay(1);        /* состояние выбора банков может измениться */
#elif WITHSPLITEX
    const uint_fast8_t bi_main = getbankindex_ab(0);        /* состояние выбора банков может измениться */
    const uint_fast8_t bi_sub = getbankindex_ab(1);        /* состояние выбора банков может измениться */
#endif /* WITHSPLIT, WITHSPLITEX */
	return bi_main;
}

static uint_fast8_t
getbankindexsub(void)
{
#if 1//WITHSPLIT
    const uint_fast8_t bi_main = getbankindex_ab_fordisplay(0);        /* состояние выбора банков может измениться */
    const uint_fast8_t bi_sub = getbankindex_ab_fordisplay(1);        /* состояние выбора банков может измениться */
#elif WITHSPLITEX
    const uint_fast8_t bi_main = getbankindex_ab(0);        /* состояние выбора банков может измениться */
    const uint_fast8_t bi_sub = getbankindex_ab(1);        /* состояние выбора банков может измениться */
#endif /* WITHSPLIT, WITHSPLITEX */
	return bi_sub;
}

#if WITHUSEDUALWATCH

static uint_fast8_t
getactualmainsubrx(void)
{
#if WITHSPLIT || WITHSPLITEX

	switch (gsplitmode)
	{
	default:
		return BOARD_RXMAINSUB_A_A;

	case VFOMODES_VFOSPLIT:
		return mainsubrxmodes [mainsubrxmode].code;
	}
#else /* WITHSPLIT || WITHSPLITEX */

	return BOARD_RXMAINSUB_A_A;

#endif /* WITHSPLIT || WITHSPLITEX */
}

#endif /* WITHUSEDUALWATCH */

// VFO mode
// Через flag возвращается признак активного SPLIT (0/1)
const char * hamradio_get_vfomode3_value(uint_fast8_t * flag)
{
	static const char spl [] = "SPL";
	static const char sp3 [] = "";

	switch (gsplitmode)	/* (vfo/vfoa/vfob/mem) */
	{
	default:
	case VFOMODES_VFOINIT:	/* no SPLIT -  Обычная перестройка */
		* flag = 0;
		return sp3;
	case VFOMODES_VFOSPLIT:
		* flag = 1;
		return spl;
		//return (gvfoab != tx) ? b : a;
	}
}

// VFO mode
const char * hamradio_get_vfomode5_value(uint_fast8_t * flag)
{
	static const char spl [] = "SPLIT";
	static const char sp5 [] = "";

	switch (gsplitmode)	/* (vfo/vfoa/vfob/mem) */
	{
	default:
	case VFOMODES_VFOINIT:	/* no SPLIT -  Обычная перестройка */
		* flag = 0;
		return sp5;
	case VFOMODES_VFOSPLIT:
		* flag = 1;
		return spl;
		//return (gvfoab != tx) ? b : a;
	}
}

// Частота VFO A для маркировки файлов
uint_fast32_t hamradio_get_freq_rx(void)
{
#if WITHLFM
	if (iflfmactive())
		return getlfmfreq();
#endif /* WITHLFM */
	return gfreqs [getbankindex_tx(0)];
}

// Частота VFO A для отображения на дисплее
uint_fast32_t hamradio_get_freq_a(void)
{
#if WITHLFM
	if (iflfmactive())
		return getlfmfreq();
#endif /* WITHLFM */
	return gfreqs [getbankindex_ab_fordisplay(0)];		/* VFO A modifications */
}
// Частота VFO A для отображения на дисплее
uint_fast32_t hamradio_get_freq_pathi(uint_fast8_t pathi)
{
	return gfreqs [getbankindex_pathi(pathi)];
}
// SSB/CW/AM/FM/..
const char * hamradio_get_mode_a_value_P(void)
{
	return submodes [getsubmode(getbankindex_ab_fordisplay(0))].qlabel;	/* VFO A modifications */
}

// SSB/CW/AM/FM/..
const char * hamradio_get_mode_b_value_P(uint_fast8_t * flag)
{
	switch (gsplitmode)	/* (vfo/vfoa/vfob/mem) */
	{
	default:
	case VFOMODES_VFOINIT:	/* no SPLIT -  Обычная перестройка */
		* flag = 0;
	case VFOMODES_VFOSPLIT:
		* flag = 1;
		//return (gvfoab != tx) ? b : a;
	}
	return submodes [getsubmode(getbankindex_ab_fordisplay(1))].qlabel;	/* VFO B modifications */
}

// Частота VFO B для отображения на дисплее
uint_fast32_t hamradio_get_freq_b(void)
{
	return gfreqs [getbankindex_ab_fordisplay(1)];	/* VFO B modifications */
}

	enum { withonlybands = 0 };

#if WITHONLYBANDS
	#error Remove WITHONLYBANDS from configuration - not supported
#endif /* WITHONLYBANDS */

/* функция для выполнения условия перестройки только в любительских диапазонах */
static int_fast32_t
tune_top(const uint_fast8_t bi)
{
#if WITHONLYBANDS
	if (withonlybands != 0)
		return tune_top_active [bi];
#endif /* WITHONLYBANDS */
#if XVTR_R820T2
	return NOXVRTUNE_TOP;
#endif /* XVTR_R820T2 */
	return TUNE_TOP;
}

/* функция для выполнения условия перестройки только в любительских диапазонах */
static int_fast32_t
tune_bottom(const uint_fast8_t bi)
{
#if WITHONLYBANDS
	if (withonlybands != 0)
		return tune_bottom_active [bi];
#endif /* WITHONLYBANDS */
#if XVTR_R820T2
	return TUNE_BOTTOM;
#endif /* XVTR_R820T2 */
	return TUNE_BOTTOM;
}

/* скопировать теккущий банк в противоположный перед включением расстройки */
static void
copybankstate(
	const uint_fast8_t sbi,	// source bank index
	const uint_fast8_t tbi,	// target bank index
	const int_fast32_t deltaf	// рассторйка передатчика для включения способом "auto split"
	)
{
	ASSERT(tbi != sbi);
	gfreqs [tbi] = vfy32up(gfreqs [sbi] + deltaf, tune_bottom(sbi), tune_top(sbi) - 1, gfreqs [sbi]);
	gmoderows [tbi] = gmoderows [sbi];

	memcpy(gmodecolmaps [tbi], gmodecolmaps [sbi], sizeof gmodecolmaps [tbi]);
}

/* сохранить все частоту настройки в соответствующий диапазон, ячейку памяти или VFO. */
static void
//NOINLINEAT
storebandfreq(const vindex_t b, const uint_fast8_t bi)
{
	//PRINTF(PSTR("storebandfreq: b=%d, bi=%d, freq=%ld\n"), b, bi, (unsigned long) gfreqs [bi]);
	verifyband(b);

	save_i32(RMT_BFREQ_BASE(b), gfreqs [bi]);	/* сохранить в области диапазона частоту */
}

#if WITHSPECTRUMWF

/* сохранение параметров отображения спектра и водопада */
static void storezoom(uint_fast8_t bg)
{
	save_i8(OFFSETOF(struct nvmap, bandgroups [bg].gzoomxpow2), gzoomxpow2);	/* уменьшение отображаемого участка спектра */
	save_i8(OFFSETOF(struct nvmap, bandgroups [bg].gtopdb), gtopdb);	/* нижний предел FFT */
	save_i8(OFFSETOF(struct nvmap, bandgroups [bg].gbottomdb), gbottomdb);	/* верхний предел FFT */
}

/* восстановление параметров отображения спектра и водопада */
static void loadzoom(uint_fast8_t bg)
{
	gzoomxpow2 = loadvfy8up(OFFSETOF(struct nvmap, bandgroups [bg].gzoomxpow2), 0, BOARD_FFTZOOM_POW2MAX, 0);	/* масштаб панорамы */
	gtopdb = loadvfy8up(OFFSETOF(struct nvmap, bandgroups [bg].gtopdb), WITHTOPDBMIN, WITHTOPDBMAX, WITHTOPDBDEFAULT);		/* нижний предел FFT */
	gbottomdb = loadvfy8up(OFFSETOF(struct nvmap, bandgroups [bg].gbottomdb), WITHBOTTOMDBMIN, WITHBOTTOMDBMAX, WITHBOTTOMDBDEFAULT);	/* верхний предел FFT */
}

#endif /* WITHSPECTRUMWF */

static void storebandpos(uint_fast8_t b)
{
	const uint_fast8_t bandgroup = bandsmap [b].bandgroup;
	if (bandgroup != BANDGROUP_COUNT)
		save_i8(RMT_BANDPOS(bandgroup), b);
}

static void storebandgroup(uint_fast8_t bg, uint_fast8_t ant, uint_fast8_t rxant)
{

#if WITHANTSELECTRX || WITHANTSELECT1RX

	#if ! WITHONEATTONEAMP
		save_i8(RMT_PAMPBG3_BASE(bg, gantenna, grxantenna), gpamp);
	#endif /* ! WITHONEATTONEAMP */
	save_i8(RMT_RXANTENNABG_BASE(bg), grxantenna);
	save_i8(RMT_ANTENNABG_BASE(bg), gantenna);
	save_i8(RMT_ATTBG3_BASE(bg, gantenna, grxantenna), gatt);

#elif WITHANTSELECT

	#if ! WITHONEATTONEAMP
		save_i8(RMT_PAMPBG3_BASE(bg, gantenna, grxantenna), gpamp);
	#endif /* ! WITHONEATTONEAMP */
	save_i8(RMT_ANTENNABG_BASE(bg), gantenna);
	save_i8(RMT_ATTBG3_BASE(bg, gantenna, grxantenna), gatt);

#elif WITHANTSELECT2

	#if ! WITHONEATTONEAMP
		save_i8(RMT_PAMPBG3_BASE(bg, gantennabym, grxantenna), gpamp);
	#endif /* ! WITHONEATTONEAMP */
	save_i8(RMT_ATTBG3_BASE(bg, gantennabym, grxantenna), gatt);
	save_i8(RMT_ANTENNABG_BASE(bg), gantennabym);

#else

	#if ! WITHONEATTONEAMP
		save_i8(RMT_PAMPBG3_BASE(bg, gantenna, grxantenna), gpamp);
	#endif /* ! WITHONEATTONEAMP */
	save_i8(RMT_ATTBG3_BASE(bg, gantenna, grxantenna), gatt);

#endif /* WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX */


#if WITHAUTOTUNER
	storetuner(bg, ant);
#endif /* WITHAUTOTUNER */
#if WITHSPECTRUMWF
	storezoom(bg);
#endif /* WITHSPECTRUMWF */
}

static void loadantenna(uint_fast8_t bi, uint_fast8_t bg)
{
#if WITHANTSELECTRX || WITHANTSELECT1RX
	grxantenna = loadvfy8up(RMT_RXANTENNABG_BASE(bg), 0, RXANTMODE_COUNT - 1, 0);	/* вытаскиваем номер включённой антенны */
	gantenna = loadvfy8up(RMT_ANTENNABG_BASE(bg), 0, ANTMODE_COUNT - 1, 0);	/* вытаскиваем номер включённой антенны */
#elif WITHANTSELECT2
	gantennabym = loadvfy8up(RMT_ANTENNABG_BASE(bg), 0, ANTMODE_COUNT - 1, getdefantenna(gfreqs [bi]));	/* вытаскиваем номер включённой антенны */
#elif WITHANTSELECT
	gantenna = loadvfy8up(RMT_ANTENNABG_BASE(bg), 0, ANTMODE_COUNT - 1, 0);	/* вытаскиваем номер включённой антенны */
#endif /* WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX */
}

static void loadbandgroup(uint_fast8_t bg, uint_fast8_t ant, uint_fast8_t rxant)
{

#if ! WITHONEATTONEAMP
	gpamp = loadvfy8up(RMT_PAMPBG3_BASE(bg, ant, rxant), 0, PAMPMODE_COUNT - 1, DEFPREAMPSTATE);	/* вытаскиваем признак включения предусилителя */
#endif /* ! WITHONEATTONEAMP */
	gatt = loadvfy8up(RMT_ATTBG3_BASE(bg, ant, rxant), 0, ATTMODE_COUNT - 1, 0);	/* вытаскиваем признак включения аттенюатора */

#if WITHAUTOTUNER
	loadtuner(bg, ant);
#endif /* WITHAUTOTUNER */
#if WITHSPECTRUMWF
	loadzoom(bg);
#endif /* WITHSPECTRUMWF */
}

/* сохранить все параметры настройки (кроме частоты) в соответствующий диапазон, ячейку памяти или VFO. */
static void
//NOINLINEAT
storebandstate(const vindex_t b, const uint_fast8_t bi)
{
	//PRINTF(PSTR("storebandstate: b=%d, bi=%d, freq=%ld\n"), b, bi, (unsigned long) gfreqs [bi]);
	verifyband(b);
	const uint_fast32_t freq = gfreqs [bi];
	const uint_fast8_t bg = getfreqbandgroup(freq);
	const uint_fast8_t ant = geteffantenna(freq);
	const uint_fast8_t rxant = geteffrxantenna(freq);

	save_i8(RMT_MODEROW_BASE(b), gmoderows [bi]);

	uint_fast8_t i;
	for (i = 0; i < MODEROW_COUNT; ++ i)
		save_i8(RMT_MODECOLS_BASE(b, i), gmodecolmaps [bi] [i]);

	storebandgroup(bg, ant, rxant);
}

/* выборка из битовой маски, Возможно, значение modecolmap бует откорректировано. */
static uint_fast8_t
NOINLINEAT
getmodecol(
	uint_fast8_t index,
	uint_fast8_t upper, // moderow
	uint_fast8_t def,
	uint_fast8_t bi)		/* bank index */
{
	uint_fast8_t v = gmodecolmaps [bi] [index];
	if (v > upper)
	{
		gmodecolmaps [bi] [index] = def;
		return def;
	}
	return v;
}

/* внести новое значение в битовую маску */
static void
//NOINLINEAT
putmodecol(
	const uint_fast8_t index, 	// moderow
	const uint_fast8_t v,
	const uint_fast8_t bi		/* bank index */
	)
{
	gmodecolmaps [bi] [index] = v;
}

/* получение режима работы "по умолчанию" для частоты. */
static uint_fast8_t
//NOINLINEAT
getdefaultsubmode(
	uint_fast32_t freq	/* частота (изображение на дисплее) */
	)
{
#if WITHMODESETSMART
	return SUBMODE_SSBSMART;
#else
	return freq < BANDFUSBFREQ ? SUBMODE_LSB : SUBMODE_USB;
#endif
}


/* получение режима работы "по умолчанию" для частоты. */
static uint_fast8_t
//NOINLINEAT
getdefaultbandsubmode(
	uint_fast32_t freq	/* частота (изображение на дисплее) */
	)
{
	const uint_fast8_t bandset_no_check = 0;
	const vindex_t b = getfreqband(freq, bandset_no_check);
	if (b != ((vindex_t) - 1) && b < HBANDS_COUNT)
		return get_band_defsubmode(b);
	return getdefaultsubmode(freq);
}


static uint_fast8_t gtxaudio [MODE_COUNT];

static uint_fast8_t gmiddlepos [MODE_COUNT];

#if WITHIF4DSP

// Начальная загрузка значений из NVRAM
// Загрузка позиции выбора полосы пропускания и значений границ для всех режимов работы
// Сохранение происходит при модификации в обработчика нажатия клавиши BW
static void
bwseti_load(void)
{
	uint_fast8_t bwprop;
	for (bwprop = 0; bwprop < BWPROPI_count; ++ bwprop)
	{
		// Значения границ полос пропускания
		bwprop_t * const p = bwprops [bwprop];

		p->afresponce = loadvfy8up(RMT_BWPROPSAFRESPONCE_BASE(bwprop), AFRESPONCEMIN, AFRESPONCEMAX, p->afresponce);
		p->fltsofter = loadvfy8up(RMT_BWPROPSFLTSOFTER_BASE(bwprop), WITHFILTSOFTMIN, WITHFILTSOFTMAX, p->fltsofter);
		switch (p->type)
		{
		case BWSET_SINGLE:
			p->left10_width10 = loadvfy8up(RMT_BWPROPSLEFT_BASE(bwprop), p->limits->left10_width10_low, p->limits->left10_width10_high, p->left10_width10);
			break;
		default:
		case BWSET_PAIR:
			p->left10_width10 = loadvfy8up(RMT_BWPROPSLEFT_BASE(bwprop), p->limits->left10_width10_low, p->limits->left10_width10_high, p->left10_width10);
			p->right100 = loadvfy8up(RMT_BWPROPSRIGHT_BASE(bwprop), p->limits->right100_low, p->limits->right100_high, p->right100);
			break;
		}
	}
	uint_fast8_t bwseti;
	for (bwseti = 0; bwseti < BWSETI_count; ++ bwseti)
	{
		// индекс выбранной полосы
		bwsetpos [bwseti] = loadvfy8up(RMT_BWSETPOS_BASE(bwseti), 0, bwsetsc [bwseti].last, bwsetpos [bwseti]);
	}
}


/* загрузка параметров слухового приема */
static void
agcseti_load(void)
{
	uint_fast8_t agcseti;
	for (agcseti = 0; agcseti < AGCSETI_COUNT; ++ agcseti)
	{
		agcp_t * const p = & gagc [agcseti];
		const struct afsetitempl * const t = & aft [agcseti];
		// параметры АРУ
		p->rate = loadvfy8up(OFFSETOF(struct nvmap, afsets [agcseti].rate), 1, AGC_RATE_FLAT, t->rate);
		p->t0 = loadvfy8up(OFFSETOF(struct nvmap, afsets [agcseti].t0), 0, 250, t->t0);
		p->t1 = loadvfy8up(OFFSETOF(struct nvmap, afsets [agcseti].t1), 10, 250, t->t1);
		p->release10 = loadvfy8up(OFFSETOF(struct nvmap, afsets [agcseti].release10), 1, 100, t->release10);
		p->t4 = loadvfy8up(OFFSETOF(struct nvmap, afsets [agcseti].t4), 10, 250, t->t4);
		p->thung10 = loadvfy8up(OFFSETOF(struct nvmap, afsets [agcseti].thung10), 0, 250, t->thung10);
		p->scale = t->scale;
	}
}

/* чтение из NVRAM параметров профилей обработки сигнала перед модулятором */
static void micproc_load(void)
{
	uint_fast8_t proci;
	uint_fast8_t i;

	for (proci = 0; proci < NMICPROFILES; ++ proci)
	{
		mikproc_t * const p = & micprofiles [proci];

		p->comp = loadvfy8up(OFFSETOF(struct nvmap, gmicprocs [proci].comp), 0, 1, p->comp);
		p->complevel = loadvfy8up(OFFSETOF(struct nvmap, gmicprocs [proci].complevel), 0, 1, p->complevel);
	}

	/* загрузка настроек - профиль аудиообработки для групп режимов работы */
	for (i = 0; i < TXAPROFIG_count; ++ i)
	{
		gtxaprofiles [i] = loadvfy8up(RMT_TXAPROFIGLE_BASE(i), 0, NMICPROFILES - 1, gtxaprofiles [i]);
	}

#if WITHTOUCHGUI
	for (i = 0; i < NMICPROFCELLS; i++)
	{
		micprof_t * const mp = & micprof_cells[i];
		mp->cell_saved = loadvfy8up(RMT_MICPSAVE_BASE(i), 0, 1, 0);
		mp->agc = loadvfy8up(RMT_MICAGC_BASE(i), 0, 1, 1);
		mp->agcgain = loadvfy8up(RMT_MICAGCGAIN_BASE(i), WITHMIKEAGCMIN, WITHMIKEAGCMAX, 30);
		mp->clip = loadvfy8up(RMT_MICCLIP_BASE(i), WITHMIKECLIPMIN, WITHMIKECLIPMAX, 0);
		mp->level = loadvfy8up(RMT_MICLEVEL_BASE(i), WITHMIKEINGAINMIN, WITHMIKEINGAINMAX, WITHMIKEINGAINMAX);
#if WITHAFCODEC1HAVEPROC
		mp->mikeboost20db = loadvfy8up(RMT_MICBOOST_BASE(i), 0, 1, 0);
		mp->eq_enable = loadvfy8up(RMT_MICEQ_BASE(i), 0, 1, 0);
		for(uint_fast8_t j = 0; j < HARDWARE_CODEC1_NPROCPARAMS; j ++)
			mp->eq_params[j] = loadvfy8up(RMT_MICEQPARAMS_BASE(i, j), 0, EQUALIZERBASE * 2, EQUALIZERBASE);
#endif /* WITHAFCODEC1HAVEPROC */
	}
#endif /* WITHTOUCHGUI */
}

#endif /* WITHIF4DSP */

///////////////////////////
//
// работа со вторым валкодером

static unsigned getselector_bandgroupant(unsigned * count)
{
    * count = 1;

    return 0;
}

static nvramaddress_t nvramoffs_bandgroupant(nvramaddress_t base, unsigned sel)
{

	ASSERT(base != MENUNONVRAM);

	if (base == MENUNONVRAM)
		return MENUNONVRAM;

	(void) sel;

	const uint_fast8_t bi = getbankindex_ab_fordisplay(0);	/* VFO A modifications */
	const uint_fast8_t bg = getfreqbandgroup(gfreqs [bi]);
    const uint_fast8_t ant = geteffantenna(gfreqs [bi]);
    const uint_fast8_t rxant = geteffrxantenna(gfreqs [bi]);

	//
	// для диапазонов - вычисляем шаг увеличения индекса по массиву хранения в диапазонах
	return base + RMT_PAMPBG3_BASE(bg, ant, rxant) - RMT_PAMPBG3_BASE(0, 0, 0);
}

#if WITHTX

#if WITHPACLASSA
#endif

#if WITHPOWERTRIM && ! WITHPOTPOWER
static const struct paramdefdef xgnormalpower =
{
	QLABEL2("TX POWER", "TX Power"), 7, 0, RJ_UNSIGNED, ISTEP1,		/* мощность при обычной работе на передачу */
    ITEM_VALUE,
    WITHPOWERTRIMMIN, WITHPOWERTRIMMAX,
    OFFSETOF(struct nvmap, gnormalpower),
    getselector0, nvramoffs0, valueoffs0,
    NULL,
    & gnormalpower.value,
    getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif

static uint_fast8_t gtxgate = 1;		/* разрешение драйвера и оконечного усилителя */
static const struct paramdefdef xgtxgate =
{
	QLABEL2("TX GATE", "TX Gate"), 8, 3, RJ_ON,	ISTEP1,
	ITEM_VALUE,
	0, 1,
	OFFSETOF(struct nvmap, gtxgate),
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& gtxgate,
	getzerobase,
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#else
enum { gtxgate = 0 };
#endif /* WITHTX */

#if WITHIF4DSP
// CW filter bandwidth for WIDE
static const struct paramdefdef xfltbw_cwwide =
{
	QLABEL3("CW W WDT", "CW W WIDTH", "CW W WIDTH"), 7, 2, 0, 	ISTEP10,	// CW bandwidth for WIDE
	ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
	10, 180,			/* 100 Hz..1800, Hz in 100 Hz steps */
	RMT_BWPROPSLEFT_BASE(BWPROPI_CWWIDE),
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& bwprop_cwwide.left10_width10,
	getzerobase,
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
// CW filter edges for WIDE
static const struct paramdefdef xfltsofter_cwwide =
{
	QLABEL3("CW W SFT", "CW W SOFT", "CW W SOFT"), 7, 0, 0, 	ISTEP1,	// CW filter edges for WIDE
	ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
	WITHFILTSOFTMIN, WITHFILTSOFTMAX,			/* 0..100 */
	RMT_BWPROPSFLTSOFTER_BASE(BWPROPI_CWWIDE),
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& bwprop_cwwide.fltsofter,
	getzerobase,
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
// CW filter bandwidth for NARROW
static const struct paramdefdef xfltbw_cwnarrow =
{
	QLABEL3("CW N WDT", "CW N WIDTH", "CW N WIDTH"), 7, 2, 0, 	ISTEP10,	// CW bandwidth for NARROW
	ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
	10, 180,			/* 100 Hz..1800, Hz in 100 Hz steps */
	RMT_BWPROPSLEFT_BASE(BWPROPI_CWNARROW),
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& bwprop_cwnarrow.left10_width10,
	getzerobase,
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
// CW filter edges for NARROW
static const struct paramdefdef xfltsofter_cwnarrow =
{
	QLABEL3("CW N SFT", "CW N SOFT", "CW N SOFT"),	// CW filter edges for NARROW
	0, 0,
	RJ_UNSIGNED,		// rj
	ISTEP1,
	ITEM_VALUE,
	WITHFILTSOFTMIN, WITHFILTSOFTMAX,			/* 0..100 */
	RMT_BWPROPSFLTSOFTER_BASE(BWPROPI_CWNARROW),
	getselector0, nvramoffs0, valueoffs0,
	NULL,
	& bwprop_cwnarrow.fltsofter,
	getzerobase, /* складывается со смещением и отображается */
	NULL, /* getvaltext получить текст значения параметра - see RJ_CB */
};
#endif /* WITHIF4DSP */

#if WITHENCODER2

static const struct paramdefdef * enc2menus [] =
{
#if WITHIF4DSP
#if ! WITHPOTAFGAIN
	& xafgain1,	// Громкость в процентах
#endif /* ! WITHPOTAFGAIN */
#if ! WITHPOTIFGAIN
	& xrfgain1,	// Усиление ПЧ/ВЧ в процентах
#endif /* ! WITHPOTIFGAIN */
	& xfltsofter_cwnarrow,	// CW filter edges for NARROW
#endif /* WITHIF4DSP */
#if WITHELKEY && ! WITHPOTWPM
	& xgelkeywpm,
#endif /* WITHELKEY && ! WITHPOTWPM */
#if WITHTX
#if WITHTXCPATHCALIBRATE
	& xgdesignscale,
#endif /* WITHTXCPATHCALIBRATE */
#if WITHPOWERTRIM && ! WITHPOTPOWER
	& xgnormalpower,
#endif /* WITHPOWERTRIM && ! WITHPOTPOWER */
#if WITHSUBTONES
	& xgsubtoneitx,	//  Continuous Tone-Coded Squelch System or CTCSS freq
	& xgsubtoneirx,	//  Continuous Tone-Coded Squelch System or CTCSS freq
#endif /* WITHPOWERTRIM */
#if WITHMIC1LEVEL
	(const struct paramdefdef [1]) {
		QLABELENC2("MIKE LEVL"),
		0, 0,
		RJ_UNSIGNED,
		ISTEP1,		/* подстройка усиления микрофонного усилителя через меню. */
		ITEM_VALUE,
		WITHMIKEINGAINMIN, WITHMIKEINGAINMAX,
		OFFSETOF(struct nvmap, gmik1level),	/* усиление микрофонного усилителя */
		getselector0, nvramoffs0, valueoffs0,
		& gmik1level,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* ITHMIC1LEVEL */
#if WITHIF4DSP
	(const struct paramdefdef [1]) {
		QLABELENC2("MIKE CLIP"),
		0, 0,
		RJ_UNSIGNED,
		ISTEP1,
		ITEM_VALUE,
		WITHMIKECLIPMIN, WITHMIKECLIPMAX, 		/* Ограничение */
		OFFSETOF(struct nvmap, gmikehclip),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmikehclip,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */
#endif /* WITHTX */
#if WITHNOTCHFREQ && ! WITHPOTNOTCH
	(const struct paramdefdef [1]) {
		QLABELENC2("NOTCH FRQ"),
		0, 0,
		RJ_UNSIGNED,		// rj
		ISTEP50,
		ITEM_VALUE,
		WITHNOTCHFREQMIN, WITHNOTCHFREQMAX,
		OFFSETOF(struct nvmap, gnotchfreq),	/* центральная частота NOTCH */
		getselector0, nvramoffs0, valueoffs0,
		& gnotchfreq.value,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHNOTCHFREQ && ! WITHPOTNOTCH */
#if WITHIF4DSP
	(const struct paramdefdef [1]) {
		QLABELENC2("NR LEVEL"),
		0, 0,
		RJ_UNSIGNED,		// rj
		ISTEP1,		/* nr level */
		ITEM_VALUE,
		0, NRLEVELMAX,
		OFFSETOF(struct nvmap, gnoisereductvl),	/* уровень сигнала болше которого открывается шумодав */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gnoisereductvl,
		getzerobase, /* складывается со смещением и отображается */
	},
#if ! WITHPOTNFMSQL
	& xgsquelchNFM,
#endif /* ! WITHPOTNFMSQL */
#if WITHSPECTRUMWF
	& xgbottomdb,
#if BOARD_FFTZOOM_POW2MAX > 0
	& xgzoomxpow2,	/* уменьшение отображаемого участка спектра */
#endif /* BOARD_FFTZOOM_POW2MAX > 0 */
	& xgviewstyle,
#endif /* WITHSPECTRUMWF */
#endif /* WITHIF4DSP */
#if WITHIFSHIFT && ! WITHPOTIFSHIFT
	// Увеличение значения параметра смещает слышимую часть спектра в более высокие частоты
	(const struct paramdefdef [1]) {
		QLABELENC2("IF SHIFT"),
		0, 0,
		RJ_SIGNED,		// rj
		ISTEP50,
		ITEM_VALUE,
		IFSHIFTTMIN, IFSHIFTMAX,			/* -3 kHz..+3 kHz in 50 Hz steps */
		OFFSETOF(struct nvmap, ifshifoffset),
		getselector0, nvramoffs0, valueoffs0,
		& ifshifoffset.value,
		NULL,
		getifshiftbase, /* складывается со смещением и отображается */
	},
#endif /* WITHIFSHIFT && ! WITHPOTIFSHIFT */
};

#define ENC2POS_COUNT (sizeof enc2menus / sizeof enc2menus [0])

/* получение названия редактируемого параметра */
static
const char *
enc2menu_label_P(
	const struct paramdefdef * const pd
	)
{
	return pd->enc2label;
}

enum
{
	ENC2STATE_INITIALIZE,
	ENC2STATE_SELECTITEM,
	ENC2STATE_EDITITEM,
	//
	ENC2STATE_COUNT
};

static uint_fast8_t enc2state = ENC2STATE_INITIALIZE;
static uint_fast8_t enc2pos;	// выбраный пунки меню

#define RMT_ENC2STATE_BASE OFFSETOF(struct nvmap, enc2state)
#define RMT_ENC2POS_BASE OFFSETOF(struct nvmap, enc2pos)

/* нажатие на второй валкодер */
static void
uif_encoder2_press(void)
{
	switch (enc2state)
	{
	case ENC2STATE_INITIALIZE:
		enc2state = ENC2STATE_SELECTITEM;
		break;
	case ENC2STATE_SELECTITEM:
		enc2state = ENC2STATE_EDITITEM;
		break;
	case ENC2STATE_EDITITEM:
		enc2state = ENC2STATE_SELECTITEM;
		break;
	}
	save_i8(RMT_ENC2STATE_BASE, enc2state);
#if ! WITHTOUCHGUI
	//display2_mode_subset(actpageix());
	//display2_needupdate();
#else
	enc2_menu.state = enc2state;
	if (enc2state != ENC2STATE_INITIALIZE)
		hamradio_gui_enc2_update();
#endif /* ! WITHTOUCHGUI */
}

/* удержанное нажатие на второй валкодер - выход из режима редактирования */
static void
uif_encoder2_hold(void)
{
	switch (enc2state)
	{
	case ENC2STATE_INITIALIZE:
		enc2state = ENC2STATE_EDITITEM;
		break;
	case ENC2STATE_SELECTITEM:
		enc2state = ENC2STATE_INITIALIZE;
		break;
	case ENC2STATE_EDITITEM:
		enc2state = ENC2STATE_INITIALIZE;
		break;
	}
	save_i8(RMT_ENC2STATE_BASE, enc2state);
#if ! WITHTOUCHGUI
	//display2_mode_subset(actpageix());
	//display2_needupdate();
#else
	if (enc2state == ENC2STATE_INITIALIZE)
		hamradio_gui_enc2_update();
#endif /* ! WITHTOUCHGUI */
}

/* обработка вращения второго валкодера */
static uint_fast8_t
uif_encoder2_rotate(
	int_least16_t nrotate	/* знаковое число - на сколько повернут валкодер */
	)
{
	if (nrotate == 0)
		return 0;

#if WITHTOUCHGUI
	if (encoder2_redirect)
		return 0;
#endif

	switch (enc2state)
	{
	case ENC2STATE_SELECTITEM:
		/* выбор параметра для редактирования */
		while (nrotate != 0)
		{
			if (nrotate > 0)
			{
				enc2pos = calc_next(enc2pos, 0, ENC2POS_COUNT - 1);
				-- nrotate;
			}
			else
			{
				enc2pos = calc_prev(enc2pos, 0, ENC2POS_COUNT - 1);
				++ nrotate;
			}
		}
		save_i8(RMT_ENC2POS_BASE, enc2pos);
		return 1;

	case ENC2STATE_EDITITEM:
		if (nrotate != 0)
		{
			param_rotate(enc2menus [enc2pos], nrotate);	// изменение и сохранение значения параметра
			updateboard();
			return 1;
		}

	default:
		break;
	}
	return 0;
}

#else /* WITHENCODER2 */

/* заглушка - возвращает 0, если не включена оьработка */
static uint_fast8_t
uif_encoder2_rotate(
	int_least16_t nrotate	/* знаковое число - на сколько повернут валкодер */
	)
{
	return 0;
}

#endif /* WITHENCODER2 */

// FUNC menu item label & value
void display2_fnblock9(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
#if WITHENCODER2 && ! WITHTOUCHGUI
	const char * const label = enc2menu_label_P(enc2menus [enc2pos]);
	char bval [xspan + 1];	// тут формируется текст для отображения
	switch (enc2state)
	{
	case ENC2STATE_INITIALIZE:
		display_2fmenuslines(db, x, y, xspan, yspan, 0, "", "");
		break;
	case ENC2STATE_SELECTITEM:
		param_format(enc2menus [enc2pos], bval, ARRAY_SIZE(bval), param_getvalue(enc2menus [enc2pos]));
		display_2fmenuslines(db, x, y, xspan, yspan, 0, label, bval);
		break;
	case ENC2STATE_EDITITEM:
		param_format(enc2menus [enc2pos], bval, ARRAY_SIZE(bval), param_getvalue(enc2menus [enc2pos]));
		display_2fmenuslines(db, x, y, xspan, yspan, 1, label, bval);
		break;
	}
#else
	display_2fmenuslines(db, x, y, xspan, yspan, 1, "FMENU9", "FVALUE9");

#endif /* WITHENCODER2 && ! WITHTOUCHGUI */
}

// split, s-meter display
static void
loadsavedstate(void)
{
#if WITHUSEFAST
	gusefast = loadvfy8up(RMT_USEFAST_BASE, 0, 1, gusefast);	/* переключение в режим крупного шага */
#endif /* WITHUSEFAST */
#if WITHUSEDUALWATCH
	mainsubrxmode = loadvfy8up(RMT_MAINSUBRXMODE_BASE, 0, MAINSUBRXMODE_COUNT - 1, mainsubrxmode);	/* состояние dual watch */
#endif /* WITHUSEDUALWATCH */
#if WITHPOWERLPHP
	gpwri = loadvfy8up(RMT_PWR_BASE, 0, PWRMODE_COUNT - 1, gpwri);
#endif /* WITHPOWERLPHP */
#if WITHNOTCHONOFF
	gnotch = loadvfy8up(RMT_NOTCH_BASE, 0, 1, gnotch);
#elif WITHNOTCHFREQ
	gnotch = loadvfy8up(RMT_NOTCH_BASE, 0, 1, gnotch);
#endif /* WITHNOTCHONOFF */
#if WITHENCODER2
	enc2state = loadvfy8up(RMT_ENC2STATE_BASE, ENC2STATE_INITIALIZE, ENC2STATE_COUNT - 1, enc2state);	/* вытаскиваем режим режактирования паарметров вторым валкодером */
	enc2pos = loadvfy8up(RMT_ENC2POS_BASE, 0, ENC2POS_COUNT - 1, enc2pos);	/* вытаскиваем номер параметра для редактирования вторым валкодером */
#endif /* WITHENCODER2 */
	gmenuset = loadvfy8up(RMT_MENUSET_BASE, 0, display_getpagesmax(), gmenuset);		/* вытаскиваем номер субменю, с которым работаем сейчас */
#if WITHSPLIT
	gsplitmode = loadvfy8up(RMT_SPLITMODE_BASE, 0, VFOMODES_COUNT - 1, gsplitmode); /* (vfo/vfoa/vfob/mem) */
	gvfoab = loadvfy8up(RMT_VFOAB_BASE, 0, VFOS_COUNT - 1, gvfoab); /* (vfoa/vfob) */
#elif WITHSPLITEX
	gsplitmode = loadvfy8up(RMT_SPLITMODE_BASE, 0, VFOMODES_COUNT - 1, gsplitmode); /* (vfo/vfoa/vfob/mem) */
	gvfoab = loadvfy8up(RMT_VFOAB_BASE, 0, VFOS_COUNT - 1, gvfoab); /* (vfoa/vfob) */
#endif /* WITHSPLIT */
#if WITHSPKMUTE
	param_load(& xgmutespkr);	/*  выключение динамика */
#endif /* WITHSPKMUTE */
	// тюнер запоминается подиапазонно
//#if WITHAUTOTUNER
//	tunerwork = loadvfy8up(OFFSETOF(struct nvmap, tunerwork), 0, 1, tunerwork);
//#endif /* WITHAUTOTUNER */
#if WITHANTSELECT2
	gantmanual = loadvfy8up(RMT_ANTMANUAL_BASE, 0, 1, gantmanual);
#endif /* WITHANTSELECT2 */

#if WITHIF4DSP
	#if WITHUSBHW && WITHUSBUAC
		gdatamode = loadvfy8up(RMT_DATAMODE_BASE, 0, 1, gdatamode);
	#endif /* WITHUSBHW && WITHUSBUAC */
	// Загрузка позиции выбора полосы пропускания и значений границ для всех режимов работы
	// Сохранение происходит при модификации в обработчика нажатия клавиши BW
	bwseti_load();

	agcseti_load();	/* загрузка параметров слухового приема */
	micproc_load();	/* чтение из NVRAM параметров профилей обработки сигнала перед модулятором */

#endif /* WITHIF4DSP */

	/* загрузка параметров, зависящих от режима */
	uint_fast8_t mode;
	for (mode = 0; mode < MODE_COUNT; ++ mode)
	{
		unsigned middlerowsize;
		mdt [mode].middlemenu(& middlerowsize);
		gmiddlepos [mode] = loadvfy8up(RMT_MIDDLEMENUPOS_BASE(mode), 0, middlerowsize - 1, gmiddlepos [mode]);
	#if WITHIF4DSP && WITHTX
		// источник звука
		gtxaudio [mode] = loadvfy8up(RMT_TXAUDIOINDEX_BASE(mode), 0, TXAUDIOSRC_COUNT - 1, findtxaudioindex(mdt [mode].txaudiocode));
	#endif /* WITHIF4DSP && WITHTX */
		/* включение NR */
	#if WITHIF4DSP
		gnoisereducts [mode] = loadvfy8up(RMT_NR_BASE(mode), 0, 1, gnoisereducts [mode]);
	#endif /* WITHIF4DSP */
	}
}

/* по диапазону вытащить все параметры (и частоту) нового диапазона */
static void
//NOINLINEAT
loadnewband(
	vindex_t b,
	uint_fast8_t bi
	)
{
	ASSERT(bi < 2);
	//PRINTF(PSTR("loadnewband: b=%d, bi=%d, freq=%ld\n"), b, bi, (unsigned long) gfreqs [bi]);

	gfreqs [bi] = loadvfy32freq(b);		/* восстанавливаем частоту */
	glock = loadvfy8up(RMT_LOCKMODE_BASE(b), 0, 1, 0);	/* вытаскиваем признак блокировки валкодера */
	const uint_fast32_t freq = gfreqs [bi];
	const uint_fast8_t bg = getfreqbandgroup(freq);
	const uint_fast8_t ant = geteffantenna(freq);
	const uint_fast8_t rxant = geteffrxantenna(freq);

#if WITHONLYBANDS
	const vindex_t hb = getfreqband(gfreqs [bi], bandset_no_check);
	tune_bottom_active [bi] = get_band_bottom(hb);
	tune_top_active [bi] = get_band_top(hb);
#endif

	const uint_fast8_t defsubmode = getdefaultbandsubmode(gfreqs [bi]);		/* режим по-умолчанию для частоты - USB или LSB */
	uint_fast8_t defrow;
	const uint_fast8_t  defcol = locatesubmode(defsubmode, & defrow);	/* строка/колонка для SSB . А что делать если не найдено? */

	// прописываем режим работы по умолчанию для данного диапазона
	gmodecolmaps [bi] [defrow] = loadvfy8up(RMT_MODECOLS_BASE(b, defrow), 0, modes [defrow][0] - 1, defcol);

	gmoderows [bi] = loadvfy8up(RMT_MODEROW_BASE(b), 0, MODEROW_COUNT - 1, defrow);

	uint_fast8_t i;
	for (i = 0; i < MODEROW_COUNT; ++ i)
	{
		gmodecolmaps [bi] [i] = loadvfy8up(RMT_MODECOLS_BASE(b, i), 0, 255, 255);	// везде прописывается 255 - потом ещё уточним.
	}

	loadantenna(bi, bg);
	loadbandgroup(bg, ant, rxant);
}

/* Получить текущий submode для указанного банка
*/
static uint_fast8_t
//NOINLINEAT
getsubmode(
	uint_fast8_t bi		/* vfo bank index */
   )
{
	ASSERT(bi < 2);
	const uint_fast8_t moderow = gmoderows [bi];
	const uint_fast8_t modecol = getmodecol(moderow, modes [moderow][0] - 1, 0, bi);	/* выборка из битовой маски, Возможно, значение modecolmap бует откорректировано. */
	return modes [moderow][modecol + 1];	/* выборка из битовой маски */
}

/* функция вызывается из updateboard при измененияя параметров приёма
 * Устанавливает "кэшированные" состояния режима работы -
   gsubmode, gstep_ENC_MAIN, gagcmode, gfi, gmode.
*/
static void
//NOINLINEAT
setgsubmode(
	uint_fast8_t submode
   )
{
	/* подправить фильтр, АРУ и шаг перестройки в соответствии с новым режимим работы */
	/* выбор фильтра */
	const struct modetempl * const pmodet = getmodetempl(submode);
	const uint_fast8_t mode = submodes [submode].mode;
	const uint_fast8_t deffilter = getdefflt(mode, 0);	/* получить индекс фильтра "по умолчанию" для режима */

	gsubmode = submode;
	gmode = mode;
	gfi = getsuitablerx(mode, loadvfy8up(RMT_FILTER_BASE(mode), 0, getgfasize() - 1, deffilter));	/* фильтр для режима приёма */
#if ! WITHAGCMODENONE
	gagcmode = loadvfy8up(RMT_AGC_BASE(mode), 0, AGCMODE_COUNT - 1, pmodet->defagcmode);
#endif /* ! WITHAGCMODENONE */
#if WITHENCODER2 && ENCODER2_HIRES
	// ENCODER2 - тоже высокого разрешения, для перестройки по частоте второго тракта
	{
		gstep_ENC_MAIN = pmodet->step10 [0] * 10;
		gstep_ENC2 = pmodet->step10 [0] * 10;
		gencderate = 1;

	}
#else
	if (gusefast || gbigstep)
	{
		gstep_ENC_MAIN = pmodet->step10 [1] * 10;
		gstep_ENC2 = pmodet->step10 [1] * 10;
		gencderate = 1;
	}
	else
	{
		gstep_ENC_MAIN = pmodet->step10 [0] * 10;
		gstep_ENC2 = pmodet->step10 [1] * 10;
		gencderate = gstep_ENC_MAIN / STEP_MINIMAL;
	}
#endif
}
#if ! WITHAGCMODENONE
#endif /* ! WITHAGCMODENONE */

#if WITHMODEM

void
modemchangefreq(
	uint_fast32_t f		// частота, которую устанавливаем по команде от CAT
	)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);
	gfreqs [bi] = vfy32up(f, TUNE_BOTTOM, TUNE_TOP - 1, gfreqs [bi]);

#if WITHONLYBANDS
	const vindex_t hb = getfreqband(gfreqs [bi], bandset_no_check);
	tune_bottom_active [bi] = get_band_bottom(hb);
	tune_top_active [bi] = get_band_top(hb);
#endif
	//gpamps [bi] = loadvfy8up(RMT_PAMP_BASE(b), 0, PAMPMODE_COUNT - 1, DEFPREAMPSTATE);	/* вытаскиваем признак включения предусилителя */
	//gatts [bi] = loadvfy8up(RMT_ATT_BASE(b), 0, ATTMODE_COUNT - 1, 0);	/* вытаскиваем признак включения аттенюатора */
	//gantennas [bi] = loadvfy8up(RMT_ANTENNA_BASE(b), 0, ANTMODE_COUNT - 1, 0);	/* вытаскиваем код включённой антенны */
	updateboard();	/* полная перенастройка (как после смены режима) */
}

void
modemchangespeed(
	uint_fast32_t speed100		// частота, которую устанавливаем по команде от CAT
	)
{
	uint_fast8_t i;

	for (i = 0; i < sizeof modembr2int100 / sizeof sizeof modembr2int100 [0]; ++ i)
	{
		if (speed100 == modembr2int100 [i])
		{
			gmodemspeed = i;
			updateboard();	/* полная перенастройка (как после смены режима) */
			return;
		}
	}
}


void
modemchangemode(
	uint_fast32_t modemmode		// частота, которую устанавливаем по команде от CAT
	)
{
	if (modemmode < 2)	/* 0: BPSK, 1: QPSK */
	{
		gmodemmode = modemmode;
		updateboard();	/* полная перенастройка (как после смены режима) */
	}
}


#endif /* WITHMODEM */

#if WITHCAT
/* функция для cat */
/* по диапазону вытащить все параметры (частота из f) нового диапазона. Старый НЕ записывать - идея в том, что записываются только по ручным операциям. */
static void
catchangefreq(
	uint_fast32_t f,		// частота, которую устанавливаем по команде от CAT
	uint_fast8_t bi
	)
{
	const uint_fast8_t bandset_no_check = 0;
	const vindex_t b = getfreqband(f, bandset_no_check);	/* определяем по частоте, в какоq диапазон переходим */
	const uint_fast8_t bg = bandsmap [b].bandgroup;

	gfreqs [bi] = f;
#if WITHONLYBANDS
	tune_bottom_active [bi] = get_band_bottom(b);
	tune_top_active [bi] = get_band_top(b);
#endif
	if (aistate != 0)
	{
#if WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2
		cat_answer_request(CAT_AN_INDEX);
#endif /* WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2 */
		cat_answer_request(CAT_RA_INDEX);
		cat_answer_request(CAT_PA_INDEX);
	}
#if WITHKEYBOARD || WITHTOUCHGUI
	// не должно быть при удаленном управлении
	loadantenna(bi, bg);
#else
	//#warning No automatic antenna select - manual only
#endif /* WITHKEYBOARD || WITHTOUCHGUI */
//	const uint_fast8_t effantenna = geteffantenna(gfreqs [bi]);
//	const uint_fast8_t effrxantenna = geteffrxantenna(gfreqs [bi]);
//	loadbandgroup(bg, effantenna, effrxantenna);
}

static void catchangesplit(
	uint_fast8_t enable,
	int_fast16_t delta
	)
{
	gsplitmode = enable ? VFOMODES_VFOSPLIT : VFOMODES_VFOINIT;
}

#endif /* WITHCAT */

/* расчёты частот ПЧ */

#if 1
/* Смена знака параметра в зависимости от lsbflag */
#define UPPERTOSIGN(lsbflag, value) (((lsbflag) == 0) ? (value) : - (value))
#define UPPERTOSIGN16(lsbflag, value) (((lsbflag) == 0) ? (value) : - (value))
#else
static int_fast32_t UPPERTOSIGN(
	uint_fast8_t lsbflag,
	int_fast32_t v
	)
{
	return lsbflag ? - v : v;
}
static int_fast16_t UPPERTOSIGN16(
	uint_fast8_t lsbflag,
	int_fast16_t v
	)
{
	return lsbflag ? - v : v;
}
#endif
/*
 * получить число герц - тон CW сигнала (в десятках герц).
 * Для телеграфа отображается частота сигнала при частоте cwpitch, при SSB - при нулевых биениях
 */
static int_least16_t
//NOINLINEAT
gettone_bymode(
	uint_fast8_t mode		/* код режима работы */
	)
{
	if (mode != MODE_CW)
		return 0;
	return gcwpitch10 * CWPITCHSCALE;
}


/*
 * получить число герц - тон CW сигнала (в десятках герц).
 * Для телеграфа отображается частота сигнала при частоте cwpitch, при SSB - при нулевых биениях
 */
static int_least16_t
//NOINLINEAT
gettone_bysubmode(
	uint_fast8_t submode,		/* код режима работы */
	uint_fast8_t forcelsb		/* когда режим работы smart, требуется этот параметр */
	)
{
	const int_least16_t t = gettone_bymode(submodes [submode].mode);
	return UPPERTOSIGN16(getsubmodelsb(submode, forcelsb), t);
}

static uint_fast8_t getforcelsb(uint_fast32_t freq)
{
	return freq < BANDFUSBFREQ;
}

/* После установки нового режима работы кнопками попадаем сюда.
 * В случае необходимости корректируется
 * частота приема (gfreq) для сохранения прежнего тона биений.
 */
static void
gsubmodechange(
	uint_fast8_t newsubmode,
	uint_fast8_t bi				/* bank index */
	)
{
	uint_fast32_t freq = gfreqs [bi];
	const uint_fast8_t forcelsb = getforcelsb(freq);

	/* коррекция частоты настройки для сохранения постоянного тона при переходе на узкую плолсу */
	const int_fast16_t delta = stayfreq ? 0 : (gettone_bysubmode(newsubmode, forcelsb) - gettone_bysubmode(gsubmode, forcelsb));

	/* Обработка изменения частоты из-за изменения режима.
		контроль изменения частоты убран, так если происходит попытка "выпрыгнуть" из
		любительского диапазона - это имеет смысл, при входе из обзорного в любительский
		ограничение по диапазону (который шире) смысла не имеет.
		*/

	if (delta < 0)
	{
		/* автоматическое изменение "вниз" */
		//const uint_fast32_t lowfreq = bandsmap [b].bottom;
		freq = prevfreq(freq, freq - (- delta), 10UL, TUNE_BOTTOM);
		gfreqs [bi] = freq;

		{
			const vindex_t v = getvfoindex(bi);
			storebandstate(v, bi); // записать все параметры настройки (кроме частоты) в область данных диапазона */
			storebandfreq(v, bi);	/* сохранение частоты в текущем VFO */
		}
	}
	else if (delta > 0)
	{
		/* автоматическое изменение "вверх" */
		//const uint_fast32_t topfreq =  bandsmap [b].top;
		//freq = nextfreq(freq, freq + delta10 * 10UL, 10UL, topfreq);
		freq = nextfreq(freq, freq + delta, 10UL, TUNE_TOP);
		gfreqs [bi] = freq;

		{
			const vindex_t v = getvfoindex(bi);
			storebandstate(v, bi); // записать все параметры настройки (кроме частоты) в область данных диапазона */
			storebandfreq(v, bi);	/* сохранение частоты в текущем VFO */
		}
	}
	else
	{
		{
			const vindex_t v = getvfoindex(bi);
			storebandstate(v, bi); // записать все параметры настройки (кроме частоты) в область данных диапазона */
		}
	}
}


#if defined (RTC1_TYPE)

	static uint_fast16_t grtcyear;
	static uint_fast8_t grtcmonth, grtcday;
	static uint_fast8_t grtchour, grtcminute, grtcseconds;

	static uint_fast8_t grtcstrobe;
	static uint_fast8_t grtcstrobe_shadow;

	static void getstamprtc(void)
	{
		grtcstrobe = 0;
		grtcstrobe_shadow = 0;
		board_rtc_getdatetime(& grtcyear, & grtcmonth, & grtcday, & grtchour, & grtcminute, & grtcseconds);
	}

	static void board_setrtcstrobe(uint_fast8_t val)
	{
		if (val != grtcstrobe_shadow && val != 0)
		{
			board_rtc_setdatetime(grtcyear, grtcmonth, grtcday, grtchour, grtcminute, 0);
		}
		grtcstrobe_shadow = val;
	}

#endif /* defined (RTC1_TYPE) */



static uint_fast8_t findcatbaudrate(uint_fast8_t old, uint_fast32_t baudrate)
{
	const uint_fast8_t v = baudrate / BRSCALE;
	uint_fast8_t i;
	for (i = 0; i < ARRAY_SIZE(catbr2int); ++ i)
	{
		if (catbr2int [i] == v)
			return i;
	}
	return old;
}

static int_fast32_t
//NOINLINEAT
getsynthref(
	uint_fast8_t mode		/* код семейства режимов работы */
	)
{
#if defined(REFERENCE_FREQ)
	(void) mode;
	return refbase + refbias;
#else
	(void) mode;
	return 0;
#endif
}
// Возврат кода варианта управления гетеродином конвертора
// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
static uint_fast8_t
getsidelo0(
	uint_fast32_t freq
	)
{
#if defined (LO0_SIDE_F)
	/* Управление гетеродином конвертора предопределено в конфигурационном файле */
	/* Возвращаемый код зависит от частоты */
	return LO0_SIDE_F(freq);	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

#elif defined (LO0_SIDE)
	/* Управление гетеродином конвертора предопределено в конфигурационном файле */
	return LO0_SIDE;	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

#else

	#error Undefined LO0_SIDE value LOCODE_XXX

#endif
}

// Возврат кода варианта управления первым гетеродином
// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
static uint_fast8_t
getsidelo1(
	uint_fast32_t freq
	)
{
#if defined (LO1_SIDE_F)
	/* Управление первым гетеродином предопределено в конфигурационном файле */
	/* Возвращаемый код зависит от частоты */
	return LO1_SIDE_F(freq);	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

#elif defined (LO1_SIDE)
	/* Управление первым гетеродином предопределено в конфигурационном файле */
	return LO1_SIDE;	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

#else

	#error Undefined LO1_SIDE value LOCODE_XXX

#endif
}

#if WITHFIXEDBFO 	/* переворачивание боковой полосы за счёт изменения частоты первого гетеродина */
	#if defined(LSBONLO4DEFAULT)
		static uint_fast8_t glo4lsb = LSBONLO4DEFAULT;	/* признак LSB на последней ПЧ - модифицируется через меню. */
	#else
		static uint_fast8_t glo4lsb = 0;	/* признак LSB на последней ПЧ - модифицируется через меню. */
	#endif
#endif /* WITHFIXEDBFO */

// Возврат кода варианта управления последнм гетеродином
// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
static uint_fast8_t
getsidelo4(void)
{
#if defined (LO4_SIDE)
	/* Управление последгтм гетеродином предопределено в конфигурационном файле */
	return LO4_SIDE;	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

#elif WITHDUALFLTR	/* Переворот боковых за счёт переключения фильтра верхней или нижней боковой полосы */

	return LOCODE_TARGETED;	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

#elif WITHFIXEDBFO	/* переворачивание боковой полосы за счёт изменения частоты первого гетеродина */

	return glo4lsb ? LOCODE_UPPER : LOCODE_LOWER;		/* признак LSB на последней ПЧ */

#else

	#error Undefined LO4_SIDE value LOCODE_XXX and no WITHDUALFLTR or WITHFIXEDBFO

#endif
}

/* получение частоты опорника конвертора */
static int_fast32_t
getlo0(
	   uint_fast32_t lo0hint
	   )
{
	return (int_fast32_t) lo0hint;
}

/* возврат желаемой частоты гетеродина конвертора */
static uint_fast32_t
gethintlo0(
	uint_fast32_t freq,
	uint_fast8_t lo0side	/* формируем гетеродин для указанной боковой полосы */
	)
{
#if XVTR_R820T2
//	const uint_fast32_t lo0step = R820T_LOSTEP;
//	// дискретность перестройки гетеродина конвертора
//	if (lo0side == LOCODE_UPPER)		/* При преобразовании на этом гетеродине происходит инверсия спектра */
//		return ((freq + R820T_IFFREQ) + lo0step / 2) / lo0step * lo0step;
//	else if (lo0side == LOCODE_LOWER)	/* При преобразовании на этом гетеродине нет инверсии спектра */
//		return ((freq - R820T_IFFREQ) + lo0step / 2) / lo0step * lo0step;
	if (lo0side == LOCODE_UPPER || lo0side ==  LOCODE_LOWER)
		return freq;
	else
		return 0;
//	return freq;
#elif 0
	// используется конвертор с фиксировнным гетеродином
	return 116000000uL;
#else
	return 0;
#endif
}


#if FQMODEL_UW3DI

static uint_fast8_t
getlo2xtal(
	uint_fast8_t mix2lsb,	/* формируем гетеродин для указанной боковой полосы */
	uint_fast8_t hintlo2	/* код пятиисотых килогерц приходит в зависимости от частоты настройки в режиме интерполятора */
	)
{
	uint_fast8_t n;
	if (mix2lsb)
		n = (hintlo2 + IF2FREQTOP / HINTLO2_GRANULARITY);
	else
		n = labs(hintlo2 - IF2FREQBOTOM / HINTLO2_GRANULARITY);
	switch (n)
	{
		case 8000000L / HINTLO2_GRANULARITY:	return 0;	/* 1.8 и 14 мегагерц */
		case 10000000L / HINTLO2_GRANULARITY:	return 1;	/* 3.5 мегагерц */
		case 13500000L / HINTLO2_GRANULARITY:	return 2;	/* 7 мегагерц */
		case 4000000L / HINTLO2_GRANULARITY:	return 3;	/* 10.1 мегагерц */
		case 12000000L / HINTLO2_GRANULARITY:	return 4;	/* 18.68 мегагерц */
		case 15000000L / HINTLO2_GRANULARITY:	return 5;	/* 21 мегагерц */
		case 18500000L / HINTLO2_GRANULARITY:	return 6;	/* 24.89 мегагерц */
		case 22000000L / HINTLO2_GRANULARITY:	return 7;	/* 28.0 мегагерц */
		case 22500000L / HINTLO2_GRANULARITY:	return 8;	/* 28.5 мегагерц */
		case 23000000L / HINTLO2_GRANULARITY:	return 9;	/* 29.0 мегагерц */
		case 23500000L / HINTLO2_GRANULARITY:	return 10;	/* 29.5 мегагерц */
	}
	return 0;
}

#endif	/* FQMODEL_UW3DI */

/* Возврат номера кварца для второго гетеродина */
static uint_fast8_t
gethintlo2(
	uint_fast32_t freq
	)
{
#if FQMODEL_UW3DI
	return freq / HINTLO2_GRANULARITY;
#else
	return 0;
#endif
}

/* получаем частоту LO2 для текущего режима работы */
static int_fast32_t
//NOINLINEAT
getlo2(
	const filter_t * workfilter,
	uint_fast8_t mode,		/* код семейства режимов работы */
	uint_fast8_t mix2lsb,	/* формируем гетеродин для указанной боковой полосы */
	uint_fast8_t tx,		/* признак работы в режиме передачи */
	uint_fast8_t hintlo2	/* код пятиисотых килогерц приходит в зависимости от частоты настройки в режиме интерполятора */
	)
{
#if WITHWFM
	if (mode == MODE_WFM)
		return 0;
#endif /* WITHWFM */
#if FQMODEL_10M7_500K && (LO2_SIDE == LOCODE_TARGETED)

	/* RU6BK: 1-st if=10.7 MHz, 2-nd if=500k, fixed BFO */
	const int_fast32_t if2 = 10700000L;
	const int_fast32_t if3 = 500000L;
	if (mix2lsb)
		return if2 + if3;
	else
		return if2 - if3;

#elif FQMODEL_UW3DI
	// LO2 формируется набором кварцев или PLL
	// получаем частоту для справки
	if (mix2lsb)
		return (hintlo2 + IF2FREQTOP / HINTLO2_GRANULARITY) * HINTLO2_GRANULARITY;
	else
		return labs(hintlo2 - IF2FREQBOTOM / HINTLO2_GRANULARITY) * HINTLO2_GRANULARITY;

#elif /*defined (PLL2_TYPE) && */ (LO2_SIDE != LOCODE_INVALID)
	// LO2 формируется PLL с зависимостью от режима работы и приёма/передачи
	return (int_fast32_t) (((uint_least64_t) getsynthref(mode) * * getplo2n(workfilter, tx) / * getplo2r(workfilter, tx)) >> LO2_POWER2);

#elif (LO2_SIDE != LOCODE_INVALID)
	// LO2 формируется PLL или прямо берётся с опорного генератора
	(void) tx;
	return (int_fast32_t) (((uint_least64_t) getsynthref(mode) * LO2_PLL_N / LO2_PLL_R) >> LO2_POWER2);

#else
	// LO2 отсутствует
	(void) mode;
	(void) tx;
	return 0;

#endif
}

/* получаем LO3 для текущего режима работы */
static int_fast32_t
//NOINLINEAT
getlo3(
	uint_fast8_t mode,		/* код семейства режимов работы */
	uint_fast8_t mix3lsb,		/* формируем гетеродин для указанной боковой полосы */
	uint_fast8_t tx		/* признак работы в режиме передачи */
	)
{
#if (LO3_SIDE != LOCODE_INVALID)
  #if LO3_FREQADJ	/* подстройка частоты гетеродина через меню. */
	return (lo3base + lo3offset) >> LO3_POWER2;
  #else
	return (int_fast32_t) ((uint_least64_t) getsynthref(mode) * LO3_PLL_N / LO3_PLL_R >> LO3_POWER2);
  #endif

#else
	/* смесителя #2A вообще нет в тракте */
	(void) mode;
	(void) mix3lsb;
	return 0;
#endif
}

/* перенастройка формирования гетеродина для указанного режима. */
static void
//NOINLINEAT
update_lo0(
	uint_fast32_t lo0hint,		/* код частоты ковертора (не всегда совпадает с частотой) */
	uint_fast8_t lo0side
	)
{
	const uint_fast8_t enable = lo0side != LOCODE_INVALID;
	board_set_xvrtr(enable);
	synth_lo0_setfreq(getlo0(lo0hint), enable);
#if WITHIF4DSP
#if XVTR_R820T2
	board_set_swaprts(enable);	/* При преобразовании на этом гетеродине происходит инверсия спектра */
#else /* XVTR_R820T2 */
	board_set_swaprts(lo0side == LOCODE_LOWER);	/* При преобразовании на этом гетеродине происходит инверсия спектра */
#endif /* XVTR_R820T2 */
#endif /* WITHIF4DSP */
}

/* перенастройка формирования гетеродина для указанного режима. */
static void
//NOINLINEAT
update_lo2(
	uint_fast8_t pathi,		// номер тракта - 0/1: main/sub
	const filter_t * workfilter,
	uint_fast8_t mode,		/* код семейства режимов работы */
	uint_fast8_t lsb,		/* формируем гетеродин для указанной боковой полосы */
	int_fast32_t f,			/* частота, которую хотим получить на выходе DDS */
	uint_fast8_t od,		/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	uint_fast8_t tx,		/* признак работы в режиме передачи */
	uint_fast8_t hint		/* код пятиисотых килогерц приходит в зависимости от частоты настройки в режиме интерполятора */
	)
{
#if FQMODEL_UW3DI
	board_set_lo2xtal(getlo2xtal(lsb, hint));	// установка номера кварца
#elif defined (PLL2_TYPE) && (LO2_SIDE != LOCODE_INVALID)
	(void) hint;
	prog_pll2_r(getplo2r(workfilter, tx));		/* программирование PLL2 в случае управляемой частоты второго гетеродина */
	prog_pll2_n(getplo2n(workfilter, tx));		/* программирование PLL2 в случае управляемой частоты второго гетеродина */
#elif LO2_DDSGENERATED
	(void) mode;
	(void) hint;
	(void) lsb;
	(void) tx;
	synth_lo2_setfreq(pathi, f, od);		// for IGOR or EW2DZ or FQMODEL_10M7_500K
#else
	/* смесителя #2 вообще нет в тракте */
	(void) mode;
	(void) hint;
	(void) lsb;
	(void) f;
	(void) od;
	(void) tx;
#endif
}


/* перенастройка формирования гетеродина для указанного режима. */
static void
//NOINLINEAT
update_lo3(
	uint_fast8_t pathi,		// номер тракта - 0/1: main/sub
	uint_fast8_t mode,		/* код семейства режимов работы */
	uint_fast8_t lsb,		/* формируем гетеродин для указанной боковой полосы */
	int_fast32_t f,			/* частота, которую хотим получить на выходе DDS */
	uint_fast8_t od,		/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	uint_fast8_t tx			/* признак работы в режиме передачи */
	)
{
	(void) mode;
	(void) lsb;
	(void) tx;
	synth_lo3_setfreq(pathi, f, od);
}

// return value: LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
static uint_fast8_t
getsidelo6(
	uint_fast8_t mode,		/* код семейства режимов работы */
	uint_fast8_t tx
	)
{
#if defined (LO6_SIDE)
	/* Управление этим гетеродином предопределено в конфигурационном файле */
	return LO6_SIDE;	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
#elif WITHIF4DSP
	return LOCODE_TARGETED;	/* Этот гетеродин управляется для получения требуемой боковой */
#else /* WITHIF4DSP */
	return LOCODE_LOWER;	/* При преобразовании на этом гетеродине нет инверсии спектра */
#endif /* WITHIF4DSP */
}

#if WITHIF4DSP

// Значение частоты среза для фильтра ПЧ
// при применении алгоритма Уивера.
// Возвращаем полную полосу пропускания тракта.
// INT16_MAX - для обхода фильтра
static  int_fast16_t
getif6bw(
	uint_fast8_t mode,		/* код семейства режимов работы */
	uint_fast8_t tx,
	uint_fast8_t wide		/* способ расчета полосы пропускания */
	)
{
	const uint_fast8_t bwseti = mdt [mode].bwsetis [tx];	// индекс банка полос пропускания для данного режима
	const int_fast16_t bw6 = mdt [mode].bw6s [tx];
	if (bw6 != 0)
		return bw6;

	switch (mode)
	{
#if WITHMODEM
	case MODE_MODEM:
		// для передачи берется из таблицы
		return (uint_fast64_t) modembr2int100 [gmodemspeed] * 28 / 100 / 10;	// bw=symbol rate * 1.4

#endif /* WITHMODEM */

	case MODE_NFM:
		return bwseti_getnfmbw(bwseti, tx);

#if WITHFREEDV
	case MODE_FREEDV:
#endif /* WITHFREEDV */
	case MODE_CW:
	case MODE_SSB:
	case MODE_RTTY:
	case MODE_DIGI:
		if (! wide)
		{
			return bwseti_getwidth(bwseti);
		}
		else
		{
			const int_fast16_t lowcut = bwseti_getlow(bwseti);
			const int_fast16_t highcut = bwseti_gethigh(bwseti);
			return (highcut - lowcut);
		}

	case MODE_ISB:
		{
			const int_fast16_t highcut = bwseti_gethigh(bwseti);
			return 2 * highcut;
		}

	// большинство режимов по констатне из тассива mdt [mode].
	default:
		if (tx)
			return INT16_MAX;	/* PASSTROUGH */
		return bwseti_gethigh(bwseti) * 2;
	}
	return 0;
}


/* получить левый (низкочастотный) скат полосы пропускания для отображения "шторки" на спектранализаторе */
int_fast16_t
hamradio_getleft_bp(uint_fast8_t pathi)
{
	//const uint_fast8_t bi = getbankindex_pathi(pathi);
	const uint_fast8_t bi = getbankindex_ab_fordisplay(pathi);
	const uint_fast32_t freq = gfreqs [bi];
	const uint_fast8_t forcelsb = getforcelsb(freq);
	const uint_fast8_t submode = getsubmode(bi);	// брать модуляцию нужного приемника
	const uint_fast8_t alsbmode = getsubmodelsb(submode, forcelsb);	// Принимаемая модуляция на нижней боковой
	const struct modetempl * const pmodet = getmodetempl(submode);
	const uint_fast8_t bwseti = pmodet->bwsetis [gtx];
	const uint_fast8_t mode = submodes [submode].mode;
	const int_fast16_t cwpitch = gcwpitch10 * CWPITCHSCALE;

	switch (mode)
	{
	case MODE_SSB:
	case MODE_DIGI:
		return alsbmode ? - bwseti_gethigh(bwseti) : bwseti_getlow(bwseti);

	case MODE_CW:
		return alsbmode ? - bwseti_gethigh(bwseti) + cwpitch: bwseti_getlow(bwseti) - cwpitch;

	default:
		return - getif6bw(mode, gtx, bwseti_getwide(bwseti)) / 2;	// TODO: учесть возврат INT16_MAX
	}
}

/* получить правый (высокочастотный) скат полосы пропускания для отображения "шторки" на спектранализаторе */
int_fast16_t
hamradio_getright_bp(uint_fast8_t pathi)
{
	//const uint_fast8_t bi = getbankindex_pathi(pathi);
	const uint_fast8_t bi = getbankindex_ab_fordisplay(pathi);
	const uint_fast32_t freq = gfreqs [bi];
	const uint_fast8_t forcelsb = getforcelsb(freq);
	const uint_fast8_t submode = getsubmode(bi);	// брать модуляцию нужного приемника
	const uint_fast8_t alsbmode = getsubmodelsb(submode, forcelsb);	// Принимаемая модуляция на нижней боковой
	const struct modetempl * const pmodet = getmodetempl(submode);
	const uint_fast8_t bwseti = pmodet->bwsetis [gtx];
	const uint_fast8_t mode = submodes [submode].mode;
	const int_fast16_t cwpitch = gcwpitch10 * CWPITCHSCALE;

	switch (mode)
	{
	case MODE_SSB:
	case MODE_DIGI:
		return alsbmode ? - bwseti_getlow(bwseti) : bwseti_gethigh(bwseti);

	case MODE_CW:
		return alsbmode ? - bwseti_getlow(bwseti) + cwpitch : bwseti_gethigh(bwseti) - cwpitch;

	default:
		return getif6bw(mode, gtx, bwseti_getwide(bwseti)) / 2;	// TODO: учесть возврат INT16_MAX
	}
}

#endif /* WITHIF4DSP */

// Значение частоты для восстановления сигнала
static  int_fast32_t
getlo6(
	uint_fast8_t mode,		/* код семейства режимов работы */
	uint_fast8_t tx,
	uint_fast8_t wide,		/* способ осчёта полосы пропускания */
	int_fast16_t ifshift
	)
{
#if WITHIF4DSP
	const uint_fast8_t bwseti = mdt [mode].bwsetis [tx];	// индекс банка полос пропускания для данного режима
	switch (mode)
	{
	case MODE_ISB:
		return 0;

	case MODE_SSB:
	case MODE_DIGI:
	case MODE_CW:
		if (wide)
		{
			const int_fast16_t lowcut = bwseti_getlow(bwseti);
			const int_fast16_t highcut = bwseti_gethigh(bwseti);
			return (highcut - lowcut) / 2 + lowcut + ifshift; 	// частота центра полосы пропускания
		}
		else
		{
			return tx != 0 ? 0 : gcwpitch10 * CWPITCHSCALE;
		}

	case MODE_DRM:
		return DEFAULT_DRM_PITCH;	/* тон DRM - 12 кГц*/

	case MODE_RTTY:
		return DEFAULT_RTTY_PITCH;	/* тон DIGI modes - 2.125 кГц */

#if WITHMODEM
	case MODE_MODEM:
		return tx ? 0 : 0;		// модем принимает квадратуры
#endif /* WITHMODEM */

#if WITHFREEDV
	case MODE_FREEDV:
		return tx ? 0 : 0;		// модем принимает квадратуры
#endif /* WITHFREEDV */

	default:
		// AM. NFM и остальные - принимаются на baseband
		return 0;
	}
#else /* WITHIF4DSP */
	return 0; //- ifshift;	// В недопустимых режимах значение 0 уже обеспечено.
#endif /* WITHIF4DSP */
}

// Выходные частоты устройства (для SSB - частота подавленной несущей).
// Для классических структутр приёмо-передатчиков передача телеграфа делается разбалансом
// формирователя SSB или иным способом из сигнала гетеродина - это указывается
// возвратом "0" в режиме CW на передаче.
// Для IF DSP и DDC/DUC структур нулевая частота при передаче означает подачу на DUC постоянной составляющей в baseband.

static  int_fast32_t
getif6(
	uint_fast8_t mode,		/* код семейства режимов работы */
	uint_fast8_t tx,
	uint_fast8_t wide		/* способ осчёта полосы пропускания */
	)
{
	switch (mode)
	{
	case MODE_CW:
		return tx != 0 ? 0 : gcwpitch10 * CWPITCHSCALE;

	case MODE_DRM:
		return DEFAULT_DRM_PITCH;	/* тон DRM - 12 кГц*/

	case MODE_RTTY:
		return DEFAULT_RTTY_PITCH;	/* тон RTTY modes - 2.125 кГц */

	default:
		return 0;
	}
}

/*
 * Возврат значения ПЧ для цифровой обработки
 * 1/4 FS (12 kHz) или 0
*/
static  int_fast32_t
getlo5(
	uint_fast8_t mode,		/* код семейства режимов работы */
	uint_fast8_t tx
	)
{
#if WITHIF4DSP
	return dsp_get_ifreq();	/* 1/4 FS (12 kHz) или 0 для DSP */
#else /* WITHIF4DSP */
	return 0;
#endif /* WITHIF4DSP */
}

// Сторона гетеродина, переносящего сигнал ПЧ с 12 кГц на baseband для цифровой обработки
// Или, в классических структурах, перенос с последней ПЧ на звук.
// return value: LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

static uint_fast8_t
getsidelo5(
	uint_fast8_t mode,		/* код семейства режимов работы */
	uint_fast8_t tx
	)
{
#if defined (LO5_SIDE)
	/* Управление этим гетеродином предопределено в конфигурационном файле */
	return LO5_SIDE;	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
#elif WITHFIXEDBFO || WITHDUALBFO	/* переворачивание боковой полосы за счёт изменения частоты первого гетеродина */
	return LOCODE_LOWER;	/* При преобразовании на этом гетеродине нет инверсии спектра */
#else
	return mdt [mode].lo5side [tx];
#endif /* WITHFIXEDBFO || WITHDUALBFO */
}

static uint_fast8_t
getlo4enable(
	uint_fast8_t mode,
	uint_fast8_t tx
	)
{
#if defined (LO5_SIDE)
	return 1;
#else
	return mdt [mode].lo5side [tx] != LOCODE_INVALID;
#endif
}


static int_fast32_t
getlo4ref(
	const filter_t * workfilter,
	uint_fast8_t mode,			/* код семейства режима работы */
	uint_fast8_t mix4lsb,		/* формируем гетеродин для указанной боковой полосы */
	uint_fast8_t tx				/* для режима передачи - врежиме CW - смещения частоты не требуется. */
	)
{
#if WITHFIXEDBFO || WITHDUALFLTR
	// Выравнивание IF с привязкой к скату фильтра основной селекции
	return getlo4baseflt(workfilter) + lo4offset;
#elif WITHDUALBFO
	// Выравнивание IF с привязкой к скату фильтра основной селекции
	return getlo4baseflt(workfilter) + lo4offsets [mix4lsb];
#elif WITHIF4DSP
	// Выравнивание IF с привязкой к центру фильтра основной селекции
	return getif3filtercenter(workfilter);
#else
	return getif3byedge(workfilter, mode, mix4lsb, tx, gcwpitch10);
#endif
}

// вызывается из user mode
static uint_fast8_t
getactualtune(void)
{
	txreq_t * const txreqp = & txreqst0;

	return txreq_gettxtone(txreqp) || txreq_getreqautotune(txreqp);
}

// вызывается из user mode
// Возвращает поизнак необходимости сбросить мощность сейчас (например, запрос от автотюнера)
uint_fast8_t
getactualdownpower(txreq_t * txreqp)
{
	return
	#if WITHTX
		(gdownatcwtune && txreq_gettxtone(txreqp)) ||	/* снижаем мощность до "тюнерной" при нажатии TUNE */
		txreq_getreqautotune(txreqp) || hardware_get_tune() ||
	#endif /* WITHTX */
		0;
}

#if WITHTX

/* возвращаем 0..100 для кода на разъеме ACC (bandf3hint) */
static uint_fast8_t
makebandf2adjust(
	uint_fast8_t lpfno, 	// 0..15 - код диапазона bandf3hint
	int amplitude	// 0..WITHPOWERTRIMMAX 0..100 - относительная мощность
	)
{
	if (lpfno >= ARRAY_SIZE(gbandf2adj))
		return amplitude;

#if WITHPACLASSA
	if (gclassamode)
		return gbandf2adj [lpfno].adj_classa * amplitude / WITHPOWERTRIMMAX;
#endif /* WITHPACLASSA */

	// расчет наклона графика
	const int a_ref = 31;	// значение множителя для точки а - sqrt(10000)
	const int b_ref = 100;	// значение множителя для точки b

	const int a = ulmin(gbandf2adj [lpfno].adj_a, gbandf2adj [lpfno].adj_b);	/* 10%	*/
	const int b = ulmax(gbandf2adj [lpfno].adj_a, gbandf2adj [lpfno].adj_b);	/* 100%	*/

	/* наклон графика */
	const int multiplier = b - a;
	const int divider = b_ref - a_ref;	/* входная разница 100 - 10 процентов от амплитуды */

	const int distance_a = amplitude - a_ref;
	const int outv = (int) ((long) distance_a * multiplier / divider) + a;

	//PRINTF("makebandf2adjust: ampl=%u, a=%u, b=%u\n", amplitude, a, b);
	return slmin(b, slmax(outv, 0));
}

/* возвращает 0..WITHPOWERTRIMMAX */
static uint_fast8_t
getactualtxpwr(void)
{
	return getactualdownpower(& txreqst0) ? gtunepower : gnormalpower.value;
}

/* Возвращает 0..WITHPOWERTRIMMAX */
static uint_fast8_t
getactualtxampl(void)
{
#if WITHPOWERTRIM
	unsigned v = getactualtxpwr();
	return sqrtf((float) v / WITHPOWERTRIMMAX) * WITHPOWERTRIMMAX;

#elif WITHPOWERLPHP
	return WITHPOWERTRIMMAX;

#else
	return WITHPOWERTRIMMAX;

#endif /* WITHPOWERLPHP */
}

/* Возвращает  BOARDPOWERMIN..BOARDPOWERMAX */
static uint_fast8_t
getactualtxboard(void)
{
#if WITHPOWERTRIM
	return BOARDPOWERMAX;

#elif WITHPOWERLPHP
	/* установить выходную мощность передатчика BOARDPOWERMIN..BOARDPOWERMAX */
	return getactualdownpower(& txreqst0) ? pwrmodes [gpwratunei].code : pwrmodes [gpwri].code;

#else
	return BOARDPOWERMAX;

#endif /* WITHPOWERLPHP */
}

#endif /* WITHTX */

// получить submode для тракта, возможно замененный на CWZ в случае формирования сигнала настройки.
static uint_fast8_t
getasubmode(uint_fast8_t pathi)
{
	const uint_fast8_t bi = getbankindex_pathi(pathi);	/* vfo bank index */
	/* преустановка всех параметров приемника или передатчика под новый режим */
	const uint_fast8_t pathsubmode = getsubmode(bi);

#if WITHMODESETSMART
	const uint_fast8_t submode = (getactualtune() == 0) ? pathsubmode : SUBMODE_CWZSMART;	// mode_cwz обеспечивает формирование сигнала самоконтроля при передаче.
#else /* WITHMODESETSMART */
	const uint_fast8_t submode = (getactualtune() == 0) ? pathsubmode : SUBMODE_CWZ;	// mode_cwz обеспечивает формирование сигнала самоконтроля при передаче.
#endif /* WITHMODESETSMART */
	return submode;
}

// get actual mode
// Только показ названия режима (и CAT) используют getsubmode(bi);
// Для установки режимов надо использовать данную функцию.
static uint_fast8_t
getamode(uint_fast8_t pathi)
{
	const uint_fast8_t asubmode = getasubmode(pathi);
	return gdatamode && getactualtune() == 0 ? MODE_DIGI :  submodes [asubmode].mode;
}


/*
 * Установка параметров, влияющих на работу валкодера, цветовой схемой дисплея.
 */
static void
//NOINLINEAT
updateboard2(void)
{
#if WITHENCODER
	encoder_set_resolution(& encoder1, encresols [genc1pulses], genc1dynamic);
#endif /* WITHENCODER */
	display2_setbgcolor(gbluebgnd ? COLORPIP_BLUE : COLORPIP_BLACK);
}


static uint_fast8_t
//NOINLINEAT
getlsbfull(
	uint_fast8_t lsb,
	const uint_fast8_t * sides,
	uint_fast8_t size
	)
{
	while (size --)
		lsb ^= * sides ++ == LOCODE_UPPER;
	return lsb;
}

/* Получение признака LSB для LO0..LO6 */
static uint_fast8_t
//NOINLINEAT
getlsbloX(
	uint_fast8_t lsb,
	uint_fast8_t keyindex,	// 0..6 - номер LOx
	const uint_fast8_t * sides,
	uint_fast8_t size
	)
{
	const uint_fast8_t keyside = sides [keyindex];
	if (keyside == LOCODE_TARGETED)
		return getlsbfull(lsb, sides, size);
	return keyside == LOCODE_UPPER;
}

/* получение значение делителя в тракте lo1 перед подачей на смеситель */
static uint_fast8_t getlo1div(
	uint_fast8_t tx
	)
{
#if LO1FDIV_ADJ
	return 1U << lo1powmap [tx];
#elif defined (LO1_POWER2)
	return 1U << LO1_POWER2;
#else
	return 1U;
#endif
}

/* получение значение делителя в тракте lo2 перед подачей на смеситель */
static uint_fast8_t getlo2div(
	uint_fast8_t tx
	)
{
#if LO2FDIV_ADJ
	return 1U << lo2powmap [tx];
#elif defined (LO2_POWER2)
	return 1U << LO2_POWER2;
#else
	return 1U;
#endif
}

/* получение значение делителя в тракте lo3 перед подачей на смеситель */
static uint_fast8_t getlo3div(
	uint_fast8_t tx
	)
{
#if LO3FDIV_ADJ
	return 1U << lo3powmap [tx];
#elif defined (LO3_POWER2)
	return 1U << LO3_POWER2;
#else
	return 1U;
#endif
}

/* получение значение делителя в тракте lo4 перед подачей на смеситель */
static uint_fast8_t getlo4div(
	uint_fast8_t tx
	)
{
#if LO4FDIV_ADJ
	return 1U << lo4powmap [tx];
#elif defined (LO4_POWER2)
	return 1U << LO4_POWER2;
#else
	return 1U;
#endif
}

// speex

#if WITHINTEGRATEDDSP


#ifdef WITHLEAKYLMSANR

#define LEAKYLMSDLINE_SIZE 256 //512 // was 256 //2048   // dline_size
// 1024 funktioniert nicht
typedef struct
{// Automatic noise reduction
	// Variable-leak LMS algorithm
	// taken from (c) Warren Pratts wdsp library 2016
	// GPLv3 licensed
//	#define DLINE_SIZE 256 //512 //2048  // dline_size
	int16_t n_taps; // =     64; //64;                       // taps
	int16_t delay; // =    16; //16;                       // delay
	int dline_size; // = LEAKYLMSDLINE_SIZE;
	//int ANR_buff_size = FFT_length / 2.0;
	int position;// = 0;
	float32_t two_mu;// =   0.0001;   typical: 0.001 to 0.000001  = 1000 to 1 -> div by 1000000     // two_mu --> "gain"
	uint32_t two_mu_int;
	float32_t gamma;// =    0.1;      typical: 1.000 to 0.001  = 1000 to 1 -> div by 1000           // gamma --> "leakage"
	uint32_t gamma_int;
	float32_t lidx;// =     120.0;                      // lidx
	float32_t lidx_min;// = 0.0;                      // lidx_min
	float32_t lidx_max;// = 200.0;                      // lidx_max
	float32_t ngamma;// =   0.001;                      // ngamma
	float32_t den_mult;// = 6.25e-10;                   // den_mult
	float32_t lincr;// =    1.0;                      // lincr
	float32_t ldecr;// =    3.0;                     // ldecr
	//int ANR_mask = ANR_dline_size - 1;
	//int mask;// = DLINE_SIZE - 1;
	int in_idx;// = 0;
	float32_t d [LEAKYLMSDLINE_SIZE];
	float32_t w [LEAKYLMSDLINE_SIZE];
	uint8_t on;// = 0;
	uint8_t notch;// = 0;
} lLMS;

static lLMS leakyLMS;

static void AudioDriver_LeakyLmsNr_Init(void)
{
    /////////////////////// LEAKY LMS noise reduction
    leakyLMS.n_taps =     64; //64;                       // taps
    leakyLMS.delay =    16; //16;                       // delay
    leakyLMS.dline_size = LEAKYLMSDLINE_SIZE;
    //int ANR_buff_size = FFT_length / 2.0;
    leakyLMS.position = 0;
    leakyLMS.two_mu =   0.0001;                     // two_mu --> "gain"
    leakyLMS.two_mu_int = 100;
    leakyLMS.gamma =    0.1;                      // gamma --> "leakage"
    leakyLMS.gamma_int = 100;
    leakyLMS.lidx =     120.0;                      // lidx
    leakyLMS.lidx_min = 0.0;                      // lidx_min
    leakyLMS.lidx_max = 200.0;                      // lidx_max
    leakyLMS.ngamma =   0.001;                      // ngamma
    leakyLMS.den_mult = 6.25e-10;                   // den_mult
    leakyLMS.lincr =    1.0;                      // lincr
    leakyLMS.ldecr =    3.0;                     // ldecr
    //int leakyLMS.mask = leakyLMS.dline_size - 1;
    //leakyLMS.mask = LEAKYLMSDLINE_SIZE - 1;
    leakyLMS.in_idx = 0;
    leakyLMS.on = 0;
    leakyLMS.notch = 0;
    /////////////////////// LEAKY LMS END

}

// Automatic noise reduction
// Variable-leak LMS algorithm
// taken from (c) Warren Pratts wdsp library 2016
// GPLv3 licensed
void AudioDriver_LeakyLmsNr(float32_t * in_buff, float32_t * out_buff, int buff_size, int notch)
{
    int i, j, idx;
    float32_t c0, c1;
    float32_t y, error, sigma, inv_sigp;
    float32_t nel, nev;
	for (i = 0; i < buff_size; i++)
	{
		leakyLMS.d [leakyLMS.in_idx] = in_buff[i];

		y = 0;
		sigma = 0;

		for (j = 0; j < leakyLMS.n_taps; j ++)
		{
			idx = (leakyLMS.in_idx + j + leakyLMS.delay) % leakyLMS.dline_size;
			y += leakyLMS.w [j] * leakyLMS.d [idx];
			sigma += leakyLMS.d [idx] * leakyLMS.d [idx];
		}
		inv_sigp = 1 / (sigma + (float32_t) 1e-10);
		error = leakyLMS.d [leakyLMS.in_idx] - y;

		if (notch)
		{ // automatic notch filter
			out_buff[i] = error;
		}
		else
		{ // noise reduction
			out_buff[i] = y;
		}
//          leakyLMS.out_buff[2 * i + 1] = 0.0;

		if ((nel = error * (1 - leakyLMS.two_mu * sigma * inv_sigp)) < 0)
			nel = - nel;
		if ((nev = leakyLMS.d [leakyLMS.in_idx] - (1 - leakyLMS.two_mu * leakyLMS.ngamma) * y - leakyLMS.two_mu * error * sigma * inv_sigp) < 0)
			nev = - nev;
		if (nev < nel)
		{
			if ((leakyLMS.lidx += leakyLMS.lincr) > leakyLMS.lidx_max)
				leakyLMS.lidx = leakyLMS.lidx_max;
		}
		else
		{
			if ((leakyLMS.lidx -= leakyLMS.ldecr) < leakyLMS.lidx_min)
				leakyLMS.lidx = leakyLMS.lidx_min;
		}
		leakyLMS.ngamma = leakyLMS.gamma * (leakyLMS.lidx * leakyLMS.lidx) * (leakyLMS.lidx * leakyLMS.lidx) * leakyLMS.den_mult;

		c0 = 1 - leakyLMS.two_mu * leakyLMS.ngamma;
		c1 = leakyLMS.two_mu * error * inv_sigp;

		for (j = 0; j < leakyLMS.n_taps; j++)
		{
			idx = (leakyLMS.in_idx + j + leakyLMS.delay) % leakyLMS.dline_size;
			leakyLMS.w [j] = c0 * leakyLMS.w [j] + c1 * leakyLMS.d [idx];
		}
		leakyLMS.in_idx = (leakyLMS.in_idx + leakyLMS.dline_size - 1) % leakyLMS.dline_size;
	}
}

#endif /* WITHLEAKYLMSANR */

#if WITHRTTY

#define TRX_SAMPLERATE ARMI2SRATE

#define BIQUAD_COEFF_IN_STAGE 5													  // coefficients in manual Notch filter order

#if (defined(LAY_800x480))
#define RTTY_DECODER_STRLEN 66 // length of decoded string
#else
#define RTTY_DECODER_STRLEN 30 // length of decoded string
#endif

#define RTTY_LPF_STAGES 2
#define RTTY_BPF_STAGES 2
#define RTTY_BPF_WIDTH (RTTY_Shift / 4)

#define RTTY_SYMBOL_CODE (0b11011)
#define RTTY_LETTER_CODE (0b11111)

typedef enum {
	RTTY_STATE_WAIT_START,
	RTTY_STATE_BIT,
} rtty_state_t;

typedef enum {
	RTTY_MODE_LETTERS,
	RTTY_MODE_SYMBOLS
} rtty_charSetMode_t;

typedef enum {
    RTTY_STOP_1,
    RTTY_STOP_1_5,
    RTTY_STOP_2
} rtty_stopbits_t;

// Public variables
//extern char RTTY_Decoder_Text[RTTY_DECODER_STRLEN + 1];

// Public methods
extern void RTTYDecoder_Init(void);                   // initialize the CW decoder
extern void RTTYDecoder_Process(const float32_t *bufferIn, unsigned len); // start CW decoder for the data block


//Ported from https://github.com/df8oe/UHSDR/blob/active-devel/mchf-eclipse/drivers/audio/rtty.c

//char RTTY_Decoder_Text[RTTY_DECODER_STRLEN + 1] = {0}; // decoded string

static rtty_state_t RTTY_State = RTTY_STATE_WAIT_START;
static rtty_charSetMode_t RTTY_charSetMode = RTTY_MODE_LETTERS;
static uint16_t RTTY_oneBitSampleCount = 0;
static uint8_t RTTY_byteResult = 0;
static uint16_t RTTY_byteResult_bnum = 0;
static int32_t RTTY_DPLLBitPhase;
static int32_t RTTY_DPLLOldVal;

//lpf
static float32_t RTTY_LPF_Filter_Coeffs[BIQUAD_COEFF_IN_STAGE * RTTY_LPF_STAGES] = {0};
static float32_t RTTY_LPF_Filter_State[2 * RTTY_LPF_STAGES];
static arm_biquad_cascade_df2T_instance_f32 RTTY_LPF_Filter;

//mark
static float32_t RTTY_Mark_Filter_Coeffs[BIQUAD_COEFF_IN_STAGE * RTTY_BPF_STAGES];
static float32_t RTTY_Mark_Filter_State[2 * RTTY_BPF_STAGES];
static arm_biquad_cascade_df2T_instance_f32 RTTY_Mark_Filter;

//space
static float32_t RTTY_Space_Filter_Coeffs[BIQUAD_COEFF_IN_STAGE * RTTY_BPF_STAGES];
static float32_t RTTY_Space_Filter_State[2 * RTTY_BPF_STAGES];
static arm_biquad_cascade_df2T_instance_f32 RTTY_Space_Filter;

static const char RTTY_Letters[] = {
	'\0', 'E', '\n', 'A', ' ', 'S', 'I', 'U',
	'\r', 'D', 'R', 'J', 'N', 'F', 'C', 'K',
	'T', 'Z', 'L', 'W', 'H', 'Y', 'P', 'Q',
	'O', 'B', 'G', ' ', 'M', 'X', 'V', ' '};

static const char RTTY_Symbols[32] = {
	'\0', '3', '\n', '-', ' ', '\a', '8', '7',
	'\r', '$', '4', '\'', ',', '!', ':', '(',
	'5', '"', ')', '2', '#', '6', '0', '1',
	'9', '?', '&', ' ', '.', '/', ';', ' '};

static int RTTYDecoder_waitForStartBit(float32_t sample);
static int RTTYDecoder_getBitDPLL(float32_t sample, int *val_p);
static int RTTYDecoder_demodulator(float32_t sample);
static float32_t RTTYDecoder_decayavg(float32_t average, float32_t input, int weight);

// FSK shift: 170 200 425 850
// FSK tone freq 1275 2125

static float RTTY_Speed = 50; //45.45;
#define	RTTY_Shift 455 //170;
// The standard mark and space tones are 2125 hz and 2295 hz respectively
#define RTTY_FreqMark DEFAULT_RTTY_PITCH		// /* mark тон DIGI modes - 2.125 кГц (1275 2125) */
#define	RTTY_FreqSpace (DEFAULT_RTTY_PITCH + RTTY_Shift)
static int RTTY_StopBits = RTTY_STOP_1;

void RTTYDecoder_Init(void)
{
	iir_filter_t f0;
	//speed
	RTTY_oneBitSampleCount = (uint16_t)roundf((float32_t)TRX_SAMPLERATE / RTTY_Speed);

	//RTTY LPF Filter
	biquad_create(& f0, RTTY_LPF_STAGES);
	biquad_init_lowpass(& f0, TRX_SAMPLERATE, RTTY_Speed * 2);
	fill_biquad_coeffs(& f0, RTTY_LPF_Filter_Coeffs);
	arm_biquad_cascade_df2T_init_f32(&RTTY_LPF_Filter, RTTY_LPF_STAGES, RTTY_LPF_Filter_Coeffs, RTTY_LPF_Filter_State);

	//RTTY mark filter
	biquad_create(& f0, RTTY_BPF_STAGES);
	biquad_init_bandpass(& f0, TRX_SAMPLERATE, RTTY_FreqMark - RTTY_BPF_WIDTH / 2, RTTY_FreqMark + RTTY_BPF_WIDTH / 2);
	fill_biquad_coeffs(& f0, RTTY_Mark_Filter_Coeffs);
	arm_biquad_cascade_df2T_init_f32(&RTTY_Mark_Filter, RTTY_BPF_STAGES, RTTY_Mark_Filter_Coeffs, RTTY_Mark_Filter_State);

	//RTTY space filter
	biquad_create(& f0, RTTY_BPF_STAGES);
	biquad_init_bandpass(& f0, TRX_SAMPLERATE, RTTY_FreqSpace - RTTY_BPF_WIDTH / 2, RTTY_FreqSpace + RTTY_BPF_WIDTH / 2);
	fill_biquad_coeffs(& f0, RTTY_Space_Filter_Coeffs);
	arm_biquad_cascade_df2T_init_f32(&RTTY_Space_Filter, RTTY_BPF_STAGES, RTTY_Space_Filter_Coeffs, RTTY_Space_Filter_State);

	//text
//	sprintf(RTTY_Decoder_Text, " RTTY: -");
//	addSymbols(RTTY_Decoder_Text, RTTY_Decoder_Text, RTTY_DECODER_STRLEN, " ", 1);
//	LCD_UpdateQuery.TextBar = 1;
}

void RTTYDecoder_Process(const float32_t *bufferIn, unsigned len)
{
	for (uint32_t buf_pos = 0; buf_pos < len; buf_pos++)
	{
		switch (RTTY_State)
		{
		case RTTY_STATE_WAIT_START: // not synchronized, need to wait for start bit
			if (RTTYDecoder_waitForStartBit(bufferIn[buf_pos]))
			{
				RTTY_State = RTTY_STATE_BIT;
				RTTY_byteResult_bnum = 1;
				RTTY_byteResult = 0;
			}
			break;
		case RTTY_STATE_BIT:
			// reading 7 more bits
			if (RTTY_byteResult_bnum < 8)
			{
				int bitResult = 0;
				if (RTTYDecoder_getBitDPLL(bufferIn[buf_pos], &bitResult))
				{
					switch (RTTY_byteResult_bnum)
					{
					case 6: // stop bit 1
					case 7: // stop bit 2
						if (bitResult == 0)
						{
							// not in sync
							RTTY_State = RTTY_STATE_WAIT_START;
						}
						if (RTTY_StopBits != RTTY_STOP_2 && RTTY_byteResult_bnum == 6)
						{
							// we pretend to be at the 7th bit after receiving the first stop bit if we have less than 2 stop bits
							// this omits check for 1.5 bit condition but we should be more or less safe here, may cause
							// a little more unaligned receive but without that shortcut we simply cannot receive these configurations
							// so it is worth it
							RTTY_byteResult_bnum = 7;
						}
						break;
					default:
						RTTY_byteResult |= (bitResult ? 1 : 0) << (RTTY_byteResult_bnum - 1);
					}
					RTTY_byteResult_bnum++;
				}
			}
			if (RTTY_byteResult_bnum == 8 && RTTY_State == RTTY_STATE_BIT)
			{
				char charResult;

				switch (RTTY_byteResult)
				{
				case RTTY_LETTER_CODE:
					RTTY_charSetMode = RTTY_MODE_LETTERS;
					// println(" ^L^");
					break;
				case RTTY_SYMBOL_CODE:
					RTTY_charSetMode = RTTY_MODE_SYMBOLS;
					// println(" ^F^");
					break;
				default:
					switch (RTTY_charSetMode)
					{
					case RTTY_MODE_SYMBOLS:
						charResult = RTTY_Symbols[RTTY_byteResult];
						break;
					case RTTY_MODE_LETTERS:
					default:
						charResult = RTTY_Letters[RTTY_byteResult];
						break;
					}
					//RESULT !!!!
					//print(charResult);
					PRINTF("%c", charResult);
//					char str[2] = {0};
//					str[0] = charResult;
//					if (strlen(RTTY_Decoder_Text) >= RTTY_DECODER_STRLEN)
//						shiftTextLeft(RTTY_Decoder_Text, 1);
//					strcat(RTTY_Decoder_Text, str);
//					LCD_UpdateQuery.TextBar = 1;
					break;
				}
				RTTY_State = RTTY_STATE_WAIT_START;
			}
		}
	}
}

// this function returns only 1 when the start bit is successfully received
static int RTTYDecoder_waitForStartBit(float32_t sample)
{
	int retval = 0;
	int bitResult;
	static int16_t wait_for_start_state = 0;
	static int16_t wait_for_half = 0;

	bitResult = RTTYDecoder_demodulator(sample);

	switch (wait_for_start_state)
	{
	case 0:
		// waiting for a falling edge
		if (bitResult != 0)
		{
			wait_for_start_state++;
		}
		break;
	case 1:
		if (bitResult != 1)
		{
			wait_for_start_state++;
		}
		break;
	case 2:
		wait_for_half = RTTY_oneBitSampleCount / 2;
		wait_for_start_state++;
		/* no break */
	case 3:
		wait_for_half--;
		if (wait_for_half == 0)
		{
			retval = (bitResult == 0);
			wait_for_start_state = 0;
		}
		break;
	}
	return retval;
}

// this function returns 1 once at the half of a bit with the bit's value
static int RTTYDecoder_getBitDPLL(float32_t sample, int *val_p)
{
	static int phaseChanged = 0;
	int retval = 0;

	if (RTTY_DPLLBitPhase < RTTY_oneBitSampleCount)
	{
		*val_p = RTTYDecoder_demodulator(sample);

		if (!phaseChanged && *val_p != RTTY_DPLLOldVal)
		{
			if (RTTY_DPLLBitPhase < RTTY_oneBitSampleCount / 2)
			{
				RTTY_DPLLBitPhase += RTTY_oneBitSampleCount / 32; // early
			}
			else
			{
				RTTY_DPLLBitPhase -= RTTY_oneBitSampleCount / 32; // late
			}
			phaseChanged = 1;
		}
		RTTY_DPLLOldVal = *val_p;
		RTTY_DPLLBitPhase++;
	}

	if (RTTY_DPLLBitPhase >= RTTY_oneBitSampleCount)
	{
		RTTY_DPLLBitPhase -= RTTY_oneBitSampleCount;
		retval = 1;
	}

	return retval;
}

// adapted from https://github.com/ukhas/dl-fldigi/blob/master/src/include/misc.h
static float32_t RTTYDecoder_decayavg(float32_t average, float32_t input, int weight)
{
	float32_t retval;
	if (weight <= 1)
	{
		retval = input;
	}
	else
	{
		retval = ((input - average) / (float32_t)weight) + average;
	}
	return retval;
}

// this function returns the bit value of the current sample
static int RTTYDecoder_demodulator(float32_t sample)
{
	float32_t space_mag = 0;
	float32_t mark_mag = 0;
	arm_biquad_cascade_df2T_f32(&RTTY_Space_Filter, &sample, &space_mag, 1);
	arm_biquad_cascade_df2T_f32(&RTTY_Mark_Filter, &sample, &mark_mag, 1);

	float32_t v1 = 0.0;
	// calculating the RMS of the two lines (squaring them)
	space_mag *= space_mag;
	mark_mag *= mark_mag;

	// RTTY decoding with ATC = automatic threshold correction
	float32_t helper = space_mag;
	space_mag = mark_mag;
	mark_mag = helper;
	static float32_t mark_env = 0.0;
	static float32_t space_env = 0.0;
	static float32_t mark_noise = 0.0;
	static float32_t space_noise = 0.0;
	// experiment to implement an ATC (Automatic threshold correction), DD4WH, 2017_08_24
	// everything taken from FlDigi, licensed by GNU GPLv2 or later
	// https://github.com/ukhas/dl-fldigi/blob/master/src/cw_rtty/rtty.cxx
	// calculate envelope of the mark and space signals
	// uses fast attack and slow decay
	mark_env = RTTYDecoder_decayavg(mark_env, mark_mag, (mark_mag > mark_env) ? RTTY_oneBitSampleCount / 4 : RTTY_oneBitSampleCount * 16);
	space_env = RTTYDecoder_decayavg(space_env, space_mag, (space_mag > space_env) ? RTTY_oneBitSampleCount / 4 : RTTY_oneBitSampleCount * 16);
	// calculate the noise on the mark and space signals
	mark_noise = RTTYDecoder_decayavg(mark_noise, mark_mag, (mark_mag < mark_noise) ? RTTY_oneBitSampleCount / 4 : RTTY_oneBitSampleCount * 48);
	space_noise = RTTYDecoder_decayavg(space_noise, space_mag, (space_mag < space_noise) ? RTTY_oneBitSampleCount / 4 : RTTY_oneBitSampleCount * 48);
	// the noise floor is the lower signal of space and mark noise
	float32_t noise_floor = (space_noise < mark_noise) ? space_noise : mark_noise;

	// Linear ATC, section 3 of www.w7ay.net/site/Technical/ATC
	// v1 = space_mag - mark_mag - 0.5 * (space_env - mark_env);

	// Compensating for the noise floor by using clipping
	float32_t mclipped = 0.0, sclipped = 0.0;
	mclipped = mark_mag > mark_env ? mark_env : mark_mag;
	sclipped = space_mag > space_env ? space_env : space_mag;
	if (mclipped < noise_floor)
	{
		mclipped = noise_floor;
	}
	if (sclipped < noise_floor)
	{
		sclipped = noise_floor;
	}

	// Optimal ATC (Section 6 of of www.w7ay.net/site/Technical/ATC)
	v1 = (mclipped - noise_floor) * (mark_env - noise_floor) - (sclipped - noise_floor) * (space_env - noise_floor) - 0.25 * ((mark_env - noise_floor) * (mark_env - noise_floor) - (space_env - noise_floor) * (space_env - noise_floor));
	arm_biquad_cascade_df2T_f32(&RTTY_LPF_Filter, &v1, &v1, 1);

	// RTTY without ATC, which works very well too!
	// inverting line 1
	/*mark_mag *= -1;

	// summing the two lines
	v1 = mark_mag + space_mag;

	// lowpass filtering the summed line
	arm_biquad_cascade_df2T_f32(&RTTY_LPF_Filter, &v1, &v1, 1);*/

	return (v1 > 0) ? 0 : 1;
}

#endif /* WITHRTTY */

#if WITHLMSAUTONOTCH

enum {
	AUTONOTCH_NUMTAPS = 64,
	AUTONOTCH_BUFFER_SIZE = FIRBUFSIZE * 4,
	AUTONOTCH_STATE_ARRAY_SIZE = AUTONOTCH_NUMTAPS + FIRBUFSIZE - 1,
};


#if (__ARM_FP & 0x08) || __riscv_d

/* заглушки для проверки компиляции в режиме обработки float_64 по умолчанию.
 *
 */

/**
 * @brief Instance structure for the floating-point normalized LMS filter.
 */
typedef struct
{
        uint16_t numTaps;     /**< number of coefficients in the filter. */
        float64_t *pState;    /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
        float64_t *pCoeffs;   /**< points to the coefficient array. The array is of length numTaps. */
        float64_t mu;         /**< step size that control filter coefficient updates. */
        float64_t energy;     /**< saves previous frame energy. */
        float64_t x0;         /**< saves previous input sample. */
} arm_lms_norm_instance_f64;


/**
 * @brief Instance structure for the floating-point LMS filter.
 */
typedef struct
{
        uint16_t numTaps;    /**< number of coefficients in the filter. */
        float64_t *pState;   /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
        float64_t *pCoeffs;  /**< points to the coefficient array. The array is of length numTaps. */
        float64_t mu;        /**< step size that controls filter coefficient updates. */
} arm_lms_instance_f64;

void arm_lms_norm_init_f64(
        arm_lms_norm_instance_f64 * S,
        uint16_t numTaps,
		float64_t * pCoeffs,
		float64_t * pState,
		float64_t mu,
        uint32_t blockSize)
{
	  /* Assign filter taps */
	  S->numTaps = numTaps;

	  /* Assign coefficient pointer */
	  S->pCoeffs = pCoeffs;

	  /* Clear state buffer and size is always blockSize + numTaps - 1 */
	  memset(pState, 0, (numTaps + (blockSize - 1U)) * sizeof(float64_t));

	  /* Assign state pointer */
	  S->pState = pState;

	  /* Assign Step size value */
	  S->mu = mu;

	  /* Initialise Energy to zero */
	  S->energy = 0;

	  /* Initialise x0 to zero */
	  S->x0 = 0;

}

void arm_lms_norm_f64(
	arm_lms_norm_instance_f64 * S,
	const float64_t * pSrc,
	float64_t * pRef,
	float64_t * pOut,
	float64_t * pErr,
	uint32_t blockSize)
 {
	float64_t *pState = S->pState; /* State pointer */
	float64_t *pCoeffs = S->pCoeffs; /* Coefficient pointer */
	float64_t *pStateCurnt; /* Points to the current sample of the state */
	float64_t *px, *pb; /* Temporary pointers for state and coefficient buffers */
	float64_t mu = S->mu; /* Adaptive factor */
	float64_t acc, e; /* Accumulator, error */
	float64_t w; /* Weight factor */
	uint32_t numTaps = S->numTaps; /* Number of filter coefficients in the filter */
	uint32_t tapCnt, blkCnt; /* Loop counters */
	float64_t energy; /* Energy of the input */
	float64_t x0, in; /* Temporary variable to hold input sample and state */

	/* Initializations of error,  difference, Coefficient update */
	e = 0.0f;
	w = 0.0f;

	energy = S->energy;
	x0 = S->x0;

	/* S->pState points to buffer which contains previous frame (numTaps - 1) samples */
	/* pStateCurnt points to the location where the new input data should be written */
	pStateCurnt = &(S->pState[(numTaps - 1U)]);

	/* initialise loop count */
	blkCnt = blockSize;

	while (blkCnt > 0U) {
		/* Copy the new input sample into the state buffer */
		*pStateCurnt++ = *pSrc;

		/* Initialize pState pointer */
		px = pState;

		/* Initialize coefficient pointer */
		pb = pCoeffs;

		/* Read the sample from input buffer */
		in = *pSrc++;

		/* Update the energy calculation */
		energy -= x0 * x0;
		energy += in * in;

		/* Set the accumulator to zero */
		acc = 0.0f;

#if defined (ARM_MATH_LOOPUNROLL)

		/* Loop unrolling: Compute 4 taps at a time. */
		tapCnt = numTaps >> 2U;

		while (tapCnt > 0U) {
			/* Perform the multiply-accumulate */
			acc += (*px++) * (*pb++);

			acc += (*px++) * (*pb++);

			acc += (*px++) * (*pb++);

			acc += (*px++) * (*pb++);

			/* Decrement loop counter */
			tapCnt--;
		}

		/* Loop unrolling: Compute remaining taps */
		tapCnt = numTaps % 0x4U;

#else

/* Initialize tapCnt with number of samples */
tapCnt = numTaps;

#endif /* #if defined (ARM_MATH_LOOPUNROLL) */

		while (tapCnt > 0U) {
			/* Perform the multiply-accumulate */
			acc += (*px++) * (*pb++);

			/* Decrement the loop counter */
			tapCnt--;
		}

		/* Store the result from accumulator into the destination buffer. */
		*pOut++ = acc;

		/* Compute and store error */
		e = (float64_t) *pRef++ - acc;
		*pErr++ = e;

		/* Calculation of Weighting factor for updating filter coefficients */
		/* epsilon value 0.000000119209289f */
		w = (e * mu) / (energy + 0.000000119209289);

		/* Initialize pState pointer */
		px = pState;

		/* Initialize coefficient pointer */
		pb = pCoeffs;

#if defined (ARM_MATH_LOOPUNROLL)

		/* Loop unrolling: Compute 4 taps at a time. */
		tapCnt = numTaps >> 2U;

		/* Update filter coefficients */
		while (tapCnt > 0U) {
			/* Perform the multiply-accumulate */
			*pb += w * (*px++);
			pb++;

			*pb += w * (*px++);
			pb++;

			*pb += w * (*px++);
			pb++;

			*pb += w * (*px++);
			pb++;

			/* Decrement loop counter */
			tapCnt--;
		}

		/* Loop unrolling: Compute remaining taps */
		tapCnt = numTaps % 0x4U;

#else

/* Initialize tapCnt with number of samples */
tapCnt = numTaps;

#endif /* #if defined (ARM_MATH_LOOPUNROLL) */

		while (tapCnt > 0U) {
			/* Perform the multiply-accumulate */
			*pb += w * (*px++);
			pb++;

			/* Decrement loop counter */
			tapCnt--;
		}

		x0 = *pState;

		/* Advance state pointer by 1 for the next sample */
		pState = pState + 1;

		/* Decrement loop counter */
		blkCnt--;
	}

	/* Save energy and x0 values for the next frame */
	S->energy = energy;
	S->x0 = x0;

	/* Processing is complete.
	 Now copy the last numTaps - 1 samples to the start of the state buffer.
	 This prepares the state buffer for the next function call. */

	/* Points to the start of the pState buffer */
	pStateCurnt = S->pState;

	/* copy data */
#if defined (ARM_MATH_LOOPUNROLL)

	/* Loop unrolling: Compute 4 taps at a time. */
	tapCnt = (numTaps - 1U) >> 2U;

	while (tapCnt > 0U) {
		*pStateCurnt++ = *pState++;
		*pStateCurnt++ = *pState++;
		*pStateCurnt++ = *pState++;
		*pStateCurnt++ = *pState++;

		/* Decrement loop counter */
		tapCnt--;
	}

	/* Loop unrolling: Compute remaining taps */
	tapCnt = (numTaps - 1U) % 0x4U;

#else

/* Initialize tapCnt with number of samples */
tapCnt = (numTaps - 1U);

#endif /* #if defined (ARM_MATH_LOOPUNROLL) */

	while (tapCnt > 0U) {
		*pStateCurnt++ = *pState++;

		/* Decrement loop counter */
		tapCnt--;
	}

}

#endif

typedef struct
{
	ARM_MORPH(arm_lms_norm_instance)	lms2Norm_instance;
	ARM_MORPH(arm_lms_instance)	    lms2_instance;
    FLOAT_t	                lms2StateF32 [AUTONOTCH_STATE_ARRAY_SIZE];
    FLOAT_t	                norm [AUTONOTCH_NUMTAPS];
    FLOAT_t	                ref [AUTONOTCH_BUFFER_SIZE];
    unsigned 					refold;
    unsigned 					refnew;
    FLOAT_t phonefence;
} LMSData_t;

static void hamradio_autonotch_init(LMSData_t * const lmsd)
{
	lmsd->phonefence = 1;
	const FLOAT_t mu = LOG10F(((5 + 1.0f) / 1500.0f) + 1.0f);
	//const float32_t mu = 0.0001f;		// UA3REO value
	ARM_MORPH(arm_lms_norm_init)(& lmsd->lms2Norm_instance, AUTONOTCH_NUMTAPS, lmsd->norm, lmsd->lms2StateF32, mu, FIRBUFSIZE);
	ARM_MORPH(arm_fill)(0, lmsd->ref, AUTONOTCH_BUFFER_SIZE);
	ARM_MORPH(arm_fill)(0, lmsd->norm, AUTONOTCH_NUMTAPS);
	lmsd->refold = 0;
	lmsd->refnew = FIRBUFSIZE;
}

//static volatile int nrestarts;

// pInput - входной буфер FIRBUFSIZE сэмплов
// pOutput - обработаный буфер FIRBUFSIZE сэмплов
static void hamradio_autonotch_process(LMSData_t * const lmsd, FLOAT_t * pInput, FLOAT_t * pOutput)
{
    static FLOAT_t errsig2 [FIRBUFSIZE];	/* unused output */
//	float32_t diag;
//	float32_t diag2;
//
//	arm_mean_f32(lmsd->ref, AUTONOTCH_BUFFER_SIZE, & diag);
//	arm_mean_f32(lmsd->norm, AUTONOTCH_NUMTAPS, & diag2);
//	if (__isnanf(diag) || __isinff(diag) || __isnanf(diag2) || __isinff(diag2))
//	{
//		ARM_MORPH(arm_fill)(0, lmsd->ref, AUTONOTCH_BUFFER_SIZE);
//		ARM_MORPH(arm_fill)(0, lmsd->norm, AUTONOTCH_NUMTAPS);
//		lmsd->refold = 0;
//		lmsd->refnew = FIRBUFSIZE;
//		++ nrestarts;
//	}
    ARM_MORPH(arm_copy)(pInput, & lmsd->ref [lmsd->refnew], FIRBUFSIZE);
    ARM_MORPH(arm_lms_norm)(& lmsd->lms2Norm_instance, pInput, & lmsd->ref [lmsd->refold], errsig2, pOutput, FIRBUFSIZE);
	lmsd->refold += FIRBUFSIZE;
	lmsd->refnew = lmsd->refold + FIRBUFSIZE;
	lmsd->refold %= AUTONOTCH_BUFFER_SIZE;
	lmsd->refnew %= AUTONOTCH_BUFFER_SIZE;

	FLOAT_t diagmin;
	FLOAT_t diagmax;
	uint32_t index;
	ARM_MORPH(arm_min)(pOutput, FIRBUFSIZE, & diagmin, & index);
	ARM_MORPH(arm_max_no_idx)(pOutput, FIRBUFSIZE, & diagmax);
	if (diagmin < - lmsd->phonefence || diagmax > lmsd->phonefence)
	{
		ARM_MORPH(arm_fill)(0, lmsd->ref, AUTONOTCH_BUFFER_SIZE);
		ARM_MORPH(arm_fill)(0, lmsd->norm, AUTONOTCH_NUMTAPS);
		ARM_MORPH(arm_fill)(0, pOutput, FIRBUFSIZE);
		lmsd->refold = 0;
		lmsd->refnew = FIRBUFSIZE;
		//++ nrestarts;
	}
}

#endif /* WITHLMSAUTONOTCH */

#if 1//! WITHSKIPUSERMODE

#if WITHNOSPEEX

	#define NOISE_REDUCTION_BLOCK_SIZE FIRBUFSIZE
	#define NOISE_REDUCTION_TAPS 64
	#define NOISE_REDUCTION_REFERENCE_SIZE (NOISE_REDUCTION_BLOCK_SIZE * 2)
	#define NOISE_REDUCTION_STEP 0.01f

#endif /* WITHNOSPEEX */

typedef struct rxaproc_tag
{
	// FIR audio filter
	FLOAT_t firEQcoeff [Ntap_rx_AUDIO];
	ARM_MORPH(arm_fir_instance) fir_instance;
	FLOAT_t fir_state [FIRBUFSIZE + Ntap_rx_AUDIO - 1];
	FLOAT_t wire1 [FIRBUFSIZE];

#if WITHNOSPEEX
	// NLMS NR
	arm_lms_norm_instance_f32 lms2_Norm_instance;
	float32_t lms2_stateF32 [NOISE_REDUCTION_TAPS + NOISE_REDUCTION_BLOCK_SIZE - 1];
	float32_t lms2_normCoeff_f32 [NOISE_REDUCTION_TAPS];
	float32_t ref [NOISE_REDUCTION_REFERENCE_SIZE];
	float32_t lms2_errsig2 [NOISE_REDUCTION_BLOCK_SIZE];
	uint_fast16_t refold;
	uint_fast16_t refnew;

#else /* WITHNOSPEEX */

//	#include "src/speex/arch.h"
//	#include "src/speex/speex_preprocess.h"

	SpeexPreprocessState * st_handle;

#endif /* WITHNOSPEEX */

#if WITHLMSAUTONOTCH
	// LMS auto notch
	LMSData_t lmsanotch;
#endif /* WITHLMSAUTONOTCH */
} rxaproc_t;

/* Не требует работы с DMA, но размер приличный... */
static RAM_D1 rxaproc_t rxaprocs [NTRX];

#endif /* ! WITHSKIPUSERMODE */

/* на слабых процессорах второй приемник без NR и автонотч */
static uint_fast8_t ispathprocessing(uint_fast8_t pathi)
{
#if CPUSTYLE_STM32MP1 || CPUSTYLE_XC7Z || CPUSTYLE_ALLWINNER || CPUSTYLE_RK356X
	return 1;
#else /* CPUSTYLE_STM32MP1 || CPUSTYLE_XC7Z || CPUSTYLE_ALLWINNER */
	return pathi == 0;
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_XC7Z || CPUSTYLE_ALLWINNER */
}

static void speex_update_rx(void)
{
	uint_fast8_t pathi;

	for (pathi = 0; pathi < NTRX; ++ pathi)
	{
		const uint_fast8_t amode = getamode(pathi);
#if ! WITHNOSPEEX
		spx_int32_t denoise = ispathprocessing(pathi) && gnoisereducts [amode];
		spx_int32_t supress = - (int) gnoisereductvl;
#endif /* ! WITHNOSPEEX */
		rxaproc_t * const nrp = & rxaprocs [pathi];

#if WITHNOSPEEX
#else /* WITHNOSPEEX */
		SpeexPreprocessState * const st = nrp->st_handle;
		ASSERT(st != NULL);

		//PRINTF("speex_update_rx: amode=%d, pathi=%d, denoise=%d, supress=%d\n", (int) amode, (int) pathi, (int) denoise, (int) supress);
		speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DENOISE, & denoise);
		speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, & supress);

#endif /* WITHNOSPEEX */
	}
}

void filters_update_rx(uint_fast8_t pathi)
{
	const uint_fast8_t amode = getamode(pathi);
	rxaproc_t * const nrp = & rxaprocs [pathi];

	// Получение параметров эквалайзера
	FLOAT_t * const dCoefs = nrp->firEQcoeff;
	dsp_recalceq_coeffs_rx_AUDIO(pathi, dCoefs, Ntap_rx_AUDIO);	// calculate coefficients
}

static void InitNoiseReduction(void)
{
	uint_fast8_t pathi;
	for (pathi = 0; pathi < NTRX; ++ pathi)
	{
		rxaproc_t * const nrp = & rxaprocs [pathi];

		ARM_MORPH(arm_fir_init)(& nrp->fir_instance, Ntap_rx_AUDIO, nrp->firEQcoeff, nrp->fir_state, FIRBUFSIZE);

#if WITHNOSPEEX

		arm_lms_norm_init_f32(& nrp->lms2_Norm_instance, NOISE_REDUCTION_TAPS, nrp->lms2_normCoeff_f32, nrp->lms2_stateF32, NOISE_REDUCTION_STEP, NOISE_REDUCTION_BLOCK_SIZE);
		ARM_MORPH(arm_fill)(0, nrp->ref, NOISE_REDUCTION_REFERENCE_SIZE);
		ARM_MORPH(arm_fill)(0, nrp->lms2_normCoeff_f32, NOISE_REDUCTION_TAPS);

		nrp->refold = 0;
		nrp->refnew = 0;
#else /* WITHNOSPEEX */

		nrp->st_handle = speex_preprocess_state_init(FIRBUFSIZE, ARMI2SRATE);
		//PRINTF("InitNoiseReduction: pathi=%d\n", (int) pathi);

#endif /* WITHNOSPEEX */

#if WITHLMSAUTONOTCH
		hamradio_autonotch_init(& nrp->lmsanotch);
#endif /* WITHLMSAUTONOTCH */
	}

#ifdef WITHLEAKYLMSANR
    AudioDriver_LeakyLmsNr_Init(); // RX
#endif /* WITHLEAKYLMSANR */

}

#if WITHNOSPEEX

static void processNoiseReduction(rxaproc_t * nrp, const float* bufferIn, float* bufferOut)
{
	arm_copy_f32(bufferIn, & nrp->ref [nrp->refnew], NOISE_REDUCTION_BLOCK_SIZE);
	arm_lms_norm_f32(& nrp->lms2_Norm_instance, bufferIn, & nrp->ref [nrp->refold], bufferOut, nrp->lms2_errsig2, NOISE_REDUCTION_BLOCK_SIZE);

	nrp->refold += NOISE_REDUCTION_BLOCK_SIZE;
	if (nrp->refold >= NOISE_REDUCTION_REFERENCE_SIZE)
		nrp->refold = 0;
	nrp->refnew = nrp->refold + NOISE_REDUCTION_BLOCK_SIZE;
	if (nrp->refnew >= NOISE_REDUCTION_REFERENCE_SIZE)
		nrp->refnew = 0;
}

#endif /* WITHNOSPEEX */

// user-mode processing
// На выходе входной сигнал без изменений
static FLOAT_t * afpnoproc(uint_fast8_t pathi, rxaproc_t * const nrp, FLOAT_t * p)
{
	// FIXME: speex внутри использует целочисленные вычисления
//	static const float32_t ki = 32768;
//	static const float32_t ko = 1. / 32768;
#if WITHNOSPEEX
	// не делать даже коррекцию АЧХ
	return p;
#else /* WITHNOSPEEX */
	// не делать даже коррекцию АЧХ
	#if ! WITHLEAKYLMSANR
//		arm_scale_f32(p, ki, p, FIRBUFSIZE);
//		speex_preprocess_estimate_update(nrp->st_handle, p);
//		arm_scale_f32(p, ko, p, FIRBUFSIZE);
	#endif /* ! WITHLEAKYLMSANR */
	return p;
#endif /* WITHNOSPEEX */
}

static FLOAT_t * afpcw(uint_fast8_t pathi, rxaproc_t * const nrp, FLOAT_t * p)
{
	const uint_fast8_t amode = getamode(pathi);
	const uint_fast8_t denoise = ispathprocessing(pathi) && gnoisereducts [amode];
	const uint_fast8_t anotch = ispathprocessing(pathi) && gnotch && notchmodes [gnotchtype].code == BOARD_NOTCH_AUTO;
	//////////////////////////////////////////////
	// Filtering
	// Use CMSIS DSP interface
#if WITHUSBHEADSET
	return p;

#elif WITHNOSPEEX
	if (denoise)
	{
		// Filtering and denoise.
		ARM_MORPH(arm_fir)(& nrp->fir_instance, p, nrp->wire1, FIRBUFSIZE);
		processNoiseReduction(nrp, nrp->wire1, p);	// result copy back
		return p;
	}
	else
	{
		// Filtering only.
		ARM_MORPH(arm_fir)(& nrp->fir_instance, p, nrp->wire1, FIRBUFSIZE);
		return nrp->wire1;
	}
#else /* WITHNOSPEEX */

	// Filtering and denoise.
	BEGIN_STAMP();
	ARM_MORPH(arm_fir)(& nrp->fir_instance, p, nrp->wire1, FIRBUFSIZE);		/* фильтр выхода детектора */
	//ARM_MORPH(arm_copy)(p, nrp->wire1, FIRBUFSIZE);
	END_STAMP();
	if (anotch)
	{
		hamradio_autonotch_process(& nrp->lmsanotch, nrp->wire1, nrp->wire1);
	}
	else
	{
		hamradio_autonotch_process(& nrp->lmsanotch, nrp->wire1, p);	// результат не используем
	}


#if WITHLEAKYLMSANR
	if (pathi == 0)
		AudioDriver_LeakyLmsNr(nrp->wire1, nrp->wire1, FIRBUFSIZE, 0);
#else /* WITHLEAKYLMSANR */
	if (ispathprocessing(pathi))
	{
		//ARM_MORPH(arm_fill)(0, nrp->wire1, FIRBUFSIZE);
		// FIXME: speex внутри использует целочисленные вычисления
		static const FLOAT_t ki = 32768;
		static const FLOAT_t ko = 1. / 32768;
		ARM_MORPH(arm_scale)(nrp->wire1, ki, nrp->wire1, FIRBUFSIZE);
		speex_preprocess_run(nrp->st_handle, nrp->wire1);
		ARM_MORPH(arm_scale)(nrp->wire1, ko, nrp->wire1, FIRBUFSIZE);
		//ARM_MORPH(arm_fill)(0, nrp->wire1, FIRBUFSIZE);
	}
#endif /* WITHLEAKYLMSANR */
	return nrp->wire1;

#endif /* WITHNOSPEEX */

}


// user-mode processing
// На выходе формируется тишина
// прием телетайпа в приемнике A
static FLOAT_t * afprtty(uint_fast8_t pathi, rxaproc_t * const nrp, FLOAT_t * p)
{
#if WITHRTTY
	if (pathi == 0)
	{
		RTTYDecoder_Process(p, FIRBUFSIZE);
	}
#endif /* WITHRTTY */
	//nrp->outsp = p;
	//ARM_MORPH(arm_fill)(0, p, FIRBUFSIZE);
	return afpnoproc(pathi, nrp, p);
}


// user-mode processing - NR, эквалайзер приёмника
void
user_audioproc(void * ctx)
{
	(void) ctx;
#if LINUX_SUBSYSTEM
	linux_wait_iq();
#endif /* LINUX_SUBSYSTEM */
#if ! WITHSKIPUSERMODE
	speexel_t * p;
	while (takespeexready(& p))
	{
		// обработка и сохранение в savesampleout16stereo_user()
		unsigned score;
		for (score = 0; score < FIRBUFSIZE; )
		{
			const unsigned len = 256;
			const unsigned rest = (FIRBUFSIZE - score);
			const unsigned chunk = rest >= len ? len : rest;
	#if WITHUSEDUALWATCH
			deliveryfloat_buffer(& speexinfloat, p + FIRBUFSIZE + score, p + score, chunk);	// to AUDIO codec
	#else /* WITHUSEDUALWATCH */
			deliveryfloat_buffer(& speexinfloat, p + score, p + score, chunk);	// to AUDIO codec
	#endif /* WITHUSEDUALWATCH */
			score += chunk;
		}
		uint_fast8_t pathi;
		FLOAT_t * outsp [NTRX];
		for (pathi = 0; pathi < NTRX; ++ pathi)
		{
			rxaproc_t * const nrp = & rxaprocs [pathi];
			//const uint_fast8_t amode = getamode(pathi);
			// nrp->outsp указывает на результат обработки
			//outsp [pathi] = mdt [amode].afproc(pathi, nrp, p + pathi * FIRBUFSIZE);
			outsp [pathi] = afpcw(pathi, nrp, p + pathi * FIRBUFSIZE);
		#if WITHAFEQUALIZER
			audio_rx_equalizer(outsp [pathi], FIRBUFSIZE);
		#endif /* WITHAFEQUALIZER */
		}
		//////////////////////////////////////////////
		// Save results
		//unsigned score;
		for (score = 0; score < FIRBUFSIZE; )
		{
			const unsigned len = 256;
			const unsigned rest = (FIRBUFSIZE - score);
			const unsigned chunk = rest >= len ? len : rest;
	#if WITHUSEDUALWATCH
			deliveryfloat_buffer(& speexoutfloat, outsp [0] + score, outsp [1] + score, chunk);	// to AUDIO codec
	#else /* WITHUSEDUALWATCH */
			deliveryfloat_buffer(& speexoutfloat, outsp [0] + score, outsp [0] + score, chunk);	// to AUDIO codec
	#endif /* WITHUSEDUALWATCH */
			score += chunk;
		}
		// Освобождаем буфер
		releasespeexbuffer(p);
	}
#endif /* ! WITHSKIPUSERMODE */
}

#else /* WITHINTEGRATEDDSP */
//
//FLOAT_t local_pow(FLOAT_t x, FLOAT_t y)
//{
//	return 0;
//}
//
//
//FLOAT_t local_exp(FLOAT_t x)
//{
//	return 0;
//}
//
//FLOAT_t local_log(FLOAT_t x)
//{
//	return 0;
//}

#endif /* WITHINTEGRATEDDSP */

static ticker_t displatchticker;

static void codec1_directupdate(void)
{
//	{
//		static int pass;
//		if (pass)
//			return;
//		pass = 1;
//	}
#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_NAU8822L)
// nau8822 experements

	// ALC
//			unsigned ALCEN = 0;	// only left channel ALC enabled
//			unsigned ALCMXGAIN = 7;	// Set maximum gain limit for PGA volume setting changes under ALC control
//			unsigned ALCMNGAIN = 0;	// Set minimum gain value limit for PGA volume setting changes under ALC control
	unsigned alcctl1 =
			((ALCEN ? 0x02 : 0x00) << 7) |	// only left channel ALC enabled
			(ALCMXGAIN << 3) |
			(ALCMNGAIN << 0) |
			0;
	nau8822_setreg(NAU8822_ALC_CONTROL_1, alcctl1);

	// Noise gate
//			unsigned ALCNEN = 0;	// ALC noise gate function control bit
//			unsigned ALCNTH = 0;	// ALC noise gate threshold level
	unsigned ngctl1 =
			(1 << 4) |	// reserved
			(ALCNEN << 3) |
			(ALCNTH << 0) |
			0;
	nau8822_setreg(NAU8822_NOISE_GATE, ngctl1);
#endif /* defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_NAU8822L) */
}

#if WITHDEBUG
// Печать частоты в формате dddddd.ddd
static void printfreq(int_fast32_t freq)
{
	 ldiv_t v = ldiv(freq, 1000);
	PRINTF(PSTR("%s%ld.%03ld"), (v.quot >= 0 && freq < 0) ? "-" : "", v.quot, freq < 0 ? - v.rem : v.rem);
}
#endif /* WITHDEBUG */

/* Получить частоту lo1 из частоты настройки */
static int_fast32_t synth_if1 [2];

uint_fast32_t synth_freq2lo1(
	uint_fast32_t freq,	/* частота на индикаторе устройства */
	uint_fast8_t pathi		/* номер тракта (main/sub) */
	)
{
	return (int_fast32_t) freq - synth_if1 [pathi];
}

/* Если изменяемый параметр отличается от старого значения - возврат 1 */
static uint_fast8_t
flagne_u8(uint_fast8_t * oldval, uint_fast8_t v)
{
	if (* oldval != v)
	{
		* oldval = v;
		return 1;
	}
	return 0;
}

/* Если изменяемый параметр отличается от старого значения - возврат 1 */
static uint_fast8_t
flagne_u16(uint_fast16_t * oldval, uint_fast16_t v)
{
	if (* oldval != v)
	{
		* oldval = v;
		return 1;
	}
	return 0;
}

/* Если изменяемый параметр отличается от старого значения - возврат 1 */
static uint_fast8_t
flagne_u32(uint_fast32_t * oldval, uint_fast32_t v)
{
	if (* oldval != v)
	{
		* oldval = v;
		return 1;
	}
	return 0;
}


#if WITHCAT

/* формирование запроса на информирование управляющего компьютера при изменении параметра. */
static uint_fast8_t
flagne_u8_cat(dualctl8_t * oldval, uint_fast8_t v, uint_fast8_t catindex)
{
	if (flagne_u8(& oldval->potvalue, v) != 0)
	{
		oldval->value = v;
		if (aistate != 0)
		{
			cat_answer_request(catindex);
		}
		return 1;
	}
	return 0;
}

/* формирование запроса на информирование управляющего компьютера при изменении параметра. */
static uint_fast8_t
flagne_u16_cat(dualctl16_t * oldval, uint_fast16_t v, uint_fast8_t catindex)
{
	if (flagne_u16(& oldval->potvalue, v) != 0)
	{
		oldval->value = v;
		if (aistate != 0)
		{
			cat_answer_request(catindex);
		}
		return 1;
	}
	return 0;
}

/* формирование запроса на информирование управляющего компьютера при изменении параметра. */
static uint_fast8_t
flagne_u32_cat(dualctl32_t * oldval, uint_fast32_t v, uint_fast8_t catindex)
{
	if (flagne_u32(& oldval->potvalue, v) != 0)
	{
		oldval->value = v;
		if (aistate != 0)
		{
			cat_answer_request(catindex);
		}
		return 1;
	}
	return 0;
}

#define FLAGNE_U8_CAT(a,b,c) flagne_u8_cat((a), (b), (c))
#define FLAGNE_U16_CAT(a,b,c) flagne_u16_cat((a), (b), (c))
#define FLAGNE_U32_CAT(a,b,c) flagne_u32_cat((a), (b), (c))

#define CATINDEX(v)	(v)

#else /* WITHCAT */

#define FLAGNE_U8_CAT(a,b,c) flagne_u8(& (a)->value, (b))
#define FLAGNE_U16_CAT(a,b,c) flagne_u16(& (a)->value, (b))
#define FLAGNE_U32_CAT(a,b,c) flagne_u32(& (a)->value, (b))

#define CATINDEX(v)	(0)

#endif /* WITHCAT */

/* Если изменяемый параметр отличается от старого значения - возврат 1 */
/* модификация параметра с учетом границ изменения значения */
static uint_fast8_t
encoder_flagne(const struct paramdefdef * pd, int_least16_t delta, uint_fast8_t catindex, void (* updcb)(void))
{
	if (delta != 0 && updcb != NULL)
		(* updcb)();

	if (param_rotate(pd, delta))
	{
#if WITHCAT
		if (aistate != 0)
		{
			cat_answer_request(catindex);
		}
#endif /* WITHCAT */
		return 1;
	}
	return 0;
}

/*
 параметры:
 tx - не-0: переключение аппаратуры в режим передачи
 Возвращает не-0, если выяснилось, что требуется "полное" обновления
 */
static uint_fast8_t
updateboard_noui(
	uint_fast8_t full
	)
{
	/* параметры, вычисляемые по updateboard(full=1) */
	static const struct modetempl * pamodetempl;	/* Режим, используемый при передаче */

	/* Параметры, которые могут измениться при перестройке частоты и вызвать необходимость full=1 */
	static uint_fast8_t lo0side = LOCODE_INVALID;
	static uint_fast32_t lo0hint = UINT32_MAX;
	static uint_fast8_t lo1side [2] = { LOCODE_INVALID, LOCODE_INVALID };
	const uint_fast8_t lo2side = LO2_SIDE;
	static uint_fast8_t lo2hint [2] = { UINT8_MAX, UINT8_MAX };
	const uint_fast8_t lo3side = LO3_SIDE;
	static uint_fast8_t forcelsb [2];
#if WITHDCDCFREQCTL
	static uint_fast32_t bldividerout = UINT32_MAX;
#endif /* WITHDCDCFREQCTL */
#if WITHMGLOOP
	static uint_fast16_t bandf1khint = UINT16_MAX;
#endif /* WITHMGLOOP */
#if CTLSTYLE_IGOR
	static uint_fast16_t bandf100khint = UINT16_MAX;
#else /* CTLSTYLE_IGOR */
	static uint_fast8_t bandfhint = UINT8_MAX;
	static uint_fast8_t bandf2hint = UINT8_MAX;
	static uint_fast8_t bandf3hint = UINT8_MAX;	// управление через разъем ACC
#endif /* CTLSTYLE_IGOR */
	static uint_fast8_t ant2hint = UINT8_MAX;
	static uint_fast8_t txreqhint = UINT8_MAX;
	uint_fast8_t full2 = full;

	full2 |= flagne_u8(& txreqhint, txreq_gethint(& txreqst0));
	full2 |= flagne_u8(& gtx, seq_get_txstate());

	uint_fast8_t pathi;
	ASSERT(gtx < 2);
	const uint_fast8_t pathn = gtx ? 1 : NTRX;
	/* +++ проверка необходимости полной перенастройки из-за сменившихся условий выбора частот. */
	for (pathi = 0; pathi < pathn; ++ pathi)
	{
		const uint_fast8_t bi = getbankindex_pathi(pathi);
		const int_fast32_t freq = gfreqs [bi];
		/* "Эти параметры для расчёта частот выясняем здесь, так как они (могут) зависить от частоты */
		full2 |= flagne_u8(& lo1side [pathi], getsidelo1(freq));	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
		full2 |= flagne_u8(& lo2hint [pathi], gethintlo2(freq));
		full2 |= flagne_u8(& forcelsb [pathi], getforcelsb(freq));
#if WITHDCDCFREQCTL
		full2 |= flagne_u32(& bldividerout, hardware_dcdc_calcdivider(freq));	// управление частотой dc-dc преобразователя
#endif /* WITHDCDCFREQCTL */
	}
	// параметры, не имеющие специфики для разных приемников
	{
		const uint_fast8_t bi = getbankindex_pathi(0); //getbankindex_tx(gtx);
#if WITHLFM
		const int_fast32_t freq = iflfmactive() ? getlfmfreq() : gfreqs [bi];
#else /* WITHLFM */
		const int_fast32_t freq = gfreqs [bi];
#endif /*  WITHLFM */

	#if WITHMGLOOP
		full2 |= flagne_u16(& bandf1khint, freq / 1000);
	#endif /* WITHMGLOOP */
	#if CTLSTYLE_IGOR
		full2 |= flagne_u16(& bandf100khint, freq / 100000);
	#else /* CTLSTYLE_IGOR */
		full2 |= flagne_u8(& bandfhint, bandf_calc(nyquistadj(freq)));
		full2 |= flagne_u8(& bandf2hint, bandf2_calc(nyquistadj2(freq)));
		full2 |= flagne_u8(& bandf3hint, bandf3_calc(nyquistadj3(freq)));
	#endif /* CTLSTYLE_IGOR */
		full2 |= flagne_u8(& lo0side, getsidelo0(freq));	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
		full2 |= flagne_u32(& lo0hint, gethintlo0(freq, lo0side));
		full2 |= flagne_u8(& ant2hint, geteffantenna(freq));
//		PRINTF("lo0hint %d lo0side %d\n", lo0hint, lo0side);
	}
	/* --- проверка необходимости полной перенастройки из-за сменившихся условий выбора частот. */

//	if (full2 != 0 && full == 0)
//	{
//		PRINTF("full2 added from %s/%d\n", file, line);
//	}
//	if (full != 0)
//	{
//		PRINTF("full passed from %s/%d\n", file, line);
//	}
#if WITHCAT
	if (aistate != 0)
	{
		if (full)
		{
			cat_answer_request(CAT_IF_INDEX);
			cat_answer_request(CAT_MD_INDEX);
			cat_answer_request(CAT_FA_INDEX);	// добавлено для обновления индикатора частоты в ACRP-590 при переходе по диапазонам клавишами на устройстве. И помогло при нажатиях на цифры дисплея.
			cat_answer_request(gtx ? CAT_TX_INDEX : CAT_RX_INDEX);	// ignore main/sub rx selection (0 - main. 1 - sub);
#if WITHIF4DSP
			cat_answer_request(CAT_NR_INDEX);
#endif /* WITHIF4DSP */
#if WITHTX && WITHAUTOTUNER
			cat_answer_request(CAT_AC_INDEX);
#endif /* WITHTX && WITHAUTOTUNER */
		}
		else
		{
			cat_answer_request(CAT_FA_INDEX);
		}
	}
#endif /* WITHCAT */

	if (full)
	{
		const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
		/* преустановка всех параметров приемника или передатчика под новый режим */
		setgsubmode(getsubmode(bi));		/* установка gsubmode, gagc, gfi и так далее */
	}

	if (full2)
	{
		/* Полная перенастройка. Изменился режим (или одно из значений hint). */
		if (gtx == 0)
		{
			board_set_tx(0);	/* перед всеми перенастройками выключаем передатчик */
			board_update();		/* вывести забуферированные изменения в регистры */
			// todo: убрать после решения проблем с переходом между приёмом и передачей в FPGA
		}

		const uint_fast8_t rxbi = getbankindex_tx(0);
		const uint_fast8_t txbi = getbankindex_tx(1);
		const uint_fast8_t txsubmode = getsubmode(txbi);		/* код режима, который будет при передаче */
		const uint_fast8_t txmode = submodes [txsubmode].mode;

		for (pathi = 0; pathi < pathn; ++ pathi)
		{
			const uint_fast8_t asubmode = getasubmode(pathi);	// SUBMODE_CWZ/SUBMODE_CWZSMART for tune
			pamodetempl = getmodetempl(asubmode);
			const uint_fast8_t amode = submodes [asubmode].mode;
			const uint_fast8_t alsbmode = getsubmodelsb(asubmode, forcelsb [pathi]);	// Принимаемая модуляция на нижней боковой
			//
			// lo2side - определяется конфигурацией
			// lo3side - определяется конфигурацией
			//const static uint_fast8_t lo2side = LO2_SIDE;
			//const static uint_fast8_t lo3side = LO3_SIDE;
			//const uint_fast8_t lo4side = getsidelo4();	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
			//const uint_fast8_t lo5side = getsidelo5(amode, gtx);	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
			//const uint_fast8_t lo6side = getsidelo6(amode, gtx);	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
			//
			const uint_fast8_t sides [7] = { lo0side, lo1side [pathi], lo2side, lo3side, getsidelo4(), getsidelo5(amode, gtx), getsidelo6(amode, gtx) };
			uint_fast8_t mixXlsbs [sizeof sides / sizeof sides [0]];
			uint_fast8_t i;
			for (i = 0; i < sizeof sides / sizeof sides [0]; ++ i)
			{
				mixXlsbs [i] = getlsbloX(alsbmode, i, sides, sizeof sides / sizeof sides [0]);
			}
			ASSERT(pamodetempl != NULL);
			// расчёт частот в тракте
			//
		#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_DCRX)
			enum { dc = 1 };	/* работа в режиме прямого преобразования со всегда нулевой частотой LO4 */
		#else
			const uint_fast8_t dc = dctxmodecw && gtx && pamodetempl->txcw;	// на передачу формируем первым гетеродином
		#endif

			const filter_t * workfilter;
			if (gtx != 0)
			{
				workfilter = gettxfilter(asubmode, getsuitabletx(amode, 0));	/* получаем по gfi/gfitx, хранится фильтр, включенный для данного режима */
			}
			else
			{
				workfilter = getrxfilter(asubmode, gfi);	/* получаем по gfi/gfitx, хранится фильтр, включенный для данного режима */
			}
			ASSERT(workfilter != NULL);
	#if WITHIF4DSP
			// Так же, здесь можно решать, какой фильтр ПЧ требуется для данного фильтра dsp.
			// В случае DUC/DDC, используется "заглушка" - IF3_TYPE_BYPASS.
			const uint_fast8_t bwseti = mdt [amode].bwsetis [gtx];
			const uint_fast8_t wide = bwseti_getwide(bwseti);
	#else /* WITHIF4DSP */
			const uint_fast8_t wide = workfilter->widefilter;
	#endif /* WITHIF4DSP */
			//
			const int_fast16_t pbt = getpbt(workfilter, amode, gtx);
			const int_fast16_t ifshift = getifshift(workfilter, amode, gtx);	/* положительные значения - повышение тембра (фильтр сдвигается "выше"). */

			const int_fast32_t freqif6 = getif6(amode, gtx, wide); // Positive number: ssb:0, cw=700, drm=12k
			const int_fast32_t freqlo6 = UPPERTOSIGN(! mixXlsbs [6], getlo6(amode, gtx, wide, ifshift));
			const int_fast32_t freqif5 = freqlo6 + UPPERTOSIGN(mixXlsbs [6], freqif6); 	// в режиме CW должно быть 0 для DDC/DUC
			const int_fast32_t freqlo5 = UPPERTOSIGN(mixXlsbs [5], getlo5(amode, gtx));	// 0 or DSP IF freq (12 kHz)
			const int_fast32_t freqif4 = freqlo5 + UPPERTOSIGN(mixXlsbs [5], freqif5);
			const int_fast32_t freqlo4ref = getlo4ref(workfilter, amode, mixXlsbs [4], gtx);

			// Возможно две стратегии переноса частоты - с согласованием частот среза
			// и с согласованием центральных частот полос пропускания.
			// Вариант с согласованием центральных частот - это случай включённого узкого фильтра ПЧ и телеграфного режима.
			// Или, случай WITHIF4DSP.
	#if WITHFIXEDBFO || WITHDUALFLTR
			// Выравнивание IF с привязкой к скату фильтра основной селекции
			// расчёт использует freqif4, freqlo5 == 0
			const int_fast32_t freqlo4 = freqlo4ref - freqlo5;	// если гетеродин один
			const int_fast32_t freqif3 = dc ? freqif4 : (freqlo4 + UPPERTOSIGN(mixXlsbs [4], freqif4));
	#elif WITHDUALBFO
			// Выравнивание IF с привязкой к скату фильтра основной селекции
			// расчёт использует freqif4, freqlo5 == 0
			const int_fast32_t freqlo4 = freqlo4ref - freqlo5;	// гетеродинов два - для разных скатов фильтра
			const int_fast32_t freqif3 = dc ? freqif4 : (freqlo4 + UPPERTOSIGN(mixXlsbs [4], freqif4));
	#elif WITHIF4DSP
			// Выравнивание IF с привязкой к центру фильтра основной селекции
			// расчёт использует freqif4, freqlo5
			const int_fast32_t freqlo4 = freqlo4ref - freqlo5;	// Надо согласовать центр фильтра с частотой переноса на baseband
			const int_fast32_t freqif3 = dc ? freqif4 : (freqlo4 + UPPERTOSIGN(mixXlsbs [4], freqif4));
	#elif 0
			// todo: не работает сохранение картины приёма при переключении SSB->CW, зато работает IF SHIFT
			// Выравнивание IF с привязкой к скату фильтра основной селекции
			// расчёт использует freqif4, freqif5 = - ifshift
			const int_fast32_t freqlo4 = dc ? 0 : (freqlo4ref - UPPERTOSIGN(mixXlsbs [4], freqif4));	// used for programming BFO
			const int_fast32_t freqif3 = dc ? freqif4 : (freqlo4ref - UPPERTOSIGN(mixXlsbs [4], freqif5));

	#elif 0
			// todo: работает сохранение картины приёма при переключении SSB->CW, но IF SHIFT меняет частоту приёма
			// Выравнивание IF с привязкой к скату фильтра основной селекции
			// расчёт использует freqif4, freqif5 = - ifshift
			const int_fast32_t freqlo4 = dc ? 0 : (freqlo4ref - UPPERTOSIGN(mixXlsbs [4], freqif4));	// used for programming BFO
			const int_fast32_t freqif3 = dc ? freqif4 : (freqlo4ref + UPPERTOSIGN(mixXlsbs [4], freqif4) - UPPERTOSIGN(mixXlsbs [4], freqif5));
	#else
			// Убран возврат ifshif в getlo6
			const int_fast32_t freqif3 = dc ? freqif4 : (freqlo4ref - UPPERTOSIGN(mixXlsbs [4], ifshift));
			const int_fast32_t freqlo4 = dc ? 0 : (freqif3 - UPPERTOSIGN(mixXlsbs [4], freqif4));	// used for programming BFO
	#endif

			// Взаимным смещением LO1 и LO2 производится "подрезание" полосы пропускания - PBTs
			const int_fast32_t freqlo3 = getlo3(amode, mixXlsbs [3], gtx) + UPPERTOSIGN16(mixXlsbs [3], pbt);	/* частота для гетеродина, осуществляющего passband tuning = PBT */
			const int_fast32_t freqif2 = freqlo3 + UPPERTOSIGN(mixXlsbs [3], freqif3);
			const int_fast32_t freqlo2 = getlo2(workfilter, amode, mixXlsbs [2], gtx, lo2hint [pathi]);
			const int_fast32_t freqif1 = freqlo2 + UPPERTOSIGN(mixXlsbs [2], freqif2);
			const int_fast32_t freqlo0 = getlo0(lo0hint);
			synth_if1 [pathi] = UPPERTOSIGN(mixXlsbs [1], freqif1) - UPPERTOSIGN(mixXlsbs [0], freqlo0);	// Запоминается для последующего преобразования рабочей частоты к частоте гетеродина.


	#if 0 && WITHDEBUG
			{
				const uint_fast8_t bi = getbankindex_pathi(pathi);
				const int_fast32_t freq = gfreqs [bi];
				PRINTF(submodes [asubmode].qlabel);
				PRINTF(PSTR(" pathi=%d"), pathi);
				PRINTF(PSTR(" f="));	printfreq(freq);
				PRINTF(PSTR(" lo0="));	printfreq(freqlo0);
				PRINTF(PSTR(" lo1="));	printfreq(synth_freq2lo1(freq, pathi));
				PRINTF(PSTR(" pbt="));	printfreq(pbt);
				PRINTF(PSTR(" ifshift="));	printfreq(ifshift);
				PRINTF(" bw=%s", workfilter->labelf3);
				PRINTF(" dbw=%s", hamradio_get_rxbw_label3());
				PRINTF(PSTR("\n"));
				PRINTF(
					PSTR("mixXlsbs[0]=%d, [1]=%d, [2]=%d, [3]=%d, [4]=%d, [5]=%d, [6]=%d dc=%d tx=%d\n"),
						mixXlsbs [0], mixXlsbs [1], mixXlsbs [2], mixXlsbs [3], mixXlsbs [4], mixXlsbs [5], mixXlsbs [6], dc, gtx
					);
				PRINTF(PSTR(" [lo1="));	printfreq(synth_freq2lo1(freq, pathi));
				PRINTF(PSTR("]if1="));	printfreq(freqif1);
				PRINTF(PSTR(" [lo2="));	printfreq(freqlo2);
				PRINTF(PSTR("]if2="));	printfreq(freqif2);
				PRINTF(PSTR(" [lo3="));	printfreq(freqlo3);
				PRINTF(PSTR("]if3="));	printfreq(freqif3);

				//PRINTF(PSTR("\n"));

				PRINTF(PSTR(" [lo4=%d*"), getlo4enable(amode, gtx));	printfreq(freqlo4);
				PRINTF(PSTR("]if4="));	printfreq(freqif4);
				PRINTF(PSTR(" ["));	printfreq(freqlo5);
				PRINTF(PSTR("]if5="));	printfreq(freqif5);
				PRINTF(PSTR(" ["));	printfreq(freqlo6);
				PRINTF(PSTR("]if6="));	printfreq(freqif6);

				PRINTF(PSTR("\n"));
			}

	#endif /* WITHDEBUG */

			board_set_trxpath(pathi);	/* Тракт, к которому относятся все последующие вызовы. При перередаяе используется индекс 0 */
			if (gtx == 0)
			{
			#if WITHNOTCHONOFF
				board_set_notch(gnotch && notchmodes [gnotchtype].code != BOARD_NOTCH_OFF);
				board_set_notchnarrow(gnotch && notchmodes [gnotchtype].code != BOARD_NOTCH_OFF && pamodetempl->nar);
			#elif WITHNOTCHFREQ
				board_set_notch_mode(gnotch == 0 ? BOARD_NOTCH_OFF : notchmodes [gnotchtype].code);
				board_set_notch_width(gnotchwidth.value);
				board_set_notch_freq(gnotchfreq.value);	// TODO: при AUTONOTCH ставить INT16_MAX ?
			#endif /* WITHNOTCHFREQ */
			#if WITHIF4DSP
				const uint_fast8_t agcseti = pamodetempl->agcseti;
				board_set_agcrate(gagc [agcseti].rate);			/* на n децибел изменения входного сигнала 1 дБ выходного. UINT8_MAX - "плоская" АРУ */
				board_set_agc_scale(gagc [agcseti].scale);		/* Для эксперементов по улучшению приема АМ */
				board_set_agc_t0(gagc [agcseti].t0);
				board_set_agc_t1(gagc [agcseti].t1);
				board_set_agc_t2(gagc [agcseti].release10);		// время разряда медленной цепи АРУ
				board_set_agc_t4(gagc [agcseti].t4);			// время разряда быстрой цепи АРУ
				board_set_agc_thung(gagc [agcseti].thung10);	// hold time (hung time) in 0.1 sec
				board_set_squelch_level(pamodetempl->dspmode [gtx] == DSPCTL_MODE_RX_NFM ? ulmax(gsquelch.value, gsquelchNFM) : gsquelch.value);
				board_set_gainnfmrx(ggainnfmrx10 * 10);	/* дополнительное усиление по НЧ в режиме приёма NFM 100..1000% */
				#if WITHSUBTONES
					board_set_ctcssrx(pamodetempl->subtone && param_getvalue(& xgsubtoneirx) ? gsubtones [param_getvalue(& xgsubtoneirx)] : 0);	// частота subtone (до десятых долей герца).
				#endif /* WITHSUBTONES */
			#endif /* WITHIF4DSP */
				board_set_nb_enable(pathi, 0);	/* Управлением включением RX Noise Blanker */
			} /* tx == 0 */

		#if WITHIF4DSP
			#if WITHSUBTONES
				// Установка параметров  Continuous Tone-Coded Squelch System or CTCSS
				board_set_ctcsstx(gsubtones [param_getvalue(& xgsubtoneitx)]);	// частота subtone (до десятых долей герца).
			#endif /* WITHSUBTONES */
			#if WITHTX && WITHSUBTONES
				// Установка параметров  Continuous Tone-Coded Squelch System or CTCSS
			#endif /* WITHTX && WITHSUBTONES */
			board_set_aflowcutrx(bwseti_getlow(bwseti));	/* Нижняя частота среза фильтра НЧ по приему */
			board_set_afhighcutrx(bwseti_gethigh(bwseti));	/* Верхняя частота среза фильтра НЧ по приему */
			board_set_afresponcerx(bwseti_getafresponce(bwseti));	/* изменение тембра звука в приемнике - на Samplerate/2 АЧХ становится на столько децибел  */

			board_set_lo6(freqlo6);	/* иначе, в случае WITHIF4DSP - управление знаком частоты */
			board_set_fullbw6(getif6bw(amode, gtx, wide));	/* Установка частоты среза фильтров ПЧ в алгоритме Уивера - параметр полная полоса пропускания */
			board_set_fltsofter(gtx ? WITHFILTSOFTMIN : bwseti_getfltsofter(bwseti));	/* Код управления сглаживанием скатов фильтра основной селекции на приёме */
			board_set_dspmode(pamodetempl->dspmode [gtx]);
			#if WITHDSPEXTDDC	/* "Воронёнок" с DSP и FPGA */
				board_set_dactest(gdactest);		/* вместо выхода интерполятора к ЦАП передатчика подключается выход NCO */
				board_set_dacstraight(gdacstraight);	/* Требуется формирование кода для ЦАП в режиме беззнакового кода */
				board_set_tx_inh_enable(gtxinhenable);				/* разрешение реакции на вход tx_inh */
				board_set_tx_bpsk_enable(pamodetempl->dspmode [gtx] == DSPCTL_MODE_TX_BPSK);	/* разрешение прямого формирования модуляции в FPGA */				/* разрешение прямого формирования модуляции в FPGA  */
				board_set_mode_wfm(pamodetempl->dspmode [gtx] == DSPCTL_MODE_RX_WFM);	/* разрешение прямого формирования модуляции в FPGA */				/* разрешение прямого формирования модуляции в FPGA  */
			#endif /* WITHDSPEXTDDC */
		#endif /* WITHIF4DSP */

		#if WITHTX
			/* переносить эти параметры под условие перенастройки в режиме приёма не стал, так как меню может быть вызвано и при передаче */
			board_set_txcw(pamodetempl->txcw);	// при передаче будет режим без SSB модулятора
			board_set_vox(gvoxenable && getmodetempl(txsubmode)->vox);	// включение внешних схем VOX
			#if WITHSUBTONES
				// Установка параметров  Continuous Tone-Coded Squelch System or CTCSS
				board_set_ctcsstx(getmodetempl(txsubmode)->subtone && param_getvalue(& xgsubtoneitx) && gtx ?
						gsubtones [param_getvalue(& xgsubtoneitx)] : 0);	// частота subtone (до десятых долей герца).
				board_set_subtonelevel(getmodetempl(txsubmode)->subtone && param_getvalue(& xgsubtoneitx) && gtx ?
						param_getvalue(& xgctsslevel) : 0);	/* Уровень сигнала CTCSS в процентах - 0%..100% */
			#endif /* WITHSUBTONES */
			#if WITHVOX
				seq_set_vox_time(voxdelay);		/* разрешение голосового управления переходом на передачу */
				vox_set_levels(gvoxlevel, gavoxlevel);		/* установка параметров vox */
			#endif /* WITHVOX */
			board_set_mikemute(gmuteall || getactualtune() || getmodetempl(txsubmode)->mute);	/* отключить микрофонный усилитель */
			seq_set_txgate(pamodetempl->txgfva, pamodetempl->sdtnva);		/* как должен переключаться тракт на передачу */
			board_set_txlevel(getactualtxboard());	/* BOARDPOWERMIN..BOARDPOWERMAX */

		#endif /* WITHTX */

		/* Этот блок (установка опорной частоты DDS) вызывать до настроек частот */
		#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
			synth_lo1_setreference(si570_get_xtall_base() + si570_xtall_offset);
		#else
			synth_lo1_setreference(getsynthref(amode));	// расчет коэфф. для работы синтезаторв
		#endif
			synth_setreference(getsynthref(amode));	// расчет коэфф. для работы синтезаторв

		/* А теперь настраиваем частоты. */
			update_lo2(pathi, workfilter, amode, mixXlsbs [2], freqlo2, getlo2div(gtx), gtx, lo2hint [pathi]);
			update_lo3(pathi, amode, mixXlsbs [3], freqlo3, getlo3div(gtx), gtx);

			synth_lo4_setfreq(pathi, freqlo4, getlo4div(gtx), getlo4enable(amode, gtx));	/* утстановка третьего гетеродина */

		} // pathi

		// параметры, не имеющие специфики для разных приемников
		update_lo0(lo0hint, lo0side);
		board_set_sleep(sleepflag);

		if (gtx == 0)
		{
			/* при переходе в режим передачи эти параметры не трогаем -
			- возможно, передача будет идти cross-band, но нам не нужно,
			чтобы хлопали реле аттенюатора.
			*/
			/* при переходе в режим передачи эти параметры не трогаем -
			- возможно, передача будет идти cross-band, но нам не нужно,
			чтобы хлопали реле аттенюатора.
			*/
		#if WITHNOATTNOPREAMP
			// без управления аттенюатором и УВЧ
		#elif WITHONEATTONEAMP
			board_set_att(attmodes [gatt].codeatt);
			board_set_preamp(attmodes [gatt].codepre);
			board_set_attvalue(attmodes [gatt].atten10 / 10);
		#else /* WITHONEATTONEAMP */
			board_set_att(attmodes [gatt].code);
			board_set_preamp(pampmodes [gpamp].code);
			board_set_attvalue(attmodes [gatt].atten10 / 10);
		#endif /* WITHONEATTONEAMP */
		#if ! WITHAGCMODENONE
			board_set_dspagc(gagcoff ? BOARD_AGCCODE_OFF : agcmodes [gagcmode].code);
		#else /* ! WITHAGCMODENONE */
			board_set_dspagc(gagcoff ? BOARD_AGCCODE_OFF : BOARD_AGCCODE_ON);
		#endif /* ! WITHAGCMODENONE */
		#if WITHDSPEXTDDC	/* "Воронёнок" с DSP и FPGA */
			board_set_dither(gdither);	/* управление зашумлением в LTC2208 */
			board_set_adcrand(gadcrand);	/* управление интерфейсом в LTC2208 */
			board_set_showovf(gshowovf);	/* Показ индикатора переполнения АЦП */
		#endif /* WITHDSPEXTDDC */
		} /* (gtx == 0) */
	#if WITHIF4DSP
		speex_update_rx();
	#endif /* WITHIF4DSP */

	#if defined (RTC1_TYPE)
		board_setrtcstrobe(grtcstrobe);
	#endif /* defined (RTC1_TYPE) */
	#if WITHANTSELECTRX || WITHANTSELECT1RX
		board_set_rxantenna(rxantmodes [grxantenna].code);
		board_set_antenna(antmodes [ant2hint].code);
	#elif WITHANTSELECT2
		board_set_antenna(antmodes [ant2hint].code);
	#elif WITHANTSELECT
		board_set_antenna(antmodes [ant2hint].code [gtx]);
	#endif /* WITHANTSELECT || WITHANTSELECTRX */
	#if WITHELKEY
		board_set_wpm(param_getvalue(& xgelkeywpm));	/* скорость электронного ключа */
		#if WITHVIBROPLEX
			elkey_set_slope(param_getvalue(& xgelkeyslope));	/* скорость уменьшения длительности точки и паузы - имитация виброплекса */
		#endif /* WITHVIBROPLEX */
			elkey_set_format(param_getvalue(& xgdashratio), param_getvalue(& xgspaceratio));	/* соотношение тире к точке (в десятках процентов) */
			elkey_set_mode(elkeymodes [param_getvalue(& xgelkeymode)].code, elkeyreverse);	/* режим электронного ключа - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
		#if WITHTX && WITHELKEY
			seq_set_bkin_time(bkindelay);			/* параметры BREAK-IN */
			/*seq_rgbeep(0); */								/* формирование roger beep */
		#endif /* WITHTX && WITHELKEY */
	#endif /* WITHELKEY */

	#if WITHIF4DSP
		board_set_afgain(sleepflag == 0 ? param_getvalue(& xafgain1) : BOARD_AFGAIN_MIN);	// Параметр для регулировки уровня на выходе аудио-ЦАП
		board_set_ifgain(sleepflag == 0  ? param_getvalue(& xrfgain1) : BOARD_IFGAIN_MIN);	// Параметр для регулировки усиления ПЧ

		const uint_fast8_t txaprofile = gtxaprofiles [getmodetempl(txsubmode)->txaprofgp];	// значения 0..NMICPROFILES-1

		#if ! defined (CODEC1_TYPE) && WITHUSBHW && WITHUSBUACOUT
			/* если конфигурация без автнонмного аудиокодека - все входы модулятора получают звук с USB AUDIO */
			const uint_fast8_t txaudiocodenormal = BOARD_TXAUDIO_USB;
		#elif WITHBBOX && defined (WITHBBOXMIKESRC)
			const uint_fast8_t txaudiocodenormal = WITHBBOXMIKESRC;
		#else /* defined (WITHBBOXMIKESRC) */
			const uint_fast8_t txaudiocodenormal = txaudiosrcs [gtxaudio [txmode]].code;	// Код источника
		#endif /* defined (WITHBBOXMIKESRC) */

		// Источник для передачи в цифровом режиме.
		// Если обычный источник BT или USB - не меняем.
		const uint_fast8_t txaudiocodedigi =
				(txaudiocodenormal == BOARD_TXAUDIO_USB || txaudiocodenormal == BOARD_TXAUDIO_USB) ?
						txaudiocodenormal : BOARD_TXAUDIO_USB;
		const uint_fast8_t txaudiocode = (gdatamode || txreq_gettxdata(& txreqst0)) ? txaudiocodedigi : txaudiocodenormal;

		board_set_lineinput(txaudiocode == BOARD_TXAUDIO_LINE);

		board_set_digigainmax(gdigigainmax);
		board_set_fsadcpower10(	/*	Мощность, соответствующая full scale от IF ADC */
				param_getvalue(lo0side != LOCODE_INVALID ? & xgfsadcpower10xv : & xgfsadcpower10) +
				gerflossdb10(lo0side != LOCODE_INVALID, gatt, gpamp));
		display2_set_showdbm(gshowdbm);		// Отображение уровня сигнала в dBm или S-memter (в зависимости от настроек)
		#if WITHUSEDUALWATCH
			board_set_mainsubrxmode(getactualmainsubrx());		// Левый/правый, A - main RX, B - sub RX
		#endif /* WITHUSEDUALWATCH */
		#if WITHUSBHW && WITHUSBUAC
			board_set_btaudioplayer(param_getvalue(& xgbtaudioplayer));
			board_set_uacplayer((gtx && param_getvalue(& xgdatamode)) || param_getvalue(& xguacplayer));	/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
			#if WITHRTS96 || WITHRTS192
				board_set_swaprts(param_getvalue(& xgswapiq));	/* Поменять местами I и Q сэмплы в потоке RTS96 */
			#endif /* WITHRTS96 || WITHRTS192 */
			#if WITHTX
				board_set_datatx(param_getvalue(& xgdatatx));	/* автоматическое изменение источника при появлении звука со стороны компьютера */
			#endif /* WITHTX */
				board_set_usb_ft8cn(param_getvalue(& xgusb_ft8cn));	/* совместимость VID/PID для работы с программой FT8CN */
				board_set_usb_hs(param_getvalue(& xgusb_hs));	/* Использование USB HS dvtcn USB FS */
		#endif /* WITHUSBHW && WITHUSBUAC */
		#if WITHTX
			board_set_mikeboost20db(gmikeboost20db);	// Включение предусилителя за микрофоном
			board_set_lineamp(glineamp);	/* усиление с линейного входа */
			board_set_txaudio(txaudiocode);	// Альтернативные источники сигнала при передаче
			board_set_mikeagc(gmikeagc);	/* Включение программной АРУ перед модулятором */
			board_set_mikeagcgain(gmikeagcgain);	/* Максимальное усидение АРУ микрофона */
			board_set_mikehclip(gmikehclip);	/* Ограничитель */
			#if WITHCOMPRESSOR
				board_set_compressor(gcompressor_attack, gcompressor_release, gcompressor_hold, gcompressor_gain, gcompressor_threshold);
			#endif /* WITHCOMPRESSOR */
			#if WITHREVERB
				board_set_reverb(greverb, greverbdelay, greverbloss);	/* ревербератор */
			#endif /* WITHREVERB */
			#if WITHELKEY
				board_set_cwedgetime(gcwedgetime);	/* Время нарастания/спада огибающей телеграфа при передаче - в 1 мс */
				board_set_cwssbtx(gcwssbtx);	/* разрешение передачи телеграфа как тона в режиме SSB */
			#endif /* WITHELKEY */
		#endif /* WITHTX */
		board_set_moniflag(gmoniflag);	/* разрешение самопрослушивания */
		board_set_sidetonelevel(gsidetonelevel);	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
		#if (WITHSPECTRUMWF && ! LCDMODE_DUMMY) || WITHAFSPECTRE
			const uint8_t bi_main = getbankindexmain();	/* VFO A modifications */
			board_set_topdb(param_getvalue(gtxloopback && gtx ? & xgtopdbtx : & xgtopdb));		/* верхний предел FFT */
			board_set_bottomdb(param_getvalue(gtxloopback && gtx ? & xgbottomdbtx : & xgbottomdb));		/* нижний предел FFT */
			board_set_zoomxpow2(gzoomxpow2);	/* уменьшение отображаемого участка спектра */
			display2_set_lvlgridstep(glvlgridstep);	/* Шаг сетки уровней в децибелах */
			board_set_view_style(param_getvalue(& xgviewstyle));			/* стиль отображения спектра и панорамы */
			board_set_view3dss_mark(gview3dss_mark);	/* Для VIEW_3DSS - индикация полосы пропускания на спектре */
			board_set_tx_loopback(gtxloopback && gtx);	/* включение спектроанализатора сигнала передачи */
			board_set_afspeclow(gafspeclow);	// нижняя частота отображения спектроанализатора
			board_set_afspechigh(gafspechigh);	// верхняя частота отображения спектроанализатора
			display2_set_rxbwsatu(grxbwsatu);	/* 0..100 - насыщнность цвета заполнения "шторки" - индикатор полосы пропускания примника на спкктре. */
			display2_set_filter_spe(gspecbeta100);	/* beta - парамеры видеофильтра спектра */
			display2_set_filter_wfl(gwflbeta100);	/* beta - парамеры видеофильтра водопада */
			display2_set_spectrumpart(gspectrumpart);	/* Часть отведенной под спектр высоты экрана 0..100 */
		#endif /* (WITHSPECTRUMWF && ! LCDMODE_DUMMY) || WITHAFSPECTRE */
	#endif /* WITHIF4DSP */

	#if WITHHDMITVHW
		board_set_tvoutformat(param_getvalue(& xhdmiformat));	// Установить режим отображения на выдеовыходе
	#endif /* WITHHDMITVHW */

	#if WITHAFEQUALIZER
		board_set_equalizer_rx(geqrx);
		board_set_equalizer_tx(geqtx);
		board_set_equalizer_rx_gains(geqrxparams);
		board_set_equalizer_tx_gains(geqtxparams);
	#endif /* WITHAFEQUALIZER */

	#if WITHTX
		#if defined (CODEC1_TYPE) && WITHAFCODEC1HAVEPROC
			board_set_mikeequal(param_getvalue(& xgmikeequalizer));	// включение обработки сигнала с микрофона (эффекты, эквалайзер, ...)
			board_set_mikeequalparams(gmikeequalizerparams);	// Эквалайзер 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
		#endif /* defined (CODEC1_TYPE) && WITHAFCODEC1HAVEPROC */
		#if WITHIF4DSP
		{
			const uint_fast8_t asubmode = getasubmode(0);	// SUBMODE_CWZ/SUBMODE_CWZSMART for tune
			const uint_fast8_t amode = submodes [asubmode].mode;
			const uint_fast8_t bwseti = mdt [amode].bwsetis [1];	// индекс банка полос пропускания для данного режима на передаче
			board_set_aflowcuttx(bwseti_getlow(bwseti));	/* Нижняя частота среза фильтра НЧ по передаче */
			board_set_afhighcuttx(bwseti_gethigh(bwseti));	/* Верхняя частота среза фильтра НЧ по передаче */
			board_set_afresponcetx(bwseti_getafresponce(bwseti));	/* коррекция АЧХ НЧ тракта передатчика */
			/* мощность регулируется умножнением выходных значений в потоке к FPGA / IF CODEC */
			// 0..10000
			board_set_dacscale(makebandf2adjust(bandf3hint, getactualtxampl()) * (int) gdacscale);	// BOARDDACSCALEMAX

			board_set_digiscale(ggaindigitx);	/* Увеличение усиления при передаче в цифровых режимах 100..300% */
			board_set_cwscale(ggaincwtx);	/* Увеличение усиления при передаче в CW режимах 50..100% */
			board_set_designscale(gdesignscale);	/* используется при калибровке параметров интерполятора */
			board_set_amdepth(gamdepth);	/* Глубина модуляции в АМ - 0..100% */
			board_rgrbeep_setfreq(1000);	/* roger beep - установка тона */
		}
		#endif /* WITHIF4DSP */
		seq_set_rgbeep(grgbeep);	/* разрешение (не-0) или запрещение (0) формирования roger beep */
		seq_set_rxtxdelay(rxtxdelay, txrxdelay, pretxdelay ? txrxdelay : 0);	/* установить задержку пре переходе на передачу и обратно. */
		board_sidetone_setfreq(gcwpitch10 * CWPITCHSCALE);	// Минимум - 400 герц (определено набором команд CAT Kenwood).
		board_set_classamode(gclassamode);	/* использование режима клвсс А при передаче */
		board_set_txgate(gtxgate);		/* разрешение драйвера и оконечного усилителя */
		board_set_forcexvrtr(gforcexvrtr);
		#if WITHMIC1LEVEL
			board_set_mik1level(gmik1level);
		#endif /* WITHMIC1LEVEL */
		board_set_autotune(txreq_getreqautotune(& txreqst0));
	#endif /* WITHTX */
	#if WITHMGLOOP
		board_set_bcdfreq1k(bandf1khint);
	#endif /* WITHMGLOOP */
	#if CTLSTYLE_IGOR
		board_set_bcdfreq100k(bandf100khint);
	#else /* CTLSTYLE_IGOR */
		board_set_bandf(bandfhint);		/* включение нужного полосового фильтра - возможно переключение УВЧ */
		board_set_bandf2(bandf2hint);	/* включение нужного полосового фильтра (ФНЧ) передатчика */
		board_set_bandf3(bandf3hint);	/* управление через разъем ACC */
	#endif /* CTLSTYLE_IGOR */
		board_keybeep_setfreq(gkeybeep10 * 10);	// Частота озвучивания нажатий клавиш (герц)

	#if WITHFANTIMER
		board_setfanflag(! fanpaflag);	// fanpaflag - сигнал выключения вентилятора
		#if WITHFANPWM
				board_setfanpwm(gfanpapwm);
		#endif /* WITHFANPWM */
	#endif /* WITHFANTIMER */
	#if WITHDCDCFREQCTL
		board_set_bldivider(bldividerout);
	#endif /* WITHDCDCFREQCTL */
	#if WITHLCDBACKLIGHT
		board_set_bglight(dimmflag || sleepflag || dimmmode, gbglight);		/* подсветка дисплея  */
	#endif /* WITHLCDBACKLIGHT */
	#if WITHKBDBACKLIGHT
		board_set_kblight((dimmflag || sleepflag || dimmmode) ? 0 : gkblight);			/* подсвтка клавиатуры */
	#endif /* WITHKBDBACKLIGHT */
		board_set_poweron(gpoweronhold);

	#if WITHSPKMUTE
		board_set_dsploudspeaker(gmutespkr); /*  выключение динамика (управление кодеком) */
	#endif /* WITHSPKMUTE */

	#if WITHAUTOTUNER
		board_set_tuner_group();
		ticker_setperiod(& ticker_tuner, NTICKS(gtunerdelay));
	#else /* WITHAUTOTUNER */
		board_set_tuner_bypass(1);
	#endif /* WITHAUTOTUNER */

	ticker_setperiod(& displatchticker, NTICKS(calcdivround2(1000, glatchfps)));	// частота обновления изображения на экране
	board_set_displayfps(glatchfps);
		/* просто настройки тракта и не относящиеся к приёму-пеердаче. */
	#if WITHCAT
		processcat_enable(catenable);
		cat_set_speed(catbr2int [catbaudrate] * BRSCALE);
		#if WITHCAT_MUX
			board_set_catmux(catmuxmodes [gcatmux].code);	// BOARD_CATMUX_USBCDC or BOARD_CATMUX_DIN8
		#endif /* WITHCAT_MUX */
	#endif	/* WITHCAT */

	#if WITHMODEM
		board_set_modem_speed100(modembr2int100 [gmodemspeed]);	// скорость передачи (модуляция, не последовательный порт) с точностью 1/100 бод
		board_set_modem_mode(gmodemmode);	// применяемая модуляция
	#endif /* WITHMODEM */

	#if WITHLFM
		synth_lfm_setparams(lfmstart100k * 100000ul + (int32_t) lfmfreqbias - LFMFREQBIAS, lfmstop100k * 100000ul + (int32_t) lfmfreqbias - LFMFREQBIAS, lfmspeed1k * 1000ul, getlo1div(gtx));
	#endif /* WITHLFM */

	#if WITHLO1LEVELADJ
		prog_dds1_setlevel(lo1level);
	#endif /* WITHLO1LEVELADJ */
	#if defined (DAC1_TYPE)
		board_set_dac1(dac1level);	/* подстройка частоты опорного генератора */
	#endif /* defined (DAC1_TYPE) */

	#if (WITHSWRMTR || WITHSHOWSWRPWR)
		display2_set_smetertype(param_getvalue(& xgsmetertype));
	#endif /* (WITHSWRMTR || WITHSHOWSWRPWR) */

		codec1_directupdate();
		board_update();		/* вывести забуферированные изменения в регистры */
	} // full2 != 0

	if (userfsg)
	{
		const uint_fast8_t bi = getbankindex_tx(gtx);
		const int_fast32_t freq = gfreqs [bi];
		synth_lo1_setfrequ(0, freq, getlo1div(gtx));
		//synth_bfo_setfreq(0);
		//synth_lopbt_setfreq(0);
	}
	else
	{
		for (pathi = 0; pathi < pathn; ++ pathi)
		{
			const uint_fast8_t bi = getbankindex_pathi(pathi);
			const int_fast32_t freq = gfreqs [bi];

			/* частота первого гетеродина может оказаться отрицательной */
			const uint_fast32_t lo1 = synth_freq2lo1(freq, pathi);
			synth_lo1_setfreq(pathi, lo1, getlo1div(gtx)); /* установка частоты первого гетеродина */
			synth_rts1_setfreq(pathi, getlo0(lo0hint) - freq);	// Установка центральной частоты панорамного индикатора

			if (pathi == 0)
			{
				// Хотя, сюда правильнее было бы передавать то же что и в synth_lo1_setfreq - lo1
				// Или, перенести туда
				// для учёта боковой и смещения частоты для алгоритма Уивера.
				// Пока, передаём dial frequency

		#if LINUX_SUBSYSTEM && WITHAD936XIIO
				if (get_ad936x_stream_status())
					ad936x_set_freq(freq);
		#elif LINUX_SUBSYSTEM && WITHAD936XDEV
				ad936xdev_set_freq(freq);
		#endif /* #if LINUX_SUBSYSTEM && WITHAD936XIIO */

			}
		}
	}

/* после всех перенастроек включаем передатчик */
	board_set_tx(gtx);		/* в конце выдаём сигнал разрешения передачи */
	board_update();		/* вывести забуферированные изменения в регистры */
	seq_ask_txstate(gtx);

	return full2;
}

static IRQLSPINLOCK_t boardupdatelock;
/* полная перенастройка */
void updateboard(void)
{
	uint_fast8_t f;
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& boardupdatelock, & oldIrql, BRDSYS_IRQL);
	f = updateboard_noui(1);
	IRQLSPIN_UNLOCK(& boardupdatelock, oldIrql);
	if (f)
		gui_update();
}

/* частичная перенастройка - без смены режима работы. может вызвать полную перенастройку */
void updateboard_freq(void)
{
	uint_fast8_t f;
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& boardupdatelock, & oldIrql, BRDSYS_IRQL);
	f = updateboard_noui(0);
	IRQLSPIN_UNLOCK(& boardupdatelock, oldIrql);
	if (f)
		gui_update();
}


///////////////////////////
// обработчики кнопок клавиатуры

//////////////////////////
#if WITHELKEY && WITHTX

void uif_key_bkintoggle(void)
{
	gbkinenable = calc_next(gbkinenable, 0, 1);
	save_i8(OFFSETOF(struct nvmap, gbkinenable), gbkinenable);
	updateboard();
}

uint_fast8_t hamradio_get_bkin_value(void)
{
	return gbkinenable;
}

static IRQLSPINLOCK_t lockcwmsg = IRQLSPINLOCK_INIT;
static const char * usersend;

void uif_key_sendcw(const char * msg)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& lockcwmsg, & oldIrql, ELKEY_IRQL);
	if (usersend != 0 && * usersend != '\0')
		usersend = NULL;
	else
		usersend = msg;
	IRQLSPIN_UNLOCK(& lockcwmsg, oldIrql);
}

#else

uint_fast8_t hamradio_get_bkin_value(void)
{
	return 0;
}

#endif /* WITHELKEY */

#if WITHVOX && WITHTX

static void
uif_key_voxtoggle(void)
{
	gvoxenable = calc_next(gvoxenable, 0, 1);
	save_i8(OFFSETOF(struct nvmap, gvoxenable), gvoxenable);
	updateboard();
}

// текущее состояние VOX
uint_fast8_t hamradio_get_voxvalue(void)
{
	return gvoxenable;
}

#else /* WITHVOX && WITHTX */
// текущее состояние VOX
uint_fast8_t hamradio_get_voxvalue(void)
{
	return 0;
}

#endif /* WITHVOX && WITHTX */


#if WITHANTSELECT1RX

// antenna
const char * hamradio_get_ant5_value(void)
{
	static char b [6];
	local_snprintf_P(b, ARRAY_SIZE(b),
			PSTR("   %s"),
			rxantmodes[grxantenna].label
	);
	return b;
}

#elif WITHANTSELECTRX

// antenna
const char * hamradio_get_ant5_value(void)
{
	static char b [6];
	if (strlen(rxantmodes[grxantenna].label))
		local_snprintf_P(b, ARRAY_SIZE(b), PSTR("%s %s"), antmodes [gantenna].label, rxantmodes[grxantenna].label);
	else
		local_snprintf_P(b, ARRAY_SIZE(b), PSTR("%s"), antmodes [gantenna].label);

	return b;
}


#elif WITHANTSELECT

// antenna
const char * hamradio_get_ant5_value(void)
{
	return antmodes [gantenna].label5;
}

#elif WITHANTSELECT2
// antenna
const char * hamradio_get_ant5_value(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	static char b [6];
	local_snprintf_P(b, ARRAY_SIZE(b),
			PSTR("%s %s"),
			antmodes [geteffantenna(gfreqs [bi])].label,
			gantmanual ? "MN" : "AU"
	);
	return b;
}
#endif /* WITHANTSELECT || WITHANTSELECTRX */

// включение режима split (возможо, с расстройко от текущей частоты)
static void
uif_key_spliton(uint_fast8_t holded)
{
#if WITHSPLIT

	const uint_fast8_t srbi = getbankindex_raw(0);
	const uint_fast8_t tgbi = getbankindex_raw(1);
	const vindex_t tgvi = getvfoindex(tgbi);

	copybankstate(srbi, tgbi, holded == 0 ? 0 : getmodetempl(getsubmode(srbi))->autosplitK * 1000L);	/* копируем состояние текущего банка в противоположный */
	gsplitmode = VFOMODES_VFOSPLIT;

	storebandstate(tgvi, tgbi); // записать все параметры настройки (кроме частоты) в область данных VFO */
	storebandfreq(tgvi, tgbi);

	save_i8(RMT_SPLITMODE_BASE, gsplitmode);
	updateboard();

#elif WITHSPLITEX

	if (holded != 0)
	{
		const uint_fast8_t srbi = getbankindex_raw(0);
		const uint_fast8_t tgbi = getbankindex_raw(1);
		const vindex_t tgvi = getvfoindex(tgbi);

		copybankstate(srbi, tgbi, getmodetempl(getsubmode(srbi))->autosplitK * 1000L);	/* копируем состояние текущего банка в противоположный */

		storebandstate(tgvi, tgbi); // записать все параметры настройки (кроме частоты) в область данных VFO */
		storebandfreq(tgvi, tgbi);
	}
	gsplitmode = VFOMODES_VFOSPLIT;

	save_i8(RMT_SPLITMODE_BASE, gsplitmode);
	updateboard();

#else

#endif /* WITHSPLIT */
}

/* копирование в VFO B состояния VFO A */
// Performs the VFO copy (A=B) function.
// for WITHSPLITEX
static void
uif_key_click_b_from_a(void)
{
#if (WITHSPLIT || WITHSPLITEX)

	if (gsplitmode == VFOMODES_VFOSPLIT)
	{
		const uint_fast8_t sbi = getbankindex_ab(0);	// bank index исходных данных
		const uint_fast8_t tbi = getbankindex_ab(1);	// bank index куда копируются данные
		const vindex_t tgvi = getvfoindex(tbi);		// vfo index куда копируются данные

		copybankstate(sbi, tbi, 0);
		storebandstate(tgvi, tbi); // записать все параметры настройки (кроме частоты) в область данных VFO */
		storebandfreq(tgvi, tbi); // записать частоту в область данных VFO */
		updateboard();
	}

#endif /* (WITHSPLIT || WITHSPLITEX) */
}

// вылючение режима split
static void
uif_key_splitoff(void)
{
#if (WITHSPLIT || WITHSPLITEX)

	gsplitmode = VFOMODES_VFOINIT;
	save_i8(RMT_SPLITMODE_BASE, gsplitmode);

	updateboard();

#endif /* (WITHSPLIT || WITHSPLITEX) */
}

/* обмен частотой между VFO */
// for WITHSPLITEX
static void
uif_key_click_a_ex_b(void)
{
#if (WITHSPLIT || WITHSPLITEX)

	gvfoab = ! gvfoab;	/* меняем текущий VFO на протвоположный */
	save_i8(RMT_VFOAB_BASE, gvfoab);
	updateboard();

#endif /* (WITHSPLIT || WITHSPLITEX) */
}

///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////
/* Переход между режимами фиксированных частот
   или плавной настройки.
  */
//static void
//uif_key_memmode(uint_fast8_t tx)
//{
//while (repeat --)
//	gsplitmode = calc_next(gsplitmode, 0, VFOMODES_COUNT - 1); /* (vfo/vfoa/vfob/mem) */
//save_i8(RMT_SPLITMODE_BASE, gsplitmode);
//updateboard();
//}

///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////
/* переход по "строке" режимов - удержанное нажатие */
// step to next modecol
static void
uif_key_hold_modecol(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const uint_fast8_t moderow = gmoderows [bi];	/* строка таблицы запомненных режимов */

	uint_fast8_t modecol = getmodecol(moderow, modes [moderow][0] - 1, 0, bi);	/* выборка из битовой маски. Возможно, значение modecolmap бует откорректировано.  */
	modecol = calc_next(modecol, 0, modes [moderow][0] - 1);
	putmodecol(moderow, modecol, bi);	/* внести новое значение в битовую маску */
	/* переустановка частот всех гетеродинов после смены режимов */
	/* gband должен быть уже известен */
	gsubmodechange(getsubmode(bi), bi); /* если надо - сохранение частоты в текущем VFO */
	updateboard();
}


///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////
/* переход по "столбцу" режимов - быстрое нажатие */
/* switch to next moderow */
static void
uif_key_click_moderow(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	uint_fast8_t defrow = gmoderows [bi];		/* строка таблицы режимов, которую покидаем */
	uint_fast8_t defcol = getmodecol(defrow, modes [defrow][0] - 1, 0, bi);	/* выборка из битовой маски. Возможно, значение modecolmap бует откорректировано.  */
	const uint_fast8_t forcelsb = getforcelsb(gfreqs [bi]);

	gmoderows [bi] = calc_next(gmoderows [bi], 0, MODEROW_COUNT - 1);		/* идём на следующую строку таблицы запомненых режимов */

#if WITHMODESETSMART
	defcol = locatesubmode(SUBMODE_SSBSMART, & defrow);
#else /* WITHMODESETSMART */
	if (gsubmode == SUBMODE_USB)		// если текущий режим USB - ищемм CW
		defcol = locatesubmode(SUBMODE_CW, & defrow);
	else if (gsubmode == SUBMODE_LSB)	// если текущий режим LSB - ищемм CWR
		defcol = locatesubmode(SUBMODE_CWR, & defrow);
	else if (gsubmode == SUBMODE_DGU)	// если текущий режим LSB - ищемм CWR
		defcol = locatesubmode(SUBMODE_USB, & defrow);
	else if (gsubmode == SUBMODE_DGL)	// если текущий режим LSB - ищемм CWR
		defcol = locatesubmode(SUBMODE_LSB, & defrow);
	#if WITHMODESETFULLNFM
	else if (gsubmode == SUBMODE_AM)	// если текущий режим AM - ищемм FM
		defcol = locatesubmode(SUBMODE_NFM, & defrow);
	#endif
	else								// в остальных случаях ищем режим по умолчанию для данного диапазона частот
		defcol = locatesubmode(forcelsb ? SUBMODE_LSB : SUBMODE_USB, & defrow);
#endif /* WITHMODESETSMART */
	/* если переходим не на строку с найденными режимаим */
	if (defrow != gmoderows [bi])
		defcol = 0;	/* default value (other cases, then switch from usb to cw, from lsb to cwr) */
	/* пытаемся обратиться за битами - они, взоможно, заменяться значением defcol */
	(void) getmodecol(gmoderows [bi], modes [gmoderows [bi]][0] - 1, defcol, bi); /* Возможно, значение modecolmap бует откорректировано. */

	/* переустановка частот всех гетеродинов после смены режимов */
	/* gband должен быть уже известен */
	gsubmodechange(getsubmode(bi), bi); /* если надо - сохранение частоты в текущем VFO */
	updateboard();
}

///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////
/* переход по "столбцу" режимов - быстрое нажатие */
/* switch to next moderow */
static void
uif_key_click_moderows(uint_fast8_t moderow)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const uint_fast8_t rowchanged = (gmoderows [bi] != moderow);
	uint_fast8_t defrow = gmoderows [bi] = moderow;		/* строка таблицы режимов, которую покидаем */
	uint_fast8_t defcol = getmodecol(defrow, modes [defrow][0] - 1, 0, bi);	/* выборка из битовой маски. Возможно, значение modecolmap бует откорректировано.  */
	const uint_fast8_t forcelsb = getforcelsb(gfreqs [bi]);

#if WITHMODESETSMART
	defcol = locatesubmode(SUBMODE_SSBSMART, & defrow);
#else /* WITHMODESETSMART */
	if (gsubmode == SUBMODE_USB)		// если текущий режим USB - ищемм CW
		defcol = locatesubmode(SUBMODE_CW, & defrow);
	else if (gsubmode == SUBMODE_LSB)	// если текущий режим LSB - ищемм CWR
		defcol = locatesubmode(SUBMODE_CWR, & defrow);
	else if (gsubmode == SUBMODE_DGU)	// если текущий режим LSB - ищемм CWR
		defcol = locatesubmode(SUBMODE_USB, & defrow);
	else if (gsubmode == SUBMODE_DGL)	// если текущий режим LSB - ищемм CWR
		defcol = locatesubmode(SUBMODE_LSB, & defrow);
	#if WITHMODESETFULLNFM
	else if (gsubmode == SUBMODE_AM)	// если текущий режим AM - ищемм FM
		defcol = locatesubmode(SUBMODE_NFM, & defrow);
	#endif
	else								// в остальных случаях ищем режим по умолчанию для данного диапазона частот
		defcol = locatesubmode(forcelsb ? SUBMODE_LSB : SUBMODE_USB, & defrow);
#endif /* WITHMODESETSMART */
	/* если переходим не на строку с найденными режимаим */
	if (defrow != gmoderows [bi])
		defcol = 0;	/* default value (other cases, then switch from usb to cw, from lsb to cwr) */
	/* пытаемся обратиться за битами - они, взоможно, заменяться значением defcol */
	(void) getmodecol(gmoderows [bi], modes [gmoderows [bi]][0] - 1, defcol, bi); /* Возможно, значение modecolmap бует откорректировано. */

	/* переустановка частот всех гетеродинов после смены режимов */
	/* gband должен быть уже известен */
	gsubmodechange(getsubmode(bi), bi); /* если надо - сохранение частоты в текущем VFO */
	updateboard();
}

///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////
/* переход по "строке" режимов - удержанное нажатие */
// step to next modecol
static void
uif_key_hold_modecols(uint_fast8_t moderow)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	if (gmoderows [bi] != moderow)	/* строка таблицы запомненных режимов */
	{
		uif_key_click_moderows(moderow);
		return;
	}

	uint_fast8_t modecol = getmodecol(moderow, modes [moderow][0] - 1, 0, bi);	/* выборка из битовой маски. Возможно, значение modecolmap бует откорректировано.  */
	modecol = calc_next(modecol, 0, modes [moderow][0] - 1);
	putmodecol(moderow, modecol, bi);	/* внести новое значение в битовую маску */
	/* переустановка частот всех гетеродинов после смены режимов */
	/* gband должен быть уже известен */
	gsubmodechange(getsubmode(bi), bi); /* если надо - сохранение частоты в текущем VFO */
	updateboard();
}

///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////

// короткое нажатие кнопки BAND UP
/* переход на следующий (с большей частотой) диапазон */
static void
uif_key_click_bandup(void)
{
	const uint_fast8_t bandset_no_check = 0;
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);
	const vindex_t b = getfreqband(gfreqs [bi], bandset_no_check);	/* определяем по частоте, в каком диапазоне находимся */
	verifyband(b);
	storebandstate(b, bi); // записать все параметры настройки (кроме частоты) в область данных диапазона */
	storebandfreq(b, bi);
	const vindex_t bn = getnext_ham_band(b, gfreqs [bi]);
	loadnewband(bn, bi);	/* загрузка всех параметров (и частоты) нового режима */
	storebandpos(bn);
	storebandfreq(vi, bi);	/* сохранение частоты в текущем VFO */
	storebandstate(vi, bi); // записать все параметры настройки (кроме частоты)  в текущем VFO */
	updateboard();
}
///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////
// короткое нажатие кнопки BAND DOWN
/* переход на предыдущий (с меньшей частотой) диапазон */
static void
uif_key_click_banddown(void)
{
	const uint_fast8_t bandset_no_check = 0;
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);
	const vindex_t b = getfreqband(gfreqs [bi], bandset_no_check);	/* определяем по частоте, в каком диапазоне находимся */
	verifyband(b);
	storebandstate(b, bi); // записать все параметры настройки (кроме частоты) в область данных диапазона */
	storebandfreq(b, bi);
	const uint_fast8_t bn = getprev_ham_band(b, gfreqs [bi]);
	loadnewband(bn, bi);	/* загрузка всех параметров (и частоты) нового режима */
	storebandpos(bn);
	storebandfreq(vi, bi);	/* сохранение частоты в текущем VFO */
	storebandstate(vi, bi); // записать все параметры настройки (кроме частоты)  в текущем VFO */
	updateboard();
}


/* переход на диапазон, содержащий указанную частоту */
static void
uif_key_click_bandjump(uint_fast32_t f)
{
	const uint_fast8_t bandset_no_check = 0;
#if	WITHDIRECTBANDS
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);
	const vindex_t b = getfreqband(gfreqs [bi], bandset_no_check);	/* определяем по частоте, в каком диапазоне находимся */
	vindex_t bn = getfreqband(f, bandset_no_check);

	const uint_fast8_t bandgroup = bandsmap [bn].bandgroup;
	verifyband(b);
	verifyband(bn);
	storebandstate(b, bi); // записать все параметры настройки (кроме частоты) в область данных диапазона */
	storebandfreq(b, bi);
	//
	//
	if (bandgroup != BANDGROUP_COUNT)
	{
		// новый поддиапазон является частью группы
		bn = loadvfy8up(RMT_BANDPOS(bandgroup), 0, HBANDS_COUNT - 1, bn);
		verifyband(bn);
		if (bandgroup == bandsmap [b].bandgroup)
		{
			// переключение в диапазон той же группы - переход в пределах группы
			bn = getnextbandingroup(bn, bandgroup);
			verifyband(bn);
			save_i8(RMT_BANDPOS(bandgroup), bn);
		}
	}
	loadnewband(bn, bi);	/* загрузка всех параметров (и частоты) нового режима */
	storebandfreq(vi, bi);	/* сохранение частоты в текущем VFO */
	storebandstate(vi, bi); // записать все параметры настройки (кроме частоты)  в текущем VFO */
	updateboard();
#endif /* WITHDIRECTBANDS */
}

/* переход на указанную частоту без задействования механизма bandgroup */
static void
uif_key_click_bandjump2(uint_fast32_t f, uint_fast8_t bandset_no_check)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);
	const vindex_t b = getfreqband(gfreqs [bi], bandset_no_check);	/* определяем по частоте, в каком диапазоне находимся */
	vindex_t bn = getfreqband(f, bandset_no_check);

	verifyband(b);
	verifyband(bn);

	storebandstate(b, bi); // записать все параметры настройки (кроме частоты) в область данных диапазона */
	storebandfreq(b, bi);

	loadnewband(bn, bi);	/* загрузка всех параметров (и частоты) нового режима */
	storebandfreq(vi, bi);	/* сохранение частоты в текущем VFO */
	storebandstate(vi, bi); // записать все параметры настройки (кроме частоты)  в текущем VFO */
	updateboard();
}

#if ! WITHAGCMODENONE
/* AGC mode switch
	 - вызывает сохранение состояния режима */
static void
uif_key_click_agcmode(void)
{
	gagcmode = calc_next(gagcmode, 0, AGCMODE_COUNT - 1);
	save_i8(RMT_AGC_BASE(submodes [gsubmode].mode), gagcmode);
	updateboard();
}
#endif /* ! WITHAGCMODENONE */

#if WITHANTSELECTRX || WITHANTSELECT1RX

/* действительно выбранная антенна с учетом ручного или автоматического переключения */
static uint_fast8_t geteffantenna(uint_fast32_t f)
{
	return gantenna;
}

static uint_fast8_t geteffrxantenna(uint_fast32_t f)
{
	return grxantenna;
}

/* Antenna switch
	  */
static void
uif_key_next_antenna(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);
	const uint_fast8_t bg = getfreqbandgroup(gfreqs [bi]);

	gantenna = calc_next(gantenna, 0, ANTMODE_COUNT - 1);
	loadbandgroup(bg, gantenna, grxantenna);
	storebandstate(vi, bi);	// запись всех режимов в область памяти диапазона
	updateboard();
}

/* Antenna switch
	  */
static void
uif_key_next_rxantenna(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);
	const uint_fast8_t bg = getfreqbandgroup(gfreqs [bi]);

	grxantenna = calc_next(grxantenna, 0, RXANTMODE_COUNT - 1);
	loadbandgroup(bg, gantenna, grxantenna);
	storebandstate(vi, bi);	// запись всех режимов в область памяти диапазона
	updateboard();
}

#elif WITHANTSELECT2

/* получить номер антенны в зависимости от частоты */
static uint_fast8_t getdefantenna(uint_fast32_t f)
{
	const uint_fast32_t fsw = hffreqswitch * 1000000uL;
	ASSERT((fsw > TUNE_BOTTOM) && (fsw < TUNE_TOP));
	return f > fsw;
}

/* действительно выбранная антенна с учетом ручного или автоматического переключения */
static uint_fast8_t geteffantenna(uint_fast32_t f)
{
	return gantmanual ? gantennabym : getdefantenna(f);
}

static uint_fast8_t geteffrxantenna(uint_fast32_t f)
{
	return grxantenna;
}

/* Antenna switch
	  */
static void
uif_key_next_antenna(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);
	const uint_fast8_t bg = getfreqbandgroup(gfreqs [bi]);

	gantennabym = calc_next(gantennabym, 0, ANTMODE_COUNT - 1);
	const uint_fast8_t effantenna = geteffantenna(gfreqs [bi]);
	const uint_fast8_t effrxantenna = geteffrxantenna(gfreqs [bi]);
	loadbandgroup(bg, effantenna, effrxantenna);
	storebandstate(vi, bi);	// запись всех режимов в область памяти диапазона
	updateboard();
}

/* ручной/автоматический выбор антенны */
static void
uif_key_next_autoantmode(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	gantmanual = calc_next(gantmanual, 0, 1);
	save_i8(RMT_ANTMANUAL_BASE, gantmanual);
	const uint_fast8_t bg = getfreqbandgroup(gfreqs [bi]);
	const uint_fast8_t effantenna = geteffantenna(gfreqs [bi]);
	const uint_fast8_t effrxantenna = geteffrxantenna(gfreqs [bi]);
	loadbandgroup(bg, effantenna, effrxantenna);
	updateboard();
}

#elif WITHANTSELECT

/* действительно выбранная антенна с учетом ручного или автоматического переключения */
static uint_fast8_t geteffantenna(uint_fast32_t f)
{
	return gantenna;
}

static uint_fast8_t geteffrxantenna(uint_fast32_t f)
{
	return grxantenna;
}

/* Antenna switch
	  */
static void
uif_key_next_antenna(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);
	const uint_fast8_t bg = getfreqbandgroup(gfreqs [bi]);

	gantenna = calc_next(gantenna, 0, ANTMODE_COUNT - 1);
	loadbandgroup(bg, gantenna, grxantenna);
	storebandstate(vi, bi);	// запись всех режимов в область памяти диапазона
	updateboard();
}

#else

/* действительно выбранная антенна с учетом ручного или автоматического переключения */
static uint_fast8_t geteffantenna(uint_fast32_t f)
{
	return gantenna;
}

static uint_fast8_t geteffrxantenna(uint_fast32_t f)
{
	return grxantenna;
}

#endif /* WITHANTSELECT || WITHANTSELECTRX */

#if ! WITHONEATTONEAMP
/* переключение режима предусилителя  */
static void
uif_key_click_pamp(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);

	verifyband(vi);

	gpamp = calc_next(gpamp, 0, PAMPMODE_COUNT - 1);
	storebandstate(vi, bi);	// запись всех режимов в область памяти диапазона
	updateboard();
}
#endif /* ! WITHONEATTONEAMP */


/* переключение режима аттенюатора  */
static void
uif_key_click_attenuator(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);

	verifyband(vi);

	gatt = calc_next(gatt, 0, ATTMODE_COUNT - 1);
	storebandstate(vi, bi);	// запись всех режимов в область памяти диапазона
	updateboard();
}

#if WITHPOWERLPHP
/* переключение режима мощности  */
static void
uif_key_click_pwr(void)
{
	gpwri = calc_next(gpwri, 0, PWRMODE_COUNT - 1);
	save_i8(RMT_PWR_BASE, gpwri);

	updateboard();
}
#endif /* WITHPOWERLPHP */

#if WITHNOTCHONOFF || WITHNOTCHFREQ
/* включение/выключение NOTCH  */
static void
uif_key_click_notch(void)
{
	gnotch = calc_next(gnotch, 0, 1);
	save_i8(RMT_NOTCH_BASE, gnotch);

	updateboard();
}

#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */

/* блокировка енкодера */
static void
uif_key_lockencoder(void)
{
	const uint_fast8_t bandset_no_check = 0;
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t b = getfreqband(gfreqs [bi], bandset_no_check);	/* определяем по частоте, в каком диапазоне находимся */

	glock = calc_next(glock, 0, 1);
	save_i8(RMT_LOCKMODE_BASE(b), glock);
	updateboard();
}

#if WITHBCBANDS
/* переход в режим переключения по вещательным диапазонам */
static void
uif_key_genham(void)
{
	gbandsetbcast = calc_next(gbandsetbcast, 0, 1);
	save_i8(OFFSETOF(struct nvmap, gbandsetbcast), gbandsetbcast);
	updateboard();
}

uint_fast8_t hamradio_get_genham_value(void)
{
	return gbandsetbcast;
}

#endif /* WITHBCBANDS */

#if WITHUSEFAST
/* переключение в режим крупного шага */
static void
uif_key_usefast(void)
{
	gusefast = calc_next(gusefast, 0, 1);
	save_i8(RMT_USEFAST_BASE, gusefast);
	updateboard();
}
#endif /* WITHUSEFAST */

#if WITHSPKMUTE

/* Включение-выключение динамика */
static void
uif_key_loudsp(void)
{
	param_keyclick(& xgmutespkr);
	updateboard();
}

uint_fast8_t hamradio_get_spkon_value(void)
{
	return ! param_getvalue(& xgmutespkr);
}

#endif /* WITHSPKMUTE */

///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////
#if WITHIF4DSP
/* Переключение полос пропускания в CW приёмника
	 - не вызывает сохранение состояния диапазона */

static void
uif_key_changebw(void)
{
	const uint_fast8_t bwseti = mdt [gmode].bwsetis [0];	// индекс банка полос пропускания для данного режима на приеме

	bwsetpos [bwseti] = calc_next(bwsetpos [bwseti], 0, bwsetsc [bwseti].last);
	save_i8(RMT_BWSETPOS_BASE(bwseti), bwsetpos [bwseti]);	/* только здесь сохраняем новый фильтр для режима */
	updateboard();
}

/* Переключение шумоподавления
	 - не вызывает сохранение состояния диапазона */

static void
uif_key_changenr(void)
{
	gnoisereducts [gmode] = calc_next(gnoisereducts [gmode], 0, 1);
	save_i8(RMT_NR_BASE(gmode), gnoisereducts [gmode]);
	updateboard();
}

#if WITHUSBHW && WITHUSBUAC

/* переключение источника звука с USB или обычного для данного режима */
static void
uif_key_click_datamode(void)
{
	gdatamode = calc_next(gdatamode, 0, 1);
	save_i8(RMT_DATAMODE_BASE, gdatamode);
	updateboard();
}

#endif /* WITHUSBHW && WITHUSBUAC */


#else /* WITHIF4DSP */
/* Переключение фильтров приёмника
	 - не вызывает сохранение состояния диапазона */
static void
uif_key_changefilter(void)
{
	gfi = getsuitablerx(gmode, calc_next(gfi, 0, getgfasize() - 1));
	save_i8(RMT_FILTER_BASE(gmode), gfi);	/* только здесь сохраняем новый фильтр для режима */
	updateboard();
}

#endif /* WITHIF4DSP */
///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////
#if WITHTX

/* включение режима настройки */
static void
uif_key_moxclick(void)
{
	if (txreq_get_tx(& txreqst0))
		txreq_rx(& txreqst0, NULL);
	else
		txreq_mox(& txreqst0);
}

///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////
/* включение режима настройки */

static void
uif_key_tune(void)
{
	txreq_txtone(& txreqst0);
}

#endif /* WITHTX */

#if WITHKEYBOARD
#endif /* WITHKEYBOARD */


#if WITHAUTOTUNER

static void
uif_key_bypasstoggle(void)
{
	const uint_fast8_t tx = 1;
	const uint_fast8_t bi = getbankindex_tx(tx);
	const uint_fast8_t bg = getfreqbandgroup(gfreqs [bi]);
    const uint_fast8_t ant = geteffantenna(gfreqs [bi]);

	tunerwork = calc_next(tunerwork, 0, 1);	// переключаем в противоположное состояние

	storetuner(bg, ant);

	if (tunerwork == 0)
	{
		txreq_rx(& txreqst0, NULL);	// сброс идущей настройки
	}
}

static void
uif_key_atunerstart(void)
{
	const uint_fast8_t tx = 1;
	const uint_fast8_t bi = getbankindex_tx(tx);
	const vindex_t b = getvfoindex(bi);
	const uint_fast32_t freq = gfreqs [bi];
	const uint_fast8_t bg = getfreqbandgroup(freq);
	const uint_fast8_t ant = geteffantenna(freq);

	const uint_fast8_t oldtunerwork = tunerwork;
	tunerwork = 1;
	txreq_reqautotune(& txreqst0, 1);
	if (txreq_getreqautotune(& txreqst0))
	{
		tunerwork = 1;
		save_i8(OFFSETOF(struct nvmap, bandgroups [bg].otxants [ant].tunerwork), tunerwork);
	}
	else
	{
		tunerwork = oldtunerwork;
	}
	updateboard();
}

uint_fast8_t
hamradio_get_bypvalue(void)
{
	return ! tunerwork;
}

uint_fast8_t
hamradio_get_atuvalue(void)
{
	return txreq_getreqautotune(& txreqst0);
}
#endif /* WITHAUTOTUNER */

#if WITHNOTCHONOFF || WITHNOTCHFREQ

uint_fast8_t hamradio_get_notchvalue(int_fast32_t * p)
{
#if WITHNOTCHFREQ
	* p = gnotchfreq.value;
#else /* WITHNOTCHFREQ */
	* p = 0;
#endif /* WITHNOTCHFREQ */
	return gnotch && notchmodes [gnotchtype].code != BOARD_NOTCH_OFF;
}

const char * hamradio_get_notchtype5_P(void)
{
	return notchmodes [gnotchtype].label;
}


#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ  */

#if WITHINTEGRATEDDSP
// NR ON/OFF
uint_fast8_t hamradio_get_nrvalue(int_fast32_t * p)
{
	* p = gnoisereductvl;
	return gnoisereducts [gmode] != 0;
}
#endif /* WITHINTEGRATEDDSP */

// текущее состояние TUNE
uint_fast8_t hamradio_get_tunemodevalue(void)
{
	return getactualtune();
}

#if WITHVOLTLEVEL

// Вольты в десятых долях
uint_fast8_t hamradio_get_volt_value(void)
{
#if WITHTDIRECTDATA

	return (sadcvalholder_t) board_getadc_filtered_truevalue(VOLTMRRIX) / 100;	// миливольты в 0.1 вольта

#elif WITHTARGETVREF

	unsigned Vref_mV = board_getadc_unfiltered_truevalue(VREFMVMRRIX); //WITHTARGETVREF;//ADCVREF_CPU * 100;
	if (Vref_mV == 0)
		Vref_mV = WITHTARGETVREF;
	const unsigned voltcalibr_mV = (Vref_mV * (VOLTLEVEL_UPPER + VOLTLEVEL_LOWER) + VOLTLEVEL_LOWER / 2) / VOLTLEVEL_LOWER;		// Напряжение fullscale - что показать при ADCVREF_CPU вольт на входе АЦП
	const uint_fast16_t mv = board_getadc_filtered_u16(VOLTMRRIX, 0, voltcalibr_mV);
	//PRINTF(PSTR("hamradio_get_volt_value: ref=%u, VrefmV=%u, v=%u, out=%u\n"), ref, Vref_mV, mv, (mv + 50) / 100);
	return (mv + 50) / 100;	// Приводим к десятым долям вольта

#elif WITHREFSENSOR
	// Измерение опрного напряжения
	const uint_fast8_t vrefi = VREFIX;
	const adcvalholder_t ref = board_getadc_unfiltered_truevalue(vrefi);	// текущее значение данного АЦП
	if (ref != 0)
	{
		//const unsigned Vref_mV = ADCVREF_CPU * 100;
		const unsigned Vref_mV = (uint_fast32_t) board_getadc_fsval(vrefi) * WITHREFSENSORVAL / ref;
		const unsigned voltcalibr_mV = (Vref_mV * (VOLTLEVEL_UPPER + VOLTLEVEL_LOWER) + VOLTLEVEL_LOWER / 2) / VOLTLEVEL_LOWER;		// Напряжение fullscale - что показать при ADCVREF_CPU вольт на входе АЦП
		const uint_fast16_t mv = board_getadc_filtered_u16(VOLTMRRIX, 0, voltcalibr_mV);
		//PRINTF(PSTR("hamradio_get_volt_value: ref=%u, VrefmV=%u, v=%u, out=%u\n"), ref, Vref_mV, mv, (mv + 50) / 100);
		return (mv + 50) / 100;	// Приводим к десятым долям вольта
	}
	else
	{
		//PRINTF(PSTR("hamradio_get_volt_value: ref=%u\n"), ref);
		return UINT8_MAX;
	}

#else /* WITHREFSENSOR */

	// TODO: разобраться почему это не работает на SW20xx
	//PRINTF(PSTR("hamradio_get_volt_value: VOLTMRRIX=%u, voltcalibr100mV=%u\n"), board_getadc_unfiltered_truevalue(VOLTMRRIX), voltcalibr100mV);
	return board_getadc_filtered_u8(VOLTMRRIX, 0, voltcalibr100mV);

#endif /* WITHREFSENSOR */
}

#endif /* WITHVOLTLEVEL */

#if (WITHTHERMOLEVEL || WITHTHERMOLEVEL2)

// Градусы в десятых долях
// Read from thermo sensor ST LM235Z (2 kOhm to +12)
int_fast16_t hamradio_get_PAtemp_value(void)
{
	const int_fast16_t thermo_offset = THERMOSENSOR_OFFSET;

	// XTHERMOIX - данные с АЦП напрямую
#if WITHTDIRECTDATA

	return (sadcvalholder_t) board_getadc_filtered_truevalue(XTHERMOMRRIX);	// 0.1 градуса

#elif WITHTARGETVREF

	unsigned Vref_mV = board_getadc_unfiltered_truevalue(VREFMVMRRIX); //WITHTARGETVREF;//ADCVREF_CPU * 100;
	if (Vref_mV == 0)
		Vref_mV = WITHTARGETVREF;
	const int_fast32_t mv = (int32_t) board_getadc_filtered_u32(XTHERMOMRRIX, 0, (uint_fast64_t) Vref_mV * (THERMOSENSOR_UPPER + THERMOSENSOR_LOWER) / THERMOSENSOR_LOWER);
	return (mv + thermo_offset) / THERMOSENSOR_DENOM;	// Приводим к десятым долям градуса

#elif WITHREFSENSOR
	// Измерение опрного напряжения
	const uint_fast8_t vrefi = VREFIX;
	const adcvalholder_t ref = board_getadc_unfiltered_truevalue(vrefi);	// текущее значение данного АЦП
	if (ref != 0)
	{
		const unsigned Vref_mV = (uint_fast32_t) board_getadc_fsval(vrefi) * WITHREFSENSORVAL / ref;
		const int_fast32_t mv = (int32_t) board_getadc_filtered_u32(XTHERMOMRRIX, 0, (uint_fast64_t) Vref_mV * (THERMOSENSOR_UPPER + THERMOSENSOR_LOWER) / THERMOSENSOR_LOWER);
		return (mv + thermo_offset) / THERMOSENSOR_DENOM;	// Приводим к десятым долям градуса
	}
	else
	{
		PRINTF(PSTR("hamradio_get_PAtemp_value: ref=%u\n"), ref);
		return 999;
	}

#elif WITHTHERMOLEVEL2

	const unsigned Vref_mV = ADCVREF_CPU * 100;
	const unsigned vrefff = (uint_fast64_t) Vref_mV * (THERMOSENSOR_UPPER + THERMOSENSOR_LOWER) / THERMOSENSOR_LOWER;
	const int_fast32_t mv = (int32_t) board_getadc_filtered_u32(XTHERMOMRRIX, 0, vrefff) - (int32_t) board_getadc_filtered_u32(XTHERMOREFMRRIX, 0, vrefff);
	return (mv + thermo_offset) / THERMOSENSOR_DENOM;	// Приводим к десятым долям градуса

#else /* WITHREFSENSOR */

	const unsigned Vref_mV = ADCVREF_CPU * 100;
	const int_fast32_t mv = (int32_t) board_getadc_filtered_u32(XTHERMOMRRIX, 0, (uint_fast64_t) Vref_mV * (THERMOSENSOR_UPPER + THERMOSENSOR_LOWER) / THERMOSENSOR_LOWER);
	return (mv + thermo_offset) / THERMOSENSOR_DENOM;	// Приводим к десятым долям градуса

#endif /* WITHREFSENSOR */
}

#endif /* WITHTHERMOLEVEL || WITHTHERMOLEVEL2 */

#if (WITHCURRLEVEL || WITHCURRLEVEL2)

// Ток в десятках милиампер (может быть отрицательным)
// PA current sense - ACS712ELCTR-05B-T chip
// PA current sense - ACS712ELCTR-30B-T chip
int_fast16_t hamradio_get_pacurrent_value(void)
{
	// Чувствительность датчиков:
	// x05B - 0.185 V/A
	// x20A - 0.100 V/A
	// x30A - 0.066 V/A

#if WITHCURRLEVEL_ACS712_30A
	// x30A - 0.066 V/A
	enum {
		sens = 66,			// millivolts / ampher
		scale = 100			// результат - в сотых долях ампера
	};
#elif WITHCURRLEVEL_ACS712_20A
	//  x20A - 0.100 V/A
	enum {
		sens = 100,			// millivolts / ampher
		scale = 100			// результат - в сотых долях ампера
	};
#else /* WITHCURRLEVEL_ACS712_30A */
	// x05B - 0.185 V/A
	enum {
		sens = 185,			// millivolts / ampher
		scale = 100			// результат - в сотых долях ампера
	};
#endif /*  */

#if WITHCURRLEVEL

	const uint_fast8_t adci = PASENSEMRRIX;

#elif WITHCURRLEVEL2

#endif

#if WITHTDIRECTDATA

#elif WITHTARGETVREF

	unsigned Vref_mV = board_getadc_unfiltered_truevalue(VREFMVMRRIX); //WITHTARGETVREF;//ADCVREF_CPU * 100;
	if (Vref_mV == 0)
		Vref_mV = WITHTARGETVREF;

#elif WITHREFSENSOR
	// Измерение опрного напряжения
	const uint_fast8_t vrefi = VREFIX;
	const adcvalholder_t ref = board_getadc_unfiltered_truevalue(vrefi);	// текущее значение данного АЦП
	if (ref == 0)
		return 0;
	//const unsigned Vref_mV = ADCVREF_CPU * 100;
	const unsigned Vref_mV = (uint_fast32_t) board_getadc_fsval(vrefi) * WITHREFSENSORVAL / ref;
#else /* WITHREFSENSOR */
	// опорное напряжение известно из конфигурации процессора.
	const unsigned Vref_mV = ADCVREF_CPU * 100;
#endif /* WITHREFSENSOR */


#if WITHTDIRECTDATA

	int curr10 = (sadcvalholder_t) board_getadc_filtered_truevalue(PASENSEMRRIX) / 10;	// милиамперы в десятки милиампер

#elif WITHCURRLEVEL2

	const adcvalholder_t FSval = board_getadc_fsval(PASENSEIX2);
	const adcvalholder_t midp = board_getadc_unfiltered_truevalue(PAREFERIX2);	// 2.5 voltage reference code
	const adcvalholder_t sense = board_getadc_unfiltered_truevalue(PASENSEIX2);

	const long curr10 = ((long) midp - (long) sense) * (int_fast64_t) Vref_mV * scale / ((long) sens * FSval);

#else /* WITHCURRLEVEL2 */

	const long vsense = board_getadc_filtered_u32(adci, 0, (uint_fast32_t) Vref_mV);

	static const long midpoint = 2500L;

	const adcvalholder_t FSval = board_getadc_fsval(adci);
	const adcvalholder_t midp = midpoint * FSval / Vref_mV;	// 2.5 voltage reference code
	const adcvalholder_t sense = board_getadc_filtered_truevalue(adci);

	const long curr10 = ((long) midp - (long) sense) * (int_fast64_t) Vref_mV * scale / ((long) sens * FSval);

#endif /* WITHCURRLEVEL2 */

	return curr10 + (gipacali + getipacalibase());
}

#endif /* (WITHCURRLEVEL || WITHCURRLEVEL2) */

uint_fast8_t hamradio_get_tx(void)
{
	return gtx;
}

// RX bandwidth
#if WITHIF4DSP

// Three-character wide printed current RX/TX bandwidth namw
const char * hamradio_get_rxbw_label3(void)
{
	const uint_fast8_t bwseti = mdt [gmode].bwsetis [gtx];	// индекс банка полос пропускания для данного режима
	return bwsetsc [bwseti].labels [bwsetpos[bwseti]];
}

// Four-character wide printed current RX/TX bandwidth value
const char * hamradio_get_rxbw_value4(void)
{
	static char s [5];
	const uint_fast8_t bwseti = mdt [gmode].bwsetis [gtx];	// индекс банка полос пропускания для данного режима
	int_fast32_t width = getif6bw(gmode, gtx, bwseti_getwide(bwseti));
	if (width >= 1000000)
		width = (1000000 - 1);
	int_fast16_t w100 = (width + 50) / 100;
	if (w100 < 10)	// до 1 кГц
		local_snprintf_P(s, ARRAY_SIZE(s), ".%02d", w100 * 10);
	else if (w100 < 100)	// 1 кГц..9 кГц
		local_snprintf_P(s, ARRAY_SIZE(s), "%1d.%1dk", w100 / 10, w100 % 10);
	else	// 10 и более кГц
		local_snprintf_P(s, ARRAY_SIZE(s), "%3dk", w100 / 10);

	return s;
}

#else /* WITHIF4DSP */

const char * hamradio_get_rxbw_label3(void)
{
#if WITHFIXEDBFO
	return PSTR("");
#else /* WITHFIXEDBFO */
	return getrxfilter(gsubmode, gfi)->labelf3;
#endif /* WITHFIXEDBFO */
}


// Four-character wide printed current RX/TX bandwidth value
// FIXME: stub implementation
const char * hamradio_get_rxbw_value4(void)
{
	static char s [5];
//	const uint_fast8_t bwseti = mdt [gmode].bwsetis [gtx];	// индекс банка полос пропускания для данного режима
//	int_fast32_t width = getif6bw(gmode, gtx, bwseti_getwide(bwseti));
	int_fast32_t width = 10;
	if (width >= 1000000)
		width = (1000000 - 1);
	int_fast16_t w100 = (width + 50) / 100;
	if (w100 < 10)	// до 1 кГц
		local_snprintf_P(s, ARRAY_SIZE(s), ".%02d", w100 * 10);
	else if (w100 < 100)	// 1 кГц..9 кГц
		local_snprintf_P(s, ARRAY_SIZE(s), "%1d.%1dk", w100 / 10, w100 % 10);
	else	// 10 и более кГц
		local_snprintf_P(s, ARRAY_SIZE(s), "%3dk", w100 / 10);

	return s;
}

#endif /* WITHIF4DSP */

// RX preamplifier
const char * hamradio_get_pre_value(void)
{
#if ! WITHONEATTONEAMP
	return pampmodes [gpamp].label;
#else /* ! WITHONEATTONEAMP */
	return PSTR("");
#endif /* ! WITHONEATTONEAMP */
}

// RX attenuator (or att/pre).
const char * hamradio_get_att_value_P(void)
{
	return attmodes [gatt].label;
}

// RX agc time - 3 символа
const char * hamradio_get_agc3_value(void)
{
#if ! WITHAGCMODENONE
	return agcmodes [gagcmode].label3;
#else /* ! WITHAGCMODENONE */
	return PSTR("");
#endif /* ! WITHAGCMODENONE */
}

// RX agc time - 4 символа
const char * hamradio_get_agc4_value(void)
{
#if ! WITHAGCMODENONE
	return agcmodes [gagcmode].label4;
#else /* ! WITHAGCMODENONE */
	return "";
#endif /* ! WITHAGCMODENONE */
}

#if WITHPOWERLPHP
// HP/LP
const char * hamradio_get_hplp_value_P(void)
{
	return pwrmodes [gpwri].label;
}
#endif /* WITHPOWERLPHP */


///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////
#if WITHUSEDUALWATCH

/* переключение основного/дополнительного приёмника */
static void
uif_key_mainsubrx(void)
{
	param_keyclick(& xmainsubrxmode);		// Левый/правый, A - main RX, B - sub RX
	updateboard();
}

// текущее состояние DUAL WATCH
const char * hamradio_get_mainsubrxmode3_value_P(void)
{
	return mainsubrxmodes [param_getvalue(& xmainsubrxmode)].label;
}


#endif /* WITHUSEDUALWATCH */

///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////
/* переключение шага
	 - не вызывает сохранение состояния диапазона */
//static void
//uif_key_changestep(uint_fast8_t tx)
//{
//while (repeat --)
//	glock = calc_next(glock, 0, 1);
//save_i8(RMT_LOCKMODE_BASE, glock);
//}

///////////////////////////
// обработчики кнопок клавиатуры
//////////////////////////
/* заглушка для клавиш, которые ничего не делают.
*/
//static void
//uif_key_dummy(void)
//{
//}

/* Template - Xxxx */

/*
static void
uif_key_click_xxxx(void)
{

}
*/
///////////////////////////
// были обработчики кнопок клавиатуры
///////////////////////////

// S-METER, SWR-METER, POWER-METER
/* отображение S-метра или SWR-метра на приёме или передаче */
// Функция вызывается из display2.c
void
display2_bars(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
#if WITHBARS
	if (userfsg)
	{
	}
	else if (gtx)
	{
		display2_bars_tx(db, x, y, xspan, yspan, pctx);
	}
	else
	{
		display2_bars_rx(db, x, y, xspan, yspan, pctx);
	}
#endif /* WITHBARS */
}

static std::atomic<uint32_t> counterupdatedfreqs;
//static std::atomic<uint32_t> counterupdatedvolume;

/*
	отсчёт времени по запрещению обновления дисплея при вращении валкодера.
	обновлению s-метра
	обновлению вольтметра
	обновлению режимов работы
	Вызывается из обработчика таймерного прерывания
*/
static void
refreshticker_cb(void * ctx)
{
	// таймер обновления частоты
	{
		const uint_fast32_t t = counterupdatedfreqs;
		if (t != 0)
			counterupdatedfreqs = t - 1;
	}
	// таймер обновления громкости
//	{
//		const uint_fast32_t t = counterupdatedvolume;
//		if (t != 0)
//			counterupdatedvolume = t - 1;
//	}
}

// Проверка разрешения обновления дисплея (индикация частоты).
static uint_fast8_t
refreshenabled_freqs(void)
{
	return counterupdatedfreqs == 0;		/* таймер дошёл до нуля - можно обновлять. */
}

// подтверждение выполненного обновления дисплея (индикация частоты).
static void
refreshperformed_freqs(void)
{
	const uint_fast32_t n = UINTICKS(calcdivround2(1000, gdisplayfreqsfps));	// 50 ms - обновление с частотой 20 герц

	counterupdatedfreqs = n;
}

//// Проверка разрешения обновления громкости.
//static uint_fast8_t
//refreshenabled_volume(void)
//{
//	return counterupdatedvolume == 0;		/* таймер дошёл до нуля - можно обновлять. */
//}
//
//// подтверждение выполненного обновления дисплея (индикация частоты).
//static void
//refreshperformed_volume(void)
//{
//	const uint_fast32_t n = UINTICKS(50);	// 50 ms - обновление с частотой 20 герц
//
//	counterupdatedvolume = n;
//}

static uint_fast8_t processpots(void)
{
	uint_fast8_t changed = 0;
	// +++ получение состояния органов управления */
#if WITHPOTPOWER
	{
		static adcvalholder_t powerstate;
		changed |= FLAGNE_U8_CAT(& gnormalpower, board_getpot_filtered_u8(POTPOWER, WITHPOWERTRIMMIN, WITHPOWERTRIMMAX, & powerstate), CAT_PC_INDEX);	// регулировка мощности
	}
#endif /* WITHPOTPOWER */
#if WITHPOTWPM
	{
		static adcvalholder_t wpmstate;
		changed |= FLAGNE_U8_CAT(& elkeywpm, board_getpot_filtered_u8(POTWPM, CWWPMMIN, CWWPMMAX, & wpmstate), CAT_KS_INDEX);
	}
#endif /* WITHPOTWPM */
#if WITHPOTNFMSQL
	{
		static adcvalholder_t sqlstate;
		changed |= flagne_u8(& gsquelchNFM, board_getpot_filtered_u8(POTNFMSQL, 0, SQUELCHMAX, & sqlstate));
	}
#endif /* WITHPOTNFMSQL */
#if WITHPOTIFGAIN
	{
		static adcvalholder_t ifgainstate;
		changed |= FLAGNE_U16_CAT(& rfgain1, board_getpot_filtered_u16(POTIFGAIN, BOARD_IFGAIN_MIN, BOARD_IFGAIN_MAX, & ifgainstate), CAT_RG_INDEX);	// Параметр для регулировки усиления ПЧ
	}
#endif /* WITHPOTIFGAIN */
#if WITHPOTAFGAIN
	{
		static adcvalholder_t afgainstate;
		changed |= FLAGNE_U16_CAT(& afgain1, board_getpot_filtered_u16(POTAFGAIN, BOARD_AFGAIN_MIN, BOARD_AFGAIN_MAX, & afgainstate), CAT_AG_INDEX);	// Параметр для регулировки уровня на выходе аудио-ЦАП
	}
#endif /* WITHPOTAFGAIN */
#if WITHPBT && WITHPOTPBT
	{
		/* установка gpbtoffset PBTMIN, PBTMAX, midscale = PBTHALF */
		static adcvalholder_t pbtstate;
		changed |= flagne_u16(& gpbtoffset, board_getpot_filtered_u16(POTPBT, PBTMIN, PBTMAX, & pbtstate) / 10 * 10);
	}
#endif /* WITHPBT && WITHPOTPBT */
#if WITHIFSHIFT && WITHPOTIFSHIFT
	{
		/* установка gifshftoffset IFSHIFTTMIN, IFSHIFTMAX, midscale = IFSHIFTHALF */
		static adcvalholder_t ifshiftstate;
		changed |= flagne_u16(& ifshifoffset.value, board_getpot_filtered_u16(POTIFSHIFT, IFSHIFTTMIN, IFSHIFTMAX, & ifshiftstate) / 10 * 10);
	}
#endif /* WITHIFSHIFT && WITHPOTIFSHIFT */
#if WITHPOTNOTCH && WITHNOTCHFREQ
	{
		static adcvalholder_t notchstate;
		changed |= flagne_u16(& gnotchfreq.value, board_getpot_filtered_u16(POTNOTCH, WITHNOTCHFREQMIN, WITHNOTCHFREQMAX, & notchstate) / 50 * 50);	// регулировка частоты NOTCH фильтра
	}
#endif /* WITHPOTNOTCH && WITHNOTCHFREQ */
	// --- конец получения состояния органов управления */
	if (changed != 0)
		updateboard();	/* полная перенастройка (как после смены режима) */
	return changed;
}

static uint_fast8_t processmainloopencoders(uint_fast8_t inmenu, inputevent_t * ev)
{
	const uint_fast8_t bi = getbankindex_ab(0);
	const uint_fast8_t submode = getsubmode(bi);
	const uint_fast8_t mode = submodes [submode].mode;
	uint_fast8_t changed = 0;
	// +++ получение состояния органов управления */
#if WITHENCODER_1F
	{
		const int_least16_t delta = event_getRotate_LoRes(& ev->encF1, BOARD_ENC1F_DIVIDE);
		if (delta)
			bring_enc1f();
		switch (enc1f_sel)
		{
		default:
			break;
		case 0:
			/* установка громкости */
			if (delta == 0)
				break;
			changed |= encoder_flagne(& xafgain1, delta, CATINDEX(CAT_AG_INDEX), bring_afvolume);
//			if (refreshenabled_volume())
//			{
//				refreshperformed_volume();
//				changed |= encoder_flagne(& xafgain1, delta, CATINDEX(CAT_AG_INDEX), bring_afvolume);
//			}
//			else
//			{
//				event_pushback_LoRes(& ev->encF1, delta, BOARD_ENC1F_DIVIDE);
//			}
			break;
		case 1:
			if (delta == 0)
				break;
			/* установка IF GAIN */
			changed |= encoder_flagne(& xrfgain1, delta, CATINDEX(CAT_RG_INDEX), bring_rfvolume);
			break;
		}
	}
#endif /* WITHENCODER_1F */
#if WITHENCODER_2F
	{
		const int_least16_t delta = event_getRotate_LoRes(& ev->encF2, BOARD_ENC2F_DIVIDE);
		if (delta)
			bring_enc2f();
		switch (enc2f_sel)
		{
		default:
			if (delta == 0)
				break;
			break;
		case 0:
			if (delta == 0)
				break;
			break;
		}
	}
#endif /* WITHENCODER_2F */
#if WITHENCODER_3F
	/* редактирование параметра в middle bar */
	if (! inmenu)
	{
		unsigned nitems;
		const unsigned apos = gmiddlepos [mode];
		const struct paramdefdef * const pd = mdt [mode].middlemenu(& nitems) [gmiddlepos [mode]];

		int_least16_t delta = event_getRotate_LoRes(& ev->encF3, BOARD_ENC3F_DIVIDE);
		changed |= param_rotate(pd, delta);	// модификация и сохранение параметра
		if (delta)
			bring_enc3f();
//		switch (enc3f_sel)
//		{
//		default:
//			break;
//		case 0:
//			while (delta < 0)
//			{
//				uif_key_click_banddown();
//				++ delta;
//				changed = 1;
//			}
//			while (delta > 0)
//			{
//				uif_key_click_bandup();
//				-- delta;
//				changed = 1;
//			}
//			break;
//		}
	}
#endif /* WITHENCODER_3F */
#if WITHENCODER_4F
	/* перемещение по middle bar */
	if (! inmenu)
	{
		const int_least16_t delta = event_getRotate_LoRes(& ev->encF4, BOARD_ENC4F_DIVIDE);
		if (delta)
		{
			unsigned middlerowsize;
			mdt [mode].middlemenu(& middlerowsize);
			gmiddlepos [mode] = calc_delta(gmiddlepos [mode], 0, middlerowsize - 1, delta);
			save_i8(RMT_MIDDLEMENUPOS_BASE(mode), gmiddlepos [mode]);
			changed = 1;
		}
		if (delta)
			bring_enc4f();
//		switch (enc4f_sel)
//		{
//		default:
//			break;
//		case 0:
//			break;
//		}
	}
#endif /* WITHENCODER_4F */

	// --- конец получения состояния органов управления */
	if (changed != 0)
		updateboard();	/* полная перенастройка (как после смены режима) */
	return changed;
}

dctx_t * display2_getcontext(void)
{
#if WITHDIRECTFREQENER
	static editfreq2_t ef;
	static dctx_t ctx;

	ef.freq = editfreq;
	ef.blinkpos = blinkpos;
	ef.blinkstate = blinkstate;

	ctx.type = DCTX_FREQ;
	ctx.pv = & ef;
	return editfreqmode ? & ctx : NULL;
#else /*  WITHDIRECTFREQENER */
	return NULL;
#endif /* WITHDIRECTFREQENER */
}

static void doadcmirror(void)
{
	/* быстро меняющиеся значения с частым опорсом */
	/* +++ переписываем значения из возможно внешних АЦП в кеш значений */
#if WITHSWRMTR && WITHTX
	if (1)
	{
		// Версия из тюнера
		adcvalholder_t r;
		const adcvalholder_t f = board_getswrpair_filtered_tuner(& r, swrcalibr);

		// обновить кеш данных для дисплея
		board_adc_store_data(PWRMRRIX, f);
		board_adc_store_data(FWDMRRIX, f);
		board_adc_store_data(REFMRRIX, r);
	}
	else
	{
		const adcvalholder_t f = board_getadc_unfiltered_truevalue(FWD);
		const adcvalholder_t r = board_getadc_unfiltered_truevalue(REF);
		board_adc_store_data(PWRMRRIX, f);
		board_adc_store_data(FWDMRRIX, f);
		board_adc_store_data(REFMRRIX, r);
	}
#endif /* WITHSWRMTR */
#if WITHCURRLEVEL2
	board_adc_store_data(PASENSEMRRIX2, board_getadc_unfiltered_truevalue(PASENSEIX2));
	board_adc_store_data(PAREFERMRRIX2, board_getadc_unfiltered_truevalue(PAREFERIX2));
#elif WITHCURRLEVEL
	board_adc_store_data(PASENSEMRRIX, board_getadc_unfiltered_truevalue(PASENSEIX));
#endif /* WITHCURRLEVEL */
	/* --- переписываем значения из возможно внешних АЦП в кеш значений */

#if WITHAUTOTUNER && 0
		//if (gtx && ! txreq_getreqautotune(& txreqst0))
		{
			adcvalholder_t r;
			adcvalholder_t f;
			const uint_fast16_t swr = tuner_get_swr("main_loop", TUS_SWRMAX, & r, & f);

		}
#endif /* WITHAUTOTUNER */
		/* медленно меняющиеся значения с редким опорсом */
		/* +++ переписываем значения из возможно внешних АЦП в кеш значений */
	#if WITHTHERMOLEVEL2
		board_adc_store_data(XTHERMOREFMRRIX, board_getadc_unfiltered_truevalue(XTHERMOREFIX));
		board_adc_store_data(XTHERMOMRRIX, board_getadc_unfiltered_truevalue(XTHERMOIX));
	#elif WITHTHERMOLEVEL
		// ST LM235Z test values:
		// 2.98 V @ 25C
		// 2.98 / 5.7 = 0.5223V at ADC input
		// 0.5223V * 4095 / 3.3 = 648.75
		// test value = 649, expected temperature approx 25C

		board_adc_store_data(XTHERMOMRRIX, board_getadc_unfiltered_truevalue(XTHERMOIX));
	#endif /* WITHTHERMOLEVEL */
	#if WITHVOLTLEVEL
		board_adc_store_data(VOLTMRRIX, board_getadc_unfiltered_truevalue(VOLTSOURCE));
	#endif /* WITHVOLTLEVEL */
	/* --- переписываем значения из возможно внешних АЦП в кеш значений */
}
// обновимть изображение частоты на дисплее
static void
//NOINLINEAT
display_redrawfreqstimed(
	uint_fast8_t immed	// Безусловная перерисовка изображения
	)
{
	if (immed || refreshenabled_freqs())
	{
		//display2_needupdate();	/* обновление показания частоты */
		refreshperformed_freqs();
	}
}

// *************************
// CAT sequence parser

// omnirig default settings: PTT control: RTS, CW control: DTR
// HXCat use same default settings


#if WITHCAT

static uint_fast8_t morsefill;	/* индекс буфера, заполняемого в данный момент. Противоположгый передаётся. */

static uint_fast8_t inpmorselength [2];
static uint_fast8_t sendmorsepos [2];

#if WITHELKEY
	static void cat_set_kyanswer(uint_fast8_t force);
	static uint_fast8_t cathasparamerror;
	static unsigned char morsestring [2][25];
#endif /* WITHELKEY */

static uint_fast8_t catstatein = CATSTATE_HALTED;

static std::atomic<uint_fast8_t> catstateout(CATSTATEO_HALTED);
static volatile const char * catsendptr;
static volatile uint_fast8_t catsendcount;

//A communication error occurred, such as an overrun or framing error during a serial data transmission.
//static const char processingcmd [2] = "E;";	// ответ на команду которая ещё выполняется

static uint_fast8_t
cat_getstateout(void)
{
	return catstateout;
}

/* вызывается из обработчика прерываний */
void cat2_sendchar(void * ctx)
{
	switch (catstateout)
	{
	case CATSTATEO_HALTED:
		//HARDWARE_CAT_ENABLETX(0);
		break;

	case CATSTATEO_WAITSENDREPLAY:
		if (catsendcount --)
			HARDWARE_CAT_TX(ctx, * catsendptr ++);
		else
		{
			HARDWARE_CAT_ENABLETX(0);
			catstateout = CATSTATEO_SENDREADY;
		}
		break;

	case CATSTATEO_SENDREADY:
		// обработка ситуации после USB DISCONNECT
		HARDWARE_CAT_ENABLETX(0);
		break;

	default:
		TP();
		// not need, but for safety.
		//HARDWARE_CAT_ENABLETX(0);
		break;
	}
}

static uint_fast8_t cat_answer_ready_uart(void)
{
	return 1;
}

static IRQLSPINLOCK_t catsyslock = IRQLSPINLOCK_INIT;
static IRQLSPINLOCK_t usbsyslock = IRQLSPINLOCK_INIT;

#if WITHUSBHW && WITHUSBCDCACM
static uint_fast8_t cat_answer_ready_cdcacm(void)
{
	uint_fast8_t f;
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& usbsyslock, & oldIrql, USBSYS_IRQL);
	f = usbd_cdc_ready();
	IRQLSPIN_UNLOCK(& usbsyslock, oldIrql);
	return f;
}
#endif /* WITHUSBHW && WITHUSBCDCACM */

#if WITHUSBHW && WITHUSBCDCACM
static void
cat_answervariable_cdcacm(const char * p, uint_fast8_t len)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& catsyslock, & oldIrql, CATSYS_IRQL);
	if (catstateout != CATSTATEO_SENDREADY)
	{
		// Сейчас ещё передается сообщение - новое игнорируем.
		// Добавлено для поддержки отладки при работающем CAT
		IRQLSPIN_UNLOCK(& catsyslock, oldIrql);
		return;
	}
	usbd_cdc_send(p, len);
	catstateout = CATSTATEO_SENDREADY;
	IRQLSPIN_UNLOCK(& catsyslock, oldIrql);
}
#endif /* WITHUSBHW && WITHUSBCDCACM */

static void
cat_answervariable_uart(const char * p, uint_fast8_t len)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& catsyslock, & oldIrql, CATSYS_IRQL);
	if (catstateout != CATSTATEO_SENDREADY)
	{
		// Сейчас ещё передается сообщение - новое игнорируем.
		// Добавлено для поддержки отладки при работающем CAT
		IRQLSPIN_UNLOCK(& catsyslock, oldIrql);
		return;
	}
	if ((catsendcount = len) != 0)
	{
		catsendptr = p;
		catstateout = CATSTATEO_WAITSENDREPLAY;
		HARDWARE_CAT_ENABLETX(1);
	}
	else
	{
		//catstateout = CATSTATEO_SENDREADY;
		HARDWARE_CAT_ENABLETX(0);
	}
	IRQLSPIN_UNLOCK(& catsyslock, oldIrql);
}


void btspp_parsechar(uint_fast8_t c)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& catsyslock, & oldIrql, CATSYS_IRQL);
	cat2_parsechar(c);
	IRQLSPIN_UNLOCK(& catsyslock, oldIrql);
}

// user-mode function
static uint_fast8_t
cat_answer_ready(void)
{
#if WITHUSBHW && WITHUSBCDCACM && WITHCAT_MUX
	switch (board_get_catmux())
	{
	case BOARD_CATMUX_USBCDC:
		return cat_answer_ready_cdcacm();
	case BOARD_CATMUX_BTSPP:
		return cat_answer_ready_btspp();
	case BOARD_CATMUX_DIN8:
		return cat_answer_ready_uart();
	default:
		return 1;
	}
#elif WITHUSBHW && WITHUSBCDCACM && WITHCAT_CDC
	return cat_answer_ready_cdcacm();
#else /* WITHUSBHW && WITHUSBCDCACM && WITHCAT_CDC */
	return cat_answer_ready_uart();
#endif /* WITHUSBHW && WITHUSBCDCACM && WITHCAT_CDC */
}

// Вызов из user-mode программы
static void
cat_answervariable(const char * p, uint_fast8_t len)
{
	//PRINTF(PSTR("cat_answervariable: '%*.*s'\n"), len, len, p);

#if WITHUSBHW && WITHUSBCDCACM && WITHCAT_MUX
	switch (board_get_catmux())
	{
	case BOARD_CATMUX_USBCDC:
		cat_answervariable_cdcacm(p, len);
		break;
	case BOARD_CATMUX_BTSPP:
		cat_answervariable_btspp(p, len);
		break;
	case BOARD_CATMUX_DIN8:
		cat_answervariable_uart(p, len);
		break;
	default:
		break;
	}
#elif WITHUSBHW && WITHUSBCDCACM && WITHCAT_CDC
	cat_answervariable_cdcacm(p, len);
#else
	cat_answervariable_uart(p, len);
#endif /* WITHUSBHW && WITHUSBCDCACM && WITHCAT_CDC */
}


// *************************

//#define CAT_ASKBUFF_SIZE (43 + 28)
#define CAT_ASKBUFF_SIZE (43)

static char cat_ask_buffer [CAT_ASKBUFF_SIZE];

static void
//NOINLINEAT
cat_answer(uint_fast8_t len)
{
	cat_answervariable(cat_ask_buffer, len);
}

#if WITHELKEY

/* переключение на следующий буфер, если можно */
static void
morseswitchnext(void)
{
	const uint_fast8_t ms = ! morsefill;	// ms: morse sent
	if (sendmorsepos [ms] == inpmorselength [ms])
	{
		// закончили передавать
		if (sendmorsepos [morsefill] < inpmorselength [morsefill])
		{
			// заполнен приёмный буфер
			sendmorsepos [ms] = inpmorselength [ms] = 0;
			morsefill = ! morsefill;
		}
	}

}

#endif /* WITHELKEY */

static uint_fast8_t
//NOINLINEAT
ascii_toupper(uint_fast8_t c)
{
	return toupper((unsigned char) c);
}

#if WITHELKEY

/* todo: переделать на обновление параметра KY в ответе, если KY запрошен. */
static void
//NOINLINEAT
cat_set_kyanswer(uint_fast8_t force)
{
	// проверка что все буферы заполнены
	//const uint_fast8_t f = (sendmorsepos [morsefill] != inpmorselength [morsefill]);
	const uint_fast8_t f = sendmorsepos [morsefill] < inpmorselength [morsefill];
	// Это была команда KY; (без параметров) - запрос состояния
	// KEYER BUFFER STATUS: 0 - BUFFER SPACE AVALIABLE, 1 - BUFFER FULL
	if (force)
		cat_answer_map [f ? CAT_KY1_INDEX : CAT_KY0_INDEX] = 1;
}

/* вызывается из обработчика прерываний */
/* Получить следующий символ для передачи (только верхний регистр) */
static char cat_getnextcw(void)
{
	morseswitchnext();	/* переключение на следующий буфер, если можно */
	cat_set_kyanswer(0);	// KEYER BUFFER STATUS: 0 - BUFFER SPACE AVALIABLE, 1 - BUFFER FULL
	const uint_fast8_t ms = ! morsefill;	// ms: morse sent
	while (sendmorsepos [ms] < inpmorselength [ms])
	{
		const char c = morsestring [ms] [sendmorsepos [ms] ++];
		if (c == ' ' && (sendmorsepos [ms] < inpmorselength [ms]) && morsestring [ms] [sendmorsepos [ms]] == ' ')
			continue;
		return c;
	}
	return '\0';
}

#endif /* WITHELKEY */

/* вызывается из обработчика прерываний */
// произошла потеря символа (символов) при получении данных с CAT компорта
void cat2_rxoverflow(void)
{

}

/* вызывается из обработчика прерываний */
// произошёл разрыв связи при работе по USB CDC
void cat2_disconnect(void)
{
	uint_fast8_t i;
	for (i = 0; i < (sizeof cat_answer_map / sizeof cat_answer_map [0]); ++ i)
		cat_answer_map [i] = 0;

#if WITHTX
	sendmorsepos  [0] =
	inpmorselength [0]=
	sendmorsepos  [1] =
	inpmorselength [1] = 0;
#endif /* WITHTX */

	aistate = 0; /* Power-up state of AI mode = 0 (TS-590). */
	catstatein = CATSTATE_WAITCOMMAND1;
	catstateout = CATSTATEO_SENDREADY;
}


static uint_fast8_t local_isdigit(char c)
{
	//return isdigit((unsigned char) c) != 0;
	return c >= '0' && c <= '9';
}

/* вызывается из обработчика прерываний */
void cat2_parsechar(uint_fast8_t c)
{
	static uint_fast8_t catcommand1;
	static uint_fast8_t catcommand2;
	static uint_fast8_t cathasparam;
	static uint_fast8_t catp [CATPCOUNTSIZE];
	static uint_fast8_t catpcount;

   // PRINTF(PSTR("c=%02x, catstatein=%d, c1=%02X, c2=%02X\n"), c, catstatein, catcommand1, catcommand2);
	switch (catstatein)
	{
	case CATSTATE_HALTED:
		break;

	case CATSTATE_WAITCOMMAND1:
		if ((catcommand1 = ascii_toupper(c)) != ';')
			catstatein = CATSTATE_WAITCOMMAND2;
		break;

	case CATSTATE_WAITCOMMAND2:
		if ((catcommand2 = ascii_toupper(c)) == ';')
		{
			cat_answer_map [CAT_BADCOMMAND_INDEX] = 1;	// второй символ не буква, а ';' - преждевременный конец команды
			catstatein = CATSTATE_WAITCOMMAND1;
		}
#if WITHELKEY
		else if (catcommand1 == 'K' && catcommand2 == 'Y')
		{
			catstatein = CATSTATE_WAITMORSE;
			cathasparam = 0;	// строка была - 1, иначе - 0
			catpcount = 0;
			cathasparamerror = 0;
		}
#endif	/* WITHELKEY */
		else
		{
			catstatein = CATSTATE_WAITPARAM;
			cathasparam = 0;
			//cathasparamerror = 0;
			catpcount = 0;
		}
		break;

	case CATSTATE_WAITPARAM:
		//if (c >= '0' && c <= '9')
		if (local_isdigit((unsigned char) c))
		{
			cathasparam = 1;
			if (catpcount < (sizeof catp / sizeof catp [0]))
				catp [catpcount ++] = c;
		}
		else if (c == ';')
		{
			//catstatein = CATSTATE_READYCOMMAND;		// команда готова для интерпретации параметров

			uint8_t * buff;
			if (takemsgbufferfree(& buff) != 0)
			{
				uint_fast8_t i;
				// check MSGBUFFERSIZE8 valie
				buff [0] = catcommand1;
				buff [1] = catcommand2;
				buff [2] = cathasparam;

				buff [8] = catpcount;
				for (i = 0; i < catpcount; ++ i)
					buff [9 + i] = catp [i];

				placesemsgbuffer(MSGT_CAT, buff);
			}
			catstatein = CATSTATE_WAITCOMMAND1;	/* в user-mode нечего делать - ответ не формируем  */
		}
		else
		{
			;// остальные символы игнорируются //
		}
		break;

#if WITHELKEY
	case CATSTATE_WAITMORSE:
		if (c == '\0')	// такой симвоь недопустим
		{
			cathasparamerror = 1;
		}
		else if (c != ';')
		{
			cathasparam = 1;	// признак наличия символов для передачи
			if (catpcount == 0 && c == '0')
			{
				sendmorsepos [0] = inpmorselength [0] = 0;	// очистить буфер передачи морзе.
				sendmorsepos [1] = inpmorselength [1] = 0;	// очистить буфер передачи морзе.
				cat_set_kyanswer(0);
			}

			if (sendmorsepos [morsefill] < inpmorselength [morsefill])
			{
				// Данные ещё не переданы - не портим буфер. Будет передана ошибка.
				cathasparamerror = 1;
			}
			else if (catpcount < (sizeof morsestring [morsefill] / sizeof morsestring [morsefill][0]))
			{
				/* запоминаем очередной символ для передачи */
				morsestring [morsefill] [catpcount ++] = ascii_toupper(c);
			}
			else
			{
				// слишком длинная строка для передачи
				cathasparamerror = 1;
			}
		}
		else
		{
			// Пришёл символ завершения команды.
			// Были ли ошибки?
			if (cathasparamerror != 0)
			{
				// были символы за кодом команды KY, но они небыли приняты из-за ошибок.
				// ошибка - это незакончившаяся ранее передача буфера.
				// Ещё не обработали старые данные для передачи
				cat_answer_map [CAT_BADCOMMAND_INDEX] = 1;
			}
			else if (cathasparam != 0)
			{
				/* более одного символа (включая пробелы в конце) - первый игнорируется */
				if (catpcount > 1)
				{
					/* устанавливаем индексы для передачи */
					inpmorselength  [morsefill] = catpcount;
					sendmorsepos  [morsefill] = 1;	/* Первый символ в буфере - пробел */

					morseswitchnext();	/* переключение на следующий буфер, если можно */
					cat_set_kyanswer(0);	// KEYER BUFFER STATUS: 0 - BUFFER SPACE AVALIABLE, 1 - BUFFER FULL
				}
			}
			else
			{
				cat_set_kyanswer(1);	// KEYER BUFFER STATUS: 0 - BUFFER SPACE AVALIABLE, 1 - BUFFER FULL
			}
			catstatein = CATSTATE_WAITCOMMAND1;	/* в user-mode нечего делать - ответ не формируем  */
		}
		break;
#endif /* WITHELKEY */
	}
}

#if WITHTX && WITHAUTOTUNER
static void acanswer(uint_fast8_t arg)
{
	static const char fmt_3 [] =
		"AC"			// 2 characters - status information code
		"%d"		// P1 1 characters - 0: RX-AT THRU 1: RX-AT IN
		"%d"		// P2 1 characters - 0: TX-AT THRU 1: TX-AT IN
		"%d"		// P3 1 characters - 0: Stop Tuning (Set)/ Tuning is stopped (Answer) 1: Start Tuning (Set)/ Tuning is active (Answer)
		";";				// 1 char - line terminator
	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_3,
		(int) tunerwork != 0,
		(int) tunerwork != 0,
		(int) txreq_getreqautotune(& txreqst0) != 0
		);
	cat_answer(len);
}

#endif /* WITHTX && WITHAUTOTUNER */
static void idanswer(uint_fast8_t arg)
{
	static const char fmt_1 [] =
		"ID"			// 2 characters - status information code
		"%03d"		// P1 3 characters - model
		";";				// 1 char - line terminator
	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) 21	// 021 - code of TS-590S model
		);
	cat_answer(len);
}

static void fvanswer(uint_fast8_t arg)
{
	static const char fmt_0 [] =
		"FV"			// 2 characters - status information code
		"1.00"			// P1 4 characters - For example, for firmware version ..00, it reads “FV..00;.
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_0
		);
	cat_answer(len);
}

static void daanswer(uint_fast8_t arg)
{
	static const char fmt_1 [] =
		"DA"			// 2 characters - status information code
		"%1d"			// P1 1 characters -  0: DATA mode OFF, 1: DATA mode ON,
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) 0	// 0/1: data mode on/off
		);
	cat_answer(len);
}

static void faanswer(uint_fast8_t arg)
{
	const uint_fast8_t bi = getbankindex_ab(0);	/* vfo A bank index */
	static const char fmt_1 [] =
		"FA"				// 2 characters - status information code
		"%011ld"			// P1 11 characters - freq
		";";				// 1 char - line terminator
	// answer VFO A frequency
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(signed long) gfreqs [bi]
		);
	cat_answer(len);
}

static void fbanswer(uint_fast8_t arg)
{
	const uint_fast8_t bi = getbankindex_ab(1);	/* vfo B bank index */
	static const char fmt_1 [] =
		"FB"				// 2 characters - status information code
		"%011ld"			// P1 11 characters - freq
		";";				// 1 char - line terminator

	// answer VFO B frequency
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(signed long) gfreqs [bi]
		);
	cat_answer(len);
}

static void spanswer(uint_fast8_t arg)
{
	//const uint_fast8_t bi = getbankindex_ab(1);	/* vfo B bank index */
	static const char fmt_1 [] =
		"SP"				// 2 characters - status information code
		"%01d"			// P1 1 characters - 0: VFO A, 1: VFO B, 2: Memory Channel
		";";				// 1 char - line terminator

	// answer VFO B frequency
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) (gsplitmode != VFOMODES_VFOINIT)
		);
	cat_answer(len);
}



static void franswer(uint_fast8_t arg)
{
	//const uint_fast8_t bi = getbankindex_ab(1);	/* vfo B bank index */
	static const char fmt_1 [] =
		"FR"				// 2 characters - status information code
		"%01d"			// P1 1 characters - 0: VFO A, 1: VFO B, 2: Memory Channel
		";";				// 1 char - line terminator

	// answer VFO B frequency
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) gvfosplit [0]	// At index 0: RX VFO A or B, at index 1: TX VFO A or B
		);
	cat_answer(len);
}

static void ftanswer(uint_fast8_t arg)
{
	//const uint_fast8_t bi = getbankindex_ab(1);	/* vfo B bank index */
	static const char fmt_1 [] =
		"FT"				// 2 characters - status information code
		"%01d"			// P1 1 characters - 0: VFO A, 1: VFO B, 2: Memory Channel
		";";				// 1 char - line terminator

	// answer VFO B frequency
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) gvfosplit [1]	// At index 0: RX VFO A or B, at index 1: TX VFO A or B
		);
	cat_answer(len);
}

static void mdanswer(uint_fast8_t arg)
{
	const uint_fast8_t bi = getbankindex_ab(0);	/* vfo B bank index */
	static const char fmt_1 [] =
		"MD"				// 2 characters - status information code
		"%1d"				// 1 char - mode
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) submodes [getsubmode(bi)].qcatmodecode
		);
	cat_answer(len);
}

static void fwanswer(uint_fast8_t arg)
{
	static const char fmt_1 [] =
		"FW"			// 2 characters - status information code
		"%04d"			// P42 4 characters - DSP filtering bandwidth.
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) getkenwoodfw(gsubmode, gfi) // полоса пропускания в герцах или код полосы пропускания
		);
	cat_answer(len);
}

#if WITHIF4DSP
static void zyanswer(uint_fast8_t arg)
{
	//ZYmSLL
	static const char fmt_3 [] =
		"ZY"			// 2 characters - status information code
		"%1d"			// 1 char - mode
		"%1d"			// state
		"%02d"			// level
		";";			// 1 char - line terminator

	const uint_fast8_t submode = findkenwoodsubmode(arg, SUBMODE_USB);	/* поиск по кенвудовскому номеру */
	const uint_fast8_t mode = submodes [submode].mode;

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_3,
		(int) arg,
		(int) gnoisereducts [mode],
		(int) gnoisereductvl
		);
	cat_answer(len);
}

static void zzanswer(uint_fast8_t arg)
{
	//ZZmLLLLUUUUSSSS
	static const char fmt_4 [] =
		"ZZ"			// 2 characters - status information code
		"%1d"			// 1 char - mode
		"%04d"			// right
		"%04d"			// left
		"%03d"			// slope
		";";			// 1 char - line terminator

	const uint_fast8_t submode = findkenwoodsubmode(arg, SUBMODE_USB);	/* поиск по кенвудовскому номеру */
	const struct modetempl * const pmodet = getmodetempl(submode);
	const uint_fast8_t bwseti = pmodet->bwsetis [0];	// индекс банка полос пропускания для данного режима на приеме
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_4,
		(int) arg,
		(int) p->left10_width10,
		(int) p->right100,
		(int) p->afresponce
		);
	cat_answer(len);
}
#endif

#if WITHIF4DSP

static void aganswer(uint_fast8_t arg)
{
	// AF gain
	static const char fmt_2 [] =
		"AG"			// 2 characters - status information code
		"%1d"			// P1 always 0
		"%03d"			// P2 0..255 Squelch level
		";";				// 1 char - line terminator
	const int p2 = (param_getvalue(& xafgain1) - BOARD_AFGAIN_MIN) * 255 / (BOARD_AFGAIN_MAX - BOARD_AFGAIN_MIN);

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_2,
		(int) arg,
		(int) p2
		);
	cat_answer(len);
}

static void rganswer(uint_fast8_t arg)
{
	// RF (IF) gain
	static const char fmt_1 [] =
		"RG"			// 2 characters - status information code
		"%03d"			// P1 0..255 RF Gain status
		";";				// 1 char - line terminator
	const int p1 = ((param_getvalue(& xrfgain1) - BOARD_IFGAIN_MIN) * 255 / (BOARD_IFGAIN_MAX - BOARD_IFGAIN_MIN));

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) p1
		);
	cat_answer(len);
}

static void sqanswer(uint_fast8_t arg)
{
	static const char fmt_2 [] =
		"SQ"			// 2 characters - status information code
		"%1d"			// P1 always 0
		"%03d"			// P2 0..255 Squelch level
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_2,
		(int) arg,
		(int) gsquelch.value
		);
	cat_answer(len);
}

static void nranswer(uint_fast8_t arg)
{
	static const char fmt_1 [] =
		"NR"			// 2 characters - status information code
		"%1d"			// P1 NR0: nr off, NR1: nr1 on NR2: nr2 on
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) (gnoisereducts [gmode] != 0 ? 2 : 0)
		);
	cat_answer(len);
}
#endif /* WITHIF4DSP */

#if WITHPOWERTRIM && WITHTX
static void pcanswer(uint_fast8_t arg)
{
	static const char fmt_1 [] =
		"PC"			// 2 characters - status information code
		"%03d"			// P1 005..100: SSB/CW/FM/FSK, 25: AM
		";";				// 1 char - line terminator

	// answer mode
	// Нормирование значений WITHPOWERTRIMMIN..WITHPOWERTRIMMAX к диапазону Kenwood CAT
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) ((param_getvalue(& xgnormalpower) - WITHPOWERTRIMMIN) * 95 / (WITHPOWERTRIMMAX - WITHPOWERTRIMMIN) + 5)
		);
	cat_answer(len);
}

#endif /* WITHPOWERTRIM && WITHTX */

#if WITHELKEY

static void ky0answer(uint_fast8_t arg)
{
	static const char fmt_1 [] =
		"KY"			// 2 characters - status information code
		"%c"			// P44 1 character - KEYER BUFFER STATUS: 0 - BUFFER SPACE AVALIABLE, 1 - BUFFER FULL
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(char) '0'
		);
	cat_answer(len);
}

static void ky1answer(uint_fast8_t arg)
{
	static const char fmt_1 [] =
		"KY"			// 2 characters - status information code
		"%c"			// P44 1 character - KEYER BUFFER STATUS: 0 - BUFFER SPACE AVALIABLE, 1 - BUFFER FULL
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(char) '1'
		);
	cat_answer(len);
}

static void ksanswer(uint_fast8_t arg)
{
	static const char fmt_1 [] =
		"KS"			// 2 characters - status information code
		"%03d"			// P42 3 characters - keyer speed in WPM
		";";				// 1 char - line terminator

	// keyer speed information
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) elkeywpm.value
		);
	cat_answer(len);
}
#endif /* WITHELKEY */

static void txanswer(uint_fast8_t arg)
{
	static const char fmt_1 [] =
		"TX"			// 2 characters - status information code
		"%01d"			// P1 1 character - TX reason
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) (getactualtune() ? 2 : 0)
		);
	cat_answer(len);
}

static void rxanswer(uint_fast8_t arg)
{
	static const char fmt_0 [] =
		"RX"			// 2 characters - status information code
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_0
		);
	cat_answer(len);
}

static void aianswer(uint_fast8_t arg)
{
	static const char fmt_1 [] =
		"AI"			// 2 characters - status information code
		"%1d"			// 1 char - mode
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) (aistate ? 2 : 0)
		);
	cat_answer(len);
}

static void psanswer(uint_fast8_t arg)
{
	static const char fmt_1 [] =
		"PS"			// 2 characters - status information code
		"%01d"			// P1 1 characters - POWER ON/OFF
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) 1
		);
	cat_answer(len);
}

static uint_fast8_t
scaletopointssmeter(void)
{
#if WITHBARS
	int_fast16_t tracemaxi10;
	int_fast16_t rssi10 = dsp_rssi10(& tracemaxi10, 0);	/* получить значение уровня сигнала для s-метра в 0.1 дБмВт */
	/* точки на шкале s-метра, к которым надо привязать измеренное значение */
	static const int32_t smetervalues [] =
	{
		- 1090,	// S3 level -109.0 dBm
		- 730,	// S9 level -73.0 dBm
		- 130,	// S9+60 level -13.0 dBm
	};

	/* Значения углов на индикаторе */
	const int32_t smeterpoints [ARRAY_SIZE(smetervalues)] =
	{
		0,	// S3 level -109.0 dBm
		15,	// S9 level -73.0 dBm
		30,	// S9+60 level -13.0 dBm
	};
	return approximate(smetervalues, smeterpoints, ARRAY_SIZE(smetervalues), tracemaxi10);
#else
	return 0;
#endif
}

#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)

//static int swrsim;
// SWR report
// 0000 ~ 0030: Meter value in dots
static uint_fast8_t kenwoodswrmeter(void)
{
	//return swrsim;
	// tested with ARCP950.
	// 0: SWR=1.0, 5: SWR=1.3,  7: SWR=1.5, 10: SWR=1.8, 12: SWR=2, 15: SWR=3.0, 22: SWR=4.0
	static const uint8_t swrmap [] =
	{
			0, 1, 2, 5, 6, 7, 8, 9, 10, 11,				// measured SWR >= 1.0
			12, 12, 12, 13, 13, 13, 14, 14, 14, 14,		// measured SWR >= 2.0
			15, 15, 16, 15, 17, 15, 18, 18, 19, 19,		// measured swr >= 3.0
			20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 	// measured swr >= 4.0
			30											// measured swr >= 5.0
	};
	enum { N = ARRAY_SIZE(swrmap) - 1 };	// максимальный индекс для получения данных из таблицы
	//const uint_fast8_t pathi = 0;	// A or B path
	//enum { FS = SWRMIN * 15 / 10 };	// swr=1.0..4.0
	adcvalholder_t r;
	const adcvalholder_t f = board_getswrmeter_cached(& r, swrcalibr);
	//const uint_fast16_t fullscale = FS - SWRMIN;
	uint_fast16_t swr10;		// рассчитанное  значение
	if (f < minforward)
		swr10 = 0;	// SWR=1
	else if (f <= r)
		swr10 = N;		// SWR is infinite
	else
		swr10 = (f + r) * 10 / (f - r) - 10;	// точность 0.1
	// v = 10..40 for swr 1..4
	// swr10 = 0..30 for swr 1..4
	if (swr10 > N)
		swr10 = N;
	return swrmap [swr10];
}

// COMP report
// 0000 ~ 0030: Meter value in dots
static uint_fast8_t kenwoodcompmeter(void)
{
	return 0;
}

// ALC report
// 0000 ~ 0030: Meter value in dots
static uint_fast8_t kenwoodalcmeter(void)
{
	return 0;
}

// RF (power)
// 0000 ~ 0030: Meter value in dots
static uint_fast8_t kenwoodpowermeter(void)
{
	uint_fast8_t pwrtrace;
	uint_fast8_t pwr = board_getpwrmeter(& pwrtrace);

	if (pwrtrace > maxpwrcali)
		pwrtrace = maxpwrcali;

	return pwrtrace * 30 / maxpwrcali;
}

#else /* WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) */

// RF (power)
// 0000 ~ 0030: Meter value in dots
static uint_fast8_t kenwoodpowermeter(void)
{
	return 0;
}

#endif /* WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) */

// The SM command reads the S-meter during reception and the RF (power) meter during transmission.
static void sm0answer(uint_fast8_t arg)
{
	// s-meter state answer
	static const char fmt0_2 [] =
		"SM"			// 2 characters - status information code
		"%1d"				// 1 char - Always 0
		"%04d"				// 4 chars - s-meter points (0000..0030)
		";";				// 1 char - line terminator

	const int p1 = 0;

	const int p2 = gtx ? kenwoodpowermeter() : scaletopointssmeter();

	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt0_2,
		p1,
		p2
		);
	cat_answer(len);

}

static void sm9answer(uint_fast8_t arg)
{
	// s-meter state answer
	// код SM9 введен для получения "сырого" уровня.
	static const char fmt9_1 [] =
		"SM"			// 2 characters - status information code
		"9"				// 1 char - Always 9
		"%+d"				// level in dBm
		";";				// 1 char - line terminator

	int_fast16_t tracemaxi10;
	int_fast16_t rssi10 = dsp_rssi10(& tracemaxi10, 0);	/* получить значение уровня сигнала для s-метра в 0.1 дБмВт */

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt9_1,
		(int) (tracemaxi10 / 10)
		);
	cat_answer(len);

}

static void raanswer(uint_fast8_t arg)
{
	// RX attenuator
	static const char fmt_1 [] =
		"RA"				// 2 characters - status information code
		"%02d"				// 2 chars - 00: ATT OFF 01: ATT ON
		"00"				// 2 chars - 00: Always 00
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) gatt

		);
	cat_answer(len);
}

static void paanswer(uint_fast8_t arg)
{
#if ! WITHONEATTONEAMP
	uint_fast8_t v = (gpamp != 0);
#else /* ! WITHONEATTONEAMP */
	enum { v = 0 };
#endif /* ! WITHONEATTONEAMP */

	// RX preamplifier
	static const char fmt_1 [] =
		"PA"				// 2 characters - status information code
		"%01d"				// 1 chars - 0: Pre-amp OFF, 1: : Pre-amp ON
		"0"					// 1 chars - Always 0
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) v

		);
	cat_answer(len);
}

#if WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2

static void ananswer(uint_fast8_t arg)
{
	const uint_fast32_t f = gfreqs [getbankindex_tx(gtx)];
	const int p1 = (geteffantenna(f) != 0) ? 2 : 1;
	const int p2 = (geteffrxantenna(f) != 0) ? 1 : 0;
	const int p3 = gtxgate ? 1 : 0;
	//const uint_fast8_t pathi = 0;	// A or B path
	// Antenna information answer
	static const char fmt_1 [] =
		"AN"				// 2 characters - status information code
		"%01d"				// 1 chars - 0: ant 1, 1: : ant 2
		"%01d"					// 1 chars - 0: RX ANT is not used
		"%01d"					// 1 chars - 1: : Drive Out ON
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		p1, p2, p3
		);
	cat_answer(len);
}

#endif /* WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX */

#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)

// SWR
static void rm1answer(uint_fast8_t arg)
{
	//const uint_fast8_t pathi = 0;	// A or B path
	// SWR-meter report
	static const char fmt_1 [] =
		"RM"			// 2 characters - status information code
		"1"				// 1 char - 1 - SWR
		"%04d"				// 4 chars - 0000 ~ 0030: Meter value in dots
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) kenwoodswrmeter()
		);
	cat_answer(len);
}

// COMP
static void rm2answer(uint_fast8_t arg)
{
	//const uint_fast8_t pathi = 0;	// A or B path
	// COMP report
	static const char fmt_1 [] =
		"RM"			// 2 characters - status information code
		"2"				// 1 char - 2 - COMP
		"%04d"				// 4 chars - 0000 ~ 0030: Meter value in dots
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) kenwoodcompmeter()
		);
	cat_answer(len);
}

// ALC
static void rm3answer(uint_fast8_t arg)
{
	//const uint_fast8_t pathi = 0;	// A or B path
	// ALC report
	static const char fmt_1 [] =
		"RM"			// 2 characters - status information code
		"3"				// 1 char - 3 - ALC
		"%04d"				// 4 chars - 0000 ~ 0030: Meter value in dots
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) kenwoodalcmeter()
		);
	cat_answer(len);
}

#endif /* WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) */

static uint_fast8_t
adjust8(uint_fast8_t v, uint_fast8_t minimal, uint_fast8_t maximal)
{
	if (v > maximal)
		return maximal;
	if (v < minimal)
		return minimal;
	return v;
}

// в трансивере допустимый диапазон изменения значений CW PITCH больше чем поддерживается протоколом CAT KENWOOD
// поэтому перед отдачей приводится к допустимому.
static void ptanswer(uint_fast8_t arg)
{
	static const char fmt_1 [] =
		"PT"			// 2 characters - status information code
		"%02d"			// P1 2 characters - pitch code
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) ((adjust8(gcwpitch10, 40, 190) - 40) / 5)
		);

	cat_answer(len);
}

// information request
// Reads status of the transceiver.
static void ifanswer(uint_fast8_t arg)
{
	static const char fmt_5 [] =
		"IF"			// 2 characters - status information code
		"%011ld"		// P1 11 characters - freq
		"     "			// P2 5 chars - unused
		"+0000"			// P3 5 chars - rit/xit freq
		"0"				// P4 1 char - rit off/on
		"0"				// P5 1 char - xit off/on
		" "				// P6 1 char - not used (hundreds of channel number)
		"00"			// P7 2 char - memory/channel
		"%1d"			// P8 1 char - tx/rx
		"%1d"			// P9 1 char - mode
		"%1d"			// P10 1 char - FR/FT function
		"0"				// P11 1 char - scan off/on
		"%1d"			// P12 1 char - split off/on
		"0"				// P13 1 char - tone off/on
		"00"			// P14 2 char - tone number
		"0"				// 1 char - unused
		";";				// 1 char - line terminator

	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_5,
		(signed long) gfreqs [getbankindex_tx(gtx)],
		(int) gtx,
		(int) submodes [gsubmode].qcatmodecode,	// P9
		(int) 0,	// P10: FR/FT state
		(int) (gsplitmode == VFOMODES_VFOSPLIT) // P12
		);
	cat_answer(len);
}

static void badcommandanswer(uint_fast8_t arg)
{
	static const char badcommand [] = "?;";	// ответ на команду с неправильным синтаксисом

	cat_answervariable(badcommand, 2);	// после ответа - ждём новую.
}

#if WITHTX

static uint_fast8_t
cat_get_signal(uint_fast8_t selector)
{
	switch (selector)
	{
	case BOARD_CATSIG_SER1_RTS: return HARDWARE_CAT_GET_RTS();
	case BOARD_CATSIG_SER1_DTR: return HARDWARE_CAT_GET_DTR();
#if WITHUSBHW && WITHUSBCDCACM && WITHUSBCDCACM_N > 1
	case BOARD_CATSIG_SER2_RTS: return HARDWARE_CAT2_GET_RTS();
	case BOARD_CATSIG_SER2_DTR: return HARDWARE_CAT2_GET_DTR();
#endif /* WITHUSBHW && WITHUSBCDCACM && WITHUSBCDCACM_N > 1 */
	default: return 0;
	}
}

// Вызывается из пользовательской программы
// Получить запрос перехода на передачу от порта управления CAT.
static uint_fast8_t
cat_get_hwptt(void)
{
	if (catprocenable != 0)
	{
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& catsyslock, & oldIrql, CATSYS_IRQL);
		const uint_fast8_t r = cat_get_signal(catsigptt);
		IRQLSPIN_UNLOCK(& catsyslock, oldIrql);

		return r;
	}
	return 0;
}

#else
static uint_fast8_t
cat_get_hwptt(void)
{
	return 0;
}
#endif /* WITHTX */


// Вызывается из обработчика перерываний
// Получить нажате ключа от порта управления, вызывается из обработчика перерываний
uint_fast8_t cat_get_keydown(void)
{
#if WITHELKEY && WITHTX
	if (catprocenable != 0)
	{
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& catsyslock, & oldIrql, CATSYS_IRQL);
		const uint_fast8_t r = cat_get_signal(catsigkey);
		IRQLSPIN_UNLOCK(& catsyslock, oldIrql);

		return r;
	}
#endif /* WITHELKEY && WITHTX */
	return 0;

}


static void
NOINLINEAT
cat_answer_request(uint_fast8_t catindex)
{
	if (CAT_MAX_INDEX == catindex)
		return;
	//PRINTF(PSTR("cat_answer_request: catindex=%u\n"), catindex);
	cat_answer_map [catindex] = 1;
}

/* эта операция вызывается неоднократно - не должна мешать работе при уже разрешённом CAT */
static void processcat_enable(uint_fast8_t enable)
{
	if (catprocenable == enable)
		return;
	catprocenable = enable;
	if (! catprocenable)
	{
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& catsyslock, & oldIrql, CATSYS_IRQL);
		HARDWARE_CAT_ENABLERX(0);
		HARDWARE_CAT_ENABLETX(0);
		catstatein = CATSTATE_HALTED;
		catstateout = CATSTATEO_HALTED;
		IRQLSPIN_UNLOCK(& catsyslock, oldIrql);
	}
	else
	{
		uint_fast8_t i;
		for (i = 0; i < (sizeof cat_answer_map / sizeof cat_answer_map [0]); ++ i)
			cat_answer_map [i] = 0;

#if WITHTX
		sendmorsepos  [0] =
		inpmorselength [0]=
		sendmorsepos  [1] =
		inpmorselength [1] = 0;
#endif /* WITHTX */

		aistate = 0; /* Power-up state of AI mode = 0 (TS-590). */
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& catsyslock, & oldIrql, CATSYS_IRQL);
		HARDWARE_CAT_ENABLERX(1);
		catstatein = CATSTATE_WAITCOMMAND1;
		catstateout = CATSTATEO_SENDREADY;
		IRQLSPIN_UNLOCK(& catsyslock, oldIrql);
	}
}

/* вызывается из USER-MODE программы для отправки ответов на CAT-запросы */

typedef void (* canapfn)(uint_fast8_t arg);

static const canapfn catanswers [CAT_MAX_INDEX] =
{
#if 1
#if WITHELKEY
	ky0answer,	// обрабатыаем первой для ускорения реакции,
	ky1answer,	// обрабатыаем первой для ускорения реакции,
	ksanswer,
#endif /* WITHELKEY */
	psanswer,
	sm0answer,
	sm9answer,
	raanswer,
	paanswer,
#if WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2
	ananswer,
#endif /* WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2 */
#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)
	rm1answer,
	rm2answer,
	rm3answer,
#endif /* WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) */
#if WITHTX && WITHAUTOTUNER
	acanswer,
#endif /* WITHTX && WITHAUTOTUNER */
#endif /* 1 */
	idanswer,
	fvanswer,
	daanswer,
	faanswer,
	fbanswer,
	spanswer,
	franswer,
	ftanswer,
	mdanswer,
	txanswer,
	rxanswer,
	aianswer,
	ptanswer,
	ifanswer,
	fwanswer,
#if WITHIF4DSP
	zyanswer,
	zzanswer,
#endif /* WITHIF4DSP */
#if WITHIF4DSP
	rganswer,
	aganswer,
	sqanswer,
#endif /* WITHIF4DSP */
#if WITHPOWERTRIM && WITHTX
	pcanswer,
#endif /* WITHPOWERTRIM && WITHTX */
#if WITHIF4DSP
	nranswer,
#endif /* WITHIF4DSP */
	badcommandanswer,
};

// user-mode function
static void
cat_answer_forming(void)
{
	static uint_fast8_t ilast;
	uint_fast8_t original;
	original = ilast;
	while (cat_answer_ready())
	{
		const uint_fast8_t i = ilast;
		ilast = calc_next(i, 0, (sizeof cat_answer_map / sizeof cat_answer_map [0]) - 1);
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& catsyslock, & oldIrql, CATSYS_IRQL);
		if (cat_answer_map [i] != 0)
		{
			const uint_fast8_t answerparam = cat_answerparam_map [i];
			cat_answer_map [i] = 0;
			IRQLSPIN_UNLOCK(& catsyslock, oldIrql);
			(* catanswers [i])(answerparam);
			return;
		}
		IRQLSPIN_UNLOCK(& catsyslock, oldIrql);
		if (ilast == original)
			break;
	}
}

static uint_fast32_t
catscanint(
	const uint8_t * p,	// массив символов
	uint_fast8_t width	// количество символов
	)
{
	uint_fast32_t v = 0;
	while (width -- && local_isdigit((unsigned char) * p))
	{
		v = v * 10 + * p ++ - '0';
	}
	return v;
}

static unsigned packcmd2(uint_fast8_t c1, uint_fast8_t c2)
{
	return 256 * (uint8_t) c1 + (uint8_t) c2;
}

/* возврат ненуля - была какая-либо команда
	требуется обновление дисплея */
static uint_fast8_t
processcatmsg(
	uint_fast8_t catcommand1,
	uint_fast8_t catcommand2,
	uint_fast8_t cathasparam,
	uint_fast8_t catpcount,	// количество символов за кодом команды
	const uint8_t * catp	// массив символов
	)
{
	//PRINTF(PSTR("processcatmsg: c1=%02X, c2=%02X, chp=%d, cp=%lu\n"), catcommand1, catcommand2, cathasparam, catparam);
	uint_fast8_t rc = 0;
	const uint_fast32_t catparam = catscanint(catp, catpcount);
	const uint_fast16_t pcmd = packcmd2(catcommand1, catcommand2);
#if WITHCAT
	if (pcmd == packcmd2('I', 'D'))
	{
		if (cathasparam != 0)
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}
		else
		{
			cat_answer_request(CAT_ID_INDEX);
		}
	}
	else if (pcmd == packcmd2('F', 'V'))
	{
		if (cathasparam != 0)
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}
		else
		{
			cat_answer_request(CAT_FV_INDEX);
		}
	}
	else if (pcmd == packcmd2('D', 'A'))
	{
		if (cathasparam != 0)
		{
			switch (catparam)
			{
			case 0:
				// DATA mode OFF
				break;
			case 1:
				// DATA mode ON
				break;
			default:
				cat_answer_request(CAT_BADCOMMAND_INDEX);
				break;
			};
		}
		else
		{
			cat_answer_request(CAT_DA_INDEX);
		}
	}
	// parse receieved command
	//
	else if (pcmd == packcmd2('A', 'I'))
	{
		if (cathasparam != 0)
		{
			switch (catparam)
			{
			case 0:
				aistate = 0;
				break;
			case 2:
				aistate = 1;
				cat_answer_request(CAT_IF_INDEX);	/* следующим ответом пойдёт состоние */
				cat_answer_request(CAT_MD_INDEX);	/* следующим ответом пойдёт состоние */
				break;
			default:
				cat_answer_request(CAT_BADCOMMAND_INDEX);
				break;
			}
		}
		else
		{
			cat_answer_request(CAT_AI_INDEX);
		}
	}
	else if (pcmd == packcmd2('F', 'A'))
	{
		if (cathasparam != 0)
		{
			const uint_fast8_t bi = getbankindex_ab(0);	/* VFO A bank index */
			vindex_t vi = getvfoindex(bi);
			const uint_fast32_t v = catparam;
			catchangefreq(vfy32up(v, TUNE_BOTTOM, TUNE_TOP - 1, gfreqs [bi]), bi);
			updateboard();	/* полная перенастройка (как после смены режима) */
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_FA_INDEX);
		}
	}
	else if (pcmd == packcmd2('F', 'B'))
	{
		if (cathasparam != 0)
		{
			const uint_fast8_t bi = getbankindex_ab(1);	/* VFO B bank index */
			vindex_t vi = getvfoindex(bi);
			const uint_fast32_t v = catparam;
			catchangefreq(vfy32up(v, TUNE_BOTTOM, TUNE_TOP - 1, gfreqs [bi]), bi);
			updateboard();	/* полная перенастройка (как после смены режима) */
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_FB_INDEX);
		}
	}
	else if (pcmd == packcmd2('P', 'T'))
	{
		if (cathasparam != 0)
		{
			const uint_fast32_t v = catparam;
			// Минимальный тон телеграфа - 400 герц.
			gcwpitch10 = vfy32up(v, 0, 14, 6) * 5 + 40;
			updateboard();	/* полная перенастройка (как после смены режима) */
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_PT_INDEX);
		}
	}
	else if (pcmd == packcmd2('M', 'D'))
	{
		if (cathasparam != 0)
		{
			const uint_fast32_t v = catparam;

			const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
			vindex_t vi = getvfoindex(bi);
			const uint_fast8_t defsubmode = findkenwoodsubmode(v, gsubmode);	/* поиск по кенвудовскому номеру */
			//defsubmode = getdefaultbandsubmode(gfreqs [bi]);		/* режим по-умолчанию для частоты - USB или LSB */
			// todo: не очень хорошо, если locatesubmode не находит режима, она обнуляет row.
			const uint_fast8_t defcol = locatesubmode(defsubmode, & gmoderows [bi]);	/* строка/колонка для SSB. Что делать, если не нашли? */
			putmodecol(gmoderows [bi], defcol, bi);	/* внести новое значение в битовую маску */
			updateboard();	/* полная перенастройка (как после смены режима) */

			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_MD_INDEX);
		}
	}
	else if (pcmd == packcmd2('I', 'F'))
	{
		if (cathasparam)
		{
		}
		else
		{
			cat_answer_request(CAT_IF_INDEX);
		}
	}
#if WITHIF4DSP
	else if (pcmd == packcmd2('B', 'C'))
	{
		// Sets and reads the Beat Cancel function status.
		// Auto Notch
		cat_answer_request(CAT_BADCOMMAND_INDEX);
	}
	else if (pcmd == packcmd2('R', 'L'))
	{
		// откуда взялось?
		cat_answer_request(CAT_BADCOMMAND_INDEX);
	}
	else if (pcmd == packcmd2('N', 'R'))
	{
		// откуда взялось?
		if (cathasparam != 0)
		{
			const uint_fast32_t p1 = vfy32up(catparam, 0, 2, 0) != 0;	// RN0; NR1; NR2;
			if (gnoisereducts [gmode] != p1)
			{
				gnoisereducts [gmode] = p1;
				updateboard();	/* полная перенастройка (как после смены режима) */
				rc = 1;
			}
			rc = 1;
		}
		else
		{
			// NR;
			cat_answer_request(CAT_NR_INDEX);	// nranswer()
		}
	}
#endif /* WITHIF4DSP */
#if WITHSPLITEX
	else if (pcmd == packcmd2('V', 'V'))
	{
		if (cathasparam == 0)
		{
			// Performs the VFO copy (A=B) function.
			uif_key_click_b_from_a();
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}

	}
	else if (pcmd == packcmd2('S', 'P'))
	{
		if (cathasparam != 0)
		{
			if (catpcount == 1)
			{
				catchangesplit(catp [0] == '1', 0);
			}
			else if (catpcount == 3)
			{
				const int_fast8_t sign = 0 - (catp [1] == '1');	// P2: 0: plus direction
				catchangesplit(catp [0] == '1', sign * (catp [2] - '0'));
			}
			else
			{
				cat_answer_request(CAT_BADCOMMAND_INDEX);
			}
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_SP_INDEX);	// spanswer()
		}
	}
	else if (pcmd == packcmd2('F', 'R'))
	{
		if (cathasparam != 0)
		{
			// gvfosplit: At index 0: RX VFO A or B, at index 1: TX VFO A or B
			switch (catparam)
			{
			case 0:
				// Set VFO A to simplex state
				gvfosplit [0] = 0;
				break;
			case 1:
				// Set VFO B to simplex state
				gvfosplit [0] = 1;
				break;
			case 2:
				// Set VFO B to Memory Channel
				break;
			default:
				cat_answer_request(CAT_BADCOMMAND_INDEX);
				break;
			}
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_FR_INDEX);
		}
	}
	else if (pcmd == packcmd2('F', 'T'))
	{
		if (cathasparam != 0)
		{
			// gvfosplit: At index 0: RX VFO A or B, at index 1: TX VFO A or B
			switch (catparam)
			{
			case 0:
				// Set VFO A to split state
				gvfosplit [1] = 0;
				break;
			case 1:
				// Set VFO B to split state
				gvfosplit [1] = 1;
				break;
			default:
				cat_answer_request(CAT_BADCOMMAND_INDEX);
				break;
			}
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_FT_INDEX);
		}
	}
#endif /* WITHSPLITEX */
#if WITHIF4DSP
	else if (pcmd == packcmd2('N', 'T'))
	{
		// Sets and reads the Notch Filter status.
		cat_answer_request(CAT_BADCOMMAND_INDEX);
	}
	else if (pcmd == packcmd2('B', 'P'))
	{
		// Adjusts the Notch Frequency of the Manual Notch Filter.
		cat_answer_request(CAT_BADCOMMAND_INDEX);
	}
	else if (pcmd == packcmd2('S', 'Q'))
	{
		// Squelch level set/report
		if (cathasparam != 0)
		{
			if (catpcount == 4)
			{
				//const uint_fast32_t p1 = vfy32up(catscanint(catp + 0, 1), 0, 0, 0);
				const uint_fast32_t p2 = vfy32up(catscanint(catp + 1, 3), 0, SQUELCHMAX, 0);
				if (flagne_u8(& gsquelch.value, p2))
				{
					updateboard();	/* полная перенастройка (как после смены режима) */
					rc = 1;
				}
			}
			else if (catpcount == 1)
			{
				const uint_fast32_t p1 = vfy32up(catscanint(catp + 0, 1), 0, 0, 0);
				cat_answerparam_map [CAT_SQ_INDEX] = p1;
				cat_answer_request(CAT_SQ_INDEX);	// sqanswer
				rc = 1;
			}
			else
			{
				cat_answer_request(CAT_BADCOMMAND_INDEX);
			}
		}
		else
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}
	}
	else if (pcmd == packcmd2('A', 'G'))
	{
		// AF gain level set/report
		if (cathasparam != 0)
		{
			if (catpcount == 4)
			{
				//const uint_fast32_t p1 = vfy32up(catscanint(catp + 0, 1), 0, 0, 0);
				const uint_fast32_t p2 = vfy32up(catscanint(catp + 1, 3), 0, 255, 255);
				const unsigned p2board = p2 * (BOARD_AFGAIN_MAX - BOARD_AFGAIN_MIN) / 255 + BOARD_AFGAIN_MIN;	// масштабирование кода от CAT (0..255) во внутренний диапазоны
				if (flagne_u16(& afgain1.value, p2board))
				{
					updateboard();	/* полная перенастройка (как после смены режима) */
					rc = 1;
				}
			}
			else if (catpcount == 1)
			{
				const uint_fast32_t p1 = vfy32up(catscanint(catp + 0, 1), 0, 0, 0);
				cat_answerparam_map [CAT_AG_INDEX] = p1;
				cat_answer_request(CAT_AG_INDEX);	// aganswer
				//rc = 1;
			}
			else
			{
				cat_answer_request(CAT_BADCOMMAND_INDEX);
			}
		}
		else
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}
	}
	else if (pcmd == packcmd2('R', 'G'))
	{
		// RF gain level set/report
		if (cathasparam != 0)
		{
			const uint_fast32_t p2 = vfy32up(catparam, 0, 255, 255);
			const unsigned p2board = p2 * (BOARD_IFGAIN_MAX - BOARD_IFGAIN_MIN) / 255 + BOARD_IFGAIN_MIN;	// масштабирование кода от CAT (0..255) во внутренний диапазон
			if (flagne_u16(& rfgain1.value, p2board))
			{
				updateboard();	/* полная перенастройка (как после смены режима) */
				rc = 1;
			}
		}
		else
		{
			cat_answer_request(CAT_RG_INDEX);	// rganswer
		}
	}
#if WITHPOWERTRIM && WITHTX
	else if (pcmd == packcmd2('P', 'C'))
	{
		// May be: CG Sets and reads the Carrier Level.
		// Sets and reads the output power
		if (cathasparam != 0)
		{
			const uint_fast32_t p2 = vfy32up(catparam, 5, 100, 100);
			// Нормирование Значений Kenwook CAT к диапазону WITHPOWERTRIMMIN..WITHPOWERTRIMMAX
			const int_fast32_t v = (p2 - 5) * (WITHPOWERTRIMMAX - WITHPOWERTRIMMIN) / 95 + WITHPOWERTRIMMIN;
#if 0
			const int_fast32_t vold = param_getvalue(& xgnormalpower);
			param_setvalue(& xgnormalpower, v);
			if (v != vold)
			{
				updateboard();	/* полная перенастройка (как после смены режима) */
				rc = 1;
			}
#else
			if (flagne_u8(& gnormalpower.value, v))
			{
				updateboard();	/* полная перенастройка (как после смены режима) */
				rc = 1;
			}
#endif
		}
		else
		{
			cat_answer_request(CAT_PC_INDEX);	// pcanswer
		}
	}
#endif /* WITHPOWERTRIM && WITHTX */
#endif /* WITHIF4DSP */
	else if (pcmd == packcmd2('R', 'A'))
	{
		// Attenuator status set/query
		if (cathasparam /* && (catparam == 0 || catparam == 1) */)
		{
			gatt = vfy32up(catparam, 0, ATTMODE_COUNT - 1, 0);	/* 0..1 */;	// one step or fully attenuated
			updateboard();	/* полная перенастройка (как после смены режима) */
			cat_answer_request(CAT_RA_INDEX);
			rc = 1;
		}
		else
		{
			// read attenuator state
			cat_answer_request(CAT_RA_INDEX);
		}
	}
	else if (pcmd == packcmd2('P', 'A'))
	{
		// Preamplifier status set/query
		if (cathasparam /* && (catparam == 0 || catparam == 1) */)
		{
#if ! WITHONEATTONEAMP
			gpamp = vfy32up(catparam, 0, PAMPMODE_COUNT - 1, 0);	/* 0..1 */;	// one step or fully attenuated
			updateboard();	/* полная перенастройка (как после смены режима) */
#endif /* ! WITHONEATTONEAMP */
			cat_answer_request(CAT_PA_INDEX);
			rc = 1;
		}
		else
		{
			// read preamplifier state
			cat_answer_request(CAT_PA_INDEX);
		}
	}
#if WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX
	else if (pcmd == packcmd2('A', 'N'))
	{
		// in differencies from documentation, ant1=1, ant2=2
		// antenna selection set/query
		if (cathasparam)
		{
			if (catpcount == 3)
			{
				const uint_fast8_t p1 = catscanint(catp + 0, 1);	// 1 - ANT1, 2 - ANT2, 9 - NO CHANGE
				const uint_fast8_t p2 = catscanint(catp + 1, 1);	// 0: RX ANT is not used, 1: RX ANT is used, 9 - NO CHANGE
				const uint_fast8_t p3 = catscanint(catp + 2, 1);	// 0: Drive Out OFF, 1: Drive Out ON, 9: No change
				if (p1 != 9)
				{
	#if WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT
					gantenna = vfy32up(p1, 1, ANTMODE_COUNT, 1) - 1;	/* 0..1 */;	// one step or fully attenuated
	#endif /* WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT */
				}
				if (p2 != 9)
				{
	#if WITHANTSELECTRX || WITHANTSELECT1RX
					grxantenna = p2 == 1;
	#endif /* WITHANTSELECTRX || WITHANTSELECT1RX */
				}
				if (p3 != 9)
				{
	#if WITHTX
					gtxgate = p3 == 1;
	#endif /* WITHTX */

				}
				{
					const uint_fast8_t bi = getbankindex_ab(0);	/* VFO A bank index */
					const uint_fast32_t freq = gfreqs [bi];
					const uint_fast8_t bg = getfreqbandgroup(freq);
					const uint_fast8_t ant = geteffantenna(freq);
				#if WITHAUTOTUNER
					loadtuner(bg, ant);
				#endif /* WITHAUTOTUNER */
					updateboard();	/* полная перенастройка (как после смены режима) */

				}
				cat_answer_request(CAT_AN_INDEX);
			}
			else
			{
				cat_answer_request(CAT_BADCOMMAND_INDEX);
			}
			rc = 1;
		}
		else
		{
			// read antenna selection
			cat_answer_request(CAT_AN_INDEX);
		}
	}
#endif /* WITHANTSELECT || WITHANTSELECTRX || WITHANTSELECT1RX */
	else if (pcmd == packcmd2('P', 'S'))
	{
		if (cathasparam)
		{
			// set param
			cat_answer_request(CAT_PS_INDEX);
		}
		else
		{
			// read state
			cat_answer_request(CAT_PS_INDEX);
		}
	}
	else if (pcmd == packcmd2('S', 'M'))
	{
		// The SM command reads the S-meter during reception and the RF (power) meter during transmission.
		if (cathasparam && catparam == 0)
		{
			cat_answer_request(CAT_SM0_INDEX);
		}
		else if (cathasparam && catparam == 9)
		{
			cat_answer_request(CAT_SM9_INDEX);
		}
		else
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}
	}
	else if (pcmd == packcmd2('U', 'P'))
	{
		if (cathasparam)
		{
			const int steps = vfy32up(catparam, 0, 99, 1);	/* 00 .. 99 */
			encoder_pushback(& encoder1, steps * (int) genc1div);
		}
	}
	else if (pcmd == packcmd2('D', 'N'))
	{
		if (cathasparam)
		{
			const int steps = vfy32up(catparam, 0, 99, 1);	/* 00 .. 99 */
			encoder_pushback(& encoder1, 0 - steps * (int) genc1div);
		}
	}
#if 0
	else if (pcmd == packcmd2('E', 'X'))
	{
		if (cathasparam)
		{
			switch (cathasparam)
			{
			case 58:
				// EX0058xxx; управление SPLIT
				break;
			}
		}
	}
#endif
#if WITHTX
	else if (pcmd == packcmd2('T', 'X'))
	{
		if (cathasparam != 0)
		{
			const uint_fast32_t v = catparam;	/* 0 - send, 1 - data send, 2 - TX TUNE */
			switch (v)
			{
			case 0:
				txreq_mox(& txreqst0);
				break;
			case 1:
				txreq_txdata(& txreqst0);
				break;
			case 2:
				txreq_txtone(& txreqst0);
				break;
			}

			if (aistate != 0)
				cat_answer_request(CAT_TX_INDEX);	// ignore main/sub rx selection (0 - main. 1 - sub);
		}
		else
		{
			txreq_mox(& txreqst0);
			if (aistate != 0)
				cat_answer_request(CAT_TX_INDEX);
		}
	}
	else if (pcmd == packcmd2('R', 'X'))
	{
		//	P1
		//		 0:  RX-AT THRU
		//		 1:  RX-AT IN
		//	P2
		//		 0:  TX-AT THRU
		//		 1:  TX-AT IN
		//	P3
		//		 0:  Stop Tuning (Set)/ Tuning is stopped (Answer)

		if (cathasparam != 0)
		{
			txreq_rx(& txreqst0, NULL);
			if (aistate != 0)
				cat_answer_request(CAT_RX_INDEX);	// POSSIBLE: ignore main/sub rx selection (0 - main. 1 - sub);
		}
		else
		{
			txreq_rx(& txreqst0, NULL);
			if (aistate != 0)
				cat_answer_request(CAT_RX_INDEX);
		}
	}
#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)
	else if (pcmd == packcmd2('R', 'M'))
	{
		// Get SWR, COMP or ALC- meter information
		if (cathasparam != 0)
		{
			switch (catparam)
			{
			case 1:	// SWR
			case 2: // COMP
			case 3: // ALC
				rmstate = catparam;
				break;
			default:
				cat_answer_request(CAT_BADCOMMAND_INDEX);
				break;
			}
		}
		else
		{
			switch (rmstate)
			{
			default:
			case 1:
				cat_answer_request(CAT_RM1_INDEX);	// SWR
				break;
			case 2:
				cat_answer_request(CAT_RM2_INDEX);	// COMP
				break;
			case 3:
				cat_answer_request(CAT_RM3_INDEX);	// ALC
				break;
			}
		}
	}
#endif /*  WITHTX && WITHSWRMTR */
#if WITHTX && WITHAUTOTUNER
	else if (pcmd == packcmd2('A', 'C'))
	{
		if (cathasparam != 0)
		{
			if (catpcount == 3)
			{
				const uint_fast32_t p1 = !! catscanint(catp + 0, 1);
				const uint_fast32_t p2 = !! catscanint(catp + 1, 1);
				const uint_fast32_t p3 = !! catscanint(catp + 2, 1);
//				catautotuner_p1 = p1;
//				catautotuner_p2 = p2;
//				catautotuner_p3 = p3;

				const uint_fast8_t tx = 1;
				const uint_fast8_t bi = getbankindex_tx(tx);
				const uint_fast8_t bg = getfreqbandgroup(gfreqs [bi]);
			    const uint_fast8_t ant = geteffantenna(gfreqs [bi]);

				tunerwork = p1 || p2;

				txreq_reqautotune(& txreqst0, !! p3);

				storetuner(bg, ant);
				updateboard();	/* полная перенастройка (как после смены режима) */
				rc = 1;
				cat_answer_request(CAT_AC_INDEX);
			}
			else
			{
				cat_answer_request(CAT_BADCOMMAND_INDEX);
			}
		}
		else
		{
			cat_answer_request(CAT_AC_INDEX);
		}
	}
#endif /* WITHTX && WITHAUTOTUNER */
#endif /*  WITHTX */
	else if (pcmd == packcmd2('F', 'W'))
	{
		// filter width
		if (cathasparam != 0)
		{
			// Ширина полосы пропускания фильтра на приёме в герцах
			const uint_fast32_t width = vfy32up(catparam, 0, 9999, 3100);
			const uint_fast8_t i = findfilter(gmode, gfi, width);	/* поиск фильтра, допустимого для данного режима */
			gfi = getsuitablerx(gmode, i); /* при переключении через CAT сохранения в NVRAM не производится */
			updateboard();	/* полная перенастройка (как после смены режима) */
		}
		else
		{
			cat_answer_request(CAT_FW_INDEX);
		}
	}
#if WITHELKEY
	else if (pcmd == packcmd2('K', 'S'))
	{
		// keyer speed
		if (cathasparam != 0)
		{
			// Скорость передачи от 10 до 60 WPM (в TS-590 от 4 до 60).
			const uint_fast32_t p1 = vfy32up(catparam, CWWPMMIN, CWWPMMAX, 20);
			if (flagne_u8(& elkeywpm.value, p1))
			{
				updateboard();	/* полная перенастройка (как после смены режима) */
			}
		}
		else
		{
			cat_answer_request(CAT_KS_INDEX);
		}
	}
#endif	/* WITHELKEY */
#if WITHIF4DSP
	else if (pcmd == packcmd2('Z', 'Y'))
	{
		if (cathasparam != 0)
		{
			if (catpcount == 4)
			{
				// Format: ZYmSLL
				const uint_fast32_t v = catp [0] - '0';
				const uint_fast8_t submode = findkenwoodsubmode(v, UINT8_MAX);	/* поиск по кенвудовскому номеру */
				if (submode == UINT8_MAX)
				{
					cat_answer_request(CAT_BADCOMMAND_INDEX);
				}
				else
				{
					const uint_fast8_t mode = submodes [submode].mode;
					gnoisereducts [mode] = vfy32up(catscanint(catp + 1, 1), 0, 1, 0);
					gnoisereductvl = vfy32up(catscanint(catp + 2, 2), 0, NRLEVELMAX, gnoisereductvl);
					updateboard();	/* полная перенастройка (как после смены режима) */
					rc = 1;
				}
			}
			else if (catpcount == 1)
			{
				/* report about NR state for specified submode and global level */
				const uint_fast32_t v = catp [0] - '0';
				cat_answerparam_map [CAT_ZY_INDEX] = v;
				cat_answer_request(CAT_ZY_INDEX);	// zyanswer
			}
			else
			{
				cat_answer_request(CAT_BADCOMMAND_INDEX);
			}
		}
		else
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}
	}
	else if (pcmd == packcmd2('Z', 'Z'))
	{
		if (cathasparam != 0)
		{
			if (catpcount == 12)
			{
				// Format: ZZmLLLLUUUUSSS
				const uint_fast32_t v = catp [0] - '0';
				const uint_fast8_t submode = findkenwoodsubmode(v, UINT8_MAX);	/* поиск по кенвудовскому номеру */
				if (submode == UINT8_MAX)
				{
					cat_answer_request(CAT_BADCOMMAND_INDEX);
				}
				else
				{
					const struct modetempl * const pmodet = getmodetempl(submode);
					//ZZmLLLLUUUUSSSS
					const uint_fast8_t bwseti = pmodet->bwsetis [0];	// индекс банка полос пропускания для данного режима на приеме
					const uint_fast8_t pos = bwsetpos [bwseti];
					bwprop_t * const p = bwsetsc [bwseti].prop [pos];
					p->left10_width10 = vfy32up(catscanint(catp + 1, 4), p->limits->left10_width10_low,p->limits->left10_width10_high, p->left10_width10);
					p->right100 = vfy32up(catscanint(catp + 5, 4), p->limits->right100_low, p->limits->right100_high, p->right100);
					if (p->type == BWSET_PAIR)
						p->afresponce = vfy32up(catscanint(catp + 9, 3), AFRESPONCEMIN, AFRESPONCEMAX, p->afresponce);
					updateboard();	/* полная перенастройка (как после смены режима) */
					rc = 1;
				}
			}
			else if (catpcount == 1)
			{
				const uint_fast32_t v = catp [0] - '0';
				cat_answerparam_map [CAT_ZZ_INDEX] = v;
				cat_answer_request(CAT_ZZ_INDEX);	// zzanswer
			}
			else
			{
				cat_answer_request(CAT_BADCOMMAND_INDEX);
			}
		}
		else
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}
	}
#endif
	else
	{
		// нераспознанная команда - ожидание следующей.
		cat_answer_request(CAT_BADCOMMAND_INDEX);
	}

#endif // WITHCAT
	return rc;
}

// user-mode function
static void cat_dpc_func(void * ctx)
{
	if (cat_getstateout() == CATSTATEO_SENDREADY)
	{
		cat_answer_forming();
	}
}

static dpcobj_t cat_dpc;

// High level cat initialize
static void cat_initialize(void)
{
	dpcobj_initialize(& cat_dpc, cat_dpc_func, NULL);
	board_dpc_addentry(& cat_dpc, board_dpc_coreid());
}

#else	/* WITHCAT */

static uint_fast8_t
cat_get_hwptt(void)
{
	return 0;
}
#endif	/* WITHCAT */


#if WITHELKEY


#if WITHBEACON

static uint_fast8_t beacon_state;
static uint_fast8_t beacon_pause;
static uint_fast8_t beacon_index;
static char beacon_text [] = "V";

enum
{
	BEACONST_INITIALIZE,
	BEACON_PAUSE,
	BEACON_TEXT,
	//
	BEACON_COUNT
};

static void beacon_initialize(void)
{
	beacon_state = BEACONST_INITIALIZE;
}

static void beacon_states(void)
{
	switch (beacon_state)
	{
	case BEACONST_INITIALIZE:
		break;
	case BEACON_PAUSE:
		break;
	case BEACON_TEXT:
		break;
	}
}

// вызывается из user mode
uint_fast8_t beacon_get_ptt(void)
{
	return beacon_text [beacon_index] != '\0';
}


#endif /* WITHBEACON */

/* вызывается из обработчика прерываний */
/* Получить следующий символ для передачи (только верхний регистр) */
static char beacon_getnextcw(void)
{
#if WITHBEACON
	char c = beacon_text [beacon_index];

	if (c == '\0')
		beacon_index = 0;
	else
		++ beacon_index;

	return c;
#elif WITHELKEY && WITHTX
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& lockcwmsg, & oldIrql, ELKEY_IRQL);
	if (usersend != NULL && * usersend != '\0')
	{
		IRQLSPIN_UNLOCK(& lockcwmsg, oldIrql);
		return * usersend ++;
	}
	IRQLSPIN_UNLOCK(& lockcwmsg, oldIrql);
	return '\0';
#else /* WITHBEACON */
	return '\0';
#endif /* WITHBEACON */
}

// Получение символа для передачи (только верхний регистр)
uint_fast8_t elkey_getnextcw(void)
{
#if WITHCAT
	const uint_fast8_t chcat = cat_getnextcw();
	const uint_fast8_t ch = (chcat != '\0') ? chcat : beacon_getnextcw();
	return ch;
#else
	return beacon_getnextcw();
#endif /* WITHCAT*/
}

#endif /* WITHELKEY */

// User-mode function.
// Called every secound
static void dpc_1s_timer_fn(void * arg)
{
#if WITHWAVPLAYER || WITHSENDWAV
		if (gloopmsg > 0)
		{
			if (++ loopticks >= gloopsec)
			{
				loopticks = 0;
				playhandler(gloopmsg);
			}
		}
#endif /* WITHWAVPLAYER || WITHSENDWAV */
#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT
		if (gdimmtime == 0)
		{
			// Функция выключена
			if (dimmflag != 0)
			{
				dimmflag = 0;
				dimmflagch = 1;		// запрос на обновление состояния аппаратуры из user mode программы
			}
			dimmcount = 0;
		}
		else if (dimmflag == 0)		// ещё не выключили
		{
			if (++ dimmcount >= gdimmtime)
			{
				dimmflag = 1;
				dimmflagch = 1;		// запрос на обновление состояния аппаратуры из user mode программы
			}
		}
#endif /* WITHLCDBACKLIGHT || WITHKBDBACKLIGHT */
#if WITHFANTIMER
		if (0)
		{

		}
	#if (WITHTHERMOLEVEL || WITHTHERMOLEVEL2)
		else if (gfanpatempflag)
		{
			// Вентилятор включается только по датчику температуры
			const int_fast16_t tempv = hamradio_get_PAtemp_value();	// Градусы в десятых долях
			const int_fast16_t tempon = ulmax16(gfanpaontemp, gfanpaofftemp) * 10;
			const int_fast16_t tempoff = ulmin16(gfanpaontemp, gfanpaofftemp) * 10;
			if (fanpaflag)
			{
				// выключено
				if (tempv >= tempon)
				{
					fanpaflag = 0;	// включаем
					fanpaflagch = 1;
				}
			}
			else
			{
				// включено
				if (tempv <= tempoff)
				{
					fanpaflag = 1;	// выключаем
					fanpaflagch = 1;
				}
			}
			fanpacount = gfanpatime;		/* счётчик времени неактивности - не требуется включаться */
		}
	#endif /* (WITHTHERMOLEVEL || WITHTHERMOLEVEL2) */
		else
		{
			// Вентилятор включаерся по таймеру
			if (gtx != 0 || gfanpatime == 0)
			{
				if (fanpaflag != 0)
				{
					fanpaflag = 0;
					fanpaflagch = 1;
				}
				fanpacount = 0;		/* счётчик времени неактивности  - еще надо будет работать */
			}
			else if (fanpaflag == 0)		// ещё не выключили
			{
				if (++ fanpacount >= gfanpatime)
				{
					fanpaflag = 1;
					fanpaflagch = 1;		// запрос на обновление состояния аппаратуры из user mode программы
				}
			}
		}
#endif /* WITHFANTIMER */
#if WITHTX
	/* обработка таймера ограничения времени передачи */
	if (gtx)
	{
		gtxtimer = (gtxtot && gtxtimer < gtxtot) ? (gtxtimer + 1) : gtxtimer;
	}
	else
	{
		gtxtimer = 0;
	}
#endif /* WITHTX */
#if WITHSLEEPTIMER
		if (gsleeptime == 0)
		{
			// Функция выключена
			if (sleepflag != 0)
			{
				sleepflag = 0;
				sleepflagch = 1;		// запрос на обновление состояния аппаратуры из user mode программы
			}
			sleepcount = 0;
		}
		else if (sleepflag == 0)		// ещё не выключили
		{
			if (++ sleepcount >= gsleeptime * 60)
			{
				sleepflag = 1;
				sleepflagch = 1;		// запрос на обновление состояния аппаратуры из user mode программы
			}
		}
#endif /* WITHSLEEPTIMER */

#if 0 && CPUSTYLE_XC7Z
	hamradio_set_freq(hamradio_get_freq_rx() + 1);
#endif /* CPUSTYLE_XC7Z */

	gui_update();
#if WITHTOUCHGUI || WITHLVGL
	//gui_gnssupdate();
#endif /*WITHTOUCHGUI || WITHLVGL*/

	gblinkphase = ! gblinkphase;
#if defined (GET_CPU_TEMPERATURE) && ! WITHTOUCHGUI && 0
	uint8_t c = GET_CPU_TEMPERATURE();
	PRINTF(PSTR("CPU temp: %dC\n"), c);
#endif

	nvram_sync();

#if WITHCAT
	if (aistate != 0)
	{
		cat_answer_request(CAT_SM0_INDEX);
#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)
		cat_answer_request(CAT_RM1_INDEX);
		cat_answer_request(CAT_RM2_INDEX);
		cat_answer_request(CAT_RM3_INDEX);
#endif
	}
#endif /* WITHCAT */
}

int board_islfmmode(void)
{
#if WITHLFM
	return lfmmode;
#else /* WITHLFM */
	return 0;
#endif /* WITHLFM */
}

#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)

uint_fast16_t get_swr(uint_fast16_t swr_fullscale)
{
	uint_fast16_t swr10; 		// swr10 = 0..30 for swr 1..4
	adcvalholder_t forward, reflected;

	forward = board_getswrpair_filtered(& reflected, swrcalibr);

								// рассчитанное  значение
	if (forward < minforward)
		swr10 = 0;				// SWR=1
	else if (forward <= reflected)
		swr10 = swr_fullscale;		// SWR is infinite
	else
		swr10 = (forward + reflected) * SWRMIN / (forward - reflected) - SWRMIN;
	return swr10;
}

#else

uint_fast16_t get_swr(uint_fast16_t swr_fullscale)
{
	return 0;
}
#endif /* WITHTX */

static uint_fast32_t ipow10(uint_fast8_t v)
{
	uint_fast32_t r = 1;
	while (v --)
		r *= 10;
	return r;
}

#if WITHMENU

const struct paramdefdef * const * getmiddlemenu_cw(unsigned * size)
{
	static const struct paramdefdef * const middlemenu [] =
	{
	#if WITHELKEY
		& xgelkeywpm,
	#endif /* WITHELKEY */
	#if WITHIF4DSP
		& xfltbw_cwnarrow,
		& xfltsofter_cwnarrow,
	#endif /* WITHIF4DSP */
		& xgcwpitch10,
	#if WITHTX && WITHELKEY
		& xgbkinenable,
	#endif /* WITHTX && WITHELKEY */
	#if WITHUSEDUALWATCH
		& xmainsubrxmode,
	#endif /* WITHUSEDUALWATCH */
	#if WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0
		& xgzoomxpow2,
	#endif /* WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0 */
	};

	* size = ARRAY_SIZE(middlemenu);
	return middlemenu;
}

const struct paramdefdef * const * getmiddlemenu_ssb(unsigned * size)
{
	static const struct paramdefdef * const middlemenu [] =
	{
		& xgcwpitch10,
	#if WITHVOX && WITHTX
		& xgvoxenable,
	#endif /* WITHVOX && WITHTX */
	#if WITHTX && WITHAFCODEC1HAVEPROC
		& xgmikeequalizer,
	#endif /* WITHTX && WITHAFCODEC1HAVEPROC */
	#if WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0
		& xgzoomxpow2,
	#endif /* WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0 */
	#if WITHIF4DSP
		& xgnoisereduct,
	#endif /* WITHIF4DSP */
	#if WITHNOTCHONOFF || WITHNOTCHFREQ
		& xgnotch,
	#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
	#if WITHUSEDUALWATCH
		& xmainsubrxmode,
	#endif /* WITHUSEDUALWATCH */
	};

	* size = ARRAY_SIZE(middlemenu);
	return middlemenu;
}

const struct paramdefdef * const * getmiddlemenu_am(unsigned * size)
{
	static const struct paramdefdef * const middlemenu [] =
	{
		& xgcwpitch10,
	#if WITHVOX && WITHTX
		& xgvoxenable,
	#endif /* WITHVOX && WITHTX */
	#if WITHTX && WITHAFCODEC1HAVEPROC
		& xgmikeequalizer,
	#endif /* WITHTX && WITHAFCODEC1HAVEPROC */
	#if WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0
		& xgzoomxpow2,
	#endif /* WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0 */
	#if WITHIF4DSP
		& xgnoisereduct,
	#endif /* WITHIF4DSP */
	};

	* size = ARRAY_SIZE(middlemenu);
	return middlemenu;
}

const struct paramdefdef * const * getmiddlemenu_digi(unsigned * size)
{
	static const struct paramdefdef * const middlemenu [] =
	{
		& xgcwpitch10,
	#if WITHVOX && WITHTX
		& xgvoxenable,
	#endif /* WITHVOX && WITHTX */
	#if WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0
		& xgzoomxpow2,
	#endif /* WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0 */
	};

	* size = ARRAY_SIZE(middlemenu);
	return middlemenu;
}

const struct paramdefdef * const * getmiddlemenu_nfm(unsigned * size)
{
	static const struct paramdefdef * const middlemenu [] =
	{
		& xgcwpitch10,
	#if WITHVOX && WITHTX
		& xgvoxenable,
	#endif /* WITHVOX && WITHTX */
	#if WITHSUBTONES && WITHIF4DSP
		& xgsubtoneirx,
		& xgsubtoneitx,
	#endif /* WITHSUBTONES && WITHIF4DSP */
	#if WITHTX && WITHAFCODEC1HAVEPROC
		& xgmikeequalizer,
	#endif /* WITHTX && WITHAFCODEC1HAVEPROC */
	#if WITHIF4DSP
		& xgnoisereduct,
		& xgsquelchNFM,
	#endif /* WITHIF4DSP */
	#if WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0
		& xgzoomxpow2,
	#endif /* WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0 */
	};

	* size = ARRAY_SIZE(middlemenu);
	return middlemenu;
}

const struct paramdefdef * const * getmiddlemenu_wfm(unsigned * nitems)
{
	static const struct paramdefdef * const middlemenu [] =
	{
		& xgcwpitch10,
	#if WITHVOX && WITHTX
		& xgvoxenable,
	#endif /* WITHVOX && WITHTX */
	#if WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0
		& xgzoomxpow2,
	#endif /* WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0 */
	#if WITHIF4DSP
		& xgnoisereduct,
	#endif /* WITHIF4DSP */
	};

	* nitems = ARRAY_SIZE(middlemenu);
	return middlemenu;
}

static const struct paramdefdef * getmiddlemenu(uint_fast8_t section, uint_fast8_t * active)
{
	unsigned nitems;
	const uint_fast8_t bi = getbankindex_ab(0);
	const uint_fast8_t submode = getsubmode(bi);
	const uint_fast8_t mode = submodes [submode].mode;
	const struct paramdefdef * const * mpd = mdt [mode].middlemenu(& nitems);
	const unsigned apos = gmiddlepos [mode];
	if (section >= nitems)
	{
		* active = 0;
		return & xgdummy;
	}
	* active = (section == apos);
	return mpd [section];
}

const char * hamradio_midlabel5(uint_fast8_t section, uint_fast8_t * active)
{
	const struct paramdefdef * pd = getmiddlemenu(section, active);
	static char buff [32];
	local_snprintf_P(buff, ARRAY_SIZE(buff), "%s", pd->qlabel);
	return buff;
}

const char * hamradio_midvalue5(uint_fast8_t section, uint_fast8_t * active)
{
	const struct paramdefdef * pd = getmiddlemenu(section, active);
	static char buff [32];
	ASSERT(pd);
	int n = param_format(pd, buff, ARRAY_SIZE(buff), param_getvalue(pd));
	return buff;
}


///

#include "menu.h"

#define MENUROW_COUNT (ARRAY_SIZE(menutable))

uint_fast16_t menutable_size(void)
{
	return MENUROW_COUNT;
}

static uint_fast8_t ginmenu0;
static const struct menudef * gmp0 = menutable;

static nvramaddress_t gposnvram = MENUNONVRAM;	// место в мамяти с позицией в текущей группе
static const struct menudef * gmpgroup = menutable;
static uint_fast8_t gmenulevel;	// 0 - groups, 1 - inside group
static uint_fast16_t gmenufirstitem [2] = { 0, 1 };
static uint_fast16_t gmenulastitem [2] = { MENUROW_COUNT - 1, MENUROW_COUNT - 1 };

static void setinmenu(uint_fast8_t inmenu, const struct menudef * mp)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& boardupdatelock, & oldIrql, BRDSYS_IRQL);

	ginmenu0 = inmenu;
	gmp0 = mp;

	IRQLSPIN_UNLOCK(& boardupdatelock, oldIrql);
}

static uint_fast8_t getinmenu(const struct menudef * * mp)
{
	uint_fast8_t f;
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& boardupdatelock, & oldIrql, BRDSYS_IRQL);

	f = ginmenu0;
	* mp = gmp0;

	IRQLSPIN_UNLOCK(& boardupdatelock, oldIrql);

	return f;
}

/* Загрузка значений из NVRAM в переменные программы.
   Значением по умолчанию является то, на которое
   переменная инициализированна при запуске программы.
   Не восстанавливаем "массивы"
   see also loadsavedstate().
   */
static void
//NOINLINEAT
loadsettings(void)
{
	uint_fast16_t i;

	for (i = 0; i < menutable_size(); ++ i)
	{
		const struct menudef * const mp = & menutable [i];
		const struct paramdefdef * const pd = mp->pd;
		if (ismenukinddp(pd, ITEM_VALUE) && ! ismenukinddp(pd, ITEM_NOINITNVRAM))
		{
			unsigned sel;
			unsigned nvalues;
			pd->qselector(& nvalues);
			if (pd->qnvram == MENUNONVRAM)
				continue;
			for (sel = 0; sel < nvalues; ++ sel)
			{
				const uint_fast16_t bottom = pd->qbottom;
				const uint_fast16_t upper = pd->qupper;
				const nvramaddress_t nvram = pd->qnvramoffs(pd->qnvram, sel);
				const ptrdiff_t offs = pd->valoffs(sel);
				uint_fast16_t * const pv16 = pd->apval16 ? pd->apval16 + offs : NULL;
				uint_fast8_t * const pv8 = pd->apval8 ? pd->apval8 + offs : NULL;

				if (pv16 != NULL)
				{
					* pv16 = loadvfy16up(nvram, bottom, upper, * pv16);
				}
				else if (pv8 != NULL)
				{
					* pv8 = loadvfy8up(nvram, bottom, upper, * pv8);
				}
			}
		}
	}
}



/* Загрузка в NVRAM значениями по умолчанию.
   Значением по умолчанию является то, на которое
   переменная инициализированна при запуске программы.
   */
static void
defaultsettings(void)
{
	uint_fast16_t i;
	PRINTF("Loading NVRAM default settings\n");

	for (i = 0; i < menutable_size(); ++ i)
	{
		const struct menudef * const mp = & menutable [i];
		if (! ismenukinddp(mp->pd, ITEM_NOINITNVRAM))
		{
			savemenuvalue(mp->pd);
		}
	}

#if WITHSPECTRUMWF
	const uint_fast8_t bi = 0;
	uint_fast8_t bg;
	for (bg = 0; bg < BANDGROUP_COUNT; ++ bg)
	{
		storezoom(bg);
	}
#endif /* WITHSPECTRUMWF */
}

#if WITHMENU
//+++ menu support

// название редактируемого параметра
// если группа - ничего не отображаем
static void display2_menu_lblng(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, const struct menudef * mp, uint_fast8_t xspan, uint_fast8_t yspan, const char * (* getlabel)(const struct paramdefdef * pd), const gxstyle_t * dbstyle)
{
	multimenuwnd_t window;

	display2_getmultimenu(& window);
	if (ismenukinddp(mp->pd, ITEM_VALUE) == 0)
		return;
	display_text(db, xcell, ycell, getlabel(mp->pd), xspan, yspan, dbstyle);
}

// группа, в которой находится редактируемый параметр
static void display2_menu_group(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, const struct menudef * mp, uint_fast8_t xspan, uint_fast8_t yspan, const char * (* getlabel)(const struct paramdefdef * pd), const gxstyle_t * dbstyle)
{
	multimenuwnd_t window;

	display2_getmultimenu(& window);
	while (ismenukinddp(mp->pd, ITEM_GROUP) == 0)
		-- mp;
	display_text(db, xcell, ycell, getlabel(mp->pd), xspan, yspan, dbstyle);
}

// Отображение многострочного меню для больших экранов (группы)
static void display2_multilinemenu_block_groups(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx, const char * (* getlabel)(const struct paramdefdef * pd))
{
	const struct menudef * const mp = (const struct menudef *) pctx->pv;
	const uint_fast16_t index = (int) (mp - menutable);
	uint_fast16_t y_position_groups = ycell;
	uint_fast16_t index_groups = 0;
	uint_fast16_t selected_group_left_margin; // первый элемент группы
	uint_fast16_t el;
	multimenuwnd_t window;
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);

	display2_getmultimenu(& window);
	const uint_fast8_t rowspan = window.ystep;

	//ищем границы текущей группы параметров
	uint_fast16_t selected_group_finder = index;
	while (selected_group_finder > 0 && ! ismenukinddp(menutable [selected_group_finder].pd, ITEM_GROUP))
		selected_group_finder --;
	selected_group_left_margin = selected_group_finder;

	// предварительно расчитываем скролл
	uint_fast16_t selected_group_index = 0;
	uint_fast16_t selected_params_index = 0;
	for (el = 0; el < menutable_size(); el ++)
	{
		const struct menudef * const mv = & menutable [el];
		if (ismenukinddp(mv->pd, ITEM_GROUP))
		{
			index_groups ++;
			if (el == selected_group_left_margin)
				selected_group_index = index_groups - 1;
		}
	}
	index_groups = 0;
	const uint_fast16_t menu_block_scroll_offset_groups = window.multilinemenu_max_rows * (selected_group_index / window.multilinemenu_max_rows);

	// выводим на экран блок с параметрами
	for (el = 0; el < menutable_size(); el ++)
	{
		const struct menudef * const mv = & menutable [el];
		if (ismenukinddp(mv->pd, ITEM_GROUP))
		{
			index_groups ++;
			if (index_groups <= menu_block_scroll_offset_groups)
				continue; //пропускаем пункты для скролла
			if ((index_groups - menu_block_scroll_offset_groups) > window.multilinemenu_max_rows)
				continue;

			if (el == selected_group_left_margin)
			{
				//подсвечиваем выбранный элемент
				//display_text(db, xcell_marker, y_position_groups, PSTR(">"), 1, (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
				gxstyle_textcolor(& dbstylev, DSGN_MENUSELCOLOR, DSGN_MENUSELBGCOLOR);
			}
			else
			{
				//снять отметку
				//display_text(db, xcell_marker, y_position_groups, PSTR(" "), 1, (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
				gxstyle_textcolor(& dbstylev, DSGN_MENUCOLOR, DSGN_MENUBGCOLOR);
			}
			display2_menu_group(db, xcell, y_position_groups, mv, xspan, rowspan, getlabel, & dbstylev); // название группы

			y_position_groups += window.ystep;
		}
	}

	//стираем ненужные имена групп, оставшиеся от предыдущей страницы
	gxstyle_textcolor(& dbstylev, DSGN_MENUCOLOR, DSGN_MENUBGCOLOR);
	for (;
			index_groups - menu_block_scroll_offset_groups < window.multilinemenu_max_rows;
			++ index_groups, y_position_groups += window.ystep)
	{
		display_text(db, xcell, y_position_groups, "", xspan, rowspan, & dbstylev);
	}
}

// Отображение многострочного меню для больших экранов (параметры)
static void display2_multilinemenu_block_params(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx, const char * (* getlabel)(const struct paramdefdef * pd))
{
	const struct menudef * const mp = (const struct menudef *) pctx->pv;
	const uint_fast16_t index = (int) (mp - menutable);
	uint_fast8_t y_position_params = ycell;
	uint_fast16_t index_params = 0;
	uint_fast16_t selected_group_left_margin; // первый элемент группы
	uint_fast16_t selected_group_right_margin; // последний элемент группы
	uint_fast16_t el;
	multimenuwnd_t window;
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);

	display2_getmultimenu(& window);
	const uint_fast8_t rowspan = window.ystep;

	// ищем границы текущей группы параметров
	uint_fast16_t selected_group_finder = index;
	while (selected_group_finder > 0 && ! ismenukinddp(menutable [selected_group_finder].pd, ITEM_GROUP))
		selected_group_finder --;
	selected_group_left_margin = selected_group_finder;
	selected_group_finder ++;
	while (selected_group_finder < menutable_size() && ! ismenukinddp(menutable [selected_group_finder].pd, ITEM_GROUP))
		selected_group_finder ++;
	selected_group_right_margin = selected_group_finder - 1;	// последний элмент в списке параметров данной группы

	// предварительно расчитываем скролл
	uint_fast16_t selected_params_index = 0;
	for (el = 0; el < menutable_size(); el ++)
	{
		const struct menudef * const mv = & menutable [el];
		if (ismenukinddp(mv->pd, ITEM_VALUE))
		{
			if (el < selected_group_left_margin || el > selected_group_right_margin)
				continue;
			index_params ++;
			if (el == index)
				selected_params_index = index_params - 1;
		}
	}
	index_params = 0;
	const uint_fast16_t menu_block_scroll_offset_params = window.multilinemenu_max_rows * (selected_params_index / window.multilinemenu_max_rows);

	// выводим на экран блок с параметрами
	for (el = 0; el < menutable_size(); el ++)
	{
		const struct menudef * const mv = & menutable [el];
		if (ismenukinddp(mv->pd, ITEM_VALUE))
		{
			if (el < selected_group_left_margin)
				continue;
			if (el > selected_group_right_margin)
				continue;
			index_params ++;
			if (index_params <= menu_block_scroll_offset_params)
				continue; //пропускаем пункты для скролла
			if ((index_params - menu_block_scroll_offset_params) > window.multilinemenu_max_rows)
				continue;

			if (el == index)
			{
				//подсвечиваем выбранный элемент
				//display_text(db, xcell_marker, y_position_params, PSTR(">"), 1, (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
				gxstyle_textcolor(& dbstylev, DSGN_MENUVALSELCOLOR, DSGN_MENUVALSELBGCOLOR);
			}
			else
			{
				//снять подсветку
				//display_text(db, xcell_marker, y_position_params, PSTR(" "), 1, (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
				gxstyle_textcolor(& dbstylev, DSGN_MENUCOLOR, DSGN_MENUBGCOLOR);
			}
			display2_menu_lblng(db, xcell, y_position_params, mv, xspan, rowspan, getlabel, & dbstylev); // название редактируемого параметра

            y_position_params += window.ystep;
		}
	}

	//стираем ненужные имена параметров, оставшиеся от предыдущей страницы

	gxstyle_textcolor(& dbstylev, DSGN_MENUCOLOR, DSGN_MENUBGCOLOR);
	for (;
			index_params - menu_block_scroll_offset_params < window.multilinemenu_max_rows;
			++ index_params, y_position_params += window.ystep)
	{
		display_text(db, xcell, y_position_params, "", xspan, rowspan, & dbstylev);
	}
}

// Отображение многострочного меню для больших экранов (значения)
static void display2_multilinemenu_block_vals(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
	const struct menudef * const mp = (const struct menudef *) pctx->pv;
	const uint_fast16_t index = (int) (mp - menutable);
	uint_fast8_t y_position_params = y;
	uint_fast16_t index_params = 0;
	uint_fast16_t selected_group_left_margin; // первый элемент группы
	uint_fast16_t selected_group_right_margin; // последний элемент группы
	uint_fast16_t el;
	multimenuwnd_t window;
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);

	display2_getmultimenu(& window);
	const uint_fast8_t rowspan = window.ystep;

	//ищем границы текущей группы параметров
	uint_fast16_t selected_group_finder = index;
	while (selected_group_finder > 0 && ! ismenukinddp(menutable [selected_group_finder].pd, ITEM_GROUP))
		selected_group_finder --;
	selected_group_left_margin = selected_group_finder;
	selected_group_finder ++;
	while (selected_group_finder < menutable_size() && ! ismenukinddp(menutable [selected_group_finder].pd, ITEM_GROUP))
		selected_group_finder ++;
	selected_group_right_margin = selected_group_finder - 1;	// последний элмент в списке параметров данной группы

	// предварительно расчитываем скролл
	uint_fast16_t selected_params_index = 0;
	for (el = 0; el < menutable_size(); el ++)
	{
		const struct menudef * const mv = & menutable [el];
		if (ismenukinddp(mv->pd, ITEM_VALUE))
		{
			if (el < selected_group_left_margin || el > selected_group_right_margin)
				continue;
			index_params ++;
			if (el == index)
				selected_params_index = index_params - 1;
		}
	}
	index_params = 0;
	const uint_fast16_t menu_block_scroll_offset_params = window.multilinemenu_max_rows * (selected_params_index / window.multilinemenu_max_rows);

	// выводим на экран блок с параметрами
	for (el = 0; el < menutable_size(); el ++)
	{
		const struct paramdefdef * const pd = menutable [el].pd;
		if (ismenukinddp(pd, ITEM_VALUE))
		{
			if (el < selected_group_left_margin)
				continue;
			if (el > selected_group_right_margin)
				continue;
			index_params ++;
			if (index_params <= menu_block_scroll_offset_params)
				continue; //пропускаем пункты для скролла
			if ((index_params - menu_block_scroll_offset_params) > window.multilinemenu_max_rows)
				continue;

            if (el == index)
            {
                gxstyle_textcolor(& dbstylev, DSGN_MENUVALSELCOLOR, DSGN_MENUVALSELBGCOLOR);
            }
            else
            {
                gxstyle_textcolor(& dbstylev, DSGN_MENUCOLOR, DSGN_MENUBGCOLOR);
            }
        	char buff [xspan + 1];

        	param_format(pd, buff, xspan + 1, param_getvalue(pd));
        	display_text(db, x, y_position_params, buff, xspan, rowspan, & dbstylev);

			y_position_params += window.ystep;
		}
	}

	/* параметры полей вывода значений в меню */
	//стираем ненужные значения параметров, оставшиеся от предыдущей страницы
	gxstyle_textcolor(& dbstylev, DSGN_MENUCOLOR, DSGN_MENUBGCOLOR);
	for (;
			index_params - menu_block_scroll_offset_params < window.multilinemenu_max_rows;
			++ index_params, y_position_params += window.ystep)
	{
		//display_menu_string(colorpip, x, y_position_params, nolabel, VALUEW, VALUEW);
		display_text(db, x, y_position_params, "", xspan, rowspan, & dbstylev);
	}
}

// вызывается по dzones
void display2_multilinemenu_block(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
	if (pctx == NULL || pctx->type != DCTX_MENU)
		return;
	multimenuwnd_t w;

	if (xspan == 0)
		return;
	display2_getmultimenu(& w);
	/* параметры полей вывода значений в меню */
	const uint_fast8_t VALUEW = w.valuew;

	if (w.xspan < 50)
	{
		// compact view
		const uint_fast8_t groupx = xcell;
		const uint_fast8_t namesx = groupx + VALUEW + 1;
		const uint_fast8_t valuesx = namesx + VALUEW + 1;
		const uint_fast8_t groupspan = namesx - groupx;
		const uint_fast8_t namesspan = valuesx - namesx;
		const uint_fast8_t valuesspan = xspan - valuesx;

		display2_multilinemenu_block_groups(db, groupx, ycell, groupspan, yspan, pctx, pd_getshortlabel);
		display2_multilinemenu_block_params(db, namesx, ycell, namesspan, yspan, pctx, pd_getshortlabel);
		display2_multilinemenu_block_vals(db, valuesx, ycell, valuesspan, yspan, pctx);
	}
	else
	{
		const uint_fast8_t groupx = xcell + 1;
		const uint_fast8_t namesx = groupx + VALUEW + 5;
		const uint_fast8_t valuesx = namesx + VALUEW + 5;
		const uint_fast8_t groupspan = namesx - groupx;
		const uint_fast8_t namesspan = valuesx - namesx;
		const uint_fast8_t valuesspan = xspan - 1 - valuesx;

		display2_multilinemenu_block_groups(db, groupx, ycell, groupspan, yspan, pctx, pd_getlonglabel);
		display2_multilinemenu_block_params(db, namesx, ycell, namesspan, yspan, pctx, pd_getlonglabel);
		display2_multilinemenu_block_vals(db, valuesx, ycell, valuesspan, yspan, pctx);
	}
}
#endif /* WITHMENU */

size_t
param_format(
	const struct paramdefdef * pd,
	char * buff,
	size_t count,	// размер буфера
	int_fast32_t value
	)
{
	buff [0] = '\0';
	if (! ismenukinddp(pd, ITEM_VALUE))
	{
		return 0;
	}
	static const char months [13] [4] =
	{
		"---",
		"JAN",
		"FEB",
		"MAR",
		"APR",
		"MAY",
		"JUN",
		"JUL",
		"AUG",
		"SEP",
		"OCT",
		"NOV",
		"DEC",
	};
	if (ismenukinddp(pd, ITEM_VALUE) == 0)
		return 0;

	if (pd->qrj == RJ_CB && pd->getvaltext != NULL)
	{
		return pd->getvaltext(buff, count, value);
	}
	// отображение параметра, отличающиеся от цифрового
	switch (pd->qrj)
	{
	case RJ_POW2:
		return local_snprintf_P(buff, count, "%" PRIu32, UINT32_C(1) << value);
	case RJ_YES:
		return local_snprintf_P(buff, count, "%s", value ? "YES" : "NO");
	case RJ_ON:
		return local_snprintf_P(buff, count, "%s", value ? "ON" : "OFF");

	case RJ_SIGNED:
		switch (pd->qcomma)
		{
		default:
		case 0:
			return local_snprintf_P(buff, count, "%+" PRIdFAST32, value);
		case 1:
			return local_snprintf_P(buff, count, "%+" PRIdFAST32 ".%01" PRIdFAST32, value / 10, iabs32(value % 10));
		case 2:
			return local_snprintf_P(buff, count, "%+" PRIdFAST32 ".%02" PRIdFAST32, value / 100, iabs32(value % 100));
		case 3:
			return local_snprintf_P(buff, count, "%+" PRIdFAST32 ".%03" PRIdFAST32, value / 1000, iabs32(value % 1000));
		}

	default:
	case RJ_UNSIGNED:
		switch (pd->qcomma)
		{
		default:
		case 0:
			return local_snprintf_P(buff, count, "%" PRIdFAST32, value);
		case 1:
			return local_snprintf_P(buff, count, "%" PRIdFAST32 ".%01" PRIdFAST32, value / 10, value % 10);
		case 2:
			return local_snprintf_P(buff, count, "%" PRIdFAST32 ".%02" PRIdFAST32, value / 100, value % 100);
		case 3:
			return local_snprintf_P(buff, count, "%" PRIdFAST32 ".%03" PRIdFAST32, value / 1000, value % 1000);
		}

#if WITHMODEM

	case RJ_MDMSPEED:
		display_menu_digit(db, x, y, modembr2int100 [value], count, comma, 0);
		break;

	case RJ_MDMMODE:
		{
			static const char msg [] [5] =
			{
 				"BPSK",
				"QPSK",
			};
			return local_snprintf_P(buff, count, "%s", msg [value]);
		}
		break;

#endif /* WITHMODEM */

	}
}

// --- menu support

static uint_fast16_t menulooklast(uint_fast16_t menupos)
{
	const struct menudef * mp;
	do
	{
		mp = & menutable [++ menupos];
	} while (menupos < menutable_size() && ismenukinddp(mp->pd, ITEM_VALUE) != 0);
	return menupos - 1;
}



/* возврат ненуля - было какое-либо нажатие,
	требуется обновление дисплея и состояния аппаратуры */
static uint_fast8_t
processmenukeyandencoder(inputevent_t * ev)
{
	uint_fast16_t firstitem = gmenufirstitem [gmenulevel];
	uint_fast16_t lastitem = gmenulastitem [gmenulevel];
	const struct menudef * mp = gmp0;
	uint_fast16_t menupos = mp - menutable;
	multimenuwnd_t window;
	const uint_fast8_t itemmask = gmenulevel ? ITEM_VALUE : ITEM_GROUP;

	display2_getmultimenu(& window);

//	if (kbready && kbch == exitkey)
//	{
//		if (gmenulevel != 0)
//		{
//			-- gmenulevel;
//		}
//		else
//		{
//			gmenulevel = 0;
//			ginmenu = 0;
//		}
//		return 0;
//	}

#if WITHENCODER_4F
	if (! ev->frontkeyevent.kbready)
	{
		const int_least16_t delta = event_getRotate_LoRes(& ev->encF4, BOARD_ENC4F_DIVIDE);  // перемещение по меню также с помощью 2го энкодера

		if (delta > 0)
		{
			ev->frontkeyevent.kbch = KBD_CODE_MENU_DOWN;
			ev->frontkeyevent.kbready = 1;
		}
		else if (delta < 0)
		{
			ev->frontkeyevent.kbch = KBD_CODE_MENU_UP;
			ev->frontkeyevent.kbready = 1;
		}
	}
#elif WITHENCODER2
	if (! ev->frontkeyevent.kbready)
	{
		const int_least16_t nr2 = event_getRotate_LoRes(& ev->encFN, genc2div);  // перемещение по меню также с помощью 2го энкодера

		if (nr2 > 0)
		{
			ev->frontkeyevent.kbch = KBD_CODE_MENU_DOWN;
			ev->frontkeyevent.kbready = 1;
		}
		else if (nr2 < 0)
		{
			ev->frontkeyevent.kbch = KBD_CODE_MENU_UP;
			ev->frontkeyevent.kbready = 1;
		}
	}
#endif /* WITHENCODER2 */

#if WITHENCODER
	{
		/* редактирование значения с помощью поворота валкодера. */
		const int_least16_t nrotate = event_getRotate_Menu(& ev->encMAIN);

		if (nrotate != 0 && ismenukinddp(mp->pd, ITEM_VALUE))
		{
			param_rotate(mp->pd, nrotate);	// модификация и сохранение параметра
			/* обновление отображения пункта */
			board_wakeup();
			updateboard();
			return 1;
		}
	}
#endif /* WITHENCODER */

	if (! ev->frontkeyevent.kbready)
		return 0;

	switch (ev->frontkeyevent.kbch)
	{
	default:
		break;

	case KBD_CODE_DISPMODE:
	case KBD_ENC2_PRESS:
		if (gmenulevel == 0)
		{
			/* выход из меню */
			setinmenu(0, NULL);
			encoders_clear();		// предотвратить переход по частоте после изменения паарметров (накопленные delta)
		}
		else if (gmenulevel != 0)
		{
			gmenulevel = 0;
			do
			{
				/* найти группу в которой находимся */
				menupos = calc_prev(menupos, 0, menutable_size() - 1);
				mp = & menutable [menupos];
			}
			while (ismenukinddp(mp->pd, ITEM_VALUE));
			setinmenu(1, mp);
		}
		ev->frontkeyevent.kbready = 0;
		return 0;

	case KBD_CODE_MENU:
	case KBD_ENC2_HOLD:
		if (gmenulevel == 0 && ismenukinddp(mp->pd, ITEM_GROUP))
		{
			/* вход в подменю */
			const uint_fast16_t first = menupos + 1;	/* следующий за текущим пунктом */
			const uint_fast16_t last = menulooklast(first);

			if (ismenukinddp(menutable [first].pd, ITEM_VALUE))
			{
			#if defined (RTC1_TYPE)
				getstamprtc();
			#endif /* defined (RTC1_TYPE) */
				// войти в подменю
				gmenulevel = 1;
				gmpgroup = mp;
				gposnvram = mp->pd->qnvram;	// место в памяти с позицией в текущей группе
				menupos = loadvfy16up(gposnvram, first, last, first);
				setinmenu(1, & menutable [menupos]);
				gmenufirstitem [1] = first;
				gmenulastitem [1] = last;
			}
			else
			{
				// группа без пунктов?
			}
		}
		ev->frontkeyevent.kbready = 0;
		return 1;	// требуется обновление индикатора

//	case KBD_CODE_LOCK:
//		savemenuvalue(mp->pd);		/* сохраняем отредактированное значение */
//		uif_key_lockencoder();
//		return 1;	// требуется обновление индикатора
#if WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC
	case KBD_CODE_LOCK_HOLDED:
		/* запись видимого изображения */
		display_snapshot_req();
		return 1;	// требуется обновление индикатора
#endif /* WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC */

	case KBD_CODE_POWEROFF:
		savemenuvalue(mp->pd);		/* сохраняем отредактированное значение */
		uif_pwbutton_press();
		ev->frontkeyevent.kbready = 0;
		return 0;

#if WITHTX
	case KBD_CODE_MOX:
		savemenuvalue(mp->pd);		/* сохраняем отредактированное значение */
		/* выключить режим настройки или приём/передача */
		uif_key_moxclick();
		ev->frontkeyevent.kbready = 0;
		return 1;	// требуется обновление индикатора

	case KBD_CODE_TXTUNE:
		savemenuvalue(mp->pd);		/* сохраняем отредактированное значение */
		/* выключить режим настройки или приём/передача */
		uif_key_tune();
		return 1;	// требуется обновление индикатора

#if WITHAUTOTUNER
	case KBD_CODE_ATUSTART:
		savemenuvalue(mp->pd);		/* сохраняем отредактированное значение */
		uif_key_atunerstart();
		ev->frontkeyevent.kbready = 0;
		return 1;	// требуется обновление индикатора

	case KBD_CODE_ATUBYPASS:
		savemenuvalue(mp->pd);		/* сохраняем отредактированное значение */
		uif_key_bypasstoggle();
		ev->frontkeyevent.kbready = 0;
		return 1;	// требуется обновление индикатора
#endif /* WITHAUTOTUNER */

#endif /* WITHTX */

	case KBD_CODE_BAND_DOWN:
#if WITHENCODER2 || WITHENCODER_4F
		savemenuvalue(mp->pd);		/* сохраняем отредактированное значение */
		/* переход на следующий (с большей частотой) диапазон или на шаг general coverage */
		uif_key_click_banddown();
		ev->frontkeyevent.kbready = 0;
		return 1;	// требуется обновление индикатора
#endif /* WITHENCODER2 || WITHENCODER_4F */

	case KBD_CODE_MENU_DOWN:
		/* переход на предыдущий пункт/группу меню */
		savemenuvalue(mp->pd);		/* сохраняем отредактированное значение */
		do
		{
			/* проход по определённому типу элементов (itemmask) */
			menupos = calc_dir(! window.reverse, menupos, firstitem, lastitem);
			mp = & menutable [menupos];
		}
		while (! ismenukinddp(mp->pd, itemmask));
		goto menuswitch;

	case KBD_CODE_BAND_UP:
#if WITHENCODER2 || WITHENCODER_4F
		savemenuvalue(mp->pd);		/* сохраняем отредактированное значение */
		/* переход на следующий (с большей частотой) диапазон или на шаг general coverage */
		uif_key_click_bandup();
		ev->frontkeyevent.kbready = 0;
		return 1;	// требуется обновление индикатора
#endif /* WITHENCODER2 || WITHENCODER_4F */

	case KBD_CODE_MENU_UP:
		/* переход на следующий пункт/группу меню */
		savemenuvalue(mp->pd);		/* сохраняем отредактированное значение */
		do
		{
			/* если спецпункты запрещены - ищем обычный */
			menupos = calc_dir(window.reverse, menupos, firstitem, lastitem);
			mp = & menutable [menupos];
		}
		while (! ismenukinddp(mp->pd, itemmask));

	menuswitch:
		if (gmenulevel == 0)
		{
			// проход по группам
			ASSERT(ITEM_GROUP == itemmask);
			save_i16(RMT_GROUP_BASE, menupos);
		}
		else
		{
			// проход по пунктам
			ASSERT(ITEM_VALUE == itemmask);
			save_i16(gposnvram, menupos);	/* сохраняем номер пункта меню, с которым работаем */
		}
		setinmenu(1, mp);
#if WITHDEBUG
		PRINTF(PSTR("menu: ")); PRINTF(mp->pd->qlabel); PRINTF(PSTR("\n"));
#endif /* WITHDEBUG */

		ev->frontkeyevent.kbready = 0;
		return 0;
	}

	return 0;
}

// Вызов конкретного пункта меню на редактирование
static void
uif_key_click_menubyname(const char * name, uint_fast8_t exitkey)
{
	uint_fast16_t menupos;
#if WITHAUTOTUNER
	if (txreq_getreqautotune(& txreqst0) != 0)
		return;
#endif /* WITHAUTOTUNER */

	for (menupos = 0; menupos < menutable_size(); ++ menupos)
	{
		const struct menudef * const mp = & menutable [menupos];
		if (ismenukinddp(mp->pd, ITEM_VALUE) == 0)
			continue;
		const int r = strcmp(name, mp->pd->qlabel);
		if (r == 0)
			break;
	}
	if (menupos >= menutable_size())
	{
		// Не нашли такой пункт
		return;
	}
#if WITHTOUCHGUI
	gui_uif_editmenu(name, menupos, exitkey);
#else

	//modifysettings(menupos, menupos, ITEM_VALUE, MENUNONVRAM, exitkey, 1);

	updateboard();
	updateboard2();			/* настройки валкодера и цветовой схемы дисплея. */
	//display2_needupdate();		/* возможно уже с новой цветовой схемой */
#endif /* WITHTOUCHGUI */
}

int hamradio_walkmenu_getgroupanme(const void * groupitem, char * buff, size_t count)
{
	const struct paramdefdef * const pd = (const struct paramdefdef *) groupitem;
	return local_snprintf_P(buff, count, "%s", pd->qlabel);
}

int hamradio_walkmenu_getparamanme(const void * paramitem, char * buff, size_t count)
{
	const struct paramdefdef * const pd = (const struct paramdefdef *) paramitem;
	return local_snprintf_P(buff, count, "%s", pd->qlabel);
}

int hamradio_walkmenu_getparamvalue(const void * paramitem, char * buff, size_t count)
{
	const struct paramdefdef * pd = (const struct paramdefdef *) paramitem;
	return param_format(pd, buff, count, param_getvalue(pd));
}

void hamradio_walkmenu(void * walkctx, void * (* groupcb)(void * walkctx, const void * groupitem), void (* itemcb)(void * walkctx, void * groupctx, const void * paramitem))
{
	uint_fast16_t menupos;
	for (menupos = 0; menupos < menutable_size(); ++ menupos)
	{
        const struct menudef * mp = & menutable [menupos];
        if (ismenukinddp(mp->pd, ITEM_GROUP) == 0)
        	continue;
        const struct menudef * const mpgroup = mp ++;	/* группа */
        void * const groupctx = groupcb ? (* groupcb)(walkctx, mpgroup->pd) : NULL;
        //PRINTF("group: %s\n", mpgroup->pd->qlabel);
        for (; mp < (menutable + menutable_size()) && ismenukinddp(mp->pd, ITEM_VALUE); ++ mp)
        {
            //PRINTF(" item: %s\n", mp->pd->qlabel);
        	if (itemcb)
        		(* itemcb)(walkctx, groupctx, mp->pd);
		}
        /* not an ITEM_VALUE */
        menupos = mp - menutable - 1;
	}

}

static void * print_menu_group(void * walkctx, const void * groupitem)
{
	char b [32];
	hamradio_walkmenu_getgroupanme(groupitem, b, ARRAY_SIZE(b));

	PRINTF("%s,,\n", b);
	return NULL;
}

static void print_menu_item(void * walkctx, void * groupctx, const void * paramitem)
{
	char b [32];
	hamradio_walkmenu_getparamanme(paramitem, b, ARRAY_SIZE(b));
	char v [32];
	hamradio_walkmenu_getparamvalue(paramitem, v, ARRAY_SIZE(v));

 	PRINTF(",,%s,=\"%s\n", b, v);
}

/* создание списка пунктов мею для получения шаблона документа */
static void menu_print(void)
{
	hamradio_walkmenu(NULL, print_menu_group, print_menu_item);
}

#else // WITHMENU

static const uint_fast8_t ginmenu0 = 0;

void hamradio_walkmenu(void * walkctx, void * (* groupcb)(void * walkctx), void * (* itemcb)(void * walkctx, void * groupctx))
{

}

const struct paramdefdef * const * getmiddlemenu_cw(unsigned * size)
{
	static const struct paramdefdef * const middlemenu [] =
	{
			& xgdummy,
	};

	* size = ARRAY_SIZE(middlemenu);
	return middlemenu;
}
const struct paramdefdef * const * getmiddlemenu_ssb(unsigned * size)
{
	static const struct paramdefdef * const middlemenu [] =
	{
			& xgdummy,
	};

	* size = ARRAY_SIZE(middlemenu);
	return middlemenu;
}
const struct paramdefdef * const * getmiddlemenu_am(unsigned * size)
{
	static const struct paramdefdef * const middlemenu [] =
	{
			& xgdummy,
	};

	* size = ARRAY_SIZE(middlemenu);
	return middlemenu;
}
const struct paramdefdef * const * getmiddlemenu_nfm(unsigned * size)
{
	static const struct paramdefdef * const middlemenu [] =
	{
			& xgdummy,
	};

	* size = ARRAY_SIZE(middlemenu);
	return middlemenu;
}
const struct paramdefdef * const * getmiddlemenu_digi(unsigned * size)
{
	static const struct paramdefdef * const middlemenu [] =
	{
			& xgdummy,
	};

	* size = ARRAY_SIZE(middlemenu);
	return middlemenu;
}
const struct paramdefdef * const * getmiddlemenu_wfm(unsigned * size)
{
	static const struct paramdefdef * const middlemenu [] =
	{
			& xgdummy,
	};

	* size = ARRAY_SIZE(middlemenu);
	return middlemenu;
}

static void
uif_key_click_menubyname(const char * name, uint_fast8_t exitkey)
{
}

#endif // WITHMENU

#if MULTIVFO

static void dispvfocode(
	uint_fast8_t vco,
	uint_fast8_t top
	)
{
	char label [16];

	local_snprintf_P(label, sizeof label / sizeof label [0],
		top ?
			PSTR("VFO #%d HIGH") :
			PSTR("VFO #%d LOW"),
		vco
		);


	uint_fast32_t freq = getvcoranges(vco, top);
	synth_lo1_setfreq(0, freq, getlo1div(gtx));

	display_text(db, 0, 1, label, (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
	display_menu_digit(db, 0, 0, freq, 9, 3, 0);

}

/* Работа в режиме настройки VCO */
static void vfoallignment(void)
{
	uint_fast8_t vfo = 0;
	uint_fast8_t top = 0;
	dispvfocode(vfo, top);
	for (;;)
	{
		uint_fast16_t kbch;
		uint_fast8_t kbready;

		processmessages(& kbch, & kbready);

		if (kbready != 0)
		{
			switch (kbch)
			{
			case KBD_CODE_MENU:
			case KBD_CODE_DISPMODE:
				/* выход из режима настройки */
				return;
			case KBD_CODE_MODE:	/* та же клавиша, что и переключение режимов */
				top = ! top;	// переключение верхний или нижний край диапазона
				goto menuswitch;


			case KBD_CODE_BAND_DOWN:
			case KBD_CODE_MENU_DOWN:
				/* переход на предыдущий пункт меню */
				vfo = calc_prev(vfo, 0, HYBRID_NVFOS - 1);
				goto menuswitch;

			case KBD_CODE_BAND_UP:
			case KBD_CODE_MENU_UP:
				/* переход на следующий пункт меню */
				vfo = calc_next(vfo, 0, HYBRID_NVFOS - 1);

			menuswitch:
				dispvfocode(vfo, top);
				break;
			}
		}
	}
	return;
}
#endif // MULTIVFO

#if WITHKEYBOARD
/* возврат ненуля - было какое-либо нажатие,
	требуется обновление дисплея */
static uint_fast8_t
process_key_menuset0(uint_fast8_t kbch)
{
	switch (kbch)
	{

#if WITHSPLIT

	case KBD_CODE_SPLIT:
		/* Переключение VFO
			 - не вызывает сохранение состояния диапазона */
		if (gsplitmode == VFOMODES_VFOINIT)
			uif_key_spliton(0);		// включение SPLIT без смещения
		else
			uif_key_click_a_ex_b();	// Обмен VFO
		return 1;	// требуется обновление индикатора

	case KBD_CODE_SPLIT_HOLDED:
		/* Выход из режима переключение VFO - возврат к простой настройке
			 - не вызывает сохранение состояния диапазона */
		if (gsplitmode == VFOMODES_VFOINIT)
			uif_key_spliton(1);
		else
			uif_key_splitoff();
		return 1;	// требуется обновление индикатора

#elif WITHSPLITEX

	case KBD_CODE_SPLIT:
		/* Переключение VFO
			 - не вызывает сохранение состояния диапазона */
		if (gsplitmode == VFOMODES_VFOINIT)
			uif_key_spliton(0);
		else
			uif_key_splitoff();
		return 1;	// требуется обновление индикатора

	case KBD_CODE_SPLIT_HOLDED:
		/* Переключение VFO
			 - не вызывает сохранение состояния диапазона */
		uif_key_spliton(1);
		return 1;	// требуется обновление индикатора

#endif /* WITHSPLIT */

	case KBD_CODE_A_EX_B:
		uif_key_click_a_ex_b();
		return 1;	// требуется обновление индикатора

	case KBD_CODE_A_EQ_B:
		uif_key_click_b_from_a();
		return 1;	// требуется обновление индикатора

	case KBD_CODE_BAND_UP:
		/* переход на следующий (с большей частотой) диапазон или на шаг general coverage */
		uif_key_click_bandup();
		return 1;	// требуется обновление индикатора

	case KBD_CODE_BAND_DOWN:
		/* переход на предыдущий (с меньшей частотой) диапазон или на шаг general coverage */
		uif_key_click_banddown();
		return 1;	// требуется обновление индикатора

#if WITHENCODER_1F
	case KBD_ENC1F_PRESS:
		bring_enc1f();
		enc1f_sel = calc_next(enc1f_sel, 0, ARRAY_SIZE(enclabelsENC1FN) - 1);
		return 1;	// требуется обновление индикатора

	case KBD_ENC1F_HOLD:
		bring_enc1f();
		//enc1f_sel = calc_next(enc1f_sel, 0, ARRAY_SIZE(enclabelsENC1FN) - 1);
		return 1;	// требуется обновление индикатора
#endif /* WITHENCODER_1F */

#if WITHENCODER_2F
	case KBD_ENC2F_PRESS:
		bring_enc2f();
		enc2f_sel = calc_next(enc2f_sel, 0, ARRAY_SIZE(enclabelsENC2FN) - 1);
		return 1;	// требуется обновление индикатора

	case KBD_ENC2F_HOLD:
		bring_enc2f();
		//enc2f_sel = calc_next(enc2f_sel, 0, ARRAY_SIZE(enclabelsENC2FN) - 1);
		return 1;	// требуется обновление индикатора
#endif /* WITHENCODER_2F */

#if WITHENCODER_3F
	case KBD_ENC3F_PRESS:
		bring_enc3f();
		enc3f_sel = calc_next(enc3f_sel, 0, ARRAY_SIZE(enclabelsENC3FN) - 1);
		return 1;	// требуется обновление индикатора

	case KBD_ENC3F_HOLD:
		bring_enc3f();
		//enc3f_sel = calc_next(enc3f_sel, 0, ARRAY_SIZE(enclabelsENC3FN) - 1);
		return 1;	// требуется обновление индикатора
#endif /* WITHENCODER_3F */

#if WITHENCODER_4F
	case KBD_ENC4F_PRESS:
		bring_enc4f();
		enc4f_sel = calc_next(enc4f_sel, 0, ARRAY_SIZE(enclabelsENC4FN) - 1);
		return 1;	// требуется обновление индикатора

	case KBD_ENC4F_HOLD:
		bring_enc4f();
		//enc4f_sel = calc_next(enc4f_sel, 0, ARRAY_SIZE(enclabelsENC4FN) - 1);
		return 1;	// требуется обновление индикатора
#endif /* WITHENCODER_4F */

#if WITHDIRECTBANDS

	case KBD_CODE_BAND_1M8:
		uif_key_click_bandjump(1800000L);
		return 1;	// требуется обновление индикатора
	case KBD_CODE_BAND_3M5:
		uif_key_click_bandjump(3500000L);
		return 1;	// требуется обновление индикатора
	case KBD_CODE_BAND_5M3:
		uif_key_click_bandjump(5351500L);
		return 1;	// требуется обновление индикатора
	case KBD_CODE_BAND_7M0:
		uif_key_click_bandjump(7000000L);
		return 1;	// требуется обновление индикатора
	case KBD_CODE_BAND_10M1:
		uif_key_click_bandjump(10100000L);
		return 1;	// требуется обновление индикатора
	case KBD_CODE_BAND_14M0:
		uif_key_click_bandjump(14000000L);
		return 1;	// требуется обновление индикатора
	case KBD_CODE_BAND_18M0:
		uif_key_click_bandjump(18068000L);
		return 1;	// требуется обновление индикатора
	case KBD_CODE_BAND_21M0:
		uif_key_click_bandjump(21000000L);
		return 1;	// требуется обновление индикатора
	case KBD_CODE_BAND_24M9:
		uif_key_click_bandjump(24890000L);
		return 1;	// требуется обновление индикатора
	case KBD_CODE_BAND_28M0:
		uif_key_click_bandjump(28000000L);
		return 1;	// требуется обновление индикатора
	case KBD_CODE_BAND_50M0:
		uif_key_click_bandjump(50100000L);
		return 1;	// требуется обновление индикатора
#endif /* WITHDIRECTBANDS */

	default:
		return 0;	// не требуется обновления индикатора
	}
}
#endif /* WITHKEYBOARD */

static int_fast32_t
getpower10(uint_fast8_t pos)
{
	int_fast32_t v = 1;
	while (pos --)
		v *= 10;

	return v;
}

/* проверка введённой вручную частоты на допустимость. */
static uint_fast8_t
freqvalid(
	int_fast32_t freq,
	uint_fast8_t tx			// могут накладываться дополнительные ограничения
	)
{
#if XVTR_R820T2
	if (get_ad936x_stream_status())
		return (freq >= XVRTUNE_BOTTOM && freq < TUNE_TOP);
	else
		return (freq >= TUNE_BOTTOM && freq < NOXVRTUNE_TOP);
#else
	return (freq >= TUNE_BOTTOM && freq < TUNE_TOP);	/* частота внутри допустимого диапазона */
#endif /* XVTR_R820T2 */
}

#if WITHKEYBOARD

/* возврат ненуля - было какое-либо нажатие, клавиша уже обработана
	требуется обновление дисплея */
static uint_fast8_t
process_key_menuset_common(uint_fast8_t kbch)
{
#if WITHAUTOTUNER
	//const vindex_t b = getvfoindex(bi);
#endif /* WITHAUTOTUNER */
	switch (kbch)
	{
	case KBD_CODE_BW:
#if WITHIF4DSP
		/* Переключение фильтров
			 - не вызывает сохранение состояния диапазона */
		uif_key_changebw();
#else /* WITHIF4DSP */
		/* Переключение фильтров
			 - не вызывает сохранение состояния диапазона */
		uif_key_changefilter();
#endif /* WITHIF4DSP */
		return 1;	/* клавиша уже обработана */

#if WITHIF4DSP
	case KBD_CODE_NR:
		/* Переключение режтима шумоподавления
			 - не вызывает сохранение состояния диапазона */
		uif_key_changenr();
		return 1;	/* клавиша уже обработана */
#endif /* WITHIF4DSP */

#if ! WITHAGCMODENONE
	case KBD_CODE_AGC:
		/* AGC mode switch
			 - не вызывает сохранение состояния диапазона */
		uif_key_click_agcmode();
		return 1;	/* клавиша уже обработана */
#endif /* ! WITHAGCMODENONE */

#if WITHANTSELECT1RX
	/* только приемная антенна может подключаться */
	case KBD_CODE_ANTENNA:
		/* RX Antenna switch */
		uif_key_next_rxantenna();
		return 1;	/* клавиша уже обработана */

#elif WITHANTSELECTRX
		/* одна из двух антенн и приемная */
	case KBD_CODE_ANTENNA_HOLDED:
		/* Antenna switch */
		uif_key_next_antenna();
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_ANTENNA:
		/* RX Antenna switch */
		uif_key_next_rxantenna();
		return 1;	/* клавиша уже обработана */

#elif WITHANTSELECT2
	case KBD_CODE_ANTENNA_HOLDED:
		/* ручной/автоматический выбор антенны */
		uif_key_next_autoantmode();
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_ANTENNA:
		/* Antenna switch */
		uif_key_next_antenna();
		return 1;	/* клавиша уже обработана */

#elif WITHANTSELECT
		/* одна из двух антенн */
	case KBD_CODE_ANTENNA:
		/* Antenna switch
			 - не вызывает сохранение состояния диапазона */
		uif_key_next_antenna();
		return 1;	/* клавиша уже обработана */
#endif /* WITHANTSELECT || WITHANTSELECTRX */

#if WITHUSEAUDIOREC
	case KBD_CODE_RECORDTOGGLE:	// SD CARD audio recording
		sdcardtoggle();
		return 1;	/* клавиша уже обработана */
	case KBD_CODE_RECORD_HOLDED:
		sdcardformat();
		return 1;	/* клавиша уже обработана */
#endif /* WITHUSEAUDIOREC */

#if WITHENCODER2
	#if WITHTOUCHGUI
		case KBD_ENC2_PRESS:
			gui_put_keyb_code(KBD_ENC2_PRESS);
			if (! encoder2_redirect)
				uif_encoder2_press();
			return 0;
		case KBD_ENC2_HOLD:
			gui_put_keyb_code(KBD_ENC2_HOLD);
			if (! encoder2_redirect)
				uif_encoder2_hold();
			return 0;
	#else
		case KBD_ENC2_PRESS:
			uif_encoder2_press();
			return 1;
		case KBD_ENC2_HOLD:
			uif_encoder2_hold();
			return 1;
	#endif /* WITHTOUCHGUI */
#endif /* WITHENCODER2 */

#if WITHTX

	case KBD_CODE_MOX:
		/* выключить режим настройки или приём/передача */
		uif_key_moxclick();
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_TXTUNE:
		/* включить режим настройки */
		uif_key_tune();
		return 1;	/* клавиша уже обработана */

#if WITHAUTOTUNER
	case KBD_CODE_ATUSTART:
		uif_key_atunerstart();
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_ATUBYPASS:
		uif_key_bypasstoggle();
		return 1;	/* клавиша уже обработана */
#endif /* WITHAUTOTUNER */

#if WITHVOX
	case KBD_CODE_VOXTOGGLE:
		uif_key_voxtoggle();
		return 1;	/* клавиша уже обработана */
#endif /* WITHVOX */
#if WITHELKEY
	case KBD_CODE_BKIN:
		uif_key_bkintoggle();
		return 1;
#endif /* WITHELKEY */

#if WITHELKEY

	case KBD_CODE_CWMSG1:
		uif_key_sendcw("CQ UA1ATD/P RR0106");
		return 1;
	case KBD_CODE_CWMSG2:
		uif_key_sendcw("UA1ATD/P");
		return 1;
	case KBD_CODE_CWMSG3:
		uif_key_sendcw("CQ DE UA1ATD/P UA1ATD/P RR0106");
		return 1;
	case KBD_CODE_CWMSG4:
		uif_key_sendcw("UA1ATD/P UA1ATD/P");
		return 1;
#endif /* WITHELKEY */

//	case KBD_CODE_MAX:
//		bring_swr();
//		return 1;

#if WITHIF4DSP && WITHUSBHW && WITHUSBUAC && WITHDATAMODE
	case KBD_CODE_DATATOGGLE:
		uif_key_click_datamode();
		return 1;	/* клавиша уже обработана */
#endif /* WITHIF4DSP && WITHUSBHW && WITHUSBUAC && WITHDATAMODE */

#endif /* WITHTX */

#if WITHLFM
	case KBD_CODE_DWATCHTOGGLE:
		lfm_run();
		return 1;	/* клавиша уже обработана */
#elif WITHUSEDUALWATCH
	case KBD_CODE_DWATCHHOLD:
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_DWATCHTOGGLE:
		uif_key_mainsubrx();
		return 1;	/* клавиша уже обработана */

#endif /* WITHUSEDUALWATCH */

#if WITHAMHIGHKBDADJ
	case KBD_CODE_AMBANDPASSUP:
		uif_key_click_amfmbandpassup();
		return 1;	/* клавиша уже обработана */
	case KBD_CODE_AMBANDPASSDOWN:
		uif_key_click_amfmbandpassdown();
		return 1;	/* клавиша уже обработана */
#endif /* WITHAMHIGHKBDADJ */

#if WITHSPKMUTE
	case KBD_CODE_LDSPTGL:
		uif_key_loudsp();
		return 1;	/* клавиша уже обработана */
#endif /* WITHSPKMUTE */

	case KBD_CODE_ATT:
		/* переключение режима аттенюатора  */
		uif_key_click_attenuator();
		return 1;	/* клавиша уже обработана */

#if ! WITHONEATTONEAMP
	case KBD_CODE_PAMP:
		/* переключение режима предусилителя  */
		uif_key_click_pamp();
		return 1;	/* клавиша уже обработана */
#endif /* ! WITHONEATTONEAMP */

	case KBD_CODE_MODE:
		/* переход по "столбцу" режимов - быстрое нажатие */
		/* switch to next moderow */
		uif_key_click_moderow();
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_MODEMOD:
		/* переход по "строке" режимов - удержанное нажатие */
		// step to next modecol
		uif_key_hold_modecol();
		return 1;	/* клавиша уже обработана */

#if WITHDIRECTBANDS

	case KBD_CODE_MODE_0:
		/* переход по "столбцу" режимов - быстрое нажатие */
		/* switch to next moderow */
		uif_key_click_moderows(0);
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_MODE_1:
		/* переход по "столбцу" режимов - быстрое нажатие */
		/* switch to next moderow */
		uif_key_click_moderows(1);
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_MODE_2:
		/* переход по "столбцу" режимов - быстрое нажатие */
		/* switch to next moderow */
		uif_key_click_moderows(2);
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_MODE_3:
		/* переход по "столбцу" режимов - быстрое нажатие */
		/* switch to next moderow */
		uif_key_click_moderows(3);
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_MODEMOD_0:
		/* переход по "строке" режимов - удержанное нажатие */
		// step to next modecol
		uif_key_hold_modecols(0);
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_MODEMOD_1:
		/* переход по "строке" режимов - удержанное нажатие */
		// step to next modecol
		uif_key_hold_modecols(1);
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_MODEMOD_2:
		/* переход по "строке" режимов - удержанное нажатие */
		// step to next modecol
		uif_key_hold_modecols(2);
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_MODEMOD_3:
		/* переход по "строке" режимов - удержанное нажатие */
		// step to next modecol
		uif_key_hold_modecols(3);
		return 1;	/* клавиша уже обработана */

#endif /* WITHDIRECTBANDS */

	case KBD_CODE_LOCK:
		/* блокировка валкодера
			 - не вызывает сохранение состояния диапазона */
		uif_key_lockencoder();
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_POWEROFF:
		uif_pwbutton_press();
		return 1;

	case KBD_CODE_LOCK_HOLDED:
#if WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC
		/* запись видимого изображения */
		display_snapshot_req();
		return 1;
#endif /* WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC */
#if WITHLCDBACKLIGHTOFF
		{
			dimmmode = calc_next(dimmmode, 0, 1);
			//display2_needupdate();
			updateboard();
		}
#endif /* WITHLCDBACKLIGHTOFF */
		return 1;	/* клавиша уже обработана */

#if WITHBCBANDS
	case KBD_CODE_GEN_HAM:
		uif_key_genham();
		return 1;	/* клавиша уже обработана */
#endif /* WITHBCBANDS */


#if WITHUSEFAST
	case KBD_CODE_USEFAST:
		uif_key_usefast();
		return 1;	/* клавиша уже обработана */
#endif /* WITHUSEFAST */
#if WITHPOWERLPHP
	case KBD_CODE_PWRTOGGLE:
		/* переключение режима мощности (для sw2012sf) */
		uif_key_click_pwr();
		return 1;	/* клавиша уже обработана */
#endif /* WITHPOWERLPHP */

#if WITHNOTCHONOFF
	case KBD_CODE_NOTCHTOGGLE:
		/* переключение NOTCH фильтра */
		uif_key_click_notch();
		return 1;	/* клавиша уже обработана */
#endif /* WITHNOTCHONOFF */

#if WITHNOTCHFREQ
	case KBD_CODE_NOTCHTOGGLE:
		/* переключение NOTCH фильтра */
		uif_key_click_notch();
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_NOTCHFREQ:
		uif_key_click_menubyname("NTCH FRQ", KBD_CODE_NOTCHTOGGLE);
		return 1;
#endif /* WITHNOTCHFREQ */


#if WITHAUTOTUNER && KEYB_UA3DKC
	case KBD_CODE_TUNERTYPE:	// переключение типа согласующего устройства
		tunertype = calc_next(tunertype, 0, KSCH_COUNT - 1);
		save_i8(OFFSETOF(struct nvmap, bandgroups [bg].tunertype), tunertype);
		updateboard_tuner();
		return 1;	// требуется обновление индикатора

	case KBD_CODE_CAP_UP:	// увеличение емкости
		// todo: добавить учет включенной антенны
		tunercap = calc_next(tunercap, CMIN, CMAX);
		save_i8(OFFSETOF(struct nvmap, bandgroups [bg].tunercap), tunercap);
		updateboard_tuner();
		return 1;	// требуется обновление индикатора

	case KBD_CODE_CAP_DOWN:	// уменьшение емкости
		// todo: добавить учет включенной антенны
		tunercap = calc_prev(tunercap, CMIN, CMAX);
		save_i8(OFFSETOF(struct nvmap, bandgroups [bg].tunercap), tunercap);
		updateboard_tuner();
		return 1;	// требуется обновление индикатора

	case KBD_CODE_IND_UP:	// увеличение индуктивности
		// todo: добавить учет включенной антенны
		tunerind = calc_next(tunerind, LMIN, LMAX);
		save_i8(OFFSETOF(struct nvmap, bandgroups [bg].tunerind), tunerind);
		updateboard_tuner();
		return 1;	// требуется обновление индикатора

	case KBD_CODE_IND_DOWN:	// уменьшение индуктивности
		// todo: добавить учет включенной антенны
		tunerind = calc_prev(tunerind, LMIN, LMAX);
		save_i8(OFFSETOF(struct nvmap, bandgroups [bg].tunerind), tunerind);
		updateboard_tuner();
		return 1;	// требуется обновление индикатора
#endif /* WITHAUTOTUNER && KEYB_UA3DKC */

	default:
		return 0;	/* клавиша не обработана */
	}
}

#if WITHWAVPLAYER || WITHSENDWAV
void playhandler(uint8_t code)
{
	if (code >= 1 && code < (sizeof loopnames / sizeof loopnames [0]))
		playwavfile(loopnames [code]);
	else
		playwavstop();

}
#endif /* WITHWAVPLAYER || WITHSENDWAV */

/* возврат ненуля - было какое-либо нажатие,
	требуется обновление дисплея и состояния аппаратуры */
static uint_fast8_t
processmainloopkeyboard(inputevent_t * ev)
{
#if WITHTOUCHGUI
	if (keyboard_redirect)
	{
		if (ev->frontkeyevent.kbready == 0)
			return 0;
		ev->frontkeyevent.kbready = 0;
		gui_put_keyb_code(ev->frontkeyevent.kbch);
		return 0;
	}
#endif

#if WITHDIRECTFREQENER
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const int DISPLAY_LEFTBLINKPOS = fullwidth - 1;

	if (editfreqmode != 0)
	{
		if (ev->frontkeyevent.kbready == 0)
			return 0;
		ev->frontkeyevent.kbready = 0;
		const char c = front_getnumpad(ev->frontkeyevent.kbch);
		if (ev->frontkeyevent.kbch == KBD_CODE_ENTERFREQDONE)
		{
			editfreqmode = 0;
			return 1;
		}
		if (c == '#' && (int) blinkpos < DISPLAY_LEFTBLINKPOS)
		{
			blinkpos += 1;	/* перемещаемся на одну позицию левее */
			updateboard();
			return 1;
		}
		if (c >= '0' && c <= '9')
		{
			const int_fast32_t m = getpower10(blinkpos + rj);
			const int_fast32_t m10 = m * 10;
			editfreq = editfreq / m10 * m10 + (c - '0') * m;
			if (blinkpos != 0)
				-- blinkpos;	/* перемещаемся на одну позицию правее */
			else if (freqvalid(editfreq, gtx))
			{
				const uint_fast8_t bi = getbankindex_tx(gtx);
				vindex_t vi = getvfoindex(bi);
				gfreqs [bi] = editfreq;
				editfreqmode = 0;
				storebandfreq(vi, bi);		/* сохранение частоты в текущем VFO */
				updateboard();
			}
			else
			{
				/* опять к начальному состоянию */
				blinkpos = DISPLAY_LEFTBLINKPOS;		/* позиция курсора */
				editfreqmode = 1;
				editfreq = gfreqs [getbankindex_tx(gtx)];
			}
			return 1;
		}
	}
	else if (ev->frontkeyevent.kbready && ev->frontkeyevent.kbch == KBD_CODE_ENTERFREQ)
	{
		ev->frontkeyevent.kbready = 0;
		blinkpos = DISPLAY_LEFTBLINKPOS;		/* позиция курсора */
		editfreqmode = 1;
		editfreq = gfreqs [getbankindex_tx(gtx)];
		return 1;
	}
#endif /* WITHDIRECTFREQENER */

	if (! ev->frontkeyevent.kbready)
		return 0;
	ev->frontkeyevent.kbready = 0;

	switch (ev->frontkeyevent.kbch)
	{
	default:
		break;

	case KBD_CODE_MENU:
		/* Вход в меню
			 - не вызывает сохранение состояния диапазона */

#if WITHMENU && ! WITHTOUCHGUI
		ASSERT(ginmenu0 == 0);
		{
			uint_fast16_t menupos = loadvfy16up(RMT_GROUP_BASE, 0, menutable_size() - 1, 0);
			const struct menudef * mpgroup = menutable + menupos;
			gmenulevel = 0;
			setinmenu(1, mpgroup);
		}

	#if WITHAUTOTUNER
		if (txreq_getreqautotune(& txreqst0) != 0)
			return 1;
	#endif /* WITHAUTOTUNER */
	#if defined (RTC1_TYPE)
		getstamprtc();
	#endif /* defined (RTC1_TYPE) */
		return 1;	// требуется обновление индикатора
#elif WITHTOUCHGUI
		gui_open_sys_menu();
		return 0;
#else
		return 0;
#endif //WITHMENU && ! WITHTOUCHGUI

	case KBD_CODE_BKIN_HOLDED:
#if WITHTOUCHGUI && WITHGUIDEBUG
		gui_open_debug_window();
#endif /* WITHTOUCHGUI && WITHGUIDEBUG*/
		break;

	case KBD_CODE_DISPMODE:
#if ! WITHTOUCHGUI
		if (display_getpagesmax() != 0)
		{
			/* Альтернативные функции кнопок - "Fn"
				 - не вызывает сохранение состояния диапазона */
			gmenuset = calc_next(gmenuset, 0, display_getpagesmax());
			save_i8(RMT_MENUSET_BASE, gmenuset);
			//display2_needupdate();
			ev->frontkeyevent.kbready = 0;
			return 1;	// требуется обновление индикатора
		}
#endif /* ! WITHTOUCHGUI */
		return 0;	// не требуется обновление индикатора

#if WITHMENU
#if WITHIF4DSP
	case KBD_CODE_MENU_CWSPEED:
		uif_key_click_menubyname("CW SPEED", KBD_CODE_MENU_CWSPEED);
		return 1;	/* клавиша уже обработана */

	case KBD_CODE_IFSHIFT:
		uif_key_click_menubyname("IF SHIFT", KBD_CODE_IFSHIFT);
		return 1;	/* клавиша уже обработана */

#endif /* WITHIF4DSP */
#endif /* WITHMENU */
	}

#if WITHWAVPLAYER || WITHSENDWAV
	switch (kbch)
	{
	case KBD_CODE_PLAYFILE1:
		playhandler(1);
		return 1;	/* клавиша уже обработана */
	case KBD_CODE_PLAYFILE2:
		playhandler(2);
		return 1;	/* клавиша уже обработана */
	case KBD_CODE_PLAYFILE3:
		playhandler(3);
		return 1;	/* клавиша уже обработана */
	case KBD_CODE_PLAYFILE4:
		playhandler(4);
		return 1;	/* клавиша уже обработана */
	case KBD_CODE_PLAYFILE5:
		playhandler(5);
		return 1;	/* клавиша уже обработана */
	case KBD_CODE_PLAYSTOP:
		playwavstop();
		return 1;	/* клавиша уже обработана */
#if ! WITHPOTAFGAIN
	case KBD_CODE_PLAYLOUD:	// громче
		if (param_rotate(& xafgain1, + 1))
		{
			updateboard();
		}
		return 1;
	case KBD_CODE_PLAYQUITE:	// тише
		if (param_rotate(& xafgain1, - 1))
		{
			updateboard();
		}
		return 1;
#endif /* ! WITHPOTAFGAIN */
	}
#endif /* WITHWAVPLAYER */

	if (process_key_menuset0(ev->frontkeyevent.kbch))
		return 1;
	if (process_key_menuset_common(ev->frontkeyevent.kbch))
		return 1;	/* клавиша уже обработана */
	return 0;	// не требуется обновления индикатора
}

#else /* WITHKEYBOARD */

static uint_fast8_t
processmainloopkeyboard(inputevent_t * ev)
{
	return 0;
}

#endif /* WITHKEYBOARD */


// Зависящий от режима запрос на передачу
// break-in, vox
static uint_fast8_t modetxrequest(void)
{
	const uint_fast8_t bi = getbankindex_tx(1);		// TX bankindex
	const uint_fast8_t submode = getsubmode(bi);	// какая модуляция в режиме передачи
	const struct modetempl * const pmodet = getmodetempl(submode);
	if (gbkinenable && pmodet->txcw && (pmodet->wbkin || (pmodet->abkin && gcwssbtx)) && vox_getbkin())
		return 1;
	if (gvoxenable && pmodet->vox && vox_getptt())
		return 1;

	return 0;
}

/* проверка, есть ли хоть на одном из входов продетектированный запрос перехода на пережачу */
void txreq_scaninputs(txreq_t * txreqp)
{
	PLIST_ENTRY t;
	for (t = txreqp->edgepins.Blink; t != & txreqp->edgepins; t = t->Blink)
	{
		edgepin_t * const p = CONTAINING_RECORD(t, edgepin_t, item);
		const uint_fast8_t f = p->getpin();
		if (f)
		{
			if (p->prevstate == 0)
			{
				p->req = 1;
				p->posedge = 1;
				p->negedge = 0;
			}
		}
		else
		{
			if (p->prevstate != 0)
			{
				p->req = 0;
				p->posedge = 0;
				p->negedge = 1;
			}
		}
		p->prevstate = f;
	}
}

void edgepin_initialize(LIST_ENTRY * list, edgepin_t * egp, uint_fast8_t (* fn)(void))
{
	egp->getpin = fn;
	egp->prevstate = fn();
	egp->posedge = 0;
	egp->negedge = 0;
	egp->req = 0;

	InsertTailList(list, & egp->item);
}

void txreq_initialize(txreq_t * txreqp)
{
	InitializeListHead(& txreqp->edgepins);

	edgepin_initialize(& txreqp->edgepins, & txreqp->edgphandptt, hardware_get_ptt);// тангента, педаль;
	edgepin_initialize(& txreqp->edgepins, & txreqp->edgpcathwptt, cat_get_hwptt);	// CAT hw signals rts/dtr
	edgepin_initialize(& txreqp->edgepins, & txreqp->edgpelkeyptt, modetxrequest);	// break-in signals
	edgepin_initialize(& txreqp->edgepins, & txreqp->edgpexttune, hardware_get_tune);	// внешний запрос на выдачу несущей

	txreqp->state = TXREQST_RX;
}

/* переход на приём, сброс всех запросов */
static void
txreq_rx0(txreq_t * txreqp, const char * label)
{
	if (label)
		bring_swr(label);
	txreqp->state = TXREQST_RX;
	// сбросить edge detectors
	PLIST_ENTRY t;
	for (t = txreqp->edgepins.Blink; t != & txreqp->edgepins; t = t->Blink)
	{
		edgepin_t * const p = CONTAINING_RECORD(t, edgepin_t, item);
		p->posedge = 0;
		p->negedge = 0;
		p->req = 0;
	}
}

/* Установка сиквенсору запроса на передачу.	*/
// проверка защит и запросов
static void
txreq_process0(txreq_t * txreqp)
{
#if WITHTX
	// todo: установка запроса на пердачу может быть вызвана ранее
	txreq_scaninputs(txreqp);
	const uint_fast8_t txreq =
			txreqp->edgphandptt.req ||
			txreqp->edgpcathwptt.req ||
			txreqp->edgpelkeyptt.req ||
			0;
	const uint_fast8_t tunereq =
			txreqp->edgpexttune.req ||
			0;
	const uint_fast8_t rxreq =
			txreqp->edgphandptt.negedge ||
			txreqp->edgpcathwptt.negedge ||
			txreqp->edgpelkeyptt.negedge ||
			txreqp->edgpexttune.negedge ||
			0;

	if (0)
		;
	else if (rxreq)
		txreq_rx0(txreqp, NULL);	// просто переход на приём
	else if (tunereq)
		txreqp->state = TXREQST_TXTONE;
	else if (txreq)
		txreqp->state = TXREQST_TX;

#if WITHSENDWAV
	if (isplayfile())
	{
		txreq_mox0(txreqp);
	}
#endif /* WITHSENDWAV */
#if WITHBEACON
	if (beacon_get_ptt())
	{
		txreq_mox0(txreqp);
	}
#endif	/* WITHCAT */
#if WITHMODEM
	if (modem_get_ptt())
	{
		txreq = 1;
	}
#endif	/* WITHMODEM */
//	if (getactualtune())
//	{
//		tunreq = 1;
//	}

	if (txreq_get_tx(txreqp) && gtxtot != 0 && gtxtimer >= gtxtot)
	{
		txreq_rx0(txreqp, "TOT");
	}
	else if (txreq_get_tx(txreqp) && hardware_get_txdisable())
	{
		txreq_rx0(txreqp, "DIS");
	}
#if (WITHTHERMOLEVEL || WITHTHERMOLEVEL2)
	//PRINTF("gheatprot=%d,t=%d,max=%d\n", gheatprot, hamradio_get_PAtemp_value(), (int) gtempvmax * 10);
	else if (txreq_get_tx(txreqp) && gheatprot != 0 && hamradio_get_PAtemp_value() >= (int) gtempvmax * 10) // Градусы в десятых долях
	{
		txreq_rx0(txreqp, "OVH");
	}
#endif /* (WITHTHERMOLEVEL || WITHTHERMOLEVEL2) */
#if (WITHSWRMTR || WITHSHOWSWRPWR) && WITHTX
	else if (txreq_get_tx(txreqp) && getactualdownpower(& txreqst0) == 0 && gswrprot != 0)
	{
		//PRINTF("1 gswrprot=%d,t=%d,swr=%d\n", gswrprot, getactualdownpower(& txreqst0) == 0, get_swr_cached(4 * SWRMIN));
		if (get_swr_cached(4 * SWRMIN) >= (4 * SWRMIN))	// SWR >= 4.0
		{
			txreq_rx0(txreqp, "SWR");
		}
	}
#endif /* (WITHSWRMTR || WITHSHOWSWRPWR) */

#endif /* WITHTX */
}

void
txreq_process(void)
{
	txreq_t * const txreqp = & txreqst0;

	const uint_fast8_t oldhint = txreq_gethint(txreqp);
	txreq_process0(txreqp);	// проверка защит и запросов
	seq_txrequest(txreq_get_tx(txreqp));
	if (oldhint != txreq_gethint(txreqp) || gtx != seq_get_txstate())
		updateboard();	/* полная перенастройка (как после смены режима) */
}

uint_fast8_t txreq_gethint(const txreq_t * txreqp)
{
	return (uint_fast8_t) txreqp->state;
}
// Выход из режима - txreq_rx
void txreq_reqautotune(txreq_t * txreqp, uint_fast8_t v)
{
	const uint_fast8_t oldhint = txreq_gethint(txreqp);
	txreqp->state = v ? TXREQST_TXAUTOTUNE : txreqp->state;
	txreq_process0(txreqp);	// проверка защит и запросов
	if (oldhint != txreq_gethint(txreqp))
		updateboard();	/* полная перенастройка (как после смены режима) */
}

uint_fast8_t txreq_getreqautotune(const txreq_t * txreqp)
{
	return txreqp->state == TXREQST_TXAUTOTUNE;
}

void txreq_txtone(txreq_t * txreqp)
{
	const uint_fast8_t oldhint = txreq_gethint(txreqp);
	txreqp->state = TXREQST_TXTONE;
	txreq_process0(txreqp);	// проверка защит и запросов
	if (oldhint != txreq_gethint(txreqp))
		updateboard();	/* полная перенастройка (как после смены режима) */
}
/* возвращаем не-0, если есть запрос на tune от пользователя или CAT */
uint_fast8_t txreq_gettxtone(const txreq_t * txreqp)
{
	return txreqp->state == TXREQST_TXTONE;
}

void txreq_mox(txreq_t * txreqp)
{
	const uint_fast8_t oldhint = txreq_gethint(txreqp);
	txreqp->state = TXREQST_TX;
	txreq_process0(txreqp);	// проверка защит и запросов
	if (oldhint != txreq_gethint(txreqp))
		updateboard();	/* полная перенастройка (как после смены режима) */
}

uint_fast8_t txreq_get_tx(const txreq_t * txreqp)
{
	return txreqp->state != TXREQST_RX;
}

void txreq_rx(txreq_t * txreqp, const char * label)
{
	const uint_fast8_t oldhint = txreq_gethint(txreqp);
	txreq_rx0(txreqp, label);
	txreq_process0(txreqp);	// проверка защит и запросов
	if (oldhint != txreq_gethint(txreqp))
		updateboard();	/* полная перенастройка (как после смены режима) */
}

// передача с USB
void txreq_txdata(txreq_t * txreqp)
{
	const uint_fast8_t oldhint = txreq_gethint(txreqp);
	txreqp->state = TXREQST_TXDATA;
	txreq_process0(txreqp);	// проверка защит и запросов
	if (oldhint != txreq_gethint(txreqp))
		updateboard();	/* полная перенастройка (как после смены режима) */
}

uint_fast8_t txreq_gettxdata(const txreq_t * txreqp)
{
	return txreqp->state == TXREQST_TXDATA;
}

static void dpc_0p1_s_timer_fn(void * ctx)
{
	bringtimers();
	/* быстро меняющиеся значения с частым опорсом */
	doadcmirror();
	main_speed_diagnostics();
	looptests();		// Периодически вызывается в главном цикле - тесты
}

/* вызывается при запрещённых прерываниях. */
void
applowinitialize(void)
{
#if ! WITHRTOS

#if WITHCAT

	HARDWARE_CAT_INITIALIZE();

#endif /* WITHCAT */

#if WITHGNSS
	gnss_initialize();
#endif /* WITHGNSS */

#if WITHNMEA && WITHAUTOTUNER_UA1CEI

	nmeatuner_initialize();

#elif WITHNMEA

	nmeagnss_initialize();

#endif /* WITHNMEA */

#if WITHMODEM

	nmeamodem_initialize();

#endif /* WITHMODEM */

#if WITHWATCHDOG
	watchdog_initialize();	/* разрешение сторожевого таймера в устройстве */
#endif /* WITHWATCHDOG */

	buffers_initialize();

	txreq_initialize(& txreqst0);

#if WITHUSBHW
	if (bootloader_withusb())
		board_usb_initialize();		// USB device and host support
#endif /* WITHUSBHW */
	hardware_encoders_initialize();	//  todo: разобраться - вызов перенесен сюда из board_initialize - иначе не собирается под Cortex-A9.
	encoders_initialize();
#if WITHELKEY
	elkey_initialize();
#endif /* WITHELKEY */
	seq_initialize();
	vox_initialize();		/* подготовка работы задержек переключения приём-передача */
#if WITHSDHCHW
	hardware_sdhost_initialize();	/* если есть аппаратный контрлллер SD CARD */
#endif /* WITHSDHCHW */

	kbd_initialize();

#if WITHDEBUG
	dbg_puts_impl_P(PSTR("Most of hardware initialized.\n"));
#endif
	//for (;;);
	//hardware_cw_diagnostics_noirq(1, 1, 0);	// 'S'
	//board_testsound_enable(0);	// Выключить 1 кГц на самоконтроле
#endif /* ! WITHRTOS */
}

static uint_fast8_t
keyboard_test(void)
{
	uint_fast8_t n;

	// 1 second total
	for (n = 0; n < 100; ++ n)
	{
		if (kbd_is_tready() != 0)
			return 1;
		local_delay_ms(10);
	}
	return 0;
}

/* вызывается при разрешённых прерываниях. */
void initialize2(void)
{
#if ! LCDMODE_DUMMY
	gxdrawb_t dbv;	// framebuffer для выдачи диагностических сообщений
	gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
#endif /* ! LCDMODE_DUMMY */
	uint_fast8_t mclearnvram;

	//hardware_cw_diagnostics(0, 1, 0);	// 'D'

	PRINTF(PSTR("initialize2() started.\n"));
	// Инициализируем то что не получается иниитить в описании перменных.
#if WITHTX
	/* запись значений по умолчанию для корректировок мощности в завивимости от диапазона ФНЧ УМ */
	bandf2adjust_initialize();
#endif /* WITHTX */
#if WITHCAT
#ifdef WITHCATSPEED
	catbaudrate = findcatbaudrate(catbaudrate, WITHCATSPEED);
#else
	catbaudrate = findcatbaudrate(catbaudrate, 9600);
#endif
#endif /* WITHCAT */

	display_gpu_initialize();	// mdma/g2d/dma2d/gpu init

	display_hardware_initialize();
	display_reset();
	display_initialize();


	display2_initialize();	// проход по элементам с необходимостью инициализации
	//display2_needupdate();

	if (keyboard_test() == 0)
	{
		static const char msg  [] = "KBD fault";
#if WITHLCDBACKLIGHT
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
#endif /* WITHLCDBACKLIGHT */
#if ! LCDMODE_DUMMY
		display2_fillbg(& dbv);
		display_text(& dbv, 0, 0, msg, strlen(msg), (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
		colmain_nextfb();
#endif /*  ! LCDMODE_DUMMY */
		PRINTF(PSTR("%s\n"), msg);
		for (;;)
			;
	}
	PRINTF(PSTR("KBD ok\n"));

#if defined(NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING)

	//PRINTF(PSTR("initialize2: NVRAM initialization started.\n"));

	mclearnvram = kbd_get_ishold(KIF_ERASE) != 0;
	//extmenu = kbd_get_ishold(KIF_EXTMENU);

	// проверить работу - потом закомментарит.
	if (sizeof (struct nvmap) > (NVRAM_END + 1))
	{
		// в случае отсутствия превышения размера этот кусок и переменная не комптилируются
		void wrong_NVRAM_END(void);

#if WITHLCDBACKLIGHT
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
#endif /* WITHLCDBACKLIGHT */

#if ! LCDMODE_DUMMY
		char msg [32];
		local_snprintf_P(msg, ARRAY_SIZE(msg), "TOO LARGE nvmap %d", (int) sizeof (struct nvmap));

		display2_fillbg(& dbv);
		display_text(& dbv, 0, 1, msg, strlen(msg), (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
		colmain_nextfb();
#endif /* ! LCDMODE_DUMMY */

		wrong_NVRAM_END();
		//hardware_cw_diagnostics(0, 0, 0);	// 'S'
		for (;;)
			;
	}
	else
	{
		// отладочная печать размера памяти, требуемого для хранения конфигурации
		/*
		static const char msg  [] = "nvmap size";

		display_menu_digit(sizeof (struct nvmap), 9, 0, 0);
		display_text(& dbv, 0, 0, msg, strlen(msg), (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
		colmain_nextfb();


		unsigned i;
		for (i = 0; i < 50; ++ i)
		{
			local_delay_ms(20);
		}
		*/
	}

	nvram_initialize();	// write enable all nvram memory regions

#endif /* defined(NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING) */

	//PRINTF(PSTR("initialize2: NVRAM initialization passed.\n"));

#if HARDWARE_IGNORENONVRAM

#elif NVRAM_TYPE == NVRAM_TYPE_FM25XXXX

	//PRINTF(PSTR("initialize2: NVRAM autodetection start.\n"));

	const uint_fast16_t erasekey = geterasekey();
	uint_fast8_t ab = 0;
	const uint_fast8_t ABMAX = 2;
	// проверка сигнатуры привсех возможных ab
	for (ab = 0; ab < ABMAX; ++ ab)
	{
		nvram_set_abytes(ab);
		if (verifynvramsignature() == 0)
			break;
	}
	if (ab >= ABMAX)
	{
		mclearnvram = 2;	/* ни при одном ab сигнатура не совпала */
	}

	if (mclearnvram)
	{
		/* есть запрос на стирание памяти - получаем подтверждение от оператора. */
		if (mclearnvram == 1)
		{
			uint_fast8_t kbch;

#if WITHLCDBACKLIGHT
			board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
			board_update();
#endif /* WITHLCDBACKLIGHT */

#if ! LCDMODE_DUMMY
			static const char msg [] = "ERASE: Press SPL";
			display2_fillbg(& dbv);
			display_text(db, 0, 0, msg, strlen(msg), (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
			colmain_nextfb();
#endif /* ! LCDMODE_DUMMY */

			for (;;)
			{
				while (kbd_scan(& kbch) == 0)
				{
					kbd_pass();
					local_delay_ms(KBD_TICKS_PERIOD * 1000 / TICKS_FREQUENCY);
				}
				PRINTF("wkbch=0x%02X (%u)\n", (unsigned) kbch, (unsigned) kbch);
				if (kbch == erasekey)
					break;
			}
			//display2_needupdate();
		}
		/* есть запрос на стирание памяти - опять перебираем все ab */
		for (ab = 0; ab < ABMAX; ++ ab)
		{
			nvram_set_abytes(ab);
			/* стирание всей памяти */
			uint_least16_t i;
			for (i = 0; i < sizeof (struct nvmap); ++ i)
				save_i8(i, 0xFF);

			initnvrampattern();

			if (verifynvrampattern() == 0)
				break;						// прочиталось!
		}
		if (ab >= ABMAX)
		{
			// в случае неправильно работающего NVRAM зависаем
			PRINTF(PSTR("initialize2: NVRAM initialization: wrong NVRAM pattern in any address sizes.\n"));

#if WITHLCDBACKLIGHT
			board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
			board_update();
#endif /* WITHLCDBACKLIGHT */

#if ! LCDMODE_DUMMY
			char msg [32];
			local_snprintf_P(msg, ARRAY_SIZE(msg), "NVRAM fault1 %d", (int) (NVRAM_END + 1));
			display2_fillbg(& dbv);
			display_text(& dbv, 0, 0, msg, strlen(msg), (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
			display_text(& dbv, 10, 20, msg, strlen(msg), (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
			colmain_nextfb();
#endif /* ! LCDMODE_DUMMY */

			PRINTF(PSTR("NVRAM fault1\n"));
			for (;;)
				;
		}
#if WITHMENU
		defaultsettings();		/* загрузка в nvram установок по умолчанию */
#endif //WITHMENU
		initnvramsignature();
		//extmenu = 1;	/* сразу включаем инженерный режим - без перезагрузки доступны все пункты */
	}

#else /* NVRAM_TYPE == NVRAM_TYPE_FM25XXXX */

	//PRINTF(PSTR("initialize2: NVRAM(BKPSRAM/CPU EEPROM/SPI MEMORY) initialization: verify NVRAM signature.\n"));

	const uint_fast16_t erasekey = geterasekey();
	if (verifynvramsignature())
		mclearnvram = 2;

	//PRINTF(PSTR("initialize2: NVRAM initialization: work on NVRAM signature, mclearnvram=%d\n"), mclearnvram);

	if (mclearnvram != 0)
	{
		if (mclearnvram == 1)
		{
			uint_fast16_t kbch;

#if WITHLCDBACKLIGHT
			board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
			board_update();
#endif /* WITHLCDBACKLIGHT */

#if ! LCDMODE_DUMMY
			static const char msg [] = "ERASE: Press SPL";
			display2_fillbg(& dbv);
			display_text(& dbv, 0, 0, msg, strlen(msg), (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
			colmain_nextfb();
#endif /* ! LCDMODE_DUMMY */

			for (;;)
			{
				while (kbd_scan(& kbch) == 0)
				{
					kbd_pass();
					local_delay_ms(KBD_TICKS_PERIOD * 1000 / TICKS_FREQUENCY);
				}
				PRINTF("kbch=0x%02X (%u)\n", (unsigned) kbch, (unsigned) kbch);
				if (kbch == erasekey)
					break;
			}
			//display2_needupdate();
		}

		//PRINTF(PSTR("initialize2: NVRAM initialization: erase NVRAM.\n"));
		/* стирание всей памяти */
		uint_least16_t i;
		for (i = 0; i < sizeof (struct nvmap); ++ i)
			save_i8(i, 0xFF);

		//PRINTF(PSTR("initialize2: NVRAM initialization: write NVRAM pattern.\n"));
		initnvrampattern();
		//PRINTF(PSTR("initialize2: NVRAM initialization: verify NVRAM pattern.\n"));

		if (verifynvrampattern())
		{
			PRINTF(PSTR("initialize2: NVRAM initialization: wrong NVRAM pattern.\n"));
			// проверяем только что записанную сигнатуру
			// в случае неправильно работающего NVRAM зависаем

#if WITHLCDBACKLIGHT
			board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
			board_update();
#endif /* WITHLCDBACKLIGHT */

#if ! LCDMODE_DUMMY
			char msg [32];
			local_snprintf_P(msg, ARRAY_SIZE(msg), "NVRAM fault %d", (int) (NVRAM_END + 1));
			display2_fillbg(& dbv);
			display_text(& dbv, 0, 0, msg, strlen(msg), (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
			display_text(& dbv, 10, 20, msg, strlen(msg), (smallfont_height() + GRID2Y(1) - 1) / GRID2Y(1), & dbstylev);
			colmain_nextfb();
#endif /* ! LCDMODE_DUMMY */

			for (;;)
				;
		}

#if WITHMENU
		defaultsettings();		/* загрузка в nvram установок по умолчанию */
#endif //WITHMENU

		//PRINTF(PSTR("initialize2: NVRAM initialization: write NVRAM signature.\n"));
		initnvramsignature();
		//extmenu = 1;	/* сразу включаем инженерный режим - без перезагрузки доступны все пункты */
	}
	//blinkmain();
#endif /* NVRAM_TYPE == NVRAM_TYPE_FM25XXXX */

	(void) mclearnvram;

#if WITHDEBUG
	dbg_puts_impl_P(PSTR("initialize2: finished.\n"));
#endif
}

#if WITHAUTOTUNER

enum tnrstate
{
	TUNERSTATE_0,
	TUNERSTATE_01,
	TUNERSTATE_1,
	TUNERSTATE_2,
	TUNERSTATE_DONE,
	TUNERSTATE_ABORTING,
};

static enum tnrstate tunerstate = TUNERSTATE_0;


#endif /* WITHAUTOTUNER */

// работа в машине состояний тюнера
// возврат STTE_OK для перехода на следующее состояние
// возврат STTE_BUSY - продолжаем тут
static STTE_t hamradio_tune_step(void)
{
#if WITHAUTOTUNER
	switch (tunerstate)
	{
	case TUNERSTATE_0:
		updateboard();
		auto_tune0_init();
		tunerstate = TUNERSTATE_01;
		break;

	case TUNERSTATE_01:
		switch (auto_tune0())
		{
		case PHASE_ABORT:
			tunerstate = TUNERSTATE_ABORTING;
			break;
		case PHASE_DONE:
			auto_tune1_init();
			tunerstate = TUNERSTATE_1;
			break;
		default:
			// keep state
			break;
		}
		break;

	case TUNERSTATE_1:
		switch (auto_tune1())
		{
		case PHASE_ABORT:
			tunerstate = TUNERSTATE_ABORTING;
			break;
		case PHASE_DONE:
			auto_tune2_init();
			tunerstate = TUNERSTATE_2;
			break;
		default:
			// keep state
			break;
		}
		break;

	case TUNERSTATE_2:
		auto_tune2();	// save to nvram
		tunerstate = TUNERSTATE_DONE;
		break;

	case TUNERSTATE_ABORTING:
		auto_tune3();	// restore tuner state from saved
		txreq_rx(& txreqst0, NULL);
		updateboard();
		tunerstate = TUNERSTATE_0;
//		{
//			// прочистить очередь сообщений
//			for (;;)
//			{
//				uint_fast16_t kbch;
//				uint_fast8_t kbready;
//				processmessages(& kbch, & kbready);
//				if (kbready == 0)
//					break;
//			}
//		}
		break;

	case TUNERSTATE_DONE:
		txreq_rx(& txreqst0, NULL);
		updateboard();
		tunerstate = TUNERSTATE_0;
		break;

	default:
		break;
	}

	return tunerstate == TUNERSTATE_0 ? STTE_OK : STTE_BUSY;

#else /* WITHAUTOTUNER */
	return STTE_OK;
#endif /* WITHAUTOTUNER */
}

#if WITHDEBUG
static void keyspoolprocess(void * ctx)
{
#if ! defined (HAVE_BTSTACK_STDIN)
#if 0
	uint_fast16_t dtmfch;
	if (dtmf_scan((& dtmfch)))
	{
		PRINTF("dtmfkey=%02X\n", (unsigned char) dtmfch);
	}
#endif
	/* здесь можно добавить обработку каких-либо команд с debug порта */
	char c;
	if (dbg_getchar(& c))
	{
		switch (c)
		{
		case 0x00:
			break;
		default:
			PRINTF("key=%02X\n", (unsigned char) c);
			break;
#if 0
		case 'd':
			if (swrsim < 30)
				++ swrsim;
			PRINTF("swrsim=%d\n", swrsim);
			break;
		case 's':
			if (swrsim > 0)
				-- swrsim;
			PRINTF("swrsim=%d\n", swrsim);
			break;
#endif
#if WITHMENU
		case 'm':
			PRINTF("menu items:\n");
			menu_print();
			PRINTF("menu items end\n");
			break;
#endif /* WITHMENU */
#if __riscv
		case ' ':
			{
				uint64_t v = csr_read_mcycle();
				static uint64_t v0;
				PRINTF("%lu\n", (long) (v - v0) / 1000000);
				v0 = v;
			}
			break;
#endif /* __riscv */
#if WITHUSBHOST_HIGHSPEEDULPI && 0
		case 'u':
			PRINTF("hkey:\n");
			ulpi_chip_debug();
			break;
#endif /* WITHUSBHOST_HIGHSPEEDULPI */
#if WITHWAVPLAYER || WITHSENDWAV
		case 'p':
			PRINTF(PSTR("Play test file\n"));
			playwavfile("1.wav");
			break;
#endif /* WITHWAVPLAYER */
		}
	}

#endif /* ! defined (HAVE_BTSTACK_STDIN) */
}
#endif /* WITHDEBUG */



// User-mode function. Вызывается для выполнения latch спектра и панорамы
static void
dpc_displatch_timer_fn(void * ctx)
{
	(void) ctx;
	display2_latch();

	const struct menudef * mp;
	if (0)
	{

	}
#if WITHDIRECTFREQENER
	else if (editfreqmode)
	{
		const uint_fast8_t bi = getbankindex_tx(gtx);
		vindex_t vi = getvfoindex(bi);

		editfreq2_t ef;

		ef.freq = gfreqs [bi];
		ef.blinkpos = blinkpos;
		ef.blinkstate = blinkstate;

		dctx_t dctx;
		dctx.type = DCTX_FREQ;
		dctx.pv = & ef;

		display2_bgprocess(0, actpageix(), & dctx);			/* выполнение шагов state machine отображения дисплея */
	}
#endif
#if WITHMENU
	else if (getinmenu(& mp))
	{
		dctx_t dctx;
		dctx.type = DCTX_MENU;
		dctx.pv = mp;

		display2_bgprocess(1, actpageix(), & dctx);			/* выполнение шагов state machine отображения дисплея */
	}
#endif /* WITHMENU */
	else
	{
		display2_bgprocess(0, actpageix(), NULL);			/* выполнение шагов state machine отображения дисплея */
	}
}

// user-mode processing
// вызывается по DPC в главном цикле core 0
static void
appspoolprocess(void * ctx)
{
	(void) ctx;
#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT
	// обработать запрос на обновление состояния аппаратуры из user mode программы
	if (dimmflagch != 0)
	{
		dimmflagch = 0;
		updateboard();
	}
#endif /* WITHLCDBACKLIGHT || WITHKBDBACKLIGHT */
#if WITHFANTIMER
	// обработать запрос на обновление состояния аппаратуры из user mode программы
	if (fanpaflagch != 0)
	{
		fanpaflagch = 0;
		updateboard();
	}
#endif /* WITHFANTIMER */
#if WITHSLEEPTIMER
	// обработать запрос на обновление состояния аппаратуры из user mode программы
	if (sleepflagch != 0)
	{
		sleepflagch = 0;
		updateboard();
	}
#endif /* WITHSLEEPTIMER */
}

//
//// работа в машине состояний меню
//// STTE_OK - вышли из меню.
//// STTE_BUSY - продолжаем работу с мену
//static STTE_t hamradio_menu_step(void)
//{
//	ginmenu = 0;
//	return STTE_OK;
//}

enum
{
	//STHTL_IDLE,
	STHRL_RXTX,
	STHRL_RXTX_FQCHANGED,
	STHRL_TUNE,
	STHRL_MENU,
	//
	STHRL_count
};

static uint_fast8_t sthrl = STHRL_RXTX;

// инициализация главной машины состояний
static void hamradio_main_initialize(void)
{
	{
		static ticker_t ticker;

		ticker_initialize(& ticker, NTICKS(UI_TICKS_PERIOD), refreshticker_cb, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
		ticker_add(& ticker);
	}

	{
		static ticker_t ticker;
		static dpcobj_t dpcobj;

		dpcobj_initialize(& dpcobj, dpc_1s_timer_fn, NULL);
		ticker_initialize_user(& ticker, NTICKS(1000), & dpcobj);
		ticker_add(& ticker);
	}

	{
		static dpcobj_t dpcobj;

		IRQLSPINLOCK_INITIALIZE(& boardupdatelock);
		dpcobj_initialize(& dpcobj, dpc_displatch_timer_fn, NULL);
		ticker_initialize_user_display(& displatchticker, NTICKS(calcdivround2(1000, glatchfps)), & dpcobj);	// 50 ms - обновление с частотой 20 герц
		ticker_add(& displatchticker);
	}

	{
		static ticker_t ticker;
		static dpcobj_t dpcobj;

		dpcobj_initialize(& dpcobj, dpc_0p1_s_timer_fn, NULL);
		ticker_initialize_user(& ticker, NTICKS(100), & dpcobj);
		ticker_add(& ticker);
	}

#if WITHAUTOTUNER
	{
		static dpcobj_t dpcobj;
		dpcobj_initialize(& dpcobj, dpc_tunertimer_fn, NULL);
		ticker_initialize_user(& ticker_tuner, NTICKS(gtunerdelay), & dpcobj);
		ticker_add(& ticker_tuner);
	}
#endif /* WITHAUTOTUNER */
#if WITHDEBUG
	{
		static dpcobj_t dpcobj;

		dpcobj_initialize(& dpcobj, keyspoolprocess, NULL);
		board_dpc_addentry(& dpcobj, board_dpc_coreid());
	}
#endif
	{
		static dpcobj_t dpcobj;

		dpcobj_initialize(& dpcobj, appspoolprocess, NULL);
		board_dpc_addentry(& dpcobj, board_dpc_coreid());
	}
	// начальная инициализация

	seq_purge();

#if FQMODEL_GEN500
	gfreqs [getbankindex_raw(0)] = 434085900UL;
	gfreqs [getbankindex_raw(1] = 434085900UL;
#endif /* FQMODEL_GEN500 */

	updateboard();	/* полная перенастройка (как после смены режима) - режим приема */
	updateboard2();			/* настройки валкодера и цветовой схемы дисплея. */
	//display2_needupdate();

#if 0 && MODEL_MAXLAB
	// тестирование алгоритма для MAXLAB
	{
		unsigned n;
		uint_fast32_t f;

		f = 5500000;
		for (n = 0; n < 1000; n += 50)
			synth_lo1_setfreq(f + n. getlo1div(tx));
		f = 5999000;
		for (n = 0; n < 1000; n += 50)
			synth_lo1_setfreq(f + n, getlo1div(tx));
		//for (;;)
		//	;
	}
#endif

#if FQMODEL_GEN500
	{
		const spitarget_t target = targetpll2;		/* addressing to chip */

		void prog_cmx992_print(spitarget_t target), prog_cmx992_initialize(spitarget_t target);

		//prog_cmx992_initialize(target);
		for (;;)
			prog_cmx992_print(target);

		//display2_needupdate();	// Обновление дисплея - всё, включая частоту

		//prog_pll1_init();
		synth_lo1_setfreq((434085900UL - 10700000UL) / 3. getlo1div(gtx));
		for (;;)
			;
	}
#endif /* FQMODEL_GEN500 */
}

/* Возвращаем не-0, если было изменение частоты настройки */
static uint_fast8_t
processmainlooptuneknobs(inputevent_t * ev)
{
	uint_fast8_t freqchanged = 0;
	const uint_fast8_t bi_main = getbankindexmain();		/* состояние выбора банков может измениться */
	const uint_fast8_t bi_sub = getbankindexsub();		/* состояние выбора банков может измениться */
	uint_fast8_t jumpsize_main;
	uint_fast8_t jumpsize_sub;

#if WITHENCODER

	/* переход по частоте - шаг берётся из gstep_ENC_MAIN */
#if WITHBBOX && defined (WITHBBOXFREQ)
	int_least16_t nrotate_main = 0;	// ignore encoder
	int_least16_t nrotate_sub = 0;	// ignore encoder
	uint_fast16_t step_main = gstep_ENC_MAIN;
	uint_fast16_t step_sub = gstep_ENC2;
#elif WITHENCODER_SUB
	int_least16_t nrotate_main = event_getRotateHiRes(& ev->encMAIN, & jumpsize_main, genc1div * gencderate);
	int_least16_t nrotate_sub = event_getRotateHiRes(& ev->encSUB, & jumpsize_sub, genc1div * gencderate);
	uint_fast16_t step_main = gstep_ENC_MAIN;
	uint_fast16_t step_sub = gstep_ENC_MAIN;
#elif WITHENCODER2 && LINUX_SUBSYSTEM
	int_least16_t nrotate_main = 0;	// ignore encoder
	jumpsize_main = 1;
	jumpsize_sub = 1;
	int_least16_t nrotate_sub = linux_get_enc2();
	uint_fast16_t step_main = gstep_ENC_MAIN;
	uint_fast16_t step_sub = gstep_ENC2;
#elif WITHENCODER2
	int_least16_t nrotate_main = event_getRotateHiRes(& ev->encMAIN, & jumpsize_main, genc1div * gencderate);
	int_least16_t nrotate_sub = event_getRotateHiRes(& ev->encFN, & jumpsize_sub, genc2div);
	uint_fast16_t step_main = gstep_ENC_MAIN;
	uint_fast16_t step_sub = gstep_ENC2;
#else
	int_least16_t nrotate_main = event_getRotateHiRes(& ev->encMAIN, & jumpsize_main, genc1div * gencderate);
	int_least16_t nrotate_sub = 0;
	uint_fast16_t step_main = gstep_ENC_MAIN;
	uint_fast16_t step_sub = 0;
#endif
	(void) step_sub;

	// обработка меню FN, если выключено - работает как перестройка частоты с крупным шагом
	if (uif_encoder2_rotate(nrotate_sub))
	{
		nrotate_sub = 0;
#if WITHTOUCHGUI
		hamradio_gui_enc2_update();
		//display2_needupdate();
#else /* WITHTOUCHGUI */
		//display2_needupdate();			/* Обновление дисплея - всё, включая частоту */
#endif /* WITHTOUCHGUI */
	}

	if (nrotate_main || nrotate_sub)
	{
		bring_tuneA();	// Начать отображение текущей частоты на водопаде
		bring_tuneB();	// Начать отображение текущей частоты на водопаде
	}

	if (glock == 0)
	{
		uint_fast32_t * const pimain = & gfreqs [bi_main];
		uint_fast32_t * const pisub = & gfreqs [bi_sub];
		/* Обработка накопленного количества импульсов от валкодера */
		if (nrotate_main < 0)
		{
			/* Валкодер A: вращали "вниз" */
			* pimain = prevfreq(* pimain, * pimain - ((uint_fast32_t) step_main * jumpsize_main * - nrotate_main), step_main, tune_bottom(bi_main));
			freqchanged = 1;
		}
		else if (nrotate_main > 0)
		{
			/* Валкодер A: вращали "вверх" */
			* pimain = nextfreq(* pimain, * pimain + ((uint_fast32_t) step_main * jumpsize_main * nrotate_main), step_main, tune_top(bi_main));
			freqchanged = 1;
		}

#if ! WITHTOUCHGUI
		/* Обработка накопленного количества импульсов от валкодера */
		if (nrotate_sub < 0)
		{
			/* Валкодер B: вращали "вниз" */
			* pisub = prevfreq(* pisub, * pisub - ((uint_fast32_t) step_sub * jumpsize_sub * - nrotate_sub), step_sub, tune_bottom(bi_sub));
			freqchanged = 1;
		}
		else if (nrotate_sub > 0)
		{
			/* Валкодер B: вращали "вверх" */
			* pisub = nextfreq(* pisub, * pisub + ((uint_fast32_t) step_sub * jumpsize_sub * nrotate_sub), step_sub, tune_top(bi_sub));
			freqchanged = 1;
		}
#endif /* ! WITHTOUCHGUI */
	}

#if WITHTOUCHGUI && WITHENCODER2
	gui_set_encoder2_rotate(nrotate_sub);
	freqchanged = freqchanged || !! nrotate_sub;
#endif /* WITHTOUCHGUI && WITHENCODER2 */


#else /* WITHENCODER */

#endif /* WITHENCODER */

#if 0 && CPUSTYLE_XC7Z		// тестовая прокрутка частоты
	hamradio_set_freq(hamradio_get_freq_rx() + 1);
	freqchanged = 1;
#endif

	return freqchanged;
}
// работа в главной машине состояний
static STTE_t
hamradio_main_step(void)
{
	inputevent_t event;
	inputevent_initialize(& event);
	inputevent_fill(& event);

	txreq_process();	/* обработка запросов */
	switch (sthrl)
	{
//	case STHRL_MENU:
//		if (hamradio_menu_step() == STTE_OK)
//			sthrl = STHRL_RXTX;
//		break;

	case STHRL_TUNE:
		if (hamradio_tune_step() == STTE_OK)
			sthrl = STHRL_RXTX;
		break;

	case STHRL_RXTX_FQCHANGED:
			/* валкодер перестал вращаться - если было изменение частоты - сохраняем конфигурацию */
			if (refreshenabled_freqs())
			{
				const uint_fast8_t bi_main = getbankindexmain();		/* состояние выбора банков может измениться */
				const uint_fast8_t bi_sub = getbankindexsub();		/* состояние выбора банков может измениться */
				/* в случае внутренней памяти микроконтроллера - частоту не запоминать (очень мал ресурс). */

				storebandfreq(getvfoindex(bi_main), bi_main);		/* сохранение частоты в текущем VFO */
				storebandfreq(getvfoindex(bi_sub), bi_sub);		/* сохранение частоты в текущем VFO */
				sthrl = STHRL_RXTX;

				board_wakeup();
			}
			else
			{
				// проваливаемся дальше, когда наступит время - refreshenabled_freqs -
				// сохраним частоту и пеерйдём к состоянию STHRL_RXTX

			}
			// @suppress("No break at end of case")

	case STHRL_RXTX:
		// работа с пользователем
		if (txreq_getreqautotune(& txreqst0) != 0 && gtx != 0)
		{
			sthrl = STHRL_TUNE;
			break;
		}


	#if WITHLFM && ! BOARD_PPSIN_BIT
		if (lfmmode)
		{
			/*  проверяем секунды начала */
				uint_fast8_t hour, minute, seconds;

				board_rtc_cached_gettime(& hour, & minute, & seconds);
				if (islfmstart(minute * 60 + seconds))
				{
					IRQL_t oldIrql;
					RiseIrql(TICKER_IRQL, & oldIrql);
					lfm_run();
					LowerIrql(oldIrql);
				}

			/* обновить настройку полосовых фильтров */
			updateboard_freq();	/* частичная перенастройка - без смены режима работы. может вызвать полную перенастройку */
		}
	#endif /* WITHLFM */
		if (alignmode)
		{
	#if MULTIVFO
			// вход в режим настройки ГУНов первого гетеродина
			//display2_needupdate();
			vfoallignment();
			//display2_needupdate();
			updateboard();	/* полная перенастройка (как после смены режима) */
	#endif // MULTIVFO
			alignmode = 0;	// в nvram осталась не-0
		}
	#if WITHUSEAUDIOREC
		if (recmode)
		{
			recmode = 0;
			sdcardrecord();
		}
	#endif /* WITHUSEAUDIOREC */

		{
#if WITHMENU
			if (ginmenu0 && processmenukeyandencoder(& event))
			{
		#if WITHTOUCHGUI
				display_redrawfreqstimed(1);
		#endif /* WITHTOUCHGUI */
				//display2_needupdate();			/* Обновление дисплея - всё, включая частоту */

			}
#endif /* WITHMENU */
			if (! ginmenu0 && processmainloopkeyboard(& event))
			{
		#if WITHTOUCHGUI
				display_redrawfreqstimed(1);
		#endif /* WITHTOUCHGUI */
				//display2_needupdate();			/* Обновление дисплея - всё, включая частоту */

			} // end keyboard processing
		}
		if (processpots())
		{
			/* обновление индикатора без сохранения состояния диапазона */
	#if WITHTOUCHGUI
			display_redrawfreqstimed(1);
			//display2_needupdate();
	#endif /* WITHTOUCHGUI */

		} // end potentiometers processing

		if (processmainloopencoders(ginmenu0, & event))
		{
	#if WITHTOUCHGUI
			display_redrawfreqstimed(1);
	#endif /* WITHTOUCHGUI */

		}
		// Knobs rotation processing
		if (! ginmenu0 && processmainlooptuneknobs(& event))
		{
			updateboard_freq();	/* частичная перенастройка - без смены режима работы. может вызвать полную перенастройку */
			// Ограничение по скорости обновления дисплея уже заложено в него
			sthrl = STHRL_RXTX_FQCHANGED;
		}
		break;

	default:
		break;
	}
	return STTE_OK;
}

#if WITHSPKMUTE
uint_fast8_t hamradio_get_gmutespkr(void)
{
	return param_getvalue(& xgmutespkr);
}

void hamradio_set_gmutespkr(uint_fast8_t v)
{
	param_setvalue(& xgmutespkr, v);
	updateboard();
}
#endif /* WITHSPKMUTE */

#if WITHIF4DSP

uint_fast16_t hamradio_get_afgain(void)
{
	return param_getvalue(& xafgain1);
}

#if ! WITHPOTAFGAIN
void hamradio_set_afgain(uint_fast16_t v)
{
	param_setvalue(& xafgain1, v);
	updateboard();
}

#endif /* ! WITHPOTAFGAIN */

#endif /* WITHIF4DSP */

#if WITHTX

void hamradio_set_tune(uint_fast8_t v)
{
	if (v)
		txreq_txtone(& txreqst0);
	else
		txreq_rx(& txreqst0, NULL);
}

#if WITHPOWERTRIM

#if WITHLOWPOWEREXTTUNE

void hamradio_set_tx_tune_power(uint_fast8_t v)
{
	ASSERT(v >= WITHPOWERTRIMMIN);
	ASSERT(v <= WITHPOWERTRIMMAX);
	gtunepower = v;
	save_i8(OFFSETOF(struct nvmap, gtunepower), gtunepower);
	updateboard();
}

uint_fast8_t hamradio_get_tx_tune_power(void)
{
	return gtunepower;
}

#endif /* WITHLOWPOWEREXTTUNE */

void hamradio_set_tx_power(uint_fast8_t v)
{
	ASSERT(v >= WITHPOWERTRIMMIN);
	ASSERT(v <= WITHPOWERTRIMMAX);
	gnormalpower.value = v;
	save_i8(OFFSETOF(struct nvmap, gnormalpower), gnormalpower.value);
	updateboard();
}

uint_fast8_t hamradio_get_tx_power(void)
{
	return gnormalpower.value;
}

void hamradio_get_tx_power_limits(uint_fast8_t * min, uint_fast8_t * max)
{
	* min = WITHPOWERTRIMMIN;
	* max = WITHPOWERTRIMMAX;
}

#else /* WITHPOWERTRIM */

void hamradio_set_tx_power(uint_fast8_t v)
{
}

uint_fast8_t hamradio_get_tx_power(void)
{
	return 0;
}

#endif /* WITHPOWERTRIM */

#endif /* WITHTX */

uint_fast8_t hamradio_verify_freq_bands(uint_fast32_t freq, uint_fast32_t * bottom, uint_fast32_t * top)
{
	for (uint_fast8_t i = 0; i < HBANDS_COUNT; i++)
	{
		if (freq >= get_band_bottom(i) && freq <= get_band_top(i) && get_band_bandset(i) == BANDSETF_HAM)
		{
			* bottom = get_band_bottom(i);
			* top = get_band_top(i);
			return 1;					// частота внутри любительского диапазона
		}
	}
	* bottom = 0;
	* top = 0;
	return 0; 							// частота вне любительских диапазонов
}

#if WITHVOX && WITHTX

void hamradio_set_gvoxenable(uint_fast8_t v)
{
	gvoxenable = v != 0;
	save_i8(OFFSETOF(struct nvmap, gvoxenable), gvoxenable);
	updateboard();
}

uint_fast8_t hamradio_get_gvoxenable(void)
{
	return gvoxenable;
}

void hamradio_get_vox_delay_limits(uint_fast8_t * min, uint_fast8_t * max)
{
	* min = WITHVOXDELAYMIN;
	* max = WITHVOXDELAYMAX;
}

uint_fast8_t hamradio_get_vox_delay(void)
{
	return voxdelay;
}

void hamradio_set_vox_delay(uint_fast8_t v)
{
	ASSERT(v >= WITHVOXDELAYMIN);
	ASSERT(v <= WITHVOXDELAYMAX);
	voxdelay = v;
	save_i8(OFFSETOF(struct nvmap, voxdelay), voxdelay);
	updateboard();
}

void hamradio_get_vox_level_limits(uint_fast8_t * min, uint_fast8_t * max)
{
	* min = WITHVOXLEVELMIN;
	* max = WITHVOXLEVELMAX;
}

uint_fast8_t hamradio_get_vox_level(void)
{
	return gvoxlevel;
}

void hamradio_set_vox_level(uint_fast8_t v)
{
	ASSERT(v >= WITHVOXLEVELMIN);
	ASSERT(v <= WITHVOXLEVELMAX);
	gvoxlevel = v;
	save_i8(OFFSETOF(struct nvmap, gvoxlevel), gvoxlevel);
	updateboard();
}

void hamradio_get_antivox_delay_limits(uint_fast8_t * min, uint_fast8_t * max)
{
	* min = WITHAVOXLEVELMIN;
	* max = WITHAVOXLEVELMAX;
}

uint_fast8_t hamradio_get_antivox_level(void)
{
	return gavoxlevel;
}

void hamradio_set_antivox_level(uint_fast8_t v)
{
	ASSERT(v >= WITHAVOXLEVELMIN);
	ASSERT(v <= WITHAVOXLEVELMAX);
	gavoxlevel = v;
	save_i8(OFFSETOF(struct nvmap, gavoxlevel), gavoxlevel);
	updateboard();
}

#endif /* WITHVOX && WITHTX */

#if WITHIF4DSP

#if WITHREVERB

void hamradio_get_reverb_delay_limits(uint_fast8_t * min, uint_fast8_t * max)
{
	* min = WITHREVERBDELAYMIN;
	* max = WITHREVERBDELAYMAX;
}

void hamradio_get_reverb_loss_limits(uint_fast8_t * min, uint_fast8_t * max)
{
	* min = WITHREVERBLOSSMIN;
	* max = WITHREVERBLOSSMAX;
}

uint_fast8_t hamradio_get_reverb_delay(void)
{
	return greverbdelay;
}

uint_fast8_t hamradio_get_reverb_loss(void)
{
	return greverbloss;
}

void hamradio_set_reverb_delay(uint_fast8_t v)
{
	ASSERT(v >= WITHREVERBDELAYMIN);
	ASSERT(v <= WITHREVERBDELAYMAX);
	greverbdelay = v;
	save_i8(OFFSETOF(struct nvmap, greverbdelay), greverbdelay);
	updateboard();
}

void hamradio_set_reverb_loss(uint_fast8_t v)
{
	ASSERT(v >= WITHREVERBLOSSMIN);
	ASSERT(v <= WITHREVERBLOSSMAX);
	greverbloss = v;
	save_i8(OFFSETOF(struct nvmap, greverbloss), greverbloss);
	updateboard();
}

void hamradio_set_greverb(uint_fast8_t v)
{
	greverb = v != 0;
	save_i8(OFFSETOF(struct nvmap, greverb), greverb);
	updateboard();
}

uint_fast8_t hamradio_get_greverb(void)
{
	return greverb;
}

#endif /* WITHREVERB */

void hamradio_set_gmoniflag(uint_fast8_t v)
{
	gmoniflag = v != 0;
	save_i8(OFFSETOF(struct nvmap, gmoniflag), gmoniflag);
	updateboard();
}

uint_fast8_t hamradio_get_gmoniflag(void)
{
	return gmoniflag;
}


void hamradio_get_mic_clip_limits(uint_fast8_t * min, uint_fast8_t * max)
{
	* min = WITHMIKECLIPMIN;
	* max = WITHMIKECLIPMAX;
}

uint_fast8_t hamradio_get_gmikehclip(void)
{
	return gmikehclip;
}

void hamradio_set_gmikehclip(uint_fast8_t v)
{
	ASSERT(v >= WITHMIKECLIPMIN);
	ASSERT(v <= WITHMIKECLIPMAX);
	gmikehclip = v;
	save_i8(OFFSETOF(struct nvmap, gmikehclip), gmikehclip);
	updateboard();
}

void hamradio_get_mic_level_limits(uint_fast8_t * min, uint_fast8_t * max)
{
	* min = WITHMIKEINGAINMIN;
	* max = WITHMIKEINGAINMAX;
}

uint_fast8_t hamradio_get_mik1level(void)
{
	return gmik1level;
}

void hamradio_set_mik1level(uint_fast8_t v)
{
	ASSERT(v >= WITHMIKEINGAINMIN);
	ASSERT(v <= WITHMIKEINGAINMAX);
	gmik1level = v;
	save_i8(OFFSETOF(struct nvmap, gmik1level), gmik1level);
	updateboard();
}

uint_fast8_t hamradio_get_gmikeagc(void)
{
	return gmikeagc;
}

void hamradio_set_gmikeagc(uint_fast8_t v)
{
	gmikeagc = v != 0;
	save_i8(OFFSETOF(struct nvmap, gmikeagc), gmikeagc);
	updateboard();
}

void hamradio_get_mic_agc_limits(uint_fast8_t * min, uint_fast8_t * max)
{
	* min = WITHMIKEAGCMIN;
	* max = WITHMIKEAGCMAX;
}

uint_fast8_t hamradio_get_gmikeagcgain(void)
{
	return gmikeagcgain;
}

void hamradio_set_gmikeagcgain(uint_fast8_t v)
{
	ASSERT(v >= WITHMIKEINGAINMIN);
	ASSERT(v <= WITHMIKEINGAINMAX);
	gmikeagcgain = v;
	save_i8(OFFSETOF(struct nvmap, gmikeagcgain), gmikeagcgain);
	updateboard();
}

#endif /* WITHIF4DSP */

#if WITHAFCODEC1HAVEPROC

uint_fast8_t hamradio_get_gmikeboost20db(void)
{
	return gmikeboost20db;
}

void hamradio_set_gmikeboost20db(uint_fast8_t v)
{
	gmikeboost20db = v != 0;
	save_i8(OFFSETOF(struct nvmap, gmikeboost20db), gmikeboost20db);
	updateboard();
}

uint_fast8_t hamradio_get_gmikeequalizer(void)
{
	return param_getvalue(& xgmikeequalizer);
}

void hamradio_set_gmikeequalizer(uint_fast8_t v)
{
	param_setvalue(& xgmikeequalizer, v != 0);	// Установить значение параметра и сохранить в nvram
	updateboard();
}

uint_fast8_t hamradio_get_gmikeequalizerparams(uint_fast8_t i)
{
	ASSERT(i < HARDWARE_CODEC1_NPROCPARAMS);
	return gmikeequalizerparams [i];
}

void hamradio_set_gmikeequalizerparams(uint_fast8_t i, uint_fast8_t v)
{
	ASSERT(i < HARDWARE_CODEC1_NPROCPARAMS);
	ASSERT(v <= EQUALIZERBASE * 2);
	gmikeequalizerparams [i] = v;
	//save_i8(OFFSETOF(struct nvmap, gmoniflagxxx), gmoniflagxxx);
	updateboard();
}

int_fast32_t hamradio_getequalizerbase(void)
{
	return getequalizerbase();
}
#endif /* WITHAFCODEC1HAVEPROC */

int_fast16_t hamradio_if_shift(int_fast8_t step)
{
#if WITHIFSHIFT

	if (step != 0)
	{
		int_fast16_t val = ifshifoffset.value + step * ISTEP50;

		if (val < IFSHIFTTMIN)
			val = IFSHIFTTMIN;

		if (val > IFSHIFTMAX)
			val = IFSHIFTMAX;

		ifshifoffset.value = val;
		updateboard();
	}
	return ifshifoffset.value + getifshiftbase();	// Добавить учет признака наличия сдвига

#else /* WITHIFSHIFT */

	return 0;

#endif /* WITHIFSHIFT */
}

#if WITHELKEY

uint_fast8_t hamradio_get_cw_wpm(void)
{
	return elkeywpm.value;
}
#endif /* WITHELKEY */

void hamradio_set_lock(uint_fast8_t lock)
{
	const uint_fast8_t bandset_no_check = 0;
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t b = getfreqband(gfreqs [bi], bandset_no_check);	/* определяем по частоте, в каком диапазоне находимся */

	glock = lock != 0;
	save_i8(RMT_LOCKMODE_BASE(b), glock);
	updateboard();
}

uint_fast8_t hamradio_set_freq(uint_fast32_t freq)
{
	const uint_fast8_t bandset_no_check = 0;
	if (freqvalid(freq, 0))
	{
		const uint_fast8_t bi = getbankindex_tx(gtx);
		gfreqs [bi] = freq;
		sthrl = STHRL_RXTX_FQCHANGED;
		updateboard_freq();	/* частичная перенастройка - без смены режима работы. может вызвать полную перенастройку */
		return 1;
	}
	return 0;
}

#if WITHNOTCHFREQ

uint_fast8_t hamradio_get_gnotch(void)
{
	return gnotch;
}

void hamradio_set_gnotch(uint_fast8_t v)
{
	gnotch = v != 0;
	updateboard();
}

uint_fast8_t hamradio_get_gnotchtype(void)
{
	return notchmodes [gnotchtype].code;
}

void hamradio_set_gnotchtype(uint_fast8_t v)
{
	gnotchtype = v;
	updateboard();
}

uint_fast16_t hamradio_notch_freq(int_fast8_t step)
{
	if (step != 0)
	{
		uint_fast16_t val = gnotchfreq.value + step * ISTEP50;

		if (val < WITHNOTCHFREQMIN)
			val = WITHNOTCHFREQMIN;

		if (val > WITHNOTCHFREQMAX)
			val = WITHNOTCHFREQMAX;

		gnotchfreq.value = val;
		updateboard();
	}
	return gnotchfreq.value;
}

uint_fast16_t hamradio_notch_width(int_fast8_t step)
{
	if (step != 0)
	{
		uint_fast16_t val = gnotchwidth.value + step * ISTEP50;

		if (val < WITHNOTCHWIDTHMIN)
			val = WITHNOTCHWIDTHMIN;

		if (val > WITHNOTCHWIDTHMAX)
			val = WITHNOTCHWIDTHMAX;

		gnotchwidth.value = val;
		updateboard();
	}
	return gnotchwidth.value;
}

#endif /* WITHNOTCHFREQ */

#if WITHTOUCHGUI

void hamradio_disable_keyboard_redirect(void)
{
	keyboard_redirect = 0;
}

void hamradio_enable_keyboard_redirect(void)
{
	keyboard_redirect = 1;
}

void hamradio_disable_encoder2_redirect(void)
{
	encoder2_redirect = 0;
}

void hamradio_enable_encoder2_redirect(void)
{
	encoder2_redirect = 1;
}

#if WITHMENU
uint_fast16_t hamradio_get_multilinemenu_block_groups(menu_names_t * vals)
{
	uint_fast16_t el;
	uint_fast16_t count = 0;

#if defined (RTC1_TYPE)
	getstamprtc();
#endif /* defined (RTC1_TYPE) */
	for (el = 0; el < menutable_size(); el ++)
	{
		const struct menudef * const mv = & menutable [el];
		if (ismenukinddp(mv->pd, ITEM_GROUP))
		{
			menu_names_t * const v = & vals [count];
			safestrcpy(v->name, ARRAY_SIZE(v->name), mv->pd->label);
			v->index = el;
			count++;
		}
	}
	return count;
}

uint_fast16_t hamradio_get_multilinemenu_block_params(menu_names_t * vals, uint_fast8_t index, uint_fast8_t max_count)
{
	uint_fast16_t el;
	uint_fast16_t count = 0;

	for (el = index + 1; el < menutable_size(); el ++)
	{
		const struct menudef * const mv = & menutable [el];
		if (ismenukinddp(mv->pd, ITEM_GROUP))
			break;
		if (ismenukinddp(mv->pd, ITEM_VALUE))
		{
			menu_names_t * const v = & vals [count];
			safestrcpy (v->name, ARRAY_SIZE(v->name), mv->pd->label);
			v->index = el;
			count++;
		}
		if (count >= max_count)
		{
			PRINTF("Block count %d exceeding the array size %d\n", count, max_count);
			ASSERT(0);
		}
	}
	return count;
}

void hamradio_get_multilinemenu_block_vals(menu_names_t * vals, uint_fast8_t index, uint_fast8_t cnt)
{
	uint_fast16_t el;

	for (el = index; el <= index + cnt; el ++)
	{
		const struct menudef * const mv = & menutable [el];
		if (ismenukinddp(mv->pd, ITEM_VALUE))
		{
			param_format(mv->pd, vals->name, ARRAY_SIZE(vals->name), param_getvalue(mv->pd));
			vals->index = el;
			return;
		}
	}
}

const char * hamradio_gui_edit_menu_item(uint_fast16_t index, int_least16_t rotate)
{
	const struct paramdefdef * const pd = menutable [index].pd;
	if (param_rotate(pd, rotate))	/* модификация и сохранение параметра по валкодеру - возврат не-0  в случае модификации */
	{
		updateboard();
		display_redrawfreqstimed(1);
		//display2_needupdate();
	}

	static char menuw [20];						// буфер для вывода значений системного меню

	param_format(menutable [index].pd, menuw, ARRAY_SIZE(menuw), param_getvalue(menutable [index].pd));
	return menuw;
}
#endif /* WITHMENU */

const char * hamradio_get_submode_label(uint_fast8_t v)
{
	ASSERT(v < SUBMODE_COUNT);
	return submodes [v].qlabel;
}

uint_fast8_t hamradio_get_submode(void)
{
	return getsubmode(0);
}

void hamradio_clean_memory_cells(uint_fast8_t i)
{
	ASSERT(i < MBANDS_COUNT);
	save_i32(RMT_BFREQ_BASE(MBANDS_BASE + i), 0);
}

void hamradio_save_memory_cells(uint_fast8_t i)
{
	ASSERT(i < MBANDS_COUNT);
	storebandstate(MBANDS_BASE + i, getbankindex_tx(gtx));
	storebandfreq(MBANDS_BASE + i, getbankindex_tx(gtx));
}

uint_fast32_t hamradio_load_memory_cells(uint_fast8_t cell, uint_fast8_t set)
{
	ASSERT(cell < MBANDS_COUNT);

	int_fast32_t freq = restore_i32(RMT_BFREQ_BASE(MBANDS_BASE + cell));
	if (freqvalid(freq, 0))
	{
		if (set)
		{
			const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
			const vindex_t vi = getvfoindex(bi);
			loadnewband(MBANDS_BASE + cell, bi);	/* загрузка всех параметров (и частоты) нового режима */
			storebandfreq(vi, bi);	/* сохранение частоты в текущем VFO */
			storebandstate(vi, bi); // записать все параметры настройки (кроме частоты)  в текущем VFO */
			updateboard();
		}
		return freq;
	}
	else
		return 0;
}



void hamradio_clean_mic_profile(uint_fast8_t cell)
{
	ASSERT(cell < NMICPROFCELLS);

	micprof_t * mp = & micprof_cells [cell];

#if WITHAFCODEC1HAVEPROC
	mp->mikeboost20db = 0;
	mp->eq_enable = 0;
#endif /* WITHAFCODEC1HAVEPROC */
	mp->level = 0;
	mp->agc = 0;
	mp->agcgain = 0;
	mp->clip = 0;
	mp->cell_saved = 0;

	save_i8(RMT_MICPSAVE_BASE(cell), mp->cell_saved);
}

void hamradio_save_mic_profile(uint_fast8_t cell)
{
	ASSERT(cell < NMICPROFCELLS);

	micprof_t * mp = & micprof_cells [cell];

	mp->cell_saved = 1;
	mp->level = gmik1level;
	mp->agc = gmikeagc;
	mp->agcgain = gmikeagcgain;
	mp->clip = gmikehclip;

#if WITHAFCODEC1HAVEPROC
	mp->mikeboost20db = gmikeboost20db;
	mp->eq_enable = gmikeequalizer;

	for(uint_fast8_t j = 0; j < HARDWARE_CODEC1_NPROCPARAMS; j ++)
	{
		mp->eq_params [j] = gmikeequalizerparams [j];
		save_i8(RMT_MICEQPARAMS_BASE(cell, j), mp->eq_params [j]);
	}

	save_i8(RMT_MICEQ_BASE(cell), mp->eq_enable);
	save_i8(RMT_MICBOOST_BASE(cell), mp->mikeboost20db);
#endif /* WITHAFCODEC1HAVEPROC */
	save_i8(RMT_MICLEVEL_BASE(cell), mp->level);
	save_i8(RMT_MICAGC_BASE(cell), mp->agc);
	save_i8(RMT_MICAGCGAIN_BASE(cell), mp->agcgain);
	save_i8(RMT_MICCLIP_BASE(cell), mp->clip);
	save_i8(RMT_MICPSAVE_BASE(cell), mp->cell_saved);
}

uint_fast8_t hamradio_load_mic_profile(uint_fast8_t cell, uint_fast8_t set)
{
	ASSERT(cell < NMICPROFCELLS);

	micprof_t * mp = & micprof_cells [cell];

	if (mp->cell_saved && set)
	{
#if WITHAFCODEC1HAVEPROC
		gmikeboost20db = mp->mikeboost20db;
		gmikeequalizer = mp->eq_enable;

		for(uint_fast8_t j = 0; j < HARDWARE_CODEC1_NPROCPARAMS; j ++)
			gmikeequalizerparams [j] = mp->eq_params [j];
#endif /* WITHAFCODEC1HAVEPROC */
		gmik1level = mp->level;
		gmikeagc = mp->agc;
		gmikeagcgain = mp->agcgain;
		gmikehclip = mp->clip;

		updateboard();
	}
	return mp->cell_saved;
}

#endif /* WITHTOUCHGUI */

uint_fast8_t hamradio_get_bands(band_array_t * bands, uint_fast8_t count_only, uint_fast8_t is_bcast_need)
{
	uint_fast8_t count = 0;

	for (uint_fast8_t i = 0; i < HBANDS_COUNT; i++)
	{
		if (existingbandsingle(i, 0))		// check for HAM bands
		{
			if (! count_only)
			{
				band_array_t * const b = & bands [count];
				const char * l = get_band_label(i);

				b->index = i;
				b->init_freq = get_band_init(i);
				b->type = BAND_TYPE_HAM;
				if (strcmp(l, ""))
				{
					strcpy(b->name, l);
				}
				else
				{
					local_snprintf_P(b->name, ARRAY_SIZE(b->name), PSTR("%ldk"), (long) (b->init_freq / 1000));
				}
			}
			count ++;
		}
	}

	if (is_bcast_need)
	{
		for (uint_fast8_t i = 0; i < HBANDS_COUNT; i++)
		{
			if (existingbandsingle(i, 1))		// check for broadcast bands
			{
				if (! count_only)
				{
					band_array_t * const b = & bands [count];
					const char * l = get_band_label(i);

					b->index = i;
					b->init_freq = get_band_init(i);
					b->type = BAND_TYPE_BROADCAST;
					if (strcmp(l, ""))
					{
						strcpy(b->name, l);
					}
					else
					{
						local_snprintf_P(b->name, ARRAY_SIZE(b->name), PSTR("%ldk"), (long) (b->init_freq / 1000));
					}
				}
				count ++;
			}
		}
	}
	return count;
}

void hamradio_goto_band_by_freq(uint_fast32_t f)
{
	if (freqvalid(f, 0))
		uif_key_click_bandjump2(f, 1);
}

uint_fast8_t hamradio_check_current_freq_by_band(uint_fast8_t band)
{
	vindex_t v = getfreqband(hamradio_get_freq_rx(), 1) == band;
	return v;
}

#if WITHIF4DSP

uint_fast8_t hamradio_get_bws(bws_t * bws, uint_fast8_t limit)
{
	const uint_fast8_t bwseti = mdt [gmode].bwsetis [gtx];	// индекс банка полос пропускания для данного режима
	const uint_fast8_t count = bwsetsc [bwseti].last + 1;

	for (uint_fast8_t i = 0; i < count; i ++)
	{
		if (i > limit)
			break;

		strncpy(bws->label[i], bwsetsc [bwseti].labels [i], ARRAY_SIZE(bws->label[i]));
	}

	return count;
}

void hamradio_set_bw(uint_fast8_t v)
{
	const uint_fast8_t bwseti = mdt [gmode].bwsetis [gtx];	// индекс банка полос пропускания для данного режима
	ASSERT(v <= bwsetsc [bwseti].last);
	bwsetpos [bwseti] = v;
	save_i8(RMT_BWSETPOS_BASE(bwseti), bwsetpos [bwseti]);	/* только здесь сохраняем новый фильтр для режима */
	updateboard();
}

void hamradio_change_submode(uint_fast8_t newsubmode, uint_fast8_t need_correct_freq)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* VFO bank index */
	const uint_fast8_t defcol = locatesubmode(newsubmode, & gmoderows [bi]);	/* строка/колонка для SSB. Что делать, если не нашли? */
	putmodecol(gmoderows [bi], defcol, bi);	/* внести новое значение в битовую маску */

	if (need_correct_freq)
		gsubmodechange(getsubmode(bi), bi);
	else
		storebandstate(getvfoindex(bi), bi); // записать все параметры настройки (кроме частоты) в область данных диапазона */

	updateboard();	/* полная перенастройка (как после смены режима) */
	display_redrawfreqstimed(1);
	//display2_needupdate();
}

uint_fast8_t hamradio_change_nr(uint_fast8_t v)
{
	if(v) uif_key_changenr();
	return gnoisereducts [gmode];
}

void hamradio_set_agc_fast(void)
{
	const struct modetempl * pamodetempl;
	const uint_fast8_t asubmode = getasubmode(0);
	pamodetempl = getmodetempl(asubmode);
	const uint_fast8_t agcseti = pamodetempl->agcseti;

	gagc [agcseti].rate = AGC_RATE_SSB;
	gagc [agcseti].scale = 100;
	gagc [agcseti].t0 = 0;
	gagc [agcseti].t1 = 120;
	gagc [agcseti].release10 = 1;
	gagc [agcseti].t4 = 50;
	gagc [agcseti].thung10 = 1;

	updateboard();
}

void hamradio_set_agc_slow(void)
{
	const struct modetempl * pamodetempl;
	const uint_fast8_t asubmode = getasubmode(0);
	pamodetempl = getmodetempl(asubmode);
	const uint_fast8_t agcseti = pamodetempl->agcseti;

	gagc [agcseti].rate = AGC_RATE_SSB;
	gagc [agcseti].scale = 100;
	gagc [agcseti].t0 = 0;
	gagc [agcseti].t1 = 120;
	gagc [agcseti].release10 = 5;
	gagc [agcseti].t4 = 50;
	gagc [agcseti].thung10 = 3;

	updateboard();
}

uint_fast8_t hamradio_get_agc_type(void)	// 0 - slow, 1 - fast
{
	const uint_fast8_t pathi = 0;
	const struct modetempl * pamodetempl;
	const uint_fast8_t asubmode = getasubmode(pathi);
	pamodetempl = getmodetempl(asubmode);
	const uint_fast8_t agcseti = pamodetempl->agcseti;

	if (gagc [agcseti].release10 >= 5)		// 0.5 секунды и более - считаем медланная. Как иначе вытянуть признак, пока не придумал. Надо переделать
		return 0;
	else
		return 1;
}

// 1 - wide (SSB style), 0 - narow (CW style)
uint_fast8_t hamradio_get_bp_type_wide(void)
{
	const uint_fast8_t pathi = 0;
	const uint_fast8_t tx = hamradio_get_tx();
	const uint_fast8_t asubmode = getasubmode(pathi);
	const uint_fast8_t amode = submodes [asubmode].mode;
	const uint_fast8_t bwseti = mdt [amode].bwsetis [tx];
	const uint_fast8_t pos = bwsetpos [bwseti];
	return bwsetsc [bwseti].prop [pos]->type == BWSET_PAIR;
}

uint_fast8_t hamradio_get_low_bp(int_least16_t rotate)
{
	const uint_fast8_t pathi = 0;
	const uint_fast8_t tx = hamradio_get_tx();
	const uint_fast8_t asubmode = getasubmode(pathi);
	const uint_fast8_t amode = submodes [asubmode].mode;
	const uint_fast8_t bwseti = mdt [amode].bwsetis [tx];
	const uint_fast8_t pos = bwsetpos [bwseti];
	bwprop_t * const p = bwsetsc [bwseti].prop [pos];

	uint_fast16_t low;
	switch (p->type)
		{
		case BWSET_PAIR:
			if (rotate != 0 && (p->left10_width10 + rotate) > 0 && (p->left10_width10 + rotate) < p->right100 * 10)
			{
				p->left10_width10 += rotate;
				updateboard();
			}
			low =  p->left10_width10;
			break;

		default:
			ASSERT(0);	// @suppress("No break at end of case")
		case BWSET_SINGLE:
			if (rotate < 0)
			{
				p->left10_width10 = prevfreq(p->left10_width10, p->left10_width10 - p->limits->granulationleft, p->limits->granulationleft, p->limits->left10_width10_low);
				updateboard();
			}
			if (rotate > 0)
			{
				p->left10_width10 = nextfreq(p->left10_width10, p->left10_width10 + p->limits->granulationleft, p->limits->granulationleft, p->limits->left10_width10_high);
				updateboard();
			}

			low = p->left10_width10;
		}
	return low;
}

uint_fast8_t hamradio_get_high_bp(int_least16_t rotate)
{
	const uint_fast8_t pathi = 0;
	const uint_fast8_t tx = hamradio_get_tx();
	const uint_fast8_t asubmode = getasubmode(pathi);
	const uint_fast8_t amode = submodes [asubmode].mode;
	const uint_fast8_t bwseti = mdt [amode].bwsetis [tx];
	const uint_fast8_t pos = bwsetpos [bwseti];
	bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	uint_fast16_t high;

	switch (p->type)
	{
	case BWSET_PAIR:
		if (rotate != 0 && (p->right100 + rotate) * 10 > p->left10_width10 && (p->right100 + rotate) < 50)
		{
			p->right100 += rotate;
			updateboard();
		}
		high =  p->right100;
		break;

	default:
		ASSERT(0); // @suppress("No break at end of case")
	case BWSET_SINGLE:
		if (rotate != 0 && gcwpitch10 + rotate <= CWPITCHMAX10 && gcwpitch10 + rotate >= CWPITCHMIN10)
		{
			gcwpitch10 += rotate;
			updateboard();
		}
		high = gcwpitch10;
	}
	return high;
}

int_fast8_t hamradio_afresponce(int_fast8_t v)
{
	const uint_fast8_t pathi = 0;
	const uint_fast8_t tx = hamradio_get_tx();
	const uint_fast8_t asubmode = getasubmode(pathi);
	const uint_fast8_t amode = submodes [asubmode].mode;
	const uint_fast8_t bwseti = mdt [amode].bwsetis [tx];
	const uint_fast8_t pos = bwsetpos [bwseti];
	bwprop_t * const p = bwsetsc [bwseti].prop [pos];

	if (v > 0)
		p->afresponce = calc_next(p->afresponce, AFRESPONCEMIN, AFRESPONCEMAX);
	else if (v < 0)
		p->afresponce = calc_prev(p->afresponce, AFRESPONCEMIN, AFRESPONCEMAX);

	if (v != 0)
	{
		save_i8(RMT_BWPROPSAFRESPONCE_BASE(bwseti), p->afresponce);
		updateboard();
	}

	return p->afresponce + getafresponcebase();
}

#endif /* WITHIF4DSP */

#if (WITHSWRMTR || WITHSHOWSWRPWR)
uint_fast8_t hamradio_get_gsmetertype(void)
{
	return gsmetertype;
}
#endif /* (WITHSWRMTR || WITHSHOWSWRPWR) */

#if WITHSPECTRUMWF && WITHMENU
const char * hamradio_change_view_style(uint_fast8_t v)
{
	if (v)
	{
		if (param_keyclick(& xgviewstyle))
		{
			updateboard();
		}
	}

	return view_types [param_getvalue(& xgviewstyle)];
}

uint_fast8_t hamradio_get_viewstyle(void)
{
	return param_getvalue(& xgviewstyle);
}

void hamradio_settemp_viewstyle(uint_fast8_t v)
{
	ASSERT(v < VIEW_count);
	gviewstyle = v;
	updateboard();
}

#if WITHSPECTRUMWF && BOARD_FFTZOOM_POW2MAX > 0

uint_fast8_t hamradio_get_gzoomxpow2(void)
{
	return param_getvalue(& xgzoomxpow2);
}

void hamradio_set_gzoomxpow2(uint_fast8_t v)
{
	param_setvalue(& xgzoomxpow2, v);	// Установить значение параметра и сохранить в nvram
	updateboard();
}

/* значения со знаком */
void hamradio_set_bottomdb(int_fast16_t v)
{
	param_setvalue(& xgbottomdb, v);
	updateboard();
}

/* значения со знаком */
void hamradio_set_topdb(int_fast16_t v)
{
	param_setvalue(& xgtopdb, v);
	updateboard();
}

/* значения со знаком */
int_fast16_t hamradio_get_bottomdb(void)
{
	return param_getvalue(& xgbottomdb);
}

/* значения со знаком */
int_fast16_t hamradio_get_topdb(void)
{
	return param_getvalue(& xgtopdb);
}

uint8_t hamradio_get_spectrumpart(void)
{
	return param_getvalue(& xgspectrumpart);
}

void hamradio_set_spectrumpart(uint8_t v)
{
	param_setvalue(& xgspectrumpart, v);
	updateboard();
}

#endif

#endif /* WITHSPECTRUMWF && WITHMENU */

const char * hamradio_get_preamp_value(void)
{
#if ! WITHONEATTONEAMP
	return pampmodes [gpamp].label;
#else
	return attmodes [gatt].label;
#endif
}

uint_fast8_t hamradio_get_att_db(void)
{
	const uint_fast8_t bi = getbankindex_ab_fordisplay(0);	/* VFO A modifications */
	return attmodes [gatt].atten10 / 10;
}

uint_fast8_t hamradio_get_att_dbs(uint_fast8_t * values, uint_fast8_t limit)
{
	const uint_fast8_t bi = getbankindex_ab_fordisplay(0);	/* vfo A bank index */
	for (uint_fast8_t i = 0; i < ATTMODE_COUNT; i ++)
	{
		if ( i > limit)
			break;

		values [i] = attmodes [i].atten10;
	}

	return ATTMODE_COUNT;
}

void hamradio_set_att_db(uint_fast8_t db)
{
	const uint_fast8_t bi = getbankindex_ab_fordisplay(0);	/* VFO A modifications */
	const vindex_t vi = getvfoindex(bi);

	verifyband(vi);
	gatt = db;
	storebandstate(vi, bi);	// запись всех режимов в область памяти диапазона
	updateboard();
}

void hamradio_change_att(void)
{
	uif_key_click_attenuator();
}

#if ! WITHONEATTONEAMP
uint_fast8_t hamradio_change_preamp(uint_fast8_t v)
{
	if (v) uif_key_click_pamp();
	return pampmodes [gpamp].code;
}
#endif /* ! WITHONEATTONEAMP */

#if WITHTX

void hamradio_set_moxmode(uint_fast8_t mode)
{
	if (mode)
		txreq_mox(& txreqst0);
	else
		txreq_rx(& txreqst0, NULL);

	updateboard();
}

uint_fast8_t hamradio_moxmode(uint_fast8_t v)
{
	if (v)
		uif_key_moxclick();
	return txreq_get_tx(& txreqst0);
}

void hamradio_setrx(void)
{
	txreq_rx(& txreqst0, NULL);
}

void hamradio_setautotune(void)
{
	txreq_reqautotune(& txreqst0, 1);
}

uint_fast8_t hamradio_tunemode(uint_fast8_t v)
{
	if (v)
		uif_key_tune();
	return txreq_gettxtone(& txreqst0);
}

#endif /* WITHTX */

#if WITHTOUCHGUI

void hamradio_load_gui_settings(void * ptrv)
{
	uint8_t * ptr = (uint8_t *) ptrv;
	nvramaddress_t offset = OFFSETOF(struct nvmap, gui_nvram);
	const size_t gui_nvram_size = sizeof (struct gui_nvram_t);
	size_t i;

	for (i = 0; i < gui_nvram_size; i ++)
	{
		* ptr ++ = restore_i8(offset ++);
	}
}

void hamradio_save_gui_settings(const void * ptrv)
{
	const uint8_t * ptr = (uint8_t *) ptrv;
	nvramaddress_t offset = OFFSETOF(struct nvmap, gui_nvram);
	const size_t gui_nvram_size = sizeof (struct gui_nvram_t);
	size_t i;

	for (i = 0; i < gui_nvram_size; i ++)
	{
		save_i8(offset ++, * ptr ++);
	}
}

#if WITHENCODER2
void hamradio_gui_enc2_update(void)
{
	const char * const text = enc2menu_label_P(enc2menus [enc2pos]);
	safestrcpy(enc2_menu.param, ARRAY_SIZE(enc2_menu.param), text);
	param_format(enc2menus [enc2pos], enc2_menu.val, ARRAY_SIZE(enc2_menu.val), param_getvalue(enc2menus [enc2pos]));
	enc2_menu.updated = 1;
	enc2_menu.state = enc2state;
	gui_encoder2_menu(& enc2_menu);
}
#else /* WITHENCODER2 */
void hamradio_gui_enc2_update(void)
{
}
#endif /* WITHENCODER2 */

void display2_set_page_temp(uint_fast8_t page)
{
	gmenuset = page;
	//display2_needupdate();
}

#endif /* WITHTOUCHGUI */

#if WITHDSPEXTDDC

void hamradio_set_gdactest(uint8_t v)
{
	gdactest = v != 0;
	updateboard();
}

uint32_t hamradio_get_gadcrand(void)
{
	return gadcrand != 0;
}

#endif /* WITHDSPEXTDDC */

#if WITHUSEDUALWATCH
uint_fast8_t hamradio_split_toggle(void)
{
	if (gsplitmode == VFOMODES_VFOINIT)
		uif_key_spliton(0);
	else
		uif_key_splitoff();

	return gsplitmode != 0;
}

void hamradio_split_mode_toggle(void)
{
	uif_key_mainsubrx();
}

void hamradio_split_vfo_swap(void)
{
	uif_key_click_a_ex_b();
}

uint_fast8_t hamradio_get_gvfoab(void)
{
	return gvfoab;
}
#endif /* WITHUSEDUALWATCH */

// основной цикл программы при работе в режиме любительского премника
void
application_mainloop(void)
{
	hamradio_main_initialize();
	for (;;)
	{
		hamradio_main_step();
#if LINUX_SUBSYSTEM
		usleep(1000);
#endif /* LINUX_SUBSYSTEM */
	}
}

#if 0

static volatile uint16_t f255, f255cnt, f255period;

void beacon_255(void)
{
	if (++ f255cnt == f255period)
	{
		f255cnt = 0;
		f255 = 1;
	}
}

static const char textbeacon [] =
	"00"
	"1011001101010100110010110100101011001010110101010011010011001100"
	"1010110011010101010101001101001011010010110101001011001011001100"
	"1010110011010011001011010010110100110100101100101100101101001101"
	"01010011001100101101001011001010101101010011010"
	"00"
	;

static unsigned textbeaconindex = 0;

static void ff256cycle(unsigned long f, unsigned period)
{
	f255period = period;
	f255cnt = 0;

	synth_lo1_setfreq(f, 1);
	for (;;)
	{
		while (f255 == 0)
			;
		f255 = 0;
		if (textbeacon [textbeaconindex] == '\0')
		{
			textbeaconindex = 0;
			return;
		}
		prog_dds1_setlevel(100 * (textbeacon [textbeaconindex ++] != '0'));
	}

}

struct band
{
	unsigned long freq;
	unsigned period;
};

static void
hamradio_mainloop_beacon(void)
{
	uint8_t ifreq;
	static const struct band ffs [] =
	{
		{ 3548500L + 50,	256, },
		{ 7039500L + 105, 	256, },	//14063710L,
		{ 10136500L + 140, 	256, },	//14063710L,
		{ 14063500L + 210, 	256, },	//14063710L,
		{ 18106500L + 270, 	256, },	//14063710L,
		{ 21075500L + 315, 	256, },	//14063710L,
		{ 24926500L + 390, 	256, },
		{ 28071500L + 420, 	256, },
	};

	hardware_timer_initialize(1000);
	updateboard();	/* полная перенастройка (как после смены режима) - режим приема */
	for (ifreq = 0; ; )
	{
		ff256cycle(ffs [ifreq].freq, ffs [ifreq].period);
		if (++ ifreq >= (sizeof ffs / sizeof ffs [0]))
			ifreq = 0;
	}
}

#endif

#if 0 && WITHDEBUG

static void local_gets(char * buff, size_t len)
{
	size_t pos = 0;

	for (;;)
	{
		char c;
		if (dbg_getchar(& c))
		{
			if (c == '\r')
			{
				dbg_putchar('\n');
				buff [pos] = '\0';
				return;
			}
			if ((pos + 1) >= len)
				continue;
			if (isprint((unsigned char) c))
			{
				buff [pos ++] = c;
				dbg_putchar(c);
				continue;
			}
			if (pos != 0 && c == '\b')
			{
				PRINTF(PSTR("\b \b"));
				-- pos;
				continue;
			}
		}
	}
}

static void siggen_mainloop(void)
{

	PRINTF(PSTR("RF Signal generator\n"));
	uint_fast8_t tx = 0;
	// signal-generator tests
	board_set_attvalue(0);
	updateboard();
	for (;;)
	{
		PRINTF(PSTR("Enter tx=%d, command (a#/g/n):\n"), tx);
		char buff [132];
		local_gets(buff, sizeof buff / sizeof buff [0]);
		char * cp = buff;
		while (isspace((unsigned char) * cp))
			++ cp;
		switch (tolower((unsigned char) * cp))
		{
		case 'a':
			// set att value
			++ cp;
			unsigned long value = _strtoul_r(& treent, cp, NULL, 10);
			PRINTF(PSTR("RFSG ATT value: %lu\n"), value);
			if (value < 63)
			{
				board_set_attvalue(value);
				updateboard();
			}
			break;
		case 'g':
			// generaton on
			PRINTF(PSTR("RFSG output ON\n"));
			tx = 1;
			updateboard();
			break;
		case 'n':
			// generator off
			PRINTF(PSTR("RFSG output OFF\n"));
			tx = 0;
			updateboard();
			break;
		}
	}
}
#endif

#if WITHSUBTONES


// Goertzel

void goertzel_win_initialize(FLOAT_t * w, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		// init window (Hamming)
		w [i] = ((FLOAT_t) 0.54 - (FLOAT_t) 0.46 * COSF(M_TWOPI * (FLOAT_t) i / (FLOAT_t) (n - 1)));
	}
}

// Goertzel constants
typedef struct goeCOEF_tag
{
	FLOAT_t C, CW, SW;
} goeCOEF_t;

// Goertzel status registers
typedef struct goeSTATE_tag
{
	FLOAT_t Z1, Z2;
} goeSTATE_t;

static void goe_initialize(goeCOEF_t * goe, FLOAT_t freq, int_fast32_t Fs, int_fast32_t n)
{
	ASSERT(freq != 0);
	const FLOAT_t w = M_TWOPI * (FLOAT_t) n * freq /(FLOAT_t) Fs / (FLOAT_t) n;
	goe->CW = COSF(w);
	goe->C = goe->CW * 2;// 2 * cosf(w)
	goe->SW = SINF(w);
}

// Goertzel iteration
static void goe_process(const goeCOEF_t * goe, goeSTATE_t * const goes, FLOAT_t x)
{
	const FLOAT_t z0 = x + goe->C * goes->Z1 - goes->Z2;
	goes->Z2 = goes->Z1;
	goes->Z1 = z0;
}

// Goertzel reset
static void goe_reset(goeSTATE_t * const goes)
{
	goes->Z1 = 0;
	goes->Z2 = 0;
}

static FLOAT_t goe_result(const goeCOEF_t * goe, const goeSTATE_t * const goes)
{
	// CORDIC may be used here to compute atan2() and sqrt()
	const FLOAT_t I = goe->CW * goes->Z1 - goes->Z2;      // Goertzel final goeI
	const FLOAT_t Q = goe->SW * goes->Z1;              // Goertzel final goeQ
	return I * I + Q * Q;         // magnitude squared (power)
}

// CTCSS decoding
enum { CTCSS_DECIM = ARMI2SRATE / 800 };

// CTCSS при частоте дискретизации 48/16 = 3 кГц распознается при 2048 и при 1530 точках
// CTCSS при частоте дискретизации 48/32 = 1.5 кГц распознается при 1024 точках

#define goe_ctssLENGTH  	768 // points for Goertzel - зависит точность определения частоты

// IIR filter before decimation
#define CTCSS_LPF_STAGES_IIR 9

// Дециматор для Zoom FFT
#define CTCSS_DECIM_STAGES_FIR 6

static goeCOEF_t ctcssCOEFs [CTCSS_NFREQUES];
static goeSTATE_t ctcssSTATEs [CTCSS_NFREQUES];
static ARM_MORPH(arm_biquad_cascade_df2T_instance) ctcss_iir_config;
static ARM_MORPH(arm_fir_decimate_instance) ctcss_fir_config;
#if defined(ARM_MATH_NEON)
static FLOAT_t ctcss_iir_state [CTCSS_LPF_STAGES_IIR * 8];
#else /* defined(ARM_MATH_NEON) */
static FLOAT_t ctcss_iir_state [CTCSS_LPF_STAGES_IIR * 4];
#endif /* defined(ARM_MATH_NEON) */
static FLOAT_t ctcss_fir_state [CTCSS_DECIM_STAGES_FIR + goe_ctssLENGTH  * CTCSS_DECIM - 1];

static FLOAT_t goertz_ctss_win [goe_ctssLENGTH]; // Window

static std::atomic<int> searchix(-1);

uint_fast8_t hamradio_get_ctcss_active(uint_fast32_t * freq)
{
	int i = searchix;
	if (i > 0)
	{
		* freq = gsubtones [i];
		return 1;
	}
	else
	{
		return 0;
	}
}

static int_fast16_t glob_ctcssrx;

int ctcss_squelch(void)
{
	if (glob_ctcssrx == 0)
		return 1;
	return 1;
}

void
board_set_ctcssrx(
	uint_least16_t tonefreq01)	/* tonefreq - частота в десятых долях герца. частота не-0 - разрешить. */
{
	uint_least16_t v = tonefreq01 * 10;
	if (glob_ctcssrx != v)
	{
		glob_ctcssrx = v;
		//board_dsp1regchanged();
	}
}

void ctcss_processing(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	static FLOAT_t ctcss_buffer [goe_ctssLENGTH * CTCSS_DECIM];	// входные данные для обработки
	static unsigned incount;
	unsigned i;
	unsigned n;

	ctcss_buffer [incount] = ch0;
	if (++ incount < ARRAY_SIZE(ctcss_buffer))
		return;
	incount = 0; // finalize and decode

	// filter
	ARM_MORPH(arm_biquad_cascade_df2T)(& ctcss_iir_config, ctcss_buffer, ctcss_buffer, ARRAY_SIZE(ctcss_buffer));
	if (CTCSS_DECIM > 1)
	{
		// decimator
		ARM_MORPH(arm_fir_decimate)(& ctcss_fir_config, ctcss_buffer, ctcss_buffer, ARRAY_SIZE(ctcss_buffer));
	}
	// обработка буфера после децимации
	for (n = 0; n < goe_ctssLENGTH; ++ n)
	{
		const FLOAT_t x = ctcss_buffer [n] * goertz_ctss_win [n]; // windowing
		// **** GOERTZEL ITERATION ****
		for (i = 1; i < CTCSS_NFREQUES; ++ i)
		{
			const goeCOEF_t * const goe = & ctcssCOEFs [i];
			goeSTATE_t * const goes = & ctcssSTATEs [i];

			goe_process(goe, goes, x);
		} 	// Goertzel status update
	}
	// **** GOERTZEL ITERATION ****

	static FLOAT_t goeM2 [CTCSS_NFREQUES]; // Goertzel output: real, imag, squared magnitude

	for (i = 1; i < CTCSS_NFREQUES; ++ i)
	{
		const goeCOEF_t * const goe = & ctcssCOEFs [i];
		const goeSTATE_t * const goes = & ctcssSTATEs [i];
		goeM2 [i] = goe_result(goe, goes);
	}
	for (i = 1; i < CTCSS_NFREQUES; ++ i)
	{
		goeSTATE_t * const goes = & ctcssSTATEs [i];
		goe_reset(goes);
	} // Goertzel reset

	FLOAT_t noisemax1, max1;
	uint32_t index1;

	ARM_MORPH(arm_max)(goeM2, CTCSS_NFREQUES, & max1, & index1);
	goeM2 [index1] = - 1;
	ARM_MORPH(arm_max_no_idx)(goeM2, CTCSS_NFREQUES, & noisemax1);

	const FLOAT_t goeTH = noisemax1 * 100; // threshold = 10 dB

	if (max1 > goeTH)
	{
		searchix = index1;
		//PRINTF("z%i ", (int) index1);
	}
	else
	{
		searchix = - 1;
	}

}

static void ctcss_initialize(void)
{
	static FLOAT_t ctcss_IIRCoeffs [CTCSS_LPF_STAGES_IIR * 5];
	// IowaHills calculated coeffs:
	static const FLOAT_t ctcss_FIRCoeffs [CTCSS_DECIM_STAGES_FIR] =
	{
			 0.125304137688421385,
			 0.166819426964949558,
			 0.190300554610376316,
			 0.190300554610376316,
			 0.166819426964949558,
			 0.125304137688421385,
	};
	//PRINTF("ctcss_initialize start\n");
	const int_fast32_t samplerate = ARMI2SRATE;	// Hz sampling frequency

	goertzel_win_initialize(goertz_ctss_win, goe_ctssLENGTH);
	//if (1)
	{
		iir_filter_t f0;
		biquad_create(& f0, CTCSS_LPF_STAGES_IIR);
		biquad_init_lowpass(& f0, samplerate, 300);
		fill_biquad_coeffs(& f0, ctcss_IIRCoeffs);
	}
	// filter
	// Initialize floating-point Biquad cascade filter.
	ARM_MORPH(arm_biquad_cascade_df2T_init)(& ctcss_iir_config, CTCSS_LPF_STAGES_IIR, ctcss_IIRCoeffs, ctcss_iir_state);

	if (CTCSS_DECIM > 1)
	{
		// decimator
		VERIFY(ARM_MATH_SUCCESS == ARM_MORPH(arm_fir_decimate_init)(& ctcss_fir_config,
							CTCSS_DECIM_STAGES_FIR,
							CTCSS_DECIM,          // Decimation factor
							ctcss_FIRCoeffs,
							ctcss_fir_state,            // Filter state variables
							goe_ctssLENGTH * CTCSS_DECIM));
	}


	unsigned i;
	for (i = 1; i < CTCSS_NFREQUES; ++ i)
	{
		// init Goertzel constants
		goeCOEF_t * const goe = & ctcssCOEFs [i];
		goe_initialize(goe, (FLOAT_t) gsubtones [i] / 10, samplerate / CTCSS_DECIM, goe_ctssLENGTH);
	}

	for (i = 1; i < CTCSS_NFREQUES; ++ i)
	{
		goeSTATE_t * const goes = & ctcssSTATEs [i];
		goe_reset(goes);
	} // Goertzel reset

	static subscribefloat_t ctcss_register;
	subscribefloat(& speexinfloat, & ctcss_register, NULL, ctcss_processing);	// выход приёмника до фильтров
	//PRINTF("ctcss_initialize done\n");
}

// DTMF decoding

#define goe_dtmfLENGTH  	2048 // points for Goertzel - зависит точность определения частоты


// DTMF frequencies
static const int frow [4] = { 697, 770, 852, 941 }; // 1st tone
static const int fcol [4] = { 1209, 1336, 1477, 1633 }; // 2nd tone

// DTMF symbols
static const char sym [16] =
{
	'1', '4', '7', '*', '2', '5', '8', '0', '3', '6', '9', '#', 'A', 'B',
	'C', 'D'
};

// DTMF decoding matrix
static const int symmtx[4][4] =
{
	{ 0, 4, 8, 12 },
	{ 1, 5, 9, 13 },
	{ 2, 6, 10, 14 },
	{ 3, 7, 11, 15 }
};

#define DTMF_NFREQUES 8
static goeCOEF_t goeCOEFs [DTMF_NFREQUES];
static goeSTATE_t goeSTATEs [DTMF_NFREQUES];

static void dtmf_out(void * ctx, char c)
{
	PRINTF("%c", c);
}

const struct qmkey * dtmf_get_pressed_pkey(void)
{
	return NULL;
}

static FLOAT_t goertz_dtmf_win [goe_dtmfLENGTH]; // Window

void dtmf_processing(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	static unsigned n;
	unsigned i;

	const FLOAT_t x = ch0 * goertz_dtmf_win [n]; // windowing
	// **** GOERTZEL ITERATION ****
	for (i = 0; i < DTMF_NFREQUES; ++ i)
	{
		const goeCOEF_t * const goe = & goeCOEFs [i];
		goeSTATE_t * const goes = & goeSTATEs [i];

		goe_process(goe, goes, x);
	} 	// Goertzel status update

	// **** GOERTZEL ITERATION ****
	if (++ n >= goe_dtmfLENGTH)
	{
		// finalize and decode
		n = 0;

		static FLOAT_t goeM2 [DTMF_NFREQUES]; // Goertzel output: real, imag, squared magnitude

		for (i = 0; i < DTMF_NFREQUES; ++ i)
		{
			const goeCOEF_t * const goe = & goeCOEFs [i];
			const goeSTATE_t * const goes = & goeSTATEs [i];
			goeM2 [i] = goe_result(goe, goes);

		}
		// Initial state
		for (i = 0; i < DTMF_NFREQUES; ++ i)
		{
			goeSTATE_t * const goes = & goeSTATEs [i];
			goe_reset(goes);
		} // Goertzel reset

		// Определение наличия двух тонов
		FLOAT_t noisemax1, max1, max2;
		uint32_t index1, index2;

		ARM_MORPH(arm_max)(goeM2 + 0, DTMF_NFREQUES / 2, & max1, & index1);
		goeM2 [index1] = - 1;
		ARM_MORPH(arm_max)(goeM2 + 4, DTMF_NFREQUES / 2, & max2, & index2);
		index2 += 4;
		goeM2 [index2] = - 1;
		ARM_MORPH(arm_max_no_idx)(goeM2, DTMF_NFREQUES, & noisemax1);

		const FLOAT_t goeTH = noisemax1 * 100; // threshold = 10 dB

		if (max1 > goeTH && max2 > goeTH)
		{
			//PRINTF("i1=%u i2=%u\n", index1, index2);
			if (index1 >= 4 && index2 < 4)
			{
				dtmf_out(ctx, sym [symmtx [index2] [index1 - 4]]);
				//PRINTF(" %u %u %u\n", (int) (100 * goeTH), (int) (100 * max1), (int) (100 * max2));
			}
			else if (index1 < 4 && index2 >= 4)
			{
				dtmf_out(ctx, sym [symmtx [index1] [index2 - 4]]);
				//PRINTF(" %u %u %u\n", (int) (100 * goeTH), (int) (100 * max1), (int) (100 * max2));
			}
			else
			{
				// Равны или в одной группе
			}
		}
	}

}

static void dtmf_initialize(void)
{
	//PRINTF("dtmf_initialize start\n");
	const int_fast32_t Fs = ARMI2SRATE;	// Hz sampling frequency
	unsigned i;
	goertzel_win_initialize(goertz_dtmf_win, goe_dtmfLENGTH);

	for (i = 0; i < 4; i++)
	{
		// init Goertzel constants
		// CORDIC may be used here to compute sin() and cos()
		goeCOEF_t * const goe = & goeCOEFs [i];
		goe_initialize(goe, frow [i], Fs, goe_dtmfLENGTH);
	}

	for (i = 0; i < 4; i++)
	{
		// init Goertzel constants

		goeCOEF_t * const goe = & goeCOEFs [i + 4];
		goe_initialize(goe, fcol [i], Fs, goe_dtmfLENGTH);
	}
	for (i = 0; i < DTMF_NFREQUES; ++ i)
	{
		goeSTATE_t * const goes = & goeSTATEs [i];
		goe_reset(goes);
	} // Goertzel reset

	static subscribefloat_t dtmf_register;
	subscribefloat(& speexoutfloat, & dtmf_register, NULL, dtmf_processing);	// выход speex и фильтра
	//PRINTF("dtmf_initialize done\n");
}

#else /* WITHSUBTONES */

int ctcss_squelch(void)
{
	return 1;
}

#endif /* WITHSUBTONES */


/* вызывается при разрешённых прерываниях. */
void
application_initialize(void)
{
	/* NVRAM уже можно пользоваться */
#if WITHMENU && ! HARDWARE_IGNORENONVRAM
	loadsettings();		/* загрузка всех установок из nvram. */
#endif /* WITHMENU && ! HARDWARE_IGNORENONVRAM */
	//extmenu = extmenu || alignmode;
	loadsavedstate();	// split, lock, s-meter display, see also loadsettings().
	loadnewband(getvfoindex(1), 1);	/* загрузка последнего сохраненного состояния - всегда VFO или MEMxx */
	loadnewband(getvfoindex(0), 0);	/* загрузка последнего сохраненного состояния - всегда VFO или MEMxx */

	synthcalc_init();
	bandf_calc_initialize();
	bandf2_calc_initialize();
	bandf3_calc_initialize();
#if XVTR_R820T2
	//board_set_bandfxvrtr(bandf_calc(R820T_IFFREQ))	// Этот диапазон подставляется как ПЧ для трансвертора
#endif /* XVTR_R820T2 */
	board_init_chips();	// программирование всех микросхем синтезатора.

#if WITHUSESDCARD
	sdcardhw_initialize();
#endif /* WITHUSESDCARD */
#if WITHCAT
	cat_initialize();
#endif /* WITHCAT */
#if WITHUSERAMDISK
	{
		ALIGNX_BEGIN BYTE work [FF_MAX_SS] ALIGNX_END;
		FRESULT rc;
		PRINTF(PSTR("ramdisk: start formatting\n"));
		rc = f_mkfs("0:", NULL, work, sizeof (work));
		if (rc != FR_OK)
		{
			PRINTF(PSTR("ramdisk: f_mkfs failure\n"));
		}
		else
		{
			PRINTF(PSTR("ramdisk: f_mkfs okay\n"));
		}
	}
#endif
#if WITHUSEAUDIOREC
	sdcardinitialize();			// перевод state machine в начальное состояние
#endif /* WITHUSEAUDIOREC */

#if WITHMODEM
	modem_initialze();
#endif /* WITHMODEM */

#if WITHINTEGRATEDDSP	/* в программу включена инициализация и запуск DSP части. */
	dsp_initialize();		// цифровая обработка подготавливается
	InitNoiseReduction();
#if WITHRTTY
	RTTYDecoder_Init();
#endif /* WITHRTTY */
#if WITHFT8
	ft8_initialize();
#endif /* WITHFT8 */

#if WITHSUBTONES && 1
	dtmf_initialize();
	ctcss_initialize();
#endif /* WITHSUBTONES */

#endif /* WITHINTEGRATEDDSP */

#if WITHUSBHW
	if (bootloader_withusb())
		board_usb_activate();		// USB device and host start
#endif /* WITHUSBHW */

	// TODO: у аудио кодека и IF кодека могут быть раные требования
#if defined(CODEC1_TYPE)
	/* требуется ли подача тактирования для инициализации кодека */
	const int reqclk = board_getaudiocodecif()->clocksneed();
#else /* defined(CODEC1_TYPE) */
	const int reqclk = 1;
#endif /* defined(CODEC1_TYPE) */

	if (reqclk != 0)
	{
		hardware_channels_enable();	// SAI, I2S и подключенная на них периферия
		board_set_i2s_enable(1);	// Разрешить FPGA формирование тактовой частоты для кодеков и тактирование I2S
		board_update();
	}

	board_init_chips2();	// программирование кодеков при подающейся тактовой частоте

	if (reqclk == 0)
	{
		hardware_channels_enable();	// SAI, I2S и подключенная на них периферия
		board_set_i2s_enable(1);	// Разрешить FPGA формирование тактовой частоты для кодеков и тактирование I2S
		board_update();
	}

#if WITHTOUCHGUI
	gui_initialize();

#if WITHENCODER2
	hamradio_gui_enc2_update();
#endif /* WITHENCODER2 */
#endif /* WITHTOUCHGUI */
#if WITHUSEUSBBT
	bt_initialize();
#endif /* WITHUSEUSBBT */

}

// LVGL interface functions
int infocb_modea(char * b, size_t len, int * pstate)
{
	return local_snprintf_P(b, len, "%s", hamradio_get_mode_a_value_P());
}

int infocb_modeb(char * b, size_t len, int * pstate)
{
	uint_fast8_t state;
	const char * const name = hamradio_get_mode_b_value_P(& state);
	* pstate = state;
	return local_snprintf_P(b, len, "%s", name);
}

int infocb_ant5(char * b, size_t len, int * pstate)
{
#if WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2 || WITHANTSELECT
	return local_snprintf_P(b, len, "%s", hamradio_get_ant5_value());
#else
	return 0;
#endif /* xxx */
}

int infocb_preamp_ovf(char * b, size_t len, int * pstate)
{
	return local_snprintf_P(b, len, "%s", hamradio_get_pre_value());
}

int infocb_tune(char * b, size_t len, int * pstate)
{
#if WITHTX && WITHAUTOTUNER
	const uint_fast8_t state = hamradio_get_tunemodevalue();
	* pstate = state;
	return local_snprintf_P(b, len, "%s", state ? "TUN" : "");
#else
	return 0;
#endif
}

int infocb_bypass(char * b, size_t len, int * pstate)
{
#if WITHAUTOTUNER
	const uint_fast8_t state = hamradio_get_bypvalue();
	* pstate = state;
	return local_snprintf_P(b, len, "%s", state ? "BYP" : "TUN");
#else
	return 0;
#endif
}

int infocb_rxbwval(char * b, size_t len, int * pstate)
{
	return local_snprintf_P(b, len, "%s", hamradio_get_rxbw_value4());
}

#if WITHELKEY
int infocb_wpm(char * b, size_t len, int * pstate)
{
	return local_snprintf_P(b, len, "%uwpm", (int) hamradio_get_cw_wpm());
}
#endif /* WITHELKEY */

#if WITHUSEAUDIOREC
int infocb_rec(char * b, size_t len, int * pstate)
{
	const uint_fast8_t state = hamradio_get_rec_value();	// не-0: запись включена
	* pstate = state;
	return local_snprintf_P(b, len, "%s", state ? "REC" : "PAU");
}
#endif /* WITHUSEAUDIOREC */

int infocb_spk(char * b, size_t len, int * pstate)
{
	const uint_fast8_t state = hamradio_get_spkon_value();	// не-0: динамик включен
	* pstate = state;
	return local_snprintf_P(b, len, "%s", state ? "SPK" : "");
}

int infocb_bkin(char * b, size_t len, int * pstate)
{
	const uint_fast8_t state = hamradio_get_bkin_value();	// не-0: break-in включен
	* pstate = state;
	return local_snprintf_P(b, len, "%s", state ? "BKIN" : "BKIN");
}

int infocb_usbact(char * b, size_t len, int * pstate)
{
#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
	const uint_fast8_t state = hamradio_get_usbmsc_active();	// не-0: USB active
	* pstate = state;
	return local_snprintf_P(b, len, "%s", state ? "USB" : "USB");
#else
	return 0;
#endif
}

int infocb_btact(char * b, size_t len, int * pstate)
{
#if WITHUSEUSBBT && (defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI))

	const uint_fast8_t state = hamradio_get_usbbth_active();	// не-0: USB active
	* pstate = state;
	return local_snprintf_P(b, len, "%s", state ? "BT" : "BT");
#else
	return 0;
#endif
}

int infocb_vfomode(char * b, size_t len, int * pstate)
{
	uint_fast8_t state;	// state - признак активного SPLIT (0/1)
	const char * const label = hamradio_get_vfomode3_value(& state);
	return local_snprintf_P(b, len, "%s", label);
}

int infocb_classa(char * b, size_t len, int * pstate)
{
	const uint_fast8_t state = hamradio_get_classa();	// не-0: Class-A active
	return local_snprintf_P(b, len, "%s", state ? "Class-A" : "");
}

int infocb_nr(char * b, size_t len, int * pstate)
{
	int_fast32_t grade;
	const uint_fast8_t state = hamradio_get_nrvalue(& grade);
	return local_snprintf_P(b, len, "%s", state ? "NR" : "");
}

int infocb_txrx(char * b, size_t len, int * pstate)
{
	const uint_fast8_t state = hamradio_get_tx();
	* pstate = state;
	return local_snprintf_P(b, len, "%s", state ? "TX" : "RX");
}

int infocb_notch(char * b, size_t len, int * pstate)
{
#if WITHNOTCHONOFF || WITHNOTCHFREQ
	int_fast32_t freq;
	const uint_fast8_t state = hamradio_get_notchvalue(& freq);
	const char * const label = hamradio_get_notchtype5_P();
	const char * const labels [2] = { label, label, };
	return local_snprintf_P(b, len, "%s", labels [state]);
#else
	return 0;
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
}

int infocb_agc(char * b, size_t len, int * pstate)
{
	return local_snprintf_P(b, len, "%s", hamradio_get_agc3_value());
}

int infocb_lock(char * b, size_t len, int * pstate)
{
	const uint_fast8_t lockv = hamradio_get_lockvalue();
	const uint_fast8_t fastv = hamradio_get_usefastvalue();

	static const char text0 [] = "";
	static const char text1 [] = "LOCK";
	static const char text2 [] = "FAST";
	const char * const labels [4] = { text1, text2, text1, text1, };
	return local_snprintf_P(b, len, "%s", labels [lockv * 2 + fastv]);
}

int infocb_voxtune(char * b, size_t len, int * pstate)
{
	static const char text_vox [] = "VOX";
	static const char text_tun [] = "TUN";
	static const char text_nul [] = "";

	const uint_fast8_t tunev = hamradio_get_tunemodevalue();
	const uint_fast8_t voxv = hamradio_get_voxvalue();

	const char * const labels [4] = { text_nul, text_vox, text_tun, text_tun, };

	return local_snprintf_P(b, len, "%s", labels [tunev * 2 + voxv]);
}

int infocb_datamode(char * b, size_t len, int * pstate)
{
	const uint_fast8_t state = hamradio_get_datamode();	// не-0: USB active
	return local_snprintf_P(b, len, "%s", state ? "DAT" : "");
}

int infocb_voltlevel(char * b, size_t len, int * pstate)
{
#if WITHVOLTLEVEL
	const int voltx = hamradio_get_volt_value();	// Напряжение в сотнях милливольт т.е. 151 = 15.1 вольта
	const int volts = voltx / 10;
	const int volts01 = voltx > 0 ? (voltx % 10) : (- voltx % 10);
	return local_snprintf_P(b, len, "%d.%dV", volts, volts01);
#else
	return 0;
#endif
}

// Печать времени - только часы и минуты, без секунд
// Jan-01 13:40
int infocb_datetime12(char * b, size_t len, int * pstate)
{
#if defined (RTC1_TYPE)

	uint_fast16_t year;
	uint_fast8_t month, day;
	uint_fast8_t hour, minute, seconds;
	static const char months [12] [4] =
	{
		"JAN",
		"FEB",
		"MAR",
		"APR",
		"MAY",
		"JUN",
		"JUL",
		"AUG",
		"SEP",
		"OCT",
		"NOV",
		"DEC",
	};

	board_rtc_cached_getdatetime(& year, & month, & day, & hour, & minute, & seconds);

	return local_snprintf_P(b, len, "%s-%02d %02d%c%02d",
		months [month - 1],
		(int) day,
		(int) hour,
		((seconds & 1) ? ' ' : ':'),	// мигающее двоеточие с периодом две секунды
		(int) minute
		);
#else
	return 0;
#endif
}

int infocb_attenuator(char * b, size_t len, int * pstate)
{
	return local_snprintf_P(b, len, "%s", hamradio_get_att_value_P());
}

static void xsplit_freq(uint64_t freq, unsigned * mhz, unsigned * khz, unsigned * hz)
{
    * mhz = freq / 1000000;
    * khz = (freq / 1000) % 1000;
    * hz = freq % 1000;
}

int infocb_freqa(char * b, size_t len, int * pstate)
{
	unsigned mhz, khz, hz;

	xsplit_freq(hamradio_get_freq_a(), & mhz, & khz, & hz);
	return local_snprintf_P(b, len, "%u.%03u.%03u", mhz, khz, hz);
}

int infocb_freqb(char * b, size_t len, int * pstate)
{
	unsigned mhz, khz, hz;
	uint_fast8_t state;
	hamradio_get_vfomode3_value(& state);
	* pstate = state;

	xsplit_freq(hamradio_get_freq_b(), & mhz, & khz, & hz);
	return local_snprintf_P(b, len, "%u.%03u.%03u", mhz, khz, hz);
}

int infocb_rxbw(char * b, size_t len, int * pstate)
{
	return local_snprintf_P(b, len, "%s", hamradio_get_rxbw_label3());
}

int infocb_siglevel(char * b, size_t len, int * pstate)
{
#if WITHIF4DSP
	int_fast16_t tracemaxi10;
	int_fast16_t rssi10 = dsp_rssi10(& tracemaxi10, 0);	/* получить значение уровня сигнала для s-метра в 0.1 дБмВт */

	// в формате при наличии знака числа ширина формата отностися ко всему полю вместе со знаком
	return local_snprintf_P(b, len, PSTR("%-+4d" "dBm"), (int) (tracemaxi10 / 10));
#else
	return 0;
#endif
}

int infocb_currlevel(char * b, size_t len, int * pstate)
{
#if WITHCURRLEVEL || WITHCURRLEVEL2
	int_fast16_t drainx = hamradio_get_pacurrent_value();	// Ток в десятках милиампер (до 2.55 ампера), может быть отрицательным

	const int draina = drainx / 100;
	const int drains01a = drainx > 0 ? (drainx % 100) : (- drainx % 100);
	return local_snprintf_P(b, len, "%d.%dA", draina, drains01a);
#else
	return 0;
#endif
}

int infocb_thermo(char * b, size_t len, int * pstate)
{
#if (WITHTHERMOLEVEL || WITHTHERMOLEVEL2)
	int_fast16_t tempv = hamradio_get_PAtemp_value();	// Градусы в десятых долях

	// 50+ - красный
	// 30+ - желтый
	// ниже 30 зеленый
	if (tempv > 999)
		tempv = 999; //- tempv;

	if (tempv < 0)
	{
		tempv = 0; //- tempv;
	}
	else if (tempv >= 500)
	{
		;
	}
	else if (tempv >= 300)
	{
		;
	}
	else
	{
		;
	}
	const int thermoa = tempv / 10;
	const int thermos01a = tempv > 0 ? (tempv % 10) : (- tempv % 10);
	return local_snprintf_P(b, len, "%d.%d", thermoa, thermos01a);
#else
	return 0;
#endif
}
