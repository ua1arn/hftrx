#if (CPUSTYLE_T113 || CPUSTYLE_F133)

#include "src/display/display.h"
#define VIDEO_MEMORY0 ((uintptr_t) colmain_fb_draw())
#define VIDEO_MEMORY1 ((uintptr_t) colmain_fb_draw())

#ifndef min
#define min( x, y ) ( (x) < (y) ? (x) : (y) )
#endif

#include <string.h>
#include <math.h>

#include "T113-s3/Type.h"
//
//#include "LowLevel.h"
//#include "UART.h"
//#include "PIO.h"
//#include "delay.h"
//#include "Gate.h"
//#include "timer.h"
//#include "core_armv7.h"
//#include "interrupt.h"

#include "T113-s3/fb-t113-rgb.h"
#include "T113-s3/g2d.h"

#include "LuPng/lupng.h"

//#include "sdcard.h"

//PNG-файлы
#include "PICTURE.PNG.h"
#include "dog.png.h"
//#include "Back0.png.h"
#include "Back1.png.h"
//#include "Bubble.png.h"
//#include "Cobra.png.h"
//#include "Logo.png.h"
//#include "scale03.png.h"

LuImage *png[5]; //массив указателей на объект PNG (число PNG)

//----------------------------------------------------------------------------------------------------------

void PNG_Load(LuImage **png,const void *buffer)      //разжимает PNG из buffer в новую выделенную память(malloc)
{
 *png=luPngReadMemory((char*)buffer);
}

void PNG_Draw(u32 px,u32 py,LuImage *png,u8 layer)   //выводит PNG на дисплей в заданных координатах
{
 if(png==NULL)return;

 u32 line=png->width<<2;

 u32 * __restrict__ src=(u32*)png->data;
 u32 * __restrict__ dst=(u32*)(layer?VIDEO_MEMORY1:VIDEO_MEMORY0);
 u32 * __restrict__ dst0=dst;

 dst+=(LCD_PIXEL_WIDTH*py)+px;

 for(u32 y=0;y<png->height;y++)
 {
  memcpy(dst,src,line);
  dst+=LCD_PIXEL_WIDTH;
  src+=png->width;
 }
 arm_hardware_flush((uintptr_t) dst0, LCD_PIXEL_WIDTH * png->height);
}

void PNG_Free(LuImage *png)                          //освобождает память разжатого PNG
{
 if(png)luImageRelease(png,NULL);
}

//----------------------------------------------------------------------------------------------------------
//
//void ClearVideoMemory(void)
//{
// memset((void*)VIDEO_MEMORY,0,LCD_PIXEL_WIDTH*LCD_PIXEL_HEIGHT*BYTE_PER_PIXEL*2); //очистка видеопамяти
//}

g2d_fillrect   G2D_FILLRECT;
g2d_blt        G2D_BLT;
g2d_stretchblt G2D_STRETCHBLT;

void PNG_Background(LuImage *png,u32 memory)           //выводит фоновый PNG на дисплей
{
 if(png==NULL)return;

 G2D_BLT.flag=G2D_BLT_NONE|G2D_BLT_PLANE_ALPHA;

 G2D_BLT.src_image.addr[0]=(u32)png->data;    //память, где хранится картинка
 G2D_BLT.src_image.addr[0]=(u32)png->data;
 G2D_BLT.src_image.addr[0]=(u32)png->data;
 G2D_BLT.src_image.w=png->width;              //габариты атласа
 G2D_BLT.src_image.h=png->height;
 G2D_BLT.src_image.format=G2D_FMT_ABGR_AVUY8888;
 G2D_BLT.src_image.pixel_seq=G2D_SEQ_NORMAL;

 G2D_BLT.src_rect.x=0;                        //смещение
 G2D_BLT.src_rect.y=0;

 G2D_BLT.src_rect.w=png->width;               //размер
 G2D_BLT.src_rect.h=png->height;

 G2D_BLT.dst_image.addr[0]=memory;
 G2D_BLT.dst_image.addr[0]=memory;
 G2D_BLT.dst_image.addr[0]=memory;
 G2D_BLT.dst_image.w=LCD_PIXEL_WIDTH;
 G2D_BLT.dst_image.h=LCD_PIXEL_HEIGHT;
 G2D_BLT.dst_image.format=G2D_FMT_ABGR_AVUY8888;
 G2D_BLT.dst_image.pixel_seq=G2D_SEQ_NORMAL;

 G2D_BLT.dst_x=0;                              //координаты вывода
 G2D_BLT.dst_y=0;

 G2D_BLT.color=0x00000000; //цветовой ключ RGB
 G2D_BLT.alpha=0xFF;       //альфа плоскости

 g2d_blit(&G2D_BLT);
}
//
//void PNG_Background_NEON(LuImage *png,u32 memory)
//{
// if(png)MEMCPY((u8*)memory,png->data,LCD_PIXEL_WIDTH*LCD_PIXEL_HEIGHT*4);
//}

