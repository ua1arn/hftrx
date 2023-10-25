/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints

#include "mslist.h"
#include "audio.h"
#include "buffers.h"


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
		void * tag2;
		ALIGNX_BEGIN  element_t v ALIGNX_END;
		ALIGNX_BEGIN  LIST_ENTRY item ALIGNX_END;	// should be placed after 'u' field
		void * tag3;
	} buffitem_t;

	LIST_ENTRY freelist;
	LIST_ENTRY readylist;
	IRQL_t irql;
	LCLSPINLOCK_t lock;
	buffitem_t storage [capacity];

public:
	blists(IRQL_t airql) : irql(airql)
	{
		unsigned i;

		InitializeListHead(& freelist);
		InitializeListHead(& readylist);
		LCLSPINLOCK_INITIALIZE(& lock);
		for (i = 0; i < capacity; ++ i)
		{
			buffitem_t * const p = & storage [i];
			//p->tag = BUFFTAG_RTS192;
			p->tag2 = p;
			p->tag3 = p;
			InsertHeadList(& freelist, & p->item);
		}
		PRINTF("blists %u %u\n", sizeof (element_t), capacity);
	}

	// сохранить в списке свободных
	void release_buffer(element_t * addr)
	{
		buffitem_t * const p = CONTAINING_RECORD(addr, buffitem_t, v);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		IRQL_t oldIrql;
		RiseIrql(irql, & oldIrql);
		LCLSPIN_LOCK(& lock);

		InsertHeadList(& freelist, & p->item);

		LCLSPIN_UNLOCK(& lock);
		LowerIrql(oldIrql);
	}

	// сохранить в списке готовых
	void save_buffer(element_t * addr)
	{
		buffitem_t * const p = CONTAINING_RECORD(addr, buffitem_t, v);
		ASSERT(p->tag2 == p);
		ASSERT(p->tag3 == p);
		IRQL_t oldIrql;
		RiseIrql(irql, & oldIrql);
		LCLSPIN_LOCK(& lock);

		InsertHeadList(& readylist, & p->item);

		LCLSPIN_UNLOCK(& lock);
		LowerIrql(oldIrql);
	}

	// получить из списка готовых
	int get_readybuffer(element_t * * dest)
	{
		IRQL_t oldIrql;
		RiseIrql(irql, & oldIrql);
		LCLSPIN_LOCK(& lock);
		if (! IsListEmpty(& readylist))
		{
			const PLIST_ENTRY t = RemoveTailList(& readylist);
			LCLSPIN_UNLOCK(& lock);
			LowerIrql(oldIrql);
			buffitem_t * const p = CONTAINING_RECORD(t, buffitem_t, item);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			* dest = & p->v;
			return 1;
		}
		LCLSPIN_UNLOCK(& lock);
		LowerIrql(oldIrql);
		return 0;
	}

	// получить из списка свободных
	int get_freebuffer(element_t * * dest)
	{
		IRQL_t oldIrql;
		RiseIrql(irql, & oldIrql);
		LCLSPIN_LOCK(& lock);
		if (! IsListEmpty(& freelist))
		{
			const PLIST_ENTRY t = RemoveTailList(& freelist);
			LCLSPIN_UNLOCK(& lock);
			LowerIrql(oldIrql);
			buffitem_t * const p = CONTAINING_RECORD(t, buffitem_t, item);
			ASSERT(p->tag2 == p);
			ASSERT(p->tag3 == p);
			* dest = & p->v;
			return 1;
		}
		LCLSPIN_UNLOCK(& lock);
		LowerIrql(oldIrql);
		return 0;
	}
};

#if defined(WITHRTS96) && defined(WITHRTS192)
	#error Configuration Error: WITHRTS96 and WITHRTS192 can not be used together
#endif /* defined(WITHRTS96) && defined(WITHRTS192) */


#if WITHINTEGRATEDDSP

#if 0
// Denoise operations
// буферы: один заполняется, один воспроизводлится и два свободных (с одинм бывают пропуски).
typedef blists<speexel_t, NTRX * FIRBUFSIZE, 5> denoise16list_t;
static denoise16list_t denoise16list(IRQL_REALTIME);

// получить готоввый
extern "C" uint_fast8_t takespeexready(speexel_t * * dest)
{
	speexel_t * addr;
	if (denoise16list.get_readybuffer(& addr))
	{
		* dest = addr;
		return 1;
	}
	return 0;
}

// получить свободный
extern "C" speexel_t * allocatespeexbuffer(void)
{
	speexel_t * addr;
	if (denoise16list.get_freebuffer(& addr))
	{
		return addr;
	}
	if (denoise16list.get_readybuffer(& addr))
	{
		return addr;
	}
	return NULL;
}

// Освобождение буфера
extern "C" void releasespeexbuffer(speexel_t * t)
{
	denoise16list.release_buffer(t);
}

// сохранение длч обработки буфера
extern "C" void savespeexbuffer(speexel_t * t)
{
	denoise16list.save_buffer(t);
}

#endif

#if 0
// Audio CODEC in/out

typedef blists<aubufv_t, DMABUFFSIZE16RX, 20> voice16rxlist_t;
static voice16rxlist_t voice16rxlist(IRQL_REALTIME);

extern "C" int_fast32_t buffers_dmabuffer16rxcachesize(void)
{
	return voice16rxlist.get_cachesize();
}

