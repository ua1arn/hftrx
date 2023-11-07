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

//#undef RAMNC
//#define RAMNC

//#define WITHBUFFERSDEBUG WITHDEBUG
#define BUFOVERSIZE 1

// Одна из задач resampler - привести частоту кодека к требуемой для 48 кГц (lrckf=24576000, (clk=24571428)) = 0.99981396484375

#define VOICE16RX_CAPACITY (64 * BUFOVERSIZE)	// прием от кодекв
#define VOICE16TX_CAPACITY (64 * BUFOVERSIZE)	// должно быть достаточное количество буферов чтобы запомнить буфер с выхода speex
#define VOICE16TXMONI_CAPACITY (64 * BUFOVERSIZE)	// во столько же на сколько буфр от кодека больше чем буфер к кодеку (если наоборот - минимум)

#define VOICE16RX_RESAMPLING 1	// прием от кодека - требуется ли resampling
#define VOICE16TX_RESAMPLING 1	// передача в кодек - требуется ли resampling

#define UACINRTS192_CAPACITY ((48 / OUTSAMPLES_AUDIO48) * 128 * BUFOVERSIZE)
#define UACINRTS96_CAPACITY ((48 / OUTSAMPLES_AUDIO48) * 128 * BUFOVERSIZE)
#define UACOUT48_CAPACITY ((48 / OUTSAMPLES_AUDIO48) * 128 * BUFOVERSIZE)
#define UACIN48_CAPACITY ((48 / OUTSAMPLES_AUDIO48) * 128 * BUFOVERSIZE)	// должно быть достаточное количество буферов чтобы запомнить буфер с выхода speex

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

static uint_fast8_t		glob_swaprts;		// управление боковой выхода спектроанализатора


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

template <typename element_t>
struct buffitem
{
	LIST_ENTRY item;
	void * tag0;
	void * tag2;
	ALIGNX_BEGIN  element_t v ALIGNX_END;
	void * tag3;
};

template <typename buffitem_t, int hasresample>
class blists
{
	typedef typeof(buffitem_t::v) element_t;
public:
	IRQLSPINLOCK_t irqllocl;
#if WITHBUFFERSDEBUG
	int errallocate;
	int saveount;
#endif /* WITHBUFFERSDEBUG */
public:
	int readycount;
	int freecount;
	int rslevel;
	LIST_ENTRY freelist;
	LIST_ENTRY readylist;
	const char * name;
	element_t * workbuff;	// буфер над которым выполняется ресэмплинг
	unsigned wbstart;	// start position of work buffer - zero has not meaning
	unsigned wbskip;	// сколько блоков пропускаем без контроля расхожденеия скорости
	unsigned wbadded;
	unsigned wbdeleted;
	unsigned wbhatoffs [3];		// смещения в буфере для формирования нового блока
	unsigned wbhatsize [3];		// рамеры
	unsigned wbgatcnt;			// Текуший инлдекс параметров для заполнения выходного буфера

	fqmeter fqin, fqout;
	enum { LEVELSTEP = 8 };
	// параметры чувствтительности ресэмплера
	enum { MINMLEVEL = LEVELSTEP * 1, NORMLEVEL = LEVELSTEP * 2, MAXLEVEL = LEVELSTEP * 3 };
	bool outready;

public:
	blists(IRQL_t airql, const char * aname, buffitem_t * storage, unsigned capacity) :
#if WITHBUFFERSDEBUG
		errallocate(0),
		saveount(0),
#endif /* WITHBUFFERSDEBUG */
		readycount(0),
		freecount(capacity),
		rslevel(0),
		name(aname),
		wbstart(0),	// начало данных в workbuff->buff
		wbskip(0),	// сколько блоков пропускаем без контроля расхожденеия скорости
		wbadded(0),
		wbdeleted(0),
		wbgatcnt(ARRAY_SIZE(wbhatsize)),	// состояние - нет ничего для формирования нового блокв
		outready(false)	// накоплено нужное количество буферов для старта
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
			VERIFY3(get_freebuffer(& workbuff), __FILE__, __LINE__, name);
			wbstart = 7 * element_t::ss * element_t::nch;
			//PRINTF("%s: test resampler: wbstart=%u\n", name, wbstart);
		}
	}

	/* готовность буферов с "гистерезисом". */
	void fiforeadyupdate()
	{
		outready = outready ? readycount != 0 : readycount >= NORMLEVEL;
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
		++ readycount;
		fiforeadyupdate();
#if WITHBUFFERSDEBUG
		fqin.pass(sizeof addr->buff / (addr->ss * addr->nch));
		++ saveount;
		rslevel += sizeof addr->buff / (addr->ss * addr->nch);
#endif /* WITHBUFFERSDEBUG */

		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
	}

	// получить из списка готовых
	bool get_readybuffer_raw(element_t * * dest)
	{
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql);
		if (! IsListEmpty(& readylist))
		{
			const PLIST_ENTRY t = RemoveTailList(& readylist);
			-- readycount;
			fiforeadyupdate();
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
			return true;
		}
		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
		return false;
	}
	// получить из списка свободных
	bool get_freebuffer_raw(element_t * * dest)
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
			return true;
		}
#if WITHBUFFERSDEBUG
		++ errallocate;
