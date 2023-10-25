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

#define BUFOVERSIZE 1


#define UACINRTS192_CAPACITY (14 * BUFOVERSIZE)
#define UACINRTS96_CAPACITY (14 * BUFOVERSIZE)
#define UACOUT48_CAPACITY (4 * BUFOVERSIZE)
#define UACIN48_CAPACITY (24 * BUFOVERSIZE)

#define SPEEX_CAPACITY (5 * BUFOVERSIZE)

#define VOICE32RX_CAPACITY (2 + 6 * BUFOVERSIZE)
#define VOICE32TX_CAPACITY (6 * BUFOVERSIZE)
#define VOICE32RTS_CAPACITY (1 * BUFOVERSIZE)	// dummy fn

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
	// получить из списка свободных, если нет - из готовых
	int get_freebufferforced(element_t * * dest)
	{
		return get_freebuffer(dest) || get_readybuffer(dest);
	}
};

#if defined(WITHRTS96) && defined(WITHRTS192)
	#error Configuration Error: WITHRTS96 and WITHRTS192 can not be used together
#endif /* defined(WITHRTS96) && defined(WITHRTS192) */


#if WITHINTEGRATEDDSP

#if 0
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

#endif

#if 0
// Audio CODEC in/out

typedef blists<aubufv_t, DMABUFFSIZE16RX, 20> voice16rxlist_t;

static voice16rxlist_t voice16rxlist(IRQL_REALTIME);

int_fast32_t cachesize_dmabuffer16rx(void)
{
	return voice16rxlist.get_cachesize();
}

uintptr_t allocate_dmabuffer16rx(void)
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

int_fast32_t cachesize_dmabuffer16tx(void)
{
	return voice16txlist.get_cachesize();
}

uintptr_t allocate_dmabuffer16tx(void)
{
	aubufv_t * dest;
	while (voice16txlist.get_freebuffer(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) dest;
}
#endif

#if 0
// I/Q data to FPGA or IF CODEC

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
	return offsetof(voice32tx_t, pad) - offsetof(voice32tx_t, buff);
}

void release_dmabuffer32tx(uintptr_t addr)
{
	voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
	voice32txlist.release_buffer(p);
}

uintptr_t allocate_dmabuffer32tx(void)
{
	voice32tx_t * dest;
	while (voice32txlist.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) & dest->buff;
}

void save_dmabuffer32tx(uintptr_t addr)
{
	voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
	voice32txlist.release_buffer(p);
}

uintptr_t getfilled_dmabuffer32tx(void)
{
	voice32tx_t * dest;
	if (voice32txlist.get_readybuffer(& dest))
		return (uintptr_t) & dest->buff;
	if (voice32txlist.get_freebuffer(& dest))
		return (uintptr_t) & dest->buff;
	ASSERT(0);
	for (;;)
		;
	return 0;
}

uintptr_t getfilled_dmabuffer32tx_main(void)
{
	return getfilled_dmabuffer32tx();
}

uintptr_t getfilled_dmabuffer32tx_sub(void)
{
	return allocate_dmabuffer32tx();
}

#endif

#if 0
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
	return offsetof(voice32rx_t, pad) - offsetof(voice32rx_t, buff);
}

void release_dmabuffer32rx(uintptr_t addr)
{
	voice32rx_t * const p = CONTAINING_RECORD(addr, voice32rx_t, buff);
	voice32rxlist.release_buffer(p);
}

