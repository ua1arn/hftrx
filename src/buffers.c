/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints

//#define WITHBUFFERSDEBUG WITHDEBUG
//#undef SPIN_LOCK
//#undef SPIN_UNLOCK
//#define SPIN_LOCK(p) do { (void) p; } while (0)
//#define SPIN_UNLOCK(p) do { (void) p; } while (0)

#if WITHINTEGRATEDDSP

#include "list.h"
#include "audio.h"
#include "usb/usb200.h"
#include "usb/usbch9.h"

#include <string.h>		// for memset

#if defined(WITHRTS96) && defined(WITHRTS192)
	#error Configuration Error: WITHRTS96 and WITHRTS192 can not be used together
#endif /* defined(WITHRTS96) && defined(WITHRTS192) */
enum
{
	BUFFTAG_UACIN16 = 44,
	BUFFTAG_RTS192,
	BUFFTAG_RTS96,
	BUFFTAG_total
};

/* отладочные врапперы для функций работы со списком - позволяют получить размер очереди */
typedef struct listcnt
{
	unsigned long tag2;
	LIST_ENTRY item0;
	unsigned Count;	// количество элментов в списке
	//SPINLOCK_t lock2;
	unsigned long tag3;
} LIST_HEAD2, * PLIST_HEAD2;

#define LIST2TAG2 0xABBA1990uL
#define LIST2TAG3 0xDEADBEEFuL

#define LIST2PRINT(name) do { \
		PRINTF(PSTR(# name "[%3d] "), (int) name.Count); \
	} while (0)

static void
InitializeListHead2(LIST_HEAD2 * ListHead)
{
	ListHead->tag2 = LIST2TAG2;
	ListHead->tag3 = LIST2TAG3;
	//SPINLOCK_INITIALIZE(& ListHead->lock2);
	(ListHead)->Count = 0;
	InitializeListHead(& (ListHead)->item0);
}

static int
IsListEmpty2(LIST_HEAD2 * ListHead/*, const char * file, int line*/)
{
	//SPIN_LOCK2(& ListHead->lock2, file, line);
	ASSERT(ListHead->tag2 == LIST2TAG2 && ListHead->tag3 == LIST2TAG3);
	ASSERT(ListHead->item0.Flink != NULL && ListHead->item0.Blink != NULL);
	const int v = (ListHead)->Count == 0;
	//SPIN_UNLOCK2(& ListHead->lock2);
	return v;
}

static void
(InsertHeadList2)(PLIST_HEAD2 ListHead, PLIST_ENTRY Entry/*, const char * file, int line*/)
{
	ASSERT(ListHead->tag2 == LIST2TAG2 && ListHead->tag3 == LIST2TAG3);
	//SPIN_LOCK2(& ListHead->lock2, file, line);
	ASSERT(ListHead->item0.Flink != NULL && ListHead->item0.Blink != NULL);
	(ListHead)->Count += 1;
	InsertHeadList(& (ListHead)->item0, (Entry));
	//SPIN_UNLOCK2(& ListHead->lock2);
}

static PLIST_ENTRY
(RemoveTailList2)(PLIST_HEAD2 ListHead/*, const char * file, int line*/)
{
	ASSERT(ListHead->tag2 == LIST2TAG2 && ListHead->tag3 == LIST2TAG3);
	//SPIN_LOCK2(& ListHead->lock2, file, line);
	ASSERT(ListHead->item0.Flink != NULL && ListHead->item0.Blink != NULL);
	ASSERT((ListHead)->Count != 0);
	ASSERT(! IsListEmpty(& (ListHead)->item0));
	(ListHead)->Count -= 1;
	const PLIST_ENTRY t = RemoveTailList(& (ListHead)->item0);	/* прямо вернуть значение RemoveTailList нельзя - Microsoft сделал не совсем правильный макрос. Но по другому и не плучилось бы в стандартном языке C. */
	//SPIN_UNLOCK2(& ListHead->lock2);
	return t;
}

static unsigned GetCountList2(LIST_HEAD2 * ListHead/*, const char * file, int line*/)
{
	ASSERT(ListHead->tag2 == LIST2TAG2 && ListHead->tag3 == LIST2TAG3);
	//SPIN_LOCK2(& ListHead->lock2, file, line);
	ASSERT(ListHead->item0.Flink != NULL && ListHead->item0.Blink != NULL);
	const unsigned count = (ListHead)->Count;
	//SPIN_UNLOCK2(& ListHead->lock2);
	return count;
}

//#define InsertHeadList2(h, e) (InsertHeadList2)((h), (e), __FILE__, __LINE__)
//#define RemoveTailList2(h) (RemoveTailList2)((h), __FILE__, __LINE__)
//#define GetCountList2(h) (GetCountList2)((h), __FILE__, __LINE__)
//#define IsListEmpty2(h) (IsListEmpty2)((h), __FILE__, __LINE__)

/* готовность буферов с "гистерезисом". */
static uint_fast8_t fiforeadyupdate(
	uint_fast8_t ready,		// текущее состояние готовности
	unsigned Count,		// сколько элементов сейчас в очереди
	unsigned normal		// граница включения готовности
	)
{
	return ready ? Count != 0 : Count >= normal;
}


/* отладочные врапперы для функций работы со списком - позволяют получить размер очереди */
typedef struct listcnt3
{
	LIST_HEAD2 item2;
	unsigned RdyLevel;	// Требуемое количество
	unsigned Rdy;		// количество элментов в списке
} LIST_HEAD3, * PLIST_HEAD3;

static int
IsListEmpty3(LIST_HEAD3 * ListHead)
{
	return IsListEmpty2(& (ListHead)->item2);
}

static void
InitializeListHead3(LIST_HEAD3 * ListHead, unsigned RdyLevel)
{
	(ListHead)->Rdy = 0;
	(ListHead)->RdyLevel = RdyLevel;
	InitializeListHead2(& (ListHead)->item2);
}

// forceReady - если в источнике данных закончился поток.
static void
InsertHeadList3(PLIST_HEAD3 ListHead, PLIST_ENTRY Entry, uint_fast8_t forceReady)
{
	InsertHeadList2(& (ListHead)->item2, (Entry));
	(ListHead)->Rdy = forceReady || fiforeadyupdate((ListHead)->Rdy, (ListHead)->item2.Count, (ListHead)->RdyLevel);
}

static PLIST_ENTRY
RemoveTailList3(PLIST_HEAD3 ListHead)
{
	const PLIST_ENTRY t = RemoveTailList2(& (ListHead)->item2);
	(ListHead)->Rdy = fiforeadyupdate((ListHead)->Rdy, (ListHead)->item2.Count, (ListHead)->RdyLevel);
	return t;
}

static unsigned GetCountList3(LIST_HEAD3 * ListHead)
{
	return GetCountList2(& (ListHead)->item2);
}

static uint_fast8_t GetReadyList3(const LIST_HEAD3 * ListHead)
{
	return (ListHead)->Rdy;
}

#define LIST3PRINT(name) do { \
		PRINTF(PSTR(# name "[%3d] "), (int) GetCountList3(& (name))); \
	} while (0)

#if 0
static RAMDTCM int16_t vfyseq;
static RAMDTCM int16_t lastseq;
static RAMDTCM int lastseqvalid;

int16_t vfydataget(void)
{
	return ++ vfyseq;
}

void vfydata(int16_t v)
{
	if (lastseqvalid == 0)
	{
		lastseq = v;
		lastseqvalid = 1;
		return;
	}
	++ lastseq;
	ASSERT(lastseq == v);
}

void vfyalign2(void * p)
{
	ASSERT(((uintptr_t) p & 0x01) == 0);
}

void vfyalign4(void * p)
{
	ASSERT(((uintptr_t) p & 0x03) == 0);
}

void vfylist(LIST_HEAD2 * head)
{
	LIST_ENTRY * list = & head->item0;
	LIST_ENTRY * t;
	for (t = list->Flink; t != list; t = t->Flink)
	{
		vfyalign4(t);
	}
}
#endif
//////////////////////////////////
// Система буферизации аудиоданных
//
// Audio CODEC in/out
typedef ALIGNX_BEGIN struct voice16_tag
{
	void * tag2;
	ALIGNX_BEGIN aubufv_t buff [DMABUFFSIZE16] ALIGNX_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
	void * tag3;
} ALIGNX_END voice16_t;

int_fast32_t buffers_dmabuffer16cachesize(void)
{
	return offsetof(voice16_t, item) - offsetof(voice16_t, buff);
}

// I/Q data to FPGA or IF CODEC
typedef ALIGNX_BEGIN struct voices32tx_tag
{
	void * tag2;
	ALIGNX_BEGIN int32_t buff [DMABUFFSIZE32TX] ALIGNX_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
	void * tag3;
} ALIGNX_END voice32tx_t;

int_fast32_t buffers_dmabuffer32txcachesize(void)
{
	return offsetof(voice32tx_t, item) - offsetof(voice32tx_t, buff);
}

// I/Q data from FPGA or IF CODEC
typedef ALIGN1K_BEGIN struct voices32rx_tag
{
	ALIGN1K_BEGIN int32_t buff [DMABUFFSIZE32RX] ALIGN1K_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
} ALIGN1K_END voice32rx_t;
// исправляемая погрешность = 0.02% - один сэмпл добавить/убрать на 5000 сэмплов

int_fast32_t buffers_dmabuffer32rxcachesize(void)
{
	return offsetof(voice32rx_t, item) - offsetof(voice32rx_t, buff);
}

enum { SKIPPED = 5000 / (DMABUFFSIZE16 / DMABUFSTEP16) };
enum { VOICESMIKE16NORMAL = 5 };	// Нормальное количество буферов в очереди
enum { RESAMPLE16NORMAL = SKIPPED * 2 };	// Нормальное количество буферов в очереди
enum { CNT16 = DMABUFFSIZE16 / DMABUFSTEP16 };
enum { CNT32RX = DMABUFFSIZE32RX / DMABUFSTEP32RX };
//enum { PHONESLEVELx = CNT16 / CNT32RX };
enum { PHONESLEVEL = 8 };

static RAMDTCM LIST_HEAD3 voicesmike16;	// буферы с оцифрованными звуками с микрофона/Line in
static RAMDTCM LIST_HEAD3 resample16;		// буферы от USB для синхронизации

static RAMDTCM LIST_HEAD2 voicesfree16;
static RAMDTCM LIST_HEAD2 voicesphones16;	// буферы, предназначенные для выдачи на наушники
static RAMDTCM LIST_HEAD2 voicesmoni16;	// буферы, предназначенные для звука самоконтроля
static RAMDTCM SPINLOCK_t locklist16 = SPINLOCK_INIT;

static RAMDTCM LIST_HEAD2 voicesready32tx;	// буферы, предназначенные для выдачи на IF DAC
static RAMDTCM LIST_HEAD2 voicesfree32tx;
static RAMDTCM SPINLOCK_t locklist32tx = SPINLOCK_INIT;

static RAMDTCM LIST_HEAD2 voicesfree32rx;
static RAMDTCM SPINLOCK_t locklist32rx = SPINLOCK_INIT;

static RAMDTCM LIST_HEAD2 speexfree16;		// Свободные буферы
static RAMDTCM LIST_HEAD2 speexready16;	// Буферы для обработки speex
static RAMDTCM SPINLOCK_t speexlock = SPINLOCK_INIT;

static RAMDTCM volatile uint_fast8_t uacoutplayer = 0;	/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
static RAMDTCM volatile uint_fast8_t uacoutmike = 0;	/* на вход трансивера берутся аудиоданные с USB виртуальной платы, а не с микрофона */
static RAMDTCM volatile uint_fast8_t uacinalt = UACINALT_NONE;		/* выбор альтернативной конфигурации для UAC IN interface */
static RAMDTCM volatile uint_fast8_t uacinrtsalt = UACINRTSALT_NONE;		/* выбор альтернативной конфигурации для RTS UAC IN interface */
static RAMDTCM volatile uint_fast8_t uacoutalt;

static void savesampleout16stereo_user(void * ctx, FLOAT_t ch0, FLOAT_t ch1);
static void savesampleout16stereo(void * ctx, FLOAT_t ch0, FLOAT_t ch1);

#if WITHUSBUACIN

// USB AUDIO IN
typedef ALIGNX_BEGIN struct uacin16_tag
{
	LIST_ENTRY item;	// layout should be same in uacin16_t, voice96rts_t and voice192rts_t
	uint_fast8_t tag;	// layout should be same in uacin16_t, voice96rts_t and voice192rts_t
	void * tag2;
	ALIGNX_BEGIN union
	{
		uint16_t buff [DMABUFFSIZEUACIN16];
		uint8_t filler [(DMABUFFSIZEUACIN16 * 2 + DCACHEROWSIZE - 1) / DCACHEROWSIZE * DCACHEROWSIZE / 2];
	} u ALIGNX_END;		// спектр, 2*24*192 kS/S
	void * tag3;
} ALIGNX_END uacin16_t;

int_fast32_t buffers_dmabufferuacin16cachesize(void)
{
	return (DMABUFFSIZE192RTS + DCACHEROWSIZE - 1) / DCACHEROWSIZE * DCACHEROWSIZE;
}

#if WITHRTS192

	typedef ALIGNX_BEGIN struct voices192rts
	{
		LIST_ENTRY item;	// layout should be same in uacin16_t, voice96rts_t and voice192rts_t
		uint_fast8_t tag;	// layout should be same in uacin16_t, voice96rts_t and voice192rts_t
		void * tag2;
		ALIGNX_BEGIN union
		{
			uint8_t buff [DMABUFFSIZE192RTS];
			uint8_t	filler [(DMABUFFSIZE192RTS + DCACHEROWSIZE - 1) / DCACHEROWSIZE * DCACHEROWSIZE];
		} u ALIGNX_END;		// спектр, 2*24*192 kS/S
		void * tag3;
	} ALIGNX_END voice192rts_t;

	int_fast32_t buffers_dmabuffer192rtscachesize(void)
	{
		return (DMABUFFSIZE192RTS + DCACHEROWSIZE - 1) / DCACHEROWSIZE * DCACHEROWSIZE;
	}

	static RAMDTCM LIST_HEAD2 voicesfree192rts;
	static RAMDTCM LIST_HEAD2 uacin192rts;	// Буферы для записи в вудиоканал USB к компьютеру спектра, 2*32*192 kS/S

#endif /* WITHRTS192 */

#if WITHRTS96

	typedef ALIGNX_BEGIN struct voices96rts
	{
		LIST_ENTRY item;	// layout should be same in uacin16_t, voice96rts_t and voice192rts_t
		uint_fast8_t tag;	// layout should be same in uacin16_t, voice96rts_t and voice192rts_t
		void * tag2;
		ALIGNX_BEGIN union
		{
			uint8_t buff [DMABUFFSIZE96RTS];
			uint8_t	 filler [(DMABUFFSIZE96RTS + DCACHEROWSIZE - 1) / DCACHEROWSIZE * DCACHEROWSIZE];
		} u ALIGNX_END;		// спектр, 2*24*192 kS/S
		void * tag3;
	} ALIGNX_END voice96rts_t;

	int_fast32_t buffers_dmabuffer96rtscachesize(void)
	{
		return (DMABUFFSIZE96RTS + DCACHEROWSIZE - 1) / DCACHEROWSIZE * DCACHEROWSIZE;
	}

	static RAMDTCM LIST_HEAD2 uacin96rtsfree;
	static RAMDTCM LIST_HEAD2 uacin96rtsready;	// Буферы для записи в вудиоканал USB к компьютер спектра, 2*32*192 kS/S

#endif /* WITHRTS96 */

static RAMDTCM SPINLOCK_t locklistrts = SPINLOCK_INIT;
static subscribeint32_t uacinrtssubscribe;

static RAMDTCM LIST_HEAD2 uacinfree16;
static RAMDTCM LIST_HEAD2 uacinready16;	// Буферы для записи в вудиоканал USB к компьютер 2*16*24 kS/S
static RAMDTCM SPINLOCK_t locklistuacin16 = SPINLOCK_INIT;

#endif /* WITHUSBUACIN */

#endif /* WITHINTEGRATEDDSP */

#if WITHUSEAUDIOREC

typedef ALIGNX_BEGIN struct records16
{
	ALIGNX_BEGIN int16_t buff [AUDIORECBUFFSIZE16] ALIGNX_END;
	LIST_ENTRY item;
	unsigned startdata;	// data start
	unsigned topdata;	// index after last element
} ALIGNX_END records16_t;

static RAMDTCM LIST_HEAD2 recordsfree16;		// Свободные буферы
static RAMDTCM LIST_HEAD2 recordsready16;	// Буферы для записи на SD CARD

static RAMDTCM volatile unsigned recdropped;
static RAMDTCM volatile unsigned recbuffered;

#endif /* WITHUSEAUDIOREC */

#if WITHMODEM


typedef struct modems8
{
	LIST_ENTRY item;
	size_t length;
	uint8_t buff [MODEMBUFFERSIZE8];
} modems8_t;

static RAMDTCM LIST_HEAD2 modemsfree8;		// Свободные буферы
static RAMDTCM LIST_HEAD2 modemsrx8;	// Буферы с принятымти через модем данными
//static LIST_ENTRY modemstx8;	// Буферы с данными для передачи через модем

#endif /* WITHMODEM */

/* Cообщения от уровня обработчиков прерываний к user-level функциям. */

typedef struct message
{
	void * tag2;
	LIST_ENTRY item;
	uint8_t type;
	uint8_t data [MSGBUFFERSIZE8];
	void * tag3;
} message_t;

static RAMDTCM LIST_ENTRY msgsfree8;		// Свободные буферы
static RAMDTCM LIST_ENTRY msgsready8;		// Заполненные - готовые к обработке
static RAMDTCM SPINLOCK_t locklistmsg8 = SPINLOCK_INIT;

#if WITHBUFFERSDEBUG

static volatile unsigned n1, n1wfm, n2, n3, n4, n5, n6;
static volatile unsigned e1, e2, e3, e4, e5, e6, e7, e8, purge16;
static volatile unsigned nbadd, nbdel, nbzero;

static volatile unsigned debugcount_ms10;	// с точностью 0.1 ms

static volatile unsigned debugcount_uacout;
static volatile unsigned debugcount_mikeadc;
static volatile unsigned debugcount_phonesdac;
static volatile unsigned debugcount_rtsadc;
static volatile unsigned debugcount_uacin;
static volatile unsigned debugcount_rx32adc;
static volatile unsigned debugcount_rx32wfm;
static volatile unsigned debugcount_tx32dac;
	
#endif /* WITHBUFFERSDEBUG */

static ticker_t buffticker;

/* вызывается из обработчика таймерного прерывания */
static void buffers_spool(void * ctx)
{
#if WITHBUFFERSDEBUG
	debugcount_ms10 += 10000 / TICKS_FREQUENCY;
#endif /* WITHBUFFERSDEBUG */
}

#if WITHBUFFERSDEBUG

static unsigned 
getresetval(volatile unsigned * p)
{
	unsigned v;
	global_disableIRQ();
	v = * p;
	* p = 0;
	global_enableIRQ();
	return v;
}

#endif /* WITHBUFFERSDEBUG */

void buffers_diagnostics(void)
{
#if 1 && WITHDEBUG && WITHINTEGRATEDDSP && WITHBUFFERSDEBUG

	LIST2PRINT(speexfree16);
	LIST2PRINT(speexready16);
	LIST2PRINT(voicesfree32tx);
	LIST2PRINT(voicesready32tx);
	PRINTF(PSTR("\n"));
	LIST2PRINT(voicesfree16);
	LIST3PRINT(voicesmike16);
	LIST2PRINT(voicesphones16);
	LIST2PRINT(voicesmoni16);
	PRINTF(PSTR("\n"));

	#if WITHUSBUACIN
		#if WITHRTS192
			//LIST2PRINT(voicesfree192rts);
			//LIST2PRINT(uacin192rts);
		#elif WITHRTS96
			//LIST2PRINT(uacin96rtsfree);
			//LIST2PRINT(uacin96rtsready);
		#endif
		//LIST2PRINT(uacinfree16);
		//LIST2PRINT(uacinready16);
	#endif /* WITHUSBUACIN */
	#if WITHUSBUACOUT
		LIST3PRINT(resample16);
	#endif /* WITHUSBUACOUT */

	PRINTF(PSTR(" add=%u, del=%u, zero=%u, "), nbadd, nbdel, nbzero);
		
	PRINTF(PSTR("\n"));

#endif

#if 1 && WITHDEBUG && WITHINTEGRATEDDSP && WITHBUFFERSDEBUG
	PRINTF(PSTR("n1=%u n1wfm=%u n2=%u n3=%u n4=%u n5=%u n6=%u uacinalt=%d, purge16=%u\n"), n1, n1wfm, n2, n3, n4, n5, n6, uacinalt, purge16);
	PRINTF(PSTR("e1=%u e2=%u e3=%u e4=%u e5=%u e6=%u e7=%u e8=%u\n"), e1, e2, e3, e4, e5, e6, e7, e8);

	{
		const unsigned ms10 = getresetval(& debugcount_ms10);
		const unsigned uacout = getresetval(& debugcount_uacout);
		const unsigned mikeadc = getresetval(& debugcount_mikeadc);
		const unsigned phonesdac = getresetval(& debugcount_phonesdac);
		const unsigned rtsadc = getresetval(& debugcount_rtsadc);
		const unsigned rx32adc = getresetval(& debugcount_rx32adc);
		const unsigned rx32wfm = getresetval(& debugcount_rx32wfm);
		const unsigned tx32dac = getresetval(& debugcount_tx32dac);
		const unsigned uacin = getresetval(& debugcount_uacin);

		PRINTF(PSTR("FREQ: uacout=%u, uacin=%u, mikeadc=%u, phonesdac=%u, rtsadc=%u, rx32adc=%u, rx32wfm=%u, tx32dac=%u\n"),
			uacout * 10000 / ms10, 
			uacin * 10000 / ms10, 
			mikeadc * 10000 / ms10, 
			phonesdac * 10000 / ms10, 
			rtsadc * 10000 / ms10, 
			rx32adc * 10000 / ms10, 
			rx32wfm * 10000 / ms10, 
			tx32dac * 10000 / ms10
			);
	}
#endif
}


#if WITHINTEGRATEDDSP

typedef ALIGNX_BEGIN struct denoise16
{
	LIST_ENTRY item;
	ALIGNX_BEGIN speexel_t buff [NTRX * FIRBUFSIZE] ALIGNX_END;
} ALIGNX_END denoise16_t;


// Буферы с принятымти от обработчиков прерываний сообщениями
uint_fast8_t takespeexready_user(speexel_t * * dest)
{
	global_disableIRQ();
	SPIN_LOCK(& speexlock);
	if (! IsListEmpty2(& speexready16))
	{
		const PLIST_ENTRY t = RemoveTailList2(& speexready16);
		SPIN_UNLOCK(& speexlock);
		global_enableIRQ();
		denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, item);
		* dest = p->buff;
		return 1;
	}
	SPIN_UNLOCK(& speexlock);
	global_enableIRQ();
	return 0;
}

// Освобождение обработанного буфера сообщения
void releasespeexbuffer_user(speexel_t * t)
{
	denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, buff);
	global_disableIRQ();
	SPIN_LOCK(& speexlock);

	InsertHeadList2(& speexfree16, & p->item);

	SPIN_UNLOCK(& speexlock);
	global_enableIRQ();
}


