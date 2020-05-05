/*
 * Copyright (c) 2017-2018, STMicroelectronics - All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//#include <asm/byteorder.h>
#include <errno.h>
#include <fcntl.h>
//#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <unistd.h>
#include "getopt_win.h"

#define __cpu_to_le32(v) (v)
#define __le32_to_cpu(v) (v)

/* Magic = 'S' 'T' 'M' 0x32 */
#define HEADER_MAGIC	0x324d5453  //	__be32_to_cpu(0x53544D32)
#define VER_MAJOR		2
#define VER_MINOR		1
#define VER_VARIANT		0
#define HEADER_VERSION_V1	0x1
#define TF_BINARY_TYPE		0x10
#define UB_BINARY_TYPE		0x00	// u-boot

/* Default option : bit0 => no signature */
#define HEADER_DEFAULT_OPTION	(__cpu_to_le32(0x00000001))

typedef unsigned long uint32_t;
typedef unsigned char uint8_t;

struct stm32_header {
	uint32_t magic_number;
	uint8_t image_signature[64];
	uint32_t image_checksum;
	uint8_t  header_version[4];
	uint32_t image_length;
	uint32_t image_entry_point;
	uint32_t reserved1;
	uint32_t load_address;
	uint32_t reserved2;
	uint32_t version_number;
	uint32_t option_flags;
	uint32_t ecdsa_algorithm;
	uint8_t ecdsa_public_key[64];
	uint8_t padding[83];
	uint8_t binary_type;
};

static struct stm32_header stm32image_header;

static void stm32image_default_header(struct stm32_header *ptr)
{
	if (!ptr) {
		return;
	}

	ptr->magic_number = HEADER_MAGIC;
	ptr->header_version[VER_MAJOR] = HEADER_VERSION_V1;
	ptr->option_flags = HEADER_DEFAULT_OPTION;
	ptr->ecdsa_algorithm = 1;
	ptr->version_number = 0;
	ptr->binary_type = TF_BINARY_TYPE;
}

static uint32_t stm32image_checksum(void *start, uint32_t len)
{
	uint32_t csum = 0;
	uint8_t *p;

	p = (unsigned char *)start;

	while (len > 0) {
		csum += *p;
		p++;
		len--;
	}

	return csum;
}

static void stm32image_print_header(const void *ptr)
{
	struct stm32_header *stm32hdr = (struct stm32_header *)ptr;

	printf("Image Type   : ST Microelectronics STM32 V%d.%d\n",
	       stm32hdr->header_version[VER_MAJOR],
	       stm32hdr->header_version[VER_MINOR]);
	printf("Image Size   : %lu bytes\n",
	       (unsigned long)__le32_to_cpu(stm32hdr->image_length));
	printf("Image Load   : 0x%08x\n",
	       __le32_to_cpu(stm32hdr->load_address));
	printf("Entry Point  : 0x%08x\n",
	       __le32_to_cpu(stm32hdr->image_entry_point));
	printf("Checksum     : 0x%08x\n",
	       __le32_to_cpu(stm32hdr->image_checksum));
	printf("Option     : 0x%08x\n",
	       __le32_to_cpu(stm32hdr->option_flags));
	printf("Version	   : 0x%08x\n",
	       __le32_to_cpu(stm32hdr->version_number));
}

static void stm32image_set_header(struct stm32_header *stm32hdr, uint32_t image_length, 
				  uint32_t loadaddr, uint32_t ep, uint32_t ver, uint32_t checksum)
{
	stm32image_default_header(stm32hdr);

	stm32hdr->load_address = __cpu_to_le32(loadaddr);
	stm32hdr->image_entry_point = __cpu_to_le32(ep);
	stm32hdr->image_length = __cpu_to_le32(image_length);
	stm32hdr->image_checksum = checksum;
	stm32hdr->version_number = __cpu_to_le32(ver);
}

