/*
 * usbd_cdc.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
 *
 * Много взято
 * https://github.com/rawaaw/cdc-eem-f103.git
 *
 * see also https://www.belcarra.com/2012/01/belcarra-eem-configuration-extension.html
*/

#include "hardware.h"

#if WITHUSBHW && WITHUSBCDCEEM

#include "buffers.h"
#include "formats.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usb200.h"
#include "usbch9.h"


typedef void (*cdcdeem_rxproc_t)(const uint8_t *data, int size);

static cdcdeem_rxproc_t cdceem_rxproc = NULL;

#define BPOOL_FLAG_BPOOL_FULL 0x00000001

typedef enum _eem_bpool_idx_enum {
  EEM_RX_BUFFER = 0,
  EEM_TX_BUFFER = 1,
  EEM_RX_DUMMY_BUFFER = 2
}t_eem_bpool_idx_enum;


int init_eem_pkt_bpool(t_eem_bpool_idx_enum bidx);
int deinit_eem_pkt_bpool(t_eem_bpool_idx_enum bidx);

uint8_t* alloc_eem_pkt_buffer(t_eem_bpool_idx_enum bidx,
                              uint32_t n_pkt_received,
                              uint32_t n_pkt_sent,
                              uint32_t*buf_size,
                              uint8_t *prev_buf,
                              uint32_t*bpool_full);

uint8_t* get_eem_pkt_buffer(t_eem_bpool_idx_enum bidx,
                            uint32_t n_pkt,
                            uint32_t *buf_size);

uint32_t get_eem_pkt_size(uint8_t *buffer);

void set_eem_pkt_size(uint8_t *buffer, uint32_t pkt_size);

/*---------------------------------------------------------------------*/
/*  CDC definitions                                                    */
/*---------------------------------------------------------------------*/
#if 0
#define CDC_SEND_ENCAPSULATED_COMMAND               0x00
#define CDC_GET_ENCAPSULATED_RESPONSE               0x01
#define CDC_SET_COMM_FEATURE                        0x02
#define CDC_GET_COMM_FEATURE                        0x03
#define CDC_CLEAR_COMM_FEATURE                      0x04
#define CDC_SET_LINE_CODING                         0x20
#define CDC_GET_LINE_CODING                         0x21
#define CDC_SET_CONTROL_LINE_STATE                  0x22
#define CDC_SEND_BREAK                              0x23
#endif

/* eem buffer size */
#define EEM_RX_DATA_SIZE  1524
#define EEM_TX_DATA_SIZE  1524

#define CDC_EEM_CMD_PACKET_SIZE                         64  /* Control Endpoint Packet size */ // ??? 8

#define EEM_RX_BUF_CNT 3
#define EEM_TX_BUF_CNT 1

/*
extern uint8_t UserRxBufferFS[EEM_RX_DATA_SIZE];
extern uint8_t UserTxBufferFS[EEM_TX_DATA_SIZE];
*/
static uint8_t UserRxBufferFS[EEM_RX_DATA_SIZE];
static uint8_t UserTxBufferFS[EEM_TX_DATA_SIZE];


/**
  * @}
  */


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

/**
  * @}
  */
/*
typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
}USBD_CDC_EEM_LineCodingTypeDef;
*/

typedef struct _USBD_CDC_EEM_Itf
{
  int8_t (* Init)          (void);
  int8_t (* DeInit)        (void);
  int8_t (* Control)       (uint8_t, uint8_t * , uint16_t);
  int8_t (* Receive)       (uint8_t *, uint32_t *);

} USBD_CDC_EEM_ItfTypeDef;


typedef struct
{
  uint32_t data [CDC_EEM_CMD_PACKET_SIZE/4]; /* control transfers */     /* Force 32bits alignment */
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;

  uint8_t  *tx_buffer;
  uint8_t  *rx_buffer;

  uint32_t tx_buf_size;
  uint32_t tx_buf_rd_pos;
  uint32_t tx_length;

  uint32_t rx_buf_size;
  uint32_t rx_buf_wr_pos;

  __IO uint32_t tx_state;
  __IO uint32_t rx_state;

  __IO uint64_t rx_cnt_received;
  __IO uint64_t rx_cnt_processed;
  __IO uint64_t rx_cnt_sending;
  __IO uint64_t rx_cnt_sent;
}
USBD_CDC_EEM_HandleTypeDef;

typedef struct
{
  uint32_t data[USBD_CDCEEM_BUFSIZE/4];      /* Force 32bits alignment */
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;
  uint8_t  *RxBuffer;
  uint8_t  *TxBuffer;
  uint32_t RxLength;
  uint32_t TxLength;

  __IO uint32_t TxState;
  __IO uint32_t RxState;
}
USBD_CDC_HandleTypeDef;


typedef enum _eem_receiver_state_enum {
  EEM_RECEIVER_INITIAL          = 0,
  EEM_RECEIVER_CMD_IN_PROGRESS  = 1,
  EEM_RECEIVER_CMD_DONE         = 2,
  EEM_RECEIVER_DATA_IN_PROGRESS = 3,
  EEM_RECEIVER_DATA_DONE        = 4,

  EEM_RECEIVER_FAULT            = 0xFF
}eem_receiver_state_enum;


typedef enum _eem_transmitter_state_enum {
  EEM_TRANSMITTER_INITIAL       = 0,
  EEM_TRANSMITTER_SEND_DATA     = 1,
  EEM_TRANSMITTER_SENDING_ZPKT  = 2,
  EEM_TRANSMITTER_SENT_ZPKT     = 3,
  EEM_TRANSMITTER_SEND_EEM_ZPKT = 4,

  EEM_TRANSMITTER_FAULT         = 0xFF
}eem_transmitter_state_enum;

typedef enum _eem_packet_type_enum {
  EEM_DATA_PACKET = 0,
  EEM_CMD_PACKET =1
}eem_packet_type_enum;


/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */

//extern USBD_ClassTypeDef  USBD_CDC_EEM;
//#define USBD_CDC_EEM_CLASS    &USBD_CDC_EEM
/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
uint8_t  USBD_CDC_EEM_RegisterInterface  (USBD_HandleTypeDef   *pdev,
                                      USBD_CDC_EEM_ItfTypeDef *fops);

#if 1
uint8_t  USBD_CDC_EEM_SetTxBuffer        (USBD_HandleTypeDef   *pdev,
                                      uint8_t  *pbuff,
                                      uint16_t buf_size,
                                      uint16_t length
                                         );

uint8_t  USBD_CDC_EEM_SetRxBuffer        (USBD_HandleTypeDef   *pdev,
                                      uint8_t  *pbuff,
                                      uint16_t buf_size);
#endif

#if 1
uint8_t  USBD_CDC_EEM_ReceivePacket      (USBD_HandleTypeDef *pdev,
                                          uint8_t*pbuf);
#endif

uint8_t  USBD_CDC_EEM_TransmitPacket     (USBD_HandleTypeDef *pdev,
                                          eem_transmitter_state_enum tx_state);
/**
  * @}
  */

/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/* Received Data over USB are stored in this buffer       */
static __ALIGN_BEGIN uint8_t eem_rx_buffer_pool_fs[EEM_RX_DATA_SIZE * EEM_RX_BUF_CNT] __ALIGN_END;
static __ALIGN_BEGIN uint8_t eem_tx_buffer_pool_fs[EEM_TX_DATA_SIZE * EEM_TX_BUF_CNT] __ALIGN_END;

