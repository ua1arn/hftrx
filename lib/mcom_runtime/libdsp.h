#ifndef __LIBDSP_H__
#define __LIBDSP_H__

#ifndef LIB_ELCORE_SDK
#include <bits/types.h>

// Структура, собирающая в себе данные о
// каждой области адресов
typedef struct _dsp_mem_area {
    char name[10]; // Текстовое наименование области
    unsigned size; // Размер
    void *addr; // Адрес в виртуальном пространстве
                // прикладной программы (если
                // область была смаппирована)
    unsigned phys_number; // Физический номер области в
                          // драйвере UIO (для
                          // пользователя не
                          // представляет интереса - для
                          // внутренних нужд библиотеки)
} dsp_mem_area;

//
// �?нициализация библиотеки доступа к DSP
//
int dsp_access_init();

//
// Выгрузка библиотеки доступа к DSP
//
void dsp_access_close();

//
// Получение доступа к массиву
// структур-описателей областей для целей
// просмотра пользователем (если вдруг это
// для чего-нибудь потребуется).
// Пользователь не должен менять содержимое
// данного массива, иначе возможна
// некорректная работа библиотеки.
//
const dsp_mem_area *dsp_mem_areas();

#else

// <TODO> check at mips, do common code base
#if _MIPS_
typedef unsigned short __uint16_t;
typedef unsigned int __uint32_t;
typedef unsigned long long __uint64_t;
#else // _MIPS_
#include <stdlib.h>
#endif // _MIPS_

// mcom dsp memory
#define _PRAM0 ((unsigned int volatile *)0x3A600000)
#define _PRAM1 ((unsigned int volatile *)0x3A620000)
#define _XYRAM0 ((unsigned int volatile *)0x3A400000)
#define _XYRAM1 ((unsigned int volatile *)0x3A420000)
#define _XBUF ((unsigned int volatile *)0x37001100)
#define _REGS0 ((unsigned char volatile *)0x37000000)
#define _REGS1 ((unsigned char volatile *)0x37010000)
#define _REGSCM ((unsigned char volatile *)0x37001000)

#define _AGU0 ((AGU volatile *)(_REGS0 + 0x80))
#define _AGU1 ((AGU volatile *)(_REGS1 + 0x80))

#endif

// Перечень доступных областей адресов,
// используемых DSP
enum {
    AREA_DSP0_REGS,   // Регистры DSP0
    AREA_DSP1_REGS,   // Регистры DSP1
    AREA_DSP0_PRAM,   // Память программ DSP0 (PRAM0)
    AREA_DSP1_PRAM,   // Память программ DSP1 (PRAM1)
    AREA_XYRAM0,      // Память данных XYRAM0 (ближе к DSP0)
    AREA_XYRAM1,      // Память данных XYRAM1 (ближе к DSP1)
    AREA_COMMON_REGS, // Общие регистры для DSP0 и DSP1
    AREA_XBUF,        // Синхронная память XBUF
    NB_DSP_AREAS
};

//
// Подключение (маппирование) области
// адресов. Параметр area_number - номер области из
// перечня (AREA_DSP0_REGS, AREA_DSP1_REGS и т.д.). Функция
// возвращает указатель на начало области в
// виртуальном пространстве адресов
// процесса.
//
void *dsp_map_area(int area_number);

