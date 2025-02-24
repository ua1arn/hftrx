/**
  ******************************************************************************
  * File Name          : ethernetif.c
  * Description        : This file provides code for the configuration
  *                      of the ethernetif.c MiddleWare.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hardware.h"

#if WITHETHHW && CPUSTYLE_ALLWINNER

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/ip.h"
#include "ethernetif.h"
//#include "lan8742.h"
#include <string.h>

/* Within 'USER CODE' section, code will be kept by default at each generation */
/* USER CODE BEGIN 0 */

#if CPUSTYLE_XC7Z && WITHLWIP
#warning Should be implemented for Zynq 7000

#define GEM_MTU 1540



typedef struct gembuf_tag
{
	LIST_ENTRY item;
	struct pbuf *frame;
} gembuf_t;


static void gem_buffers_initialize(void)
{

}

static struct netif gem_netif_data;


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
static err_t gem_linkoutput_fn(struct netif *netif, struct pbuf *p)
{
#if 0
	//PRINTF("gem_linkoutput_fn\n");
    int i;
    struct pbuf *q;
    static char data [RNDIS_HEADER_SIZE + RNDIS_MTU + 14 + 4];
    int size = 0;

    for (i = 0; i < 200; i++)
    {
        if (gem_can_send()) break;
        local_delay_ms(1);
    }

    if (!gem_can_send())
    {
		return ERR_MEM;
    }

	pbuf_header(p, - ETH_PAD_SIZE);
    size = pbuf_copy_partial(p, data, sizeof data, 0);

    gem_send(data, size);
#endif
    return ERR_OK;
}


static err_t gem_output_fn(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
	err_t e = etharp_output(netif, q, ipaddr);
	if (e == ERR_OK)
	{
#if 0
		gem_data_packet_t * hdr;
		unsigned size = q->len;
		// добавляем свои заголовки требуеющиеся для физического уповня
		  /* make room for RNDIS header - should not fail */
		  if (pbuf_header(q, RNDIS_HEADER_SIZE) != 0) {
		    /* bail out */
		    LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
		      ("gem_output_fn: could not allocate room for header.\n"));
		    return ERR_BUF;
		  }

		  hdr = (gem_data_packet_t *) q->payload;
		  memset(hdr, 0, RNDIS_HEADER_SIZE);
		  hdr->MessageType = REMOTE_NDIS_PACKET_MSG;
		  hdr->MessageLength = RNDIS_HEADER_SIZE + size;
		  hdr->DataOffset = RNDIS_HEADER_SIZE - offsetof(gem_data_packet_t, DataOffset);
		  hdr->DataLength = size;
#endif
	}
	return e;
}

static err_t netif_init_cb(struct netif *netif)
{
	PRINTF("rndis netif_init_cb\n");
	LWIP_ASSERT("netif != NULL", (netif != NULL));
#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "storch";
#endif /* LWIP_NETIF_HOSTNAME */
	netif->mtu = GEM_MTU;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
	netif->state = NULL;
	netif->name[0] = 'E';
	netif->name[1] = 'X';
	netif->output = gem_output_fn;	// если бы не требовалось добавлять ethernet заголовки, передачва делалась бы тут.
												// и слкдующий callback linkoutput не требовался бы вообще
	netif->linkoutput = gem_linkoutput_fn;	// используется внутри etharp_output
	return ERR_OK;
}

void init_netif(void)
{
#if 0
	gem_buffers_initialize();
	gem_rxproc = on_packet;		// разрешаем принимать пакеты адаптеру и отправлять в LWIP

	static const  uint8_t hwaddrv [6]  = { HWADDR };

	static ip_addr_t netmask;// [4] = NETMASK;
	static ip_addr_t gateway;// [4] = GATEWAY;

	IP4_ADDR(& netmask, myNETMASK [0], myNETMASK [1], myNETMASK [2], myNETMASK [3]);
	IP4_ADDR(& gateway, myGATEWAY [0], myGATEWAY [1], myGATEWAY [2], myGATEWAY [3]);

	static ip_addr_t vaddr;// [4]  = IPADDR;
	IP4_ADDR(& vaddr, myIP [0], myIP [1], myIP [2], myIP [3]);

	struct netif  *netif = &gem_netif_data;
	netif->hwaddr_len = 6;
	memcpy(netif->hwaddr, hwaddrv, 6);

	netif = netif_add(netif, & vaddr, & netmask, & gateway, NULL, netif_init_cb, ip_input);
	netif_set_default(netif);

	while (!netif_is_up(netif))
		;

#if LWIP_AUTOIP
	  autoip_start(netif);
#endif /* LWIP_AUTOIP */

#endif
}



// Receiving Ethernet packets
// user-mode function
void usb_polling(void)
{
#if 0
	gembuf_t * p;
	if (gem_buffers_ready_user(& p) != 0)
	{
		struct pbuf *frame = p->frame;
		gem_buffers_release_user(p);

		err_t e = ethernet_input(frame, & gem_netif_data);
		if (e != ERR_OK)
		{
			  /* This means the pbuf is freed or consumed,
			     so the caller doesn't have to free it again */
		}

	}
#endif
}

struct netif  * getNetifData(void)
{
	return & gem_netif_data;
}