void Rect(void)
{
 G2D_FILLRECT.flag=G2D_FIL_NONE; //G2D_FIL_PIXEL_ALPHA; //G2D_FIL_PLANE_ALPHA; //опеделяет смешение на плоскости

 //Параметры приёмной плоскости
 G2D_FILLRECT.dst_image.addr[0]=VIDEO_MEMORY1;
 G2D_FILLRECT.dst_image.addr[1]=VIDEO_MEMORY1;
 G2D_FILLRECT.dst_image.addr[2]=VIDEO_MEMORY1;
 G2D_FILLRECT.dst_image.w=LCD_PIXEL_WIDTH;
 G2D_FILLRECT.dst_image.h=LCD_PIXEL_HEIGHT;
 G2D_FILLRECT.dst_image.format=G2D_FMT_XBGR8888;
 G2D_FILLRECT.dst_image.pixel_seq=G2D_SEQ_NORMAL;

 G2D_FILLRECT.dst_rect.x=0;   //координаты прямоугольника
 G2D_FILLRECT.dst_rect.y=0;

 G2D_FILLRECT.dst_rect.w=DIM_X; //размеры прямоугольника
 G2D_FILLRECT.dst_rect.h=DIM_Y;

 G2D_FILLRECT.color=0x00ff0000; //0xFFFF0000; //цвет прямоугольника ARGB
 G2D_FILLRECT.alpha=0; //0xFF;       //альфа прямоугольника (с точностью наоборот 0 - прямоугольник глухой, 0xFF - прозрачный)

 g2d_fill(&G2D_FILLRECT);
}

float RAND(float min,float max) //random value [min..max]
{
 int MIN=(int)(min*1000.0F);
 int MAX=(int)(max*1000.0F);
 return ((float)((rand()%(MAX-MIN+1))+MIN))*0.001F;
}

