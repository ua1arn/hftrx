/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHLWIP && WITHETHHW && (CPUSTYLE_T507 || CPUSTYLE_T113 || CPUSTYLE_F133)

#include "gpio.h"
#include "formats.h"

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/ip.h"
#include "lwip/dhcp.h"

#include <string.h>


#ifndef LWIP_PBUF_CUSTOM_DATA
#error Check lwipopts.h for LWIP_PBUF_CUSTOM_DATA
#endif

/* Offsets for the 128-bit unique Chip ID inside Allwinner SID eFuse space */
#define SID_OFFSET_CHIPID_W0    0x00
#define SID_OFFSET_CHIPID_W1    0x04
#define SID_OFFSET_CHIPID_W2    0x08
#define SID_OFFSET_CHIPID_W3    0x0C

/* External hardware abstraction function provided by your codebase */
extern uint_fast32_t allwnr_sid_read(unsigned offs);

/**
 * @brief Generates a stable, unique MAC address for a specific EMAC port derived from the SID.
 * @param mac_out Pointer to a 6-byte array where the generated MAC address will be stored.
 * @param port_index Zero-based index of the EMAC port (0 for EMAC0, 1 for EMAC1).
 */
static void allwinner_get_mac_from_sid_dual(uint8_t *mac_out, uint8_t port_index) {
    /* Read the 128-bit unique hardware key word by word using the provided SID API */
    uint32_t w0 = (uint32_t)allwnr_sid_read(SID_OFFSET_CHIPID_W0);
    uint32_t w1 = (uint32_t)allwnr_sid_read(SID_OFFSET_CHIPID_W1);
    uint32_t w2 = (uint32_t)allwnr_sid_read(SID_OFFSET_CHIPID_W2);
    uint32_t w3 = (uint32_t)allwnr_sid_read(SID_OFFSET_CHIPID_W3);

    /* Compress 16 unique identity bytes into 4 hash bytes using bitwise XOR */
    uint32_t hash_low  = w0 ^ w2;
    uint32_t hash_high = w1 ^ w3;

    /* Build the base 6-byte Ethernet MAC address array */
    mac_out[0] = 0x02; /* Enforce 'Locally Administered' address bit, clear Multicast bit */
    mac_out[1] = 0x81; /* Custom static identifier token for the device family */
    mac_out[2] = (uint8_t)(hash_high >> 8);
    mac_out[3] = (uint8_t)(hash_high >> 0);
    mac_out[4] = (uint8_t)(hash_low >> 8);

    /* Modify the least significant byte based on the requested EMAC port index */
    mac_out[5] = (uint8_t)(hash_low >> 0) + port_index;
}

/**
 * @brief Programs the derived unique hardware MAC address into the specific EMAC interface registers.
 * @param emac_peripheral Pointer to the CMSIS-style EMAC peripheral base (e.g., EMAC0 or EMAC1).
 * @param netif Pointer to the corresponding lwIP network interface structure.
 * @param port_index Zero-based index of the EMAC hardware interface.
 */
static void allwinner_emac_apply_mac_dual(EMAC_TypeDef *emac_peripheral, struct netif *netif, uint8_t port_index) {
    uint8_t computed_mac[6];

    /* Fetch the unique MAC address with port-specific byte shifting */
    allwinner_get_mac_from_sid_dual(computed_mac, port_index);

    /* Sync the generated address with the lwIP netif structure layer for ARP processing */
    netif->hwaddr_len = 6;
    for (uint8_t i = 0; i < 6; i++) {
        netif->hwaddr[i] = computed_mac[i];
    }

    /* Construct the 32-bit Low register payload (Bytes 0, 1, 2, 3) */
    uint32_t mac_low = ((uint32_t)computed_mac[0] << 0)  |
                       ((uint32_t)computed_mac[1] << 8)  |
                       ((uint32_t)computed_mac[2] << 16) |
                       ((uint32_t)computed_mac[3] << 24);

    /* Construct the 32-bit High register payload (Bytes 4, 5) */
    uint32_t mac_high = ((uint32_t)computed_mac[4] << 0) |
                        ((uint32_t)computed_mac[5] << 8);

    /* Write directly to the specified hardware MAC filter registers */
    emac_peripheral->EMAC_ADDR [0].LOW  = mac_low;	// lower 32 bits of the 6-byte first MAC address
    emac_peripheral->EMAC_ADDR [0].HIGH = mac_high & 0xFFFF;	// upper 16 bits of the first 6-byte MAC address
    emac_peripheral->EMAC_ADDR [0].HIGH |= (UINT32_C(1) << 31);	// MAC_ADDR_CTL 1: Valid
}

#define EMAC_MAX_PACKET_SIZE      2040

/* Allwinner EMAC DMA Ring Configurations */
#define EMAC_RX_BUFFERS_COUNT     	64//16


/* Allwinner T507-H EMAC TX Descriptor Configuration */

#define DESC_OWN_BY_DMA         (UINT32_C(1) << 31)
#define DESC_RX_LAST            (UINT32_C(1) << 8)
#define DESC_RX_FIRST           (UINT32_C(1) << 9)
//#define DESC_RX_ERRORS_MASK     (UINT32_C(1) << 15)
#define DESC_RX_FL_MASK         0x3FFF0000
#define DESC_RX_FL_SHIFT        16
#define DESC_RX_CHAINED         (UINT32_C(1) << 14)

#if 0
/* Driver private variables tracking state and rings */
static struct emac_dma_desc rx_desc_ring[EMAC_RX_BUFFERS_COUNT] __ALIGNED(4);
static uint8_t rx_buffer_pool[EMAC_RX_BUFFERS_COUNT][EMAC_MAX_PACKET_SIZE] __ALIGNED(4);
static uint32_t rx_index = 0;
#endif

/* Bit definitions for RX/TX Descriptors */
#define DESC_OWN_BY_DMA           (UINT32_C(1) << 31)
//#define DESC_RX_CHAINED           (UINT32_C(1) << 14)
//#define TDES0_TX_CHAINED          (UINT32_C(1) << 20)
#define DESC1_RX_BUF_SIZE_MASK     UINT32_C(0x7FF)
#define DESC1_TX_BUF_SIZE_MASK     UINT32_C(0x7FF)

/* Register Bits for EMAC Control Blocks */
#define EMAC_BASIC_CTL0_DUPLEX    (1 << 0)   /* Default to Full Duplex on start */
#define EMAC_BASIC_CTL0_SPEED_100 (3 << 2)   /* Default safe fallback speed 100M */
#define EMAC_TX_CTL0_TX_EN        (UINT32_C(1) << 31) /* Enable Transmit MAC */
#define EMAC_TX_CTL1_TX_DMA_EN    (UINT32_C(1) << 30) /* Start Transmit DMA Engine */
#define EMAC_TX_CTL1_TX_MD_FORW   (UINT32_C(1) << 1)  /* Store-and-Forward mode for TX */
#define EMAC_RX_CTL0_RX_EN        (UINT32_C(1) << 31) /* Enable Receive MAC */
#define EMAC_RX_CTL1_RX_DMA_EN    (UINT32_C(1) << 30) /* Start Receive DMA Engine */
#define EMAC_RX_CTL1_RX_MD_FORW   (UINT32_C(1) << 1)  /* Store-and-Forward mode for RX */


