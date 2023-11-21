/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	/* sprintf() replacement */
#include <ctype.h>
#include <string.h>

#if WITHUSEAUDIOREC

#include "board.h"
#include "sdcard.h"
#include "fatfs/ff.h"

#include "audio.h"
#include "buffers.h"
#include "display/display.h"	// PACKEDCOLORPIP_T

///////////////////////////////////////////////////

static ALIGNX_BEGIN RAMNOINIT_D1 FIL wav_file ALIGNX_END;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
static FSIZE_t wav_lengthpos_riff;	/* position for write length at RIFF header */
static FSIZE_t wav_lengthpos_data;	/* position for write length at data subchunk*/
static uint_fast32_t wave_num_bytes;
static const unsigned int bytes_per_sample = 2;	/* 2: 16-bit samples */
static const unsigned long wFormatTag = 0x01;	/* 1 - integers, 3 - float */
#if WITHUSEAUDIOREC2CH
static const unsigned int num_channels = 2;	/* 2: stereo */
#else /* WITHUSEAUDIOREC2CH */
static const unsigned int num_channels = 1;	/* 1: monoaural */
#endif /* WITHUSEAUDIOREC2CH */

#if WITHUSEAUDIORECCLASSIC
	// Минимальный формат
	static const unsigned int DATACHUNKSTARTOFFSET = 0x0024;		// miltibloak write используется через раз (3-1-3-1-3-1-3-1-3-1-1-1-3-1-3-1)
#else
	// С дополнительным объёмом данных (не всеми устройствами распознается)
	static const unsigned int DATACHUNKSTARTOFFSET = (AUDIORECBUFFSIZE16 * 2) - 8;	// 4-4-4-4-4-4-4
#endif

/* make_wav.c
 * Creates a WAV file from an array of ints.
 * Output is monophonic, signed 16-bit samples
 * copyright
 * Fri Jun 18 16:36:23 PDT 2010 Kevin Karplus
 * Creative Commons license Attribution-NonCommercial
 *  http://creativecommons.org/licenses/by-nc/3.0/
 */

/* information about the WAV file format from

http://ccrma.stanford.edu/courses/422/projects/WaveFormat/

 */

unsigned audiorec_getwidth(void)
{
	return bytes_per_sample * 8;
}

static FRESULT write_little_endian(uint_fast32_t word, int num_bytes)
{
	uint8_t buff [4];
	UINT bw;
	ASSERT(num_bytes <= ARRAY_SIZE(buff));
	USBD_poke_u32(buff, word);
	return (f_write(& wav_file, & word, num_bytes, & bw) != FR_OK || bw != num_bytes) ? FR_INT_ERR : FR_OK;
}
/*
static FRESULT write_wav_sample(int data)
{
	unsigned i;
	for (i = 0; i < num_channels; ++ i)
	{
		FRESULT rc;
		rc = write_little_endian(data, bytes_per_sample);
		if (rc != FR_OK)
			return rc;
	}
	wave_num_bytes += bytes_per_sample;
	return FR_OK;
}
*/
/*
static void write_wav_samplestereo(int ldata, int rdata)
{
	unsigned i;
	for (i = 0; i < num_channels; ++ i)
	{
		switch (i)
		{
		case 0:
			write_little_endian(ldata, bytes_per_sample);
			break;
		case 1:
			write_little_endian(rdata, bytes_per_sample);
			break;
		default:
			write_little_endian(0, bytes_per_sample);
			break;
		}
	}
	wave_num_bytes += bytes_per_sample;
}
*/

#if 0
// Winrad wav chunk (from Winrad sources)
struct auxihdr     // used by SpectraVue in WAV files
{
//	char  	   chunkID[4];	          // ="auxi" (chunk rfspace)
//	long  	   chunkSize;	          // lunghezza del chunk
	SYSTEMTIME StartTime;
	SYSTEMTIME StopTime;
	DWORD CenterFreq; //receiver center frequency
	DWORD ADFrequency; //A/D sample frequency before downsampling
	DWORD IFFrequency; //IF freq if an external down converter is used
	DWORD Bandwidth; //displayable BW if you want to limit the display to less than Nyquist band
	DWORD IQOffset; //DC offset of the I and Q channels in 1/1000's of a count
	DWORD Unused2;
	DWORD Unused3;
	DWORD Unused4;
	DWORD Unused5;
	char  nextfilename[96];
};