#endif /* WITHBUFFERSDEBUG */
		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
		return false;
	}

	bool get_readybuffer(element_t * * dest)
	{
		if (hasresample)
		{
			if (! outready)
				return false;
			if (wbskip != 0)
			{
				-- wbskip;
				return get_readybufferarj(dest, false, false);
			}
			else if (readycount < MINMLEVEL)
			{
				++ wbadded;
				wbskip = 2000;
				return get_readybufferarj(dest, true, false);
			}
			else if (readycount >= MAXLEVEL)
			{
				++ wbdeleted;
				wbskip = 2000;
				return get_readybufferarj(dest, false, true);
			}
			else
			{
				return get_readybufferarj(dest, false, false);
			}
		}
		else
		{
			return get_readybuffer_raw(dest);
		}
	}

	// получить из списка свободных, если нет - ошибка
	bool get_freebuffer(element_t * * dest)
	{
		return get_freebuffer_raw(dest);
	}

	// получить из списка свободных, если нет - из готовых
	bool get_freebufferforced_nopurge(element_t * * dest)
	{
		return get_freebuffer_raw(dest) || get_readybuffer_raw(dest);
	}

	// получить из списка свободных, если нет - из готовых
	bool get_freebufferforced(element_t * * dest)
	{
		if (get_freebuffer_raw(dest))
			return true;
		for (unsigned i = 3; i -- && get_readybuffer_raw(dest);)
		{
			release_buffer(* dest);
		}
		return get_freebuffer_raw(dest);
	}

	// получить из списка готовых, если нет - из свободных
	bool get_readybufferforced(element_t * * dest)
	{
		if (get_readybuffer_raw(dest))
			return true;
		for (unsigned i = 3; i -- && get_freebuffer(dest);)
		{
			save_buffer(* dest);
		}
		return get_readybuffer_raw(dest);
	}

	// получить из списка свободных, если нет - из готовых
	bool get_readybufferforced_nopurge(element_t * * dest)
	{
		return get_readybuffer_raw(dest) ||  get_freebuffer_raw(dest);
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
		//PRINTF("%s:s=%d,a=%d,o=%d,f=%d ", name, saveount, errallocate, readycount, freecount);
		PRINTF(" %s:e=%d,y=%d,f=%d,%uk/%uk,v=%d", name, errallocate, readycount, freecount, (fin + 500) / 1000, (fout + 500) / 1000, rslevel);
		if (hasresample)
		{
			PRINTF("+%u,-%u", wbadded, wbdeleted);
		}
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
	int get_readybufferarj(element_t * * dest, bool add, bool del)
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
				while (! get_readybufferforced(& workbuff))	// следующий готовый
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

template <typename sample_t, typename element_t, int hasresample>
class dmahandle: public blists<element_t, hasresample>
{
	typedef blists<element_t, hasresample> parent_t;
	typedef typeof (element_t::v) wel_t;
	wel_t * wb;
	unsigned wbn;
	wel_t * rb;
	unsigned rbn;


public:
	dmahandle(IRQL_t airql, const char * aname, element_t * storage, unsigned capacity) :
		parent_t(airql, aname, storage, capacity),
		wb(NULL),
		rb(NULL)
	{

	}

	// поэлементное заполнение буферов
	void savedata(sample_t ch0, sample_t ch1, unsigned (* putcbf)(typeof (element_t::v.buff [0]) * b, sample_t ch0, sample_t ch1))
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

	uint_fast8_t fetchdata(sample_t * dest, unsigned (* getcbf)(typeof (element_t::v.buff [0]) * b, sample_t * dest))
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
enum { CNT16MONI = DMABUFFSIZE16MONI / DMABUFFSTEP16MONI };
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

typedef buffitem<denoise16_t> denoise16buf_t;
typedef dmahandle<FLOAT_t, denoise16buf_t, 0> denoise16dma_t;

// буферы: один заполняется, один воспроизводлится и два свободных (с одинм бывают пропуски).
static denoise16buf_t denoise16buf [SPEEX_CAPACITY];

static denoise16dma_t denoise16list(IRQL_REALTIME, "denoise16", denoise16buf, ARRAY_SIZE(denoise16buf));

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

// DMA data from codec
typedef ALIGNX_BEGIN struct voice16rx_tag
{
	ALIGNX_BEGIN aubufv_t buff [DMABUFFSIZE16RX] ALIGNX_END;
	ALIGNX_BEGIN uint8_t pad ALIGNX_END;
	enum { ss = sizeof (aubufv_t), nch = DMABUFFSTEP16RX };	// resampling support
} ALIGNX_END voice16rx_t;

typedef buffitem<voice16rx_t> voice16rxbuf_t;

// DMA data to codec
typedef ALIGNX_BEGIN struct voice16tx_tag
{
	ALIGNX_BEGIN aubufv_t buff [DMABUFFSIZE16TX] ALIGNX_END;
	ALIGNX_BEGIN uint8_t pad ALIGNX_END;
	enum { ss = sizeof (aubufv_t), nch = DMABUFFSTEP16TX };	// resampling support
} ALIGNX_END voice16tx_t;

typedef buffitem<voice16tx_t> voice16txbuf_t;

//typedef adapters<FLOAT_t, (int) UACOUT_AUDIO48_SAMPLEBYTES, (int) UACOUT_FMT_CHANNELS_AUDIO48> voice16txadpt_t;

typedef dmahandle<FLOAT_t, voice16rxbuf_t, VOICE16RX_RESAMPLING> voice16rxdma_t;
typedef dmahandle<FLOAT_t, voice16txbuf_t, VOICE16TX_RESAMPLING> voice16txdma_t;

static RAMNC voice16rxbuf_t voice16rxbuf [VOICE16RX_CAPACITY];
static voice16txbuf_t voice16txbuf [VOICE16TX_CAPACITY];


static voice16rxdma_t codec16rx(IRQL_REALTIME, "16rx", voice16rxbuf, ARRAY_SIZE(voice16rxbuf));		// from codec
static voice16txdma_t codec16tx(IRQL_REALTIME, "16tx", voice16txbuf, ARRAY_SIZE(voice16txbuf));		// to codec

int_fast32_t cachesize_dmabuffer16rx(void)
{
	return codec16rx.get_cachesize();
}

int_fast32_t cachesize_dmabuffer16tx(void)
{
	return codec16tx.get_cachesize();
}

// can not be zero
uintptr_t allocate_dmabuffer16rx(void)
{
	voice16rx_t * dest;
	while (! codec16rx.get_freebufferforced(& dest))
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

//// may be zero
//uintptr_t getfilled_dmabuffer16rx(void)
//{
//	voice16rx_t * dest;
//	if (! codec16rx.get_readybuffer(& dest))
//		return 0;
//	return (uintptr_t) dest->buff;
//}

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned getcbf_dmabuffer16rx(aubufv_t * b, FLOAT_t * dest)
{
	enum { L, R };
	dest [L] = adpt_input(& afcodecrx, b [DMABUFF16RX_MIKE]);
	dest [R] = adpt_input(& afcodecrx, b [DMABUFF16RX_MIKE]);
	return DMABUFFSTEP16RX;
}

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned putcbf_dmabuffer16tx(aubufv_t * b, FLOAT_t ch0, FLOAT_t ch1)
{
	b [DMABUFF16TX_LEFT] = adpt_output(& afcodectx, ch0);
	b [DMABUFF16TX_RIGHT] = adpt_output(& afcodectx, ch1);
	return DMABUFFSTEP16TX;
}

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned putcbf_dmabuffer16rx(aubufv_t * b, FLOAT_t ch0, FLOAT_t ch1)
{
	b [DMABUFF16RX_LEFT] = adpt_output(& afcodecrx, ch0);
	b [DMABUFF16RX_RIGHT] = adpt_output(& afcodecrx, ch1);
	return DMABUFFSTEP16RX;
}

// Возвращает не-ноль если данные есть
uint_fast8_t elfetch_dmabuffer16rx(FLOAT_t * dest)
{
	return codec16rx.fetchdata(dest, getcbf_dmabuffer16rx);
}

void save_dmabuffer16rx(uintptr_t addr)
{
	voice16rx_t * const p = CONTAINING_RECORD(addr, voice16rx_t, buff);
	codec16rx.save_buffer(p);
}

void release_dmabuffer16rx(uintptr_t addr)
{
	voice16rx_t * const p = CONTAINING_RECORD(addr, voice16rx_t, buff);
	codec16rx.release_buffer(p);
}

void elfill_dmabuffer16tx(FLOAT_t ch0, FLOAT_t ch1)
{
	codec16tx.savedata(ch0, ch1, putcbf_dmabuffer16tx);
}

void elfill_dmabuffer16rx(FLOAT_t ch0, FLOAT_t ch1)
{
	codec16rx.savedata(ch0, ch1, putcbf_dmabuffer16rx);
}

// Поэлементное заполнение DMA буфера AF DAC
static void savesampleout16stereo_float(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	voice_put(VOICE_REC16, ch0, ch1);	// аудиоданные - выход приемника
}

// Used at initialization DMA
// can not be zero
uintptr_t allocate_dmabuffer16tx(void)
{
	voice16tx_t * dest;
	while (! codec16tx.get_freebufferforced(& dest))
		ASSERT(0);
	memset(dest->buff, 0, sizeof dest->buff);
	return (uintptr_t) dest->buff;
}

//void save_dmabuffer16tx(uintptr_t addr)
//{
//	voice16tx_t * const p = CONTAINING_RECORD(addr, voice16tx_t, buff);
//	codec16tx.save_buffer(p);
//}

void release_dmabuffer16tx(uintptr_t addr)
{
	voice16tx_t * const p = CONTAINING_RECORD(addr, voice16tx_t, buff);
	codec16tx.release_buffer(p);
}

static void dsp_loopback(unsigned nsamples)
{
	enum { L = 0, R = 1 };

	// Требуется обработать указанное количество сэмплов
	while (nsamples --)
	{
		FLOAT_t v [2];

#if WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE)
		// Канал от USB в наушники
		if (elfetch_dmabufferuacout48(v))
			elfill_dmabuffer16tx(v [L], v [R]);
#endif

#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)
		// Канал от микрофона в USB
		if (elfetch_dmabuffer16rx(v))
			elfill_dmabufferuacin48(v [L], v [R]);
#endif

	}
}

// can not be zero
uintptr_t getfilled_dmabuffer16tx(void)
{
#if WITHUSBHEADSET
	dsp_loopback(CNT16TX);
#else /* WITHUSBHEADSET */
	dsp_fillphones(CNT16TX);
#endif

	voice16tx_t * dest;
	while (! codec16tx.get_readybuffer(& dest) && ! codec16tx.get_freebufferforced(& dest))
		ASSERT(0);

#if 0
	// тестирование вывода на кодек
	for (unsigned i = 0; i < ARRAY_SIZE(phones->buff); i += DMABUFFSTEP16TX)
	{
		dest->buff [i + DMABUFF16TX_LEFT] = adpt_output(& afcodectx, get_lout());
		dest->buff [i + DMABUFF16TX_RIGHT] = adpt_output(& afcodectx, get_rout());
	}
	//printhex32(0, phones->buff, sizeof phones->buff);
#endif

	return (uintptr_t) dest->buff;
}

////////////////////////////////////////////////////
///

// Sidetone
typedef ALIGNX_BEGIN struct moni16_tag
{
	ALIGNX_BEGIN FLOAT_t buff [DMABUFFSIZE16MONI] ALIGNX_END;
	enum { ss = sizeof (FLOAT_t), nch = DMABUFFSTEP16MONI };	// stub for resampling support
} ALIGNX_END moni16_t;

typedef buffitem<moni16_t> moni16buf_t;

// sidetone forming
// Возвращает количество элементов буфера, обработанных за вызов
static unsigned putcbf_dmabuffer16moni(FLOAT_t * b, FLOAT_t ch0, FLOAT_t ch1)
{
	ASSERT(DMABUFFSTEP16MONI == 2);
	b [0] = ch0;
	b [1] = ch1;
	return DMABUFFSTEP16MONI;
}

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned getcbf_dmabuffer16moni(FLOAT_t * b, FLOAT_t * dest)
{
	dest [0] = b [0];
	dest [1] = b [1];
	return DMABUFFSTEP16MONI;
}



typedef dmahandle<FLOAT_t, moni16buf_t, 0> moni16txdma_t;

static moni16buf_t moni16buf [VOICE16TXMONI_CAPACITY];
static moni16buf_t rx16recbuf [VOICE16TXMONI_CAPACITY];

static moni16txdma_t moni16(IRQL_REALTIME, "16moni", moni16buf, ARRAY_SIZE(moni16buf));		// аудиоданные - самоконтроль (ключ, голос).
static moni16txdma_t rx16rec(IRQL_REALTIME, "rx16rec", rx16recbuf, ARRAY_SIZE(rx16recbuf));	// аудиоданные - выход приемника

///////////////////////////////////
///

//// Возвращает не-ноль если данные есть
//uint_fast8_t elfetch_dmabuffer16moni(FLOAT_t * dest)
//{
//	return moni16.fetchdata(dest, getcbf_dmabuffer16moni);
//}
//
//// sidetone forming
//void elfill_dmabuffer16moni(FLOAT_t ch0, FLOAT_t ch1)
//{
//	moni16.savedata(ch0, ch1, putcbf_dmabuffer16moni);
//}
//
//
//// Возвращает не-ноль если данные есть
//uint_fast8_t elfetch_dmabufferrx16rec(FLOAT_t * dest)
//{
//	return rx16rec.fetchdata(dest, getcbf_dmabuffer16moni);
//}
//
//// выход приемника после фильтра
//void elfill_dmabufferrx16rec(FLOAT_t ch0, FLOAT_t ch1)
//{
//	rx16rec.savedata(ch0, ch1, putcbf_dmabuffer16moni);
//}

///////////////////////////////////
///

// I/Q data to FPGA or IF CODEC
typedef ALIGNX_BEGIN struct voices32tx_tag
{
	ALIGNX_BEGIN IFDACvalue_t buff [DMABUFFSIZE32TX] ALIGNX_END;
	ALIGNX_BEGIN uint8_t pad ALIGNX_END;
	enum { ss = sizeof (IFDACvalue_t), nch = DMABUFFSTEP32TX };	// resampling support
} ALIGNX_END voice32tx_t;

typedef buffitem<voice32tx_t> voice32txbuf_t;

static voice32txbuf_t voice32txbuf [VOICE32TX_CAPACITY];

typedef dmahandle<FLOAT_t, voice32txbuf_t, 0> voice32txdma_t;

//static voice32txlist_t voice32tx(IRQL_REALTIME, "32tx");
static voice32txdma_t voice32tx(IRQL_REALTIME, "32tx", voice32txbuf, ARRAY_SIZE(voice32txbuf));

int_fast32_t cachesize_dmabuffer32tx(void)
{
	return voice32tx.get_cachesize();
}

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned putcbf_dmabuffer32tx(IFDACvalue_t * buff, FLOAT_t ch0, FLOAT_t ch1)
{

#if WITHTXCPATHCALIBRATE
	buff [DMABUF32TXI] = adpt_outputexact(& ifcodectx, ch0);
	buff [DMABUF32TXQ] = adpt_outputexact(& ifcodectx, ch1);
#else /* WITHTXCPATHCALIBRATE */
	buff [DMABUF32TXI] = adpt_output(& ifcodectx, ch0);
	buff [DMABUF32TXQ] = adpt_output(& ifcodectx, ch1);
#endif /* WITHTXCPATHCALIBRATE */


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

void elfill_dmabuffer32tx(FLOAT_t ch0, FLOAT_t ch1)
{
	voice32tx.savedata(ch0, ch1, putcbf_dmabuffer32tx);
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
	while (! voice32tx.get_freebufferforced(& dest))
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
	while (! voice32tx.get_readybuffer(& dest) && ! voice32tx.get_freebufferforced(& dest))
		ASSERT(0);
	return (uintptr_t) dest->buff;
}


#if WITHFPGAPIPE_CODEC1

// копирование полей в передаваемый на FPGA буфер
static uintptr_t RAMFUNC
pipe_dmabuffer32tx(uintptr_t addr32tx, uintptr_t addr16tx)
{
	// Предполагается что типы данных позволяют транзитом передавать сэмплы, не беспокоясь о преобразовании форматов

	IFDACvalue_t * const tx32 = (IFDACvalue_t *) addr32tx;
	aubufv_t * const tx16 = (aubufv_t *) addr16tx;
	unsigned i;
	const FLOAT_t scale = 1.0 / 32;	// for test

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

// can not be be zero
uintptr_t getfilled_dmabuffer32tx_main(void)
{
	uintptr_t addr = getfilled_dmabuffer32tx();
#if WITHFPGAPIPE_CODEC1
	/* при необходимости добавляем слоты для передачи на кодек */
	const uintptr_t addr16 = getfilled_dmabuffer16tx();
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

typedef buffitem<voice32rx_t> voice32rxbuf_t;

static RAMNC voice32rxbuf_t voice32rxbuf [VOICE32RX_CAPACITY];

typedef dmahandle<int_fast32_t, voice32rxbuf_t, 0> voice32rxdma_t;

static voice32rxdma_t voice32rx(IRQL_REALTIME, "32rx", voice32rxbuf, ARRAY_SIZE(voice32rxbuf));

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
	while (! voice32rx.get_freebuffer(& dest))
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
	ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
	enum { ss = UACOUT_AUDIO48_SAMPLEBYTES, nch = UACOUT_FMT_CHANNELS_AUDIO48 };	// resampling support
} uacout48_t;

typedef buffitem<uacout48_t> uacout48buf_t;

static RAMNC uacout48buf_t uacout48buf [UACOUT48_CAPACITY];

typedef dmahandle<FLOAT_t, uacout48buf_t, 1> uacout48dma_t;

typedef adapters<FLOAT_t, (int) UACOUT_AUDIO48_SAMPLEBYTES, (int) UACOUT_FMT_CHANNELS_AUDIO48> uacout48adpt_t;

static uacout48dma_t uacout48(IRQL_REALTIME, "uaco48", uacout48buf, ARRAY_SIZE(uacout48buf));

static uacout48adpt_t uacout48adpt(UACOUT_AUDIO48_SAMPLEBYTES * 8, 0, "uaco48");

int_fast32_t cachesize_dmabufferuacout48(void)
{
	return uacout48.get_cachesize();
}

// can not be zero
uintptr_t allocate_dmabufferuacout48(void)
{
	uacout48_t * dest;
	while (! uacout48.get_freebufferforced(& dest))
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

//// may be zero
//uintptr_t getfilled_dmabufferuacout48(void)
//{
//	uacout48_t * dest;
//	if (! uacout48.get_readybuffer(& dest))
//		return 0;
//	return (uintptr_t) dest->buff;
//}

void release_dmabufferuacout48(uintptr_t addr)
{
	uacout48_t * const p = CONTAINING_RECORD(addr, uacout48_t, buff);
	uacout48.release_buffer(p);
}

void save_dmabufferuacout48(uintptr_t addr)
{
	uacout48_t * const p = CONTAINING_RECORD(addr, uacout48_t, buff);
	if (uacoutalt != UACOUTALT_AUDIO48)
	{
		uacout48.release_buffer(p);
		return;
	}
	uacout48.save_buffer(p);
}

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned getcbf_dmabufferuacout48(uint8_t * b, FLOAT_t * dest)
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
	return uacout48.fetchdata(dest, getcbf_dmabufferuacout48);
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
		ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
		enum { ss = UACIN_RTS192_SAMPLEBYTES, nch = UACIN_FMT_CHANNELS_RTS192 };	// resampling support
	} uacinrts192_t;

	typedef buffitem<uacinrts192_t> uacinrts192buf_t;
	static uacinrts192buf_t uacinrts192buf [UACINRTS192_CAPACITY];

	typedef dmahandle<int_fast32_t, uacinrts192buf_t, 1> uacinrts192dma_t;

	typedef adapters<int_fast32_t, (int) UACIN_RTS192_SAMPLEBYTES, (int) UACIN_FMT_CHANNELS_RTS192> uacinrts192adpt_t;

	static uacinrts192dma_t uacinrts192(IRQL_REALTIME, "uacin192", uacinrts192buf, ARRAY_SIZE(uacinrts192buf));
	static uacinrts192adpt_t uacinrts192adpt(UACIN_RTS192_SAMPLEBYTES * 8, 0, "uacin192");

	int_fast32_t cachesize_dmabufferuacinrts192(void)
	{
		return uacinrts192.get_cachesize();
	}

	// Возвращает количество элементов буфера, обработанных за вызов
	static unsigned putcbf_dmabufferuacinrts192(uint8_t * b, int_fast32_t ch0, int_fast32_t ch1)
	{
		return uacinrts192adpt.poketransf(& if2rts192out, b, ch0, ch1);
	}

	void elfill_dmabufferuacinrts192(int_fast32_t ch0, int_fast32_t ch1)
	{
		uacinrts192.savedata(ch0, ch1, putcbf_dmabufferuacinrts192);
	}

	// can not be zero
	uintptr_t allocate_dmabufferuacinrts192(void)
	{
		uacinrts192_t * dest;
		while (! uacinrts192.get_freebufferforced(& dest))
			ASSERT(0);
		dest->tag = BUFFTAG_RTS192;
		return (uintptr_t) & dest->buff;
	}

	// can not be zero
	uintptr_t getfilled_dmabufferuacinrts192(void)
	{
		uacinrts192_t * dest;
		while (! uacinrts192.get_readybuffer(& dest) && ! uacinrts192.get_freebufferforced(& dest))
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
		ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
		enum { ss = UACIN_RTS96_SAMPLEBYTES, nch = UACIN_FMT_CHANNELS_RTS96 };	// resampling support
	} uacinrts96_t;

	typedef buffitem<uacinrts96_t> uacinrts96buf_t;
	static uacinrts96buf_t uacinrts96buf [UACINRTS96_CAPACITY];

	typedef dmahandle<int_fast32_t, uacinrts96buf_t, 1> uacinrts96dma_t;

	typedef adapters<int_fast32_t, (int) UACIN_RTS96_SAMPLEBYTES, (int) UACIN_FMT_CHANNELS_RTS96> uacinrts96adpt_t;

	static uacinrts96dma_t uacinrts96(IRQL_REALTIME, "uacin96", uacinrts96buf, ARRAY_SIZE(uacinrts96buf));
	static uacinrts96adpt_t uacinrts96adpt(UACIN_RTS96_SAMPLEBYTES * 8, 0, "uacin96");

	int_fast32_t cachesize_dmabufferuacinrts96(void)
	{
		return uacinrts96.get_cachesize();
	}

	// Возвращает количество элементов буфера, обработанных за вызов
	static unsigned putcbf_dmabufferuacinrts96(uint8_t * b, int_fast32_t ch0, int_fast32_t ch1)
	{
		return uacinrts96adpt.poketransf(& if2rts96out, b, ch0, ch1);
	}

	void elfill_dmabufferuacinrts96(int_fast32_t ch0, int_fast32_t ch1)
	{
		uacinrts96.savedata(ch0, ch1, putcbf_dmabufferuacinrts96);
	}

	// can not be zero
	uintptr_t allocate_dmabufferuacinrts96(void)
	{
		uacinrts96_t * dest;
		while (! uacinrts96.get_freebufferforced(& dest))
			ASSERT(0);
		dest->tag = BUFFTAG_RTS96;
		return (uintptr_t) & dest->buff;
	}

	// can not be zero
	uintptr_t getfilled_dmabufferuacinrts96(void)
	{
		uacinrts96_t * dest;
		while (! uacinrts96.get_readybuffer(& dest) && ! uacinrts96.get_freebufferforced(& dest))
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
	ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
	enum { ss = UACIN_AUDIO48_SAMPLEBYTES, nch = UACIN_FMT_CHANNELS_AUDIO48 };
} uacin48_t;

typedef buffitem<uacin48_t> uacin48buf_t;

static uacin48buf_t uacin48buf [UACIN48_CAPACITY];

typedef dmahandle<FLOAT_t, uacin48buf_t, 1> uacin48dma_t;


typedef adapters<FLOAT_t, (int) UACIN_AUDIO48_SAMPLEBYTES, (int) UACIN_FMT_CHANNELS_AUDIO48> uacin48adpt_t;

static uacin48adpt_t uacin48adpt(UACIN_AUDIO48_SAMPLEBYTES * 8, 0, "uacin48");

static uacin48dma_t uacin48(IRQL_REALTIME, "uacin48", uacin48buf, ARRAY_SIZE(uacin48buf));

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
	while (! uacin48.get_freebufferforced(& dest))
		ASSERT(0);
	dest->tag = BUFFTAG_UACIN48;
	return (uintptr_t) dest->buff;
}

// can not be zero
uintptr_t getfilled_dmabufferuacin48(void)
{
	uacin48_t * dest;
	if (uacin48.get_readybuffer(& dest) || uacin48.get_freebuffer(& dest) || uacin48.get_freebufferforced(& dest))
	{
		dest->tag = BUFFTAG_UACIN48;
		return (uintptr_t) & dest->buff;
	}
	uacin48.debug();
	PRINTF("uacin48.readycount=%u\n", uacin48.readycount);
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
#endif /* WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) */

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
#endif
	return 0;
}

// звук для самоконтроля
void savemonistereo(FLOAT_t ch0, FLOAT_t ch1)
{
	voice_put(VOICE_MONI16, ch0, ch1);
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

typedef buffitem<message8buff_t> message8v_t;
static message8v_t message8v [MESSAGE_CAPACITY];

// Данному интерфейсу не требуется побайтный доступ или ресэмплниг
typedef blists<message8v_t, 0> message8list_t;

static message8list_t message8(MESSAGE_IRQL, "msg8", message8v, ARRAY_SIZE(message8v));

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

typedef buffitem<recordswav48_t> recordswav48buf_t;

static recordswav48buf_t recordswav48buf [AUDIOREC_CAPACITY];

// буферы: один заполняется, один воспроизводлится и два свободных (с одинм бывают пропуски).
typedef dmahandle<FLOAT_t, recordswav48buf_t, 0> recordswav48dma_t;

static recordswav48dma_t recordswav48dma(IRQL_REALTIME, "rec", recordswav48buf, ARRAY_SIZE(recordswav48buf));

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
	return 1;

#endif /* WITHUSEAUDIOREC2CH */
}

void elfill_recordswav48(FLOAT_t ch0, FLOAT_t ch1)
{
	recordswav48dma.savedata(ch0, ch1, recordswav48_putcbf);
}

// user-mode function
unsigned takerecordbuffer(void * * dest)
{
	recordswav48_t * p;
	if (recordswav48dma.get_readybuffer(& p))
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
	if (recordswav48dma.get_freebuffer(& p))
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

	recordswav48dma.save_buffer(p);
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
		if (recordswav48dma.get_readybuffer(& p))
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
		recordswav48dma.release_buffer(p);
		p = NULL;
		//PRINTF("Release record buffer\n");
	}
	return 1;	// Сэмпл считан
}

#endif

void releaserecordbuffer(void * dest)
{
	recordswav48_t * const p = CONTAINING_RECORD(dest, recordswav48_t, buff);
	recordswav48dma.release_buffer(p);
}

void saverecordbuffer(void * dest)
{
	recordswav48_t * const p = CONTAINING_RECORD(dest, recordswav48_t, buff);
	recordswav48dma.save_buffer(p);
}

#endif /* WITHUSEAUDIOREC */

// Запись на SD CARD
void recordsampleSD(FLOAT_t left, FLOAT_t right)
{
#if WITHUSEAUDIOREC && ! (WITHWAVPLAYER || WITHSENDWAV)

	elfill_recordswav48(left, right);

#endif /* WITHUSEAUDIOREC && ! (WITHWAVPLAYER || WITHSENDWAV) */
}

//////////////////////////////////
///
///

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


void voice_put(VOICE_t * p, FLOAT_t ch0, FLOAT_t ch1)
{
	moni16txdma_t * const obj = (moni16txdma_t *) p;
	obj->savedata(ch0, ch1, putcbf_dmabuffer16moni);
}

uint_fast8_t voice_get(VOICE_t * p, FLOAT32P_t * v)
{
	moni16txdma_t * const obj = (moni16txdma_t *) p;
	return obj->fetchdata(v->ivqv, getcbf_dmabuffer16moni);
}

VOICE_t * voice_moni16(void)
{
	return (VOICE_t *) & moni16;
}

// аудиоданные - выход приемника
VOICE_t * voice_rec16(void)
{
	return (VOICE_t *) & rx16rec;
}
//
//VOICE_t * voice_uacin48(void)
//{
//	return (VOICE_t *) & uacin48;
//}
//
//VOICE_t * voice_uacout48(void)
//{
//	return (VOICE_t *) & uacout48;
//}
//
//VOICE_t * voice_16txphones(void)
//{
//	return (VOICE_t *) & codec16tx;
//}
//
//VOICE_t * voice_swav48(void)
//{
//	return (VOICE_t *) & recordswav48dma;
//}

#endif /* WITHINTEGRATEDDSP */

#if WITHINTEGRATEDDSP

#if WITHUSBUAC && defined (WITHUSBHW_DEVICE)

//static uint_fast8_t isaudio48(void)
//{
//#if WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE)
//	return UACINALT_AUDIO48 == uacinalt;
//#else /* WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE) */
//	return 0;
//#endif /* WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE) */
//}

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

//static uint_fast8_t isrts96(void)
//{
//#if WITHUSBHW && WITHUSBUAC
//	#if WITHUSBUACIN2 && WITHRTS96
//		return uacinrtsalt == UACINRTSALT_RTS96;
//	#elif WITHRTS96
//		return uacinalt == UACINALT_RTS96;
//	#else /* WITHUSBUACIN2 */
//		return 0;
//	#endif /* WITHUSBUACIN2 */
//#else /* WITHUSBHW && WITHUSBUAC */
//	return 0;
//#endif /* WITHUSBHW && WITHUSBUAC */
//}

#endif

#else

//static uint_fast8_t isrts96(void)
//{
//	return 0;
//}

#endif /*  WITHRTS96 && WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE) */

#if WITHRTS192
// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки - канал спектроанализатора
void RAMFUNC save_dmabuffer32rts192(uintptr_t addr)
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

	release_savetonull192rts(addr);
}
#endif /* WITHRTS192 */

#if WITHDSPEXTDDC && WITHRTS96
// использование данных о спектре, передаваемых в общем фрейме
static void RAMFUNC
saverts96pair(const IFADCvalue_t * buff)
{
#if FPGAMODE_GW2A
#else /* FPGAMODE_GW2A */
	// формирование отображения спектра
	// если используется конвертор на Rafael Micro R820T - требуется инверсия спектра
	if (glob_swaprts != 0)
	{
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RTS0Q],	// previous
			buff [DMABUF32RTS0I]
			);
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RTS1Q],	// current
			buff [DMABUF32RTS1I]
			);
	}
	else
	{
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RTS0I],	// previous
			buff [DMABUF32RTS0Q]
			);
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RTS1I],	// current
			buff [DMABUF32RTS1Q]
			);
	}