static uint8_t* get_bpool(t_eem_bpool_idx_enum bidx, uint32_t *bpool_size){
  if (bidx == EEM_RX_BUFFER){
    *bpool_size = EEM_RX_BUF_CNT;
    return eem_rx_buffer_pool_fs;
  }else if (bidx == EEM_TX_BUFFER){
    *bpool_size = EEM_TX_BUF_CNT;
    return eem_tx_buffer_pool_fs;
  }else{
    *bpool_size = 0;
    return NULL;
  }
}

int init_eem_pkt_bpool(t_eem_bpool_idx_enum bidx){
  uint32_t bpool_size;
  uint8_t *pbpool = get_bpool(bidx, &bpool_size);
  memset(pbpool, 0, bpool_size * EEM_RX_DATA_SIZE);
  return 0;
}

int deinit_eem_pkt_bpool(t_eem_bpool_idx_enum bidx){
  return 0;
}

uint8_t* alloc_eem_pkt_buffer(t_eem_bpool_idx_enum bidx,
                              uint32_t n_pkt_received,
                              uint32_t n_pkt_sent,
                              uint32_t *buf_size,
                              uint8_t  *prev_buf,
                              uint32_t *bpool_full){
  uint32_t bpool_size;
  uint8_t *pbpool = get_bpool(bidx, &bpool_size);
  if (bidx == EEM_RX_BUFFER){
    *buf_size = EEM_RX_DATA_SIZE;
    if (n_pkt_received - n_pkt_sent >= EEM_RX_BUF_CNT){
      *bpool_full = 1;
      return prev_buf;
    }
    *bpool_full = 0;
    return &pbpool[EEM_RX_DATA_SIZE * (n_pkt_received % EEM_RX_BUF_CNT)];
  }else if (bidx == EEM_TX_BUFFER){
    *buf_size = EEM_TX_DATA_SIZE;
    *bpool_full = 1;
    return prev_buf;
  }
  return NULL;
}


uint8_t* get_eem_pkt_buffer(t_eem_bpool_idx_enum bidx,
                            uint32_t n_pkt,
                            uint32_t *buf_size){
  uint32_t bpool_size;
  uint8_t *pbpool = get_bpool(bidx, &bpool_size);

  if (bidx == EEM_RX_BUFFER){
    if (buf_size)
      *buf_size = EEM_RX_DATA_SIZE;
    return &pbpool[EEM_RX_DATA_SIZE * (n_pkt % EEM_RX_BUF_CNT)];
  }else if (bidx == EEM_TX_BUFFER){
    if (buf_size)
      *buf_size = EEM_TX_DATA_SIZE;
    return &pbpool[EEM_TX_DATA_SIZE * (n_pkt % EEM_TX_BUF_CNT)];
  }
  return NULL;
}

uint32_t get_eem_pkt_size(uint8_t *buffer){
  return (uint32_t)buffer[0] | ((((uint32_t)buffer[1]) & 0x3F) << 8);
}


void set_eem_pkt_size(uint8_t *buffer, uint32_t pkt_size){
  buffer[0] = (uint8_t)(pkt_size & 0xFF);
  buffer[1] = (uint8_t)((pkt_size >> 8) & 0x3F);
  return;
}


#define CDCEEM_MTU 1500  // MTU value

static volatile unsigned eemtxready;
static volatile unsigned eemtxleft;
static uint8_t  * volatile eemtxpointer;
static uint8_t eemtxbuffer [8192];

static int cdceem_can_send(void)
{
	return eemtxready;
}

static void cdceem_send(const uint8_t *data, int size)
{
	// прербразуем пакет в CDC EEM пакеты
	uint8_t * tg = & eemtxbuffer [0];

	if (size == 0)
		return;

	uint_fast16_t header =
			(0 << 15) |		// bmType. Set to 0
			(0 << 14) |		// D14: bmCRC
			(((size + 4) << 0) & 0x3FFF) |
			0;
	uint_fast32_t crc = 0xDEADBEEF;

	* tg ++ = (header >> 8) & 0xFF;
	* tg ++ = (header >> 0) & 0xFF;

	memcpy(tg, data, size);
	tg += size;

	* tg ++ = (crc >> 24) & 0xFF;
	* tg ++ = (crc >> 16) & 0xFF;
	* tg ++ = (crc >> 8) & 0xFF;
	* tg ++ = (crc >> 0) & 0xFF;

	// а теперь передаем
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	eemtxleft = tg - eemtxbuffer;
	eemtxpointer = eemtxbuffer;
	eemtxready = 0;
	LowerIrql(oldIrql);
}

// CDC class-specific request codes
// (usbcdc11.pdf, 6.2, Table 46)
// see Table 45 for info about the specific requests.
#define CDC_SET_CONTROL_LINE_STATE              0x22

static USBALIGN_BEGIN uint8_t cdceem1buffout [USBD_CDCEEM_BUFSIZE] USBALIGN_END;
static USBALIGN_BEGIN uint8_t cdceem1buffin [USBD_CDCEEM_BUFSIZE] USBALIGN_END;
static RAMDTCM uint_fast16_t cdceem1buffinlevel;
static USBALIGN_BEGIN uint8_t cdceem_epXdatabuffout [USB_OTG_MAX_EP0_SIZE] USBALIGN_END;



/* Собираем поток из CDC EEM USB пакетов */
// construct EEM packets
enum
{
	CDCEEMOUT_COMMAND,
	CDCEEMOUT_DATA,
	CDCEEMOUT_CRC,
	//
	CDCEEMOUT_nstates
};

static uint_fast8_t cdceemoutstate = CDCEEMOUT_COMMAND;
static uint_fast32_t cdceemoutscore = 0;
static uint_fast32_t cdceemoutacc;
static uint_fast32_t cdceemlength;
static uint_fast32_t cdceematcrc;
static uint_fast32_t cdceemnpackets;

// see SIZEOF_ETHARP_PACKET
static uint8_t cdceemrxbuff [1514];

static void cdceemout_initialize(void)
{
	cdceemoutstate = CDCEEMOUT_COMMAND;
	cdceemoutscore = 0;
}

static void cdceemout_buffer_print(
	const uint8_t * data,
	uint_fast16_t length
	)
{
	// debug print
	uint_fast16_t pos;
	for (pos = 0; pos < length; ++ pos)
	{
		PRINTF(PSTR("%02X%s"), data [pos], ((pos + 1) % 16) == 0 ? ",\n" : ", ");
	}
	PRINTF(PSTR("\n"));
}

