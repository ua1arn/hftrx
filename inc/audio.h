/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

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

		#define DMABUFFSTEP16RX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
		#define DMABUFF16RX_MIKE 	0		/* индекс сэмпла левого канала */

		#define DMABUFFSTEP16TX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
		#define DMABUFF16TX_LEFT 	0		/* индекс сэмпла левого канала */
		#define DMABUFF16TX_RIGHT 	1		/* индекс сэмпла правого канала */

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

		#define DMABUFFSTEP16RX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
		#define DMABUFF16RX_MIKE 	0		/* индекс сэмпла левого канала */

		#define DMABUFFSTEP16TX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
		#define DMABUFF16TX_LEFT 	0		/* индекс сэмпла левого канала */
		#define DMABUFF16TX_RIGHT 	1		/* индекс сэмпла правого канала */

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
	#endif

		#define DMABUFFSTEP16RX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
		#define DMABUFF16RX_MIKE 	0		/* индекс сэмпла левого канала */

		#define DMABUFFSTEP16TX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
		#define DMABUFF16TX_LEFT 	0		/* индекс сэмпла левого канала */
		#define DMABUFF16TX_RIGHT 	1		/* индекс сэмпла правого канала */

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

		/* CODEC */
		#define DMABUFFSTEP16TX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
		#define DMABUFF16TX_LEFT 	0		/* индекс сэмпла левого канала */
		#define DMABUFF16TX_RIGHT 	1		/* индекс сэмпла правого канала */

		/* CODEC */
		#define DMABUFFSTEP16RX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
		#define DMABUFF16RX_MIKE 	0		/* индекс сэмпла левого канала */


	#elif CPUSTYLE_T113 || CPUSTYLE_F133
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

		#define DMABUFFSTEP16RX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
		#define DMABUFF16RX_MIKE 	0		/* индекс сэмпла левого канала */

		#define DMABUFFSTEP16TX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
		#define DMABUFF16TX_LEFT 	0		/* индекс сэмпла левого канала */
		#define DMABUFF16TX_RIGHT 	1		/* индекс сэмпла правого канала */

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

		#define DMABUFFSTEP16RX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
		#define DMABUFF16RX_MIKE 	0		/* индекс сэмпла левого канала */

		#define DMABUFFSTEP16TX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
		#define DMABUFF16TX_LEFT 	0		/* индекс сэмпла левого канала */
		#define DMABUFF16TX_RIGHT 	1		/* индекс сэмпла правого канала */

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

	#define DMABUFFSTEP16RX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
	#define DMABUFF16RX_MIKE 	0		/* индекс сэмпла канала микрофона */

	#define DMABUFFSTEP16TX		2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере
	#define DMABUFF16TX_LEFT 	0		/* индекс сэмпла левого канала */
	#define DMABUFF16TX_RIGHT 	1		/* индекс сэмпла правого канала */

#endif /* WITHDSPEXTDDC */


// Требования по кратности размера буфера для передачи по USB DMA
#if CPUSTYLE_R7S721
	#define HARDWARE_RTSDMABYTES	4
#else /* CPUSTYLE_R7S721 */
	#define HARDWARE_RTSDMABYTES	1
#endif /* CPUSTYLE_R7S721 */

#if WITHUAC2
	#define WITHUSENOFU_IN48 			1	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
	#define WITHUSENOFU_INRTS 			1	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
	#define WITHUSENOFU_IN48_INRTS 		1	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
	#define WITHUSENOFU_OUT48 			1	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
#else /* WITHUAC2 */
	#define WITHUSENOFU_IN48 			0	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
	#define WITHUSENOFU_INRTS 			0	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
	#define WITHUSENOFU_IN48_INRTS 		0	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
	#define WITHUSENOFU_OUT48 			0	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
#endif /* WITHUAC2 */

// Конфигурация потоков в Input Terminal Descriptor
// bNrChannels в 4.3.2.1 Input Terminal Descriptor образуется подсчетом битов в данном поле
// Может быть использовано AUDIO_CHANNEL_M
#define UACIN_CONFIG_IN48 			(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)
#define UACIN_CONFIG_INRTS 			(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)
#define UACIN_CONFIG_IN48_INRTS 	(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)
#define UACOUT_CONFIG_OUT48 		(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)