denoise16_t * allocate_dmabuffer16denoise(void)
{
	SPIN_LOCK(& speexlock);
	if (! IsListEmpty2(& speexfree16))
	{
		const PLIST_ENTRY t = RemoveTailList2(& speexfree16);
		SPIN_UNLOCK(& speexlock);
		denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, item);
		return p;
	}
#if WITHBUFFERSDEBUG
	++ e7;
#endif /* WITHBUFFERSDEBUG */
	//PRINTF(PSTR("allocate_dmabuffer16denoise() failure\n"));
	if (! IsListEmpty2(& speexready16))
	{
		const PLIST_ENTRY t = RemoveTailList2(& speexready16);
		SPIN_UNLOCK(& speexlock);
		denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, item);
		return p;
	}
	SPIN_UNLOCK(& speexlock);
	PRINTF(PSTR("allocate_dmabuffer16denoise() failure\n"));
	ASSERT(0);
	for (;;)
		;
	return 0;
}

static void
savesampleout16tospeex(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	static denoise16_t * p = NULL;
	static unsigned n;

	if (p == NULL)
	{
		p = allocate_dmabuffer16denoise();
		n = 0;
	}

	p->buff [n] = ch0;		// sample value
#if WITHUSEDUALWATCH
	p->buff [n + FIRBUFSIZE] = ch1;	// sample value
#endif /* WITHUSEDUALWATCH */

	n += 1;

	if (n >= FIRBUFSIZE)
	{
		SPIN_LOCK(& speexlock);
		InsertHeadList2(& speexready16, & p->item);
		SPIN_UNLOCK(& speexlock);
		p = NULL;
	}
}

deliverylist_t rtstargetsint;	// выход обработчика DMA приема от FPGA
deliverylist_t afoutfloat_user;	// выход sppeex и фильтра
deliverylist_t afoutfloat;	// выход приемника

#endif /* WITHINTEGRATEDDSP */