#elif defined (ETH) && WITHLWIP

/* USER CODE END 0 */

/* Private define ------------------------------------------------------------*/

/* Network interface name */
#define IFNAME0 's'
#define IFNAME1 't'

/* ETH Setting  */
#define ETH_DMA_TRANSMIT_TIMEOUT               ( 20U )
/* ETH_RX_BUFFER_SIZE parameter is defined in lwipopts.h */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Private variables ---------------------------------------------------------*/
/*
@Note: This interface is implemented to operate in zero-copy mode only:
        - Rx buffers are allocated statically and passed directly to the LwIP stack
          they will return back to ETH DMA after been processed by the stack.
        - Tx Buffers will be allocated from LwIP stack memory heap,
          then passed to ETH HAL driver.

@Notes:
  1.a. ETH DMA Rx descriptors must be contiguous, the default count is 4,
       to customize it please redefine ETH_RX_DESC_CNT in ETH GUI (Rx Descriptor Length)
       so that updated value will be generated in stm32xxxx_hal_conf.h
  1.b. ETH DMA Tx descriptors must be contiguous, the default count is 4,
       to customize it please redefine ETH_TX_DESC_CNT in ETH GUI (Tx Descriptor Length)
       so that updated value will be generated in stm32xxxx_hal_conf.h

  2.a. Rx Buffers number must be between ETH_RX_DESC_CNT and 2*ETH_RX_DESC_CNT
  2.b. Rx Buffers must have the same size: ETH_RX_BUFFER_SIZE, this value must
       passed to ETH DMA in the init field (heth.Init.RxBuffLen)
  2.c  The RX Ruffers addresses and sizes must be properly defined to be aligned
       to L1-CACHE line size (32 bytes).
*/

#if defined ( __ICCARM__ ) /*!< IAR Compiler */

#pragma location=0x30040000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x30040060
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
#pragma location=0x30040200
uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_RX_BUFFER_SIZE]; /* Ethernet Receive Buffers */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x30040000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x30040060))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
__attribute__((at(0x30040200))) uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_RX_BUFFER_SIZE]; /* Ethernet Receive Buffer */

#elif defined ( __GNUC__ ) /* GNU Compiler */

__ALIGN_BEGIN ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __ALIGN_END; /* Ethernet Rx DMA Descriptors */
__ALIGN_BEGIN ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __ALIGN_END;   /* Ethernet Tx DMA Descriptors */
__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_RX_BUFFER_SIZE] __ALIGN_END; /* Ethernet Receive Buffers */

#endif

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/* Global Ethernet handle */
ETH_HandleTypeDef heth;
ETH_TxPacketConfig TxConfig;

/* Memory Pool Declaration */
LWIP_MEMPOOL_DECLARE(RX_POOL, 10, sizeof(struct pbuf_custom), "Zero-copy RX PBUF pool");

/* Private function prototypes -----------------------------------------------*/
int32_t ETH_PHY_IO_Init(void);
int32_t ETH_PHY_IO_DeInit (void);
int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal);
int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal);
int32_t ETH_PHY_IO_GetTick(void);
//
//lan8742_Object_t LAN8742;
//lan8742_IOCtx_t  LAN8742_IOCtx = {ETH_PHY_IO_Init,
//                                  ETH_PHY_IO_DeInit,
//                                  ETH_PHY_IO_WriteReg,
//                                  ETH_PHY_IO_ReadReg,
//                                  ETH_PHY_IO_GetTick};

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */

/* Private functions ---------------------------------------------------------*/
void pbuf_free_custom(struct pbuf *p);
void Error_Handler(void);

void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspInit 0 */

  /* USER CODE END ETH_MspInit 0 */
    /* Enable Peripheral clock */
    __HAL_RCC_ETH1MAC_CLK_ENABLE();
    __HAL_RCC_ETH1TX_CLK_ENABLE();
    __HAL_RCC_ETH1RX_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ETH GPIO Configuration
    PG11     ------> ETH_TX_EN
    PG14     ------> ETH_TXD1
    PG13     ------> ETH_TXD0
    PC1     ------> ETH_MDC
    PA7     ------> ETH_CRS_DV
    PA1     ------> ETH_REF_CLK
    PC4     ------> ETH_RXD0
    PA2     ------> ETH_MDIO
    PC5     ------> ETH_RXD1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_1|GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN ETH_MspInit 1 */

  /* USER CODE END ETH_MspInit 1 */
  }
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{
  if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspDeInit 0 */

  /* USER CODE END ETH_MspDeInit 0 */
    /* Disable Peripheral clock */
    __HAL_RCC_ETH1MAC_CLK_DISABLE();
    __HAL_RCC_ETH1TX_CLK_DISABLE();
    __HAL_RCC_ETH1RX_CLK_DISABLE();

    /**ETH GPIO Configuration
    PG11     ------> ETH_TX_EN
    PG14     ------> ETH_TXD1
    PG13     ------> ETH_TXD0
    PC1     ------> ETH_MDC
    PA7     ------> ETH_CRS_DV
    PA1     ------> ETH_REF_CLK
    PC4     ------> ETH_RXD0
    PA2     ------> ETH_MDIO
    PC5     ------> ETH_RXD1
    */
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_13);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7|GPIO_PIN_1|GPIO_PIN_2);

  /* USER CODE BEGIN ETH_MspDeInit 1 */

  /* USER CODE END ETH_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/*******************************************************************************
                       LL Driver Interface ( LwIP stack --> ETH)
*******************************************************************************/
/**
 * @brief In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
  HAL_StatusTypeDef hal_eth_init_status = HAL_OK;
  uint32_t idx = 0;
  /* Start ETH HAL Init */

   uint8_t MACAddr[6] ;
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  hal_eth_init_status = HAL_ETH_Init(&heth);

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;

  /* End ETH HAL Init */

  /* Initialize the RX POOL */
  LWIP_MEMPOOL_INIT(RX_POOL);