// Ethernet II frame print (64..1518 bytes)
static void cdceemout_buffer_print2(
	const uint8_t * data,
	uint_fast16_t length
	)
{
	// +++ MAC header
	PRINTF(PSTR("Dst MAC = %02X:%02X:%02X:%02X:%02X:%02X, "), data [0], data [1], data [2], data [3], data [4], data [5]);
	PRINTF(PSTR("Src MAC = %02X:%02X:%02X:%02X:%02X:%02X, "), data [6], data [7], data [8], data [9], data [10], data [11]);
	const uint_fast16_t EtherType = data [12] * 256 + data [13];	// 0x0800
	PRINTF(PSTR("EtherType %04X\n"), EtherType);
	// --- MAC header
	data += 14;
	length = length >= 14 ? length - 14 : 0;

	switch (EtherType)
	{
	case 0x0800:
		// IPv4 datagram
		PRINTF(PSTR("IPv4 datagram: "));
		PRINTF(PSTR("Protocol=%02X, "), data [9]);	// 0x11 == UDP,
		PRINTF(PSTR("Src IP=%d.%d.%d.%d, "), data [12], data [13], data [14], data [15]);
		PRINTF(PSTR("Dst IP=%d.%d.%d.%d\n"), data [16], data [17], data [18], data [19]);
		{
		}
		break;

	case 0x86DD:
		// IPv6 datagram
		//PRINTF(PSTR("IPv6 datagram\n"));
		break;

	case 0x0806:
		// ARP frame
		//PRINTF(PSTR("ARP frame\n"));
		break;

	case 0x88CC:
		// IEEE Std 802.1AB - Link Layer Discovery Protocol (LLDP)s
		//PRINTF(PSTR("LLDP\n"));
		break;

	default:
		{
			// pyloaddebug print
			uint_fast16_t pos;
			for (pos = 0; pos < length; ++ pos)
			{
				PRINTF(PSTR("%02X%s"), data [pos], ((pos + 1) % 16) == 0 ? ",\n" : ", ");
			}
			PRINTF(PSTR("\n"));
		}
		break;
	}


}

static USBALIGN_BEGIN uint8_t dbd [2] USBALIGN_END;

// called in context of interrupt
static void cdceemout_buffer_save(
	const uint8_t * data,
	uint_fast16_t length
	)
{
	uint_fast16_t pos;
	//PRINTF("cdceemout_buffer_save: EP len=%u\n", length);
	for (pos = 0; pos < length; )
	{
		switch (cdceemoutstate)
		{
		case CDCEEMOUT_COMMAND:
			cdceemoutacc = cdceemoutacc * 256 + data [pos ++];
			if (++ cdceemoutscore >= 2)
			{
				const uint_fast8_t w = (cdceemoutacc & 0xFFFF);
				cdceemoutscore = 0;
				// разбор команды
				const uint_fast8_t bmType = (w >> 15) & 0x0001;	// 0: EEM data payload 1: EEM Command
				if (bmType == 0)
				{
					const uint_fast8_t bmCRC = (w >> 14) & 0x0001;	// 0: Ethernet Frame CRC is set to 0xdeadbeef
					cdceemlength = (w >> 0) & 0x3FFF;					// размер включая 4 байта СКС
					if (cdceemlength > 0)
					{
						if (cdceemlength >= 4)
						{
							cdceematcrc = cdceemlength - 4;
							cdceemoutstate = CDCEEMOUT_DATA;
						}
					}
					//PRINTF(PSTR("Data: bmCRC=%02X, cdceemlength=%u, pyload=0x%04X\n"), bmCRC, cdceemlength, cdceematcrc);
				}
				else
				{
					const uint_fast8_t bmEEMCmd = (w >> 11) & 0x0007;	// 0: Ethernet Frame CRC is set to 0xdeadbeef
					const uint_fast16_t bmEEMCmdParam = (w >> 11) & 0x07FF;
					PRINTF(PSTR("Command: bmEEMCmd=%02X, bmEEMCmdParam=%u\n"), bmEEMCmd, bmEEMCmdParam);
				}
			}
			break;

		case CDCEEMOUT_DATA:
			{
				const uint_fast16_t chunk = ulmin16(length - pos, cdceematcrc - cdceemoutscore);
				memcpy(cdceemrxbuff + cdceemoutscore, data + pos, ulmin16(sizeof cdceemrxbuff - cdceemoutscore, chunk)); // use data bytes
				pos += chunk;
				if ((cdceemoutscore += chunk) >= cdceematcrc)
				{
					cdceemoutscore = 0;
					cdceemoutstate = CDCEEMOUT_CRC;
				}
			}
			break;

		case CDCEEMOUT_CRC:
			cdceemoutacc = cdceemoutacc * 256 + data [pos ++];
			if (++ cdceemoutscore >= 4)
			{
				cdceemoutscore = 0;
				cdceemoutstate = CDCEEMOUT_COMMAND;

				++ cdceemnpackets;
				//PRINTF(PSTR("CDCEEMOUT packets=%lu\n"), (unsigned long) cdceemnpackets);

				//PRINTF(PSTR("crc=%08lX\n"), (cdceemoutacc & 0xFFFFFFFF));
				// Тут полностью собран ethernet пакет, используем его (или например печатаем содержимое).
				//PRINTF("cdceemout_buffer_save: LWIP len=%u\n", cdceematcrc);
				//cdceemout_buffer_print2(cdceemrxbuff, cdceematcrc);
#if WITHLWIP
				// Save to LWIP
				{
					if (cdceem_rxproc != NULL)
						cdceem_rxproc(cdceemrxbuff, cdceematcrc);

				}

#elif 0
				// Отладочная печать
				PRINTF(PSTR("Data pyload length=0x%04X\n"), cdceematcrc);
				//cdceemout_buffer_print(cdceemrxbuff, cdceematcrc);
				cdceemout_buffer_print2(cdceemrxbuff, cdceematcrc);
#endif
			}
		}

	}

}

/*
* @param  epnum: endpoint index without direction bit
*
 */
static USBD_StatusTypeDef USBD_CDCEEM_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	//PRINTF("USBD_CDCEEM_DataIn: epnum=%d\n", (int) epnum);
	switch (epnum)
	{
	case (USBD_EP_CDCEEM_IN & 0x7F):
		//USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), cdceem1buffin, cdceem1buffinlevel);
		//cdceem1buffinlevel = 0;
		if (eemtxready == 0 && eemtxleft != 0)
		{
			const unsigned eemtxsize = ulmin32(eemtxleft, USBD_CDCEEM_BUFSIZE);
			USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), eemtxpointer, eemtxsize);
			eemtxpointer += eemtxsize;
			eemtxleft -= eemtxsize;
		}
		else
		{
			USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), dbd, sizeof dbd);
			eemtxready = 1;
		}
		break;
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDCEEM_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	switch (epnum)
	{
	case USBD_EP_CDCEEM_OUT:
		/* EEM EP OUT */
		// use CDC EEM data
		cdceemout_buffer_save(cdceem1buffout, USBD_LL_GetRxDataSize(pdev, epnum));	/* использование буфера принятых данных */
		//memcpy(cdc1buffin, cdc1buffout, cdc1buffinlevel = USBD_LL_GetRxDataSize(pdev, epnum));
		/* Prepare Out endpoint to receive next cdc data packet */
		USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(epnum), cdceem1buffout, USBD_CDCEEM_BUFSIZE);
		break;
	default:
		break;
	}
	return USBD_OK;
}

// При возврате из этой функции в usbd_core.c происходит вызов USBD_CtlSendStatus
static USBD_StatusTypeDef USBD_CDCEEM_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	const USBD_SetupReqTypedef * const req = & pdev->request;

	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	//PRINTF(PSTR("1 USBD_CDC_EP0_RxReady: interfacev=%u: bRequest=%u, wLength=%u\n"), interfacev, req->bRequest, req->wLength);
	switch (interfacev)
	{
	case INTERFACE_CDCEEM_DATA:	// CDC EEM interface
		{
			switch (req->bRequest)
			{
//			case CDC_SET_LINE_CODING:
//				{
////					const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
////					ASSERT(req->wLength == 7);
////					dwDTERate [interfacev] = USBD_peek_u32(& cdceem_epXdatabuffout [0]);
//					//PRINTF(PSTR("USBD_CDCEEM_EP0_RxReady: CDC_SET_LINE_CODING: interfacev=%u, dwDTERate=%lu, bits=%u\n"), interfacev, dwDTERate [interfacev], cdceem_epXdatabuffout [6]);
//				}
//				break;
			default:
				// непонятно, для чего эти данные?
				TP();
				break;
			}
		}
		break;

	default:
		break;
	}
	return USBD_OK;
}