// инициализация системы буферов
void buffers_initialize(void)
{
#if WITHBUFFERSDEBUG
	ticker_initialize(& buffticker, 1, buffers_spool, NULL);
#endif /* WITHBUFFERSDEBUG */

	unsigned i;

#if WITHINTEGRATEDDSP

	deliverylist_initialize(& rtstargetsint);
	deliverylist_initialize(& afoutfloat_user);
	deliverylist_initialize(& afoutfloat);

	static subscribefloat_t afsample16reregister_user;
	subscribefloat_user(& afoutfloat_user, & afsample16reregister_user, NULL, savesampleout16stereo_user);

#if WITHSKIPUSERMODE

	static subscribefloat_t afsample16reregister;
	subscribefloat_user(& afoutfloat, & afsample16reregister, NULL, savesampleout16stereo);

#else /* WITHSKIPUSERMODE */

	static subscribefloat_t afsample16reregister;
	subscribefloat_user(& afoutfloat, & afsample16reregister, NULL, savesampleout16tospeex);

#endif /* WITHSKIPUSERMODE */

	#if WITHUSBUAC
		/* буферы требуются для ресэмплера */
		static ALIGNX_BEGIN RAM_D2 voice16_t voicesarray16 [228] ALIGNX_END;
	#else /* WITHUSBUAC */
		static ALIGNX_BEGIN RAM_D2 voice16_t voicesarray16 [96] ALIGNX_END;
	#endif /* WITHUSBUAC */

	#if WITHUSBUACOUT
		InitializeListHead3(& resample16, RESAMPLE16NORMAL);	// буферы от USB для синхронизации
	#endif /* WITHUSBUACOUT */

	InitializeListHead3(& voicesmike16, VOICESMIKE16NORMAL);	// список оцифрованных с АЦП кодека
	InitializeListHead2(& voicesphones16);	// список для выдачи на ЦАП кодека
	InitializeListHead2(& voicesmoni16);	// самоконтроль
	InitializeListHead2(& voicesfree16);	// Незаполненные
	for (i = 0; i < (sizeof voicesarray16 / sizeof voicesarray16 [0]); ++ i)
	{
		voice16_t * const p = & voicesarray16 [i];
		p->tag2 = p;
		p->tag3 = p;
		InsertHeadList2(& voicesfree16, & p->item);
	}
	SPINLOCK_INITIALIZE(& locklist16);

#if WITHUSBUACIN

	static uacin16_t uacinarray16 [24];

	InitializeListHead2(& uacinfree16);	// Незаполненные
	InitializeListHead2(& uacinready16);	// список для выдачи в канал USB AUDIO

	for (i = 0; i < (sizeof uacinarray16 / sizeof uacinarray16 [0]); ++ i)
	{
		uacin16_t * const p = & uacinarray16 [i];
		p->tag = BUFFTAG_UACIN16;
		p->tag2 = p;
		p->tag3 = p;
		InsertHeadList2(& uacinfree16, & p->item);
	}
	SPINLOCK_INITIALIZE(& locklistuacin16);

	//ASSERT((DMABUFFSIZEUACIN16 % HARDWARE_RTSDMABYTES) == 0);
	ASSERT((DMABUFFSIZE192RTS % HARDWARE_RTSDMABYTES) == 0);
	ASSERT((DMABUFFSIZE96RTS % HARDWARE_RTSDMABYTES) == 0);

	#if WITHRTS192

		RAMBIG static voice192rts_t voicesarray192rts [4];

		ASSERT(offsetof(uacin16_t, item) == offsetof(voice192rts_t, item));
		ASSERT(offsetof(uacin16_t, u.buff) == offsetof(voice192rts_t, u.buff));
		ASSERT(offsetof(uacin16_t, tag) == offsetof(voice192rts_t, tag));

		InitializeListHead2(& uacin192rts);		// список для выдачи в канал USB AUDIO - спектр
		InitializeListHead2(& voicesfree192rts);	// Незаполненные
		for (i = 0; i < (sizeof voicesarray192rts / sizeof voicesarray192rts [0]); ++ i)
		{
			voice192rts_t * const p = & voicesarray192rts [i];
			p->tag = BUFFTAG_RTS192;
			p->tag2 = p;
			p->tag3 = p;
			InsertHeadList2(& voicesfree192rts, & p->item);
		}
		subscribeint(& rtstargetsint, & uacinrtssubscribe, NULL, savesampleout192stereo);

	#endif /* WITHRTS192 */

	#if WITHRTS96

		static RAMBIG ALIGNX_BEGIN voice96rts_t voicesarray96rts [4] ALIGNX_END;

		ASSERT(offsetof(uacin16_t, item) == offsetof(voice96rts_t, item));
		ASSERT(offsetof(uacin16_t, u.buff) == offsetof(voice96rts_t, u.buff));
		ASSERT(offsetof(uacin16_t, tag) == offsetof(voice96rts_t, tag));

		InitializeListHead2(& uacin96rtsready);		// список для выдачи в канал USB AUDIO - спектр
		InitializeListHead2(& uacin96rtsfree);	// Незаполненные
		for (i = 0; i < (sizeof voicesarray96rts / sizeof voicesarray96rts [0]); ++ i)
		{
			voice96rts_t * const p = & voicesarray96rts [i];
			p->tag = BUFFTAG_RTS96;
			p->tag2 = p;
			p->tag3 = p;
			//PRINTF("Add p=%p, tag=%d, tag2=%p, tag3=%p\n", p, p->tag, p->tag2, p->tag3);
			InsertHeadList2(& uacin96rtsfree, & p->item);
		}
		subscribeint(& rtstargetsint, & uacinrtssubscribe, NULL, savesampleout96stereo);

	#endif /* WITHRTS192 */
	SPINLOCK_INITIALIZE(& locklistrts);

#endif /* WITHUSBUACIN */

	static ALIGNX_BEGIN RAM_D2 voice32tx_t voicesarray32tx [6] ALIGNX_END;

	InitializeListHead2(& voicesready32tx);	// список для выдачи на ЦАП
	InitializeListHead2(& voicesfree32tx);	// Незаполненные
	for (i = 0; i < (sizeof voicesarray32tx / sizeof voicesarray32tx [0]); ++ i)
	{
		voice32tx_t * const p = & voicesarray32tx [i];
		p->tag2 = p;
		p->tag3 = p;
		InsertHeadList2(& voicesfree32tx, & p->item);
	}
	SPINLOCK_INITIALIZE(& locklist32tx);

    static ALIGN1K_BEGIN RAM_D2 voice32rx_t voicesarray32rx [6] ALIGN1K_END;	// без WFM надо 2

	InitializeListHead2(& voicesfree32rx);	// Незаполненные
	for (i = 0; i < (sizeof voicesarray32rx / sizeof voicesarray32rx [0]); ++ i)
	{
		voice32rx_t * const p = & voicesarray32rx [i];
		InsertHeadList2(& voicesfree32rx, & p->item);
	}
	SPINLOCK_INITIALIZE(& locklist32rx);

#if WITHUSEAUDIOREC

	#if CPUSTYLE_R7S721
		RAMNOINIT_D1 static records16_t recordsarray16 [8];
	#elif defined (STM32F767xx)
		RAMNOINIT_D1 static records16_t recordsarray16 [8];
	#elif defined (STM32F746xx)
		RAMNOINIT_D1 static records16_t recordsarray16 [8];
	#elif defined (STM32F429xx)
		RAMNOINIT_D1 static records16_t recordsarray16 [8];
	#elif defined (STM32H743xx)
		RAM_D2 static records16_t recordsarray16 [6];
	#else
		RAMNOINIT_D1 static records16_t recordsarray16 [8];
	#endif

	/* Подготовка буферов для записи на SD CARD */
	InitializeListHead2(& recordsready16);	// Заполненные - готовые для записи на SD CARD
	InitializeListHead2(& recordsfree16);	// Незаполненные
	for (i = 0; i < (sizeof recordsarray16 / sizeof recordsarray16 [0]); ++ i)
	{
		records16_t * const p = & recordsarray16 [i];
		InsertHeadList2(& recordsfree16, & p->item);
	}

#endif /* WITHUSEAUDIOREC */

#if WITHMODEM
	static modems8_t modemsarray8 [8];

	/* Подготовка буферов для обмена с модемом */
	InitializeListHead2(& modemsrx8);	// Заполненные - принятые с модема
	//InitializeListHead2(& modemstx8);	// Заполненные - готовые для передачи через модем
	InitializeListHead2(& modemsfree8);	// Незаполненные
	for (i = 0; i < (sizeof modemsarray8 / sizeof modemsarray8 [0]); ++ i)
	{
		modems8_t * const p = & modemsarray8 [i];
		//InitializeListHead2(& p->item);
		InsertHeadList2(& modemsfree8, & p->item);
	}
#endif /* WITHMODEM */

	static RAMDTCM denoise16_t speexarray16 [8];

	InitializeListHead2(& speexfree16);	// Незаполненные
	InitializeListHead2(& speexready16);	// Для обработки

	for (i = 0; i < (sizeof speexarray16 / sizeof speexarray16 [0]); ++ i)
	{
		denoise16_t * const p = & speexarray16 [i];
		InsertHeadList2(& speexfree16, & p->item);
	}
	SPINLOCK_INITIALIZE(& speexlock);

#endif /* WITHINTEGRATEDDSP */

	/* Cообщения от уровня обработчиков прерываний к user-level функциям. */
	static message_t messagesarray8 [12];

	/* Подготовка буферов для обмена с модемом */
	InitializeListHead(& msgsready8);	// Заполненные - готовые к обработке
	InitializeListHead(& msgsfree8);	// Незаполненные
	for (i = 0; i < (sizeof messagesarray8 / sizeof messagesarray8 [0]); ++ i)
	{
		message_t * const p = & messagesarray8 [i];
		p->tag2 = p;
		p->tag3 = p;
		//InitializeListHead2(& p->item);
		InsertHeadList(& msgsfree8, & p->item);
	}
	SPINLOCK_INITIALIZE(& locklistmsg8);
}

/* Cообщения от уровня обработчиков прерываний к user-level функциям. */

// Буферы с принятымти от обработчиков прерываний сообщениями
uint_fast8_t takemsgready_user(uint8_t * * dest)
{
	system_disableIRQ();

	SPIN_LOCK(& locklistmsg8);
	if (! IsListEmpty(& msgsready8))
	{
		PLIST_ENTRY t = RemoveTailList(& msgsready8);
		SPIN_UNLOCK(& locklistmsg8);
		system_enableIRQ();
		message_t * const p = CONTAINING_RECORD(t, message_t, item);
		* dest = p->data;
		ASSERT(p->type != MSGT_EMPTY);
		return p->type;
	}
	SPIN_UNLOCK(& locklistmsg8);
	system_enableIRQ();
	return MSGT_EMPTY;
}


// Освобождение обработанного буфера сообщения
void releasemsgbuffer_user(uint8_t * dest)
{
	message_t * const p = CONTAINING_RECORD(dest, message_t, data);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	system_disableIRQ();
	SPIN_LOCK(& locklistmsg8);
	InsertHeadList(& msgsfree8, & p->item);
	SPIN_UNLOCK(& locklistmsg8);
	system_enableIRQ();
}

// Буфер для формирования сообщения
size_t takemsgbufferfree_low(uint8_t * * dest)
{
	SPIN_LOCK(& locklistmsg8);
	if (! IsListEmpty(& msgsfree8))
	{
		PLIST_ENTRY t = RemoveTailList(& msgsfree8);
		SPIN_UNLOCK(& locklistmsg8);
		message_t * const p = CONTAINING_RECORD(t, message_t, item);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		* dest = p->data;
		return (MSGBUFFERSIZE8 * sizeof p->data [0]);
	}
	SPIN_UNLOCK(& locklistmsg8);
	return 0;
}

// поместить сообщение в очередь к исполнению
void placesemsgbuffer_low(uint_fast8_t type, uint8_t * dest)
{
	ASSERT(type != MSGT_EMPTY);
	message_t * p = CONTAINING_RECORD(dest, message_t, data);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	p->type = type;
	SPIN_LOCK(& locklistmsg8);
	InsertHeadList(& msgsready8, & p->item);
	SPIN_UNLOCK(& locklistmsg8);
}

#if WITHINTEGRATEDDSP

// Оставить в указанной очереди не более PHONESLEVEL буферов
static void buffers_purge16(LIST_HEAD2 * list)
{
	return;
	if (GetCountList2(list) > PHONESLEVEL * 3)
	{
		do
		{
			const PLIST_ENTRY t = RemoveTailList2(list);
			InsertHeadList2(& voicesfree16, t);
	#if WITHBUFFERSDEBUG
		++ purge16;
	#endif /* WITHBUFFERSDEBUG */
		}
		while (GetCountList2(list) > PHONESLEVEL);
	}
}

// Сохранить звук на звуковой выход трансивера
static RAMFUNC void buffers_tophones16(voice16_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	SPIN_LOCK(& locklist16);
	buffers_purge16(& voicesphones16);
	buffers_purge16(& voicesmoni16);
	InsertHeadList2(& voicesphones16, & p->item);
	SPIN_UNLOCK(& locklist16);
}

// Сохранить звук для самоконтроля на звуковой выход трансивера
static RAMFUNC void buffers_tomoni16(voice16_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	SPIN_LOCK(& locklist16);
	buffers_purge16(& voicesphones16);
	buffers_purge16(& voicesmoni16);
	InsertHeadList2(& voicesmoni16, & p->item);
	SPIN_UNLOCK(& locklist16);
}

// Сохранить звук в никуда...
static RAMFUNC void buffers_tonull16(voice16_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	SPIN_LOCK(& locklist16);
	InsertHeadList2(& voicesfree16, & p->item);
	SPIN_UNLOCK(& locklist16);
}

// Сохранить звук на вход передатчика
static RAMFUNC void buffers_tomodulators16(voice16_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	SPIN_LOCK(& locklist16);
	InsertHeadList3(& voicesmike16, & p->item, 0);
	SPIN_UNLOCK(& locklist16);
}

// Сохранить звук от АЦП пикрофона
static RAMFUNC void buffers_savefrommikeadc(voice16_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_mikeadc += sizeof p->buff / sizeof p->buff [0] / DMABUFSTEP16;	// в буфере пары сэмплов по два байта
#endif /* WITHBUFFERSDEBUG */

	if (uacoutmike == 0)
		buffers_tomodulators16(p);
	else
		buffers_tonull16(p);

}