// Complete WAV file header
struct WavHdr
{
    char  _RIFF[4]; // "RIFF"
    DWORD FileLen;  // length of all data after this (FileLength - 8)

    char _WAVE[4];  // "WAVE"

    char _fmt[4];        // "fmt "
    DWORD FmtLen;        // length of the next item (sizeof(WAVEFORMATEX))
    WAVEFORMATEX Format; // wave format description

    char _auxi[4];       // "auxi"
    DWORD AuxiLen;       // length of the next item (sizeof(struct auxihdr))
    struct auxihdr Auxi; // RF setup description

    char  _data[4];  // "data"
    DWORD DataLen;   // length of the next data (FileLength - sizeof(struct WavHdr))

};

// Complete WAV file header for 24-bit files
struct WavHdr24
{
    char  _RIFF[4]; // "RIFF"
    DWORD FileLen;  // length of all data after this (FileLength - 8)

    char _WAVE[4];  // "WAVE"

    char _fmt[4];        // "fmt "
    DWORD FmtLen;        // length of the next item (sizeof(WAVEFORMATEXTENSIBLE))
    WAVEFORMATEXTENSIBLE Format; // wave format description

    char _auxi[4];       // "auxi"
    DWORD AuxiLen;       // length of the next item (sizeof(struct auxihdr))
    struct auxihdr Auxi; // RF setup description

    char  _data[4];  // "data"
    DWORD DataLen;   // length of the next data (FileLength - sizeof(struct WavHdr24) - ListBufLen)

};
#endif