#if LWIP_ARP || LWIP_ETHERNET

  /* set MAC hardware address length */
  netif->hwaddr_len = ETH_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] =  heth.Init.MACAddr[0];
  netif->hwaddr[1] =  heth.Init.MACAddr[1];
  netif->hwaddr[2] =  heth.Init.MACAddr[2];
  netif->hwaddr[3] =  heth.Init.MACAddr[3];
  netif->hwaddr[4] =  heth.Init.MACAddr[4];
  netif->hwaddr[5] =  heth.Init.MACAddr[5];

  /* maximum transfer unit */
  netif->mtu = ETH_MAX_PAYLOAD;

  /* Accept broadcast address and ARP traffic */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  #if LWIP_ARP
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
  #else
    netif->flags |= NETIF_FLAG_BROADCAST;
  #endif /* LWIP_ARP */

  for(idx = 0; idx < ETH_RX_DESC_CNT; idx ++)
  {
    HAL_ETH_DescAssignMemory(&heth, idx, Rx_Buff[idx], NULL);
  }

/* USER CODE BEGIN PHY_PRE_CONFIG */

/* USER CODE END PHY_PRE_CONFIG */
  /* Set PHY IO functions */
//  LAN8742_RegisterBusIO(&LAN8742, &LAN8742_IOCtx);
//
//  /* Initialize the LAN8742 ETH PHY */
//  LAN8742_Init(&LAN8742);
//
//  if (hal_eth_init_status == HAL_OK)
//  {
//  /* Get link state */
//  ethernet_link_check_state(netif);
//  }
//  else
//  {
//    Error_Handler();
//  }
#endif /* LWIP_ARP || LWIP_ETHERNET */

/* USER CODE BEGIN LOW_LEVEL_INIT */

/* USER CODE END LOW_LEVEL_INIT */
}

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
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  uint32_t i=0;
  struct pbuf *q;
  err_t errval = ERR_OK;
  ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT];

  memset(Txbuffer, 0 , ETH_TX_DESC_CNT*sizeof(ETH_BufferTypeDef));

  for(q = p; q != NULL; q = q->next)
  {
    if(i >= ETH_TX_DESC_CNT)
      return ERR_IF;

    Txbuffer[i].buffer = q->payload;
    Txbuffer[i].len = q->len;

    if(i>0)
    {
      Txbuffer[i-1].next = &Txbuffer[i];
    }

    if(q->next == NULL)
    {
      Txbuffer[i].next = NULL;
    }

    i++;
  }

  TxConfig.Length =  p->tot_len;
  TxConfig.TxBuffer = Txbuffer;

  HAL_ETH_Transmit(&heth, &TxConfig, ETH_DMA_TRANSMIT_TIMEOUT);

  return errval;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
   */
static struct pbuf * low_level_input(struct netif *netif)
{
  struct pbuf *p = NULL;
  ETH_BufferTypeDef RxBuff[ETH_RX_DESC_CNT];
  uint32_t framelength = 0, i = 0;
  struct pbuf_custom* custom_pbuf;

  memset(RxBuff, 0 , ETH_RX_DESC_CNT*sizeof(ETH_BufferTypeDef));

  for(i = 0; i < ETH_RX_DESC_CNT -1; i++)
  {
    RxBuff[i].next=&RxBuff[i+1];
  }

  if (HAL_ETH_IsRxDataAvailable(&heth))
  {
    HAL_ETH_GetRxDataBuffer(&heth, RxBuff);
    HAL_ETH_GetRxDataLength(&heth, &framelength);

    /* Build Rx descriptor to be ready for next data reception */
    HAL_ETH_BuildRxDescriptors(&heth);

#if !defined(DUAL_CORE) || defined(CORE_CM7)
    /* Invalidate data cache for ETH Rx Buffers */
    //SCB_InvalidateDCache_by_Addr((uint32_t *)RxBuff->buffer, framelength);
    dcache_invalidate((uintptr_t)RxBuff->buffer, framelength);
#endif

    custom_pbuf  = (struct pbuf_custom*)LWIP_MEMPOOL_ALLOC(RX_POOL);
    custom_pbuf->custom_free_function = pbuf_free_custom;

    p = pbuf_alloced_custom(PBUF_RAW, framelength, PBUF_REF, custom_pbuf, RxBuff->buffer, framelength);

    return p;
  }
  else
  {
    return NULL;
  }
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(struct netif *netif)
{
  err_t err;
  struct pbuf *p;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);

