/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "buffers.h"
#include "formats.h"	// for debug prints

#include "mslist.h"
#include "audio.h"

//#define WITHBUFFERSDEBUG WITHDEBUG
#define BUFOVERSIZE 1

#define VOICE16RX_CAPACITY (4 * BUFOVERSIZE)	// прием от кодекв
#define VOICE16RXPHONES_CAPACITY (48 * BUFOVERSIZE)	// должно быть достаточное количество буферов чтобы запомнить буфер с выхода speex
#define VOICE16TXMONI_CAPACITY (4 * BUFOVERSIZE)	// во столько же на сколько буфр от кодека больше чем буфер к кодеку (если наоборот - минимум)

#define UACINRTS192_CAPACITY (8 * BUFOVERSIZE)
#define UACINRTS96_CAPACITY (8 * BUFOVERSIZE)
#define UACOUT48_CAPACITY (16 * BUFOVERSIZE)
#define UACIN48_CAPACITY (16 * BUFOVERSIZE)

#define SPEEX_CAPACITY (5 * BUFOVERSIZE)

#define VOICE32RX_CAPACITY (2 + 6 * BUFOVERSIZE)
#define VOICE32TX_CAPACITY (6 * BUFOVERSIZE)
#define VOICE32RTS_CAPACITY (4 * BUFOVERSIZE)	// dummy fn

#define AUDIOREC_CAPACITY (18 * BUFOVERSIZE)

#define MESSAGE_CAPACITY (12)
#define MESSAGE_IRQL IRQL_SYSTEM

#if WITHUSBHW
	#include "usb/usb200.h"
	#include "usb/usbch9.h"
#endif /* WITHUSBHW */

#include <string.h>		// for memset

template <typename element_t, unsigned capacity>
class blists
{
	typedef struct buffitem
	{
		LIST_ENTRY item;
		void * tag0;
		void * tag2;
		ALIGNX_BEGIN  element_t v ALIGNX_END;
		void * tag3;
		unsigned cks;
	} buffitem_t;


	IRQLSPINLOCK_t irqllocl;
#if WITHBUFFERSDEBUG
	int errallocate;
	int saveount;
#endif /* WITHBUFFERSDEBUG */

	int outcount;
	int freecount;
	LIST_ENTRY freelist;
	LIST_ENTRY readylist;
	buffitem_t storage [capacity];

public:
	blists(IRQL_t airql) :
#if WITHBUFFERSDEBUG
		errallocate(0),
		saveount(0),
#endif /* WITHBUFFERSDEBUG */
		outcount(0),
		freecount(capacity)
	{
		InitializeListHead(& freelist);
		InitializeListHead(& readylist);
		IRQLSPINLOCK_INITIALIZE(& irqllocl, airql);
		for (unsigned i = 0; i < capacity; ++ i)
		{
			buffitem_t * const p = & storage [i];
			p->tag0 = this;
			p->tag2 = p;
			p->tag3 = p;
			InsertHeadList(& freelist, & p->item);
		}
		//PRINTF("blists %u %u\n", sizeof (storage [0].v.buff), capacity);
	}

	// сохранить в списке свободных
	void release_buffer(element_t * addr)
	{
		buffitem_t * const p = CONTAINING_RECORD(addr, buffitem_t, v);
		ASSERT(p->tag0 == this);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql);

		InsertHeadList(& freelist, & p->item);
		++ freecount;

		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
	}

	// сохранить в списке готовых
	void save_buffer(element_t * addr)
	{
		buffitem_t * const p = CONTAINING_RECORD(addr, buffitem_t, v);
		ASSERT(p->tag0 == this);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);

		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql);

		InsertHeadList(& readylist, & p->item);
		++ outcount;
#if WITHBUFFERSDEBUG
		++ saveount;
#endif /* WITHBUFFERSDEBUG */

		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
	}

	// получить из списка готовых
	int get_readybuffer(element_t * * dest)
	{
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql);
		if (! IsListEmpty(& readylist))
		{
			const PLIST_ENTRY t = RemoveTailList(& readylist);
			-- outcount;
			IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
			buffitem_t * const p = CONTAINING_RECORD(t, buffitem_t, item);
			ASSERT(p->tag0 == this);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			* dest = & p->v;
			return 1;
		}
		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
		return 0;
	}

	// получить из списка свободных
	int get_freebuffer(element_t * * dest)
	{
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql);
		if (! IsListEmpty(& freelist))
		{
			const PLIST_ENTRY t = RemoveTailList(& freelist);
			ASSERT(freecount != 0);
			-- freecount;
			IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
			buffitem_t * const p = CONTAINING_RECORD(t, buffitem_t, item);
			ASSERT(p->tag0 == this);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			* dest = & p->v;
			return 1;
		}