static USBD_StatusTypeDef USBD_CDCEEM_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	static USBALIGN_BEGIN uint8_t buff [32] USBALIGN_END;	// was: 7
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	if ((req->bmRequest & USB_REQ_TYPE_DIR) != 0)
	{
		// IN direction
		switch (req->bmRequest & USB_REQ_TYPE_MASK)
		{
		case USB_REQ_TYPE_CLASS:
			switch (interfacev)
			{

			case INTERFACE_CDCEEM_DATA:	// CDC EEM interface
				{
					switch (req->bRequest)
					{
//					case CDC_GET_LINE_CODING:
//						PRINTF(PSTR("USBD_CDC_Setup IN: CDC_GET_LINE_CODING, dwDTERate=%lu\n"), (unsigned long) dwDTERate [interfacev]);
//						USBD_poke_u32(& buff [0], dwDTERate [interfacev]); // dwDTERate
//						buff [4] = 0;	// 1 stop bit
//						buff [5] = 0;	// parity=none
//						buff [6] = 8;	// bDataBits
//
//						USBD_CtlSendData(pdev, buff, ulmin16(7, req->wLength));
						break;

					default:
						TP();
						PRINTF(PSTR("USBD_CDCEEM_Setup IN: bRequest=%02X, wValue=%04X\n"), (unsigned) req->bRequest, (unsigned) req->wValue);
						USBD_CtlError(pdev, req);
						break;
					}
				}
				break;

			default:
				break;
			}
			break;

		case USB_REQ_TYPE_STANDARD:
			switch (interfacev)
			{

			case INTERFACE_CDCEEM_DATA:	// CDC EEM interface
				{
					switch (req->bRequest)
					{
						case USB_REQ_GET_INTERFACE:
						{
							static USBALIGN_BEGIN uint8_t buff [64] USBALIGN_END;
							//PRINTF(PSTR("USBD_CDC_Setup: USB_REQ_TYPE_STANDARD USB_REQ_GET_INTERFACE dir=%02X interfacev=%d, req->wLength=%d\n"), req->bmRequest & 0x80, interfacev, (int) req->wLength);
							buff [0] = 0;
							USBD_CtlSendData(pdev, buff, ulmin16(ARRAY_SIZE(buff), req->wLength));
						}
						break;
					}
				}
			}
			break;
		}
	}
	else
	{
		// OUT direction
		switch (req->bmRequest & USB_REQ_TYPE_MASK)
		{
		case USB_REQ_TYPE_CLASS:
			switch (interfacev)
			{
			case INTERFACE_CDCEEM_DATA:	// CDC EEM interface
				switch (req->bRequest)
				{
				case CDC_SET_CONTROL_LINE_STATE:
					// Выполнение этого запроса не требует дополнительного чтения данных
					//PRINTF(PSTR("USBD_CDCEEM_Setup OUT: CDC_SET_CONTROL_LINE_STATE, wValue=%04X, wLength=%04X\n"), req->wValue, (unsigned) req->wLength);
//					usb_cdc_control_state [interfacev] = req->wValue;
					ASSERT(req->wLength == 0);
					break;

				default:
					TP();
					PRINTF(PSTR("USBD_CDCEEM_Setup OUT: bRequest=%02X, wValue=%04X, wLength=%04X\n"), (unsigned) req->bRequest, (unsigned) req->wValue, (unsigned) req->wLength);
					break;
				}
				/* все запросы этого класса устройств */
				if (req->wLength != 0)
				{
					USBD_CtlPrepareRx(pdev, cdceem_epXdatabuffout, ulmin16(ARRAY_SIZE(cdceem_epXdatabuffout), req->wLength));
				}
				else
				{
					USBD_CtlSendStatus(pdev);
				}
				break;

			default:
				break;
			}
			break;

		case USB_REQ_TYPE_STANDARD:
			switch (req->bRequest)
			{
			case USB_REQ_SET_INTERFACE:
				switch (interfacev)
				{
				case INTERFACE_CDCEEM_DATA:	// CDC EEM interface
					// Only zero value here
					//altinterfaces [interfacev] = LO_BYTE(req->wValue);
					PRINTF("USBD_CDCEEM_Setup: CDC interface %d set to %d\n", (int) interfacev, (int) LO_BYTE(req->wValue));
					//bufers_set_cdcalt(altinterfaces [interfacev]);
					USBD_CtlSendStatus(pdev);
					break;

				default:
					// Другие интерфейсы - ничего не отправляем.
					//TP();
					break;
				}
			}
			break;

		default:
			break;
		}
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDCEEM_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	/* CDC EEM Open EP IN */
	USBD_LL_OpenEP(pdev, USBD_EP_CDCEEM_IN, USBD_EP_TYPE_BULK, USBD_CDCEEM_BUFSIZE);
	USBD_LL_Transmit(pdev, USBD_EP_CDCEEM_IN, NULL, 0);
	/* CDC EEM Open EP OUT */
	USBD_LL_OpenEP(pdev, USBD_EP_CDCEEM_OUT, USBD_EP_TYPE_BULK, USBD_CDCEEM_BUFSIZE);
    /* CDC EEM Prepare Out endpoint to receive 1st packet */
    USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(USBD_EP_CDCEEM_OUT), cdceem1buffout,  USBD_CDCEEM_BUFSIZE);

    cdceemoutstate = CDCEEMOUT_COMMAND;

	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDCEEM_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	USBD_LL_CloseEP(pdev, USBD_EP_CDCEEM_IN);
	USBD_LL_CloseEP(pdev, USBD_EP_CDCEEM_OUT);
	return USBD_OK;
}


static USBD_CDC_EEM_HandleTypeDef gxdc;
/**
  * @}
  */



/* Private functions ---------------------------------------------------------*/
/**
  * @brief  CDC_EEM_Init_FS
  *         Initializes the CDC media low layer over the FS USB IP
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_EEM_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
  /* Set Application Buffers */
#if 0
  USBD_CDC_EEM_SetTxBuffer(&hUsbDevice, UserTxBufferFS, EEM_TX_DATA_SIZE, 0);
  USBD_CDC_EEM_SetRxBuffer(&hUsbDevice, UserRxBufferFS, EEM_RX_DATA_SIZE);

  init_eem_pkt_bpool(EEM_RX_BUFFER);
  init_eem_pkt_bpool(EEM_TX_BUFFER);
