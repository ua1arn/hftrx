/**
 * @file h3_lcdc_dump.c
 *
 */
/* Copyright (C) 2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>

///extern int uart0_printf(const char* fmt, ...);
///#define PRINTF uart0_printf

#include "h3.h"

/*
 * The LCD0 module is used for HDMI
 */

void h3_lcdc_dump(void) {
	PRINTF("LCD0 [%p]:\n", H3_LCD0);
	PRINTF(" GCTL         %08X\n", (unsigned) H3_LCD0->GCTL);
	PRINTF(" GINT0        %08X\n", (unsigned) H3_LCD0->GINT0);
	PRINTF(" GINT1        %08X\n", (unsigned) H3_LCD0->GINT1);
	PRINTF(" TCON1_CTL    %08X\n", (unsigned) H3_LCD0->TCON1_CTL);
	PRINTF(" TCON1_BASIC0 %08X\n", (unsigned) H3_LCD0->TCON1_BASIC0);
	PRINTF(" TCON1_BASIC1 %08X\n", (unsigned) H3_LCD0->TCON1_BASIC1);
	PRINTF(" TCON1_BASIC2 %08X\n", (unsigned) H3_LCD0->TCON1_BASIC2);
	PRINTF(" TCON1_BASIC3 %08X\n", (unsigned) H3_LCD0->TCON1_BASIC3);
	PRINTF(" TCON1_BASIC4 %08X\n", (unsigned) H3_LCD0->TCON1_BASIC4);
	PRINTF(" TCON1_BASIC5 %08X\n", (unsigned) H3_LCD0->TCON1_BASIC5);
}