// IN/OUT path topology parameters
#define UAC2_IN_bNrChannels 2	//UAC_count_channels(wChannelConfig); 1: Only master channel controls, 3: master, left and right
#define UAC2_OUT_bNrChannels 2	//UAC_count_channels(wChannelConfig); 1: Only master channel controls, 3: master, left and right

// количество каналов в дескрипторах формата потока
#define UACIN_FMT_CHANNELS_AUDIO48			2
#define UACIN_FMT_CHANNELS_RTS				2	// I/Q всегда стерео
#define UACIN_FMT_CHANNELS_AUDIO48_RTS		2	// при совмещении аудио и I/Q всегда стерео

#if WITHUABUACOUTAUDIO48MONO
	// количество каналов в дескрипторах формата потока
	#define UACOUT_AUDIO48_FMT_CHANNELS	1
#else /* WITHUABUACOUTAUDIO48MONO */
	// количество каналов в дескрипторах формата потока
	#define UACOUT_AUDIO48_FMT_CHANNELS	2
#endif /* WITHUABUACOUTAUDIO48MONO */

// коррекция размера с учетом требуемого выравнивания
#define DMAHWEPADJUST(sz, granulation) (((sz) + ((granulation) - 1)) / (granulation) * (granulation))

/*
	For full-/high-speed isochronous endpoints, this value
	must be in the range from 1 to 16. The bInterval value
	is used as the exponent for a 2^(bInterval-1) value; e.g.,
	a bInterval of 4 means a period of 8 (2^(4-1))."

  */
/* константы. С запасом чтобы работало и при тактовой 125 МГц на FPGA при децимации 2560 = 48.828125 kHz sample rate */
//#define MSOUTSAMPLES	49 /* количество сэмплов за милисекунду в UAC OUT */
// без запаса - только для 48000
#if WITHUSBDEV_HSDESC
	#define MSOUTSAMPLES	48 /* количество сэмплов за милисекунду в UAC OUT */
	#define HSINTERVAL_AUDIO48 4	// endpoint descriptor parameters - для обеспечения 1 кГц периода
	#define FSINTERVAL_AUDIO48 1

#else /* WITHUSBDEV_HSDESC */
	#define MSOUTSAMPLES	48 /* количество сэмплов за милисекунду в UAC OUT */
	#define HSINTERVAL_AUDIO48 1//4	// endpoint descriptor parameters - для обеспечения 1 кГц периода
	#define FSINTERVAL_AUDIO48 1

#endif /* WITHUSBDEV_HSDESC */

#define MSINSAMPLES		(MSOUTSAMPLES + 1) /* количество сэмплов за милисекунду в UAC IN */


#define DMABUFFSTEPUACIN16	(UACIN_FMT_CHANNELS_AUDIO48)		// 2 - каждому сэмплу соответствует два числа в  буфере для выдачи по USB в host

#define DMABUFFSIZEUACIN16_AJ (MSINSAMPLES * DMABUFFSTEPUACIN16)	/* размер под USB ENDPOINT PACKET SIZE В буфере помещаются пары значений - стерео кодек */

#define DMABUFFSIZEUACIN16 DMAHWEPADJUST(DMABUFFSIZEUACIN16_AJ, DMABUFFSTEPUACIN16 * HARDWARE_RTSDMABYTES)


/* если приоритет прерываний USB не выше чем у аудиобработки - она должна длиться не более 1 мс (WITHRTS192 - 0.5 ms) */
#define DMABUFCLUSTER	19	// Прерывания по приему от IF CODEC или FPGA RX должны происходить не реже 1 раз в милисекунду (чтобы USB работать могло) */
#define DMABUFSCALE		4	// внутрений параметр, указыват на сколько реже ьулут происходить прерывания по обмену буфрами от остальны каналов по отношению к приему от FPGA
#define DMABUFFSIZE16RX	(DMABUFCLUSTER * DMABUFFSTEP16RX * DMABUFSCALE)		/* AF CODEC ADC */
#define DMABUFFSIZE16TX	(DMABUFCLUSTER * DMABUFFSTEP16TX * DMABUFSCALE)		/* AF CODEC DAC */
#define DMABUFFSIZE32RX (DMABUFCLUSTER * DMABUFFSTEP32RX)		/* FPGA RX or IF CODEC RX */
#define DMABUFFSIZE32RTS (DMABUFCLUSTER * DMABUFFSTEP32RTS)		/* FPGA RX or IF CODEC RX */
#define DMABUFFSIZE32TX (DMABUFCLUSTER * DMABUFFSTEP32TX * DMABUFSCALE)	/* FPGA TX or IF CODEC TX	*/

