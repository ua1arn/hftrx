/**
 * @file h3_de2_dump.c
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

///extern int uart0_uart_print(const char* fmt, ...);
///#define PRINTF uart0_uart_print

#include "h3_de2.h"

#include "h3_ccu.h"
char bufl[100];

void h3_de2_dump(void) {
//
//
//
//	///sprintf(bufl," pll= %d\n",h3_cpu_get_clock());
//	sprintf(bufl," pll core= %d\n",sunxi_clock_get_corepll());
//	PRINTF(bufl);
//	sprintf(bufl," pll video= %d\n",clock_get_pll_video());
//	PRINTF(bufl);
//	PRINTF("DE2\n");
//	//PRINTF(" H3_CCU->PLL_DE_CTRL %p\n", H3_CCU->PLL_DE_CTRL);
//	sprintf(bufl," H3_CCU->PLL_DE_CTRL %p\n", H3_CCU->PLL_DE_CTRL);
//	PRINTF(bufl);
//	//PRINTF(" H3_CCU->DE_CLK %p\n", H3_CCU->DE_CLK);
//    sprintf(bufl," H3_CCU->DE_CLK %p\n", H3_CCU->DE_CLK);
//	PRINTF(bufl);
//	PRINTF(" MUX0\n");
//	PRINTF("  CLK\n");
//	//PRINTF("  GATE %p\n", H3_DE2->GATE);
//	sprintf(bufl,"GATE %p\n", H3_DE2->GATE);
//	PRINTF(bufl);
//	//PRINTF("  BUS  %p\n", H3_DE2->BUS);
//	sprintf(bufl,"  BUS %p\n", H3_DE2->BUS);
//	PRINTF(bufl);
//    //PRINTF("  RST  %p\n", H3_DE2->RST);
//	sprintf(bufl,"  RST  %p\n", H3_DE2->RST);
//	PRINTF(bufl);
//	//PRINTF("  DIV  %p\n", H3_DE2->DIV);
//	sprintf(bufl,"  DIV  %p\n", H3_DE2->DIV);
//	PRINTF(bufl);
////	PRINTF("  SEL  %p\n", H3_DE2->SEL);
//	sprintf(bufl,"  SEL  %p\n", H3_DE2->SEL);
//	PRINTF(bufl);
//	PRINTF(" GLB\n");
//	//PRINTF("  CTL     %p\n", H3_DE2_MUX0_GLB->CTL);
//	sprintf(bufl,"CTL %p\n", H3_DE2_MUX0_GLB->CTL);
//	PRINTF(bufl);
//	//PRINTF("  STATUS  %p\n", H3_DE2_MUX0_GLB->STATUS);
//	sprintf(bufl,"  STATUS  %p\n", H3_DE2_MUX0_GLB->STATUS);
//	PRINTF(bufl);
//    //PRINTF("  DBUFFER %p\n", H3_DE2_MUX0_GLB->DBUFFER);
//	sprintf(bufl,"  DBUFFER %p\n", H3_DE2_MUX0_GLB->DBUFFER);
//	PRINTF(bufl);
//	//PRINTF("  SIZE    %p\n", H3_DE2_MUX0_GLB->SIZE);
//	sprintf(bufl,"  SIZE    %d\n", H3_DE2_MUX0_GLB->SIZE);
//	PRINTF(bufl);
//	PRINTF(" BLD\n");
//	//PRINTF("  FCOLOR_CTL   %p\n", H3_DE2_MUX0_BLD->FCOLOR_CTL);
//	sprintf(bufl,"  FCOLOR_CTL   %p\n", H3_DE2_MUX0_BLD->FCOLOR_CTL);
//	PRINTF(bufl);
//	//PRINTF("  ROUTE        %p\n", H3_DE2_MUX0_BLD->ROUTE);
//	sprintf(bufl,"  ROUTE        %p\n", H3_DE2_MUX0_BLD->ROUTE);
//	PRINTF(bufl);
//	//PRINTF("  PREMULIPLY  %p\n", H3_DE2_MUX0_BLD->PREMULTIPLY);
//	sprintf(bufl,"  PREMULIPLY  %p\n", H3_DE2_MUX0_BLD->PREMULTIPLY);
//	PRINTF(bufl);
//	//PRINTF("  BKCOLOR      %p\n", H3_DE2_MUX0_BLD->BKCOLOR);
//	sprintf(bufl,"  BKCOLOR      %p\n", H3_DE2_MUX0_BLD->BKCOLOR);
//	PRINTF(bufl);
//	//PRINTF("  OUTPUT_SIZE  %p\n", H3_DE2_MUX0_BLD->OUTPUT_SIZE);
//	sprintf(bufl,"  OUTPUT_SIZE  %d\n", H3_DE2_MUX0_BLD->OUTPUT_SIZE);
//	PRINTF(bufl);
//	//PRINTF("  BLD_MODE[0]  %p\n", H3_DE2_MUX0_BLD->BLD_MODE[0]);
//	sprintf(bufl,"  BLD_MODE[0]  %p\n", H3_DE2_MUX0_BLD->BLD_MODE[0]);
//	PRINTF(bufl);
//	//PRINTF("  CK_CTL       %p\n", H3_DE2_MUX0_BLD->CK_CTL);
//	sprintf(bufl,"  CK_CTL       %p\n", H3_DE2_MUX0_BLD->CK_CTL);
//	PRINTF(bufl);
//	///PRINTF("  CK_CFG       %p\n", H3_DE2_MUX0_BLD->CK_CFG);
//	sprintf(bufl,"  CK_CFG       %p\n", H3_DE2_MUX0_BLD->CK_CFG);
//	PRINTF(bufl);
//	//PRINTF("  OUT_CTL      %p\n", H3_DE2_MUX0_BLD->OUT_CTL);
//	sprintf(bufl,"  OUT_CTL      %p\n", H3_DE2_MUX0_BLD->OUT_CTL);
//	PRINTF(bufl);
//	PRINTF("   ATTR[0]");
//	//PRINTF("    FCOLOR  %p\n", H3_DE2_MUX0_BLD->ATTR[0].FCOLOR);
//	sprintf(bufl,"    FCOLOR  %p\n", H3_DE2_MUX0_BLD->ATTR[0].FCOLOR);
//	PRINTF(bufl);
//	//PRINTF("    INSIZE  %p\n", H3_DE2_MUX0_BLD->ATTR[0].INSIZE);
//	sprintf(bufl,"    INSIZE  %d\n", H3_DE2_MUX0_BLD->ATTR[0].INSIZE);
//	PRINTF(bufl);
//	//PRINTF("    OFFSET  %p\n", H3_DE2_MUX0_BLD->ATTR[0].OFFSET);
//	sprintf(bufl,"    OFFSET  %d\n", H3_DE2_MUX0_BLD->ATTR[0].OFFSET);
//	PRINTF(bufl);
//
//	PRINTF(" CSC\n");
//	//PRINTF("  CTL %p\n", H3_DE2_MUX0_CSC->CTL);
//	sprintf(bufl,"  CTL %p\n", H3_DE2_MUX0_CSC->CTL);
//	PRINTF(bufl);
//	PRINTF(" UI\n");
//	//PRINTF("  TOP_HADDR %p\n", H3_DE2_MUX0_UI->TOP_HADDR);
//	sprintf(bufl,"  TOP_HADDR %p\n", H3_DE2_MUX0_UI->TOP_HADDR);
//	PRINTF(bufl);
//	//PRINTF("  BOT_HADDR %p\n", H3_DE2_MUX0_UI->BOT_HADDR);
//	sprintf(bufl,"  BOT_HADDR %p\n", H3_DE2_MUX0_UI->BOT_HADDR);
//	PRINTF(bufl);
//	//PRINTF("  OVL_SIZE  %p\n", H3_DE2_MUX0_UI->OVL_SIZE);
//	sprintf(bufl,"  OVL_SIZE  %d\n", H3_DE2_MUX0_UI->OVL_SIZE);
//	PRINTF(bufl);
//	PRINTF("  CFG[0]\n");
//	//PRINTF("   ATTR      %p\n", H3_DE2_MUX0_UI->CFG[0].ATTR);
//	sprintf(bufl,"   ATTR      %p\n", H3_DE2_MUX0_UI->CFG[0].ATTR);
//	PRINTF(bufl);
//	//PRINTF("   SIZE      %p\n", H3_DE2_MUX0_UI->CFG[0].SIZE);
//	sprintf(bufl,"   SIZE      %d\n", H3_DE2_MUX0_UI->CFG[0].SIZE);
//	PRINTF(bufl);
//	//PRINTF("   COORD     %p\n", H3_DE2_MUX0_UI->CFG[0].COORD);
//	sprintf(bufl,"   COORD     %p\n", H3_DE2_MUX0_UI->CFG[0].COORD);
//	PRINTF(bufl);
//	//PRINTF("   PITCH     %p\n", H3_DE2_MUX0_UI->CFG[0].PITCH);
//	sprintf(bufl,"   PITCH     %p\n", H3_DE2_MUX0_UI->CFG[0].PITCH);
//	PRINTF(bufl);
//	//PRINTF("   TOP_LADDR %p\n", H3_DE2_MUX0_UI->CFG[0].TOP_LADDR);
//	sprintf(bufl,"  TOP_LADDR %p\n", H3_DE2_MUX0_UI->CFG[0].TOP_LADDR);
//	PRINTF(bufl);
//	//PRINTF("   BOT_LADDR %p\n", H3_DE2_MUX0_UI->CFG[0].BOT_LADDR);
//	sprintf(bufl,"   BOT_LADDR %p\n", H3_DE2_MUX0_UI->CFG[0].BOT_LADDR);
//	PRINTF(bufl);
//	//PRINTF("   FCOLOR    %p\n", H3_DE2_MUX0_UI->CFG[0].FCOLOR);
//	sprintf(bufl,"   FCOLOR    %p\n", H3_DE2_MUX0_UI->CFG[0].FCOLOR);
//	PRINTF(bufl);
}