  /* no packet could be read, silently ignore this */
  if (p == NULL) return;

  /* entry point to the LwIP stack */
  err = netif->input(p, netif);

  if (err != ERR_OK)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
    p = NULL;
  }

}

#if !LWIP_ARP
/**
 * This function has to be completed by user in case of ARP OFF.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if ...
 */
static err_t low_level_output_arp_off(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
  err_t errval;
  errval = ERR_OK;

/* USER CODE BEGIN 5 */

/* USER CODE END 5 */

  return errval;

}
#endif /* LWIP_ARP */

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
  netif->output = etharp_output;
#else
  /* The user should write its own code in low_level_output_arp_off function */
  netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */

  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

/**
  * @brief  Custom Rx pbuf free callback
  * @param  pbuf: pbuf to be freed
  * @retval None
  */
void pbuf_free_custom(struct pbuf *p)
{
  struct pbuf_custom* custom_pbuf = (struct pbuf_custom*)p;

  LWIP_MEMPOOL_FREE(RX_POOL, custom_pbuf);
}

/* USER CODE BEGIN 6 */

/**
* @brief  Returns the current time in milliseconds
*         when LWIP_TIMERS == 1 and NO_SYS == 1
* @param  None
* @retval Current Time value
*/
u32_t sys_jiffies(void)
{
  return HAL_GetTick();
}

/**
* @brief  Returns the current time in milliseconds
*         when LWIP_TIMERS == 1 and NO_SYS == 1
* @param  None
* @retval Current Time value
*/
u32_t sys_now(void)
{
  return HAL_GetTick();
}

/* USER CODE END 6 */

/*******************************************************************************
                       PHI IO Functions
*******************************************************************************/
/**
  * @brief  Initializes the MDIO interface GPIO and clocks.
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
int32_t ETH_PHY_IO_Init(void)
{
  /* We assume that MDIO GPIO configuration is already done
     in the ETH_MspInit() else it should be done here
  */

  /* Configure the MDIO Clock */
  HAL_ETH_SetMDIOClockRange(&heth);

  return 0;
}

/**
  * @brief  De-Initializes the MDIO interface .
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
int32_t ETH_PHY_IO_DeInit (void)
{
  return 0;
}

/**
  * @brief  Read a PHY register through the MDIO interface.
  * @param  DevAddr: PHY port address
  * @param  RegAddr: PHY register address
  * @param  pRegVal: pointer to hold the register value
  * @retval 0 if OK -1 if Error
  */
int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal)
{
  if(HAL_ETH_ReadPHYRegister(&heth, DevAddr, RegAddr, pRegVal) != HAL_OK)
  {
    return -1;
  }

  return 0;
}

/**
  * @brief  Write a value to a PHY register through the MDIO interface.
  * @param  DevAddr: PHY port address
  * @param  RegAddr: PHY register address
  * @param  RegVal: Value to be written
  * @retval 0 if OK -1 if Error
  */
int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal)
{
  if(HAL_ETH_WritePHYRegister(&heth, DevAddr, RegAddr, RegVal) != HAL_OK)
  {
    return -1;
  }

  return 0;
}

/**
  * @brief  Get the time in millisecons used for internal PHY driver process.
  * @retval Time value
  */
int32_t ETH_PHY_IO_GetTick(void)
{
  return HAL_GetTick();
}

/**
  * @brief  Check the ETH link state then update ETH driver and netif link accordingly.
  * @param  argument: netif
  * @retval None
  */