// Сохранить звук после получения из него информации для модулятора
static RAMFUNC void buffers_aftermodulators(voice16_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	// если поток используется и как источник аудиоинформации для модулятора и для динамиков,
	// в динамики будет направлен после модулятора

	if (uacoutplayer && uacoutmike)
		buffers_tophones16(p);
	else
		buffers_tonull16(p);
}

// +++ Коммутация потоков аудиоданных
// первый канал выхода приёмника - для прослушивания
static RAMFUNC void
buffers_savefromrxout(voice16_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	if (uacoutplayer != 0)
		buffers_tonull16(p);
	else
		buffers_tophones16(p);
}


static RAMFUNC void
buffers_savefrommoni(voice16_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	buffers_tomoni16(p);
}


#if WITHUSBUAC
// приняли данные от синхронизатора
static RAMFUNC void
buffers_savefromresampling(voice16_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	// если поток используется и как источник аудиоинформации для модулятора и для динамиков,
	// в динамики будет направлен после модулятора

	if (uacoutmike != 0)
		buffers_tomodulators16(p);
	else if (uacoutplayer != 0)
		buffers_tophones16(p);
	else
		buffers_tonull16(p);
}

#endif /* WITHUSBUAC */

// Сохранить звук от несинхронного источника - USB - для последующего ресэмплинга
RAMFUNC static void buffers_savetoresampling16(voice16_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	SPIN_LOCK(& locklist16);
	// Помеестить в очередь принятых с USB UAC
	InsertHeadList3(& resample16, & p->item, 0);

	if (GetCountList3(& resample16) > (RESAMPLE16NORMAL * 2))
	{
		// Из-за ошибок с асинхронным аудио пришлось добавить ограничение на размер этой очереди
		const PLIST_ENTRY t = RemoveTailList3(& resample16);
		InsertHeadList2(& voicesfree16, t);

	#if WITHBUFFERSDEBUG
		++ e6;
	#endif /* WITHBUFFERSDEBUG */
	}
	SPIN_UNLOCK(& locklist16);
}

// приняли данные от USB AUDIO
static RAMFUNC void
buffers_savefromuacout(voice16_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_uacout += sizeof p->buff / sizeof p->buff [0] / DMABUFSTEP16;	// в буфере пары сэмплов по два байта
#endif /* WITHBUFFERSDEBUG */

#if WITHUSBUAC

	if (uacoutplayer || uacoutmike)
		buffers_savetoresampling16(p);
	else
		buffers_tonull16(p);
#else /* WITHUSBUAC */
	buffers_tonull16(p);
#endif /* WITHUSBUAC */
}

//////////////////////////////////////////
// Поэлементное чтение буфера AF ADC

// 16 bit, signed
// в паре значений, возвращаемых данной функцией, vi получает значение от микрофона. vq зарезервированно для работы ISB (две независимых боковых)
// При отсутствии данных в очереди - возвращаем 0
// TODO: переделаь на denoise16_t
RAMFUNC uint_fast8_t getsampmlemike(FLOAT32P_t * v)
{
	enum { L, R };
	static voice16_t * RAMDTCM p = NULL;
	static RAMDTCM unsigned pos = 0;	// позиция по выходному количеству

	if (p == NULL)
	{
		SPIN_LOCK(& locklist16);
		if (GetReadyList3(& voicesmike16))
		{
			PLIST_ENTRY t = RemoveTailList3(& voicesmike16);
			SPIN_UNLOCK(& locklist16);
			p = CONTAINING_RECORD(t, voice16_t, item);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			pos = 0;
		}
		else
		{
			// Микрофонный кодек ещё не успел начать работать - возвращаем 0.
			SPIN_UNLOCK(& locklist16);
			return 0;
		}
	}
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	// Использование данных.
	v->ivqv [L] = AUBTOAUDIO16(p->buff [pos * DMABUFSTEP16 + L]);	// микрофон или левый канал
	v->ivqv [R] = AUBTOAUDIO16(p->buff [pos * DMABUFSTEP16 + R]);	// правый канал

	if (++ pos >= CNT16)
	{
		buffers_aftermodulators(p);
		p = NULL;
	}
	return 1;	
}

// 16 bit, signed
RAMFUNC uint_fast8_t getsampmlemoni(FLOAT32P_t * v)
{
	enum { L, R };
	static voice16_t * RAMDTCM p = NULL;
	static RAMDTCM unsigned pos = 0;	// позиция по выходному количеству

	if (p == NULL)
	{
		SPIN_LOCK(& locklist16);
		if (! IsListEmpty2(& voicesmoni16))
		{
			PLIST_ENTRY t = RemoveTailList2(& voicesmoni16);
			SPIN_UNLOCK(& locklist16);
			p = CONTAINING_RECORD(t, voice16_t, item);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			pos = 0;
		}
		else
		{
			// Микрофонный кодек ещё не успел начать работать - возвращаем 0.
			SPIN_UNLOCK(& locklist16);
			return 0;
		}
	}

	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	// Использование данных.
	v->ivqv [L] = AUBTOAUDIO16(p->buff [pos * DMABUFSTEP16 + L]);	// микрофон или левый канал
	v->ivqv [R] = AUBTOAUDIO16(p->buff [pos * DMABUFSTEP16 + R]);	// правый канал

	if (++ pos >= CNT16)
	{
		buffers_tonull16(p);
		p = NULL;
	}
	return 1;
}

// звук для самоконтроля
void savemoni16stereo(FLOAT_t ch0, FLOAT_t ch1)
{
	enum { L, R };
	// если есть инициализированный канал для выдачи звука
	static voice16_t * RAMDTCM p = NULL;
	static RAMDTCM unsigned n;

	if (p == NULL)
	{
		uintptr_t addr = allocate_dmabuffer16();
		p = CONTAINING_RECORD(addr, voice16_t, buff);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		n = 0;
	}

	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	p->buff [n * DMABUFSTEP16 + L] = AUDIO16TOAUB(ch0);		// sample value
#if DMABUFSTEP16 > 1
	p->buff [n * DMABUFSTEP16 + R] = AUDIO16TOAUB(ch1);	// sample value
#endif

	if (++ n >= CNT16)
	{
		buffers_savefrommoni(p);
		p = NULL;
	}
}



#if WITHUSBUAC

static uint_fast8_t isaudio48(void)
{
#if WITHUSBHW && WITHUSBUAC
	return UACINALT_AUDIO48 == uacinalt;
#else /* WITHUSBHW && WITHUSBUAC */
	return 0;
#endif /* WITHUSBHW && WITHUSBUAC */
}

// UAC OUT samplerate
int_fast32_t dsp_get_samplerateuacout(void)
{
	return dsp_get_sampleratetx();
}

#if WITHRTS192 && WITHUSBHW && WITHUSBUAC

static uint_fast8_t isrts192(void)
{
#if WITHUSBHW && WITHUSBUAC
	#if WITHUSBUACIN2 && WITHRTS192
		return UACINRTSALT_RTS192 == uacinrtsalt;
	#elif WITHRTS192
		return UACINALT_RTS192 == uacinalt;
	#else /* WITHUSBUACIN2 */
		return 0;
	#endif /* WITHUSBUACIN2 */
#else /* WITHUSBHW && WITHUSBUAC */
	return 0;
#endif /* WITHUSBHW && WITHUSBUAC */
}

// Сохранить буфер сэмплов для передачи в компьютер
static RAMFUNC void
buffers_savetouacin192rts(voice192rts_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_rtsadc += sizeof p->buff / sizeof p->buff [0] / DMABUFSTEP192RTS;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */

	SPIN_LOCK(& locklistrts);
	InsertHeadList2(& uacin192rts, & p->item);
	SPIN_UNLOCK(& locklistrts);

	refreshDMA_uacin();		// если DMA  остановлено - начать обмен
}

static void buffers_savetonull192rts(voice192rts_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	SPIN_LOCK(& locklistrts);
	InsertHeadList2(& voicesfree192rts, & p->item);
	SPIN_UNLOCK(& locklistrts);
}


#endif /* WITHRTS192 && WITHUSBHW && WITHUSBUAC */

#if WITHRTS96 && WITHUSBHW && WITHUSBUAC

static uint_fast8_t isrts96(void)
{
#if WITHUSBHW && WITHUSBUAC
	#if WITHUSBUACIN2 && WITHRTS96
		return uacinrtsalt == UACINRTSALT_RTS96;
	#elif WITHRTS96
		return uacinalt == UACINALT_RTS96;
	#else /* WITHUSBUACIN2 */
		return 0;
	#endif /* WITHUSBUACIN2 */
#else /* WITHUSBHW && WITHUSBUAC */
	return 0;
#endif /* WITHUSBHW && WITHUSBUAC */
}

// Сохранить буфер сэмплов для передачи в компьютер
static RAMFUNC void
buffers_savetouacin96rts(voice96rts_t * p)
{
	ASSERT(p->tag == BUFFTAG_RTS96);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_rtsadc += sizeof p->u.buff / sizeof p->u.buff [0] / DMABUFSTEP96RTS;	// в буфере пары сэмплов по три байта
#endif /* WITHBUFFERSDEBUG */
	
	SPIN_LOCK(& locklistrts);
	InsertHeadList2(& uacin96rtsready, & p->item);
	SPIN_UNLOCK(& locklistrts);

	refreshDMA_uacin();		// если DMA  остановлено - начать обмен
}

static void buffers_savetonull96rts(voice96rts_t * p)
{
	ASSERT(p->tag == BUFFTAG_RTS96);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	SPIN_LOCK(& locklistrts);
	InsertHeadList2(& uacin96rtsfree, & p->item);
	SPIN_UNLOCK(& locklistrts);
}

#else

static uint_fast8_t isrts96(void)
{
	return 0;
}

#endif /* WITHRTS96 && WITHUSBHW && WITHUSBUAC */

// Сохранить буфер сэмплов для передачи в компьютер
static RAMFUNC void
buffers_savetouacin(uacin16_t * p)
{
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_uacin += sizeof p->u.buff / sizeof p->u.buff [0] / DMABUFSTEPUACIN16;	// в буфере пары сэмплов по три байта
#endif /* WITHBUFFERSDEBUG */
	SPIN_LOCK(& locklistuacin16);
	InsertHeadList2(& uacinready16, & p->item);
	SPIN_UNLOCK(& locklistuacin16);

	refreshDMA_uacin();		// если DMA  остановлено - начать обмен
}

#endif /* WITHUSBUAC */

#if WITHUSBUAC

static RAMFUNC unsigned long ulmin(
	unsigned long a,
	unsigned long b
	)
{
	return a < b ? a : b;
}

// возвращает количество полученых сэмплов
static RAMFUNC unsigned getsamplemsuacout(
	aubufv_t * buff,	// текущая позиция в целевом буфере
	unsigned size		// количество оставшихся одиночных сэмплов
	)
{
	static voice16_t * RAMDTCM p = NULL;
	enum { NPARTS = 3 };
	static RAMDTCM uint_fast8_t part = 0;
	static aubufv_t * RAMDTCM datas [NPARTS] = { NULL, NULL };		// начальный адрес пары сэмплов во входном буфере
	static RAMDTCM unsigned sizes [NPARTS] = { 0, 0 };			// количество сэмплов во входном буфере

	static unsigned skipsense = SKIPPED;

	SPIN_LOCK(& locklist16);
	if (p == NULL)
	{
		if (GetReadyList3(& resample16) == 0)
		{
#if WITHBUFFERSDEBUG
			++ nbzero;
#endif /* WITHBUFFERSDEBUG */
			// Микрофонный кодек ещё не успел начать работать - возвращаем 0.
			SPIN_UNLOCK(& locklist16);
			memset(buff, 0x00, size * sizeof (* buff));	// тишина
			return size;	// ноль нельзя возвращать - зацикливается проуелура ресэмплинга
		}
		else
		{
			PLIST_ENTRY t = RemoveTailList3(& resample16);
			p = CONTAINING_RECORD(t, voice16_t, item);
			if (GetReadyList3(& resample16) == 0)
				skipsense = SKIPPED;
			const uint_fast8_t valid = GetReadyList3(& resample16) && skipsense == 0;
			skipsense = (skipsense == 0) ? SKIPPED : skipsense - 1;

			const unsigned LOW = RESAMPLE16NORMAL - (SKIPPED * 1);
			const unsigned HIGH = RESAMPLE16NORMAL + (SKIPPED * 1);

			if (valid && GetCountList3(& resample16) <= LOW)
			{
				SPIN_UNLOCK(& locklist16);
				// добавляется один сэмпл к выходному потоку раз в SKIPPED блоков
#if WITHBUFFERSDEBUG
				++ nbadd;
#endif /* WITHBUFFERSDEBUG */

#if 0
				part = NPARTS - 2;
				datas [part + 0] = & p->buff [0];	// дублируем первый сэмпл
				sizes [part + 0] = DMABUFSTEP16;
				datas [part + 1] = & p->buff [0];
				sizes [part + 1] = DMABUFFSIZE16;
#else
				static aubufv_t addsample [DMABUFSTEP16];
				enum { HALF = DMABUFFSIZE16 / 2 };
				// значения как среднее арифметическое сэмплов, между которыми вставляем дополнительный.
				addsample [0] = ((aufastbufv2x_t) p->buff [HALF - DMABUFSTEP16 + 0] + p->buff [HALF + 0]) / 2;	// Left
		#if DMABUFSTEP16 > 1
				addsample [1] = ((aufastbufv2x_t) p->buff [HALF - DMABUFSTEP16 + 1] + p->buff [HALF + 1]) / 2;	// Right
		#endif
				part = NPARTS - 3;
				datas [0] = & p->buff [0];		// часть перед вставкой
				sizes [0] = HALF;
				datas [1] = & addsample [0];	// вставляемые данные
				sizes [1] = DMABUFSTEP16;
				datas [2] = & p->buff [HALF];	// часть после вставки
				sizes [2] = DMABUFFSIZE16 - HALF;
#endif
			}
			else if (valid && GetCountList3(& resample16) >= HIGH)
			{
				SPIN_UNLOCK(& locklist16);
#if WITHBUFFERSDEBUG
				++ nbdel;
#endif /* WITHBUFFERSDEBUG */
				// убирается один сэмпл из выходного потока раз в SKIPPED блоков
				part = NPARTS - 1;
				datas [part] = & p->buff [DMABUFSTEP16];	// пропускаем первый сэмпл
				sizes [part] = DMABUFFSIZE16 - DMABUFSTEP16;
			}
			else
			{
				SPIN_UNLOCK(& locklist16);
				// Ресэмплинг не требуется или нет запаса входных данных
				part = NPARTS - 1;
				datas [part] = & p->buff [0];
				sizes [part] = DMABUFFSIZE16;
			}
		}
	}
	else
	{
		SPIN_UNLOCK(& locklist16);
	}

	const unsigned chunk = ulmin(sizes [part], size);

	memcpy(buff, datas [part], chunk * sizeof (* buff));

	datas [part] += chunk;
	if ((sizes [part] -= chunk) == 0 && ++ part >= NPARTS)
	{
		buffers_tonull16(p);
		p = NULL;
	}
	return chunk;
}