#endif
}
// использование данных о спектре, передаваемых в общем фрейме
static void RAMFUNC
saverts96(const IFADCvalue_t * buff)
{
	// формирование отображения спектра
	// если используется конвертор на Rafael Micro R820T - требуется инверсия спектра
	if (glob_swaprts != 0)
	{
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RTS0Q],	// previous
			buff [DMABUF32RTS0I]
			);
	}
	else
	{
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RTS0I],	// previous
			buff [DMABUF32RTS0Q]
			);
	}
}

#endif /* WITHDSPEXTDDC && WITHRTS96 */

#if 0

// Проверка качества линии передачи от FPGA
static int32_t seqNext [DMABUFFSTEP32RX];
static uint_fast8_t  seqValid [DMABUFFSTEP32RX];
static int seqErrors;
static int seqTotal;
static int seqRun;
static int seqDone;

enum { MAXSEQHIST = DMABUFCLUSTER + 5 };

static int32_t seqHist [MAXSEQHIST] [DMABUFFSTEP32RX];
static const void * seqHistP [MAXSEQHIST];
static unsigned seqPos;
static unsigned seqAfterError;

static void printSeqError(void)
{
	PRINTF("seqErrors=%d, seqTotal=%d, seqRun=%d\n", seqErrors, seqTotal, seqRun);
	unsigned i;
	for (i = 0; i < MAXSEQHIST; ++ i)
	{
		unsigned ix = ((MAXSEQHIST - 1) - i + seqPos) % MAXSEQHIST;
		PRINTF("hist %02d @%p :", i, seqHistP [ix]);
		unsigned col;
		for (col = 0; col < DMABUFFSTEP32RX; ++ col)
			PRINTF("%08x ", (unsigned) seqHist [ix] [col]);
		PRINTF("\n");
	}
	for (;;)
		;
}