#if WITHBUFFERSDEBUG
		++ errallocate;
#endif /* WITHBUFFERSDEBUG */
		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
		return 0;
	}
	// получить из списка свободных, если нет - из готовых
	int get_freebufferforced(element_t * * dest)
	{
		return get_freebuffer(dest) || get_readybuffer(dest);
	}

	// все готовые перенести в свободные
//	void purge_buffers()
//	{
//		element_t * dest;
//		while (get_readybuffer(& dest))
//			release_buffer(dest);
//	}

	static int_fast32_t get_cachesize(void)
	{
		return offsetof(element_t, pad) - offsetof(element_t, buff);
	}

	void debug(const char * name)
	{
#if WITHBUFFERSDEBUG
		//PRINTF("%s:s=%d,a=%d,o=%d,f=%d ", name, saveount, errallocate, outcount, freecount);
		PRINTF("%s:a=%d,o=%d,f=%d ", name, errallocate, outcount, freecount);
#endif /* WITHBUFFERSDEBUG */
	}
};

template <typename element_t, unsigned capacity>
class blistsresample: public blists<element_t, capacity>
{
	element_t * workbuff;	// буфер над которым выполняется ресэмплинг
	unsigned wbstart;	// start position of work buffer - zero has not meaning
	LCLSPINLOCK_t lock;
	typedef blists<element_t, capacity> parent_t;
public:
	blistsresample(IRQL_t airql) :
		blists<element_t, capacity>(airql),
		//workbuff(nullptr),
		wbstart(0)
		{
			LCLSPINLOCK_INITIALIZE(& lock);
			// Один элемент в выходном буфере присутствует
			element_t * dest;
			VERIFY(parent_t::get_freebuffer(& dest));
			parent_t::save_buffer(dest);

			// test
//			VERIFY(parent_t::get_freebuffer(& workbuff));
//			wbstart = element_t::ss * element_t::nch;

		}
	// функция вызывается получателем (получаем после ресэмплинга.
	// Гарантированно получене буфера
	int get_readybuffer(element_t * * dest)
	{
		return parent_t::get_readybuffer(dest);
		if (wbstart)
		{
			// Есть не полностью израсходованный остаток в буфере
			const int p1 = sizeof (workbuff->buff) - wbstart;	// размер оставшийся до конца буфера
			const int p3 = wbstart;	// размер от начала буфера - в рабочем буфере уже передано
			// есть остаток старого буфера
			while (! parent_t::get_freebufferforced(dest))
				ASSERT(0);
			memcpy((* dest)->buff, (uint8_t *) workbuff->buff + wbstart, p1);
			parent_t::release_buffer(workbuff);
			if (parent_t::get_readybuffer(& workbuff) || parent_t::get_freebufferforced(& workbuff))
			{
				memcpy((uint8_t *) (* dest)->buff + p1, workbuff->buff, p3);
				wbstart = p3;
				return 1;
			}
		}
		return parent_t::get_readybuffer(dest) || parent_t::get_freebufferforced(dest);
	}
};

#if defined(WITHRTS96) && defined(WITHRTS192)
	#error Configuration Error: WITHRTS96 and WITHRTS192 can not be used together
#endif /* defined(WITHRTS96) && defined(WITHRTS192) */


#if WITHINTEGRATEDDSP

//////////////////////////////////
// Система буферизации аудиоданных
//

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

// Denoise operations

typedef ALIGNX_BEGIN struct denoise16
{
	ALIGNX_BEGIN speexel_t buff [NTRX * FIRBUFSIZE] ALIGNX_END;
} ALIGNX_END denoise16_t;

// буферы: один заполняется, один воспроизводлится и два свободных (с одинм бывают пропуски).
typedef blists<denoise16_t, SPEEX_CAPACITY> denoise16list_t;

static denoise16list_t denoise16list(IRQL_REALTIME);

// получить готоввый
uint_fast8_t takespeexready(speexel_t * * dest)
{
	denoise16_t * addr;
	if (denoise16list.get_readybuffer(& addr))
	{
		* dest = addr->buff;
		return 1;
	}
	return 0;
}

// получить свободный
speexel_t * allocatespeexbuffer(void)
{
	denoise16_t * addr;
	if (denoise16list.get_freebufferforced(& addr))
	{
		return addr->buff;
	}
	return NULL;
}

// Освобождение буфера
void releasespeexbuffer(speexel_t * t)
{
	denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, buff);
	denoise16list.release_buffer(p);
}

// сохранение для обработки буфера
void savespeexbuffer(speexel_t * t)
{
	denoise16_t * const p = CONTAINING_RECORD(t, denoise16_t, buff);
	denoise16list.save_buffer(p);
}

