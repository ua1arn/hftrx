/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include "hardware.h"
#include "dspdefines.h"

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MODEMBUFFERSIZE8	1024

#if WITHUSEDUALWATCH
	#define NTRX 2	/* количество трактов приемника. */
#else /* WITHUSEDUALWATCH */
	#define NTRX 1	/* количество трактов приемника. */
#endif /* WITHUSEDUALWATCH */

#define FIRBUFSIZE 1024	/* это не порядок фильтра, просто размер буфера при передачи данных к user mode обработчику */

/* Применённая система диспетчеризации требует,
   чтобы во всех буферах помещалось не меньше сэмплов,
   чем в DMABUFFSIZE32RX
 */
#if WITHDSPEXTDDC

	#if CPUSTYLE_R7S721

		// buff data layout: I main/I sub/Q main/Q sub
		#define DMABUFFSTEP32RX		8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
		// buff data layout: I_T0/Q_T0/I_T1/Q_T1
		#define DMABUFFSTEP32RTS	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

		#define DMABUF32RX0I	0		// RX0, I
		#define DMABUF32RX1I	1		// RX1, I
		#define DMABUF32RX0Q	4		// RX0, Q
		#define DMABUF32RX1Q	5		// RX1, Q

		#define DMABUFFSTEP32TX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
		#define DMABUF32TXI	0		// TX, I
		#define DMABUF32TXQ	4		// TX, Q

		#define DMABUF32TX_NCO1		6		// NCO RX A
		#define DMABUF32TX_NCO2		2		// NCO RX B
		#define DMABUF32TX_NCORTS	7		// NCO RTS

		// ws=0: 00 01 02 03
		// ws=1: 04 05 06 07

		#if WITHRTS96
			#define DMABUF32RTS0I	2		// RTS0, I	// previous - oldest
			#define DMABUF32RTS0Q	6		// RTS0, Q	// previous
			#define DMABUF32RTS1I	3		// RTS1, I	// current	- nevest
			#define DMABUF32RTS1Q	7		// RTS1, Q	// current
		#endif /* WITHRTS96 */


		// Slot S0, S4: Oldest sample (T-3)
		// Slot S1, S5: Old sample (T-2)
		// Slot S2, S6: Old sample (T-1)
		// Slot S3, S7: Newest sample (T-0)
		#define DMABUF32RXWFM0I	0		// WFM OLDEST
		#define DMABUF32RXWFM0Q	4		// WFM
		#define DMABUF32RXWFM1I	1		// WFM
		#define DMABUF32RXWFM1Q	5		// WFM
		#define DMABUF32RXWFM2I	2		// WFM
		#define DMABUF32RXWFM2Q	6		// WFM
		#define DMABUF32RXWFM3I	3		// WFM NEWEST
		#define DMABUF32RXWFM3Q	7		// WFM

	#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

		#if WITHFPGAIF_FRAMEBITS == 64

			#define DMABUFFSTEP32RX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUFFSTEP32RTS	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUF32RX0I	0		// RX0, I
			#define DMABUF32RX0Q	1		// RX0, Q
			#define DMABUFFSTEP32TX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUF32TXI		0		// TX, I
			#define DMABUF32TXQ		1		// TX, Q

		#elif WITHFPGAIF_FRAMEBITS == 256
			// buff data layout: I main/I sub/Q main/Q sub
			#define DMABUFFSTEP32RX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
			// buff data layout: I_T0/Q_T0/I_T1/Q_T1
			#define DMABUFFSTEP32RTS	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

			#define DMABUF32RX0I	0		// RX0, I
			#define DMABUF32RX0Q	1		// RX0, Q
			#define DMABUF32RX1I	2		// RX1, I
			#define DMABUF32RX1Q	3		// RX1, Q

		#if WITHRTS96
			#define DMABUF32RTS0I	4		// RTS0, I	// previous - oldest
			#define DMABUF32RTS0Q	5		// RTS0, Q	// previous
			#define DMABUF32RTS1I	6		// RTS1, I	// current	- nevest
			#define DMABUF32RTS1Q	7		// RTS1, Q	// current
		#endif /* WITHRTS96 */
		
			// Slot S0, S4: Oldest sample (T-3)
			// Slot S1, S5: Old sample (T-2)
			// Slot S2, S6: Old sample (T-1)
			// Slot S3, S7: Newest sample (T-0)
			#define DMABUF32RXWFM0I	0		// WFM OLDEST
			#define DMABUF32RXWFM0Q	4		// WFM
			#define DMABUF32RXWFM1I	1		// WFM
			#define DMABUF32RXWFM1Q	5		// WFM
			#define DMABUF32RXWFM2I	2		// WFM
			#define DMABUF32RXWFM2Q	6		// WFM
			#define DMABUF32RXWFM3I	3		// WFM NEWEST
			#define DMABUF32RXWFM3Q	7		// WFM

			#define DMABUFFSTEP32TX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUF32TXI	0		// TX, I
			#define DMABUF32TXQ	1		// TX, Q

		#endif /* WITHFPGAIF_FRAMEBITS */

	#elif CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

		#if WITHFPGAIF_FRAMEBITS == 64

			#define DMABUFFSTEP32RX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUFFSTEP32RTS	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUF32RX0I	0		// RX0, I
			#define DMABUF32RX0Q	1		// RX0, Q
			#define DMABUFFSTEP32TX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUF32TXI		0		// TX, I
			#define DMABUF32TXQ		1		// TX, Q

		#elif WITHFPGAIF_FRAMEBITS == 256
			// buff data layout: I main/I sub/Q main/Q sub
			#define DMABUFFSTEP32RX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
			// buff data layout: I_T0/Q_T0/I_T1/Q_T1
			#define DMABUFFSTEP32RTS	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

			#define DMABUF32RX0I	0		// RX0, I
			#define DMABUF32RX1I	1		// RX1, I
			#define DMABUF32RX0Q	4		// RX0, Q
			#define DMABUF32RX1Q	5		// RX1, Q

			#if WITHRTS96
				#define DMABUF32RTS0I	2		// RTS0, I	// previous - oldest
				#define DMABUF32RTS0Q	6		// RTS0, Q	// previous
				#define DMABUF32RTS1I	3		// RTS1, I	// current	- nevest
				#define DMABUF32RTS1Q	7		// RTS1, Q	// current
			#endif /* WITHRTS96 */

			// Slot S0, S4: Oldest sample (T-3)
			// Slot S1, S5: Old sample (T-2)
			// Slot S2, S6: Old sample (T-1)
			// Slot S3, S7: Newest sample (T-0)
			#define DMABUF32RXWFM0I	0		// WFM OLDEST
			#define DMABUF32RXWFM0Q	4		// WFM
			#define DMABUF32RXWFM1I	1		// WFM
			#define DMABUF32RXWFM1Q	5		// WFM
			#define DMABUF32RXWFM2I	2		// WFM
			#define DMABUF32RXWFM2Q	6		// WFM
			#define DMABUF32RXWFM3I	3		// WFM NEWEST
			#define DMABUF32RXWFM3Q	7		// WFM

			#define DMABUFFSTEP32TX	8		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUF32TXI	0		// TX, I
			#define DMABUF32TXQ	4		// TX, Q

			#define DMABUF32TX_NCO1		6		// NCO RX A
			#define DMABUF32TX_NCO2		2		// NCO RX B
			#define DMABUF32TX_NCORTS	7		// NCO RTS

			// ws=0: 00 01 02 03
			// ws=1: 04 05 06 07
		#else
			#error Undefined WITHFPGAIF_FRAMEBITS value
		#endif

	#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_GW2A_V0)
		// Allwinner t113-s3, Allwinner D1s (F133): I2S/PCM have non-sequential numbering of samples in DMA buffer
		// ws=0: even samples, ws=1: odd samples

		/* FPGA */
		// buff data layout: I main/I sub/Q main/Q sub
		#define DMABUFFSTEP32RX	2		// Каждому сэмплу соответствует восемь чисел в DMA буфере
		// buff data layout: I_T0/Q_T0/I_T1/Q_T1
		#define DMABUFFSTEP32RTS	2		// Каждому сэмплу соответствует восемь чисел в DMA буфере

		#define DMABUF32RX0I	0		// RX0, I
		#define DMABUF32RX0Q	1		// RX0, Q

		#define DMABUF32RTS0I	0		// RTS0, I	// previous - oldest
		#define DMABUF32RTS0Q	1		// RTS0, Q	// previous

		/* FPGA */
		#define DMABUFFSTEP32TX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
		#define DMABUF32TXI	0		// TX, I
		#define DMABUF32TXQ	1		// TX, Q

	#elif WITHUSBMIKET113
		// Allwinner t113-s3, Allwinner D1s (F133): I2S/PCM have non-sequential numbering of samples in DMA buffer
		// ws=0: even samples, ws=1: odd samples


		// buff data layout: I main/I sub/Q main/Q sub
		#define DMABUFFSTEP32RX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
		// buff data layout: I_T0/Q_T0/I_T1/Q_T1
		#define DMABUFFSTEP32RTS	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

		#define DMABUF32RX0I	0		// RX0, I
		#define DMABUF32RX1I	2		// RX1, I
		#define DMABUF32RX0Q	1		// RX0, Q
		#define DMABUF32RX1Q	3		// RX1, Q

		#define DMABUFFSTEP32TX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
		#define DMABUF32TXI	0		// TX, I
		#define DMABUF32TXQ	1		// TX, Q

		#define DMABUF32TX_NCO1		5		// NCO RX A
		#define DMABUF32TX_NCO2		4		// NCO RX B
		#define DMABUF32TX_NCORTS	7		// NCO RTS

		// ws=0: 00 02 04 06
		// ws=1: 01 03 05 07

		#if WITHRTS96
			#define DMABUF32RTS0I	4		// RTS0, I	// previous - oldest
			#define DMABUF32RTS0Q	5		// RTS0, Q	// previous
			#define DMABUF32RTS1I	6		// RTS1, I	// current	- nevest
			#define DMABUF32RTS1Q	7		// RTS1, Q	// current
		#endif /* WITHRTS96 */

		// Allwinner t113-s3: I2S/PCM have non-sequential numbering of samples in DMA buffer
		// ws=0: even samples, ws=1: odd samples

		// Slot S0, S4: Oldest sample (T-3)
		// Slot S1, S5: Old sample (T-2)
		// Slot S2, S6: Old sample (T-1)
		// Slot S3, S7: Newest sample (T-0)
		#define DMABUF32RXWFM0I	0		// WFM OLDEST
		#define DMABUF32RXWFM0Q	1		// WFM
		#define DMABUF32RXWFM1I	2		// WFM
		#define DMABUF32RXWFM1Q	3		// WFM
		#define DMABUF32RXWFM2I	4		// WFM
		#define DMABUF32RXWFM2Q	5		// WFM
		#define DMABUF32RXWFM3I	6		// WFM NEWEST
		#define DMABUF32RXWFM3Q	7		// WFM

	#elif CPUSTYLE_ALLWINNER
		// Allwinner t113-s3, Allwinner D1s (F133): I2S/PCM have non-sequential numbering of samples in DMA buffer
		// ws=0: even samples, ws=1: odd samples
		#if WITHFPGAIF_FRAMEBITS == 256

			// buff data layout: I main/I sub/Q main/Q sub
			#define DMABUFFSTEP32RX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
			// buff data layout: I_T0/Q_T0/I_T1/Q_T1
			#define DMABUFFSTEP32RTS	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

			#define DMABUF32RX0I	0		// RX0, I
			#define DMABUF32RX1I	2		// RX1, I
			#define DMABUF32RX0Q	1		// RX0, Q
			#define DMABUF32RX1Q	3		// RX1, Q

			#define DMABUFFSTEP32TX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
			#define DMABUF32TXI	0		// TX, I
			#define DMABUF32TXQ	1		// TX, Q

			#define DMABUF32TX_NCO1		5		// NCO RX A
			#define DMABUF32TX_NCO2		4		// NCO RX B
			#define DMABUF32TX_NCORTS	7		// NCO RTS

			// ws=0: 00 02 04 06
			// ws=1: 01 03 05 07

			#if WITHRTS96
				#define DMABUF32RTS0I	4		// RTS0, I	// previous - oldest
				#define DMABUF32RTS0Q	5		// RTS0, Q	// previous
				#define DMABUF32RTS1I	6		// RTS1, I	// current	- nevest
				#define DMABUF32RTS1Q	7		// RTS1, Q	// current
			#endif /* WITHRTS96 */

			// Allwinner t113-s3: I2S/PCM have non-sequential numbering of samples in DMA buffer
			// ws=0: even samples, ws=1: odd samples

			// Slot S0, S4: Oldest sample (T-3)
			// Slot S1, S5: Old sample (T-2)
			// Slot S2, S6: Old sample (T-1)
			// Slot S3, S7: Newest sample (T-0)
			#define DMABUF32RXWFM0I	0		// WFM OLDEST
			#define DMABUF32RXWFM0Q	1		// WFM
			#define DMABUF32RXWFM1I	2		// WFM
			#define DMABUF32RXWFM1Q	3		// WFM
			#define DMABUF32RXWFM2I	4		// WFM
			#define DMABUF32RXWFM2Q	5		// WFM
			#define DMABUF32RXWFM3I	6		// WFM NEWEST
			#define DMABUF32RXWFM3Q	7		// WFM

		#elif WITHFPGAIF_FRAMEBITS == 512
			// buff data layout: I main/I sub/Q main/Q sub
			#define DMABUFFSTEP32RX	16		// Каждому сэмплу соответствует шестнадцать чисел в DMA буфере
			// buff data layout: I_T0/Q_T0/I_T1/Q_T1
			#define DMABUFFSTEP32RTS	DMABUFFSTEP32RX		// Каждому сэмплу соответствует восемь чисел в DMA буфере

			#define DMABUF32RX0I	8		// RX0, I
			#define DMABUF32RX0Q	9		// RX0, Q
			#define DMABUF32RX1I	10		// RX1, I
			#define DMABUF32RX1Q	11		// RX1, Q

			#define DMABUFF32RX_CODEC1_LEFT 	12		/* индекс сэмпла левого канала от кодека (через PIPE) */
			#define	DMABUFF32RX_CODEC1_RIGHT 	13		/* индекс сэмпла правого канала от кодека (через PIPE)  */
			// ws=0: 00 02 04 06
			// ws=1: 01 03 05 07

			#if WITHRTS96
				#define DMABUF32RTS0I	4		// RTS0, I	// previous - oldest
				#define DMABUF32RTS0Q	5		// RTS0, Q	// previous
				#define DMABUF32RTS1I	6		// RTS1, I	// current	- nevest
				#define DMABUF32RTS1Q	7		// RTS1, Q	// current
			#endif /* WITHRTS96 */


			// Allwinner t113-s3: I2S/PCM have non-sequential numbering of samples in DMA buffer
			// ws=0: even samples, ws=1: odd samples

			#if WITHWFM
				#define DMABUF32RXWFM0I	0		// WFM OLDEST
				#define DMABUF32RXWFM0Q	1		// WFM
				#define DMABUF32RXWFM1I	2		// WFM
				#define DMABUF32RXWFM1Q	3		// WFM
				#define DMABUF32RXWFM2I	4		// WFM
				#define DMABUF32RXWFM2Q	5		// WFM
				#define DMABUF32RXWFM3I	6		// WFM NEWEST
				#define DMABUF32RXWFM3Q	7		// WFM
			#endif /* WITHWFM */

			#define DMABUFFSTEP32TX	16		// Каждому сэмплу соответствует шестнадцать чисел в DMA буфере

			#define DMABUF32TXI	0		// TX, I
			#define DMABUF32TXQ	1		// TX, Q

			#define DMABUF32TX_NCO1		5		// NCO RX A
			#define DMABUF32TX_NCO2		4		// NCO RX B
			#define DMABUF32TX_NCORTS	7		// NCO RTS
			/* звук идет по PIPE */

			#define DMABUFF32TX_CODEC1_LEFT 	14		/* индекс сэмпла левого канала к кодеку (через PIPE) */
			#define	DMABUFF32TX_CODEC1_RIGHT 	15		/* индекс сэмпла правого канала к кодеку (через PIPE)  */


		#else
			#error Undefined WITHFPGAIF_FRAMEBITS
		#endif

	#else
		#warning Define I2S layout for this CPUSTYLE_XXX

		// buff data layout: I main/I sub/Q main/Q sub
		#define DMABUFFSTEP32RX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
		// buff data layout: I_T0/Q_T0/I_T1/Q_T1
		#define DMABUFFSTEP32RTS	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

		#define DMABUF32RX0I	0		// RX0, I
		#define DMABUF32RX1I	1		// RX1, I
		#define DMABUF32RX0Q	4		// RX0, Q
		#define DMABUF32RX1Q	5		// RX1, Q

		#define DMABUFFSTEP32TX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
		#define DMABUF32TXI	0		// TX, I
		#define DMABUF32TXQ	4		// TX, Q

		#if WITHRTS96
			#define DMABUF32RTS0I	2		// RTS0, I	// previous - oldest
			#define DMABUF32RTS0Q	6		// RTS0, Q	// previous
			#define DMABUF32RTS1I	3		// RTS1, I	// current	- nevest
			#define DMABUF32RTS1Q	7		// RTS1, Q	// current
		#endif /* WITHRTS96 */


		// Slot S0, S4: Oldest sample (T-3)
		// Slot S1, S5: Old sample (T-2)
		// Slot S2, S6: Old sample (T-1)
		// Slot S3, S7: Newest sample (T-0)
		#define DMABUF32RXWFM0I	0		// WFM OLDEST
		#define DMABUF32RXWFM0Q	4		// WFM
		#define DMABUF32RXWFM1I	1		// WFM
		#define DMABUF32RXWFM1Q	5		// WFM
		#define DMABUF32RXWFM2I	2		// WFM
		#define DMABUF32RXWFM2Q	6		// WFM
		#define DMABUF32RXWFM3I	3		// WFM NEWEST
		#define DMABUF32RXWFM3Q	7		// WFM

	#endif