/* Bit definitions for emac_dma_desc.status (TDES1) */
#define TDES0_OWN_BY_DMA        (UINT32_C(1) << 31)  /* 1 = Hardware owns descriptor, 0 = CPU owns it */
/* Bit definitions for emac_dma_desc.control (TDES1) */
#define TDES1_TX_LAST           (UINT32_C(1) << 30)  /* Last segment of the frame */
#define TDES1_TX_FIRST          (UINT32_C(1) << 29)  /* First segment of the frame */
#define TDES1_CHECKSUM_INSERT   (UINT32_C(0x03) << 27)  /* Enable IP/TCP/UDP hardware checksum calculation */

/* Bit definitions for emac_dma_desc.control (TDES1) */
#define TDES1_BUFFER_SIZE_MASK  0x7FF        /* Size of transmit buffer */

#if 0
/* Driver static structures for TX ring execution */
static struct emac_dma_desc tx_desc_ring[EMAC_TX_BUFFERS_COUNT] __ALIGNED(64);
static uint8_t tx_buffer_pool[EMAC_TX_BUFFERS_COUNT][EMAC_TX_MAX_PACKET_SIZE] __ALIGNED(64);
static uint32_t tx_index = 0;


/*
 * Dual-port Ring Buffers allocation.
 * Explicitly aligned to a 64-byte boundary to meet Cortex-A53 L1 D-Cache line requirements.
 */
static struct emac_dma_desc dual_rx_ring[2][EMAC_RX_BUFFERS_COUNT] __ALIGNED(64);
static struct emac_dma_desc dual_tx_ring[2][EMAC_TX_BUFFERS_COUNT] __ALIGNED(64);

static uint8_t dual_rx_buffers[2][EMAC_RX_BUFFERS_COUNT][EMAC_MAX_PACKET_SIZE] __ALIGNED(64);
static uint8_t dual_tx_buffers[2][EMAC_TX_BUFFERS_COUNT][EMAC_MAX_PACKET_SIZE] __ALIGNED(64);

/* Track variables for current software execution points */
static uint32_t dual_rx_index[2] = {0, 0};
static uint32_t dual_tx_index[2] = {0, 0};



/**
 * @brief Polls the HARDWARE_EMAC_PTR DMA RX ring, extracts packets, and shifts them into lwIP.
 */
static void ethernetif_poll(struct netif *netif) {
	EMAC_TypeDef * const emac_peripheral = HARDWARE_EMAC_PTR;
    struct emac_dma_desc *current_desc;
    struct pbuf *p = NULL;
    struct pbuf *q;
    uint32_t len;

    while (1) {
        current_desc = &rx_desc_ring[rx_index];

        /* Check if the hardware DMA controller still owns this descriptor segment */
        if (current_desc->status & DESC_OWN_BY_DMA) {
            break;
        }

        /* Enforce processing only on fully assembled error-free incoming frames */
        if ((current_desc->status & DESC_RX_FIRST) &&
            (current_desc->status & DESC_RX_LAST) &&
           //!(current_desc->status & DESC_RX_ERRORS_MASK) &&
           1
		   ) {

            len = (current_desc->status & DESC_RX_FL_MASK) >> DESC_RX_FL_SHIFT;

            if (len > 0) {
                p = pbuf_alloc(PBUF_RAW, (uint16_t)len, PBUF_POOL);

                if (p != NULL) {
                    uint32_t bytes_copied = 0;
                    uint8_t *src_ptr = (uint8_t *)(uintptr_t) current_desc->buf_addr;

                    for (q = p; q != NULL; q = q->next) {
                        memcpy(q->payload, &src_ptr[bytes_copied], q->len);
                        bytes_copied += q->len;
                    }

                    if (netif->input(p, netif) != ERR_OK) {
                        pbuf_free(p);
                    }
                }
            }
        }

        /* Hand the descriptor frame back to the EMAC hardware engine */
        current_desc->status = DESC_OWN_BY_DMA;

        /* Increment and wrap the ring array tracker index */
        rx_index++;
        if (rx_index >= EMAC_RX_BUFFERS_COUNT) {
            rx_index = 0;
        }

        /* Poke the receive DMA poll command register to force ring re-scanning */
       // emac_peripheral->EMAC_RX_CTL0 = 0x1;
    }
}

/**
 * @brief Transmits an lwIP pbuf packet chain through the Allwinner HARDWARE_EMAC_PTR TX descriptor ring.
 * @param netif Pointer to the lwIP network interface configuration.
 * @param p Pointer to the allocated lwIP packet buffer containing data frames.
 * @return ERR_OK on success, ERR_MEM if the TX ring is saturated.
 */
err_t XXXlow_level_output(struct netif *netif, struct pbuf *p) {
	EMAC_TypeDef * const emac_peripheral = HARDWARE_EMAC_PTR;
	TP();
    struct emac_dma_desc *current_desc;
    struct pbuf *q;
    uint32_t total_bytes = 0;
    uint8_t *dst_ptr;

    (void)netif;

    /* Read the current descriptor index state from the ring */
    current_desc = &tx_desc_ring[tx_index];

    /* Invalidate descriptor from D-Cache to get the actual hardware state */
    dcache_invalidate((uintptr_t)current_desc, sizeof(struct emac_dma_desc));

    /* Check if the descriptor is still held and processed by the EMAC DMA engine */
    if (current_desc->status & TDES0_OWN_BY_DMA) {
        /* TX ring buffer saturation - core stack must retry later */
        return ERR_MEM;
    }

    dst_ptr = &tx_buffer_pool[tx_index][0];

    /* Flatten the multi-segmented lwIP pbuf chain into a linear hardware buffer */
    for (q = p; q != NULL; q = q->next) {
        if ((total_bytes + q->len) > EMAC_TX_MAX_PACKET_SIZE) {
            /* Packet size exceeds hardware buffer limits, safety abort */
            return ERR_VAL;
        }
        memcpy(&dst_ptr[total_bytes], q->payload, q->len);
        total_bytes += q->len;
    }

    /* Push the newly populated data payload out of the CPU data cache into physical DDR memory */
    dcache_clean((uintptr_t)dst_ptr, total_bytes);

    /* Set buffer parameters and establish the descriptor configuration */
    current_desc->control = (total_bytes & TDES1_BUFFER_SIZE_MASK);
    current_desc->buf_addr = (uintptr_t)dst_ptr;

    /* Mark the descriptor as a single complete frame with hardware checksum offloading */
    current_desc->status = TDES0_TX_FIRST | TDES0_TX_LAST | TDES0_TX_CHAINED | TDES0_CHECKSUM_INSERT;

    /* Hand over descriptor ownership to the EMAC hardware engine */
    current_desc->status |= TDES0_OWN_BY_DMA;

    /* Push the modified descriptor out of the CPU cache to ensure DMA visible execution */
    dcache_clean((uintptr_t)current_desc, sizeof(struct emac_dma_desc));

    /* Increment and wrap around the active TX index tracker */
    tx_index++;
    if (tx_index >= EMAC_TX_BUFFERS_COUNT) {
        tx_index = 0;
    }

    /* Poke the transmit poll command register to awake the TX DMA controller if suspended */
    emac_peripheral->EMAC_TX_CTL1 = 0x1;

    return ERR_OK;
}


/**
 * @brief Fully initializes a specific EMAC hardware peripheral interface.
 * @param emac_peripheral Pointer to the CMSIS-style peripheral struct (EMAC0 or EMAC1).
 * @param netif Pointer to the target lwIP layer interface mapping.
 * @param port_index Zero-based index specifying the target port (0 for EMAC0, 1 for EMAC1).
 * @return ERR_OK on successful configuration.
 */
