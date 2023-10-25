/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints

//#define WITHBUFFERSDEBUG WITHDEBUG
#define BUFOVERSIZE 1

#if WITHINTEGRATEDDSP

#include "mslist.h"
#include "audio.h"
#include "buffers.h"

#if WITHUSBHW
#include "usb/usb200.h"
#include "usb/usbch9.h"
#endif /* WITHUSBHW */

#include <string.h>		// for memset

#if defined(WITHRTS96) && defined(WITHRTS192)
	#error Configuration Error: WITHRTS96 and WITHRTS192 can not be used together
#endif /* defined(WITHRTS96) && defined(WITHRTS192) */
enum
{
	BUFFTAG_UACIN48 = 44,
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
	//LCLSPINLOCK_t lock2;
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
	//LCLSPINLOCK_INITIALIZE(& ListHead->lock2);
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
	ASSERT(! IsListEmpty3(ListHead));
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
		PRINTF(PSTR(# name "[%3d,rdy=%d(%u)] "), (int) GetCountList3(& (name)), (int) GetReadyList3(& (name)), (unsigned) (name).RdyLevel); \
	} while (0)

#if 0
static RAMBIGDTCM int16_t vfyseq;
static RAMBIGDTCM int16_t lastseq;
static RAMBIGDTCM int lastseqvalid;

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
typedef ALIGNX_BEGIN struct voice16rx_tag
{
	void * tag2;
	ALIGNX_BEGIN aubufv_t rbuff [DMABUFFSIZE16RX] ALIGNX_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
	void * tag3;
} ALIGNX_END voice16rx_t;

int_fast32_t buffers_dmabuffer16rxcachesize(void)
{
	return offsetof(voice16rx_t, item) - offsetof(voice16rx_t, rbuff);
}
//
// Audio CODEC in/out
typedef ALIGNX_BEGIN struct voice16tx_tag
{
	void * tag2;
	ALIGNX_BEGIN aubufv_t tbuff [DMABUFFSIZE16TX] ALIGNX_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
	void * tag3;
} ALIGNX_END voice16tx_t;

int_fast32_t buffers_dmabuffer16txcachesize(void)
{
	return offsetof(voice16tx_t, item) - offsetof(voice16tx_t, tbuff);
}

// I/Q data to FPGA or IF CODEC
typedef ALIGNX_BEGIN struct voices32tx_tag
{
	void * tag2;
	ALIGNX_BEGIN IFDACvalue_t buff [DMABUFFSIZE32TX] ALIGNX_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
	void * tag3;
} ALIGNX_END voice32tx_t;

int_fast32_t buffers_dmabuffer32txcachesize(void)
{
	return offsetof(voice32tx_t, item) - offsetof(voice32tx_t, buff);
}

// I/Q data from FPGA or IF CODEC
typedef ALIGNX_BEGIN struct voices32rx_tag
{
	ALIGNX_BEGIN IFADCvalue_t buff [DMABUFFSIZE32RX] ALIGNX_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
} ALIGNX_END voice32rx_t;

int_fast32_t buffers_dmabuffer32rxcachesize(void)
{
	return offsetof(voice32rx_t, item) - offsetof(voice32rx_t, buff);
}

// I/Q SPECTRUM data from FPGA or IF CODEC
typedef ALIGNX_BEGIN struct voices32rts_tag
{
	ALIGNX_BEGIN IFADCvalue_t buff [DMABUFFSIZE32RTS] ALIGNX_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
} ALIGNX_END voice32rts_t;

int_fast32_t buffers_dmabuffer32rtscachesize(void)
{
	/* надо сделать правильно - вернуь размер буфера в байтах */
	ASSERT(0);
	return offsetof(voice32rts_t, item) - offsetof(voice32rts_t, buff);
}

#if 1
	// исправляемая погрешность = 0.02% - один сэмпл добавить/убрать на 5000 сэмплов
	//enum { SKIPPEDBLOCKS = 4000 / (DMABUFFSIZE16RX / DMABUFFSTEP16RX) };
	// исправляемая погрешность = 0.02% - один сэмпл добавить/убрать на 2000 сэмплов
	enum { SKIPPEDBLOCKS = 2000 / (DMABUFFSIZE16RX / DMABUFFSTEP16RX) };

#else
	// исправляемая погрешность = 0.1% - один сэмпл добавить/убрать на 1000 сэмплов
	enum { SKIPPEDBLOCKS = 1000 / (DMABUFFSIZE16RX / DMABUFFSTEP16RX) };
#endif

enum { RESAMPLE16NORMAL = SKIPPEDBLOCKS * 2 };	// Нормальное количество буферов в очереди

enum { CNT16RX = DMABUFFSIZE16RX / DMABUFFSTEP16RX };
enum { CNT16TX = DMABUFFSIZE16TX / DMABUFFSTEP16TX };
enum { CNT32RX = DMABUFFSIZE32RX / DMABUFFSTEP32RX };
enum { CNT32TX = DMABUFFSIZE32TX / DMABUFFSTEP32TX };
enum { CNT32RTS = DMABUFFSIZE32RTS / DMABUFFSTEP32RTS };
enum { VOICESMIKE16NORMAL = 6 };	// Нормальное количество буферов в очереди
enum { MIKELEVEL = 6 };
enum { PHONESLEVEL = 6 };

static RAMBIGDTCM LIST_HEAD2 voicesfree16rx;
static RAMBIGDTCM LIST_HEAD3 voicesmike16rx;	// буферы с оцифрованными звуками с микрофона/Line in
static RAMBIGDTCM LIST_HEAD3 resample16rx;		// буферы от USB для синхронизации
static RAMBIGDTCM LIST_HEAD3 voicesusb16rx;	// буферы с оцифрованными звуками с USB AUDIO (после ресэмплинга)
static RAMBIGDTCM LCLSPINLOCK_t locklist16rx = LCLSPINLOCK_INIT;

static RAMBIGDTCM LIST_HEAD2 voicesfree16tx;
static RAMBIGDTCM LIST_HEAD3 voicesphones16tx;	// буферы, предназначенные для выдачи на наушники
static RAMBIGDTCM LIST_HEAD3 voicesmoni16tx;	// буферы, предназначенные для звука самоконтроля
static RAMBIGDTCM LCLSPINLOCK_t locklist16tx = LCLSPINLOCK_INIT;

static RAMBIGDTCM LIST_HEAD2 voicesready32tx;	// буферы, предназначенные для выдачи на IF DAC
static RAMBIGDTCM LIST_HEAD2 voicesfree32tx;
static RAMBIGDTCM LCLSPINLOCK_t locklist32tx = LCLSPINLOCK_INIT;

static RAMBIGDTCM LIST_HEAD2 voicesfree32rx;
static RAMBIGDTCM LCLSPINLOCK_t locklist32rx = LCLSPINLOCK_INIT;

#if 1
static RAMBIGDTCM LIST_HEAD2 speexfree16;		// Свободные буферы
static RAMBIGDTCM LIST_HEAD2 speexready16;	// Буферы для обработки speex
static RAMBIGDTCM LCLSPINLOCK_t speexlock = LCLSPINLOCK_INIT;
#endif

#if WITHUSBHW && WITHUSBUAC

static volatile uint_fast8_t uacinalt = UACINALT_NONE;		/* выбор альтернативной конфигурации для UAC IN interface */
static volatile uint_fast8_t uacinrtsalt = UACINRTSALT_NONE;		/* выбор альтернативной конфигурации для RTS UAC IN interface */
static volatile uint_fast8_t uacoutalt;

#else /* WITHUSBHW && WITHUSBUAC */

static const uint_fast8_t uacinalt = 0;		/* выбор альтернативной конфигурации для UAC IN interface */
static const uint_fast8_t uacinrtsalt = 0;		/* выбор альтернативной конфигурации для RTS UAC IN interface */
static const uint_fast8_t uacoutalt = 0;

#endif /* WITHUSBHW && WITHUSBUAC */


static void savesampleout16stereo(int_fast32_t ch0, int_fast32_t ch1);
static void savesampleout16stereo_float(void * ctx, FLOAT_t ch0, FLOAT_t ch1);

#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)

// USB AUDIO IN
typedef ALIGNX_BEGIN struct uacin48_tag
{
	LIST_ENTRY item;	// layout should be same in uacin48_t, voice96rts_t and voice192rts_t
	uint_fast8_t tag;	// layout should be same in uacin48_t, voice96rts_t and voice192rts_t
	void * tag2;
	ALIGNX_BEGIN union
	{
		uint8_t buff [UACIN_AUDIO48_DATASIZE_DMAC];
		uint8_t filler [EP_align(UACIN_AUDIO48_DATASIZE_DMAC, DCACHEROWSIZE)];
	} u ALIGNX_END;		// спектр, 2*24*192 kS/S
	void * tag3;
} ALIGNX_END uacin48_t;

int_fast32_t buffers_dmabufferuacin48cachesize(void)
{
	return EP_align(UACIN_AUDIO48_DATASIZE_DMAC, DCACHEROWSIZE);
}

static RAMBIGDTCM LIST_HEAD2 uacin48free;
static RAMBIGDTCM LIST_HEAD2 uacin48ready;	// Буферы для записи в вудиоканал USB к компьютер 2*16*24 kS/S
static RAMBIGDTCM LCLSPINLOCK_t locklistuacin48 = LCLSPINLOCK_INIT;

#endif /* WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) */

#if WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE) && 0

// USB AUDIO OUT
typedef ALIGNX_BEGIN struct uacout48_tag
{
	LIST_ENTRY item;
	//uint_fast8_t tag;
	void * tag2;
	ALIGNX_BEGIN union
	{
		uint8_t buff [UACOUT_AUDIO48_DATASIZE_DMAC];
		uint8_t filler [EP_align(UACOUT_AUDIO48_DATASIZE_DMAC, DCACHEROWSIZE)];
	} u ALIGNX_END;
	void * tag3;
} ALIGNX_END uacout48_t;

int_fast32_t buffers_dmabufferuacout48cachesize(void)
{
	return EP_align(UACOUT_AUDIO48_DATASIZE_DMAC, DCACHEROWSIZE);
}

static RAMBIGDTCM LIST_HEAD2 uacout48free;
static RAMBIGDTCM LIST_HEAD2 uacout48ready;	// Буферы для записи в вудиоканал USB к компьютер 2*16*24 kS/S
static RAMBIGDTCM LCLSPINLOCK_t locklistuacout48 = LCLSPINLOCK_INIT;

#endif /* WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE) */

#if WITHRTS192

	typedef ALIGNX_BEGIN struct voices192rts
	{
		LIST_ENTRY item;	// layout should be same in uacin48_t, voice96rts_t and voice192rts_t
		uint_fast8_t tag;	// layout should be same in uacin48_t, voice96rts_t and voice192rts_t
		void * tag2;
		ALIGNX_BEGIN union
		{
			uint8_t buff [UACIN_RTS192_DATASIZE_DMAC];
			uint8_t	filler [EP_align(UACIN_RTS192_DATASIZE_DMAC, DCACHEROWSIZE)];
		} u ALIGNX_END;
		void * tag3;
	} ALIGNX_END voice192rts_t;

	int_fast32_t buffers_dmabuffer192rtscachesize(void)
	{
		return EP_align(UACIN_RTS192_DATASIZE_DMAC, DCACHEROWSIZE);
	}

	static RAMBIGDTCM LIST_HEAD2 voicesfree192rts;
	static RAMBIGDTCM LIST_HEAD2 uacin192rts;	// Буферы для записи в вудиоканал USB к компьютеру спектра, 2*32*192 kS/S

	static RAMBIGDTCM LCLSPINLOCK_t locklistrts = LCLSPINLOCK_INIT;
	static subscribeint32_t uacinrtssubscribe;

#elif WITHRTS96

	typedef ALIGNX_BEGIN struct voices96rts
	{
		LIST_ENTRY item;	// layout should be same in uacin48_t, voice96rts_t and voice192rts_t
		uint_fast8_t tag;	// layout should be same in uacin48_t, voice96rts_t and voice192rts_t
		void * tag2;
		ALIGNX_BEGIN union
		{
			uint8_t buff [UACIN_RTS96_DATASIZE_DMAC];
			uint8_t	 filler [EP_align(UACIN_RTS96_DATASIZE_DMAC, DCACHEROWSIZE)];
		} u ALIGNX_END;
		void * tag3;
	} ALIGNX_END voice96rts_t;

	int_fast32_t buffers_dmabuffer96rtscachesize(void)
	{
		return EP_align(UACIN_RTS96_DATASIZE_DMAC, DCACHEROWSIZE);
	}

	static RAMBIGDTCM LIST_HEAD2 uacinrts96free;
	static RAMBIGDTCM LIST_HEAD2 uacinrts96ready;	// Буферы для записи в вудиоканал USB к компьютер спектра, 2*32*192 kS/S

	static RAMBIGDTCM LCLSPINLOCK_t locklistrts = LCLSPINLOCK_INIT;
	static subscribeint32_t uacinrtssubscribe;

#endif /* WITHRTS96 */


#endif /* WITHINTEGRATEDDSP */

#if WITHUSEAUDIOREC

typedef ALIGNX_BEGIN struct recordswav48
{
	ALIGNX_BEGIN int16_t buff [AUDIORECBUFFSIZE16] ALIGNX_END;
	LIST_ENTRY item;
	unsigned startdata;	// data start
	unsigned topdata;	// index after last element
} ALIGNX_END recordswav48_t;

static RAMBIGDTCM LIST_HEAD2 recordswav48free;		// Свободные буферы
static RAMBIGDTCM LIST_HEAD2 recordswav48ready;	// Буферы для записи на SD CARD
static RAMBIGDTCM LCLSPINLOCK_t lockwav48 = LCLSPINLOCK_INIT;

#endif /* WITHUSEAUDIOREC */

#if WITHMODEM


typedef struct modems8
{
	LIST_ENTRY item;
	size_t length;
	uint8_t buff [MODEMBUFFERSIZE8];
} modems8_t;

static RAMBIGDTCM LIST_HEAD2 modemsfree8;		// Свободные буферы
static RAMBIGDTCM LIST_HEAD2 modemsrx8;	// Буферы с принятымти через модем данными
//static LIST_ENTRY modemstx8;	// Буферы с данными для передачи через модем

#endif /* WITHMODEM */

#if 0
/* Cообщения от уровня обработчиков прерываний к user-level функциям. */

typedef struct message
{
	void * tag2;
	volatile VLIST_ENTRY item;
	uint8_t type;
	uint8_t data [MSGBUFFERSIZE8];
	void * tag3;
} message_t;

static volatile RAMBIGDTCM VLIST_ENTRY msgsfree8;		// Свободные буферы
static volatile RAMBIGDTCM VLIST_ENTRY msgsready8;		// Заполненные - готовые к обработке
static RAMBIGDTCM LCLSPINLOCK_t locklistmsg8 = LCLSPINLOCK_INIT;
#endif

#if WITHBUFFERSDEBUG

static unsigned n1, n1wfm, n2, n3, n4, n5, n6, n7;
static unsigned e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, purge16;
static unsigned nbadd, nbdel, nbzero, nbnorm;

static unsigned debugcount_ms10;	// с точностью 0.1 ms

static unsigned debugcount_uacout;
static unsigned debugcount_mikeadc;
static unsigned debugcount_phonesdac;
static unsigned debugcount_uacinrts;
static unsigned debugcount_uacin;
static unsigned debugcount_rx32adc;
static unsigned debugcount_rx32wfm;
static unsigned debugcount_tx32dac;
	
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

/* Запрещение прерывений убрано для уменьшения влияения на работу системы. Иногда может ошибаться в расчете скорости */
static unsigned 
getresetval(volatile unsigned * p)
{
	unsigned v;
	v = * p;
	* p = 0;
	return v;
}

#endif /* WITHBUFFERSDEBUG */

void buffers_diagnostics(void)
{
#if 1 && WITHDEBUG && WITHINTEGRATEDDSP && WITHBUFFERSDEBUG && ! WITHBUFFERSSMALLDEBUG

	LIST2PRINT(speexfree16);
	LIST2PRINT(voicesfree32tx);
	LIST2PRINT(voicesfree16rx);
	LIST2PRINT(voicesfree16tx);
	LIST2PRINT(speexready16);
	LIST2PRINT(voicesready32tx);
	PRINTF(PSTR("\n"));
	LIST3PRINT(voicesmike16rx);
	LIST3PRINT(voicesphones16tx);
	LIST3PRINT(voicesmoni16tx);

	#if WITHUSBUACIN
		#if WITHRTS192
			LIST2PRINT(voicesfree192rts);
			LIST2PRINT(uacin192rts);
		#elif WITHRTS96
			LIST2PRINT(uacinrts96free);
			LIST2PRINT(uacinrts96ready);
		#endif
		LIST2PRINT(uacin48free);
		LIST2PRINT(uacin48ready);
	#endif /* WITHUSBUACIN */
			PRINTF(PSTR("\n"));
	#if WITHUSBUACOUT
		LIST3PRINT(voicesusb16rx);
		LIST3PRINT(resample16rx);
		PRINTF(PSTR(" (NORMAL=%d), uacoutalt=%d, add=%u, del=%u, zero=%u, norm=%u "), RESAMPLE16NORMAL, uacoutalt, nbadd, nbdel, nbzero, nbnorm);
	#endif /* WITHUSBUACOUT */

		
	PRINTF(PSTR("\n"));

#endif

#if 1 && WITHDEBUG && WITHINTEGRATEDDSP && WITHBUFFERSDEBUG
#if WITHBUFFERSSMALLDEBUG
	const unsigned ms10 = getresetval(& debugcount_ms10);
	const unsigned mikeadc = getresetval(& debugcount_mikeadc);
	const unsigned phonesdac = getresetval(& debugcount_phonesdac);
	const unsigned rx32adc = getresetval(& debugcount_rx32adc);
	const unsigned tx32dac = getresetval(& debugcount_tx32dac);
	PRINTF(PSTR("FREQ: mikeadc=%u, phonesdac=%u, rx32adc=%u, tx32dac=%u\n"),
		mikeadc * 10000 / ms10,
		phonesdac * 10000 / ms10,
		rx32adc * 10000 / ms10,
		tx32dac * 10000 / ms10
		);
#else
	PRINTF(PSTR("n1=%u n1wfm=%u n2=%u n3=%u n4=%u n5=%u n6=%u n7=%u uacinalt=%d, purge16=%u\n"), n1, n1wfm, n2, n3, n4, n5, n6, n7, uacinalt, purge16);
	PRINTF(PSTR("e1=%u e2=%u e3=%u e4=%u e5=%u e6=%u e7=%u e8=%u e9=%u e10=%u e11=%u e12=%u e13=%u\n"), e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13);

	{
		const unsigned ms10 = getresetval(& debugcount_ms10);
		const unsigned uacout = getresetval(& debugcount_uacout);
		const unsigned mikeadc = getresetval(& debugcount_mikeadc);
		const unsigned phonesdac = getresetval(& debugcount_phonesdac);
		const unsigned uacinrts = getresetval(& debugcount_uacinrts);
		const unsigned rx32adc = getresetval(& debugcount_rx32adc);
		const unsigned rx32wfm = getresetval(& debugcount_rx32wfm);
		const unsigned tx32dac = getresetval(& debugcount_tx32dac);
		const unsigned uacin = getresetval(& debugcount_uacin);

		PRINTF(PSTR("FREQ: uacout=%u, uacin=%u, uacinrts=%u, mikeadc=%u, phonesdac=%u, rx32adc=%u, rx32wfm=%u, tx32dac=%u\n"),
			uacout * 10000 / ms10, 
			uacin * 10000 / ms10, 
			uacinrts * 10000 / ms10,
			mikeadc * 10000 / ms10, 
			phonesdac * 10000 / ms10, 
			rx32adc * 10000 / ms10, 
			rx32wfm * 10000 / ms10, 
			tx32dac * 10000 / ms10
			);
	}
#endif /* WITHBUFFERSSMALLDEBUG */
#endif
}


#if WITHINTEGRATEDDSP

#if 1
typedef ALIGNX_BEGIN struct denoise16
{
	LIST_ENTRY item;
	ALIGNX_BEGIN speexel_t buff [NTRX * FIRBUFSIZE] ALIGNX_END;
} ALIGNX_END denoise16_t;


// Буферы с принятымти от обработчиков прерываний сообщениями
uint_fast8_t takespeexready(speexel_t * * dest)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& speexlock);
	if (! IsListEmpty2(& speexready16))
	{
		const PLIST_ENTRY t = RemoveTailList2(& speexready16);
		LCLSPIN_UNLOCK(& speexlock);
		LowerIrql(oldIrql);
		denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, item);
		* dest = p->buff;
		return 1;
	}
	LCLSPIN_UNLOCK(& speexlock);
	LowerIrql(oldIrql);
	return 0;
}