#else /* WITHDSPEXTDDC */
	// buff data layout: ADC data/unused channel
	#define DMABUF32RX		0		// ADC data index
	#define DMABUFFSTEP32RX	(WITHFPGAIF_FRAMEBITS / 32) //2		// 2 - каждому сэмплу соответствует два числа в DMA буфере
	#define DMABUFFSTEP32RTS	(WITHFPGARTS_FRAMEBITS / 32) //2		// 2 - каждому сэмплу соответствует два числа в DMA буфере
	#define DMABUF32RXI	0		// RX0, I
	#define DMABUF32RXQ	1		// RX0, Q

	#define DMABUFFSTEP32TX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
	#define DMABUF32TXI	0		// TX, I
	#define DMABUF32TXQ	1		// TX, Q

#endif /* WITHDSPEXTDDC */

#if WITHCODEC1_WHBLOCK_DUPLEX_MASTER && (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	/* встороенный в процессор кодек */

	//	ix = 0: R5 & R11 (lineinL - pin 96 & fminL pin 94)
	//	ix = 1: R6 & R10 (lineinR - pin 95 & fminR pin 93)
	//	ix = 2: micin3N & micin3P

	#define DMABUFFSTEP16RX		3		/* 3 - каждому сэмплу при получении от AUDIO CODEC соответствует три числа в DMA буфере */
	#define DMABUFF16RX_MIKE 	2		/* индекс сэмпла левого канала */

	#define DMABUFFSTEP16TX		2		/* 2 - каждому сэмплу при передаче в AUDIO CODEC соответствует два числа в DMA буфере */
	#define DMABUFF16TX_LEFT 	0		/* индекс сэмпла левого канала */
	#define DMABUFF16TX_RIGHT 	1		/* индекс сэмпла правого канала */