static err_t allwinner_emac_init_port(EMAC_TypeDef *emac_peripheral, struct netif *netif, uint8_t port_index) {
    if (port_index > 1) {
        return ERR_ARG;
    }

    /* Reset software internal ring pointers */
    dual_rx_index[port_index] = 0;
    dual_tx_index[port_index] = 0;

    /* 1. Build and link the RX Ring Descriptors list */
    for (uint32_t i = 0; i < EMAC_RX_BUFFERS_COUNT; i++) {
        dual_rx_ring[port_index][i].status = DESC_OWN_BY_DMA;
        dual_rx_ring[port_index][i].control = DESC_RX_CHAINED | (EMAC_MAX_PACKET_SIZE & DESC_RX_BUF_SIZE_MASK);
        dual_rx_ring[port_index][i].buf_addr = (uintptr_t)&dual_rx_buffers[port_index][i][0];

        /* Loop the final element descriptor back to the root start address */
        if (i == (EMAC_RX_BUFFERS_COUNT - 1)) {
            dual_rx_ring[port_index][i].next_desc = (uintptr_t)&dual_rx_ring[port_index][0];
        } else {
            dual_rx_ring[port_index][i].next_desc = (uintptr_t)&dual_rx_ring[port_index][i + 1];
        }
    }

    /* 2. Build and link the TX Ring Descriptors list */
    for (uint32_t i = 0; i < EMAC_TX_BUFFERS_COUNT; i++) {
        dual_tx_ring[port_index][i].status = 0; /* CPU owns TX rings on bootup */
        dual_tx_ring[port_index][i].control = 0;
        dual_tx_ring[port_index][i].buf_addr = (uintptr_t)&dual_tx_buffers[port_index][i][0];

        if (i == (EMAC_TX_BUFFERS_COUNT - 1)) {
            dual_tx_ring[port_index][i].next_desc = (uintptr_t)&dual_tx_ring[port_index][0];
        } else {
            dual_tx_ring[port_index][i].next_desc = (uintptr_t)&dual_tx_ring[port_index][i + 1];
        }
    }

    /* Flush all memory structures out of D-Cache to prevent memory hazards for the DMA engine */
    dcache_clean((uintptr_t)&dual_rx_ring[port_index][0], sizeof(dual_rx_ring[port_index]));
    dcache_clean((uintptr_t)&dual_tx_ring[port_index][0], sizeof(dual_tx_ring[port_index]));
    dcache_clean((uintptr_t)&dual_rx_buffers[port_index][0][0], sizeof(dual_rx_buffers[port_index]));

    /* 3. Program core base registers inside the EMAC controller */
    emac_peripheral->EMAC_RX_DMA_DESC_LIST = (uintptr_t)&dual_rx_ring[port_index][0];
    emac_peripheral->EMAC_TX_DMA_DESC_LIST = (uintptr_t)&dual_tx_ring[port_index][0];

    /* Setup safe default speed and duplex in Basic configuration */
    emac_peripheral->EMAC_BASIC_CTL0 = EMAC_BASIC_CTL0_SPEED_100 | EMAC_BASIC_CTL0_DUPLEX;

    /* Generate and program the unique port hardware MAC address derived from eFuses */
    allwinner_emac_apply_mac_dual(emac_peripheral, netif, port_index);

    /* 4. Configure DMA options and awake the transmission layers */
    /* Setup Store-and-Forward mode to prevent buffer underrun/overflow errors */
    emac_peripheral->EMAC_TX_CTL1 = EMAC_TX_CTL1_TX_MD_FORW;
    emac_peripheral->EMAC_RX_CTL1 = EMAC_RX_CTL1_RX_MD_FORW;

    /* Activate both MAC state controllers */
    emac_peripheral->EMAC_TX_CTL0 |= EMAC_TX_CTL0_TX_EN;
    emac_peripheral->EMAC_RX_CTL0 |= EMAC_RX_CTL0_RX_EN;

    /* Fire up the main pipeline DMA execution rings */
    emac_peripheral->EMAC_TX_CTL1 |= EMAC_TX_CTL1_TX_DMA_EN;
    emac_peripheral->EMAC_RX_CTL1 |= EMAC_RX_CTL1_RX_DMA_EN;

    return ERR_OK;
}
#endif

static uint8_t rxbuffs [EMAC_RX_BUFFERS_COUNT] [EMAC_MAX_PACKET_SIZE];
static RAMNC __ALIGNED(4) struct emac_dma_desc emac_rxdesc [EMAC_RX_BUFFERS_COUNT];

static LIST_ENTRY TxList;
//static LIST_ENTRY TxDoneList;
static int relinktxdesc(EMAC_TypeDef * const emac_peripheral);

static void printlisttx(EMAC_TypeDef * const emac_peripheral, const char * title)
{
	if (! IsListEmpty(& TxList))
	{
		PRINTF("tx dma desc list %s: %p\n", title, (void *) (uintptr_t) emac_peripheral->EMAC_TX_DMA_DESC_LIST);
		LIST_ENTRY * const head = TxList.Flink;
		LIST_ENTRY * t = TxList.Flink;
		listsupport_t * const lshead = CONTAINING_RECORD(head, listsupport_t, item);
		do {
			listsupport_t * const ls = CONTAINING_RECORD(t, listsupport_t, item);
			struct emac_dma_desc * const txd = & ls->dmadesc;
			dcache_invalidate((uintptr_t) & ls->dmadesc, sizeof ls->dmadesc);
			PRINTF("list item %p: status=%08X control=%08X %s%s, next=%p, len=%u\n",
					txd,
					(unsigned) txd->status, (unsigned) txd->control, txd->control & TDES1_TX_FIRST ? "F" : " ", txd->control & TDES1_TX_LAST ? "L" : " ",
					(void *) (uintptr_t) txd->next_desc,
					(unsigned) (txd->control & DESC1_TX_BUF_SIZE_MASK));
			t = t->Flink;
		} while (t != & TxList);

	}
	else
	{
		PRINTF("TxList is empty\n");
	}
}

static void stoptxdma(EMAC_TypeDef * const emac_peripheral)
{
	if ((emac_peripheral->EMAC_TX_DMA_STA & 0x07) == 0x00)	// STOP state
		return;
   	//if (local_wait32mask(& emac_peripheral->EMAC_TX_DMA_STA, 0x07, 0x06, 500))	// SUSPEND state
   	if (local_wait32mask(& emac_peripheral->EMAC_TX_DMA_STA, 0x07, 0x06, 500))	// SUSPEND state
   	{
		TP();
		PRINTF("EMAC_TX_CTL1=%08X\n", (unsigned) emac_peripheral->EMAC_TX_CTL1);
		PRINTF("EMAC_TX_DMA_STA=%08X\n", (unsigned) emac_peripheral->EMAC_TX_DMA_STA);
   	}
	emac_peripheral->EMAC_TX_CTL1 &= ~ EMAC_TX_CTL1_TX_DMA_EN;	// DMA EN
   	if (local_wait32mask(& emac_peripheral->EMAC_TX_DMA_STA, 0x07, 0x00, 500))	// STOP state
   	{
		TP();
		PRINTF("EMAC_TX_CTL1=%08X\n", (unsigned) emac_peripheral->EMAC_TX_CTL1);
		PRINTF("EMAC_TX_DMA_STA=%08X\n", (unsigned) emac_peripheral->EMAC_TX_DMA_STA);
   	}
}

static void starttxdma(EMAC_TypeDef * const emac_peripheral)
{

	emac_peripheral->EMAC_TX_CTL1 |= EMAC_TX_CTL1_TX_DMA_EN;	// DMA EN
	emac_peripheral->EMAC_TX_CTL1 |= (UINT32_C(1) << 31);	// TX_DMA_START (auto-clear)
	if (local_wait32mask(& emac_peripheral->EMAC_TX_CTL1, (UINT32_C(1) << 31), 0 * (UINT32_C(1) << 31), 10))
		TP();
}

