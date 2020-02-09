/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "list.h"
#include "formats.h"	// for debug prints

//#define WITHBUFFERSDEBUG WITHDEBUG

#if WITHINTEGRATEDDSP

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
		LIST_ENTRY item0;
		uint_fast8_t Count;	// количество элментов в списке
	} LIST_ENTRY2, * PLIST_ENTRY2;

	#define LIST2PRINT(name) do { \
			debug_printf_P(PSTR(# name "[%3d] "), (int) name.Count); \
		} while (0)

	__STATIC_INLINE int
	IsListEmpty2(const LIST_ENTRY2 * ListHead)
	{
		return (ListHead)->Count == 0;
		//return IsListEmpty(& (ListHead)->item0);
	}

	__STATIC_INLINE void
	InitializeListHead2(LIST_ENTRY2 * ListHead)
	{
		(ListHead)->Count = 0;
		InitializeListHead(& (ListHead)->item0);
	}

	__STATIC_INLINE void
	InsertHeadList2(PLIST_ENTRY2 ListHead, PLIST_ENTRY Entry)
	{
		(ListHead)->Count += 1;
		InsertHeadList(& (ListHead)->item0, (Entry));
	}

	__STATIC_INLINE PLIST_ENTRY
	RemoveTailList2(PLIST_ENTRY2 ListHead)
	{
		(ListHead)->Count -= 1;
		const PLIST_ENTRY t = RemoveTailList(& (ListHead)->item0);	/* прямо вернуть значение RemoveTailList нельзя - Microsoft сделал не совсем правильный макрос. Но по другому и не плучилось бы в стандартном языке C. */
		return t;
	}

	__STATIC_INLINE uint_fast8_t GetCountList2(const LIST_ENTRY2 * ListHead)
	{
		return (ListHead)->Count;
	}

	/* готовность буферов с "гистерезисом". */
	__STATIC_INLINE uint_fast8_t fiforeadyupdate(
		uint_fast8_t ready,		// текущее состояние готовности
		uint_fast8_t Count,		// сколько элементов сейчас в очереди
		uint_fast8_t normal		// граница включения готовности
		)
	{
		return ready ? Count != 0 : Count >= normal;
	}


	/* отладочные врапперы для функций работы со списком - позволяют получить размер очереди */
	typedef struct listcnt3
	{
		LIST_ENTRY2 item2;
		uint_fast8_t RdyLevel;	// Требуемое количество
		uint_fast8_t Rdy;		// количество элментов в списке
	} LIST_ENTRY3, * PLIST_ENTRY3;

	__STATIC_INLINE int
	IsListEmpty3(const LIST_ENTRY3 * ListHead)
	{
		return IsListEmpty2(& (ListHead)->item2);
	}

	__STATIC_INLINE void
	InitializeListHead3(LIST_ENTRY3 * ListHead, uint_fast8_t RdyLevel)
	{
		(ListHead)->Rdy = 0;
		(ListHead)->RdyLevel = RdyLevel;
		InitializeListHead2(& (ListHead)->item2);
	}

	// forceReady - если в источнике данных закончился поток.
	__STATIC_INLINE void
	InsertHeadList3(PLIST_ENTRY3 ListHead, PLIST_ENTRY Entry, uint_fast8_t forceReady)
	{
		InsertHeadList2(& (ListHead)->item2, (Entry));
		(ListHead)->Rdy = forceReady || fiforeadyupdate((ListHead)->Rdy, (ListHead)->item2.Count, (ListHead)->RdyLevel);
	}

	__STATIC_INLINE PLIST_ENTRY
	RemoveTailList3(PLIST_ENTRY3 ListHead)
	{
		const PLIST_ENTRY t = RemoveTailList2(& (ListHead)->item2);	/* прямо вернуть значение RemoveTailList нельзя - Microsoft сделал не совсем правильный макрос. Но по другому и не плучилось бы в стандартном языке C. */
		(ListHead)->Rdy = fiforeadyupdate((ListHead)->Rdy, (ListHead)->item2.Count, (ListHead)->RdyLevel);
		return t;
	}

	__STATIC_INLINE uint_fast8_t GetCountList3(const LIST_ENTRY3 * ListHead)
	{
		return GetCountList2(& (ListHead)->item2);
	}

	__STATIC_INLINE uint_fast8_t GetReadyList3(const LIST_ENTRY3 * ListHead)
	{
		return (ListHead)->Rdy;
	}

	#define LIST3PRINT(name) do { \
			debug_printf_P(PSTR(# name "[%3d] "), (int) GetCountList3(& (name))); \
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

	void vfylist(LIST_ENTRY2 * head)
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
		LIST_ENTRY item;
		ALIGNX_BEGIN int16_t buff [DMABUFFSIZE16] ALIGNX_END;
	} ALIGNX_END voice16_t;

	// I/Q data to FPGA or IF CODEC
	typedef ALIGNX_BEGIN struct voices32tx_tag
	{
		LIST_ENTRY item;
		ALIGNX_BEGIN int32_t buff [DMABUFFSIZE32TX] ALIGNX_END;
	} ALIGNX_END voice32tx_t;

	// I/Q data from FPGA or IF CODEC
	typedef ALIGNX_BEGIN struct voices32rx_tag
	{
		LIST_ENTRY item;
		ALIGNX_BEGIN int32_t buff [DMABUFFSIZE32RX] ALIGNX_END;
	} ALIGNX_END voice32rx_t;
	// исправляемая погрешность = 0.02% - один сэмпл добавить/убрать на 5000 сэмплов

	enum { SKIPPED = 5000 / (DMABUFFSIZE16 / DMABUFSTEP16) };
	enum { VOICESMIKE16NORMAL = 5 };	// Нормальное количество буферов в очереди
	enum { RESAMPLE16NORMAL = SKIPPED * 2 };	// Нормальное количество буферов в очереди

	static RAMDTCM LIST_ENTRY3 voicesmike16;	// буферы с оцифрованными звуками с микрофона/Line in
	static RAMDTCM LIST_ENTRY3 resample16;		// буферы от USB для синхронизации

	static RAMDTCM LIST_ENTRY2 voicesfree16;
	static RAMDTCM LIST_ENTRY2 voicesphones16;	// буферы, предназначенные для выдачи на наушники
	static RAMDTCM LIST_ENTRY2 voicesmoni16;	// буферы, предназначенные для звука самоконтроля

	static RAMDTCM LIST_ENTRY2 voicesready32tx;	// буферы, предназначенные для выдачи на IF DAC
	static RAMDTCM LIST_ENTRY2 voicesfree32tx;
	static RAMDTCM LIST_ENTRY2 voicesfree32rx;

	static RAMDTCM LIST_ENTRY2 speexfree16;		// Свободные буферы
	static RAMDTCM LIST_ENTRY2 speexready16;	// Буферы для обработки speex
	//static int speexready16enable;

	static RAMDTCM volatile uint_fast8_t uacoutplayer = 0;	/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
	static RAMDTCM volatile uint_fast8_t uacoutmike = 0;	/* на вход трансивера берутся аудиоданные с USB виртуальной платы, а не с микрофона */
	static RAMDTCM volatile uint_fast8_t uacinalt = UACINALT_NONE;		/* выбор альтернативной конфигурации для UAC IN interface */
	static RAMDTCM volatile uint_fast8_t uacinrtsalt = UACINRTSALT_NONE;		/* выбор альтернативной конфигурации для RTS UAC IN interface */
	static RAMDTCM volatile uint_fast8_t uacoutalt;

#if 1//WITHUSBUAC

	// USB AUDIO IN
	typedef ALIGNX_BEGIN struct uacin16_tag
	{
		LIST_ENTRY item;	// layout should be same in uacin16_t, voice96rts_t and voice192rts_t
		uint_fast8_t tag;	// layout should be same in uacin16_t, voice96rts_t and voice192rts_t
		ALIGNX_BEGIN uint16_t buff [DMABUFFSIZEUACIN16] ALIGNX_END;
	} ALIGNX_END uacin16_t;

	#if WITHRTS192

		typedef ALIGNX_BEGIN struct voices192rts
		{
			LIST_ENTRY item;	// layout should be same in uacin16_t, voice96rts_t and voice192rts_t
			uint_fast8_t tag;	// layout should be same in uacin16_t, voice96rts_t and voice192rts_t
			ALIGNX_BEGIN uint8_t buff [DMABUFFSIZE192RTS] ALIGNX_END;		// спектр, 2*24*192 kS/S
		} ALIGNX_END voice192rts_t;

		static RAMDTCM LIST_ENTRY2 voicesfree192rts;
		static RAMDTCM LIST_ENTRY2 uacin192rts;	// Буферы для записи в вудиоканал USB к компьютеру спектра, 2*32*192 kS/S
	
	#endif /* WITHRTS192 */

	#if WITHRTS96

		typedef ALIGNX_BEGIN struct voices96rts
		{
			LIST_ENTRY item;	// layout should be same in uacin16_t, voice96rts_t and voice192rts_t
			uint_fast8_t tag;	// layout should be same in uacin16_t, voice96rts_t and voice192rts_t
			ALIGNX_BEGIN uint8_t buff [DMABUFFSIZE96RTS] ALIGNX_END;		// спектр, 2*24*192 kS/S
		} ALIGNX_END voice96rts_t;

		static RAMDTCM LIST_ENTRY2 voicesfree96rts;
		static RAMDTCM LIST_ENTRY2 uacin96rts;	// Буферы для записи в вудиоканал USB к компьютер спектра, 2*32*192 kS/S
	
	#endif /* WITHRTS96 */

	static RAMDTCM LIST_ENTRY2 uacinfree16;
	static RAMDTCM LIST_ENTRY2 uacinready16;	// Буферы для записи в вудиоканал USB к компьютер 2*16*24 kS/S

#endif /* WITHUSBUAC */

#endif /* WITHINTEGRATEDDSP */

#if WITHUSEAUDIOREC

	typedef ALIGNX_BEGIN struct records16
	{
		LIST_ENTRY item;
		ALIGNX_BEGIN int16_t buff [AUDIORECBUFFSIZE16] ALIGNX_END;
		unsigned startdata;	// data start
		unsigned topdata;	// index after last element
	} ALIGNX_END records16_t;

	static RAMDTCM LIST_ENTRY2 recordsfree16;		// Свободные буферы
	static RAMDTCM LIST_ENTRY2 recordsready16;	// Буферы для записи на SD CARD

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

static RAMDTCM LIST_ENTRY2 modemsfree8;		// Свободные буферы
static RAMDTCM LIST_ENTRY2 modemsrx8;	// Буферы с принятымти через модем данными
//static LIST_ENTRY modemstx8;	// Буферы с данными для передачи через модем

#endif /* WITHMODEM */

/* Cообщения от уровня обработчиков прерываний к user-level функциям. */

typedef struct message
{
	LIST_ENTRY item;
	uint8_t type;
	uint8_t data [MSGBUFFERSIZE8];
} message_t;

static RAMDTCM LIST_ENTRY msgsfree8;		// Свободные буферы
static RAMDTCM LIST_ENTRY msgsready8;		// Заполненные - готовые к обработке

#if WITHBUFFERSDEBUG
	static volatile unsigned n1, n1wfm, n2, n3, n4, n5, n6;
	static volatile unsigned e1, e2, e3, e4, e5, e6, e7;
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

	/* Normal processing (with DIGI mode and RTS):
	voicesfree16[  9] voicesmike16[  1] voicesphones16[  1] voicesfree96rts[  5] uacin96rts[  0] uacin16[  0] resample16[  5]
	voicesfree16[ 10] voicesmike16[  0] voicesphones16[  1] voicesfree96rts[  6] uacin96rts[  0] uacin16[  0] resample16[  6]
	voicesfree16[  9] voicesmike16[  0] voicesphones16[  1] voicesfree96rts[  6] uacin96rts[  0] uacin16[  0] resample16[  6]
	*/

	LIST2PRINT(speexfree16);
	LIST2PRINT(speexready16);
	LIST2PRINT(voicesfree16);
	LIST3PRINT(voicesmike16);
	LIST2PRINT(voicesphones16);
	LIST2PRINT(voicesmoni16);

	#if WITHUSBUAC
		#if WITHRTS192
			//LIST2PRINT(voicesfree192rts);
			//LIST2PRINT(uacin192rts);
		#elif WITHRTS96
			//LIST2PRINT(voicesfree96rts);
			//LIST2PRINT(uacin96rts);
		#endif
		//LIST2PRINT(uacinfree16);
		//LIST2PRINT(uacinready16);
		LIST3PRINT(resample16);
	#endif /* WITHUSBUAC */

	debug_printf_P(PSTR(" add=%u, del=%u, zero=%u, "), nbadd, nbdel, nbzero);
		
	debug_printf_P(PSTR("\n"));

#endif

#if 1 && WITHDEBUG && WITHINTEGRATEDDSP && WITHBUFFERSDEBUG
	debug_printf_P(PSTR("n1=%u n1wfm=%u n2=%u n3=%u n4=%u n5=%u n6=%u\n"), n1, n1wfm, n2, n3, n4, n5, n6);
	debug_printf_P(PSTR("e1=%u e2=%u e3=%u e4=%u e5=%u e6=%u e7=%u uacinalt=%d\n"), e1, e2, e3, e4, e5, e6, e7, uacinalt);

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

		debug_printf_P(PSTR("uacout=%u, uacin=%u, mikeadc=%u, phonesdac=%u, rtsadc=%u rx32adc=%u rx32wfm=%u tx32dac=%u\n"), 
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



#if 0 && WITHBUFFERSDEBUG

	typedef struct
	{
		uint8_t lock;
		int line;
		const char * file;
	} LOCK_T;

	static RAMDTCM volatile LOCK_T locklist16;
	static RAMDTCM volatile LOCK_T locklist16ststem;
	static RAMDTCM volatile LOCK_T locklist32;
	static RAMDTCM volatile LOCK_T locklist8;

	static void lock_impl(volatile LOCK_T * p, int line, const char * file, const char * variable)
	{
	#if WITHHARDINTERLOCK
		uint8_t r;
		do
			r = __LDREXB(& p->lock);
		while (__STREXB(1, & p->lock));
		if (r != 0)
		{
			debug_printf_P(PSTR("LOCK @%p %s already locked at %d in %s by %d in %s\n"), p, variable, line, file, p->line, p->file);
			for (;;)
				;
		}
		else
		{
			p->file = file;
			p->line = line;
		}
	#endif /* WITHHARDINTERLOCK */
	}

	static void unlock_impl(volatile LOCK_T * p, int line, const char * file, const char * variable)
	{
	#if WITHHARDINTERLOCK
		uint8_t r;
		do
			r = __LDREXB(& p->lock);
		while (__STREXB(0, & p->lock));
		if (r == 0)
		{
			debug_printf_P(PSTR("LOCK @%p %s already unlocked at %d in %s by %d in %s\n"), p, variable, line, file, p->line, p->file);
			for (;;)
				;
		}
		else
		{
			p->file = file;
			p->line = line;
		}
	#endif /* WITHHARDINTERLOCK */
	}

	#define LOCK(p) do { lock_impl((p), __LINE__, __FILE__, # p); } while (0)
	#define UNLOCK(p) do { unlock_impl((p), __LINE__, __FILE__, # p); } while (0)

#else /* WITHBUFFERSDEBUG */

	#define LOCK(p) do {  } while (0)
	#define UNLOCK(p) do {  } while (0)

#endif /* WITHBUFFERSDEBUG */

#if WITHINTEGRATEDDSP

typedef ALIGNX_BEGIN struct denoise16
{
	LIST_ENTRY item;
	ALIGNX_BEGIN speexel_t buff [NTRX * FIRBUFSIZE] ALIGNX_END;
} ALIGNX_END denoise16_t;

// Буферы с принятымти от обработчиков прерываний сообщениями
uint_fast8_t takespeexready_user(speexel_t * * dest)
{
	ASSERT_IRQL_USER();
	global_disableIRQ();

	//if (speexready16enable == 0)
	//{
	//	speexready16enable = speexready16.Count > 1;
	//}
	if (/*speexready16enable && */ ! IsListEmpty2(& speexready16))
	{
		PLIST_ENTRY t = RemoveTailList2(& speexready16);
		global_enableIRQ();
		denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, item);
		* dest = p->buff;
		return 1;
	}
	global_enableIRQ();
	return 0;
}

// Освобождение обработанного буфера сообщения
void releasespeexbuffer_user(speexel_t * t)
{
	ASSERT_IRQL_USER();
	denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, buff);
	global_disableIRQ();
	InsertHeadList2(& speexfree16, & p->item);
	global_enableIRQ();
}


denoise16_t * allocate_dmabuffer16denoise(void)
{
	if (! IsListEmpty2(& speexfree16))
	{
		PLIST_ENTRY t = RemoveTailList2(& speexfree16);
		denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, item);
		return p;
	}
#if WITHBUFFERSDEBUG
	++ e7;
#endif /* WITHBUFFERSDEBUG */
	//debug_printf_P(PSTR("allocate_dmabuffer16denoise() failure\n"));
	if (! IsListEmpty2(& speexready16))
	{
		PLIST_ENTRY t = RemoveTailList2(& speexready16);
		denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, item);
		return p;
	}
	debug_printf_P(PSTR("allocate_dmabuffer16denoise() failure\n"));
	ASSERT(0);
	for (;;)
		;
	return 0;
}