// Освобождение обработанного буфера
void releasespeexbuffer(speexel_t * t)
{
	denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, buff);
	IRQL_t oldIrql;
	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& speexlock);

	InsertHeadList2(& speexfree16, & p->item);

	LCLSPIN_UNLOCK(& speexlock);
	LowerIrql(oldIrql);
}

// Сохранние буфера
void savespeexbuffer(speexel_t * t)
{
	denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, buff);
	IRQL_t oldIrql;
	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& speexlock);

	InsertHeadList2(& speexready16, & p->item);

	LCLSPIN_UNLOCK(& speexlock);
	LowerIrql(oldIrql);
}


speexel_t * allocatespeexbuffer(void)
{
	LCLSPIN_LOCK(& speexlock);
	if (! IsListEmpty2(& speexfree16))
	{
		const PLIST_ENTRY t = RemoveTailList2(& speexfree16);
		LCLSPIN_UNLOCK(& speexlock);
		denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, item);
		return p->buff;
	}
#if WITHBUFFERSDEBUG
	++ e7;
#endif /* WITHBUFFERSDEBUG */
	//PRINTF(PSTR("allocatespeexbuffer() failure\n"));
	if (! IsListEmpty2(& speexready16))
	{
		const PLIST_ENTRY t = RemoveTailList2(& speexready16);
		LCLSPIN_UNLOCK(& speexlock);
		denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, item);
		unsigned i;
		for (i = 0; i < FIRBUFSIZE; ++ i)
		{
			deliveryfloat(& speexoutfloat, 0, 0);	// to AUDIO codec
		}
		return p->buff;
	}
	LCLSPIN_UNLOCK(& speexlock);
	PRINTF(PSTR("allocatespeexbuffer() failure\n"));
	ASSERT(0);
	for (;;)
		;
	return 0;
}
#endif
// in realime context
static void
savesampleout16tospeex(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	static speexel_t * buff = NULL;
	static unsigned n;

	if (buff == NULL)
	{
		buff = allocatespeexbuffer();
		n = 0;
	}

	buff [n] = ch0;		// sample value
#if WITHUSEDUALWATCH
	buff [n + FIRBUFSIZE] = ch1;	// sample value
#endif /* WITHUSEDUALWATCH */

	n += 1;

	if (n >= FIRBUFSIZE)
	{
		savespeexbuffer(buff);
		buff = NULL;
	}
}

deliverylist_t rtstargetsint;	// выход обработчика DMA приема от FPGA
deliverylist_t speexoutfloat;	// выход speex и фильтра
deliverylist_t afdemodoutfloat;	// выход приемника

#endif /* WITHINTEGRATEDDSP */

#if 0
/* Cообщения от уровня обработчиков прерываний к user-level функциям. */

// Буферы с принятымти от обработчиков прерываний сообщениями
uint_fast8_t takemsgready(uint8_t * * dest)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& locklistmsg8);
	if (! IsListEmpty(& msgsready8))
	{
		PVLIST_ENTRY t = RemoveTailVList(& msgsready8);
		LCLSPIN_UNLOCK(& locklistmsg8);
		LowerIrql(oldIrql);
		message_t * const p = CONTAINING_RECORD(t, message_t, item);
		* dest = p->data;
		ASSERT(p->type != MSGT_EMPTY);
		return p->type;
	}
	LCLSPIN_UNLOCK(& locklistmsg8);
	LowerIrql(oldIrql);
	return MSGT_EMPTY;
}


// Освобождение обработанного буфера сообщения
void releasemsgbuffer(uint8_t * dest)
{
	message_t * const p = CONTAINING_RECORD(dest, message_t, data);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	IRQL_t oldIrql;

	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& locklistmsg8);
	InsertHeadVList(& msgsfree8, & p->item);
	LCLSPIN_UNLOCK(& locklistmsg8);
	LowerIrql(oldIrql);
}

// Буфер для формирования сообщения
size_t takemsgbufferfree(uint8_t * * dest)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& locklistmsg8);
	if (! IsListEmpty(& msgsfree8))
	{
		PVLIST_ENTRY t = RemoveTailVList(& msgsfree8);
		LCLSPIN_UNLOCK(& locklistmsg8);
		LowerIrql(oldIrql);
		message_t * const p = CONTAINING_RECORD(t, message_t, item);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		* dest = p->data;
		return (MSGBUFFERSIZE8 * sizeof p->data [0]);
	}
	LCLSPIN_UNLOCK(& locklistmsg8);
	LowerIrql(oldIrql);
	return 0;
}

// поместить сообщение в очередь к исполнению
void placesemsgbuffer(uint_fast8_t type, uint8_t * dest)
{
	ASSERT(type != MSGT_EMPTY);
	message_t * p = CONTAINING_RECORD(dest, message_t, data);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	p->type = type;
	IRQL_t oldIrql;

	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& locklistmsg8);
	InsertHeadVList(& msgsready8, & p->item);
	LCLSPIN_UNLOCK(& locklistmsg8);
	LowerIrql(oldIrql);
}
#endif

#if WITHINTEGRATEDDSP

// Сохранить звук на звуковой выход трансивера
static RAMFUNC void buffers_tophones16tx(voice16tx_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	LCLSPIN_LOCK(& locklist16tx);
	InsertHeadList3(& voicesphones16tx, & p->item, 0);
	LCLSPIN_UNLOCK(& locklist16tx);
}

// Сохранить звук для самоконтроля на звуковой выход трансивера
static RAMFUNC void buffers_tomoni16tx(voice16tx_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	LCLSPIN_LOCK(& locklist16tx);
	InsertHeadList3(& voicesmoni16tx, & p->item, 0);
	LCLSPIN_UNLOCK(& locklist16tx);
}

// Сохранить звук в никуда...
static RAMFUNC void buffers_tonull16rx(voice16rx_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	LCLSPIN_LOCK(& locklist16rx);
	InsertHeadList2(& voicesfree16rx, & p->item);
	LCLSPIN_UNLOCK(& locklist16rx);
}

// Сохранить звук в никуда...
static RAMFUNC void buffers_tonull16tx(voice16tx_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	LCLSPIN_LOCK(& locklist16tx);
	InsertHeadList2(& voicesfree16tx, & p->item);
	LCLSPIN_UNLOCK(& locklist16tx);
}

// +++ Коммутация потоков аудиоданных
// первый канал выхода приёмника - для прослушивания
static RAMFUNC void
buffers_savefromrxout16tx(voice16tx_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	buffers_tophones16tx(p);
}


static RAMFUNC void
buffers_savefrommoni16tx(voice16tx_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	buffers_tomoni16tx(p);
}


#if WITHUSBUAC

// Сохранить звук от несинхронного источника - USB - для последующего ресэмплинга
RAMFUNC static void buffers_savetoresampling16rx(voice16rx_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	LCLSPIN_LOCK(& locklist16rx);

	if (uacoutalt == 0)
	{
		// Поместить в свободные
		InsertHeadList2(& voicesfree16rx, & p->item);

		// Очистить очередь принятых от USB UAC
//		while (GetCountList3(& resample16rx) != 0)
//		{
//			const PLIST_ENTRY t = RemoveTailList3(& resample16rx);
//			InsertHeadList2(& voicesfree16rx, t);
//		}
	}
	else
	{
		// Поместить в очередь принятых от USB UAC
		InsertHeadList3(& resample16rx, & p->item, 0);

		while (GetCountList3(& resample16rx) > (RESAMPLE16NORMAL + SKIPPEDBLOCKS))
		{
			// Из-за ошибок с асинхронным аудио пришлось добавить ограничение на размер этой очереди
			const PLIST_ENTRY t = RemoveTailList3(& resample16rx);
			InsertHeadList2(& voicesfree16rx, t);

		#if WITHBUFFERSDEBUG
			++ e6;
		#endif /* WITHBUFFERSDEBUG */
		}
	}
	LCLSPIN_UNLOCK(& locklist16rx);
}