static void addtotxdmalist(struct pbuf * p)
{

	p->custom_item.sign1 = & p->custom_item;
	p->custom_item.sign2 = & p->custom_item;
	InsertTailList(& TxList, & p->custom_item.item);
}

// Перестройка ссылок в списке dma descriptors
static int relinktxdesc(EMAC_TypeDef * const emac_peripheral)
{
	// Layout descriptors list
	if (! IsListEmpty(& TxList))
	{

		LIST_ENTRY * const head = TxList.Flink;
		LIST_ENTRY * t = TxList.Flink;
		listsupport_t * const lshead = CONTAINING_RECORD(head, listsupport_t, item);
		do {

			listsupport_t * const ls = CONTAINING_RECORD(t, listsupport_t, item);
			listsupport_t * const lsnext = CONTAINING_RECORD(t->Flink, listsupport_t, item);

			ls->dmadesc.next_desc = (t->Flink == & TxList) ?
					(uintptr_t) & lshead->dmadesc :
					(uintptr_t) & lsnext->dmadesc;
			ASSERT(ls->dmadesc.next_desc != 0);
			//ASSERT(ls->dmadesc.status & (UINT32_C(1) << 31));
			dcache_clean((uintptr_t) & ls->dmadesc, sizeof ls->dmadesc);

			t = t->Flink;
		} while (t != & TxList);

		emac_peripheral->EMAC_TX_DMA_DESC_LIST = (uintptr_t) & lshead->dmadesc;
		return 1;
	}
	return 0;
}

static void emac_txhandler(void * ctx)
{
	struct netif * const netif = (struct netif *) ctx;
	EMAC_TypeDef * const emac_peripheral = HARDWARE_EMAC_PTR;
	enum {
		ALLERR =
				(UINT32_C(1) << 16) |
				(UINT32_C(1) << 14) |
				(UINT32_C(1) << 12) |
				(UINT32_C(1) << 10) |
				(UINT32_C(1) << 9) |
				(UINT32_C(1) << 8) |
				(UINT32_C(1) << 2) |
				(UINT32_C(1) << 1) |
				(UINT32_C(1) << 0) |
			0
	};
   	unsigned deleted = 0;
   	unsigned total = 0;
   	PLIST_ENTRY t;
   	PLIST_ENTRY next;
	for (t = TxList.Flink; t != & TxList; t = next)
	{
		ASSERT(t != NULL);
		++ total;
		next = t->Flink;
		listsupport_t * const pl = CONTAINING_RECORD(t, listsupport_t, item);
		ASSERT(pl->sign1 == pl && pl->sign2 == pl);
		struct emac_dma_desc * txd = & pl->dmadesc;
		dcache_invalidate((uintptr_t) txd, sizeof * txd);
		if (pl->dmadesc.status & DESC_OWN_BY_DMA)
			continue;	// пока в работе

		RemoveEntryList(t);
		struct pbuf *p = CONTAINING_RECORD(pl, struct pbuf, custom_item);
		pbuf_free(p);
		++ deleted;
	}
	//PRINTF("emac_txhandler: deleted=%u,total=%u\n", deleted, total);
	if (deleted)
	{
		stoptxdma(emac_peripheral);		// возможно удаляли
		if (relinktxdesc(emac_peripheral))
			starttxdma(emac_peripheral);
		//printlisttx(emac_peripheral, "after txhandler");
	}
}

// опрос принятых
static void emac_rxhandler(void * ctx)
{
	struct netif * const netif = (struct netif *) ctx;
	EMAC_TypeDef * const emac_peripheral = HARDWARE_EMAC_PTR;
	const uint_fast32_t RXERRMASK =
			(UINT32_C(1) << 11) |	// RX_OVERFLOW_ERR - When set, a buffer overflow error occurred and current frame is wrong.
			0x000000DB |			// hardware errors
			0;
	const uint_fast32_t RXADDRERR =
			(UINT32_C(1) << 30) |	// RX_DAF_FAIL - destination address filter
			(UINT32_C(1) << 13) |	// RX_SAF_FAIL - source address filter
			0;
	// ставятся, скорее всего, если дескриптор часть цепочки (не проверялось)
	const int_fast32_t FIRSTANDLAST =
			DESC_RX_FIRST |
			DESC_RX_LAST |
			0;

	dcache_clean_invalidate((uintptr_t) emac_rxdesc, sizeof emac_rxdesc);
	unsigned i;
	for (i = 0; i < ARRAY_SIZE(emac_rxdesc); ++ i)
	{
		struct emac_dma_desc * const rxd = & emac_rxdesc [i];
		const uint_fast32_t status = rxd->status;
		if (status & DESC_OWN_BY_DMA)
			continue;

		if (status & RXADDRERR)
			;
		else if ((status & RXERRMASK) == 0 /* && (status & FIRSTANDLAST) == FIRSTANDLAST */)	// Error flags
		{
			//PRINTF("rxd->status: %08X (err=%08X)\n", (unsigned) rxd->status, (unsigned) (rxd->status & 0x000000DB));

			const uintptr_t dataptr = (uintptr_t) rxd->buf_addr;
			const int size = (status & DESC_RX_FL_MASK) >> DESC_RX_FL_SHIFT;
	 		struct pbuf * const p = pbuf_alloc(PBUF_RAW, size + ETH_PAD_SIZE, PBUF_POOL);
			if (p == NULL)
			{
				TP();
				continue;
			}
			//PRINTF("rx: %d\n", size);
			//printhex(dataptr, (void *) dataptr, size);
			const err_t e = pbuf_take_at(p, (void *) dataptr, size, ETH_PAD_SIZE);
			if (e == ERR_OK)
			{
				const err_t e = ethernet_input(p, netif);
				if (e != ERR_OK)
				{
					  /* This means the pbuf is freed or consumed,
					     so the caller doesn't have to free it again */
					TP();
				}
			}
			else
			{
				pbuf_free(p);
				TP();
			}
		}
		else
		{
			PRINTF("RX errors: %08X (%08X)\n", (unsigned) status, (unsigned) (status & RXERRMASK));
		}

		dcache_invalidate((uintptr_t) rxd->buf_addr, EMAC_MAX_PACKET_SIZE);	// подготовка к приёму следующего
		rxd->status = DESC_OWN_BY_DMA;	// RX_DESC_CTL
		dcache_clean((uintptr_t) rxd, sizeof * rxd);
	}
}

static void printchain(const char * title, const struct pbuf *p)
{
	// print segmented buffer
	unsigned sc = 0;
	const struct pbuf *p1 = p;
	PRINTF("%s: tx all (%u %04X bytes): ", title, p->tot_len, p->tot_len);
	while (p1 != 0)
	{
		PRINTF("ref=%u,n=%u ", p1->ref, p1->len);
		//printhex(0 + sc, p1->payload, p1->len);
		sc += p1->len;
		p1 = p1->next;
	}
	PRINTF("\n");
}

static void printchain2(const char * title, const struct pbuf *p)
{
	// print segmented buffer
	unsigned sc = 0;
	const struct pbuf *p1 = p;
	PRINTF("%s: tx all (%u %04X bytes):\n", title, p->tot_len, p->tot_len);
	while (p1 != 0)
	{
		//PRINTF("ref=%u,n=%u ", p1->ref, p1->len);
		printhex(0 + sc, p1->payload, p1->len);
		sc += p1->len;
		p1 = p1->next;
	}
	//PRINTF("\n");
}

