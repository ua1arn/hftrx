/*
 * buffers.h
 */
/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef INC_BUFFERS_H_
#define INC_BUFFERS_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define MODEMBUFFERSIZE8	1024

#define FIRBUFSIZE 1024	/* это не порядок фильтра, просто размер буфера для передачи данных к user mode обработчику */

//#define AUDIORECBUFFSIZE16 (16384)	// размер данных должен быть не меньше размера кластера на SD карте
#define AUDIORECBUFFSIZE16 (1024)	// размер данных должен быть не меньше размера кластера на SD карте

/* Применённая система диспетчеризации требует,
   чтобы во всех буферах помещалось не меньше сэмплов,
   чем в DMABUFFSIZE32RX
 */
#if WITHDSPEXTDDC

	#if CPUSTYLE_R7S721

		// buff data layout: I main/I sub/Q main/Q sub
		#define DMABUFFSTEP32RX		8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
		// buff data layout: I_T0/Q_T0/I_T1/Q_T1
		//#define DMABUFFSTEP32RTS96	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

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
			#define DMABUF32RXRTS0I	2		// RTS0, I	// previous - oldest
			#define DMABUF32RXRTS0Q	6		// RTS0, Q	// previous
			#define DMABUF32RXRTS1I	3		// RTS1, I	// current	- newest
			#define DMABUF32RXRTS1Q	7		// RTS1, Q	// current
		#endif /* WITHRTS96 */


		// Slot S0, S4: Oldest sample (T-3)
		// Slot S1, S5: Old sample (T-2)
		// Slot S2, S6: Old sample (T-1)
		// Slot S3, S7: Newest sample (T-0)
	#if WITHWFM
		#define DMABUF32RXWFM0I	0		// WFM OLDEST
		#define DMABUF32RXWFM0Q	4		// WFM
		#define DMABUF32RXWFM1I	1		// WFM
		#define DMABUF32RXWFM1Q	5		// WFM
		#define DMABUF32RXWFM2I	2		// WFM
		#define DMABUF32RXWFM2Q	6		// WFM
		#define DMABUF32RXWFM3I	3		// WFM NEWEST
		#define DMABUF32RXWFM3Q	7		// WFM
	#endif /* WITHWFM */

	#elif CPUSTYLE_RK356X &&  WITHFPGAIF_FRAMEBITS == 512

		#define DMABUFFSTEP32RX	16		// Каждому сэмплу соответствует шестнадцать чисел в DMA буфере

		#define DMABUF32RX0I	0		// RX0, I
		#define DMABUF32RX0Q	1		// RX0, Q
		#define DMABUF32RX1I	2		// RX1, I
		#define DMABUF32RX1Q	3		// RX1, Q

		#if WITHRTS96
			#define DMABUF32RXRTS0I	4		// RTS(t-1), I	// previous - oldest
			#define DMABUF32RXRTS0Q	5		// RTS(t-1), Q	// previous
			#define DMABUF32RXRTS1I	6		// RTS(t), I	// current	- newest
			#define DMABUF32RXRTS1Q	7		// RTS(t), Q	// current
		#endif /* WITHRTS96 */

		#define DMABUFF32RX_CODEC1_LEFT 	8		/* индекс сэмпла левого канала от кодека (через PIPE) */
		#define	DMABUFF32RX_CODEC1_RIGHT 	9		/* индекс сэмпла правого канала от кодека (через PIPE)  */

		#define DMABUFF32RX_ADCTEST_C0 14			// состояние выходов RF ADC
		#define DMABUFF32RX_ADCTEST_C1 15

		// *************************************

		#define DMABUFFSTEP32TX	16		// Каждому сэмплу соответствует шестнадцать чисел в DMA буфере

		#define DMABUF32TXI	0		// TX, I
		#define DMABUF32TXQ	1		// TX, Q

		#define DMABUF32TX_NCO1		2		// NCO RX A
		#define DMABUF32TX_NCO2		3		// NCO RX B
		#define DMABUF32TX_NCORTS	4		// NCO RTS

		/* звук идет по PIPE */
		#define DMABUFF32TX_CODEC1_LEFT 	6		/* индекс сэмпла левого канала к кодеку (через PIPE) */
		#define	DMABUFF32TX_CODEC1_RIGHT 	7		/* индекс сэмпла правого канала к кодеку (через PIPE)  */


	#elif CPUSTYLE_XC7Z || CPUSTYLE_RK356X

		#if WITHFPGAIF_FRAMEBITS == 64

			#define DMABUFFSTEP32RX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			//#define DMABUFFSTEP32RTS96	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUF32RX0I	0		// RX0, I
			#define DMABUF32RX0Q	1		// RX0, Q
			#define DMABUFFSTEP32TX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUF32TXI		0		// TX, I
			#define DMABUF32TXQ		1		// TX, Q

		#elif WITHFPGAIF_FRAMEBITS == 256
			// buff data layout: I main/I sub/Q main/Q sub
			#define DMABUFFSTEP32RX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
			// buff data layout: I_T0/Q_T0/I_T1/Q_T1
			//#define DMABUFFSTEP32RTS96	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

			#define DMABUF32RX0I	0		// RX0, I
			#define DMABUF32RX0Q	1		// RX0, Q
			#define DMABUF32RX1I	2		// RX1, I
			#define DMABUF32RX1Q	3		// RX1, Q

		#if WITHRTS96
			#define DMABUF32RXRTS0I	4		// RTS0, I	// previous - oldest
			#define DMABUF32RXRTS0Q	5		// RTS0, Q	// previous
			#define DMABUF32RXRTS1I	6		// RTS1, I	// current	- newest
			#define DMABUF32RXRTS1Q	7		// RTS1, Q	// current
		#endif /* WITHRTS96 */

		#if WITHWFM
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
		#endif /* WITHWFM */
			#define DMABUFFSTEP32TX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUF32TXI	0		// TX, I
			#define DMABUF32TXQ	1		// TX, Q

		#endif /* WITHFPGAIF_FRAMEBITS */

	#elif CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

		#if WITHFPGAIF_FRAMEBITS == 64

			#define DMABUFFSTEP32RX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			//#define DMABUFFSTEP32RTS96	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUF32RX0I	0		// RX0, I
			#define DMABUF32RX0Q	1		// RX0, Q
			#define DMABUFFSTEP32TX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
			#define DMABUF32TXI		0		// TX, I
			#define DMABUF32TXQ		1		// TX, Q

		#elif WITHFPGAIF_FRAMEBITS == 256
			// buff data layout: I main/I sub/Q main/Q sub
			#define DMABUFFSTEP32RX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
			// buff data layout: I_T0/Q_T0/I_T1/Q_T1
			//#define DMABUFFSTEP32RTS96	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

			#define DMABUF32RX0I	0		// RX0, I
			#define DMABUF32RX1I	1		// RX1, I
			#define DMABUF32RX0Q	4		// RX0, Q
			#define DMABUF32RX1Q	5		// RX1, Q

			#if WITHRTS96
				#define DMABUF32RXRTS0I	2		// RTS0, I	// previous - oldest
				#define DMABUF32RXRTS0Q	6		// RTS0, Q	// previous
				#define DMABUF32RXRTS1I	3		// RTS1, I	// current	- newest
				#define DMABUF32RXRTS1Q	7		// RTS1, Q	// current
			#endif /* WITHRTS96 */

			#if WITHWFM
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
			#endif /* WITHWFM */
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

	#elif CPUSTYLE_ALLWINNER
		// Allwinner t113-s3, Allwinner D1s (F133): I2S/PCM have non-sequential numbering of samples in DMA buffer
		// ws=0: even samples, ws=1: odd samples
		#if WITHFPGAIF_FRAMEBITS == 64 && WITHFPGAIF_USE_I2S_DIN12 == 1 // 64b only for one DDC channel, use x2 for mem allocation because : WITHFPGAIF_USE_I2S_DIN12

			// @! TODO for DIN 1,2 - 128b 96kfs (two DDC channel from FPGA)
			// Gena help me :)

			// buff data layout: I main/I sub/Q main/Q sub
			#define DMABUFFSTEP32RX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
			// buff data layout: I_T0/Q_T0/I_T1/Q_T1
			//#define DMABUFFSTEP32RTS96	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

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
				#define DMABUF32RXRTS0I	4		// RTS0, I	// previous - oldest
				#define DMABUF32RXRTS0Q	5		// RTS0, Q	// previous
				#define DMABUF32RXRTS1I	6		// RTS1, I	// current	- newest
				#define DMABUF32RXRTS1Q	7		// RTS1, Q	// current
			#endif /* WITHRTS96 */

			// Allwinner t113-s3: I2S/PCM have non-sequential numbering of samples in DMA buffer
			// ws=0: even samples, ws=1: odd samples
			#if WITHWFM
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
			#endif /* WITHWFM */
		#elif WITHFPGAIF_FRAMEBITS == 256

			// buff data layout: I main/I sub/Q main/Q sub
			#define DMABUFFSTEP32RX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
			// buff data layout: I_T0/Q_T0/I_T1/Q_T1
			//#define DMABUFFSTEP32RTS96	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

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
				#define DMABUF32RXRTS0I	4		// RTS0, I	// previous - oldest
				#define DMABUF32RXRTS0Q	5		// RTS0, Q	// previous
				#define DMABUF32RXRTS1I	6		// RTS1, I	// current	- newest
				#define DMABUF32RXRTS1Q	7		// RTS1, Q	// current
			#endif /* WITHRTS96 */

			// Allwinner t113-s3: I2S/PCM have non-sequential numbering of samples in DMA buffer
			// ws=0: even samples, ws=1: odd samples

			#if WITHWFM
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
			#endif /* WITHWFM */

		#elif WITHFPGAIF_FRAMEBITS == 512
			// buff data layout: I main/I sub/Q main/Q sub
			#define DMABUFFSTEP32RX	16		// Каждому сэмплу соответствует шестнадцать чисел в DMA буфере
			// buff data layout: I_T0/Q_T0/I_T1/Q_T1
			//#define DMABUFFSTEP32RTS96	DMABUFFSTEP32RX		// Каждому сэмплу соответствует восемь чисел в DMA буфере

			#define DMABUF32RX0I	8		// RX0, I
			#define DMABUF32RX0Q	9		// RX0, Q
			#define DMABUF32RX1I	10		// RX1, I
			#define DMABUF32RX1Q	11		// RX1, Q

			#define DMABUFF32RX_CODEC1_LEFT 	12		/* индекс сэмпла левого канала от кодека (через PIPE) */
			#define	DMABUFF32RX_CODEC1_RIGHT 	13		/* индекс сэмпла правого канала от кодека (через PIPE)  */

			#define DMABUFF32RX_ADCTEST_C0 14			// состояние выходов RF ADC
			#define DMABUFF32RX_ADCTEST_C1 15

			// ws=0: 00 02 04 06
			// ws=1: 01 03 05 07

		#if WITHRTS96
			#define DMABUF32RXRTS0I	4		// RTS(t-1), I	// previous - oldest
			#define DMABUF32RXRTS0Q	5		// RTS(t-1), Q	// previous
			#define DMABUF32RXRTS1I	6		// RTS(t), I	// current	- newest
			#define DMABUF32RXRTS1Q	7		// RTS(t), Q	// current
		#endif /* WITHRTS96 */
		#if WITHRTS192
			#define DMABUF32RXRTS0I	0		// RTS(t-3), I	// previous - oldest
			#define DMABUF32RXRTS0Q	1		// RTS(t-3), Q	// previous
			#define DMABUF32RXRTS1I	2		// RTS(t-2), I	// previous
			#define DMABUF32RXRTS1Q	3		// RTS(t-2), Q	// previous
			#define DMABUF32RXRTS2I	4		// RTS(t-1), I	// previous
			#define DMABUF32RXRTS2Q	5		// RTS(t-1), Q	// previous
			#define DMABUF32RXRTS3I	6		// RTS(t), I	// current	- newest
			#define DMABUF32RXRTS3Q	7		// RTS(t), Q	// current
		#endif /* WITHRTS192 */


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
		//#define DMABUFFSTEP32RTS96	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

		#define DMABUF32RX0I	0		// RX0, I
		#define DMABUF32RX1I	1		// RX1, I
		#define DMABUF32RX0Q	4		// RX0, Q
		#define DMABUF32RX1Q	5		// RX1, Q

		#define DMABUFFSTEP32TX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
		#define DMABUF32TXI	0		// TX, I
		#define DMABUF32TXQ	4		// TX, Q

		#if WITHRTS96
			#define DMABUF32RXRTS0I	2		// RTS0, I	// previous - oldest
			#define DMABUF32RXRTS0Q	6		// RTS0, Q	// previous
			#define DMABUF32RXRTS1I	3		// RTS1, I	// current	- newest
			#define DMABUF32RXRTS1Q	7		// RTS1, Q	// current
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
	//#define DMABUFFSTEP32RTS96	(WITHFPGARTS_FRAMEBITS / 32) //2		// 2 - каждому сэмплу соответствует два числа в DMA буфере
	#define DMABUF32RXI	0		// RX0, I
	#define DMABUF32RXQ	1		// RX0, Q

	#define DMABUFFSTEP32TX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
	#define DMABUF32TXI	0		// TX, I
	#define DMABUF32TXQ	1		// TX, Q

