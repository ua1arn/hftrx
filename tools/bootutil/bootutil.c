/*
 * bootutil.c
 *
 *  Created on: 31 июл. 2023 г.
 *      Author: User
 */

#include <stdio.h>
#include <string.h>

// http://nskhuman.ru/allwinner/firmware.php?np=2&nf=3

typedef unsigned int u32;
typedef unsigned char u8;

#define TOC_MAIN_INFO_MAGIC 0x89119800

// Структура заголовка пакета boot_package.fex, имеющего размер 64 байта:

struct head_info // заголовок с общей информацией о пакете
{
	char name[16]; // пользователь может изменить
	u32 magic; // должен быть равен TOC_MAIN_INFO_MAGIC
	u32 add_sum; // контрольная сумма
	u32 serial_num; // пользователь может изменить
	u32 status; // пользователь может изменить
	u32 items_nr; // общее количество записей (файлов) = 4
	u32 valid_len; // размер файла boot_package.fex
	u32 version_main; // только один байт
	u32 version_sub; // два байта
	u32 reserved[3]; // зарезервировано
	u32 end;
};

//	Дальше лежат четыре заголовка для каждого item (т.е. содержимого упакованных файлов). Размер каждого заголовка 368 байт.
//	Структура заголовка каждого item:
//

struct item_info {
	char name[64]; // имя файла, например monitor
	u32 data_offset; // смещение
	u32 data_len; // размер
	u32 encrypt; // шифрование 0: нет AES, 1: AES
	u32 type; // 0: обычный файл, все равно
	// 1: ключ сертификата
	// 2: подпись сертификата
	// 3: бинарный файл
	u32 run_addr;	// если это bin-файл, то запускать по этому адресу; если нет, то должно быть 0
	u32 index;	// если это bin-файл, то значение показывает индекс для запуска
	// если это файл сертификата, он должен быть равен индексу файла bin,
	// это означает, что они в одной группе
	// должно быть = 0 для любого другого типа данных
	u32 reserved[69];	// зарезервировано на будущее;
	u32 end;
};

void getfileinfo(FILE * src, unsigned * length, unsigned * checksum)
{
	unsigned cks = 0;
	int c;

	* length = fseek(src, 0, SEEK_END);
	rewind(src);
	while ((c = fgetc(src)) != EOF)
		cks += (unsigned char) c;
	* checksum = cks;
}

void copyfile(FILE * dst, FILE * src)
{
	int c;

	rewind(src);
	while ((c = fgetc(src)) != EOF)
		fputc(c, dst);
}

void makedata(FILE * fp)
{
	FILE * datafile;
	struct head_info hi;
	struct item_info ii;
	unsigned i;

	datafile = fopen("", "rb");
	printf("sizeof (struct head_info) = %u\n", sizeof (struct head_info));
	printf("sizeof (struct item_info) = %u\n", sizeof (struct item_info));

	memset(& hi, 0, sizeof hi);
	memset(& ii, 0, sizeof ii);

	hi.magic = TOC_MAIN_INFO_MAGIC;
	hi.items_nr = 1;

	// Fill item info

	fclose(datafile);
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
