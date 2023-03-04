//CAN подключен к шине APB1

#include <lib/t113-s3-drv_can.h>
#include "gpio.h"



void CAN_Receive(uint32_t can_base, CanTxMsg *msq) {
	volatile uint32_t * src;
	if (CAN_TRBUF0(can_base)&0x80) {	//ext
		msq->IDE = CAN_ID_EXT;
		msq->DLC = CAN_TRBUF0(can_base)&0x0F;
		msq->Id =((CAN_TRBUF1(can_base)&0xFF)<<21)|((CAN_TRBUF2(can_base)&0xFF)<<13)|((CAN_TRBUF3(can_base)&0xFF)<<5)|((CAN_TRBUF4(can_base)&0xF8)>>3);
		src = &CAN_TRBUF5(can_base);
	}else{	//std
		msq->IDE = CAN_ID_STD;
		msq->DLC = CAN_TRBUF0(can_base)&0x0F;
		msq->Id = ((CAN_TRBUF1(can_base)& 0xFF)<<3)|((CAN_TRBUF2(can_base)& 0xE0)>>5);
		src = &CAN_TRBUF3(can_base);
	}
	for (int i = 0; i < msq->DLC; i++){
		msq->Data[i] = src[i];
	}
	CAN_CMD(can_base) |= (1 << 2);	//сброс флага что буфер считали
}

typedef  struct _t113_can_control_t_
{
  volatile uint32_t  * can_clk_gate;
  uint32_t  can_base;
  uint32_t  int_id;
}t113_can_control_t;

t113_can_control_t t113_can[2] =
{
  {
    (volatile uint32_t  *)0x0200192C,
    0x02504000,
    53
  },
  {
    (volatile uint32_t  *)0x0200192C,
    0x02504400,
    54
  }

};


uint32_t rxd_buf[13];
void can_read_data(uint32_t    can_base,uint8_t * buf){
    uint32_t * src =  &CAN_TRBUF0(can_base);
    for(int i = 0; i < 13; i++)
       buf[i] = (char)src[i];

     CAN_CMD(can_base) |= (1 << 2);	//сброс флага что буфер считали
}

CanTxMsg Test_msq;
unsigned int t113_can_irq_handle(void*  p){
   uint32_t    can_base = CAN0;
   uint32_t    int_sta =   CAN_INT(can_base);

   if(int_sta &  1){
       //прерывание по приему сообщения
	   while (CAN_STA(can_base)& 0x01) {
		   sys_uart_puts("int_sta &  1 \n");
		   //can_read_data(can_base,rxd_buf);
		   CAN_Receive(CAN0, &Test_msq);
	   }
       /*  ????  */
   }
   if(int_sta & 2 ){//прерывание по отправке сообщения
	  // sys_uart_puts("int_sta &  2 \n");
      /*????*/
   }
   if(int_sta & 0xFC ){	//прерываине по ошибке - не отправлено сообщение например
	   sys_uart_puts("int_sta &  FC \n");
      /* ? ? ??  */
   }
   CAN_INT(can_base) = int_sta;
   return 0;
}





void ini_can(int can_id){
    if(can_id > 1) return;

	GPIO_InitTypeDef InitGpio;
	InitGpio.GPIO_Mode = GPIO_Mode_AF8;
	InitGpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	InitGpio.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_Init(GPIOB, &InitGpio);


    t113_can_control_t * can_info = &t113_can[can_id];

    uint32_t   can_base = can_info->can_base;

    int can_int_id = can_info->int_id;

    *can_info->can_clk_gate  |= (1 << (16 + can_id)) | (1 << can_id);/* CCU_CAN_BGR */



    CAN_MSEL(can_base) |= 1; /* Reset mode selected*/
   /* 500KHz CAN?? */
    CAN_BUSTIME(can_base) = (19          /* apb_clk = 100Mz , 100/10 = 10MHz*/
                           | (1 << 14)   /* Synchronization Jump Width :2 Tq clock cycles *///(1 << 14)
                           | (6 << 16)  /*Phase Segment 1 : 14 Tq clock cycles  *///(11 << 16)
                           | ( 1 << 20)  /*Phase Segment 2 :  4 Tq clock cycles  *///( 4 << 20)
                           | ( 0 << 23));  /*  Bus line is sampled three times at the sample point */

   CAN_ACPC(can_base)  = 0xffffffff; /*?????????????*/
   CAN_ACPM(can_base)  = 0xffffffff; /*?????????????*/

   CAN_MSEL(can_base) &= (~1);

   // CAN_MSEL(can_base) |= (1 << 2); /* Loopback Mode */

   CAN_MSEL(can_base) |= (1 << 3);/* Single Filter  */

    /* ????*/
    CAN_INTEN(can_base) = 0x0FD;	//все прерывания кроме передачи
}


void can_send_data(uint32_t    can_base,uint8_t * buf,int  len){

    CAN_CMD(can_base) |= (1 << 2);

    CAN_MSEL(can_base) &= (~1);

    int retry = 1000;
    do{

       if(--retry == 0) break;
    }while(!(CAN_STA(can_base) & (1 << 2)));

    if(retry == 0) return;//

    uint32_t * des =  &CAN_TRBUF0(can_base);

    for(int i = 0; i < len; i++)
     des[i] = buf[i];
    //CAN_CMD(can_base) = (1 << 4);
     CAN_CMD(can_base) |= (1 << 0);
}

CanTxMsg CanTxMessage={0x256,CAN_ID_EXT,8,0xAA,0xAA,0xAA,0xAA,0x55,0xAA,0xAA,0x55};

void CAN_Transmit(uint32_t can_base, CanTxMsg *msq) {
	CAN_CMD(can_base) |= (1 << 2);
	CAN_MSEL(can_base) &= (~1);

	int retry = 1000;
	do {
		if (--retry == 0)
			break;
	} while (!(CAN_STA(can_base) & (1 << 2)));

	if (retry == 0)
		return;

	volatile uint32_t *des;
	if (msq->IDE == CAN_ID_STD) {
		CAN_TRBUF0(can_base) = msq->DLC;
		CAN_TRBUF1(can_base) = ((msq->Id >> 3) & 0xFF);
		CAN_TRBUF2(can_base) = ((msq->Id << 5) & 0xE0);
		des = &CAN_TRBUF3(can_base);
	} else {
		CAN_TRBUF0(can_base) = msq->DLC | 0x80;
		CAN_TRBUF1(can_base) = ((msq->Id >> 21) & 0xFF);
		CAN_TRBUF2(can_base) = ((msq->Id >> 13) & 0xFF);
		CAN_TRBUF3(can_base) = ((msq->Id >> 5) & 0xFF);
		CAN_TRBUF4(can_base) = ((msq->Id << 3) & 0xF8);
		des = &CAN_TRBUF5(can_base);
	}

	for (int i = 0; i < msq->DLC; i++){
		des[i] = msq->Data[i];
	}
	//CAN_CMD(can_base) = (1 << 4);
	CAN_CMD(can_base) |= (1 << 0);	//команда отправить данные
}





void t113_can_test(void){
//    ini_can(0);
	CAN_Transmit(CAN0,&CanTxMessage);
}