#endif /* WITHDSPEXTDDC */

#if 1
	// stub
	// параметры дополнительного канала передачи в FPGA
	#define DMABUFFSTEP32TXSUB	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
#endif

#if WITHCODEC1_WHBLOCK_DUPLEX_MASTER && (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	/* встороенный в процессор кодек */

	//	ix = 0: R5 & R11 (lineinL - pin 96 & fminL pin 94)
	//	ix = 1: R6 & R10 (lineinR - pin 95 & fminR pin 93)
	//	ix = 2: micin3N & micin3P

	#define DMABUFFSTEP16RX		3		/* 3 - каждому сэмплу от AUDIO CODEC соответствует три числа в DMA буфере */

	#define DMABUFF16RX_LEFT 	0		/* индекс сэмпла lineinL/fminL */
	#define DMABUFF16RX_RIGHT 	1		/* индекс сэмпла lineinR/fminR */
	#define DMABUFF16RX_MIKE 	2		/* micin3 ADC */

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

#if 1
	#define DMABUFFSTEP16RX8K		2		/* 2 - каждому сэмплу при получении от AUDIO CODEC соответствует два числа в DMA буфере */
	#define DMABUFFSTEP16TX8K		2		/* 2 - каждому сэмплу при передаче в AUDIO CODEC соответствует два числа в DMA буфере */