//
// Отключение области адресов, указанной в
// параметре area_number.
//
int dsp_unmap_area(int area_number);
//
// Структура регистрового файла одного ядра
// DSP.
//
typedef struct _dsp_regs {
    // Регистры данных (первая секция)
    volatile __uint32_t R0L;
    volatile __uint32_t R2L;
    volatile __uint32_t R4L;
    volatile __uint32_t R6L;
    volatile __uint32_t R8L;
    volatile __uint32_t R10L;
    volatile __uint32_t R12L;
    volatile __uint32_t R14L;
    volatile __uint32_t R16L;
    volatile __uint32_t R18L;
    volatile __uint32_t R20L;
    volatile __uint32_t R22L;
    volatile __uint32_t R24L;
    volatile __uint32_t R26L;
    volatile __uint32_t R28L;
    volatile __uint32_t R30L;
    volatile __uint32_t R1L;
    volatile __uint32_t R3L;
    volatile __uint32_t R5L;
    volatile __uint32_t R7L;
    volatile __uint32_t R9L;
    volatile __uint32_t R11L;
    volatile __uint32_t R13L;
    volatile __uint32_t R15L;
    volatile __uint32_t R17L;
    volatile __uint32_t R19L;
    volatile __uint32_t R21L;
    volatile __uint32_t R23L;
    volatile __uint32_t R25L;
    volatile __uint32_t R27L;
    volatile __uint32_t R29L;
    volatile __uint32_t R31L;
    // AGU, AGU-Y
    volatile __uint32_t A0;
    volatile __uint32_t A1;
    volatile __uint32_t A2;
    volatile __uint32_t A3;
    volatile __uint32_t A4;
    volatile __uint32_t A5;
    volatile __uint32_t A6;
    volatile __uint32_t A7;
    volatile __uint32_t I0;
    volatile __uint32_t I1;
    volatile __uint32_t I2;
    volatile __uint32_t I3;
    volatile __uint32_t I4;
    volatile __uint32_t I5;
    volatile __uint32_t I6;
    volatile __uint32_t I7;
    volatile __uint32_t M0;
    volatile __uint32_t M1;
    volatile __uint32_t M2;
    volatile __uint32_t M3;
    volatile __uint32_t M4;
    volatile __uint32_t M5;
    volatile __uint32_t M6;
    volatile __uint32_t M7;
    volatile __uint32_t AT;
    volatile __uint32_t
        IT; // Elcore30M architecture allows 32-bit external access to registers
            //  volatile __uint16_t   gap0_1;
    volatile __uint32_t MT;
    //  volatile __uint16_t   gap0_2;
    volatile __uint32_t DT;
    //  volatile __uint16_t   gap0[7];
    volatile __uint16_t gap0[6];
    volatile __uint32_t IVAR;
    //    volatile __uint16_t   gap0_3;
    // PCU
    volatile __uint32_t DCSR;
    //    volatile __uint16_t   gap1;
    volatile __uint32_t SR;
    //  volatile __uint16_t   gap2;
    volatile __uint32_t IDR;
    //  volatile __uint16_t   gap3;
    volatile __uint32_t EFR_DSTART;
    volatile __uint32_t IRQR;
    volatile __uint32_t IMASKR;
    volatile __uint32_t TMR;
    volatile __uint32_t ARBR;
    //  volatile __uint16_t   gap4;
    volatile __uint32_t PC;
    //  volatile __uint16_t   gap5;
    volatile __uint32_t SS;
    //    volatile __uint16_t   gap6;
    volatile __uint32_t LA;
    //  volatile __uint16_t   gap7;
    volatile __uint32_t CSL;
    //  volatile __uint16_t   gap8;
    volatile __uint32_t LC;
    //  volatile __uint16_t   gap9;
    volatile __uint32_t CSH;
    //  volatile __uint16_t   gap10;
    volatile __uint32_t SP;
    //    volatile __uint16_t   gap11;
    volatile __uint32_t SAR;
    //  volatile __uint16_t   gap12;
    volatile __uint32_t CNTR;
    //  volatile __uint16_t   gap13;
    volatile __uint32_t SAR1;
    //  volatile __uint16_t   gap14;
    volatile __uint32_t SAR2;
    //  volatile __uint16_t   gap15;
    volatile __uint32_t SAR3;
    //  volatile __uint16_t   gap16;
    volatile __uint32_t SAR4;
    //  volatile __uint16_t   gap17;
    volatile __uint32_t SAR5;
    //  volatile __uint16_t   gap18;
    volatile __uint32_t SAR6;
    //  volatile __uint16_t   gap19;
    volatile __uint32_t SAR7;
    //  volatile __uint16_t   gap20;
    // Регистры состояния ALU
    volatile __uint32_t CCR;
    //  volatile __uint16_t   gap21;
    volatile __uint32_t PNDR;
    //  volatile __uint16_t   gap22;
    volatile __uint32_t SFR;
    volatile __uint32_t gap22_;
    volatile __uint32_t QMASR0;
    volatile __uint32_t QMASR1;
    volatile __uint32_t QMASR2;
    volatile __uint32_t gap23;
    // Регистры данных (вторая секция)
    volatile __uint64_t R1D;
    volatile __uint64_t R3D;
    volatile __uint64_t R5D;
    volatile __uint64_t R7D;
    volatile __uint64_t R9D;
    volatile __uint64_t R11D;
    volatile __uint64_t R13D;
    volatile __uint64_t R15D;
    volatile __uint64_t R17D;
    volatile __uint64_t R19D;
    volatile __uint64_t R21D;
    volatile __uint64_t R23D;
    volatile __uint64_t R25D;
    volatile __uint64_t R27D;
    volatile __uint64_t R29D;
    volatile __uint64_t R31D;
    // Регистры-аккумуляторы
    volatile __uint32_t AC0;
    volatile __uint32_t AC1;
    volatile __uint32_t AC2;
    volatile __uint32_t AC3;
    volatile __uint32_t AC4;
    volatile __uint32_t AC5;
    volatile __uint32_t AC6;
    volatile __uint32_t AC7;
    volatile __uint32_t AC8;
    volatile __uint32_t AC9;
    volatile __uint32_t AC10;
    volatile __uint32_t AC11;
    volatile __uint32_t AC12;
    volatile __uint32_t AC13;
    volatile __uint32_t AC14;
    volatile __uint32_t AC15;

    volatile __uint32_t gap24[176];

    // Отладочные регистры
    volatile __uint32_t dbDCSR;
    //    volatile __uint16_t   gap25[11];
    volatile __uint16_t gap25[10];
    volatile __uint32_t Cnt_RUN;
    volatile __uint32_t gap26;
    volatile __uint32_t dbPCe;
    //  volatile __uint16_t   gap27;
    volatile __uint32_t dbPCa;
    //  volatile __uint16_t   gap28;
    volatile __uint32_t dbPCf;
    //  volatile __uint16_t   gap29;
    volatile __uint32_t dbPCd;
    //  volatile __uint16_t   gap30;
    volatile __uint32_t dbPCe1;
    //  volatile __uint16_t   gap31;
    volatile __uint32_t dbPCe2;
    //  volatile __uint16_t   gap32;
    volatile __uint32_t dbPCe3;
    //  volatile __uint16_t   gap33;
    volatile __uint32_t dbSAR;
    //  volatile __uint16_t   gap34;
    volatile __uint32_t dbCNTR;
    //  volatile __uint16_t   gap35;
    volatile __uint32_t dbSAR1;
    //  volatile __uint16_t   gap36;
    volatile __uint32_t dbSAR2;
    //  volatile __uint16_t   gap37;
    volatile __uint32_t dbSAR3;
    //  volatile __uint16_t   gap38;
    volatile __uint32_t dbSAR4;
    //  volatile __uint16_t   gap39;
    volatile __uint32_t dbSAR5;
    //  volatile __uint16_t   gap40;
    volatile __uint32_t dbSAR6;
    //  volatile __uint16_t   gap41;
    volatile __uint32_t dbSAR7;
    //  volatile __uint16_t   gap42;

} __attribute((packed)) dsp_regs;

