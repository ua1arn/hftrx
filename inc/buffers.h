/*
 * buffers.h
 *
 *  Created on: Aug 25, 2023
 *      Author: Gena
 */

#ifndef INC_BUFFERS_H_
#define INC_BUFFERS_H_

#define MODEMBUFFERSIZE8	1024

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

// Конфигурация потоков в Input Terminal Descriptor
// bNrChannels в 4.3.2.1 Input Terminal Descriptor образуется подсчетом битов в данном поле
// Может быть использовано AUDIO_CHANNEL_M
#define UACIN_CONFIG_IN48 			(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)
#define UACIN_CONFIG_INRTS 			(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)
#define UACIN_CONFIG_IN48_INRTS 	(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)
#define UACOUT_CONFIG_OUT48 		(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)

// количество каналов в дескрипторах формата потока
#define UACIN_FMT_CHANNELS_AUDIO48			2	// приём стерео
#define UACIN_FMT_CHANNELS_AUDIO48_RTS		2	// при совмещении аудио и I/Q всегда стерео
#define UACIN_FMT_CHANNELS_RTS96 2				// I/Q всегда стерео
#define UACIN_FMT_CHANNELS_RTS192 2				// I/Q всегда стерео

#if WITHUABUACOUTAUDIO48MONO
	// количество каналов в дескрипторах формата потока
	#define UACOUT_FMT_CHANNELS_AUDIO48	1	// передача - моно
#else /* WITHUABUACOUTAUDIO48MONO */
	// количество каналов в дескрипторах формата потока
	#define UACOUT_FMT_CHANNELS_AUDIO48	2	// передача - стерео
#endif /* WITHUABUACOUTAUDIO48MONO */

/*
    For full-/high-speed isochronous endpoints, this value
    must be in the range from 1 to 16. The bInterval value
    is used as the exponent for a 2^(bInterval-1) value; e.g.,
    a bInterval of 4 means a period of 8 (2^(4-1))."

  */

#define HSINTERVAL_1MS 4    // endpoint descriptor parameters - для обеспечения 1 ms периода
#define FSINTERVAL_1MS 1

#define HSINTERVAL_8MS 7    // endpoint descriptor parameters - для обеспечения 10 ms периода
#define FSINTERVAL_8MS 8

#define HSINTERVAL_32MS 9    // endpoint descriptor parameters - для обеспечения 32 ms периода
#define FSINTERVAL_32MS 32

#define HSINTERVAL_256MS 12    // endpoint descriptor parameters - для обеспечения 255 ms периода (interrupt endpoint for CDC)
#define FSINTERVAL_255MS 255

#if WITHUSBDEV_HSDESC //&& CPUSTYLE_ALLWINNER
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

#define UAC_n1c1g1 (1)	// 1 байт в ячейку 1
#define UAC_n2c1g1 (1)	// 2 байта в ячейку 1
#define UAC_n3c1g1 (1)	// 3 байт в ячейку 1
#define UAC_n4c1g1 (1)	// 4 байт в ячейку 1

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
#if 1
	#define UAC_DATASIZE(n, ss, ch) ((n) * (ss) * (ch))
#elif CPUSTYLE_ALLWINNER || CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7
	#define UAC_DATASIZE(n, ss, ch) (UAC_DATASIZEgr((n), UAC_ng(ss, ch, 4), (ss) * (ch)))
#elif CPUSTYLE_R7S721
	#define UAC_DATASIZE(n, ss, ch) (UAC_DATASIZEgr((n), UAC_ng(ss, ch, 8), (ss) * (ch)))
#else
	#define UAC_DATASIZE(n, ss, ch) (UAC_DATASIZEgr((n), UAC_ng(ss, ch, 4), (ss) * (ch)))
#endif

/* Размры буферов ендпоинт в байтах */
// Добавление единицы для UACOUT требуется для нормальной работы UAC2 устройства.
#define UACOUT_AUDIO48_DATASIZE_DMAC	UAC_DATASIZE(OUTSAMPLES_AUDIO48 + 0, UACOUT_AUDIO48_SAMPLEBYTES, UACOUT_FMT_CHANNELS_AUDIO48)
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

uintptr_t getfilled_dmabufferuacinX(uint_fast16_t * sizep);	/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferuacinX(uintptr_t addr);	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
// WITHUSBUACIN2 specific
uintptr_t getfilled_dmabufferxrts(uint_fast16_t * sizep);	/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferuacinrtsX(uintptr_t addr);	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */

void refreshDMA_uacin48(void); // Канал DMA ещё занят - оставляем в очереди, иначе получить данные через getfilled_dmabufferuacinX
void refreshDMA_uacinrts96(void); // Канал DMA ещё занят - оставляем в очереди, иначе получить данные через getfilled_dmabufferuacinX
void refreshDMA_uacinrts192(void); // Канал DMA ещё занят - оставляем в очереди, иначе получить данные через getfilled_dmabufferuacinX

uintptr_t getfilled_dmabuffer32tx_main(void);
uintptr_t getfilled_dmabuffer32tx_sub(void);
uintptr_t getfilled_dmabuffer16txphones(void);
uintptr_t getfilled_dmabuffer16txmoni(void);

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

void savesampleout32stereo(int_fast32_t ch0, int_fast32_t ch1);
void savesampleout96stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1);
void savesampleout192stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1);

uintptr_t allocate_dmabufferuacin192rts(void);
void release_dmabufferuacin192rts(uintptr_t addr);
uintptr_t getfilled_dmabufferuacin192rts(void);

uintptr_t allocate_dmabufferuacinrts96(void);
void release_dmabufferuacinrts96(uintptr_t addr);
uintptr_t getfilled_dmabufferuacinrts96(void);

uintptr_t allocate_dmabufferuacout48(void);
void release_dmabufferuacout48(uintptr_t addr);

uintptr_t allocate_dmabufferuacin48(void);
uintptr_t getfilled_dmabufferuacin48(void);
void release_dmabufferuacin48(uintptr_t addr);

/* +++ UAC OUT data save - использование данных от компьютера. */
void uacout_buffer_initialize(void);
void uacout_buffer_start(void);
void uacout_buffer_stop(void);
void uacout_buffer_save(const uint8_t * buff, uint_fast16_t size, uint_fast8_t ichannels, uint_fast8_t usbsz);

void buffers_set_uacinalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC IN interface */
void buffers_set_uacoutalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC OUT interface */
void buffers_set_uacinrtsalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC IN interface */
uint_fast8_t buffers_get_uacoutalt(void);

void DMAC_USB_RX_initialize_UACOUT48(uint32_t ep);
void DMAC_USB_TX_initialize_UACIN48(uint32_t ep);
void DMAC_USB_TX_initialize_UACINRTS(uint32_t ep);


#endif /* INC_BUFFERS_H_ */