#endif

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

#define CDC_NOTIFY_PERIOD_FS	255
#define CDC_NOTIFY_PERIOD_HS	0x0B // (1024 microframes)

#define HSINTERVAL_1MS 4    // endpoint descriptor parameters - для обеспечения 1 ms периода
#define FSINTERVAL_1MS 1

#define HSINTERVAL_8MS 7    // endpoint descriptor parameters - для обеспечения 10 ms периода
#define FSINTERVAL_8MS 8

#define HSINTERVAL_32MS 9    // endpoint descriptor parameters - для обеспечения 32 ms периода
#define FSINTERVAL_32MS 32

#define HSINTERVAL_256MS 12    // endpoint descriptor parameters - для обеспечения 255 ms периода (interrupt endpoint for CDC)
#define FSINTERVAL_255MS 255

#if WITHUSBDEV_HSDESC
	/* вариант передачи с периодом 0.5 мс */
	#define OUTSAMPLES_AUDIO48	24 /* количество сэмплов за SOF в UAC OUT */
	#define UAC_GROUPING_DMAC 1	/* Во сколько раз реже происходит прерывание по буферу обмена - увеличение размера буфера DMA */

	#define HSINTERVAL_AUDIO48 3	// 1 - 125 uS, 2 - 250 uS, 3 - 500 uS 4 - 1 mS
	#define HSINTERVAL_RTS96 3
	#define HSINTERVAL_RTS192 3

	#define FSINTERVAL_AUDIO48 1
	#define FSINTERVAL_RTS96 1
	#define FSINTERVAL_RTS192 1

#elif WITHUSBDEV_HSDESC
/* вариант передачи с периодом 1 мс */
	#define OUTSAMPLES_AUDIO48	48 /* количество сэмплов за SOF в UAC OUT */
	#define UAC_GROUPING_DMAC 1 /* Во сколько раз реже происходит прерывание по буферу обмена - увеличение размера буфера DMA */

	#define HSINTERVAL_AUDIO48 4	// 1 - 125 uS, 2 - 250 uS, 3 - 500 uS 4 - 1 mS
	#define HSINTERVAL_RTS96 4
	#define HSINTERVAL_RTS192 4

	#define FSINTERVAL_AUDIO48 1
	#define FSINTERVAL_RTS96 1
	#define FSINTERVAL_RTS192 1

#else /* WITHUSBDEV_HSDESC */
	#define OUTSAMPLES_AUDIO48	48 /* количество сэмплов за милисекунду в UAC OUT */
	#define UAC_GROUPING_DMAC 1 /* Во сколько раз реже происходит прерывание по буферу обмена - увеличение размера буфера DMA */

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

#define UAC_DATASIZE(n, ss, ch) ((n) * (ss) * (ch))

// Получить размер для программирования FIFO endpoint - с учетом разбивки на микрофреймы
// See also encodeMaxPacketSize function
#define EPUF_Adj(n) ((n) <= 1024 ? (n) : (n) < 2048 ? ((n) + 1) / 2 : ((n) + 2) / 3)

