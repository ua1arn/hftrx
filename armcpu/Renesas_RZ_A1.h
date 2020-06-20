/**************************************************************************//**
 * @file     Renesas_RZ_A1.h
 * @brief    CMSIS Cortex-A9 Core Peripheral Access Layer Header File
 *           for the Renesas_RZ_A1
 * @version
 * @date     9 Sept 2014
 *
 * @note
 *
 ******************************************************************************/

/* Copyright (C) 2013-2017 Renesas Electronics Corporation. All rights reserved */
#ifndef __Renesas_RZ_A1_H__
#define __Renesas_RZ_A1_H__

#include "mcu_board_select.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/******  SGI Interrupts Numbers                 ****************************************/
  SGI0_IRQn           =  0,                                       //!< SGI0_IRQn
  SGI1_IRQn           =  1,                                       //!< SGI1_IRQn
  SGI2_IRQn           =  2,                                       //!< SGI2_IRQn
  SGI3_IRQn           =  3,                                       //!< SGI3_IRQn
  SGI4_IRQn           =  4,                                       //!< SGI4_IRQn
  SGI5_IRQn           =  5,                                       //!< SGI5_IRQn
  SGI6_IRQn           =  6,                                       //!< SGI6_IRQn
  SGI7_IRQn           =  7,                                       //!< SGI7_IRQn
  SGI8_IRQn           =  8,                                       //!< SGI8_IRQn
  SGI9_IRQn           =  9,                                       //!< SGI9_IRQn
  SGI10_IRQn          = 10,                                       //!< SGI10_IRQn
  SGI11_IRQn          = 11,                                       //!< SGI11_IRQn
  SGI12_IRQn          = 12,                                       //!< SGI12_IRQn
  SGI13_IRQn          = 13,                                       //!< SGI13_IRQn
  SGI14_IRQn          = 14,                                       //!< SGI14_IRQn
  SGI15_IRQn          = 15,                                       //!< SGI15_IRQn

/******  Cortex-A9 Processor Exceptions Numbers ****************************************/
  /* 16 - 578 */
  PMUIRQ0_IRQn         = 16,                                      //!< PMUIRQ0_IRQn
  COMMRX0_IRQn         = 17,                                      //!< COMMRX0_IRQn
  COMMTX0_IRQn         = 18,                                      //!< COMMTX0_IRQn
  CTIIRQ0_IRQn         = 19,                                      //!< CTIIRQ0_IRQn
  /* 20-31 Reserved */
  IRQ0_IRQn            = 32,                                      //!< IRQ0_IRQn
  IRQ1_IRQn            = 33,                                      //!< IRQ1_IRQn
  IRQ2_IRQn            = 34,                                      //!< IRQ2_IRQn
  IRQ3_IRQn            = 35,                                      //!< IRQ3_IRQn
  IRQ4_IRQn            = 36,                                      //!< IRQ4_IRQn
  IRQ5_IRQn            = 37,                                      //!< IRQ5_IRQn
  IRQ6_IRQn            = 38,                                      //!< IRQ6_IRQn
  IRQ7_IRQn            = 39,                                      //!< IRQ7_IRQn

  PL310ERR_IRQn        = 40,                                      //!< PL310ERR_IRQn

  DMAINT0_IRQn         = 41,        /*!< DMAC Interrupt         *///!< DMAINT0_IRQn
  DMAINT1_IRQn         = 42,        /*!< DMAC Interrupt         *///!< DMAINT1_IRQn
  DMAINT2_IRQn         = 43,        /*!< DMAC Interrupt         *///!< DMAINT2_IRQn
  DMAINT3_IRQn         = 44,        /*!< DMAC Interrupt         *///!< DMAINT3_IRQn
  DMAINT4_IRQn         = 45,        /*!< DMAC Interrupt         *///!< DMAINT4_IRQn
  DMAINT5_IRQn         = 46,        /*!< DMAC Interrupt         *///!< DMAINT5_IRQn
  DMAINT6_IRQn         = 47,        /*!< DMAC Interrupt         *///!< DMAINT6_IRQn
  DMAINT7_IRQn         = 48,        /*!< DMAC Interrupt         *///!< DMAINT7_IRQn
  DMAINT8_IRQn         = 49,        /*!< DMAC Interrupt         *///!< DMAINT8_IRQn
  DMAINT9_IRQn         = 50,        /*!< DMAC Interrupt         *///!< DMAINT9_IRQn
  DMAINT10_IRQn        = 51,        /*!< DMAC Interrupt         *///!< DMAINT10_IRQn
  DMAINT11_IRQn        = 52,        /*!< DMAC Interrupt         *///!< DMAINT11_IRQn
  DMAINT12_IRQn        = 53,        /*!< DMAC Interrupt         *///!< DMAINT12_IRQn
  DMAINT13_IRQn        = 54,        /*!< DMAC Interrupt         *///!< DMAINT13_IRQn
  DMAINT14_IRQn        = 55,        /*!< DMAC Interrupt         *///!< DMAINT14_IRQn
  DMAINT15_IRQn        = 56,        /*!< DMAC Interrupt         *///!< DMAINT15_IRQn
  DMAERR_IRQn          = 57,        /*!< DMAC Interrupt         *///!< DMAERR_IRQn

  /* 58-72 Reserved */

  USBI0_IRQn           = 73,                                      //!< USBI0_IRQn
  USBI1_IRQn           = 74,                                      //!< USBI1_IRQn

  S0_VI_VSYNC0_IRQn    = 75,                                      //!< S0_VI_VSYNC0_IRQn
  S0_LO_VSYNC0_IRQn    = 76,                                      //!< S0_LO_VSYNC0_IRQn
  S0_VSYNCERR0_IRQn    = 77,                                      //!< S0_VSYNCERR0_IRQn
  GR3_VLINE0_IRQn      = 78,                                      //!< GR3_VLINE0_IRQn
  S0_VFIELD0_IRQn      = 79,                                      //!< S0_VFIELD0_IRQn
  IV1_VBUFERR0_IRQn    = 80,                                      //!< IV1_VBUFERR0_IRQn
  IV3_VBUFERR0_IRQn    = 81,                                      //!< IV3_VBUFERR0_IRQn
  IV5_VBUFERR0_IRQn    = 82,                                      //!< IV5_VBUFERR0_IRQn
  IV6_VBUFERR0_IRQn    = 83,                                      //!< IV6_VBUFERR0_IRQn
  S0_WLINE0_IRQn       = 84,                                      //!< S0_WLINE0_IRQn
#if (TARGET_RZA1 <= TARGET_RZA1LU)
  /* 85-126 Reserved */
  JEDI_IRQn            = 126,                                     //!< JEDI_IRQn
  JDTI_IRQn            = 127,                                     //!< JDTI_IRQn
  /* 128-133 Reserved */
#else
  S1_VI_VSYNC0_IRQn    = 85,
  S1_LO_VSYNC0_IRQn    = 86,
  S1_VSYNCERR0_IRQn    = 87,
  S1_VFIELD0_IRQn      = 88,
  IV2_VBUFERR0_IRQn    = 89,
  IV4_VBUFERR0_IRQn    = 90,
  S1_WLINE0_IRQn       = 91,
  OIR_VI_VSYNC0_IRQn   = 92,
  OIR_LO_VSYNC0_IRQn   = 93,
  OIR_VSYNCERR0_IRQn   = 94,
  OIR_VFIELD0_IRQn     = 95,
  IV7_VBUFERR0_IRQn    = 96,
  IV8_VBUFERR0_IRQn    = 97,
  /* 98 Reserved */
  S0_VI_VSYNC1_IRQn    = 99,
  S0_LO_VSYNC1_IRQn    = 100,
  S0_VSYNCERR1_IRQn    = 101,
  GR3_VLINE1_IRQn      = 102,
  S0_VFIELD1_IRQn      = 103,
  IV1_VBUFERR1_IRQn    = 104,
  IV3_VBUFERR1_IRQn    = 105,
  IV5_VBUFERR1_IRQn    = 106,
  IV6_VBUFERR1_IRQn    = 107,
  S0_WLINE1_IRQn       = 108,
  S1_VI_VSYNC1_IRQn    = 109,
  S1_LO_VSYNC1_IRQn    = 110,
  S1_VSYNCERR1_IRQn    = 111,
  S1_VFIELD1_IRQn      = 112,
  IV2_VBUFERR1_IRQn    = 113,
  IV4_VBUFERR1_IRQn    = 114,
  S1_WLINE1_IRQn       = 115,
  OIR_VI_VSYNC1_IRQn   = 116,
  OIR_LO_VSYNC1_IRQn   = 117,
  OIR_VSYNCERR1_IRQn   = 118,
  OIR_VFIELD1_IRQn     = 119,
  IV7_VBUFERR1_IRQn    = 120,
  IV8_VBUFERR1_IRQn    = 121,
  /* Reserved = 122 */

  IMRDI_IRQn           = 123,
  IMR2I0_IRQn          = 124,
  IMR2I1_IRQn          = 125,

  JEDI_IRQn            = 126,
  JDTI_IRQn            = 127,

  CMP0_IRQn            = 128,
  CMP1_IRQn            = 129,

  INT0_IRQn            = 130,
  INT1_IRQn            = 131,
  INT2_IRQn            = 132,
  INT3_IRQn            = 133,
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
  OSTMI0TINT_IRQn      = 134,       /*!< OSTM Interrupt         *///!< OSTMI0TINT_IRQn
  OSTMI1TINT_IRQn      = 135,       /*!< OSTM Interrupt         *///!< OSTMI1TINT_IRQn

  CMI_IRQn             = 136,                                     //!< CMI_IRQn
  WTOUT_IRQn           = 137,                                     //!< WTOUT_IRQn

  ITI_IRQn             = 138,                                     //!< ITI_IRQn

  TGI0A_IRQn           = 139,                                     //!< TGI0A_IRQn
  TGI0B_IRQn           = 140,                                     //!< TGI0B_IRQn
  TGI0C_IRQn           = 141,                                     //!< TGI0C_IRQn
  TGI0D_IRQn           = 142,                                     //!< TGI0D_IRQn
  TGI0V_IRQn           = 143,                                     //!< TGI0V_IRQn
  TGI0E_IRQn           = 144,                                     //!< TGI0E_IRQn
  TGI0F_IRQn           = 145,                                     //!< TGI0F_IRQn
  TGI1A_IRQn           = 146,                                     //!< TGI1A_IRQn
  TGI1B_IRQn           = 147,                                     //!< TGI1B_IRQn
  TGI1V_IRQn           = 148,                                     //!< TGI1V_IRQn
  TGI1U_IRQn           = 149,                                     //!< TGI1U_IRQn
  TGI2A_IRQn           = 150,                                     //!< TGI2A_IRQn
  TGI2B_IRQn           = 151,                                     //!< TGI2B_IRQn
  TGI2V_IRQn           = 152,                                     //!< TGI2V_IRQn
  TGI2U_IRQn           = 153,                                     //!< TGI2U_IRQn
  TGI3A_IRQn           = 154,                                     //!< TGI3A_IRQn
  TGI3B_IRQn           = 155,                                     //!< TGI3B_IRQn
  TGI3C_IRQn           = 156,                                     //!< TGI3C_IRQn
  TGI3D_IRQn           = 157,                                     //!< TGI3D_IRQn
  TGI3V_IRQn           = 158,                                     //!< TGI3V_IRQn
  TGI4A_IRQn           = 159,                                     //!< TGI4A_IRQn
  TGI4B_IRQn           = 160,                                     //!< TGI4B_IRQn
  TGI4C_IRQn           = 161,                                     //!< TGI4C_IRQn
  TGI4D_IRQn           = 162,                                     //!< TGI4D_IRQn
  TGI4V_IRQn           = 163,                                     //!< TGI4V_IRQn