// формирование одного буфера синхронного потока из N несинхронного
static RAMFUNC void buffers_resample(void)
{
	const uintptr_t addr = allocate_dmabuffer16();	// выходной буфер
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	//
	// выполнение ресэмплинга
	unsigned pos;
	for (pos = 0; pos < DMABUFFSIZE16; )
	{
		pos += getsamplemsuacout(& p->buff [pos], DMABUFFSIZE16 - pos);
	}

	// направление получившегося буфера получателю.
	buffers_savefromresampling(p);
}

#endif /* WITHUSBUAC */
// --- Коммутация потоков аудиоданных

#if WITHUSEAUDIOREC
// Поэлементное заполнение буфера SD CARD

unsigned long hamradio_get_recdropped(void)
{
	return recdropped;
}

int hamradio_get_recdbuffered(void)
{
	return recbuffered;
}

/* to SD CARD */
// 16 bit, signed
void RAMFUNC savesamplerecord16SD(int_fast16_t left, int_fast16_t right)
{
	// если есть инициализированный канал для выдачи звука
	static records16_t * preparerecord16 = NULL;
	static unsigned level16record;

	if (preparerecord16 == NULL)
	{
		if (! IsListEmpty2(& recordsfree16))
		{
			PLIST_ENTRY t = RemoveTailList2(& recordsfree16);
			preparerecord16 = CONTAINING_RECORD(t, records16_t, item);
		}
		else
		{
			-- recbuffered;
			++ recdropped;
			// Если нет свободных - использум самый давно подготовленный для записи буфер
			PLIST_ENTRY t = RemoveTailList2(& recordsready16);
			preparerecord16 = CONTAINING_RECORD(t, records16_t, item);
		}
		level16record = 0;
		
		// Подготовка к записи файла WAV со множеством DATA CHUNK, но получившийся файл
		// нормально читает только ADOBE AUDITION, Windows Media Player 12 проигрывает только один - первый.
		// Windows Media Player Classic (https://github.com/mpc-hc/mpc-hc) вообще не проигрывает этот файл.
		
		//preparerecord16->buff [0] = 'd' | 'a' * 256;
		//preparerecord16->buff [1] = 't' | 'a' * 256;
		//preparerecord16->buff [2] = ((AUDIORECBUFFSIZE16 * sizeof preparerecord16->buff [0]) - 8) >> 0;
		//preparerecord16->buff [3] = ((AUDIORECBUFFSIZE16 * sizeof preparerecord16->buff [0]) - 8) >> 16;
		//level16record = 4;

	}

#if WITHUSEAUDIOREC2CH
	// Запись звука на SD CARD в стерео
	preparerecord16->buff [level16record ++] = left;	// sample value
	preparerecord16->buff [level16record ++] = right;	// sample value

#else /* WITHUSEAUDIOREC2CH */
	// Запись звука на SD CARD в моно
	preparerecord16->buff [level16record ++] = left;	// sample value

#endif /* WITHUSEAUDIOREC2CH */

	if (level16record >= AUDIORECBUFFSIZE16)
	{
		++ recbuffered;
		/* используется буфер целиклом */
		preparerecord16->startdata = 0;
		preparerecord16->topdata = AUDIORECBUFFSIZE16;
		InsertHeadList2(& recordsready16, & preparerecord16->item);
		preparerecord16 = NULL;
	}
}

// user-mode function
unsigned takerecordbuffer(void * * dest)
{
	global_disableIRQ();
	if (! IsListEmpty2(& recordsready16))
	{
		PLIST_ENTRY t = RemoveTailList2(& recordsready16);
		global_enableIRQ();
		-- recbuffered;
		records16_t * const p = CONTAINING_RECORD(t, records16_t, item);
		* dest = p->buff;
		return (AUDIORECBUFFSIZE16 * sizeof p->buff [0]);
	}
	global_enableIRQ();
	return 0;
}

// user-mode function
unsigned takefreerecordbuffer(void * * dest)
{
	global_disableIRQ();
	if (! IsListEmpty2(& recordsfree16))
	{
		PLIST_ENTRY t = RemoveTailList2(& recordsfree16);
		global_enableIRQ();
		-- recbuffered;
		records16_t * const p = CONTAINING_RECORD(t, records16_t, item);
		* dest = p->buff;
		return (AUDIORECBUFFSIZE16 * sizeof p->buff [0]);
	}
	global_enableIRQ();
	return 0;
}

// user-mode function
void saveplaybuffer(void * dest, unsigned used)
{
	records16_t * const p = CONTAINING_RECORD(dest, records16_t, buff);
	p->startdata = 0;	// перыфй сэмпл в буфере
	p->topdata = used / sizeof p->buff [0];	// количество сэмплов
	global_disableIRQ();
	InsertHeadList2(& recordsready16, & p->item);
	global_enableIRQ();
}

/* data to play */
unsigned savesamplesplay_user(
	const void * buff,
	unsigned length
	)
{
	void * p;
	unsigned size;

	size = takefreerecordbuffer(& p);

	if (size == 0)
	{
		//PRINTF("savesamplesplay_user: length=%u - no memory\n", length);
		return 0;
	}

	//PRINTF("savesamplesplay_user: length=%u\n", length);
	unsigned chunk = ulmin(size, length);
	memcpy(p, buff, chunk);
	saveplaybuffer(p, chunk);
	return chunk;
}

// user-mode function
void releaserecordbuffer(void * dest)
{
	records16_t * const p = CONTAINING_RECORD(dest, records16_t, buff);
	global_disableIRQ();
	InsertHeadList2(& recordsfree16, & p->item);
	global_enableIRQ();
}

/* Получение пары (левый и правый) сжмплов для воспроизведения через аудиовыход трансивера
 * или для переачи
 * Возврат 0, если нет ничего для воспроизведения.
 */
uint_fast8_t takewavsample(FLOAT32P_t * rv, uint_fast8_t suspend)
{
	static records16_t * p = NULL;
	static unsigned n;
	if (p == NULL)
	{
		if (! IsListEmpty2(& recordsready16))
		{
			PLIST_ENTRY t = RemoveTailList2(& recordsready16);
			-- recbuffered;
			p = CONTAINING_RECORD(t, records16_t, item);
			n = p->startdata;	// reset samples count
			//PRINTF("takewavsample: startdata=%u, topdata=%u\n", p->startdata, p->topdata);
		}
		else
		{
			// Нет данных для воспроизведения
			return 0;
		}
	}
	int_fast16_t sample = p->buff [n];
	rv->IV = sample;
	rv->QV = sample;

	if (++ n >= AUDIORECBUFFSIZE16 || n >= p->topdata)
	{
		// Last sample used
		InsertHeadList2(& recordsfree16, & p->item);
		p = NULL;
		//PRINTF("Release record buffer\n");
	}
	return 1;	// Сэмпл считан
}

#endif /* WITHUSEAUDIOREC */

#if WITHMODEM


// Буферы с принятымти через модем данными
size_t takemodemrxbuffer(uint8_t * * dest)
{
	global_disableIRQ();
	if (! IsListEmpty2(& modemsrx8))
	{
		PLIST_ENTRY t = RemoveTailList2(& modemsrx8);
		global_enableIRQ();
		modems8_t * const p = CONTAINING_RECORD(t, modems8_t, item);
		* dest = p->buff;
		return p->length;
	}
	global_enableIRQ();
	* dest = NULL;
	return 0;
}

// Буферы для заполнения данными
size_t takemodembuffer(uint8_t * * dest)
{
	global_disableIRQ();
	if (! IsListEmpty2(& modemsfree8))
	{
		PLIST_ENTRY t = RemoveTailList2(& modemsfree8);
		global_enableIRQ();
		modems8_t * const p = CONTAINING_RECORD(t, modems8_t, item);
		* dest = p->buff;
		return (MODEMBUFFERSIZE8 * sizeof p->buff [0]);
	}
	global_enableIRQ();
	* dest = NULL;
	return 0;
}

// Буферы для заполнения данными
// вызывается из real-time обработчика прерывания
size_t takemodembuffer_low(uint8_t * * dest)
{
	if (! IsListEmpty2(& modemsfree8))
	{
		PLIST_ENTRY t = RemoveTailList2(& modemsfree8);
		modems8_t * const p = CONTAINING_RECORD(t, modems8_t, item);
		* dest = p->buff;
		return (MODEMBUFFERSIZE8 * sizeof p->buff [0]);
	}
	* dest = NULL;
	return 0;
}

// Готов буфер с принятыми данными
// вызывается из real-time обработчика прерывания
void savemodemrxbuffer_low(uint8_t * dest, size_t length)
{
	modems8_t * const p = CONTAINING_RECORD(dest, modems8_t, buff);
	p->length = length;
	InsertHeadList2(& modemsrx8, & p->item);
}

void releasemodembuffer(uint8_t * dest)
{
	modems8_t * const p = CONTAINING_RECORD(dest, modems8_t, buff);
	global_disableIRQ();
	InsertHeadList2(& modemsfree8, & p->item);
	global_enableIRQ();
}

// вызывается из real-time обработчика прерывания
void releasemodembuffer_low(uint8_t * dest)
{
	modems8_t * const p = CONTAINING_RECORD(dest, modems8_t, buff);
	InsertHeadList2(& modemsfree8, & p->item);
}

#endif /* WITHMODEM */

// Этой функцией пользуются обработчики прерываний DMA на передачу данных по SAI
RAMFUNC uintptr_t allocate_dmabuffer32tx(void)
{
	SPIN_LOCK(& locklist32tx);
	if (! IsListEmpty2(& voicesfree32tx))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesfree32tx);
		voice32tx_t * const p = CONTAINING_RECORD(t, voice32tx_t, item);
		SPIN_UNLOCK(& locklist32tx);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		return (uintptr_t) & p->buff;
	}
	else if (! IsListEmpty2(& voicesready32tx))
	{
		// Ошибочная ситуация - если буферы не освобождены вовремя -
		// берём из очереди готовых к передаче

		uint_fast8_t n = 3;
		do
		{
			const PLIST_ENTRY t = RemoveTailList2(& voicesready32tx);
			InsertHeadList2(& voicesfree32tx, t);
		}
		while (-- n && ! IsListEmpty2(& voicesready32tx));

		PLIST_ENTRY t = RemoveTailList2(& voicesfree32tx);
		voice32tx_t * const p = CONTAINING_RECORD(t, voice32tx_t, item);
		SPIN_UNLOCK(& locklist32tx);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
#if WITHBUFFERSDEBUG
		++ e8;
#endif /* WITHBUFFERSDEBUG */
		return (uintptr_t) & p->buff;
	}
	SPIN_UNLOCK(& locklist32tx);
	/* error path */
	PRINTF(PSTR("allocate_dmabuffer32tx() failure\n"));
	for (;;)
		;
}

// Этой функцией пользуются обработчики прерываний DMA на приём данных по SAI
RAMFUNC uintptr_t allocate_dmabuffer32rx(void)
{
	SPIN_LOCK(& locklist32rx);
	if (! IsListEmpty2(& voicesfree32rx))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesfree32rx);
		voice32rx_t * const p = CONTAINING_RECORD(t, voice32rx_t, item);
		SPIN_UNLOCK(& locklist32rx);
		return (uintptr_t) & p->buff;
	}
	else
	{
		SPIN_UNLOCK(& locklist32rx);
		PRINTF(PSTR("allocate_dmabuffer32rx() failure\n"));
		for (;;)
			;
	}
}