#else /* WITHCODEC1_WHBLOCK_DUPLEX_MASTER && (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64) */

	/* Обычный I2S канал */

	#define DMABUFFSTEP16RX		2		/* 2 - каждому сэмплу при получении от AUDIO CODEC соответствует два числа в DMA буфере */

	#define DMABUFF16RX_MIKE 	0		/* индекс сэмпла канала микрофона */
	#define DMABUFF16RX_LEFT 	0		/* for PIPE: индекс сэмпла левого канала */
	#define DMABUFF16RX_RIGHT 	1		/* for PIPE: индекс сэмпла правого канала */

	#define DMABUFFSTEP16TX		2		/* 2 - каждому сэмплу при передаче в AUDIO CODEC соответствует два числа в DMA буфере */

	#define DMABUFF16TX_LEFT 	0		/* индекс сэмпла левого канала */
	#define DMABUFF16TX_RIGHT 	1		/* индекс сэмпла правого канала */

#endif /* WITHCODEC1_WHBLOCK_DUPLEX_MASTER && (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64) */

// Требования по кратности размера буфера для передачи по USB DMA
#if CPUSTYLE_R7S721
	#define HARDWARE_RTSDMABYTES	4
#else /* CPUSTYLE_R7S721 */
	#define HARDWARE_RTSDMABYTES	1
#endif /* CPUSTYLE_R7S721 */

// Конфигурация потоков в Input Terminal Descriptor
// bNrChannels в 4.3.2.1 Input Terminal Descriptor образуется подсчетом битов в данном поле
// Может быть использовано AUDIO_CHANNEL_M
#define UACIN_CONFIG_IN48 			(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)
#define UACIN_CONFIG_INRTS 			(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)
#define UACIN_CONFIG_IN48_INRTS 	(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)
#define UACOUT_CONFIG_OUT48 		(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)

// количество каналов в дескрипторах формата потока
#define UACIN_FMT_CHANNELS_AUDIO48			2
#define UACIN_FMT_CHANNELS_RTS				2	// I/Q всегда стерео
#define UACIN_FMT_CHANNELS_AUDIO48_RTS		2	// при совмещении аудио и I/Q всегда стерео

#if WITHUABUACOUTAUDIO48MONO
	// количество каналов в дескрипторах формата потока
	#define UACOUT_FMT_CHANNELS_AUDIO48	1
#else /* WITHUABUACOUTAUDIO48MONO */
	// количество каналов в дескрипторах формата потока
	#define UACOUT_FMT_CHANNELS_AUDIO48	2
#endif /* WITHUABUACOUTAUDIO48MONO */


#define UACIN_FMT_CHANNELS_RTS96 2	// всегда I/Q
#define UACIN_FMT_CHANNELS_RTS192 2	// всегда I/Q

/*
	For full-/high-speed isochronous endpoints, this value
	must be in the range from 1 to 16. The bInterval value
	is used as the exponent for a 2^(bInterval-1) value; e.g.,
	a bInterval of 4 means a period of 8 (2^(4-1))."

  */
/* константы. С запасом чтобы работало и при тактовой 125 МГц на FPGA при децимации 2560 = 48.828125 kHz sample rate */
//#define OUTSAMPLES_AUDIO48	49 /* количество сэмплов за милисекунду в UAC OUT */
// без запаса - только для 48000

#if WITHUSBDEV_HSDESC && CPUSTYLE_ALLWINNER
	#define OUTSAMPLES_AUDIO48	6 /* количество сэмплов за SOF в UAC OUT */

	#define HSINTERVAL_AUDIO48 1	// 1 - 125 uS, 2 - 250 uS, 3 - 500 uS 4 - 1 mS
	#define HSINTERVAL_RTS96 1
	#define HSINTERVAL_RTS192 1

	#define FSINTERVAL_AUDIO48 1
	#define FSINTERVAL_AUDIO48 1
	#define FSINTERVAL_RTS96 1
	#define FSINTERVAL_RTS192 1

#elif WITHUSBDEV_HSDESC
	#define OUTSAMPLES_AUDIO48	12 /* количество сэмплов за SOF в UAC OUT */

	#define HSINTERVAL_AUDIO48 2	// 1 - 125 uS, 2 - 250 uS, 3 - 500 uS 4 - 1 mS
	#define HSINTERVAL_RTS96 2
	#define HSINTERVAL_RTS192 2

	#define FSINTERVAL_AUDIO48 1
	#define FSINTERVAL_RTS96 1
	#define FSINTERVAL_RTS192 1

#else /* WITHUSBDEV_HSDESC */
	#define OUTSAMPLES_AUDIO48	48 /* количество сэмплов за милисекунду в UAC OUT */

	#define HSINTERVAL_AUDIO48 1	// dummy parameter
	#define HSINTERVAL_RTS96 1		// dummy parameter
	#define HSINTERVAL_RTS192 1		// dummy parameter

	#define FSINTERVAL_AUDIO48 1
	#define FSINTERVAL_RTS96 1
	#define FSINTERVAL_RTS192 1

#endif /* WITHUSBDEV_HSDESC */

/* Разрядности сэмплов в каналах USB AUDIO устрйоств. UACIN - в компьютер, UACOUT - из компьютера */
#if ! defined (UACOUT_AUDIO48_SAMPLEBYTES)
	#define UACOUT_AUDIO48_SAMPLEBYTES	2	/* должны быть 2, 3 или 4 */
#endif /* ! defined (UACOUT_AUDIO48_SAMPLEBYTES) */

#if ! defined (UACIN_AUDIO48_SAMPLEBYTES)
	#define UACIN_AUDIO48_SAMPLEBYTES	2	/* должны быть 2, 3 или 4 */
#endif /* ! defined (UACIN_AUDIO48_SAMPLEBYTES) */