static void emac_dma_desc_set(struct emac_dma_desc * txd, uint_fast32_t control, uintptr_t buf_addr, unsigned len)
{
	ASSERT(len <= EMAC_MAX_PACKET_SIZE);
	txd->buf_addr = buf_addr;
	txd->control =	// ctl
		control |
		(len & DESC1_TX_BUF_SIZE_MASK) |	// 10:0 BUF_SIZE
		0;
}

static err_t low_level_output(struct netif *netif, struct pbuf *p) {

	EMAC_TypeDef * const emac_peripheral = HARDWARE_EMAC_PTR;

  	const uint_fast32_t CONTROLMODE =
 			1 * (UINT32_C(1) << 31) |	// TX_INT_CTL
			//TDES0_CHECKSUM_INSERT |
			//0x03 * (UINT32_C(1) << 27) |	// CHECKSUM_CTL
			//1 * (UINT32_C(1) << 26) |	// CRC_CTL When it is set, the CRC field is not transmitted.
		//		1 * (UINT32_C(1) << 24) |	// magic. Without it, packets never be sent on H3 SoC
			0;

   	const uint_fast32_t ONLYONECONTROL =
   			CONTROLMODE |
			TDES1_TX_LAST |	// LAST_DESC
			TDES1_TX_FIRST |	// FIR_DESC
			0;

   	const uint_fast32_t FIRSTCONTROL =
  			CONTROLMODE |
			TDES1_TX_FIRST |	// FIR_DESC
			0;
   	const uint_fast32_t MIDDLECONTROL =
  			CONTROLMODE |
			0;
   	const uint_fast32_t LASTCONTROL =
   			CONTROLMODE |
			TDES1_TX_LAST |	// LAST_DESC
			0;

	ASSERT(p);
	if (p->tot_len <= ETH_PAD_SIZE)
	{
		// нечего передавать
		ASSERT(0);
	}
	else if (p->next == NULL)
	{
		//printchain2("send1", p);
		// состоит из одного сегмента
		ASSERT(p->next == NULL);
		pbuf_ref(p);	// Then use pbuf_free for only first element in chain
		struct emac_dma_desc * txd = & p->custom_item.dmadesc;
		const unsigned chunk = p->len - ETH_PAD_SIZE;
	    const uintptr_t dataptr = (uintptr_t) p->payload + ETH_PAD_SIZE;
		dcache_clean((uintptr_t) dataptr, chunk);
		emac_dma_desc_set(txd, ONLYONECONTROL, dataptr, chunk);
		txd->status = (UINT32_C(1) << 31); // TX_DESC_CTL
		addtotxdmalist(p);
	}
	else if (!0)
	{
		// Эксперементальное
		const unsigned buflen = ((p->tot_len - ETH_PAD_SIZE) + 0x7FF) & ~ UINT32_C(0x7FF);
		struct pbuf * const newb = pbuf_alloc(PBUF_RAW, buflen, PBUF_POOL);
		ASSERT(newb);
	    const uintptr_t dataptr = (uintptr_t) newb->payload;
		//pbuf_ref(p);	// Then use pbuf_free for only first element in chain
		const unsigned chunk = pbuf_copy_partial(p, (void *) dataptr, buflen, ETH_PAD_SIZE);

		//printhex(dataptr, p->custom_item.memp, chunk);

		struct emac_dma_desc * const txd = & newb->custom_item.dmadesc;
		dcache_clean(dataptr, chunk);
		emac_dma_desc_set(txd, ONLYONECONTROL, dataptr, chunk);
		txd->status = (UINT32_C(1) << 31); // TX_DESC_CTL
		addtotxdmalist(newb);
	}
	else
	{
		// состоит из двух и более сегментов
		//PRINTF("%s: Segmented pbuf: p->tot_len=%u, p->len=%u (siz=%u)\n", __func__, (unsigned) p->tot_len, (unsigned) p->len, EMAC_MAX_PACKET_SIZE);
		//printchain2("Before", p);
		//ASSERT(0);
		{
			pbuf_ref(p);
			struct emac_dma_desc * txd = & p->custom_item.dmadesc;
			const unsigned chunk = p->len - ETH_PAD_SIZE;
		    const uintptr_t dataptr = (uintptr_t) p->payload + ETH_PAD_SIZE;
			dcache_clean(dataptr, chunk);
			emac_dma_desc_set(txd, FIRSTCONTROL, dataptr, chunk);
			txd->status = (UINT32_C(1) << 31); // TX_DESC_CTL
			addtotxdmalist(p);
		}
		p = p->next;
		while (p)
		{
			const unsigned chunk = p->len;
			const int ismiddle = p->next != NULL;
			pbuf_ref(p);
			struct emac_dma_desc * txd = & p->custom_item.dmadesc;
		    const uintptr_t dataptr = (uintptr_t) p->payload;
			dcache_clean(dataptr, chunk);
			emac_dma_desc_set(txd, ismiddle ? MIDDLECONTROL : LASTCONTROL, dataptr, chunk);
			txd->status |= (UINT32_C(1) << 31); // TX_DESC_CTL
			addtotxdmalist(p);
			p = p->next;
		}
	}
	stoptxdma(emac_peripheral);		// возможно удаляли
	ASSERT(relinktxdesc(emac_peripheral));
	starttxdma(emac_peripheral);
	//printlisttx(emac_peripheral, "after linkout");
	return ERR_OK;
}

static dpcobj_t dpclinkspoolirq;
static dpcobj_t dpcrxirq;
static dpcobj_t dpctxirq;

static void EMAC_Handler(void)
{
	EMAC_TypeDef * const emac_peripheral = HARDWARE_EMAC_PTR;
	const portholder_t sta0 = emac_peripheral->EMAC_INT_STA;
	const portholder_t stamask = emac_peripheral->EMAC_INT_EN;
	const portholder_t sta = sta0 & stamask;
	emac_peripheral->EMAC_INT_STA = sta;

//	if (sta & (UINT32_C(1) << 16))	// RGMII_LINK_STA_P
//	{
//		TP();
//		board_dpc_call(& dpclinkspoolirq, board_dpc_coreid());
//	}

//	PRINTF("sta0=%08X,stamask=%08X,sta=%08X\n", (unsigned) sta0, (unsigned) stamask, (unsigned) sta);
//	if (sta & (UINT32_C(1) << 1))
//		PRINTF("TX DMA STOPPED interrupt\n");
//	if (sta & (UINT32_C(1) << 10))
//		PRINTF("RX DMA STOPPED interrupt\n");

	if (sta & (UINT32_C(1) << 8))	// // RX_INT
		board_dpc_call(& dpcrxirq, board_dpc_coreid());
	if (sta & (UINT32_C(1) << 0))	// // TX_INT
		board_dpc_call(& dpctxirq, board_dpc_coreid());
}

/* Realtek RTL8211F PHY Registers */
// Page 0xa42
#define RTL8211F_IER			0x12	// INER (Interrupt Enable Register, Page 0xa42, Address 0x12)

// Page 0xa43
#define RTL8211F_PHYSR          0x1A	// PHYSR (PHY Specific Status Register, Page 0xa43, Address 0x1A)
#define RTL8211F_INSR			0x1D	// INSR (Interrupt Status Register, Page 0xa43, Address 0x1D)
#define RTL8211F_PAGSR 			0x1F 	// PAGSR (Page Select Register, Page 0xa43, Address 0x1F)