void ethernet_link_check_state(struct netif *netif)
{
//  ETH_MACConfigTypeDef MACConf;
//  int32_t PHYLinkState;
//  uint32_t linkchanged = 0, speed = 0, duplex =0;
//
//  PHYLinkState = LAN8742_GetLinkState(&LAN8742);
//
//  if(netif_is_link_up(netif) && (PHYLinkState <= LAN8742_STATUS_LINK_DOWN))
//  {
//    HAL_ETH_Stop(&heth);
//    netif_set_down(netif);
//    netif_set_link_down(netif);
//  }
//  else if(!netif_is_link_up(netif) && (PHYLinkState > LAN8742_STATUS_LINK_DOWN))
//  {
//    switch (PHYLinkState)
//    {
//    case LAN8742_STATUS_100MBITS_FULLDUPLEX:
//      duplex = ETH_FULLDUPLEX_MODE;
//      speed = ETH_SPEED_100M;
//      linkchanged = 1;
//      break;
//    case LAN8742_STATUS_100MBITS_HALFDUPLEX:
//      duplex = ETH_HALFDUPLEX_MODE;
//      speed = ETH_SPEED_100M;
//      linkchanged = 1;
//      break;
//    case LAN8742_STATUS_10MBITS_FULLDUPLEX:
//      duplex = ETH_FULLDUPLEX_MODE;
//      speed = ETH_SPEED_10M;
//      linkchanged = 1;
//      break;
//    case LAN8742_STATUS_10MBITS_HALFDUPLEX:
//      duplex = ETH_HALFDUPLEX_MODE;
//      speed = ETH_SPEED_10M;
//      linkchanged = 1;
//      break;
//    default:
//      break;
//    }
//
//    if(linkchanged)
//    {
//      /* Get MAC Config MAC */
//      HAL_ETH_GetMACConfig(&heth, &MACConf);
//      MACConf.DuplexMode = duplex;
//      MACConf.Speed = speed;
//      HAL_ETH_SetMACConfig(&heth, &MACConf);
//
//      HAL_ETH_Start(&heth);
//      netif_set_up(netif);
//      netif_set_link_up(netif);
//    }
//  }
//
}
/* USER CODE BEGIN 8 */
/* USER CODE END 8 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
#elif  WITHLWIP && WITHETHHW && (CPUSTYLE_T507 || CPUSTYLE_H616)

#define EMAC_MTU                                       1500                           // MTU value

#define ETH_HEADER_SIZE                 14
#define ETH_MIN_PACKET_SIZE             60
#define ETH_MAX_PACKET_SIZE             (ETH_HEADER_SIZE + EMAC_MTU)
//#define EMAC_HEADER_SIZE               (sizeof (emac_data_packet_t))
//#define EMAC_RX_BUFFER_SIZE            (EMAC_HEADER_SIZE + ETH_MAX_PACKET_SIZE)

//////////////
typedef struct emacbuf_tag
{
	LIST_ENTRY item;
	struct pbuf *frame;
} emacbuf_t;


static LIST_ENTRY emac_free;
static LIST_ENTRY emac_ready;

static void emac_buffers_initialize(void)
{
	static RAMFRAMEBUFF emacbuf_t buffers [64];
	unsigned i;

	InitializeListHead(& emac_free);	// Незаполненные
	InitializeListHead(& emac_ready);	// Для обработки

	for (i = 0; i < ARRAY_SIZE(buffers); ++ i)
	{
		emacbuf_t * const p = & buffers [i];
		InsertHeadList(& emac_free, & p->item);
	}
}

static int emac_buffers_alloc(emacbuf_t * * tp)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	if (! IsListEmpty(& emac_free))
	{
		const PLIST_ENTRY t = RemoveTailList(& emac_free);
		emacbuf_t * const p = CONTAINING_RECORD(t, emacbuf_t, item);
		* tp = p;
		LowerIrql(oldIrql);
		return 1;
	}
	if (! IsListEmpty(& emac_ready))
	{
		const PLIST_ENTRY t = RemoveTailList(& emac_ready);
		emacbuf_t * const p = CONTAINING_RECORD(t, emacbuf_t, item);
		* tp = p;
		LowerIrql(oldIrql);
		return 1;
	}
	LowerIrql(oldIrql);
	return 0;
}

static int emac_buffers_ready(emacbuf_t * * tp)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	if (! IsListEmpty(& emac_ready))
	{
		const PLIST_ENTRY t = RemoveTailList(& emac_ready);
		LowerIrql(oldIrql);
		emacbuf_t * const p = CONTAINING_RECORD(t, emacbuf_t, item);
		* tp = p;
		return 1;
	}
	LowerIrql(oldIrql);
	return 0;
}

static void emac_buffers_release(emacbuf_t * p)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	InsertHeadList(& emac_free, & p->item);
	LowerIrql(oldIrql);
}

// сохранить принятый
static void emac_buffers_rx(emacbuf_t * p)
{
	InsertHeadList(& emac_ready, & p->item);
}


enum { EMAC_FRAMESZ = 2048 - 4 };
static RAMNC uint8_t rxbuff [EMAC_FRAMESZ];
static RAMNC __ALIGNED(4) uint32_t emac_rxdesc [1] [4];
static RAMNC uint8_t txbuff [EMAC_FRAMESZ];
static RAMNC __ALIGNED(4) uint32_t emac_txdesc [1] [4];

static struct netif emac_netif_data;

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
static err_t emac_linkoutput_fn(struct netif *netif, struct pbuf *p)
{
	//PRINTF("emac_linkoutput_fn\n");
    int i = 0;
    //struct pbuf *q;
	const portholder_t sta = HARDWARE_EMAC_PTR->EMAC_INT_STA;
	//printhex32((uintptr_t) emac_txdesc [i], emac_txdesc [i], sizeof emac_txdesc [i]);
	unsigned w;
	w = 100;
	while (w -- && (emac_txdesc [i][0] & (UINT32_C(1) << 31)) != 0)
	{
		local_delay_ms(1);
		//board_dpc_processing();
	}

	if ((emac_txdesc [i][0] & (UINT32_C(1) << 31)) == 0)
	{
		//HARDWARE_EMAC_PTR->EMAC_TX_CTL1 &= ~ (UINT32_C(1) << 30);	// DMA EN
		//PRINTF("emac_linkoutput_fn: sta=%08X\n", (unsigned) sta);	// 40000025

		//HARDWARE_EMAC_PTR->EMAC_INT_STA = sta;//(UINT32_C(1) << 0);	// TX_P
		pbuf_header(p, - ETH_PAD_SIZE);
		u16_t size = pbuf_copy_partial(p, txbuff, sizeof txbuff, 0);

		// test data
//		memset(txbuff, 0xE5, sizeof txbuff);
//		size = 128;

		emac_txdesc [i][0] =	// status
			1 * (UINT32_C(1) << 31) |	// TX_DESC_CTL
			0;
		// CRC_CTL=0 и CHECKSUM_CTL=3: просто передаёт заказанный в дескрипторе размер
		// CRC_CTL=0 и CHECKSUM_CTL=2: просто передаёт заказанный в дескрипторе размер
		// CRC_CTL=0 и CHECKSUM_CTL=1: просто передаёт заказанный в дескрипторе размер
		// CRC_CTL=0 и CHECKSUM_CTL=0: просто передаёт заказанный в дескрипторе размер
		// CRC_CTL=1 и CHECKSUM_CTL=3: передаёт на 4 меньше
		// CRC_CTL=1 и CHECKSUM_CTL=2: передаёт на 4 меньше
		// CRC_CTL=1 и CHECKSUM_CTL=1: передаёт на 4 меньше
		// CRC_CTL=1 и CHECKSUM_CTL=0: передаёт на 4 меньше
		emac_txdesc [i][1] =	// ctl
			1 * (UINT32_C(1) << 31) |	// TX_INT_CTL
			1 * (UINT32_C(1) << 30) |	// LAST_DESC
			1 * (UINT32_C(1) << 29) |	// FIR_DESC
			//0x03 * (UINT32_C(1) << 27) |	// CHECKSUM_CTL
			//1 * (UINT32_C(1) << 26) |	// CRC_CTL When it is set, the CRC field is not transmitted.
			1 * (UINT32_C(1) << 24) |	// magic. Without it, packets never be sent on H3 SoC
			(size) * (UINT32_C(1) << 0) |	// 10:0 BUF_SIZE
			0;
		emac_txdesc [i][2] = (uintptr_t) txbuff;	// BUF_ADDR
		emac_txdesc [i][3] = (uintptr_t) emac_txdesc [i];	// NEXT_DESC_ADDR

		dcache_clean((uintptr_t) txbuff, sizeof txbuff);

		dcache_clean((uintptr_t) emac_txdesc, sizeof emac_txdesc);
		HARDWARE_EMAC_PTR->EMAC_TX_DMA_DESC_LIST = (uintptr_t) & emac_txdesc [i];

		HARDWARE_EMAC_PTR->EMAC_TX_CTL0 =
			1 * (UINT32_C(1) << 31) |	// TX_EN
			//1 * (UINT32_C(1) << 30) |	// TX_FRM_LEN_CTL
			0;

		//HARDWARE_EMAC_PTR->EMAC_TX_CTL1 &= ~ (UINT32_C(1) << 30);	// DMA EN
		HARDWARE_EMAC_PTR->EMAC_TX_CTL1 |= (UINT32_C(1) << 1);	// TX_MD 1: TX start after TX DMA FIFO located a full frame
		HARDWARE_EMAC_PTR->EMAC_TX_CTL1 |= (UINT32_C(1) << 30);	// DMA EN
		HARDWARE_EMAC_PTR->EMAC_TX_CTL1 |= (UINT32_C(1) << 31);	// TX_DMA_START (auto-clear)
		while (HARDWARE_EMAC_PTR->EMAC_TX_CTL1 & (UINT32_C(1) << 31))
			;
	}
    return ERR_OK;
}

static err_t netif_init_cb(struct netif *netif)
{
	//PRINTF("emac netif_init_cb\n");
	LWIP_ASSERT("netif != NULL", (netif != NULL));
#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "storch";
#endif /* LWIP_NETIF_HOSTNAME */
	netif->mtu = EMAC_MTU;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP
			| NETIF_FLAG_UP | NETIF_FLAG_ETHERNET;
	netif->state = NULL;
	netif->name[0] = 'E';
	netif->name[1] = 'X';
	netif->output = etharp_output;
	netif->linkoutput = emac_linkoutput_fn;	// используется внутри etharp_output
	return ERR_OK;
}