#if ! defined (UACIN_RTS96_SAMPLEBYTES)
	#define UACIN_RTS96_SAMPLEBYTES	3	/* должны быть 2, 3 или 4 */
#endif /* ! defined (UACIN_RTS96_SAMPLEBYTES) */

#if ! defined (UACIN_RTS192_SAMPLEBYTES)
	#if CPUSTYLE_R7S721
		#define UACIN_RTS192_SAMPLEBYTES	4	/* должны быть 2, 3 или 4 */
	#else /* CPUSTYLE_R7S721 */
		#define UACIN_RTS192_SAMPLEBYTES	3	/* должны быть 2, 3 или 4 */
	#endif /* CPUSTYLE_R7S721 */
#endif /* ! defined (UACIN_RTS192_SAMPLEBYTES) */

#define EP_align(v, g) (((v) + (g) - 1) / (g) * (g))	// Округление v до g
#define EPDSZMAX(a, b) ((a) > (b) ? (a) : (b))

#define UAC_n1c2g1 (1)	// 2 байта в ячейку 1
#define UAC_n2c2g1 (1)	// 4 байта в ячейку 1
#define UAC_n3c2g1 (1)	// 6 байт в ячейку 1
#define UAC_n4c2g1 (1)	// 8 байт в ячейку 1

#define UAC_n1c2g2 (1)	// 2 байта в ячейку 2
#define UAC_n2c2g2 (1)	// 4 байта в ячейку 2
#define UAC_n3c2g2 (1)	// 6 байт в ячейку 2
#define UAC_n4c2g2 (1)	// 8 байт в ячейку 2

#define UAC_n1c2g4 (2)	// 2 байта в ячейку 4
#define UAC_n2c2g4 (1)	// 4 байта в ячейку 4
#define UAC_n3c2g4 (2)	// 6 байт в ячейку 4
#define UAC_n4c2g4 (2)	// 8 байт в ячейку 4

#define UAC_n1c2g8 (4)	// 2 байта в ячейку 8
#define UAC_n2c2g8 (2)	// 4 байта в ячейку 8
#define UAC_n3c2g8 (4)	// 6 байт в ячейку 8
#define UAC_n4c2g8 (1)	// 8 байт в ячейку 8

// ss - sample size, ch - количество каналов, ga - granulation for size
// выдаёт грануляцию количества сэмплов
#define UAC_ng(ss, ch, ga) (UAC_n ## ss ## c ## ch ## g ## ga)	// таблица



// расчет количества байтов для endpoint
// ga - требование выравнивания от DMA = EPALIGN
// ss - требование выравнивания от канала связи

// n - sample number, ng - sample number granulation, ss - sample size
// выдаёт колчиество байтов
#define UAC_DATASIZEgr(n, ng, ss) ( EP_align((n), (ng)) * (ss))

// n - sample number, ss - sample size, ch - количество каналов
// выдаёт колчиество байтов
/* требования по выравниванию DMA про обмене с USB */
#if CPUSTYLE_ALLWINNER
	#define UAC_DATASIZE(n, ss, ch) (UAC_DATASIZEgr((n), UAC_ng(ss, ch, 4), (ss) * (ch)))
#elif CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7
	#define UAC_DATASIZE(n, ss, ch) (UAC_DATASIZEgr((n), UAC_ng(ss, ch, 1), (ss) * (ch)))
#elif CPUSTYLE_R7S721
	#define UAC_DATASIZE(n, ss, ch) (UAC_DATASIZEgr((n), UAC_ng(ss, ch, 8), (ss) * (ch)))
#else
	#define UAC_DATASIZE(n, ss, ch) (UAC_DATASIZEgr((n), UAC_ng(ss, ch, 1), (ss) * (ch)))
#endif

/* Размры буферов ендпоинт в байтах */

#define UACOUT_AUDIO48_DATASIZE	UAC_DATASIZE(OUTSAMPLES_AUDIO48 + 1, UACOUT_AUDIO48_SAMPLEBYTES, UACOUT_FMT_CHANNELS_AUDIO48)
#define UACIN_AUDIO48_DATASIZE 	UAC_DATASIZE(OUTSAMPLES_AUDIO48 + 1, UACIN_AUDIO48_SAMPLEBYTES, UACIN_FMT_CHANNELS_AUDIO48)
#define UACIN_RTS96_DATASIZE 	UAC_DATASIZE(OUTSAMPLES_AUDIO48 * 2 + 1, UACIN_RTS96_SAMPLEBYTES, UACIN_FMT_CHANNELS_RTS96)
#define UACIN_RTS192_DATASIZE 	UAC_DATASIZE(OUTSAMPLES_AUDIO48 * 4 + 1, UACIN_RTS192_SAMPLEBYTES, UACIN_FMT_CHANNELS_RTS192)

#ifndef DMABUFCLUSTER
/* если приоритет прерываний USB не выше чем у аудиобработки - она должна длиться не более 1 мс (WITHRTS192 - 0.5 ms) */
#define DMABUFCLUSTER	33	// Прерывания по приему от IF CODEC или FPGA RX должны происходить не реже 1 раз в милисекунду (чтобы USB работать могло) */
#endif /* DMABUFCLUSTER */
#define DMABUFSCALE		2	// внутрений параметр, указывает, на сколько реже будут происходить прерывания по обмену буфрами от остальны каналов по отношению к приему от FPGA

#define DMABUFFSIZE16RX	(DMABUFCLUSTER * DMABUFFSTEP16RX)		/* AF CODEC ADC */
#define DMABUFFSIZE32RX (DMABUFCLUSTER * DMABUFFSTEP32RX)		/* FPGA RX or IF CODEC RX */
#define DMABUFFSIZE32RTS (DMABUFCLUSTER * DMABUFFSTEP32RTS)		/* FPGA RX or IF CODEC RX */

#define DMABUFFSIZE16TX	(DMABUFCLUSTER * DMABUFFSTEP16TX * DMABUFSCALE)		/* AF CODEC DAC */
#define DMABUFFSIZE32TX (DMABUFCLUSTER * DMABUFFSTEP32TX * DMABUFSCALE)	/* FPGA TX or IF CODEC TX	*/

#define DMABUFFSTEP192RTS 2
#define DMABUFFSIZE192RTS	(DMABUFCLUSTER * DMABUFFSTEP192RTS)		/* RTS192 data from I2S */


/*
	For full-/high-speed isochronous endpoints, this value
	must be in the range from 1 to 16. The bInterval value
	is used as the exponent for a 2^(bInterval-1) value; e.g.,
	a bInterval of 4 means a period of 8 (2^(4-1))."

  */

#define HSINTERVAL_1MS 4	// endpoint descriptor parameters - для обеспечения 10 ms периода
#define FSINTERVAL_1MS 1

#define HSINTERVAL_8MS 7	// endpoint descriptor parameters - для обеспечения 10 ms периода
#define FSINTERVAL_8MS 8

#define HSINTERVAL_32MS 9	// endpoint descriptor parameters - для обеспечения 32 ms периода
#define FSINTERVAL_32MS 32

#define HSINTERVAL_256MS 12	// endpoint descriptor parameters - для обеспечения 255 ms периода (interrupt endpoint for CDC)
#define FSINTERVAL_255MS 255

enum
{
	BOARD_WTYPE_BLACKMAN_HARRIS,
	BOARD_WTYPE_BLACKMAN_HARRIS_MOD,
	BOARD_WTYPE_BLACKMAN_HARRIS_3TERM,
	BOARD_WTYPE_BLACKMAN_HARRIS_3TERM_MOD,
	BOARD_WTYPE_BLACKMAN_HARRIS_4TERM,
	BOARD_WTYPE_BLACKMAN_HARRIS_7TERM,
	BOARD_WTYPE_BLACKMAN_NUTTALL,
	BOARD_WTYPE_NUTTALL,		// Nuttall window, continuous first derivative
	BOARD_WTYPE_HAMMING,
	BOARD_WTYPE_HANN,
	BOARD_WTYPE_RECTANGULAR,
	//
	BOARD_WTYPE_count
};

#define BOARD_WTYPE_FILTERS BOARD_WTYPE_BLACKMAN_HARRIS_4TERM
#define BOARD_WTYPE_SPECTRUM BOARD_WTYPE_NUTTALL	// такой же тип окна испольуется по умолчанию в HDSDR

FLOAT_t fir_design_window(int iCnt, int iCoefNum, int wtype); // Calculate window function (blackman-harris, hamming, rectangular)

// Ограничение алгоритма генерации параметров фильтра - нечётное значение Ntap.
// Кроме того, для функций фильтрации с использованием симметрии коэффициентов, требуется кратность 2 половины Ntap