#endif
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  CDC_EEM_DeInit_FS
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_EEM_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  CDC_EEM_Control_FS
  *         Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_EEM_Control_FS  (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
#if 0
  switch (cmd)
  {
  case CDC_EEM_SEND_ENCAPSULATED_COMMAND:

    break;

  case CDC_EEM_GET_ENCAPSULATED_RESPONSE:

    break;

  case CDC_EEM_SET_COMM_FEATURE:

    break;

  case CDC_EEM_GET_COMM_FEATURE:

    break;

  case CDC_EEM_CLEAR_COMM_FEATURE:

    break;

  case CDC_EEM_SET_LINE_CODING:

    break;

  case CDC_EEM_GET_LINE_CODING:

    break;

  case CDC_EEM_SET_CONTROL_LINE_STATE:

    break;

  case CDC_EEM_SEND_BREAK:

    break;

  default:
    break;
  }
#endif

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  CDC_EEM_Receive_FS
  *         Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will block any OUT packet reception on USB endpoint
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result
  *         in receiving more data while previous ones are still not sent.
  *
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
#if 0
static int8_t CDC_EEM_Receive_FS (uint8_t*buf, uint32_t *size)
{
  /* USER CODE BEGIN 6 */
  USBD_CDC_EEM_ReceivePacket(&hUsbDevice, buf, size);
  return (USBD_OK);
  /* USER CODE END 6 */
}
#endif

#if 0
/**
  * @brief  CDC_EEM_Transmit_FS
  *         Data send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be send
  * @param  Len: Number of data to be send (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_EEM_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  //USBD_CDC_EEM_HandleTypeDef *hcdc = (USBD_CDC_EEM_HandleTypeDef*)hUsbDevice.pClassData;
  USBD_CDC_EEM_HandleTypeDef   * const hcdc = & gxdc;
  if (hcdc->tx_state != 0){
    return USBD_BUSY;
  }

  if (Buf == NULL){ /* send zero packet */
    Len = 2;
    Buf = UserTxBufferFS;
    Buf[0] = 0;
    Buf[1] = 0;
  }
  USBD_CDC_EEM_SetTxBuffer(&hUsbDevice, Buf, EEM_TX_DATA_SIZE, Len);
  result = USBD_CDC_EEM_TransmitPacket(&hUsbDevice);
  /* USER CODE END 7 */
  return result;
}
#endif


/** @defgroup USBD_CDC_EEM_Private_Functions
  * @{
  */

/**
  * @brief  USBD_CDC_EEM_Init
  *         Initialize the CDC interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static USBD_StatusTypeDef  USBD_CDC_EEM_Init (USBD_HandleTypeDef *pdev,
		uint_fast8_t cfgidx)
{
  uint8_t ret = 0;
  uint32_t buf_size;
  uint32_t bpool_full;
  USBD_CDC_EEM_HandleTypeDef   * const hcdc = & gxdc;

  if(pdev->dev_speed == USBD_SPEED_HIGH  ) {
    /* Open EP OUT */
    USBD_LL_OpenEP(pdev,
                   USB_ENDPOINT_OUT(USBD_EP_CDCEEM_OUT),
                   USBD_EP_TYPE_BULK,
                   USBD_CDCEEM_BUFSIZE);
    /* Open IN EP */
    USBD_LL_OpenEP(pdev,
                   USBD_EP_CDCEEM_IN,
                   USBD_EP_TYPE_BULK,
                   USBD_CDCEEM_BUFSIZE);

  }
  else {
    /* Open EP OUT */
    USBD_LL_OpenEP(pdev,
                   USB_ENDPOINT_OUT(USBD_EP_CDCEEM_OUT),
                   USBD_EP_TYPE_BULK,
                   USBD_CDCEEM_BUFSIZE);
    /* Open IN EP */
    USBD_LL_OpenEP(pdev,
                   USBD_EP_CDCEEM_IN,
                   //USBD_EP_TYPE_INTR,
                   USBD_EP_TYPE_BULK,
                   USBD_CDCEEM_BUFSIZE);
  }


  //pdev->pClassData = USBD_malloc(sizeof (USBD_CDC_EEM_HandleTypeDef));

//  if(pdev->pClassData == NULL) {
//    ret = 1;
//  }
//  else
  {
    //hcdc = (USBD_CDC_EEM_HandleTypeDef*) pdev->pClassData;

    /* Init  physical Interface components */
    // ((USBD_CDC_EEM_ItfTypeDef *)pdev->pUserData)->Init();
    CDC_EEM_Init_FS();

    /* Init Xfer states */
    hcdc->tx_state = EEM_TRANSMITTER_INITIAL;

    hcdc->rx_state = EEM_RECEIVER_INITIAL;
    hcdc->rx_buf_wr_pos = 0;
    hcdc->rx_cnt_received = 0;
    hcdc->rx_cnt_processed = 0;
    hcdc->rx_cnt_sending = 0;
    hcdc->rx_cnt_sent = 0;
    hcdc->rx_buffer = alloc_eem_pkt_buffer(EEM_RX_BUFFER, hcdc->rx_cnt_received, hcdc->rx_cnt_sent, &buf_size, NULL, &bpool_full);
    hcdc->rx_buf_size = buf_size;

    if(pdev->dev_speed == USBD_SPEED_HIGH  ) {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev,
                             USB_ENDPOINT_OUT(USBD_EP_CDCEEM_OUT),
                             hcdc->rx_buffer,
                             USBD_CDCEEM_BUFSIZE);
    }
    else {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev,
                             USB_ENDPOINT_OUT(USBD_EP_CDCEEM_OUT),
                             hcdc->rx_buffer,
                             USBD_CDCEEM_BUFSIZE);
    }
  }
  return ret;
}

/**
  * @brief  USBD_CDC_EEM_Init
  *         DeInitialize the CDC layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static USBD_StatusTypeDef  USBD_CDC_EEM_DeInit (USBD_HandleTypeDef *pdev,
		uint_fast8_t cfgidx)
{
  uint8_t ret = 0;

  /* Close EP OUT */
  USBD_LL_CloseEP(pdev,
              USB_ENDPOINT_OUT(USBD_EP_CDCEEM_OUT));

  /* Close IN EP */
  USBD_LL_CloseEP(pdev,
              USBD_EP_CDCEEM_IN);


  /* DeInit  physical Interface components */
  if(/* pdev->pClassData != NULL */ 1) {
    //((USBD_CDC_EEM_ItfTypeDef *)pdev->pUserData)->DeInit();
//    USBD_free(pdev->pClassData);
//    pdev->pClassData = NULL;
  }

  return ret;
}