void savesampleout16tospeex(speexel_t ch0, speexel_t ch1)
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
		InsertHeadList2(& speexready16, & p->item);
		p = NULL;
	}
}
#endif /* WITHINTEGRATEDDSP */

// инициализация системы буферов
void buffers_initialize(void)
{
#if WITHBUFFERSDEBUG
	ticker_initialize(& buffticker, 1, buffers_spool, NULL);
#endif /* WITHBUFFERSDEBUG */
	unsigned i;

#if WITHINTEGRATEDDSP

	static voice16_t voicesarray16 [192];

	InitializeListHead3(& resample16, RESAMPLE16NORMAL);	// буферы от USB для синхронизации
	InitializeListHead3(& voicesmike16, VOICESMIKE16NORMAL);	// список оцифрованных с АЦП
	InitializeListHead2(& voicesphones16);	// список для выдачи на ЦАП
	InitializeListHead2(& voicesmoni16);	// самоконтроль
	InitializeListHead2(& voicesfree16);	// Незаполненные
	for (i = 0; i < (sizeof voicesarray16 / sizeof voicesarray16 [0]); ++ i)
	{
		voice16_t * const p = & voicesarray16 [i];
		InsertHeadList2(& voicesfree16, & p->item);
	}

#if WITHUSBUAC

	static uacin16_t uacinarray16 [24];

	InitializeListHead2(& uacinfree16);	// Незаполненные
	InitializeListHead2(& uacinready16);	// список для выдачи в канал USB AUDIO

	for (i = 0; i < (sizeof uacinarray16 / sizeof uacinarray16 [0]); ++ i)
	{
		uacin16_t * const p = & uacinarray16 [i];
		p->tag = BUFFTAG_UACIN16;
		InsertHeadList2(& uacinfree16, & p->item);
	}

	//ASSERT((DMABUFFSIZEUACIN16 % HARDWARE_RTSDMABYTES) == 0);
	ASSERT((DMABUFFSIZE192RTS % HARDWARE_RTSDMABYTES) == 0);
	ASSERT((DMABUFFSIZE96RTS % HARDWARE_RTSDMABYTES) == 0);

	#if WITHRTS192

		RAMNOINIT_D1 static voice192rts_t voicesarray192rts [4];

		ASSERT(offsetof(uacin16_t, item) == offsetof(voice96rts_t, item));
		ASSERT(offsetof(uacin16_t, buff) == offsetof(voice96rts_t, buff));
		ASSERT(offsetof(uacin16_t, tag) == offsetof(voice96rts_t, tag));

		InitializeListHead2(& uacin192rts);		// список для выдачи в канал USB AUDIO - спектр
		InitializeListHead2(& voicesfree192rts);	// Незаполненные
		for (i = 0; i < (sizeof voicesarray192rts / sizeof voicesarray192rts [0]); ++ i)
		{
			voice192rts_t * const p = & voicesarray192rts [i];
			p->tag = BUFFTAG_RTS192;
			InsertHeadList2(& voicesfree192rts, & p->item);
		}

	#endif /* WITHRTS192 */

	#if WITHRTS96

		RAMNOINIT_D1 static voice96rts_t voicesarray96rts [4];

		ASSERT(offsetof(uacin16_t, item) == offsetof(voice96rts_t, item));
		ASSERT(offsetof(uacin16_t, buff) == offsetof(voice96rts_t, buff));
		ASSERT(offsetof(uacin16_t, tag) == offsetof(voice96rts_t, tag));

		InitializeListHead2(& uacin96rts);		// список для выдачи в канал USB AUDIO - спектр
		InitializeListHead2(& voicesfree96rts);	// Незаполненные
		for (i = 0; i < (sizeof voicesarray96rts / sizeof voicesarray96rts [0]); ++ i)
		{
			voice96rts_t * const p = & voicesarray96rts [i];
			p->tag = BUFFTAG_RTS96;
			InsertHeadList2(& voicesfree96rts, & p->item);
		}

	#endif /* WITHRTS192 */

#endif /* WITHUSBUAC */

	static voice32tx_t voicesarray32tx [(DMABUFFSIZE32RX / DMABUFSTEP32RX) / (DMABUFFSIZE32TX / DMABUFSTEP32TX) + 4];

	InitializeListHead2(& voicesready32tx);	// список для выдачи на ЦАП
	InitializeListHead2(& voicesfree32tx);	// Незаполненные
	for (i = 0; i < (sizeof voicesarray32tx / sizeof voicesarray32tx [0]); ++ i)
	{
		voice32tx_t * const p = & voicesarray32tx [i];
		InsertHeadList2(& voicesfree32tx, & p->item);
	}

	static voice32rx_t voicesarray32rx [6];	// без WFM надо 2

	InitializeListHead2(& voicesfree32rx);	// Незаполненные
	for (i = 0; i < (sizeof voicesarray32rx / sizeof voicesarray32rx [0]); ++ i)
	{
		voice32rx_t * const p = & voicesarray32rx [i];
		InsertHeadList2(& voicesfree32rx, & p->item);
	}

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
		/*RAMNOINIT_D1 */ static records16_t recordsarray16 [6];
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

	static RAMDTCM denoise16_t speexarray16 [6];

	InitializeListHead2(& speexfree16);	// Незаполненные
	InitializeListHead2(& speexready16);	// Для обработки

	for (i = 0; i < (sizeof speexarray16 / sizeof speexarray16 [0]); ++ i)
	{
		denoise16_t * const p = & speexarray16 [i];
		InsertHeadList2(& speexfree16, & p->item);
	}

#endif /* WITHINTEGRATEDDSP */

	/* Cообщения от уровня обработчиков прерываний к user-level функциям. */
	static message_t messagesarray8 [12];

	/* Подготовка буферов для обмена с модемом */
	InitializeListHead(& msgsready8);	// Заполненные - готовые к обработке
	InitializeListHead(& msgsfree8);	// Незаполненные
	for (i = 0; i < (sizeof messagesarray8 / sizeof messagesarray8 [0]); ++ i)
	{
		message_t * const p = & messagesarray8 [i];
		//InitializeListHead2(& p->item);
		InsertHeadList(& msgsfree8, & p->item);
	}
}

