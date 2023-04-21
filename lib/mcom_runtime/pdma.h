#ifndef        _PDMA_H_
#define        _PDMA_H_

#define PDMA_MAX_BLOCK_SIZE 4095

#define         PDMA_MCC_CA          15
#define         PDMA_MCC             14
#define         PDMA_SSI1_RX         13
#define         PDMA_SSI1_TX         12
#define         PDMA_SSI0_RX         11
#define         PDMA_SSI0_TX         10
#define         PDMA_I2C1_RX         9 
#define         PDMA_I2C1_TX         8 
#define         PDMA_I2C0_RX         7 
#define         PDMA_I2C0_TX         6 
#define         PDMA_UART2_RX        5 
#define         PDMA_UART2_TX        4 
#define         PDMA_UART1_RX        3 
#define         PDMA_UART1_TX        2 
#define         PDMA_UART0_RX        1 
#define         PDMA_UART0_TX        0 


#define        PDMA_BASE        0x38000000
#define        PDMA_CHAN0        0x0000
#define        PDMA_CHAN1        0x0058
#define        PDMA_CHAN2        0x00B0
#define        PDMA_CHAN3        0x0108
#define        PDMA_CHAN4        0x0160
#define        PDMA_CHAN5        0x01B8
#define        PDMA_CHAN6        0x0210
#define        PDMA_CHAN7        0x0268



#define        PDMA_SAR0                0x0000
#define        PDMA_DAR0                (PDMA_SAR0+8)                ///0x0008
#define        PDMA_LLP0                (PDMA_SAR0+16)                ///0x0010
#define        PDMA_CTL0                (PDMA_SAR0+24)                ///0x0018
#define        PDMA_SSTAT0              (PDMA_SAR0+32)                ///0x0020
#define        PDMA_DSTAT0              (PDMA_SAR0+40)                ///0x0028
#define        PDMA_SSTATAR0        	(PDMA_SAR0+48)                ///0x0030
#define        PDMA_DSTATAR0        	(PDMA_SAR0+56)                ///0x0038
#define        PDMA_CFG0                (PDMA_SAR0+64)                ///0x0040
#define        PDMA_SGR0                (PDMA_SAR0+72)                ///0x0048
#define        PDMA_DSR0                (PDMA_SAR0+80)                ///0x0050


#define        PDMA_CHN        			PDMA_CHAN1

#define        PDMA_SAR1                (PDMA_CHN)
#define        PDMA_DAR1                (PDMA_CHN+8)
#define        PDMA_LLP1                (PDMA_CHN+16)
#define        PDMA_CTL1                (PDMA_CHN+24)
#define        PDMA_SSTAT1              (PDMA_CHN+32)
#define        PDMA_DSTAT1              (PDMA_CHN+40)
#define        PDMA_SSTATAR1        	(PDMA_CHN+48)
#define        PDMA_DSTATAR1        	(PDMA_CHN+56)
#define        PDMA_CFG1                (PDMA_CHN+64)
#define        PDMA_SGR1                (PDMA_CHN+72)
#define        PDMA_DSR1                (PDMA_CHN+80)

#undef        PDMA_CHN


#define        PDMA_CHN        			PDMA_CHAN2

#define        PDMA_SAR2                (PDMA_CHN)
#define        PDMA_DAR2                (PDMA_CHN+8)
#define        PDMA_LLP2                (PDMA_CHN+16)
#define        PDMA_CTL2                (PDMA_CHN+24)
#define        PDMA_SSTAT2              (PDMA_CHN+32)
#define        PDMA_DSTAT2              (PDMA_CHN+40)
#define        PDMA_SSTATAR2        	(PDMA_CHN+48)
#define        PDMA_DSTATAR2        	(PDMA_CHN+56)
#define        PDMA_CFG2                (PDMA_CHN+64)
#define        PDMA_SGR2                (PDMA_CHN+72)
#define        PDMA_DSR2                (PDMA_CHN+80)

#undef        PDMA_CHN

#define	PDMA_CHN	PDMA_CHAN3