// Audio CODEC out (to processor)
typedef ALIGNX_BEGIN struct voice16rx_tag
{
	ALIGNX_BEGIN aubufv_t buff [DMABUFFSIZE16RX] ALIGNX_END;
	ALIGNX_BEGIN uint8_t pad ALIGNX_END;
	enum { ss = sizeof (aubufv_t) };
	enum { nch = DMABUFFSTEP16RX };
} ALIGNX_END voice16rx_t;

typedef blists<voice16rx_t, VOICE16RX_CAPACITY> voice16rxcodeclist_t;

static voice16rxcodeclist_t voice16rxcodeclist(IRQL_REALTIME);		// from codec

int_fast32_t cachesize_dmabuffer16rx(void)
{
	return voice16rxcodeclist.get_cachesize();
}

// can not be zero
uintptr_t allocate_dmabuffer16rx(void)
{
	voice16rx_t * dest;
	while (voice16rxcodeclist.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

// may be zero
uintptr_t getfilled_dmabuffer16rx(void)
{
	voice16rx_t * dest;
	if (voice16rxcodeclist.get_readybuffer(& dest) == 0)
		return 0;
	return (uintptr_t) dest->buff;
}

void save_dmabuffer16rx(uintptr_t addr)
{
	voice16rx_t * const p = CONTAINING_RECORD(addr, voice16rx_t, buff);
	voice16rxcodeclist.save_buffer(p);
}

void release_dmabuffer16rx(uintptr_t addr)
{
	voice16rx_t * const p = CONTAINING_RECORD(addr, voice16rx_t, buff);
	voice16rxcodeclist.release_buffer(p);
}

// Audio CODEC in (from processor)
typedef ALIGNX_BEGIN struct voice16tx_tag
{
	ALIGNX_BEGIN aubufv_t buff [DMABUFFSIZE16TX] ALIGNX_END;
	ALIGNX_BEGIN uint8_t pad ALIGNX_END;
} ALIGNX_END voice16tx_t;

typedef blists<voice16tx_t, VOICE16RXPHONES_CAPACITY> voice16txphones_t;
typedef blists<voice16tx_t, VOICE16TXMONI_CAPACITY> voice16txmoni_t;

static voice16txphones_t voice16txphones(IRQL_REALTIME);
static voice16txphones_t voice16txmoni(IRQL_REALTIME);

int_fast32_t cachesize_dmabuffer16txphones(void)
{
	return voice16txphones.get_cachesize();
}

// can not be zero
uintptr_t allocate_dmabuffer16txphones(void)
{
	voice16tx_t * dest;
	while (voice16txphones.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

void save_dmabuffer16txphones(uintptr_t addr)
{
	voice16tx_t * const p = CONTAINING_RECORD(addr, voice16tx_t, buff);
	voice16txphones.save_buffer(p);
}

void release_dmabuffer16txphones(uintptr_t addr)
{
	voice16tx_t * const p = CONTAINING_RECORD(addr, voice16tx_t, buff);
	voice16txphones.release_buffer(p);
}

// add sidetone
uintptr_t getfilled_dmabuffer16txphones(void)
{
	voice16tx_t * phones;
	voice16tx_t * moni;
	do
	{
		if (voice16txphones.get_readybuffer(& phones) )
			break;
		if (voice16txphones.get_freebuffer(& phones) )
			break;
		ASSERT(0);
		for (;;)
			;
		return 0;
	} while (0);

	// Добавить самоконтроль
	while ((voice16txmoni.get_readybuffer(& moni) || voice16txmoni.get_freebuffer(& moni)) == 0)
		ASSERT(0);

	dsp_addsidetone(phones->buff, moni->buff, 1);

	voice16txmoni.release_buffer(moni);

	return (uintptr_t) phones->buff;
}

// sidetone forming

// can not be zero
uintptr_t allocate_dmabuffer16txmoni(void)
{
	voice16tx_t * dest;
	while (voice16txmoni.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

void save_dmabuffer16txmoni(uintptr_t addr)
{
	voice16tx_t * const p = CONTAINING_RECORD(addr, voice16tx_t, buff);
	voice16txmoni.save_buffer(p);
}

uintptr_t getfilled_dmabuffer16txmoni(void)
{
	voice16tx_t * dest;
	if (voice16txmoni.get_readybuffer(& dest) )
		return (uintptr_t) dest->buff;
	if (voice16txmoni.get_freebuffer(& dest) )
		return (uintptr_t) dest->buff;
	ASSERT(0);
	for (;;)
		;
	return 0;
}

void release_dmabuffer16txmoni(uintptr_t addr)
{
	voice16tx_t * const p = CONTAINING_RECORD(addr, voice16tx_t, buff);
	voice16txmoni.release_buffer(p);
}

// I/Q data to FPGA or IF CODEC
typedef ALIGNX_BEGIN struct voices32tx_tag
{
	ALIGNX_BEGIN IFDACvalue_t buff [DMABUFFSIZE32TX] ALIGNX_END;
	ALIGNX_BEGIN uint8_t pad ALIGNX_END;
} ALIGNX_END voice32tx_t;


typedef blists<voice32tx_t, VOICE32TX_CAPACITY> voice32txlist_t;

static voice32txlist_t voice32txlist(IRQL_REALTIME);

int_fast32_t cachesize_dmabuffer32tx(void)
{
	return voice32txlist.get_cachesize();
}

void release_dmabuffer32tx(uintptr_t addr)
{
	voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
	voice32txlist.release_buffer(p);
}

// can not be zero
uintptr_t allocate_dmabuffer32tx(void)
{
	voice32tx_t * dest;
	while (voice32txlist.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

void save_dmabuffer32tx(uintptr_t addr)
{
	voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
	voice32txlist.save_buffer(p);
}

// can not be be zero
uintptr_t getfilled_dmabuffer32tx(void)
{
	voice32tx_t * dest;
	if (voice32txlist.get_readybuffer(& dest))
		return (uintptr_t) dest->buff;
	if (voice32txlist.get_freebuffer(& dest))
		return (uintptr_t) dest->buff;
	ASSERT(0);
	for (;;)
		;
	return 0;
}

// can not be be zero
uintptr_t getfilled_dmabuffer32tx_main(void)
{
	return getfilled_dmabuffer32tx();
}

// can not be be zero
uintptr_t getfilled_dmabuffer32tx_sub(void)
{
	return allocate_dmabuffer32tx();
}

// I/Q data from FPGA or IF CODEC
// I/Q data from FPGA or IF CODEC
typedef ALIGNX_BEGIN struct voices32rx_tag
{
	ALIGNX_BEGIN IFADCvalue_t buff [DMABUFFSIZE32RX] ALIGNX_END;
	ALIGNX_BEGIN uint8_t pad ALIGNX_END;
} ALIGNX_END voice32rx_t;


typedef blists<voice32rx_t, VOICE32RX_CAPACITY> voice32rxlist_t;

static voice32rxlist_t voice32rxlist(IRQL_REALTIME);

int_fast32_t cachesize_dmabuffer32rx(void)
{
	return voice32rxlist.get_cachesize();
}

void release_dmabuffer32rx(uintptr_t addr)
{
	voice32rx_t * const p = CONTAINING_RECORD(addr, voice32rx_t, buff);
	voice32rxlist.release_buffer(p);
}

// can not be zero
uintptr_t allocate_dmabuffer32rx(void)
{
	voice32rx_t * dest;
	while (voice32rxlist.get_freebuffer(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest->buff;
}


///////////////////////////////////////
///

#if WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE)

// USB AUDIO OUT

typedef struct
{
	ALIGNX_BEGIN  uint8_t buff [UACOUT_AUDIO48_DATASIZE_DMAC] ALIGNX_END;
	ALIGNX_BEGIN  uint8_t pad ALIGNX_END;
	enum { ss = UACOUT_AUDIO48_SAMPLEBYTES };
	enum { nch = UACOUT_FMT_CHANNELS_AUDIO48 };
} uacout48_t;

typedef blistsresample<uacout48_t, UACOUT48_CAPACITY> uacout48list_t;

static uacout48list_t uacout48list(IRQL_REALTIME);

int_fast32_t cachesize_dmabufferuacout48(void)
{
	return uacout48list.get_cachesize();
}

// can not be zero
uintptr_t allocate_dmabufferuacout48(void)
{
	uacout48_t * dest;
	while (uacout48list.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

// may be zero
uintptr_t getfilled_dmabufferuacout48(void)
{
	uacout48_t * dest;
	if (uacout48list.get_readybuffer(& dest) == 0)
		return 0;
	return (uintptr_t) dest->buff;
}

void release_dmabufferuacout48(uintptr_t addr)
{
	uacout48_t * const p = CONTAINING_RECORD(addr, uacout48_t, buff);
	uacout48list.release_buffer(p);
}

static void pp(const void * p, size_t n)
{
	const volatile uint8_t * pb = (const volatile uint8_t *) p;
	while (n --)
		* pb ++;
}

void save_dmabufferuacout48(uintptr_t addr)
{
	uacout48_t * const p = CONTAINING_RECORD(addr, uacout48_t, buff);

	// временное решение проблемы щелчклчков
	pp(p->buff, sizeof p->buff);
	pp(p->buff, sizeof p->buff);

	uacout48list.save_buffer(p);
}

#endif /* WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE) */

///////////////////////////////////////
///

#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)

typedef enum
{
	BUFFTAG_UACIN48 = 44,
	BUFFTAG_RTS192,
	BUFFTAG_RTS96,
	//
	BUFFTAG_total
} uacintag_t;

// USB AUDIO RTS IN

#if WITHRTS192


	typedef struct
	{
		uacintag_t tag;
		ALIGNX_BEGIN  uint8_t buff [UACIN_RTS192_DATASIZE_DMAC] ALIGNX_END;
		ALIGNX_BEGIN  uint8_t pad ALIGNX_END;
		enum { ss = UACIN_RTS192_SAMPLEBYTES };
		enum { nch = UACIN_FMT_CHANNELS_RTS192 };
	} uacinrts192_t;

	typedef blistsresample<uacinrts192_t, UACINRTS192_CAPACITY> uacinrts192list_t;

	static uacinrts192list_t uacinrts192list(IRQL_REALTIME);

	int_fast32_t cachesize_dmabufferuacinrts192(void)
	{
		return uacinrts192list.get_cachesize();
	}

	// can not be zero
	uintptr_t allocate_dmabufferuacinrts192(void)
	{
		uacinrts192_t * dest;
		while (uacinrts192list.get_freebufferforced(& dest) == 0)
			ASSERT(0);
		dest->tag = BUFFTAG_RTS192;
		return (uintptr_t) & dest->buff;
	}

	// can not be zero
	uintptr_t getfilled_dmabufferuacinrts192(void)
	{
		uacinrts192_t * dest;
		if (uacinrts192list.get_readybuffer(& dest))
		{
			dest->tag = BUFFTAG_RTS96;
			return (uintptr_t) & dest->buff;
		}
		if (uacinrts192list.get_freebuffer(& dest))
		{
			dest->tag = BUFFTAG_RTS96;
			return (uintptr_t) & dest->buff;
		}
		ASSERT(0);
		return 0;
	}

	void save_dmabufferuacinrts192(uintptr_t addr)
	{
		uacinrts192_t * const p = CONTAINING_RECORD(addr, uacinrts192_t, buff);
		ASSERT(p->tag == BUFFTAG_RTS192);
		uacinrts192list.save_buffer(p);
	}


	void release_dmabufferuacinrts192(uintptr_t addr)
	{
		uacinrts192_t * const p = CONTAINING_RECORD(addr, uacinrts192_t, buff);
		ASSERT(p->tag == BUFFTAG_RTS192);
		uacinrts192list.release_buffer(p);
	}

#elif WITHRTS96

	typedef struct
	{
		uacintag_t tag;
		ALIGNX_BEGIN  uint8_t buff [UACIN_RTS96_DATASIZE_DMAC] ALIGNX_END;
		ALIGNX_BEGIN  uint8_t pad ALIGNX_END;
		enum { ss = UACIN_RTS96_SAMPLEBYTES };
		enum { nch = UACIN_FMT_CHANNELS_RTS96 };
	} uacinrts96_t;

	typedef blistsresample<uacinrts96_t, UACINRTS96_CAPACITY> uacinrts96list_t;

	static uacinrts96list_t uacinrts96list(IRQL_REALTIME);

	int_fast32_t cachesize_dmabufferuacinrts96(void)
	{
		return uacinrts96list.get_cachesize();
	}

	// can not be zero
	uintptr_t allocate_dmabufferuacinrts96(void)
	{
		uacinrts96_t * dest;
		while (uacinrts96list.get_freebufferforced(& dest) == 0)
			ASSERT(0);
		dest->tag = BUFFTAG_RTS96;
		return (uintptr_t) & dest->buff;
	}

	// can not be zero
	uintptr_t getfilled_dmabufferuacinrts96(void)
	{
		uacinrts96_t * dest;
		if (uacinrts96list.get_readybuffer(& dest))
		{
			dest->tag = BUFFTAG_RTS96;
			return (uintptr_t) & dest->buff;
		}
		if (uacinrts96list.get_freebuffer(& dest))
		{
			dest->tag = BUFFTAG_RTS96;
			return (uintptr_t) & dest->buff;
		}
		ASSERT(0);
		return 0;
	}

	void save_dmabufferuacinrts96(uintptr_t addr)
	{
		uacinrts96_t * const p = CONTAINING_RECORD(addr, uacinrts96_t, buff);
		ASSERT(p->tag == BUFFTAG_RTS96);
		uacinrts96list.save_buffer(p);
	}


	void release_dmabufferuacinrts96(uintptr_t addr)
	{
		uacinrts96_t * const p = CONTAINING_RECORD(addr, uacinrts96_t, buff);
		ASSERT(p->tag == BUFFTAG_RTS96);
		uacinrts96list.release_buffer(p);
	}

#endif /* WITHRTS96 */

// USB AUDIO IN

typedef struct
{
	uacintag_t tag;
	ALIGNX_BEGIN  uint8_t buff [UACIN_AUDIO48_DATASIZE_DMAC] ALIGNX_END;
	ALIGNX_BEGIN  uint8_t pad ALIGNX_END;
	enum { ss = UACIN_AUDIO48_SAMPLEBYTES };
	enum { nch = UACIN_FMT_CHANNELS_AUDIO48 };
} uacin48_t;

typedef blistsresample<uacin48_t, UACIN48_CAPACITY> uacin48list_t;

static uacin48list_t uacin48list(IRQL_REALTIME);

int_fast32_t cachesize_dmabufferuacin48(void)
{
	return uacin48list.get_cachesize();
}

// can not be zero
uintptr_t allocate_dmabufferuacin48(void)
{
	uacin48_t * dest;
	while (uacin48list.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	dest->tag = BUFFTAG_UACIN48;
	return (uintptr_t) dest->buff;
}

// can not be zero
uintptr_t getfilled_dmabufferuacin48(void)
{
	uacin48_t * dest;
	if (uacin48list.get_readybuffer(& dest))
	{
		dest->tag = BUFFTAG_UACIN48;
		return (uintptr_t) & dest->buff;
	}
	if (uacin48list.get_freebuffer(& dest))
	{
		dest->tag = BUFFTAG_UACIN48;
		memset(dest->buff, 0, sizeof dest->buff);
		return (uintptr_t) & dest->buff;
	}
	ASSERT(0);
	return 0;
}

void save_dmabufferuacin48(uintptr_t addr)
{
	uacin48_t * const p = CONTAINING_RECORD(addr, uacin48_t, buff);
	ASSERT(p->tag == BUFFTAG_UACIN48);
	uacin48list.save_buffer(p);
}

void release_dmabufferuacin48(uintptr_t addr)
{
	uacin48_t * const p = CONTAINING_RECORD(addr, uacin48_t, buff);
	uacin48list.release_buffer(p);
}

/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferuacinX(uintptr_t addr)
{
	//ASSERT(addr != 0);
	uacin48_t * const p = CONTAINING_RECORD(addr, uacin48_t, buff);
	switch (p->tag)
	{
#if WITHRTS96
	case BUFFTAG_RTS96:
		release_dmabufferuacinrts96(addr);
		return;
#endif /* WITHRTS96 */

#if WITHRTS192
	case BUFFTAG_RTS192:
		release_dmabufferuacinrts192(addr);
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
#endif /* uacin */

//////////////////////////////////////////
// Поэлементное чтение буфера AF ADC

// в паре значений, возвращаемых данной функцией, vi получает значение от микрофона. vq зарезервированно для работы ISB (две независимых боковых)
// При отсутствии данных в очереди - возвращаем 0
RAMFUNC uint_fast8_t getsampmlemike(FLOAT32P_t * v)
{
	enum { L, R };
	static aubufv_t * buff = NULL;
	static unsigned n = 0;	// позиция по выходному количеству

	if (buff == NULL)
	{
		buff = (aubufv_t *) getfilled_dmabuffer16rx();
		if (buff == 0)
		{
			// Микрофонный кодек ещё не успел начать работать - возвращаем 0.
			return 0;
		}
		n = 0;
	}

	const FLOAT_t sample = adpt_input(& afcodecrx, buff [n * DMABUFFSTEP16RX + DMABUFF16RX_MIKE]);	// микрофон или левый канал

	// Использование данных.
	v->ivqv [L] = sample;
	v->ivqv [R] = sample;

	if (++ n >= CNT16RX)
	{
		release_dmabuffer16rx((uintptr_t) buff);
		buff = NULL;
	}
	return 1;
}

//////////////////////////////////////////
// Поэлементное чтение буфера UAC OUT

// в паре значений, возвращаемых данной функцией, vi получает значение от микрофона. vq зарезервированно для работы ISB (две независимых боковых)
// При отсутствии данных в очереди - возвращаем 0
RAMFUNC uint_fast8_t getsampmleusb(FLOAT32P_t * v)
{
	enum { L, R };
	static uint8_t * buff = NULL;
	static unsigned n = 0;	// позиция по выходному количеству байт

	if (buff == NULL)
	{
		buff = (uint8_t *) getfilled_dmabufferuacout48();
		if (buff == NULL)
		{
			// Канал ещё не успел начать работать - возвращаем 0.
			return 0;
		}
		n = 0;
	}

	// Использование данных.
	ASSERT(UACOUT_FMT_CHANNELS_AUDIO48 == 2);
	switch (UACOUT_AUDIO48_SAMPLEBYTES)
	{
	default:
		break;
	case 2:
		v->ivqv [L] = adpt_input(& uac48out, USBD_peek_u16(buff + n));	// левый канал
		n += UACOUT_AUDIO48_SAMPLEBYTES;
		v->ivqv [R] = adpt_input(& uac48out, USBD_peek_u16(buff + n));	// правый канал
		n += UACOUT_AUDIO48_SAMPLEBYTES;
		break;
	case 3:
		v->ivqv [L] = adpt_input(& uac48out, USBD_peek_u24(buff + n));	// левый канал
		n += UACOUT_AUDIO48_SAMPLEBYTES;
		v->ivqv [R] = adpt_input(& uac48out, USBD_peek_u24(buff + n));	// правый канал
		n += UACOUT_AUDIO48_SAMPLEBYTES;
		break;
	case 4:
		v->ivqv [L] = adpt_input(& uac48out, USBD_peek_u32(buff + n));	// левый канал
		n += UACOUT_AUDIO48_SAMPLEBYTES;
		v->ivqv [R] = adpt_input(& uac48out, USBD_peek_u32(buff + n));	// правый канал
		n += UACOUT_AUDIO48_SAMPLEBYTES;
		break;
	}

	// test
//	v->ivqv [L] = get_lout();	// левый канал
//	v->ivqv [R] = get_rout();	// правый канал

	if (n >= UACOUT_AUDIO48_DATASIZE_DMAC)
	{
		release_dmabufferuacout48((uintptr_t) buff);
		buff = NULL;
	}
	return 1;
}

// звук для самоконтроля
void savemonistereo(FLOAT_t ch0, FLOAT_t ch1)
{
	enum { L, R };
	// если есть инициализированный канал для выдачи звука
	static aubufv_t * buff = NULL;
	static unsigned n;

	if (buff == NULL)
	{
		buff = (aubufv_t *) allocate_dmabuffer16txmoni();
		n = 0;
	}

	buff [n * DMABUFFSTEP16TX + DMABUFF16TX_LEFT] = adpt_outputexact(& afcodectx, ch0);	// sample value
	buff [n * DMABUFFSTEP16TX + DMABUFF16TX_RIGHT] = adpt_outputexact(& afcodectx, ch1);	// sample value

	if (++ n >= CNT16TX)
	{
		save_dmabuffer16txmoni((uintptr_t) buff);
		buff = NULL;
	}
}

#endif /* WITHINTEGRATEDDSP */

// message buffers
/* Cообщения от уровня обработчиков прерываний к user-level функциям. */

typedef struct
{
	messagetypes_t type;
	uint8_t buff [MSGBUFFERSIZE8];
} message8buff_t;

typedef blists<message8buff_t, MESSAGE_CAPACITY> message8list_t;

static message8list_t message8list(MESSAGE_IRQL);

// Освобождение обработанного буфера сообщения
void releasemsgbuffer(uint8_t * dest)
{
	message8buff_t * const p = CONTAINING_RECORD(dest, message8buff_t, buff);
	message8list.release_buffer(p);
}
// Буфер для формирования сообщения
size_t takemsgbufferfree(uint8_t * * dest)
{
	message8buff_t * addr;
	if (message8list.get_freebuffer(& addr))
	{
		* dest = addr->buff;
		return sizeof addr->buff;
	}
	return 0;
}
// поместить сообщение в очередь к исполнению
void placesemsgbuffer(messagetypes_t type, uint8_t * dest)
{
	message8buff_t * const p = CONTAINING_RECORD(dest, message8buff_t, buff);
	p->type = type;
	message8list.save_buffer(p);
}

// Буферы с принятымти от обработчиков прерываний сообщениями
messagetypes_t takemsgready(uint8_t * * dest)
{
	message8buff_t * addr;
	if (message8list.get_readybuffer(& addr))
	{
		* dest = addr->buff;
		return addr->type;
	}
	return MSGT_EMPTY;
}

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


typedef struct modems8
{
	LIST_ENTRY item;
	size_t length;
	uint8_t buff [MODEMBUFFERSIZE8];
} modems8_t;

static RAMBIGDTCM LIST_HEAD2 modemsfree8;		// Свободные буферы
static RAMBIGDTCM LIST_HEAD2 modemsrx8;	// Буферы с принятымти через модем данными
//static LIST_ENTRY modemstx8;	// Буферы с данными для передачи через модем



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

#if WITHUSEAUDIOREC

// работа с аудиофайлами на накопителе

typedef ALIGNX_BEGIN struct recordswav48
{
	ALIGNX_BEGIN int16_t buff [AUDIORECBUFFSIZE16] ALIGNX_END;
	unsigned startdata;
	unsigned topdata;
} ALIGNX_END recordswav48_t;

// буферы: один заполняется, один воспроизводлится и два свободных (с одинм бывают пропуски).
typedef blists<recordswav48_t, AUDIOREC_CAPACITY> recordswav48list_t;

static recordswav48list_t recordswav48list(IRQL_REALTIME);


// Поэлементное заполнение буфера SD CARD

/* to SD CARD */
// 16 bit, signed
void RAMFUNC savesamplewav48(int_fast32_t left, int_fast32_t right)
{
	// если есть инициализированный канал для выдачи звука
	static recordswav48_t * p = NULL;
	static unsigned n;

	if (p == NULL)
	{
		while (recordswav48list.get_freebufferforced(& p) == 0)
			ASSERT(0);

		n = 0;

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
	p->buff [n ++] = left;	// sample value
	p->buff [n ++] = right;	// sample value

#else /* WITHUSEAUDIOREC2CH */
	// Запись звука на SD CARD в моно
	p->buff [n ++] = left;	// sample value

#endif /* WITHUSEAUDIOREC2CH */

	if (n >= AUDIORECBUFFSIZE16)
	{
		/* используется буфер целиклом */
		p->startdata = 0;
		p->topdata = AUDIORECBUFFSIZE16;
		recordswav48list.save_buffer(p);
		p = NULL;
	}
}

// user-mode function
unsigned takerecordbuffer(void * * dest)
{
	recordswav48_t * p;
	if (recordswav48list.get_readybuffer(& p))
	{
		* dest = p->buff;
		return (AUDIORECBUFFSIZE16 * sizeof p->buff [0]);
	}
	return 0;
}

// user-mode function
unsigned takefreerecordbuffer(void * * dest)
{
	recordswav48_t * p;
	if (recordswav48list.get_freebuffer(& p))
	{
		* dest = p->buff;
		return (AUDIORECBUFFSIZE16 * sizeof p->buff [0]);
	}
	return 0;
}

static void saveplaybuffer(void * dest, unsigned used)
{
	recordswav48_t * const p = CONTAINING_RECORD(dest, recordswav48_t, buff);
	p->startdata = 0;	// первый сэмпл в буфере
	p->topdata = used / sizeof p->buff [0];	// количество сэмплов

	recordswav48list.save_buffer(p);
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
	recordswav48list.release_buffer(p);
}

void saverecordbuffer(void * dest)
{
	recordswav48_t * const p = CONTAINING_RECORD(dest, recordswav48_t, buff);
	recordswav48list.save_buffer(p);
}


/* Получение пары (левый и правый) сжмплов для воспроизведения через аудиовыход трансивера
 * или для переачи
 * Возврат 0, если нет ничего для воспроизведения.
 */
uint_fast8_t takewavsample(FLOAT32P_t * rv, uint_fast8_t suspend)
{
	static recordswav48_t * p = NULL;
	static unsigned n;
	if (p == NULL)
	{
		if (recordswav48list.get_readybuffer(& p))
		{
			n = p->startdata;	// reset samples count
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
		recordswav48list.release_buffer(p);
		p = NULL;
		//PRINTF("Release record buffer\n");
	}
	return 1;	// Сэмпл считан
}

#endif /* WITHUSEAUDIOREC */

#if 0

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
		.pdata = NULL,
		.skipsense = SKIPPEDBLOCKS,
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

#if WITHUSBUAC && defined (WITHUSBHW_DEVICE)

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
#if WITHUSBUACOUT && defined (WITHUSBHW_DEVICE)
	buffer_resample(& uacout48rsmpl);
#endif /* WITHUSBUACOUT */
#if WITHUSBUACIN && defined (WITHUSBHW_DEVICE)
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
		release_dmabuffer16txphones(getfilled_dmabuffer16txphones());
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
		release_dmabuffer16txphones(getfilled_dmabuffer16txphones());
#endif /* ! WITHI2S2HW && ! CPUSTYLE_XC7Z */
		n -= CNT16TX;
	}

}

#endif /* WITHUSBUAC */

#endif

void buffers2_diagnostics(void)
{
#if WITHBUFFERSDEBUG
#if 0
	//denoise16list.debug("denoise16");
	voice16rxcodeclist.debug("voice16rxcodec");
	voice16txphones.debug("voice16txphones");
	voice16txmoni.debug("voice16txmoni");
	voice32txlist.debug("voice32tx");
	voice32rxlist.debug("voice32rx");
#endif
#if 1
	// USB
	uacout48list.debug("uacout48");
	//uacinrts192list.debug("uacinrts192");
	uacinrts96list.debug("uacinrts96");
	uacin48list.debug("uacin48");
#endif
	//message8list.debug("message8");

	PRINTF("\n");

#endif /* WITHBUFFERSDEBUG */
//	PRINTF("__get_CPUACTLR()=%016" PRIX64 "\n", __get_CPUACTLR());
//	PRINTF("__get_CPUACTLR()=%016" PRIX64 "\n", UINT64_C(1) << 44);
}