// stereo, 16 bit samples
// По звуковому каналу передается стерео, 16 бит, 48 кГц - 288 байт размер данных в ендпонтт
#define UACIN_AUDIO48_SAMPLEBITS	16
#define UACIN_AUDIO48_DATASIZE (DMABUFFSIZEUACIN16 * sizeof (int16_t))

// Параметры для канала передачи Real Time Spectrum - stereo, 32 bit, 192 kS/S
#define DMABUFFSTEP192RTS 8	// 8: стерео по 32 бит, 6: стерео по 24 бит
#define DMABUFFSIZE192RTS_AJ (128/*288*/ * DMABUFFSTEP192RTS)

#define DMABUFFSIZE192RTS DMAHWEPADJUST(DMABUFFSIZE192RTS_AJ, DMABUFFSTEP192RTS * HARDWARE_RTSDMABYTES)

#define DMABUFFSTEP96RTS 6	// 6: стерео по 24 бит
#define DMABUFFSIZE96RTS_AJ ((MSOUTSAMPLES * 2 + 1) * DMABUFFSTEP96RTS) //((96 + 4) * DMABUFFSTEP96RTS)		// 588 - должно быть кратно 4 байтам - для работы DMA в Renesas

#define DMABUFFSIZE96RTS DMAHWEPADJUST(DMABUFFSIZE96RTS_AJ, DMABUFFSTEP96RTS * HARDWARE_RTSDMABYTES)

#if WITHRTS96

	// stereo, 24 bit samples
	#define UACIN_RTS96_SAMPLEBITS		24
	#define UACIN_RTS96_DATASIZE		(DMABUFFSIZE96RTS * sizeof (uint8_t))

	#define HSINTERVAL_RTS96 4	// endpoint descriptor parameters
	#define FSINTERVAL_RTS96 1

#endif /* WITHRTS96 */
#if WITHRTS192

	// По каналу real-time спектра стерео, 32 бит, 192 кГц - 288*2*4 = 2304 байта
	// stereo, 32 bit samples
	#define UACIN_RTS192_SAMPLEBITS	32
	#define UACIN_RTS192_DATASIZE		(DMABUFFSIZE192RTS * sizeof (int8_t))

	#define HSINTERVAL_RTS192 3	// 500 us
	#define FSINTERVAL_RTS192 1

#endif /* WITHRTS192 */


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


#define UACOUT_AUDIO48_SAMPLEBITS	16	// may be 24

// буфер приема потока данных от USB к модулятору
#define UACOUT_AUDIO48_DATASIZE	( \
	MSOUTSAMPLES * \
	((UACOUT_AUDIO48_SAMPLEBITS * UACOUT_AUDIO48_FMT_CHANNELS + 7) / 8) \
	)


//#if WITHINTEGRATEDDSP

	#if (((__ARM_FP & 0x08) && defined(__aarch64__)) || (__riscv_d)) && 0

		typedef double FLOAT_t;

		#define ARM_MORPH(name) name ## _f64
		#define FLOORF	floor
		#define LOG10F	local_log10 //log10
		#define LOGF	log
		#define POWF	pow
		#define LOG2F	log2
//		#define LOGF	local_log
//		#define POWF	local_pow
//		#define LOG2F	local_log2
		#define SINF	sin
		#define COSF	cos
		#define ATAN2F	atan2
		#define ATANF	atan
		#define TANF	tan
		//#define EXPF	exp
		#define EXPF	local_exp
		#define FABSF	fabs
		#define SQRTF	sqrt
		#define FMAXF	fmax
		#define FMINF	fmin
		#define LDEXPF	ldexp
		#define FREXPF	frexp
		
		#if defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMA)
			#define FMAF	fma
		#endif /* defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMA) */
		#define DSP_FLOAT_BITSMANTISSA 54

	#elif 1 //(__ARM_FP & 0x04)

		typedef float FLOAT_t;

		#define ARM_MORPH(name) name ## _f32
		#define FLOORF	floorf
		#define LOG10F	local_log10 //log10f
		#define LOGF	logf
		#define POWF	powf
		#define LOG2F	log2f