#define NtapValidate(n)	((unsigned) (n) / 8 * 8 + 1)	/* Гарантируется пригодность для симметричного фильтра */
#define NtapCoeffs(n)	((unsigned) (n) / 2 + 1)

#if WITHDSPLOCALFIR || WITHDSPLOCALTXFIR

	/* Фильтрация квадратур осуществляется процессором */

	#if CPUSTYLE_R7S721
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation
		#define	Ntap_rx_AUDIO	NtapValidate(241)

	#elif CPUSTYLE_STM32MP1 || CPUSTYLE_XC7Z || CPUSTYLE_XCZU || (CPUSTYLE_T113 && WITHDSPLOCALFIR)
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(241)	// single samples, floating point implementation
		#define	Ntap_rx_AUDIO	NtapValidate(241)

	#elif CPUSTYLE_STM32F7XX
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation
		#define	Ntap_rx_AUDIO	NtapValidate(241)

	#else
		#define Ntap_rx_SSB_IQ	NtapValidate(181)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(181)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation
		#define	Ntap_rx_AUDIO	NtapValidate(241)

	#endif

#else /* WITHDSPLOCALFIR */

	/* Фильтрация квадратур осуществляется FPGA */

	#if CPUSTYLE_STM32MP1 || CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_XC7Z || CPUSTYLE_XCZU
		#define	Ntap_rx_AUDIO	NtapValidate(1023)
		#define Ntap_tx_MIKE	NtapValidate(511)

	#else /* CPUSTYLE_STM32MP1 */
		#define	Ntap_rx_AUDIO	NtapValidate(511)
		#define Ntap_tx_MIKE	NtapValidate(511)

	#endif /* CPUSTYLE_STM32MP1 */

#endif /* WITHDSPLOCALFIR */

#if WITHUSBMIKET113 && WITHCODEC1_WHBLOCK_DUPLEX_MASTER

	/* работа со встроенным кодеком Allwinner t113-s3 */
	/* параметры входного/выходного адаптеров */
	#define WITHADAPTERCODEC1WIDTH	20		// 1 бит знак и 19 бит значащих
	#define WITHADAPTERCODEC1SHIFT	12		// количество незанятых битов справа.
	typedef int32_t aubufv_t;
	typedef int_fast32_t aufastbufv_t;
	typedef int_fast64_t aufastbufv2x_t;	/* тип для работы ресэмплера при получении среднего арифметического */

#elif CODEC1_FRAMEBITS == 64

	/* параметры входного/выходного адаптеров */
	#define WITHADAPTERCODEC1WIDTH	24		// 1 бит знак и 23 бит значащих
	#define WITHADAPTERCODEC1SHIFT	8		// количество незанятых битов справа.
	typedef int32_t aubufv_t;
	typedef int_fast32_t aufastbufv_t;
	typedef int_fast64_t aufastbufv2x_t;	/* тип для работы ресэмплера при получении среднего арифметического */

#elif CODEC1_FRAMEBITS == 32

	/* параметры входного/выходного адаптеров */
	#define WITHADAPTERCODEC1WIDTH	16		// 1 бит знак и 15 бит значащих
	#define WITHADAPTERCODEC1SHIFT	0		// количество незанятых битов справа.
	typedef int16_t aubufv_t;
	typedef int_fast16_t aufastbufv_t;
	typedef int_fast32_t aufastbufv2x_t;	/* тип для работы ресэмплера при получении среднего арифметического */

#else /* CODEC1_FRAMEBITS == 64 */

	//#error Unsupported CODEC1_FRAMEBITS value

#endif /* CODEC1_FRAMEBITS == 64 */


#if CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	/* параметры входного/выходного адаптеров */
	// IF RX
	#define WITHADAPTERIFADCWIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERIFADCSHIFT	0		// количество незанятых битов справа.
	// RTS96
	#define WITHADAPTERRTS96_WIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERRTS96_SHIFT	0		// количество незанятых битов справа.
	// RTS192
	#define WITHADAPTERRTS192_WIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERRTS192_SHIFT	0		// количество незанятых битов справа.
	// IF TX
	#define WITHADAPTERIFDACWIDTH	16		// 1 бит знак и 15 бит значащих
	#define WITHADAPTERIFDACSHIFT	0		// количество незанятых битов справа.
	typedef int32_t IFADCvalue_t;
	typedef int16_t IFDACvalue_t;

#elif CPUSTYLE_T113 && WITHDSPLOCALFIR

	/* параметры входного/выходного адаптеров */
	// IF RX
	#define WITHADAPTERIFADCWIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERIFADCSHIFT	0		// количество незанятых битов справа.
	// RTS96
	#define WITHADAPTERRTS96_WIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERRTS96_SHIFT	0		// количество незанятых битов справа.
	// RTS192
	#define WITHADAPTERRTS192_WIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERRTS192_SHIFT	0		// количество незанятых битов справа.
	// IF TX
	#define WITHADAPTERIFDACWIDTH	32		// 1 бит знак и 15 бит значащих
	#define WITHADAPTERIFDACSHIFT	0		// количество незанятых битов справа.
	typedef int32_t IFADCvalue_t;
	typedef int32_t IFDACvalue_t;

#else /* CPUSTYLE_XC7Z */

	/* параметры входного/выходного адаптеров */
	// IF RX
	#define WITHADAPTERIFADCWIDTH	28		// 1 бит знак и 27 бит значащих
	#define WITHADAPTERIFADCSHIFT	0		// количество незанятых битов справа.
	// RTS96
	#define WITHADAPTERRTS96_WIDTH	28		// 1 бит знак и 27 бит значащих
	#define WITHADAPTERRTS96_SHIFT	0		// количество незанятых битов справа.
	// RTS192
	#define WITHADAPTERRTS192_WIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERRTS192_SHIFT	0		// количество незанятых битов справа.
	// IF TX
	#define WITHADAPTERIFDACWIDTH	28		// 1 бит знак и 27 бит значащих
	#define WITHADAPTERIFDACSHIFT	0		// количество незанятых битов справа.
	typedef int32_t IFADCvalue_t;
	typedef int32_t IFDACvalue_t;

#endif /* CPUSTYLE_XC7Z */

typedef struct adapter_tag
{
	FLOAT_t inputK;
	FLOAT_t outputK;
	FLOAT_t outputKexact;
	int leftbit;
	int rightspace;
	int lshift32;	// input convrtsion
	int rshift32;
	const char * name;
} adapter_t;

typedef struct transform_tag
{
	int lshift32;
	int rshift32;
	int lshift64;
	int rshift64;
} transform_t;

FLOAT_t adpt_input(const adapter_t * adp, int32_t v);
int32_t adpt_output(const adapter_t * adp, FLOAT_t v);
int32_t adpt_outputL(const adapter_t * adp, double v);
int32_t adpt_outputexact(const adapter_t * adp, FLOAT_t v);	// точное преобразование между внешними целочисленными представлениями.
int32_t adpt_outputexactL(const adapter_t * adp, double v);	// точное преобразование между внешними целочисленными представлениями.
void adpt_initialize(adapter_t * adp, int leftbit, int rightspace, const char * name);
int32_t transform_do32(const transform_t * tfm, int32_t v); // точное преобразование между внешними целочисленными представлениями.
int64_t transform_do64(const transform_t * tfm, int64_t v); // точное преобразование между внешними целочисленными представлениями.
void transform_initialize(transform_t * tfm, const adapter_t * informat, const adapter_t * outformat);

extern adapter_t afcodecrx;	/* от микрофона */
extern adapter_t afcodectx;	/* к наушникам */

//extern adapter_t ifcodecrx;	/* канал от FPGA к процессору */
//extern adapter_t ifcodectx;	/* канал от процессора к FPGA */
extern adapter_t ifspectrumin96;	/* канал от FPGA к процессору */
extern adapter_t ifspectrumin192;	/* канал от FPGA к процессору */

extern adapter_t uac48out;	/* Аудиоданные из компютера в трансивер */
extern adapter_t uac48in;	/* Аудиоданные в компютер из трансивера */
extern adapter_t rts96in;	/* Аудиоданные (спектр) в компютер из трансивера */
extern adapter_t rts192in;	/* Аудиоданные (спектр) в компютер из трансивера */
extern adapter_t sdcardio;
extern transform_t if2rts96out;	// преобразование из выхода панорамы FPGA в формат UAB AUDIO RTS
extern transform_t if2rts192out;	// преобразование из выхода панорамы FPGA в формат UAB AUDIO RTS
extern transform_t uac48out2afcodecrx;	// преобразование из выхода UAB AUDIO48 в формат кодека