#if (TARGET_RZA1 <= TARGET_RZA1LU)
/* 164-169 Reserved */
#else
  CMI1_IRQn            = 164,
  CMI2_IRQn            = 165,

  SGDEI0_IRQn          = 166,
  SGDEI1_IRQn          = 167,
  SGDEI2_IRQn          = 168,
  SGDEI3_IRQn          = 169,
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */

  ADI_IRQn             = 170,                                     //!< ADI_IRQn
  LMTI_IRQn            = 171,                                     //!< LMTI_IRQn

  SSII0_IRQn           = 172,       /*!< SSIF Interrupt         *///!< SSII0_IRQn
  SSIRXI0_IRQn         = 173,       /*!< SSIF Interrupt         *///!< SSIRXI0_IRQn
  SSITXI0_IRQn         = 174,       /*!< SSIF Interrupt         *///!< SSITXI0_IRQn
  SSII1_IRQn           = 175,       /*!< SSIF Interrupt         *///!< SSII1_IRQn
  SSIRXI1_IRQn         = 176,       /*!< SSIF Interrupt         *///!< SSIRXI1_IRQn
  SSITXI1_IRQn         = 177,       /*!< SSIF Interrupt         *///!< SSITXI1_IRQn
  SSII2_IRQn           = 178,       /*!< SSIF Interrupt         *///!< SSII2_IRQn
  SSIRTI2_IRQn         = 179,       /*!< SSIF Interrupt         *///!< SSIRTI2_IRQn
  SSII3_IRQn           = 180,       /*!< SSIF Interrupt         *///!< SSII3_IRQn
  SSIRXI3_IRQn         = 181,       /*!< SSIF Interrupt         *///!< SSIRXI3_IRQn
  SSITXI3_IRQn         = 182,       /*!< SSIF Interrupt         *///!< SSITXI3_IRQn
#if (TARGET_RZA1 <= TARGET_RZA1LU)
  /* 183-187 Reserved */
#else
  SSII4_IRQn           = 183,       /*!< SSIF Interrupt         */
  SSIRTI4_IRQn         = 184,       /*!< SSIF Interrupt         */
  SSII5_IRQn           = 185,       /*!< SSIF Interrupt         */
  SSIRXI5_IRQn         = 186,       /*!< SSIF Interrupt         */
  SSITXI5_IRQn         = 187,       /*!< SSIF Interrupt         */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */

  SPDIFI_IRQn          = 188,                                     //!< SPDIFI_IRQn

  INTIICTEI0_IRQn      = 189,       /*!< RIIC Interrupt         *///!< INTIICTEI0_IRQn
  INTIICRI0_IRQn       = 190,       /*!< RIIC Interrupt         *///!< INTIICRI0_IRQn
  INTIICTI0_IRQn       = 191,       /*!< RIIC Interrupt         *///!< INTIICTI0_IRQn
  INTIICSPI0_IRQn      = 192,       /*!< RIIC Interrupt         *///!< INTIICSPI0_IRQn
  INTIICSTI0_IRQn      = 193,       /*!< RIIC Interrupt         *///!< INTIICSTI0_IRQn
  INTIICNAKI0_IRQn     = 194,       /*!< RIIC Interrupt         *///!< INTIICNAKI0_IRQn
  INTIICALI0_IRQn      = 195,       /*!< RIIC Interrupt         *///!< INTIICALI0_IRQn
  INTIICTMOI0_IRQn     = 196,       /*!< RIIC Interrupt         *///!< INTIICTMOI0_IRQn
  INTIICTEI1_IRQn      = 197,       /*!< RIIC Interrupt         *///!< INTIICTEI1_IRQn
  INTIICRI1_IRQn       = 198,       /*!< RIIC Interrupt         *///!< INTIICRI1_IRQn
  INTIICTI1_IRQn       = 199,       /*!< RIIC Interrupt         *///!< INTIICTI1_IRQn
  INTIICSPI1_IRQn      = 200,       /*!< RIIC Interrupt         *///!< INTIICSPI1_IRQn
  INTIICSTI1_IRQn      = 201,       /*!< RIIC Interrupt         *///!< INTIICSTI1_IRQn
  INTIICNAKI1_IRQn     = 202,       /*!< RIIC Interrupt         *///!< INTIICNAKI1_IRQn
  INTIICALI1_IRQn      = 203,       /*!< RIIC Interrupt         *///!< INTIICALI1_IRQn
  INTIICTMOI1_IRQn     = 204,       /*!< RIIC Interrupt         *///!< INTIICTMOI1_IRQn
  INTIICTEI2_IRQn      = 205,       /*!< RIIC Interrupt         *///!< INTIICTEI2_IRQn
  INTIICRI2_IRQn       = 206,       /*!< RIIC Interrupt         *///!< INTIICRI2_IRQn
  INTIICTI2_IRQn       = 207,       /*!< RIIC Interrupt         *///!< INTIICTI2_IRQn
  INTIICSPI2_IRQn      = 208,       /*!< RIIC Interrupt         *///!< INTIICSPI2_IRQn
  INTIICSTI2_IRQn      = 209,       /*!< RIIC Interrupt         *///!< INTIICSTI2_IRQn
  INTIICNAKI2_IRQn     = 210,       /*!< RIIC Interrupt         *///!< INTIICNAKI2_IRQn
  INTIICALI2_IRQn      = 211,       /*!< RIIC Interrupt         *///!< INTIICALI2_IRQn
  INTIICTMOI2_IRQn     = 212,       /*!< RIIC Interrupt         *///!< INTIICTMOI2_IRQn
  INTIICTEI3_IRQn      = 213,       /*!< RIIC Interrupt         *///!< INTIICTEI3_IRQn
  INTIICRI3_IRQn       = 214,       /*!< RIIC Interrupt         *///!< INTIICRI3_IRQn
  INTIICTI3_IRQn       = 215,       /*!< RIIC Interrupt         *///!< INTIICTI3_IRQn
  INTIICSPI3_IRQn      = 216,       /*!< RIIC Interrupt         *///!< INTIICSPI3_IRQn
  INTIICSTI3_IRQn      = 217,       /*!< RIIC Interrupt         *///!< INTIICSTI3_IRQn
  INTIICNAKI3_IRQn     = 218,       /*!< RIIC Interrupt         *///!< INTIICNAKI3_IRQn
  INTIICALI3_IRQn      = 219,       /*!< RIIC Interrupt         *///!< INTIICALI3_IRQn
  INTIICTMOI3_IRQn     = 220,       /*!< RIIC Interrupt         *///!< INTIICTMOI3_IRQn

  SCIFBRI0_IRQn        = 221,       /*!< SCIF Interrupt         *///!< SCIFBRI0_IRQn
  SCIFERI0_IRQn        = 222,       /*!< SCIF Interrupt         *///!< SCIFERI0_IRQn
  SCIFRXI0_IRQn        = 223,       /*!< SCIF Interrupt         *///!< SCIFRXI0_IRQn
  SCIFTXI0_IRQn        = 224,       /*!< SCIF Interrupt         *///!< SCIFTXI0_IRQn
  SCIFBRI1_IRQn        = 225,       /*!< SCIF Interrupt         *///!< SCIFBRI1_IRQn
  SCIFERI1_IRQn        = 226,       /*!< SCIF Interrupt         *///!< SCIFERI1_IRQn
  SCIFRXI1_IRQn        = 227,       /*!< SCIF Interrupt         *///!< SCIFRXI1_IRQn
  SCIFTXI1_IRQn        = 228,       /*!< SCIF Interrupt         *///!< SCIFTXI1_IRQn
  SCIFBRI2_IRQn        = 229,       /*!< SCIF Interrupt         *///!< SCIFBRI2_IRQn
  SCIFERI2_IRQn        = 230,       /*!< SCIF Interrupt         *///!< SCIFERI2_IRQn
  SCIFRXI2_IRQn        = 231,       /*!< SCIF Interrupt         *///!< SCIFRXI2_IRQn
  SCIFTXI2_IRQn        = 232,       /*!< SCIF Interrupt         *///!< SCIFTXI2_IRQn
  SCIFBRI3_IRQn        = 233,       /*!< SCIF Interrupt         *///!< SCIFBRI3_IRQn
  SCIFERI3_IRQn        = 234,       /*!< SCIF Interrupt         *///!< SCIFERI3_IRQn
  SCIFRXI3_IRQn        = 235,       /*!< SCIF Interrupt         *///!< SCIFRXI3_IRQn
  SCIFTXI3_IRQn        = 236,       /*!< SCIF Interrupt         *///!< SCIFTXI3_IRQn
  SCIFBRI4_IRQn        = 237,       /*!< SCIF Interrupt         *///!< SCIFBRI4_IRQn
  SCIFERI4_IRQn        = 238,       /*!< SCIF Interrupt         *///!< SCIFERI4_IRQn
  SCIFRXI4_IRQn        = 239,       /*!< SCIF Interrupt         *///!< SCIFRXI4_IRQn
  SCIFTXI4_IRQn        = 240,       /*!< SCIF Interrupt         *///!< SCIFTXI4_IRQn
