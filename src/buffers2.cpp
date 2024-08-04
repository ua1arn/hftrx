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
#include "display/display.h"

//#undef RAMNC
//#define RAMNC

//#define WITHBUFFERSDEBUG WITHDEBUG
#define BUFOVERSIZE 1

// Одна из задач resampler - привести частоту кодека к требуемой для 48 кГц (lrckf=24576000, (clk=24571428)) = 0.99981396484375
// Для USB - исправляемая погрешность = 0.02% - один сэмпл добавить/убрать на 5000 сэмплов
static const unsigned SKIPSAMPLES = 5000;	// раз в 5000 сэмплов добавление/удаление одного сэмпла

#define VOICE16RX_CAPACITY (16 * BUFOVERSIZE)	// прием от кодекв
#define VOICE16TX_CAPACITY (32 * BUFOVERSIZE)	// должно быть достаточное количество буферов чтобы запомнить буфер с выхода speex
#define VOICE16TXMONI_CAPACITY (64 * BUFOVERSIZE)	// во столько же на сколько буфр от кодека больше чем буфер к кодеку (если наоборот - минимум)

#define VOICE16RX_RESAMPLING 1	// прием от кодека - требуется ли resampling
#define VOICE16TX_RESAMPLING 1	// передача в кодек - требуется ли resampling

#define UACINRTS192_CAPACITY ((48 / OUTSAMPLES_AUDIO48) * 32 * BUFOVERSIZE)
#define UACINRTS96_CAPACITY ((48 / OUTSAMPLES_AUDIO48) * 32 * BUFOVERSIZE)
#define UACIN48_CAPACITY ((48 / OUTSAMPLES_AUDIO48) * 32 * BUFOVERSIZE)	// должно быть достаточное количество буферов чтобы запомнить буфер с выхода speex

#define UACOUT48_CAPACITY ((48 / OUTSAMPLES_AUDIO48) * 32 * BUFOVERSIZE)

#define SPEEX_CAPACITY (5 * BUFOVERSIZE)

#define VOICE32RX_CAPACITY (4 * BUFOVERSIZE)
#define VOICE32TX_CAPACITY (16 * BUFOVERSIZE)
#define VOICE32RTS_CAPACITY (4 * BUFOVERSIZE)	// dummy fn

#define AUDIOREC_CAPACITY (18 * BUFOVERSIZE)


