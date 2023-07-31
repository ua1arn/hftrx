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

#define UBOOT_MAGIC             "uboot"
#define STAMP_VALUE             0x5F0A6C39
#define ALIGN_SIZE              16 * 1024
#define MAGIC_SIZE              8
#define STORAGE_BUFFER_SIZE    (256)

static const char sunxi_package_sign [16] = "sunxi-package";
static const char uboot_sign [64] = "u-boot";

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
	char name[64]; // имя файла, например monitor
	uint32_t data_offset; // смещение
	uint32_t data_len; // размер
	uint32_t encrypt; // шифрование 0: нет AES, 1: AES
	uint32_t type; // 0: обычный файл, все равно
	// 1: ключ сертификата
	// 2: подпись сертификата
	// 3: бинарный файл
	uint32_t run_addr;	// если это bin-файл, то запускать по этому адресу; если нет, то должно быть 0
	uint32_t index;	// если это bin-файл, то значение показывает индекс для запуска
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

void makedata(FILE * fp)
{
	FILE * datafile;
	struct head_info hi;
	struct item_info ii;
	unsigned i;
	long int isize;
	unsigned cks;
	unsigned dataoffset = alignup((sizeof hi + sizeof ii), 2048);
	unsigned datapad = dataoffset - (sizeof hi + sizeof ii);

	datafile = fopen("tc1_awt507_app.bin", "rb");
	if (datafile == NULL)
		return;
	getfileinfo(datafile, & isize, & cks, STAMP_VALUE);
	printf("sizeof (struct head_info) = %u\n", sizeof (struct head_info));
	printf("sizeof (struct item_info) = %u\n", sizeof (struct item_info));
	printf("ckecksum=%08X, isize=%u\n", cks, isize);


	memset(& hi, 0, sizeof hi);
	memset(& ii, 0, sizeof ii);

	memcpy(hi.name, sunxi_package_sign, sizeof hi.name);
	hi.magic = TOC_MAIN_INFO_MAGIC;
	hi.end = 0x3B45494D;	// MIE;
	hi.items_nr = 1;

	// Fill item info
	hi.valid_len = sizeof hi + sizeof ii + datapad + isize;
	hi.add_sum = calccks(& hi, sizeof hi, STAMP_VALUE);

	memcpy(ii.name, uboot_sign, sizeof ii.name);
	ii.run_addr = 0x40800000 + 256;
	ii.data_offset = dataoffset;
	ii.end = 0x3B454949;	// IIE;

	// save result
	fwrite(& hi, sizeof hi, 1, fp);
	fwrite(& ii, sizeof ii, 1, fp);
	fillfile(fp, datapad);
	copyfile(fp, datafile);


	fclose(datafile);

	printf("v=%u\n", alignup(77, 512));
}

int main(int argc, char **argv) {
	FILE * out;

	out = fopen("o.bin", "wb");
	if (out == NULL)
		return 1;

	makedata(out);

	fclose(out);
	return 0;
}