#if (TARGET_RZA1 <= TARGET_RZA1LU)
  /* 241 - 252 Reserved */
#else
  SCIFBRI5_IRQn        = 241,       /*!< SCIF Interrupt         */
  SCIFERI5_IRQn        = 242,       /*!< SCIF Interrupt         */
  SCIFRXI5_IRQn        = 243,       /*!< SCIF Interrupt         */
  SCIFTXI5_IRQn        = 244,       /*!< SCIF Interrupt         */
  SCIFBRI6_IRQn        = 245,       /*!< SCIF Interrupt         */
  SCIFERI6_IRQn        = 246,       /*!< SCIF Interrupt         */
  SCIFRXI6_IRQn        = 247,       /*!< SCIF Interrupt         */
  SCIFTXI6_IRQn        = 248,       /*!< SCIF Interrupt         */
  SCIFBRI7_IRQn        = 249,       /*!< SCIF Interrupt         */
  SCIFERI7_IRQn        = 250,       /*!< SCIF Interrupt         */
  SCIFRXI7_IRQn        = 251,       /*!< SCIF Interrupt         */
  SCIFTXI7_IRQn        = 252,       /*!< SCIF Interrupt         */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */

  INTRCANGERR_IRQn     = 253,                                     //!< INTRCANGERR_IRQn
  INTRCANGRECC_IRQn    = 254,                                     //!< INTRCANGRECC_IRQn
  INTRCAN0REC_IRQn     = 255,                                     //!< INTRCAN0REC_IRQn
  INTRCAN0ERR_IRQn     = 256,                                     //!< INTRCAN0ERR_IRQn
  INTRCAN0TRX_IRQn     = 257,                                     //!< INTRCAN0TRX_IRQn
  INTRCAN1REC_IRQn     = 258,                                     //!< INTRCAN1REC_IRQn
  INTRCAN1ERR_IRQn     = 259,                                     //!< INTRCAN1ERR_IRQn
  INTRCAN1TRX_IRQn     = 260,                                     //!< INTRCAN1TRX_IRQn
#if (TARGET_RZA1 <= TARGET_RZA1LU)
  /* 261-269 Reserved */
#else

  INTRCAN2REC_IRQn     = 261,
  INTRCAN2ERR_IRQn     = 262,
  INTRCAN2TRX_IRQn     = 263,
  INTRCAN3REC_IRQn     = 264,
  INTRCAN3ERR_IRQn     = 265,
  INTRCAN3TRX_IRQn     = 266,
  INTRCAN4REC_IRQn     = 267,
  INTRCAN4ERR_IRQn     = 268,
  INTRCAN4TRX_IRQn     = 269,
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
  RSPISPEI0_IRQn       = 270,       /*!< RSPI Interrupt         *///!< RSPISPEI0_IRQn
  RSPISPRI0_IRQn       = 271,       /*!< RSPI Interrupt         *///!< RSPISPRI0_IRQn
  RSPISPTI0_IRQn       = 272,       /*!< RSPI Interrupt         *///!< RSPISPTI0_IRQn
  RSPISPEI1_IRQn       = 273,       /*!< RSPI Interrupt         *///!< RSPISPEI1_IRQn
  RSPISPRI1_IRQn       = 274,       /*!< RSPI Interrupt         *///!< RSPISPRI1_IRQn
  RSPISPTI1_IRQn       = 275,       /*!< RSPI Interrupt         *///!< RSPISPTI1_IRQn
  RSPISPEI2_IRQn       = 276,       /*!< RSPI Interrupt         *///!< RSPISPEI2_IRQn
  RSPISPRI2_IRQn       = 277,       /*!< RSPI Interrupt         *///!< RSPISPRI2_IRQn
  RSPISPTI2_IRQn       = 278,       /*!< RSPI Interrupt         *///!< RSPISPTI2_IRQn
#if (TARGET_RZA1 <= TARGET_RZA1LU)
  /* 279-288 Reserved */
#else
  RSPISPEI3_IRQn       = 279,       /*!< RSPI Interrupt         */
  RSPISPRI3_IRQn       = 280,       /*!< RSPI Interrupt         */
  RSPISPTI3_IRQn       = 281,       /*!< RSPI Interrupt         */
  RSPISPEI4_IRQn       = 282,       /*!< RSPI Interrupt         */
  RSPISPRI4_IRQn       = 283,       /*!< RSPI Interrupt         */
  RSPISPTI4_IRQn       = 284,       /*!< RSPI Interrupt         */
  IEBBTD_IRQn          = 285,
  IEBBTERR_IRQn        = 286,
  IEBBTSTA_IRQn        = 287,
  IEBBTV_IRQn          = 288,
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */

  ISY_IRQn             = 289,                                     //!< ISY_IRQn
  IERR_IRQn            = 290,                                     //!< IERR_IRQn
  ITARG_IRQn           = 291,                                     //!< ITARG_IRQn
  ISEC_IRQn            = 292,                                     //!< ISEC_IRQn
  IBUF_IRQn            = 293,                                     //!< IBUF_IRQn
  IREADY_IRQn          = 294,                                     //!< IREADY_IRQn
#if (TARGET_RZA1 <= TARGET_RZA1LU)
  /* 295-298 Reserved */
#else
  STERB_IRQn           = 295,
  FLTENDI_IRQn         = 296,
  FLTREQ0I_IRQn        = 297,
  FLTREQ1I_IRQn        = 298,
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */

  MMC0_IRQn            = 299,                                     //!< MMC0_IRQn
  MMC1_IRQn            = 300,                                     //!< MMC1_IRQn
  MMC2_IRQn            = 301,                                     //!< MMC2_IRQn

  SCHI0_3_IRQn         = 302,                                     //!< SCHI0_3_IRQn
  SDHI0_0_IRQn         = 303,                                     //!< SDHI0_0_IRQn
  SDHI0_1_IRQn         = 304,                                     //!< SDHI0_1_IRQn
  SCHI1_3_IRQn         = 305,                                     //!< SCHI1_3_IRQn
  SDHI1_0_IRQn         = 306,                                     //!< SDHI1_0_IRQn
  SDHI1_1_IRQn         = 307,                                     //!< SDHI1_1_IRQn

  ARM_IRQn             = 308,                                     //!< ARM_IRQn
  PRD_IRQn             = 309,                                     //!< PRD_IRQn
  CUP_IRQn             = 310,                                     //!< CUP_IRQn

  SCUAI0_IRQn          = 311,                                     //!< SCUAI0_IRQn
  SCUAI1_IRQn          = 312,                                     //!< SCUAI1_IRQn
  SCUFDI0_IRQn         = 313,                                     //!< SCUFDI0_IRQn
  SCUFDI1_IRQn         = 314,                                     //!< SCUFDI1_IRQn
  SCUFDI2_IRQn         = 315,                                     //!< SCUFDI2_IRQn
  SCUFDI3_IRQn         = 316,                                     //!< SCUFDI3_IRQn
  SCUFUI0_IRQn         = 317,                                     //!< SCUFUI0_IRQn
  SCUFUI1_IRQn         = 318,                                     //!< SCUFUI1_IRQn
  SCUFUI2_IRQn         = 319,                                     //!< SCUFUI2_IRQn
  SCUFUI3_IRQn         = 320,                                     //!< SCUFUI3_IRQn
  SCUDVI0_IRQn         = 321,                                     //!< SCUDVI0_IRQn
  SCUDVI1_IRQn         = 322,                                     //!< SCUDVI1_IRQn
  SCUDVI2_IRQn         = 323,                                     //!< SCUDVI2_IRQn
  SCUDVI3_IRQn         = 324,                                     //!< SCUDVI3_IRQn
#if (TARGET_RZA1 <= TARGET_RZA1LU)

  /* 325-346 Reserved  */

