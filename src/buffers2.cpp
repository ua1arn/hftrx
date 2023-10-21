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

#if 0

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


template <typename element, unsigned elements, unsigned capacity>
class blists
{
	struct buffitem
	{
		void * tag2;
		ALIGNX_BEGIN union
		{
			element buff [elements];
			uint8_t filler [EP_align(elements * sizeof (element), DCACHEROWSIZE)];
		} u ALIGNX_END;
		ALIGNX_BEGIN  LIST_ENTRY item ALIGNX_END;
		void * tag3;
	};

	LIST_ENTRY freelist;
	LIST_ENTRY readylist;
	LCLSPINLOCK_t lock;
	buffitem storage [capacity];
public:
	blists()
	{
		unsigned i;

		InitializeListHead(& freelist);
		InitializeListHead(& readylist);
		LCLSPINLOCK_INITIALIZE(& lock);
		for (i = 0; i < capacity; ++ i)
		{
			buffitem * const p = & storage [i];
			//p->tag = BUFFTAG_RTS192;
			p->tag2 = p;
			p->tag3 = p;
			InsertHeadList(& freelist, & p->item);
		}
		PRINTF("Buffer %u %u %u\n", sizeof (element), elements, capacity);
	}
	static int_fast32_t get_cachesize()
	{
		return offsetof(struct buffitem, item) - offsetof(struct buffitem, u);
	}
};
#if WITHINTEGRATEDDSP

// Audio CODEC in/out
typedef ALIGNX_BEGIN struct voice16rx_tag
{
	void * tag2;
	ALIGNX_BEGIN aubufv_t rbuff [DMABUFFSIZE16RX] ALIGNX_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
	void * tag3;
} ALIGNX_END voice16rx_t;

typedef blists<aubufv_t, DMABUFFSIZE16RX, 20> voice16rxlist_t;
static voice16rxlist_t voice16rxlist;

extern "C" int_fast32_t buffers_dmabuffer16rxcachesize(void)
{
	return voice16rxlist.get_cachesize();
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


typedef blists<aubufv_t, DMABUFFSIZE16TX, 20> voice16txlist_t;
static voice16txlist_t voice16txlist;

extern "C" int_fast32_t buffers_dmabuffer16txcachesize(void)
{
	return voice16txlist.get_cachesize();
}

// I/Q data to FPGA or IF CODEC
typedef ALIGNX_BEGIN struct voices32tx_tag
{
	void * tag2;
	ALIGNX_BEGIN IFDACvalue_t buff [DMABUFFSIZE32TX] ALIGNX_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
	void * tag3;
} ALIGNX_END voice32tx_t;

typedef blists<IFDACvalue_t, DMABUFFSIZE32TX, 20> voice32txlist_t;
static voice32txlist_t voice32txlist;

extern "C" int_fast32_t buffers_dmabuffer32txcachesize(void)
{
	return voice32txlist.get_cachesize();
}

// I/Q data from FPGA or IF CODEC
typedef ALIGNX_BEGIN struct voices32rx_tag
{
	ALIGNX_BEGIN IFADCvalue_t buff [DMABUFFSIZE32RX] ALIGNX_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
} ALIGNX_END voice32rx_t;

typedef blists<IFADCvalue_t, DMABUFFSIZE32RX, 20> voice32rxlist_t;
static voice32rxlist_t voice32rxlist;

extern "C" int_fast32_t buffers_dmabuffer32rxcachesize(void)
{
	return voice32rxlist.get_cachesize();
}

// I/Q SPECTRUM data from FPGA or IF CODEC
typedef ALIGNX_BEGIN struct voices32rts_tag
{
	ALIGNX_BEGIN IFADCvalue_t buff [DMABUFFSIZE32RTS] ALIGNX_END;
	ALIGNX_BEGIN LIST_ENTRY item ALIGNX_END;
} ALIGNX_END voice32rts_t;


typedef blists<IFADCvalue_t, DMABUFFSIZE32RTS, 20> voice32rtslist_t;
static voice32rtslist_t voice32rtslist;

extern "C" int_fast32_t buffers_dmabuffer32rtscachesize(void)
{
	return voice32rtslist.get_cachesize();
}

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

typedef blists<uint8_t, UACIN_AUDIO48_DATASIZE_DMAC, 20> uacin48list_t;
static uacin48list_t uacin48list;

extern "C" int_fast32_t buffers_dmabufferuacin48cachesize(void)
{
	return uacin48list.get_cachesize();
}

#endif /* WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) */

#if WITHUSBHW && WITHUSBUACOUT && defined (WITHUSBHW_DEVICE)

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

typedef blists<uint8_t, UACOUT_AUDIO48_DATASIZE_DMAC, 20> uacout48list_t;
static uacout48list_t uacout48list;

extern "C" int_fast32_t buffers_dmabufferuacout48cachesize(void)
{
	return uacout48list.get_cachesize();
}

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

	typedef blists<uint8_t, UACIN_RTS192_DATASIZE_DMAC, 20> voice192rtslist_t;
	static voice192rtslist_t voice192rtslist;

	extern "C" int_fast32_t buffers_dmabuffer192rtscachesize(void)
	{
		return voice192rtslist.get_cachesize();
	}

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

	typedef blists<uint8_t, UACIN_RTS96_DATASIZE_DMAC, 20> voice96rtslist_t;
	static voice96rtslist_t voice96rtslist;

	extern "C" int_fast32_t buffers_dmabuffer96rtscachesize(void)
	{
		return voice96rtslist.get_cachesize();
	}

#endif /* WITHRTS96 */

extern "C" void buffers_initialize(void)
{
}

#endif
#endif /* WITHINTEGRATEDDSP */
