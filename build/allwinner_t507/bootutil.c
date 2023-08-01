/*
 * bootutil.c
 *
 *  Created on: 31 июл. 2023 г.
 *      Author: User
 */


#include <stdio.h>
#include <string.h>

// http://nskhuman.ru/allwinner/firmware.php?np=2&nf=3

// offset 0x2000
// offset 0x1004000

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

#define TOC_MAIN_INFO_MAGIC 0x89119800

#define STAMP_VALUE             0x5F0A6C39
#define ALIGN_SIZE              16 * 1024
#define MAGIC_SIZE              8
//#define STORAGE_BUFFER_SIZE    (256)
#define  TOC_MAIN_INFO_MAGIC   0x89119800
#define  TOC_MAIN_INFO_END     0x3b45494d
#define  TOC_ITEM_INFO_END     0x3b454949

static const char sunxi_package_sign [16] = "sunxi-package";

// https://github.com/smaeul/sun20i_d1_spl/blob/mainline/include/private_toc.h#L157
#define ITEM_PARAMETER_NAME             "parameter"
#define ITEM_OPTEE_NAME			"optee"
#define ITEM_SCP_NAME			"scp"
#define ITEM_MONITOR_NAME		"monitor"
#define ITEM_UBOOT_NAME			"u-boot"
#define ITEM_RTOS_NAME			"freertos"
#define ITEM_MELIS_NAME			"melis"
#define ITEM_MELIS_GZ_NAME		"melis-gz"
#define ITEM_MELIS_LZ4_NAME		"melis-lz4"
#define ITEM_MELIS_LZMA_NAME    "melis-lzma"
#define ITEM_MELIS_ZSTD_NAME    "melis-zstd"
#define ITEM_MELIS_CONFIG_NAME	"melis-config"
#define ITEM_OPENSBI_NAME		"opensbi"
#define ITEM_LOGO_NAME			"logo"
#define ITEM_DTB_NAME			"dtb"
#define ITEM_DTBO_NAME			"dtbo"
#define ITEM_SOCCFG_NAME		"soc-cfg"
#define ITEM_BDCFG_NAME			"board-cfg"
#define ITEM_ESM_IMG_NAME          "esm-img"
#define ITEM_SHUTDOWNCHARGE_LOGO_NAME	"shutdowncharge"
#define ITEM_ANDROIDCHARGE_LOGO_NAME	"androidcharge"
#define ITEM_EMMC_FW_NAME		"emmc-fw"

static const char item_signature [64] = ITEM_UBOOT_NAME;
//static const char item_signature [64] = "falcon-boot";

// Структура заголовка пакета boot_package.fex, имеющего размер 64 байта:

struct head_info // заголовок с общей информацией о пакете
{
	char name[16]; // пользователь может изменить
	uint32_t magic; // должен быть равен TOC_MAIN_INFO_MAGIC
	uint32_t add_sum; // контрольная сумма
	uint32_t serial_num; // пользователь может изменить
	uint32_t status; // пользователь может изменить
	uint32_t items_nr; // общее количество записей (файлов) = 4
	uint32_t valid_len; // размер файла boot_package.fex
	uint32_t version_main; // только один байт
	uint32_t version_sub; // два байта
	uint32_t reserved[3]; // зарезервировано
	uint32_t end;
};

//	Дальше лежат четыре заголовка для каждого item (т.е. содержимого упакованных файлов). Размер каждого заголовка 368 байт.
//	Структура заголовка каждого item:
//

struct item_info {
	char item_name[64]; // имя файла, например monitor
	uint32_t data_offset; // + 0x40 смещение
	uint32_t data_len; // + 0x44 размер
	uint32_t encrypt; // + 0x48 шифрование 0: нет AES, 1: AES
	uint32_t type; // + 0x4C  0: обычный файл, все равно
	// 1: ключ сертификата
	// 2: подпись сертификата
	// 3: бинарный файл
	uint32_t run_addr;	// + 0x50 если это bin-файл, то запускать по этому адресу; если нет, то должно быть 0
	uint32_t index;	// 0x54 если это bin-файл, то значение показывает индекс для запуска
	// если это файл сертификата, он должен быть равен индексу файла bin,
	// это означает, что они в одной группе
	// должно быть = 0 для любого другого типа данных
	uint32_t reserved[69];	// зарезервировано на будущее;
	uint32_t end;
};