// Этой функцией пользуются обработчики прерываний DMA на передачу и приём данных по I2S и USB AUDIO
RAMFUNC uintptr_t allocate_dmabuffer16(void)
{
	SPIN_LOCK(& locklist16);
	if (! IsListEmpty2(& voicesfree16))
	{
		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16);
		SPIN_UNLOCK(& locklist16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		return (uintptr_t) & p->buff;
	}
#if WITHUSBUAC
	else if (! IsListEmpty3(& resample16))
	{
		// Ошибочная ситуация - если буферы не освобождены вовремя -
		// берём из очереди готовых к ресэмплингу
		uint_fast8_t n = 3;
		do
		{
			const PLIST_ENTRY t = RemoveTailList3(& resample16);
			voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			InsertHeadList2(& voicesfree16, t);
		}
		while (-- n && ! IsListEmpty3(& resample16));

		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16);
		SPIN_UNLOCK(& locklist16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
	#if WITHBUFFERSDEBUG
		++ e3;
	#endif /* WITHBUFFERSDEBUG */
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		return (uintptr_t) & p->buff;
	}
#endif /* WITHUSBUAC */
	else if (! IsListEmpty2(& voicesphones16))
	{
		// Ошибочная ситуация - если буферы не освобождены вовремя -
		// берём из очереди готовых к прослушиванию

		uint_fast8_t n = 3;
		do
		{
			const PLIST_ENTRY t = RemoveTailList2(& voicesphones16);
			voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			InsertHeadList2(& voicesfree16, t);
		}
		while (-- n && ! IsListEmpty2(& voicesphones16));

		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16);
		SPIN_UNLOCK(& locklist16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
	#if WITHBUFFERSDEBUG
		++ e4;
	#endif /* WITHBUFFERSDEBUG */
		return (uintptr_t) & p->buff;
	}
	else
	{
		SPIN_UNLOCK(& locklist16);
		PRINTF(PSTR("allocate_dmabuffer16() failure\n"));
		for (;;)
			;
	}
}

// Этой функцией пользуются обработчики прерываний DMA
// передали буфер, считать свободным
void RAMFUNC release_dmabuffer32tx(uintptr_t addr)
{
	//ASSERT(addr != 0);
	voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	SPIN_LOCK(& locklist32tx);
	InsertHeadList2(& voicesfree32tx, & p->item);
	SPIN_UNLOCK(& locklist32tx);
}

// Этой функцией пользуются обработчики прерываний DMA
// передали буфер, считать свободным
void RAMFUNC release_dmabuffer16(uintptr_t addr)
{
	//ASSERT(addr != 0);
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	buffers_tonull16(p);
}

// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки AF ADC
void RAMFUNC processing_dmabuffer16rx(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n3;
#endif /* WITHBUFFERSDEBUG */
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	buffers_savefrommikeadc(p);
}

// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после приёма пакета с USB AUDIO
void RAMFUNC processing_dmabuffer16rxuac(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n2;
#endif /* WITHBUFFERSDEBUG */
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	buffers_savefromuacout(p);
}

// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки IF ADC (MAIN RX/SUB RX)
// Вызывается на ARM_REALTIME_PRIORITY уровне.
void RAMFUNC processing_dmabuffer32rx(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n1;
	// подсчёт скорости в сэмплах за секунду
	debugcount_rx32adc += CNT32RX;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */
	dsp_extbuffer32rx((const int32_t *) addr);

#if WITHUSBUAC
	static RAMDTCM unsigned rx32adc = 0;
	rx32adc += CNT32RX; 
	while (rx32adc >= CNT16)
	{
		buffers_resample();		// формирование одного буфера синхронного потока из N несинхронного
#if ! WITHI2SHW
		release_dmabuffer16(getfilled_dmabuffer16phones());
#endif /* WITHI2SHW */
		rx32adc -= CNT16;
	}
#endif /* WITHUSBUAC */
}

void release_dmabuffer32rx(uintptr_t addr)
{
	//ASSERT(addr != 0);
	voice32rx_t * const p = CONTAINING_RECORD(addr, voice32rx_t, buff);

	SPIN_LOCK(& locklist32rx);
	InsertHeadList2(& voicesfree32rx, & p->item);
	SPIN_UNLOCK(& locklist32rx);

}


// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки IF ADC (MAIN RX/SUB RX)
// Вызывается на ARM_REALTIME_PRIORITY уровне.
void RAMFUNC processing_dmabuffer32wfm(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n1wfm;
	// подсчёт скорости в сэмплах за секунду
	debugcount_rx32wfm += CNT32RX;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */
	dsp_extbuffer32wfm((const int32_t *) addr);
}

#if WITHRTS192
// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки - канал спектроанализатора
void RAMFUNC processing_dmabuffer32rts(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n4;
#endif /* WITHBUFFERSDEBUG */
	voice192rts_t * const p = CONTAINING_RECORD(addr, voice192rts_t, u.buff);

#if ! WITHTRANSPARENTIQ
	unsigned i;
	for (i = 0; i < DMABUFFSIZE192RTS; i += DMABUFSTEP192RTS)
	{
		const int32_t * const b = (const int32_t *) & p->u.buff [i];

        //saveIQRTSxx(NULL, b [0], b [1]);
        deliveryint(& rtstargetsint, b [0], b [1]);
	}
#endif /* ! WITHTRANSPARENTIQ */

	//sbuffers_savetouacin192rts(p);
	buffers_savetonull192rts(p);
}
#endif /* WITHRTS192 */


// Этой функцией пользуются обработчики прерываний DMA
// получить буфер для передачи через IF DAC
uintptr_t getfilled_dmabuffer32tx_main(void)
{
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_tx32dac += DMABUFFSIZE32TX / DMABUFSTEP32TX;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */

	SPIN_LOCK(& locklist32tx);
	if (! IsListEmpty2(& voicesready32tx))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesready32tx);
		voice32tx_t * const p = CONTAINING_RECORD(t, voice32tx_t, item);
		SPIN_UNLOCK(& locklist32tx);
		return (uintptr_t) & p->buff;
	}
	SPIN_UNLOCK(& locklist32tx);
	return allocate_dmabuffer32tx();	// аварийная ветка - работает первые несколько раз
}

// Этой функцией пользуются обработчики прерываний DMA
// получить буфер для передачи через IF DAC2
uintptr_t getfilled_dmabuffer32tx_sub(void)
{
	return allocate_dmabuffer32tx();
}

// Этой функцией пользуются обработчики прерываний DMA
// получить буфер для передачи через AF DAC
uintptr_t getfilled_dmabuffer16phones(void)
{
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_phonesdac += DMABUFFSIZE16 / DMABUFSTEP16;	// в буфере пары сэмплов по два байта
#endif /* WITHBUFFERSDEBUG */

	SPIN_LOCK(& locklist16);
	if (! IsListEmpty2(& voicesphones16))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesphones16);
		SPIN_UNLOCK(& locklist16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
		dsp_addsidetone(p->buff, 1);
		return (uintptr_t) & p->buff;	// алрес для DMA
	}
	SPIN_UNLOCK(& locklist16);

#if WITHBUFFERSDEBUG
	++ e1;
#endif /* WITHBUFFERSDEBUG */

	const uintptr_t addr = allocate_dmabuffer16();
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	dsp_addsidetone(p->buff, 0); // Заполнение "тишиной"
	return (uintptr_t) & p->buff;
}

//////////////////////////////////////////
// Поэлементное заполнение буфера IF DAC

// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
// 32 bit, signed
void savesampleout32stereo(int_fast32_t ch0, int_fast32_t ch1)
{
	static voice32tx_t * RAMDTCM prepareout32tx = NULL;
	static RAMDTCM unsigned level32tx;

	if (prepareout32tx == NULL)
	{
		const uint32_t addr = allocate_dmabuffer32tx();
		voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
		prepareout32tx = p;
		level32tx = 0;
	}

	prepareout32tx->buff [level32tx + DMABUF32TXI] = ch0;
	prepareout32tx->buff [level32tx + DMABUF32TXQ] = ch1;

	if ((level32tx += DMABUFSTEP32TX) >= DMABUFFSIZE32TX)
	{
		SPIN_LOCK(& locklist32tx);
		InsertHeadList2(& voicesready32tx, & prepareout32tx->item);
		SPIN_UNLOCK(& locklist32tx);
		prepareout32tx = NULL;
	}
}

//////////////////////////////////////////
// Поэлементное заполнение буфера AF DAC

static void savesampleout16stereo_user(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	enum { L, R };
	// если есть инициализированный канал для выдачи звука
	static voice16_t * RAMDTCM p = NULL;
	static RAMDTCM unsigned n;

	if (p == NULL)
	{
		global_disableIRQ();
		uintptr_t addr = allocate_dmabuffer16();
		global_enableIRQ();
		p = CONTAINING_RECORD(addr, voice16_t, buff);
		n = 0;
	}
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	p->buff [n * DMABUFSTEP16 + L] = AUDIO16TOAUB(ch0);		// sample value
#if DMABUFSTEP16 > 1
	p->buff [n * DMABUFSTEP16 + R] = AUDIO16TOAUB(ch1);	// sample value
#endif

	if (++ n >= CNT16)
	{
		global_disableIRQ();
		buffers_savefromrxout(p);
		global_enableIRQ();
		p = NULL;
	}
}

static void savesampleout16stereo(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	enum { L, R };
	// если есть инициализированный канал для выдачи звука
	static voice16_t * RAMDTCM p = NULL;
	static RAMDTCM unsigned n;

	if (p == NULL)
	{
		uintptr_t addr = allocate_dmabuffer16();
		p = CONTAINING_RECORD(addr, voice16_t, buff);
		n = 0;
	}
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	p->buff [n * DMABUFSTEP16 + L] = AUDIO16TOAUB(ch0);		// sample value
#if DMABUFSTEP16 > 1
	p->buff [n * DMABUFSTEP16 + R] = AUDIO16TOAUB(ch1);	// sample value
#endif

	if (++ n >= CNT16)
	{
		buffers_savefromrxout(p);
		p = NULL;
	}
}