#else
  MLB_CINT_IRQn        = 325,
  MLB_SINT_IRQn        = 326,
  DRC10_IRQn           = 327,
  DRC11_IRQn           = 328,

  /* 329-330 Reserved  */

  LINI0_INT_T_IRQn     = 331,
  LINI0_INT_R_IRQn     = 332,
  LINI0_INT_S_IRQn     = 333,
  LINI0_INT_M_IRQn     = 334,
  LINI1_INT_T_IRQn     = 335,
  LINI1_INT_R_IRQn     = 336,
  LINI1_INT_S_IRQn     = 337,
  LINI1_INT_M_IRQn     = 338,
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */

  /* 339-346 Reserved */

  SCIERI0_IRQn         = 347,                                     //!< SCIERI0_IRQn
  SCIRXI0_IRQn         = 348,                                     //!< SCIRXI0_IRQn
  SCITXI0_IRQn         = 349,                                     //!< SCITXI0_IRQn
  SCITEI0_IRQn         = 350,                                     //!< SCITEI0_IRQn
  SCIERI1_IRQn         = 351,                                     //!< SCIERI1_IRQn
  SCIRXI1_IRQn         = 352,                                     //!< SCIRXI1_IRQn
  SCITXI1_IRQn         = 353,                                     //!< SCITXI1_IRQn
  SCITEI1_IRQn         = 354,                                     //!< SCITEI1_IRQn

  AVBI_DATA            = 355,                                     //!< AVBI_DATA
  AVBI_ERROR           = 356,                                     //!< AVBI_ERROR
  AVBI_MANAGE          = 357,                                     //!< AVBI_MANAGE
  AVBI_MAC             = 358,                                     //!< AVBI_MAC

  ETHERI_IRQn          = 359,                                     //!< ETHERI_IRQn

  /* 360-363 Reserved */

  CEUI_IRQn            = 364,                                     //!< CEUI_IRQn

  /* 365-380 Reserved */

  H2XMLB_ERRINT_IRQn   = 381,                                     //!< H2XMLB_ERRINT_IRQn
  H2XIC1_ERRINT_IRQn   = 382,                                     //!< H2XIC1_ERRINT_IRQn
  X2HPERI1_ERRINT_IRQn = 383,                                     //!< X2HPERI1_ERRINT_IRQn
  X2HPERR2_ERRINT_IRQn = 384,                                     //!< X2HPERR2_ERRINT_IRQn
  X2HPERR34_ERRINT_IRQn= 385,                                     //!< X2HPERR34_ERRINT_IRQn
  X2HPERR5_ERRINT_IRQn = 386,                                     //!< X2HPERR5_ERRINT_IRQn
  X2HPERR67_ERRINT_IRQn= 387,                                     //!< X2HPERR67_ERRINT_IRQn
  X2HDBGR_ERRINT_IRQn  = 388,                                     //!< X2HDBGR_ERRINT_IRQn
  X2HBSC_ERRINT_IRQn   = 389,                                     //!< X2HBSC_ERRINT_IRQn
  X2HSPI1_ERRINT_IRQn  = 390,                                     //!< X2HSPI1_ERRINT_IRQn
  X2HSPI2_ERRINT_IRQn  = 391,                                     //!< X2HSPI2_ERRINT_IRQn
  PRRI_IRQn            = 392,                                     //!< PRRI_IRQn
#if (TARGET_RZA1 <= TARGET_RZA1LU)

  /* 393-415 Reserved */

#else
  IFEI0_IRQn           = 393,
  OFFI0_IRQn           = 394,
  PFVEI0_IRQn          = 395,
  IFEI1_IRQn           = 396,
  OFFI1_IRQn           = 397,
  PFVEI1_IRQn          = 398,

  /* 399-415 Reserved */

#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
  TINT0_IRQn           = 416,                                     //!< TINT0_IRQn
  TINT1_IRQn           = 417,                                     //!< TINT1_IRQn
  TINT2_IRQn           = 418,                                     //!< TINT2_IRQn
  TINT3_IRQn           = 419,                                     //!< TINT3_IRQn
  TINT4_IRQn           = 420,                                     //!< TINT4_IRQn
  TINT5_IRQn           = 421,                                     //!< TINT5_IRQn
  TINT6_IRQn           = 422,                                     //!< TINT6_IRQn
  TINT7_IRQn           = 423,                                     //!< TINT7_IRQn
  TINT8_IRQn           = 424,                                     //!< TINT8_IRQn
  TINT9_IRQn           = 425,                                     //!< TINT9_IRQn
  TINT10_IRQn          = 426,                                     //!< TINT10_IRQn
  TINT11_IRQn          = 427,                                     //!< TINT11_IRQn
  TINT12_IRQn          = 428,                                     //!< TINT12_IRQn
  TINT13_IRQn          = 429,                                     //!< TINT13_IRQn
  TINT14_IRQn          = 430,                                     //!< TINT14_IRQn
  TINT15_IRQn          = 431,                                     //!< TINT15_IRQn
  TINT16_IRQn          = 432,                                     //!< TINT16_IRQn
  TINT17_IRQn          = 433,                                     //!< TINT17_IRQn
  TINT18_IRQn          = 434,                                     //!< TINT18_IRQn
  TINT19_IRQn          = 435,                                     //!< TINT19_IRQn
  TINT20_IRQn          = 436,                                     //!< TINT20_IRQn
  TINT21_IRQn          = 437,                                     //!< TINT21_IRQn
  TINT22_IRQn          = 438,                                     //!< TINT22_IRQn
  TINT23_IRQn          = 439,                                     //!< TINT23_IRQn
  TINT24_IRQn          = 440,                                     //!< TINT24_IRQn
  TINT25_IRQn          = 441,                                     //!< TINT25_IRQn
  TINT26_IRQn          = 442,                                     //!< TINT26_IRQn
  TINT27_IRQn          = 443,                                     //!< TINT27_IRQn
  TINT28_IRQn          = 444,                                     //!< TINT28_IRQn
  TINT29_IRQn          = 445,                                     //!< TINT29_IRQn
  TINT30_IRQn          = 446,                                     //!< TINT30_IRQn
  TINT31_IRQn          = 447,                                     //!< TINT31_IRQn
  TINT32_IRQn          = 448,                                     //!< TINT32_IRQn
  TINT33_IRQn          = 449,                                     //!< TINT33_IRQn
  TINT34_IRQn          = 450,                                     //!< TINT34_IRQn
  TINT35_IRQn          = 451,                                     //!< TINT35_IRQn
  TINT36_IRQn          = 452,                                     //!< TINT36_IRQn
  TINT37_IRQn          = 453,                                     //!< TINT37_IRQn
  TINT38_IRQn          = 454,                                     //!< TINT38_IRQn
  TINT39_IRQn          = 455,                                     //!< TINT39_IRQn
  TINT40_IRQn          = 456,                                     //!< TINT40_IRQn
  TINT41_IRQn          = 457,                                     //!< TINT41_IRQn
  TINT42_IRQn          = 458,                                     //!< TINT42_IRQn
  TINT43_IRQn          = 459,                                     //!< TINT43_IRQn
  TINT44_IRQn          = 460,                                     //!< TINT44_IRQn
  TINT45_IRQn          = 461,                                     //!< TINT45_IRQn
  TINT46_IRQn          = 462,                                     //!< TINT46_IRQn
  TINT47_IRQn          = 463,                                     //!< TINT47_IRQn
  TINT48_IRQn          = 464,                                     //!< TINT48_IRQn
  TINT49_IRQn          = 465,                                     //!< TINT49_IRQn
  TINT50_IRQn          = 466,                                     //!< TINT50_IRQn
  TINT51_IRQn          = 467,                                     //!< TINT51_IRQn
  TINT52_IRQn          = 468,                                     //!< TINT52_IRQn
  TINT53_IRQn          = 469,                                     //!< TINT53_IRQn
  TINT54_IRQn          = 470,                                     //!< TINT54_IRQn
  TINT55_IRQn          = 471,                                     //!< TINT55_IRQn
  TINT56_IRQn          = 472,                                     //!< TINT56_IRQn
  TINT57_IRQn          = 473,                                     //!< TINT57_IRQn
  TINT58_IRQn          = 474,                                     //!< TINT58_IRQn
  TINT59_IRQn          = 475,                                     //!< TINT59_IRQn
  TINT60_IRQn          = 476,                                     //!< TINT60_IRQn
  TINT61_IRQn          = 477,                                     //!< TINT61_IRQn
  TINT62_IRQn          = 478,                                     //!< TINT62_IRQn
  TINT63_IRQn          = 479,                                     //!< TINT63_IRQn
  TINT64_IRQn          = 480,                                     //!< TINT64_IRQn
  TINT65_IRQn          = 481,                                     //!< TINT65_IRQn
  TINT66_IRQn          = 482,                                     //!< TINT66_IRQn
  TINT67_IRQn          = 483,                                     //!< TINT67_IRQn
  TINT68_IRQn          = 484,                                     //!< TINT68_IRQn
  TINT69_IRQn          = 485,                                     //!< TINT69_IRQn
  TINT70_IRQn          = 486,                                     //!< TINT70_IRQn
  TINT71_IRQn          = 487,                                     //!< TINT71_IRQn
  TINT72_IRQn          = 488,                                     //!< TINT72_IRQn
  TINT73_IRQn          = 489,                                     //!< TINT73_IRQn
  TINT74_IRQn          = 490,                                     //!< TINT74_IRQn
  TINT75_IRQn          = 491,                                     //!< TINT75_IRQn
  TINT76_IRQn          = 492,                                     //!< TINT76_IRQn
  TINT77_IRQn          = 493,                                     //!< TINT77_IRQn
  TINT78_IRQn          = 494,                                     //!< TINT78_IRQn
  TINT79_IRQn          = 495,                                     //!< TINT79_IRQn
  TINT80_IRQn          = 496,                                     //!< TINT80_IRQn
  TINT81_IRQn          = 497,                                     //!< TINT81_IRQn
  TINT82_IRQn          = 498,                                     //!< TINT82_IRQn
  TINT83_IRQn          = 499,                                     //!< TINT83_IRQn
  TINT84_IRQn          = 500,                                     //!< TINT84_IRQn
  TINT85_IRQn          = 501,                                     //!< TINT85_IRQn
  TINT86_IRQn          = 502,                                     //!< TINT86_IRQn
  TINT87_IRQn          = 503,                                     //!< TINT87_IRQn
  TINT88_IRQn          = 504,                                     //!< TINT88_IRQn
  TINT89_IRQn          = 505,                                     //!< TINT89_IRQn
  TINT90_IRQn          = 506,                                     //!< TINT90_IRQn
  TINT91_IRQn          = 507,                                     //!< TINT91_IRQn
  TINT92_IRQn          = 508,                                     //!< TINT92_IRQn
  TINT93_IRQn          = 509,                                     //!< TINT93_IRQn
  TINT94_IRQn          = 510,                                     //!< TINT94_IRQn
  TINT95_IRQn          = 511,                                     //!< TINT95_IRQn
  TINT96_IRQn          = 512,                                     //!< TINT96_IRQn
  TINT97_IRQn          = 513,                                     //!< TINT97_IRQn
  TINT98_IRQn          = 514,                                     //!< TINT98_IRQn
  TINT99_IRQn          = 515,                                     //!< TINT99_IRQn
  TINT100_IRQn         = 516,                                     //!< TINT100_IRQn
  TINT101_IRQn         = 517,                                     //!< TINT101_IRQn
  TINT102_IRQn         = 518,                                     //!< TINT102_IRQn
  TINT103_IRQn         = 519,                                     //!< TINT103_IRQn
  TINT104_IRQn         = 520,                                     //!< TINT104_IRQn
  TINT105_IRQn         = 521,                                     //!< TINT105_IRQn
  TINT106_IRQn         = 522,                                     //!< TINT106_IRQn
  TINT107_IRQn         = 523,                                     //!< TINT107_IRQn
  TINT108_IRQn         = 524,                                     //!< TINT108_IRQn
  TINT109_IRQn         = 525,                                     //!< TINT109_IRQn
  TINT110_IRQn         = 526,                                     //!< TINT110_IRQn
  TINT111_IRQn         = 527,                                     //!< TINT111_IRQn
  TINT112_IRQn         = 528,                                     //!< TINT112_IRQn
  TINT113_IRQn         = 529,                                     //!< TINT113_IRQn
  TINT114_IRQn         = 530,                                     //!< TINT114_IRQn
  TINT115_IRQn         = 531,                                     //!< TINT115_IRQn
  TINT116_IRQn         = 532,                                     //!< TINT116_IRQn
  TINT117_IRQn         = 533,                                     //!< TINT117_IRQn
  TINT118_IRQn         = 534,                                     //!< TINT118_IRQn
  TINT119_IRQn         = 535,                                     //!< TINT119_IRQn
  TINT120_IRQn         = 536,                                     //!< TINT120_IRQn
