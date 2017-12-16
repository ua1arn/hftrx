/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
*******************************************************************************/
/*******************************************************************************
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name     : adc_iobitmask.h
* Device(s)     : RZ/A1H RSK2+RZA1H
* H/W Platform  : RSK2+RZA1H CPU Board
* Description   : ADC register define header
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 18.06.2013 1.00
*******************************************************************************/

#ifndef ADC_IOBITMASK_H
#define ADC_IOBITMASK_H


/* ==== Mask values for IO registers ==== */
#define ADC_ADDRA_D                            (0xFFFFu)

#define ADC_ADDRB_D                            (0xFFFFu)

#define ADC_ADDRC_D                            (0xFFFFu)

#define ADC_ADDRD_D                            (0xFFFFu)

#define ADC_ADDRE_D                            (0xFFFFu)

#define ADC_ADDRF_D                            (0xFFFFu)

#define ADC_ADDRG_D                            (0xFFFFu)

#define ADC_ADDRH_D                            (0xFFFFu)

#define ADC_ADCMPHA_D                          (0xFFFFu)

#define ADC_ADCMPLA_D                          (0xFFFFu)

#define ADC_ADCMPHB_D                          (0xFFFFu)

#define ADC_ADCMPLB_D                          (0xFFFFu)

#define ADC_ADCMPHC_D                          (0xFFFFu)

#define ADC_ADCMPLC_D                          (0xFFFFu)

#define ADC_ADCMPHD_D                          (0xFFFFu)

#define ADC_ADCMPLD_D                          (0xFFFFu)

#define ADC_ADCMPHE_D                          (0xFFFFu)

#define ADC_ADCMPLE_D                          (0xFFFFu)

#define ADC_ADCMPHF_D                          (0xFFFFu)

#define ADC_ADCMPLF_D                          (0xFFFFu)

#define ADC_ADCMPHG_D                          (0xFFFFu)

#define ADC_ADCMPLG_D                          (0xFFFFu)

#define ADC_ADCMPHH_D                          (0xFFFFu)

#define ADC_ADCMPLH_D                          (0xFFFFu)

#define ADC_ADCSR_CH                           (0x0007u)
#define ADC_ADCSR_MDS                          (0x0038u)
#define ADC_ADCSR_CKS                          (0x01C0u)
#define ADC_ADCSR_TRGS                         (0x1E00u)
#define ADC_ADCSR_ADST                         (0x2000u)
#define ADC_ADCSR_ADIE                         (0x4000u)
#define ADC_ADCSR_ADF                          (0x8000u)

#define ADC_ADCMPER_LLMENA                     (0x0001u)
#define ADC_ADCMPER_LLMENB                     (0x0002u)
#define ADC_ADCMPER_LLMENC                     (0x0004u)
#define ADC_ADCMPER_LLMEND                     (0x0008u)
#define ADC_ADCMPER_LLMENE                     (0x0010u)
#define ADC_ADCMPER_LLMENF                     (0x0020u)
#define ADC_ADCMPER_LLMENG                     (0x0040u)
#define ADC_ADCMPER_LLMENH                     (0x0080u)
#define ADC_ADCMPER_HLMENA                     (0x0100u)
#define ADC_ADCMPER_HLMENB                     (0x0200u)
#define ADC_ADCMPER_HLMENC                     (0x0400u)
#define ADC_ADCMPER_HLMEND                     (0x0800u)
#define ADC_ADCMPER_HLMENE                     (0x1000u)
#define ADC_ADCMPER_HLMENF                     (0x2000u)
#define ADC_ADCMPER_HLMENG                     (0x4000u)
#define ADC_ADCMPER_HLMENH                     (0x8000u)

#define ADC_ADCMPSR_LUDRA                      (0x0001u)
#define ADC_ADCMPSR_LUDRB                      (0x0002u)
#define ADC_ADCMPSR_LUDRC                      (0x0004u)
#define ADC_ADCMPSR_LUDRD                      (0x0008u)
#define ADC_ADCMPSR_LUDRE                      (0x0010u)
#define ADC_ADCMPSR_LUDRF                      (0x0020u)
#define ADC_ADCMPSR_LUDRG                      (0x0040u)
#define ADC_ADCMPSR_LUDRH                      (0x0080u)
#define ADC_ADCMPSR_HOVRA                      (0x0100u)
#define ADC_ADCMPSR_HOVRB                      (0x0200u)
#define ADC_ADCMPSR_HOVRC                      (0x0400u)
#define ADC_ADCMPSR_HOVRD                      (0x0800u)
#define ADC_ADCMPSR_HOVRE                      (0x1000u)
#define ADC_ADCMPSR_HOVRF                      (0x2000u)
#define ADC_ADCMPSR_HOVRG                      (0x4000u)
#define ADC_ADCMPSR_HOVRH                      (0x8000u)