void g2d_main(void)
{
// CLI();
//
// LowLevel_Init();
// PRINTF("\nT113-s3...\n");
//
// ClearVideoMemory();

 PNG_Load(&png[1],Back1_png);

 PNG_Load(&png[0],dog_png);

/*
 u32 *p=(u32*)png[0]->data;
 for(int y=0;y<png[0]->height;y++)
 for(int x=0;x<png[0]->width;x++)
 {
  if(*p==0xFFA449A3)*p&=0x00000000; // *p=0x010101;
  p++;
 }
*/


// flush_dcache();

 G2D_Init();

 Again:

 PNG_Background(png[1],VIDEO_MEMORY1);

#if 1
 G2D_BLT.flag=G2D_BLT_NONE|G2D_BLT_SRC_COLORKEY; // G2D_BLT_PIXEL_ALPHA; //обе прозрачности работают - через colorkey или alpha

 G2D_BLT.src_image.addr[0]=(u32)png[0]->data;    //память, где хранится картинка
 G2D_BLT.src_image.addr[0]=(u32)png[0]->data;
 G2D_BLT.src_image.addr[0]=(u32)png[0]->data;
 G2D_BLT.src_image.w=png[0]->width;              //габариты атласа
 G2D_BLT.src_image.h=png[0]->height;
 G2D_BLT.src_image.format=G2D_FMT_ABGR_AVUY8888;
 G2D_BLT.src_image.pixel_seq=G2D_SEQ_NORMAL;

 G2D_BLT.src_rect.x=0;                           //смещение
 G2D_BLT.src_rect.y=0;

 G2D_BLT.src_rect.w=png[0]->width;               //размер
 G2D_BLT.src_rect.h=png[0]->height;

 G2D_BLT.dst_image.addr[0]=VIDEO_MEMORY1;
 G2D_BLT.dst_image.addr[0]=VIDEO_MEMORY1;
 G2D_BLT.dst_image.addr[0]=VIDEO_MEMORY1;
 G2D_BLT.dst_image.w=LCD_PIXEL_WIDTH;
 G2D_BLT.dst_image.h=LCD_PIXEL_HEIGHT;
 G2D_BLT.dst_image.format=G2D_FMT_ABGR_AVUY8888;
 G2D_BLT.dst_image.pixel_seq=G2D_SEQ_NORMAL;

// G2D_BLT.dst_x=0;                                 //координаты вывода
// G2D_BLT.dst_y=0;

 G2D_BLT.color=*(u32*)png[0]->data; //0x00000000; //цветовой ключ RGB
 G2D_BLT.alpha=0xFF;       //альфа плоскости

 u32 t;
 //t=AVS_CNT0_REG;
 u32 d;

 for (t=0; t < 100; ++t)
 {
//  d=AVS_CNT0_REG-t;
//
//  if(d< 5*6000000)DelayMS(150);
//  else
//  if(d<10*6000000)VSync();
//  else
//  if(d<15*6000000);
//  else break;

  G2D_BLT.dst_x=rand()%(DIM_X-png[0]->width );
  G2D_BLT.dst_y=rand()%(DIM_Y-png[0]->height);

  g2d_blit(&G2D_BLT);

  local_delay_ms(50);

 }
#endif

#if 1

 G2D_STRETCHBLT.flag=G2D_BLT_NONE|G2D_BLT_PIXEL_ALPHA;

 G2D_STRETCHBLT.src_image.addr[0]=(u32)png[0]->data;    //память, где хранится картинка
 G2D_STRETCHBLT.src_image.addr[1]=(u32)png[0]->data;
 G2D_STRETCHBLT.src_image.addr[2]=(u32)png[0]->data;

 G2D_STRETCHBLT.src_image.w=png[0]->width;              //габариты атласа
 G2D_STRETCHBLT.src_image.h=png[0]->height;

 G2D_STRETCHBLT.src_image.format=G2D_FMT_ABGR_AVUY8888;
 G2D_STRETCHBLT.src_image.pixel_seq=G2D_SEQ_NORMAL;

 G2D_STRETCHBLT.src_rect.x=0;                           //смещение
 G2D_STRETCHBLT.src_rect.y=0;

 G2D_STRETCHBLT.src_rect.w=png[0]->width;               //размер
 G2D_STRETCHBLT.src_rect.h=png[0]->height;

 G2D_STRETCHBLT.dst_image.addr[0]=VIDEO_MEMORY1;        //выводить на плоскость 1 (UI), так как плоскость 0 (VI) не имеет пиксельной альфы (будет шов на краях)
 G2D_STRETCHBLT.dst_image.addr[1]=VIDEO_MEMORY1;
 G2D_STRETCHBLT.dst_image.addr[2]=VIDEO_MEMORY1;

 G2D_STRETCHBLT.dst_image.w=LCD_PIXEL_WIDTH;
 G2D_STRETCHBLT.dst_image.h=LCD_PIXEL_HEIGHT;

 G2D_STRETCHBLT.dst_image.format=G2D_FMT_ABGR_AVUY8888;
 G2D_STRETCHBLT.dst_image.pixel_seq=G2D_SEQ_NORMAL;

 G2D_STRETCHBLT.dst_rect.x=0;                           //коодинаты вывода
 G2D_STRETCHBLT.dst_rect.y=0;

 G2D_STRETCHBLT.dst_rect.w=png[0]->width/2;             //размер
 G2D_STRETCHBLT.dst_rect.h=png[0]->height/2;

 G2D_STRETCHBLT.color=*(u32*)png[0]->data; //0x00000000; //цветовой ключ RGB
 G2D_STRETCHBLT.alpha=0xFF;       //альфа плоскости

 PorterDuff=G2D_BLD_SRCOVER;

// g2d_stretchblit(&G2D_STRETCHBLT);

 float scale;

 float min_scale=0.2F;
 float max_scale=min((float)LCD_PIXEL_WIDTH/(float)png[0]->width,(float)LCD_PIXEL_HEIGHT/(float)png[0]->height);

 //t=AVS_CNT0_REG;

 for (t=0; t < 100; ++t)
 {
//  d=AVS_CNT0_REG-t;
//
//  if(d< 5*6000000)DelayMS(150);
//  else
//  if(d<10*6000000)VSync();
//  else
//  if(d<15*6000000);
//  else goto Again;

 scale=RAND(min_scale,max_scale);

 int w=png[0]->width*scale;
 int h=png[0]->height*scale;

 G2D_STRETCHBLT.dst_rect.x=rand()%(DIM_X-w);        //координаты вывода
 G2D_STRETCHBLT.dst_rect.y=rand()%(DIM_Y-h);

 G2D_STRETCHBLT.dst_rect.w=w;                     //размер
 G2D_STRETCHBLT.dst_rect.h=h;

// VSync();

 g2d_stretchblit(&G2D_STRETCHBLT);

 local_delay_ms(50);


// DelayMS(300);

 }

#endif


#if 0


 ClearVideoMemory();

 PNG_Load(&png[0],Back0_png);
 PNG_Load(&png[1],Back1_png);

// PNG_Load(&png[1],Cobra_png);
 flush_dcache();

 G2D_Init();

 u8 p=0;

 u32 t;

 for (t=0; t < 100; ++t)
 {
  //t=AVS_CNT0_REG;

  for(int i=0;i<100;i++)
  {
   PNG_Background /*_NEON*/ (png[p],VIDEO_MEMORY1 +0x400000  );
   p^=1;
  }

  t=AVS_CNT0_REG-t;

  UART0_putn(t);
 }

 G2D_BLT.flag=G2D_BLT_NONE|G2D_BLT_SRC_PREMULTIPLY;

 G2D_BLT.src_image.addr[0]=(u32)png[1]->data;    //память, где хранится картинка
 G2D_BLT.src_image.addr[0]=(u32)png[1]->data;
 G2D_BLT.src_image.addr[0]=(u32)png[1]->data;
 G2D_BLT.src_image.w=png[1]->width;              //габариты атласа
 G2D_BLT.src_image.h=png[1]->height;
 G2D_BLT.src_image.format=G2D_FMT_ABGR_AVUY8888;
 G2D_BLT.src_image.pixel_seq=G2D_SEQ_NORMAL;

 G2D_BLT.src_rect.x=0;                           //смещение
 G2D_BLT.src_rect.y=0;
 G2D_BLT.src_rect.w=png[1]->width;               //размер
 G2D_BLT.src_rect.h=png[1]->height;

 G2D_BLT.dst_image.addr[0]=VIDEO_MEMORY1;
 G2D_BLT.dst_image.addr[0]=VIDEO_MEMORY1;
 G2D_BLT.dst_image.addr[0]=VIDEO_MEMORY1;
 G2D_BLT.dst_image.w=LCD_PIXEL_WIDTH;
 G2D_BLT.dst_image.h=LCD_PIXEL_HEIGHT;
 G2D_BLT.dst_image.format=G2D_FMT_ABGR_AVUY8888;
 G2D_BLT.dst_image.pixel_seq=G2D_SEQ_NORMAL;

// G2D_BLT.dst_x=0;                                 //координаты вывода
// G2D_BLT.dst_y=0;

 G2D_BLT.color=0x00000000; //цветовой ключ RGB
 G2D_BLT.alpha=0xFF;       //альфа плоскости



 //u32 t=0;

 Loop:

 G2D_BLT.dst_x=32+(rand()%(DIM_X-png[1]->width -64));
 G2D_BLT.dst_y=32+(rand()%(DIM_Y-png[1]->height-64));

 g2d_blit(&G2D_BLT);

 local_delay_ms(50);


 if(t<10)DelayMS(1000); //mdelay(100);

 t++;

 goto Loop;
#endif

 ASSERT(0);
}
#endif