static void EMAC_Handler(void)
{
	const portholder_t sta = HARDWARE_EMAC_PTR->EMAC_INT_STA;
	if (sta & ((UINT32_C(1) << 8)))	// RX_P
	{
		HARDWARE_EMAC_PTR->EMAC_INT_STA = (UINT32_C(1) << 8);	// RX_P
		if (1) //((HARDWARE_EMAC_PTR->EMAC_RX_DMA_STA & 0x07) == 0x03)
		{
			// 0..5 - Destination MAC (multicast)
			// 6..11 - Source MAC [38:D5:47:82:A4:78]
			unsigned v1 = USBD_peek_u16_BE(rxbuff + 12);	// 0x0800 EtherType (Type field)
			unsigned v2 = USBD_peek_u16_BE(rxbuff + 14); 	// 0x4500 IP packet starts from here.
			//printhex32((uintptr_t) 0, emac_rxdesc, sizeof emac_rxdesc);
			//emac_free0, rxbuff, 128);
			emacbuf_t * p;
			if (emac_buffers_alloc(& p) != 0)
			{
				struct pbuf *frame;
				frame = pbuf_alloc(PBUF_RAW, sizeof rxbuff, PBUF_POOL);
				if (frame == NULL)
				{
					TP();
					emac_buffers_release(p);
				}
				else
				{
					pbuf_header(frame, - ETH_PAD_SIZE);
					err_t e = pbuf_take(frame, rxbuff, sizeof rxbuff);	// Copy application supplied data into a pbuf.
					pbuf_header(frame, + ETH_PAD_SIZE);
					if (e == ERR_OK)
					{
						p->frame = frame;
						emac_buffers_rx(p);
					}
					else
					{
						pbuf_free(frame);
						emac_buffers_release(p);
					}
				}
			}
			unsigned i = 0;
			emac_rxdesc [i][0] =
				1 * (UINT32_C(1) << 31) |	// RX_DESC_CTL
		//				1 * (UINT32_C(1) << 9) |	// FIR_DESC
		//				1 * (UINT32_C(1) << 8) |	// LAST_DESC
				0;
			dcache_clean_invalidate((uintptr_t) rxbuff, sizeof rxbuff);
			dcache_clean((uintptr_t) emac_rxdesc, sizeof emac_rxdesc);
		}
		else
		{
			TP();
			PRINTF("EMAC_RX_DMA_STA=%08X\n", (unsigned) HARDWARE_EMAC_PTR->EMAC_RX_DMA_STA);
		}
	}
}

