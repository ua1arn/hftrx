#include "hardware.h"
#include "formats.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//
//#include "Type.h"
//
//#include "timer.h"
//#include "PWM.h"
//#include "LowLevel.h"

#include "rs.h"

dtype data[NN];
dtype data2[NN];

void hmain(void)
{
	PRINTF("cpufreq=%u\n", (unsigned) CPU_FREQ);
// LowLevel_Init();
// PWM_Init(75);

 init_rs(KK); //init_rs(KK,K);

 PRINTF("RS init\n");

 memset(data ,0,sizeof(data ));
 memset(data2,0,sizeof(data2));

 unsigned long long t;

 Again:

 for(int i=0;i<K;i++)data[i]=rand()&NN;

 t=cpu_getdebugticks();
 int r=encode_rs(data,&data[KK]); //encode_rs(data);
 t=cpu_getdebugticks()-t;
 PRINTF("Encode: %0.1lf FPS   ",(double)CPU_FREQ/(double)t); //"%llu\n"

 memcpy(data2,data,NN*sizeof(data[0]));

 if(r==-1)
 {
  PRINTF("\n\nEncode Error!\n");
  while(1);
 }

 uint16_t n=((uint16_t)rand())%((EE/2)+1); //0      .. (EE/2)
 uint16_t m=(EE/2)-n;                 //(EE/2) .. 0

 for(uint16_t i=0;i<n;i++)data[   (((uint16_t)rand())%K )]=(uint16_t)rand();
 for(uint16_t i=0;i<m;i++)data[KK+(((uint16_t)rand())%EE)]=(uint16_t)rand();

// for(int i=0;i<EE/4;i++)data[   (((uint16_t)rand())%K )]=(uint16_t)rand();
// for(int i=0;i<EE/4;i++)data[KK+(((uint16_t)rand())%EE)]=(uint16_t)rand();

 t=cpu_getdebugticks();
 r=eras_dec_rs(data,NULL,0);
 t=cpu_getdebugticks()-t;
 PRINTF("Decode: %0.1lf FPS\n",(double)CPU_FREQ/(double)t); //"%llu\n"

 if(r==-1)
 {
  PRINTF("\n\nDecode Error!\n");
  while(1);
 }

 if(memcmp(data,data2,NN*sizeof(data[0]))!=0)
 {
  PRINTF("\n\nNot Compare!\n");
  while(1);
 }

 goto Again;

}
