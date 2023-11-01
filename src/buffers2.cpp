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
#define VOICE16TX_CAPACITY (64 * BUFOVERSIZE)	// должно быть достаточное количество буферов чтобы запомнить буфер с выхода speex
#define VOICE16TXMONI_CAPACITY (4 * BUFOVERSIZE)	// во столько же на сколько буфр от кодека больше чем буфер к кодеку (если наоборот - минимум)

#define VOICE16RX_RESAMPLING 0	// прием от кодека - требуется ли resampling
#define VOICE16TX_RESAMPLING 0	// передача в кодек - требуется ли resampling

#define UACINRTS192_CAPACITY (2 * 8 * BUFOVERSIZE)
#define UACINRTS96_CAPACITY (2 * 8 * BUFOVERSIZE)
#define UACOUT48_CAPACITY (128 + 16 * BUFOVERSIZE)
#define UACIN48_CAPACITY (128 * BUFOVERSIZE)	// должно быть достаточное количество буферов чтобы запомнить буфер с выхода speex

#define SPEEX_CAPACITY (5 * BUFOVERSIZE)

#define VOICE32RX_CAPACITY (2 + 6 * BUFOVERSIZE)
#define VOICE32TX_CAPACITY (6 * BUFOVERSIZE)
#define VOICE32RTS_CAPACITY (4 * BUFOVERSIZE)	// dummy fn

#define AUDIOREC_CAPACITY (18 * BUFOVERSIZE)

#define MESSAGE_CAPACITY (12)
#define MESSAGE_IRQL IRQL_SYSTEM

#define MODEM8_CAPACITY (8)
#define MODEM8_IRQL IRQL_SYSTEM

#if WITHUSBHW
	#include "usb/usb200.h"
	#include "usb/usbch9.h"
#endif /* WITHUSBHW */

#include <string.h>		// for memset



#if WITHINTEGRATEDDSP

#if WITHUSBHW && WITHUSBUAC

static volatile uint_fast8_t uacinalt = UACINALT_NONE;		/* выбор альтернативной конфигурации для UAC IN interface */
static volatile uint_fast8_t uacinrtsalt = UACINRTSALT_NONE;		/* выбор альтернативной конфигурации для RTS UAC IN interface */
static volatile uint_fast8_t uacoutalt;

#else /* WITHUSBHW && WITHUSBUAC */

static const uint_fast8_t uacinalt = 0;		/* выбор альтернативной конфигурации для UAC IN interface */
static const uint_fast8_t uacinrtsalt = 0;		/* выбор альтернативной конфигурации для RTS UAC IN interface */
static const uint_fast8_t uacoutalt = 0;

#endif /* WITHUSBHW && WITHUSBUAC */


#if WITHRTS192 || WITHRTS96

	static subscribeint32_t uacinrtssubscribe;

#endif /* WITHRTS96 */


#endif /* WITHINTEGRATEDDSP */


template<typename apptype, int ss, int nch>
class adapters
{
	const char * name;
	adapter_t adp;
public:
	adapters(int leftbit, int rightspace, const char * aname) :
		name(aname)
	{
		adpt_initialize(& adp, leftbit, rightspace, name);
	}

	// преобразование в буфер из внутреннего представления
	unsigned poke(uint8_t * buff, apptype ch0, apptype ch1)
	{
		switch (nch)
		{
		case 1:
			switch (ss)
			{
			case 2:
				USBD_poke_u16(buff + 0, adpt_output(& adp, ch0));
				break;
			case 3:
				USBD_poke_u24(buff + 0, adpt_output(& adp, ch0));
				break;
			case 4:
				USBD_poke_u32(buff + 0, adpt_output(& adp, ch0));
				break;
			}
			break;

		case 2:
			switch (ss)
			{
			case 2:
				USBD_poke_u16(buff + 0, adpt_output(& adp, ch0));
				USBD_poke_u16(buff + 2, adpt_output(& adp, ch1));
				break;
			case 3:
				USBD_poke_u24(buff + 0, adpt_output(& adp, ch0));
				USBD_poke_u24(buff + 3, adpt_output(& adp, ch1));
				break;
			case 4:
				USBD_poke_u32(buff + 0, adpt_output(& adp, ch0));
				USBD_poke_u32(buff + 4, adpt_output(& adp, ch1));
				break;
			}
			break;
		}
		return ss * nch;
	}


	// преобразование в буфер из внутреннего представления
	unsigned poketransf(const transform_t * tfm, uint8_t * buff, apptype ch0, apptype ch1)
	{
		switch (nch)
		{
		case 1:
			switch (ss)
			{
			case 2:
				USBD_poke_u16(buff + 0, transform_do32(tfm, ch0));
				break;
			case 3:
				USBD_poke_u24(buff + 0, transform_do32(tfm, ch0));
				break;
			case 4:
				USBD_poke_u32(buff + 0, transform_do32(tfm, ch0));
				break;
			}
			break;

		case 2:
			switch (ss)
			{
			case 2:
				USBD_poke_u16(buff + 0, transform_do32(tfm, ch0));
				USBD_poke_u16(buff + 2, transform_do32(tfm, ch1));
				break;
			case 3:
				USBD_poke_u24(buff + 0, transform_do32(tfm, ch0));
				USBD_poke_u24(buff + 3, transform_do32(tfm, ch1));
				break;
			case 4:
				USBD_poke_u32(buff + 0, transform_do32(tfm, ch0));
				USBD_poke_u32(buff + 4, transform_do32(tfm, ch1));
				break;
			}
			break;
		}
		return ss * nch;
	}

	// во внутреннее представление из буфера
	unsigned peek(const uint8_t * buff, apptype * dest)
	{
		switch (nch)
		{
		case 1:
			switch (ss)
			{
			case 2:
				dest [0] = adpt_input(& adp, USBD_peek_u16(buff + 0));
				break;
			case 3:
				dest [0] = adpt_input(& adp, USBD_peek_u24(buff + 0));
				break;
			case 4:
				dest [0] = adpt_input(& adp, USBD_peek_u32(buff + 0));
				break;
			}
			break;

		case 2:
			switch (ss)
			{
			case 2:
				dest [0] = adpt_input(& adp, USBD_peek_u16(buff + 0));
				dest [1] = adpt_input(& adp, USBD_peek_u16(buff + 2));
				break;
			case 3:
				dest [0] = adpt_input(& adp, USBD_peek_u24(buff + 0));
				dest [1] = adpt_input(& adp, USBD_peek_u24(buff + 3));
				break;
			case 4:
				dest [0] = adpt_input(& adp, USBD_peek_u32(buff + 0));
				dest [1] = adpt_input(& adp, USBD_peek_u32(buff + 4));
				break;
			}
			break;
		}
		return ss * nch;
	}