#define	PDMA_SAR3		(PDMA_CHN)
#define	PDMA_DAR3		(PDMA_CHN+8)
#define	PDMA_LLP3		(PDMA_CHN+16)
#define	PDMA_CTL3		(PDMA_CHN+24)
#define	PDMA_SSTAT3		(PDMA_CHN+32)
#define	PDMA_DSTAT3		(PDMA_CHN+40)
#define	PDMA_SSTATAR3	(PDMA_CHN+48)
#define	PDMA_DSTATAR3	(PDMA_CHN+56)
#define	PDMA_CFG3		(PDMA_CHN+64)
#define	PDMA_SGR3		(PDMA_CHN+72)
#define	PDMA_DSR3		(PDMA_CHN+80)

#undef	PDMA_CHN

#define	PDMA_CHN	PDMA_CHAN4

#define	PDMA_SAR4		(PDMA_CHN)
#define	PDMA_DAR4		(PDMA_CHN+8)
#define	PDMA_LLP4		(PDMA_CHN+16)
#define	PDMA_CTL4		(PDMA_CHN+24)
#define	PDMA_SSTAT4		(PDMA_CHN+32)
#define	PDMA_DSTAT4		(PDMA_CHN+40)
#define	PDMA_SSTATAR4	(PDMA_CHN+48)
#define	PDMA_DSTATAR4	(PDMA_CHN+56)
#define	PDMA_CFG4		(PDMA_CHN+64)
#define	PDMA_SGR4		(PDMA_CHN+72)
#define	PDMA_DSR4		(PDMA_CHN+80)

#undef	PDMA_CHN


#define	PDMA_CHN	PDMA_CHAN5

#define	PDMA_SAR5		(PDMA_CHN)
#define	PDMA_DAR5		(PDMA_CHN+8)
#define	PDMA_LLP5		(PDMA_CHN+16)
#define	PDMA_CTL5		(PDMA_CHN+24)
#define	PDMA_SSTAT5		(PDMA_CHN+32)
#define	PDMA_DSTAT5		(PDMA_CHN+40)
#define	PDMA_SSTATAR5	(PDMA_CHN+48)
#define	PDMA_DSTATAR5	(PDMA_CHN+56)
#define	PDMA_CFG5		(PDMA_CHN+64)
#define	PDMA_SGR5		(PDMA_CHN+72)
#define	PDMA_DSR5		(PDMA_CHN+80)

#undef	PDMA_CHN


#define	PDMA_CHN	PDMA_CHAN6

#define	PDMA_SAR6		(PDMA_CHN)
#define	PDMA_DAR6		(PDMA_CHN+8)
#define	PDMA_LLP6		(PDMA_CHN+16)
#define	PDMA_CTL6		(PDMA_CHN+24)
#define	PDMA_SSTAT6		(PDMA_CHN+32)
#define	PDMA_DSTAT6		(PDMA_CHN+40)
#define	PDMA_SSTATAR6	(PDMA_CHN+48)
#define	PDMA_DSTATAR6	(PDMA_CHN+56)
#define	PDMA_CFG6		(PDMA_CHN+64)
#define	PDMA_SGR6		(PDMA_CHN+72)
#define	PDMA_DSR6		(PDMA_CHN+80)

#undef	PDMA_CHN


#define	PDMA_CHN	PDMA_CHAN7

#define	PDMA_SAR7		(PDMA_CHN)
#define	PDMA_DAR7		(PDMA_CHN+8)
#define	PDMA_LLP7		(PDMA_CHN+16)
#define	PDMA_CTL7		(PDMA_CHN+24)
#define	PDMA_SSTAT7		(PDMA_CHN+32)
#define	PDMA_DSTAT7		(PDMA_CHN+40)
#define	PDMA_SSTATAR7	(PDMA_CHN+48)
#define	PDMA_DSTATAR7	(PDMA_CHN+56)
#define	PDMA_CFG7		(PDMA_CHN+64)
#define	PDMA_SGR7		(PDMA_CHN+72)
#define	PDMA_DSR7		(PDMA_CHN+80)