static void validateSeq(uint_fast8_t slot, int32_t v, const int32_t * base)
{
	seqPos = (seqPos == 0) ? MAXSEQHIST - 1 : seqPos - 1;
	//memcpy(seqHist [seqPos], base, sizeof seqHist [seqPos]);
	unsigned col;
	for (col = 0; col < DMABUFFSTEP32RX; ++ col)
		seqHist [seqPos] [col] = base [col];
	seqHistP [seqPos] = base;

	if (seqAfterError)
	{

		if (seqAfterError != 0)
		{
			seqAfterError = seqAfterError - 1;
			if (seqAfterError == 0)
			{
				printSeqError();
			}
		}
		return;
	}


//	PRINTF("%d:%08lX ", slot, v);
//	return;
	if (seqDone)
		return;
	if (seqTotal >= ((DMABUFFSIZE32RX / DMABUFFSTEP32RX) * 10000L))
	{
		seqDone = 1;
		printSeqError();
		return;
	}
	if (! seqValid [slot])
	{
		seqValid [slot] = 1;
	}
	else
	{
		if (seqNext [slot] != v)
		{
			++ seqErrors;
			seqRun = 0;
			if (seqErrors == 2 && seqAfterError == 0)
				seqAfterError = 4;	// Еще четыре фрейма и стоп
		}
		else
		{
			++ seqRun;
		}
		++ seqTotal;
	}
	seqNext [slot] = v + 2;
}
#endif