/* xxx_DMAC - Размры буферов ендпоинт в байтах */
// Добавление единицы к размеру для UACOUT и UACIN требуется для нормальной работы UAC2 устройства.
#if WITHUAC2

	#define UACOUT_AUDIO48_DATASIZE		UAC_DATASIZE(OUTSAMPLES_AUDIO48 + 1, UACOUT_AUDIO48_SAMPLEBYTES, UACOUT_FMT_CHANNELS_AUDIO48)
	#define UACOUT_AUDIO48_DATASIZE_DMAC EPUF_Adj(UAC_DATASIZE(OUTSAMPLES_AUDIO48 + 0, UACOUT_AUDIO48_SAMPLEBYTES, UACOUT_FMT_CHANNELS_AUDIO48))

	#define UACIN_AUDIO48_DATASIZE 		UAC_DATASIZE(OUTSAMPLES_AUDIO48 + 1, UACIN_AUDIO48_SAMPLEBYTES, UACIN_FMT_CHANNELS_AUDIO48)
	#define UACIN_AUDIO48_DATASIZE_DMAC EPUF_Adj(UAC_DATASIZE(OUTSAMPLES_AUDIO48 + 0, UACIN_AUDIO48_SAMPLEBYTES, UACIN_FMT_CHANNELS_AUDIO48))

	#define UACIN_RTS96_DATASIZE 		UAC_DATASIZE(OUTSAMPLES_AUDIO48 * 2 + 1, UACIN_RTS96_SAMPLEBYTES, UACIN_FMT_CHANNELS_RTS96)
	#define UACIN_RTS96_DATASIZE_DMAC 	EPUF_Adj(UAC_DATASIZE(OUTSAMPLES_AUDIO48 * 2 + 0, UACIN_RTS96_SAMPLEBYTES, UACIN_FMT_CHANNELS_RTS96))

	#define UACIN_RTS192_DATASIZE 		UAC_DATASIZE(OUTSAMPLES_AUDIO48 * 4 + 1, UACIN_RTS192_SAMPLEBYTES, UACIN_FMT_CHANNELS_RTS192)
	#define UACIN_RTS192_DATASIZE_DMAC 	EPUF_Adj(UAC_DATASIZE(OUTSAMPLES_AUDIO48 * 4 + 0, UACIN_RTS192_SAMPLEBYTES, UACIN_FMT_CHANNELS_RTS192))

#else /* WITHUAC2 */

	// Добавление единицы к размеру для UACOUT и UACIN требуется для нормальной работы UAC2 устройства.
	#define UACOUT_AUDIO48_DATASIZE		UAC_DATASIZE(OUTSAMPLES_AUDIO48 + 0, UACOUT_AUDIO48_SAMPLEBYTES, UACOUT_FMT_CHANNELS_AUDIO48)
	#define UACOUT_AUDIO48_DATASIZE_DMAC EPUF_Adj(UAC_DATASIZE(OUTSAMPLES_AUDIO48 + 0, UACOUT_AUDIO48_SAMPLEBYTES, UACOUT_FMT_CHANNELS_AUDIO48))

	#define UACIN_AUDIO48_DATASIZE 		UAC_DATASIZE(OUTSAMPLES_AUDIO48 + 0, UACIN_AUDIO48_SAMPLEBYTES, UACIN_FMT_CHANNELS_AUDIO48)
	#define UACIN_AUDIO48_DATASIZE_DMAC EPUF_Adj(UAC_DATASIZE(OUTSAMPLES_AUDIO48 + 0, UACIN_AUDIO48_SAMPLEBYTES, UACIN_FMT_CHANNELS_AUDIO48))

	#define UACIN_RTS96_DATASIZE 		UAC_DATASIZE(OUTSAMPLES_AUDIO48 * 2 + 0, UACIN_RTS96_SAMPLEBYTES, UACIN_FMT_CHANNELS_RTS96)
	#define UACIN_RTS96_DATASIZE_DMAC 	EPUF_Adj(UAC_DATASIZE(OUTSAMPLES_AUDIO48 * 2 + 0, UACIN_RTS96_SAMPLEBYTES, UACIN_FMT_CHANNELS_RTS96))

	#define UACIN_RTS192_DATASIZE 		UAC_DATASIZE(OUTSAMPLES_AUDIO48 * 4 + 0, UACIN_RTS192_SAMPLEBYTES, UACIN_FMT_CHANNELS_RTS192)
	#define UACIN_RTS192_DATASIZE_DMAC 	EPUF_Adj(UAC_DATASIZE(OUTSAMPLES_AUDIO48 * 4 + 0, UACIN_RTS192_SAMPLEBYTES, UACIN_FMT_CHANNELS_RTS192))

#endif /* WITHUAC2 */



#if WITHUSBDMSC
	#if WITHUSBDEV_HSDESC && CPUSTYLE_ALLWINNER && 0
		#define MSC_DATA_MAX_PACKET_SIZE_HS 512
		#define MSC_DATA_MAX_PACKET_SIZE_FS 64
		#define MSC_DATA_MAX_PACKET_SIZE			MSC_DATA_MAX_PACKET_SIZE_HS
	#else /* WITHUSBDEV_HSDESC */
		#define MSC_DATA_MAX_PACKET_SIZE_HS 64
		#define MSC_DATA_MAX_PACKET_SIZE_FS 64
		#define MSC_DATA_MAX_PACKET_SIZE			MSC_DATA_MAX_PACKET_SIZE_FS
	#endif /* WITHUSBDEV_HSDESC */
#endif /* WITHUSBDMSC */

#if WITHUSBCDCACM
#if ! WITHUSBCDCACM_NOINT
	#define VIRTUAL_COM_PORT_NOTIFY_SIZE 			10
#endif /* ! WITHUSBCDCACM_NOINT */
	#if WITHUSBDEV_HSDESC && CPUSTYLE_ALLWINNER && 0
		#define VIRTUAL_COM_PORT_IN_DATA_SIZE			USB_OTG_HS_MAX_PACKET_SIZE
		#define VIRTUAL_COM_PORT_OUT_DATA_SIZE			USB_OTG_HS_MAX_PACKET_SIZE
	#else /* WITHUSBDEV_HSDESC */
		#define VIRTUAL_COM_PORT_IN_DATA_SIZE			64U
		#define VIRTUAL_COM_PORT_OUT_DATA_SIZE			64U
	#endif /* WITHUSBDEV_HSDESC */
#endif /* WITHUSBCDCACM */

#if WITHUSBDMTP
	#define MTP_HS_BINTERVAL 0x10U
	#define MTP_FS_BINTERVAL 0x10U
	#define MTP_DATA_MAX_PACKET_SIZE                                 64U         /* Endpoint IN & OUT Packet size */
	#define MTP_CMD_PACKET_SIZE                                         8U          /* Control Endpoint Packet size */
#endif /* WITHUSBDMTP */

#if WITHUSBHID
	/* наличие одного из определений выбирает нужный тип HID DEVICE */
	//#define HIDMOUSE_NOTIFY_DATA_SIZE 4
	#define HIDKEYBOARD_NOTIFY_DATA_SIZE 64
#endif /* WITHUSBHID */

#if WITHUSBCDCEEM
	#if WITHUSBDEV_HSDESC
		#define USBD_CDCEEM_BUFSIZE			USB_OTG_HS_MAX_PACKET_SIZE
	#else /* WITHUSBDEV_HSDESC */
		#define USBD_CDCEEM_BUFSIZE			USB_OTG_FS_MAX_PACKET_SIZE
	#endif /* WITHUSBDEV_HSDESC */
