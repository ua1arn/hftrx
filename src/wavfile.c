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

#include "display/display.h"	/* используем функцию получения рабочей частоты */
#include "audio.h"

///////////////////////////////////////////////////

static RAMNOINIT_D1 FIL wav_file;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
static FSIZE_t wav_lengthpos_riff;	/* position for write length at RIFF header */
static FSIZE_t wav_lengthpos_data;	/* position for write length at data subchunk*/
static unsigned long wave_num_bytes;
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


static FRESULT write_little_endian(unsigned int word, int num_bytes)
{
#if 1
	// for little-endian machines
	UINT bw;
	return f_write(& wav_file, & word, num_bytes, & bw) != FR_OK || bw != num_bytes;
#else
	// machine-independent version
    while (num_bytes>0)
    {   unsigned char buf = word & 0xff;
        fwrite(& buf, 1, 1, wav_file);
        num_bytes --;
		word >>= 8;
    }
#endif
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
		rc = f_open(& wav_file, filename, FA_WRITE | FA_CREATE_ALWAYS);
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

	} while (0);
	return rc;
}

static FRESULT write_wav_resync(void)
{
	FRESULT rc;				/* Result code */

	do
	{
		FSIZE_t wav_pos = f_tell(& wav_file);
		/* update data subchunk */
		rc = f_lseek(& wav_file, wav_lengthpos_data);
		if (rc != FR_OK)
			break;
		rc = write_little_endian(wave_num_bytes, 4);
		if (rc != FR_OK)
			break;
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
static RAMNOINIT_D1 FATFS wave_Fatfs;		/* File system object  - нельзя располагать в Cortex-M4 CCM */

static uint_fast8_t waveUnmount(void)
{
	FRESULT rc;				/* Result code */

	rc = f_mount(NULL, "", 0);
	return rc != FR_OK;
}

/* Register volume work area (never fails) */
static uint_fast8_t waveMount(void)
{
	FRESULT rc;				/* Result code */

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
	uint_fast8_t hour, minute, secounds;
	static unsigned long ver;

	board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);

	local_snprintf_P(fname, sizeof fname / sizeof fname [0],
		PSTR("rec_%lu_%04d-%02d-%02d_%02d%02d%02d_%lu.wav"),
		(hamradio_get_freq_rx() + 500) / 1000uL,	// частота с точностью до килогерц
		year, month, day,
		hour, minute, secounds,
		++ ver
		);

#else /* defined (RTC1_TYPE) */

	static uint_fast32_t rnd;
	static unsigned long ver;

	if (rnd == 0)
		rnd = hardware_get_random();

	local_snprintf_P(fname, sizeof fname / sizeof fname [0],
		PSTR("rec_%lu_%08lX_%lu.wav"),
		(hamradio_get_freq_rx() + 500) / 1000uL,	// частота с точностью до килогерц
		rnd,
		++ ver
		);

#endif /* defined (RTC1_TYPE) */

	debug_printf_P(PSTR("Write wav file '%s'.\n"), fname);

	rc = write_wav_header(fname, dsp_get_sampleraterx());
	wave_irecorded = 0;
	return (rc != FR_OK);	// 1 - ошибка - заканчиваем запись.
}

// Заканчиваем запись
static uint_fast8_t wave_stoprecording(void)
{
	FRESULT rc;				/* Result code */
	rc = write_wav_tail();
	return (rc != FR_OK);	// 1 - ошибка - заканчиваем запись.
}


static uint_fast8_t wave_resync(void)
{
	FRESULT rc;				/* Result code */
	rc = write_wav_resync();
	return (rc != FR_OK);	// 1 - ошибка - заканчиваем запись.
}

// 1 - Заканчиваем запись
// 2 - Заканчиваем запись и начинаем следующий фрагмент
// 3 - выполнить resync
static uint_fast8_t wave_nextblockrecording(void)
{
	const uint_fast32_t RSYNLEN = 10UL * 1024 * 1024;
	const uint_fast32_t FILELEN = 695UL * 1024 * 1024;	// Размер ограничивающий файл не кратен предидущему числу
	//const uint_fast32_t FILELEN = 33UL * 1024 * 1024;	// Размер ограничивающий файл не кратен предидущему числу
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
			//debug_printf_P(PSTR("wave_nextblockrecording: force resync\n"));
			// Каждые 10 мегабайт
			wave_irecorded = 0;
			return 3;		// 3 - выполнить resync
			//rc = write_wav_resync();
			//if (rc != FR_OK)
			//	return 1;	// 1 - ошибка - заканчиваем запись.
			//debug_printf_P(PSTR("wave_nextblockrecording: resync okay\n"));
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
		debug_printf_P(PSTR("sdcardrecord: start recording\n"));
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
		debug_printf_P(PSTR("sdcardstop: stop recording\n"));
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
	board_usb_spool();
	switch (sdstate)
	{
	case SDSTATE_IDLE:
		//debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_IDLE\n"));
		break;

	case SDSTATE_RECORDING:
		//debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_RECORDING\n"));
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
		debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_RESYNC\n"));
		// выполнить resync
		if (wave_resync() == 0)
			sdstate = SDSTATE_RECORDING;
		else
			sdstate = SDSTATE_STOPRECORDING;
		break;

	case SDSTATE_BREAKCHUNK:
		debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_BREAKCHUNK\n"));
		// Заканчиваем запись и начинаем следующий фрагмент
		if (wave_stoprecording() == 0)
			sdstate = SDSTATE_CONTRECORDING;
		else
			sdstate = SDSTATE_STOPRECORDING;
		break;

	case SDSTATE_CONTRECORDING:
		debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_CONTRECORDING\n"));
		if (wave_startrecording() != 0)
		{
			debug_printf_P(PSTR("sdcardbgprocess: wave_startrecording failure\n"));
			sdstate = SDSTATE_UNMOUNT;
		}
		else
			sdstate = SDSTATE_RECORDING;
		break;

	case SDSTATE_STOPRECORDING:
		debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_STOPRECORDING\n"));
		wave_stoprecording();
		sdstate = SDSTATE_UNMOUNT;
		break;

	case SDSTATE_UNMOUNT:
		debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_UNMOUNT\n"));
		waveUnmount();		/* Unregister volume work area (never fails) */
		sdstate = SDSTATE_IDLE;
		break;

	case SDSTATE_STARTREC:
		debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_STARTREC\n"));
		if (wave_startrecording() == 0)
		{
			debug_printf_P(PSTR("sdcardrecord: wave_startrecording success\n"));
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
			debug_printf_P(PSTR("sdcardrecord: wave_startrecording failure\n"));
		}
		break;
	}
}

void sdcardformat(void)
{
	ALIGNX_BEGIN BYTE work [FF_MAX_SS] ALIGNX_END;
	FRESULT rc;

	switch (sdstate)
	{
	case SDSTATE_IDLE:
		debug_printf_P(PSTR("sdcardformat: start formatting\n"));
		rc = f_mkfs("0:", NULL, work, sizeof (work));
		if (rc != FR_OK)
		{
			debug_printf_P(PSTR("sdcardformat: f_mkfs failure\n"));
		}
		else
		{
			debug_printf_P(PSTR("sdcardformat: f_mkfs okay\n"));
		}
		break;
	}
}

#endif /* WITHUSEAUDIOREC */


#if WITHWAVPLAYER || WITHSENDWAV

static FATFSALIGN_BEGIN RAMNOINIT_D1 FATFS Fatfs FATFSALIGN_END;		/* File system object  - нельзя располагать в Cortex-M4 CCM */
static FATFSALIGN_BEGIN RAMNOINIT_D1 FIL Fil FATFSALIGN_END;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
static RAMNOINIT_D1 FATFSALIGN_BEGIN uint8_t rbuff [FF_MAX_SS * 16] FATFSALIGN_END;		// буфер записи - при совпадении с _MAX_SS нельзя располагать в Cortex-M4 CCM
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
		debug_printf_P(PSTR("Stop active play file\n"));
		f_close(& Fil);
		playfile = 0;
	}
}