#define BTIN48_CAPACITY 12
#define BTOUT48_CAPACITY 12

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
static volatile uint_fast8_t uacoutalt = UACOUTALT_NONE;

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
public:
	const char * name;
	adapter_t adp;
	adapters(int leftbit, int rightspace, const char * aname) :
		name(aname)
	{
		adpt_initialize(& adp, leftbit, rightspace, name);
	}

	// преобразование в буфер из внутреннего представления
	unsigned poke_LE(uint8_t * buff, apptype ch0, apptype ch1)
	{
		switch (nch)
		{
		case 1:
			switch (ss)
			{
			case 2:
				// 16 bit values - mono
				USBD_poke_u16(buff + 0 * ss, adpt_output(& adp, ch0));
				break;
			case 3:
				// 24 bit values - mono
				USBD_poke_u24(buff + 0 * ss, adpt_output(& adp, ch0));
				break;
			case 4:
				// 32 bit values - mono
				USBD_poke_u32(buff + 0 * ss, adpt_output(& adp, ch0));
				break;
			}
			break;

		case 2:
			switch (ss)
			{
			case 2:
				// 16 bit values - stereo
				USBD_poke_u16(buff + 0 * ss, adpt_output(& adp, ch0));
				USBD_poke_u16(buff + 1 * ss, adpt_output(& adp, ch1));
				break;
			case 3:
				// 24 bit values - stereo
				USBD_poke_u24(buff + 0 * ss, adpt_output(& adp, ch0));
				USBD_poke_u24(buff + 1 * ss, adpt_output(& adp, ch1));
				break;
			case 4:
				// 32 bit values - stereo
				USBD_poke_u32(buff + 0 * ss, adpt_output(& adp, ch0));
				USBD_poke_u32(buff + 1 * ss, adpt_output(& adp, ch1));
				break;
			}
			break;
		}
		return ss * nch;
	}

	// преобразование в буфер из внутреннего представления
	unsigned poke_IEEE_FLOAT(uint8_t * buff, apptype ch0, apptype ch1)
	{
		switch (nch)
		{
		case 1:
			switch (ss)
			{
			case 4:
				// 32 bit values - mono
				USBD_poke_IEEE_FLOAT(buff + 0 * ss, ch0);
				break;
			}
			break;

		case 2:
			switch (ss)
			{
			case 4:
				// 32 bit values - stereo
				USBD_poke_IEEE_FLOAT(buff + 0 * ss, ch0);
				USBD_poke_IEEE_FLOAT(buff + 1 * ss, ch1);
				break;
			}
			break;
		}
		return ss * nch;
	}


	// преобразование в буфер из внутреннего представления
	unsigned poketransf_LE(const transform_t * tfm, uint8_t * buff, apptype ch0, apptype ch1)
	{
		switch (nch)
		{
		case 1:
			switch (ss)
			{
			case 2:
				// 16 bit values - mono
				USBD_poke_u16(buff + 0 * ss, transform_do32(tfm, ch0));
				break;
			case 3:
				// 24 bit values - mono
				USBD_poke_u24(buff + 0 * ss, transform_do32(tfm, ch0));
				break;
			case 4:
				// 32 bit values - mono
				USBD_poke_u32(buff + 0 * ss, transform_do32(tfm, ch0));
				break;
			}
			break;

		case 2:
			switch (ss)
			{
			case 2:
				// 16 bit values - stereo
				USBD_poke_u16(buff + 0 * ss, transform_do32(tfm, ch0));
				USBD_poke_u16(buff + 1 * ss, transform_do32(tfm, ch1));
				break;
			case 3:
				// 24 bit values - stereo
				USBD_poke_u24(buff + 0 * ss, transform_do32(tfm, ch0));
				USBD_poke_u24(buff + 1 * ss, transform_do32(tfm, ch1));
				break;
			case 4:
				// 32 bit values - stereo
				USBD_poke_u32(buff + 0 * ss, transform_do32(tfm, ch0));
				USBD_poke_u32(buff + 1 * ss, transform_do32(tfm, ch1));
				break;
			}
			break;
		}
		return ss * nch;
	}

	// преобразование в буфер из внутреннего представления
	unsigned poketransf_IEEE_FLOAT(const transform_t * tfm, uint8_t * buff, apptype ch0, apptype ch1)
	{
		switch (nch)
		{
		case 1:
			switch (ss)
			{
			case 4:
				// 32 bit values - mono
				USBD_poke_IEEE_FLOAT(buff + 0 * ss, (int32_t) transform_do32(tfm, ch0) / INT32_MAX);
				break;
			}
			break;

		case 2:
			switch (ss)
			{
			case 4:
				// 32 bit values - stereo
				USBD_poke_IEEE_FLOAT(buff + 0 * ss, (int32_t) transform_do32(tfm, ch0) / INT32_MAX);
				USBD_poke_IEEE_FLOAT(buff + 1 * ss, (int32_t) transform_do32(tfm, ch1) / INT32_MAX);
				break;
			}
			break;
		}
		return ss * nch;
	}

	// во внутреннее представление из буфера
	unsigned peek_LE(const uint8_t * buff, apptype * dest)
	{
		switch (nch)
		{
		case 1:
			switch (ss)
			{
			case 2:
				// 16 bit values - mono
				dest [0] = adpt_input(& adp, USBD_peek_u16(buff + 0 * ss));
				break;
			case 3:
				// 24 bit values - mono
				dest [0] = adpt_input(& adp, USBD_peek_u24(buff + 0 * ss));
				break;
			case 4:
				// 32 bit values - mono
				dest [0] = adpt_input(& adp, USBD_peek_u32(buff + 0 * ss));
				break;
			}
			break;

		case 2:
			switch (ss)
			{
			case 2:
				// 16 bit values - stereo
				dest [0] = adpt_input(& adp, USBD_peek_u16(buff + 0 * ss));
				dest [1] = adpt_input(& adp, USBD_peek_u16(buff + 1 * ss));
				break;
			case 3:
				// 24 bit values - stereo
				dest [0] = adpt_input(& adp, USBD_peek_u24(buff + 0 * ss));
				dest [1] = adpt_input(& adp, USBD_peek_u24(buff + 1 * ss));
				break;
			case 4:
				// 32 bit values - stereo
				dest [0] = adpt_input(& adp, USBD_peek_u32(buff + 0 * ss));
				dest [1] = adpt_input(& adp, USBD_peek_u32(buff + 1 * ss));
				break;
			}
			break;
		}
		return ss * nch;
	}

	// во внутреннее представление из буфера
	unsigned peek_IEEE_FLOAT(const uint8_t * buff, apptype * dest)
	{
		switch (nch)
		{
		case 1:
			switch (ss)
			{
			case 4:
				// 32 bit values - mono
				dest [0] = USBD_peek_IEEE_FLOAT(buff + 0 * ss);
				break;
			}
			break;

		case 2:
			switch (ss)
			{
			case 4:
				// 32 bit values - stereo
				dest [0] = USBD_peek_IEEE_FLOAT(buff + 0 * ss);
				dest [1] = USBD_peek_IEEE_FLOAT(buff + 1 * ss);
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

template <typename buffitem_t, int hasresample, int hacheckready>
class blists
{
	typedef typeof(buffitem_t::v) element_t;
public:
	IRQLSPINLOCK_t irqllocl;
	IRQL_t irqllockarg;
#if WITHBUFFERSDEBUG
	int errallocate;
	int saveount;
#endif /* WITHBUFFERSDEBUG */

	int readycount;
	int freecount;
	LIST_ENTRY freelist;
	LIST_ENTRY readylist;
	const char * name;
	unsigned wbskip;	// сколько блоков пропускаем без контроля расхожденеия скорости
	unsigned wbadded;
	unsigned wbdeleted;
	unsigned wbgatoffs [3];		// смещения в буфере для формирования нового блока
	unsigned wbgatsize [3];		// рамеры
	unsigned wbgatix;			// Текуший инлдекс параметров для заполнения выходного буфера
	element_t * workbuff;		// буфер - источник данных, над которым выполняется ресэмплинг (не валиден если wbgatcnt == 3)

	fqmeter fqin, fqout;

	// параметры реакции ресэмплера
	int MINMLEVEL, NORMLEVEL, MAXLEVEL;
	bool outready;

public:
	blists() :
		name("uninited")
	{

	}
	blists(IRQL_t airql, const char * aname, buffitem_t * storage, unsigned capacity) :
		irqllockarg(airql),
#if WITHBUFFERSDEBUG
		errallocate(0),
		saveount(0),
#endif /* WITHBUFFERSDEBUG */
		readycount(0),
		freecount(capacity),
		name(aname),
		wbskip(0),	// сколько блоков пропускаем без контроля расхожденеия скорости
		wbadded(0),
		wbdeleted(0),
		wbgatix(ARRAY_SIZE(wbgatsize)),	// состояние - нет ничего для формирования нового блокв
		MINMLEVEL(hasresample ? 1 * capacity / 4 : 0),
		NORMLEVEL(hasresample ? 2 * capacity / 4 : capacity / 4),	// без resample используется только этот параметр
		MAXLEVEL(hasresample ? 3 * capacity / 4 : 0),
		outready(false)	// накоплено нужное количество буферов для старта
	{
		InitializeListHead(& freelist);
		InitializeListHead(& readylist);
		IRQLSPINLOCK_INITIALIZE(& irqllocl);
		for (unsigned i = 0; i < capacity; ++ i)
		{
			buffitem_t * const p = & storage [i];
			p->tag0 = this;
			p->tag2 = p;
			p->tag3 = p;
			InsertHeadList(& freelist, & p->item);
		}
		//PRINTF("%s: %u[%u]\n", name, sizeof (storage [0].v.buff), capacity);
 	}

	/* готовность буферов с "гистерезисом". */
	void fiforeadyupdate()
	{
		outready = outready ? readycount != 0 : readycount >= NORMLEVEL;
	}

	// сохранить в списке свободных
	void release_buffer(element_t * addr)
	{
		buffitem_t * const p = CONTAINING_RECORD(addr, buffitem_t, v);
		ASSERT3(p->tag0 == this, __FILE__, __LINE__, name);
		ASSERT3(p->tag2 == p, __FILE__, __LINE__, name);
		ASSERT3(p->tag3 == p, __FILE__, __LINE__, name);
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql, irqllockarg);

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
		IRQLSPIN_LOCK(& irqllocl, & oldIrql, irqllockarg);

		InsertHeadList(& readylist, & p->item);
		++ readycount;
		fiforeadyupdate();
#if WITHBUFFERSDEBUG
		fqin.pass(get_datasize() / (addr->ss * addr->nch));
		++ saveount;
#endif /* WITHBUFFERSDEBUG */

		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
	}

	// получить из списка готовых
	bool get_readybuffer_raw(element_t * * dest)
	{
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql, irqllockarg);
		if (! IsListEmpty(& readylist))
		{
			const PLIST_ENTRY t = RemoveTailList(& readylist);
			-- readycount;
			fiforeadyupdate();
	#if WITHBUFFERSDEBUG
			fqout.pass(get_datasize() / ((* dest)->ss * (* dest)->nch));
	#endif /* WITHBUFFERSDEBUG */
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
		IRQLSPIN_LOCK(& irqllocl, & oldIrql, irqllockarg);
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

	unsigned take_readys(LIST_ENTRY * list, unsigned n)
	{
		unsigned v = 0;
		IRQL_t oldIrql;

		InitializeListHead(list);
		IRQLSPIN_LOCK(& irqllocl, & oldIrql, irqllockarg);
		while (n -- && ! IsListEmpty(& readylist))
		{
			const PLIST_ENTRY t = RemoveTailList(& readylist);
			-- readycount;
			InsertHeadList(list, t);
			++ v;
		}
		fiforeadyupdate();
		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
		return v;
	}

	void save_frees(LIST_ENTRY * list)
	{
		IRQL_t oldIrql;

		IRQLSPIN_LOCK(& irqllocl, & oldIrql, irqllockarg);

		while (! IsListEmpty(list))
		{
			const PLIST_ENTRY t = RemoveTailList(list);
			buffitem_t * const p = CONTAINING_RECORD(t, buffitem_t, item);
			ASSERT3(p->tag0 == this, __FILE__, __LINE__, name);
			ASSERT3(p->tag2 == p, __FILE__, __LINE__, name);
			ASSERT3(p->tag3 == p, __FILE__, __LINE__, name);
			InsertHeadList(& freelist, t);
			++ freecount;
		}

		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
	}

	bool get_readybuffer(element_t * * dest)
	{
		if (hasresample)
		{
			const unsigned wbgss = element_t::ss * element_t::nch;	// кодчиество байтов одного сэмпла на вссе каналы
			const unsigned total = get_datasize();	// полный размер буфера
			const unsigned SKIPBUFFS = SKIPSAMPLES / (total / wbgss);
			if (! outready)
				return false;
			if (wbskip != 0)
			{
				-- wbskip;
				return get_readybufferarj(dest, false, false);
			}
			else if (readycount <= MINMLEVEL)
			{
				++ wbadded;
				wbskip = SKIPBUFFS;
				return get_readybufferarj(dest, true, false);
			}
			else if (readycount >= MAXLEVEL)
			{
				++ wbdeleted;
				wbskip = SKIPBUFFS;
				return get_readybufferarj(dest, false, true);
			}
			else
			{
				return get_readybufferarj(dest, false, false);
			}
		}
		else
		{
			if (hacheckready && ! outready)
				return false;
			return get_readybuffer_raw(dest);
		}
	}

	// получить из списка свободных, если нет - ошибка
	bool get_freebuffer(element_t * * dest)
	{
		return get_freebuffer_raw(dest);
	}

	// получить из списка свободных, если нет - из готовых
	bool get_freebufferforced(element_t * * dest)
	{
		if (get_freebuffer_raw(dest))
			return true;

		// сбрасываем количество до половины (NORMLEVEL)
		LIST_ENTRY list;
		take_readys(& list, NORMLEVEL);
		save_frees(& list);

		return get_freebuffer_raw(dest);
	}

	// получить из списка свободных, если нет - из готовых
	bool get_readybufferforced_nopurge(element_t * * dest)
	{
		return get_readybuffer_raw(dest) ||  get_freebuffer_raw(dest);
	}

	static int_fast32_t get_cachesize(void)
	{
		return offsetof(element_t, pad) - offsetof(element_t, buff);
	}

	static int_fast32_t get_datasize(void)
	{
		return sizeof element_t::buff;
	}

	void debug()
	{
#if WITHBUFFERSDEBUG
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllocl, & oldIrql, irqllockarg);
		unsigned fin = fqin.getfreq();
		unsigned fout = fqout.getfreq();
		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
		//PRINTF("%s:s=%d,a=%d,o=%d,f=%d ", name, saveount, errallocate, readycount, freecount);
		PRINTF(" %s:e=%d,y=%d,f=%d,%uk/%uk", name, errallocate, readycount, freecount, (fin + 500) / 1000, (fout + 500) / 1000);
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
		IRQLSPIN_LOCK(& irqllocl, & oldIrql, irqllockarg);

		fqin.spool10ms();
		fqout.spool10ms();

		IRQLSPIN_UNLOCK(& irqllocl, oldIrql);
#endif /* WITHBUFFERSDEBUG */
	}

	///////////
	/// resampler

	// Гарантированно получене полного буфера или отказ
	// исполнение команд на добавление или удаление одного семплв
	int get_readybufferarj(element_t * * dest, bool add, bool del)
	{
		const unsigned wbgss = element_t::ss * element_t::nch;	// кодчиество байтов одного сэмпла на вссе каналы
		const unsigned total = get_datasize();
		const unsigned wbgatixtotal = ARRAY_SIZE(wbgatsize);	// все элементы выданы

		// ситуация, когда позиция источника и получателя
		// выровнены на границу буфера и нет запросов на ресэмплинг
		if (wbgatixtotal == wbgatix && ! add && ! del)
		{
			return get_readybuffer_raw(dest);
		}

		// в остальных случаях заполняем новый буфер
		while(! get_freebufferforced(dest))	// выходной буфер
			ASSERT(0);

		// Цикл, пока не наберем требуемого колчиества samples в выходной буфер
		for (unsigned score = 0; score < total;)
		{
			if (wbgatix >= wbgatixtotal)
			{
				// Данных нет в workbuff
				if (! get_readybuffer_raw(& workbuff))
				{
					release_buffer(* dest);	// освбодождаем ранее полученный буфер результата
					return false;			// Данных нет и в источнике
				}

				// Есть полный буфер в workbuff
				wbgatoffs [wbgatixtotal - 1] = 0;
				wbgatsize [wbgatixtotal - 1] = total;

				wbgatix = wbgatixtotal - 1;
			}

			// выполнение ресэмплинга
			if (add)
			{
				// ещё остались данные
				// Добавляем sample
				ASSERT(wbgatix > 0);
				wbgatoffs [wbgatix - 1] = wbgatoffs [wbgatix];
				wbgatsize [wbgatix - 1] = wbgss;
				wbgatix -= 1;
				add = false;
			}
			else if (del)
			{
				// ещё остались данные
				// удаляем sample
				ASSERT(wbgatsize [wbgatix] >= wbgss);
				wbgatoffs [wbgatix] += wbgss;
				wbgatsize [wbgatix] -= wbgss;
				del = false;
			}

			// формируем содержимое выходного буфера
			{
				const unsigned chunk = ulmin32(total - score, wbgatsize [wbgatix]);
				memcpy((uint8_t *) (* dest)->buff + score, (uint8_t *) workbuff->buff + wbgatoffs [wbgatix], chunk);	// копируем остаток предыдущего буфера
				score += chunk;
				wbgatoffs [wbgatix] += chunk;
				wbgatsize [wbgatix] -= chunk;
				if (0 == wbgatsize [wbgatix])
				{
					if (++ wbgatix >= wbgatixtotal)
					{
						release_buffer(workbuff);
					}
				}
			}
		}
		// выходной буфер готов
		return true;
	}
};

template <typename sample_t, typename element_t, int hasresample, int hacheckready>
class dmahandle: public blists<element_t, hasresample, hacheckready>
{
	typedef blists<element_t, hasresample, hacheckready> parent_t;
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
	void savedata(sample_t ch0, sample_t ch1, unsigned (* putcbf)(typeof (wel_t::buff [0]) * b, sample_t ch0, sample_t ch1))
	{
		if (wb == NULL)
		{
			if (! parent_t::get_freebuffer(& wb))
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

	uint_fast8_t fetchdata(sample_t * dest, unsigned (* getcbf)(typeof (wel_t::buff [0]) * b, sample_t * dest))
	{
		if (rb == NULL)
		{
			if (! parent_t::get_readybuffer(& rb))
				return 0;
			rbn = 0;
		}
		rbn += getcbf(rb->buff + rbn, dest);
		if (rbn >= ARRAY_SIZE(rb->buff))
		{
			parent_t::release_buffer(rb);
			rb = NULL;
		}
		return 1;
	}

	uint_fast8_t fetchdata_resample(sample_t * dest, unsigned (* getcbf)(typeof (wel_t::buff [0]) * b, sample_t * dest), bool (fillresampled)(sample_t *, unsigned ndst, unsigned ndstch, unsigned nsrc, unsigned nsrcch), unsigned nsrc, unsigned nsrcch)
	{
		if (rb == NULL)
		{
			if (! parent_t::get_freebuffer(& rb))
				return 0;
			if (!fillresampled(rb->buff, ARRAY_SIZE(rb->buff), rb->nch, nsrc, nsrcch))
			{
				parent_t::release_buffer(rb);
				rb = NULL;
				return 0;
			}
			rbn = 0;
		}
		rbn += getcbf(rb->buff + rbn, dest);
		if (rbn >= ARRAY_SIZE(rb->buff))
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

enum { CNT16RX = DMABUFFSIZE16RX / DMABUFFSTEP16RX };
enum { CNT16TX = DMABUFFSIZE16TX / DMABUFFSTEP16TX };
enum { CNT16MONI = DMABUFFSIZE16MONI / DMABUFFSTEP16MONI };
enum { CNT32RX = DMABUFFSIZE32RX / DMABUFFSTEP32RX };
enum { CNT32TX = DMABUFFSIZE32TX / DMABUFFSTEP32TX };
//enum { CNT32RTS96 = DMABUFFSIZE32RTS96 / DMABUFFSTEP32RTS96 };
//enum { VOICESMIKE16NORMAL = 6 };	// Нормальное количество буферов в очереди
//enum { MIKELEVEL = 6 };
//enum { PHONESLEVEL = 6 };

// Denoise operations

typedef ALIGNX_BEGIN struct denoise16
{
	ALIGNX_BEGIN speexel_t buff [NTRX * FIRBUFSIZE] ALIGNX_END;
	enum { ss = 1, nch = NTRX };	// stub for resampling support
} ALIGNX_END denoise16_t;

typedef buffitem<denoise16_t> denoise16buf_t;
typedef dmahandle<FLOAT_t, denoise16buf_t, 0, 0> denoise16dma_t;

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

typedef dmahandle<FLOAT_t, voice16rxbuf_t, VOICE16RX_RESAMPLING, 1> voice16rxdma_t;
typedef dmahandle<FLOAT_t, voice16txbuf_t, VOICE16TX_RESAMPLING, 1> voice16txdma_t;

static RAMNC voice16rxbuf_t voice16rxbuf [VOICE16RX_CAPACITY];
static RAMNC voice16txbuf_t voice16txbuf [VOICE16TX_CAPACITY];


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

int_fast32_t datasize_dmabuffer16rx(void)
{
	return codec16rx.get_datasize();
}

int_fast32_t datasize_dmabuffer16tx(void)
{
	return codec16tx.get_datasize();
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
#elif defined CODEC1_TYPE
	dsp_fillphones(CNT16TX);
#endif

	voice16tx_t * dest;
	while (! codec16tx.get_readybuffer(& dest) && ! codec16tx.get_freebufferforced(& dest))
		ASSERT(0);

#if 0
	// тестирование вывода на кодек
	for (unsigned i = 0; i < ARRAY_SIZE(dest->buff); i += DMABUFFSTEP16TX)
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



typedef dmahandle<FLOAT_t, moni16buf_t, 0, 1> moni16txdma_t;

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

static RAMNC voice32txbuf_t voice32txbuf [VOICE32TX_CAPACITY];

typedef dmahandle<FLOAT_t, voice32txbuf_t, 0, 1> voice32txdma_t;

//static voice32txlist_t voice32tx(IRQL_REALTIME, "32tx");
static voice32txdma_t voice32tx(IRQL_REALTIME, "32tx", voice32txbuf, ARRAY_SIZE(voice32txbuf));

int_fast32_t cachesize_dmabuffer32tx(void)
{
	return voice32tx.get_cachesize();
}

int_fast32_t datasize_dmabuffer32tx(void)
{
	return voice32tx.get_datasize();
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

#if (CPUSTYLE_XC7Z || CPUSTYLE_XCZU) && WITHLFM
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

#if WITHFPGAPIPE_CODEC1

// копирование полей для кодека в передаваемый на FPGA буфер
static uintptr_t RAMFUNC
pipe_dmabuffer32tx_codec1(uintptr_t addr32tx, uintptr_t addr16tx)
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
uintptr_t getfilled_dmabuffer32tx(void)
{
#if ! defined CODEC1_TYPE
	dsp_fillphones(CNT32TX);
#endif
	unsigned i;
	voice32tx_t * dest;
	while (! voice32tx.get_readybuffer(& dest) && ! voice32tx.get_freebufferforced(& dest))
		ASSERT(0);
	const uintptr_t addr = (uintptr_t) dest->buff;
	// дополнение совместно передаваемыми данными

#if WITHFPGAPIPE_CODEC1
	/* при необходимости добавляем слоты для передачи на кодек */
	const uintptr_t addr16 = getfilled_dmabuffer16tx();
	pipe_dmabuffer32tx_codec1(addr, addr16);	// копирование полей для кодека в передаваемый на FPGA буфер
	release_dmabuffer16tx(addr16);	/* освобождаем буфер как переданный */
#endif /* WITHFPGAPIPE_CODEC1 */

	/* Слоты с информацией о частоте приёма */
	for (i = 0; i < (unsigned) CNT32TX; ++ i)
	{
		IFDACvalue_t * const buff = dest->buff + i * DMABUFFSTEP32TX;
	#if WITHFPGAPIPE_NCORX0
		buff [DMABUF32TX_NCO1] = dspfpga_get_nco1();
	#endif /* WITHFPGAPIPE_NCORX0 */
	#if WITHFPGAPIPE_NCORX1
		buff [DMABUF32TX_NCO2] = dspfpga_get_nco2();
	#endif /* WITHFPGAPIPE_NCORX1 */
	#if WITHFPGAPIPE_NCORTS
		buff [DMABUF32TX_NCORTS] = dspfpga_get_ncorts();
	#endif /* WITHFPGAPIPE_NCORTS */
	}
	return addr;
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

typedef dmahandle<int_fast32_t, voice32rxbuf_t, 0, 1> voice32rxdma_t;

static voice32rxdma_t voice32rx(IRQL_REALTIME, "32rx", voice32rxbuf, ARRAY_SIZE(voice32rxbuf));

int_fast32_t cachesize_dmabuffer32rx(void)
{
	return voice32rx.get_cachesize();
}

int_fast32_t datasize_dmabuffer32rx(void)
{
	return voice32rx.get_datasize();
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
	while (! voice32rx.get_freebufferforced(& dest))
		ASSERT(0);
//	memset(dest->buff, 0, sizeof dest->buff);
//	dcache_clean((uintptr_t) dest->buff, voice32rx.get_cachesize());
	return (uintptr_t) dest->buff;
}

// can be zero
uintptr_t getfilled_dmabuffer32rx(void)
{
	voice32rx_t * dest;
	if (! voice32rx.get_readybuffer(& dest))
		return 0;
	return (uintptr_t) dest->buff;
}

void dsphftrxproc_spool_user(void)
{
	voice32rx_t * dest;
	if (voice32rx.get_readybuffer(& dest))
	{
		process_dmabuffer32rx(dest->buff);
		voice32rx.release_buffer(dest);
		dsp_processtx(CNT32RX);	/* выборка семплов из источников звука и формирование потока на передатчик */
	}
}

void save_dmabuffer32rx(uintptr_t addr)
{
	voice32rx_t * const p = CONTAINING_RECORD(addr, voice32rx_t, buff);
	voice32rx.save_buffer(p);
	// dsphftrxproc_spool_user on other CPUs
#if LINUX_SUBSYSTEM || (WITHINTEGRATEDDSP && ((HARDWARE_NCORES <= 2) || ! WITHSMPSYSTEM))
	dsphftrxproc_spool_user();
#endif /* WITHINTEGRATEDDSP */
}

///////////////////////////////////////
///

#if WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE)

// USB AUDIO OUT

typedef struct
{
	ALIGNX_BEGIN  uint8_t buff [UAC_GROUPING_DMAC * UACOUT_AUDIO48_DATASIZE_DMAC] ALIGNX_END;
	ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
	enum { ss = UACOUT_AUDIO48_SAMPLEBYTES, nch = UACOUT_FMT_CHANNELS_AUDIO48 };	// resampling support
} uacout48_t;

typedef buffitem<uacout48_t> uacout48buf_t;

static RAMNC uacout48buf_t uacout48buf [UACOUT48_CAPACITY];

typedef dmahandle<FLOAT_t, uacout48buf_t, 1, 1> uacout48dma_t;

typedef adapters<FLOAT_t, (int) UACOUT_AUDIO48_SAMPLEBYTES, (int) UACOUT_FMT_CHANNELS_AUDIO48> uacout48adpt_t;

static uacout48dma_t uacout48(IRQL_REALTIME, "uaco48", uacout48buf, ARRAY_SIZE(uacout48buf));

static uacout48adpt_t uacout48adpt(UACOUT_AUDIO48_SAMPLEBYTES * 8, 0, "uaco48");

int_fast32_t cachesize_dmabufferuacout48(void)
{
	return uacout48.get_cachesize();
}

int_fast32_t datasize_dmabufferuacout48(void)
{
	return uacout48.get_datasize();
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
	return uacout48adpt.peek_LE(b, dest);
}

// Возвращает не-ноль если данные есть
uint_fast8_t elfetch_dmabufferuacout48(FLOAT_t * dest)
{
	if (uacout48.fetchdata(dest, getcbf_dmabufferuacout48))
	{
		// WITHUSBUACOUT test
		enum { L, R };
	//	dest [L] = get_lout();	// левый канал
	//	dest [R] = get_rout();	// правый канал
	//	dest [R] = dest [L];
		return 1;
	}
	return 0;
}

#endif /* WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE) */

#if WITHUSEUSBBT

#define BTSSCALE 20

#define BTIO44P1_SAMPLEBYTES 2
#define BTIO44P1_CHANNELS 2

#define BTIO32_SAMPLEBYTES 2
#define BTIO32_CHANNELS 2

#define BTIO16_SAMPLEBYTES 2
#define BTIO16_CHANNELS 2

// resampling 44.1 <-> 48 делается на интервале 10 мс

// Буфер на стороне 48 кГц
typedef struct
{
	ALIGNX_BEGIN  FLOAT_t buff [BTSSCALE * 480 * 2] ALIGNX_END;
	//ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
	enum { ss = sizeof (FLOAT_t), nch = 2 };	// resampling support
} btio48k_t;


// Буфер на стороне 44.1 кГц
typedef struct
{
	ALIGNX_BEGIN  int16_t buff [BTSSCALE * 441 * BTIO44P1_CHANNELS] ALIGNX_END;
	//ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
	enum { ss = sizeof (int16_t), nch = BTIO44P1_CHANNELS };	// resampling support
} btio44p1k_t;

// Буфер на стороне 32 кГц
typedef struct
{
	ALIGNX_BEGIN  int16_t buff [BTSSCALE * 32 * BTIO32_CHANNELS] ALIGNX_END;
	//ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
	enum { ss = sizeof (int16_t), nch = BTIO32_CHANNELS };	// resampling support
} btio32k_t;

// Буфер на стороне 16 кГц
typedef struct
{
	ALIGNX_BEGIN  int16_t buff [BTSSCALE * 16 * 1] ALIGNX_END;
	//ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
	enum { ss = sizeof (int16_t), nch = 1 };	// resampling support
} btio16k_t;

// Буфер на стороне 8 кГц
typedef struct
{
	ALIGNX_BEGIN  int16_t buff [BTSSCALE * 80 * 1] ALIGNX_END;
	//ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
	enum { ss = sizeof (int16_t), nch = 1 };	// resampling support
} btio8k_t;

typedef adapters<FLOAT_t, (int) BTIO44P1_SAMPLEBYTES, (int) BTIO44P1_CHANNELS> btioadpt_t;
static btioadpt_t btioadpt(BTIO44P1_SAMPLEBYTES * 8, 0, "btioadpt");

typedef buffitem<btio48k_t> btio48kbuf_t;
typedef buffitem<btio44p1k_t> btio44p1kbuf_t;
typedef buffitem<btio32k_t> btio32kbuf_t;
typedef buffitem<btio16k_t> btio16kbuf_t;
typedef buffitem<btio8k_t> btio8kbuf_t;

typedef dmahandle<FLOAT_t, btio48kbuf_t, 0, 1> btio48kdma_t;
typedef dmahandle<FLOAT_t, btio48kbuf_t, 1, 1> btio48kdmaRS_t;
typedef dmahandle<int16_t, btio44p1kbuf_t, 0, 1> btio44p1kdma_t;
typedef dmahandle<int16_t, btio44p1kbuf_t, 1, 1> btio44p1kdmaRS_t;
typedef dmahandle<int16_t, btio32kbuf_t, 0, 1> btio32kdma_t;
typedef dmahandle<int16_t, btio32kbuf_t, 1, 1> btio32kdmaRS_t;
typedef dmahandle<int16_t, btio16kbuf_t, 0, 1> btio16kdma_t;
typedef dmahandle<int16_t, btio16kbuf_t, 1, 1> btio16kdmaRS_t;
typedef dmahandle<int16_t, btio8kbuf_t, 0, 1> btio8kdma_t;
typedef dmahandle<int16_t, btio8kbuf_t, 1, 1> btio8kdmaRS_t;

static btio44p1kbuf_t  btout44p1kbuf [BTOUT48_CAPACITY];
static btio32kbuf_t  btout32kbuf [BTOUT48_CAPACITY];
static btio16kbuf_t  btout16kbuf [BTOUT48_CAPACITY];
static btio8kbuf_t  btout8kbuf [BTOUT48_CAPACITY];
static btio48kbuf_t  btout48kbuf [2];

static btio44p1kbuf_t  btin44p1kbuf [2];
static btio32kbuf_t  btin32kbuf [2];
static btio16kbuf_t  btin16kbuf [2];
static btio8kbuf_t  btin8kbuf [2];
static btio48kbuf_t  btin48kbuf [BTIN48_CAPACITY];

/* Канал из трансивера в BT */
static btio48kdmaRS_t btin48k(IRQL_REALTIME, "btin48k", btin48kbuf, ARRAY_SIZE(btin48kbuf));
static btio44p1kdma_t btin44p1k(IRQL_REALTIME, "btin44p1k", btin44p1kbuf, ARRAY_SIZE(btin44p1kbuf));
static btio32kdma_t btin32k(IRQL_REALTIME, "btin32k", btin32kbuf, ARRAY_SIZE(btin32kbuf));
static btio16kdma_t btin16k(IRQL_REALTIME, "btin16k", btin16kbuf, ARRAY_SIZE(btin16kbuf));
static btio8kdma_t btin8k(IRQL_REALTIME, "btin8k", btin8kbuf, ARRAY_SIZE(btin8kbuf));

/* Канал из BT в трансивер */
static btio44p1kdmaRS_t btout44p1k(IRQL_REALTIME, "btout44p1k", btout44p1kbuf, ARRAY_SIZE(btout44p1kbuf));
static btio32kdmaRS_t btout32k(IRQL_REALTIME, "btout32k", btout32kbuf, ARRAY_SIZE(btout32kbuf));
static btio16kdmaRS_t btout16k(IRQL_REALTIME, "btout16k", btout16kbuf, ARRAY_SIZE(btout16kbuf));
static btio8kdmaRS_t btout8k(IRQL_REALTIME, "btout8k", btout8kbuf, ARRAY_SIZE(btout8kbuf));
static btio48kdma_t btout48k(IRQL_REALTIME, "btout48k", btout48kbuf, ARRAY_SIZE(btout48kbuf));


static ARM_MORPH(arm_biquad_cascade_stereo_df2T_instance) fltout44p1k;
static ARM_MORPH(arm_biquad_cascade_stereo_df2T_instance) fltout32k;
static ARM_MORPH(arm_biquad_cascade_stereo_df2T_instance) fltout16k;
static ARM_MORPH(arm_biquad_cascade_stereo_df2T_instance) fltout8k;

// Заполнение с ресэмплингом буфера данными из btin48k
// n - требуемое количество samples
// возвращает признак того, что данные в источнике есть
// btin48k -> resampler -> btin44p1k
static bool fetchdata_RS_btin48(FLOAT_t * dst, unsigned ndst, unsigned ndstch, unsigned nsrc, unsigned nsrcch)
{
	btio48k_t * addr;
	if (! btin48k.get_readybuffer(& addr))
		return false;
	const FLOAT_t * const src = addr->buff;
	//unsigned nsrc = ARRAY_SIZE(addr->buff);
	ASSERT(ndst == BTSSCALE * 480 * 2);

	const unsigned srcframes = nsrc / nsrcch;
	const unsigned dstframes = ndst / ndstch;
	const FLOAT_t scale = (FLOAT_t) srcframes / dstframes;
	unsigned dsttop = ndst / 2 - 1;
	unsigned srctop = BTSSCALE * nsrc / 2 - 1;
	//ARM_MORPH(arm_fill)(0, dst, ndst);
	for (unsigned srci = 0; srci <= srctop; ++ srci)
	{
		const unsigned dsti = srci * srctop / dsttop;
		dst [dsti * 2 + 0] = adpt_output(& btioadpt.adp, src [srci * 2 + 0]) * scale;	// получить sample
		dst [dsti * 2 + 1] = adpt_output(& btioadpt.adp, src [srci * 2 + 1]) * scale;	// получить sample

	}

	btin48k.release_buffer(addr);
	return true;
}

// Заполнение с ресэмплингом буфера данными из btout44p1k
// n - требуемое количество samples
// возвращает признак того, что данные в источнике есть
// btout44p1k -> resampler -> btout48k
static bool fetchdata_RS_btout44p1k(FLOAT_t * dst, unsigned ndst, unsigned ndstch, unsigned nsrc, unsigned nsrcch)
{
	btio44p1k_t * addr;
	if (! btout44p1k.get_readybuffer(& addr))
		return false;
	const int16_t * const src = addr->buff;

	const unsigned srcframes = nsrc / nsrcch;
	const unsigned dstframes = ndst / ndstch;
	const FLOAT_t scale = (FLOAT_t) srcframes / dstframes;
	unsigned dsttop = dstframes - 1;
	unsigned srctop = srcframes - 1;
	ASSERT(dstframes >= srcframes);
	ASSERT(nsrcch == 2);
	ASSERT(ndstch == 2);

	ARM_MORPH(arm_fill)(0, dst, ndst);
	for (unsigned dsti = 0; dsti <= dsttop; ++ dsti)
	{
		unsigned srci = dsti * srctop / dsttop;
		dst [dsti * 2 + 0] = adpt_input(& btioadpt.adp, src [srci * 2 + 0]) * scale;	// получить sample
		dst [dsti * 2 + 1] = adpt_input(& btioadpt.adp, src [srci * 2 + 1]) * scale;	// получить sample
//		dst [dsti * 2 + 0] = get_lout();
//		dst [dsti * 2 + 1] = get_rout();

	}
	ARM_MORPH(arm_biquad_cascade_stereo_df2T)(& fltout44p1k, dst, dst, dstframes);
	btout44p1k.release_buffer(addr);
	return true;
}

// Заполнение с ресэмплингом буфера данными из btout32k
// n - требуемое количество samples
// возвращает признак того, что данные в источнике есть
// btout32k -> resampler -> btout48k
static bool fetchdata_RS_btout32k(FLOAT_t * dst, unsigned ndst, unsigned ndstch, unsigned nsrc, unsigned nsrcch)
{
	btio32k_t * addr;
	if (! btout32k.get_readybuffer(& addr))
		return false;
	const int16_t * const src = addr->buff;

	const unsigned srcframes = nsrc / nsrcch;
	const unsigned dstframes = ndst / ndstch;
	const FLOAT_t scale = (FLOAT_t) srcframes / dstframes;
	unsigned dsttop = dstframes - 1;
	unsigned srctop = srcframes - 1;
	ASSERT(dstframes >= srcframes);
	ASSERT(nsrcch == 2);
	ASSERT(ndstch == 2);

	ARM_MORPH(arm_fill)(0, dst, ndst);
	for (unsigned dsti = 0; dsti <= dsttop; ++ dsti)
	{
		unsigned srci = dsti * srctop / dsttop;
		dst [dsti * 2 + 0] = adpt_input(& btioadpt.adp, src [srci * 2 + 0]) * scale;	// получить sample
		dst [dsti * 2 + 1] = adpt_input(& btioadpt.adp, src [srci * 2 + 1]) * scale;	// получить sample
//		dst [dsti * 2 + 0] = get_lout();
//		dst [dsti * 2 + 1] = get_rout();

	}
	ARM_MORPH(arm_biquad_cascade_stereo_df2T)(& fltout32k, dst, dst, dstframes);
	btout32k.release_buffer(addr);
	return true;
}

// Заполнение с ресэмплингом буфера данными из btout16k
// n - требуемое количество samples
// возвращает признак того, что данные в источнике есть
// btout16k -> resampler -> btout48k
static bool fetchdata_RS_btout16k(FLOAT_t * dst, unsigned ndst, unsigned ndstch, unsigned nsrc, unsigned nsrcch)
{
	btio16k_t * addr;
	if (! btout16k.get_readybuffer(& addr))
		return false;
	const int16_t * const src = addr->buff;

	const unsigned srcframes = nsrc / nsrcch;
	const unsigned dstframes = ndst / ndstch;
	const FLOAT_t scale = (FLOAT_t) srcframes / dstframes;
	unsigned dsttop = dstframes - 1;
	unsigned srctop = srcframes - 1;
	ASSERT(dstframes >= srcframes);
	ASSERT(nsrcch == 2);
	ASSERT(ndstch == 2);

	ARM_MORPH(arm_fill)(0, dst, ndst);
	for (unsigned dsti = 0; dsti <= dsttop; ++ dsti)
	{
		unsigned srci = dsti * srctop / dsttop;
		dst [dsti * 2 + 0] = adpt_input(& btioadpt.adp, src [srci * 2 + 0]) * scale;	// получить sample
		dst [dsti * 2 + 1] = adpt_input(& btioadpt.adp, src [srci * 2 + 1]) * scale;	// получить sample
//		dst [dsti * 2 + 0] = get_lout();
//		dst [dsti * 2 + 1] = get_rout();

	}
	ARM_MORPH(arm_biquad_cascade_stereo_df2T)(& fltout16k, dst, dst, dstframes);
	btout16k.release_buffer(addr);
	return true;
}

// Заполнение с ресэмплингом буфера данными из btout8k
// n - требуемое количество samples
// возвращает признак того, что данные в источнике есть
// btout8k -> resampler -> btout48k
static bool fetchdata_RS_btout8k(FLOAT_t * dst, unsigned ndst, unsigned ndstch, unsigned nsrc, unsigned nsrcch)
{
	btio8k_t * addr;
	if (! btout8k.get_readybuffer(& addr))
		return false;
	const int16_t * const src = addr->buff;

	const unsigned srcframes = nsrc / nsrcch;
	const unsigned dstframes = ndst / ndstch;
	const FLOAT_t scale = (FLOAT_t) srcframes / dstframes;
	unsigned dsttop = dstframes - 1;
	unsigned srctop = srcframes - 1;
	ASSERT(dstframes >= srcframes);
	ASSERT(nsrcch == 1);
	ASSERT(ndstch == 2);

	ARM_MORPH(arm_fill)(0, dst, ndst);
	for (unsigned dsti = 0; dsti <= dsttop; ++ dsti)
	{
		unsigned srci = dsti * srctop / dsttop;
		dst [dsti * 2 + 0] = adpt_input(& btioadpt.adp, src [srci * 2 + 0]) * scale;	// получить sample
		dst [dsti * 2 + 1] = adpt_input(& btioadpt.adp, src [srci * 2 + 0]) * scale;	// получить sample
//		dst [dsti * 2 + 0] = get_lout();
//		dst [dsti * 2 + 1] = get_rout();

	}
	ARM_MORPH(arm_biquad_cascade_stereo_df2T)(& fltout8k, dst, dst, dstframes);
	btout8k.release_buffer(addr);
	return true;
}

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned getcbf_dmabufferbtout48(FLOAT_t * b, FLOAT_t * dest)
{
	dest [0] = b [0];
	dest [1] = b [1];
	return 2;
}

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned getcbf_dmabufferbtin44p1(FLOAT_t * b, FLOAT_t * dest)
{
	dest [0] = b [0];
	dest [1] = b [1];
	return 2;
}

// Функции обмена звуком черз Bluetooth
// Resampling к/от 44.1 делается прозрачно

uint_fast8_t elfetch_dmabufferbtout48(FLOAT_t * dest)
{
	return btout48k.fetchdata_resample(dest, getcbf_dmabufferbtout48, fetchdata_RS_btout44p1k, ARRAY_SIZE(btio44p1k_t::buff), btio44p1k_t::nch);
	//return btout48k.fetchdata_resample(dest, getcbf_dmabufferbtout48, fetchdata_RS_btout32k, nsrc, btio32k_t::nch);
	//return btout48k.fetchdata_resample(dest, getcbf_dmabufferbtout48, fetchdata_RS_btout16k, nsrc, btio16k_t::nch);
	//return btout48k.fetchdata_resample(dest, getcbf_dmabufferbtout48, fetchdata_RS_btout8k, nsrc, btio8k_t::nch);
}

//uint_fast8_t elfetch_dmabufferbtin44p1(FLOAT_t * dest)
//{
//	return btout48k.fetchdata_resample(dest, getcbf_dmabufferbtin44p1, fetchdata_RS_btin48, ARRAY_SIZE(btio48k_t::buff), btio48k_t::nch);
//}

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned putcbf_dmabufferbtio48(FLOAT_t * b, FLOAT_t ch0, FLOAT_t ch1)
{
	b [0] = ch0;
	b [1] = ch1;
	return 2;
}

// Возвращает количество элементов буфера, обработанных за вызов
//static unsigned putcbf_dmabufferbtio44p1k(int16_t * b, int16_t ch0, int16_t ch1)
//{
//	b [0] = ch0;
//	b [1] = ch1;
//	return 2;
//}

void elfill_dmabufferbtin48(FLOAT_t ch0, FLOAT_t ch1)
{
	btin48k.savedata(ch0, ch1, putcbf_dmabufferbtio48);
}
///////
// can not be zero
uintptr_t allocate_dmabufferbtout44p1k(void)
{
	btio44p1k_t * dest;
	while (! btout44p1k.get_freebufferforced(& dest))
		ASSERT(0);
	return (uintptr_t) & dest->buff;
}

//void release_dmabufferbttout44p1(uintptr_t addr);
void save_dmabufferbtout44p1k(uintptr_t addr)
{
	btio44p1k_t * const p = CONTAINING_RECORD(addr, btio44p1k_t, buff);
	btout44p1k.save_buffer(p);
}

int_fast32_t datasize_dmabufferbtout44p1k(void)
{
	return btout44p1k.get_datasize();
}
///////
///

// can not be zero
uintptr_t allocate_dmabufferbtout32k(void)
{
	btio32k_t * dest;
	while (! btout32k.get_freebufferforced(& dest))
		ASSERT(0);
	return (uintptr_t) & dest->buff;
}

//void release_dmabufferbttout32(uintptr_t addr);
void save_dmabufferbtout32k(uintptr_t addr)
{
	btio32k_t * const p = CONTAINING_RECORD(addr, btio32k_t, buff);
	btout32k.save_buffer(p);
}

int_fast32_t datasize_dmabufferbtout32k(void)
{
	return btout32k.get_datasize();
}
/////////////
///

// can not be zero
uintptr_t allocate_dmabufferbtout16k(void)
{
	btio16k_t * dest;
	while (! btout16k.get_freebufferforced(& dest))
		ASSERT(0);
	return (uintptr_t) & dest->buff;
}

//void release_dmabufferbttout16(uintptr_t addr);
void save_dmabufferbtout16k(uintptr_t addr)
{
	btio16k_t * const p = CONTAINING_RECORD(addr, btio16k_t, buff);
	btout16k.save_buffer(p);
}

int_fast32_t datasize_dmabufferbtout16k(void)
{
	return btout16k.get_datasize();
}
/////////////
///

// can not be zero
uintptr_t allocate_dmabufferbtout8k(void)
{
	btio8k_t * dest;
	while (! btout8k.get_freebufferforced(& dest))
		ASSERT(0);
	return (uintptr_t) & dest->buff;
}

//void release_dmabufferbttout16(uintptr_t addr);
void save_dmabufferbtout8k(uintptr_t addr)
{
	btio8k_t * const p = CONTAINING_RECORD(addr, btio8k_t, buff);
	btout8k.save_buffer(p);
}

int_fast32_t datasize_dmabufferbtout8k(void)
{
	return btout8k.get_datasize();
}

///
///
// can be zero
uintptr_t getfilled_dmabufferbtin44p1k(void)
{
	btio44p1k_t * dest;
	if (! btin44p1k.get_readybuffer(& dest))
		return 0;
	return (uintptr_t) & dest->buff;
}

//void release_dmabufferbttout44p1(uintptr_t addr);
void release_dmabufferbtin44p1k(uintptr_t addr)
{
	btio44p1k_t * const p = CONTAINING_RECORD(addr, btio44p1k_t, buff);
	btin44p1k.release_buffer(p);
}

int_fast32_t datasize_dmabufferbtin44p1k(void)
{
	return btin44p1k.get_datasize();
}
///////
///

// can be zero
uintptr_t getfilled_dmabufferbtin32k(void)
{
	btio32k_t * dest;
	if (! btin32k.get_readybuffer(& dest))
		return 0;
	return (uintptr_t) & dest->buff;
}

//void release_dmabufferbttout32(uintptr_t addr);
void release_dmabufferbtin32k(uintptr_t addr)
{
	btio32k_t * const p = CONTAINING_RECORD(addr, btio32k_t, buff);
	btin32k.release_buffer(p);
}

int_fast32_t datasize_dmabufferbtin32k(void)
{
	return btin32k.get_datasize();
}
/////////////
///

// can be zero
uintptr_t getfilled_dmabufferbtin16k(void)
{
	btio16k_t * dest;
	if (! btin16k.get_readybuffer(& dest))
		return 0;
	return (uintptr_t) & dest->buff;
}

//void release_dmabufferbttout16(uintptr_t addr);
void release_dmabufferbtin16k(uintptr_t addr)
{
	btio16k_t * const p = CONTAINING_RECORD(addr, btio16k_t, buff);
	btin16k.release_buffer(p);
}

int_fast32_t datasize_dmabufferbtin16k(void)
{
	return btin16k.get_datasize();
}
/////////////
///

// can be zero
uintptr_t getfilled_dmabufferbtin8k(void)
{
	btio8k_t * dest;
	if (! btin8k.get_readybuffer(& dest))
		return 0;
	return (uintptr_t) & dest->buff;
}

//void release_dmabufferbttout16(uintptr_t addr);
void release_dmabufferbtin8k(uintptr_t addr)
{
	btio8k_t * const p = CONTAINING_RECORD(addr, btio8k_t, buff);
	btin8k.release_buffer(p);
}

int_fast32_t datasize_dmabufferbtin8k(void)
{
	return btin8k.get_datasize();
}

///////////////////////////////
///

#endif /* WITHUSEUSBBT */
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

#if WITHRTS192 || WITHRTS96

	typedef struct
	{
		uacintag_t tag;
		ALIGNX_BEGIN  uint8_t buff [UAC_GROUPING_DMAC * UACIN_RTS192_DATASIZE_DMAC] ALIGNX_END;
		ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
		enum { ss = UACIN_RTS192_SAMPLEBYTES, nch = UACIN_FMT_CHANNELS_RTS192 };	// resampling support
	} uacinrts192_t;

	typedef buffitem<uacinrts192_t> uacinrts192buf_t;
	static RAMNC uacinrts192buf_t uacinrts192buf [UACINRTS192_CAPACITY];

	typedef dmahandle<int_fast32_t, uacinrts192buf_t, 1, 1> uacinrts192dma_t;

	typedef adapters<int_fast32_t, (int) UACIN_RTS192_SAMPLEBYTES, (int) UACIN_FMT_CHANNELS_RTS192> uacinrts192adpt_t;

	static uacinrts192dma_t uacinrts192(IRQL_REALTIME, "uacin192", uacinrts192buf, ARRAY_SIZE(uacinrts192buf));
	static uacinrts192adpt_t uacinrts192adpt(UACIN_RTS192_SAMPLEBYTES * 8, 0, "uacin192");

	int_fast32_t cachesize_dmabufferuacinrts192(void)
	{
		return uacinrts192.get_cachesize();
	}

	int_fast32_t datasize_dmabufferuacinrts192(void)
	{
		return uacinrts192.get_datasize();
	}

	// Возвращает количество элементов буфера, обработанных за вызов
	static unsigned putcbf_dmabufferuacinrts192(uint8_t * b, int_fast32_t ch0, int_fast32_t ch1)
	{
		return uacinrts192adpt.poketransf_LE(& if2rts192out, b, ch0, ch1);
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

//#elif WITHRTS96

	typedef struct
	{
		uacintag_t tag;
		ALIGNX_BEGIN  uint8_t buff [UAC_GROUPING_DMAC * UACIN_RTS96_DATASIZE_DMAC] ALIGNX_END;
		ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
		enum { ss = UACIN_RTS96_SAMPLEBYTES, nch = UACIN_FMT_CHANNELS_RTS96 };	// resampling support
	} uacinrts96_t;

	typedef buffitem<uacinrts96_t> uacinrts96buf_t;
	static RAMNC uacinrts96buf_t uacinrts96buf [UACINRTS96_CAPACITY];

	typedef dmahandle<int_fast32_t, uacinrts96buf_t, 1, 1> uacinrts96dma_t;

	typedef adapters<int_fast32_t, (int) UACIN_RTS96_SAMPLEBYTES, (int) UACIN_FMT_CHANNELS_RTS96> uacinrts96adpt_t;

	static uacinrts96dma_t uacinrts96(IRQL_REALTIME, "uacin96", uacinrts96buf, ARRAY_SIZE(uacinrts96buf));
	static uacinrts96adpt_t uacinrts96adpt(UACIN_RTS96_SAMPLEBYTES * 8, 0, "uacin96");

	int_fast32_t cachesize_dmabufferuacinrts96(void)
	{
		return uacinrts96.get_cachesize();
	}

	int_fast32_t datasize_dmabufferuacinrts96(void)
	{
		return uacinrts96.get_datasize();
	}

	// Возвращает количество элементов буфера, обработанных за вызов
	static unsigned putcbf_dmabufferuacinrts96(uint8_t * b, int_fast32_t ch0, int_fast32_t ch1)
	{
		return uacinrts96adpt.poketransf_LE(& if2rts96out, b, ch0, ch1);
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
	ALIGNX_BEGIN  uint8_t buff [UAC_GROUPING_DMAC * UACIN_AUDIO48_DATASIZE_DMAC] ALIGNX_END;
	ALIGNX_BEGIN  uint8_t pad ALIGNX_END;	// для вычисления размера требуемого для операций с кеш памятью
	enum { ss = UACIN_AUDIO48_SAMPLEBYTES, nch = UACIN_FMT_CHANNELS_AUDIO48 };
} uacin48_t;

typedef buffitem<uacin48_t> uacin48buf_t;

static RAMNC uacin48buf_t uacin48buf [UACIN48_CAPACITY];

typedef dmahandle<FLOAT_t, uacin48buf_t, 1, 1> uacin48dma_t;


typedef adapters<FLOAT_t, (int) UACIN_AUDIO48_SAMPLEBYTES, (int) UACIN_FMT_CHANNELS_AUDIO48> uacin48adpt_t;

static uacin48adpt_t uacin48adpt(UACIN_AUDIO48_SAMPLEBYTES * 8, 0, "uacin48");

static uacin48dma_t uacin48(IRQL_REALTIME, "uacin48", uacin48buf, ARRAY_SIZE(uacin48buf));

// Возвращает количество элементов буфера, обработанных за вызов
static unsigned uacin48_putcbf(uint8_t * b, FLOAT_t ch0, FLOAT_t ch1)
{
	return uacin48adpt.poke_LE(b, ch0, ch1);
}

int_fast32_t cachesize_dmabufferuacin48(void)
{
	return uacin48.get_cachesize();

}

int_fast32_t datasize_dmabufferuacin48(void)
{
	return uacin48.get_datasize();

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
	if (uacin48.get_readybuffer(& dest) || uacin48.get_freebufferforced(& dest))
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
#if WITHUSEUSBBT
	extern int glob_btenable;
	if (glob_btenable)
	{
		return elfetch_dmabufferbtout48(v->ivqv);
	}
#endif /* WITHUSEUSBBT */
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
typedef blists<message8v_t, 0, 0> message8list_t;

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
typedef dmahandle<FLOAT_t, recordswav48buf_t, 0, 0> recordswav48dma_t;

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
#if WITHUSBUACIN
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
#endif /* WITHUSBUACIN */
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

#if WITHRTS192 && 0
// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки - отдельный канал спектроанализатора
void RAMFUNC save_dmabuffer32rts192(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n4;
#endif /* WITHBUFFERSDEBUG */
	voice32rts192_t * const p = CONTAINING_RECORD(addr, voice32rts192_t, u.buff);

	unsigned i;
	for (i = 0; i < DMABUFFSIZE32RTS192; i += DMABUFFSTEP32RTS192)
	{
		const int32_t * const b = (const int32_t *) & p->u.buff [i];

        //saveIQRTSxx(NULL, b [0], b [1]);
        deliveryint(& rtstargetsint, b [0], b [1]);
	}

	release_dmabuffer32rts192(addr);
}
#endif /* WITHRTS192 */

#if WITHDSPEXTDDC && WITHRTS96
// использование данных о спектре, передаваемых в общем фрейме
static void RAMFUNC
saverts96pair(const IFADCvalue_t * buff)
{
	// формирование отображения спектра
	// если используется конвертор на Rafael Micro R820T - требуется инверсия спектра
	if (glob_swaprts != 0)
	{
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RXRTS0Q],	// previous
			buff [DMABUF32RXRTS0I]
			);
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RXRTS1Q],	// current
			buff [DMABUF32RXRTS1I]
			);
	}
	else
	{
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RXRTS0I],	// previous
			buff [DMABUF32RXRTS0Q]
			);
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RXRTS1I],	// current
			buff [DMABUF32RXRTS1Q]
			);
	}
}

#endif /* WITHDSPEXTDDC && WITHRTS96 */

#if WITHDSPEXTDDC && WITHRTS192
// использование данных о спектре, передаваемых в общем фрейме
static void RAMFUNC
saverts192quad(const IFADCvalue_t * buff)
{
	// формирование отображения спектра
	// если используется конвертор на Rafael Micro R820T - требуется инверсия спектра
	if (glob_swaprts != 0)
	{
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RXRTS0Q],	// previous
			buff [DMABUF32RXRTS0I]
			);
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RXRTS1Q],	// previous
			buff [DMABUF32RXRTS1I]
			);
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RXRTS2Q],	// previous
			buff [DMABUF32RXRTS2I]
			);
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RXRTS3Q],	// current
			buff [DMABUF32RXRTS3I]
			);
	}
	else
	{
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RXRTS0I],	// previous
			buff [DMABUF32RXRTS0Q]
			);
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RXRTS1I],	// previous
			buff [DMABUF32RXRTS1Q]
			);
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RXRTS2I],	// previous
			buff [DMABUF32RXRTS2Q]
			);
		deliveryint(
			& rtstargetsint,
			buff [DMABUF32RXRTS3I],	// current
			buff [DMABUF32RXRTS3Q]
			);
	}
}
#endif /* WITHDSPEXTDDC && WITHRTS192 */

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

