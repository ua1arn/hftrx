/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHETHHW && CPUSTYLE_STM32MP1 && WITHLWIP

#include "stm32mp1xx_hal_eth.h"

//#ifndef HAL_ETH_MODULE_ENABLED
//	#error Check HAL_ETH_MODULE_ENABLED in src/hal/stm32mp1xx_hal_conf.h
//#endif

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

/* ETH Setting  */
#define ETH_DMA_TRANSMIT_TIMEOUT               ( 20U )
/* ETH_RX_BUFFER_SIZE parameter is defined in lwipopts.h */


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

__ALIGN_BEGIN ETH_DMADescTypeDef DMARxDscrTab [ETH_RX_DESC_CNT] __ALIGN_END; /* Ethernet Rx DMA Descriptors */
__ALIGN_BEGIN ETH_DMADescTypeDef DMATxDscrTab [ETH_TX_DESC_CNT] __ALIGN_END;   /* Ethernet Tx DMA Descriptors */
__ALIGN_BEGIN uint8_t Rx_Buff [ETH_RX_DESC_CNT][ETH_RX_BUFFER_SIZE] __ALIGN_END; /* Ethernet Receive Buffers */

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

//    custom_pbuf  = (struct pbuf_custom*)LWIP_MEMPOOL_ALLOC(RX_POOL);
//    custom_pbuf->custom_free_function = pbuf_free_custom;
//
//    p = pbuf_alloced_custom(PBUF_RAW, framelength, PBUF_REF, custom_pbuf, RxBuff->buffer, framelength);

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

int nic_can_send(void)
{
	return 0;
}

void nic_send(const uint8_t * data, int size)
{

}

void nic_initialize(void)
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
#endif /* WITHETHHW */