uintptr_t allocate_dmabuffer32rx(void)
{
	voice32rx_t * dest;
	while (voice32rxlist.get_freebuffer(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) & dest->buff;
}

#endif

#if 0
// I/Q SPECTRUM data from FPGA or IF CODEC
typedef ALIGNX_BEGIN struct voices32rts_tag
{
	ALIGNX_BEGIN IFADCvalue_t buff [DMABUFFSIZE32RTS] ALIGNX_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
} ALIGNX_END voice32rts_t;


typedef blists<voice32rts_t, VOICE32RTS_CAPACITY> voice32rtslist_t;

static voice32rtslist_t voice32rtslist(IRQL_REALTIME);

int_fast32_t cachesize_dmabuffer32rts(void)
{
	return offsetof(voice32rts_t, item) - offsetof(voice32rts_t, buff);
}

void release_dmabuffer32rts(uintptr_t addr)
{
	voice32rts_t * const p = CONTAINING_RECORD(addr, voice32rts_t, buff);
	voice32rtslist.release_buffer(p);
}

uintptr_t getfilled_dmabuffer32rts(void)
{
	voice32rts_t * dest;
	while (voice32rtslist.get_readybuffer(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) & dest->buff;
}

uintptr_t allocate_dmabuffer32rts(void)
{
	voice32rts_t * dest;
	while (voice32rtslist.get_freebuffer(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) & dest->buff;
}
#endif


#if WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE)

// USB AUDIO OUT

typedef struct
{
	ALIGNX_BEGIN  uint8_t buff [UACOUT_AUDIO48_DATASIZE_DMAC] ALIGNX_END;
	ALIGNX_BEGIN  uint8_t pad ALIGNX_END;
} uacout48_t;

typedef blists<uacout48_t, UACOUT48_CAPACITY> uacout48list_t;

static uacout48list_t uacout48list(IRQL_REALTIME);

int_fast32_t cachesize_dmabufferuacout48(void)
{
	return offsetof(uacout48_t, pad) - offsetof(uacout48_t, buff);
}

uintptr_t allocate_dmabufferuacout48(void)
{
	uacout48_t * dest;
	while (uacout48list.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	return (uintptr_t) & dest->buff;
}

void release_dmabufferuacout48(uintptr_t addr)
{
	uacout48_t * const p = CONTAINING_RECORD(addr, uacout48_t, buff);
	uacout48list.release_buffer(p);
}

#endif /* WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE) */

#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)

typedef enum
{
	BUFFTAG_UACIN48 = 44,
	BUFFTAG_RTS192,
	BUFFTAG_RTS96,
	BUFFTAG_total
} uacintag_t;

// USB AUDIO RTS IN

#if WITHRTS192


	typedef struct
	{
		uacintag_t tag;
		ALIGNX_BEGIN  uint8_t buff [UACIN_RTS192_DATASIZE_DMAC] ALIGNX_END;
		ALIGNX_BEGIN  uint8_t pad ALIGNX_END;
	} uacinrts192_t;

	typedef blists<uacinrts192_t, UACINRTS192_CAPACITY> uacinrts192list_t;

	static uacinrts192list_t uacinrts192list(IRQL_REALTIME);

	int_fast32_t cachesize_dmabufferuacinrts192(void)
	{
		return offsetof(uacinrts192_t, pad) - offsetof(uacinrts192_t, buff);
	}

	uintptr_t allocate_dmabufferuacinrts192(void)
	{
		uacinrts192_t * dest;
		while (uacinrts192list.get_freebufferforced(& dest) == 0)
			ASSERT(0);
		dest->tag = BUFFTAG_RTS192;
		return (uintptr_t) & dest->buff;
	}

	uintptr_t getfilled_dmabufferuacinrts192(void)
	{
		uacinrts192_t * dest;
		if (uacinrts192list.get_readybuffer(& dest) != 0)
		{
			dest->tag = BUFFTAG_RTS192;
			return (uintptr_t) & dest->buff;
		}
	}


	void save_dmabufferuacinrts192(uintptr_t addr)
	{
		uacinrts192_t * const p = CONTAINING_RECORD(addr, uacinrts192_t, buff);
		uacinrts192list.save_buffer(p);

		refreshDMA_uacinrts192();		// если DMA  остановлено - начать обмен
	}


	void release_dmabufferuacinrts192(uintptr_t addr)
	{
		uacinrts192_t * const p = CONTAINING_RECORD(addr, uacinrts192_t, buff);
		uacinrts192list.release_buffer(p);
	}

#elif WITHRTS96

	typedef struct
	{
		uacintag_t tag;
		ALIGNX_BEGIN  uint8_t buff [UACIN_RTS96_DATASIZE_DMAC] ALIGNX_END;
		ALIGNX_BEGIN  uint8_t pad ALIGNX_END;
	} uacinrts96_t;

	typedef blists<uacinrts96_t, UACINRTS96_CAPACITY> uacinrts96list_t;

	static uacinrts96list_t uacinrts96list(IRQL_REALTIME);

	int_fast32_t cachesize_dmabufferuacinrts96(void)
	{
		return offsetof(uacinrts96_t, pad) - offsetof(uacinrts96_t, buff);
	}

	uintptr_t allocate_dmabufferuacinrts96(void)
	{
		uacinrts96_t * dest;
		while (uacinrts96list.get_freebufferforced(& dest) == 0)
			ASSERT(0);
		dest->tag = BUFFTAG_RTS96;
		return (uintptr_t) & dest->buff;
	}

	uintptr_t getfilled_dmabufferuacinrts96(void)
	{
		uacinrts96_t * dest;
		if (uacinrts96list.get_readybuffer(& dest))
		{
			dest->tag = BUFFTAG_RTS96;
			return (uintptr_t) & dest->buff;
		}
		return 0;
	}

	void save_dmabufferuacinrts96(uintptr_t addr)
	{
		uacinrts96_t * const p = CONTAINING_RECORD(addr, uacinrts96_t, buff);
		uacinrts96list.save_buffer(p);

		refreshDMA_uacinrts96();		// если DMA  остановлено - начать обмен
	}


	void release_dmabufferuacinrts96(uintptr_t addr)
	{
		uacinrts96_t * const p = CONTAINING_RECORD(addr, uacinrts96_t, buff);
		uacinrts96list.release_buffer(p);
	}

#endif /* WITHRTS96 */

// USB AUDIO IN

typedef struct
{
	uacintag_t tag;
	ALIGNX_BEGIN  uint8_t buff [UACIN_AUDIO48_DATASIZE_DMAC] ALIGNX_END;
	ALIGNX_BEGIN  uint8_t pad ALIGNX_END;
} uacin48_t;

typedef blists<uacin48_t, UACIN48_CAPACITY> uacin48list_t;

static uacin48list_t uacin48list(IRQL_REALTIME);

int_fast32_t cachesize_dmabufferuacin48(void)
{
	return offsetof(uacin48_t, pad) - offsetof(uacin48_t, buff);
}

uintptr_t allocate_dmabufferuacin48(void)
{
	uacin48_t * dest;
	while (uacin48list.get_freebufferforced(& dest) == 0)
		ASSERT(0);
	dest->tag = BUFFTAG_UACIN48;
	return (uintptr_t) & dest->buff;
}

uintptr_t getfilled_dmabufferuacin48(void)
{
	uacin48_t * dest;
	if (uacin48list.get_readybuffer(& dest))
	{
		dest->tag = BUFFTAG_UACIN48;
		return (uintptr_t) & dest->buff;
	}
	return 0;
}

void save_dmabufferuacin48(uintptr_t addr)
{
	uacin48_t * const p = CONTAINING_RECORD(addr, uacin48_t, buff);
	uacin48list.save_buffer(p);

	refreshDMA_uacin48();		// если DMA  остановлено - начать обмен
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

#endif /* WITHINTEGRATEDDSP */

// message buffers
/* Cообщения от уровня обработчиков прерываний к user-level функциям. */

typedef struct
{
	uint8_t type;
	uint8_t data [MSGBUFFERSIZE8];
} message8buff_t;

typedef blists<message8buff_t, 12> message8list_t;

static message8list_t message8list(IRQL_SYSTEM);

// Освобождение обработанного буфера сообщения
void releasemsgbuffer(uint8_t * dest)
{
	message8buff_t * const p = CONTAINING_RECORD(dest, message8buff_t, data);
	message8list.release_buffer(p);
}
// Буфер для формирования сообщения
size_t takemsgbufferfree(uint8_t * * dest)
{
	message8buff_t * addr;
	if (message8list.get_freebuffer(& addr))
	{
		* dest = addr->data;
		return sizeof addr->data;
	}
	return 0;
}
// поместить сообщение в очередь к исполнению
void placesemsgbuffer(uint_fast8_t type, uint8_t * dest)
{
	message8buff_t * const p = CONTAINING_RECORD(dest, message8buff_t, data);
	p->type = type;
	message8list.save_buffer(p);
}

// Буферы с принятымти от обработчиков прерываний сообщениями
uint_fast8_t takemsgready(uint8_t * * dest)
{
	message8buff_t * addr;
	if (message8list.get_readybuffer(& addr))
	{
		* dest = addr->data;
		return addr->type;
	}
	return MSGT_EMPTY;
}

void buffers2_initialize(void)
{
}