#endif /* WITHUSBUAC */


// приняли данные от USB AUDIO
static RAMFUNC void
buffers_savefromuacout(voice16rx_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_uacout += DMABUFFSIZE16RX / DMABUFFSTEP16RX;	// в буфере пары сэмплов по два байта
#endif /* WITHBUFFERSDEBUG */

#if WITHUSBUAC
	buffers_savetoresampling16rx(p);
#else /* WITHUSBUAC */
	buffers_tonull16rx(p);
#endif /* WITHUSBUAC */
}

//////////////////////////////////////////
// Поэлементное чтение буфера AF ADC

// 16 bit, signed
// в паре значений, возвращаемых данной функцией, vi получает значение от микрофона. vq зарезервированно для работы ISB (две независимых боковых)
// При отсутствии данных в очереди - возвращаем 0
// TODO: переделать на denoise16_t
RAMFUNC uint_fast8_t getsampmlemike(FLOAT32P_t * v)
{
	enum { L, R };
	static voice16rx_t * p = NULL;
	static unsigned pos = 0;	// позиция по выходному количеству

	if (p == NULL)
	{
		LCLSPIN_LOCK(& locklist16rx);
		if (GetReadyList3(& voicesmike16rx))
		{
			PLIST_ENTRY t = RemoveTailList3(& voicesmike16rx);
			LCLSPIN_UNLOCK(& locklist16rx);
			p = CONTAINING_RECORD(t, voice16rx_t, item);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			pos = 0;
		}
		else
		{
			// Микрофонный кодек ещё не успел начать работать - возвращаем 0.
			LCLSPIN_UNLOCK(& locklist16rx);
			return 0;
		}
	}
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	const FLOAT_t sample = adpt_input(& afcodecrx, p->rbuff [pos * DMABUFFSTEP16RX + DMABUFF16RX_MIKE]);	// микрофон или левый канал

	// Использование данных.
	v->ivqv [L] = sample;
	v->ivqv [R] = sample;

	if (++ pos >= CNT16RX)
	{
		buffers_tonull16rx(p);
		p = NULL;
	}
	return 1;	
}

// 16 bit, signed
// в паре значений, возвращаемых данной функцией, vi получает значение от микрофона. vq зарезервированно для работы ISB (две независимых боковых)
// При отсутствии данных в очереди - возвращаем 0
// TODO: переделать на denoise16_t
RAMFUNC uint_fast8_t getsampmleusb(FLOAT32P_t * v)
{
	enum { L, R };
	static voice16rx_t * p = NULL;
	static unsigned pos = 0;	// позиция по выходному количеству

	if (p == NULL)
	{
		LCLSPIN_LOCK(& locklist16rx);
		if (GetReadyList3(& voicesusb16rx))
		{
			PLIST_ENTRY t = RemoveTailList3(& voicesusb16rx);
			LCLSPIN_UNLOCK(& locklist16rx);
			p = CONTAINING_RECORD(t, voice16rx_t, item);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			pos = 0;
#if WITHBUFFERSDEBUG
			++ n7;
#endif /* WITHBUFFERSDEBUG */
		}
		else
		{
			// resampler не успел начать работать - возвращаем 0.
			LCLSPIN_UNLOCK(& locklist16rx);
			return 0;
		}
	}
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	// Использование данных.
	v->ivqv [L] = adpt_input(& afcodecrx, p->rbuff [pos * DMABUFFSTEP16RX + 0]);	// левый канал
	v->ivqv [R] = adpt_input(& afcodecrx, p->rbuff [pos * DMABUFFSTEP16RX + 1]);	// правый канал

	if (++ pos >= CNT16RX)
	{
		buffers_tonull16rx(p);
		p = NULL;
	}
	return 1;
}

// звук для самоконтроля
void savemonistereo(FLOAT_t ch0, FLOAT_t ch1)
{
	enum { L, R };
	// если есть инициализированный канал для выдачи звука
	static voice16tx_t * p = NULL;
	static unsigned n;

	if (p == NULL)
	{
		uintptr_t addr = allocate_dmabuffer16tx();
		p = CONTAINING_RECORD(addr, voice16tx_t, tbuff);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		n = 0;
	}

	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	p->tbuff [n * DMABUFFSTEP16TX + DMABUFF16TX_LEFT] = adpt_outputexact(& afcodectx, ch0);	// sample value
	p->tbuff [n * DMABUFFSTEP16TX + DMABUFF16TX_RIGHT] = adpt_outputexact(& afcodectx, ch1);	// sample value

	if (++ n >= CNT16TX)
	{
		buffers_savefrommoni16tx(p);
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
	debugcount_uacinrts += sizeof p->buff / sizeof p->buff [0] / DMABUFFSTEP192RTS;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */

	LCLSPIN_LOCK(& locklistrts);
	InsertHeadList2(& uacin192rts, & p->item);
	LCLSPIN_UNLOCK(& locklistrts);

	refreshDMA_uacinrts192();		// если DMA  остановлено - начать обмен
}

static void buffers_savetonull192rts(voice192rts_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	LCLSPIN_LOCK(& locklistrts);
	InsertHeadList2(& voicesfree192rts, & p->item);
	LCLSPIN_UNLOCK(& locklistrts);
}


#endif /* WITHRTS192 && WITHUSBHW && WITHUSBUAC */

#if WITHRTS96 && WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE)

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
buffers_savetouacinrts96(voice96rts_t * p)
{
	ASSERT(p->tag == BUFFTAG_RTS96);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_uacinrts += sizeof p->u.buff / sizeof p->u.buff [0] / UACIN_RTS96_SAMPLEBYTES / 2;	// в буфере пары сэмплов по три байта
#endif /* WITHBUFFERSDEBUG */
	
	LCLSPIN_LOCK(& locklistrts);
	InsertHeadList2(& uacinrts96ready, & p->item);
	LCLSPIN_UNLOCK(& locklistrts);

	refreshDMA_uacinrts96();		// если DMA  остановлено - начать обмен
}

static void buffers_savetonull96rts(voice96rts_t * p)
{
	ASSERT(p->tag == BUFFTAG_RTS96);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	LCLSPIN_LOCK(& locklistrts);
	InsertHeadList2(& uacinrts96free, & p->item);
	LCLSPIN_UNLOCK(& locklistrts);
}

#else

static uint_fast8_t isrts96(void)
{
	return 0;
}

#endif /*  WITHRTS96 && WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE) */

#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)
// Сохранить буфер сэмплов для передачи в компьютер
static RAMFUNC void
buffers_savetouacin(uacin48_t * p)
{
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_uacin += sizeof p->u.buff / sizeof p->u.buff [0] / (UACIN_FMT_CHANNELS_AUDIO48 * UACIN_AUDIO48_SAMPLEBYTES);	// в буфере пары сэмплов по три байта
#endif /* WITHBUFFERSDEBUG */
	LCLSPIN_LOCK(& locklistuacin48);
	InsertHeadList2(& uacin48ready, & p->item);
	LCLSPIN_UNLOCK(& locklistuacin48);

	refreshDMA_uacin48();		// если DMA  остановлено - начать обмен
}
#endif /* WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) */

#endif /* WITHUSBUAC */

enum { NPARTS = 3 };

typedef struct rsmpl_tag
{
	lclspinlock_t * lock;//locklist16rx
	aubufv_t * pdata;// = NULL;
	PLIST_ENTRY titem;
	uint_fast8_t part;// = 0;
	aubufv_t * datas [NPARTS];// = { NULL, NULL };		// начальный адрес пары сэмплов во входном буфере
	unsigned sizes [NPARTS];// = { 0, 0 };			// количество сэмплов во входном буфере
	unsigned skipsense;// = SKIPPEDBLOCKS;
	unsigned bufsize;
	unsigned bufstep;
	aubufv_t addsample [2];
	LIST_HEAD3 * rx;//resample16rx
	LIST_HEAD2 * freelist;//resample16rx
	LIST_HEAD3 * outlist;//voicesusb16rx
	uintptr_t (*getoutputdmabuff)(void); // получить выходной буфер
	aubufv_t * (* getdataptr)(PLIST_ENTRY pitem);
	aubufv_t (* getaverage)(aubufv_t v1, aubufv_t v2);
} rsmpl_t;

static aubufv_t * uacin48rsmgetdata(PLIST_ENTRY pitem)
{
	voice16rx_t * const p = CONTAINING_RECORD(pitem, voice16rx_t, item);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	return p->rbuff;
}

static aubufv_t uacin48average(aubufv_t v1, aubufv_t v2)
{
	return ((aufastbufv2x_t) v1 + v2) / 2;
}

static rsmpl_t uacout48rsmpl =
{
		.lock = & locklist16rx,
		.skipsense = SKIPPEDBLOCKS,
		.pdata = NULL,
		.bufsize = DMABUFFSIZE16RX,
		.bufstep = DMABUFFSTEP16RX,
		.rx = & resample16rx,
		.freelist = & voicesfree16rx,
		.outlist = & voicesusb16rx,
		.getoutputdmabuff = allocate_dmabuffer16rx,
		.getdataptr = uacin48rsmgetdata,
		.getaverage = uacin48average,
};

static rsmpl_t uacin48rsmpl;
static rsmpl_t uacinrts96rsmpl;
static rsmpl_t uacinrts192rsmpl;

#if WITHUSBUAC

// получает массив сэмплов
// возвращает количество полученых сэмплов
// Выборка из очереди resample16rx
static RAMFUNC unsigned getsamplemsuacout(
	rsmpl_t * rsmpl,
	aubufv_t * buff,	// текущая позиция в целевом буфере
	unsigned size		// количество оставшихся одиночных сэмплов
	)
{

	LCLSPIN_LOCK(rsmpl->lock);
	if (rsmpl->pdata == NULL)
	{
		if (GetReadyList3(rsmpl->rx) == 0)
		{
#if WITHBUFFERSDEBUG
			++ nbzero;
#endif /* WITHBUFFERSDEBUG */
			// Микрофонный кодек ещё не успел начать работать - возвращаем 0.
			LCLSPIN_UNLOCK(rsmpl->lock);
			memset(buff, 0x00, size * sizeof (* buff));	// тишина
			return size;	// ноль нельзя возвращать - зацикливается проуедура ресэмплинга
		}
		else
		{
			rsmpl->titem = RemoveTailList3(rsmpl->rx);
			rsmpl->pdata = rsmpl->getdataptr(rsmpl->titem);
			if (GetReadyList3(rsmpl->rx) == 0)	// готовность пропала
				rsmpl->skipsense = SKIPPEDBLOCKS;
			const uint_fast8_t valid = GetReadyList3(rsmpl->rx) && rsmpl->skipsense == 0;
			rsmpl->skipsense = (rsmpl->skipsense == 0) ? SKIPPEDBLOCKS : rsmpl->skipsense - 1;

			const unsigned LOW = RESAMPLE16NORMAL - (SKIPPEDBLOCKS / 2);
			const unsigned HIGH = RESAMPLE16NORMAL + (SKIPPEDBLOCKS / 2);

			if (valid && GetCountList3(rsmpl->rx) <= LOW)
			{
				LCLSPIN_UNLOCK(rsmpl->lock);
				// добавляется один сэмпл к выходному потоку раз в SKIPPEDBLOCKS блоков
#if WITHBUFFERSDEBUG
				++ nbadd;
#endif /* WITHBUFFERSDEBUG */

#if 0
				rsmpl->part = NPARTS - 2;
				rsmpl->datas [part + 0] = & rsmpl->pdata [0];	// дублируем первый сэмпл
				rsmpl->sizes [part + 0] = rsmpl->bufstep;
				rsmpl->datas [part + 1] = & rsmpl->pdata [0];
				rsmpl->sizes [part + 1] = rsmpl->bufsize;
#else
				unsigned HALF = rsmpl->bufsize / 2;
				// значения как среднее арифметическое сэмплов, между которыми вставляем дополнительный.
				rsmpl->addsample [0] = rsmpl->getaverage(rsmpl->pdata [HALF - rsmpl->bufstep + 0], rsmpl->pdata [HALF + 0]);	// Left
				rsmpl->addsample [1] = rsmpl->getaverage(rsmpl->pdata [HALF - rsmpl->bufstep + 1], rsmpl->pdata [HALF + 1]);	// Right

				rsmpl->part = NPARTS - 3;
				rsmpl->datas [0] = & rsmpl->pdata [0];		// часть перед вставкой
				rsmpl->sizes [0] = HALF;
				rsmpl->datas [1] = & rsmpl->addsample [0];	// вставляемые данные
				rsmpl->sizes [1] = rsmpl->bufstep;
				rsmpl->datas [2] = & rsmpl->pdata [HALF];	// часть после вставки
				rsmpl->sizes [2] = rsmpl->bufsize - HALF;
#endif
			}
			else if (valid && GetCountList3(rsmpl->rx) >= HIGH)
			{
				LCLSPIN_UNLOCK(rsmpl->lock);
#if WITHBUFFERSDEBUG
				++ nbdel;
#endif /* WITHBUFFERSDEBUG */
				// убирается один сэмпл из выходного потока раз в SKIPPEDBLOCKS блоков
				rsmpl->part = NPARTS - 1;
				rsmpl->datas [rsmpl->part] = & rsmpl->pdata [rsmpl->bufstep];	// пропускаем первый сэмпл
				rsmpl->sizes [rsmpl->part] = rsmpl->bufsize - rsmpl->bufstep;
			}
			else
			{
				LCLSPIN_UNLOCK(rsmpl->lock);
				// Ресэмплинг не требуется или нет запаса входных данных
				rsmpl->part = NPARTS - 1;
				rsmpl->datas [rsmpl->part] = & rsmpl->pdata [0];
				rsmpl->sizes [rsmpl->part] = rsmpl->bufsize;
#if WITHBUFFERSDEBUG
			++ nbnorm;
#endif /* WITHBUFFERSDEBUG */
			}
		}
	}
	else
	{
		/* нужное количество зон для вывода уже подготовленно */
		LCLSPIN_UNLOCK(rsmpl->lock);
	}

	const unsigned chunk = ulmin32(rsmpl->sizes [rsmpl->part], size);

	memcpy(buff, rsmpl->datas [rsmpl->part], chunk * sizeof (* buff));

	rsmpl->datas [rsmpl->part] += chunk;
	if ((rsmpl->sizes [rsmpl->part] -= chunk) == 0 && ++ rsmpl->part >= NPARTS)
	{
		// освобождаем ранее полученый от UAC буфер
		LCLSPIN_LOCK(rsmpl->lock);
		InsertHeadList2(rsmpl->freelist, rsmpl->titem);
		LCLSPIN_UNLOCK(rsmpl->lock);
		rsmpl->pdata = NULL;
	}
	return chunk;
}

// формирование одного буфера синхронного потока из N несинхронного
static RAMFUNC void buffer_resample(rsmpl_t * rsmpl)
{
	const uintptr_t addr = rsmpl->getoutputdmabuff();//allocate_dmabuffer16rx();	// выходной буфер
	voice16rx_t * const p = CONTAINING_RECORD(addr, voice16rx_t, rbuff);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	//
	// выполнение ресэмплинга
	unsigned pos;
	for (pos = 0; pos < rsmpl->bufsize; )
	{
		pos += getsamplemsuacout(rsmpl, & p->rbuff [pos], rsmpl->bufsize - pos);	// Выборка из очеререди rsmpl->rx
	}

	// направление получившегося буфера получателю.
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	LCLSPIN_LOCK(rsmpl->lock);
	InsertHeadList3(rsmpl->outlist, & p->item, 0);
	LCLSPIN_UNLOCK(rsmpl->lock);
}

static void buffers_resample(void)
{
#if WITHUSBUACOUT
	buffer_resample(& uacout48rsmpl);
#endif /* WITHUSBUACOUT */
#if WITHUSBUACIN
	//buffer_resample(& uacin48rsmpl);
#if WITHUSBUACIN2
	//buffer_resample(& uacinrts96rsmpl);
#endif /* WITHUSBUACIN2 */
#endif /* WITHUSBUACIN */
}

// вызывается из какой-либо функции обслуживания I2S каналов (все синхронны).
// Параметр - количество сэмплов (стерео пар или квадратур) в обмене этого обработчика.
void RAMFUNC buffers_resampleuacin(unsigned nsamples)
{
	static unsigned nrx = 0;
	static unsigned ntx = 0;
	nrx += nsamples;
	ntx += nsamples;
	while (nrx >= CNT16RX)
	{
		buffers_resample();		// формирование одного буфера синхронного потока из N несинхронного
		nrx -= CNT16RX;
	}

	// Часть, необходимая в конфигурациях без канала выдачи на кодек
	while (ntx >= CNT16TX)
	{
#if ! WITHI2S2HW && ! CPUSTYLE_XC7Z && ! WITHFPGAPIPE_CODEC1
		release_dmabuffer16tx(getfilled_dmabuffer16txphones());
#endif /* ! WITHI2S2HW && ! CPUSTYLE_XC7Z */
		ntx -= CNT16TX;
	}
}

#else /* WITHUSBUAC */

// вызывается из какой-либо функции обслуживания I2S каналов (все синхронны).
// Параметр - количество сэмплов (стерео пар или квадратур) в обмене этого обработчика.
void RAMFUNC buffers_resampleuacin(unsigned nsamples)
{
	static unsigned n = 0;
	n += nsamples;
	while (n >= CNT16TX)
	{
#if ! WITHI2S2HW && ! CPUSTYLE_XC7Z
		release_dmabuffer16tx(getfilled_dmabuffer16txphones());
#endif /* ! WITHI2S2HW && ! CPUSTYLE_XC7Z */
		n -= CNT16TX;
	}

}

#endif /* WITHUSBUAC */

// --- Коммутация потоков аудиоданных

#if WITHUSEAUDIOREC
// Поэлементное заполнение буфера SD CARD

/* to SD CARD */
// 16 bit, signed
void RAMFUNC savesamplewav48(int_fast32_t left, int_fast32_t right)
{
	// если есть инициализированный канал для выдачи звука
	static recordswav48_t * preparerecord16 = NULL;
	static unsigned level16record;
	IRQL_t oldIrql;

	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& lockwav48);
	if (preparerecord16 == NULL)
	{
		if (! IsListEmpty2(& recordswav48free))
		{
			PLIST_ENTRY t = RemoveTailList2(& recordswav48free);
			preparerecord16 = CONTAINING_RECORD(t, recordswav48_t, item);
		}
		else
		{
			// Если нет свободных - использум самый давно подготовленный для записи буфер
			PLIST_ENTRY t = RemoveTailList2(& recordswav48ready);
			preparerecord16 = CONTAINING_RECORD(t, recordswav48_t, item);
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
		/* используется буфер целиклом */
		preparerecord16->startdata = 0;
		preparerecord16->topdata = AUDIORECBUFFSIZE16;
		InsertHeadList2(& recordswav48ready, & preparerecord16->item);
		preparerecord16 = NULL;
	}
	LCLSPIN_UNLOCK(& lockwav48);
	LowerIrql(oldIrql);
}

// user-mode function
unsigned takerecordbuffer(void * * dest)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& lockwav48);
	if (! IsListEmpty2(& recordswav48ready))
	{
		PLIST_ENTRY t = RemoveTailList2(& recordswav48ready);
		LCLSPIN_UNLOCK(& lockwav48);
		LowerIrql(oldIrql);
		recordswav48_t * const p = CONTAINING_RECORD(t, recordswav48_t, item);
		* dest = p->buff;
		return (AUDIORECBUFFSIZE16 * sizeof p->buff [0]);
	}
	LCLSPIN_UNLOCK(& lockwav48);
	LowerIrql(oldIrql);
	return 0;
}

