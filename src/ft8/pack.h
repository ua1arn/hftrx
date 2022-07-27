#ifndef _INCLUDE_PACK_H_
#define _INCLUDE_PACK_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * FT8 implementation given from https://github.com/kgoba/ft8_lib,
 * adapted to Storch by RA4ASN
 */
#include "hardware.h"

#if WITHFT8


    // Pack FT8 text message into 72 bits
    // [IN] msg      - FT8 message (e.g. "CQ TE5T KN01")
    // [OUT] c77     - 10 byte array to store the 77 bit payload (MSB first)
    int pack77(const char* msg, uint8_t* c77);

#endif /* WITHFT8 */

#ifdef __cplusplus
}
#endif

#endif // _INCLUDE_PACK_H_