void playwavfile(const char * filename)
{
	FRESULT rc;				/* Result code */

	if (playfile)
	{
		debug_printf_P(PSTR("Stop active play file\n"));
		f_close(& Fil);
		playfile = 0;
	}
	f_mount(NULL, "", 0);		/* Unregister volume work area (never fails) */
	f_mount(& Fatfs, "", 0);		/* Register volume work area (never fails) */
	// открываем файл
	rc = f_open(& Fil, filename, FA_READ);
	if (rc)
	{
		debug_printf_P(PSTR("Can not open file '%s'\n"), filename);
		debug_printf_P(PSTR("Failed with rc=%u.\n"), rc);
		playfile = 0;
		return;
	}
	// check format
	{
		rc = f_read(& Fil, & hdr0, sizeof hdr0, & ibr);	/* Read a chunk of file */
		if (rc || ibr < sizeof hdr0)
		{
			debug_printf_P(PSTR("Can not check format of file '%s'\n"), filename);
			debug_printf_P(PSTR("Failed with rc=%u.\n"), rc);
			playfile = 0;
			rc = f_close(& Fil);
			return;
		}
		if (memcmp(hdr0._fmt, "fmt ", 4) != 0 || hdr0.nChannels != 1 || hdr0.wBitsPerSample != 16 || hdr0.nSamplesPerSec != dsp_get_sampleraterx())
		{
			debug_printf_P(PSTR("Wrong format of file '%s'\n"), filename);
			playfile = 0;
			rc = f_close(& Fil);
			return;
		}
		FSIZE_t startdata = hdr0.FmtLen + 0x001C;
		PRINTF("startdata = %08lX\n", (unsigned long) startdata);
		rc = f_lseek(& Fil, startdata);
		if (rc)
		{
			debug_printf_P(PSTR("Can not seek to wav data of file '%s'\n"), filename);
			debug_printf_P(PSTR("Failed with rc=%u.\n"), rc);
			playfile = 0;
			rc = f_close(& Fil);
			return;
		}
		unsigned long offs = startdata % sizeof rbuff;
		rc = f_read(& Fil, rbuff + offs, sizeof rbuff - offs, & ibr);	/* Read a chunk of file */
		if (rc || ! ibr)
		{
			debug_printf_P(PSTR("1-st read Failed with rc=%u.\n"), rc);
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
			unsigned saved = savesamplesplay_user(rbuff, ibr - ipos);
			ipos += saved;
		}
	} while (0);

	if (endoffile)
	{
		playfile = 0;

		debug_printf_P("End file play.\n");
		rc = f_close(& Fil);
		if (rc)
		{
			TP();
			debug_printf_P(PSTR("Failed with rc=%u.\n"), rc);
			return;
		}

	}
}

#endif /* WITHWAVPLAYER || WITHSENDWAV */