	static int sssize() { return ss * nch; }

};


//////////////////////

class fqmeter
{
	unsigned debugcount;
	unsigned ms10;
public:
	fqmeter() : debugcount(0), ms10(0)
	{

	}

	void pass(unsigned samples)
	{
		debugcount += samples;
	}

	unsigned getfreq()
	{
		unsigned v = debugcount;
		debugcount = 0;

		unsigned v10ms = ms10;
		ms10 = 0;

		if (v10ms == 0)
			return 0;
		return v * 10000 / v10ms;
	}
	void spool10ms()
	{
		ms10 += 10000 / TICKS_FREQUENCY;
	}
};


template <typename element_t, unsigned capacity, int hasresample>
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
	int rslevel;
	LIST_ENTRY freelist;
	LIST_ENTRY readylist;
	buffitem_t storage [capacity];
	const char * name;
	element_t * workbuff;	// буфер над которым выполняется ресэмплинг
	unsigned wbstart;	// start position of work buffer - zero has not meaning

	fqmeter fqin, fqout;

public:
	blists(IRQL_t airql, const char * aname) :
#if WITHBUFFERSDEBUG
		errallocate(0),
		saveount(0),
#endif /* WITHBUFFERSDEBUG */
		outcount(0),
		freecount(capacity),
		rslevel(0),
		name(aname),
		wbstart(0)	// начало данных в workbuff->buff
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
		//PRINTF("%s: %u[%u]\n", name, sizeof (storage [0].v.buff), capacity);

        // resampler test
		if (hasresample)
		{
			VERIFY(get_freebuffer(& workbuff));
			wbstart = 7 * element_t::ss * element_t::nch;
			//PRINTF("%s: test resampler: wbstart=%u\n", name, wbstart);
		}
	}

	void rslevelreset()
	{
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql);

		rslevel = 0;

		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
	}
	void rsleveladd(int inc)
	{
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql);

		rslevel += inc;

		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
	}
	// сохранить в списке свободных
	void release_buffer(element_t * addr)
	{
		buffitem_t * const p = CONTAINING_RECORD(addr, buffitem_t, v);
		ASSERT3(p->tag0 == this, __FILE__, __LINE__, name);
		ASSERT3(p->tag2 == p, __FILE__, __LINE__, name);
		ASSERT3(p->tag3 == p, __FILE__, __LINE__, name);
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
		ASSERT3(p->tag0 == this, __FILE__, __LINE__, name);
		ASSERT3(p->tag2 == p, __FILE__, __LINE__, name);
		ASSERT3(p->tag3 == p, __FILE__, __LINE__, name);

		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql);

		InsertHeadList(& readylist, & p->item);
		++ outcount;
#if WITHBUFFERSDEBUG
		fqin.pass(sizeof addr->buff / (addr->ss * addr->nch));
		++ saveount;
		rslevel += sizeof addr->buff / (addr->ss * addr->nch);
#endif /* WITHBUFFERSDEBUG */

		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
	}

	// получить из списка готовых
	int get_readybuffer_raw(element_t * * dest)
	{
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql);
		if (! IsListEmpty(& readylist))
		{
			const PLIST_ENTRY t = RemoveTailList(& readylist);
			-- outcount;
#if WITHBUFFERSDEBUG
			fqout.pass(sizeof (* dest)->buff / ((* dest)->ss * (* dest)->nch));
#endif /* WITHBUFFERSDEBUG */
			rslevel -= sizeof (* dest)->buff / ((* dest)->ss * (* dest)->nch);
			IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
			buffitem_t * const p = CONTAINING_RECORD(t, buffitem_t, item);
			ASSERT3(p->tag0 == this, __FILE__, __LINE__, name);
			ASSERT3(p->tag2 == p, __FILE__, __LINE__, name);
			ASSERT3(p->tag3 == p, __FILE__, __LINE__, name);
			* dest = & p->v;
			return 1;
		}
		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
		return 0;
	}
	int get_readybuffer(element_t * * dest)
	{
		if (hasresample)
		{
			return get_readybufferarj(dest, false, false, false);
		}
		else
		{
			return get_readybuffer_raw(dest);
		}
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
			ASSERT3(p->tag0 == this, __FILE__, __LINE__, name);
			ASSERT3(p->tag2 == p, __FILE__, __LINE__, name);
			ASSERT3(p->tag3 == p, __FILE__, __LINE__, name);
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
		return get_freebuffer(dest) || get_readybuffer_raw(dest);
	}

	// получить из списка готовых, если нет - из свободных
	int get_readybufferforced(element_t * * dest)
	{
		return get_readybuffer_raw(dest) || get_freebuffer(dest);
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

	void debug()
	{
#if WITHBUFFERSDEBUG
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql);
		unsigned fin = fqin.getfreq();
		unsigned fout = fqout.getfreq();
		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
		//PRINTF("%s:s=%d,a=%d,o=%d,f=%d ", name, saveount, errallocate, outcount, freecount);
		PRINTF("%s:b=%d/%d,q=%u/%u,v=%d ", name, outcount, freecount, fin, fout, rslevel);
#endif /* WITHBUFFERSDEBUG */
	}

	void spool10ms()
	{
#if WITHBUFFERSDEBUG
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql);

		fqin.spool10ms();
		fqout.spool10ms();

		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