#endif /* WITHUSBCDCEEM */

#if WITHUSBCDCECM
	#define USBD_CDCECM_NOTIFY_SIZE	64	// interrupt data - AN00131:-USB-CDC-ECM-Class-for-Ethernet-over-USB(2.0.2rc1).pdf says 64
	#if WITHUSBDEV_HSDESC
		#define USBD_CDCECM_IN_BUFSIZE			USB_OTG_HS_MAX_PACKET_SIZE
		#define USBD_CDCECM_OUT_BUFSIZE			USB_OTG_HS_MAX_PACKET_SIZE
	#else /* WITHUSBDEV_HSDESC */
		#define USBD_CDCECM_IN_BUFSIZE			USB_OTG_FS_MAX_PACKET_SIZE
		#define USBD_CDCECM_OUT_BUFSIZE			USB_OTG_FS_MAX_PACKET_SIZE
	#endif /* WITHUSBDEV_HSDESC */
#endif /* WITHUSBCDCECM */

#if WITHUSBCDCNCM
	#define USBD_CDCNCM_NOTIFY_SIZE	8
	#if WITHUSBDEV_HSDESC
		#define USBD_CDCNCM_IN_BUFSIZE			USB_OTG_HS_MAX_PACKET_SIZE
		#define USBD_CDCNCM_OUT_BUFSIZE			USB_OTG_HS_MAX_PACKET_SIZE
	#else /* WITHUSBDEV_HSDESC */
		#define USBD_CDCNCM_IN_BUFSIZE			USB_OTG_FS_MAX_PACKET_SIZE
		#define USBD_CDCNCM_OUT_BUFSIZE			USB_OTG_FS_MAX_PACKET_SIZE
	#endif /* WITHUSBDEV_HSDESC */
#endif /* WITHUSBCDCNCM */

#if WITHUSBRNDIS
	#define USBD_RNDIS_NOTIFY_SIZE	8	// interrupt data
	#if WITHUSBDEV_HSDESC
		#define USBD_RNDIS_IN_BUFSIZE			USB_OTG_HS_MAX_PACKET_SIZE
		#define USBD_RNDIS_OUT_BUFSIZE			USB_OTG_HS_MAX_PACKET_SIZE
	#else /* WITHUSBDEV_HSDESC */
		#define USBD_RNDIS_IN_BUFSIZE			USB_OTG_FS_MAX_PACKET_SIZE
		#define USBD_RNDIS_OUT_BUFSIZE			USB_OTG_FS_MAX_PACKET_SIZE
	#endif /* WITHUSBDEV_HSDESC */
#endif /* WITHUSBRNDIS */

#ifndef DMABUFCLUSTER
/* если приоритет прерываний USB не выше чем у аудиобработки - она должна длиться не более 1 мс (WITHRTS192 - 0.5 ms) */
#define DMABUFCLUSTER	33	// Прерывания по приему от IF CODEC или FPGA RX должны происходить не реже 1 раз в милисекунду (чтобы USB работать могло) */
#endif /* DMABUFCLUSTER */
#ifndef DMABUFSCALE
#define DMABUFSCALE		2	// внутрений параметр, указывает, на сколько реже будут происходить прерывания по обмену буфрами от остальны каналов по отношению к приему от FPGA
#endif /* DMABUFSCALE */

#define DMABUFFSIZE16RX	(DMABUFCLUSTER * DMABUFFSTEP16RX)		/* AF CODEC ADC */
#define DMABUFFSIZE16RX8K	(DMABUFCLUSTER * DMABUFFSTEP16RX8K)		/* AF CODEC ADC */
#define DMABUFFSIZE32RX (DMABUFCLUSTER * DMABUFFSTEP32RX)		/* FPGA RX or IF CODEC RX */
//#define DMABUFFSIZE32RTS96 (DMABUFCLUSTER * DMABUFFSTEP32RTS96)		/* FPGA RX or IF CODEC RX */

#define DMABUFFSIZE16TX	(DMABUFCLUSTER * DMABUFFSTEP16TX * DMABUFSCALE)		/* AF CODEC DAC */
#define DMABUFFSIZE16TX8K	(DMABUFCLUSTER * DMABUFFSTEP16TX8K * DMABUFSCALE)		/* AF CODEC DAC */
#define DMABUFFSIZE32TX (DMABUFCLUSTER * DMABUFFSTEP32TX * DMABUFSCALE)	/* FPGA TX or IF CODEC TX	*/
#define DMABUFFSIZE32TXSUB (DMABUFCLUSTER * DMABUFFSTEP32TXSUB * DMABUFSCALE)	/* Additional channel FPGA TX or IF CODEC TX	*/

#define DMABUFFSTEP32RTS192 2
#define DMABUFFSIZE32RTS192	(DMABUFCLUSTER * DMABUFFSTEP32RTS192)		/* RTS192 data from I2S */


#define DMABUFFSTEP16MONI 2	// самоконтроль - левый и правый
#define DMABUFFSIZE16MONI	(48 * DMABUFFSTEP16MONI)		/* самоконтроль - дискретность задержки 1 мс */

#define DMABUFFSIZEHDMI48TX	(DMABUFCLUSTER * DMABUFFSTEPHDMI48TX * DMABUFSCALE)		/* HDMI OUT */
#define DMABUFFSTEPHDMI48TX 2

// Buffers interface functions
void buffers_initialize(void);
void dsp_processtx(unsigned nsamples);	/* выборка CNT32TX семплов из источников звука и формирование потока на передатчик */

uintptr_t getfilled_dmabufferuacinX(uint_fast16_t * sizep);	/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferuacinX(uintptr_t addr);	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
// WITHUSBUACIN2 specific
uintptr_t getfilled_dmabufferuacinrtsX(uint_fast16_t * sizep);	/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferuacinX(uintptr_t addr);	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */

//+++++++++++++++++++++
/* FPGA to CPU */
uintptr_t allocate_dmabuffer32rx(void); /* take free buffer FPGA to CPU */
uintptr_t getfilled_dmabuffer32rx(void); /* take from queue FPGA to CPU */
void release_dmabuffer32rx(uintptr_t addr);  /* release FPGA to CPU */
void save_dmabuffer32rx(uintptr_t addr); /* save to queue FPGA to CPU */
int_fast32_t cachesize_dmabuffer32rx(void); /* parameter for cache manipulation functions FPGA to CPU */
int_fast32_t datasize_dmabuffer32rx(void); /* parameter for DMA FPGA to CPU */

/* CPU to FPGA */
uintptr_t allocate_dmabuffer32tx(void); /* take free buffer CPU to FPGA */
uintptr_t getfilled_dmabuffer32tx(void); /* take from queue CPU to FPGA */
void release_dmabuffer32tx(uintptr_t addr);  /* release CPU to FPGA */
void save_dmabuffer32tx(uintptr_t addr); /* save to queue CPU to FPGA */
int_fast32_t cachesize_dmabuffer32tx(void); /* parameter for cache manipulation functions CPU to FPGA */
int_fast32_t datasize_dmabuffer32tx(void); /* parameter for DMA CPU to FPGA */

/* CPU to FPGA (additional channel) */
uintptr_t allocate_dmabuffer32tx_sub(void); /* take free buffer CPU to FPGA (additional channel) */
uintptr_t getfilled_dmabuffer32tx_sub(void); /* take from queue CPU to FPGA (additional channel) */
void release_dmabuffer32tx_sub(uintptr_t addr);  /* release CPU to FPGA (additional channel) */
void save_dmabuffer32tx_sub(uintptr_t addr); /* save to queue CPU to FPGA (additional channel) */
int_fast32_t cachesize_dmabuffer32tx_sub(void); /* parameter for cache manipulation functions CPU to FPGA (additional channel) */
int_fast32_t datasize_dmabuffer32tx_sub(void); /* parameter for DMA CPU to FPGA (additional channel) */

/* CODEC to CPU */
uintptr_t allocate_dmabuffer16rx(void); /* take free buffer CODEC to CPU */
uintptr_t getfilled_dmabuffer16rx(void); /* take from queue CODEC to CPU */
void release_dmabuffer16rx(uintptr_t addr);  /* release CODEC to CPU */
void save_dmabuffer16rx(uintptr_t addr); /* save to queue CODEC to CPU */
int_fast32_t cachesize_dmabuffer16rx(void); /* parameter for cache manipulation functions CODEC to CPU */
int_fast32_t datasize_dmabuffer16rx(void); /* parameter for DMA CODEC to CPU */

/* CPU to CODEC */
uintptr_t allocate_dmabuffer16tx(void); /* take free buffer CPU to CODEC */
uintptr_t getfilled_dmabuffer16tx(void); /* take from queue CPU to CODEC */
void release_dmabuffer16tx(uintptr_t addr);  /* release CPU to CODEC */
void save_dmabuffer16tx(uintptr_t addr); /* save to queue CPU to CODEC */
int_fast32_t cachesize_dmabuffer16tx(void); /* parameter for cache manipulation functions CPU to CODEC */
int_fast32_t datasize_dmabuffer16tx(void); /* parameter for DMA CPU to CODEC */

/* CPU to HDMI */
uintptr_t allocate_dmabufferhdmi48tx(void); /* take free buffer CPU to HDMI */
uintptr_t getfilled_dmabufferhdmi48tx(void); /* take from queue CPU to HDMI */
void release_dmabufferhdmi48tx(uintptr_t addr);  /* release CPU to HDMI */
void save_dmabufferhdmi48tx(uintptr_t addr); /* save to queue CPU to HDMI */
int_fast32_t cachesize_dmabufferhdmi48tx(void); /* parameter for cache manipulation functions CPU to HDMI */
int_fast32_t datasize_dmabufferhdmi48tx(void); /* parameter for DMA CPU to HDMI */

/* CODEC to CPU, sample rate 8000 */
uintptr_t allocate_dmabuffer16rx8k(void); /* take free buffer CODEC to CPU, sample rate 8000 */
uintptr_t getfilled_dmabuffer16rx8k(void); /* take from queue CODEC to CPU, sample rate 8000 */
void release_dmabuffer16rx8k(uintptr_t addr);  /* release CODEC to CPU, sample rate 8000 */
void save_dmabuffer16rx8k(uintptr_t addr); /* save to queue CODEC to CPU, sample rate 8000 */
int_fast32_t cachesize_dmabuffer16rx8k(void); /* parameter for cache manipulation functions CODEC to CPU, sample rate 8000 */
int_fast32_t datasize_dmabuffer16rx8k(void); /* parameter for DMA CODEC to CPU, sample rate 8000 */

/* CPU to CODEC, sample rate 8000 */
uintptr_t allocate_dmabuffer16tx8k(void); /* take free buffer CPU to CODEC, sample rate 8000 */
uintptr_t getfilled_dmabuffer16tx8k(void); /* take from queue CPU to CODEC, sample rate 8000 */
void release_dmabuffer16tx8k(uintptr_t addr);  /* release CPU to CODEC, sample rate 8000 */
void save_dmabuffer16tx8k(uintptr_t addr); /* save to queue CPU to CODEC, sample rate 8000 */
int_fast32_t cachesize_dmabuffer16tx8k(void); /* parameter for cache manipulation functions CPU to CODEC, sample rate 8000 */
int_fast32_t datasize_dmabuffer16tx8k(void); /* parameter for DMA CPU to CODEC, sample rate 8000 */

/* FPGA to CPU */
uintptr_t allocate_dmabuffer32rts192(void); /* take free buffer FPGA to CPU */
uintptr_t getfilled_dmabuffer32rts192(void); /* take from queue FPGA to CPU */
void release_dmabuffer32rts192(uintptr_t addr);  /* release FPGA to CPU */
void save_dmabuffer32rts192(uintptr_t addr); /* save to queue FPGA to CPU */
int_fast32_t cachesize_dmabuffer32rts192(void); /* parameter for cache manipulation functions FPGA to CPU */
int_fast32_t datasize_dmabuffer32rts192(void); /* parameter for DMA FPGA to CPU */

