#include "hardware.h"
#include "formats.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <conio.h>
#include <time.h>

#include "rs.h"

#define u8  unsigned char
#define u16 unsigned short

#define EE 256       /* 8*/
#define KK (NN-EE)
#define K  4096      /*16*/

dtype data[NN];
dtype data2[NN];

//#define show

static __inline__ unsigned long long rdtsc(void)
{
//    unsigned hi, lo;
//    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
//    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
	return cpu_getdebugticks();
}

void hmain(void)
{
 srand(time(NULL));

 init_rs(KK,K);

 PRINTF("RS init\n");

 memset(data ,0,sizeof(data ));
 memset(data2,0,sizeof(data2));

 unsigned long long t;

 Again:

 for(int i=0;i<K;i++)data[i]=rand()&NN;

 t=rdtsc();
 int r=encode_rs(data);
 t=rdtsc()-t;
 PRINTF("Encode: %0.1lf FPS   ",(double)3000000000ULL/(double)t); //"%llu\n"

 memcpy(data2,data,NN*sizeof(data[0]));

 if(r==-1)
 {
  PRINTF("\n\nEncode Error!\n");
  //getch();
  //exit(-1);
  return;
 }

#ifdef show
 PRINTF("\n\n");

 for(int i=0;i<K;i++)
 {
  PRINTF("%04X ",data[i]);
 }
#endif

 int n=rand()%((EE/2)+1); //0    ..EE/2
 int m=(EE/2)-n;          //EE/2 ..0

 for(int i=0;i<n+0;i++)data[   (((u16)rand())%K )]=(u16)rand();
 for(int i=0;i<m+0;i++)data[KK+(((u16)rand())%EE)]=(u16)rand();

#ifdef show
 PRINTF("\n\n");

 for(int i=0;i<K;i++)
 {
  if(data[i])     PRINTF("%04X ",data[i]);
  else            PRINTF("     ");
 }
 for(int i=0;i<EE;i++)
 {
  if(data[KK+i])PRINTF("%04X ",data[KK+i]);
  else         PRINTF("     ");
 }

#endif

//getch();

// t=rdtsc();
// r=eras_dec_rs(data,NULL,0);
// t=rdtsc()-t;
// PRINTF("Decode: %0.1lf FPS\n",(double)3000000000ULL/(double)t); //"%llu\n"

 if(r==-1)
 {
  PRINTF("\n\nDecode Error!\n");
  return;
 }

 if(memcmp(data,data2,NN*sizeof(data[0]))!=0)
 {
  PRINTF("\n\nNot Compare!\n");
  return;
 }

#ifdef show

 PRINTF("\n\n");

 for(int i=0;i<K ;i++)PRINTF("%04X ",data[   i]);
 for(int i=0;i<EE;i++)PRINTF("%04X ",data[KK+i]);


#endif

// PRINTF("%c",(rand()%10)+'0');

 goto Again;

}