#endif /* WITHBUFFERSDEBUG */
	}

	///////////
	/// resampler

	// функция вызывается получателем (получаем после ресэмплинга.
	// Гарантированно получене буфера
	// исполнение команд на добавление или удаление одного семплв, на замену потока тишиной
	int get_readybufferarj(element_t * * dest, bool zero, bool add, bool del)
	{
		if (wbstart != 0 && 1)
		{
			// Выходной буфер всегда запрашивается.
			// каждый вызов метода get_readybufferarj возвращает или целый буфер или ничего.
			if (get_freebuffer(dest) == 0)	// выходной буфр
				return 0;

			// Есть не полностью израсходованный остаток в буфере
			const int ototal = sizeof (workbuff->buff);

			const int o1 = wbstart;
			const int p1 = sizeof (workbuff->buff) - wbstart;	// размер, оставшийся до конца буфера

			const int o2 = 0;
			const int p2 = 0;	// вставляемый кусок

			const int o3 = 0;
			const int p3 = wbstart;	// размер от начала буфера - в рабочем буфере уже передано
			// есть остаток старого буфера

			int o = 0;
			// Часть, оставшаяся от предыдущих пересылок

			memcpy((uint8_t *) (* dest)->buff + o, (uint8_t *) workbuff->buff + o1, p1);	// копируем остаток предыдущего буфера
			o += p1;
			// остаток передали весь
			release_buffer(workbuff);

			// Вставляемая часть
			if (p2 != 0)
			{
				o += p2;
			}

			if (o < ototal)
			{
				while (get_readybufferforced(& workbuff) == 0)	// следующий готовый
					ASSERT(0);
				memcpy((uint8_t *) (* dest)->buff + o, (uint8_t *) workbuff->buff + o3, p3);	// копируем часть следующего буфера
				o += p3;
			}
			ASSERT(o == ototal);
			return 1;
		}
		return get_readybuffer_raw(dest);
	}

};

template <typename sample_t, typename element_t, unsigned capacity, int hasresample>
class dmahandle: public blists<element_t, capacity, hasresample>
{
	typedef blists<element_t, capacity, hasresample> parent_t;
	element_t * wb;
	unsigned wbn;
	element_t * rb;
	unsigned rbn;

public:
	dmahandle(IRQL_t airql, const char * aname) :
		parent_t(airql, aname),
		wb(NULL),
		rb(NULL)
	{

	}

	// поэлементное заполнение буферов
	void savedata(sample_t ch0, sample_t ch1, unsigned (* putcbf)(typeof (element_t::buff [0]) * b, sample_t ch0, sample_t ch1))
	{
		if (wb == NULL)
		{
			if (parent_t::get_freebuffer(& wb) == 0)
				return;
			wbn = 0;
		}
		wbn += putcbf(wb->buff + wbn, ch0, ch1);
		if (wbn >= ARRAY_SIZE(wb->buff))
		{
			parent_t::save_buffer(wb);
			wb = NULL;
		}
	}

	uint_fast8_t fetchdata(sample_t * dest, unsigned (* getcbf)(typeof (element_t::buff [0]) * b, sample_t * dest))
	{
		if (rb == NULL)
		{
			if (parent_t::get_readybuffer(& rb) == 0)
				return 0;
			rbn = 0;
		}
		rbn += getcbf(rb->buff + rbn, dest);
		if (rbn >= ARRAY_SIZE(wb->buff))
		{
			parent_t::release_buffer(rb);
			rb = NULL;
		}
		return 1;
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
enum { CNT16TXF = DMABUFFSIZE16TXF / DMABUFFSTEP16TXF };
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
	enum { ss = 1, nch = NTRX };	// stub for resampling support
} ALIGNX_END denoise16_t;

// буферы: один заполняется, один воспроизводлится и два свободных (с одинм бывают пропуски).
typedef dmahandle<FLOAT_t, denoise16_t, SPEEX_CAPACITY, 0> denoise16dma_t;

static denoise16dma_t denoise16list(IRQL_REALTIME, "denoise16");

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
	enum { ss = sizeof (aubufv_t), nch = DMABUFFSTEP16RX };	// resampling support
} ALIGNX_END voice16rx_t;

typedef dmahandle<FLOAT_t, voice16rx_t, VOICE16RX_CAPACITY, VOICE16RX_RESAMPLING> voice16rxdma_t;

static voice16rxdma_t voice16rx(IRQL_REALTIME, "16rx");		// from codec

int_fast32_t cachesize_dmabuffer16rx(void)
{
	return voice16rx.get_cachesize();
}