/* Cообщения от уровня обработчиков прерываний к user-level функциям. */

// Буферы с принятымти от обработчиков прерываний сообщениями
uint_fast8_t takemsgready_user(uint8_t * * dest)
{
	ASSERT_IRQL_USER();
	global_disableIRQ();

	LOCK(& locklist8);
	if (! IsListEmpty(& msgsready8))
	{
		PLIST_ENTRY t = RemoveTailList(& msgsready8);
		UNLOCK(& locklist8);
		global_enableIRQ();
		message_t * const p = CONTAINING_RECORD(t, message_t, item);
		* dest = p->data;
		ASSERT(p->type != MSGT_EMPTY);
		return p->type;
	}
	UNLOCK(& locklist8);
	global_enableIRQ();
	return MSGT_EMPTY;
}


// Освобождение обработанного буфера сообщения
void releasemsgbuffer_user(uint8_t * dest)
{
	ASSERT_IRQL_USER();
	message_t * const p = CONTAINING_RECORD(dest, message_t, data);
	global_disableIRQ();
	LOCK(& locklist8);
	InsertHeadList(& msgsfree8, & p->item);
	UNLOCK(& locklist8);
	global_enableIRQ();
}

// Буфер для формирования сообщения
size_t takemsgbufferfree_low(uint8_t * * dest)
{
	ASSERT_IRQL_SYSTEM();
	LOCK(& locklist8);
	if (! IsListEmpty(& msgsfree8))
	{
		PLIST_ENTRY t = RemoveTailList(& msgsfree8);
		UNLOCK(& locklist8);
		message_t * const p = CONTAINING_RECORD(t, message_t, item);
		* dest = p->data;
		return (MSGBUFFERSIZE8 * sizeof p->data [0]);
	}
	UNLOCK(& locklist8);
	return 0;
}