/* ==== IOレジスタへのシフト値 ==== */
#define ADC_ADDRA_D_SHIFT                      (0u)

#define ADC_ADDRB_D_SHIFT                      (0u)

#define ADC_ADDRC_D_SHIFT                      (0u)

#define ADC_ADDRD_D_SHIFT                      (0u)

#define ADC_ADDRE_D_SHIFT                      (0u)

#define ADC_ADDRF_D_SHIFT                      (0u)

#define ADC_ADDRG_D_SHIFT                      (0u)

#define ADC_ADDRH_D_SHIFT                      (0u)

#define ADC_ADCMPHA_D_SHIFT                    (0u)

#define ADC_ADCMPLA_D_SHIFT                    (0u)

#define ADC_ADCMPHB_D_SHIFT                    (0u)

#define ADC_ADCMPLB_D_SHIFT                    (0u)

#define ADC_ADCMPHC_D_SHIFT                    (0u)

#define ADC_ADCMPLC_D_SHIFT                    (0u)

#define ADC_ADCMPHD_D_SHIFT                    (0u)

#define ADC_ADCMPLD_D_SHIFT                    (0u)

#define ADC_ADCMPHE_D_SHIFT                    (0u)

#define ADC_ADCMPLE_D_SHIFT                    (0u)

#define ADC_ADCMPHF_D_SHIFT                    (0u)

#define ADC_ADCMPLF_D_SHIFT                    (0u)

#define ADC_ADCMPHG_D_SHIFT                    (0u)

#define ADC_ADCMPLG_D_SHIFT                    (0u)

#define ADC_ADCMPHH_D_SHIFT                    (0u)

#define ADC_ADCMPLH_D_SHIFT                    (0u)

#define ADC_ADCSR_CH_SHIFT                     (0u)
#define ADC_ADCSR_MDS_SHIFT                    (3u)
#define ADC_ADCSR_CKS_SHIFT                    (6u)
#define ADC_ADCSR_TRGS_SHIFT                   (9u)
#define ADC_ADCSR_ADST_SHIFT                   (13u)
#define ADC_ADCSR_ADIE_SHIFT                   (14u)
#define ADC_ADCSR_ADF_SHIFT                    (15u)

#define ADC_ADCMPER_LLMENA_SHIFT               (0u)
#define ADC_ADCMPER_LLMENB_SHIFT               (1u)
#define ADC_ADCMPER_LLMENC_SHIFT               (2u)
#define ADC_ADCMPER_LLMEND_SHIFT               (3u)
#define ADC_ADCMPER_LLMENE_SHIFT               (4u)
#define ADC_ADCMPER_LLMENF_SHIFT               (5u)
#define ADC_ADCMPER_LLMENG_SHIFT               (6u)
#define ADC_ADCMPER_LLMENH_SHIFT               (7u)
#define ADC_ADCMPER_HLMENA_SHIFT               (8u)
#define ADC_ADCMPER_HLMENB_SHIFT               (9u)
#define ADC_ADCMPER_HLMENC_SHIFT               (10u)
#define ADC_ADCMPER_HLMEND_SHIFT               (11u)
#define ADC_ADCMPER_HLMENE_SHIFT               (12u)
#define ADC_ADCMPER_HLMENF_SHIFT               (13u)
#define ADC_ADCMPER_HLMENG_SHIFT               (14u)
#define ADC_ADCMPER_HLMENH_SHIFT               (15u)

#define ADC_ADCMPSR_LUDRA_SHIFT                (0u)
#define ADC_ADCMPSR_LUDRB_SHIFT                (1u)
#define ADC_ADCMPSR_LUDRC_SHIFT                (2u)
#define ADC_ADCMPSR_LUDRD_SHIFT                (3u)
#define ADC_ADCMPSR_LUDRE_SHIFT                (4u)
#define ADC_ADCMPSR_LUDRF_SHIFT                (5u)
#define ADC_ADCMPSR_LUDRG_SHIFT                (6u)
#define ADC_ADCMPSR_LUDRH_SHIFT                (7u)
#define ADC_ADCMPSR_HOVRA_SHIFT                (8u)
#define ADC_ADCMPSR_HOVRB_SHIFT                (9u)
#define ADC_ADCMPSR_HOVRC_SHIFT                (10u)
#define ADC_ADCMPSR_HOVRD_SHIFT                (11u)
#define ADC_ADCMPSR_HOVRE_SHIFT                (12u)
#define ADC_ADCMPSR_HOVRF_SHIFT                (13u)
#define ADC_ADCMPSR_HOVRG_SHIFT                (14u)
#define ADC_ADCMPSR_HOVRH_SHIFT                (15u)


#endif /* __ADC_IOBITMASK_H__ */

/* End of File */