/* usb audio48 to host */
uintptr_t allocate_dmabufferuacin48(void); /* take free buffer usb audio48 to host */
uintptr_t getfilled_dmabufferuacin48(void); /* take from queue usb audio48 to host */
void release_dmabufferuacin48(uintptr_t addr);  /* release usb audio48 to host */
void save_dmabufferuacin48(uintptr_t addr); /* save to queue usb audio48 to host */
int_fast32_t cachesize_dmabufferuacin48(void); /* parameter for cache manipulation functions usb audio48 to host */
int_fast32_t datasize_dmabufferuacin48(void); /* parameter for DMA usb audio48 to host */

/* usb rts96 to host */
uintptr_t allocate_dmabufferuacinrts96(void); /* take free buffer usb rts96 to host */
uintptr_t getfilled_dmabufferuacinrts96(void); /* take from queue usb rts96 to host */
void release_dmabufferuacinrts96(uintptr_t addr);  /* release usb rts96 to host */
void save_dmabufferuacinrts96(uintptr_t addr); /* save to queue usb rts96 to host */
int_fast32_t cachesize_dmabufferuacinrts96(void); /* parameter for cache manipulation functions usb rts96 to host */
int_fast32_t datasize_dmabufferuacinrts96(void); /* parameter for DMA usb rts96 to host */

/* usb rts192 to host */
uintptr_t allocate_dmabufferuacinrts192(void); /* take free buffer usb rts192 to host */
uintptr_t getfilled_dmabufferuacinrts192(void); /* take from queue usb rts192 to host */
void release_dmabufferuacinrts192(uintptr_t addr);  /* release usb rts192 to host */
void save_dmabufferuacinrts192(uintptr_t addr); /* save to queue usb rts192 to host */
int_fast32_t cachesize_dmabufferuacinrts192(void); /* parameter for cache manipulation functions usb rts192 to host */
int_fast32_t datasize_dmabufferuacinrts192(void); /* parameter for DMA usb rts192 to host */

/* usb audio48 from host */
uintptr_t allocate_dmabufferuacout48(void); /* take free buffer usb audio48 from host */
uintptr_t getfilled_dmabufferuacout48(void); /* take from queue usb audio48 from host */
void release_dmabufferuacout48(uintptr_t addr);  /* release usb audio48 from host */
void save_dmabufferuacout48(uintptr_t addr); /* save to queue usb audio48 from host */
int_fast32_t cachesize_dmabufferuacout48(void); /* parameter for cache manipulation functions usb audio48 from host */
int_fast32_t datasize_dmabufferuacout48(void); /* parameter for DMA usb audio48 from host */

/* BT audio to radio, sample rate 8000 */
uintptr_t allocate_dmabufferbtout8k(void); /* take free buffer BT audio to radio, sample rate 8000 */
uintptr_t getfilled_dmabufferbtout8k(void); /* take from queue BT audio to radio, sample rate 8000 */
void release_dmabufferbtout8k(uintptr_t addr);  /* release BT audio to radio, sample rate 8000 */
void save_dmabufferbtout8k(uintptr_t addr); /* save to queue BT audio to radio, sample rate 8000 */
int_fast32_t cachesize_dmabufferbtout8k(void); /* parameter for cache manipulation functions BT audio to radio, sample rate 8000 */
int_fast32_t datasize_dmabufferbtout8k(void); /* parameter for DMA BT audio to radio, sample rate 8000 */

/* BT audio to radio, sample rate 16000 */
uintptr_t allocate_dmabufferbtout16k(void); /* take free buffer BT audio to radio, sample rate 16000 */
uintptr_t getfilled_dmabufferbtout16k(void); /* take from queue BT audio to radio, sample rate 16000 */
void release_dmabufferbtout16k(uintptr_t addr);  /* release BT audio to radio, sample rate 16000 */
void save_dmabufferbtout16k(uintptr_t addr); /* save to queue BT audio to radio, sample rate 16000 */
int_fast32_t cachesize_dmabufferbtout16k(void); /* parameter for cache manipulation functions BT audio to radio, sample rate 16000 */
int_fast32_t datasize_dmabufferbtout16k(void); /* parameter for DMA BT audio to radio, sample rate 16000 */

/* BT audio to radio, sample rate 32000 */
uintptr_t allocate_dmabufferbtout32k(void); /* take free buffer BT audio to radio, sample rate 32000 */
uintptr_t getfilled_dmabufferbtout32k(void); /* take from queue BT audio to radio, sample rate 32000 */
void release_dmabufferbtout32k(uintptr_t addr);  /* release BT audio to radio, sample rate 32000 */
void save_dmabufferbtout32k(uintptr_t addr); /* save to queue BT audio to radio, sample rate 32000 */
int_fast32_t cachesize_dmabufferbtout32k(void); /* parameter for cache manipulation functions BT audio to radio, sample rate 32000 */
int_fast32_t datasize_dmabufferbtout32k(void); /* parameter for DMA BT audio to radio, sample rate 32000 */

/* BT audio to radio, sample rate 44100 */
uintptr_t allocate_dmabufferbtout44p1k(void); /* take free buffer BT audio to radio, sample rate 44100 */
uintptr_t getfilled_dmabufferbtout44p1k(void); /* take from queue BT audio to radio, sample rate 44100 */
void release_dmabufferbtout44p1k(uintptr_t addr);  /* release BT audio to radio, sample rate 44100 */
void save_dmabufferbtout44p1k(uintptr_t addr); /* save to queue BT audio to radio, sample rate 44100 */
int_fast32_t cachesize_dmabufferbtout44p1k(void); /* parameter for cache manipulation functions BT audio to radio, sample rate 44100 */
int_fast32_t datasize_dmabufferbtout44p1k(void); /* parameter for DMA BT audio to radio, sample rate 44100 */

/* BT audio from radio, sample rate 8000 */
uintptr_t allocate_dmabufferbtin8k(void); /* take free buffer BT audio from radio, sample rate 8000 */
uintptr_t getfilled_dmabufferbtin8k(void); /* take from queue BT audio from radio, sample rate 8000 */
void release_dmabufferbtin8k(uintptr_t addr);  /* release BT audio from radio, sample rate 8000 */
void save_dmabufferbtin8k(uintptr_t addr); /* save to queue BT audio from radio, sample rate 8000 */
int_fast32_t cachesize_dmabufferbtin8k(void); /* parameter for cache manipulation functions BT audio from radio, sample rate 8000 */
int_fast32_t datasize_dmabufferbtin8k(void); /* parameter for DMA BT audio from radio, sample rate 8000 */