// поместить сообщение в очередь к исполнению
void placesemsgbuffer_low(uint_fast8_t type, uint8_t * dest)
{
	ASSERT_IRQL_SYSTEM();
	ASSERT(type != MSGT_EMPTY);
	message_t * p = CONTAINING_RECORD(dest, message_t, data);
	p->type = type;
	LOCK(& locklist8);
	InsertHeadList(& msgsready8, & p->item);
	UNLOCK(& locklist8);
}


#if WITHINTEGRATEDDSP


// Сохранить звук на звуковой выход трансивера
static RAMFUNC void buffers_tophones16(voice16_t * p)
{
	LOCK(& locklist16);
	InsertHeadList2(& voicesphones16, & p->item);
	UNLOCK(& locklist16);
}
// Сохранить звук для самоклнтроля на звуковой выход трансивера
static RAMFUNC void buffers_tomoni16(voice16_t * p)
{
	LOCK(& locklist16);
	InsertHeadList2(& voicesmoni16, & p->item);
	UNLOCK(& locklist16);
}

// Сохранить звук в никуда...
static RAMFUNC void buffers_tonull16(voice16_t * p)
{
	LOCK(& locklist16);
	InsertHeadList2(& voicesfree16, & p->item);
	UNLOCK(& locklist16);
}

// Сохранить USB UAC IN буфер в никуда...
static RAMFUNC void buffers_tonulluacin(uacin16_t * p)
{
	LOCK(& locklist16);
	InsertHeadList2(& uacinfree16, & p->item);
	UNLOCK(& locklist16);
}

// Сохранить звук на вход передатчика
static RAMFUNC void buffers_tomodulators16(voice16_t * p)
{
	LOCK(& locklist16);
	InsertHeadList3(& voicesmike16, & p->item, 0);
	UNLOCK(& locklist16);
}

// Сохранить звук от АЦП пикрофона
static RAMFUNC void buffers_savefrommikeadc(voice16_t * p)
{

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
	if (uacoutplayer != 0)
		buffers_tonull16(p);
	else
		buffers_tophones16(p);
}


static RAMFUNC void
buffers_savefrommoni(voice16_t * p)
{
	buffers_tomoni16(p);
}

#if WITHUSBUAC
// приняли данные от синхронизатора
static RAMFUNC void
buffers_savefromresampling(voice16_t * p)
{
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
	LOCK(& locklist16);
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
	UNLOCK(& locklist16);
}

