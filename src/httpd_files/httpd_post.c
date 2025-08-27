/*
 * httpd_post.c
 *
 *  Created on: Dec 10, 2020
 *      Author: gena
 */

/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	/* зависящие от процессора функции работы с портами */

#if WITHLWIP


#include <stdbool.h>
#include "lwip/opt.h"

#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/ip.h"
#include "lwip/udp.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"
#include "lwip/ip_addr.h"

#include "../lib/dhcp-server/dhserver.h"
#include "../lib/dns-server/dnserver.h"
#include "lwip/apps/httpd.h"


#if LWIP_HTTPD_SUPPORT_POST

static int writeok;
static unsigned bufoffset;
static ALIGNX_BEGIN RAMNOINIT_D1 uint8_t appbuff [4096uL * 1024] ALIGNX_END;


static const uint8_t * findpattern(const uint8_t * buff, unsigned size, const void * pbuff, unsigned psize)
{
	while (size >= psize)
	{
		if (memcmp(buff, pbuff, psize) == 0)
			return buff + psize;		// found
		++ buff;
		-- size;
		const uint8_t * p = memchr(buff, * (const uint8_t *) pbuff, size);
		if (p != NULL)
		{
			ptrdiff_t skip = p - buff;
			size -= skip;
			buff += skip;
		}
		else
		{
			break;
		}
	}
	return NULL;
}

/* These functions must be implemented by the application */

/** Called when a POST request has been received. The application can decide
 * whether to accept it or not.
 *
 * @param connection Unique connection identifier, valid until httpd_post_end
 *        is called.
 * @param uri The HTTP header URI receiving the POST request.
 * @param http_request The raw HTTP request (the first packet, normally).
 * @param http_request_len Size of 'http_request'.
 * @param content_len Content-Length from HTTP header.
 * @param response_uri Filename of response file, to be filled when denying the
 *        request
 * @param response_uri_len Size of the 'response_uri' buffer.
 * @param post_auto_wnd Set this to 0 to let the callback code handle window
 *        updates by calling 'httpd_post_data_recved' (to throttle rx speed)
 *        default is 1 (httpd handles window updates automatically)
 * @return ERR_OK: Accept the POST request, data may be passed in
 *         another err_t: Deny the POST request, send back 'bad request'.
 */
err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       u16_t http_request_len, int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd)
{
	PRINTF("httpd_post_begin, url='%s', content_len=%u\n", uri, content_len);
	//printhex(0, http_request, http_request_len);

	writeok = 1;
	bufoffset = 0;
	//local_snprintf_P(response_uri, response_uri_len, "/error.html");
	return ERR_OK;
}

/** Called for each pbuf of data that has been received for a POST.
 * ATTENTION: The application is responsible for freeing the pbufs passed in!
 *
 * @param connection Unique connection identifier.
 * @param p Received data.
 * @return ERR_OK: Data accepted.
 *         another err_t: Data denied, http_post_get_response_uri will be called.
 */
err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
	//PRINTF("httpd_post_receive_data\n");
	struct pbuf *q;
	for (q = p; q != NULL; q = q->next)
	{
		//PRINTF("httpd_post_receive_data: @%p len=%u\n", q->payload, q->len);
		//printhex(bufoffset, q->payload, q->len);
		size_t part = ulmin(sizeof appbuff - bufoffset, q->len);
		memcpy(appbuff + bufoffset, q->payload, part);
		bufoffset += q->len;
	}
	return ERR_OK;
}


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
} ATTRPACKED;

#define HEADER_MAGIC	0x324d5453  //	__be32_to_cpu(0x53544D32)

uint_fast8_t zbootloader_vfycks(
		const uint8_t * apparea,	/* ������� ����� ��� �������� ������ - ����� ����� ��������� ����� */
		const uint8_t * body	/* ������� ����� ��� �������� ������ - ����� ����� ��������� ����� */
		)
{
	const volatile struct stm32_header * const hdr = (const volatile struct stm32_header *) apparea;
	uint_fast32_t checksum = hdr->image_checksum;
	uint_fast32_t length = hdr->image_length;
	const uint8_t * p = (const uint8_t *) body; //hdr->load_address;
	if (hdr->magic_number != HEADER_MAGIC)
		return 1;
	while (length --)
		checksum -= * p ++;
	return checksum != 0;	// ������� 0 ���� ����������� ����� �������
}