/**
  * @brief  USBD_CDC_EEM_Setup
  *         Handle the CDC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static USBD_StatusTypeDef  USBD_CDC_EEM_Setup (USBD_HandleTypeDef *pdev,
                                const USBD_SetupReqTypedef *req)
{
  //USBD_CDC_EEM_HandleTypeDef   *hcdc = (USBD_CDC_EEM_HandleTypeDef*) pdev->pClassData;
  USBD_CDC_EEM_HandleTypeDef   * const hcdc = & gxdc;

  static uint8_t ifalt = 0;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :
    if (req->wLength)
    {
      if (req->bmRequest & 0x80)
      {
//          ((USBD_CDC_EEM_ItfTypeDef *)pdev->pUserData)->Control(req->bRequest,
//                                                            (uint8_t *)hcdc->data,
//                                                            req->wLength);
          CDC_EEM_Control_FS(req->bRequest,
                                                        (uint8_t *)hcdc->data,
                                                        req->wLength);
          USBD_CtlSendData (pdev,
                            (uint8_t *)hcdc->data,
                            req->wLength);
      }
      else
      {
        hcdc->CmdOpCode = req->bRequest;
        hcdc->CmdLength = req->wLength;

        USBD_CtlPrepareRx (pdev,
                           (uint8_t *)hcdc->data,
                           req->wLength);
      }

    }
    else
    {
//      ((USBD_CDC_EEM_ItfTypeDef *)pdev->pUserData)->Control(req->bRequest,
//                                                        (uint8_t*)req,
//                                                        0);
				  CDC_EEM_Control_FS(req->bRequest,
						  (uint8_t*)req,
						  0);
	}
break;

  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        &ifalt,
                        1);
      break;

    case USB_REQ_SET_INTERFACE :
      break;
    }

  default:
    break;
  }
  return USBD_OK;
}

/**
  * @brief  USBD_CDC_EEM_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static USBD_StatusTypeDef USBD_CDC_EEM_DataIn (USBD_HandleTypeDef *pdev, uint_fast8_t epnum) {

  uint32_t tx_len;
  uint8_t *tx_buf;
  //USBD_CDC_EEM_HandleTypeDef   *hcdc = (USBD_CDC_EEM_HandleTypeDef*) pdev->pClassData;
  USBD_CDC_EEM_HandleTypeDef   * const hcdc = & gxdc;
  USBD_StatusTypeDef ret_code;

  if(/* pdev->pClassData != NULL */ 1) {

    if (hcdc->tx_state == EEM_TRANSMITTER_SEND_DATA){
      tx_len = (hcdc->tx_length >= USBD_CDCEEM_BUFSIZE) ? USBD_CDCEEM_BUFSIZE:hcdc->tx_length;
      if (tx_len != 0){
#if 0 /* code added for experiment */
        if (tx_len < USBD_CDCEEM_BUFSIZE){
          hcdc->tx_state = EEM_TRANSMITTER_SENDING_ZPKT;
        }
#endif
        tx_buf = hcdc->tx_buffer;

        hcdc->tx_buffer += tx_len;
        hcdc->tx_length -= tx_len;
        while ((ret_code = USBD_LL_Transmit(pdev,
                                            USBD_EP_CDCEEM_IN,
                                            tx_buf,
                                            tx_len)) == USBD_BUSY)
          ;
        if (ret_code != USBD_OK) {
          hcdc->tx_state = EEM_TRANSMITTER_FAULT;
          return ret_code;
        }
      }else
      {
        hcdc->tx_state = EEM_TRANSMITTER_INITIAL;
        hcdc->rx_cnt_sent ++;
      }
#if 0 /* code added for experiment */
    }else if (hcdc->tx_state == EEM_TRANSMITTER_SENDING_ZPKT){
      hcdc->tx_state = EEM_TRANSMITTER_SENT_ZPKT;
      while ((ret_code = USBD_LL_Transmit(pdev,
                                          USBD_EP_CDCEEM_IN,
                                          hcdc->tx_buffer,
                                          0)) == USBD_BUSY)
        ;
      if (ret_code != USBD_OK) {
        hcdc->tx_state = EEM_TRANSMITTER_FAULT;
        return ret_code;
      }
    }
    else if (hcdc->tx_state == EEM_TRANSMITTER_SEND_EEM_ZPKT ||
             hcdc->tx_state == EEM_TRANSMITTER_SENT_ZPKT){
#else
    }else if (hcdc->tx_state == EEM_TRANSMITTER_SEND_EEM_ZPKT){
#endif
      /* sent 2 bytes */
      hcdc->tx_state = EEM_TRANSMITTER_INITIAL;
    }else{
    }
    return USBD_OK;
  }
  else {
    return USBD_FAIL;
  }
  return USBD_OK;
}

/**
  * @brief  USBD_CDC_EEM_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */

static uint32_t eem_packet_type, eem_packet_size, eem_cmd;

static USBD_StatusTypeDef  USBD_CDC_EEM_DataOut (USBD_HandleTypeDef *pdev, uint_fast8_t epnum) {

	  //USBD_CDC_EEM_HandleTypeDef   *hcdc = (USBD_CDC_EEM_HandleTypeDef*) pdev->pClassData;
	  USBD_CDC_EEM_HandleTypeDef   * const hcdc = & gxdc;
  uint32_t rx_data_size;

  if(/* pdev->pClassData != NULL */ 1) {
    rx_data_size  = USBD_LL_GetRxDataSize (pdev, epnum);

    switch (hcdc->rx_state){
      case EEM_RECEIVER_INITIAL: // initial state

        eem_packet_type = (hcdc->rx_buffer[1] & 0x80) ? EEM_CMD_PACKET : EEM_DATA_PACKET;
        /* get eem header */
        if (eem_packet_type == EEM_CMD_PACKET){
          eem_cmd = ((((uint32_t)hcdc->rx_buffer[1]) & 0x03) >> 3);
          eem_packet_size = (uint32_t)hcdc->rx_buffer[0] | ((((uint32_t)hcdc->rx_buffer[1]) & 0x03) << 8);
          if (eem_packet_size > hcdc->rx_buf_size){
            hcdc->rx_state = EEM_RECEIVER_FAULT;
            return USBD_FAIL;
          }

          if (rx_data_size >= eem_packet_size + 2){/* usb packet contains whole eem packet */
            hcdc->rx_state = EEM_RECEIVER_CMD_DONE;
            hcdc->rx_buf_wr_pos = 0;
          }else{
            hcdc->rx_state = EEM_RECEIVER_CMD_IN_PROGRESS;
            hcdc->rx_buf_wr_pos += rx_data_size;
          }

        }else{
          eem_packet_size = (uint32_t)hcdc->rx_buffer[0] | ((((uint32_t)hcdc->rx_buffer[1]) & 0x3F) << 8);
          if (eem_packet_size > hcdc->rx_buf_size){
            hcdc->rx_state = EEM_RECEIVER_FAULT;
            return USBD_FAIL;
          }

          if (rx_data_size >= eem_packet_size + 2){/* usb packet contains whole eem packet */
            uint32_t buf_size;
            uint32_t bpool_full;

            hcdc->rx_state = EEM_RECEIVER_INITIAL;
            hcdc->rx_buffer = alloc_eem_pkt_buffer(EEM_RX_BUFFER, hcdc->rx_cnt_received + 1, hcdc->rx_cnt_sent, &buf_size, hcdc->rx_buffer, &bpool_full);
            hcdc->rx_buf_wr_pos = 0;

            if (!bpool_full)
              hcdc->rx_cnt_received ++;
          }else{
            hcdc->rx_state = EEM_RECEIVER_DATA_IN_PROGRESS;
            hcdc->rx_buf_wr_pos += rx_data_size;
          }
        }
      break;

      case EEM_RECEIVER_CMD_IN_PROGRESS: /* command reception in progress */
        /* now we ignore command reception: after all cmd received we are ready receive next packet  */
        if ((hcdc->rx_buf_wr_pos + rx_data_size) >= (eem_packet_size + 2)){

          hcdc->rx_state = EEM_RECEIVER_INITIAL;
          hcdc->rx_buf_wr_pos = 0;
        }else{
          hcdc->rx_buf_wr_pos += rx_data_size;
        }
      break;

      case EEM_RECEIVER_DATA_IN_PROGRESS: /* data reception in progress */

        if ((hcdc->rx_buf_wr_pos + rx_data_size)  >= (eem_packet_size + 2)){
          uint32_t buf_size;
          uint32_t bpool_full;

          hcdc->rx_state = EEM_RECEIVER_INITIAL;
          hcdc->rx_buffer = alloc_eem_pkt_buffer(EEM_RX_BUFFER, hcdc->rx_cnt_received + 1, hcdc->rx_cnt_sent, &buf_size, hcdc->rx_buffer, &bpool_full);
          hcdc->rx_buf_size = buf_size;
          hcdc->rx_buf_wr_pos = 0;

          if (!bpool_full)
            hcdc->rx_cnt_received ++;
        }else{
          hcdc->rx_buf_wr_pos += rx_data_size;
        }

      break;

      default:
      break;
    }

    if (USBD_LL_PrepareReceive(pdev,
                               USB_ENDPOINT_OUT(USBD_EP_CDCEEM_OUT),
                               &hcdc->rx_buffer[hcdc->rx_buf_wr_pos],
                               USBD_CDCEEM_BUFSIZE) == USBD_FAIL){
      hcdc->rx_state = EEM_RECEIVER_FAULT;
      return USBD_FAIL;

    }

    return USBD_OK;


  }
  else {
    return USBD_FAIL;
  }
  return USBD_OK;
}