unsigned audiorec_getwidth(void);

// DUCDDC_FREQ = REFERENCE_FREQ * DDS1_CLK_MUL
#if WITHDSPEXTFIR || WITHDSPEXTDDC
	#if (CPUSTYLE_XC7Z || CPUSTYLE_XCZU) && DIRECT_122M88_X1
		// Параметры фильтров в случае использования FPGA с фильтром на квадратурных каналах
		//#define Ntap_trxi_IQ		1535	// Фильтр в FPGA (1024+512-1)
		#define Ntap_trxi_IQ		1023	// Фильтр в FPGA
		#define HARDWARE_COEFWIDTH	24		// Разрядность коэффициентов. format is S0.23
		// калибровка делается при использовании параметра WITHTXCPATHCALIBRATE
		//#define HARDWARE_DACSCALE	(0.88)	// stages=8, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора
		#define HARDWARE_DACSCALE	(0.71)	// stages=9, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

		#define FPGADECIMATION 2560uL	// должно быть кратно 256

		#define ARMI2SMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz
		#define ARMSAIMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz

	#elif (CPUSTYLE_XC7Z || CPUSTYLE_XCZU) && DIRECT_61M440_X1
		// Параметры фильтров в случае использования FPGA с фильтром на квадратурных каналах
		//#define Ntap_trxi_IQ		1535	// Фильтр в FPGA (1024+512-1)
		#define Ntap_trxi_IQ		1023	// Фильтр в FPGA
		#define HARDWARE_COEFWIDTH	24		// Разрядность коэффициентов. format is S0.23
		// калибровка делается при использовании параметра WITHTXCPATHCALIBRATE
		//#define HARDWARE_DACSCALE	(0.88)	// stages=8, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора
		#define HARDWARE_DACSCALE	(0.71)	// stages=9, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

		#define FPGADECIMATION 1280uL	// должно быть кратно 256

		#define ARMI2SMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz
		#define ARMSAIMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz

	#elif (CPUSTYLE_XC7Z || CPUSTYLE_XCZU) && DIRECT_96M_X1
		// Параметры фильтров в случае использования FPGA с фильтром на квадратурных каналах
		//#define Ntap_trxi_IQ		1535	// Фильтр в FPGA (1024+512-1)
		#define Ntap_trxi_IQ		1023	// Фильтр в FPGA
		#define HARDWARE_COEFWIDTH	24		// Разрядность коэффициентов. format is S0.23
		// калибровка делается при использовании параметра WITHTXCPATHCALIBRATE
		//#define HARDWARE_DACSCALE	(0.88)	// stages=8, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора
		#define HARDWARE_DACSCALE	(0.71)	// stages=9, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

		#define FPGADECIMATION 2000uL	// должно быть кратно 256

		#define ARMI2SMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz
		#define ARMSAIMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz

	#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU
		// Параметры фильтров в случае использования FPGA с фильтром на квадратурных каналах
		//#define Ntap_trxi_IQ		1535	// Фильтр в FPGA (1024+512-1)
		#define Ntap_trxi_IQ		1023	// Фильтр в FPGA
		#define HARDWARE_COEFWIDTH	24		// Разрядность коэффициентов. format is S0.23
		// калибровка делается при использовании параметра WITHTXCPATHCALIBRATE
		//#define HARDWARE_DACSCALE	(0.88)	// stages=8, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора
		#define HARDWARE_DACSCALE	(0.71)	// stages=9, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

		#define FPGADECIMATION 1024uL	// должно быть кратно 256

		#define ARMI2SMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz
		#define ARMSAIMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz

	#else /* CPUSTYLE_XC7Z */
		// Параметры фильтров в случае использования FPGA с фильтром на квадратурных каналах
		//#define Ntap_trxi_IQ		1535	// Фильтр в FPGA (1024+512-1)
		#define Ntap_trxi_IQ		1023	// Фильтр в FPGA
		#define HARDWARE_COEFWIDTH	24		// Разрядность коэффициентов. format is S0.23
		// калибровка делается при использовании параметра WITHTXCPATHCALIBRATE
		//#define HARDWARE_DACSCALE	(0.88)	// stages=8, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора
		#define HARDWARE_DACSCALE	(0.71)	// stages=9, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

		#define FPGADECIMATION 2560uL
		#define FPGADIVIDERATIO 5uL
		#define EXTI2S_FREQ (DUCDDC_FREQ / FPGADIVIDERATIO)
		#define EXTSAI_FREQ (DUCDDC_FREQ / FPGADIVIDERATIO)

		#define ARMI2SMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))
		#define ARMSAIMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))

	#endif /* CPUSTYLE_XC7Z */
#else
	// калибровка делается при использовании параметра WITHTXCPATHCALIBRATE
	#define HARDWARE_DACSCALE	(1)	// на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

#endif /* WITHDSPEXTFIR || WITHDSPEXTDDC */

#if WITHDSPEXTFIR && WITHI2SCLOCKFROMPIN
	#define ARMI2SMCLKX(scale)	(DUCDDC_FREQ * (uint_fast64_t) (scale) / FPGADECIMATION)
#else /* WITHDSPEXTFIR && WITHI2SCLOCKFROMPIN */
	#define ARMI2SMCLKX(scale)	(ARMSAIMCLK * (uint_fast64_t) (scale) / 256)
#endif /* WITHDSPEXTFIR && WITHI2SCLOCKFROMPIN */

#define ARMSAIRATE		(ARMSAIMCLK / 256)	// SAI sample rate (FPGA/IF CODEC side)

#if WITHDTMFPROCESSING

	#define ARMI2SRATE			((int32_t) 8000)	// I2S sample rate audio codec (human side)
	#define ARMI2SRATEX(scale)	((int32_t) (ARMI2SRATE * (scale)))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATE100		((int32_t) ARMI2SRATEX(100))

#else /* WITHDTMFPROCESSING */

	#define ARMI2SRATE			((int32_t) (ARMI2SMCLK / 256))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATEX(scale)	((int32_t) (ARMI2SMCLKX(scale)))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATE100		((int32_t) (ARMI2SRATEX(100)))

#endif /* WITHDTMFPROCESSING */

uint_fast8_t modem_getnextbit(
	uint_fast8_t suspend	// передавать модему ещё рано - не полностью завершено формирование огибающей
	 );
// обработка бита на приёме
void
modem_frames_decode(
	uint_fast8_t v
	);

uint_fast8_t getsampmlemike(FLOAT32P_t * v);			/* получить очередной оцифрованый сэмпл с микрофона */
uint_fast8_t getsampmleusb(FLOAT32P_t * v);				/* получить очередной оцифрованый сэмпл с USB UAC OUT после ресэмплигнга */

FLOAT_t local_log(FLOAT_t x);
FLOAT_t local_pow(FLOAT_t x, FLOAT_t y);

//#endif /* WITHINTEGRATEDDSP */


// Buffers interface functions
void buffers_initialize(void);

uint_fast8_t processmodem(void);

uintptr_t allocate_dmabuffer32tx(void);
uintptr_t allocate_dmabuffer32rx(void);
void release_dmabuffer32tx(uintptr_t addr);

uintptr_t allocate_dmabuffer16tx(void);
uintptr_t allocate_dmabuffer16rx(void);
void release_dmabuffer16tx(uintptr_t addr);
void release_dmabuffer16rx(uintptr_t addr);
uintptr_t processing_pipe32tx(uintptr_t addr);
uintptr_t processing_pipe32rx(uintptr_t addr);

uintptr_t getfilled_dmabufferx(uint_fast16_t * sizep);	/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferx(uintptr_t addr);	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
// WITHUSBUACIN2 specific
uintptr_t getfilled_dmabufferxrts(uint_fast16_t * sizep);	/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferxrts(uintptr_t addr);	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */

void refreshDMA_uacin(void); // Канал DMA ещё занят - оставляем в очереди, иначе получить данные через getfilled_dmabufferx

uintptr_t getfilled_dmabuffer32tx_main(void);
uintptr_t getfilled_dmabuffer32tx_sub(void);
uintptr_t getfilled_dmabuffer16txphones(void);
uintptr_t getfilled_dmabuffer16txmoni(void);

void dsp_extbuffer32rx(const IFADCvalue_t * buff);	// RX
void dsp_extbuffer32rts(const IFADCvalue_t * buff);	// RX
void dsp_extbuffer32wfm(const IFADCvalue_t * buff);	// RX