void process_dmabuffer32rx(const IFADCvalue_t * buff)
{
	unsigned i;
	for (i = 0; i < DMABUFFSIZE32RX; i += DMABUFFSTEP32RX)
	{
		const IFADCvalue_t * const b = buff + i;
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
		uint_fast8_t slot = DMABUF32RXRTS0I;	// slot 4
		validateSeq(slot, b [slot], b);
	}
#endif
#if 0
		// Тестирование - заменить приянтые квадратуры синтезированными
		inject_testsignals(b);
#endif
#if WITHRTS96
		saverts96pair(b);	// использование данных о спектре, передаваемых в общем фрейме
#elif WITHRTS192
		saverts192quad(b);	// использование данных о спектре, передаваемых в общем фрейме
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

uint_fast8_t buffers_get_uacoutactive(void)
{
	return uacoutalt != UACOUTALT_NONE;
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
		* sizep = datasize_dmabufferuacin48();
		return getfilled_dmabufferuacin48();

#if ! WITHUSBUACIN2

#if WITHRTS96
	case UACINALT_RTS96:
		* sizep = datasize_dmabufferuacinrts96();
		return getfilled_dmabufferuacinrts96();
#endif /* WITHRTS192 */

#if WITHRTS192
	case UACINALT_RTS192:
		* sizep = datasize_dmabufferuacinrts192();
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
		* sizep = datasize_dmabufferuacinrts96();
		return getfilled_dmabufferuacinrts96();
#endif /* WITHRTS192 */

#if WITHRTS192
	case UACINRTSALT_RTS192:
		* sizep = datasize_dmabufferuacinrts192();
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

	IRQLSPIN_LOCK(& list->listlock, & oldIrql, list->irql);
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

	IRQLSPIN_LOCK(& list->listlock, & oldIrql, list->irql);
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

	IRQLSPIN_LOCK(& list->listlock, & oldIrql, list->irql);
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
	IRQLSPIN_LOCK(& list->listlock, & oldIrql, list->irql);
	InsertHeadList(& list->head, & target->item);
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void unsubscribefloat(deliverylist_t * list, subscribefloat_t * target)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& list->listlock, & oldIrql, list->irql);
	RemoveEntryList(& target->item);
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void subscribeint32(deliverylist_t * list, subscribeint32_t * target, void * ctx, void (* pfn)(void * ctx, int_fast32_t ch0, int_fast32_t ch1))
{
	IRQL_t oldIrql;

	target->cb = pfn;
	target->ctx = ctx;
	IRQLSPIN_LOCK(& list->listlock, & oldIrql, list->irql);
	InsertHeadList(& list->head, & target->item);
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void unsubscribeint32(deliverylist_t * list, subscribeint32_t * target)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& list->listlock, & oldIrql, list->irql);
	RemoveEntryList(& target->item);
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void deliverylist_initialize(deliverylist_t * list, IRQL_t irqlv)
{
	InitializeListHead(& list->head);
	list->irql = irqlv;
	IRQLSPINLOCK_INITIALIZE(& list->listlock);
}

#endif /* WITHINTEGRATEDDSP */


#if WITHBUFFERSDEBUG

void buffers_diagnostics(void)
{
#if WITHUSEUSBBT
	btout44p1k.debug();
	btout32k.debug();
	btout16k.debug();
	btout8k.debug();
#endif
#if WITHINTEGRATEDDSP
#if 1
	denoise16list.debug();
	codec16rx.debug();
	codec16tx.debug();
	moni16.debug();
	voice32tx.debug();
	voice32rx.debug();
#endif
#if 0
	// USB
#if WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE) && 0
	uacout48.debug();
#endif
#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) && 0
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
#if WITHUSEUSBBT
	btout44p1k.spool10ms();
	btout32k.spool10ms();
	btout16k.spool10ms();
	btout8k.spool10ms();
#endif
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
#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)
	uacin48.spool10ms();
#endif /* WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) */
	//message8.spool10ms();
#endif /* WITHINTEGRATEDDSP */
}

#else /* WITHBUFFERSDEBUG */

void buffers_diagnostics(void)
{
}

#endif /* WITHBUFFERSDEBUG */

#if LCDMODE_LTDC && WITHLTDCHW

// работа с видеобуферами

/* Frame buffer for display 0 */
typedef ALIGNX_BEGIN struct colmain0fb
{
	ALIGNX_BEGIN PACKEDCOLORPIP_T buff [GXSIZE(DIM_X, DIM_Y)] ALIGNX_END;
	ALIGNX_BEGIN uint8_t pad ALIGNX_END;
} ALIGNX_END colmain0fb_t;

typedef buffitem<colmain0fb_t> colmain0fbbuf_t;

static RAMFRAMEBUFF colmain0fbbuf_t colmain0fbbuf [LCDMODE_MAIN_PAGES];
typedef blists<colmain0fbbuf_t, 0, 0> colmain0fblist_t;
static colmain0fblist_t colmain0fblist(IRQL_OVERREALTIME, "fb0", colmain0fbbuf, ARRAY_SIZE(colmain0fbbuf));

uintptr_t allocate_dmabuffercolmain0fb(void) /* take free buffer Frame buffer for display 0 */
{
	colmain0fb_t * dest;
	// если нет свободного - берём из очереди готовых к отображению - он уже не нужен, будет новое изображение
	while (! colmain0fblist.get_freebuffer_raw(& dest) && ! colmain0fblist.get_readybuffer_raw(& dest))
		ASSERT(0);
	return (uintptr_t) dest->buff;
}

uintptr_t getfilled_dmabuffercolmain0fb(void) /* take from queue Frame buffer for display 0 */
{
	colmain0fb_t * dest;
	if (! colmain0fblist.get_readybuffer_raw(& dest))
		return 0;
	return (uintptr_t) dest->buff;
}

void release_dmabuffercolmain0fb(uintptr_t addr)  /* release Frame buffer for display 0 */
{
	colmain0fb_t * const p = CONTAINING_RECORD(addr, colmain0fb_t, buff);
	colmain0fblist.release_buffer(p);
}

void save_dmabuffercolmain0fb(uintptr_t addr) /* save to queue Frame buffer for display 0 */
{
	// поддерживаем один элемент в очереди готовых
	colmain0fb_t * old;
	if (colmain0fblist.get_readybuffer_raw(& old))
		colmain0fblist.release_buffer(old);
	colmain0fb_t * const p = CONTAINING_RECORD(addr, colmain0fb_t, buff);
	colmain0fblist.save_buffer(p);
}

int_fast32_t cachesize_dmabuffercolmain0fb(void) /* parameter for cache manipulation functions Frame buffer for display 0 */
{
	return colmain0fblist.get_cachesize();
}

int_fast32_t datasize_dmabuffercolmain0fb(void) /* parameter for DMA Frame buffer for display 0 */
{
	return colmain0fblist.get_datasize();
}

#if WITHLTDCHWVBLANKIRQ

static uintptr_t fb0;
static uintptr_t lastsetfb0;

PACKEDCOLORPIP_T *
colmain_fb_draw(void)
{
	if (fb0 == 0)
	{
		fb0 = allocate_dmabuffercolmain0fb();
	}
	return (PACKEDCOLORPIP_T *) fb0;
}

/* поставить на отображение этот буфер, запросить следующий */
void colmain_nextfb(void)
{
	if (fb0 != 0)
	{
	//	char s [32];
	//	local_snprintf_P(s, 32, "F=%08lX", (unsigned long) fb0);
	//	display_at(0, 0, s);
		dcache_clean_invalidate(fb0, cachesize_dmabuffercolmain0fb());
		save_dmabuffercolmain0fb(fb0);
	}
	fb0 = allocate_dmabuffercolmain0fb();
#if WITHOPENVG
	openvg_next(colmain_getindexbyaddr(colmain_fb_draw()));
#endif /* WITHOPENVG */
}

// Update framebuffer if needed
void hardware_ltdc_vblank(unsigned ix)
{
	const uintptr_t p1 = getfilled_dmabuffercolmain0fb();
	if (p1 != 0)
	{
		if (lastsetfb0 != 0)
		{
			release_dmabuffercolmain0fb(lastsetfb0);
		}
		lastsetfb0 = p1;

		hardware_ltdc_main_set_no_vsync(p1);
	}
}

#else /* WITHLTDCHWVBLANKIRQ */

static uint_fast8_t drawframe;

PACKEDCOLORPIP_T *
colmain_fb_draw(void)
{
	return colmain0fbbuf [drawframe].v.buff;
}

/* поставить на отображение этот буфер, запросить следующий */
void colmain_nextfb(void)
{
	const uintptr_t frame = (uintptr_t) colmain_fb_draw();
	dcache_clean_invalidate(frame, cachesize_dmabuffercolmain0fb());
	hardware_ltdc_main_set(frame);
	drawframe = (drawframe + 1) % LCDMODE_MAIN_PAGES;	// переключиться на использование для DRAW следующего фреймбуфера
#if WITHOPENVG
	openvg_next(colmain_getindexbyaddr());
#endif /* WITHOPENVG */
}

#endif /* WITHLTDCHWVBLANKIRQ */

// Вспомогательная функция - для систем где видеоконтроллер работает со своим массивом видеобуферов
// получить индекс видеобуфера по его адресу
uint_fast8_t colmain_getindexbyaddr(uintptr_t addr)
{
	uint_fast8_t i;
	for (i = 0; i < LCDMODE_MAIN_PAGES; ++ i)
	{
		if ((uintptr_t) colmain0fbbuf [i].v.buff == addr)
			return i;
	}
	ASSERT(0);
	return 0;
}

// Вспомогательная функция - для систем где видеоконтроллер работает со своим массивом видеобуферов
// получение массива планирующихся для работы framebuffers
void colmain_fb_list(uintptr_t * frames)
{
	unsigned i;
	for (i = 0; i < LCDMODE_MAIN_PAGES; ++ i)
	{
		frames [i] = (uintptr_t) colmain0fbbuf [i].v.buff;
	}
}

#endif /* LCDMODE_LTDC && WITHLTDCHW */

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
//	ASSERT((DMABUFFSIZE32RTS192 % HARDWARE_RTSDMABYTES) == 0);
//	ASSERT((DMABUFFSIZE96RTS % HARDWARE_RTSDMABYTES) == 0);

#if WITHRTS192 && WITHUSBUACIN && WITHUSBHW && defined (WITHUSBHW_DEVICE)

	subscribeint32(& rtstargetsint, & uacinrtssubscribe, NULL, savesampleout192stereo);

#elif WITHRTS96 && WITHUSBUACIN && WITHUSBHW && defined (WITHUSBHW_DEVICE)

	subscribeint32(& rtstargetsint, & uacinrtssubscribe, NULL, savesampleout96stereo);

#endif /* WITHRTS192 */

#if WITHUSEUSBBT


	#define BTAUDIO_LPF_STAGES 2
	iir_filter_t f0;
	const FLOAT_t samplerate = ARMI2SRATE;	// 48 kHz
	{
		// 44.1 kHs -> 48 kHz
		static FLOAT_t state [4 * BTAUDIO_LPF_STAGES];	// state buffer and size is always 4 * numStages
		static FLOAT_t coeffs [BIQUAD_COEFF_IN_STAGE * BTAUDIO_LPF_STAGES];

		biquad_create(& f0, BTAUDIO_LPF_STAGES);
		biquad_init_lowpass(& f0, samplerate, 44100 / 2);
		fill_biquad_coeffs(& f0, coeffs, BTAUDIO_LPF_STAGES);
		ARM_MORPH(arm_biquad_cascade_stereo_df2T_init)(& fltout44p1k, BTAUDIO_LPF_STAGES, coeffs, state);
	}
	{
		// 32 kHs -> 48 kHz
		static FLOAT_t state [4 * BTAUDIO_LPF_STAGES];	// state buffer and size is always 4 * numStages
		static FLOAT_t coeffs [BIQUAD_COEFF_IN_STAGE * BTAUDIO_LPF_STAGES];

		biquad_create(& f0, BTAUDIO_LPF_STAGES);
		biquad_init_lowpass(& f0, samplerate, 32000 / 2);
		fill_biquad_coeffs(& f0, coeffs, BTAUDIO_LPF_STAGES);
		ARM_MORPH(arm_biquad_cascade_stereo_df2T_init)(& fltout32k, BTAUDIO_LPF_STAGES, coeffs, state);
	}
	{
		// 16 kHs -> 48 kHz
		static FLOAT_t state [4 * BTAUDIO_LPF_STAGES];	// state buffer and size is always 4 * numStages
		static FLOAT_t coeffs [BIQUAD_COEFF_IN_STAGE * BTAUDIO_LPF_STAGES];

		biquad_create(& f0, BTAUDIO_LPF_STAGES);
		biquad_init_lowpass(& f0, samplerate, 16000 / 2);
		fill_biquad_coeffs(& f0, coeffs, BTAUDIO_LPF_STAGES);
		ARM_MORPH(arm_biquad_cascade_stereo_df2T_init)(& fltout16k, BTAUDIO_LPF_STAGES, coeffs, state);
	}
	{
		// 8 kHs -> 48 kHz
		static FLOAT_t state [4 * BTAUDIO_LPF_STAGES];	// state buffer and size is always 4 * numStages
		static FLOAT_t coeffs [BIQUAD_COEFF_IN_STAGE * BTAUDIO_LPF_STAGES];

		biquad_create(& f0, BTAUDIO_LPF_STAGES);
		biquad_init_lowpass(& f0, samplerate, 8000 / 2);
		fill_biquad_coeffs(& f0, coeffs, BTAUDIO_LPF_STAGES);
		ARM_MORPH(arm_biquad_cascade_stereo_df2T_init)(& fltout8k, BTAUDIO_LPF_STAGES, coeffs, state);
	}

#endif /* WITHUSEUSBBT */

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