//
// Макроопределения для вычисления пложения
// регистра в структуре dsp_regs Перечислены
// регистры данных и адресные регистры.
//

#define OFF_NONE (-1)
#define OFF_R0L offsetof(dsp_regs, R0L)
#define OFF_R2L offsetof(dsp_regs, R2L)
#define OFF_R4L offsetof(dsp_regs, R4L)
#define OFF_R6L offsetof(dsp_regs, R6L)
#define OFF_R8L offsetof(dsp_regs, R8L)
#define OFF_R10L offsetof(dsp_regs, R10L)
#define OFF_R12L offsetof(dsp_regs, R12L)
#define OFF_R14L offsetof(dsp_regs, R14L)
#define OFF_R16L offsetof(dsp_regs, R16L)
#define OFF_R18L offsetof(dsp_regs, R18L)
#define OFF_R20L offsetof(dsp_regs, R20L)
#define OFF_R22L offsetof(dsp_regs, R22L)
#define OFF_R24L offsetof(dsp_regs, R24L)
#define OFF_R26L offsetof(dsp_regs, R26L)
#define OFF_R28L offsetof(dsp_regs, R28L)
#define OFF_R30L offsetof(dsp_regs, R30L)
#define OFF_R1L offsetof(dsp_regs, R1L)
#define OFF_R3L offsetof(dsp_regs, R3L)
#define OFF_R5L offsetof(dsp_regs, R5L)
#define OFF_R7L offsetof(dsp_regs, R7L)
#define OFF_R9L offsetof(dsp_regs, R9L)
#define OFF_R11L offsetof(dsp_regs, R11L)
#define OFF_R13L offsetof(dsp_regs, R13L)
#define OFF_R15L offsetof(dsp_regs, R15L)
#define OFF_R17L offsetof(dsp_regs, R17L)
#define OFF_R19L offsetof(dsp_regs, R19L)
#define OFF_R21L offsetof(dsp_regs, R21L)
#define OFF_R23L offsetof(dsp_regs, R23L)
#define OFF_R25L offsetof(dsp_regs, R25L)
#define OFF_R27L offsetof(dsp_regs, R27L)
#define OFF_R29L offsetof(dsp_regs, R29L)
#define OFF_R31L offsetof(dsp_regs, R31L)