// can not be zero
uintptr_t allocate_dmabuffer16rx(void)
{
	voice16rx_t * dest;
	while (voice16rx.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

// may be zero
uintptr_t getfilled_dmabuffer16rx(void)
{
	voice16rx_t * dest;
	if (voice16rx.get_readybuffer(& dest) == 0)
		return 0;
	return (uintptr_t) dest->buff;
}

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned voice16rx_getcbf(aubufv_t * b, FLOAT_t * dest)
{
	enum { L, R };
	dest [L] = adpt_input(& afcodecrx, b [DMABUFF16RX_MIKE]);
	dest [R] = adpt_input(& afcodecrx, b [DMABUFF16RX_MIKE]);
	return DMABUFFSTEP16RX;
}

// Возвращает не-ноль если данные есть
uint_fast8_t elfetch_dmabuffer16rx(FLOAT_t * dest)
{
	return voice16rx.fetchdata(dest, voice16rx_getcbf);
}

void save_dmabuffer16rx(uintptr_t addr)
{
	voice16rx_t * const p = CONTAINING_RECORD(addr, voice16rx_t, buff);
	voice16rx.save_buffer(p);
}

void release_dmabuffer16rx(uintptr_t addr)
{
	voice16rx_t * const p = CONTAINING_RECORD(addr, voice16rx_t, buff);
	voice16rx.release_buffer(p);
}

// Audio CODEC in (from processor)
typedef ALIGNX_BEGIN struct voice16tx_tag
{
	ALIGNX_BEGIN aubufv_t buff [DMABUFFSIZE16TX] ALIGNX_END;
	ALIGNX_BEGIN uint8_t pad ALIGNX_END;
	enum { ss = sizeof (aubufv_t), nch = DMABUFFSTEP16TX };	// resampling support
} ALIGNX_END voice16tx_t;

// Sidetone
typedef ALIGNX_BEGIN struct voice16txF_tag
{
	ALIGNX_BEGIN FLOAT_t buff [DMABUFFSIZE16TXF] ALIGNX_END;
	enum { ss = 2, nch = DMABUFFSTEP16TXF };	// stub for resampling support
} ALIGNX_END voice16txF_t;

typedef adapters<FLOAT_t, (int) UACOUT_AUDIO48_SAMPLEBYTES, (int) UACOUT_FMT_CHANNELS_AUDIO48> voice16txadpt_t;
typedef dmahandle<FLOAT_t, voice16tx_t, VOICE16TX_CAPACITY, VOICE16TX_RESAMPLING> voice16txdma_t;

static voice16txdma_t voice16tx(IRQL_REALTIME, "16tx");

typedef dmahandle<FLOAT_t, voice16txF_t, VOICE16TXMONI_CAPACITY, 0> voice16txmonidma_t;

static voice16txmonidma_t voice16txmoni(IRQL_REALTIME, "16moni");

int_fast32_t cachesize_dmabuffer16txphones(void)
{
	return voice16tx.get_cachesize();
}

static unsigned putbf_dmabuffer16txphones(aubufv_t * b, FLOAT_t ch0, FLOAT_t ch1)
{
	b [DMABUFF16TX_LEFT] = adpt_output(& afcodectx, ch0);
	b [DMABUFF16TX_RIGHT] = adpt_output(& afcodectx, ch1);
	return DMABUFFSTEP16TX;
}

void elfill_dmabuffer16txphones(FLOAT_t ch0, FLOAT_t ch1)
{
	voice16tx.savedata(ch0, ch1, putbf_dmabuffer16txphones);
}

// Поэлементное заполнение DMA буфера AF DAC
static void savesampleout16stereo_float(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	elfill_dmabuffer16txphones(ch0, ch1);
}

// can not be zero
uintptr_t allocate_dmabuffer16txphones(void)
{
	voice16tx_t * dest;
	while (voice16tx.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

void save_dmabuffer16txphones(uintptr_t addr)
{
	voice16tx_t * const p = CONTAINING_RECORD(addr, voice16tx_t, buff);
	voice16tx.save_buffer(p);
}

void release_dmabuffer16txphones(uintptr_t addr)
{
	voice16tx_t * const p = CONTAINING_RECORD(addr, voice16tx_t, buff);
	voice16tx.release_buffer(p);
}

uintptr_t getfilled_dmabuffer16txphones(void)
{
	voice16tx_t * phones;
	voice16txF_t * moni;
	do
	{
		if (voice16tx.get_readybuffer(& phones) )
			break;
		if (voice16tx.get_freebuffer(& phones) )
			break;
		voice16tx.debug();
		ASSERT(0);
		for (;;)
			;
		return 0;
	} while (0);

	if (voice16txmoni.get_readybuffer(& moni))
	{
		// Добавить самоконтроль
		// add sidetone
		dsp_addsidetone(phones->buff, moni->buff);
		voice16txmoni.release_buffer(moni);
	}
	else if (voice16txmoni.get_freebuffer(& moni))
	{
		// Добавить самоконтроль
		// add sidetone
		memset(moni->buff, 0, sizeof moni->buff);
		dsp_addsidetone(phones->buff, moni->buff);
		voice16txmoni.release_buffer(moni);
	}

#if 0
	// тестирование вывода на кодек
	for (unsigned i = 0; i < ARRAY_SIZE(phones->buff); i += DMABUFFSIZE16TX)
	{
		phones->buff [i + DMABUFF16TX_LEFT] = adpt_output(& afcodectx, get_lout());
		phones->buff [i + DMABUFF16TX_RIGHT] = adpt_output(& afcodectx, get_rout());
	}
#endif

	return (uintptr_t) phones->buff;
}

// sidetone forming


static unsigned putbf_dmabuffer16txmoni(FLOAT_t * b, FLOAT_t ch0, FLOAT_t ch1)
{
	b [0] = ch0;
	b [1] = ch1;
	return 2;
}

void elfill_dmabuffer16txmoni(FLOAT_t ch0, FLOAT_t ch1)
{
	voice16txmoni.savedata(ch0, ch1, putbf_dmabuffer16txmoni);
}

// can not be zero
uintptr_t allocate_dmabuffer16txmoni(void)
{
	voice16txF_t * dest;
	while (voice16txmoni.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

void save_dmabuffer16txmoni(uintptr_t addr)
{
	voice16txF_t * const p = CONTAINING_RECORD(addr, voice16txF_t, buff);
	voice16txmoni.save_buffer(p);
}

uintptr_t getfilled_dmabuffer16txmoni(void)
{
	voice16txF_t * dest;
	if (voice16txmoni.get_readybuffer(& dest) )
		return (uintptr_t) dest->buff;
	if (voice16txmoni.get_freebuffer(& dest) )
	{
		memset(dest->buff, 0, sizeof dest->buff);
		return (uintptr_t) dest->buff;
	}
	ASSERT(0);
	for (;;)
		;
	return 0;
}

void release_dmabuffer16txmoni(uintptr_t addr)
{
	voice16txF_t * const p = CONTAINING_RECORD(addr, voice16txF_t, buff);
	voice16txmoni.release_buffer(p);
}

// I/Q data to FPGA or IF CODEC
typedef ALIGNX_BEGIN struct voices32tx_tag
{
	ALIGNX_BEGIN IFDACvalue_t buff [DMABUFFSIZE32TX] ALIGNX_END;
	ALIGNX_BEGIN uint8_t pad ALIGNX_END;
	enum { ss = sizeof (IFDACvalue_t), nch = DMABUFFSTEP32TX };	// resampling support
} ALIGNX_END voice32tx_t;


typedef dmahandle<int_fast32_t, voice32tx_t, VOICE32TX_CAPACITY, 0> voice32txdma_t;

//static voice32txlist_t voice32tx(IRQL_REALTIME, "32tx");
static voice32txdma_t voice32tx(IRQL_REALTIME, "32tx");

int_fast32_t cachesize_dmabuffer32tx(void)
{
	return voice32tx.get_cachesize();
}


static unsigned putbf_dmabuffer32tx(IFDACvalue_t * buff, int_fast32_t ch0, int_fast32_t ch1)
{

	buff [DMABUF32TXI] = ch0;
	buff [DMABUF32TXQ] = ch1;

#if defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_GW2A_V0)

#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_FPGAV1) && ! (CTLREGMODE_OLEG4Z_V1 || CTLREGMODE_OLEG4Z_V2)
	/* установка параметров приемника, передаваемых чрез I2S канал в FPGA */
	buff [DMABUF32TX_NCO1] = dspfpga_get_nco1();
	buff [DMABUF32TX_NCO2] = dspfpga_get_nco2();
	buff [DMABUF32TX_NCORTS] = dspfpga_get_ncorts();
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

	return DMABUFFSTEP32TX;
}

void elfill_dmabuffer32tx(int_fast32_t ch0, int_fast32_t ch1)
{
	voice32tx.savedata(ch0, ch1, putbf_dmabuffer32tx);
}

void release_dmabuffer32tx(uintptr_t addr)
{
	voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
	voice32tx.release_buffer(p);
}

// can not be zero
uintptr_t allocate_dmabuffer32tx(void)
{
	voice32tx_t * dest;
	while (voice32tx.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

void save_dmabuffer32tx(uintptr_t addr)
{
	voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
	voice32tx.save_buffer(p);
}

// can not be be zero
uintptr_t getfilled_dmabuffer32tx(void)
{
	voice32tx_t * dest;
	if (voice32tx.get_readybuffer(& dest))
		return (uintptr_t) dest->buff;
	if (voice32tx.get_freebuffer(& dest))
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
	enum { ss = sizeof (IFADCvalue_t), nch = DMABUFFSTEP32RX };	// resampling support
} ALIGNX_END voice32rx_t;


typedef dmahandle<int_fast32_t, voice32rx_t, VOICE32RX_CAPACITY, 0> voice32rxlist_t;

static voice32rxlist_t voice32rx(IRQL_REALTIME, "32rx");

int_fast32_t cachesize_dmabuffer32rx(void)
{
	return voice32rx.get_cachesize();
}

void release_dmabuffer32rx(uintptr_t addr)
{
	voice32rx_t * const p = CONTAINING_RECORD(addr, voice32rx_t, buff);
	voice32rx.release_buffer(p);
}

// can not be zero
uintptr_t allocate_dmabuffer32rx(void)
{
	voice32rx_t * dest;
	while (voice32rx.get_freebuffer(& dest) == 0)
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
	enum { ss = UACOUT_AUDIO48_SAMPLEBYTES, nch = UACOUT_FMT_CHANNELS_AUDIO48 };	// resampling support
} uacout48_t;

typedef dmahandle<FLOAT_t, uacout48_t, UACOUT48_CAPACITY, 1> uacout48dma_t;

typedef adapters<FLOAT_t, (int) UACOUT_AUDIO48_SAMPLEBYTES, (int) UACOUT_FMT_CHANNELS_AUDIO48> uacout48adpt_t;

static uacout48dma_t uacout48(IRQL_REALTIME, "uaco48");
//static uacout48dma_t uacout48_rs(IRQL_REALTIME, "uaco48_rs");

static uacout48adpt_t uacout48adpt(UACOUT_AUDIO48_SAMPLEBYTES * 8, 0, "uaco48");

int_fast32_t cachesize_dmabufferuacout48(void)
{
	return uacout48.get_cachesize();
}

// can not be zero
uintptr_t allocate_dmabufferuacout48(void)
{
	uacout48_t * dest;
	while (uacout48.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

// may be zero
uintptr_t getfilled_dmabufferuacout48(void)
{
	uacout48_t * dest;
	if (uacout48.get_readybuffer(& dest) == 0)
		return 0;
	return (uintptr_t) dest->buff;
}

void release_dmabufferuacout48(uintptr_t addr)
{
	uacout48_t * const p = CONTAINING_RECORD(addr, uacout48_t, buff);
	uacout48.release_buffer(p);
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
	if (uacoutalt != UACOUTALT_AUDIO48)
	{
		uacout48.release_buffer(p);
		return;
	}
	// временное решение проблемы щелчкчков
//	pp(p->buff, sizeof p->buff);
//	pp(p->buff, sizeof p->buff);
	uacout48.save_buffer(p);
	return;

//	uacout48_t * p2;
//	while (uacout48_rs.get_freebufferforced(& p2) == 0)
//		ASSERT(0);
//	memcpy(p2->buff, p->buff, sizeof p2->buff);
//	//printhex(0, p2->buff, sizeof p2->buff);
//	uacout48.release_buffer(p);
//
//	uacout48_rs.save_buffer(p2);
}

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned uacout48_getcbf(uint8_t * b, FLOAT_t * dest)
{
	return uacout48adpt.peek(b, dest);
}

// Возвращает не-ноль если данные есть
uint_fast8_t elfetch_dmabufferuacout48(FLOAT_t * dest)
{
	// WITHUSBUACOUT test
//	enum { L, R };
//	dest [L] = get_lout();	// левый канал
//	dest [R] = get_rout();	// правый канал
//	return 1;
	return uacout48.fetchdata(dest, uacout48_getcbf);
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
	enum { ss = UACIN_RTS192_SAMPLEBYTES, nch = UACIN_FMT_CHANNELS_RTS192 };	// resampling support
} uacinrts192_t;

typedef dmahandle<int_fast32_t, uacinrts192_t, UACINRTS192_CAPACITY, 1> uacinrts192dma_t;

typedef adapters<int_fast32_t, (int) UACIN_RTS192_SAMPLEBYTES, (int) UACIN_FMT_CHANNELS_RTS192> uacinrts192adpt_t;

//static uacinrts192list_t uacinrts192(IRQL_REALTIME, "uacin192");
static uacinrts192dma_t uacinrts192(IRQL_REALTIME, "uacin192");
static uacinrts192adpt_t uacinrts192adpt(UACIN_RTS192_SAMPLEBYTES * 8, 0, "uacin192");

int_fast32_t cachesize_dmabufferuacinrts192(void)
{
	return uacinrts192.get_cachesize();
}

static unsigned putbf_dmabufferuacinrts192(uint8_t * b, int_fast32_t ch0, int_fast32_t ch1)
{
	return uacinrts192adpt.poketransf(& if2rts192out, b, ch0, ch1);
}

void elfill_dmabufferuacinrts192(int_fast32_t ch0, int_fast32_t ch1)
{
	uacinrts192.savedata(ch0, ch1, putbf_dmabufferuacinrts192);
}

// can not be zero
uintptr_t allocate_dmabufferuacinrts192(void)
{
	uacinrts192_t * dest;
	while (uacinrts192.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	dest->tag = BUFFTAG_RTS192;
	return (uintptr_t) & dest->buff;
}

// can not be zero
uintptr_t getfilled_dmabufferuacinrts192(void)
{
	uacinrts192_t * dest;
	while (uacinrts192.get_readybufferforced(& dest) == 0)
		ASSERT(0);
	dest->tag = BUFFTAG_RTS192;
	return (uintptr_t) & dest->buff;
	return 0;
}

void save_dmabufferuacinrts192(uintptr_t addr)
{
	uacinrts192_t * const p = CONTAINING_RECORD(addr, uacinrts192_t, buff);
	ASSERT(p->tag == BUFFTAG_RTS192);
	uacinrts192.save_buffer(p);
}


void release_dmabufferuacinrts192(uintptr_t addr)
{
	uacinrts192_t * const p = CONTAINING_RECORD(addr, uacinrts192_t, buff);
	ASSERT(p->tag == BUFFTAG_RTS192);
	uacinrts192.release_buffer(p);
}

#elif WITHRTS96

	typedef struct
	{
		uacintag_t tag;
		ALIGNX_BEGIN  uint8_t buff [UACIN_RTS96_DATASIZE_DMAC] ALIGNX_END;
		ALIGNX_BEGIN  uint8_t pad ALIGNX_END;
		enum { ss = UACIN_RTS96_SAMPLEBYTES, nch = UACIN_FMT_CHANNELS_RTS96 };	// resampling support
	} uacinrts96_t;

	typedef dmahandle<int_fast32_t, uacinrts96_t, UACINRTS96_CAPACITY, 1> uacinrts96dma_t;

	typedef adapters<int_fast32_t, (int) UACIN_RTS96_SAMPLEBYTES, (int) UACIN_FMT_CHANNELS_RTS96> uacinrts96adpt_t;

	static uacinrts96dma_t uacinrts96(IRQL_REALTIME, "uacin96");
	static uacinrts96adpt_t uacinrts96adpt(UACIN_RTS96_SAMPLEBYTES * 8, 0, "uacin96");

	int_fast32_t cachesize_dmabufferuacinrts96(void)
	{
		return uacinrts96.get_cachesize();
	}

	static unsigned putbf_dmabufferuacinrts96(uint8_t * b, int_fast32_t ch0, int_fast32_t ch1)
	{
		return uacinrts96adpt.poketransf(& if2rts96out, b, ch0, ch1);
	}

	void elfill_dmabufferuacinrts96(int_fast32_t ch0, int_fast32_t ch1)
	{
		uacinrts96.savedata(ch0, ch1, putbf_dmabufferuacinrts96);
	}

	// can not be zero
	uintptr_t allocate_dmabufferuacinrts96(void)
	{
		uacinrts96_t * dest;
		while (uacinrts96.get_freebufferforced(& dest) == 0)
			ASSERT(0);
		dest->tag = BUFFTAG_RTS96;
		return (uintptr_t) & dest->buff;
	}

	// can not be zero
	uintptr_t getfilled_dmabufferuacinrts96(void)
	{
		uacinrts96_t * dest;
		while (uacinrts96.get_readybufferforced(& dest) == 0)
			ASSERT(0);
		dest->tag = BUFFTAG_RTS96;
		return (uintptr_t) & dest->buff;
		return 0;
	}

	void save_dmabufferuacinrts96(uintptr_t addr)
	{
		uacinrts96_t * const p = CONTAINING_RECORD(addr, uacinrts96_t, buff);
		ASSERT(p->tag == BUFFTAG_RTS96);
		uacinrts96.save_buffer(p);
	}


	void release_dmabufferuacinrts96(uintptr_t addr)
	{
		uacinrts96_t * const p = CONTAINING_RECORD(addr, uacinrts96_t, buff);
		ASSERT(p->tag == BUFFTAG_RTS96);
		uacinrts96.release_buffer(p);
	}

#endif /* WITHRTS96 */

// USB AUDIO IN

typedef struct
{
	uacintag_t tag;
	ALIGNX_BEGIN  uint8_t buff [UACIN_AUDIO48_DATASIZE_DMAC] ALIGNX_END;
	ALIGNX_BEGIN  uint8_t pad ALIGNX_END;
	enum { ss = UACIN_AUDIO48_SAMPLEBYTES, nch = UACIN_FMT_CHANNELS_AUDIO48 };
} uacin48_t;

typedef dmahandle<FLOAT_t, uacin48_t, UACIN48_CAPACITY, 1> uacin48dma_t;


typedef adapters<FLOAT_t, (int) UACIN_AUDIO48_SAMPLEBYTES, (int) UACIN_FMT_CHANNELS_AUDIO48> uacin48adpt_t;

static uacin48adpt_t uacin48adpt(UACIN_AUDIO48_SAMPLEBYTES * 8, 0, "uacin48");

//static uacin48list_t uacin48(IRQL_REALTIME, "uacin48");
static uacin48dma_t uacin48(IRQL_REALTIME, "uacin48");

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned uacin48_putcbf(uint8_t * b, FLOAT_t ch0, FLOAT_t ch1)
{
	return uacin48adpt.poke(b, ch0, ch1);
}

int_fast32_t cachesize_dmabufferuacin48(void)
{
	return uacin48.get_cachesize();

}

void elfill_dmabufferuacin48(FLOAT_t ch0, FLOAT_t ch1)
{
	uacin48.savedata(ch0, ch1, uacin48_putcbf);
}

// can not be zero
uintptr_t allocate_dmabufferuacin48(void)
{
	uacin48_t * dest;
	while (uacin48.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	dest->tag = BUFFTAG_UACIN48;
	return (uintptr_t) dest->buff;
}

// can not be zero
uintptr_t getfilled_dmabufferuacin48(void)
{
	uacin48_t * dest;
	if (uacin48.get_readybuffer(& dest))
	{
		dest->tag = BUFFTAG_UACIN48;
//		unsigned i;
//		for (i = 0; i < DMABUFFSIZE16RX;)
//		{
//			i += uacin48adpt.poke(dest->buff + i, get_lout(), get_rout());
//		}
		return (uintptr_t) & dest->buff;
	}
	if (uacin48.get_freebuffer(& dest))
	{
		dest->tag = BUFFTAG_UACIN48;
		memset(dest->buff, 0, sizeof dest->buff);
//		unsigned i;
//		for (i = 0; i < DMABUFFSIZE16RX;)
//		{
//			i += uacin48adpt.poke(dest->buff + i, get_lout(), get_rout());
//		}
		return (uintptr_t) & dest->buff;
	}
	uacin48.debug();
	ASSERT(0);
	return 0;
}

void save_dmabufferuacin48(uintptr_t addr)
{
	uacin48_t * const p = CONTAINING_RECORD(addr, uacin48_t, buff);
	ASSERT(p->tag == BUFFTAG_UACIN48);
	uacin48.save_buffer(p);
}

void release_dmabufferuacin48(uintptr_t addr)
{
	uacin48_t * const p = CONTAINING_RECORD(addr, uacin48_t, buff);
	uacin48.release_buffer(p);
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
	return elfetch_dmabuffer16rx(v->ivqv);
}

//////////////////////////////////////////
// Поэлементное чтение буфера UAC OUT

// в паре значений, возвращаемых данной функцией, vi получает значение от микрофона. vq зарезервированно для работы ISB (две независимых боковых)
// При отсутствии данных в очереди - возвращаем 0
RAMFUNC uint_fast8_t getsampmleusb(FLOAT32P_t * v)
{
#if WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE)
	return elfetch_dmabufferuacout48(v->ivqv);
#else
	return 0;
#endif
}

// звук для самоконтроля
void savemonistereo(FLOAT_t ch0, FLOAT_t ch1)
{
	elfill_dmabuffer16txmoni(ch0, ch1);
}

#endif /* WITHINTEGRATEDDSP */

// message buffers
/* Cообщения от уровня обработчиков прерываний к user-level функциям. */

typedef struct
{
	messagetypes_t type;
	uint8_t buff [MSGBUFFERSIZE8];
	enum { ss = 1, nch = 1 };	// stub for resampling support
} message8buff_t;

// Данному интерфейсу не требуется побайтный доступ или ресэмплниг
typedef blists<message8buff_t, MESSAGE_CAPACITY, 0> message8list_t;

static message8list_t message8(MESSAGE_IRQL, "msg8");

// Освобождение обработанного буфера сообщения
void releasemsgbuffer(uint8_t * dest)
{
	message8buff_t * const p = CONTAINING_RECORD(dest, message8buff_t, buff);
	message8.release_buffer(p);
}
// Буфер для формирования сообщения
size_t takemsgbufferfree(uint8_t * * dest)
{
	message8buff_t * addr;
	if (message8.get_freebuffer(& addr))
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
	message8.save_buffer(p);
}

// Буферы с принятымти от обработчиков прерываний сообщениями
messagetypes_t takemsgready(uint8_t * * dest)
{
	message8buff_t * addr;
	if (message8.get_readybuffer(& addr))
	{
		* dest = addr->buff;
		return addr->type;
	}
	return MSGT_EMPTY;
}

#if WITHMODEM

typedef struct modems8
{
	LIST_ENTRY item;
	size_t length;
	uint8_t buff [MODEMBUFFERSIZE8];
} modems8_t;

// Данному интерфейсу не требуется побайтный доступ или ресэмплниг
typedef blists<modems8_t, MODEM8_CAPACITY> modems8list_t;

static modems8list_t modems8list(MODEM8_IRQL, "mdm8");

//static RAMBIGDTCM LIST_HEAD2 modemsfree8;		// Свободные буферы
//static RAMBIGDTCM LIST_HEAD2 modemsrx8;	// Буферы с принятымти через модем данными
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
//	unsigned startdata;
//	unsigned topdata;
#if WITHUSEAUDIOREC2CH
	enum { ss = 1, nch = 2 };	// stub for resampling support
#else /* WITHUSEAUDIOREC2CH */
	enum { ss = 1, nch = 1 };	// stub for resampling support
#endif /* WITHUSEAUDIOREC2CH */
} ALIGNX_END recordswav48_t;

// буферы: один заполняется, один воспроизводлится и два свободных (с одинм бывают пропуски).
typedef dmahandle<FLOAT_t, recordswav48_t, AUDIOREC_CAPACITY, 0> recordswav48dma_t;

static recordswav48dma_t recordswav48list(IRQL_REALTIME, "rec");

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned recordswav48_putcbf(int16_t * buff, FLOAT_t ch0, FLOAT_t ch1)
{
#if WITHUSEAUDIOREC2CH

	// Запись звука на SD CARD в стерео
	buff [0] = adpt_output(& sdcardio, ch0);
	buff [1] = adpt_output(& sdcardio, ch1);
	return 2;

#else /* WITHUSEAUDIOREC2CH */

	// Запись звука на SD CARD в моно
	buff [0] = adpt_output(& sdcardio, ch0);
	returt 1;

#endif /* WITHUSEAUDIOREC2CH */
}

void elfill_recordswav48(FLOAT_t ch0, FLOAT_t ch1)
{
	recordswav48list.savedata(ch0, ch1, recordswav48_putcbf);
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

#if 0

// поддержка воспроизведения файлов

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

	const FLOAT_t sample = adpt_input(& sdcardio, p->buff [n]);
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

#endif

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



// Выдача в USB UAC
void recordsampleUAC(FLOAT_t left, FLOAT_t right)
{
#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)
	// WITHUSBUACIN test
//	left = get_lout();
//	right = get_rout();

	if (uacinalt == UACINALT_AUDIO48)
	{
		elfill_dmabufferuacin48(left, right);
	}
#endif /* WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) */
}

// Запись на SD CARD
void recordsampleSD(FLOAT_t left, FLOAT_t right)
{
#if WITHUSEAUDIOREC && ! (WITHWAVPLAYER || WITHSENDWAV)

	elfill_recordswav48(left, right);

#endif /* WITHUSEAUDIOREC && ! (WITHWAVPLAYER || WITHSENDWAV) */
}



#if WITHUSBUAC && WITHUSBHW && defined (WITHUSBHW_DEVICE)

#if WITHRTS96

	// Поэлементное заполнение буфера RTS96

	// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
	// vl, vr: 32 bit, signed - преобразуем к требуемому формату для передачи по USB здесь.
	void savesampleout96stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
	{
#if WITHUSBUACIN2
	#if WITHRTS96
		if (uacinrtsalt == UACINRTSALT_RTS96)
		{
			elfill_dmabufferuacinrts96(ch0, ch1);
		}
	#endif /* WITHRTS96 */
	#if WITHRTS192
	#endif /* WITHRTS192 */
#else
	#if WITHRTS96
		if (uacinalt == UACINALT_RTS96)
		{
			elfill_dmabufferuacinrts96(ch0, ch1);
		}
	#endif /* WITHRTS96 */
	#if WITHRTS192
	#endif /* WITHRTS192 */
#endif /* WITHUSBUACIN2 */
	}

#endif /* WITHRTS96 */

#if WITHRTS192

	// Поэлементное заполнение буфера RTS192

	void savesampleout192stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
	{
#if WITHUSBUACIN2
	#if WITHRTS96
	#endif /* WITHRTS96 */
	#if WITHRTS192
		if (uacinrtsalt == UACINRTSALT_RTS192)
		{
			elfill_dmabufferuacinrts192(ch0, ch1);
		}
	#endif /* WITHRTS192 */
#else
	#if WITHRTS96
	#endif /* WITHRTS96 */
	#if WITHRTS192
		if (uacinalt == UACINALT_RTS192)
		{
			elfill_dmabufferuacinrts192(ch0, ch1);
		}
	#endif /* WITHRTS192 */
#endif /* WITHUSBUACIN2 */
	}

#endif /* WITHRTS192 */

#endif /* WITHUSBUAC */


#if WITHINTEGRATEDDSP

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

#if WITHINTEGRATEDDSP

#if WITHUSBUAC && defined (WITHUSBHW_DEVICE)

static uint_fast8_t isaudio48(void)
{
#if WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE)
	return UACINALT_AUDIO48 == uacinalt;
#else /* WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE) */
	return 0;
#endif /* WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE) */
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

#endif

#else

static uint_fast8_t isrts96(void)
{
	return 0;
}

#endif /*  WITHRTS96 && WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE) */


// --- Коммутация потоков аудиоданных


// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки IF ADC (спектроанализатор)
// Вызывается на ARM_REALTIME_PRIORITY уровне.
void RAMFUNC processing_dmabuffer32rx(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
//	++ n1;
//	// подсчёт скорости в сэмплах за секунду
//	debugcount_rx32adc += CNT32RX;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */

	dsp_extbuffer32rx((IFADCvalue_t *) addr);

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
	dsp_extbuffer32rts((IFADCvalue_t *) addr);
}


// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки IF ADC (MAIN RX/SUB RX)
// Вызывается на ARM_REALTIME_PRIORITY уровне.
void RAMFUNC processing_dmabuffer32wfm(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
//	++ n1wfm;
//	// подсчёт скорости в сэмплах за секунду
//	debugcount_rx32wfm += CNT32RX;	// в буфере пары сэмплов по четыре байта
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
	unsigned i;
	IFADCvalue_t * const rx32 = (IFADCvalue_t *) addr32rx;
	aubufv_t * const rx16 = (aubufv_t *) addr16rx;

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

	IFDACvalue_t * const tx32 = (IFDACvalue_t *) addr32tx;
	aubufv_t * const tx16 = (aubufv_t *) addr16tx;
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

	//save_dmabufferuacinrts192(p);
	buffers_savetonull192rts(p);
}
#endif /* WITHRTS192 */



/* при необходимости копируем сэмплы от кодекв */
uintptr_t processing_pipe32rx(uintptr_t addr)
{
#if WITHFPGAPIPE_CODEC1
	save_dmabuffer16rx(pipe_dmabuffer16rx(allocate_dmabuffer16rx(), addr));
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
	release_dmabuffer16txphones(addr16);	/* освоюождаем буфер как переданный */
#endif /* WITHFPGAPIPE_CODEC1 */
#if WITHFPGAPIPE_NCORX0
#endif /* WITHFPGAPIPE_NCORX0 */
#if WITHFPGAPIPE_NCORX1
#endif /* WITHFPGAPIPE_NCORX1 */
#if WITHFPGAPIPE_NCORTS
#endif /* WITHFPGAPIPE_NCORTS */
	return addr;
}

//////////////////////////////////////////
// Поэлементное заполнение буфера IF DAC

// 32 bit, signed
void savesampleout32stereo(int_fast32_t ch0, int_fast32_t ch1)
{
	elfill_dmabuffer32tx(ch0, ch1);
}

//////////////////////////////////////////
///


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
}

/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
uintptr_t getfilled_dmabufferuacinX(uint_fast16_t * sizep)
{
#if WITHBUFFERSDEBUG
//	++ n6;
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
uintptr_t getfilled_dmabufferuacinrtsX(uint_fast16_t * sizep)
{
#if WITHBUFFERSDEBUG
//	++ n6;
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
		PRINTF(PSTR("getfilled_dmabufferuacinrtsX: uacinrtsalt=%u\n"), uacinrtsalt);
		ASSERT(0);
		return 0;
	}
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


void buffers_diagnostics(void)
{
#if WITHBUFFERSDEBUG
#if 1
	//denoise16list.debug();
	voice16rx.debug();
	voice16tx.debug();
	voice16txmoni.debug();
	voice32tx.debug();
	voice32rx.debug();
#endif
#if 0
	// USB
	uacout48.debug();
#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)
#if WITHRTS192
	uacinrts192.debug();
#endif
#if WITHRTS96
	uacinrts96.debug();
#endif
#endif
	uacin48.debug();
#endif
	//message8.debug();

	PRINTF("\n");

#endif /* WITHBUFFERSDEBUG */
//	PRINTF("__get_CPUACTLR()=%016" PRIX64 "\n", __get_CPUACTLR());
//	PRINTF("__get_CPUACTLR()=%016" PRIX64 "\n", UINT64_C(1) << 44);
}


/* вызывается из обработчика таймерного прерывания */
static void buffers_spool(void * ctx)
{
#if WITHBUFFERSDEBUG
#if 1
	//denoise16list.spool10ms();
	voice16rx.spool10ms();
	voice16tx.spool10ms();
	voice16txmoni.spool10ms();
	voice32tx.spool10ms();
	voice32rx.spool10ms();
#endif
#if 1
	// USB
	uacout48.spool10ms();
#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)
#if WITHRTS192
	uacinrts192.spool10ms();
#endif
#if WITHRTS96
	uacinrts96.spool10ms();
#endif
#endif
	uacin48.spool10ms();
#endif
	//message8.spool10ms();

#endif /* WITHBUFFERSDEBUG */
}

// инициализация системы буферов
void buffers_initialize(void)
{
	static ticker_t buffticker;

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

//	ASSERT((DMABUFFSIZE_UACIN % HARDWARE_RTSDMABYTES) == 0);
//	ASSERT((DMABUFFSIZE192RTS % HARDWARE_RTSDMABYTES) == 0);
//	ASSERT((DMABUFFSIZE96RTS % HARDWARE_RTSDMABYTES) == 0);

	#if WITHRTS192 && WITHUSBUACIN && WITHUSBHW && defined (WITHUSBHW_DEVICE)

	subscribeint32(& rtstargetsint, & uacinrtssubscribe, NULL, savesampleout192stereo);

	#elif WITHRTS96 && WITHUSBUACIN && WITHUSBHW && defined (WITHUSBHW_DEVICE)

	subscribeint32(& rtstargetsint, & uacinrtssubscribe, NULL, savesampleout96stereo);

	#endif /* WITHRTS192 */



#endif /* WITHINTEGRATEDDSP */
}
