/* Generated section start */ 
/*
 * @brief UNNAMED
 */
/*!< UNNAMED Controller Interface */
typedef struct UNNAMED_Type
{
	__IO uint32_t CSR;                                   /*!< Offset 0x000 Channel status for DMA channel */
	__IO uint32_t CPC;                                   /*!< Offset 0x004 Channel PC for DMA channel */
} UNNAMED_TypeDef; /* size of structure = 0x008 */
/*
 * @brief UNNAMED
 */
/*!< UNNAMED Controller Interface */
typedef struct UNNAMED_Type
{
	__IO uint32_t SAR;                                   /*!< Offset 0x000 Source address for DMA channel */
	__IO uint32_t DAR;                                   /*!< Offset 0x004 Destination address for DMA channel */
	__IO uint32_t CCR;                                   /*!< Offset 0x008 Channel control for DMA channel  */
	__IO uint32_t LCR0;                                  /*!< Offset 0x00C Loop counter 0 for DMA channel */
	__IO uint32_t LCR1;                                  /*!< Offset 0x010 Loop counter 1 for DMA channel */
	uint32_t reserved_0x014 [0x0003];
} UNNAMED_TypeDef; /* size of structure = 0x020 */
/*
 * @brief UNNAMED
 */
/*!< UNNAMED Controller Interface */
typedef struct UNNAMED_Type
{
	__IO uint32_t DSR;                                   /*!< Offset 0x000 DMA Manager Status Register  */
	__IO uint32_t DPC;                                   /*!< Offset 0x004 DMA Program Counter Register */
	uint32_t reserved_0x008 [0x0006];
	__IO uint32_t INTEN;                                 /*!< Offset 0x020 Interrupt Enable Register */
	__IO uint32_t INT_EVENT_RIS;                         /*!< Offset 0x024 Event-Interrupt Raw Status Register */
	__IO uint32_t INTMIS;                                /*!< Offset 0x028 Interrupt Status Register on page 3-14 */
	__IO uint32_t INTCLR;                                /*!< Offset 0x02C Interrupt Clear Register on page 3-15 */
	__IO uint32_t FSRD;                                  /*!< Offset 0x030 Fault Status DMA Manager Register */
	__IO uint32_t FSRC;                                  /*!< Offset 0x034 Fault Status DMA Channel Register */
	__IO uint32_t FTRD;                                  /*!< Offset 0x038 Fault Type DMA Manager Register */
	uint32_t reserved_0x03C;
	__IO uint32_t FTR [0x008];                           /*!< Offset 0x040 Fault type for DMA channel 0..7 */
	uint32_t reserved_0x060 [0x0028];
	DMAC_THREAD_TypeDef CH_THREAD [0x008];               /*!< Offset 0x100 Channel status and PC for DMA channel 0..7 */
	uint32_t reserved_0x140 [0x00B0];
	DMAC_CH_TypeDef CH [0x008];                          /*!< Offset 0x400 Channel status and PC for DMA channel 0..7 */
} UNNAMED_TypeDef; /* size of structure = 0x500 */
/* Generated section end */ 