#if WITHUSBUAC && WITHUSBHW

	#if WITHRTS96

		// Этой функцией пользуются обработчики прерываний DMA на приём данных по SAI
		static uintptr_t allocate_dmabuffer96rts(void)
		{
			SPIN_LOCK(& locklistrts);
			if (! IsListEmpty2(& uacin96rtsfree))
			{
				const PLIST_ENTRY t = RemoveTailList2(& uacin96rtsfree);
				voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
				SPIN_UNLOCK(& locklistrts);
			#if WITHBUFFERSDEBUG
				++ n5;
			#endif /* WITHBUFFERSDEBUG */
				if (p->tag != BUFFTAG_RTS96)
				{
					PRINTF(" p=%p, tag=%d, tag2=%p, tag3=%p\n", p, p->tag, p->tag2, p->tag3);
					//printhex(0, p->u.buff, sizeof p->u.buff);
				}
				ASSERT(p->tag == BUFFTAG_RTS96);
				ASSERT(p->tag2 == p);
				ASSERT(p->tag3 == p);
				return (uintptr_t) & p->u.buff;
			}
			else if (! IsListEmpty2(& uacin96rtsready))
			{
				// Ошибочная ситуация - если буферы не освобождены вовремя -
				// берём из очереди готовых к передаче в компьютер по USB.
				// Очередь очищается возможно не полностью.
				uint_fast8_t n = 3;
				do
				{
					const PLIST_ENTRY t = RemoveTailList2(& uacin96rtsready);
					voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
					if (p->tag != BUFFTAG_RTS96)
					{
						PRINTF(" p=%p, tag=%d, tag2=%p, tag3=%p\n", p, p->tag, p->tag2, p->tag3);
						//printhex(0, p->u.buff, sizeof p->u.buff);
					}
					ASSERT(p->tag == BUFFTAG_RTS96);
					ASSERT(p->tag2 == p);
					ASSERT(p->tag3 == p);
					InsertHeadList2(& uacin96rtsfree, t);
				} while (-- n && ! IsListEmpty2(& uacin96rtsready));

				const PLIST_ENTRY t = RemoveTailList2(& uacin96rtsfree);
				voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
				SPIN_UNLOCK(& locklistrts);
			#if WITHBUFFERSDEBUG
				++ e5;
			#endif /* WITHBUFFERSDEBUG */
				if (p->tag != BUFFTAG_RTS96)
				{
					PRINTF(" p=%p, tag=%d, tag2=%p, tag3=%p\n", p, p->tag, p->tag2, p->tag3);
					//printhex(0, p->u.buff, sizeof p->u.buff);
				}
				ASSERT(p->tag == BUFFTAG_RTS96);
				ASSERT(p->tag2 == p);
				ASSERT(p->tag3 == p);
				return (uintptr_t) & p->u.buff;
			}
			else
			{
				SPIN_UNLOCK(& locklistrts);
				PRINTF(PSTR("allocate_dmabuffer96rts() failure\n"));
				for (;;)
					;
			}
		}

		// Этой функцией пользуются обработчики прерываний DMA
		// передали буфер, считать свободным
		static void release_dmabuffer96rts(uintptr_t addr)
		{
			ASSERT(addr != 0);
			voice96rts_t * const p = CONTAINING_RECORD(addr, voice96rts_t, u.buff);
			ASSERT(p->tag == BUFFTAG_RTS96);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			SPIN_LOCK(& locklistrts);
			InsertHeadList2(& uacin96rtsfree, & p->item);
			SPIN_UNLOCK(& locklistrts);
		}

		// Этой функцией пользуются обработчики прерываний DMA
		// получить буфер для передачи в компьютер, через USB AUDIO
		// Если в данный момент нет готового буфера, возврат 0
		static uint32_t getfilled_dmabuffer96uacinrts(void)
		{
			SPIN_LOCK(& locklistrts);
			if (! IsListEmpty2(& uacin96rtsready))
			{
				PLIST_ENTRY t = RemoveTailList2(& uacin96rtsready);
				voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
				SPIN_UNLOCK(& locklistrts);
				if (p->tag != BUFFTAG_RTS96)
				{
					PRINTF(" p=%p, tag=%d, tag2=%p, tag3=%p\n", p, p->tag, p->tag2, p->tag3);
					//printhex(0, p->u.buff, sizeof p->u.buff);
				}
				ASSERT(p->tag == BUFFTAG_RTS96);
				ASSERT(p->tag2 == p);
				ASSERT(p->tag3 == p);
				return (uintptr_t) & p->u.buff;
			}
			SPIN_UNLOCK(& locklistrts);
			return 0;
		}

		// Поэлементное заполнение буфера RTS96

		// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
		// vl, vr: 32 bit, signed - преобразуем к требуемому формату для передачи по USB здесь.
		void savesampleout96stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
		{
			// если есть инициализированный канал для выдачи звука
			static voice96rts_t * RAMDTCM p = NULL;
			static RAMDTCM unsigned n;

			if (p == NULL)
			{
				if (! isrts96())
					return;
				uint32_t addr = allocate_dmabuffer96rts();
				p = CONTAINING_RECORD(addr, voice96rts_t, u.buff);
				n = 0;
				ASSERT(p->tag == BUFFTAG_RTS96);
				ASSERT(p->tag2 == p);
				ASSERT(p->tag3 == p);
			}
			else if (! isrts96())
			{
				buffers_savetonull96rts(p);
				p = NULL;
				return;
			}

			ASSERT(p->tag == BUFFTAG_RTS96);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			ASSERT(DMABUFSTEP96RTS == 6);
			p->u.buff [n ++] = ch0 >> 8;	// sample value
			p->u.buff [n ++] = ch0 >> 16;	// sample value
			p->u.buff [n ++] = ch0 >> 24;	// sample value
			p->u.buff [n ++] = ch1 >> 8;	// sample value
			p->u.buff [n ++] = ch1 >> 16;	// sample value
			p->u.buff [n ++] = ch1 >> 24;	// sample value

			if (n >= DMABUFFSIZE96RTS)
			{
				ASSERT(p->tag == BUFFTAG_RTS96);
				ASSERT(p->tag2 == p);
				ASSERT(p->tag3 == p);
				buffers_savetouacin96rts(p);
				p = NULL;
			}
		}
			
	#endif /* WITHRTS96 */

	#if WITHRTS192

		// Этой функцией пользуются обработчики прерываний DMA
		// получить буфер для передачи в компьютер, через USB AUDIO
		// Если в данный момент нет готового буфера, возврат 0
		static uintptr_t getfilled_dmabuffer192uacinrts(void)
		{
			SPIN_LOCK(& locklistrts);
			if (! IsListEmpty2(& uacin192rts))
			{
				PLIST_ENTRY t = RemoveTailList2(& uacin192rts);
				voice192rts_t * const p = CONTAINING_RECORD(t, voice192rts_t, item);
				SPIN_UNLOCK(& locklistrts);
				return (uintptr_t) & p->u.buff;
			}
			SPIN_UNLOCK(& locklistrts);
			return 0;
		}

		// Этой функцией пользуются обработчики прерываний DMA на приём данных по SAI
		uint32_t allocate_dmabuffer192rts(void)
		{
			SPIN_LOCK(& locklistrts);
			if (! IsListEmpty2(& voicesfree192rts))
			{
				PLIST_ENTRY t = RemoveTailList2(& voicesfree192rts);
				SPIN_UNLOCK(& locklistrts);
				voice192rts_t * const p = CONTAINING_RECORD(t, voice192rts_t, item);
				ASSERT(p->tag == BUFFTAG_RTS192);
				ASSERT(p->tag2 == p);
				ASSERT(p->tag3 == p);
			#if WITHBUFFERSDEBUG
				++ n5;
			#endif /* WITHBUFFERSDEBUG */
				return (uintptr_t) & p->u.buff;
			}
			else if (! IsListEmpty2(& uacin192rts))
			{
				// Ошибочная ситуация - если буферы не освобождены вовремя -
				// берём из очереди готовых к передаче в компьютер по USB.
				// Очередь очищается возможно не полностью.
				uint_fast8_t n = 3;
				do
				{
					const PLIST_ENTRY t = RemoveTailList2(& uacin192rts);
					voice192rts_t * const p = CONTAINING_RECORD(t, voice192rts_t, item);
					ASSERT(p->tag == BUFFTAG_RTS192);
					ASSERT(p->tag2 == p);
					ASSERT(p->tag3 == p);
					InsertHeadList2(& voicesfree192rts, t);
				} while (-- n && ! IsListEmpty2(& uacin192rts));

				const PLIST_ENTRY t = RemoveTailList2(& voicesfree192rts);
				SPIN_UNLOCK(& locklistrts);
				voice192rts_t * const p = CONTAINING_RECORD(t, voice192rts_t, item);
				ASSERT(p->tag == BUFFTAG_RTS192);
				ASSERT(p->tag2 == p);
				ASSERT(p->tag3 == p);
			#if WITHBUFFERSDEBUG
				++ e5;
			#endif /* WITHBUFFERSDEBUG */
				return (uintptr_t) & p->u.buff;
			}
			else
			{
				SPIN_UNLOCK(& locklistrts);
				PRINTF(PSTR("allocate_dmabuffer192rts() failure\n"));
				for (;;)
					;
			}
		}

		// Этой функцией пользуются обработчики прерываний DMA
		// передали буфер, считать свободным
		static void release_dmabuffer192rts(uint32_t addr)
		{
			voice192rts_t * const p = CONTAINING_RECORD(addr, voice192rts_t, u.buff);
			ASSERT(p->tag == BUFFTAG_RTS192);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			SPIN_LOCK(& locklistrts);
			InsertHeadList2(& voicesfree192rts, & p->item);
			SPIN_UNLOCK(& locklistrts);
		}

		// NOT USED
		// Поэлементное заполнение буфера RTS192

		// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
		// vl, vr: 32 bit, signed - преобразуем к требуемому формату для передачи по USB здесь.
		void savesampleout192stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
		{
			// если есть инициализированный канал для выдачи звука
			static voice192rts_t * RAMDTCM p = NULL;
			static RAMDTCM unsigned n;

			if (p == NULL)
			{
				if (! isrts192())
					return;
				uint32_t addr = allocate_dmabuffer192rts();
				p = CONTAINING_RECORD(addr, voice192rts_t, u.buff);
				ASSERT(p->tag == BUFFTAG_RTS192);
				ASSERT(p->tag2 == p);
				ASSERT(p->tag3 == p);
				n = 0;
			}
			else if (! isrts192())
			{
				buffers_savetonull192rts(p);
				p = NULL;
				return;
			}

			ASSERT(p->tag == BUFFTAG_RTS192);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			p->u.buff [n ++] = ch0;	// sample value
			p->u.buff [n ++] = ch1;	// sample value

			if (n >= DMABUFFSIZE192RTS)
			{
				buffers_savetouacin192rts(p);
				p = NULL;
			}
		}

	#endif /* WITHRTS192 */

	// Сохранить USB UAC IN буфер в никуда...
	static RAMFUNC void buffers_tonulluacin(uacin16_t * p)
	{
		SPIN_LOCK(& locklistuacin16);
		InsertHeadList2(& uacinfree16, & p->item);
		SPIN_UNLOCK(& locklistuacin16);
	}

	void RAMFUNC release_dmabufferuacin16(uintptr_t addr)
	{
		//ASSERT(addr != 0);
		uacin16_t * const p = CONTAINING_RECORD(addr, uacin16_t, u.buff);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		buffers_tonulluacin(p);
	}

	RAMFUNC uintptr_t allocate_dmabufferuacin16(void)
	{
		SPIN_LOCK(& locklistuacin16);
		if (! IsListEmpty2(& uacinfree16))
		{
			PLIST_ENTRY t = RemoveTailList2(& uacinfree16);
			SPIN_UNLOCK(& locklistuacin16);
			uacin16_t * const p = CONTAINING_RECORD(t, uacin16_t, item);
			ASSERT(p->tag == BUFFTAG_UACIN16);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			return (uintptr_t) & p->u.buff;
		}
		else if (! IsListEmpty2(& uacinready16))
		{
			// Ошибочная ситуация - если буферы не освобождены вовремя -
			// берём из очереди готовых к передаче

			uint_fast8_t n = 3;
			do
			{
				const PLIST_ENTRY t = RemoveTailList2(& uacinready16);
				InsertHeadList2(& uacinfree16, t);
			}
			while (-- n && ! IsListEmpty2(& uacinready16));

			PLIST_ENTRY t = RemoveTailList2(& uacinfree16);
			SPIN_UNLOCK(& locklistuacin16);
			uacin16_t * const p = CONTAINING_RECORD(t, uacin16_t, item);
			ASSERT(p->tag == BUFFTAG_UACIN16);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			return (uintptr_t) & p->u.buff;
		}
		else
		{
			SPIN_UNLOCK(& locklistuacin16);
			PRINTF(PSTR("allocate_dmabufferuacin16() failure, uacinalt=%d\n"), uacinalt);
			for (;;)
				;
		}
		return 0;
	}

	// Этой функцией пользуются обработчики прерываний DMA
	// получить буфер для передачи в компьютер, через USB AUDIO
	// Если в данный момент нет готового буфера, возврат 0
	static uintptr_t getfilled_dmabuffer16uacin(void)
	{
		SPIN_LOCK(& locklistuacin16);
		if (! IsListEmpty2(& uacinready16))
		{
			PLIST_ENTRY t = RemoveTailList2(& uacinready16);
			SPIN_UNLOCK(& locklistuacin16);
			uacin16_t * const p = CONTAINING_RECORD(t, uacin16_t, item);
			ASSERT(p->tag == BUFFTAG_UACIN16);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			return (uintptr_t) & p->u.buff;
		}
		SPIN_UNLOCK(& locklistuacin16);
		return 0;
	}

	// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
	// vl, vr: 16 bit, signed - требуемый формат для передачи по USB.

	void savesamplerecord16uacin(int_fast16_t ch0, int_fast16_t ch1)
	{
	#if WITHUSBUACIN
		// если есть инициализированный канал для выдачи звука
		static uacin16_t * p = NULL;
		static unsigned n = 0;

		if (p == NULL)
		{
			if (! isaudio48())
				return;
			uintptr_t addr = allocate_dmabufferuacin16();
			p = CONTAINING_RECORD(addr, uacin16_t, u.buff);
			ASSERT(p->tag == BUFFTAG_UACIN16);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			n = 0;
		}
		else if (! isaudio48())
		{
			buffers_tonulluacin(p);
			p = NULL;
			return;
		}

		ASSERT(p->tag == BUFFTAG_UACIN16);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		p->u.buff [n + 0] = ch0;		// sample value
#if DMABUFSTEPUACIN16 > 1
		p->u.buff [n + 1] = ch1;	// sample value
#endif
		n += DMABUFSTEPUACIN16;

		if (n >= DMABUFFSIZEUACIN16)
		{
			buffers_savetouacin(p);
			p = NULL;
		}
	#endif /* WITHUSBUACIN */
	}

#else /* WITHUSBUAC */

void savesampleout96stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
{
}

void savesampleout192stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
{
}

void savesamplerecord16uacin(int_fast16_t ch0, int_fast16_t ch1)
{
}

#endif /* WITHUSBUAC */

/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
void board_set_uacplayer(uint_fast8_t v)
{
	uacoutplayer = v;
}

/* на вход трансивера берутся аудиоданные с USB виртуальной платы, а не с микрофона */
void board_set_uacmike(uint_fast8_t v)
{
	uacoutmike = v;
}


#if WITHUSBUAC && WITHUSBHW

/* +++ UAC OUT data save */



void 
buffers_set_uacinalt(uint_fast8_t v)	/* выбор альтернативной конфигурации для UAC IN interface */
{
	//PRINTF(PSTR("buffers_set_uacinalt: v=%d\n"), (int) v);
	uacinalt = v;
}

void 
buffers_set_uacinrtsalt(uint_fast8_t v)	/* выбор альтернативной конфигурации для UAC IN interface */
{
	//PRINTF(PSTR("buffers_set_uacinrtsalt: v=%d\n"), (int) v);
	uacinrtsalt = v;
}

void 
buffers_set_uacoutalt(uint_fast8_t v)	/* выбор альтернативной конфигурации для UAC OUT interface */
{
	//PRINTF(PSTR("buffers_set_uacoutalt: v=%d\n"), (int) v);
	uacoutalt = v;
}

