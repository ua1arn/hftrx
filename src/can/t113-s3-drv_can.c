#include "hardware.h"
#include "gpio.h"
#include "formats.h"

#define CAN_ID_STD	0
#define CAN_ID_EXT	1

typedef struct
{
	uint16_t Id; /*!< Specifies the standard identifier. This parameter can be a value between 0 to 0x7FF. */
	uint8_t IDE; /*!< Specifies the type of identifier for the message that will be transmitted. This parameter can be a value of @ref CAN_identifier_type */
	uint8_t DLC; /*!< Specifies the length of the frame that will be transmitted. This parameter can be a value between 0 to 8 */
	uint8_t Data[8]; /*!< Contains the data to be transmitted. It ranges from 0 to 0xFF. */
} CanTxMsg;

void hardwate_can0_initialize(void);
void hardwate_can1_initialize(void);

#if WITHCAN0HW

void hardwate_can0_initialize(void)
{
	const unsigned busfreq = 5000000; /* 5 MHz ? 500KHz CAN?? */
	const unsigned ix = 0;

	HARDWARE_CAN0_INITIALIZE();

    CCU->CAN_BGR_REG  |= (1 << (16 + ix)) | (1 << ix); /* CCU_CAN_BGR */

    CAN0->CAN_MSEL |= 1; /* Reset mode selected*/

    unsigned divider = calcdivround2(allwnr_t113_get_can_freq(), busfreq);
    CAN0->CAN_BUSTIME = (
		((divider - 1)   << 0) |        /* apb_clk = 100Mz , 100/10 = 10MHz*/
		(1 << 14) |   /* Synchronization Jump Width :2 Tq clock cycles *///(1 << 14)
		(6 << 16) |  /*Phase Segment 1 : 14 Tq clock cycles  *///(11 << 16)
		( 1 << 20) |  /*Phase Segment 2 :  4 Tq clock cycles  *///( 4 << 20)
		( 0 << 23)) |
		0;  /*  Bus line is sampled three times at the sample point */

    CAN0->CAN_ACPC  = 0xffffffff; /*?????????????*/
    CAN0->CAN_ACPM  = 0xffffffff; /*?????????????*/

    CAN0->CAN_MSEL &= (~1);

   // CAN_MSEL |= (1 << 2); /* Loopback Mode */

    CAN0->CAN_MSEL |= (1 << 3);/* Single Filter  */

    /* ????*/
    CAN0->CAN_INTEN = 0x0FD;	//все прерывания кроме передачи
}


void CAN_Receive(CAN_TypeDef * can, CanTxMsg *msq) {
	volatile uint32_t * src;
	if (can->CAN_TRBUF  [0] & 0x80) {	//ext
		msq->IDE = CAN_ID_EXT;
		msq->DLC = can->CAN_TRBUF [0] & 0x0F;
		msq->Id =((can->CAN_TRBUF [1] & 0xFF)<<21)|((can->CAN_TRBUF [2] & 0xFF)<<13)|((can->CAN_TRBUF [3] & 0xFF)<<5)|((can->CAN_TRBUF [4] & 0xF8)>>3);
		src = &can->CAN_TRBUF [5];
	}else{	//std
		msq->IDE = CAN_ID_STD;
		msq->DLC = can->CAN_TRBUF [0] & 0x0F;
		msq->Id = ((can->CAN_TRBUF [1] &  0xFF)<<3)|((can->CAN_TRBUF [2] &  0xE0)>>5);
		src = &can->CAN_TRBUF [3];
	}
	for (int i = 0; i < ARRAY_SIZE(msq->Data) && i < msq->DLC; i++){
		msq->Data [i] = src [i];
	}
	can->CAN_CMD |= (1 << 2);	//сброс флага что буфер считали
}


static uint32_t rxd_buf [13];
void can_read_data(CAN_TypeDef * can, uint8_t * buf){
    volatile uint32_t * src =  &can->CAN_TRBUF [0];
    for(int i = 0; i < 13; i++)
       buf [i] = (uint8_t)src [i];

    can->CAN_CMD |= (1 << 2);	//сброс флага что буфер считали
}

static CanTxMsg Test_msq;
unsigned int t113_can_irq_handle(CAN_TypeDef * can) {
   uint32_t    int_sta =   can->CAN_INT;

   if(int_sta &  1){
       //прерывание по приему сообщения
	   while (can->CAN_STA& 0x01) {
		   PRINTF("int_sta &  1 \n");
		   //can_read_data(can_base,rxd_buf);
		   CAN_Receive(can, &Test_msq);
	   }
       /*  ????  */
   }
   if(int_sta & 2 ){//прерывание по отправке сообщения
	  // PRINTF("int_sta &  2 \n");
      /*????*/
   }
   if(int_sta & 0xFC ){	//прерываине по ошибке - не отправлено сообщение например
	   PRINTF("int_sta &  FC \n");
      /* ? ? ??  */
   }
   can->CAN_INT = int_sta;
   return 0;
}



void can_send_data(CAN_TypeDef * can, uint32_t    can_base,uint8_t * buf,int  len){

    can->CAN_CMD |= (1 << 2);

    can->CAN_MSEL &= (~1);

    int retry = 1000;
    do{

       if(--retry == 0) break;
    }while(!(can->CAN_STA & (1 << 2)));

    if(retry == 0) return;//

    volatile uint32_t * des =  &can->CAN_TRBUF [0];

    for(int i = 0; i < len; i++)
     des [i] = buf [i];
    //CAN_CMD = (1 << 4);
    can->CAN_CMD |= (1 << 0);
}

static CanTxMsg CanTxMessage={0x256,CAN_ID_EXT,8,{0xAA,0xAA,0xAA,0xAA,0x55,0xAA,0xAA,0x55}};

void CAN_Transmit(CAN_TypeDef * can, CanTxMsg *msq) {
	can->CAN_CMD |= (1 << 2);
	can->CAN_MSEL &= (~1);

	int retry = 1000;
	do {
		if (--retry == 0)
			break;
	} while (!(can->CAN_STA & (1 << 2)));

	if (retry == 0)
		return;

	volatile uint32_t *des;
	if (msq->IDE == CAN_ID_STD) {
		can->CAN_TRBUF [0] = msq->DLC;
		can->CAN_TRBUF [1] = ((msq->Id >> 3) & 0xFF);
		can->CAN_TRBUF [2] = ((msq->Id << 5) & 0xE0);
		des = &can->CAN_TRBUF [3];
	} else {
		can->CAN_TRBUF [0] = msq->DLC | 0x80;
		can->CAN_TRBUF [1] = ((msq->Id >> 21) & 0xFF);
		can->CAN_TRBUF [2] = ((msq->Id >> 13) & 0xFF);
		can->CAN_TRBUF [3] = ((msq->Id >> 5) & 0xFF);
		can->CAN_TRBUF [4] = ((msq->Id << 3) & 0xF8);
		des = &can->CAN_TRBUF [5];
	}

	for (int i = 0; i < msq->DLC; i++){
		des [i] = msq->Data [i];
	}
	//CAN_CMD = (1 << 4);
	can->CAN_CMD |= (1 << 0);	//команда отправить данные
}





void t113_can_test(void){
//    ini_can(0);
	CAN_Transmit(CAN0,&CanTxMessage);
}



#endif /* WITHCAN0HW */

#if WITHCAN1HW

#endif /* WITHCAN1HW */