#define OFF_A0 offsetof(dsp_regs, A0)
#define OFF_A1 offsetof(dsp_regs, A1)
#define OFF_A2 offsetof(dsp_regs, A2)
#define OFF_A3 offsetof(dsp_regs, A3)
#define OFF_A4 offsetof(dsp_regs, A4)
#define OFF_A5 offsetof(dsp_regs, A5)
#define OFF_A6 offsetof(dsp_regs, A6)
#define OFF_A7 offsetof(dsp_regs, A7)
#define OFF_I0 offsetof(dsp_regs, I0)
#define OFF_I1 offsetof(dsp_regs, I1)
#define OFF_I2 offsetof(dsp_regs, I2)
#define OFF_I3 offsetof(dsp_regs, I3)
#define OFF_I4 offsetof(dsp_regs, I4)
#define OFF_I5 offsetof(dsp_regs, I5)
#define OFF_I6 offsetof(dsp_regs, I6)
#define OFF_I7 offsetof(dsp_regs, I7)
#define OFF_DCSR offsetof(dsp_regs, DCSR)
#define OFF_PC offsetof(dsp_regs, PC)
#define OFF_SS offsetof(dsp_regs, SS)
#define OFF_SP offsetof(dsp_regs, SP)

//
// Макроопределения для доступа к битам
// регистров
//

// DCSR
#define DSP_PI (1 << 0)  // программное прерывание
#define DSP_SE (1 << 1)  // прерывание по ошибке стека SE
#define DSP_BRK (1 << 2) // прерывание по останову BREAK
#define DSP_STP (1 << 3) // прерывание по останову STOP
#define DSP_WT                                                                 \
    (1 << 4) // состояние ожидания обмена с XBUF
#define DSP_RUN                                                                \
    (1 << 14) // состояние исполнения программы

// SR
#define DSP_C (1 << 0) // перенос
#define DSP_V (1 << 1) // признак переполнения
#define DSP_Z (1 << 2) // признак нулевого результата
#define DSP_N                                                                  \
    (1 << 3) // признак отрицательного результата
#define DSP_U                                                                  \
    (1 << 4) // признак ненормализованного
             // результата
#define DSP_EV                                                                 \
    (1 << 5) // флаг переполнения (с сохранением)
#define DSP_E (1 << 6) // экспоненциальный признак
#define DSP_T                                                                  \
    (1 << 7) // признак истинности последнего
             // условия
#define DSP_DD                                                                 \
    (1 << 9) // управление режимом записи
             // результата в
             // инструкциях ADDSUB, ADDSUBL, ADDSUBX, FAS,
             // CVFE (Double Destination)
#define DSP_BD                                                                 \
    (1 << 10) // управление блокировкой конвейера
#define DSP_YM (1 << 11) // режим адресации памяти YRAM
#define DSP_SPLIT_MODE(n)                                                      \
    ((n) << 14) // Управление режимом разбиения
                // пересылок

// LC
#define DSP_LF                                                                 \
    (1 << 14) // флаг цикла DO – разряд 14 регистра LC
#define DSP_FV                                                                 \
    (1 << 15) // флаг цикла DOFOR – разряд 15 регистра
              // LC

// SP
#define DSP_SP(n) (n) // указатель системного стека
#define DSP_SSE                                                                \
    (1 << 4) // флаг ошибки системного стека
#define DSP_UFS                                                                \
    (1 << 5)                 // флаг переполнения системного
                             // стека
#define DSP_CP(n) ((n) << 8) // указатель стека циклов
#define DSP_CSE (1 << 11)    // флаг ошибки стека циклов
#define DSP_UFC                                                                \
    (1 << 12) // флаг переполнения стека циклов

// IRQR
#define DSP_DRQ0                                                               \
    (1 << 0) // Запрос на прерывание DSP со стороны
             // канала DMA MemCh0
#define DSP_DRQ1                                                               \
    (1 << 1) // Запрос на прерывание DSP со стороны
             // канала DMA MemCh1
