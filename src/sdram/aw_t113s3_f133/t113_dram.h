/*
 * dram.h
 *
 *  Created on: Jul 30, 2023
 *      Author: Gena
 */

#ifndef SRC_SDRAM_AW_T113S3_F133_DRAM_H_
#define SRC_SDRAM_AW_T113S3_F133_DRAM_H_

#include "hardware.h"


typedef struct dram_para
{
	//normal configuration
	uint32_t        	dram_clk;
	uint32_t        	dram_type;	//dram_type DDR2: 2 DDR3: 3 LPDDR2: 6 LPDDR3: 7 DDR3L: 31
	//uint32_t        	lpddr2_type;	//LPDDR2 type S4:0 S2:1 NVM:2
	uint32_t        	dram_zq;	//do not need
	uint32_t		dram_odt_en;

	//control configuration
	uint32_t		dram_para1;
	uint32_t		dram_para2;

	//timing configuration
	uint32_t		dram_mr0;
	uint32_t		dram_mr1;
	uint32_t		dram_mr2;
	uint32_t		dram_mr3;
	uint32_t		dram_tpr0;	//DRAMTMG0
	uint32_t		dram_tpr1;	//DRAMTMG1
	uint32_t		dram_tpr2;	//DRAMTMG2
	uint32_t		dram_tpr3;	//DRAMTMG3
	uint32_t		dram_tpr4;	//DRAMTMG4
	uint32_t		dram_tpr5;	//DRAMTMG5
   	uint32_t		dram_tpr6;	//DRAMTMG8

	//reserved for future use
	uint32_t		dram_tpr7;
	uint32_t		dram_tpr8;
	uint32_t		dram_tpr9;
	uint32_t		dram_tpr10;
	uint32_t		dram_tpr11;
	uint32_t		dram_tpr12;
	uint32_t		dram_tpr13;
} dram_para_t;

signed int init_DRAM(int type, dram_para_t *para);


#endif /* SRC_SDRAM_AW_T113S3_F133_DRAM_H_ */
