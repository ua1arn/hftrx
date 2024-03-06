


#define  CAN0  0x02504000
#define  CAN1  0x02504400


#define CAN_OFFSET(can_base,offset)          (*((volatile uint32_t *)(can_base + (offset))))

#define CAN_MSEL(can_base)       CAN_OFFSET(can_base,0x0000) //CAN mode select register
#define CAN_CMD(can_base)        CAN_OFFSET(can_base,0x0004)  //CAN command register
#define CAN_STA(can_base)        CAN_OFFSET(can_base,0x0008)  //CAN status register
#define CAN_INT(can_base)        CAN_OFFSET(can_base,0x000C)  //CAN interrupt register
#define CAN_INTEN(can_base)      CAN_OFFSET(can_base,0x0010) //CAN interrupt enable register
#define CAN_BUSTIME(can_base)    CAN_OFFSET(can_base,0x0014) //CAN bus timing register
#define CAN_TEWL(can_base)       CAN_OFFSET(can_base,0x0018)  //CAN TX error warning limit register
#define CAN_ERRC(can_base)       CAN_OFFSET(can_base,0x001c)  //CAN error counter register
#define CAN_RMCNT(can_base)      CAN_OFFSET(can_base,0x0020)  //CAN receive message counter register
#define CAN_RBUF_SADDR(can_base) CAN_OFFSET(can_base,0x0024) //CAN receive buffer start address register
#define CAN_TRBUF0(can_base)     CAN_OFFSET(can_base,0x0040) //CAN TX/RX message buffer 0 register
#define CAN_TRBUF1(can_base)     CAN_OFFSET(can_base,0x0044) //CAN TX/RX message buffer 0 register
#define CAN_TRBUF2(can_base)     CAN_OFFSET(can_base,0x0048) //CAN TX/RX message buffer 0 register
#define CAN_TRBUF3(can_base)     CAN_OFFSET(can_base,0x004c) //CAN TX/RX message buffer 0 register
#define CAN_TRBUF4(can_base)     CAN_OFFSET(can_base,0x0050) //CAN TX/RX message buffer 0 register
#define CAN_TRBUF5(can_base)     CAN_OFFSET(can_base,0x0054) //CAN TX/RX message buffer 0 register
#define CAN_TRBUF6(can_base)     CAN_OFFSET(can_base,0x0058) //CAN TX/RX message buffer 0 register
#define CAN_TRBUF7(can_base)     CAN_OFFSET(can_base,0x005c) //CAN TX/RX message buffer 0 register
#define CAN_TRBUF8(can_base)     CAN_OFFSET(can_base,0x0060) //CAN TX/RX message buffer 0 register
#define CAN_TRBUF9(can_base)     CAN_OFFSET(can_base,0x0064) //CAN TX/RX message buffer 0 register
#define CAN_TRBUF10(can_base)    CAN_OFFSET(can_base,0x0068) //CAN TX/RX message buffer 0 register
#define CAN_TRBUF11(can_base)    CAN_OFFSET(can_base,0x006c) //CAN TX/RX message buffer 0 register
#define CAN_TRBUF12(can_base)    CAN_OFFSET(can_base,0x0070) //CAN TX/RX message buffer 0 register
#define CAN_ACPC(can_base)       CAN_OFFSET(can_base,0x0028) //CAN acceptance code 0 register(reset mode)
#define CAN_ACPM(can_base)       CAN_OFFSET(can_base,0x002C) //CAN acceptance mask 0 register(reset mode)
#define CAN_RBUF_RBACK(can_base) CAN_OFFSET(can_base,0x0180) //~0x1b0 //CAN transmit buffer for read back register
#define CAN_VERSION(can_base)    CAN_OFFSET(can_base,0x0300) //CAN Version Register


#define CAN_ID_STD	0
#define CAN_ID_EXT	1


typedef struct
{
  unsigned int  Id;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

  unsigned char IDE;     /*!< Specifies the type of identifier for the message that
                        will be transmitted. This parameter can be a value
                        of @ref CAN_identifier_type */

  unsigned char DLC;     /*!< Specifies the length of the frame that will be
                        transmitted. This parameter can be a value between
                        0 to 8 */

  unsigned char Data[8]; /*!< Contains the data to be transmitted. It ranges from 0
                        to 0xFF. */
} CanTxMsg;




unsigned int t113_can_irq_handle(void*  p);