// user-mode function
unsigned takefreerecordbuffer(void * * dest)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& lockwav48);
	if (! IsListEmpty2(& recordswav48free))
	{
		PLIST_ENTRY t = RemoveTailList2(& recordswav48free);
		LCLSPIN_UNLOCK(& lockwav48);
		LowerIrql(oldIrql);
		recordswav48_t * const p = CONTAINING_RECORD(t, recordswav48_t, item);
		* dest = p->buff;
		return (AUDIORECBUFFSIZE16 * sizeof p->buff [0]);
	}
	LCLSPIN_UNLOCK(& lockwav48);
	LowerIrql(oldIrql);
	return 0;
}

static void saveplaybuffer(void * dest, unsigned used)
{
	IRQL_t oldIrql;
	recordswav48_t * const p = CONTAINING_RECORD(dest, recordswav48_t, buff);
	p->startdata = 0;	// первый сэмпл в буфере
	p->topdata = used / sizeof p->buff [0];	// количество сэмплов

	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& lockwav48);
	InsertHeadList2(& recordswav48ready, & p->item);
	LCLSPIN_UNLOCK(& lockwav48);
	LowerIrql(oldIrql);
}

/* data to play */
unsigned savesamplesplay(
	const void * buff,
	unsigned length
	)
{
	void * p;
	unsigned size;

	size = takefreerecordbuffer(& p);

	if (size == 0)
	{
		//PRINTF("savesamplesplay: length=%u - no memory\n", length);
		return 0;
	}

	//PRINTF("savesamplesplay: length=%u\n", length);
	unsigned chunk = ulmin32(size, length);
	memcpy(p, buff, chunk);
	saveplaybuffer(p, chunk);
	return chunk;
}

void releaserecordbuffer(void * dest)
{
	recordswav48_t * const p = CONTAINING_RECORD(dest, recordswav48_t, buff);
	IRQL_t oldIrql;
	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& lockwav48);
	InsertHeadList2(& recordswav48free, & p->item);
	LCLSPIN_UNLOCK(& lockwav48);
	LowerIrql(oldIrql);
}

void saverecordbuffer(void * dest)
{
	recordswav48_t * const p = CONTAINING_RECORD(dest, recordswav48_t, buff);
	IRQL_t oldIrql;
	RiseIrql(IRQL_REALTIME, & oldIrql);
	LCLSPIN_LOCK(& lockwav48);
	InsertHeadList2(& recordswav48ready, & p->item);
	LCLSPIN_UNLOCK(& lockwav48);
	LowerIrql(oldIrql);
}


/* Получение пары (левый и правый) сжмплов для воспроизведения через аудиовыход трансивера
 * или для переачи
 * Возврат 0, если нет ничего для воспроизведения.
 */
uint_fast8_t takewavsample(FLOAT32P_t * rv, uint_fast8_t suspend)
{
	static recordswav48_t * p = NULL;
	static unsigned n;
	LCLSPIN_LOCK(& lockwav48);
	if (p == NULL)
	{
		if (! IsListEmpty2(& recordswav48ready))
		{
			PLIST_ENTRY t = RemoveTailList2(& recordswav48ready);
			p = CONTAINING_RECORD(t, recordswav48_t, item);
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
		InsertHeadList2(& recordswav48free, & p->item);
		p = NULL;
		//PRINTF("Release record buffer\n");
	}
	LCLSPIN_UNLOCK(& lockwav48);
	return 1;	// Сэмпл считан
}

#endif /* WITHUSEAUDIOREC */

#if WITHMODEM


// Буферы с принятымти через модем данными
size_t takemodemrxbuffer(uint8_t * * dest)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_REALTIME, & oldIrql);
	if (! IsListEmpty2(& modemsrx8))
	{
		PLIST_ENTRY t = RemoveTailList2(& modemsrx8);
		LowerIrql(oldIrql);
		modems8_t * const p = CONTAINING_RECORD(t, modems8_t, item);
		* dest = p->buff;
		return p->length;
	}
	LowerIrql(oldIrql);
	* dest = NULL;
	return 0;
}

// Буферы для заполнения данными
size_t takemodembuffer(uint8_t * * dest)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_REALTIME, & oldIrql);
	if (! IsListEmpty2(& modemsfree8))
	{
		PLIST_ENTRY t = RemoveTailList2(& modemsfree8);
		LowerIrql(oldIrql);
		modems8_t * const p = CONTAINING_RECORD(t, modems8_t, item);
		* dest = p->buff;
		return (MODEMBUFFERSIZE8 * sizeof p->buff [0]);
	}
	LowerIrql(oldIrql);
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
	IRQL_t oldIrql;
	RiseIrql(IRQL_REALTIME, & oldIrql);
	InsertHeadList2(& modemsfree8, & p->item);
	LowerIrql(oldIrql);
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
	LCLSPIN_LOCK(& locklist32tx);
	if (! IsListEmpty2(& voicesfree32tx))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesfree32tx);
		voice32tx_t * const p = CONTAINING_RECORD(t, voice32tx_t, item);
		LCLSPIN_UNLOCK(& locklist32tx);
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
		LCLSPIN_UNLOCK(& locklist32tx);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
#if WITHBUFFERSDEBUG
		++ e8;
#endif /* WITHBUFFERSDEBUG */
		return (uintptr_t) & p->buff;
	}
	LCLSPIN_UNLOCK(& locklist32tx);
	/* error path */
	PRINTF(PSTR("allocate_dmabuffer32tx() failure\n"));
	for (;;)
		;
	return 0;
}

// Этой функцией пользуются обработчики прерываний DMA на приём данных по SAI
RAMFUNC uintptr_t allocate_dmabuffer32rx(void)
{
	LCLSPIN_LOCK(& locklist32rx);
	if (! IsListEmpty2(& voicesfree32rx))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesfree32rx);
		voice32rx_t * const p = CONTAINING_RECORD(t, voice32rx_t, item);
		LCLSPIN_UNLOCK(& locklist32rx);
		return (uintptr_t) & p->buff;
	}
	else
	{
		LCLSPIN_UNLOCK(& locklist32rx);
		PRINTF(PSTR("allocate_dmabuffer32rx() failure\n"));
		for (;;)
			;
	}
	return 0;
}

// Этой функцией пользуются обработчики прерываний DMA на передачу и приём данных по I2S и USB AUDIO
RAMFUNC uintptr_t allocate_dmabuffer16rx(void)
{
	LCLSPIN_LOCK(& locklist16rx);
	if (! IsListEmpty2(& voicesfree16rx))
	{
		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16rx);
		LCLSPIN_UNLOCK(& locklist16rx);
		voice16rx_t * const p = CONTAINING_RECORD(t, voice16rx_t, item);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		return (uintptr_t) & p->rbuff;
	}
#if WITHUSBUAC
	else if (! IsListEmpty3(& resample16rx))
	{
		// Ошибочная ситуация - если буферы не освобождены вовремя -
		// берём из очереди готовых к ресэмплингу
		uint_fast8_t n = 3;
		do
		{
			const PLIST_ENTRY t = RemoveTailList3(& resample16rx);
			voice16rx_t * const p = CONTAINING_RECORD(t, voice16rx_t, item);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			InsertHeadList2(& voicesfree16rx, t);
		}
		while (-- n && ! IsListEmpty3(& resample16rx));

		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16rx);
		LCLSPIN_UNLOCK(& locklist16rx);
		voice16rx_t * const p = CONTAINING_RECORD(t, voice16rx_t, item);
	#if WITHBUFFERSDEBUG
		++ e4;
	#endif /* WITHBUFFERSDEBUG */
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		return (uintptr_t) & p->rbuff;
	}
#endif /* WITHUSBUAC */
	else
	{
		LCLSPIN_UNLOCK(& locklist16rx);
		PRINTF(PSTR("allocate_dmabuffer16rx() failure\n"));
		for (;;)
			;
	}
	return 0;
}