/* RTL8211F PHYSR (Register 26) Bit Definitions */
#define PHYSR_LINK_STATUS       (1 << 2)
#define PHYSR_DUPLEX_STATUS     (1 << 3)
#define PHYSR_SPEED_MASK        (3 << 4)
#define PHYSR_SPEED_10          (0 << 4)
#define PHYSR_SPEED_100         (1 << 4)
#define PHYSR_SPEED_1000        (2 << 4)

/* Allwinner EMAC Register Offsets and Base Addresses */
//#define SYS_CTRL_BASE           0x01C00000
//#define EMAC_CLK_REG            (*(volatile uint32_t *)(SYS_CTRL_BASE + 0x30))
//
//#define EMAC_BASE               0x01C30000
//#define EMAC_BASIC_CTL_0        (*(volatile uint32_t *)(EMAC_BASE + 0x00))
//#define EMAC_MII_CMD            (*(volatile uint32_t *)(EMAC_BASE + 0x48))
//#define EMAC_MII_DATA           (*(volatile uint32_t *)(EMAC_BASE + 0x4C))

/* Allwinner EMAC Bit Definitions */
#define EMAC_MII_BUSY           (1 << 0)
#define EMAC_MII_WRITE          (1 << 1)
#define EMAC_MII_CLK_DIV_64     (2 << 2)

#define EMAC_CTL_SPEED_1000     (0 << 2)
#define EMAC_CTL_SPEED_100      (3 << 2)
#define EMAC_CTL_SPEED_10       (2 << 2)
#define EMAC_CTL_SPEED_MASK     (0x03 << 2)
#define EMAC_CTL_DUPLEX_FULL    (1 << 0)

/**
 * @brief Reads a 16-bit register from the PHY via MDIO interface.
 */
static uint16_t emac_mdio_read(uint8_t phy_addr, uint8_t reg_addr) {
	EMAC_TypeDef * const emac_peripheral = HARDWARE_EMAC_PTR;
    /* Wait until MDIO interface is idle */
    /* Wait for command execution completion */
    if (local_wait32mask(& emac_peripheral->EMAC_MII_CMD, EMAC_MII_BUSY, 0 * EMAC_MII_BUSY, 100)) {
		TP();
		return 0;
    }

    /* Form read command with safe clock divider */
    uint32_t cmd = ((phy_addr & 0x1F) << 16) |
                   ((reg_addr & 0x1F) << 4)  |
                   EMAC_MII_CLK_DIV_64       |
                   EMAC_MII_BUSY;

    emac_peripheral->EMAC_MII_CMD = cmd;

    /* Wait for command execution completion */
    /* Wait for command execution completion */
    if (local_wait32mask(& emac_peripheral->EMAC_MII_CMD, EMAC_MII_BUSY, 0 * EMAC_MII_BUSY, 100)) {
		TP();
    }

    return (uint16_t)(emac_peripheral->EMAC_MII_DATA & 0xFFFF);
}

static void emac_mdio_write(uint8_t phy_addr, uint8_t reg_addr, uint16_t reg_value) {
	EMAC_TypeDef * const emac_peripheral = HARDWARE_EMAC_PTR;
    /* Wait until MDIO interface is idle */
    if (local_wait32mask(& emac_peripheral->EMAC_MII_CMD, EMAC_MII_BUSY, 0 * EMAC_MII_BUSY, 100)) {
		TP();
    	return;
	}

    /* Form read command with safe clock divider */
    uint32_t cmd = ((phy_addr & 0x1F) << 16) |
                   ((reg_addr & 0x1F) << 4)  |
                   EMAC_MII_CLK_DIV_64       |
				   EMAC_MII_WRITE |
                   EMAC_MII_BUSY;

    emac_peripheral->EMAC_MII_DATA = reg_value & 0xFFFF;
    emac_peripheral->EMAC_MII_CMD = cmd;

    /* Wait for command execution completion */
    if (local_wait32mask(& emac_peripheral->EMAC_MII_CMD, EMAC_MII_BUSY, 0 * EMAC_MII_BUSY, 100)) {
		TP();
    }
}

/**
 * @brief Updates Allwinner EMAC internal MAC controller speed and duplex settings.
 */
static void allwinner_emac_update_mac_speed(EMAC_TypeDef * const emac_peripheral, uint32_t speed, uint32_t is_full_duplex) {
	uint_fast32_t ctl_val = emac_peripheral->EMAC_BASIC_CTL0;
    PRINTF("allwinner_emac_update_mac_speed: speed=%u, is_full_duplex=%u\n", (unsigned) speed, (unsigned) is_full_duplex);

    /* Clear existing speed (bits 3:2) and duplex (bit 0) configurations */
    ctl_val &= ~ (EMAC_CTL_SPEED_MASK | EMAC_CTL_DUPLEX_FULL);

    /* Apply full or half duplex state */
    if (is_full_duplex) {
        ctl_val |= EMAC_CTL_DUPLEX_FULL;
    }

    /* Apply internal MAC speed bits (SYS_CTRL / CCU clocks are bypassed) */
    if (speed == 1000) {
        ctl_val |= EMAC_CTL_SPEED_1000;
    }
    else if (speed == 100) {
        ctl_val |= EMAC_CTL_SPEED_100;
    }
    else if (speed == 10) {
        ctl_val |= EMAC_CTL_SPEED_10;
    }

    /* Write updated values back to the EMAC configuration register */
    emac_peripheral->EMAC_BASIC_CTL0 = ctl_val;
}

// Non-zero if other then required
static int allwinner_emac_compare_mac_speed(EMAC_TypeDef * const emac_peripheral, uint32_t speed, uint32_t is_full_duplex) {
    const uint_fast32_t mask = EMAC_CTL_SPEED_MASK | EMAC_CTL_DUPLEX_FULL;
    uint_fast32_t ctl_val = 0;
    //PRINTF("allwinner_emac_compare_mac_speed: speed=%u, is_full_duplex=%u\n", (unsigned) speed, (unsigned) is_full_duplex);

    /* Apply full or half duplex state */
    if (is_full_duplex) {
        ctl_val |= EMAC_CTL_DUPLEX_FULL;
    }

    /* Apply internal MAC speed bits (SYS_CTRL / CCU clocks are bypassed) */
    if (speed == 1000) {
        ctl_val |= EMAC_CTL_SPEED_1000;
    }
    else if (speed == 100) {
        ctl_val |= EMAC_CTL_SPEED_100;
    }
    else if (speed == 10) {
        ctl_val |= EMAC_CTL_SPEED_10;
    }
    /* Write updated values back to the EMAC configuration register */
    return (emac_peripheral->EMAC_BASIC_CTL0 & mask) != ctl_val;
}

static const uint_fast16_t insr_mask =
		(UINT16_C(1) << 10) |	// jabber
		(UINT16_C(1) << 9) |	// ALDPS state changed
		(UINT16_C(1) << 7) |	// WOL event occurred
		//(UINT16_C(1) << 5) |	// Can access PHY Register through MDC/MDIO
		(UINT16_C(1) << 4) |	// Link status changed
		(UINT16_C(1) << 3) |	// Auto-Negotiation completed
		//(UINT16_C(1) << 2) |	// Page (a new LCW) received
		(UINT16_C(1) << 0) |	// Auto-Negotiation Error
		0;

/**
 * @brief Periodically checks RTL8211F link status and updates the lwIP network interface.
 */