// Receiving Ethernet packets
// user-mode function
static void netif_polling(void * ctx)
{
	(void) ctx;
	emacbuf_t * p;
	while (emac_buffers_ready(& p) != 0)
	{
		struct pbuf *frame = p->frame;
		emac_buffers_release(p);

		err_t e = ethernet_input(frame, & emac_netif_data);
		if (e != ERR_OK)
		{
			  /* This means the pbuf is freed or consumed,
			     so the caller doesn't have to free it again */
		}

	}
}

static void lwip_1s_spool(void * ctx)
{
	(void) ctx;
	sys_check_timeouts();
}

void init_netif(void)
{
	//PRINTF("init_netif start\n");
	emac_buffers_initialize();

	const unsigned ix = HARDWARE_EMAC_IX;	// 0: EMAC0, 1: EMAC1
	CCU->EMAC_BGR_REG |= (UINT32_C(1) << ((0 + ix)));	// Gating Clock for EMACx
	CCU->EMAC_BGR_REG &= ~ (UINT32_C(1) << ((16 + ix)));	// EMACx Reset
	CCU->EMAC_BGR_REG |= (UINT32_C(1) << ((16 + ix)));	// EMACx Reset
	//PRINTF("CCU->EMAC_BGR_REG=%08X (@%p)\n", (unsigned) CCU->EMAC_BGR_REG, & CCU->EMAC_BGR_REG);

	{
		// The working clock of EMAC is from AHB3.

		HARDWARE_EMAC_EPHY_CLK_REG = 0x00051c06; // 0x00051c06 0x00053c01
		//PRINTF("EMAC_BASIC_CTL1=%08X\n", (unsigned) HARDWARE_EMAC_PTR->EMAC_BASIC_CTL1);
		//printhex32((uintptr_t) HARDWARE_EMAC_PTR, HARDWARE_EMAC_PTR, 256);
		HARDWARE_EMAC_PTR->EMAC_BASIC_CTL1 |= (UINT32_C(1) << 0);	// Soft reset
		while ((HARDWARE_EMAC_PTR->EMAC_BASIC_CTL1 & (UINT32_C(1) << 0)) != 0)
			;


		HARDWARE_EMAC_PTR->EMAC_BASIC_CTL0 =
			//0x03 * (UINT32_C(1) << 2) |	// SPEED - 00: 1000 Mbit/s, 10: 10 Mbit/s, 11: 100 Mbit/s
			0x01 * (UINT32_C(1) << 0) | // DUPLEX - 1: Full-duplex
			0;
		HARDWARE_EMAC_PTR->EMAC_BASIC_CTL1 =
			0x08 * (UINT32_C(1) << 24) |	// BURST_LEN - The burst length of RX and TX DMA transfer.
			0;

//			PRINTF("EMAC_BASIC_CTL0=%08X\n", (unsigned) HARDWARE_EMAC_PTR->EMAC_BASIC_CTL0);
//			PRINTF("EMAC_BASIC_CTL1=%08X\n", (unsigned) HARDWARE_EMAC_PTR->EMAC_BASIC_CTL1);
//			PRINTF("EMAC_RGMII_STA=%08X\n", (unsigned) HARDWARE_EMAC_PTR->EMAC_RGMII_STA);

		// ether 1A:0C:74:06:AF:64
		HARDWARE_EMAC_PTR->EMAC_ADDR [0].HIGH = 0x000064AF;
		HARDWARE_EMAC_PTR->EMAC_ADDR [0].LOW = 0x06740C1A;
	}
	// RX init
	{
		unsigned len = EMAC_FRAMESZ;
		unsigned i = 0;
		emac_rxdesc [i][0] =
			1 * (UINT32_C(1) << 31) |	// RX_DESC_CTL
//				1 * (UINT32_C(1) << 9) |	// FIR_DESC
//				1 * (UINT32_C(1) << 8) |	// LAST_DESC
			0;
		emac_rxdesc [i][1] =
				len * (UINT32_C(1) << 0) |	// 10:0 BUF_SIZE
			0;
		emac_rxdesc [i][2] = (uintptr_t) rxbuff;	// BUF_ADDR
		emac_rxdesc [i][3] = (uintptr_t) emac_rxdesc [0];	// NEXT_DESC_ADDR
		//printhex32((uintptr_t) emac_rxdesc, emac_rxdesc, sizeof emac_rxdesc);

		arm_hardware_set_handler_system(HARDWARE_EMAC_IRQ, EMAC_Handler);

		HARDWARE_EMAC_PTR->EMAC_RX_FRM_FLT =
//					1 * (UINT32_C(1) << 31) |	// DIS_ADDR_FILTER
				1 * (UINT32_C(1) << 0) |	// RX_ALL
				0;

		HARDWARE_EMAC_PTR->EMAC_RX_DMA_DESC_LIST = (uintptr_t) emac_rxdesc;
		//HARDWARE_EMAC_PTR->EMAC_RX_CTL0 = 0xb8000000;
		HARDWARE_EMAC_PTR->EMAC_RX_CTL0 =
			1 * (UINT32_C(1) << 31) |	// RX_EN
			//1 * (UINT32_C(1) << 29) |	// JUMBO_FRM_EN
			//1 * (UINT32_C(1) << 28) |	// STRIP_FCS
			1 * (UINT32_C(1) << 27) |	// CHECK_CRC 1: Calculate CRC and check the IPv4 Header Checksum.
			0;
		HARDWARE_EMAC_PTR->EMAC_RX_CTL1 =
				1 * (UINT32_C(1) << 1) |	// 1: RX start read after RX DMA FIFO located a full frame
				1 * (UINT32_C(1) << 30) |	// /RX_DMA_EN
				0;

		HARDWARE_EMAC_PTR->EMAC_INT_EN |= (UINT32_C(1) << 8); // RX_INT_EN

		HARDWARE_EMAC_PTR->EMAC_RX_CTL1 |= (UINT32_C(1) << 31);	// RX_DMA_START (auto-clear)
	}
	// TX init
	{
		unsigned len = EMAC_FRAMESZ;
		unsigned i = 0;
		emac_txdesc [i][0] =
			//1 * (UINT32_C(1) << 31) |	// TX_DESC_CTL
//				1 * (UINT32_C(1) << 9) |	// FIR_DESC
//				1 * (UINT32_C(1) << 8) |	// LAST_DESC
			0;
		emac_txdesc [i][1] =
				len * (UINT32_C(1) << 0) |	// 10:0 BUF_SIZE
			0;
		emac_txdesc [i][2] = (uintptr_t) txbuff;	// BUF_ADDR
		emac_txdesc [i][3] = (uintptr_t) emac_txdesc [0];	// NEXT_DESC_ADDR
		//printhex32((uintptr_t) emac_rxdesc, emac_rxdesc, sizeof emac_rxdesc);

		//arm_hardware_set_handler_system(HARDWARE_EMAC_IRQ, EMAC_Handler);

		//HARDWARE_EMAC_PTR->EMAC_RX_CTL0 = 0xb8000000;

		//HARDWARE_EMAC_PTR->EMAC_INT_EN |= (UINT32_C(1) << 0); // TX_INT_EN

		//HARDWARE_EMAC_PTR->EMAC_TX_CTL1 |= (UINT32_C(1) << 31);	// TX_DMA_START (auto-clear)
	}

	static const  uint8_t hwaddrv [6]  = { HWADDR };

	static ip_addr_t netmask;// [4] = NETMASK;
	static ip_addr_t gateway;// [4] = GATEWAY;

	IP4_ADDR(& netmask, myNETMASK [0], myNETMASK [1], myNETMASK [2], myNETMASK [3]);
	IP4_ADDR(& gateway, myGATEWAY [0], myGATEWAY [1], myGATEWAY [2], myGATEWAY [3]);

	static ip_addr_t vaddr;// [4]  = IPADDR;
	IP4_ADDR(& vaddr, myIP [0], myIP [1], myIP [2], myIP [3]);

	struct netif  *netif = &emac_netif_data;
	netif->hwaddr_len = 6;
	memcpy(netif->hwaddr, hwaddrv, 6);

	netif = netif_add(netif, & vaddr, & netmask, & gateway, NULL, netif_init_cb, ip_input);
	netif_set_default(netif);

	while (!netif_is_up(netif))
		;

#if LWIP_AUTOIP
	  autoip_start(netif);
#endif /* LWIP_AUTOIP */

	{
		static dpcobj_t dpcobj;

		dpcobj_initialize(& dpcobj, netif_polling, NULL);
		board_dpc_addentry(& dpcobj, board_dpc_coreid());
	}
	{
		static ticker_t ticker;
		static dpcobj_t dpcobj;

		dpcobj_initialize(& dpcobj, lwip_1s_spool, NULL);
		ticker_initialize_user(& ticker, NTICKS(1000), & dpcobj);
		ticker_add(& ticker);
	}

	//PRINTF("init_netif done\n");
}
u32_t sys_jiffies(void)
{
	return 33;
}

#endif /* defined (ETH) && WITHLWIP */

#endif /* WITHETHHW */