void processing_dmabuffer16rx(uintptr_t addr);	// обработать буфер после оцифровки AF ADC
//void processing_dmabuffer16rxuac(uintptr_t addr);	// обработать буфер после приёма пакета с USB AUDIO
void processing_dmabuffer32rx(uintptr_t addr);
void processing_dmabuffer32rts(uintptr_t addr);
void release_dmabuffer32rx(uintptr_t addr);
void processing_dmabuffer32rts192(uintptr_t addr);
void processing_dmabuffer32wfm(uintptr_t addr);
void buffers_resampleuacin(unsigned nsamples);
void dsp_processtx(void);	/* выборка CNT32TX семплов из источников звука и формирование потока на передатчик */

int_fast32_t buffers_dmabuffer32rxcachesize(void);
int_fast32_t buffers_dmabuffer32txcachesize(void);
int_fast32_t buffers_dmabuffer16rxcachesize(void);
int_fast32_t buffers_dmabuffer16txcachesize(void);
int_fast32_t buffers_dmabuffer192rtscachesize(void);
int_fast32_t buffers_dmabuffer32rtscachesize(void);
int_fast32_t buffers_dmabuffer96rtscachesize(void);

void savesamplewav48(int_fast32_t ch0, int_fast32_t ch1); /* to SD CARD */
void savesampleuacin48(int_fast32_t ch0, int_fast32_t ch1); /* to USB AUDIO */
unsigned takerecordbuffer(void * * dest);
void releaserecordbuffer(void * dest);
/* data to play */
unsigned savesamplesplay_user(
	const void * buff,
	unsigned length
	);
void spoolplayfile(void);
void playwavfile(const char * filename);
uint_fast8_t isplayfile(void);
void playwavstop(void);

// Обслуживание модема
size_t takemodemtxbuffer(uint8_t * * dest);	// Буферы с данными для передачи через модем
size_t takemodemtxbuffer_low(uint8_t * * dest);	// Буферы с данными для передачи через модем
uint_fast8_t statusmodemtxbuffer(void);		// есть ли буферы для передачи
size_t takemodemrxbuffer(uint8_t * * dest);	// Буферы с принятымти через модем данными
size_t takemodembuffer(uint8_t * * dest);	// Буферы для заполнения данными
size_t takemodembuffer_low(uint8_t * * dest);	// Буферы для заполнения данными
void savemodemrxbuffer(uint8_t * dest, unsigned size_t);	// Готов буфер с принятыми данными
void savemodemrxbuffer_low(uint8_t * dest, unsigned size_t);	// Готов буфер с принятыми данными
void savemodemtxbuffer(uint8_t * dest, unsigned size_t);	// Готов буфер с данными для передачи
void releasemodembuffer(uint8_t * dest);
void releasemodembuffer_low(uint8_t * dest);

void savemonistereo(FLOAT_t ch0, FLOAT_t ch1);
void savesampleout32stereo(int_fast32_t ch0, int_fast32_t ch1);
void savesampleout96stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1);
void savesampleout192stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1);

#if WITHINTEGRATEDDSP

	void dsp_addsidetone(aubufv_t * buff, const aubufv_t * monibuff, int usebuf);			// перед передачей по DMA в аудиокодек
	typedef FLOAT_t speexel_t;
	uint_fast8_t takespeexready_user(FLOAT_t * * dest);
	void releasespeexbuffer_user(FLOAT_t * t);

	/* загрузка коэффициентов FIR фильтра в FPGA */
	void board_fpga_fir_initialize(void);
	void board_reload_fir(uint_fast8_t ifir, const int32_t * const k, const FLOAT_t * const kf, unsigned Ntap, unsigned CWidth); /* Выдача рассчитанных параметров фильтра в FPGA (симметричные) */

#endif /* WITHINTEGRATEDDSP */


uintptr_t allocate_dmabuffer192rts(void);

FLOAT_t get_lout(void); // тестовые функции
FLOAT_t get_rout(void);

void prog_dsplreg(void);
void prog_fltlreg(void);
void prog_codec1reg(void);
void prog_codecreg_update(void);		// услолвное обновление регистров аудио кодека
void prog_dsplreg_update(void);
void prog_fltlreg_update(void);
void board_dsp1regchanged(void);
void prog_codec1reg(void);