#if (TARGET_RZA1 <= TARGET_RZA1LU)
  TINT121_IRQn         = 537                                      //!< TINT121_IRQn
#else
  TINT121_IRQn         = 537,
  TINT122_IRQn         = 538,
  TINT123_IRQn         = 539,
  TINT124_IRQn         = 540,
  TINT125_IRQn         = 541,
  TINT126_IRQn         = 542,
  TINT127_IRQn         = 543,
  TINT128_IRQn         = 544,
  TINT129_IRQn         = 545,
  TINT130_IRQn         = 546,
  TINT131_IRQn         = 547,
  TINT132_IRQn         = 548,
  TINT133_IRQn         = 549,
  TINT134_IRQn         = 550,
  TINT135_IRQn         = 551,
  TINT136_IRQn         = 552,
  TINT137_IRQn         = 553,
  TINT138_IRQn         = 554,
  TINT139_IRQn         = 555,
  TINT140_IRQn         = 556,
  TINT141_IRQn         = 557,
  TINT142_IRQn         = 558,
  TINT143_IRQn         = 559,
  TINT144_IRQn         = 560,
  TINT145_IRQn         = 561,
  TINT146_IRQn         = 562,
  TINT147_IRQn         = 563,
  TINT148_IRQn         = 564,
  TINT149_IRQn         = 565,
  TINT150_IRQn         = 566,
  TINT151_IRQn         = 567,
  TINT152_IRQn         = 568,
  TINT153_IRQn         = 569,
  TINT154_IRQn         = 570,
  TINT155_IRQn         = 571,
  TINT156_IRQn         = 572,
  TINT157_IRQn         = 573,
  TINT158_IRQn         = 574,
  TINT159_IRQn         = 575,
  TINT160_IRQn         = 576,
  TINT161_IRQn         = 577,
  TINT162_IRQn         = 578,
  TINT163_IRQn         = 579,
  TINT164_IRQn         = 580,
  TINT165_IRQn         = 581,
  TINT166_IRQn         = 582,
  TINT167_IRQn         = 583,
  TINT168_IRQn         = 584,
  TINT169_IRQn         = 585,
  TINT170_IRQn         = 586
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */

} IRQn_Type;
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#define Renesas_RZ_A1_IRQ_MAX  TINT121_IRQn
#else
#define Renesas_RZ_A1_IRQ_MAX  TINT170_IRQn
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */


/******************************************************************************/
/*                Device Specific Peripheral Section                          */
/******************************************************************************/
/** @addtogroup Renesas_RZ_A1_Peripherals Renesas_RZ_A1 Peripherals
  Renesas_RZ_A1 Device Specific Peripheral registers structures
  @{
*/

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

//#include "pl310.h"
//#include "gic.h"

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

/*@}*/ /* end of group Renesas_RZ_A1_Peripherals */


/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/** @addtogroup Renesas_RZ_A1_MemoryMap Renesas_RZ_A1 Memory Mapping
  @{
*/

#if (TARGET_RZA1 <= TARGET_RZA1LU)
/* R7S72103 Stream it! board  */
#define Renesas_RZ_A1_SDRAM_BASE0                  (0x0C000000UL)                        /*!< (SDRAM0    ) Base Address */
#else
/* R7S72100 CPU board  */
#define Renesas_RZ_A1_NORFLASH_BASE0               (0x00000000UL)                        /*!< (FLASH0    ) Base Address */
#define Renesas_RZ_A1_NORFLASH_BASE1               (0x04000000UL)                        /*!< (FLASH1    ) Base Address */
#define Renesas_RZ_A1_SDRAM_BASE0                  (0x08000000UL)                        /*!< (SDRAM0    ) Base Address */
#define Renesas_RZ_A1_SDRAM_BASE1                  (0x0C000000UL)                        /*!< (SDRAM1    ) Base Address */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define Renesas_RZ_A1_USER_AREA0                   (0x10000000UL)                        /*!< (USER0     ) Base Address */
#define Renesas_RZ_A1_USER_AREA1                   (0x14000000UL)                        /*!< (USER1     ) Base Address */
#define Renesas_RZ_A1_SPI_IO0                      (0x18000000UL)                        /*!< (SPI_IO0   ) Base Address */
#define Renesas_RZ_A1_SPI_IO1                      (0x1C000000UL)                        /*!< (SPI_IO1   ) Base Address */
#define Renesas_RZ_A1_ONCHIP_SRAM_BASE             (0x20000000UL)                        /*!< (SRAM_OC   ) Base Address */
#define Renesas_RZ_A1_SPI_MIO_BASE                 (0x3fe00000UL)                        /*!< (SPI_MIO   ) Base Address */
#define Renesas_RZ_A1_BSC_BASE                     (0x3ff00000UL)                        /*!< (BSC       ) Base Address */
#define Renesas_RZ_A1_PERIPH_BASE0                 (0xe8000000UL)                        /*!< (PERIPH0   ) Base Address */
#define Renesas_RZ_A1_PERIPH_BASE1                 (0xfcf00000UL)                        /*!< (PERIPH1   ) Base Address */
#define Renesas_RZ_A1_GIC_DISTRIBUTOR_BASE         (0xe8201000UL)                        /*!< (GIC DIST  ) Base Address */
#define Renesas_RZ_A1_GIC_INTERFACE_BASE           (0xe8202000UL)                        /*!< (GIC CPU IF) Base Address */
#define Renesas_RZ_A1_PL310_BASE                   (0x3ffff000UL)                        /*!< (PL310     ) Base Address */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#define Renesas_RZ_A1_ONCHIP_SRAM_MIRROR_BASE      (0x60000000UL)                        /*!< (SRAM_OC MIRROR  ) Base Address */
#endif

#define __CORTEX_A				9
#define __FPU_PRESENT             1       /*!< FPU present                                   */
#define __GIC_PRESENT				1U
#define __L2C_PRESENT				1U

#define GIC_DISTRIBUTOR_BASE         Renesas_RZ_A1_GIC_DISTRIBUTOR_BASE                        /*!< (GIC DIST  ) Base Address */
#define GIC_INTERFACE_BASE           Renesas_RZ_A1_GIC_INTERFACE_BASE                        /*!< (GIC CPU IF) Base Address */
#define L2C_310_BASE                 Renesas_RZ_A1_PL310_BASE                        /*!< (PL310     ) Base Address */


/* --------  Configuration of the Cortex-A9 Processor and Core Peripherals  ------- */
#define __CA_REV         0x0000    /*!< Core revision r0       */

#include "core_ca.h"
#include "system_Renesas_RZ_A1.h"

// Already defined in core_ca.h
#if 0
//
//Following macros define the descriptors and attributes used to define the Renesas_RZ_A1 MMU flat-map
//Sect_Normal. Outer & inner wb/wa, non-secure, non-shareable, executable, rw, domain 0.
#define section_normal(descriptor_l1, region)     region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = WB_WA; \
                                   region.outer_norm_t = WB_WA; \
                                   region.mem_t = NORMAL; \
                                   region.sec_t = NON_SECURE; \
                                   region.xn_t = EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   __get_section_descriptor(&descriptor_l1, region);

//Sect_Normal_Cod. Outer & inner wb/wa, non-secure, non-shareable, executable, ro, domain 0.
#define section_normal_cod(descriptor_l1, region) region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = WB_WA; \
                                   region.outer_norm_t = WB_WA; \
                                   region.mem_t = NORMAL; \
                                   region.sec_t = NON_SECURE; \
                                   region.xn_t = EXECUTE; \
                                   region.priv_t = READ; \
                                   region.user_t = READ; \
                                   region.sh_t = NON_SHARED; \
                                   __get_section_descriptor(&descriptor_l1, region);