//		#define LOGF	local_log
//		#define POWF	local_pow
//		#define LOG2F	local_log2
		#define SINF	sinf
		#define COSF	cosf
		#define ATAN2F	atan2f
		#define ATANF	atanf
		#define TANF	tanf
		//#define EXPF	expf
		#define EXPF	local_exp
		#define FABSF	fabsf
		#define SQRTF	sqrtf
		#define FMAXF	fmaxf
		#define FMINF	fminf
		#define LDEXPF	ldexpf
		#define FREXPF	frexpf

		#if defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMAF)
			#define FMAF	fmaf
		#endif /* defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMAF) */
		#define DSP_FLOAT_BITSMANTISSA 24

	#else

		#error This CPU not support floating point

	#endif

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


/* from "C Language Algorithms for Digital Signal Processing"
   by Paul M. Embree and Bruce Kimble, Prentice Hall, 1991 */


FLOAT_t local_exp(FLOAT_t x);
FLOAT_t local_pow(FLOAT_t x, FLOAT_t y);
FLOAT_t local_log(FLOAT_t x);
FLOAT_t local_log10(FLOAT_t X);

/* для возможности работы с функциями сопроцессора NEON - vld1_f32 например */
#define IV ivqv [0]
#define QV ivqv [1]

typedef struct
{
	FLOAT_t ivqv [2];
} FLOAT32P_t;

typedef struct
{
	int_fast32_t ivqv [2];
} INT32P_t;

// Ограничение алгоритма генерации параметров фильтра - нечётное значение Ntap.
// Кроме того, для функций фильтрации с использованием симметрии коэффициентов, требуется кратность 2 половины Ntap

#define NtapValidate(n)	((unsigned) (n) / 8 * 8 + 1)
#define NtapCoeffs(n)	((unsigned) (n) / 2 + 1)

#if WITHDSPLOCALFIR
	/* Фильтрация квадратур осуществляется процессором */
	#define	Ntap_rx_AUDIO	NtapValidate(241)

#else /* WITHDSPLOCALFIR */

	#if CPUSTYLE_STM32MP1

		#define	Ntap_rx_AUDIO	NtapValidate(1023)
		#define Ntap_tx_MIKE	NtapValidate(511)

	#else /* CPUSTYLE_STM32MP1 */

	#define	Ntap_rx_AUDIO	NtapValidate(511)
	#define Ntap_tx_MIKE	NtapValidate(241)

	#endif /* CPUSTYLE_STM32MP1 */
	#if WITHNOSPEEX
		////#define	Ntap_rx_AUDIO	NtapValidate(511)
		////#define Ntap_tx_MIKE	NtapValidate(241)

	#else /* WITHNOSPEEX */
		////#define	Ntap_rx_AUDIO	NtapValidate(SPEEXNN * 2 - 7)
		////#define Ntap_tx_MIKE	NtapValidate(241) //Ntap_rx_AUDIO

	#endif /* ! WITHDSPLOCALFIR */

#endif /* WITHDSPLOCALFIR */

#if CODEC1_FRAMEBITS == 64

	/* параметры входного/выходного адаптеров */
	#define WITHADAPTERCODEC1WIDTH	24		// 1 бит знак и 23 бит значащих
	#define WITHADAPTERCODEC1SHIFT	8		// количество незанятых битов справа.
	typedef int32_t aubufv_t;
	typedef int_fast32_t aufastbufv_t;
	typedef int_fast64_t aufastbufv2x_t;	/* тип для работы ресэмплера при получении среднего арифметического */


#else /* CODEC1_FRAMEBITS == 64 */

	/* параметры входного/выходного адаптеров */
	#define WITHADAPTERCODEC1WIDTH	16		// 1 бит знак и 15 бит значащих
	#define WITHADAPTERCODEC1SHIFT	0		// количество незанятых битов справа.
	typedef int16_t aubufv_t;
	typedef int_fast16_t aufastbufv_t;
	typedef int_fast32_t aufastbufv2x_t;	/* тип для работы ресэмплера при получении среднего арифметического */

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