/**
  * @brief  USBD_CDC_EEM_EP0_RxReady
  *         Data received on control endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static USBD_StatusTypeDef  USBD_CDC_EEM_EP0_RxReady (USBD_HandleTypeDef *pdev)
{
	const USBD_SetupReqTypedef * const req = & pdev->request;
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	  //USBD_CDC_EEM_HandleTypeDef   *hcdc = (USBD_CDC_EEM_HandleTypeDef*) pdev->pClassData;
	  USBD_CDC_EEM_HandleTypeDef   * const hcdc = & gxdc;

  if((/* pdev->pUserData != NULL */ 1) && (hcdc->CmdOpCode != 0xFF))
  {
//    ((USBD_CDC_EEM_ItfTypeDef *)pdev->pUserData)->Control(hcdc->CmdOpCode,
//                                                      (uint8_t *)hcdc->data,
//                                                      hcdc->CmdLength);
	  CDC_EEM_Control_FS(hcdc->CmdOpCode,
              (uint8_t *)hcdc->data,
              hcdc->CmdLength);
      hcdc->CmdOpCode = 0xFF;

  }
  return USBD_OK;
}

/**
* @brief  USBD_CDC_EEM_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CD  Interface callback
  * @retval status
  */
//USBD_StatusTypeDef  USBD_CDC_EEM_RegisterInterface  (USBD_HandleTypeDef   *pdev,
//                                      USBD_CDC_EEM_ItfTypeDef *fops)
//{
//  uint8_t  ret = USBD_FAIL;
//
//  if(fops != NULL)
//  {
//    pdev->pUserData= fops;
//    ret = USBD_OK;
//  }
//
//  return ret;
//}

#if 0
/**
  * @brief  USBD_CDC_EEM_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @retval status
  */
USBD_StatusTypeDef  USBD_CDC_EEM_SetTxBuffer  (USBD_HandleTypeDef   *pdev,
                                uint8_t  *pbuff,
                                uint16_t buf_size,
                                uint16_t length)
{
	  //USBD_CDC_EEM_HandleTypeDef   *hcdc = (USBD_CDC_EEM_HandleTypeDef*) pdev->pClassData;
	  USBD_CDC_EEM_HandleTypeDef   * const hcdc = & gxdc;

  hcdc->TxBuffer = pbuff;
  hcdc->TxBufferSize = buf_size;
  hcdc->TxBufRdPos = 0;
  hcdc->TxLength = length;

  return USBD_OK;
}
#endif

#if 0
/**
  * @brief  USBD_CDC_EEM_SetRxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Rx Buffer
  * @retval status
  */
USBD_StatusTypeDef  USBD_CDC_EEM_SetRxBuffer  (USBD_HandleTypeDef   *pdev,
                                   uint8_t  *pbuff,
                                   uint16_t buf_size)
{
	  //USBD_CDC_EEM_HandleTypeDef   *hcdc = (USBD_CDC_EEM_HandleTypeDef*) pdev->pClassData;
	  USBD_CDC_EEM_HandleTypeDef   * const hcdc = & gxdc;

  hcdc->RxBuffer = pbuff;
  hcdc->RxBufferSize = buf_size;
  hcdc->RxBufWrPos = 0;
  hcdc->RxLength = 0;


  return USBD_OK;
}
#endif

/**
  * @brief  USBD_CDC_EEM_TransmitPacket
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
USBD_StatusTypeDef  USBD_CDC_EEM_TransmitPacket(USBD_HandleTypeDef *pdev, eem_transmitter_state_enum tx_state)
{
  uint32_t tx_len;
  uint8_t *tx_buf;
  //USBD_CDC_EEM_HandleTypeDef   *hcdc = (USBD_CDC_EEM_HandleTypeDef*) pdev->pClassData;
  USBD_CDC_EEM_HandleTypeDef   * const hcdc = & gxdc;

  if(/* pdev->pClassData != NULL */ 1) {
    if (hcdc->tx_state == EEM_TRANSMITTER_INITIAL) {
      /* Tx Transfer in progress */
      if (hcdc->tx_length == 80){
        hcdc->tx_state = tx_state;

      }

      hcdc->tx_state = tx_state;

      tx_len = (hcdc->tx_length >= USBD_CDCEEM_BUFSIZE) ? USBD_CDCEEM_BUFSIZE:hcdc->tx_length;
      tx_buf = hcdc->tx_buffer;

      hcdc->tx_length -= tx_len;
      hcdc->tx_buffer += tx_len;

      /* Transmit next packet */
      USBD_LL_Transmit(pdev,
                       USBD_EP_CDCEEM_IN,
                       tx_buf,
                       tx_len);
      return USBD_OK;
    }
    else {
      return USBD_BUSY;
    }
  }
  else {
    return USBD_FAIL;
  }
  return USBD_OK;
}


/**
  * @brief  USBD_CDC_EEM_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
#if 1
USBD_StatusTypeDef  USBD_CDC_EEM_ReceivePacket(USBD_HandleTypeDef *pdev,
                                    uint8_t*pbuf)
{
  /* Suspend or Resume USB Out process */
  if(/* pdev->pClassData != NULL */ 1)
  {
    if(pdev->dev_speed == USBD_SPEED_HIGH  )
    {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev,
                             USB_ENDPOINT_OUT(USBD_EP_CDCEEM_OUT),
                             pbuf,
                             USBD_CDCEEM_BUFSIZE);
    }
    else
    {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev,
                             USB_ENDPOINT_OUT(USBD_EP_CDCEEM_OUT),
                             pbuf,
                             USBD_CDCEEM_BUFSIZE);
    }
    return USBD_OK;
  }
  else
  {
    return USBD_FAIL;
  }
}
#endif
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


static void USBD_CDCEEM_ColdInit(void)
{
}
#if 0
const USBD_ClassTypeDef USBD_CLASS_CDC_EEMx =
{
	USBD_CDCEEM_ColdInit,
	USBD_CDC_EEM_Init,	// Init
	USBD_CDC_EEM_DeInit,	// DeInit
	USBD_CDC_EEM_Setup,		// Setup
	NULL,	// EP0_TxSent
	USBD_CDC_EEM_EP0_RxReady,	// EP0_RxReady
	USBD_CDC_EEM_DataIn,	// DataIn
	USBD_CDC_EEM_DataOut,	// DataOut
	NULL,	//USBD_XXX_SOF,	// SOF
	NULL,	//USBD_XXX_IsoINIncomplete,	// IsoINIncomplete
	NULL,	//USBD_XXX_IsoOUTIncomplete,	// IsoOUTIncomplete
};
#endif