void save_dmabuffer32rx(uintptr_t addr)
{
	voice32rx_t * const p = CONTAINING_RECORD(addr, voice32rx_t, buff);
	unsigned i;
	for (i = 0; i < DMABUFFSIZE32RX; i += DMABUFFSTEP32RX)
	{
		const IFADCvalue_t * const b = p->buff + i;
		//
#if 0
	if (0)
	{
		// Проверка качества линии передачи от FPGA
		uint_fast8_t slot;
		for (slot = 0; slot < DMABUFFSTEP32RX; ++ slot)
			validateSeq(slot, b [slot], b);
	}
	else if (1)
	{
		uint_fast8_t slot = DMABUF32RTS0I;	// slot 4
		validateSeq(slot, b [slot], b);
	}
#endif
#if 0
		// Тестирование - заменить приянтые квадратуры синтезированными
		inject_testsignals(b);
#endif
#if FPGAMODE_GW2A
		saverts96(b);	// использование данных о спектре, передаваемых в общем фрейме
#elif WITHRTS96
		saverts96pair(b);	// использование данных о спектре, передаваемых в общем фрейме
#endif /* WITHRTS96 */

#if WITHDSPEXTDDC
#if WITHUSEDUALWATCH
		FLOAT_t left = rxdmaproc(0, b [DMABUF32RX0I], b [DMABUF32RX0Q]);
		FLOAT_t right = rxdmaproc(1, b [DMABUF32RX1I], b [DMABUF32RX1Q]);
		savedemod_to_AF_proc(left, right);
#else /* WITHUSEDUALWATCH */
		FLOAT_t left = rxdmaproc(0, b [ DMABUF32RX0I], b [DMABUF32RX0Q]);
		savedemod_to_AF_proc(left, left);
#endif /* WITHUSEDUALWATCH */
#else /* WITHDSPEXTDDC */
		// обработка IF4
		FLOAT_t left = rxdmaproc(0, b [DMABUF32RX], 0);
		savedemod_to_AF_proc(left, left);
#endif /* WITHDSPEXTDDC */
#if WITHFPGAPIPE_CODEC1
		elfill_dmabuffer16rx(adpt_input(& afcodecrx, b [DMABUFF32RX_CODEC1_LEFT]), adpt_input(& afcodecrx, b [DMABUFF32RX_CODEC1_RIGHT]));
#endif /* WITHFPGAPIPE_CODEC1 */
	}

#if WITHWFM
	dsp_extbuffer32wfm(p->buff);
#endif /* WITHWFM */

	voice32rx.release_buffer(p);

	dsp_processtx(CNT32RX);	/* выборка семплов из источников звука и формирование потока на передатчик */
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
	return uacoutalt == UACOUTALT_AUDIO48;
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


#if WITHBUFFERSDEBUG

void buffers_diagnostics(void)
{
#if WITHINTEGRATEDDSP
#if 1
	//denoise16list.debug();
	codec16rx.debug();
	codec16tx.debug();
	//moni16.debug();
	//voice32tx.debug();
	//voice32rx.debug();
#endif
#if 1
	// USB
#if WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE) && 1
	uacout48.debug();
#endif
#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) && 1
#if WITHRTS192
	uacinrts192.debug();
#endif
#if WITHRTS96
	uacinrts96.debug();
#endif
	uacin48.debug();
#endif
#endif
	//message8.debug();

	PRINTF("\n");
#endif /* WITHINTEGRATEDDSP */
}


/* вызывается из обработчика таймерного прерывания */
static void buffers_spool(void * ctx)
{
#if WITHINTEGRATEDDSP
	// internal sources/targets
	//denoise16list.spool10ms();
	codec16rx.spool10ms();
	codec16tx.spool10ms();
	moni16.spool10ms();
	voice32tx.spool10ms();
	voice32rx.spool10ms();
	// USB
#if WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE)
	uacout48.spool10ms();
#endif
#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)
#if WITHRTS192
	uacinrts192.spool10ms();
#endif
#if WITHRTS96
	uacinrts96.spool10ms();
#endif
#endif
	uacin48.spool10ms();
	//message8.spool10ms();
#endif /* WITHINTEGRATEDDSP */
}

#else /* WITHBUFFERSDEBUG */

void buffers_diagnostics(void)
{
}

#endif /* WITHBUFFERSDEBUG */

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


#if WITHSKIPUSERMODE || CTLSTYLE_V3D

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


void
board_set_swaprts(uint_fast8_t v)	/* если используется конвертор на Rafael Micro R820T - требуется инверсия спектра */
{
	const uint_fast8_t n = v != 0;
	if (glob_swaprts != n)
	{
		glob_swaprts = n;
		//board_dsp1regchanged();
	}
}