#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_GW2A_V0)

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

	#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_GW2A_V0)

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

	#define ARMI2SRATE			((unsigned long) 8000)	// I2S sample rate audio codec (human side)
	#define ARMI2SRATEX(scale)	((unsigned long) (ARMI2SRATE * (scale)))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATE100		((unsigned long) ARMI2SRATEX(100))

#else /* WITHDTMFPROCESSING */

	#define ARMI2SRATE			((unsigned long) (ARMI2SMCLK / 256))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATEX(scale)	((unsigned long) (ARMI2SMCLKX(scale)))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATE100		((unsigned long) (ARMI2SRATEX(100)))

#endif /* WITHDTMFPROCESSING */

#if WITHDSPLOCALFIR || WITHDSPLOCALTXFIR
	/* Фильтрация квадратур осуществляется процессором */
	#if CPUSTYLE_R7S721
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation

	#elif CPUSTYLE_STM32MP1 || CPUSTYLE_XC7Z
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(241)	// single samples, floating point implementation

	#elif CPUSTYLE_STM32F7XX
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation

	#else
		#define Ntap_rx_SSB_IQ	NtapValidate(181)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(181)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation

	#endif

#endif /* WITHDSPLOCALFIR */

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
uint_fast8_t getsampmlemoni(FLOAT32P_t * v);			/* получить очередной сэмпл для самоконтроля */

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

uintptr_t getfilled_dmabufferx(uint_fast16_t * sizep);	/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferx(uintptr_t addr);	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
// WITHUSBUACIN2 specific
uintptr_t getfilled_dmabufferxrts(uint_fast16_t * sizep);	/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferxrts(uintptr_t addr);	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */

void refreshDMA_uacin(void); // Канал DMA ещё занят - оставляем в очереди, иначе получить данные через getfilled_dmabufferx

uintptr_t getfilled_dmabuffer32tx_main(void);
uintptr_t getfilled_dmabuffer32tx_sub(void);
uintptr_t getfilled_dmabuffer16txphones(void);

void dsp_extbuffer32rx(const IFADCvalue_t * buff);	// RX
void dsp_extbuffer32rts(const IFADCvalue_t * buff);	// RX
void dsp_extbuffer32wfm(const IFADCvalue_t * buff);	// RX
void dsp_addsidetone(aubufv_t * buff, int usebuf);			// перед передачей по DMA в аудиокодек

void processing_dmabuffer16rx(uintptr_t addr);	// обработать буфер после оцифровки AF ADC
//void processing_dmabuffer16rxuac(uintptr_t addr);	// обработать буфер после приёма пакета с USB AUDIO
void processing_dmabuffer32rx(uintptr_t addr);
void processing_dmabuffer32rts(uintptr_t addr);
void release_dmabuffer32rx(uintptr_t addr);
void processing_dmabuffer32rts192(uintptr_t addr);
void processing_dmabuffer32wfm(uintptr_t addr);
void buffers_resampleuacin(unsigned nsamples);

void dsp_sidetone_ping(void);	// system_level irq handler: формирование маркера начала записи по PPS в одном из каналов USB

int_fast32_t buffers_dmabuffer32rxcachesize(void);
int_fast32_t buffers_dmabuffer32txcachesize(void);
int_fast32_t buffers_dmabuffer16rxcachesize(void);
int_fast32_t buffers_dmabuffer16txcachesize(void);
int_fast32_t buffers_dmabuffer192rtscachesize(void);
int_fast32_t buffers_dmabuffer32rtscachesize(void);
int_fast32_t buffers_dmabuffer96rtscachesize(void);

void savesamplerecord16SD(int_fast16_t ch0, int_fast16_t ch1); /* to SD CARD */
void savesamplerecord16uacin(int_fast16_t ch0, int_fast16_t ch1); /* to USB AUDIO */
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

	typedef FLOAT_t speexel_t;
	uint_fast8_t takespeexready_user(FLOAT_t * * dest);
	void releasespeexbuffer_user(FLOAT_t * t);
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
void board_set_datavox(uint_fast8_t v);	/* автоматический переход на передачу при появлении звука со стороны компьютера */

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
void dsp_recalceq_coeffs(uint_fast8_t pathi, FLOAT_t * dCoeff, int iCoefNum);	// calculate full array of coefficients