/* BT audio from radio, sample rate 16000 */
uintptr_t allocate_dmabufferbtin16k(void); /* take free buffer BT audio from radio, sample rate 16000 */
uintptr_t getfilled_dmabufferbtin16k(void); /* take from queue BT audio from radio, sample rate 16000 */
void release_dmabufferbtin16k(uintptr_t addr);  /* release BT audio from radio, sample rate 16000 */
void save_dmabufferbtin16k(uintptr_t addr); /* save to queue BT audio from radio, sample rate 16000 */
int_fast32_t cachesize_dmabufferbtin16k(void); /* parameter for cache manipulation functions BT audio from radio, sample rate 16000 */
int_fast32_t datasize_dmabufferbtin16k(void); /* parameter for DMA BT audio from radio, sample rate 16000 */

/* BT audio from radio, sample rate 32000 */
uintptr_t allocate_dmabufferbtin32k(void); /* take free buffer BT audio from radio, sample rate 32000 */
uintptr_t getfilled_dmabufferbtin32k(void); /* take from queue BT audio from radio, sample rate 32000 */
void release_dmabufferbtin32k(uintptr_t addr);  /* release BT audio from radio, sample rate 32000 */
void save_dmabufferbtin32k(uintptr_t addr); /* save to queue BT audio from radio, sample rate 32000 */
int_fast32_t cachesize_dmabufferbtin32k(void); /* parameter for cache manipulation functions BT audio from radio, sample rate 32000 */
int_fast32_t datasize_dmabufferbtin32k(void); /* parameter for DMA BT audio from radio, sample rate 32000 */

/* BT audio from radio, sample rate 44100 */
uintptr_t allocate_dmabufferbtin44p1k(void); /* take free buffer BT audio from radio, sample rate 44100 */
uintptr_t getfilled_dmabufferbtin44p1k(void); /* take from queue BT audio from radio, sample rate 44100 */
void release_dmabufferbtin44p1k(uintptr_t addr);  /* release BT audio from radio, sample rate 44100 */
void save_dmabufferbtin44p1k(uintptr_t addr); /* save to queue BT audio from radio, sample rate 44100 */
int_fast32_t cachesize_dmabufferbtin44p1k(void); /* parameter for cache manipulation functions BT audio from radio, sample rate 44100 */
int_fast32_t datasize_dmabufferbtin44p1k(void); /* parameter for DMA BT audio from radio, sample rate 44100 */

/* Frame buffer for display 0 */
uintptr_t allocate_dmabuffercolmain0fb(void); /* take free buffer Frame buffer for display 0 */
uintptr_t getfilled_dmabuffercolmain0fb(void); /* take from queue Frame buffer for display 0 */
void release_dmabuffercolmain0fb(uintptr_t addr);  /* release Frame buffer for display 0 */
void save_dmabuffercolmain0fb(uintptr_t addr); /* save to queue Frame buffer for display 0 */
int_fast32_t cachesize_dmabuffercolmain0fb(void); /* parameter for cache manipulation functions Frame buffer for display 0 */
int_fast32_t datasize_dmabuffercolmain0fb(void); /* parameter for DMA Frame buffer for display 0 */

/* Frame buffer for display 1 (HDMI) */
uintptr_t allocate_dmabuffercolmain1fb(void); /* take free buffer Frame buffer for display 1 (HDMI) */
uintptr_t getfilled_dmabuffercolmain1fb(void); /* take from queue Frame buffer for display 1 (HDMI) */
void release_dmabuffercolmain1fb(uintptr_t addr);  /* release Frame buffer for display 1 (HDMI) */
void save_dmabuffercolmain1fb(uintptr_t addr); /* save to queue Frame buffer for display 1 (HDMI) */
int_fast32_t cachesize_dmabuffercolmain1fb(void); /* parameter for cache manipulation functions Frame buffer for display 1 (HDMI) */
int_fast32_t datasize_dmabuffercolmain1fb(void); /* parameter for DMA Frame buffer for display 1 (HDMI) */

/* Ethernet0 buffers */
uintptr_t allocate_dmabuffereth0io(void); /* take free buffer Ethernet0 buffers */
uintptr_t getfilled_dmabuffereth0io(void); /* take from queue Ethernet0 buffers */
void release_dmabuffereth0io(uintptr_t addr);  /* release Ethernet0 buffers */
void save_dmabuffereth0io(uintptr_t addr); /* save to queue Ethernet0 buffers */
int_fast32_t cachesize_dmabuffereth0io(void); /* parameter for cache manipulation functions Ethernet0 buffers */
int_fast32_t datasize_dmabuffereth0io(void); /* parameter for DMA Ethernet0 buffers */

//-----------------------

/* audio samples for recording */
unsigned takerecordbuffer(void * * dest);
void releaserecordbuffer(void * dest);
void saverecordbuffer(void * dest);
/* data to play */
unsigned savesamplesplay(
	const void * buff,
	unsigned length
	);
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

void savesampleout96stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1);
void savesampleout192stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1);

void buffers_set_uacinalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC IN interface */
void buffers_set_uacoutalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC OUT interface */
void buffers_set_uacinrtsalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC IN interface */
uint_fast8_t buffers_get_uacoutactive(void);

void DMAC_USB_RX_initialize_UACOUT48(uint32_t ep);
void DMAC_USB_TX_initialize_UACIN48(uint32_t ep);
void DMAC_USB_TX_initialize_UACINRTS96(uint32_t ep);
void DMAC_USB_TX_initialize_UACINRTS192(uint32_t ep);


#define CATPCOUNTSIZE (13)
#define MSGBUFFERSIZE8 64//(9 + CATPCOUNTSIZE)

typedef enum messagetypes
{
	MSGT_EMPTY,
	MSGT_CAT,	// 12 bytes as parameter
	//
	MSGT_count
} messagetypes_t;

messagetypes_t takemsgready(uint8_t * * dest);	// Буферы с принятымти от обработчиков прерываний сообщениями
void releasemsgbuffer(uint8_t * dest);	// Освобождение обработанного буфера сообщения
size_t takemsgbufferfree(uint8_t * * dest);	// Буфер для формирования сообщения
void placesemsgbuffer(messagetypes_t type, uint8_t * dest);	// поместить сообщение в очередь к исполнению

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* INC_BUFFERS_H_ */