// приняли данные от USB AUDIO
static RAMFUNC void
buffers_savefromuacout(voice16_t * p)
{
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
RAMFUNC uint_fast8_t getsampmlemike(INT32P_t * v)
{
	enum { L, R };
	static voice16_t * RAMDTCM p = NULL;
	static RAMDTCM unsigned pos = 0;	// позиция по выходному количеству
	const unsigned CNT = (DMABUFFSIZE16 / DMABUFSTEP16);	// фиксированное число сэмплов во входном буфере

	LOCK(& locklist16);
	if (p == NULL)
	{
		if (GetReadyList3(& voicesmike16))
		{
			PLIST_ENTRY t = RemoveTailList3(& voicesmike16);
			p = CONTAINING_RECORD(t, voice16_t, item);
			UNLOCK(& locklist16);
			pos = 0;
		}
		else
		{
			// Микрофонный кодек ещё не успел начать работать - возвращаем 0.
			UNLOCK(& locklist16);
			return 0;
		}
	}
	else
	{
		UNLOCK(& locklist16);
	}

	// Использование данных.
	v->ivqv [L] = (int16_t) p->buff [pos * DMABUFSTEP16 + L];	// микрофон или левый канал
	v->ivqv [R] = (int16_t) p->buff [pos * DMABUFSTEP16 + R];	// правый канал

	if (++ pos >= CNT)
	{
		buffers_aftermodulators(p);
		p = NULL;
	}
	return 1;	
}

// 16 bit, signed
RAMFUNC uint_fast8_t getsampmlemoni(INT32P_t * v)
{
	enum { L, R };
	static voice16_t * RAMDTCM p = NULL;
	static RAMDTCM unsigned pos = 0;	// позиция по выходному количеству
	const unsigned CNT = (DMABUFFSIZE16 / DMABUFSTEP16);	// фиксированное число сэмплов во входном буфере

	LOCK(& locklist16);
	if (p == NULL)
	{
		if (! IsListEmpty2(& voicesmoni16))
		{
			PLIST_ENTRY t = RemoveTailList2(& voicesmoni16);
			p = CONTAINING_RECORD(t, voice16_t, item);
			UNLOCK(& locklist16);
			pos = 0;
		}
		else
		{
			// Микрофонный кодек ещё не успел начать работать - возвращаем 0.
			UNLOCK(& locklist16);
			return 0;
		}
	}
	else
	{
		UNLOCK(& locklist16);
	}

	// Использование данных.
	v->ivqv [L] = (int16_t) p->buff [pos * DMABUFSTEP16 + L];	// левый канал
	v->ivqv [R] = (int16_t) p->buff [pos * DMABUFSTEP16 + R];	// правый канал

	if (++ pos >= CNT)
	{
		buffers_tonull16(p);
		p = NULL;
	}
	return 1;
}

// звук для самоконтроля
void savemoni16stereo(int_fast32_t ch0, int_fast32_t ch1)
{
	// если есть инициализированный канал для выдачи звука
	static voice16_t * p = NULL;
	static unsigned n;

	if (p == NULL)
	{
		uintptr_t addr = allocate_dmabuffer16();
		p = CONTAINING_RECORD(addr, voice16_t, buff);
		n = 0;
	}

	p->buff [n + 0] = ch0;		// sample value
#if DMABUFSTEP16 > 1
	p->buff [n + 1] = ch1;	// sample value
#endif
	n += DMABUFSTEP16;

	if (n >= DMABUFFSIZE16)
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

// UAC IN samplerate
// todo: сделать нормальный расчёт для некруглых значений ARMI2SRATE
int_fast32_t dsp_get_samplerateuacin_audio48(void)
{
	return dsp_get_sampleraterx();
}
// UAC IN samplerate
// todo: сделать нормальный расчёт для некруглых значений ARMI2SRATE
int_fast32_t dsp_get_samplerateuacin_RTS96(void)
{
	return dsp_get_sampleraterxscaled(2);
}

// UAC IN samplerate
// todo: сделать нормальный расчёт для некруглых значений ARMI2SRATE
int_fast32_t dsp_get_samplerateuacin_RTS192(void)
{
	return dsp_get_sampleraterxscaled(4);
}

int_fast32_t dsp_get_samplerateuacin_rts(void)		// RTS samplerate
{
#if WITHRTS192
	return dsp_get_samplerateuacin_RTS192();
#elif WITHRTS96
	return dsp_get_samplerateuacin_RTS96();
#else
	return dsp_get_samplerateuacin_audio48();
#endif
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
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_rtsadc += sizeof p->buff / sizeof p->buff [0] / DMABUFSTEP192RTS;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */

	LOCK(& locklist16);
	InsertHeadList2(& uacin192rts, & p->item);
	UNLOCK(& locklist16);

	refreshDMA_uacin();		// если DMA  остановлено - начать обмен
}

static void buffers_savetonull192rts(voice192rts_t * p)
{
	LOCK(& locklist16);
	InsertHeadList2(& voicesfree192rts, & p->item);
	UNLOCK(& locklist16);
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
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_rtsadc += sizeof p->buff / sizeof p->buff [0] / DMABUFSTEP96RTS;	// в буфере пары сэмплов по три байта
#endif /* WITHBUFFERSDEBUG */
	
	LOCK(& locklist16);
	InsertHeadList2(& uacin96rts, & p->item);
	UNLOCK(& locklist16);

	refreshDMA_uacin();		// если DMA  остановлено - начать обмен
}

static void buffers_savetonull96rts(voice96rts_t * p)
{
	LOCK(& locklist16);
	InsertHeadList2(& voicesfree96rts, & p->item);
	UNLOCK(& locklist16);
}

#endif /* WITHRTS96 && WITHUSBHW && WITHUSBUAC */

// Сохранить буфер сэмплов для передачи в компьютер
static RAMFUNC void
buffers_savetouacin(uacin16_t * p)
{
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_uacin += sizeof p->buff / sizeof p->buff [0] / DMABUFSTEPUACIN16;	// в буфере пары сэмплов по три байта
#endif /* WITHBUFFERSDEBUG */
	LOCK(& locklist16);
	InsertHeadList2(& uacinready16, & p->item);
	UNLOCK(& locklist16);

	refreshDMA_uacin();		// если DMA  остановлено - начать обмен
}

#else

int_fast32_t dsp_get_samplerateuacin_rts(void)		// RTS samplerate
{
	return 48000L;
}

#endif /* WITHUSBUAC */

#if WITHUSBUAC

static RAMFUNC unsigned long ulmin(
	unsigned long a,
	unsigned long b)
{
	return a < b ? a : b;
}

// возвращает количество полученых сэмплов
static RAMFUNC unsigned getsamplemsuacout(
	int16_t * buff,	// текущая позиция в целевом буфере
	unsigned size		// количество оставшихся одиночных сэмплов
	)
{
	static voice16_t * RAMDTCM p = NULL;
	enum { NPARTS = 3 };
	static RAMDTCM uint_fast8_t part = 0;
	static int16_t * RAMDTCM datas [NPARTS] = { NULL, NULL };		// начальный адрес пары сэмплов во входном буфере
	static RAMDTCM unsigned sizes [NPARTS] = { 0, 0 };			// количество сэмплов во входном буфере

	static unsigned skipsense = SKIPPED;

	LOCK(& locklist16);
	if (p == NULL)
	{
		if (GetReadyList3(& resample16) == 0)
		{
#if WITHBUFFERSDEBUG
			++ nbzero;
#endif /* WITHBUFFERSDEBUG */
			// Микрофонный кодек ещё не успел начать работать - возвращаем 0.
			UNLOCK(& locklist16);
			memset(buff, 0x00, size * sizeof (* buff));	// тишина
			return size;	// ноль нельзя возвращать - зацикливается проуелура ресэмплинга
		}
		else
		{
			PLIST_ENTRY t = RemoveTailList3(& resample16);

			p = CONTAINING_RECORD(t, voice16_t, item);
			UNLOCK(& locklist16);
			
			if (GetReadyList3(& resample16) == 0)
				skipsense = SKIPPED;
			const uint_fast8_t valid = GetReadyList3(& resample16) && skipsense == 0;

			skipsense = (skipsense == 0) ? SKIPPED : skipsense - 1;

			const unsigned LOW = RESAMPLE16NORMAL - (SKIPPED * 1);
			const unsigned HIGH = RESAMPLE16NORMAL + (SKIPPED * 1);

			if (valid && GetCountList3(& resample16) <= LOW)
			{
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
				static int16_t addsample [DMABUFSTEP16];
				enum { HALF = DMABUFFSIZE16 / 2 };
				// значения как среднее арифметическое сэмплов, между которыми вставляем дополнительный.
				addsample [0] = ((int_fast32_t) p->buff [HALF - DMABUFSTEP16 + 0] +  p->buff [HALF + 0]) / 2;	// Left
				addsample [1] = ((int_fast32_t) p->buff [HALF - DMABUFSTEP16 + 1] +  p->buff [HALF + 1]) / 2;	// Right
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
				// Ресэмплинг не требуется или нет запаса входных данных
				part = NPARTS - 1;
				datas [part] = & p->buff [0];
				sizes [part] = DMABUFFSIZE16;
			}
		}
	}
	else
	{
		UNLOCK(& locklist16);
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
uint_fast8_t takewavsample(INT32P_t * rv, uint_fast8_t suspend)
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
	LOCK(& locklist32);
	if (! IsListEmpty2(& voicesfree32tx))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesfree32tx);
		voice32tx_t * const p = CONTAINING_RECORD(t, voice32tx_t, item);
		UNLOCK(& locklist32);
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
		UNLOCK(& locklist32);
		return (uintptr_t) & p->buff;
	}
	else
	{
		debug_printf_P(PSTR("allocate_dmabuffer32tx() failure\n"));
		for (;;)
			;
	}
	UNLOCK(& locklist32);
}

// Этой функцией пользуются обработчики прерываний DMA на приём данных по SAI
RAMFUNC uintptr_t allocate_dmabuffer32rx(void)
{
	LOCK(& locklist32);
	if (! IsListEmpty2(& voicesfree32rx))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesfree32rx);
		voice32rx_t * const p = CONTAINING_RECORD(t, voice32rx_t, item);
		UNLOCK(& locklist32);
		return (uintptr_t) & p->buff;
	}
	else
	{
		debug_printf_P(PSTR("allocate_dmabuffer32rx() failure\n"));
		for (;;)
			;
	}
	UNLOCK(& locklist32);
}

// Этой функцией пользуются обработчики прерываний DMA на передачу и приём данных по I2S и USB AUDIO
RAMFUNC uintptr_t allocate_dmabuffer16(void)
{
	LOCK(& locklist16);
	if (! IsListEmpty2(& voicesfree16))
	{
		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16);
		UNLOCK(& locklist16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
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

			InsertHeadList2(& voicesfree16, t);
		}
		while (-- n && ! IsListEmpty3(& resample16));

		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16);
		UNLOCK(& locklist16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
	#if WITHBUFFERSDEBUG
		++ e3;
	#endif /* WITHBUFFERSDEBUG */
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
			InsertHeadList2(& voicesfree16, t);
		}
		while (-- n && ! IsListEmpty2(& voicesphones16));

		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16);
		UNLOCK(& locklist16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
	#if WITHBUFFERSDEBUG
		++ e4;
	#endif /* WITHBUFFERSDEBUG */
		return (uintptr_t) & p->buff;
	}
	else
	{
		UNLOCK(& locklist16);
		debug_printf_P(PSTR("allocate_dmabuffer16() failure\n"));
		for (;;)
			;
	}
}

// Этой функцией пользуются обработчики прерываний DMA
// передали буфер, считать свободным
void RAMFUNC release_dmabuffer32tx(uintptr_t addr)
{
	ASSERT(addr != 0);
	LOCK(& locklist32);
	voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
	InsertHeadList2(& voicesfree32tx, & p->item);
	UNLOCK(& locklist32);
}

// Этой функцией пользуются обработчики прерываний DMA
// передали буфер, считать свободным
void RAMFUNC release_dmabuffer16(uintptr_t addr)
{
	ASSERT(addr != 0);
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	buffers_tonull16(p);
}

// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки AF ADC
void RAMFUNC processing_dmabuffer16rx(uintptr_t addr)
{
	ASSERT(addr != 0);
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
	ASSERT(addr != 0);
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
	enum { CNT16 = DMABUFFSIZE16 / DMABUFSTEP16 };
	enum { CNT32RX = DMABUFFSIZE32RX / DMABUFSTEP32RX };
	ASSERT(addr != 0);
	voice32rx_t * const p = CONTAINING_RECORD(addr, voice32rx_t, buff);
#if WITHBUFFERSDEBUG
	++ n1;
	// подсчёт скорости в сэмплах за секунду
	debugcount_rx32adc += CNT32RX;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */
	dsp_extbuffer32rx(p->buff);

	LOCK(& locklist32);
	InsertHeadList2(& voicesfree32rx, & p->item);
	UNLOCK(& locklist32);

#if WITHUSBUAC
	static unsigned rx32adc = 0;
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

// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки IF ADC (MAIN RX/SUB RX)
// Вызывается на ARM_REALTIME_PRIORITY уровне.
void RAMFUNC processing_dmabuffer32wfm(uintptr_t addr)
{
	//enum { CNT16 = DMABUFFSIZE16 / DMABUFSTEP16 };
	enum { CNT32RX = DMABUFFSIZE32RX / DMABUFSTEP32RX };
	ASSERT(addr != 0);
	voice32rx_t * const p = CONTAINING_RECORD(addr, voice32rx_t, buff);
#if WITHBUFFERSDEBUG
	++ n1wfm;
	// подсчёт скорости в сэмплах за секунду
	debugcount_rx32wfm += CNT32RX;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */
	dsp_extbuffer32wfm(p->buff);

	LOCK(& locklist32);
	InsertHeadList2(& voicesfree32rx, & p->item);
	UNLOCK(& locklist32);

}

#if WITHRTS192
// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки - канал спектроанализатора
void RAMFUNC processing_dmabuffer32rts(uintptr_t addr)
{
	ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n4;
#endif /* WITHBUFFERSDEBUG */
	voice192rts_t * const p = CONTAINING_RECORD(addr, voice192rts_t, buff);

#if ! WITHTRANSPARENTIQ
	unsigned i;
	for (i = 0; i < DMABUFFSIZE192RTS; i += DMABUFSTEP192RTS)
	{
		const int32_t * const b = (const int32_t *) & p->buff [i];
		saveIQRTSxx(b [0], b [1]);
	}
#endif /* ! WITHTRANSPARENTIQ */

	buffers_savetouacin192rts(p);
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

	LOCK(& locklist32);
	if (! IsListEmpty2(& voicesready32tx))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesready32tx);
		voice32tx_t * const p = CONTAINING_RECORD(t, voice32tx_t, item);
		UNLOCK(& locklist32);
		return (uintptr_t) & p->buff;
	}
	UNLOCK(& locklist32);
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

	LOCK(& locklist16);
	if (! IsListEmpty2(& voicesphones16))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesphones16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
		UNLOCK(& locklist16);
		dsp_addsidetone(p->buff);
		return (uintptr_t) & p->buff;
	}
	UNLOCK(& locklist16);
#if WITHBUFFERSDEBUG
	++ e1;
#endif /* WITHBUFFERSDEBUG */
	const uintptr_t addr = allocate_dmabuffer16();
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	memset(p->buff, 0, sizeof p->buff);	// Заполнение "тишиной"
	dsp_addsidetone(p->buff);
	return (uintptr_t) & p->buff;
}

//////////////////////////////////////////
// Поэлементное заполнение буфера IF DAC

// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
// 32 bit, signed
void savesampleout32stereo(int_fast32_t ch0, int_fast32_t ch1)
{
	LOCK(& locklist32);
	static voice32tx_t * RAMDTCM prepareout32tx = NULL;
	static RAMDTCM unsigned level32tx;

	if (prepareout32tx == NULL)
	{
		UNLOCK(& locklist32);
		const uint32_t addr = allocate_dmabuffer32tx();
		LOCK(& locklist32);
		voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
		prepareout32tx = p;
		level32tx = 0;
	}

	prepareout32tx->buff [level32tx + DMABUF32TXI] = ch0;
	prepareout32tx->buff [level32tx + DMABUF32TXQ] = ch1;

	if ((level32tx += DMABUFSTEP32TX) >= DMABUFFSIZE32TX)
	{
		InsertHeadList2(& voicesready32tx, & prepareout32tx->item);
		prepareout32tx = NULL;
	}
	UNLOCK(& locklist32);
}

//////////////////////////////////////////
// Поэлементное заполнение буфера AF DAC

void savesampleout16stereo_user(int_fast32_t ch0, int_fast32_t ch1)
{
	// если есть инициализированный канал для выдачи звука
	static voice16_t * p = NULL;
	static unsigned n;

	if (p == NULL)
	{
		global_disableIRQ();
		uintptr_t addr = allocate_dmabuffer16();
		global_enableIRQ();
		p = CONTAINING_RECORD(addr, voice16_t, buff);
		n = 0;
	}

	p->buff [n + 0] = ch0;		// sample value
#if DMABUFSTEP16 > 1
	p->buff [n + 1] = ch1;	// sample value
#endif
	n += DMABUFSTEP16;

	if (n >= DMABUFFSIZE16)
	{
		global_disableIRQ();
		buffers_savefromrxout(p);
		global_enableIRQ();
		p = NULL;
	}
}

void savesampleout16stereo(int_fast32_t ch0, int_fast32_t ch1)
{
	// если есть инициализированный канал для выдачи звука
	static voice16_t * p = NULL;
	static unsigned n;

	if (p == NULL)
	{
		uintptr_t addr = allocate_dmabuffer16();
		p = CONTAINING_RECORD(addr, voice16_t, buff);
		n = 0;
	}

	p->buff [n + 0] = ch0;		// sample value
#if DMABUFSTEP16 > 1
	p->buff [n + 1] = ch1;	// sample value
#endif
	n += DMABUFSTEP16;

	if (n >= DMABUFFSIZE16)
	{
		buffers_savefromrxout(p);
		p = NULL;
	}
}