static void check_ethernet_link_status(struct netif *netif) {
	EMAC_TypeDef * const emac_peripheral = HARDWARE_EMAC_PTR;
    const uint16_t physr = emac_mdio_read(RTL8211F_PHY_ADDR, RTL8211F_PHYSR);
    const uint16_t insr = emac_mdio_read(RTL8211F_PHY_ADDR, RTL8211F_INSR);
    //PRINTF("physr=0x%04X, insr=0x%02X\n", (unsigned) physr, (unsigned) insr);

    /* Get actual hardware status: 1 = connected, 0 = disconnected */
    const uint8_t hw_link_up = !! (physr & PHYSR_LINK_STATUS);
    const uint8_t hw_speed_bits = physr & PHYSR_SPEED_MASK;
    const uint8_t hw_duplex_bit = !! (physr & PHYSR_DUPLEX_STATUS);
    const int8_t hw_insr = !! (insr & insr_mask);

    uint32_t hw_speed = 100;
    switch (hw_speed_bits) {
    case PHYSR_SPEED_10:
    	hw_speed = 10; break;
    case PHYSR_SPEED_100:
    	hw_speed = 100; break;
    case PHYSR_SPEED_1000:
    	hw_speed = 1000; break;
    default:
    	TP();
    	break;
    }

    /* Get current lwIP software status: 1 = up, 0 = down */
    uint8_t sw_link_up = !!netif_is_link_up(netif);

    /* Compare hardware reality with software stack state */
    if (hw_insr ||
    		hw_link_up != sw_link_up ||
    		allwinner_emac_compare_mac_speed(emac_peripheral, hw_speed, hw_duplex_bit)) {
        if (hw_link_up) {

            allwinner_emac_update_mac_speed(emac_peripheral, hw_speed, hw_duplex_bit);

            netif_set_link_up(netif);
            if (board_get_eth_dhcp())
            {
                err_t e = dhcp_start(netif);
                ASSERT(ERR_OK == e);
            }
        } else {
    		dhcp_release_and_stop(netif);
            netif_set_ipaddr(netif, IP4_ADDR_ANY4);
            netif_set_netmask(netif, IP4_ADDR_ANY4);
            netif_set_gw(netif, IP4_ADDR_ANY4);
            netif_set_link_down(netif);
        }
    }
}


static void allwinner_emac_ccu_init(void)
{
	const unsigned ix = HARDWARE_EMAC_IX;	// 0: HARDWARE_EMAC_PTR, 1: EMAC1
	CCU->EMAC_BGR_REG |= (UINT32_C(1) << ((0 + ix)));	// Gating Clock for EMACx
	CCU->EMAC_BGR_REG &= ~ (UINT32_C(1) << ((16 + ix)));	// EMACx Reset
	CCU->EMAC_BGR_REG |= (UINT32_C(1) << ((16 + ix)));	// EMACx Reset
	//PRINTF("CCU->EMAC_BGR_REG=%08X (@%p)\n", (unsigned) CCU->EMAC_BGR_REG, & CCU->EMAC_BGR_REG);

	//CCU->EMAC_25M_CLK_REG |= (UINT32_C(1) << 31) | (UINT32_C(1) << 30);	// moved to HARDWARE_ETH_INITIALIZE

	HARDWARE_ETH_INITIALIZE();	// Должно быть тут - снять ресет с PHY до инициализации
}

static void allwinner_emac_phy_init(EMAC_TypeDef * const emac_peripheral)
{
	// The working clock of EMAC is from AHB3.
#if (CPUSTYLE_T507)
	HARDWARE_EMAC_EPHY_CLK_REG =
		0x00051c06 | // 0x00051c06 0x00053c01
		0;
	//PRINTF("EMAC_BASIC_CTL1=%08X\n", (unsigned) emac_peripheral->EMAC_BASIC_CTL1);
	//printhex32((uintptr_t) HARDWARE_EMAC_PTR, HARDWARE_EMAC_PTR, 256);
#elif (CPUSTYLE_T113 || CPUSTYLE_F133)
	HARDWARE_EMAC_EPHY_CLK_REG =
		1 * (UINT32_C(1) << 13) |
		0;
#endif
	// Сигнал phyrstb тут уже должен бьыть неактивен

	// Ожидание в 100 мс мало
	emac_peripheral->EMAC_BASIC_CTL1 |= (UINT32_C(1) << 0);	// Soft reset
	if (local_wait32mask(& emac_peripheral->EMAC_BASIC_CTL1, (UINT32_C(1) << 0), 0 * (UINT32_C(1) << 0), 1000))
		TP();
//	while ((emac_peripheral->EMAC_BASIC_CTL1 & (UINT32_C(1) << 0)) != 0)
//		;

	emac_peripheral->EMAC_BASIC_CTL0 =
		//0x03 * (UINT32_C(1) << 2) |	// SPEED - 00: 1000 Mbit/s, 10: 10 Mbit/s, 11: 100 Mbit/s
		//0x01 * (UINT32_C(1) << 0) | // DUPLEX - 1: Full-duplex
		0;
	emac_peripheral->EMAC_BASIC_CTL1 =
		0x08 * (UINT32_C(1) << 24) |	// BURST_LEN - The burst length of RX and TX DMA transfer.
		0;

//			PRINTF("EMAC_BASIC_CTL0=%08X\n", (unsigned) emac_peripheral->EMAC_BASIC_CTL0);
//			PRINTF("EMAC_BASIC_CTL1=%08X\n", (unsigned) emac_peripheral->EMAC_BASIC_CTL1);
//			PRINTF("EMAC_RGMII_STA=%08X\n", (unsigned) emac_peripheral->EMAC_RGMII_STA);


//	const uint8_t hwaddr [6] = { HWADDR };
//	// ether 1A:0C:74:06:AF:64
////		emac_peripheral->EMAC_ADDR [0].HIGH = 0x000064AF;
////		emac_peripheral->EMAC_ADDR [0].LOW = 0x06740C1A;
//	emac_peripheral->EMAC_ADDR [0].HIGH = USBD_peek_u16(hwaddr + 4);	// upper 16 bits of the first 6-byte MAC address
//	emac_peripheral->EMAC_ADDR [0].LOW = USBD_peek_u32(hwaddr + 0);	// lower 32 bits of the 6-byte first MAC address

	//emac_mdio_write(RTL8211F_PHY_ADDR, RTL8211F_IER, insr_mask);
}

