/*
 * File:   ddr.h
 * Author: oen
 *
 * Created on March 26, 2016, 12:52 PM
 */

#ifndef DDR_H
#define DDR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ddr_cfg.h"

#define ROM_DDR_INIT 0
#define POST_DDR_CFG_POINTER_NULL 0x1
#define POST_DDR_PHY_INIT_FAILED 0x2
#define POST_DDR_TESTMEM_FAILED 0x3

#define DDR_DEFAULT_FREQ 432

// Function: init_ddr3
//
// DDR memory initialization
//
// Parameters:
//      MHz       - frequency to setup,
//                     it is recommended to use  DDR_DEFAULT_FREQ paramter
//      bDDRMC0   - may be 1 or 0 - initialize or not DDR-0 controller
//      bDDRMC1   - may be 1 or 0 - initialize or not DDR-1 controller
//
// Return:
//		0 - memory is initialized
//		other - memory is not initialized
//          1 - #define POST_DDR_CFG_POINTER_NULL           0x1
//          2 - #define POST_DDR_PHY_INIT_FAILED            0x2
//          3 - #define POST_DDR_TESTMEM_FAILED             0x3
//
int init_ddr3(unsigned int MHz, int bDDRMC0, int bDDRMC1);

#ifdef __cplusplus
}
#endif

#endif /* DDR_H */