#if WITHUSBUAC

	#if WITHRTS96

		// Этой функцией пользуются обработчики прерываний DMA на приём данных по SAI
		static uintptr_t allocate_dmabuffer96rts(void)
		{
			LOCK(& locklist32);
			if (! IsListEmpty2(& voicesfree96rts))
			{
				PLIST_ENTRY t = RemoveTailList2(& voicesfree96rts);
				voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
				UNLOCK(& locklist32);
			#if WITHBUFFERSDEBUG
				++ n5;
			#endif /* WITHBUFFERSDEBUG */
				return (uintptr_t) & p->buff;
			}
			else if (! IsListEmpty2(& uacin96rts))
			{
				// Ошибочная ситуация - если буферы не освобождены вовремя -
				// берём из очереди готовых к передаче в компьютер по USB.
				// Очередь очищается возможно не полностью.
				uint_fast8_t n = 3;
				do
				{
					const PLIST_ENTRY t = RemoveTailList2(& uacin96rts);
					InsertHeadList2(& voicesfree96rts, t);
				} while (-- n && ! IsListEmpty2(& uacin96rts));

				const PLIST_ENTRY t = RemoveTailList2(& voicesfree96rts);
				voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
				UNLOCK(& locklist32);
			#if WITHBUFFERSDEBUG
				++ e5;
			#endif /* WITHBUFFERSDEBUG */
				return (uintptr_t) & p->buff;
			}
			else
			{
				debug_printf_P(PSTR("allocate_dmabuffer96rts() failure\n"));
				for (;;)
					;
			}
			UNLOCK(& locklist32);
		}

		// Этой функцией пользуются обработчики прерываний DMA
		// передали буфер, считать свободным
		static void release_dmabuffer96rts(uintptr_t addr)
		{
			LOCK(& locklist32);
			voice96rts_t * const p = CONTAINING_RECORD(addr, voice96rts_t, buff);
			InsertHeadList2(& voicesfree96rts, & p->item);
			UNLOCK(& locklist32);
		}

		// Этой функцией пользуются обработчики прерываний DMA
		// получить буфер для передачи в компьютер, через USB AUDIO
		// Если в данный момент нет готового буфера, возврат 0
		static uint32_t getfilled_dmabuffer96uacinrts(void)
		{
			LOCK(& locklist16);
			if (! IsListEmpty2(& uacin96rts))
			{
				PLIST_ENTRY t = RemoveTailList2(& uacin96rts);
				voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
				UNLOCK(& locklist16);
				return (uintptr_t) & p->buff;
			}
			UNLOCK(& locklist16);
			return 0;
		}

		// Поэлементное заполнение буфера RTS96

		// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
		// vl, vr: 32 bit, signed - преобразуем к требуемому формату для передачи по USB здесь.
		void savesampleout96stereo(int_fast32_t ch0, int_fast32_t ch1)
		{
			// если есть инициализированный канал для выдачи звука
			static voice96rts_t * RAMDTCM p = NULL;
			static RAMDTCM unsigned n;

			if (p == NULL)
			{
				if (! isrts96())
					return;
				uint32_t addr = allocate_dmabuffer96rts();
				p = CONTAINING_RECORD(addr, voice96rts_t, buff);
				n = 0;
			}
			else if (! isrts96())
			{
				buffers_savetonull96rts(p);
				p = NULL;
				return;
			}

			ASSERT(DMABUFSTEP96RTS == 6);
			p->buff [n ++] = ch0 >> 8;	// sample value
			p->buff [n ++] = ch0 >> 16;	// sample value
			p->buff [n ++] = ch0 >> 24;	// sample value
			p->buff [n ++] = ch1 >> 8;	// sample value
			p->buff [n ++] = ch1 >> 16;	// sample value
			p->buff [n ++] = ch1 >> 24;	// sample value

			if (n >= DMABUFFSIZE96RTS)
			{
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
			LOCK(& locklist16);
			if (! IsListEmpty2(& uacin192rts))
			{
				PLIST_ENTRY t = RemoveTailList2(& uacin192rts);
				voice192rts_t * const p = CONTAINING_RECORD(t, voice192rts_t, item);
				UNLOCK(& locklist16);
				return (uintptr_t) & p->buff;
			}
			UNLOCK(& locklist16);
			return 0;
		}

		// Этой функцией пользуются обработчики прерываний DMA на приём данных по SAI
		uint32_t allocate_dmabuffer192rts(void)
		{
			LOCK(& locklist32);
			if (! IsListEmpty2(& voicesfree192rts))
			{
				PLIST_ENTRY t = RemoveTailList2(& voicesfree192rts);
				voice192rts_t * const p = CONTAINING_RECORD(t, voice192rts_t, item);
				UNLOCK(& locklist32);
			#if WITHBUFFERSDEBUG
				++ n5;
			#endif /* WITHBUFFERSDEBUG */
				return (uintptr_t) & p->buff;
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
					InsertHeadList2(& voicesfree192rts, t);
				} while (-- n && ! IsListEmpty2(& uacin192rts));

				const PLIST_ENTRY t = RemoveTailList2(& voicesfree192rts);
				voice192rts_t * const p = CONTAINING_RECORD(t, voice192rts_t, item);
				UNLOCK(& locklist32);
			#if WITHBUFFERSDEBUG
				++ e5;
			#endif /* WITHBUFFERSDEBUG */
				return (uintptr_t) & p->buff;
			}
			else
			{
				debug_printf_P(PSTR("allocate_dmabuffer192rts() failure\n"));
				for (;;)
					;
			}
			UNLOCK(& locklist32);
		}

		// Этой функцией пользуются обработчики прерываний DMA
		// передали буфер, считать свободным
		static void release_dmabuffer192rts(uint32_t addr)
		{
			LOCK(& locklist32);
			voice192rts_t * const p = CONTAINING_RECORD(addr, voice192rts_t, buff);
			InsertHeadList2(& voicesfree192rts, & p->item);
			UNLOCK(& locklist32);
		}

		// NOT USED
		// Поэлементное заполнение буфера RTS192

		// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
		// vl, vr: 32 bit, signed - преобразуем к требуемому формату для передачи по USB здесь.
		void savesampleout192stereo(int_fast32_t ch0, int_fast32_t ch1)
		{
			// если есть инициализированный канал для выдачи звука
			static voice192rts_t * RAMDTCM p = NULL;
			static RAMDTCM unsigned n;

			if (p == NULL)
			{
				if (! isrts192())
					return;
				uint32_t addr = allocate_dmabuffer192rts();
				p = CONTAINING_RECORD(addr, voice192rts_t, buff);
				n = 0;
			}
			else if (! isrts192())
			{
				buffers_savetonull192rts(p);
				p = NULL;
				return;
			}

			p->buff [n ++] = ch0;	// sample value
			p->buff [n ++] = ch1;	// sample value

			if (n >= DMABUFFSIZE192RTS)
			{
				buffers_savetouacin192rts(p);
				p = NULL;
			}
		}

	#endif /* WITHRTS192 */

	void RAMFUNC release_dmabufferuacin16(uintptr_t addr)
	{
		ASSERT(addr != 0);
		uacin16_t * const p = CONTAINING_RECORD(addr, uacin16_t, buff);
		buffers_tonulluacin(p);
	}

	RAMFUNC uintptr_t allocate_dmabufferuacin16(void)
	{
		LOCK(& locklist16);
		if (! IsListEmpty2(& uacinfree16))
		{
			PLIST_ENTRY t = RemoveTailList2(& uacinfree16);
			uacin16_t * const p = CONTAINING_RECORD(t, uacin16_t, item);
			UNLOCK(& locklist16);
			return (uintptr_t) & p->buff;
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
			uacin16_t * const p = CONTAINING_RECORD(t, uacin16_t, item);
			UNLOCK(& locklist16);
			return (uintptr_t) & p->buff;
		}
		else
		{
			UNLOCK(& locklist16);
			debug_printf_P(PSTR("allocate_dmabufferuacin16() failure, uacinalt=%d\n"), uacinalt);
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
		LOCK(& locklist16);
		if (! IsListEmpty2(& uacinready16))
		{
			PLIST_ENTRY t = RemoveTailList2(& uacinready16);
			uacin16_t * const p = CONTAINING_RECORD(t, uacin16_t, item);
			UNLOCK(& locklist16);
			return (uintptr_t) & p->buff;
		}
		UNLOCK(& locklist16);
		return 0;
	}

	// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
	// vl, vr: 16 bit, signed - требуемый формат для передачи по USB.

	void savesamplerecord16uacin(int_fast16_t ch0, int_fast16_t ch1)
	{
	#if WITHUSBHW && WITHUSBUACIN
		// если есть инициализированный канал для выдачи звука
		static uacin16_t * p = NULL;
		static unsigned n = 0;

		if (p == NULL)
		{
			if (! isaudio48())
				return;
			uintptr_t addr = allocate_dmabufferuacin16();
			p = CONTAINING_RECORD(addr, uacin16_t, buff);
			n = 0;
		}
		else if (! isaudio48())
		{
			buffers_tonulluacin(p);
			p = NULL;
			return;
		}

		p->buff [n + 0] = ch0;		// sample value
#if DMABUFSTEPUACIN16 > 1
		p->buff [n + 1] = ch1;	// sample value
#endif
		n += DMABUFSTEPUACIN16;

		if (n >= DMABUFFSIZEUACIN16)
		{
			buffers_savetouacin(p);
			p = NULL;
		}
	#endif /* WITHUSBHW && WITHUSBUACIN */
	}

#else /* WITHUSBUAC */

void savesampleout96stereo(int_fast32_t ch0, int_fast32_t ch1)
{
}
void savesampleout192stereo(int_fast32_t ch0, int_fast32_t ch1)
{
}

#endif /* WITHUSBUAC */

#if WITHUSBUAC

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


void 
buffers_set_uacinalt(uint_fast8_t v)	/* выбор альтернативной конфигурации для UAC IN interface */
{
	//debug_printf_P(PSTR("buffers_set_uacinalt: v=%d\n"), v);
	uacinalt = v;
}

#if WITHUSBUACIN2

void 
buffers_set_uacinrtsalt(uint_fast8_t v)	/* выбор альтернативной конфигурации для UAC IN interface */
{
	//debug_printf_P(PSTR("buffers_set_uacinrtsalt: v=%d\n"), v);
	uacinrtsalt = v;
}

#endif /* WITHUSBUACIN2 */

void 
buffers_set_uacoutalt(uint_fast8_t v)	/* выбор альтернативной конфигурации для UAC OUT interface */
{
	//debug_printf_P(PSTR("buffers_set_uacoutalt: v=%d\n"), v);
	uacoutalt = v;
}

/* +++ UAC OUT data save */


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

/* вызыватся из не-realtime функции обработчика прерывания */
// Работает на ARM_SYSTEM_PRIORITY
void uacout_buffer_save_system(const uint8_t * buff, uint_fast16_t size)
{
	const size_t dmabuffer16size = DMABUFFSIZE16 * sizeof (int16_t);	// размер в байтах

#if WITHUABUACOUTAUDIO48MONO

	for (;;)
	{
		const uint_fast16_t insamples = size / 2;	// количество сэмплов во входном буфере
		const uint_fast16_t outsamples = (dmabuffer16size - uacoutbufflevel) / 2 / DMABUFSTEP16;
		const uint_fast16_t chunksamples = ulmin16(insamples, outsamples);
		if (chunksamples == 0)
			break;
		if (uacoutaddr == 0)
		{
 			global_disableIRQ();
			uacoutaddr = allocate_dmabuffer16();
			global_enableIRQ();
			uacoutbufflevel = 0;
		}
		//memcpy((uint8_t *) uacoutaddr + uacoutbufflevel, buff, chunk);
		{
			// копирование нужного количества сэмплов с прербразованием из моно в стерео
			const uint16_t * src = (const uint16_t *) buff;
			uint16_t * dst = (uint16_t *) ((uint8_t *) uacoutaddr + uacoutbufflevel);
			uint_fast16_t n = chunksamples;
			while (n --)
			{
				const uint_fast16_t v = * src ++;
				* dst ++ = v;
				* dst ++ = v;
			}
		}
		const uint_fast16_t inchunk = chunksamples * 2;
		const uint_fast16_t outchunk = chunksamples * 2 * DMABUFSTEP16;	// разхмер в байтах
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
#else /* WITHUABUACOUTAUDIO48MONO */
	
	for (;;)
	{
		const uint_fast16_t chunk = ulmin16(size, dmabuffer16size - uacoutbufflevel);
		if (chunk == 0)
			break;
		if (uacoutaddr == 0)
		{
 			global_disableIRQ();
			uacoutaddr = allocate_dmabuffer16();
			global_enableIRQ();
			uacoutbufflevel = 0;
		}
		memcpy((uint8_t *) uacoutaddr + uacoutbufflevel, buff, chunk);
		size -= chunk;		// проход по входому буферу
		buff += chunk;		// проход по входому буферу

		if ((uacoutbufflevel += chunk) >= dmabuffer16size)	// проход по вывходому буферу
		{
			global_disableIRQ();
			processing_dmabuffer16rxuac(uacoutaddr);
			global_enableIRQ();
			uacoutaddr = 0;
			uacoutbufflevel = 0;
		}
	}

#endif /* WITHUABUACOUTAUDIO48MONO */
}
/* вызыватся из realtime функции обработчика прерывания */
// Работает на ARM_REALTIME_PRIORITY
void uacout_buffer_save_realtime(const uint8_t * buff, uint_fast16_t size)
{
	const size_t dmabuffer16size = DMABUFFSIZE16 * sizeof (int16_t);	// размер в байтах

#if WITHUABUACOUTAUDIO48MONO

	for (;;)
	{
		const uint_fast16_t insamples = size / 2;	// количество сэмплов во входном буфере
		const uint_fast16_t outsamples = (dmabuffer16size - uacoutbufflevel) / 2 / DMABUFSTEP16;
		const uint_fast16_t chunksamples = ulmin16(insamples, outsamples);
		if (chunksamples == 0)
			break;
		if (uacoutaddr == 0)
		{
 			//global_disableIRQ();
			uacoutaddr = allocate_dmabuffer16();
			//global_enableIRQ();
			uacoutbufflevel = 0;
		}
		//memcpy((uint8_t *) uacoutaddr + uacoutbufflevel, buff, chunk);
		{
			// копирование нужного количества сэмплов с прербразованием из моно в стерео
			const uint16_t * src = (const uint16_t *) buff;
			uint16_t * dst = (uint16_t *) ((uint8_t *) uacoutaddr + uacoutbufflevel);
			uint_fast16_t n = chunksamples;
			while (n --)
			{
				const uint_fast16_t v = * src ++;
				* dst ++ = v;
				* dst ++ = v;
			}
		}
		const uint_fast16_t inchunk = chunksamples * 2;
		const uint_fast16_t outchunk = chunksamples * 2 * DMABUFSTEP16;	// разхмер в байтах
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
#else /* WITHUABUACOUTAUDIO48MONO */

	for (;;)
	{
		const uint_fast16_t chunk = ulmin16(size, dmabuffer16size - uacoutbufflevel);
		if (chunk == 0)
			break;
		if (uacoutaddr == 0)
		{
 			//global_disableIRQ();
			uacoutaddr = allocate_dmabuffer16();
			//global_enableIRQ();
			uacoutbufflevel = 0;
		}
		memcpy((uint8_t *) uacoutaddr + uacoutbufflevel, buff, chunk);
		size -= chunk;		// проход по входому буферу
		buff += chunk;		// проход по входому буферу

		if ((uacoutbufflevel += chunk) >= dmabuffer16size)	// проход по вывходому буферу
		{
			//global_disableIRQ();
			processing_dmabuffer16rxuac(uacoutaddr);
			//global_enableIRQ();
			uacoutaddr = 0;
			uacoutbufflevel = 0;
		}
	}

#endif /* WITHUABUACOUTAUDIO48MONO */
}
/* --- UAC OUT data save */


/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferx(uintptr_t addr)
{
	ASSERT(addr != 0);
	uacin16_t * const p = CONTAINING_RECORD(addr, uacin16_t, buff);
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
		debug_printf_P(PSTR("release_dmabufferx: wrong tag value: p=%p, %02X\n"), p, p->tag);
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
		* sizep = UAC_IN48_DATA_SIZE;
		return getfilled_dmabuffer16uacin();

#if ! WITHUSBUACIN2

#if WITHRTS96
	case UACINALT_RTS96:
		* sizep = VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS96;
		return getfilled_dmabuffer96uacinrts();
#endif /* WITHRTS192 */

#if WITHRTS192
	case UACINALT_RTS192:
		* sizep = VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS192;
		return getfilled_dmabuffer192uacinrts();
#endif /* WITHRTS192 */

#endif /* ! WITHUSBUACIN2 */

	default:
		debug_printf_P(PSTR("getfilled_dmabufferx: uacinalt=%u\n"), uacinalt);
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

#if WITHUSBUACIN2

#if WITHRTS96
	case UACINRTSALT_RTS96:
		* sizep = VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS96;
		return getfilled_dmabuffer96uacinrts();
#endif /* WITHRTS192 */

#if WITHRTS192
	case UACINRTSALT_RTS192:
		* sizep = VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS192;
		return getfilled_dmabuffer192uacinrts();
#endif /* WITHRTS192 */

#endif /* WITHUSBUACIN2 */

	default:
		debug_printf_P(PSTR("getfilled_dmabufferxrts: uacinrtsalt=%u\n"), uacinrtsalt);
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

#endif /* WITHINTEGRATEDDSP */