const USBD_ClassTypeDef USBD_CLASS_CDC_EEM =
{
	USBD_CDCEEM_ColdInit,
	USBD_CDCEEM_Init,	// Init
	USBD_CDCEEM_DeInit,	// DeInit
	USBD_CDCEEM_Setup,		// Setup
	NULL,	// EP0_TxSent
	USBD_CDCEEM_EP0_RxReady,	// EP0_RxReady
	USBD_CDCEEM_DataIn,	// DataIn
	USBD_CDCEEM_DataOut,	// DataOut
	NULL,	//USBD_XXX_SOF,	// SOF
	NULL,	//USBD_XXX_IsoINIncomplete,	// IsoINIncomplete
	NULL,	//USBD_XXX_IsoOUTIncomplete,	// IsoOUTIncomplete
};



#if WITHLWIP

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/autoip.h"
#include "netif/etharp.h"
#include "lwip/ip.h"

//static struct netif test_netif1, test_netif2;

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

// Transceiving Ethernet packets
static err_t cdceem_linkoutput_fn(struct netif *netif, struct pbuf *p)
{
	//PRINTF("cdceem_linkoutput_fn\n");
    int i;
    struct pbuf *q;
    static uint8_t data [ETH_PAD_SIZE + CDCEEM_MTU + 14 + 4];
    int size = 0;

    for (i = 0; i < 200; i++)
    {
        if (cdceem_can_send()) break;
        local_delay_ms(1);
    }

    if (! cdceem_can_send())
    {
		return ERR_MEM;
    }

	pbuf_header(p, - ETH_PAD_SIZE);
    size = pbuf_copy_partial(p, data, sizeof data, 0);
    cdceem_send(data, size);

    return ERR_OK;
}


static struct netif cdceem_netif_data;



struct netif  * getNetifData(void)
{
	return & cdceem_netif_data;
}


static err_t cdceem_output_fn(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
	err_t e = etharp_output(netif, p, ipaddr);
	if (e == ERR_OK)
	{
		// добавляем свои заголовки требуеющиеся для физического уповня

	}
	return e;
}

static err_t netif_init_cb(struct netif *netif)
{
	//PRINTF("cdc eem netif_init_cb\n");
	LWIP_ASSERT("netif != NULL", (netif != NULL));
#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "storch";
#endif /* LWIP_NETIF_HOSTNAME */
	netif->mtu = CDCEEM_MTU;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
	netif->state = NULL;
	netif->name[0] = 'E';
	netif->name[1] = 'X';
	netif->output = etharp_output; //cdceem_output_fn;	// если бы не требовалось добавлять ethernet заголовки, передачва делалась бы тут.
												// и слкдующий callback linkoutput не требовался бы вообще
	netif->linkoutput = cdceem_linkoutput_fn;	// используется внутри etharp_output
	return ERR_OK;
}


typedef struct cdceembuf_tag
{
	LIST_ENTRY item;
	struct pbuf *frame;
} cdceembuf_t;


static LIST_ENTRY cdceem_free;
static LIST_ENTRY cdceem_ready;

static void cdceem_buffers_initialize(void)
{
	static RAMFRAMEBUFF cdceembuf_t sliparray [64];
	unsigned i;

	InitializeListHead(& cdceem_free);	// Незаполненные
	InitializeListHead(& cdceem_ready);	// Для обработки

	for (i = 0; i < (sizeof sliparray / sizeof sliparray [0]); ++ i)
	{
		cdceembuf_t * const p = & sliparray [i];
		InsertHeadList(& cdceem_free, & p->item);
	}
}

static int cdceem_buffers_alloc(cdceembuf_t * * tp)
{
	if (! IsListEmpty(& cdceem_free))
	{
		const PLIST_ENTRY t = RemoveTailList(& cdceem_free);
		cdceembuf_t * const p = CONTAINING_RECORD(t, cdceembuf_t, item);
		* tp = p;
		return 1;
	}
	if (! IsListEmpty(& cdceem_ready))
	{
		const PLIST_ENTRY t = RemoveTailList(& cdceem_ready);
		cdceembuf_t * const p = CONTAINING_RECORD(t, cdceembuf_t, item);
		* tp = p;
		return 1;
	}
	return 0;
}

static int cdceem_buffers_ready_user(cdceembuf_t * * tp)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	if (! IsListEmpty(& cdceem_ready))
	{
		const PLIST_ENTRY t = RemoveTailList(& cdceem_ready);
		LowerIrql(oldIrql);
		cdceembuf_t * const p = CONTAINING_RECORD(t, cdceembuf_t, item);
		* tp = p;
		return 1;
	}
	LowerIrql(oldIrql);
	return 0;
}


static void cdceem_buffers_release(cdceembuf_t * p)
{
	InsertHeadList(& cdceem_free, & p->item);
}

static void cdceem_buffers_release_user(cdceembuf_t * p)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	cdceem_buffers_release(p);
	LowerIrql(oldIrql);
}

// сохранить принятый
static void cdceem_buffers_rx(cdceembuf_t * p)
{
	InsertHeadList(& cdceem_ready, & p->item);
}

static void on_packet(const uint8_t *data, int size)
{
	cdceembuf_t * p;
	if (cdceem_buffers_alloc(& p) != 0)
	{
		struct pbuf *frame;
		frame = pbuf_alloc(PBUF_RAW, size + ETH_PAD_SIZE, PBUF_POOL);
		if (frame == NULL)
		{
			TP();
			cdceem_buffers_release(p);
			return;
		}
		pbuf_header(frame, - ETH_PAD_SIZE);
		err_t e = pbuf_take(frame, data, size);
		pbuf_header(frame, + ETH_PAD_SIZE);
		if (e == ERR_OK)
		{
			p->frame = frame;
			cdceem_buffers_rx(p);
		}
		else
		{
			pbuf_free(frame);
			cdceem_buffers_release(p);
		}
	}
}



// Receiving Ethernet packets
// user-mode function
void usb_polling(void)
{
	cdceembuf_t * p;
	if (cdceem_buffers_ready_user(& p) != 0)
	{
		struct pbuf * const frame = p->frame;
		cdceem_buffers_release_user(p);

		err_t e = ethernet_input(p->frame, & cdceem_netif_data);
		if (e != ERR_OK)
		{
			  /* This means the pbuf is freed or consumed,
			     so the caller doesn't have to free it again */
		}

	}
}


void init_netif(void)
{
	cdceem_buffers_initialize();
	cdceem_rxproc = on_packet;		// разрешаем принимать пакеты адаптеру и отправлять в LWIP

	static const  uint8_t hwaddrv [6]  = { HWADDR };

	static ip_addr_t netmask;// [4] = NETMASK;
	static ip_addr_t gateway;// [4] = GATEWAY;

	IP4_ADDR(& netmask, myNETMASK [0], myNETMASK [1], myNETMASK [2], myNETMASK [3]);
	IP4_ADDR(& gateway, myGATEWAY [0], myGATEWAY [1], myGATEWAY [2], myGATEWAY [3]);

	static ip_addr_t vaddr;// [4]  = IPADDR;
	IP4_ADDR(& vaddr, myIP [0], myIP [1], myIP [2], myIP [3]);

	struct netif  *netif = & cdceem_netif_data;
	netif->hwaddr_len = 6;
	memcpy(netif->hwaddr, hwaddrv, 6);

	netif = netif_add(netif, & vaddr, & netmask, & gateway, NULL, netif_init_cb, ip_input);
#if LWIP_AUTOIP
	  autoip_start(netif);
#endif /* LWIP_AUTOIP */
	netif_set_default(netif);

	while (!netif_is_up(netif))
		;

}

#endif /* WITHLWIP */

#endif /* WITHUSBHW && WITHUSBCDCEEM */

