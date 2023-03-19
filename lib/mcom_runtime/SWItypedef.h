/********************************************************************
* Описание модуля:                                                  *
*     Обеспечение взаимодействия задач сигнальной обработки         *
*-------------------------------------------------------------------*
* Файл содержит объявления общих типов данных и макросы, общие для  *
* всего пакета программ                                             *
********************************************************************/
//#include <tistdtypes.h>

#ifndef SWITYPEDEF
#define SWITYPEDEF

#ifndef NULL
#ifdef __GNUG__
#define NULL __null
#else   /* G++ */
#ifndef __cplusplus
#define NULL ((void *)0)
#else   /* C++ */
#define NULL 0
#endif  /* C++ */
#endif  /* G++ */
#endif	/* NULL not defined and <stddef.h> or need NULL.  */


/*================ ОБЩИЕ ТИПЫ ======================================*/
typedef unsigned char 	   UCHAR;            /* диапазон  0 ... 2^16-1   */
typedef char         	   CHAR;                      /* диапазон -2^15 ... 2^15-1*/
typedef short       	   SINT16;                     /* диапазон -2^15 ... 2^15-1*/
typedef unsigned short     UINT16;                /* диапазон  0  ... 2^16-1  */
typedef unsigned short     BIN16;             /* битовые переменные       */
typedef int           	   SINT32;                /* диапазон -2^31 ... 2^31-1*/
typedef unsigned int  	   UINT32;        /* диапазон  0 ... 2^32-1   */
typedef unsigned long	   U32;        /* диапазон  0 ... 2^32-1   */
typedef float        	   REAL32;               /* диапазон 1,19e-38...-3,4е+38 */
//typedef bool        	   bit;               /* диапазон 1,19e-38...-3,4е+38 */

//typedef unsigned char BOOLEAN;          /* булевская FALSE, TRUE    */
#ifndef __cplusplus
typedef int 				bool;          /* булевская FALSE, TRUE    */
#endif
//typedef unsigned long int UINT32;       /* диапазон  0 ... 2^32-1   */

typedef unsigned char      UCHAR_8;
typedef char               CHAR_8;
#ifdef _MSC_VER
typedef int                INT32;
#else
typedef int                INT32;
#endif
typedef float              FLT_32;
typedef unsigned long long UINT40;

typedef unsigned char	   BYTE;
typedef UINT16             WORD;
typedef UINT32             DWORD;
typedef unsigned long      UINT;

typedef struct
  {
   SINT32 integer;
   UINT16 fract;
   UINT16 AddressBlank;
  } SINT48;

typedef struct
  {
   UINT32 integer;
   UINT16 fract;
   UINT16 AddressBlank;
  } UINT48;

#define ClrInt48(x)   x.fract = 0; x.integer = 0
typedef enum
  {
  INVALID,
  VALID
  } VALUE;
/*================ ОБЩИЕ МАКРОСЫ ===================================*/

#define ON              1
#define OFF             0
#define YES             1
#define NO              0
//#define NULL            0

#define true            (1 == 1)
#define false           (1 == 0)
/*------- Сдвиг 1 на x разрядов влево в 16-разрядном слове ---------*/
#define BIT_16(x)          (1u << (x))

/*------- Сдвиг 1 на x разрядов влево в 32-разрядном слове ---------*/
#define BIT_32(x)         ((UINT32)1 << (x))

/*------ Проверка  бита х на равенство 1 в 16-разрядном слове w ----*/
#define IS_BIT_SET(w,x) ((w & BIT(x)) != 0)

/*------ Проверка  бита х на равенство 0 в 16-разрядном слове w ----*/
#define IS_BIT_CLR(w,x) ((w & BIT(x)) == 0)

/*------ Проверка  бита х на равенство 1 в 32-разрядном слове w ----*/
#define IS_BIT_SET2(w,x) ((w & BIT2(x)) != 0)

/*------- Установка  бита х в 16-разрядном слове -------------------*/
#define SET_BIT_16(w,x)     ((w) |= BIT(x))

/*------- Установка  бита х в 32-разрядном слове -------------------*/
#define SET_BIT_32(w,x)     ((w) |= BIT2(x))

/*---------- Очистка бита х в 16-разрядном слове -------------------*/
#define CLR_BIT_16(w,x)      ((w) &= ~BIT(x))

/*---------- Очистка бита х в 32-разрядном слове -------------------*/
#define CLR_BIT2_32(w,x)      ((w) &= ~BIT2(x))

// used with UU16
# define LSB 0
# define MSB 1

typedef union UU16
{
   UINT16 U16;
   SINT16 S16;
   UCHAR U8[2];
   CHAR  S8[2];
} UU16;

#endif