static int stm32image_info_file(const char *srcname)
{
	FILE * src_fp;
	src_fp = fopen(srcname, "rb");
	if (src_fp == NULL) {
		fprintf(stderr, "Can't open %s: %s\n", srcname,
			strerror(errno));
		return -1;
	}
    if (fread(&stm32image_header, 1, sizeof stm32image_header, src_fp) != sizeof stm32image_header) {
		fprintf(stderr, "Can't read %s\n", srcname);
		return -1;
	}
	stm32image_print_header(& stm32image_header);
    fclose(src_fp);
    return 0;
}

static int stm32image_create_header_file(const char *srcname, const char *destname,
					 uint32_t loadaddr, uint32_t entry,
					 uint32_t version)
{
	FILE * src_fp;
    FILE * dest_fp;
	unsigned int filesize;
	unsigned char *ptr;
    unsigned long checksum;

	dest_fp = fopen(destname, "w+b");
	if (dest_fp == NULL) {
		fprintf(stderr, "Can't open %s: %s\n", destname,
			strerror(errno));
		return -1;
	}

	src_fp = fopen(srcname, "rb");
	if (src_fp == NULL) {
		fprintf(stderr, "Can't open %s: %s\n", srcname,
			strerror(errno));
		return -1;
	}

    fseek(src_fp, 0, SEEK_END);

    filesize = ftell(src_fp);

    rewind(src_fp);

    ptr = malloc(filesize);
 
	if (ptr == NULL) {
		fprintf(stderr, "Can't allocate memory for %s\n", srcname);
		return -1;
	}

    if (fread(ptr, 1, filesize, src_fp) != filesize) {
		fprintf(stderr, "Can't read %s\n", srcname);
		return -1;
	}

	memset(&stm32image_header, 0, sizeof (struct stm32_header));
    checksum = stm32image_checksum(ptr, filesize);
	stm32image_set_header(&stm32image_header, filesize, loadaddr, entry, version, checksum);

	if (fwrite(&stm32image_header, 1, sizeof(struct stm32_header), dest_fp) !=
	    sizeof(struct stm32_header)) {
		fprintf(stderr, "Write error %s: %s\n", destname,
			strerror(errno));
		return -1;
	}

	if (fwrite(ptr, 1, filesize, dest_fp) != filesize) {
		fprintf(stderr, "Write error on %s: %s\n", destname,
			strerror(errno));
		return -1;
	}

	free(ptr);
	fclose(src_fp);
	fclose(dest_fp);

	stm32image_print_header(& stm32image_header);
	return 0;
}

int main(int argc, char *argv[])
{
	int opt, loadaddr = -1, entry = -1, err = 0, version = 0;
	char *dest = NULL, *src = NULL, * info = NULL;

	while ((opt = getopt(argc, argv, ":s:d:l:e:v:i:")) != -1) {
		switch (opt) {
		case 's':
			src = optarg;
			break;
		case 'd':
			dest = optarg;
			break;
		case 'l':
			loadaddr = strtoul(optarg, NULL, 16);
			break;
		case 'i':
			info = optarg;
			break;
		case 'e':
			entry = strtoul(optarg, NULL, 16);
			break;
		case 'v':
			version = strtoul(optarg, NULL, 10);
			break;
		default:
			
				fprintf(stderr, "Usage :\n");
				fprintf(stderr, " %s -i srcfile\n", argv[0]);
				fprintf(stderr, " %s [-s srcfile] [-d destfile] [-l loadaddr] [-e entry_point]\n", argv[0]);
			return -1;
		}
	}

    if (info) {
	    err = stm32image_info_file(info);
        return err;
    }
	if (!src) {
		fprintf(stderr, "Missing -s option\n");
		return -1;
	}

	if (!dest) {
		fprintf(stderr, "Missing -d option\n");
		return -1;
	}

	if (loadaddr == -1) {
		fprintf(stderr, "Missing -l option\n");
		return -1;
	}

	if (entry == -1) {
		fprintf(stderr, "Missing -e option\n");
		return -1;
	}

	err = stm32image_create_header_file(src, dest, loadaddr,
					    entry, version);

	return err;
}