#undef	PDMA_CHN
    typedef union
    {
        unsigned long long value;
        struct
        {
			unsigned INT_EN  			: 1; //Разрешение прерываний (Interrupt Enable Bit).
			unsigned DST_TR_WIDTH  		: 3; //Разрядность одиночного обращения AHB Приёмника (Destination Transfer
			unsigned SRC_TR_WIDTH  		: 3; //Разрядность одиночного обращения AHB Источника(Source Transfer
			unsigned DINC  				: 2; //Тип изменения адреса Приёмника (Destination Address Increment).
			unsigned SINC  				: 2; //Тип изменения адреса Источника (Source Address Increment).
			unsigned DEST_MSIZE  		: 3; //Размер пакета Приёмника (Destination Burst Transaction Length).
			unsigned SRC_MSIZE  		: 3; //Размер пакета Источника (Source Burst Transaction Length). Определяет
			unsigned SRC_GATHER_EN  	: 1; //Разрешение режима Сбора данных Источника.
			unsigned DST_SCATTER_EN  	: 1; //Разрешение режима Разброса данных Приёмника.
			unsigned Undefined1 		: 1; //Зарезервировано.
			unsigned TT_FC  			: 3; //Тип передачи и Управление размером Блока (Transfer Type and Flow
			unsigned DMS  				: 2; //Destination Master Select. Выбор AHB интерфейса, по которому PDMA
			unsigned SMS  				: 2; //Source Master Select. Выбор AHB интерфейса, по которому PDMA
			unsigned LLP_DST_EN  		: 1; //Разрешение цепочки Блоков Приёмника. Цепочки Блоков Приёмника
			unsigned LLP_SRC_EN  		: 1; //Разрешение цепочки Блоков Источника. Цепочки Блоков Источника
			unsigned Undefined2 		: 3; //Зарезервировано.
			unsigned BLOCK_TS  			: 12; //Размер Блока (Block Transfer Size). Если размером Блока управляет PDMA,
			unsigned DONE  				: 1; //При использовании возможности обратной записи разряды CTLx[63:32]
			unsigned Undefined3 		: 19; //Зарезервировано.
        };
    }PDMA_CTL_t;
    typedef PDMA_CTL_t pdma_ctl_reg;
    typedef union
    {
        unsigned long long value;
        struct
        {
			unsigned int Undefined  : 5; // Зарезервировано.
			unsigned int CH_PRIOR   : 3; //Приоритет Канала (Channel priority).
			unsigned int CH_SUSP    : 1; //Channel Suspend. Запись 1 приостанавливает все передачи данных от
			unsigned int FIFO_EMPTY : 1; // Признак наличия данных в FIFO. Может использоваться совместно с
			unsigned int HS_SEL_DST : 1; // Выбор интерфейса запросов приёмника (Destination Software or Hardware
			unsigned int HS_SEL_SRC : 1; // Выбор интерфейса запросов источника (Source Software or Hardware
			unsigned int LOCK_CH_L  : 2; //Channel Lock Level. Определяет интервал в течении которого
			unsigned int LOCK_B_L   : 2; //Bus Lock Level. Определяет период в течении которого применяется
			unsigned int LOCK_CH    : 1; //Блокировка канала (Channel Lock Bit).
			unsigned int LOCK_B     : 1; //Блокировка шины (Bus Lock Bit).
			unsigned int DST_HS_POL : 1; //Активный уровень интерфейса запросов приёмника (Destination
			unsigned int SRC_HS_POL : 1; //Активный уровень интерфейса запросов источника (Source Handshaking
			unsigned int MAX_ABRST  : 10;//Максимальный размер пакетной транзакции AHB (Maximum AMBA
			unsigned int RELOAD_SRC : 1; //Автоматическое обновление адреса источника (Automatic Source Reload).
			unsigned int RELOAD_DST : 1; //Автоматическое обновление адреса приёмника (Automatic Destination
			unsigned int FCMODE     : 1; //Flow Control Mode. Определяет момент обслуживания запроса передачи
			unsigned int FIFO_MODE  : 1; //Выбор режима FIFO(FIFO Mode Select). Определяет количество данных
			unsigned int PROTCTL    : 3; //(Protection Control). Используется для задания сигналов HPROT[3:1]
			unsigned int DS_UPD_EN  : 1; //Разрешение считывания статуса Приёмника (Destination Status Update
			unsigned int SS_UPD_EN  : 1; //Разрешение считывания статуса Источника (Source Status Update Enable).
			unsigned int SRC_PER    : 4; //Определяет аппаратный интерфейс Запросов Источника для Канала x,
			unsigned int DEST_PER   : 4; //Определяет аппаратный Интерфейс запросов Приёмника для Канала x,
        };
    }PDMA_CFG_t;