// Этой функцией пользуются обработчики прерываний DMA на передачу и приём данных по I2S и USB AUDIO
RAMFUNC uintptr_t allocate_dmabuffer16tx(void)
{
	LCLSPIN_LOCK(& locklist16tx);
	if (! IsListEmpty2(& voicesfree16tx))
	{
		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16tx);
		LCLSPIN_UNLOCK(& locklist16tx);
		voice16tx_t * const p = CONTAINING_RECORD(t, voice16tx_t, item);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		return (uintptr_t) & p->tbuff;
	}
	else if (! IsListEmpty3(& voicesphones16tx) && ! IsListEmpty3(& voicesmoni16tx))
	{
		// Ошибочная ситуация - если буферы не освобождены вовремя -
		// берём из очереди готовых к прослушиванию

		uint_fast8_t n = 3;
		do
		{
			{
				const PLIST_ENTRY t = RemoveTailList3(& voicesphones16tx);
				voice16tx_t * const p = CONTAINING_RECORD(t, voice16tx_t, item);
				ASSERT(p->tag2 == p);
				ASSERT(p->tag3 == p);
				InsertHeadList2(& voicesfree16tx, t);
			}
			{
				const PLIST_ENTRY t = RemoveTailList3(& voicesmoni16tx);
				voice16tx_t * const p = CONTAINING_RECORD(t, voice16tx_t, item);
				ASSERT(p->tag2 == p);
				ASSERT(p->tag3 == p);
				InsertHeadList2(& voicesfree16tx, t);
			}
		}
		while (-- n && ! IsListEmpty3(& voicesphones16tx) && ! IsListEmpty3(& voicesmoni16tx));

		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16tx);
		LCLSPIN_UNLOCK(& locklist16tx);
		voice16tx_t * const p = CONTAINING_RECORD(t, voice16tx_t, item);
	#if WITHBUFFERSDEBUG
		++ e3;
	#endif /* WITHBUFFERSDEBUG */
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		return (uintptr_t) & p->tbuff;
	}
	else
	{
		LCLSPIN_UNLOCK(& locklist16tx);
		PRINTF(PSTR("allocate_dmabuffer16tx() failure\n"));
		for (;;)
			;
	}
	return 0;
}

// Этой функцией пользуются обработчики прерываний DMA
// передали буфер, считать свободным
void RAMFUNC release_dmabuffer32tx(uintptr_t addr)
{
	//ASSERT(addr != 0);
	voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	LCLSPIN_LOCK(& locklist32tx);
	InsertHeadList2(& voicesfree32tx, & p->item);
	LCLSPIN_UNLOCK(& locklist32tx);
}

// Этой функцией пользуются обработчики прерываний DMA
// передали буфер, считать свободным
void RAMFUNC release_dmabuffer16rx(uintptr_t addr)
{
	//ASSERT(addr != 0);
	voice16rx_t * const p = CONTAINING_RECORD(addr, voice16rx_t, rbuff);
	buffers_tonull16rx(p);
}

// Этой функцией пользуются обработчики прерываний DMA
// передали буфер, считать свободным
void RAMFUNC release_dmabuffer16tx(uintptr_t addr)
{
	//ASSERT(addr != 0);
	voice16tx_t * const p = CONTAINING_RECORD(addr, voice16tx_t, tbuff);
	buffers_tonull16tx(p);
}

static void debaudio(int v)
{
	static const char hex [16] = "0123456789ABCDEF";

	dbg_putchar(hex [(v >> 12) & 0x0F]);
	dbg_putchar(hex [(v >> 8) & 0x0F]);
	dbg_putchar(hex [(v >> 4) & 0x0F]);
	dbg_putchar(hex [(v >> 0) & 0x0F]);
	dbg_putchar(' ');

}

// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки AF ADC
void RAMFUNC processing_dmabuffer16rx(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n3;
#endif /* WITHBUFFERSDEBUG */
	voice16rx_t * const p = CONTAINING_RECORD(addr, voice16rx_t, rbuff);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_mikeadc += DMABUFFSIZE16RX / DMABUFFSTEP16RX;	// в буфере пары сэмплов по два байта
#endif /* WITHBUFFERSDEBUG */

	//debaudio(p->buff [0]);

	LCLSPIN_LOCK(& locklist16rx);
	InsertHeadList3(& voicesmike16rx, & p->item, 0);
	LCLSPIN_UNLOCK(& locklist16rx);

	//dsp_processtx();	/* выборка семплов из источников звука и формирование потока на передатчик */

}

// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после приёма пакета с USB AUDIO
static void processing_dmabuffer16rxuac(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n2;
#endif /* WITHBUFFERSDEBUG */
	voice16rx_t * const p = CONTAINING_RECORD(addr, voice16rx_t, rbuff);
//	// Замена приходящего с USB тоном
//	{
//		unsigned i;
//		for (i = 0; i < DMABUFFSIZE16RX; i += DMABUFFSTEP16RX)
//		{
//			p->rbuff [i + DMABUFF16RX_LEFT] = adpt_output(& afcodecrx, get_lout());
//			p->rbuff [i + DMABUFF16RX_RIGHT] = adpt_output(& afcodecrx, get_rout());
//		}
//	}
	buffers_savefromuacout(p);
}

// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки IF ADC (спектроанализатор)
// Вызывается на ARM_REALTIME_PRIORITY уровне.
void RAMFUNC processing_dmabuffer32rx(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n1;
	// подсчёт скорости в сэмплах за секунду
	debugcount_rx32adc += CNT32RX;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */

	voice32rx_t * const p = CONTAINING_RECORD(addr, voice32rx_t, buff);
	dsp_extbuffer32rx(p->buff);

	dsp_processtx();	/* выборка семплов из источников звука и формирование потока на передатчик */

}

// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки IF ADC (MAIN RX/SUB RX)
// Вызывается на ARM_REALTIME_PRIORITY уровне.
void RAMFUNC processing_dmabuffer32rts(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	//++ n77;
	// подсчёт скорости в сэмплах за секунду
	//debugcount_rx32rtsadc += CNT32RTS;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */
	voice32rts_t * const p = CONTAINING_RECORD(addr, voice32rts_t, buff);
	dsp_extbuffer32rts(p->buff);
}

void release_dmabuffer32rx(uintptr_t addr)
{
	//ASSERT(addr != 0);
	voice32rx_t * const p = CONTAINING_RECORD(addr, voice32rx_t, buff);

	LCLSPIN_LOCK(& locklist32rx);
	InsertHeadList2(& voicesfree32rx, & p->item);
	LCLSPIN_UNLOCK(& locklist32rx);

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
#if WITHWFM
	dsp_extbuffer32wfm((const IFADCvalue_t *) addr);
#endif /* WITHWFM */
}

#if WITHFPGAPIPE_CODEC1

// копирование полей из принятого от FPGA буфера
static uintptr_t RAMFUNC
pipe_dmabuffer16rx(uintptr_t addr16rx, uintptr_t addr32rx)
{
	// Предполагается что типы данных позволяют транзитом передавать сэмплы, не беспокоясь о преобразовании форматов
	voice16rx_t * const vrx16 = CONTAINING_RECORD(addr16rx, voice16rx_t, rbuff);
	ASSERT(vrx16->tag2 == vrx16);
	ASSERT(vrx16->tag3 == vrx16);
	voice32rx_t * const vrx32 = CONTAINING_RECORD(addr32rx, voice32rx_t, buff);
	unsigned i;
	IFADCvalue_t * const rx32 = vrx32->buff;
	aubufv_t * const rx16 = vrx16->rbuff;

	ASSERT((unsigned) CNT32RX == (unsigned) CNT16RX);
	ASSERT(sizeof * rx32 == sizeof * rx16);
	for (i = 0; i < (unsigned) CNT32RX; ++ i)
	{
		rx16 [i * DMABUFFSTEP16RX + DMABUFF16RX_LEFT] = rx32 [i * DMABUFFSTEP32RX + DMABUFF32RX_CODEC1_LEFT];
		rx16 [i * DMABUFFSTEP16RX + DMABUFF16RX_RIGHT] = rx32 [i * DMABUFFSTEP32RX + DMABUFF32RX_CODEC1_RIGHT];
	}
	return addr16rx;
}

// копирование полей в передаваемый на FPGA буфер
static uintptr_t RAMFUNC
pipe_dmabuffer32tx(uintptr_t addr32tx, uintptr_t addr16tx)
{
	// Предполагается что типы данных позволяют транзитом передавать сэмплы, не беспокоясь о преобразовании форматов
	voice16tx_t * const vtx16 = CONTAINING_RECORD(addr16tx, voice16tx_t, tbuff);
	ASSERT(vtx16->tag2 == vtx16);
	ASSERT(vtx16->tag3 == vtx16);
	voice32tx_t * const vtx32 = CONTAINING_RECORD(addr32tx, voice32tx_t, buff);
	ASSERT(vtx32->tag2 == vtx32);
	ASSERT(vtx32->tag3 == vtx32);

	IFDACvalue_t * const tx32 = vtx32->buff;
	aubufv_t * const tx16 = vtx16->tbuff;
	unsigned i;
	const FLOAT_t scale = 1.0 / 32;

	ASSERT((unsigned) CNT32TX == (unsigned) CNT16TX);
	ASSERT(DMABUFFSTEP16TX >= 2);
	ASSERT(sizeof * tx32 == sizeof * tx16);
	for (i = 0; i < (unsigned) CNT32TX; ++ i)
	{
		tx32 [i * DMABUFFSTEP32TX + DMABUFF32TX_CODEC1_LEFT] = tx16 [i * DMABUFFSTEP16TX + DMABUFF16TX_LEFT];
		tx32 [i * DMABUFFSTEP32TX + DMABUFF32TX_CODEC1_RIGHT] = tx16 [i * DMABUFFSTEP16TX + DMABUFF16TX_RIGHT];
//		tx32 [i * DMABUFFSTEP32TX + DMABUFF32TX_CODEC1_LEFT] = adpt_outputexact(& afcodectx, get_lout() * scale);
//		tx32 [i * DMABUFFSTEP32TX + DMABUFF32TX_CODEC1_RIGHT] = adpt_outputexact(& afcodectx, get_rout() * scale);
	}
	return addr32tx;
}

#endif /* WITHFPGAPIPE_CODEC1 */

#if WITHRTS192
// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки - канал спектроанализатора
void RAMFUNC processing_dmabuffer32rts192(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n4;
#endif /* WITHBUFFERSDEBUG */
	voice192rts_t * const p = CONTAINING_RECORD(addr, voice192rts_t, u.buff);

#if ! WITHTRANSPARENTIQ
	unsigned i;
	for (i = 0; i < DMABUFFSIZE32RTS; i += DMABUFFSTEP32RTS)
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
	debugcount_tx32dac += CNT32TX;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */

	//dsp_processtx();	/* выборка семплов из источников звука и формирование потока на передатчик */

	LCLSPIN_LOCK(& locklist32tx);
	if (! IsListEmpty2(& voicesready32tx))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesready32tx);
		voice32tx_t * const p = CONTAINING_RECORD(t, voice32tx_t, item);
		LCLSPIN_UNLOCK(& locklist32tx);
		return (uintptr_t) & p->buff;
	}
	LCLSPIN_UNLOCK(& locklist32tx);
#if WITHBUFFERSDEBUG
	++ e9;
#endif /* WITHBUFFERSDEBUG */
	return allocate_dmabuffer32tx();	// аварийная ветка - работает первые несколько раз
}

// Этой функцией пользуются обработчики прерываний DMA
// получить буфер для передачи через IF DAC2
uintptr_t getfilled_dmabuffer32tx_sub(void)
{
	return allocate_dmabuffer32tx();
}



/* при необходимости копируем сэмплы от кодекв */
uintptr_t processing_pipe32rx(uintptr_t addr)
{
#if WITHFPGAPIPE_CODEC1
	processing_dmabuffer16rx(pipe_dmabuffer16rx(allocate_dmabuffer16rx(), addr));
#endif /* WITHFPGAPIPE_CODEC1 */
#if WITHFPGAPIPE_RTS96
	//processing_dmabuffer32rts(addr);
#endif /* WITHFPGAPIPE_RTS96 */
#if WITHFPGAPIPE_RTS192
	//processing_dmabuffer32rts(addr);
#endif /* WITHFPGAPIPE_RTS192 */
	return addr;
}

/* при необходимости добавляем слоты для передачи на кодек */
uintptr_t processing_pipe32tx(uintptr_t addr)
{
#if WITHFPGAPIPE_CODEC1
	const uintptr_t addr16 = getfilled_dmabuffer16txphones();
	pipe_dmabuffer32tx(addr, addr16);
	release_dmabuffer16tx(addr16);	/* освоюождаем буфер как переданный */
#endif /* WITHFPGAPIPE_CODEC1 */
#if WITHFPGAPIPE_NCORX0
#endif /* WITHFPGAPIPE_NCORX0 */
#if WITHFPGAPIPE_NCORX1
#endif /* WITHFPGAPIPE_NCORX1 */
#if WITHFPGAPIPE_NCORTS
#endif /* WITHFPGAPIPE_NCORTS */
	return addr;
}

// Отладочная функция для тестирования обмена с кодеком
void dsp_calctx(void)
{
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_phonesdac += DMABUFFSIZE16TX / DMABUFFSTEP16TX;	// в буфере пары сэмплов по два байта
#endif /* WITHBUFFERSDEBUG */
}

// Отладочная функция для тестирования обмена с кодеком
void dsp_calcrx(void)
{
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_mikeadc += DMABUFFSIZE16RX / DMABUFFSTEP16RX;	// в буфере пары сэмплов по два байта
#endif /* WITHBUFFERSDEBUG */
}

// Этой функцией пользуются обработчики прерываний DMA
// получить буфер для передачи через AF DAC
uintptr_t getfilled_dmabuffer16txphones(void)
{
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_phonesdac += DMABUFFSIZE16TX / DMABUFFSTEP16TX;	// в буфере пары сэмплов по два байта
#endif /* WITHBUFFERSDEBUG */
	const uintptr_t monibuf = getfilled_dmabuffer16txmoni();
	LCLSPIN_LOCK(& locklist16tx);
	if (GetReadyList3(& voicesphones16tx))
	{
		PLIST_ENTRY t = RemoveTailList3(& voicesphones16tx);
		LCLSPIN_UNLOCK(& locklist16tx);
		voice16tx_t * const p = CONTAINING_RECORD(t, voice16tx_t, item);
		dsp_addsidetone(p->tbuff, (aubufv_t *) monibuf, 1);
		release_dmabuffer16tx(monibuf);
		return (uintptr_t) & p->tbuff;	// алрес для DMA
	}
	LCLSPIN_UNLOCK(& locklist16tx);

#if WITHBUFFERSDEBUG
	++ e1;
#endif /* WITHBUFFERSDEBUG */

	const uintptr_t addr = allocate_dmabuffer16tx();
	voice16tx_t * const p = CONTAINING_RECORD(addr, voice16tx_t, tbuff);
	dsp_addsidetone(p->tbuff, (aubufv_t *) monibuf, 0);
	release_dmabuffer16tx(monibuf);
	return (uintptr_t) & p->tbuff;
}