static uint_fast16_t ulmin16(uint_fast16_t a, uint_fast16_t b)
{
	return a < b ? a : b;
}

static uintptr_t uacoutaddr;	// address of DMABUFFSIZE16 * sizeof (int16_t) bytes
static uint_fast16_t uacoutbufflevel;	// количество байтовЮ на которые заполнен буфер

/* вызывается при запрещённых прерываниях. */
void uacout_buffer_initialize(void)
{
	uacoutaddr = 0;
	uacoutbufflevel = 0;
}

/* вызыватся из ARM_SYSTEM_PRIORITY функции обработчика прерывания */
void uacout_buffer_start(void)
{
	if (uacoutaddr)
		TP();
}
/* вызыватся из ARM_SYSTEM_PRIORITY функции обработчика прерывания */
void uacout_buffer_stop(void)
{
	if (uacoutaddr != 0)
	{
		global_disableIRQ();
		release_dmabuffer16(uacoutaddr);
		global_enableIRQ();
		uacoutaddr = 0;
		uacoutbufflevel = 0;
	}
}

static aufastbufv_t fetch_le(const uint8_t * p, size_t usbsz)
{
	int32_t v = 0;
	uint_fast8_t i;
	for (i = 0; i < usbsz; ++ i)
	{
		v = v * 256 + p [usbsz - i - 1];
	}
	for (; i < 4; ++ i)
	{
		v = v * 256;
	}
	/* теперь старший бит полученного с USB сэмпла находится в знаковом бите переменной v */
	return v >> (8 * (4 - sizeof (aubufv_t)));
}

/* вызыватся из не-realtime функции обработчика прерывания */
// Работает на ARM_SYSTEM_PRIORITY
void uacout_buffer_save_system(const uint8_t * buff, uint_fast16_t size, uint_fast8_t ichannels, uint_fast8_t ibits)
{
	const size_t dmabuffer16size = DMABUFFSIZE16 * sizeof (aubufv_t);	// размер в байтах
	const size_t usbsz = (ibits + 7) / 8;

	for (;;)
	{
		const uint_fast16_t insamples = size / usbsz / ichannels;	// количество сэмплов во входном буфере
		const uint_fast16_t outsamples = (dmabuffer16size - uacoutbufflevel) / sizeof (aubufv_t) / DMABUFSTEP16;
		const uint_fast16_t chunksamples = ulmin16(insamples, outsamples);
		const size_t inchunk = chunksamples * usbsz * ichannels;
		const size_t outchunk = chunksamples * sizeof (aubufv_t) * DMABUFSTEP16;	// разхмер в байтах

		if (chunksamples == 0)
			break;
		if (uacoutaddr == 0)
		{
 			global_disableIRQ();
			uacoutaddr = allocate_dmabuffer16();
			global_enableIRQ();
			uacoutbufflevel = 0;
		}

		if (ichannels < DMABUFSTEP16)
		{
			// копирование нужного количества сэмплов с прербразованием из моно в стерео
			const uint8_t * src = buff;
			aubufv_t * dst = (aubufv_t *) ((uint8_t *) uacoutaddr + uacoutbufflevel);
			uint_fast16_t n = chunksamples;
			while (n --)
			{
				const aufastbufv_t v = fetch_le(src, usbsz);
				* dst ++ = v;
				* dst ++ = v;
				src += usbsz;
			}
		}
		else if (usbsz != sizeof (aubufv_t))
		// требуется преобразование формата из 16-бит семплов ко внутреннему формату aubufv_t
		{
			/* копирование 16 бит сэмплов с расширением */
			const uint8_t * src = buff;
			aubufv_t * dst = (aubufv_t *) ((uint8_t *) uacoutaddr + uacoutbufflevel);
			uint_fast16_t n = chunksamples * ichannels;
			while (n --)
			{
				const aufastbufv_t v = fetch_le(src, usbsz);
				* dst ++ = v;
				src += usbsz;
			}
		}
		else
		{
			memcpy((uint8_t *) uacoutaddr + uacoutbufflevel, buff, inchunk);
		}

		size -= inchunk;	// проход по входому буферу
		buff += inchunk;	// проход входому буферу

		if ((uacoutbufflevel += outchunk) >= dmabuffer16size)
		{
			global_disableIRQ();
			processing_dmabuffer16rxuac(uacoutaddr);
			global_enableIRQ();
			uacoutaddr = 0;
			uacoutbufflevel = 0;
		}
	}
}

/* вызыватся из realtime функции обработчика прерывания */
// Работает на ARM_REALTIME_PRIORITY
void uacout_buffer_save_realtime(const uint8_t * buff, uint_fast16_t size, uint_fast8_t ichannels, uint_fast8_t ibits)
{
	const size_t dmabuffer16size = DMABUFFSIZE16 * sizeof (aubufv_t);	// размер в байтах
	const size_t usbsz = (ibits + 7) / 8;

	for (;;)
	{
		const uint_fast16_t insamples = size / usbsz / ichannels;	// количество сэмплов во входном буфере
		const uint_fast16_t outsamples = (dmabuffer16size - uacoutbufflevel) / sizeof (aubufv_t) / DMABUFSTEP16;
		const uint_fast16_t chunksamples = ulmin16(insamples, outsamples);
		const size_t inchunk = chunksamples * usbsz * ichannels;
		const size_t outchunk = chunksamples * sizeof (aubufv_t) * DMABUFSTEP16;	// разхмер в байтах

		if (chunksamples == 0)
			break;
		if (uacoutaddr == 0)
		{
 			//global_disableIRQ();
			uacoutaddr = allocate_dmabuffer16();
			//global_enableIRQ();
			uacoutbufflevel = 0;
		}

		if (ichannels < DMABUFSTEP16)
		{
			// копирование нужного количества сэмплов с прербразованием из моно в стерео
			const uint8_t * src = buff;
			aubufv_t * dst = (aubufv_t *) ((uint8_t *) uacoutaddr + uacoutbufflevel);
			uint_fast16_t n = chunksamples;
			while (n --)
			{
				const aufastbufv_t v = fetch_le(src, usbsz);
				* dst ++ = v;
				* dst ++ = v;
				src += usbsz;
			}
		}
		else if (usbsz != sizeof (aubufv_t))
		// требуется преобразование формата из 16-бит семплов ко внутреннему формату aubufv_t
		{
			/* копирование 16 бит сэмплов с расширением */
			const uint8_t * src = buff;
			aubufv_t * dst = (aubufv_t *) ((uint8_t *) uacoutaddr + uacoutbufflevel);
			uint_fast16_t n = chunksamples * ichannels;
			while (n --)
			{
				const aufastbufv_t v = fetch_le(src, usbsz);
				* dst ++ = v;
				src += usbsz;
			}
		}
		else
		{
			memcpy((uint8_t *) uacoutaddr + uacoutbufflevel, buff, inchunk);
		}

		size -= inchunk;	// проход по входому буферу
		buff += inchunk;	// проход входому буферу

		if ((uacoutbufflevel += outchunk) >= dmabuffer16size)
		{
			//global_disableIRQ();
			processing_dmabuffer16rxuac(uacoutaddr);
			//global_enableIRQ();
			uacoutaddr = 0;
			uacoutbufflevel = 0;
		}
	}
}
/* --- UAC OUT data save */


/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferx(uintptr_t addr)
{
	//ASSERT(addr != 0);
	uacin16_t * const p = CONTAINING_RECORD(addr, uacin16_t, u.buff);
	switch (p->tag)
	{
#if WITHRTS96
	case BUFFTAG_RTS96:
		release_dmabuffer96rts(addr);
		return;
#endif /* WITHRTS96 */

#if WITHRTS192
	case BUFFTAG_RTS192:
		release_dmabuffer192rts(addr);
		return;
#endif /* WITHRTS192 */

	case BUFFTAG_UACIN16:
		release_dmabufferuacin16(addr);
		return;

	default:
		PRINTF(PSTR("release_dmabufferx: wrong tag value: p=%p, %02X\n"), p, p->tag);
		for (;;)
			;
	}
}

/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
uintptr_t getfilled_dmabufferx(uint_fast16_t * sizep)
{
#if WITHBUFFERSDEBUG
	++ n6;
#endif /* WITHBUFFERSDEBUG */
	switch (uacinalt)
	{
	case UACINALT_NONE:
		return 0;

	case UACINALT_AUDIO48:
		* sizep = UACIN_AUDIO48_DATASIZE;
		return getfilled_dmabuffer16uacin();

#if ! WITHUSBUACIN2

#if WITHRTS96
	case UACINALT_RTS96:
		* sizep = UACIN_RTS96_DATASIZE;
		return getfilled_dmabuffer96uacinrts();
#endif /* WITHRTS192 */

#if WITHRTS192
	case UACINALT_RTS192:
		* sizep = UACIN_RTS192_DATASIZE;
		return getfilled_dmabuffer192uacinrts();
#endif /* WITHRTS192 */

#endif /* ! WITHUSBUACIN2 */

	default:
		PRINTF(PSTR("getfilled_dmabufferx: uacinalt=%u\n"), uacinalt);
		ASSERT(0);
		return 0;
	}
}

#if WITHUSBUACIN2

/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
uintptr_t getfilled_dmabufferxrts(uint_fast16_t * sizep)
{
#if WITHBUFFERSDEBUG
	++ n6;
#endif /* WITHBUFFERSDEBUG */
	switch (uacinrtsalt)
	{
	case UACINRTSALT_NONE:
		return 0;

#if WITHUSBUACIN2 && WITHUSBHW

#if WITHRTS96
	case UACINRTSALT_RTS96:
		* sizep = UACIN_RTS96_DATASIZE;
		return getfilled_dmabuffer96uacinrts();
#endif /* WITHRTS192 */

#if WITHRTS192
	case UACINRTSALT_RTS192:
		* sizep = UACIN_RTS192_DATASIZE;
		return getfilled_dmabuffer192uacinrts();
#endif /* WITHRTS192 */

#endif /* WITHUSBUACIN2 && WITHUSBHW */

	default:
		PRINTF(PSTR("getfilled_dmabufferxrts: uacinrtsalt=%u\n"), uacinrtsalt);
		ASSERT(0);
		return 0;
	}
}

void release_dmabufferxrts(uintptr_t addr)	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
{
	release_dmabufferx(addr);
}

#endif /* WITHUSBUACIN2 */

#endif /* WITHUSBUAC */


void deliveryfloat(deliverylist_t * list, FLOAT_t ch0, FLOAT_t ch1)
{
	PLIST_ENTRY t;
	SPIN_LOCK(& list->listlock);
	for (t = list->head.Blink; t != & list->head; t = t->Blink)
	{
		subscribefloat_t * const p = CONTAINING_RECORD(t, subscribefloat_t, item);
		(p->cb)(p->ctx, ch0, ch1);
	}
	SPIN_UNLOCK(& list->listlock);
}

void deliveryint(deliverylist_t * list, int_fast32_t ch0, int_fast32_t ch1)
{
	PLIST_ENTRY t;
	SPIN_LOCK(& list->listlock);
	for (t = list->head.Blink; t != & list->head; t = t->Blink)
	{
		subscribeint32_t * const p = CONTAINING_RECORD(t, subscribeint32_t, item);
		(p->cb)(p->ctx, ch0, ch1);
	}
	SPIN_UNLOCK(& list->listlock);
}

void subscribefloat(deliverylist_t * list, subscribefloat_t * target, void * ctx, void (* pfn)(void * ctx, FLOAT_t ch0, FLOAT_t ch1))
{
	target->cb = pfn;
	target->ctx = ctx;
	SPIN_LOCK(& list->listlock);
	InsertHeadList(& list->head, & target->item);
	SPIN_UNLOCK(& list->listlock);
}

void subscribeint(deliverylist_t * list, subscribeint32_t * target, void * ctx, void (* pfn)(void * ctx, int_fast32_t ch0, int_fast32_t ch1))
{
	target->cb = pfn;
	target->ctx = ctx;
	SPIN_LOCK(& list->listlock);
	InsertHeadList(& list->head, & target->item);
	SPIN_UNLOCK(& list->listlock);
}

void subscribefloat_user(deliverylist_t * list, subscribefloat_t * target, void * ctx, void (* pfn)(void * ctx, FLOAT_t ch0, FLOAT_t ch1))
{
	target->cb = pfn;
	target->ctx = ctx;
	global_disableIRQ();
	SPIN_LOCK(& list->listlock);
	InsertHeadList(& list->head, & target->item);
	SPIN_UNLOCK(& list->listlock);
	global_enableIRQ();
}

void subscribeint_user(deliverylist_t * list, subscribeint32_t * target, void * ctx, void (* pfn)(void * ctx, int_fast32_t ch0, int_fast32_t ch1))
{
	target->cb = pfn;
	target->ctx = ctx;
	global_disableIRQ();
	SPIN_LOCK(& list->listlock);
	InsertHeadList(& list->head, & target->item);
	SPIN_UNLOCK(& list->listlock);
	global_enableIRQ();
}

void deliverylist_initialize(deliverylist_t * list)
{
	InitializeListHead(& list->head);
	SPINLOCK_INITIALIZE(& list->listlock);
}

#endif /* WITHINTEGRATEDDSP */