typedef volatile struct
{
    volatile unsigned long long SAR;
    volatile unsigned long long DAR;
    volatile unsigned long long LLP;
    volatile unsigned long long CTL;
    //volatile PDMA_CTL_t 		CTL;
    volatile unsigned long long SSTAT;
    volatile unsigned long long DSTAT;
    volatile unsigned long long SSTATAR;
    volatile unsigned long long DSTATAR;
    volatile unsigned long long CFG;
    //volatile PDMA_CFG_t         CFG;
    volatile unsigned long long SGR;
    volatile unsigned long long DSR;
} pdma_mem_channel;

typedef struct
{
    unsigned int 			SAR;
    unsigned int 			DAR;
    unsigned int 			LLP;
    unsigned long long int  CTL;
    unsigned int 			SSTAT;
    unsigned int 			DSTAT;
} pdma_mem_chain;

typedef struct
{
    unsigned int	SAR;
    unsigned int	DAR;
    unsigned int	LLP;
    unsigned int	CTL_LO;
    unsigned int 	CTL_HI;
    unsigned int	SSTAT;
    unsigned int	DSTAT;
} pdma_mem_chain2;

typedef enum _dma_irq {
	IntTfr     = 0U,
    IntBlock   = 1U,
    IntDstTran = 2U,
    IntSrcTran = 3U,
    IntError   = 4U,
} dma_irq;

typedef enum {
    MemoryToMemory     = 0U,
    MemoryToPeripheral = 1U,
    PeripheralToMemory = 2U,
} pdma_transfer_type;

typedef enum {
    DMA_BurstSize1   = 0U,
    DMA_BurstSize4   = 1U,
    DMA_BurstSize8   = 2U,
    DMA_BurstSize16  = 3U,
    DMA_BurstSize32  = 4U,
} pdma_burst_size;

typedef enum {
    Incr     = 0U,
    Decr     = 1U,
    NoChange = 2U
} pdma_incr;

#define _MemCh	((pdma_mem_channel volatile *)PDMA_BASE)