unsigned calccks(const void * p, size_t len, unsigned cks)
{
	const uint32_t * data = (const uint32_t *) p;
	len = (len + 3) / 4;
	while (len --)
		cks += * data ++;

	return cks;
}

unsigned alignup(unsigned val, unsigned granulation)
{
	return (val + granulation - 1) & ~ (granulation - 1);
}

#define FILLV 0x00

void getfileinfo(FILE * src, long int * length, unsigned * checksum, unsigned cks)
{
	int c;

	fseek(src, 0, SEEK_END);
	* length = ftell(src);
	rewind(src);
	for (;;)
	{
		enum { CHUNK = 1024 };
		size_t n;
		unsigned char b [CHUNK];
		n = fread(b, 1, CHUNK, src);
		if (n == 0)
			break;
		else if (n < CHUNK)
			memset(b + n, FILLV, CHUNK - n);

		cks = calccks(b, n, cks);
	}
	* checksum = cks;
}

void copyfile(FILE * dst, FILE * src)
{
	int c;

	rewind(src);
	for (;;)
	{
		enum { CHUNK = 1024 };
		size_t n;
		unsigned char b [CHUNK];
		n = fread(b, 1, CHUNK, src);
		if (n == 0)
			break;
		fwrite(b, 1, n, dst);
	}
}

void fillfile(FILE * dst, size_t n)
{
	while (n --)
		fputc(FILLV, dst);
}

void makedata(FILE * fp, const char * datafilename, unsigned baseaddr)
{
	FILE * datafile;
	struct head_info hi;
	struct item_info ii [4];
	unsigned item;
	long int isize;
	unsigned datafilecks;
	unsigned dataoffset = alignup((sizeof hi + sizeof ii [0]), ALIGN_SIZE);
	unsigned headerpad = dataoffset - (sizeof hi + sizeof ii [0]);
	unsigned datapad;
	unsigned isizealigned;
	unsigned totaldatalen;

	datafile = fopen(datafilename, "rb");
	if (datafile == NULL)
		return;
	getfileinfo(datafile, & isize, & datafilecks, STAMP_VALUE);
	isizealigned = alignup(isize, ALIGN_SIZE);
	datapad = isizealigned - isize;
//	printf("sizeof (struct head_info) = %u\n", sizeof (struct head_info));
//	printf("sizeof (struct item_info) = %u\n", sizeof (struct item_info));
	printf("file: '%s', base=0x%08X, datafilecks=%08X, isize=%u\n", datafilename, baseaddr, datafilecks, isize);

	totaldatalen = 0;
	for (item = 0; item < 1; ++ item)
	{
		memset(& ii [item], 0, sizeof ii [item]);
		memcpy(ii [item].item_name, item_signature, sizeof ii [item].item_name);
		ii [item].type = 3;
		ii [item].run_addr = baseaddr;	// Loaded image start address
		ii [item].data_offset = dataoffset;
		ii [item].data_len = isizealigned;
		ii [item].end = TOC_ITEM_INFO_END;	// IIE; - не обязательно
		//printf("dataoffset=%08X\n", dataoffset);
		totaldatalen += isizealigned;
	}

	memset(& hi, 0, sizeof hi);
	memcpy(hi.name, sunxi_package_sign, sizeof hi.name);
	hi.magic = TOC_MAIN_INFO_MAGIC;
	hi.end = TOC_MAIN_INFO_END;	// MIE;
	hi.items_nr = sizeof ii / sizeof ii [0];

	// Fill item info
	hi.valid_len = sizeof hi + sizeof ii + headerpad + totaldatalen;
	hi.add_sum = calccks(& hi, sizeof hi, calccks(& ii, sizeof ii, datafilecks));


	// save result
	fwrite(& hi, sizeof hi, 1, fp);
	fwrite(& ii [0], sizeof ii, 1, fp);
	fillfile(fp, headerpad);

	for (item = 0; item < 1; ++ item)
	{
		copyfile(fp, datafile);
		fillfile(fp, datapad);
	}

	fclose(datafile);

	//printf("v=%u\n", alignup(77, 512));
}

int main(int argc, char **argv) {
	unsigned loadaddr = 0x40400100;
	const char * outfilename = "o.bin";
	const char * file = "tc1_awt507_app.bin";
	FILE * out;

	out = fopen(outfilename, "wb");
	if (out == NULL)
		return 1;

	makedata(out, file, loadaddr);

	fclose(out);
	return 0;
}