/** Called when all data is received or when the connection is closed.
 * The application must return the filename/URI of a file to send in response
 * to this POST request. If the response_uri buffer is untouched, a 404
 * response is returned.
 *
 * @param connection Unique connection identifier.
 * @param response_uri Filename of response file, to be filled when denying the request
 * @param response_uri_len Size of the 'response_uri' buffer.
 */
void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
	static uint8_t eofmarker [128];
	size_t eofmarkerlen = 0;
	unsigned startoffset = 0;
	unsigned endoffset = 0;

	(void) endoffset;
	PRINTF("httpd_post_finished, bufoffset=%u\n", bufoffset);

	if (writeok)
	{
		// ������ �����
		const uint8_t * eol = memchr(appbuff, 0x0D, bufoffset);
		if (eol != NULL)
		{
			size_t len = eol - appbuff;
			memcpy(eofmarker + 0, "\r\n", 2);
			memcpy(eofmarker + 2, appbuff, len);
			memcpy(eofmarker + len + 2, "--\r\n", 4);
			eofmarkerlen = len + 6;
		}
		else
		{
			writeok = 0;
		}
	}

	if (writeok)
	{
		const uint8_t * sob = findpattern(appbuff, bufoffset, "\r\n\r\n", 4);
		if (sob != NULL)
			startoffset = (sob - appbuff);
		else
			writeok = 0;

	}
	else
	{
		writeok = 0;
	}

	if (writeok)
	{
		const uint8_t * sob = findpattern(appbuff, bufoffset, eofmarker, eofmarkerlen);
		if (sob != NULL)
			endoffset = (sob - appbuff) - eofmarkerlen;
		else
			writeok = 0;

	}

	if (writeok)
	{
		writeok = ! zbootloader_vfycks(appbuff + startoffset, appbuff + startoffset + 256);
		if (writeok == 0)
		{
			PRINTF("image length=%u, wrong checksum\n", endoffset - startoffset);

		}
		else
		{
			PRINTF("image length=%u, right checksum\n", endoffset - startoffset);
		}
	}
#if 0
	if (writeok)
	{
		do
		{
			enum { SECTORSIZE = 512 };
			BYTE targetdrv = 0;
			DSTATUS st = disk_initialize (targetdrv);				/* Physical drive nmuber (0..) */
			//PRINTF(PSTR("disk_initialize code=%02X\n"), st);
			if (st != RES_OK)
			{
				PRINTF("BOOTLOADER: SD not ready\n");
				display_vtty_printf("BOOTLOADER: SD not ready\n");
				break;
			}
			display_vtty_printf("BOOTLOADER: size of image=%08lX\n", endoffset - startoffset);
			st = disk_write(targetdrv, appbuff + startoffset, SDCARDLOCATION, (endoffset - startoffset + SECTORSIZE - 1) / SECTORSIZE);
			if (st != RES_OK)
			{
				PRINTF("BOOTLOADER: Can not write image\n");
				display_vtty_printf("BOOTLOADER: Can not write image\n");
				writeok = 0;
			}
			else
			{
				PRINTF("BOOTLOADER: Done write image\n");

			}
		} while (0);

	}
#endif
	local_snprintf_P(response_uri, response_uri_len, writeok ? "/done.html" : "/error.html");
}

#ifndef LWIP_HTTPD_POST_MANUAL_WND
#define LWIP_HTTPD_POST_MANUAL_WND  0
#endif

#if LWIP_HTTPD_POST_MANUAL_WND
void httpd_post_data_recved(void *connection, u16_t recved_len);
#endif /* LWIP_HTTPD_POST_MANUAL_WND */

#endif /* LWIP_HTTPD_SUPPORT_POST */

#endif /* WITHLWIP */