//Регистры прерываний 
typedef volatile struct
{
    volatile unsigned long long RawTfr  ;//0x2C0  R  Немаскируемые прерывания IntTfr 
    volatile unsigned long long RawBlock  ;//0x2C8  R  Немаскируемые прерывания IntBlock 
    volatile unsigned long long RawSrcTran  ;//0x2D0  R  Немаскируемые прерывания IntSrcTran 
    volatile unsigned long long RawDstTrn  ;//0x2D8  R  Немаскируемые прерывания IntDstTran 
    volatile unsigned long long RawErr  ;//0x2E0  R  Немаскируемые прерывания IntErr 
    volatile unsigned long long StatusTfr  ;//0x2E8  R  Статус прерывания IntTfr 
    volatile unsigned long long StatusBlock  ;//0x2F0  R  Статус прерываний IntBlock 
    volatile unsigned long long StatusSrcTran  ;//0x2F8  R  Статус прерываний IntSrcTran 
    volatile unsigned long long StatusDstTran  ;//0x300  R  Статус прерываний IntDstTran 
    volatile unsigned long long StatusErr  ;//0x308  R  Статус прерываний IntErr 
    volatile unsigned long long MaskTfr  ;//0x310  RW  Маска прерываний IntTfr 
    volatile unsigned long long MaskBlock  ;//0x318  RW  Маска прерываний IntBlock 
    volatile unsigned long long MaskSrcTran  ;//0x320  RW  Маска прерываний IntSrcTran 
    volatile unsigned long long MaskDstTran  ;//0x328  RW  Маска прерываний IntDstTran 
    volatile unsigned long long MaskErr  ;//0x330  RW  Маска прерываний IntErr 
    volatile unsigned long long ClearTfr  ;//0x338  W  Сброс прерываний IntTfr 
    volatile unsigned long long ClearBlock  ;//0x340  W  Сброс прерываний IntBlock 
    volatile unsigned long long ClearSrcTran  ;//0x348  W  Сброс прерываний IntSrcTran 
    volatile unsigned long long ClearDstTran  ;//0x350  W  Сброс прерываний IntDstTran 
    volatile unsigned long long ClearErr  ;//0x358  W  Сброс прерываний IntErr 
    volatile unsigned long long StatusInt  ;//0x360  W  Объединённый регистр прерываний 
}pdma_interrupt_regs_t;
//Регистры программного Интерфейса Запросов 
typedef volatile struct
{
    volatile unsigned long long ReqSrcReg  ;//0x368  RW  Запрос передачи пакета Источника 
    volatile unsigned long long ReqDstReg  ;//0x370  RW  Запрос передачи пакета Приёмника 
    volatile unsigned long long SglReqSrcReg  ;//0x378  RW  Запрос одиночного обращения Источника 
    volatile unsigned long long SglReqDstReg  ;//0x380  RW  Запрос одиночного обращения Приёмника 
    volatile unsigned long long LstSrcReg  ;//0x388  RW  Признак последнего обращения Источника 
    volatile unsigned long long LstDstReg  ;//0x390  RW  Признак последнего обращения Приёмника 
}pdma_interrupt_req_regs_t;
//Регистры конфигурации и управления PDMA 
typedef volatile struct
{
    volatile unsigned long long DmaCfgReg  ;//0x398  RW  Конфигурационный регистр PDMA
    volatile unsigned long long ChEnReg  ;//0x3a0  RW  Регистр включения каналов PDMA
    volatile unsigned long long DmaIdReg  ;//0x3a8  R  ID регистр DMA
    volatile unsigned long long DmaTestReg  ;//0x3b0  RW  Тестовый регистр PDMA
}pdma_interrupt_cfg_regs_t;
//Регистры аппаратных параметров PDMA 
typedef volatile struct
{
    volatile unsigned long long Reserved  ;//0x3b8    Зарезервировано 
    volatile unsigned long long Reserved1  ;//0x3c0    Зарезервировано
    volatile unsigned long long DMA_COMP_PARAMS_6  ;//0x3c8  R  6-ой регистр параметров PDMA 
    volatile unsigned long long DMA_COMP_PARAMS_5  ;//0x3d0  R  5-ый регистр параметров PDMA 
    volatile unsigned long long DMA_COMP_PARAMS_4  ;//0x3d8  R  4-ый регистр параметров PDMA 
    volatile unsigned long long DMA_COMP_PARAMS_3  ;//0x3e0  R  3-ий регистр параметров PDMA 
    volatile unsigned long long DMA_COMP_PARAMS_2  ;//0x3e8  R  2-ой регистр параметров PDMA 
    volatile unsigned long long DMA_COMP_PARAMS_1  ;//0x3f0  R  1-ый регистр параметров PDMA 
    volatile unsigned long long Dma_Component_ID  ;//0x3f8  R  ID регистр компонента DMA
}pdma_interrupt_hw_regs_t;


#define pdma_interrupt_regs      ((pdma_interrupt_regs_t     *)(PDMA_BASE + 0x2c0))
#define pdma_interrupt_req_regs  ((pdma_interrupt_req_regs_t *)(PDMA_BASE + 0x368))
#define pdma_interrupt_cfg_regs  ((pdma_interrupt_cfg_regs_t *)(PDMA_BASE + 0x398))
#define pdma_interrupt_hw_regs   ((pdma_interrupt_hw_regs_t  *)(PDMA_BASE + 0x3b8))


typedef void (*handler_funct_ptr)(int);
pdma_mem_channel* get_pdma_dev(int dma_id);
void Init_Pdma_dev(void);
int pdma_configure(int dma_id, void * dst, const void * src, int size);
int pdma_hw_configure(int dma_id, void * dst, const void * src, int size, pdma_mem_chain * link, unsigned int dir);
int pdma_run(int dma_id);
int pdma_wait(int dma_id);
int pdma_is_free(int dma_id);
int pdma_get_free(void);
int pdma_stop(int dma_id);
int pdma_copy(unsigned int dma_id, void * dst, const void * src, unsigned int size);
int pdma_enable_interrupt(int channel, dma_irq irq, handler_funct_ptr funct_pointer);
void pdma_handshake_enable(int channel, int handshake_num);

//pdma_mem_chain * pdma_fill_link(pdma_mem_chain *link, void * dst, void * src, unsigned int size, unsigned int llp_en, pdma_mem_chain * link2);
pdma_mem_chain * pdma_fill_link(pdma_mem_chain * link, void * dst, void * src, unsigned int size, pdma_mem_chain * link2, unsigned int dir);
//int pdma_start_chain(int ch, pdma_mem_chain *link);
int pdma_start_chain(int dma_id, pdma_mem_chain *link, pdma_mem_chain *link2);


#endif