#define DSP_DRQ2                                                               \
    (1 << 2) // Запрос на прерывание DSP со стороны
             // канала DMA MemCh2
#define DSP_DRQ3                                                               \
    (1 << 3) // Запрос на прерывание DSP со стороны
             // канала DMA MemCh3
#define DSP_IRQ0                                                               \
    (1 << 24) // Запрос на прерывание DSP со стороны
              // DSP0
#define DSP_IRQ1                                                               \
    (1 << 25) // Запрос на прерывание DSP со стороны
              // DSP1
#define DSP_ITMR                                                               \
    (1 << 28) // Запрос на прерывание DSP со стороны
              // таймера TMR
#define DSP_FPE                                                                \
    (1 << 29) // �?сключение при исполении операции
              // в формате плавающей точки (V=1)
#define DSP_QT0                                                                \
    (1 << 30) // Запрос на прерывание DSP со стороны
              // CPU (QSTR0)
#define DSP_QT1                                                                \
    (1 << 31) // Запрос на прерывание DSP со стороны
              // CPU (QSTR1, QSTR2)

// DSTART
#define DSP_DE0                                                                \
    (1 << 0) // Запрос со стороны DSP на запуск
             // канала DMA MemCh0
#define DSP_DE1                                                                \
    (1 << 1) // Запрос со стороны DSP на запуск
             // канала DMA MemCh1
#define DSP_DE2                                                                \
    (1 << 2) // Запрос со стороны DSP на запуск
             // канала DMA MemCh2
#define DSP_DE3                                                                \
    (1 << 3)               // Запрос со стороны DSP на запуск
                           // канала DMA MemCh3
#define DSP_DSP0 (1 << 24) // Запрос на прерывание DSP0
#define DSP_DSP1 (1 << 25) // Запрос на прерывание DSP1

// ARBR
#define DSP_HEN                                                                \
    (1 << 0) // Включение режима определения
             // высокой плотности потоков
#define DSP_DEN                                                                \
    (1 << 1) // разрешение установки явного
             // приоритета (статический режим)
#define DSP_LEN (1 << 2) // бит разрешения ограничителя
#define DSP_DPTR(n)                                                            \
    ((n) << 4) // номер ядра, обладающего наивысшим
               // приоритетом
#define DSP_LIMIT(n)                                                           \
    ((n) << 8) // максимальное значение счетчика
               // обращений

// dbDCSR
#define DSP_dbBRK                                                              \
    (1 << 2) // флаг останов исполнения программы
             // в режиме отладки
#define DSP_dbRUN                                                              \
    (1 << 14) // состояние исполнения программы в
              // режиме отладки

//
// Структура общих регистров для всех ядер
// DSP.
//
typedef struct _common_regs {
    volatile __uint32_t MASKR;
    volatile __uint32_t QSTR;
    volatile __uint32_t CSR;
    volatile __uint32_t TOTAL_CLK_CNTR;
} __attribute((packed)) common_regs;

typedef struct _dsp_xbuf_regs {
    volatile __uint64_t X0_D;
    volatile __uint64_t X1_D;
    volatile __uint64_t X2_D;
    volatile __uint64_t X3_D;
    volatile __uint64_t X4_D;
    volatile __uint64_t X5_D;
    volatile __uint64_t X6_D;
    volatile __uint64_t X7_D;
    volatile __uint64_t X8_D;
    volatile __uint64_t X9_D;
    volatile __uint64_t X10_D;
    volatile __uint64_t X11_D;
    volatile __uint64_t X12_D;
    volatile __uint64_t X13_D;
    volatile __uint64_t X14_D;
    volatile __uint64_t X15_D;
    volatile __uint64_t X16_D;
    volatile __uint64_t X17_D;
    volatile __uint64_t X18_D;
    volatile __uint64_t X19_D;
    volatile __uint64_t X20_D;
    volatile __uint64_t X21_D;
    volatile __uint64_t X22_D;
    volatile __uint64_t X23_D;
    volatile __uint64_t X24_D;
    volatile __uint64_t X25_D;
    volatile __uint64_t X26_D;
    volatile __uint64_t X27_D;
    volatile __uint64_t X28_D;
    volatile __uint64_t X29_D;
    volatile __uint64_t X30_D;
    volatile __uint64_t X31_D;
} __attribute((packed)) dsp_xbuf_regs;

#endif
