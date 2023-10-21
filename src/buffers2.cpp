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

template <typename element, unsigned elements, unsigned capacity>
class blists
{
	typedef struct buffitem
	{
		void * tag2;
		ALIGNX_BEGIN union
		{
			element buff [elements];
			uint8_t filler [EP_align(elements * sizeof (element), DCACHEROWSIZE)];
		} u ALIGNX_END;
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
		PRINTF("Buffer %u %u %u\n", sizeof (element), elements, capacity);
	}

	static int_fast32_t get_cachesize()
	{
		return offsetof(buffitem_t, item) - offsetof(buffitem_t, u);
	}

	// сохранить в списке свободных
	void release_buffer(uintptr_t addr)
	{
		buffitem_t * const p = CONTAINING_RECORD(addr, buffitem_t, u.buff);
		IRQL_t oldIrql;
		RiseIrql(irql, & oldIrql);
		LCLSPIN_LOCK(& lock);

		InsertHeadList(& freelist, & p->item);

		LCLSPIN_UNLOCK(& lock);
		LowerIrql(oldIrql);
	}

	// сохранить в списке готовых
	void save_buffer(uintptr_t addr)
	{
		buffitem_t * const p = CONTAINING_RECORD(addr, buffitem_t, u.buff);
		IRQL_t oldIrql;
		RiseIrql(irql, & oldIrql);
		LCLSPIN_LOCK(& lock);

		InsertHeadList(& readylist, & p->item);

		LCLSPIN_UNLOCK(& lock);
		LowerIrql(oldIrql);
	}

	// получить из списка готовых
	int get_readybuffer(uintptr_t * dest)
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
			* dest = (uintptr_t) p->u.buff;
			return 1;
		}
		LCLSPIN_UNLOCK(& lock);
		LowerIrql(oldIrql);
		return 0;
	}

	// получить из списка свободных
	int get_freebuffer(uintptr_t * dest)
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
			* dest = (uintptr_t) p->u.buff;
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
enum
{
	BUFFTAG_UACIN48 = 44,
	BUFFTAG_RTS192,
	BUFFTAG_RTS96,
	BUFFTAG_total
};


#if WITHINTEGRATEDDSP

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
	uintptr_t dest;
	while (voice16rxlist.get_freebuffer(& dest) == 0)
		ASSERT(0);
	return dest;
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
	uintptr_t dest;
	while (voice16txlist.get_freebuffer(& dest) == 0)
		ASSERT(0);
	return dest;
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

typedef blists<uint8_t, UACOUT_AUDIO48_DATASIZE_DMAC, 4> uacout48list_t;
static uacout48list_t uacout48list(IRQL_REALTIME);

extern "C" int_fast32_t buffers_dmabufferuacout48cachesize(void)
{
	return uacout48list.get_cachesize();
}

extern "C"
uintptr_t allocate_dmabufferuacout48(void)
{
	uintptr_t dest;
	while (uacout48list.get_freebuffer(& dest) == 0)
		ASSERT(0);
	return dest;
}

extern "C"
void release_dmabufferuacout48(uintptr_t addr)
{
	uacout48list.release_buffer(addr);
}

extern "C"
void processing_dmabufferuacout48(uintptr_t addr)
{
	uacout_buffer_save((const uint8_t *) addr, UACOUT_AUDIO48_DATASIZE_DMAC, UACOUT_FMT_CHANNELS_AUDIO48, UACOUT_AUDIO48_SAMPLEBYTES);

	release_dmabufferuacout48(addr);
}

#endif /* WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE) */

#if 0

// USB AUDIO RTS IN

#if WITHRTS192

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


extern "C" void XXbuffers_initialize(void)
{
}

#endif /* WITHINTEGRATEDDSP */