static err_t allwinner_emac_init_port0(EMAC_TypeDef *emac_peripheral, struct netif *netif, uint8_t port_index)
{
    if (port_index > 1) {
        return ERR_ARG;
    }

	// RX init
	unsigned i;
	for (i = 0; i < ARRAY_SIZE(emac_rxdesc); ++ i)
	{
		struct emac_dma_desc * rxd = & emac_rxdesc [i];
		rxd->status =
			1 * (UINT32_C(1) << 31) |	// RX_DESC_CTL
//				1 * (UINT32_C(1) << 9) |	// FIR_DESC
//				1 * (UINT32_C(1) << 8) |	// LAST_DESC
			0;
		rxd->control =
				//1 * (UINT32_C(1) << 31) |	// RX_INT_CTL
				//DESC_RX_CHAINED |
				(EMAC_MAX_PACKET_SIZE & DESC1_RX_BUF_SIZE_MASK) |	// 10:0 BUF_SIZE
			0;
		rxd->buf_addr = (uintptr_t) rxbuffs [i];	// BUF_ADDR
		rxd->next_desc = (uintptr_t) & emac_rxdesc [(i + 1) % ARRAY_SIZE(emac_rxdesc)];	// NEXT_DESC_ADDR

	}
	emac_peripheral->EMAC_RX_FRM_FLT =
//					1 * (UINT32_C(1) << 31) |	// DIS_ADDR_FILTER
			1 * (UINT32_C(1) << 0) |	// RX_ALL
			0;

	emac_peripheral->EMAC_RX_DMA_DESC_LIST = (uintptr_t) emac_rxdesc;
	dcache_clean_invalidate((uintptr_t) emac_rxdesc, sizeof emac_rxdesc);
	dcache_clean_invalidate((uintptr_t) rxbuffs, sizeof rxbuffs);

	// TX init
	InitializeListHead(& TxList);
	//InitializeListHead(& TxDoneList);

//	for (i = 0; i < ARRAY_SIZE(emac_txdesc); ++ i)
//	{
//		struct emac_dma_desc * txd = & emac_txdesc [i];
//		txd->status = 0;
//		txd->control =
//				//len * (UINT32_C(1) << 0) |	// 10:0 BUF_SIZE
//			0;
//		txd->buf_addr = (uintptr_t) txbuffs [i];	// BUF_ADDR
//		txd->next_desc = (uintptr_t) & emac_txdesc [(i + 1) % ARRAY_SIZE(emac_txdesc)];	// NEXT_DESC_ADDR
//		txd->next_desc = (uintptr_t) & emac_txdesc [i];	// NEXT_DESC_ADDR
//
//	}
//	dcache_clean((uintptr_t) emac_txdesc, sizeof emac_txdesc);
//	dcache_clean((uintptr_t) txbuffs, sizeof txbuffs);

    /* Setup safe default speed and duplex in Basic configuration */
    //emac_peripheral->EMAC_BASIC_CTL0 = EMAC_BASIC_CTL0_SPEED_100 | EMAC_BASIC_CTL0_DUPLEX;

    /* Generate and program the unique port hardware MAC address derived from eFuses */
    allwinner_emac_apply_mac_dual(emac_peripheral, netif, port_index);

    {
    	// RX

		//emac_peripheral->EMAC_RX_CTL0 = 0xb8000000;
		emac_peripheral->EMAC_RX_CTL0 =
			1 * (UINT32_C(1) << 31) |	// RX_EN
			//1 * (UINT32_C(1) << 29) |	// JUMBO_FRM_EN
			//1 * (UINT32_C(1) << 28) |	// STRIP_FCS
	//		1 * (UINT32_C(1) << 27) |	// CHECK_CRC 1: Calculate CRC and check the IPv4 Header Checksum.
			0;
		emac_peripheral->EMAC_RX_CTL1 =
				1 * (UINT32_C(1) << 1) |	// 1: RX start read after RX DMA FIFO located a full frame
				EMAC_RX_CTL1_RX_MD_FORW |
				1 * (UINT32_C(1) << 30) |	// /RX_DMA_EN
				0;

		emac_peripheral->EMAC_RX_CTL1 |= (UINT32_C(1) << 31);	// RX_DMA_START (auto-clear)
		if (local_wait32mask(& emac_peripheral->EMAC_RX_CTL1, (UINT32_C(1) << 31), 0 * (UINT32_C(1) << 31), 100))
			TP();

    }
    {
    	// TX
		//emac_peripheral->EMAC_TX_DMA_DESC_LIST = (uintptr_t) emac_txdesc;
    	emac_peripheral->EMAC_TX_CTL1 = EMAC_TX_CTL1_TX_MD_FORW;

    	emac_peripheral->EMAC_TX_CTL0 =
    		1 * (UINT32_C(1) << 31) |	// TX_EN
    		//1 * (UINT32_C(1) << 30) |	// TX_FRM_LEN_CTL
    		0;
    }

	emac_peripheral->EMAC_INT_EN |= (UINT32_C(1) << 0); // TX_INT_EN
	emac_peripheral->EMAC_INT_EN |= (UINT32_C(1) << 8); // RX_INT_EN

	//emac_peripheral->EMAC_INT_EN |= (UINT32_C(1) << 10); // RX_DMA_STOPPED_INT_EN
	//emac_peripheral->EMAC_INT_EN |= (UINT32_C(1) << 1); // TX_DMA_STOPPED_INT_EN

	//emac_peripheral->EMAC_INT_EN |= ~ UINT32_C(0);

    return ERR_OK;
}

//static void board_nic_dpc(void * ctx)
//{
//	struct netif * const netif = (struct netif *) ctx;
//	ethernetif_poll(netif);
//}

// 1 s periodic calls or by EMAC interrupt flag
void nic_linkspool(void * ctx)
{
	EMAC_TypeDef * const emac_peripheral = HARDWARE_EMAC_PTR;
	struct netif * const netif = (struct netif *) ctx;
	check_ethernet_link_status(netif);

//	PRINTF("EMAC_TX_CTL1=%08X\n", (unsigned) emac_peripheral->EMAC_TX_CTL1);
//	PRINTF("EMAC_TX_DMA_STA=%08X\n", (unsigned) emac_peripheral->EMAC_TX_DMA_STA);
//	stoptxdma(emac_peripheral);		// возможно удаляли
//	relinktxdesc(emac_peripheral);
//	starttxdma(emac_peripheral);
}

void nic_initialize(struct netif *netif)
{
	//PRINTF("nic_initialize start\n");
	allwinner_emac_ccu_init();
	allwinner_emac_phy_init(HARDWARE_EMAC_PTR);

	if (1)
	{
		allwinner_emac_init_port0(HARDWARE_EMAC_PTR, netif, HARDWARE_EMAC_IX);
		dpcobj_initialize(& dpcrxirq, emac_rxhandler, netif);
		dpcobj_initialize(& dpctxirq, emac_txhandler, netif);
		dpcobj_initialize(& dpclinkspoolirq, nic_linkspool, netif);
		arm_hardware_set_handler_system(HARDWARE_EMAC_IRQ, EMAC_Handler);

//		static dpcobj_t nic_dpc_entry;
//		dpcobj_initialize(& nic_dpc_entry, emac_nohandler, netif);
//		board_dpc_addentry(& nic_dpc_entry, board_dpc_coreid());

		netif->linkoutput = low_level_output; // используется внутри etharp_output
	}
	else
	{
//		//allwinner_emac_init_port0(HARDWARE_EMAC_PTR, netif, HARDWARE_EMAC_IX);
//		allwinner_emac_init_port(HARDWARE_EMAC_PTR, netif, HARDWARE_EMAC_IX);
//
//		{
//			static dpcobj_t nic_dpc_entry;
//			dpcobj_initialize(& nic_dpc_entry, board_nic_dpc, netif);
//			board_dpc_addentry(& nic_dpc_entry, board_dpc_coreid());
//
//		}
//		netif->linkoutput = low_level_output; // используется внутри etharp_output
	}
		//PRINTF("nic_initialize done\n");

}

void nic_set_mac(void * ctx)
{
	struct netif * const netif = (struct netif *) ctx;
//	uint8_t hwaddr [6];
//	allwinner_get_mac_from_sid_dual(hwaddr, HARDWARE_EMAC_IX);
//	memcpy(netif->hwaddr, hwaddr, 6);
	allwinner_emac_apply_mac_dual(HARDWARE_EMAC_PTR, netif, HARDWARE_EMAC_IX);
}


/* Параметры интерфейса для отображения в меню */
size_t getvaltextethmacaddr(char * buff, size_t count, int_fast32_t value)
{
	//struct netif * const netif = & nic_netif_data;
	(void) value;
    uint8_t b[6];

    /* Fetch the unique MAC address with port-specific byte shifting */
    allwinner_get_mac_from_sid_dual(b, HARDWARE_EMAC_IX);
	return local_snprintf_P(buff, count, "%02X:%02X:%02X:%02X:%02X:%02X", b [0], b [1], b [2], b [3], b [4], b [5]);
}

#endif /* WITHLWIP && WITHETHHW && (CPUSTYLE_T507) */