void board_set_trxpath(uint_fast8_t v);	/* Тракт, к которому относятся все последующие вызовы. При перередаяе используется индекс 0 */
void board_set_mikemute(uint_fast8_t v);	/* отключить микрофонный усилитель */
void board_set_mik1level(uint_fast16_t v);	/* усиление микрофонного усилителя */
void board_set_agcrate(uint_fast8_t v);	/* на n децибел изменения входного сигнала 1 дБ выходного. UINT8_MAX - "плоская" АРУ */
void board_set_agc_t0(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_t1(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_t2(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_t4(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_thung(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_scale(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_squelch(uint_fast8_t v);	/* уровень открывания шумоподавителя */
void board_set_notch_freq(uint_fast16_t n);	/* частота NOTCH фильтра */
void board_set_notch_width(uint_fast16_t n);	/* полоса NOTCH фильтра */
void board_set_notch_mode(uint_fast8_t n);	/* включение NOTCH фильтра */
void board_set_cwedgetime(uint_fast8_t n);	/* Время нарастания/спада огибающей телеграфа при передаче - в 1 мс */
void board_set_sidetonelevel(uint_fast8_t n);	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
void board_set_moniflag(uint_fast8_t n);	/* разрешение самопрослушивания */
void board_set_cwssbtx(uint_fast8_t v);	/* разрешение передачи телеграфа как тона в режиме SSB */
void board_set_subtonelevel(uint_fast8_t n);	/* Уровень сигнала CTCSS в процентах - 0%..100% */
void board_set_amdepth(uint_fast8_t n);		/* Глубина модуляции в АМ - 0..100% */
void board_set_swaprts(uint_fast8_t v);	/* если используется конвертор на Rafael Micro R820T - требуется инверсия спектра */
void board_set_lo6(int_fast32_t f);
void board_set_fullbw6(int_fast16_t f);	/* Установка частоты среза фильтров ПЧ в алгоритме Уивера - параметр полная полоса пропускания */

void board_set_aflowcutrx(int_fast16_t v);		/* Нижняя частота среза фильтра НЧ */
void board_set_afhighcutrx(int_fast16_t v);	/* Верхняя частота среза фильтра НЧ */
void board_set_aflowcuttx(int_fast16_t v);		/* Нижняя частота среза фильтра НЧ */
void board_set_afhighcuttx(int_fast16_t v);	/* Верхняя частота среза фильтра НЧ */
void board_set_nfmdeviation100(uint_fast8_t v);	/* Девиация в NFM (сотни герц) */

void board_set_afgain(uint_fast16_t v);	// Параметр для регулировки уровня на выходе аудио-ЦАП
void board_set_ifgain(uint_fast16_t v);	// Параметр для регулировки усиления ПЧ/ВЧ
void board_set_dspmode(uint_fast8_t v);	// Параметр для установки режима работы приёмника A/передатчика A
void board_set_lineinput(uint_fast8_t n);	// Включение line input вместо микрофона
void board_set_lineamp(uint_fast16_t v);	// Параметр для регулировки уровня на входе аудио-ЦАП при работе с LINE IN
void board_set_txaudio(uint_fast8_t v);	// Альтернативные источники сигнала при передаче
void board_set_mikeboost20db(uint_fast8_t n);	// Включение предусилителя за микрофоном
void board_set_afmute(uint_fast8_t n);	// Отключение звука
void board_set_mikeequal(uint_fast8_t n);	// включение обработки сигнала с микрофона (эффекты, эквалайзер, ...)
void board_set_mikeequalparams(const uint_fast8_t * p);	// Эквалайзер 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
void board_set_mikeagc(uint_fast8_t n);		/* Включение программной АРУ перед модулятором */
void board_set_mikeagcgain(uint_fast8_t v);	/* Максимальное усидение АРУ микрофона */
void board_set_afresponcerx(int_fast8_t v);	/* изменение тембра звука в канале приемника */
void board_set_afresponcetx(int_fast8_t v);	/* изменение тембра звука в канале передатчика */
void board_set_mikehclip(uint_fast8_t gmikehclip);	/* Ограничитель */
void board_set_reverb(uint_fast8_t greverb, uint_fast8_t greverbdelay, uint_fast8_t greverbloss); /* ревербератор */
void board_set_compressor(uint_fast8_t attack, uint_fast8_t release, uint_fast8_t hold, uint_fast8_t gain, uint_fast8_t threshold);


void board_set_uacplayer(uint_fast8_t v);	/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
void board_set_datatx(uint_fast8_t v);	/* автоматическое изменение источника при появлении звука со стороны компьютера */
void board_set_usb_ft8cn(uint_fast8_t v);	/* совместимость VID/PID для работы с программой FT8CN */

void dsp_initialize(void);

#if WITHINTEGRATEDDSP

	// Нормирование уровня сигнала к шкале
	// возвращает значения от 0 до ymax включительно
	// 0 - минимальный сигнал, ymax - максимальный
	int dsp_mag2y(FLOAT_t mag, int ymax, int_fast16_t topdb, int_fast16_t bottomdb);

#endif /* WITHINTEGRATEDDSP */

int_fast32_t dsp_get_ifreq(void);		/* Получить значение входной ПЧ для обработки DSP */
int_fast32_t dsp_get_sampleraterx(void);	/* Получить значение частоты выборок выходного потока DSP */
int_fast32_t dsp_get_sampleraterxscaled(uint_fast8_t scale);	/* Получить значение частоты выборок выходного потока DSP */
int_fast32_t dsp_get_sampleratetx(void);	/* Получить значение частоты выборок входного потока DSP */
int_fast32_t dsp_get_samplerate100(void);	/* Получить значение частоты выборок выходного потока DSP */

int_fast32_t dsp_get_samplerateuacin_audio48(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_RTS96(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_RTS192(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_rts(void);			// RTS samplerate
int_fast32_t dsp_get_samplerateuacout(void);			// UAC OUT samplerate

uint_fast8_t dsp_getsmeter(uint_fast8_t * tracemax, uint_fast8_t lower, uint_fast8_t upper, uint_fast8_t clean);	/* получить значение от АЦП s-метра */
uint_fast16_t dsp_getsmeter10(uint_fast16_t * tracemax, uint_fast16_t lower, uint_fast16_t upper, uint_fast8_t clean);	/* получить значение от АЦП s-метра */
uint_fast8_t dsp_getvox(uint_fast8_t fullscale);	/* получить значение от детектора VOX */
uint_fast8_t dsp_getavox(uint_fast8_t fullscale);	/* получить значение от детектора Anti-VOX */
uint_fast8_t dsp_getfreqdelta10(int_fast32_t * p, uint_fast8_t pathi);	/* Получить значение отклонения частоты с точностью 0.1 герца */
uint_fast8_t dsp_getmikeadcoverflow(void); /* получения признака переполнения АЦП микрофонного тракта */

void dsp_speed_diagnostics(void);	/* DSP speed test */
void beginstamp(void);
void endstamp(void);
void beginstamp2(void);
void endstamp2(void);
void beginstamp3(void);
void endstamp3(void);

void buffers_diagnostics(void);
void dtmftest(void);
void dsp_recalceq_coeffs_rx_AUDIO(uint_fast8_t pathi, FLOAT_t * dCoeff);	// calculate full array of coefficients

void modem_initialze(void);
uint_fast8_t modem_get_ptt(void);

/* Интерфейс к AF кодеку */
typedef struct codec1if_tag
{
	uint_fast8_t (* clocksneed)(void);	/* требуется ли подача тактирования для инициадизации кодека */
	void (* stop) (void);
	void (* initialize) (void (* io_control)(uint_fast8_t on), uint_fast8_t master);
	void (* setvolume) (uint_fast16_t gain, uint_fast8_t mute, uint_fast8_t mutespk);	/* Установка громкости на наушники */
	void (* setlineinput) (uint_fast8_t linein, uint_fast8_t mikeboost20db, uint_fast16_t mikegain, uint_fast16_t linegain);	/* Выбор LINE IN как источника для АЦП вместо микрофона */
	void (* setprocparams) (uint_fast8_t procenable, const uint_fast8_t * gains);	/* параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
	const char * label;									/* Название кодека (всегда последний элемент в структуре) */
} codec1if_t;

const codec1if_t * board_getaudiocodecif(void);		// получить интерфейс управления кодеком в сторону оператора

/* Интерфейс к IF кодеку */
typedef struct codec2if_tag
{
	uint_fast8_t (* clocksneed)(void);	/* требуется ли подача тактирования для инициадизации кодека */
	void (* initialize) (void (* io_control)(uint_fast8_t on), uint_fast8_t master);
	const char * label;
} codec2if_t;

const codec2if_t * board_getfpgacodecif(void);		// получить интерфейс управления кодеком для получения данных о радиосигнале

/* +++ UAC OUT data save - использование данных от компьютера. */
void uacout_buffer_initialize(void);
void uacout_buffer_start(void);
void uacout_buffer_stop(void);
void uacout_buffer_save(const uint8_t * buff, uint_fast16_t size, uint_fast8_t ichannels, uint_fast8_t usbsz);

void buffers_set_uacinalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC IN interface */
void buffers_set_uacoutalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC OUT interface */
void buffers_set_uacinrtsalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC IN interface */
uint_fast8_t buffers_get_uacoutalt(void);

/* Получение пары (левый и правый) сжмплов для воспроизведения через аудиовыход трансивера.
 * Возврат 0, если нет ничего для воспроизведения.
 */
uint_fast8_t takewavsample(FLOAT32P_t * rv, uint_fast8_t suspend);

typedef struct subscribefloat_tag
{
	LIST_ENTRY item;
	void * ctx;
	void (* cb)(void * ctx, FLOAT_t ch0, FLOAT_t ch1);
} subscribefloat_t;

typedef struct subscribefint_tag
{
	LIST_ENTRY item;
	void * ctx;
	void (* cb)(void * ctx, int_fast32_t ch0, int_fast32_t ch1);
} subscribeint32_t;

typedef struct deliverylist_tag
{
	LIST_ENTRY head;
	IRQLSPINLOCK_t listlock;
} deliverylist_t;

/* irqlv - на каком уровне IRQL_t будут вызываться фуекции подписавшихся */
void deliverylist_initialize(deliverylist_t * list, IRQL_t irqlv);

void deliveryfloat(deliverylist_t * head, FLOAT_t ch0, FLOAT_t ch1);
void deliveryfloat_buffer(deliverylist_t * head, const FLOAT_t * ch0, const FLOAT_t * ch1, unsigned n);
void deliveryint(deliverylist_t * head, int_fast32_t ch0, int_fast32_t ch1);

void subscribefloat(deliverylist_t * head, subscribefloat_t * target, void * ctx, void (* pfn)(void * ctx, FLOAT_t ch0, FLOAT_t ch1));
void unsubscribefloat(deliverylist_t * head, subscribefloat_t * target);
void subscribeint32(deliverylist_t * head, subscribeint32_t * target, void * ctx, void (* pfn)(void * ctx, int_fast32_t ch0, int_fast32_t ch1));
void unsubscribeint32(deliverylist_t * head, subscribeint32_t * target);

extern deliverylist_t rtstargetsint;	// выход обработчика DMA приема от FPGA
extern deliverylist_t speexoutfloat;	// выход speex и фильтра
extern deliverylist_t afdemodoutfloat;	// выход приемника


#if __STDC__ && ! CPUSTYLE_ATMEGA

#define MAXFLOAT	3.40282347e+38F

#if ! LINUX_SUBSYSTEM
	#define M_LN2		_M_LN2
#endif

#define M_E		2.7182818284590452354
#define M_LOG2E		1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN10		2.30258509299404568402
#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#define M_PI_4		0.78539816339744830962
#define M_1_PI		0.31830988618379067154
#define M_2_PI		0.63661977236758134308
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2		1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440

#define M_TWOPI         (M_PI * 2.0)
#define M_3PI_4		2.3561944901923448370E0
#define M_SQRTPI        1.77245385090551602792981
#define M_LN2LO         1.9082149292705877000E-10
#define M_LN2HI         6.9314718036912381649E-1
#define M_SQRT3	1.73205080756887719000
#define M_IVLN10        0.43429448190325182765 /* 1 / log(10) */
#define M_LOG2_E        _M_LN2
#define M_INVLN2        1.4426950408889633870E0  /* 1 / log(2) */

#endif /* __STDC__ && ! CPUSTYLE_ATMEGA */

/* установка параметров приемника, передаваемых чрез I2S канал в FPGA */
uint_fast32_t dspfpga_get_nco1(void);
uint_fast32_t dspfpga_get_nco2(void);
uint_fast32_t dspfpga_get_nco3(void);
uint_fast32_t dspfpga_get_nco4(void);
uint_fast32_t dspfpga_get_ncorts(void);

extern volatile phase_t mirror_nco1;
extern volatile phase_t mirror_nco2;
extern volatile phase_t mirror_nco3;
extern volatile phase_t mirror_nco4;
extern volatile phase_t mirror_ncorts;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AUDIO_H_INCLUDED */