void modem_initialze(void);
uint_fast8_t modem_get_ptt(void);

/* Интерфейс к AF кодеку */
typedef struct codec1if_tag
{
	uint_fast8_t (* clocksneed)(void);	/* требуется ли подача тактирования для инициадизации кодека */
	void (* stop) (void);
	void (* initialize) (void);
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
	void (* initialize)(void);
	const char * label;
} codec2if_t;

const codec2if_t * board_getfpgacodecif(void);		// получить интерфейс управления кодеком для получения данных о радиосигнале

/* +++ UAC OUT data save - использование данных от компьютера. */
void uacout_buffer_initialize(void);
void uacout_buffer_start(void);
void uacout_buffer_stop(void);
void uacout_buffer_save_system(const uint8_t * buff, uint_fast16_t size, uint_fast8_t ichannels, uint_fast8_t ibits);
void uacout_buffer_save_realtime(const uint8_t * buff, uint_fast16_t size, uint_fast8_t ichannels, uint_fast8_t ibits);

void buffers_set_uacinalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC IN interface */
void buffers_set_uacoutalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC OUT interface */
void buffers_set_uacinrtsalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC IN interface */

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
	SPINLOCK_t listlock;
} deliverylist_t;

void deliverylist_initialize(deliverylist_t * list);

void deliveryfloat(deliverylist_t * head, FLOAT_t ch0, FLOAT_t ch1);
void deliveryfloat_user(deliverylist_t * head, const FLOAT_t * ch0, const FLOAT_t * ch1, unsigned n);
void deliveryint(deliverylist_t * head, int_fast32_t ch0, int_fast32_t ch1);

/* Функции target всегда вызываются в режиме с запрещенными прерываниями */
void subscribefloat_user(deliverylist_t * head, subscribefloat_t * target, void * ctx, void (* pfn)(void * ctx, FLOAT_t ch0, FLOAT_t ch1));
void subscribeint_user(deliverylist_t * head, subscribeint32_t * target, void * ctx, void (* pfn)(void * ctx, int_fast32_t ch0, int_fast32_t ch1));

/* Функции target всегда вызываются в режиме с запрещенными прерываниями */
void subscribefloat(deliverylist_t * head, subscribefloat_t * target, void * ctx, void (* pfn)(void * ctx, FLOAT_t ch0, FLOAT_t ch1));
void subscribeint(deliverylist_t * head, subscribeint32_t * target, void * ctx, void (* pfn)(void * ctx, int_fast32_t ch0, int_fast32_t ch1));

extern deliverylist_t rtstargetsint;	// выход обработчика DMA приема от FPGA
extern deliverylist_t speexoutfloat;	// выход sppeex и фильтра
extern deliverylist_t afdemodoutfloat;	// выход приемника

#if WITHAFEQUALIZER

enum {
	AF_EQUALIZER_BANDS = 3,		// число полос
	AF_EQUALIZER_BASE = 8,		// предел регулировки
	AF_EQUALIZER_LOW = 400,		// частота нижней полосы
	AF_EQUALIZER_MID = 1500,	// частота средней полосы
	AF_EQUALIZER_HIGH = 2700	// частота верхней полосы
};

int_fast32_t hamradio_get_af_equalizer_base(void);
int_fast32_t hamradio_get_af_equalizer_gain_rx(uint_fast8_t v);
void hamradio_set_af_equalizer_gain_rx(uint_fast8_t index, uint_fast8_t gain);
void board_set_equalizer_rx(uint_fast8_t n);
void board_set_equalizer_tx(uint_fast8_t n);
void board_set_equalizer_rx_gains(const uint_fast8_t * p);
void board_set_equalizer_tx_gains(const uint_fast8_t * p);
uint_fast8_t hamradio_get_geqrx(void);
void hamradio_set_geqrx(uint_fast8_t v);

void audio_rx_equalizer(float32_t *buffer, uint_fast16_t size);

#endif /* WITHAFEQUALIZER */


#if __STDC__ && ! CPUSTYLE_ATMEGA

#define MAXFLOAT	3.40282347e+38F

#define M_E		2.7182818284590452354
#define M_LOG2E		1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN2		_M_LN2
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