static FRESULT write_wav_header(const char * filename, unsigned int sample_rate)
{
    //unsigned int byte_rate = sample_rate * num_channels * bytes_per_sample;

 	wave_num_bytes = 0;

	FRESULT rc;				/* Result code */

	do
	{
		memset(& wav_file, 0, sizeof wav_file);
		rc = f_open(& wav_file, filename, FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
		if (rc != FR_OK)
			break;
 		rc = f_lseek(& wav_file, 0);
		if (rc != FR_OK)
			break;
		/* write RIFF header */
		UINT bw;
		/* offs 0x0000 */ rc = f_write(& wav_file, "RIFF", 4, & bw);
		if (rc != FR_OK)
			break;
		wav_lengthpos_riff = f_tell(& wav_file);
		/* offs 0x0004 */ rc = write_little_endian(0, 4);	// write palceholder - remaining length after this header
 		if (rc != FR_OK)
			break;
		/* offs 0x0008 */ rc = f_write(& wav_file, "WAVE", 4, & bw);
		if (rc != FR_OK || bw != 4)
			break;

		/* write fmt  subchunk */
 		/* offs 0x000c */ rc = f_write(& wav_file, "fmt ", 4, & bw);
		if (rc != FR_OK || bw != 4)
			break;
		/* offs 0x0010 */ rc = write_little_endian(DATACHUNKSTARTOFFSET - 0x14, 4);   /* SubChunk1Size is 16 - remaining length after this header */
		if (rc != FR_OK)
			break;
		/* Это начало данных для учёта в SubChunk1Size */
		/* offs 0x0014 */ rc = write_little_endian(wFormatTag, 2);    /* wFormatTag PCM is format 1 */
		if (rc != FR_OK)
			break;
		/* offs 0x0016 */ rc = write_little_endian(num_channels, 2);	/* wChannels */
		if (rc != FR_OK)
			break;
		/* offs 0x0018 */ rc = write_little_endian(sample_rate, 4);		/* dwSamplesPerSec */
		if (rc != FR_OK)
			break;
		/* offs 0x001c */ rc = write_little_endian(sample_rate * num_channels * bytes_per_sample, 4); /* dwAvgBytesPerSec - byte rate */
		if (rc != FR_OK)
			break;
		/* offs 0x0020 */ rc = write_little_endian(num_channels * bytes_per_sample, 2);  /* wBlockAlign - block align */
		if (rc != FR_OK)
			break;
		/* offs 0x0022 */ rc = write_little_endian(8 * bytes_per_sample, 2);  /* PCM format specific data: wBitsPerSample - bits/sample */
		if (rc != FR_OK)
			break;
		rc = f_sync(& wav_file);
		if (rc != FR_OK)
			break;

		/* write data subchunk */
 		/* offs 0x0024 */  rc = f_lseek(& wav_file, DATACHUNKSTARTOFFSET);
		if (rc != FR_OK)
			break;
		/* Это конец данных для учёта в SubChunk1Size */
		rc = f_write(& wav_file, "data", 4, & bw);
		if (rc != FR_OK || bw != 4)
			break;
		wav_lengthpos_data = f_tell(& wav_file);
		rc = write_little_endian(0, 4);	// write palceholder - remaining length after this header
		if (rc != FR_OK)
			break;
		rc = f_sync(& wav_file);
		if (rc != FR_OK)
			break;

	} while (0);
	return rc;
}

static FRESULT write_wav_resync(void)
{
	FRESULT rc;				/* Result code */

	do
	{
		rc = f_sync(& wav_file);
		if (rc != FR_OK)
			break;
		FSIZE_t wav_pos = f_tell(& wav_file);
		/* update data subchunk */
		rc = f_lseek(& wav_file, wav_lengthpos_data);
		if (rc != FR_OK)
			break;
		rc = write_little_endian(wave_num_bytes, 4);
		if (rc != FR_OK)
			break;
		{
			UINT rb4;
			uint8_t rb [4];
			rc = f_lseek(& wav_file, 0);
			if (rc != FR_OK)
				break;
			rc = f_read(& wav_file, rb, 4, & rb4);
			if (rc != FR_OK)
				break;
			if (rb4 != 4)
				break;
			if (memcmp(rb, "RIFF", 4) != 0)
				break;
		}
		/* update RIFF header */
		rc = f_lseek(& wav_file, wav_lengthpos_riff);
		if (rc != FR_OK)
			break;
		rc = write_little_endian(DATACHUNKSTARTOFFSET + wave_num_bytes, 4);
		if (rc != FR_OK)
			break;

		rc = f_lseek(& wav_file, wav_pos);
		if (rc != FR_OK)
			break;
		rc = f_sync(& wav_file);
		if (rc != FR_OK)
			break;
	} while (0);
	return rc;
}

static FRESULT write_wav_tail(void)
{
 	FRESULT rc;				/* Result code */
	do
	{
		rc = write_wav_resync();
		if (rc != FR_OK)
			break;
		rc = f_close(& wav_file);
		if (rc != FR_OK)
			break;
	} while (0);
	return rc;
}

///////////////////////////////////////////
//
// See also: http://en.wikipedia.org/wiki/RF64

static uint_fast32_t wave_irecorded;
static ALIGNX_BEGIN RAMNOINIT_D1 FATFS wave_Fatfs ALIGNX_END;		/* File system object  - нельзя располагать в Cortex-M4 CCM */

static uint_fast8_t waveUnmount(void)
{
	if (! hamradio_get_usbh_active())
		return 1;
	FRESULT rc;				/* Result code */

	rc = f_mount(NULL, "", 0);
	return rc != FR_OK;
}

/* Register volume work area (never fails) */
static uint_fast8_t waveMount(void)
{
	if (! hamradio_get_usbh_active())
		return 1;

	FRESULT rc;				/* Result code */

	memset(& wave_Fatfs, 0, sizeof wave_Fatfs);
	rc = f_mount(& wave_Fatfs, "", 0);
	return rc != FR_OK;
}

// Начинаем запись
// 1 - неудачно
static uint_fast8_t wave_startrecording(void)
{
	FRESULT rc;				/* Result code */
	char fname [FF_MAX_LFN + 1];

#if defined (RTC1_TYPE)

	uint_fast16_t year;
	uint_fast8_t month, day;
	uint_fast8_t hour, minute, seconds;
	static unsigned ver;

	board_rtc_cached_getdatetime(& year, & month, & day, & hour, & minute, & seconds);

	local_snprintf_P(fname, sizeof fname / sizeof fname [0],
		PSTR("rec_%lu_%04d-%02d-%02d_%02d%02d%02d_%u.wav"),
		(hamradio_get_freq_rx() + 500) / 1000uL,	// частота с точностью до килогерц
		year, month, day,
		hour, minute, seconds,
		++ ver
		);

#else /* defined (RTC1_TYPE) */

	static uint_fast32_t rnd;
	static unsigned ver;

	if (rnd == 0)
		rnd = hardware_get_random();

	local_snprintf_P(fname, sizeof fname / sizeof fname [0],
		PSTR("rec_%u_%08X_%u.wav"),
		(hamradio_get_freq_rx() + 500) / 1000u,	// частота с точностью до килогерц
		(unsigned) rnd,
		++ ver
		);

#endif /* defined (RTC1_TYPE) */

	PRINTF(PSTR("Write wav file '%s'.\n"), fname);

	rc = write_wav_header(fname, dsp_get_sampleraterx());
	wave_irecorded = 0;
	return (rc != FR_OK);	// 1 - ошибка - заканчиваем запись.
}

// Заканчиваем запись
static uint_fast8_t wave_stoprecording(void)
{
	if (! hamradio_get_usbh_active())
		return 1;
	FRESULT rc;				/* Result code */
	rc = write_wav_tail();
	return (rc != FR_OK);	// 1 - ошибка - заканчиваем запись.
}


static uint_fast8_t wave_resync(void)
{
	if (! hamradio_get_usbh_active())
		return 1;
	FRESULT rc;				/* Result code */
	rc = write_wav_resync();
	return (rc != FR_OK);	// 1 - ошибка - заканчиваем запись.
}

// 1 - Заканчиваем запись
// 2 - Заканчиваем запись и начинаем следующий фрагмент
// 3 - выполнить resync
static uint_fast8_t wave_nextblockrecording(void)
{
	if (! hamradio_get_usbh_active())
		return 1;
	const uint_fast32_t RSYNLEN = UINT32_C(10) * 1024 * 1024;
	const uint_fast32_t FILELEN = UINT32_C(695) * 1024 * 1024;	// Размер ограничивающий файл не кратен предидущему числу
	//const uint_fast32_t FILELEN = UINT32_C(33) * 1024 * 1024;	// Размер ограничивающий файл не кратен предидущему числу
	void * p;
	unsigned n = takerecordbuffer(& p);
	if (n != 0)	// количество байтов для записи
	{
		FRESULT rc;				/* Result code */
		UINT bw;
		rc = f_write(& wav_file, p, n, & bw);
		releaserecordbuffer(p);

		if (rc != FR_OK || bw != n)
			return 1;	// 1 - ошибка - заканчиваем запись.

		wave_num_bytes += n;
		// Периодическая запись структур файла
		if (1 && (wave_irecorded += n) >= RSYNLEN)
		{
			//PRINTF(PSTR("wave_nextblockrecording: force resync\n"));
			// Каждые 10 мегабайт
			wave_irecorded = 0;
			return 3;		// 3 - выполнить resync
			//rc = write_wav_resync();
			//if (rc != FR_OK)
			//	return 1;	// 1 - ошибка - заканчиваем запись.
			//PRINTF(PSTR("wave_nextblockrecording: resync okay\n"));
		}
		if (f_size(& wav_file) >= FILELEN)
		{
			// Каждые 695 мегабайт
			return 2;	// Начать следующий фрагмент
		}
	}
	return 0;	// 1 - ошибка - заканчиваем запись.
}


// код завершения из вложенной state machune
enum
{
	SDSTATUS_OK,		// можно переходить к следующему
	SDSTATUS_BUSY,		// состояние state machime не меняется
	SDSTATUS_ERROR		// ошибка, возможно влтяет на переход к слдующему состояни.
};

enum
{
	SDSTATE_IDLE,
	SDSTATE_STARTREC,
	SDSTATE_RECORDING,
	SDSTATE_STOPRECORDING,
	SDSTATE_BREAKCHUNK,
	SDSTATE_CONTRECORDING,
	SDSTATE_UNMOUNT,
	SDSTATE_RESYNC
};

// AUDIO recording state
static uint_fast8_t	sdstate = SDSTATE_IDLE;

// перевод state machine в начальное состояние
void sdcardinitialize(void)
{
	sdstate = SDSTATE_IDLE;
}


// Возвращаем в display2.c состояние записи
uint_fast8_t hamradio_get_rec_value(void)
{
	return sdstate != SDSTATE_IDLE;
}

void sdcardrecord(void)
{
	switch (sdstate)
	{
	case SDSTATE_IDLE:
		PRINTF(PSTR("sdcardrecord: start recording\n"));
		// Начинаем запись
		if (waveMount() == 0)		/* Register volume work area (never fails) */
			sdstate = SDSTATE_STARTREC;
		break;
	}
}

void sdcardstop(void)	// функция "остановить запись"
{
	switch (sdstate)
	{
	case SDSTATE_RECORDING:
	case SDSTATE_BREAKCHUNK:
	case SDSTATE_RESYNC:
	case SDSTATE_CONTRECORDING:
		// Заканчиваем запись
		PRINTF(PSTR("sdcardstop: stop recording\n"));
		wave_stoprecording();
		sdstate = SDSTATE_UNMOUNT;
		break;
	}
}

void sdcardtoggle(void)
{
	if (sdstate == SDSTATE_IDLE)
		sdcardrecord();
	else
		sdcardstop();
}

// Функци вызывается из основного цикла для периодического "проталкивания"
// записанных буферов на SD CARD
void sdcardbgprocess(void)
{
	switch (sdstate)
	{
	case SDSTATE_IDLE:
		//PRINTF(PSTR("sdcardbgprocess: SDSTATE_IDLE\n"));
		break;

	case SDSTATE_RECORDING:
		//PRINTF(PSTR("sdcardbgprocess: SDSTATE_RECORDING\n"));
		switch (wave_nextblockrecording())
		{
		default:
		//case 1:
			// Заканчиваем запись
			sdstate = SDSTATE_STOPRECORDING;
			break;

		case 2:
			// Заканчиваем запись и начинаем следующий фрагмент
			sdstate = SDSTATE_BREAKCHUNK;
			break;

		case 3:
			// выполнить resync
			sdstate = SDSTATE_RESYNC;
			break;

		case 0:
			// остаемся в состоянии запись
			break;
		}
		break;

	case SDSTATE_RESYNC:
		PRINTF(PSTR("sdcardbgprocess: SDSTATE_RESYNC\n"));
		// выполнить resync
		if (wave_resync() == 0)
			sdstate = SDSTATE_RECORDING;
		else
			sdstate = SDSTATE_STOPRECORDING;
		break;

	case SDSTATE_BREAKCHUNK:
		PRINTF(PSTR("sdcardbgprocess: SDSTATE_BREAKCHUNK\n"));
		// Заканчиваем запись и начинаем следующий фрагмент
		if (wave_stoprecording() == 0)
			sdstate = SDSTATE_CONTRECORDING;
		else
			sdstate = SDSTATE_STOPRECORDING;
		break;

	case SDSTATE_CONTRECORDING:
		PRINTF(PSTR("sdcardbgprocess: SDSTATE_CONTRECORDING\n"));
		if (wave_startrecording() != 0)
		{
			PRINTF(PSTR("sdcardbgprocess: wave_startrecording failure\n"));
			sdstate = SDSTATE_UNMOUNT;
		}
		else
			sdstate = SDSTATE_RECORDING;
		break;

	case SDSTATE_STOPRECORDING:
		PRINTF(PSTR("sdcardbgprocess: SDSTATE_STOPRECORDING\n"));
		wave_stoprecording();
		sdstate = SDSTATE_UNMOUNT;
		break;

	case SDSTATE_UNMOUNT:
		PRINTF(PSTR("sdcardbgprocess: SDSTATE_UNMOUNT\n"));
		waveUnmount();		/* Unregister volume work area (never fails) */
		sdstate = SDSTATE_IDLE;
		break;

	case SDSTATE_STARTREC:
		PRINTF(PSTR("sdcardbgprocess: SDSTATE_STARTREC\n"));
		if (wave_startrecording() == 0)
		{
			PRINTF(PSTR("sdcardrecord: wave_startrecording success\n"));
			sdstate = SDSTATE_RECORDING;

			// Освобождаем несколько самых старых буферов для
			// исключения щёлкания в начале записи
			uint_fast8_t n;
			for (n = 0; n < 5; ++ n)
			{
				void * p;
				if (takerecordbuffer(& p) != 0)	// количество байтов для записи
					releaserecordbuffer(p);
			}
			sdstate = SDSTATE_RECORDING;

		}
		else
		{
			sdstate = SDSTATE_UNMOUNT;
			PRINTF(PSTR("sdcardrecord: wave_startrecording failure\n"));
		}
		break;
	}
}

void sdcardformat(void)
{
	static ALIGNX_BEGIN RAMNOINIT_D1 BYTE work [FF_MAX_SS] ALIGNX_END;
	FRESULT rc;

	switch (sdstate)
	{
	case SDSTATE_IDLE:
		PRINTF(PSTR("sdcardformat: start formatting\n"));
		rc = f_mkfs("0:", NULL, work, sizeof (work));
		if (rc != FR_OK)
		{
			PRINTF(PSTR("sdcardformat: f_mkfs failure\n"));
		}
		else
		{
			PRINTF(PSTR("sdcardformat: f_mkfs okay\n"));
		}
		break;
	}
}

#endif /* WITHUSEAUDIOREC */

#if WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC


static ALIGNX_BEGIN RAMNOINIT_D1 FIL bmp_file ALIGNX_END;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */

// Начинаем запись
// 1 - неудачно
static uint_fast8_t screenshot_startrecording(void)
{
	FRESULT rc;				/* Result code */
	char fname [FF_MAX_LFN + 1];

#if defined (RTC1_TYPE)

	uint_fast16_t year;
	uint_fast8_t month, day;
	uint_fast8_t hour, minute, seconds;
	static unsigned ver;

	board_rtc_cached_getdatetime(& year, & month, & day, & hour, & minute, & seconds);

	local_snprintf_P(fname, sizeof fname / sizeof fname [0],
		PSTR("scr_%04d-%02d-%02d_%02d%02d%02d_%u.bmp"),
		year, month, day,
		hour, minute, seconds,
		++ ver
		);

#else /* defined (RTC1_TYPE) */

	static uint_fast32_t rnd;
	static unsigned ver;

	if (rnd == 0)
		rnd = hardware_get_random();

	local_snprintf_P(fname, sizeof fname / sizeof fname [0],
		PSTR("rec_%08X_%u.bmp"),
		(unsigned) rnd,
		++ ver
		);

#endif /* defined (RTC1_TYPE) */

	PRINTF(PSTR("Write bmp file '%s'.\n"), fname);

	rc = FR_OK;

	memset(& bmp_file, 0, sizeof bmp_file);
	rc = f_open(& bmp_file, fname, FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
	return (rc != FR_OK);	// 1 - ошибка - заканчиваем запись.
}

typedef struct
{
    uint8_t signature[2];
    uint32_t filesize;
    uint32_t reserved;
    uint32_t fileoffset_to_pixelarray;
} ATTRPACKED fileheader_t;

typedef struct
{
    uint32_t dibheadersize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitsperpixel;
    uint32_t compression;
    uint32_t imagesize;
    uint32_t ypixelpermeter;
    uint32_t xpixelpermeter;
    uint32_t numcolorspallette;
    uint32_t mostimpcolor;
} ATTRPACKED bitmapinfoheader_t;

typedef struct {
	fileheader_t fileheader;
    bitmapinfoheader_t bitmapinfoheader;
} ATTRPACKED bitmap_t;

//#define pixel 0xFF

// Выполняем запись
// 1 - неудачно
static uint_fast8_t screenshot_bodyrecording(PACKEDCOLORPIP_T * buffer, uint_fast16_t dx, uint_fast16_t dy)
{
	enum { PIX_BYTES = 3 };
	const unsigned _bitsperpixel = PIX_BYTES * 8;
	const unsigned rowpadsize = (4 - (dx * PIX_BYTES) % 4) % 4;
	const unsigned _planes = 1;
	const unsigned _compression = 0;	// RGB
	const unsigned _pixelbytesize = (dy * (dx * _bitsperpixel / 8 + rowpadsize));
	const unsigned _filesize = (_pixelbytesize + sizeof (bitmap_t));
	const unsigned _xpixelpermeter = 0x130B; //2835 , 72 DPI
	const unsigned _ypixelpermeter = 0x130B; //2835 , 72 DPI
//	const unsigned rastersize = (dx * PIX_BYTES + rowpadsize) * dy;
	bitmap_t bm;
	FRESULT rc;				/* Result code */
	UINT wrCount;
	rc = FR_OK;

	memset(& bm, 0, sizeof bm);
	bm.fileheader.signature [0] = 'B';
	bm.fileheader.signature [1] = 'M';
	bm.fileheader.filesize = _filesize;
    bm.fileheader.fileoffset_to_pixelarray = sizeof (bitmap_t);
    bm.bitmapinfoheader.dibheadersize = sizeof (bitmapinfoheader_t);
    bm.bitmapinfoheader.width = dx;
    bm.bitmapinfoheader.height = dy;
    bm.bitmapinfoheader.planes = _planes;
    bm.bitmapinfoheader.bitsperpixel = _bitsperpixel;
    bm.bitmapinfoheader.compression = _compression;
    bm.bitmapinfoheader.imagesize = _pixelbytesize;
    bm.bitmapinfoheader.ypixelpermeter = _ypixelpermeter;
    bm.bitmapinfoheader.xpixelpermeter = _xpixelpermeter;
    bm.bitmapinfoheader.numcolorspallette = 0;

	if (rc != FR_OK)
		return 1;

	rc = f_write(& bmp_file, & bm, sizeof bm, & wrCount);
	if (rc != FR_OK || wrCount != sizeof bm)
		return 1;

#if LCDMODE_MAIN_L8
	COLOR24_T xltrgb24 [256];
	display2_xltrgb24(xltrgb24);
#endif /* LCDMODE_MAIN_L8 */

	unsigned y;
	for (y = 0; y < dy; ++ y)
	{
		uint8_t row [dx][PIX_BYTES];	// b, g, r, reserved
		unsigned x;
		for (x = 0; x < dx; ++ x)
		{
			const COLORPIP_T c = * colpip_mem_at(buffer, dx, dy, x, dy - y - 1);
#if LCDMODE_MAIN_L8
			const COLOR24_T v24 = xltrgb24 [c];
			row [x][0] = COLOR24_B(v24);
			row [x][1] = COLOR24_G(v24);
			row [x][2] = COLOR24_R(v24);
#else /* LCDMODE_MAIN_L8 */
			row [x][0] = COLORPIP_B(c);
			row [x][1] = COLORPIP_G(c);
			row [x][2] = COLORPIP_R(c);
#endif /* LCDMODE_MAIN_L8 */
		}
		rc = f_write(& bmp_file, row, sizeof row, & wrCount);
		if (rc != FR_OK || wrCount != sizeof row)
			return 1;
		if (rowpadsize != 0)
		{
			static const uint8_t zero [3];
			rc = f_write(& bmp_file, zero, rowpadsize, & wrCount);
			if (rc != FR_OK || wrCount != rowpadsize)
				return 1;
		}
	}

	return (rc != FR_OK);	// 1 - ошибка - заканчиваем запись.
}

// Завершаем запись
// 1 - неудачно
static uint_fast8_t screenshot_stoprecording(void)
{
	FRESULT rc;				/* Result code */

	rc = f_close(& bmp_file);
	return (rc != FR_OK);	// 1 - ошибка - заканчиваем запись.
}

/* запись видимого изображения в файл */
void display_snapshot_write(PACKEDCOLORPIP_T * buffer, uint_fast16_t dx, uint_fast16_t dy)
{
	if (sdstate == SDSTATE_IDLE)
	{
		waveMount();
		(void) (screenshot_startrecording() || screenshot_bodyrecording(buffer, dx, dy) || screenshot_stoprecording());
		waveUnmount();
	}
	else
	{
		(void) (screenshot_startrecording() || screenshot_bodyrecording(buffer, dx, dy) || screenshot_stoprecording());
	}
}


#endif /* WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC */

#if WITHWAVPLAYER || WITHSENDWAV

static ALIGNX_BEGIN RAMNOINIT_D1 FATFS Fatfs ALIGNX_END;		/* File system object  - нельзя располагать в Cortex-M4 CCM */
static ALIGNX_BEGIN RAMNOINIT_D1 FIL Fil ALIGNX_END;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
static ALIGNX_BEGIN RAMNOINIT_D1 uint8_t rbuff [FF_MAX_SS * 16] ALIGNX_END;		// буфер записи - при совпадении с _MAX_SS нельзя располагать в Cortex-M4 CCM
static UINT ibr = 0;		//  количество считанных байтов
static UINT ipos = 0;			// номер выводимого байта

// Complete WAV file header
struct WavHdr0
{
	char  _RIFF[4]; // "RIFF"
	DWORD FileLen;  // length of all data after this (FileLength - 8)

	char _WAVE[4];  // "WAVE"

	char _fmt[4];        // "fmt "
	DWORD FmtLen;        // length of the next item (sizeof(WAVEFORMATEX))

	WORD  wFormatTag;
	WORD  nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD  nBlockAlign;
	WORD  wBitsPerSample;
	WORD  cbSize;

} ATTRPACKED;

static struct WavHdr0 hdr0;

static int playfile = 0;

uint_fast8_t isplayfile(void)
{
	return playfile;
}

void playwavstop(void)
{
	if (playfile)
	{
		PRINTF(PSTR("Stop active play file\n"));
		f_close(& Fil);
		playfile = 0;
	}
}

void playwavfile(const char * filename)
{
	FRESULT rc;				/* Result code */

	if (playfile)
	{
		PRINTF(PSTR("Stop active play file\n"));
		f_close(& Fil);
		playfile = 0;
	}
	f_mount(NULL, "", 0);		/* Unregister volume work area (never fails) */
	memset(& Fatfs, 0, sizeof Fatfs);
	f_mount(& Fatfs, "", 0);		/* Register volume work area (never fails) */
	// открываем файл
	memset(& Fil, 0, sizeof Fil);
	rc = f_open(& Fil, filename, FA_READ);
	if (rc)
	{
		PRINTF(PSTR("Can not open file '%s'\n"), filename);
		PRINTF(PSTR("Failed with rc=%u.\n"), rc);
		playfile = 0;
		return;
	}
	// check format
	{
		rc = f_read(& Fil, & hdr0, sizeof hdr0, & ibr);	/* Read a chunk of file */
		if (rc || ibr < sizeof hdr0)
		{
			PRINTF(PSTR("Can not check format of file '%s'\n"), filename);
			PRINTF(PSTR("Failed with rc=%u.\n"), rc);
			playfile = 0;
			rc = f_close(& Fil);
			return;
		}
		if (memcmp(hdr0._fmt, "fmt ", 4) != 0 || hdr0.nChannels != 1 || hdr0.wBitsPerSample != 16 || hdr0.nSamplesPerSec != dsp_get_sampleraterx())
		{
			PRINTF(PSTR("Wrong format of file '%s'\n"), filename);
			playfile = 0;
			rc = f_close(& Fil);
			return;
		}
		FSIZE_t startdata = hdr0.FmtLen + 0x001C;
		PRINTF("startdata = %08lX\n", (unsigned long) startdata);
		rc = f_lseek(& Fil, startdata);
		if (rc)
		{
			PRINTF(PSTR("Can not seek to wav data of file '%s'\n"), filename);
			PRINTF(PSTR("Failed with rc=%u.\n"), rc);
			playfile = 0;
			rc = f_close(& Fil);
			return;
		}
		unsigned long offs = startdata % sizeof rbuff;
		rc = f_read(& Fil, rbuff + offs, sizeof rbuff - offs, & ibr);	/* Read a chunk of file */
		if (rc || ! ibr)
		{
			PRINTF(PSTR("1-st read Failed with rc=%u.\n"), rc);
			playfile = 0;
			rc = f_close(& Fil);
			return;
		}
		ipos = offs;		// начальное положение указателя в буфере для вывода данных
	}
	ibr = 0;
	ipos = 0;
	playfile = 1;
}

void spoolplayfile(void)
{
	FRESULT rc;				/* Result code */
	int endoffile = 0;
	if (playfile == 0)
		return;

	do
	{
		if (ipos >= ibr)
		{
			// если буфер не заполнен - читаем
			rc = f_read(& Fil, rbuff, sizeof rbuff, & ibr);	/* Read a chunk of file */
			if (rc || ! ibr)
			{
				endoffile = 1;
				break;			/* Error or end of file */
			}
			ipos = 0;		// начальное положение указателя в буфере для вывода данных
		}
		else
		{
			unsigned saved = savesamplesplay(rbuff, ibr - ipos);
			ipos += saved;
		}
	} while (0);

	if (endoffile)
	{
		playfile = 0;

		PRINTF("End file play.\n");
		rc = f_close(& Fil);
		if (rc)
		{
			TP();
			PRINTF(PSTR("Failed with rc=%u.\n"), rc);
			return;
		}

	}
}

#endif /* WITHWAVPLAYER || WITHSENDWAV */