extern "C" uintptr_t allocate_dmabuffer16rx(void)
{
	aubufv_t * dest;
	while (voice16rxlist.get_freebuffer(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest;
}
#endif

#if 0
// Audio CODEC in/out

typedef blists<aubufv_t, DMABUFFSIZE16TX, 20> voice16txlist_t;
static voice16txlist_t voice16txlist(IRQL_REALTIME);

extern "C" int_fast32_t buffers_dmabuffer16txcachesize(void)
{
	return voice16txlist.get_cachesize();
}

extern "C" uintptr_t allocate_dmabuffer16tx(void)
{
	aubufv_t * dest;
	while (voice16txlist.get_freebuffer(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest;
}
#endif

#if 0
// I/Q data to FPGA or IF CODEC

typedef blists<IFDACvalue_t, DMABUFFSIZE32TX, 20> voice32txlist_t;
static voice32txlist_t voice32txlist(IRQL_REALTIME);

extern "C" int_fast32_t buffers_dmabuffer32txcachesize(void)
{
	return voice32txlist.get_cachesize();
}

#endif

#if 0
// I/Q data from FPGA or IF CODEC

typedef blists<IFADCvalue_t, DMABUFFSIZE32RX, 20> voice32rxlist_t;
static voice32rxlist_t voice32rxlist(IRQL_REALTIME);

extern "C" int_fast32_t buffers_dmabuffer32rxcachesize(void)
{
	return voice32rxlist.get_cachesize();
}
#endif

#if 0
// I/Q SPECTRUM data from FPGA or IF CODEC

typedef blists<IFADCvalue_t, DMABUFFSIZE32RTS, 20> voice32rtslist_t;
static voice32rtslist_t voice32rtslist(IRQL_REALTIME);

extern "C" int_fast32_t buffers_dmabuffer32rtscachesize(void)
{
	return voice32rtslist.get_cachesize();
}
#endif

#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) && 0

// USB AUDIO IN

typedef blists<uint8_t, UACIN_AUDIO48_DATASIZE_DMAC, 20> uacin48list_t;
static uacin48list_t uacin48list(IRQL_REALTIME);

extern "C" int_fast32_t buffers_dmabufferuacin48cachesize(void)
{
	return uacin48list.get_cachesize();
}

#endif /* WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) */

#if WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE) && 1

// USB AUDIO OUT

typedef struct
{
	ALIGNX_BEGIN  uint8_t buff [UACOUT_AUDIO48_DATASIZE_DMAC] ALIGNX_END;
	ALIGNX_BEGIN  uint8_t pad ALIGNX_END;
} uacout48buff_t;

typedef blists<uacout48buff_t, 4> uacout48list_t;
static uacout48list_t uacout48list(IRQL_REALTIME);

extern "C" int_fast32_t buffers_dmabufferuacout48cachesize(void)
{
	return offsetof(uacout48buff_t, pad) - offsetof(uacout48buff_t, buff);
}

extern "C"
uintptr_t allocate_dmabufferuacout48(void)
{
	uacout48buff_t * dest;
	while (uacout48list.get_freebuffer(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) & dest->buff;
}

extern "C"
void release_dmabufferuacout48(uintptr_t addr)
{
	uacout48buff_t * const p = CONTAINING_RECORD(addr, uacout48buff_t, buff);
	uacout48list.release_buffer(p);
}

extern "C"
void processing_dmabufferuacout48(uintptr_t addr)
{
	uacout48buff_t * const p = CONTAINING_RECORD(addr, uacout48buff_t, buff);
	uacout_buffer_save(p->buff, UACOUT_AUDIO48_DATASIZE_DMAC, UACOUT_FMT_CHANNELS_AUDIO48, UACOUT_AUDIO48_SAMPLEBYTES);

	release_dmabufferuacout48(addr);
}

#endif /* WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE) */

enum
{
	BUFFTAG_UACIN48 = 44,
	BUFFTAG_RTS192,
	BUFFTAG_RTS96,
	BUFFTAG_total
};

#if 0

// USB AUDIO RTS IN

#if WITHRTS192

	typedef struct
	{
		ALIGNX_BEGIN  uint8_t buff [UACIN_RTS192_DATASIZE_DMAC] ALIGNX_END;
		ALIGNX_BEGIN  uint8_t pad ALIGNX_END;
		unsigned tag;
	} voice192rtsbuff_t;
	typedef blists<uint8_t, UACIN_RTS192_DATASIZE_DMAC, 20> voice192rtslist_t;
	static voice192rtslist_t voice192rtslist(IRQL_REALTIME);

	extern "C" int_fast32_t buffers_dmabuffer192rtscachesize(void)
	{
		return voice192rtslist.get_cachesize();
	}

#elif WITHRTS96

	typedef blists<uint8_t, UACIN_RTS96_DATASIZE_DMAC, 20> voice96rtslist_t;
	static voice96rtslist_t voice96rtslist(IRQL_REALTIME);

	extern "C" int_fast32_t buffers_dmabuffer96rtscachesize(void)
	{
		return voice96rtslist.get_cachesize();
	}

#endif /* WITHRTS96 */

#endif


extern "C" void buffers2_initialize(void)
{
}

#else  /* WITHINTEGRATEDDSP */

// stub function
extern "C" void buffers2_initialize(void)
{
}

#endif /* WITHINTEGRATEDDSP */