// Этой функцией пользуются обработчики прерываний DMA
uintptr_t getfilled_dmabuffer16txmoni(void)
{
	LCLSPIN_LOCK(& locklist16tx);
	if (GetReadyList3(& voicesmoni16tx))
	{
		PLIST_ENTRY t = RemoveTailList3(& voicesmoni16tx);
		LCLSPIN_UNLOCK(& locklist16tx);
		voice16tx_t * const p = CONTAINING_RECORD(t, voice16tx_t, item);
		return (uintptr_t) & p->tbuff;	// алрес для DMA
	}
	LCLSPIN_UNLOCK(& locklist16tx);

#if WITHBUFFERSDEBUG
	++ e10;
#endif /* WITHBUFFERSDEBUG */

	const uintptr_t addr = allocate_dmabuffer16tx();
	voice16tx_t * const p = CONTAINING_RECORD(addr, voice16tx_t, tbuff);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	memset(p->tbuff, 0, sizeof p->tbuff); // Заполнение "тишиной"
	return (uintptr_t) & p->tbuff;
}

//////////////////////////////////////////
// Поэлементное заполнение буфера IF DAC

// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
// 32 bit, signed
void savesampleout32stereo(int_fast32_t ch0, int_fast32_t ch1)
{
	static voice32tx_t * prepareout32tx = NULL;
	static unsigned level32tx;

	if (prepareout32tx == NULL)
	{
		const uintptr_t addr = allocate_dmabuffer32tx();
		voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
		prepareout32tx = p;
		level32tx = 0;
	}

	prepareout32tx->buff [level32tx + DMABUF32TXI] = ch0;
	prepareout32tx->buff [level32tx + DMABUF32TXQ] = ch1;

#if defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_GW2A_V0)

#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_FPGAV1) && ! (CTLREGMODE_OLEG4Z_V1 || CTLREGMODE_OLEG4Z_V2)
	/* установка параметров приемника, передаваемых чрез I2S канал в FPGA */
	prepareout32tx->buff [level32tx + DMABUF32TX_NCO1] = dspfpga_get_nco1();
	prepareout32tx->buff [level32tx + DMABUF32TX_NCO2] = dspfpga_get_nco2();
	prepareout32tx->buff [level32tx + DMABUF32TX_NCORTS] = dspfpga_get_ncorts();
#endif /* (DDS1_TYPE == DDS_TYPE_FPGAV1) && ! (CTLREGMODE_OLEG4Z_V1 || CTLREGMODE_OLEG4Z_V2) */

#if CPUSTYLE_XC7Z && WITHLFM
	if (iflfmactive())
	{
		ftw_t v = dspfpga_get_nco1();
		xcz_dds_ftw(& v);
		v = dspfpga_get_ncorts();
		xcz_dds_rts(& v);
	}
#endif /* CPUSTYLE_XC7Z && WITHLFM */

	if ((level32tx += DMABUFFSTEP32TX) >= DMABUFFSIZE32TX)
	{
		LCLSPIN_LOCK(& locklist32tx);
		InsertHeadList2(& voicesready32tx, & prepareout32tx->item);
		LCLSPIN_UNLOCK(& locklist32tx);
		prepareout32tx = NULL;
	}
}

//////////////////////////////////////////
// Поэлементное заполнение DMA буфера AF DAC
/* вызывается при запрещённых глобавльных прерываниях */
static void savesampleout16stereo(int_fast32_t ch0, int_fast32_t ch1)
{
	// если есть инициализированный канал для выдачи звука
	static voice16tx_t * p = NULL;
	static unsigned n;

	if (p == NULL)
	{
		uintptr_t addr = allocate_dmabuffer16tx();
		p = CONTAINING_RECORD(addr, voice16tx_t, tbuff);
		n = 0;
	}
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	p->tbuff [n * DMABUFFSTEP16TX + DMABUFF16TX_LEFT] = ch0;	// sample value
	p->tbuff [n * DMABUFFSTEP16TX + DMABUFF16TX_RIGHT] = ch1;	// sample value

	if (++ n >= CNT16TX)
	{
		buffers_savefromrxout16tx(p);
		p = NULL;
	}
}

/* вызывается при запрещённых глобальных прерываниях */
static void savesampleout16stereo_float(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	savesampleout16stereo(
		adpt_output(& afcodectx, ch0),
		adpt_output(& afcodectx, ch1)
		);
}


#if WITHUSBUAC && WITHUSBHW

/* выборка нужного количества байт из UAC OUT буфера. Расширение знака не требуется, результат обрабатывается функцией transform_do32 */
static int32_t fetch_le(const uint8_t * p, size_t usbsz)
{
	switch (usbsz)
	{
	default:
	case 1:
		return p [0];
	case 2:
		return p [1] * 256u + p [0];
	case 3:
		return (p [2] * 256u + p [1]) * 256u + p [0];
	case 4:
		return ((p [3] * 256u + p [2]) * 256u + p [1]) * 256u + p [0];
	}
}

/* Поместить в буфер UAC IN endpoint значения сжмпла */
static void place_le(uint8_t * p, int32_t value, size_t usbsz)
{
	switch (usbsz)
	{
	default:
		ASSERT(0);
		break;
	case 1:
		p [0] = (uint8_t) (value >> 0);
		break;
	case 2:
		p [1] = (uint8_t) (value >> 8);
		p [0] = (uint8_t) (value >> 0);
		break;
	case 3:
		p [2] = (uint8_t) (value >> 16);
		p [1] = (uint8_t) (value >> 8);
		p [0] = (uint8_t) (value >> 0);
		break;
	case 4:
		p [3] = (uint8_t) (value >> 24);
		p [2] = (uint8_t) (value >> 16);
		p [1] = (uint8_t) (value >> 8);
		p [0] = (uint8_t) (value >> 0);
		break;
	}
}