//Sect_Normal_RO. Sect_Normal_Cod, but not executable
#define section_normal_ro(descriptor_l1, region)  region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = WB_WA; \
                                   region.outer_norm_t = WB_WA; \
                                   region.mem_t = NORMAL; \
                                   region.sec_t = NON_SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = READ; \
                                   region.user_t = READ; \
                                   region.sh_t = NON_SHARED; \
                                   __get_section_descriptor(&descriptor_l1, region);

//Sect_Normal_RW. Sect_Normal_Cod, but writeable and not executable
#define section_normal_rw(descriptor_l1, region) region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = WB_WA; \
                                   region.outer_norm_t = WB_WA; \
                                   region.mem_t = NORMAL; \
                                   region.sec_t = NON_SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   __get_section_descriptor(&descriptor_l1, region);

//Sect_Normal_RW_NC. Outer & inner non-cacheable, non-secure, non-shareable, executable, rw, domain 0.
#define section_normal_rw_nc(descriptor_l1, region) region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = NON_CACHEABLE; \
                                   region.outer_norm_t = NON_CACHEABLE; \
                                   region.mem_t = NORMAL; \
                                   region.sec_t = NON_SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   __get_section_descriptor(&descriptor_l1, region);

//Sect_SO. Strongly-ordered (therefore shareable), non-secure, not executable, rw, domain 0, base addr 0
#define section_so(descriptor_l1, region) region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = NON_CACHEABLE; \
                                   region.outer_norm_t = NON_CACHEABLE; \
                                   region.mem_t = STRONGLY_ORDERED; \
                                   region.sec_t = NON_SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   __get_section_descriptor(&descriptor_l1, region);

//Sect_Device_RO. Device, secure, non-shareable, non-executable, ro, domain 0, base addr 0
#define section_device_ro(descriptor_l1, region) region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = NON_CACHEABLE; \
                                   region.outer_norm_t = NON_CACHEABLE; \
                                   region.mem_t = STRONGLY_ORDERED; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = READ; \
                                   region.user_t = READ; \
                                   region.sh_t = NON_SHARED; \
                                   __get_section_descriptor(&descriptor_l1, region);

//Sect_Device_RW. Sect_Device_RO, but writeable
#define section_device_rw(descriptor_l1, region) region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = NON_CACHEABLE; \
                                   region.outer_norm_t = NON_CACHEABLE; \
                                   region.mem_t = STRONGLY_ORDERED; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   __get_section_descriptor(&descriptor_l1, region);
//Page_4k_Device_RW. Shared device, secure, not executable, rw, domain 0
#define page4k_device_rw(descriptor_l1, descriptor_l2, region) region.rg_t = PAGE_4k; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = NON_CACHEABLE; \
                                   region.outer_norm_t = NON_CACHEABLE; \
                                   region.mem_t = SHARED_DEVICE; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   __get_page_descriptor(&descriptor_l1, &descriptor_l2, region);

//Page_64k_Device_RW. Shared device, secure, not executable, rw, domain 0
#define page64k_device_rw(descriptor_l1, descriptor_l2, region)  region.rg_t = PAGE_64k; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = NON_CACHEABLE; \
                                   region.outer_norm_t = NON_CACHEABLE; \
                                   region.mem_t = SHARED_DEVICE; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   __get_page_descriptor(&descriptor_l1, &descriptor_l2, region);

/*@}*/ /* end of group Renesas_RZ_A1_MemoryMap */
#endif



/******************************************************************************/
/*                         Clock Settings                                     */
/******************************************************************************/

#if 0
/** @addtogroup Renesas_RZ_A1_Clocks Renesas_RZ_A1 Clock definitions
  @{
*/

/*
 * Clock Mode 0 settings
 * TARGET_RZA1LU : SW1-3(MD_CLK):ON
 * TARGET_RZA1H  : SW1-4(MD_CLK):ON
 *               : SW1-5(MD_CLKS):ON
 * FRQCR=0x1035
 *   CLKEN2    = 0b - unstable
 *   CLKEN[1:0]=01b - Output, Low, Low
 *   IFC[1:0]  =00b - CPU clock is 1/1 PLL clock
 * TARGET_RZA1H  : FRQCR2=0x0001
 *   GFC[1:0]  =01b - Graphic clock is 2/3 bus clock
 */
#define CM0_RENESAS_RZ_A1_CLKIN  ( 13333333u)
#define CM0_RENESAS_RZ_A1_CLKO   ( 66666666u)
#define CM0_RENESAS_RZ_A1_I_CLK  (400000000u)
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CM0_RENESAS_RZ_A1_G_CLK  (266666666u)
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CM0_RENESAS_RZ_A1_B_CLK  (133333333u)
#define CM0_RENESAS_RZ_A1_P1_CLK ( 66666666u)
#define CM0_RENESAS_RZ_A1_P0_CLK ( 33333333u)

/*
 * Clock Mode 1 settings
 * TARGET_RZA1LU : SW1-3(MD_CLK):OFF
 * TARGET_RZA1H  : SW1-4(MD_CLK):OFF
 *                 SW1-5(MD_CLKS):ON
 * FRQCR=0x1335
 *   CLKEN2    = 0b - unstable
 *   CLKEN[1:0]=01b - Output, Low, Low
 *   IFC[1:0]  =11b - CPU clock is 1/3 PLL clock
 * TARGET_RZA1H  : FRQCR2=0x0003
 *   GFC[1:0]  =11b - graphic clock is 1/3 bus clock
 */
#define CM1_RENESAS_RZ_A1_CLKIN  ( 48000000u)
#define CM1_RENESAS_RZ_A1_CLKO   ( 64000000u)
#define CM1_RENESAS_RZ_A1_I_CLK  (128000000u)
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CM1_RENESAS_RZ_A1_G_CLK  (128000000u)
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CM1_RENESAS_RZ_A1_B_CLK  (128000000u)
#define CM1_RENESAS_RZ_A1_P1_CLK ( 64000000u)
#define CM1_RENESAS_RZ_A1_P0_CLK ( 32000000u)

/* Below CLK settings depends on user environment */
#define RENESAS_RZ_A1_AUDIO_CLK  (        0u)
#define RENESAS_RZ_A1_AUDIO_X1   ( 22579200u)
#define RENESAS_RZ_A1_MLB_CLK    ( 66670000u)


/*@}*/ /* end of group Renesas_RZ_A1_Clocks */

#endif

/******************************************************************************/
/*                         CPG   Settings                                     */
/******************************************************************************/
/** @addtogroup Renesas_RZ_A1_CPG Renesas_RZ_A1 CPG Bit definitions
    @{
*/

#define CPG_FRQCR_SHIFT_CKOEN2  (14)
#define CPG_FRQCR_BIT_CKOEN2    (0x1 << CPG_FRQCR_SHIFT_CKOEN2)
#define CPG_FRQCR_SHIFT_CKOEN0  (12)
#define CPG_FRQCR_BITS_CKOEN0   (0x3 << CPG_FRQCR_SHIFT_CKOEN0)
#define CPG_FRQCR_SHIFT_IFC     (8)
#define CPG_FRQCR_BITS_IFC      (0x3 << CPG_FRQCR_SHIFT_IFC)
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_FRQCR2_SHIFT_GFC    (0)
#define CPG_FRQCR2_BITS_GFC     (0x3 << CPG_FRQCR2_SHIFT_GFC)
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */

#define CPG_STBCR1_BIT_STBY     (0x80u)
#define CPG_STBCR1_BIT_DEEP     (0x40u)
#define CPG_STBCR2_BIT_HIZ      (0x80u)
#define CPG_STBCR2_BIT_MSTP20   (0x01u) /* CoreSight */
#define CPG_STBCR3_BIT_MSTP37   (0x80u) /* IEBus */
#define CPG_STBCR3_BIT_MSTP36   (0x40u) /* IrDA */
#define CPG_STBCR3_BIT_MSTP35   (0x20u) /* LIN0 */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR3_BIT_MSTP34   (0x10u) /* LIN1 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR3_BIT_MSTP33   (0x08u) /* Multi-Function Timer */
#define CPG_STBCR3_BIT_MSTP32   (0x04u) /* CAN */
#define CPG_STBCR3_BIT_MSTP31   (0x02u) /* A/D converter (analog voltage) */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR3_BIT_MSTP30   (0x01u) /* Motor Control PWM Timer */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR4_BIT_MSTP47   (0x80u) /* SCIF0 */
#define CPG_STBCR4_BIT_MSTP46   (0x40u) /* SCIF1 */
#define CPG_STBCR4_BIT_MSTP45   (0x20u) /* SCIF2 */
#define CPG_STBCR4_BIT_MSTP44   (0x10u) /* SCIF3 */
#define CPG_STBCR4_BIT_MSTP43   (0x08u) /* SCIF4 */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR4_BIT_MSTP42   (0x04u) /* SCIF5 */
#define CPG_STBCR4_BIT_MSTP41   (0x02u) /* SCIF6 */
#define CPG_STBCR4_BIT_MSTP40   (0x01u) /* SCIF7 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR5_BIT_MSTP57   (0x80u) /* SCI0 */
#define CPG_STBCR5_BIT_MSTP56   (0x40u) /* SCI1 */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR5_BIT_MSTP55   (0x20u) /* Sound Generator0 */
#define CPG_STBCR5_BIT_MSTP54   (0x10u) /* Sound Generator1 */
#define CPG_STBCR5_BIT_MSTP53   (0x08u) /* Sound Generator2 */
#define CPG_STBCR5_BIT_MSTP52   (0x04u) /* Sound Generator3 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR5_BIT_MSTP51   (0x02u) /* OSTM0 */
#define CPG_STBCR5_BIT_MSTP50   (0x01u) /* OSTM1 */
#define CPG_STBCR6_BIT_MSTP67   (0x80u) /* A/D converter (clock) */
#define CPG_STBCR6_BIT_MSTP66   (0x40u) /* Capture Engine */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR6_BIT_MSTP65   (0x20u) /* Display out comparison0 */
#define CPG_STBCR6_BIT_MSTP64   (0x10u) /* Display out comparison1 */
#define CPG_STBCR6_BIT_MSTP63   (0x08u) /* Dynamic Range compression0 */
#define CPG_STBCR6_BIT_MSTP62   (0x04u) /* Dynamic Range compression1 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR6_BIT_MSTP61   (0x02u) /* JPEG Decoder */
#define CPG_STBCR6_BIT_MSTP60   (0x01u) /* Realtime Clock */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR7_BIT_MSTP77   (0x80u) /* Video Decoder0 */
#define CPG_STBCR7_BIT_MSTP76   (0x40u) /* Video Decoder1 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR7_BIT_MSTP74   (0x10u) /* Ethernet */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR7_BIT_MSTP73   (0x04u) /* NAND Flash Memory Controller */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR7_BIT_MSTP71   (0x02u) /* USB0 */
#define CPG_STBCR7_BIT_MSTP70   (0x01u) /* USB1 */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR8_BIT_MSTP87   (0x80u) /* IMR-LS2_0 */
#define CPG_STBCR8_BIT_MSTP86   (0x40u) /* IMR-LS2_1 */
#define CPG_STBCR8_BIT_MSTP85   (0x20u) /* IMR-LSD */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR8_BIT_MSTP84   (0x10u) /* MMC Host Interface */
#define CPG_STBCR8_BIT_MSTP83   (0x08u) /* MediaLB */
#define CPG_STBCR8_BIT_MSTP82   (0x04u) /* EthernetAVB */
#define CPG_STBCR8_BIT_MSTP81   (0x02u) /* SCUX */
#define CPG_STBCR9_BIT_MSTP97   (0x80u) /* RIIC0 */
#define CPG_STBCR9_BIT_MSTP96   (0x40u) /* RIIC1 */
#define CPG_STBCR9_BIT_MSTP95   (0x20u) /* RIIC2 */
#define CPG_STBCR9_BIT_MSTP94   (0x10u) /* RIIC3 */
#define CPG_STBCR9_BIT_MSTP93   (0x08u) /* SPI Multi I/O Bus Controller0 */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR9_BIT_MSTP92   (0x04u) /* SPI Multi I/O Bus Controller1 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR9_BIT_MSTP91   (0x02u) /* VDC5_0 */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR9_BIT_MSTP90   (0x01u) /* VDC5_1 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR10_BIT_MSTP107 (0x80u) /* RSPI0 */
#define CPG_STBCR10_BIT_MSTP106 (0x40u) /* RSPI1 */
#define CPG_STBCR10_BIT_MSTP105 (0x20u) /* RSPI2 */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR10_BIT_MSTP104 (0x10u) /* RSPI3 */
#define CPG_STBCR10_BIT_MSTP103 (0x08u) /* RSPI4 */
#define CPG_STBCR10_BIT_MSTP102 (0x04u) /* ROMDEC */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR10_BIT_MSTP101 (0x02u) /* SPIDF */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR10_BIT_MSTP100 (0x01u) /* OpenVG */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR11_BIT_MSTP115 (0x20u) /* SSIF0 */
#define CPG_STBCR11_BIT_MSTP114 (0x10u) /* SSIF1 */
#define CPG_STBCR11_BIT_MSTP113 (0x08u) /* SSIF2 */
#define CPG_STBCR11_BIT_MSTP112 (0x04u) /* SSIF3 */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR11_BIT_MSTP111 (0x02u) /* SSIF4 */
#define CPG_STBCR11_BIT_MSTP110 (0x01u) /* SSIF5 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBCR12_BIT_MSTP123 (0x08u) /* SD Host Interface00 */
#define CPG_STBCR12_BIT_MSTP122 (0x04u) /* SD Host Interface01 */
#define CPG_STBCR12_BIT_MSTP121 (0x02u) /* SD Host Interface10 */
#define CPG_STBCR12_BIT_MSTP120 (0x01u) /* SD Host Interface11 */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBCR13_BIT_MSTP132 (0x04u) /* PFV1 */
#define CPG_STBCR13_BIT_MSTP131 (0x02u) /* PFV0 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_SWRSTCR1_BIT_AXTALE (0x80u) /* AUDIO_X1 */
#define CPG_SWRSTCR1_BIT_SRST16 (0x40u) /* SSIF0 */
#define CPG_SWRSTCR1_BIT_SRST15 (0x20u) /* SSIF1 */
#define CPG_SWRSTCR1_BIT_SRST14 (0x10u) /* SSIF2 */
#define CPG_SWRSTCR1_BIT_SRST13 (0x08u) /* SSIF3 */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_SWRSTCR1_BIT_SRST12 (0x04u) /* SSIF4 */
#define CPG_SWRSTCR1_BIT_SRST11 (0x02u) /* SSIF5 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_SWRSTCR2_BIT_SRST21 (0x02u) /* JPEG Decoder */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_SWRSTCR3_BIT_SRST32 (0x04u) /* OpenVG */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_SYSCR1_BIT_VRAME4   (0x10u) /* VRAM E Page4 */
#define CPG_SYSCR1_BIT_VRAME3   (0x08u) /* VRAM E Page3 */
#define CPG_SYSCR1_BIT_VRAME2   (0x04u) /* VRAM E Page2 */
#define CPG_SYSCR1_BIT_VRAME1   (0x02u) /* VRAM E Page1 */
#define CPG_SYSCR1_BIT_VRAME0   (0x01u) /* VRAM E Page0 */
#define CPG_SYSCR2_BIT_VRAMWE4  (0x10u) /* VRAM WE Page4 */
#define CPG_SYSCR2_BIT_VRAMWE3  (0x08u) /* VRAM WE Page3 */
#define CPG_SYSCR2_BIT_VRAMWE2  (0x04u) /* VRAM WE Page2 */
#define CPG_SYSCR2_BIT_VRAMWE1  (0x02u) /* VRAM WE Page1 */
#define CPG_SYSCR2_BIT_VRAMWE0  (0x01u) /* VRAM WE Page0 */
#define CPG_SYSCR3_BIT_RRAMWE3  (0x08u) /* RRAM WE Page3 */
#define CPG_SYSCR3_BIT_RRAMWE2  (0x04u) /* RRAM WE Page2 */
#define CPG_SYSCR3_BIT_RRAMWE1  (0x02u) /* RRAM WE Page1 */
#define CPG_SYSCR3_BIT_RRAMWE0  (0x01u) /* RRAM WE Page0 */
#define CPG_CPUSTS_BIT_ISBUSY   (0x10u) /* State during Changing of the Frequency of CPU and Return from Software Standby */
#define CPG_STBREQ1_BIT_STBRQ15 (0x20u) /* CoreSight */
#define CPG_STBREQ1_BIT_STBRQ13 (0x08u) /* JPEG Control */
#define CPG_STBREQ1_BIT_STBRQ12 (0x04u) /* EthernetAVB */
#define CPG_STBREQ1_BIT_STBRQ10 (0x01u) /* Capture Engine */
#define CPG_STBREQ2_BIT_STBRQ27 (0x80u) /* MediaLB */
#define CPG_STBREQ2_BIT_STBRQ26 (0x40u) /* Ethernet */
#define CPG_STBREQ2_BIT_STBRQ25 (0x20u) /* VDC5_0 */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBREQ2_BIT_STBRQ24 (0x10u) /* VCD5_1 */
#define CPG_STBREQ2_BIT_STBRQ23 (0x08u) /* IMR_LS2_0 */
#define CPG_STBREQ2_BIT_STBRQ22 (0x04u) /* IMR_LS2_1 */
#define CPG_STBREQ2_BIT_STBRQ21 (0x02u) /* IMR_LSD */
#define CPG_STBREQ2_BIT_STBRQ20 (0x01u) /* OpenVG */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_STBACK1_BIT_STBAK15 (0x20u) /* CoreSight */
#define CPG_STBACK1_BIT_STBAK13 (0x08u) /* JPEG Control */
#define CPG_STBACK1_BIT_STBAK12 (0x04u) /* EthernetAVB */
#define CPG_STBACK1_BIT_STBAK10 (0x01u) /* Capture Engine */
#define CPG_STBACK2_BIT_STBAK27 (0x80u) /* MediaLB */
#define CPG_STBACK2_BIT_STBAK26 (0x40u) /* Ethernet */
#define CPG_STBACK2_BIT_STBAK25 (0x20u) /* VDC5_0 */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#else
#define CPG_STBACK2_BIT_STBAK24 (0x10u) /* VCD5_1 */
#define CPG_STBACK2_BIT_STBAK23 (0x08u) /* IMR_LS2_0 */
#define CPG_STBACK2_BIT_STBAK22 (0x04u) /* IMR_LS2_1 */
#define CPG_STBACK2_BIT_STBAK21 (0x02u) /* IMR_LSD */
#define CPG_STBACK2_BIT_STBAK20 (0x01u) /* OpenVG */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_RRAMKP_BIT_RRAMKP3  (0x08u) /* RRAM KP Page3 */
#define CPG_RRAMKP_BIT_RRAMKP2  (0x04u) /* RRAM KP Page2 */
#define CPG_RRAMKP_BIT_RRAMKP1  (0x02u) /* RRAM KP Page1 */
#define CPG_RRAMKP_BIT_RRAMKP0  (0x01u) /* RRAM KP Page0 */
#define CPG_DSCTR_BIT_EBUSKEEPE (0x80u) /* Retention of External Memory Control Pin State */
#define CPG_DSCTR_BIT_RAMBOOT   (0x40u) /* Selection of Method after Returning from Deep Standby Mode */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#define CPG_DSSSR_BIT_P2_0      (0x4000u) /* P2_0 */
#define CPG_DSSSR_BIT_P7_6      (0x2000u) /* P7_6 */
#define CPG_DSSSR_BIT_P7_9      (0x1000u) /* P7_9 */
#define CPG_DSSSR_BIT_P5_10     (0x0800u) /* P5_10 */
#define CPG_DSSSR_BIT_P2_2      (0x0400u) /* P2_2 */
#define CPG_DSSSR_BIT_P7_2      (0x0200u) /* P7_2 */
#else
#define CPG_DSSSR_BIT_P6_2      (0x4000u) /* P6_2 */
#define CPG_DSSSR_BIT_P3_9      (0x2000u) /* P3_9 */
#define CPG_DSSSR_BIT_P3_1      (0x1000u) /* P3_1 */
#define CPG_DSSSR_BIT_P2_12     (0x0800u) /* P2_12 */
#define CPG_DSSSR_BIT_P8_7      (0x0400u) /* P8_7 */
#define CPG_DSSSR_BIT_P3_3      (0x0200u) /* P3_3 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_DSSSR_BIT_NMI       (0x0100u) /* NMI */
#define CPG_DSSSR_BIT_RTCAR     (0x0040u) /* RTCAR */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#define CPG_DSSSR_BIT_P5_9      (0x0020u) /* P5_9 */
#define CPG_DSSSR_BIT_P7_3      (0x0010u) /* P7_3 */
#define CPG_DSSSR_BIT_P5_8      (0x0008u) /* P5_8 */
#define CPG_DSSSR_BIT_P2_7      (0x0004u) /* P2_7 */
#define CPG_DSSSR_BIT_P2_9      (0x0002u) /* P2_9 */
#define CPG_DSSSR_BIT_P6_7      (0x0001u) /* P6_7 */
#define CPG_DSESR_BIT_P2_0E     (0x4000u) /* P2_0 */
#define CPG_DSESR_BIT_P7_6E     (0x2000u) /* P7_6 */
#define CPG_DSESR_BIT_P7_9E     (0x1000u) /* P7_9 */
#define CPG_DSESR_BIT_P5_10E    (0x0800u) /* P5_10 */
#define CPG_DSESR_BIT_P2_2E     (0x0400u) /* P2_2 */
#define CPG_DSESR_BIT_P7_2E     (0x0200u) /* P7_2 */
#else
#define CPG_DSSSR_BIT_P6_4      (0x0020u) /* P6_4 */
#define CPG_DSSSR_BIT_P5_9      (0x0010u) /* P5_9 */
#define CPG_DSSSR_BIT_P7_8      (0x0008u) /* P7_8 */
#define CPG_DSSSR_BIT_P2_15     (0x0004u) /* P2_15 */
#define CPG_DSSSR_BIT_P9_1      (0x0002u) /* P9_1 */
#define CPG_DSSSR_BIT_P8_2      (0x0001u) /* P8_2 */
#define CPG_DSESR_BIT_P6_2E     (0x4000u) /* P6_2 */
#define CPG_DSESR_BIT_P3_9E     (0x2000u) /* P3_9 */
#define CPG_DSESR_BIT_P3_1E     (0x1000u) /* P3_1 */
#define CPG_DSESR_BIT_P2_12E    (0x0800u) /* P2_12 */
#define CPG_DSESR_BIT_P8_7E     (0x0400u) /* P8_7 */
#define CPG_DSESR_BIT_P3_3E     (0x0200u) /* P3_3 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */

#define CPG_DSESR_BIT_NMIE      (0x0100u) /* NMI */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#define CPG_DSESR_BIT_P5_9E     (0x0020u) /* P5_9 */
#define CPG_DSESR_BIT_P7_3E     (0x0010u) /* P7_3 */
#define CPG_DSESR_BIT_P5_8E     (0x0008u) /* P5_8 */
#define CPG_DSESR_BIT_P2_7E     (0x0004u) /* P2_7 */
#define CPG_DSESR_BIT_P2_9E     (0x0002u) /* P2_9 */
#define CPG_DSESR_BIT_P6_7E     (0x0001u) /* P6_7 */
#else
#define CPG_DSESR_BIT_P6_4E     (0x0020u) /* P6_4 */
#define CPG_DSESR_BIT_P5_9E     (0x0010u) /* P5_9 */
#define CPG_DSESR_BIT_P7_8E     (0x0008u) /* P7_8 */
#define CPG_DSESR_BIT_P2_15E    (0x0004u) /* P2_15 */
#define CPG_DSESR_BIT_P9_1E     (0x0002u) /* P9_1 */
#define CPG_DSESR_BIT_P8_2E     (0x0001u) /* P8_2 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_DSFR_BIT_IOKEEP     (0x8000u) /* Release of Pin State Retention */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#define CPG_DSFR_BIT_P2_0F      (0x4000u) /* P2_0 */
#define CPG_DSFR_BIT_P7_6F      (0x2000u) /* P7_6 */
#define CPG_DSFR_BIT_P7_9F      (0x1000u) /* P7_9 */
#define CPG_DSFR_BIT_P5_10F     (0x0800u) /* P5_10 */
#define CPG_DSFR_BIT_P2_2F      (0x0400u) /* P2_2 */
#define CPG_DSFR_BIT_P7_2F      (0x0200u) /* P7_2 */
#else
#define CPG_DSFR_BIT_P6_2F      (0x4000u) /* P6_2 */
#define CPG_DSFR_BIT_P3_9F      (0x2000u) /* P3_9 */
#define CPG_DSFR_BIT_P3_1F      (0x1000u) /* P3_1 */
#define CPG_DSFR_BIT_P2_12F     (0x0800u) /* P2_12 */
#define CPG_DSFR_BIT_P8_7F      (0x0400u) /* P8_7 */
#define CPG_DSFR_BIT_P3_3F      (0x0200u) /* P3_3 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_DSFR_BIT_NMIF       (0x0100u) /* NMI */
#define CPG_DSFR_BIT_RTCARF     (0x0040u) /* RTCAR */
#if (TARGET_RZA1 <= TARGET_RZA1LU)
#define CPG_DSFR_BIT_P5_9F      (0x0020u) /* P5_9 */
#define CPG_DSFR_BIT_P7_3F      (0x0010u) /* P7_3 */
#define CPG_DSFR_BIT_P5_8F      (0x0008u) /* P5_8 */
#define CPG_DSFR_BIT_P2_7F      (0x0004u) /* P2_7 */
#define CPG_DSFR_BIT_P2_9F      (0x0002u) /* P2_9 */
#define CPG_DSFR_BIT_P6_7F      (0x0001u) /* P6_7 */
#else
#define CPG_DSFR_BIT_P6_4F      (0x0020u) /* P6_4 */
#define CPG_DSFR_BIT_P5_9F      (0x0010u) /* P5_9 */
#define CPG_DSFR_BIT_P7_8F      (0x0008u) /* P7_8 */
#define CPG_DSFR_BIT_P2_15F     (0x0004u) /* P2_15 */
#define CPG_DSFR_BIT_P9_1F      (0x0002u) /* P9_1 */
#define CPG_DSFR_BIT_P8_2F      (0x0001u) /* P8_2 */
#define CPG_XTALCTR_BIT_GAIN1   (0x02u)   /* RTC_X3, RTC_X4 */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */
#define CPG_XTALCTR_BIT_GAIN0   (0x01u)   /* EXTAL, XTAL */

/*@}*/ /* end of group Renesas_RZ_A1_CPG */

/******************************************************************************/
/*                        GPIO   Settings                                     */
/******************************************************************************/
#if (TARGET_RZA1 <= TARGET_RZA1LU)
/** @addtogroup Renesas_RZ_A1_LU_GPIO Renesas_RZ_A1 GPIO Bit definitions */
#else
/** @addtogroup Renesas_RZ_A1_H_GPIO Renesas_RZ_A1 GPIO Bit definitions */
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */


#define GPIO_BIT_N0  (1u <<  0)
#define GPIO_BIT_N1  (1u <<  1)
#define GPIO_BIT_N2  (1u <<  2)
#define GPIO_BIT_N3  (1u <<  3)
#define GPIO_BIT_N4  (1u <<  4)
#define GPIO_BIT_N5  (1u <<  5)
#define GPIO_BIT_N6  (1u <<  6)
#define GPIO_BIT_N7  (1u <<  7)
#define GPIO_BIT_N8  (1u <<  8)
#define GPIO_BIT_N9  (1u <<  9)
#define GPIO_BIT_N10 (1u << 10)
#define GPIO_BIT_N11 (1u << 11)
#define GPIO_BIT_N12 (1u << 12)
#define GPIO_BIT_N13 (1u << 13)
#define GPIO_BIT_N14 (1u << 14)
#define GPIO_BIT_N15 (1u << 15)


#if (TARGET_RZA1 <= TARGET_RZA1LU)
#define MD_BOOT10_MASK    (0x3u)
#define MD_BOOT1_MASK     (0x2u)


#define MD_BOOT10_BM0     (0x0u)
#define MD_BOOT10_BM1     (0x2u)
#define MD_BOOT10_BM2     (0x1u)
#define MD_BOOT10_BM3     (0x3u)
#else
#define MD_BOOT10_MASK    (0x3)

#define MD_BOOT10_BM0     (0x0)
#define MD_BOOT10_BM1     (0x2)
#define MD_BOOT10_BM3     (0x1)
#define MD_BOOT10_BM4_5   (0x3)
#endif /* (TARGET_RZA1 <= TARGET_RZA1LU) */

#define MD_CLK        (1u << 2)
#define MD_CLKS       (1u << 3)

/*@}*/ /* end of group Renesas_RZ_A1_GPIO */

#ifdef __cplusplus
}
#endif

#endif  // __Renesas_RZ_A1_H__