#if WITHRTS96

	// Этой функцией пользуются обработчики прерываний DMA на передачу данных по USB
	static uintptr_t allocate_dmabufferuacinrts96(void)
	{
		LCLSPIN_LOCK(& locklistrts);
		if (! IsListEmpty2(& uacinrts96free))
		{
			const PLIST_ENTRY t = RemoveTailList2(& uacinrts96free);
			voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
			LCLSPIN_UNLOCK(& locklistrts);
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
		else if (! IsListEmpty2(& uacinrts96ready))
		{
#if WITHBUFFERSDEBUG
			++ e12;
#endif /* WITHBUFFERSDEBUG */
			// Ошибочная ситуация - если буферы не освобождены вовремя -
			// берём из очереди готовых к передаче в компьютер по USB.
			// Очередь очищается возможно не полностью.
			uint_fast8_t n = 3;
			do
			{
				const PLIST_ENTRY t = RemoveTailList2(& uacinrts96ready);
				voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
				if (p->tag != BUFFTAG_RTS96)
				{
					PRINTF(" p=%p, tag=%d, tag2=%p, tag3=%p\n", p, p->tag, p->tag2, p->tag3);
					//printhex(0, p->u.buff, sizeof p->u.buff);
				}
				ASSERT(p->tag == BUFFTAG_RTS96);
				ASSERT(p->tag2 == p);
				ASSERT(p->tag3 == p);
				InsertHeadList2(& uacinrts96free, t);
			} while (-- n && ! IsListEmpty2(& uacinrts96ready));

			const PLIST_ENTRY t = RemoveTailList2(& uacinrts96free);
			voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
			LCLSPIN_UNLOCK(& locklistrts);
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
			LCLSPIN_UNLOCK(& locklistrts);
			PRINTF(PSTR("allocate_dmabufferuacinrts96() failure\n"));
			for (;;)
				;
		}
	}

	// Этой функцией пользуются обработчики прерываний DMA
	// передали буфер, считать свободным
	void release_dmabufferuacinrts96(uintptr_t addr)
	{
		ASSERT(addr != 0);
		voice96rts_t * const p = CONTAINING_RECORD(addr, voice96rts_t, u.buff);
		ASSERT(p->tag == BUFFTAG_RTS96);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		LCLSPIN_LOCK(& locklistrts);
		InsertHeadList2(& uacinrts96free, & p->item);
		LCLSPIN_UNLOCK(& locklistrts);
	}

	// Этой функцией пользуются обработчики прерываний DMA
	// получить буфер для передачи в компьютер, через USB AUDIO
	// Если в данный момент нет готового буфера, возврат 0
	uintptr_t getfilled_dmabufferuacinrts96(void)
	{
		LCLSPIN_LOCK(& locklistrts);
		if (! IsListEmpty2(& uacinrts96ready))
		{
			PLIST_ENTRY t = RemoveTailList2(& uacinrts96ready);
			voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
			LCLSPIN_UNLOCK(& locklistrts);
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
		LCLSPIN_UNLOCK(& locklistrts);
		return 0;
	}

	// Поэлементное заполнение буфера RTS96

	// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
	// vl, vr: 32 bit, signed - преобразуем к требуемому формату для передачи по USB здесь.
	void savesampleout96stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
	{
		// если есть инициализированный канал для выдачи звука
		static voice96rts_t * p = NULL;
		static unsigned n;

		if (p == NULL)
		{
			if (! isrts96())
				return;
			uintptr_t addr = allocate_dmabufferuacinrts96();
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

		place_le(p->u.buff + n, transform_do32(& if2rts96out, ch0), UACIN_RTS96_SAMPLEBYTES);	// sample value
		n += UACIN_RTS96_SAMPLEBYTES;
		place_le(p->u.buff + n, transform_do32(& if2rts96out, ch1), UACIN_RTS96_SAMPLEBYTES);	// sample value
		n += UACIN_RTS96_SAMPLEBYTES;

		if (n >= ARRAY_SIZE(p->u.buff))
		{
			ASSERT(p->tag == BUFFTAG_RTS96);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			buffers_savetouacinrts96(p);
			p = NULL;
		}
	}

#endif /* WITHRTS96 */

#if WITHRTS192

	// Этой функцией пользуются обработчики прерываний DMA на передачу данных по USB
	// Этой функцией пользуются обработчики прерываний DMA
	// получить буфер для передачи в компьютер, через USB AUDIO
	// Если в данный момент нет готового буфера, возврат 0
	static uintptr_t getfilled_dmabufferuacinrts192(void)
	{
		LCLSPIN_LOCK(& locklistrts);
		if (! IsListEmpty2(& uacin192rts))
		{
			PLIST_ENTRY t = RemoveTailList2(& uacin192rts);
			voice192rts_t * const p = CONTAINING_RECORD(t, voice192rts_t, item);
			LCLSPIN_UNLOCK(& locklistrts);
			return (uintptr_t) & p->u.buff;
		}
		LCLSPIN_UNLOCK(& locklistrts);
		return 0;
	}

	// Этой функцией пользуются обработчики прерываний DMA на приём данных по SAI
	uintptr_t allocate_dmabuffer192rts(void)
	{
		LCLSPIN_LOCK(& locklistrts);
		if (! IsListEmpty2(& voicesfree192rts))
		{
			PLIST_ENTRY t = RemoveTailList2(& voicesfree192rts);
			LCLSPIN_UNLOCK(& locklistrts);
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
#if WITHBUFFERSDEBUG
			++ e13;
#endif /* WITHBUFFERSDEBUG */
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
			LCLSPIN_UNLOCK(& locklistrts);
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
			LCLSPIN_UNLOCK(& locklistrts);
			PRINTF(PSTR("allocate_dmabuffer192rts() failure\n"));
			for (;;)
				;
		}
	}

	// Этой функцией пользуются обработчики прерываний DMA
	// передали буфер, считать свободным
	static void release_dmabuffer192rts(uintptr_t addr)
	{
		voice192rts_t * const p = CONTAINING_RECORD(addr, voice192rts_t, u.buff);
		ASSERT(p->tag == BUFFTAG_RTS192);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		LCLSPIN_LOCK(& locklistrts);
		InsertHeadList2(& voicesfree192rts, & p->item);
		LCLSPIN_UNLOCK(& locklistrts);
	}

	// NOT USED
	// Поэлементное заполнение буфера RTS192

	// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
	// vl, vr: 32 bit, signed - преобразуем к требуемому формату для передачи по USB здесь.
	void savesampleout192stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
	{
		// если есть инициализированный канал для выдачи звука
		static voice192rts_t * RAMBIGDTCM p = NULL;
		static RAMBIGDTCM unsigned n;

		if (p == NULL)
		{
			if (! isrts192())
				return;
			uintptr_t addr = allocate_dmabuffer192rts();
			p = CONTAINING_RECORD(addr, voice192rts_t, u.buff);
			n = 0;

			ASSERT(p->tag == BUFFTAG_RTS192);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
		}
		else if (! isrts192())
		{
			buffers_savetonull192rts(p);
			p = NULL;
			return;
		}

		place_le(p->u.buff + n, transform_do32(& if2rts192out, ch0), UACIN_RTS192_SAMPLEBYTES);	// sample value
		n += UACIN_RTS192_SAMPLEBYTES;
		place_le(p->u.buff + n, transform_do32(& if2rts192out, ch1), UACIN_RTS192_SAMPLEBYTES);	// sample value
		n += UACIN_RTS192_SAMPLEBYTES;

		if (n >= ARRAY_SIZE(p->u.buff))
		{
			buffers_savetouacin192rts(p);
			p = NULL;
		}
	}

#endif /* WITHRTS192 */

#if WITHUSBUACOUT && 0

void RAMFUNC release_dmabufferuacout48(uintptr_t addr)
{
	//ASSERT(addr != 0);
	uacout48_t * const p = CONTAINING_RECORD(addr, uacout48_t, u.buff);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	LCLSPIN_LOCK(& locklistuacout48);
	InsertHeadList2(& uacout48free, & p->item);
	LCLSPIN_UNLOCK(& locklistuacout48);
}

void processing_dmabufferuacout48(uintptr_t addr)
{
	uacout_buffer_save((const uint8_t *) addr, UACOUT_AUDIO48_DATASIZE_DMAC, UACOUT_FMT_CHANNELS_AUDIO48, UACOUT_AUDIO48_SAMPLEBYTES);

	release_dmabufferuacout48(addr);
}

// Этой функцией пользуются обработчики прерываний DMA на передачу данных по USB
RAMFUNC uintptr_t allocate_dmabufferuacout48(void)
{
	LCLSPIN_LOCK(& locklistuacout48);
	if (! IsListEmpty2(& uacout48free))
	{
		PLIST_ENTRY t = RemoveTailList2(& uacout48free);
		LCLSPIN_UNLOCK(& locklistuacout48);
		uacout48_t * const p = CONTAINING_RECORD(t, uacout48_t, item);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		return (uintptr_t) & p->u.buff;
	}
	else if (! IsListEmpty2(& uacout48ready))
	{
		// Ошибочная ситуация - если буферы не освобождены вовремя -
		// берём из очереди готовых к передаче
#if WITHBUFFERSDEBUG
		++ e14;
#endif /* WITHBUFFERSDEBUG */

		uint_fast8_t n = 3;
		do
		{
			const PLIST_ENTRY t = RemoveTailList2(& uacout48ready);
			InsertHeadList2(& uacout48free, t);
		}
		while (-- n && ! IsListEmpty2(& uacout48ready));

		PLIST_ENTRY t = RemoveTailList2(& uacout48free);
		LCLSPIN_UNLOCK(& locklistuacout48);
		uacout48_t * const p = CONTAINING_RECORD(t, uacout48_t, item);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		return (uintptr_t) & p->u.buff;
	}
	else
	{
		LCLSPIN_UNLOCK(& locklistuacout48);
		PRINTF(PSTR("allocate_dmabufferuacout48() failure, uacinalt=%d\n"), uacinalt);
		for (;;)
			;
	}
	return 0;
}

#endif /* WITHUSBUACOUT */

#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)

// Сохранить USB UAC IN буфер в никуда...
static RAMFUNC void buffers_tonulluacin(uacin48_t * p)
{
	LCLSPIN_LOCK(& locklistuacin48);
	InsertHeadList2(& uacin48free, & p->item);
	LCLSPIN_UNLOCK(& locklistuacin48);
}

void RAMFUNC release_dmabufferuacin48(uintptr_t addr)
{
	//ASSERT(addr != 0);
	uacin48_t * const p = CONTAINING_RECORD(addr, uacin48_t, u.buff);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	buffers_tonulluacin(p);
}

// Этой функцией пользуются обработчики прерываний DMA на передачу данных по USB
static RAMFUNC uintptr_t allocate_dmabufferuacin48(void)
{
	LCLSPIN_LOCK(& locklistuacin48);
	if (! IsListEmpty2(& uacin48free))
	{
		PLIST_ENTRY t = RemoveTailList2(& uacin48free);
		LCLSPIN_UNLOCK(& locklistuacin48);
		uacin48_t * const p = CONTAINING_RECORD(t, uacin48_t, item);
		ASSERT(p->tag == BUFFTAG_UACIN48);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		return (uintptr_t) & p->u.buff;
	}
	else if (! IsListEmpty2(& uacin48ready))
	{
		// Ошибочная ситуация - если буферы не освобождены вовремя -
		// берём из очереди готовых к передаче
#if WITHBUFFERSDEBUG
		++ e11;
#endif /* WITHBUFFERSDEBUG */

		uint_fast8_t n = 3;
		do
		{
			const PLIST_ENTRY t = RemoveTailList2(& uacin48ready);
			InsertHeadList2(& uacin48free, t);
		}
		while (-- n && ! IsListEmpty2(& uacin48ready));

		PLIST_ENTRY t = RemoveTailList2(& uacin48free);
		LCLSPIN_UNLOCK(& locklistuacin48);
		uacin48_t * const p = CONTAINING_RECORD(t, uacin48_t, item);
		ASSERT(p->tag == BUFFTAG_UACIN48);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		return (uintptr_t) & p->u.buff;
	}
	else
	{
		LCLSPIN_UNLOCK(& locklistuacin48);
		PRINTF(PSTR("allocate_dmabufferuacin48() failure, uacinalt=%d\n"), uacinalt);
		for (;;)
			;
	}
	return 0;
}

// Этой функцией пользуются обработчики прерываний DMA
// получить буфер для передачи в компьютер, через USB AUDIO
// Если в данный момент нет готового буфера, возврат 0
uintptr_t getfilled_dmabufferuacin48(void)
{
	LCLSPIN_LOCK(& locklistuacin48);
	if (! IsListEmpty2(& uacin48ready))
	{
		PLIST_ENTRY t = RemoveTailList2(& uacin48ready);
		LCLSPIN_UNLOCK(& locklistuacin48);
		uacin48_t * const p = CONTAINING_RECORD(t, uacin48_t, item);
		ASSERT(p->tag == BUFFTAG_UACIN48);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		return (uintptr_t) & p->u.buff;
	}
	LCLSPIN_UNLOCK(& locklistuacin48);
	return 0;
}

#endif /* WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) */

// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
// vl, vr: 16 bit, signed - требуемый формат для передачи по USB.

void savesampleuacin48(int_fast32_t ch0, int_fast32_t ch1)
{
#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)
	// если есть инициализированный канал для выдачи звука
	static uacin48_t * p = NULL;
	static unsigned n = 0;

	if (p == NULL)
	{
		if (! isaudio48())
			return;
		uintptr_t addr = allocate_dmabufferuacin48();
		p = CONTAINING_RECORD(addr, uacin48_t, u.buff);
		ASSERT(p->tag == BUFFTAG_UACIN48);
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

	ASSERT(p->tag == BUFFTAG_UACIN48);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);

	place_le(p->u.buff + n, ch0, UACIN_AUDIO48_SAMPLEBYTES); // sample value
	n += UACIN_AUDIO48_SAMPLEBYTES;
#if UACIN_FMT_CHANNELS_AUDIO48 > 1
	place_le(p->u.buff + n, ch1, UACIN_AUDIO48_SAMPLEBYTES); // sample value
	n += UACIN_AUDIO48_SAMPLEBYTES;
#endif /* UACIN_FMT_CHANNELS_AUDIO48 */

	if (n >= ARRAY_SIZE(p->u.buff))
	{
		buffers_savetouacin(p);
		p = NULL;
	}
#endif /* WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) */
}

#else /* WITHUSBUAC */

void savesampleout96stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
{
}

void savesampleout192stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
{
}

void savesampleuacin48(int_fast16_t ch0, int_fast16_t ch1)
{
}

#endif /* WITHUSBUAC */

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

uint_fast8_t buffers_get_uacoutalt(void)
{
	return uacoutalt;
}

void 
buffers_set_uacoutalt(uint_fast8_t v)	/* выбор альтернативной конфигурации для UAC OUT interface */
{
	//PRINTF(PSTR("buffers_set_uacoutalt: v=%d\n"), (int) v);
	uacoutalt = v;

	if (v == 0)
	{
		IRQL_t oldIrql;
		RiseIrql(IRQL_REALTIME, & oldIrql);
		LCLSPIN_LOCK(& locklist16rx);

		// Очистить очередь принятых от USB UAC
		while (GetCountList3(& resample16rx) != 0)
		{
			const PLIST_ENTRY t = RemoveTailList3(& resample16rx);
			InsertHeadList2(& voicesfree16rx, t);
		}

		LCLSPIN_UNLOCK(& locklist16rx);
		LowerIrql(oldIrql);
	}
}

static uintptr_t uacoutaddr;	// address of DMABUFFSIZE16 * размер сэмпла * количество каналов bytes
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
		IRQL_t oldIrql;
		RiseIrql(IRQL_REALTIME, & oldIrql);
		release_dmabuffer16rx(uacoutaddr);
		LowerIrql(oldIrql);
		uacoutaddr = 0;
		uacoutbufflevel = 0;
	}
}

/* вызыватся из не-realtime функции обработчика прерывания */
// Работает на ARM_SYSTEM_PRIORITY
/* вызыватся из realtime функции обработчика прерывания */
// Работает на ARM_REALTIME_PRIORITY
void uacout_buffer_save(const uint8_t * buff, uint_fast16_t size, uint_fast8_t ichannels, uint_fast8_t usbsz)
{
	const size_t dmabuffer16size = DMABUFFSIZE16RX * sizeof (aubufv_t);	// размер в байтах

	for (;;)
	{
		const uint_fast16_t insamples = size / usbsz / ichannels;	// количество сэмплов во входном буфере
		const uint_fast16_t outsamples = (dmabuffer16size - uacoutbufflevel) / sizeof (aubufv_t) / DMABUFFSTEP16RX;
		const uint_fast16_t chunksamples = ulmin16(insamples, outsamples);
		const size_t inchunk = chunksamples * usbsz * ichannels;
		const size_t outchunk = chunksamples * sizeof (aubufv_t) * DMABUFFSTEP16RX;	// разхмер в байтах

		if (chunksamples == 0)
			break;
		if (uacoutaddr == 0)
		{
			IRQL_t oldIrql;
			RiseIrql(IRQL_REALTIME, & oldIrql);
			uacoutaddr = allocate_dmabuffer16rx();
			LowerIrql(oldIrql);
			uacoutbufflevel = 0;
		}

		if (ichannels == 1)
		{
			ASSERT(DMABUFFSTEP16RX == 2);
			// копирование нужного количества сэмплов с прербразованием из моно в стерео
			const uint8_t * src = buff;
			aubufv_t * dst = (aubufv_t *) (uacoutaddr + uacoutbufflevel);
			uint_fast16_t n = chunksamples;
			while (n --)
			{
				const aufastbufv_t v = transform_do32(& uac48out2afcodecrx, fetch_le(src, usbsz));
				* dst ++ = v;
				* dst ++ = v;
				src += usbsz;
			}
		}
		else
		{
			ASSERT(DMABUFFSTEP16RX == ichannels);
			// требуется преобразование формата из 16-бит семплов ко внутреннему формату aubufv_t
			/* копирование 16 бит сэмплов с расширением */
			const uint8_t * src = buff;
			aubufv_t * dst = (aubufv_t *) ((uint8_t *) uacoutaddr + uacoutbufflevel);
			uint_fast16_t n = chunksamples * ichannels;
			while (n --)
			{
				const aufastbufv_t v = transform_do32(& uac48out2afcodecrx, fetch_le(src, usbsz));
				* dst ++ = v;
				src += usbsz;
			}
		}

		size -= inchunk;	// проход по входому буферу
		buff += inchunk;	// проход входому буферу

		if ((uacoutbufflevel += outchunk) >= dmabuffer16size)
		{
			IRQL_t oldIrql;
			RiseIrql(IRQL_REALTIME, & oldIrql);
			processing_dmabuffer16rxuac(uacoutaddr);
			LowerIrql(oldIrql);
			uacoutaddr = 0;
			uacoutbufflevel = 0;
		}
	}
}
/* --- UAC OUT data save */


/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferuacinX(uintptr_t addr)
{
	//ASSERT(addr != 0);
	uacin48_t * const p = CONTAINING_RECORD(addr, uacin48_t, u.buff);
	switch (p->tag)
	{
#if WITHRTS96
	case BUFFTAG_RTS96:
		release_dmabufferuacinrts96(addr);
		return;
#endif /* WITHRTS96 */

#if WITHRTS192
	case BUFFTAG_RTS192:
		release_dmabuffer192rts(addr);
		return;
#endif /* WITHRTS192 */

	case BUFFTAG_UACIN48:
		release_dmabufferuacin48(addr);
		return;

	default:
		PRINTF(PSTR("release_dmabufferuacinX: wrong tag value: p=%p, %02X\n"), p, p->tag);
		for (;;)
			;
	}
}

/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
uintptr_t getfilled_dmabufferuacinX(uint_fast16_t * sizep)
{
#if WITHBUFFERSDEBUG
	++ n6;
#endif /* WITHBUFFERSDEBUG */
	switch (uacinalt)
	{
	case UACINALT_NONE:
		return 0;

	case UACINALT_AUDIO48:
		* sizep = UACIN_AUDIO48_DATASIZE_DMAC;
		return getfilled_dmabufferuacin48();

#if ! WITHUSBUACIN2

#if WITHRTS96
	case UACINALT_RTS96:
		* sizep = UACIN_RTS96_DATASIZE_DMAC;
		return getfilled_dmabufferuacinrts96();
#endif /* WITHRTS192 */

#if WITHRTS192
	case UACINALT_RTS192:
		* sizep = UACIN_RTS192_DATASIZE_DMAC;
		return getfilled_dmabufferuacinrts192();
#endif /* WITHRTS192 */

#endif /* ! WITHUSBUACIN2 */

	default:
		PRINTF(PSTR("getfilled_dmabufferuacinX: uacinalt=%u\n"), uacinalt);
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
		* sizep = UACIN_RTS96_DATASIZE_DMAC;
		return getfilled_dmabufferuacinrts96();
#endif /* WITHRTS192 */

#if WITHRTS192
	case UACINRTSALT_RTS192:
		* sizep = UACIN_RTS192_DATASIZE_DMAC;
		return getfilled_dmabufferuacinrts192();
#endif /* WITHRTS192 */

#endif /* WITHUSBUACIN2 && WITHUSBHW */

	default:
		PRINTF(PSTR("getfilled_dmabufferxrts: uacinrtsalt=%u\n"), uacinrtsalt);
		ASSERT(0);
		return 0;
	}
}

void release_dmabufferuacinrtsX(uintptr_t addr)	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
{
	release_dmabufferuacinX(addr);
}

#endif /* WITHUSBUACIN2 */

#endif /* WITHUSBUAC */

/* предполагается что тут значения нормирования в диапазоне -1..+1 */
void deliveryfloat(deliverylist_t * list, FLOAT_t ch0, FLOAT_t ch1)
{
	PLIST_ENTRY t;
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	for (t = list->head.Blink; t != & list->head; t = t->Blink)
	{
		subscribefloat_t * const p = CONTAINING_RECORD(t, subscribefloat_t, item);
		(p->cb)(p->ctx, ch0, ch1);
	}
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

/* предполагается что тут значения нормирования в диапазоне -1..+1 */
void deliveryfloat_buffer(deliverylist_t * list, const FLOAT_t * ch0, const FLOAT_t * ch1, unsigned n)
{
	IRQL_t oldIrql;
	PLIST_ENTRY t;

	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	for (t = list->head.Blink; t != & list->head; t = t->Blink)
	{
		subscribefloat_t * const p = CONTAINING_RECORD(t, subscribefloat_t, item);
		unsigned i;
		for (i = 0; i < n; ++ i)
		{
			(p->cb)(p->ctx, ch0 [i], ch1 [i]);
		}
	}
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void deliveryint(deliverylist_t * list, int_fast32_t ch0, int_fast32_t ch1)
{
	IRQL_t oldIrql;
	PLIST_ENTRY t;

	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	for (t = list->head.Blink; t != & list->head; t = t->Blink)
	{
		subscribeint32_t * const p = CONTAINING_RECORD(t, subscribeint32_t, item);
		(p->cb)(p->ctx, ch0, ch1);
	}
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void subscribefloat(deliverylist_t * list, subscribefloat_t * target, void * ctx, void (* pfn)(void * ctx, FLOAT_t ch0, FLOAT_t ch1))
{
	IRQL_t oldIrql;

	target->cb = pfn;
	target->ctx = ctx;
	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	InsertHeadList(& list->head, & target->item);
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void unsubscribefloat(deliverylist_t * list, subscribefloat_t * target)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	RemoveEntryList(& target->item);
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void subscribeint32(deliverylist_t * list, subscribeint32_t * target, void * ctx, void (* pfn)(void * ctx, int_fast32_t ch0, int_fast32_t ch1))
{
	IRQL_t oldIrql;

	target->cb = pfn;
	target->ctx = ctx;
	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	InsertHeadList(& list->head, & target->item);
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void unsubscribeint32(deliverylist_t * list, subscribeint32_t * target)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	RemoveEntryList(& target->item);
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void deliverylist_initialize(deliverylist_t * list, IRQL_t irqlv)
{
	InitializeListHead(& list->head);
	IRQLSPINLOCK_INITIALIZE(& list->listlock, irqlv);
}

#endif /* WITHINTEGRATEDDSP */

// инициализация системы буферов
void buffers_initialize(void)
{
#if WITHBUFFERSDEBUG
	ticker_initialize(& buffticker, 1, buffers_spool, NULL);
	ticker_add(& buffticker);
#endif /* WITHBUFFERSDEBUG */

#if WITHINTEGRATEDDSP

	deliverylist_initialize(& rtstargetsint, IRQL_REALTIME);
	deliverylist_initialize(& speexoutfloat, IRQL_REALTIME);
	deliverylist_initialize(& afdemodoutfloat, IRQL_REALTIME);


#if WITHUSBHEADSET || WITHSKIPUSERMODE || CTLSTYLE_V3D

	// Обход user mode шумоподавителя
	static subscribefloat_t afsample16register;
	subscribefloat(& afdemodoutfloat, & afsample16register, NULL, savesampleout16stereo_float);

#else /* WITHSKIPUSERMODE */

	static subscribefloat_t afsample16register;
	static subscribefloat_t afsample16registertospeex;

	subscribefloat(& speexoutfloat, & afsample16register, NULL, savesampleout16stereo_float);
	subscribefloat(& afdemodoutfloat, & afsample16registertospeex, NULL, savesampleout16tospeex);

#endif /* WITHSKIPUSERMODE */

	{
		unsigned i;
		enum { NVCOICESFREE16RX = (2 * MIKELEVEL + 1 * RESAMPLE16NORMAL) * 2 * BUFOVERSIZE };
		static RAMBIGDTCM_MDMA ALIGNX_BEGIN voice16rx_t voicesarray16rx [NVCOICESFREE16RX] ALIGNX_END;

		InitializeListHead3(& resample16rx, RESAMPLE16NORMAL);	// буферы от USB для синхронизации

		InitializeListHead3(& voicesmike16rx, VOICESMIKE16NORMAL);	// список оцифрованных с АЦП кодека
		InitializeListHead3(& voicesusb16rx, VOICESMIKE16NORMAL);	// список оцифрованных после USB ресэмплинга
		InitializeListHead2(& voicesfree16rx);	// Незаполненные
		for (i = 0; i < (sizeof voicesarray16rx / sizeof voicesarray16rx [0]); ++ i)
		{
			voice16rx_t * const p = & voicesarray16rx [i];
			p->tag2 = p;
			p->tag3 = p;
			InsertHeadList2(& voicesfree16rx, & p->item);
		}
		LCLSPINLOCK_INITIALIZE(& locklist16rx);

	}
	{
		unsigned i;
		// Могут быть преобразованы до двух буферов шумоподавителя при нормальной работе
		enum { NVCOICESFREE16TX = (16 * PHONESLEVEL + 16 * (FIRBUFSIZE + CNT16TX - 1) / CNT16TX) * BUFOVERSIZE };
		static RAMBIGDTCM_MDMA ALIGNX_BEGIN voice16tx_t voicesarray16tx [NVCOICESFREE16TX] ALIGNX_END;

		InitializeListHead3(& voicesphones16tx, 3 * DMABUFSCALE);	// список для выдачи на ЦАП кодека
		InitializeListHead3(& voicesmoni16tx, 2 * DMABUFSCALE);	// самопрослушивание
		InitializeListHead2(& voicesfree16tx);	// Незаполненные
		for (i = 0; i < (sizeof voicesarray16tx / sizeof voicesarray16tx [0]); ++ i)
		{
			voice16tx_t * const p = & voicesarray16tx [i];
			p->tag2 = p;
			p->tag3 = p;
			InsertHeadList2(& voicesfree16tx, & p->item);
		}
		LCLSPINLOCK_INITIALIZE(& locklist16tx);
	}
#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)
	{
		unsigned i;
		static RAMBIGDTCM_MDMA uacin48_t uacin48array [24 * BUFOVERSIZE];

		InitializeListHead2(& uacin48free);	// Незаполненные
		InitializeListHead2(& uacin48ready);	// список для выдачи в канал USB AUDIO

		for (i = 0; i < (sizeof uacin48array / sizeof uacin48array [0]); ++ i)
		{
			uacin48_t * const p = & uacin48array [i];
			p->tag = BUFFTAG_UACIN48;
			p->tag2 = p;
			p->tag3 = p;
			InsertHeadList2(& uacin48free, & p->item);
		}
		LCLSPINLOCK_INITIALIZE(& locklistuacin48);

	}
#endif /* WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) */

#if WITHUSBUACOUT && 0
	{
		unsigned i;
		static RAMBIGDTCM_MDMA uacout48_t uacout48array [5 * BUFOVERSIZE];

		InitializeListHead2(& uacout48free);	// Незаполненные
		InitializeListHead2(& uacout48ready);	// список для выдачи в канал USB AUDIO

		for (i = 0; i < (sizeof uacout48array / sizeof uacout48array [0]); ++ i)
		{
			uacout48_t * const p = & uacout48array [i];
			p->tag2 = p;
			p->tag3 = p;
			InsertHeadList2(& uacout48free, & p->item);
		}
		LCLSPINLOCK_INITIALIZE(& locklistuacout48);

	}
#endif /* WITHUSBUACOUT */

	//ASSERT((DMABUFFSIZE_UACIN % HARDWARE_RTSDMABYTES) == 0);
//	ASSERT((DMABUFFSIZE192RTS % HARDWARE_RTSDMABYTES) == 0);
//	ASSERT((DMABUFFSIZE96RTS % HARDWARE_RTSDMABYTES) == 0);

	#if WITHRTS192 && WITHUSBUACIN && WITHUSBHW && defined (WITHUSBHW_DEVICE)
	{
		unsigned i;
		RAMBIG static RAM_D1 voice192rts_t voicesarray192rts [4 * BUFOVERSIZE];

		ASSERT(offsetof(uacin48_t, item) == offsetof(voice192rts_t, item));
		ASSERT(offsetof(uacin48_t, u.buff) == offsetof(voice192rts_t, u.buff));
		ASSERT(offsetof(uacin48_t, tag) == offsetof(voice192rts_t, tag));

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
		LCLSPINLOCK_INITIALIZE(& locklistrts);
		subscribeint32(& rtstargetsint, & uacinrtssubscribe, NULL, savesampleout192stereo);

	}
	#elif WITHRTS96 && WITHUSBUACIN && WITHUSBHW && defined (WITHUSBHW_DEVICE)
	{
		unsigned i;
		static RAMBIGDTCM_MDMA ALIGNX_BEGIN voice96rts_t voicesarray96rts [14 * BUFOVERSIZE] ALIGNX_END;

		ASSERT(offsetof(uacin48_t, item) == offsetof(voice96rts_t, item));
		ASSERT(offsetof(uacin48_t, u.buff) == offsetof(voice96rts_t, u.buff));
		ASSERT(offsetof(uacin48_t, tag) == offsetof(voice96rts_t, tag));

		InitializeListHead2(& uacinrts96ready);		// список для выдачи в канал USB AUDIO - спектр
		InitializeListHead2(& uacinrts96free);	// Незаполненные
		for (i = 0; i < (sizeof voicesarray96rts / sizeof voicesarray96rts [0]); ++ i)
		{
			voice96rts_t * const p = & voicesarray96rts [i];
			p->tag = BUFFTAG_RTS96;
			p->tag2 = p;
			p->tag3 = p;
			//PRINTF("Add p=%p, tag=%d, tag2=%p, tag3=%p\n", p, p->tag, p->tag2, p->tag3);
			InsertHeadList2(& uacinrts96free, & p->item);
		}
		LCLSPINLOCK_INITIALIZE(& locklistrts);
		subscribeint32(& rtstargetsint, & uacinrtssubscribe, NULL, savesampleout96stereo);

	}
	#endif /* WITHRTS192 */
	{
		unsigned i;
		static RAMBIGDTCM_MDMA ALIGNX_BEGIN voice32tx_t voicesarray32tx [6 * BUFOVERSIZE] ALIGNX_END;

		InitializeListHead2(& voicesready32tx);	// список для выдачи на ЦАП
		InitializeListHead2(& voicesfree32tx);	// Незаполненные
		for (i = 0; i < (sizeof voicesarray32tx / sizeof voicesarray32tx [0]); ++ i)
		{
			voice32tx_t * const p = & voicesarray32tx [i];
			p->tag2 = p;
			p->tag3 = p;
			InsertHeadList2(& voicesfree32tx, & p->item);
		}
		LCLSPINLOCK_INITIALIZE(& locklist32tx);
	}
	{
		unsigned i;
		// +2 - для отладочной печати содеожимого буферов
	    static RAMBIGDTCM_MDMA ALIGNX_BEGIN voice32rx_t voicesarray32rx [2 + 6 * BUFOVERSIZE] ALIGNX_END;	// без WFM надо 2

		InitializeListHead2(& voicesfree32rx);	// Незаполненные
		for (i = 0; i < (sizeof voicesarray32rx / sizeof voicesarray32rx [0]); ++ i)
		{
			voice32rx_t * const p = & voicesarray32rx [i];
			InsertHeadList2(& voicesfree32rx, & p->item);
		}
		LCLSPINLOCK_INITIALIZE(& locklist32rx);
	}


#if WITHUSEAUDIOREC

	{
	#if CPUSTYLE_R7S721
		static RAM_D1 recordswav48_t recordsarray16 [8];
	#elif defined (STM32F767xx)
		static RAM_D1 recordswav48_t recordsarray16 [8];
	#elif defined (STM32F746xx)
		static RAM_D1 recordswav48_t recordsarray16 [8];
	#elif defined (STM32F429xx)
		static RAM_D1 recordswav48_t recordsarray16 [8];
	#elif defined (STM32H743xx)
		static RAM_D1 recordswav48_t recordsarray16 [5];
	#else
		static RAM_D1 recordswav48_t recordsarray16 [8 * BUFOVERSIZE];
	#endif
		unsigned i;

		/* Подготовка буферов для записи на SD CARD */
		InitializeListHead2(& recordswav48ready);	// Заполненные - готовые для записи на SD CARD
		InitializeListHead2(& recordswav48free);	// Незаполненные
		for (i = 0; i < (sizeof recordsarray16 / sizeof recordsarray16 [0]); ++ i)
		{
			recordswav48_t * const p = & recordsarray16 [i];
			InsertHeadList2(& recordswav48free, & p->item);
		}
		LCLSPINLOCK_INITIALIZE(& lockwav48);
	}

#endif /* WITHUSEAUDIOREC */

#if WITHMODEM
	{
		unsigned i;
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
	}
#endif /* WITHMODEM */

#if 1
	{
		unsigned i;
		static RAMBIGDTCM denoise16_t speexarray16 [5];	// буферы: один заполняется, один воспроизводлится и два своюбодных (с одинм бывают пропуски).

		InitializeListHead2(& speexfree16);	// Незаполненные
		InitializeListHead2(& speexready16);	// Для обработки

		for (i = 0; i < (sizeof speexarray16 / sizeof speexarray16 [0]); ++ i)
		{
			denoise16_t * const p = & speexarray16 [i];
			InsertHeadList2(& speexfree16, & p->item);
		}
		LCLSPINLOCK_INITIALIZE(& speexlock);
	}
#endif
#endif /* WITHINTEGRATEDDSP */

#if 0
	{
		unsigned i;
		/* Cообщения от уровня обработчиков прерываний к user-level функциям. */
		static RAMBIGDTCM message_t messagesarray8 [12];

		/* Подготовка буферов для обмена с модемом */
		InitializeListHead(& msgsready8);	// Заполненные - готовые к обработке
		InitializeListHead(& msgsfree8);	// Незаполненные
		for (i = 0; i < (sizeof messagesarray8 / sizeof messagesarray8 [0]); ++ i)
		{
			message_t * const p = & messagesarray8 [i];
			p->tag2 = p;
			p->tag3 = p;
			//InitializeListHead2(& p->item);
			InsertHeadVList(& msgsfree8, & p->item);
		}
		LCLSPINLOCK_INITIALIZE(& locklistmsg8);
	}
#endif
	// Инициализация ресэмплеров

}

