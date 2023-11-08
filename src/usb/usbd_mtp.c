/*
 * usbd_mtp.c
 *
 *  Created on: Sep 5, 2022
 *      Author: gena
 */
#include "hardware.h"

#if WITHUSBHW && WITHUSBDMTP

#include "buffers.h"
#include "formats.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usb200.h"
#include "usbch9.h"
#include "usbd_mtp.h"


/* Exported Define -----------------------------------------------------------*/
#define USBD_MTP_DEVICE_PROP_SUPPORTED                           1U
#define USBD_MTP_CAPTURE_FORMAT_SUPPORTED                        1U
#define USBD_MTP_VEND_EXT_DESC_SUPPORTED                         1U
#define USBD_MTP_EVENTS_SUPPORTED                                1U

#if USBD_MTP_EVENTS_SUPPORTED  == 1
#define SUPP_EVENTS_LEN                                        (uint8_t)((uint8_t)sizeof(SuppEvents) / 2U)
#else
#define SUPP_EVENTS_LEN                                         0U
#endif /* USBD_MTP_EVENTS_SUPPORTED */

#if USBD_MTP_VEND_EXT_DESC_SUPPORTED == 1
#define VEND_EXT_DESC_LEN                                      (sizeof(VendExtDesc) / 2U)
#else
#define VEND_EXT_DESC_LEN                                       0U
#endif /* USBD_MTP_VEND_EXT_DESC_SUPPORTED */

#if USBD_MTP_CAPTURE_FORMAT_SUPPORTED  == 1
#define SUPP_CAPT_FORMAT_LEN                                   (uint8_t)((uint8_t)sizeof(SuppCaptFormat) / 2U)
#else
#define SUPP_CAPT_FORMAT_LEN                                    0U
#endif /* USBD_MTP_CAPTURE_FORMAT_SUPPORTED */

#if USBD_MTP_DEVICE_PROP_SUPPORTED == 1
#define SUPP_DEVICE_PROP_LEN                                   (uint8_t)((uint8_t)sizeof(DevicePropSupp) / 2U)
#else
#define SUPP_DEVICE_PROP_LEN                                    0U
#endif /* USBD_MTP_DEVICE_PROP_SUPPORTED */

#define MTP_IF_SCRATCH_BUFF_SZE                                1024U

static __ALIGN_BEGIN USBD_MTP_HandleTypeDef mtpHandle __ALIGN_END;
static __ALIGN_BEGIN USBD_MTP_ItfTypeDef mtpIfHandle __ALIGN_END;

typedef enum
{
  DATA_TYPE = 0x00,
  REP_TYPE = 0x01,
} MTP_CONTAINER_TYPE;


typedef enum
{
  READ_FIRST_DATA = 0x00,
  READ_REST_OF_DATA = 0x01,
} MTP_READ_DATA_STATUS;

/* MTP OBJECT PROPERTIES supported*/
#define    MTP_OB_PROP_STORAGE_ID                               0xDC01U
#define    MTP_OB_PROP_OBJECT_FORMAT                            0xDC02U
#define    MTP_OB_PROP_PROTECTION_STATUS                        0xDC03U
#define    MTP_OB_PROP_OBJECT_SIZE                              0xDC04U
#define    MTP_OB_PROP_ASSOC_TYPE                               0xDC05U
#define    MTP_OB_PROP_ASSOC_DESC                               0xDC06U
#define    MTP_OB_PROP_OBJ_FILE_NAME                            0xDC07U
#define    MTP_OB_PROP_DATE_CREATED                             0xDC08U
#define    MTP_OB_PROP_DATE_MODIFIED                            0xDC09U
#define    MTP_OB_PROP_KEYWORDS                                 0xDC0AU
#define    MTP_OB_PROP_PARENT_OBJECT                            0xDC0BU
#define    MTP_OB_PROP_ALLOWED_FOLD_CONTENTS                    0xDC0CU
#define    MTP_OB_PROP_HIDDEN                                   0xDC0DU
#define    MTP_OB_PROP_SYSTEM_OBJECT                            0xDC0EU
#define    MTP_OB_PROP_PERS_UNIQ_OBJ_IDEN                       0xDC41U
#define    MTP_OB_PROP_SYNCID                                   0xDC42U
#define    MTP_OB_PROP_PROPERTY_BAG                             0xDC43U
#define    MTP_OB_PROP_NAME                                     0xDC44U
#define    MTP_OB_PROP_CREATED_BY                               0xDC45U
#define    MTP_OB_PROP_ARTIST                                   0xDC46U
#define    MTP_OB_PROP_DATE_AUTHORED                            0xDC47U
#define    MTP_OB_PROP_DESCRIPTION                              0xDC48U
#define    MTP_OB_PROP_URL_REFERENCE                            0xDC49U
#define    MTP_OB_PROP_LANGUAGELOCALE                           0xDC4AU
#define    MTP_OB_PROP_COPYRIGHT_INFORMATION                    0xDC4BU
#define    MTP_OB_PROP_SOURCE                                   0xDC4CU
#define    MTP_OB_PROP_ORIGIN_LOCATION                          0xDC4DU
#define    MTP_OB_PROP_DATE_ADDED                               0xDC4EU
#define    MTP_OB_PROP_NON_CONSUMABLE                           0xDC4FU
#define    MTP_OB_PROP_CORRUPTUNPLAYABLE                        0xDC50U
#define    MTP_OB_PROP_PRODUCERSERIALNUMBER                     0xDC51U
#define    MTP_OB_PROP_REPRESENTATIVE_SAMPLE_FORMAT             0xDC81U
#define    MTP_OB_PROP_REPRESENTATIVE_SAMPLE_SIZE               0xDC82U
#define    MTP_OB_PROP_REPRESENTATIVE_SAMPLE_HEIGHT             0xDC83U
#define    MTP_OB_PROP_REPRESENTATIVE_SAMPLE_WIDTH              0xDC84U
#define    MTP_OB_PROP_REPRESENTATIVE_SAMPLE_DURATION           0xDC85U
#define    MTP_OB_PROP_REPRESENTATIVE_SAMPLE_DATA               0xDC86U
#define    MTP_OB_PROP_WIDTH                                    0xDC87U
#define    MTP_OB_PROP_HEIGHT                                   0xDC88U
#define    MTP_OB_PROP_DURATION                                 0xDC89U
#define    MTP_OB_PROP_RATING                                   0xDC8AU
#define    MTP_OB_PROP_TRACK                                    0xDC8BU
#define    MTP_OB_PROP_GENRE                                    0xDC8CU
#define    MTP_OB_PROP_CREDITS                                  0xDC8DU
#define    MTP_OB_PROP_LYRICS                                   0xDC8EU
#define    MTP_OB_PROP_SUBSCRIPTION_CONTENT_ID                  0xDC8FU
#define    MTP_OB_PROP_PRODUCED_BY                              0xDC90U
#define    MTP_OB_PROP_USE_COUNT                                0xDC91U
#define    MTP_OB_PROP_SKIP_COUNT                               0xDC92U
#define    MTP_OB_PROP_LAST_ACCESSED                            0xDC93U
#define    MTP_OB_PROP_PARENTAL_RATING                          0xDC94U
#define    MTP_OB_PROP_META_GENRE                               0xDC95U
#define    MTP_OB_PROP_COMPOSER                                 0xDC96U
#define    MTP_OB_PROP_EFFECTIVE_RATING                         0xDC97U
#define    MTP_OB_PROP_SUBTITLE                                 0xDC98U
#define    MTP_OB_PROP_ORIGINAL_RELEASE_DATE                    0xDC99U
#define    MTP_OB_PROP_ALBUM_NAME                               0xDC9AU
#define    MTP_OB_PROP_ALBUM_ARTIST                             0xDC9BU
#define    MTP_OB_PROP_MOOD                                     0xDC9CU
#define    MTP_OB_PROP_DRM_STATUS                               0xDC9DU
#define    MTP_OB_PROP_SUB_DESCRIPTION                          0xDC9EU
#define    MTP_OB_PROP_IS_CROPPED                               0xDCD1U
#define    MTP_OB_PROP_IS_COLOUR_CORRECTED                      0xDCD2U
#define    MTP_OB_PROP_IMAGE_BIT_DEPTH                          0xDCD3U
#define    MTP_OB_PROP_FNUMBER                                  0xDCD4U
#define    MTP_OB_PROP_EXPOSURE_TIME                            0xDCD5U
#define    MTP_OB_PROP_EXPOSURE_INDEX                           0xDCD6U
#define    MTP_OB_PROP_TOTAL_BITRATE                            0xDE91U
#define    MTP_OB_PROP_BITRATE_TYPE                             0xDE92U
#define    MTP_OB_PROP_SAMPLE_RATE                              0xDE93U
#define    MTP_OB_PROP_NUMBER_OF_CHANNELS                       0xDE94U
#define    MTP_OB_PROP_AUDIO_BITDEPTH                           0xDE95U
#define    MTP_OB_PROP_SCAN_TYPE                                0xDE97U
#define    MTP_OB_PROP_AUDIO_WAVE_CODEC                         0xDE99U
#define    MTP_OB_PROP_AUDIO_BITRATE                            0xDE9AU
#define    MTP_OB_PROP_VIDEO_FOURCC_CODEC                       0xDE9BU
#define    MTP_OB_PROP_VIDEO_BITRATE                            0xDE9CU
#define    MTP_OB_PROP_FRAMES_PER_THOUSAND_SECONDS              0xDE9DU
#define    MTP_OB_PROP_KEYFRAME_DISTANCE                        0xDE9EU
#define    MTP_OB_PROP_BUFFER_SIZE                              0xDE9FU
#define    MTP_OB_PROP_ENCODING_QUALITY                         0xDEA0U
#define    MTP_OB_PROP_ENCODING_PROFILE                         0xDEA1U
#define    MTP_OB_PROP_DISPLAY_NAME                             0xDCE0U
#define    MTP_OB_PROP_BODY_TEXT                                0xDCE1U
#define    MTP_OB_PROP_SUBJECT                                  0xDCE2U
#define    MTP_OB_PROP_PRIORITY                                 0xDCE3U
#define    MTP_OB_PROP_GIVEN_NAME                               0xDD00U
#define    MTP_OB_PROP_MIDDLE_NAMES                             0xDD01U
#define    MTP_OB_PROP_FAMILY_NAME                              0xDD02U
#define    MTP_OB_PROP_PREFIX                                   0xDD03U
#define    MTP_OB_PROP_SUFFIX                                   0xDD04U
#define    MTP_OB_PROP_PHONETIC_GIVEN_NAME                      0xDD05U
#define    MTP_OB_PROP_PHONETIC_FAMILY_NAME                     0xDD06U
#define    MTP_OB_PROP_EMAIL_PRIMARY                            0xDD07U
#define    MTP_OB_PROP_EMAIL_PERSONAL_1                         0xDD08U
#define    MTP_OB_PROP_EMAIL_PERSONAL_2                         0xDD09U
#define    MTP_OB_PROP_EMAIL_BUSINESS_1                         0xDD0AU
#define    MTP_OB_PROP_EMAIL_BUSINESS_2                         0xDD0BU
#define    MTP_OB_PROP_EMAIL_OTHERS                             0xDD0CU
#define    MTP_OB_PROP_PHONE_NUMBER_PRIMARY                     0xDD0DU
#define    MTP_OB_PROP_PHONE_NUMBER_PERSONAL                    0xDD0EU
#define    MTP_OB_PROP_PHONE_NUMBER_PERSONAL_2                  0xDD0FU
#define    MTP_OB_PROP_PHONE_NUMBER_BUSINESS                    0xDD10U
#define    MTP_OB_PROP_PHONE_NUMBER_BUSINESS_2                  0xDD11U
#define    MTP_OB_PROP_PHONE_NUMBER_MOBILE                      0xDD12U
#define    MTP_OB_PROP_PHONE_NUMBER_MOBILE_2                    0xDD13U
#define    MTP_OB_PROP_FAX_NUMBER_PRIMARY                       0xDD14U
#define    MTP_OB_PROP_FAX_NUMBER_PERSONAL                      0xDD15U
#define    MTP_OB_PROP_FAX_NUMBER_BUSINESS                      0xDD16U
#define    MTP_OB_PROP_PAGER_NUMBER                             0xDD17U
#define    MTP_OB_PROP_PHONE_NUMBER_OTHERS                      0xDD18U
#define    MTP_OB_PROP_PRIMARY_WEB_ADDRESS                      0xDD19U
#define    MTP_OB_PROP_PERSONAL_WEB_ADDRESS                     0xDD1AU
#define    MTP_OB_PROP_BUSINESS_WEB_ADDRESS                     0xDD1BU
#define    MTP_OB_PROP_INSTANT_MESSENGER_ADDRESS                0xDD1CU
#define    MTP_OB_PROP_INSTANT_MESSENGER_ADDRESS_2              0xDD1DU
#define    MTP_OB_PROP_INSTANT_MESSENGER_ADDRESS_3              0xDD1EU
#define    MTP_OB_PROP_POSTAL_ADDRESS_PERSONAL_FULL             0xDD1FU
#define    MTP_OB_PROP_POSTAL_ADDRESS_PERSONAL_LINE_1           0xDD20U
#define    MTP_OB_PROP_POSTAL_ADDRESS_PERSONAL_LINE_2           0xDD21U
#define    MTP_OB_PROP_POSTAL_ADDRESS_PERSONAL_CITY             0xDD22U
#define    MTP_OB_PROP_POSTAL_ADDRESS_PERSONAL_REGION           0xDD23U
#define    MTP_OB_PROP_POSTAL_ADDRESS_PERSONAL_POSTAL_CODE      0xDD24U
#define    MTP_OB_PROP_POSTAL_ADDRESS_PERSONAL_COUNTRY          0xDD25U
#define    MTP_OB_PROP_POSTAL_ADDRESS_BUSINESS_FULL             0xDD26U
#define    MTP_OB_PROP_POSTAL_ADDRESS_BUSINESS_LINE_1           0xDD27U
#define    MTP_OB_PROP_POSTAL_ADDRESS_BUSINESS_LINE_2           0xDD28U
#define    MTP_OB_PROP_POSTAL_ADDRESS_BUSINESS_CITY             0xDD29U
#define    MTP_OB_PROP_POSTAL_ADDRESS_BUSINESS_REGION           0xDD2AU
#define    MTP_OB_PROP_POSTAL_ADDRESS_BUSINESS_POSTAL_CODE      0xDD2BU
#define    MTP_OB_PROP_POSTAL_ADDRESS_BUSINESS_COUNTRY          0xDD2CU
#define    MTP_OB_PROP_POSTAL_ADDRESS_OTHER_FULL                0xDD2DU
#define    MTP_OB_PROP_POSTAL_ADDRESS_OTHER_LINE_1              0xDD2EU
#define    MTP_OB_PROP_POSTAL_ADDRESS_OTHER_LINE_2              0xDD2FU
#define    MTP_OB_PROP_POSTAL_ADDRESS_OTHER_CITY                0xDD30U
#define    MTP_OB_PROP_POSTAL_ADDRESS_OTHER_REGION              0xDD31U
#define    MTP_OB_PROP_POSTAL_ADDRESS_OTHER_POSTAL_CODE         0xDD32U
#define    MTP_OB_PROP_POSTAL_ADDRESS_OTHER_COUNTRY             0xDD33U
#define    MTP_OB_PROP_ORGANIZATION_NAME                        0xDD34U
#define    MTP_OB_PROP_PHONETIC_ORGANIZATION_NAME               0xDD35U
#define    MTP_OB_PROP_ROLE                                     0xDD36U
#define    MTP_OB_PROP_BIRTHDATE                                0xDD37U
#define    MTP_OB_PROP_MESSAGE_TO                               0xDD40U
#define    MTP_OB_PROP_MESSAGE_CC                               0xDD41U
#define    MTP_OB_PROP_MESSAGE_BCC                              0xDD42U
#define    MTP_OB_PROP_MESSAGE_READ                             0xDD43U
#define    MTP_OB_PROP_MESSAGE_RECEIVED_TIME                    0xDD44U
#define    MTP_OB_PROP_MESSAGE_SENDER                           0xDD45U
#define    MTP_OB_PROP_ACT_BEGIN_TIME                           0xDD50U
#define    MTP_OB_PROP_ACT_END_TIME                             0xDD51U
#define    MTP_OB_PROP_ACT_LOCATION                             0xDD52U
#define    MTP_OB_PROP_ACT_REQUIRED_ATTENDEES                   0xDD54U
#define    MTP_OB_PROP_ACT_OPTIONAL_ATTENDEES                   0xDD55U
#define    MTP_OB_PROP_ACT_RESOURCES                            0xDD56U
#define    MTP_OB_PROP_ACT_ACCEPTED                             0xDD57U
#define    MTP_OB_PROP_OWNER                                    0xDD5DU
#define    MTP_OB_PROP_EDITOR                                   0xDD5EU
#define    MTP_OB_PROP_WEBMASTER                                0xDD5FU
#define    MTP_OB_PROP_URL_SOURCE                               0xDD60U
#define    MTP_OB_PROP_URL_DESTINATION                          0xDD61U
#define    MTP_OB_PROP_TIME_BOOKMARK                            0xDD62U
#define    MTP_OB_PROP_OBJECT_BOOKMARK                          0xDD63U
#define    MTP_OB_PROP_BYTE_BOOKMARK                            0xDD64U
#define    MTP_OB_PROP_LAST_BUILD_DATE                          0xDD70U
#define    MTP_OB_PROP_TIME_TO_LIVE                             0xDD71U
#define    MTP_OB_PROP_MEDIA_GUID                               0xDD72U

/*  MTP event codes*/
#define      MTP_EVENT_UNDEFINED                                0x4000U
#define      MTP_EVENT_CANCELTRANSACTION                        0x4001U
#define      MTP_EVENT_OBJECTADDED                              0x4002U
#define      MTP_EVENT_OBJECTREMOVED                            0x4003U
#define      MTP_EVENT_STOREADDED                               0x4004U
#define      MTP_EVENT_STOREREMOVED                             0x4005U
#define      MTP_EVENT_DEVICEPROPCHANGED                        0x4006U
#define      MTP_EVENT_OBJECTINFOCHANGED                        0x4007U
#define      MTP_EVENT_DEVICEINFOCHANGED                        0x4008U
#define      MTP_EVENT_REQUESTOBJECTTRANSFER                    0x4009U
#define      MTP_EVENT_STOREFULL                                0x400AU
#define      MTP_EVENT_DEVICERESET                              0x400BU
#define      MTP_EVENT_STORAGEINFOCHANGED                       0x400CU
#define      MTP_EVENT_CAPTURECOMPLETE                          0x400DU
#define      MTP_EVENT_UNREPORTEDSTATUS                         0x400EU
#define      MTP_EVENT_OBJECTPROPCHANGED                        0xC801U
#define      MTP_EVENT_OBJECTPROPDESCCHANGED                    0xC802U
#define      MTP_EVENT_OBJECTREFERENCESCHANGED                  0xC803U

/*
 * MTP Class specification Revision 1.1
 * Appendix D. Operations
 */

/* Operations code */
#define      MTP_OP_GET_DEVICE_INFO                             0x1001U
#define      MTP_OP_OPEN_SESSION                                0x1002U
#define      MTP_OP_CLOSE_SESSION                               0x1003U
#define      MTP_OP_GET_STORAGE_IDS                             0x1004U
#define      MTP_OP_GET_STORAGE_INFO                            0x1005U
#define      MTP_OP_GET_NUM_OBJECTS                             0x1006U
#define      MTP_OP_GET_OBJECT_HANDLES                          0x1007U
#define      MTP_OP_GET_OBJECT_INFO                             0x1008U
#define      MTP_OP_GET_OBJECT                                  0x1009U
#define      MTP_OP_GET_THUMB                                   0x100AU
#define      MTP_OP_DELETE_OBJECT                               0x100BU
#define      MTP_OP_SEND_OBJECT_INFO                            0x100CU
#define      MTP_OP_SEND_OBJECT                                 0x100DU
#define      MTP_OP_FORMAT_STORE                                0x100FU
#define      MTP_OP_RESET_DEVICE                                0x1010U
#define      MTP_OP_GET_DEVICE_PROP_DESC                        0x1014U
#define      MTP_OP_GET_DEVICE_PROP_VALUE                       0x1015U
#define      MTP_OP_SET_DEVICE_PROP_VALUE                       0x1016U
#define      MTP_OP_RESET_DEVICE_PROP_VALUE                     0x1017U
#define      MTP_OP_TERMINATE_OPEN_CAPTURE                      0x1018U
#define      MTP_OP_MOVE_OBJECT                                 0x1019U
#define      MTP_OP_COPY_OBJECT                                 0x101AU
#define      MTP_OP_GET_PARTIAL_OBJECT                          0x101BU
#define      MTP_OP_INITIATE_OPEN_CAPTURE                       0x101CU
#define      MTP_OP_GET_OBJECT_PROPS_SUPPORTED                  0x9801U
#define      MTP_OP_GET_OBJECT_PROP_DESC                        0x9802U
#define      MTP_OP_GET_OBJECT_PROP_VALUE                       0x9803U
#define      MTP_OP_SET_OBJECT_PROP_VALUE                       0x9804U
#define      MTP_OP_GET_OBJECT_PROPLIST                         0x9805U
#define      MTP_OP_GET_OBJECT_PROP_REFERENCES                  0x9810U
#define      MTP_OP_GETSERVICEIDS                               0x9301U
#define      MTP_OP_GETSERVICEINFO                              0x9302U
#define      MTP_OP_GETSERVICECAPABILITIES                      0x9303U
#define      MTP_OP_GETSERVICEPROPDESC                          0x9304U

/*
 * MTP Class specification Revision 1.1
 * Appendix C. Device Properties
 */

/* MTP device properties code*/
#define    MTP_DEV_PROP_UNDEFINED                               0x5000U
#define    MTP_DEV_PROP_BATTERY_LEVEL                           0x5001U
#define    MTP_DEV_PROP_FUNCTIONAL_MODE                         0x5002U
#define    MTP_DEV_PROP_IMAGE_SIZE                              0x5003U
#define    MTP_DEV_PROP_COMPRESSION_SETTING                     0x5004U
#define    MTP_DEV_PROP_WHITE_BALANCE                           0x5005U
#define    MTP_DEV_PROP_RGB_GAIN                                0x5006U
#define    MTP_DEV_PROP_F_NUMBER                                0x5007U
#define    MTP_DEV_PROP_FOCAL_LENGTH                            0x5008U
#define    MTP_DEV_PROP_FOCUS_DISTANCE                          0x5009U
#define    MTP_DEV_PROP_FOCUS_MODE                              0x500AU
#define    MTP_DEV_PROP_EXPOSURE_METERING_MODE                  0x500BU
#define    MTP_DEV_PROP_FLASH_MODE                              0x500CU
#define    MTP_DEV_PROP_EXPOSURE_TIME                           0x500DU
#define    MTP_DEV_PROP_EXPOSURE_PROGRAM_MODE                   0x500EU
#define    MTP_DEV_PROP_EXPOSURE_INDEX                          0x500FU
#define    MTP_DEV_PROP_EXPOSURE_BIAS_COMPENSATION              0x5010U
#define    MTP_DEV_PROP_DATETIME                                0x5011U
#define    MTP_DEV_PROP_CAPTURE_DELAY                           0x5012U
#define    MTP_DEV_PROP_STILL_CAPTURE_MODE                      0x5013U
#define    MTP_DEV_PROP_CONTRAST                                0x5014U
#define    MTP_DEV_PROP_SHARPNESS                               0x5015U
#define    MTP_DEV_PROP_DIGITAL_ZOOM                            0x5016U
#define    MTP_DEV_PROP_EFFECT_MODE                             0x5017U
#define    MTP_DEV_PROP_BURST_NUMBER                            0x5018U
#define    MTP_DEV_PROP_BURST_INTERVAL                          0x5019U
#define    MTP_DEV_PROP_TIMELAPSE_NUMBER                        0x501AU
#define    MTP_DEV_PROP_TIMELAPSE_INTERVAL                      0x501BU
#define    MTP_DEV_PROP_FOCUS_METERING_MODE                     0x501CU
#define    MTP_DEV_PROP_UPLOAD_URL                              0x501DU
#define    MTP_DEV_PROP_ARTIST                                  0x501EU
#define    MTP_DEV_PROP_COPYRIGHT_INFO                          0x501FU
#define    MTP_DEV_PROP_SYNCHRONIZATION_PARTNER                 0xD401U
#define    MTP_DEV_PROP_DEVICE_FRIENDLY_NAME                    0xD402U
#define    MTP_DEV_PROP_VOLUME                                  0xD403U
#define    MTP_DEV_PROP_SUPPORTEDFORMATSORDERED                 0xD404U
#define    MTP_DEV_PROP_DEVICEICON                              0xD405U
#define    MTP_DEV_PROP_PLAYBACK_RATE                           0xD410U
#define    MTP_DEV_PROP_PLAYBACK_OBJECT                         0xD411U
#define    MTP_DEV_PROP_PLAYBACK_CONTAINER                      0xD412U
#define    MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO          0xD406U
#define    MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE                   0xD407U


/* Container Types */
#define    MTP_CONT_TYPE_UNDEFINED                              0U
#define    MTP_CONT_TYPE_COMMAND                                1U
#define    MTP_CONT_TYPE_DATA                                   2U
#define    MTP_CONT_TYPE_RESPONSE                               3U
#define    MTP_CONT_TYPE_EVENT                                  4U

#ifndef    MTP_STORAGE_ID
#define    MTP_STORAGE_ID                             0x00010001U   /* SD card is inserted*/
#endif  /* MTP_STORAGE_ID */

#define    MTP_NBR_STORAGE_ID                                   1U
#define    FREE_SPACE_IN_OBJ_NOT_USED                           0xFFFFFFFFU

/* MTP storage type */
#define    MTP_STORAGE_UNDEFINED                                0U
#define    MTP_STORAGE_FIXED_ROM                                0x0001U
#define    MTP_STORAGE_REMOVABLE_ROM                            0x0002U
#define    MTP_STORAGE_FIXED_RAM                                0x0003U
#define    MTP_STORAGE_REMOVABLE_RAM                            0x0004U

/* MTP file system type */
#define    MTP_FILESYSTEM_UNDEFINED                             0U
#define    MTP_FILESYSTEM_GENERIC_FLAT                          0x0001U
#define    MTP_FILESYSTEM_GENERIC_HIERARCH                      0x0002U
#define    MTP_FILESYSTEM_DCF                                   0x0003U

/* MTP access capability */
#define    MTP_ACCESS_CAP_RW                                    0U /* read write */
#define    MTP_ACCESS_CAP_RO_WITHOUT_DEL                        0x0001U
#define    MTP_ACCESS_CAP_RO_WITH_DEL                           0x0002U

/* MTP standard data types supported */
#define    MTP_DATATYPE_INT8                                    0x0001U
#define    MTP_DATATYPE_UINT8                                   0x0002U
#define    MTP_DATATYPE_INT16                                   0x0003U
#define    MTP_DATATYPE_UINT16                                  0x0004U
#define    MTP_DATATYPE_INT32                                   0x0005U
#define    MTP_DATATYPE_UINT32                                  0x0006U
#define    MTP_DATATYPE_INT64                                   0x0007U
#define    MTP_DATATYPE_UINT64                                  0x0008U
#define    MTP_DATATYPE_UINT128                                 0x000AU
#define    MTP_DATATYPE_STR                                     0xFFFFU

/* MTP reading only or reading/writing */
#define    MTP_PROP_GET                                         0x00U
#define    MTP_PROP_GET_SET                                     0x01U


/* MTP functional mode */
#define    STANDARD_MODE                                        0U
#define    SLEEP_STATE                                          1U
#define    FUNCTIONAL_MODE                                      STANDARD_MODE

/* MTP device info */
#define    STANDARD_VERSION                                     100U
#define    VEND_EXT_ID                                          0x06U
#define    VEND_EXT_VERSION                                     100U
#define    MANUF_LEN                                            (sizeof(Manuf) / 2U)
#define    MODEL_LEN                                            (sizeof(Model) / 2U)
#define    SUPP_OP_LEN                                          (sizeof(SuppOP) / 2U)
#define    SERIAL_NBR_LEN                                       (sizeof(SerialNbr) / 2U)
#define    DEVICE_VERSION_LEN                                   (sizeof(DeviceVers) / 2U)
#define    SUPP_IMG_FORMAT_LEN                                  (sizeof(SuppImgFormat) / 2U)
#define    SUPP_OBJ_PROP_LEN                                    (sizeof(ObjectPropSupp) / 2U)

static const uint16_t Manuf[] = {'S', 'T', 'M', 0}; /* last 2 bytes must be 0*/
static const uint16_t Model[] = {'S', 'T', 'M', '3', '2', 0}; /* last 2 bytes must be 0*/
static const uint16_t VendExtDesc[] = {'m', 'i', 'c', 'r', 'o', 's', 'o', 'f', 't', '.',
                                       'c', 'o', 'm', ':', ' ', '1', '.', '0', ';', ' ', 0
                                      };  /* last 2 bytes must be 0*/
/*SerialNbr shall be 32 character hexadecimal string for legacy compatibility reasons */
static const uint16_t SerialNbr[] = {'0', '0', '0', '0', '1', '0', '0', '0', '0', '1', '0', '0', '0', '0',
                                     '1', '0', '0', '0', '0', '1', '0', '0', '0', '0', '1', '0', '0', '0',
                                     '0', '1', '0', '0', 0
                                    };  /* last 2 bytes must be 0*/
static const uint16_t DeviceVers[] = {'V', '1', '.', '0', '0', 0}; /* last 2 bytes must be 0*/

static const uint16_t DefaultFileName[] = {'N', 'e', 'w', ' ', 'F', 'o', 'l', 'd', 'e', 'r', 0};

static const uint16_t DevicePropDefVal[] = {'S', 'T', 'M', '3', '2', 0}; /* last 2 bytes must be 0*/
static const uint16_t DevicePropCurDefVal[] = {'F', 'a', 'l', 'c', 'o', 'n', 0};


#ifndef    MAX_FILE_NAME
#define    MAX_FILE_NAME                                        255U
#endif    /* MAX_FILE_NAME */

#ifndef    MAX_OBJECT_HANDLE_LEN
#define    MAX_OBJECT_HANDLE_LEN                                100U
#endif    /* MAX_OBJECT_HANDLE_LEN */

#ifndef    DEVICE_PROP_DESC_DEF_LEN
#define    DEVICE_PROP_DESC_DEF_LEN                            (uint8_t)(sizeof(DevicePropDefVal) / 2U)
#endif    /* DEVICE_PROP_DESC_DEF_LEN */

#ifndef   DEVICE_PROP_DESC_CUR_LEN
#define   DEVICE_PROP_DESC_CUR_LEN                             (uint8_t)(sizeof(DevicePropCurDefVal) / 2U)
#endif   /* DEVICE_PROP_DESC_CUR_LEN */

#ifndef   DEFAULT_FILE_NAME_LEN
#define   DEFAULT_FILE_NAME_LEN                                (uint8_t)(sizeof(DefaultFileName) / 2U)
#endif   /* DEFAULT_FILE_NAME_LEN */

/**
  * @}
  */


/** @defgroup USBD_MTP_OPT_Exported_TypesDefinitions
  * @{
  */

/**
  * @}
  */

static const uint16_t SuppOP[] = { MTP_OP_GET_DEVICE_INFO, MTP_OP_OPEN_SESSION, MTP_OP_CLOSE_SESSION,
                                   MTP_OP_GET_STORAGE_IDS, MTP_OP_GET_STORAGE_INFO, MTP_OP_GET_NUM_OBJECTS,
                                   MTP_OP_GET_OBJECT_HANDLES, MTP_OP_GET_OBJECT_INFO, MTP_OP_GET_OBJECT,
                                   MTP_OP_DELETE_OBJECT, MTP_OP_SEND_OBJECT_INFO, MTP_OP_SEND_OBJECT,
                                   MTP_OP_GET_DEVICE_PROP_DESC, MTP_OP_GET_DEVICE_PROP_VALUE,
                                   MTP_OP_SET_OBJECT_PROP_VALUE, MTP_OP_GET_OBJECT_PROP_VALUE,
                                   MTP_OP_GET_OBJECT_PROPS_SUPPORTED, MTP_OP_GET_OBJECT_PROPLIST,
                                   MTP_OP_GET_OBJECT_PROP_DESC, MTP_OP_GET_OBJECT_PROP_REFERENCES
                                 };

static const uint16_t SuppEvents[] = {MTP_EVENT_OBJECTADDED};
static const uint16_t SuppImgFormat[] = {MTP_OBJ_FORMAT_UNDEFINED, MTP_OBJ_FORMAT_TEXT, MTP_OBJ_FORMAT_ASSOCIATION,
                                         MTP_OBJ_FORMAT_EXECUTABLE, MTP_OBJ_FORMAT_WAV, MTP_OBJ_FORMAT_MP3,
                                         MTP_OBJ_FORMAT_EXIF_JPEG, MTP_OBJ_FORMAT_MPEG, MTP_OBJ_FORMAT_MP4_CONTAINER,
                                         MTP_OBJ_FORMAT_WINDOWS_IMAGE_FORMAT, MTP_OBJ_FORMAT_PNG, MTP_OBJ_FORMAT_WMA,
                                         MTP_OBJ_FORMAT_WMV
                                        };

static const uint16_t SuppCaptFormat[] = {MTP_OBJ_FORMAT_UNDEFINED, MTP_OBJ_FORMAT_ASSOCIATION, MTP_OBJ_FORMAT_TEXT};

/* required for all object format : storageID, objectFormat, ObjectCompressedSize,
persistent unique object identifier, name*/
static const uint16_t ObjectPropSupp[] = {MTP_OB_PROP_STORAGE_ID, MTP_OB_PROP_OBJECT_FORMAT, MTP_OB_PROP_OBJECT_SIZE,
                                          MTP_OB_PROP_OBJ_FILE_NAME, MTP_OB_PROP_PARENT_OBJECT, MTP_OB_PROP_NAME,
                                          MTP_OB_PROP_PERS_UNIQ_OBJ_IDEN, MTP_OB_PROP_PROTECTION_STATUS
                                         };

static const uint16_t DevicePropSupp[] = {MTP_DEV_PROP_DEVICE_FRIENDLY_NAME, MTP_DEV_PROP_BATTERY_LEVEL};

/* for all mtp struct */
/* for all mtp struct */
typedef struct
{
  uint32_t StorageIDS_len;
  uint32_t StorageIDS[MTP_NBR_STORAGE_ID];
} MTP_StorageIDSTypeDef;

#if defined ( __GNUC__ )
typedef __PACKED_STRUCT
#else
__packed typedef struct
#endif /* __GNUC__ */
{
  uint8_t FileName_len;
  uint16_t FileName[MAX_FILE_NAME];
} MTP_FileNameTypeDef;


typedef struct
{
  uint32_t ObjectHandle_len;
  uint32_t ObjectHandle[MAX_OBJECT_HANDLE_LEN];
} MTP_ObjectHandleTypeDef;

#if defined ( __GNUC__ )
typedef __PACKED_STRUCT
#else
__packed typedef struct
#endif /* __GNUC__ */
{
  uint32_t ObjectPropSupp_len;
  uint16_t ObjectPropSupp[SUPP_OBJ_PROP_LEN];
} MTP_ObjectPropSuppTypeDef;


#if defined ( __GNUC__ )
typedef __PACKED_STRUCT
#else
__packed typedef struct
#endif /* __GNUC__ */
{
  uint16_t StorageType;
  uint16_t FilesystemType;
  uint16_t AccessCapability;
  uint64_t MaxCapability;
  uint64_t FreeSpaceInBytes;
  uint32_t FreeSpaceInObjects;
  uint8_t  StorageDescription;
  uint8_t  VolumeLabel;
} MTP_StorageInfoTypedef;

typedef union
{
  uint16_t  str[100];
  uint8_t u8;
  int8_t i8;
  uint16_t u16;
  int16_t i16;
  uint32_t u32;
  int32_t i32;
  uint64_t u64;
  int64_t i64;
} MTP_PropertyValueTypedef;

#if defined ( __GNUC__ )
typedef __PACKED_STRUCT
#else
__packed typedef struct
#endif /* __GNUC__ */
{
  uint16_t    ObjectPropertyCode;
  uint16_t    DataType;
  uint8_t     GetSet;
  uint8_t    *DefValue;
  uint32_t    GroupCode;
  uint8_t     FormFlag;
} MTP_ObjectPropDescTypeDef;


#if defined ( __GNUC__ )
typedef __PACKED_STRUCT
#else
__packed typedef struct
#endif /* __GNUC__ */
{
  uint32_t   ObjectHandle;
  uint16_t   PropertyCode;
  uint16_t   Datatype;
  uint8_t   *propval;
} MTP_PropertiesTypedef;

#if defined ( __GNUC__ )
typedef __PACKED_STRUCT
#else
__packed typedef struct
#endif /* __GNUC__ */
{
  uint32_t MTP_Properties_len;
  MTP_PropertiesTypedef MTP_Properties[SUPP_OBJ_PROP_LEN];
} MTP_PropertiesListTypedef;


#if defined ( __GNUC__ )
typedef __PACKED_STRUCT
#else
__packed typedef struct
#endif /* __GNUC__ */
{
  uint32_t ref_len;
  uint32_t ref[1];
} MTP_RefTypeDef;

#if defined ( __GNUC__ )
typedef __PACKED_STRUCT
#else
__packed typedef struct
#endif /* __GNUC__ */
{
  uint16_t     DevicePropertyCode;
  uint16_t     DataType;
  uint8_t      GetSet;
  uint8_t      DefValue_len;
  uint16_t     DefValue[DEVICE_PROP_DESC_DEF_LEN];
  uint8_t      curDefValue_len;
  uint16_t     curDefValue[DEVICE_PROP_DESC_CUR_LEN];
  uint8_t      FormFlag;
} MTP_DevicePropDescTypeDef;

/* MTP device info structure */
#if defined ( __GNUC__ )
typedef __PACKED_STRUCT
#else
__packed typedef struct
#endif /* __GNUC__ */
{
  uint16_t       StandardVersion;
  uint32_t       VendorExtensionID;
  uint16_t       VendorExtensionVersion;
  uint8_t        VendorExtensionDesc_len;
#if USBD_MTP_VEND_EXT_DESC_SUPPORTED == 1
  uint16_t       VendorExtensionDesc[VEND_EXT_DESC_LEN];
#endif /* USBD_MTP_VEND_EXT_DESC_SUPPORTED */
  uint16_t       FunctionalMode;
  uint32_t       OperationsSupported_len;
  uint16_t       OperationsSupported[SUPP_OP_LEN];
  uint32_t       EventsSupported_len;
#if USBD_MTP_EVENTS_SUPPORTED == 1
  uint16_t       EventsSupported[SUPP_EVENTS_LEN];
#endif /* USBD_MTP_EVENTS_SUPPORTED */
  uint32_t       DevicePropertiesSupported_len;
#if USBD_MTP_DEVICE_PROP_SUPPORTED == 1
  uint16_t       DevicePropertiesSupported[SUPP_DEVICE_PROP_LEN];
#endif /* USBD_MTP_DEVICE_PROP_SUPPORTED */
  uint32_t       CaptureFormats_len;
#if USBD_MTP_CAPTURE_FORMAT_SUPPORTED == 1
  uint16_t       CaptureFormats[SUPP_CAPT_FORMAT_LEN];
#endif /* USBD_MTP_CAPTURE_FORMAT_SUPPORTED */
  uint32_t       ImageFormats_len;
  uint16_t       ImageFormats[SUPP_IMG_FORMAT_LEN];
  uint8_t        Manufacturer_len;
  uint16_t       Manufacturer[MANUF_LEN];
  uint8_t        Model_len;
  uint16_t       Model[MODEL_LEN];
  uint8_t        DeviceVersion_len;
  uint16_t       DeviceVersion[DEVICE_VERSION_LEN];
  uint8_t        SerialNumber_len;
  uint16_t       SerialNumber[SERIAL_NBR_LEN];
} MTP_DeviceInfoTypedef;

/** @defgroup USBD_MTP_OPT_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_MTP_OPT_Exported_Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_MTP_OPT_Exported_Functions
  * @{
  */

void USBD_MTP_OPT_CreateObjectHandle(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_GetDeviceInfo(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_GetStorageIDS(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_GetStorageInfo(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_GetObjectHandle(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_GetObjectInfo(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_GetObjectReferences(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_GetObjectPropSupp(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_GetObjectPropDesc(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_GetObjectPropValue(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_GetObjectPropList(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_GetDevicePropDesc(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_SendObjectInfo(USBD_HandleTypeDef  *pdev, uint8_t *buff, uint32_t len);
void USBD_MTP_OPT_SendObject(USBD_HandleTypeDef  *pdev, uint8_t *buff, uint32_t len);
void USBD_MTP_OPT_GetObject(USBD_HandleTypeDef  *pdev);
void USBD_MTP_OPT_DeleteObject(USBD_HandleTypeDef  *pdev);



static uint8_t  __ALIGN_BEGIN ObjInfo_buff[255] __ALIGN_END ;
static uint32_t objhandle;
static uint16_t obj_format;
static uint32_t storage_id;

static __ALIGN_BEGIN MTP_DeviceInfoTypedef     MTP_DeviceInfo __ALIGN_END ;
static __ALIGN_BEGIN MTP_StorageIDSTypeDef     MTP_StorageIDS __ALIGN_END ;
static __ALIGN_BEGIN MTP_StorageInfoTypedef    MTP_StorageInfo __ALIGN_END ;
static __ALIGN_BEGIN MTP_ObjectHandleTypeDef   MTP_ObjectHandle __ALIGN_END ;
static __ALIGN_BEGIN MTP_ObjectInfoTypeDef     MTP_ObjectInfo __ALIGN_END ;
static __ALIGN_BEGIN MTP_ObjectPropSuppTypeDef MTP_ObjectPropSupp __ALIGN_END ;
static __ALIGN_BEGIN MTP_ObjectPropDescTypeDef MTP_ObjectPropDesc __ALIGN_END ;
static __ALIGN_BEGIN MTP_PropertiesListTypedef MTP_PropertiesList __ALIGN_END ;
static __ALIGN_BEGIN MTP_RefTypeDef            MTP_Ref __ALIGN_END ;
static __ALIGN_BEGIN MTP_PropertyValueTypedef  MTP_PropertyValue __ALIGN_END ;
static __ALIGN_BEGIN MTP_FileNameTypeDef       MTP_FileName __ALIGN_END ;
static __ALIGN_BEGIN MTP_DevicePropDescTypeDef MTP_DevicePropDesc __ALIGN_END ;

/* Private function prototypes -----------------------------------------------*/
static void MTP_Get_DeviceInfo(void);
static void MTP_Get_StorageIDS(void);
static void MTP_Get_PayloadContent(USBD_HandleTypeDef *pdev);
static void MTP_Get_ObjectInfo(USBD_HandleTypeDef *pdev);
static void MTP_Get_StorageInfo(USBD_HandleTypeDef *pdev);
static void MTP_Get_ObjectHandle(USBD_HandleTypeDef *pdev);
static void MTP_Get_ObjectPropSupp(void);
static void MTP_Get_ObjectPropDesc(USBD_HandleTypeDef *pdev);
static void MTP_Get_ObjectPropList(USBD_HandleTypeDef *pdev);
static void MTP_Get_DevicePropDesc(void);
static uint8_t *MTP_Get_ObjectPropValue(USBD_HandleTypeDef *pdev);
static uint32_t MTP_build_data_propdesc(USBD_HandleTypeDef *pdev, MTP_ObjectPropDescTypeDef def);
static uint32_t MTP_build_data_ObjInfo(USBD_HandleTypeDef *pdev, MTP_ObjectInfoTypeDef objinfo);
static uint32_t MTP_build_data_proplist(USBD_HandleTypeDef *pdev,
                                        MTP_PropertiesListTypedef proplist, uint32_t idx);

/* Private functions ---------------------------------------------------------*/

/**
  * @}
  */


/**
  * @brief  USBD_MTP_OPT_CreateObjectHandle
  *         Open a new session
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_CreateObjectHandle(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;

  if (hmtp->OperationsContainer.Param1 == 0U)   /* Param1 == Session ID*/
  {
    hmtp->ResponseCode = MTP_RESPONSE_INVALID_PARAMETER;
  }
  /* driver supports single session */
  else if (hmtp->MTP_SessionState == MTP_SESSION_OPENED)
  {
    hmtp->ResponseCode = MTP_RESPONSE_SESSION_ALREADY_OPEN;
  }
  else
  {
    hmtp->ResponseCode =  MTP_RESPONSE_OK;
    hmtp->MTP_SessionState = MTP_SESSION_OPENED;
  }

  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_RESPONSE;
  hmtp->ResponseLength = MTP_CONT_HEADER_SIZE;
  hmtp->GenericContainer.length =  hmtp->ResponseLength;
}

/**
  * @brief  USBD_MTP_OPT_GetDeviceInfo
  *         Get all device information
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_GetDeviceInfo(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;

  if (hmtp->MTP_SessionState == MTP_SESSION_NOT_OPENED)        /* no session opened */
  {
    /* if GetDevice Info called outside a session then SessionID and Transaction_ID shall be 0x00000000*/
    /* Param1 == session ID*/
    if ((hmtp->OperationsContainer.Param1 == 0U) && (hmtp->OperationsContainer.trans_id == 0U))
    {
      hmtp->ResponseCode = MTP_RESPONSE_OK;
    }
    else
    {
      hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
      hmtp->GenericContainer.type = MTP_CONT_TYPE_RESPONSE;
      hmtp->ResponseCode = MTP_RESPONSE_INVALID_PARAMETER;
      hmtp->GenericContainer.length =  MTP_CONT_HEADER_SIZE;
    }
  }
  else
  {
    hmtp->ResponseCode = MTP_RESPONSE_OK;
  }

  if (hmtp->ResponseCode == MTP_RESPONSE_OK)
  {
    hmtp->GenericContainer.code =  MTP_OP_GET_DEVICE_INFO;
    hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
    hmtp->GenericContainer.type = MTP_CONT_TYPE_DATA;

    (void)MTP_Get_PayloadContent(pdev);

    hmtp->ResponseLength = sizeof(MTP_DeviceInfo) + MTP_CONT_HEADER_SIZE;
    hmtp->GenericContainer.length =  hmtp->ResponseLength;
  }
}

/**
  * @brief  USBD_MTP_OPT_GetStorageIDS
  *         Get Storage IDs
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_GetStorageIDS(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  hmtp->GenericContainer.code =  MTP_OP_GET_STORAGE_IDS;
  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_DATA;

  (void)MTP_Get_PayloadContent(pdev);

  hmtp->ResponseLength = sizeof(MTP_StorageIDS) + MTP_CONT_HEADER_SIZE;
  hmtp->GenericContainer.length =  hmtp->ResponseLength;

  hmtp->ResponseCode = MTP_RESPONSE_OK;
}

/**
  * @brief  USBD_MTP_OPT_GetStorageInfo
  *         Get Storage information
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_GetStorageInfo(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  hmtp->GenericContainer.code =  MTP_OP_GET_STORAGE_INFO;
  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_DATA;

  (void)MTP_Get_PayloadContent(pdev);

  hmtp->ResponseLength = sizeof(MTP_StorageInfo) + MTP_CONT_HEADER_SIZE;
  hmtp->GenericContainer.length =  hmtp->ResponseLength;

  hmtp->ResponseCode = MTP_RESPONSE_OK;
}

/**
  * @brief  USBD_MTP_OPT_GetObjectHandle
  *         Get all object handles
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_GetObjectHandle(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  hmtp->GenericContainer.code =  MTP_OP_GET_OBJECT_HANDLES;
  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_DATA;

  (void)MTP_Get_PayloadContent(pdev);

  hmtp->ResponseLength = hmtp->ResponseLength + MTP_CONT_HEADER_SIZE;
  hmtp->GenericContainer.length =  hmtp->ResponseLength;

  hmtp->ResponseCode = MTP_RESPONSE_OK;
}

/**
  * @brief  USBD_MTP_OPT_GetObjectInfo
  *         Get all information about the object
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_GetObjectInfo(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;

  hmtp->GenericContainer.code =  MTP_OP_GET_OBJECT_INFO;
  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_DATA;

  (void)MTP_Get_PayloadContent(pdev);

  hmtp->ResponseLength = hmtp->ResponseLength + MTP_CONT_HEADER_SIZE;
  hmtp->GenericContainer.length =  hmtp->ResponseLength;

  hmtp->ResponseCode = MTP_RESPONSE_OK;
}

/**
  * @brief  USBD_MTP_OPT_GetObjectReferences
  *         Get object references
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_GetObjectReferences(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;

  hmtp->GenericContainer.code = MTP_OP_GET_OBJECT_PROP_REFERENCES;
  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_DATA;

  (void)MTP_Get_PayloadContent(pdev);

  hmtp->ResponseLength = sizeof(MTP_Ref) + MTP_CONT_HEADER_SIZE;
  hmtp->GenericContainer.length = hmtp->ResponseLength;

  hmtp->ResponseCode = MTP_RESPONSE_OK;
}

/**
  * @brief  USBD_MTP_OPT_GetObjectPropSupp
  *         Get all object properties supported
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_GetObjectPropSupp(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;

  hmtp->GenericContainer.code = MTP_OP_GET_OBJECT_PROPS_SUPPORTED;
  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_DATA;

  (void)MTP_Get_PayloadContent(pdev);

  hmtp->ResponseLength = sizeof(MTP_ObjectPropSupp) + MTP_CONT_HEADER_SIZE;
  hmtp->GenericContainer.length =  hmtp->ResponseLength;

  hmtp->ResponseCode = MTP_RESPONSE_OK;
}

/**
  * @brief  USBD_MTP_OPT_GetObjectPropDesc
  *         Get all descriptions about object properties
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_GetObjectPropDesc(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;

  hmtp->GenericContainer.code = MTP_OP_GET_OBJECT_PROP_DESC;
  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_DATA;

  (void)MTP_Get_PayloadContent(pdev);

  hmtp->ResponseLength = hmtp->ResponseLength + MTP_CONT_HEADER_SIZE;
  hmtp->GenericContainer.length =  hmtp->ResponseLength;

  hmtp->ResponseCode = MTP_RESPONSE_OK;
}

/**
  * @brief  USBD_MTP_OPT_GetObjectPropList
  *         Get the list of object properties
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_GetObjectPropList(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;

  hmtp->GenericContainer.code = MTP_OP_GET_OBJECT_PROPLIST;
  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_DATA;

  (void)MTP_Get_PayloadContent(pdev);

  hmtp->ResponseLength = hmtp->ResponseLength + MTP_CONT_HEADER_SIZE;
  hmtp->GenericContainer.length =  hmtp->ResponseLength;

  hmtp->ResponseCode = MTP_RESPONSE_OK;
}

/**
  * @brief  USBD_MTP_OPT_GetObjectPropValue
  *         Get current value of the object property
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_GetObjectPropValue(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;

  hmtp->GenericContainer.code = MTP_OP_GET_OBJECT_PROP_VALUE;
  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_DATA;

  (void)MTP_Get_PayloadContent(pdev);

  hmtp->ResponseLength = hmtp->ResponseLength + MTP_CONT_HEADER_SIZE;
  hmtp->GenericContainer.length =  hmtp->ResponseLength;

  hmtp->ResponseCode = MTP_RESPONSE_OK;
}

/**
  * @brief  USBD_MTP_OPT_GetObject
  *         Get binary data from an object
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_GetObject(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  USBD_MTP_ItfTypeDef     * const hmtpif = & mtpIfHandle;

  hmtp->GenericContainer.length = hmtpif->GetContainerLength(hmtp->OperationsContainer.Param1);
  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_DATA;

  hmtp->ResponseCode = MTP_RESPONSE_OK;
}

/**
  * @brief  USBD_MTP_OPT_GetDevicePropDesc
  *         Get The DevicePropDesc dataset
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_GetDevicePropDesc(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;

  hmtp->GenericContainer.code = MTP_OP_GET_DEVICE_PROP_DESC;
  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_DATA;

  (void)MTP_Get_PayloadContent(pdev);

  hmtp->ResponseLength = sizeof(MTP_DevicePropDesc) + MTP_CONT_HEADER_SIZE;
  hmtp->GenericContainer.length =  hmtp->ResponseLength;

  hmtp->ResponseCode = MTP_RESPONSE_OK;
}

/**
  * @brief  USBD_MTP_OPT_SendObject
  *         Send object from host to MTP device
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_SendObject(USBD_HandleTypeDef  *pdev, uint8_t *buff, uint32_t len)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  USBD_MTP_ItfTypeDef     * const hmtpif = & mtpIfHandle;
  static uint32_t tmp = 0U;

  switch (hmtp->RECEIVE_DATA_STATUS)
  {
    case RECEIVE_IDLE_STATE:
      hmtp->RECEIVE_DATA_STATUS = RECEIVE_COMMAND_DATA;
      break;
    case RECEIVE_COMMAND_DATA:
      hmtp->RECEIVE_DATA_STATUS = RECEIVE_FIRST_DATA;
      break;
    case RECEIVE_FIRST_DATA:
      if ((uint16_t)len < (hmtp->MaxPcktLen - MTP_CONT_HEADER_SIZE))
      {
        hmtp->GenericContainer.code =  MTP_RESPONSE_OK;
        hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
        hmtp->GenericContainer.type = MTP_CONT_TYPE_RESPONSE;
        hmtp->ResponseLength = MTP_CONT_HEADER_SIZE;
        hmtp->GenericContainer.length =  hmtp->ResponseLength;

        hmtp->RECEIVE_DATA_STATUS = RECEIVE_IDLE_STATE;
      }
      else
      {
        hmtp->RECEIVE_DATA_STATUS = RECEIVE_REST_OF_DATA;
      }
      tmp = (uint32_t)buff;
      hmtpif->WriteData(len, (uint8_t *)(tmp + 12U));
      break;

    case RECEIVE_REST_OF_DATA:
      hmtpif->WriteData(len, buff);
      break;

    case SEND_RESPONSE:
      hmtpif->WriteData(0, buff); /* send 0 length to stop write process */
      hmtp->GenericContainer.code =  MTP_RESPONSE_OK;
      hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
      hmtp->GenericContainer.type = MTP_CONT_TYPE_RESPONSE;
      hmtp->ResponseLength = MTP_CONT_HEADER_SIZE;
      hmtp->GenericContainer.length =  hmtp->ResponseLength;

      hmtp->RECEIVE_DATA_STATUS = RECEIVE_IDLE_STATE;
      break;

    default:
      break;
  }

  hmtp->ResponseCode = MTP_RESPONSE_OK;
}

/**
  * @brief  USBD_MTP_OPT_SendObjectInfo
  *         Send the object information from host to MTP device
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_SendObjectInfo(USBD_HandleTypeDef  *pdev, uint8_t *buff, uint32_t len)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  USBD_MTP_ItfTypeDef     * const hmtpif = & mtpIfHandle;
  MTP_ObjectInfoTypeDef ObjectInfo;
  uint8_t dataLength = offsetof(MTP_ObjectInfoTypeDef, Filename);
  uint8_t *tmp;

  switch (hmtp->RECEIVE_DATA_STATUS)
  {
    case RECEIVE_IDLE_STATE:
      hmtp->RECEIVE_DATA_STATUS = RECEIVE_COMMAND_DATA;
      break;

    case RECEIVE_COMMAND_DATA:
      /* store object handle and storage id for future use */
      if (hmtp->OperationsContainer.Param2  == 0xFFFFFFFFU)
      {
        objhandle = 0U;
      }
      else
      {
        objhandle = hmtp->OperationsContainer.Param2;
      }
      storage_id = hmtp->OperationsContainer.Param1;
      hmtp->RECEIVE_DATA_STATUS = RECEIVE_FIRST_DATA;
      break;

    case RECEIVE_FIRST_DATA:
      tmp = buff;

      (void)USBD_memcpy(ObjInfo_buff, tmp + 12U,
                        (uint16_t)(hmtp->MaxPcktLen - MTP_CONT_HEADER_SIZE));

      hmtp->RECEIVE_DATA_STATUS = RECEIVE_REST_OF_DATA;
      break;

    case RECEIVE_REST_OF_DATA:

      (void)USBD_memcpy(ObjInfo_buff + len, buff, hmtp->MaxPcktLen);

      break;

    case SEND_RESPONSE:
      (void)USBD_memcpy((uint8_t *)&ObjectInfo, ObjInfo_buff, dataLength);
      (void)USBD_memcpy((uint8_t *)&ObjectInfo.Filename, (ObjInfo_buff + dataLength),
                        ((uint32_t)(ObjectInfo.Filename_len) * 2U));

      obj_format = ObjectInfo.ObjectFormat;

      hmtp->ResponseCode = hmtpif->Create_NewObject(ObjectInfo, objhandle);
      hmtp->GenericContainer.code = (uint16_t)hmtp->ResponseCode;
      hmtp->ResponseLength = MTP_CONT_HEADER_SIZE + (sizeof(uint32_t) * 3U); /* Header + 3 Param */
      hmtp->GenericContainer.length =  hmtp->ResponseLength;
      hmtp->GenericContainer.type = MTP_CONT_TYPE_RESPONSE;
      hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;

      (void)MTP_Get_PayloadContent(pdev);

      hmtp->RECEIVE_DATA_STATUS = RECEIVE_IDLE_STATE;
      break;

    default:
      break;
  }
}

/**
  * @brief  USBD_MTP_OPT_DeleteObject
  *         Delete the object from the device
  * @param  pdev: device instance
  * @retval None
  */
void USBD_MTP_OPT_DeleteObject(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  USBD_MTP_ItfTypeDef     * const hmtpif = & mtpIfHandle;

  hmtp->GenericContainer.trans_id = hmtp->OperationsContainer.trans_id;
  hmtp->GenericContainer.type = MTP_CONT_TYPE_RESPONSE;
  hmtp->ResponseLength = MTP_CONT_HEADER_SIZE;
  hmtp->GenericContainer.length =  hmtp->ResponseLength;
  hmtp->ResponseCode = hmtpif->DeleteObject(hmtp->OperationsContainer.Param1);
}

/**
  * @brief  MTP_Get_PayloadContent
  *         Get the payload data of generic container
  * @param  pdev: device instance
  * @retval None
  */
static void MTP_Get_PayloadContent(USBD_HandleTypeDef *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  USBD_MTP_ItfTypeDef     * const hmtpif = & mtpIfHandle;
  uint8_t *buffer = hmtp->GenericContainer.data;
  uint32_t i;
  uint32_t n_idx;

  switch (hmtp->OperationsContainer.code)
  {
    case MTP_OP_GET_DEVICE_INFO:
      (void)MTP_Get_DeviceInfo();
      (void)USBD_memcpy(buffer, (const uint8_t *)&MTP_DeviceInfo, sizeof(MTP_DeviceInfo));

      for (i = 0U; i < sizeof(MTP_StorageIDS); i++)
      {
        hmtp->GenericContainer.data[i] = buffer[i];
      }
      break;

    case MTP_OP_GET_STORAGE_IDS:
      (void)MTP_Get_StorageIDS();
      (void)USBD_memcpy(buffer, (const uint8_t *)&MTP_StorageIDS, sizeof(MTP_StorageIDS));

      for (i = 0U; i < sizeof(MTP_StorageIDS); i++)
      {
        hmtp->GenericContainer.data[i] = buffer[i];
      }
      break;

    case MTP_OP_GET_STORAGE_INFO:
      (void)MTP_Get_StorageInfo(pdev);
      (void)USBD_memcpy(buffer, (const uint8_t *)&MTP_StorageInfo, sizeof(MTP_StorageInfo));

      for (i = 0U; i < sizeof(MTP_StorageInfo); i++)
      {
        hmtp->GenericContainer.data[i] = buffer[i];
      }
      break;

    case MTP_OP_GET_OBJECT_HANDLES:
      (void)MTP_Get_ObjectHandle(pdev);
      (void)USBD_memcpy(buffer, (const uint8_t *)&MTP_ObjectHandle, hmtp->ResponseLength);

      for (i = 0U; i < hmtp->ResponseLength; i++)
      {
        hmtp->GenericContainer.data[i] = buffer[i];
      }
      break;

    case MTP_OP_GET_OBJECT_INFO:
      (void)MTP_Get_ObjectInfo(pdev);
      break;

    case MTP_OP_GET_OBJECT_PROPS_SUPPORTED:
      (void)MTP_Get_ObjectPropSupp();
      (void)USBD_memcpy(buffer, (const uint8_t *)&MTP_ObjectPropSupp, sizeof(MTP_ObjectPropSupp));

      for (i = 0U; i < sizeof(MTP_ObjectPropSupp); i++)
      {
        hmtp->GenericContainer.data[i] = buffer[i];
      }
      break;

    case MTP_OP_GET_OBJECT_PROP_DESC:
      (void)MTP_Get_ObjectPropDesc(pdev);
      hmtp->ResponseLength = MTP_build_data_propdesc(pdev, MTP_ObjectPropDesc);
      break;

    case MTP_OP_GET_OBJECT_PROP_REFERENCES:
      MTP_Ref.ref_len = 0U;
      (void)USBD_memcpy(buffer, (const uint8_t *)&MTP_Ref.ref_len, sizeof(MTP_Ref.ref_len));

      for (i = 0U; i < sizeof(MTP_Ref.ref_len); i++)
      {
        hmtp->GenericContainer.data[i] = buffer[i];
      }
      break;

    case MTP_OP_GET_OBJECT_PROPLIST:
      (void)MTP_Get_ObjectPropList(pdev);
      break;

    case MTP_OP_GET_OBJECT_PROP_VALUE:
      buffer = MTP_Get_ObjectPropValue(pdev);
      for (i = 0U; i < hmtp->ResponseLength; i++)
      {
        hmtp->GenericContainer.data[i] = buffer[i];
      }

      break;

    case MTP_OP_GET_DEVICE_PROP_DESC:
      (void)MTP_Get_DevicePropDesc();
      (void)USBD_memcpy(buffer, (const uint8_t *)&MTP_DevicePropDesc, sizeof(MTP_DevicePropDesc));
      for (i = 0U; i < sizeof(MTP_DevicePropDesc); i++)
      {
        hmtp->GenericContainer.data[i] = buffer[i];
      }
      break;

    case MTP_OP_SEND_OBJECT_INFO:
      n_idx = hmtpif->GetNewIndex(obj_format);
      (void)USBD_memcpy(hmtp->GenericContainer.data, (const uint8_t *)&storage_id, sizeof(uint32_t));
      (void)USBD_memcpy(hmtp->GenericContainer.data + 4U, (const uint8_t *)&objhandle, sizeof(uint32_t));
      (void)USBD_memcpy(hmtp->GenericContainer.data + 8U, (const uint8_t *)&n_idx, sizeof(uint32_t));
      break;

    case MTP_OP_GET_OBJECT:
      break;

    default:
      break;
  }
}

/**
  * @brief  MTP_Get_DeviceInfo
  *         Fill the MTP_DeviceInfo struct
  * @param  pdev: device instance
  * @retval None
  */
static void MTP_Get_DeviceInfo(void)
{
  MTP_DeviceInfo.StandardVersion = STANDARD_VERSION;
  MTP_DeviceInfo.VendorExtensionID = VEND_EXT_ID;
  MTP_DeviceInfo.VendorExtensionVersion = VEND_EXT_VERSION;
  MTP_DeviceInfo.VendorExtensionDesc_len = (uint8_t)VEND_EXT_DESC_LEN;
  uint32_t i;

#if USBD_MTP_VEND_EXT_DESC_SUPPORTED == 1
  for (i = 0U; i < VEND_EXT_DESC_LEN; i++)
  {
    MTP_DeviceInfo.VendorExtensionDesc[i] = VendExtDesc[i];
  }
#endif /* USBD_MTP_VEND_EXT_DESC_SUPPORTED */

  MTP_DeviceInfo.FunctionalMode = FUNCTIONAL_MODE; /* device supports one mode , standard mode */

  /* All supported operation */
  MTP_DeviceInfo.OperationsSupported_len = SUPP_OP_LEN;
  for (i = 0U; i < SUPP_OP_LEN; i++)
  {
    MTP_DeviceInfo.OperationsSupported[i] = SuppOP[i];
  }

  MTP_DeviceInfo.EventsSupported_len = SUPP_EVENTS_LEN; /* event that are currently generated by the device*/

#if USBD_MTP_EVENTS_SUPPORTED == 1
  for (i = 0U; i < SUPP_EVENTS_LEN; i++)
  {
    MTP_DeviceInfo.EventsSupported[i] = SuppEvents[i];
  }
#endif /* USBD_MTP_EVENTS_SUPPORTED */

  MTP_DeviceInfo.DevicePropertiesSupported_len = SUPP_DEVICE_PROP_LEN;

#if USBD_MTP_DEVICE_PROP_SUPPORTED == 1
  for (i = 0U; i < SUPP_DEVICE_PROP_LEN; i++)
  {
    MTP_DeviceInfo.DevicePropertiesSupported[i] = DevicePropSupp[i];
  }
#endif /* USBD_MTP_DEVICE_PROP_SUPPORTED */

  MTP_DeviceInfo.CaptureFormats_len = SUPP_CAPT_FORMAT_LEN;

#if USBD_MTP_CAPTURE_FORMAT_SUPPORTED == 1
  for (i = 0U; i < SUPP_CAPT_FORMAT_LEN; i++)
  {
    MTP_DeviceInfo.CaptureFormats[i] = SuppCaptFormat[i];
  }
#endif /* USBD_MTP_CAPTURE_FORMAT_SUPPORTED */

  MTP_DeviceInfo.ImageFormats_len = SUPP_IMG_FORMAT_LEN; /* number of image formats that are supported by the device*/
  for (i = 0U; i < SUPP_IMG_FORMAT_LEN; i++)
  {
    MTP_DeviceInfo.ImageFormats[i] = SuppImgFormat[i];
  }

  MTP_DeviceInfo.Manufacturer_len = (uint8_t)MANUF_LEN;
  for (i = 0U; i < MANUF_LEN; i++)
  {
    MTP_DeviceInfo.Manufacturer[i] = Manuf[i];
  }

  MTP_DeviceInfo.Model_len = (uint8_t)MODEL_LEN;
  for (i = 0U; i < MODEL_LEN; i++)
  {
    MTP_DeviceInfo.Model[i] = Model[i];
  }

  MTP_DeviceInfo.DeviceVersion_len = (uint8_t)DEVICE_VERSION_LEN;
  for (i = 0U; i < DEVICE_VERSION_LEN; i++)
  {
    MTP_DeviceInfo.DeviceVersion[i] = DeviceVers[i];
  }

  MTP_DeviceInfo.SerialNumber_len = (uint8_t)SERIAL_NBR_LEN;
  for (i = 0U; i < SERIAL_NBR_LEN; i++)
  {
    MTP_DeviceInfo.SerialNumber[i] = SerialNbr[i];
  }
}

/**
  * @brief  MTP_Get_StorageInfo
  *         Fill the MTP_StorageInfo struct
  * @param  pdev: device instance
  * @retval None
  */
static void MTP_Get_StorageInfo(USBD_HandleTypeDef  *pdev)
{
  USBD_MTP_ItfTypeDef     * const hmtpif = & mtpIfHandle;

  MTP_StorageInfo.StorageType = MTP_STORAGE_REMOVABLE_RAM;
  MTP_StorageInfo.FilesystemType = MTP_FILESYSTEM_GENERIC_FLAT;
  MTP_StorageInfo.AccessCapability = MTP_ACCESS_CAP_RW;
  MTP_StorageInfo.MaxCapability = hmtpif->GetMaxCapability();
  MTP_StorageInfo.FreeSpaceInBytes = hmtpif->GetFreeSpaceInBytes();
  MTP_StorageInfo.FreeSpaceInObjects = FREE_SPACE_IN_OBJ_NOT_USED; /* not used */
  MTP_StorageInfo.StorageDescription = 0U;
  MTP_StorageInfo.VolumeLabel = 0U;
}

/**
  * @brief  MTP_Get_ObjectHandle
  *         Fill the MTP_ObjectHandle struct
  * @param  pdev: device instance
  * @retval None
  */
static void MTP_Get_ObjectHandle(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  USBD_MTP_ItfTypeDef     * const hmtpif = & mtpIfHandle;

  MTP_ObjectHandle.ObjectHandle_len = (uint32_t)(hmtpif->GetIdx(hmtp->OperationsContainer.Param3,
                                                                MTP_ObjectHandle.ObjectHandle));

  hmtp->ResponseLength = (MTP_ObjectHandle.ObjectHandle_len * sizeof(uint32_t)) + sizeof(uint32_t);
}

/**
  * @brief  MTP_Get_ObjectPropSupp
  *         Fill the MTP_ObjectPropSupp struct
  * @param  pdev: device instance
  * @retval None
  */
static void MTP_Get_ObjectPropSupp(void)
{
  uint32_t i;

  MTP_ObjectPropSupp.ObjectPropSupp_len = SUPP_OBJ_PROP_LEN;

  for (i = 0U; i < SUPP_OBJ_PROP_LEN; i++)
  {
    MTP_ObjectPropSupp.ObjectPropSupp[i] = ObjectPropSupp[i];
  }
}

/**
  * @brief  MTP_Get_ObjectPropDesc
  *         Fill the MTP_ObjectPropDesc struct
  * @param  pdev: device instance
  * @retval None
  */
static void MTP_Get_ObjectPropDesc(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  uint16_t undef_format = MTP_OBJ_FORMAT_UNDEFINED;
  uint32_t storageid = MTP_STORAGE_ID;

  switch (hmtp->OperationsContainer.Param1) /* switch obj prop code */
  {
    case MTP_OB_PROP_OBJECT_FORMAT :
      MTP_ObjectPropDesc.ObjectPropertyCode = (uint16_t)(hmtp->OperationsContainer.Param1);
      MTP_ObjectPropDesc.DataType = MTP_DATATYPE_UINT16;
      MTP_ObjectPropDesc.GetSet = MTP_PROP_GET;
      MTP_ObjectPropDesc.DefValue = (uint8_t *)&undef_format;
      MTP_ObjectPropDesc.GroupCode = 0U;
      MTP_ObjectPropDesc.FormFlag = 0U;
      break;

    case MTP_OB_PROP_STORAGE_ID :
      MTP_ObjectPropDesc.ObjectPropertyCode = (uint16_t)(hmtp->OperationsContainer.Param1);
      MTP_ObjectPropDesc.DataType = MTP_DATATYPE_UINT32;
      MTP_ObjectPropDesc.GetSet = MTP_PROP_GET;
      MTP_ObjectPropDesc.DefValue = (uint8_t *)&storageid;
      MTP_ObjectPropDesc.GroupCode = 0U;
      MTP_ObjectPropDesc.FormFlag = 0U;
      break;

    case MTP_OB_PROP_OBJ_FILE_NAME :
      MTP_ObjectPropDesc.ObjectPropertyCode = (uint16_t)(hmtp->OperationsContainer.Param1);
      MTP_ObjectPropDesc.DataType = MTP_DATATYPE_STR;
      MTP_ObjectPropDesc.GetSet = MTP_PROP_GET;
      MTP_FileName.FileName_len = DEFAULT_FILE_NAME_LEN;
      (void)USBD_memcpy((void *) & (MTP_FileName.FileName), (const void *)DefaultFileName, sizeof(DefaultFileName));
      MTP_ObjectPropDesc.DefValue = (uint8_t *)&MTP_FileName;
      MTP_ObjectPropDesc.GroupCode = 0U;
      MTP_ObjectPropDesc.FormFlag = 0U;
      break;

    case MTP_OB_PROP_PARENT_OBJECT :
      MTP_ObjectPropDesc.ObjectPropertyCode = (uint16_t)(hmtp->OperationsContainer.Param1);
      MTP_ObjectPropDesc.DataType = MTP_DATATYPE_STR;
      MTP_ObjectPropDesc.GetSet = MTP_PROP_GET;
      MTP_ObjectPropDesc.DefValue = 0U;
      MTP_ObjectPropDesc.GroupCode = 0U;
      MTP_ObjectPropDesc.FormFlag = 0U;
      break;

    case MTP_OB_PROP_OBJECT_SIZE :
      MTP_ObjectPropDesc.ObjectPropertyCode = (uint16_t)(hmtp->OperationsContainer.Param1);
      MTP_ObjectPropDesc.DataType = MTP_DATATYPE_UINT64;
      MTP_ObjectPropDesc.GetSet = MTP_PROP_GET;
      MTP_ObjectPropDesc.DefValue = 0U;
      MTP_ObjectPropDesc.GroupCode = 0U;
      MTP_ObjectPropDesc.FormFlag = 0U;
      break;

    case MTP_OB_PROP_NAME :
      MTP_ObjectPropDesc.ObjectPropertyCode = (uint16_t)(hmtp->OperationsContainer.Param1);
      MTP_ObjectPropDesc.DataType = MTP_DATATYPE_STR;
      MTP_ObjectPropDesc.GetSet = MTP_PROP_GET;
      MTP_FileName.FileName_len = DEFAULT_FILE_NAME_LEN;
      (void)USBD_memcpy((void *) & (MTP_FileName.FileName),
                        (const void *)DefaultFileName, sizeof(DefaultFileName));

      MTP_ObjectPropDesc.DefValue = (uint8_t *)&MTP_FileName;
      MTP_ObjectPropDesc.GroupCode = 0U;
      MTP_ObjectPropDesc.FormFlag = 0U;
      break;

    case MTP_OB_PROP_PERS_UNIQ_OBJ_IDEN :
      MTP_ObjectPropDesc.ObjectPropertyCode = (uint16_t)(hmtp->OperationsContainer.Param1);
      MTP_ObjectPropDesc.DataType = MTP_DATATYPE_UINT128;
      MTP_ObjectPropDesc.GetSet = MTP_PROP_GET;
      MTP_ObjectPropDesc.DefValue = 0U;
      MTP_ObjectPropDesc.GroupCode = 0U;
      MTP_ObjectPropDesc.FormFlag = 0U;
      break;

    case MTP_OB_PROP_PROTECTION_STATUS :
      MTP_ObjectPropDesc.ObjectPropertyCode = (uint16_t)(hmtp->OperationsContainer.Param1);
      MTP_ObjectPropDesc.DataType = MTP_DATATYPE_UINT16;
      MTP_ObjectPropDesc.GetSet = MTP_PROP_GET_SET;
      MTP_ObjectPropDesc.DefValue = 0U;
      MTP_ObjectPropDesc.GroupCode = 0U;
      MTP_ObjectPropDesc.FormFlag = 0U;
      break;

    default:
      break;
  }
}

/**
  * @brief  MTP_Get_ObjectPropValue
  *         Get the property value
  * @param  pdev: device instance
  * @retval None
  */
static uint8_t *MTP_Get_ObjectPropValue(USBD_HandleTypeDef *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  USBD_MTP_ItfTypeDef     * const hmtpif = & mtpIfHandle;
  static uint8_t buf[512];

  /* Add all other supported object properties */
  switch (hmtp->OperationsContainer.Param2)
  {
    case MTP_OB_PROP_STORAGE_ID:
      MTP_PropertyValue.u32 = MTP_STORAGE_ID;
      (void)USBD_memcpy(buf, (const uint8_t *)&MTP_PropertyValue, sizeof(uint32_t));
      hmtp->ResponseLength = sizeof(uint32_t);
      break;

    case MTP_OB_PROP_OBJECT_FORMAT:
      MTP_PropertyValue.u16 = hmtpif->GetObjectFormat(hmtp->OperationsContainer.Param1);
      (void)USBD_memcpy(buf, (const uint8_t *)&MTP_PropertyValue, sizeof(uint16_t));
      hmtp->ResponseLength = sizeof(uint16_t);
      break;

    case MTP_OB_PROP_OBJ_FILE_NAME:
      MTP_FileName.FileName_len = hmtpif->GetObjectName_len(hmtp->OperationsContainer.Param1);
      hmtpif->GetObjectName(hmtp->OperationsContainer.Param1, MTP_FileName.FileName_len, (uint16_t *)buf);
      (void)USBD_memcpy(MTP_FileName.FileName, (uint16_t *)buf, ((uint32_t)MTP_FileName.FileName_len * 2U) + 1U);

      hmtp->ResponseLength = ((uint32_t)MTP_FileName.FileName_len * 2U) + 1U;
      break;

    case MTP_OB_PROP_PARENT_OBJECT :
      MTP_PropertyValue.u32 = hmtpif->GetParentObject(hmtp->OperationsContainer.Param1);
      (void)USBD_memcpy(buf, (const uint8_t *)&MTP_PropertyValue, sizeof(uint32_t));
      hmtp->ResponseLength = sizeof(uint32_t);
      break;

    case MTP_OB_PROP_OBJECT_SIZE :
      MTP_PropertyValue.u64 = hmtpif->GetObjectSize(hmtp->OperationsContainer.Param1);
      (void)USBD_memcpy(buf, (const uint8_t *)&MTP_PropertyValue, sizeof(uint64_t));
      hmtp->ResponseLength = sizeof(uint64_t);
      break;

    default:
      break;
  }

  return buf;
}

/**
  * @brief  MTP_Get_ObjectPropList
  *         Get the object property list data to be transmitted
  * @param  pdev: device instance
  * @retval None
  */
static void MTP_Get_ObjectPropList(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  USBD_MTP_ItfTypeDef     * const hmtpif = & mtpIfHandle;
  uint16_t filename[255];
  uint32_t storageid = MTP_STORAGE_ID;
  uint32_t default_val = 0U;
  uint32_t i;
  uint16_t format;
  uint64_t objsize;
  uint32_t parent_proval;

  MTP_PropertiesList.MTP_Properties_len = SUPP_OBJ_PROP_LEN;
  hmtp->ResponseLength = 4U;    /* size of MTP_PropertiesList.MTP_Properties_len */
  (void)USBD_memcpy(hmtp->GenericContainer.data,
                    (const uint8_t *)&MTP_PropertiesList.MTP_Properties_len, hmtp->ResponseLength);

  for (i = 0U; i < SUPP_OBJ_PROP_LEN; i++)
  {
    MTP_PropertiesList.MTP_Properties[i].ObjectHandle = hmtp->OperationsContainer.Param1;

    switch (ObjectPropSupp[i])
    {
      case MTP_OB_PROP_STORAGE_ID :
        MTP_PropertiesList.MTP_Properties[i].PropertyCode = MTP_OB_PROP_STORAGE_ID;
        MTP_PropertiesList.MTP_Properties[i].Datatype = MTP_DATATYPE_UINT32;
        MTP_PropertiesList.MTP_Properties[i].propval = (uint8_t *)&storageid;
        break;

      case MTP_OB_PROP_OBJECT_FORMAT :
        MTP_PropertiesList.MTP_Properties[i].PropertyCode = MTP_OB_PROP_OBJECT_FORMAT;
        MTP_PropertiesList.MTP_Properties[i].Datatype = MTP_DATATYPE_UINT16;
        format = hmtpif->GetObjectFormat(hmtp->OperationsContainer.Param1);
        MTP_PropertiesList.MTP_Properties[i].propval = (uint8_t *)&format;
        break;

      case MTP_OB_PROP_OBJ_FILE_NAME:
        MTP_PropertiesList.MTP_Properties[i].PropertyCode = MTP_OB_PROP_OBJ_FILE_NAME;
        MTP_PropertiesList.MTP_Properties[i].Datatype = MTP_DATATYPE_STR;
        /* MTP_FileName.FileName_len value shall be set before USBD_MTP_FS_GetObjectName */
        MTP_FileName.FileName_len = hmtpif->GetObjectName_len(hmtp->OperationsContainer.Param1);
        hmtpif->GetObjectName(hmtp->OperationsContainer.Param1, MTP_FileName.FileName_len, filename);
        (void)USBD_memcpy(MTP_FileName.FileName, filename, ((uint32_t)MTP_FileName.FileName_len * 2U) + 1U);
        MTP_PropertiesList.MTP_Properties[i].propval = (uint8_t *)&MTP_FileName;
        break;

      case MTP_OB_PROP_PARENT_OBJECT :
        MTP_PropertiesList.MTP_Properties[i].PropertyCode = MTP_OB_PROP_PARENT_OBJECT;
        MTP_PropertiesList.MTP_Properties[i].Datatype = MTP_DATATYPE_UINT32;
        parent_proval = hmtpif->GetParentObject(hmtp->OperationsContainer.Param1);
        MTP_PropertiesList.MTP_Properties[i].propval = (uint8_t *)&parent_proval;
        break;

      case MTP_OB_PROP_OBJECT_SIZE :
        MTP_PropertiesList.MTP_Properties[i].PropertyCode = MTP_OB_PROP_OBJECT_SIZE;
        MTP_PropertiesList.MTP_Properties[i].Datatype = MTP_DATATYPE_UINT64;
        objsize = hmtpif->GetObjectSize(hmtp->OperationsContainer.Param1);
        MTP_PropertiesList.MTP_Properties[i].propval = (uint8_t *)&objsize;
        break;

      case MTP_OB_PROP_NAME :
        MTP_PropertiesList.MTP_Properties[i].PropertyCode = MTP_OB_PROP_NAME;
        MTP_PropertiesList.MTP_Properties[i].Datatype = MTP_DATATYPE_STR;
        /* MTP_FileName.FileName_len value shall be set before USBD_MTP_FS_GetObjectName */
        MTP_FileName.FileName_len = hmtpif->GetObjectName_len(hmtp->OperationsContainer.Param1);
        hmtpif->GetObjectName(hmtp->OperationsContainer.Param1, MTP_FileName.FileName_len, filename);
        (void)USBD_memcpy(MTP_FileName.FileName, filename, ((uint32_t)MTP_FileName.FileName_len * 2U) + 1U);
        MTP_PropertiesList.MTP_Properties[i].propval = (uint8_t *)&MTP_FileName;
        break;

      case MTP_OB_PROP_PERS_UNIQ_OBJ_IDEN :
        MTP_PropertiesList.MTP_Properties[i].PropertyCode = MTP_OB_PROP_PERS_UNIQ_OBJ_IDEN;
        MTP_PropertiesList.MTP_Properties[i].Datatype = MTP_DATATYPE_UINT128;
        MTP_PropertiesList.MTP_Properties[i].propval = (uint8_t *)&hmtp->OperationsContainer.Param1;
        break;

      case MTP_OB_PROP_PROTECTION_STATUS :
        MTP_PropertiesList.MTP_Properties[i].PropertyCode = MTP_OB_PROP_PROTECTION_STATUS;
        MTP_PropertiesList.MTP_Properties[i].Datatype = MTP_DATATYPE_UINT16;
        MTP_PropertiesList.MTP_Properties[i].propval = (uint8_t *)&default_val;
        break;

      default:
        break;
    }

    hmtp->ResponseLength = MTP_build_data_proplist(pdev, MTP_PropertiesList, i);
  }
}

/**
  * @brief  MTP_Get_DevicePropDesc
  *         Fill the MTP_DevicePropDesc struct
  * @param  pdev: device instance
  * @retval None
  */
static void MTP_Get_DevicePropDesc(void)
{
  MTP_DevicePropDesc.DevicePropertyCode = MTP_DEV_PROP_DEVICE_FRIENDLY_NAME;
  MTP_DevicePropDesc.DataType = MTP_DATATYPE_STR;
  MTP_DevicePropDesc.GetSet = MTP_PROP_GET_SET;
  MTP_DevicePropDesc.DefValue_len = DEVICE_PROP_DESC_DEF_LEN;
  uint32_t i;

  for (i = 0U; i < (sizeof(DevicePropDefVal) / 2U); i++)
  {
    MTP_DevicePropDesc.DefValue[i] = DevicePropDefVal[i];
  }

  MTP_DevicePropDesc.curDefValue_len = DEVICE_PROP_DESC_CUR_LEN;

  for (i = 0U; i < (sizeof(DevicePropCurDefVal) / 2U); i++)
  {
    MTP_DevicePropDesc.curDefValue[i] = DevicePropCurDefVal[i];
  }

  MTP_DevicePropDesc.FormFlag = 0U;
}

/**
  * @brief  MTP_Get_ObjectInfo
  *         Fill the MTP_ObjectInfo struct
  * @param  pdev: device instance
  * @retval None
  */
static void MTP_Get_ObjectInfo(USBD_HandleTypeDef  *pdev)
{
	USBD_MTP_ItfTypeDef     * const hmtpif = & mtpIfHandle;
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  uint16_t filename[255];

  MTP_ObjectInfo.Storage_id = MTP_STORAGE_ID;
  MTP_ObjectInfo.ObjectFormat = hmtpif->GetObjectFormat(hmtp->OperationsContainer.Param1);
  MTP_ObjectInfo.ObjectCompressedSize = hmtpif->GetObjectSize(hmtp->OperationsContainer.Param1);
  MTP_ObjectInfo.ProtectionStatus = 0U;
  MTP_ObjectInfo.ThumbFormat = MTP_OBJ_FORMAT_UNDEFINED;
  MTP_ObjectInfo.ThumbCompressedSize  = 0U;
  MTP_ObjectInfo.ThumbPixWidth = 0U; /* not supported or not an image */
  MTP_ObjectInfo.ThumbPixHeight = 0U;
  MTP_ObjectInfo.ImagePixWidth = 0U;
  MTP_ObjectInfo.ImagePixHeight = 0U;
  MTP_ObjectInfo.ImageBitDepth = 0U;
  MTP_ObjectInfo.ParentObject = hmtpif->GetParentObject(hmtp->OperationsContainer.Param1);
  MTP_ObjectInfo.AssociationType = 0U;
  MTP_ObjectInfo.AssociationDesc = 0U;
  MTP_ObjectInfo.SequenceNumber = 0U;

  /* we have to get this value before MTP_ObjectInfo.Filename */
  MTP_ObjectInfo.Filename_len = hmtpif->GetObjectName_len(hmtp->OperationsContainer.Param1);
  hmtpif->GetObjectName(hmtp->OperationsContainer.Param1, MTP_ObjectInfo.Filename_len, filename);
  (void)USBD_memcpy(MTP_ObjectInfo.Filename, filename, ((uint32_t)MTP_FileName.FileName_len * 2U) + 1U);

  MTP_ObjectInfo.CaptureDate = 0U;
  MTP_ObjectInfo.ModificationDate = 0U;
  MTP_ObjectInfo.Keywords = 0U;
  hmtp->ResponseLength = MTP_build_data_ObjInfo(pdev, MTP_ObjectInfo);
}

/**
  * @brief  MTP_Get_StorageIDS
  *         Fill the MTP_StorageIDS struct
  * @param  pdev: device instance
  * @retval None
  */
static void MTP_Get_StorageIDS(void)
{
  MTP_StorageIDS.StorageIDS_len = MTP_NBR_STORAGE_ID;
  MTP_StorageIDS.StorageIDS[0] = MTP_STORAGE_ID;
}

/**
  * @brief  MTP_build_data_propdesc
  *         Copy the MTP_ObjectPropDesc dataset to the payload data
  * @param  pdev: device instance
  * @retval None
  */
static uint32_t MTP_build_data_propdesc(USBD_HandleTypeDef  *pdev, MTP_ObjectPropDescTypeDef def)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  uint8_t DefValue_size = (MTP_FileName.FileName_len * 2U) + 1U;
  uint32_t dataLength = offsetof(MTP_ObjectPropDescTypeDef, DefValue);

  (void)USBD_memcpy(hmtp->GenericContainer.data, (const uint8_t *)&def, dataLength);

  switch (def.DataType)
  {
    case MTP_DATATYPE_UINT16:
      (void)USBD_memcpy(hmtp->GenericContainer.data + dataLength, def.DefValue, sizeof(uint16_t));
      dataLength += sizeof(uint16_t);
      break;

    case MTP_DATATYPE_UINT32:
      (void)USBD_memcpy(hmtp->GenericContainer.data + dataLength, def.DefValue,  sizeof(uint32_t));
      dataLength += sizeof(uint32_t);
      break;

    case MTP_DATATYPE_UINT64:
      (void)USBD_memcpy(hmtp->GenericContainer.data + dataLength, def.DefValue,  sizeof(uint64_t));
      dataLength += sizeof(uint64_t);
      break;

    case MTP_DATATYPE_STR:
      (void)USBD_memcpy(hmtp->GenericContainer.data + dataLength, def.DefValue,  DefValue_size);
      dataLength += DefValue_size;
      break;

    case MTP_DATATYPE_UINT128:
      (void)USBD_memcpy(hmtp->GenericContainer.data + dataLength, def.DefValue, (sizeof(uint64_t) * 2U));
      dataLength += (sizeof(uint64_t) * 2U);
      break;

    default:
      break;
  }

  (void)USBD_memcpy(hmtp->GenericContainer.data + dataLength,
                    (const uint8_t *)&MTP_ObjectPropDesc.GroupCode, sizeof(MTP_ObjectPropDesc.GroupCode));

  dataLength += sizeof(MTP_ObjectPropDesc.GroupCode);

  (void)USBD_memcpy(hmtp->GenericContainer.data + dataLength,
                    (const uint8_t *)&MTP_ObjectPropDesc.FormFlag, sizeof(MTP_ObjectPropDesc.FormFlag));

  dataLength += sizeof(MTP_ObjectPropDesc.FormFlag);

  return dataLength;
}

/**
  * @brief  MTP_build_data_proplist
  *         Copy the MTP_PropertiesList dataset to the payload data
  * @param  pdev: device instance
  * @retval None
  */
static uint32_t MTP_build_data_proplist(USBD_HandleTypeDef  *pdev,
                                        MTP_PropertiesListTypedef proplist, uint32_t idx)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  uint8_t propval_size = (MTP_FileName.FileName_len * 2U) + 1U;
  uint32_t dataLength;

  dataLength = offsetof(MTP_PropertiesTypedef, propval);

  (void)USBD_memcpy(hmtp->GenericContainer.data + hmtp->ResponseLength,
                    (const uint8_t *)&proplist.MTP_Properties[idx], dataLength);

  dataLength += hmtp->ResponseLength;

  switch (proplist.MTP_Properties[idx].Datatype)
  {
    case MTP_DATATYPE_UINT16:
      (void)USBD_memcpy(hmtp->GenericContainer.data + dataLength,
                        proplist.MTP_Properties[idx].propval, sizeof(uint16_t));

      dataLength += sizeof(uint16_t);
      break;

    case MTP_DATATYPE_UINT32:
      (void)USBD_memcpy(hmtp->GenericContainer.data + dataLength,
                        proplist.MTP_Properties[idx].propval, sizeof(uint32_t));

      dataLength += sizeof(uint32_t);
      break;

    case MTP_DATATYPE_STR:
      (void)USBD_memcpy(hmtp->GenericContainer.data + dataLength,
                        proplist.MTP_Properties[idx].propval, propval_size);

      dataLength += propval_size;
      break;

    case MTP_DATATYPE_UINT64:
      (void)USBD_memcpy(hmtp->GenericContainer.data + dataLength,
                        proplist.MTP_Properties[idx].propval, sizeof(uint64_t));

      dataLength += sizeof(uint64_t);
      break;

    case MTP_DATATYPE_UINT128:
      (void)USBD_memcpy(hmtp->GenericContainer.data + dataLength,
                        proplist.MTP_Properties[idx].propval, (sizeof(uint64_t) * 2U));

      dataLength += (sizeof(uint64_t) * 2U);
      break;

    default:
      break;
  }

  return dataLength;
}

/**
  * @brief  MTP_build_data_ObjInfo
  *         Copy the MTP_ObjectInfo dataset to the payload data
  * @param  pdev: device instance
  * @retval None
  */
static uint32_t MTP_build_data_ObjInfo(USBD_HandleTypeDef *pdev, MTP_ObjectInfoTypeDef objinfo)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  uint32_t ObjInfo_len = offsetof(MTP_ObjectInfoTypeDef, Filename);

  (void)USBD_memcpy(hmtp->GenericContainer.data, (const uint8_t *)&objinfo, ObjInfo_len);
  (void)USBD_memcpy(hmtp->GenericContainer.data + ObjInfo_len,
                    (const uint8_t *)&objinfo.Filename, objinfo.Filename_len * sizeof(uint16_t));

  ObjInfo_len = ObjInfo_len + (objinfo.Filename_len * sizeof(uint16_t));

  (void)USBD_memcpy(hmtp->GenericContainer.data + ObjInfo_len,
                    (const uint8_t *)&objinfo.CaptureDate, sizeof(objinfo.CaptureDate));

  ObjInfo_len = ObjInfo_len + sizeof(objinfo.CaptureDate);

  return ObjInfo_len;
}

static const uint8_t MTPInEpAdd = USBD_EP_MTP_IN;
static const uint8_t MTPOutEpAdd = USBD_EP_MTP_OUT;
static const uint8_t MTPCmdEpAdd = USBD_EP_MTP_INT;

/* Private variables ---------------------------------------------------------*/
static MTP_DataLengthTypeDef MTP_DataLength;
static MTP_READ_DATA_STATUS ReadDataStatus;

/* Private function prototypes -----------------------------------------------*/
static uint8_t USBD_MTP_STORAGE_DecodeOperations(USBD_HandleTypeDef  *pdev);
static uint8_t USBD_MTP_STORAGE_ReceiveContainer(USBD_HandleTypeDef  *pdev, uint32_t *pDst, uint32_t len);
static uint8_t USBD_MTP_STORAGE_SendData(USBD_HandleTypeDef  *pdev, uint8_t *buf, uint32_t len);

/**
  * @brief  USBD_MTP_STORAGE_Cancel
  *         Reinitialize all states and cancel transfer through Bulk transfer
  * @param  pdev: device instance
  * @param  MTP_ResponsePhase: MTP current state
  * @retval None
  */
void USBD_MTP_STORAGE_Cancel(USBD_HandleTypeDef  *pdev,
                             MTP_ResponsePhaseTypeDef MTP_ResponsePhase)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;

  hmtp->MTP_ResponsePhase = MTP_PHASE_IDLE;
  ReadDataStatus = READ_FIRST_DATA;
  hmtp->RECEIVE_DATA_STATUS = RECEIVE_IDLE_STATE;

//  if (MTP_ResponsePhase == MTP_RECEIVE_DATA)
//  {
//    ((USBD_MTP_ItfTypeDef *)pdev->pUserData[pdev->classId])->Cancel(1U);
//  }
//  else
//  {
//    ((USBD_MTP_ItfTypeDef *)pdev->pUserData[pdev->classId])->Cancel(0U);
//  }
}

/**
  * @brief  USBD_MTP_STORAGE_SendData
  *         Send the data on bulk-in EP
  * @param  pdev: device instance
  * @param  buf: pointer to data buffer
  * @param  len: Data Length
  * @retval status value
  */
static uint8_t USBD_MTP_STORAGE_SendData(USBD_HandleTypeDef  *pdev, uint8_t *buf,
                                         uint32_t len)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  uint32_t length = MIN(hmtp->GenericContainer.length, len);
#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  MTPInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK);
#endif /* USE_USBD_COMPOSITE */

  (void)USBD_LL_Transmit(pdev, MTPInEpAdd, buf, length);

  return (uint8_t)USBD_OK;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @}
  */

/**
  * @brief  USBD_MTP_STORAGE_Init
  *         Initialize the MTP USB Layer
  * @param  pdev: device instance
  * @retval status value
  */
uint8_t USBD_MTP_STORAGE_Init(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  MTPOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK);
#endif /* USE_USBD_COMPOSITE */

  /* Initialize the HW layyer of the file system */
//  (void)((USBD_MTP_ItfTypeDef *)pdev->pUserData[pdev->classId])->Init();

  /* Prepare EP to Receive First Operation */
  (void)USBD_LL_PrepareReceive(pdev, MTPOutEpAdd, (uint8_t *)&hmtp->rx_buff,
                               hmtp->MaxPcktLen);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MTP_STORAGE_DeInit
  *         Uninitialize the MTP Machine
  * @param  pdev: device instance
  * @retval status value
  */
uint8_t USBD_MTP_STORAGE_DeInit(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;

  /* DeInit  physical Interface components */
  hmtp->MTP_SessionState = MTP_SESSION_NOT_OPENED;

  /* Stop low layer file system operations if any */
  USBD_MTP_STORAGE_Cancel(pdev, MTP_PHASE_IDLE);

  /* Free low layer file system resources */
//  (void)((USBD_MTP_ItfTypeDef *)pdev->pUserData[pdev->classId])->DeInit();

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MTP_STORAGE_ReadData
  *         Read data from device objects and send it to the host
  * @param  pdev: device instance
  * @retval status value
  */
uint8_t USBD_MTP_STORAGE_ReadData(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  //uint32_t *data_buff;

  /* Get the data buffer pointer from the low layer interface */
  //data_buff = ((USBD_MTP_ItfTypeDef *)pdev->pUserData[pdev->classId])->ScratchBuff;

  switch (ReadDataStatus)
  {
    case READ_FIRST_DATA:
      /* Reset the data length */
      MTP_DataLength.temp_length = 0U;

      /* Perform the low layer read operation on the scratch buffer */
//      (void)((USBD_MTP_ItfTypeDef *)pdev->pUserData[pdev->classId])->ReadData(hmtp->OperationsContainer.Param1,
//                                                                              (uint8_t *)data_buff, &MTP_DataLength);

      /* Add the container header to the data buffer */
//      (void)USBD_memcpy((uint8_t *)data_buff, (uint8_t *)&hmtp->GenericContainer, MTP_CONT_HEADER_SIZE);

      /* Start USB data transmission to the host */
//      (void)USBD_MTP_STORAGE_SendData(pdev, (uint8_t *)data_buff,
//                                      MTP_DataLength.readbytes + MTP_CONT_HEADER_SIZE);

      /* Check if this will be the last packet to send ? */
      if (MTP_DataLength.readbytes < ((uint32_t)hmtp->MaxPcktLen - MTP_CONT_HEADER_SIZE))
      {
        /* Move to response phase */
        hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      }
      else
      {
        /* Continue to the next packets sending */
        ReadDataStatus = READ_REST_OF_DATA;
      }
      break;

    case READ_REST_OF_DATA:
      /* Perform the low layer read operation on the scratch buffer */
//      (void)((USBD_MTP_ItfTypeDef *)pdev->pUserData[pdev->classId])->ReadData(hmtp->OperationsContainer.Param1,
//                                                                              (uint8_t *)data_buff, &MTP_DataLength);

      /* Check if more data need to be sent */
      if (MTP_DataLength.temp_length == MTP_DataLength.totallen)
      {
        /* Start USB data transmission to the host */
//        (void)USBD_MTP_STORAGE_SendData(pdev, (uint8_t *)data_buff, MTP_DataLength.readbytes);

        /* Move to response phase */
        hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;

        /* Reset the stat machine */
        ReadDataStatus = READ_FIRST_DATA;
      }
      else
      {
        /* Start USB data transmission to the host */
 //       (void)USBD_MTP_STORAGE_SendData(pdev, (uint8_t *)data_buff, MTP_DataLength.readbytes);

        /* Keep the state machine into sending next packet of data */
        ReadDataStatus = READ_REST_OF_DATA;
      }
      break;

    default:
      break;
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MTP_STORAGE_SendContainer
  *         Send generic container to the host
  * @param  pdev: device instance
  * @retval status value
  */
uint8_t USBD_MTP_STORAGE_SendContainer(USBD_HandleTypeDef  *pdev, MTP_CONTAINER_TYPE CONT_TYPE)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  switch (CONT_TYPE)
  {
    case DATA_TYPE:
      /* send header + data : hmtp->ResponseLength = header size + data size */
      (void)USBD_MTP_STORAGE_SendData(pdev, (uint8_t *)&hmtp->GenericContainer, hmtp->ResponseLength);
      break;
    case REP_TYPE:
      /* send header without data */
      hmtp->GenericContainer.code = (uint16_t)hmtp->ResponseCode;
      hmtp->ResponseLength = MTP_CONT_HEADER_SIZE;
      hmtp->GenericContainer.length = hmtp->ResponseLength;
      hmtp->GenericContainer.type = MTP_CONT_TYPE_RESPONSE;

      (void)USBD_MTP_STORAGE_SendData(pdev, (uint8_t *)&hmtp->GenericContainer, hmtp->ResponseLength);
      break;
    default:
      break;
  }
  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MTP_STORAGE_ReceiveOpt
  *         Data length Packet Received from host
  * @param  pdev: device instance
  * @retval status value
  */
uint8_t USBD_MTP_STORAGE_ReceiveOpt(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  uint32_t *pMsgBuffer;
#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  MTPOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK);
#endif /* USE_USBD_COMPOSITE */
  MTP_DataLength.rx_length = USBD_GetRxCount(pdev, MTPOutEpAdd);

  switch (hmtp->RECEIVE_DATA_STATUS)
  {
    case RECEIVE_REST_OF_DATA:
      /* we don't need to do anything here because we receive only data without operation header*/
      break;

    case RECEIVE_FIRST_DATA:
      /* Expected Data Length Packet Received */
      pMsgBuffer = (uint32_t *) &hmtp->OperationsContainer;

      /* Fill hmtp->OperationsContainer Data Buffer from USB Buffer */
      (void)USBD_MTP_STORAGE_ReceiveContainer(pdev, pMsgBuffer, MTP_DataLength.rx_length);
      break;

    default:
      /* Expected Data Length Packet Received */
      pMsgBuffer = (uint32_t *) &hmtp->OperationsContainer;

      /* Fill hmtp->OperationsContainer Data Buffer from USB Buffer */
      (void)USBD_MTP_STORAGE_ReceiveContainer(pdev, pMsgBuffer, MTP_DataLength.rx_length);
      (void)USBD_MTP_STORAGE_DecodeOperations(pdev);
      break;

  }
  return (uint8_t)USBD_OK;
}


/**
  * @brief  USBD_MTP_STORAGE_ReceiveData
  *         Receive objects or object info from host
  * @param  pdev: device instance
  * @retval status value
  */
uint8_t USBD_MTP_STORAGE_ReceiveData(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  switch (hmtp->RECEIVE_DATA_STATUS)
  {
    case RECEIVE_COMMAND_DATA :
      if (hmtp->OperationsContainer.type == MTP_CONT_TYPE_COMMAND)
      {
        MTP_DataLength.temp_length = 0;
        MTP_DataLength.prv_len = 0;
        (void)USBD_MTP_STORAGE_DecodeOperations(pdev);

      }
      break;

    case RECEIVE_FIRST_DATA :
      if (hmtp->OperationsContainer.type == MTP_CONT_TYPE_DATA)
      {
        MTP_DataLength.totallen = hmtp->OperationsContainer.length;
        MTP_DataLength.temp_length = MTP_DataLength.rx_length;
        MTP_DataLength.rx_length = MTP_DataLength.temp_length - MTP_CONT_HEADER_SIZE;
        (void)USBD_MTP_STORAGE_DecodeOperations(pdev);

        if (MTP_DataLength.temp_length < hmtp->MaxPcktLen) /* we received all data, we don't need to go to next state */
        {
          hmtp->RECEIVE_DATA_STATUS = SEND_RESPONSE;
          (void)USBD_MTP_STORAGE_DecodeOperations(pdev);

          /* send response header after receiving all data successfully */
          (void)USBD_MTP_STORAGE_SendContainer(pdev, DATA_TYPE);
        }
      }

      break;

    case RECEIVE_REST_OF_DATA :
      MTP_DataLength.prv_len = MTP_DataLength.temp_length - MTP_CONT_HEADER_SIZE;
      (void)USBD_MTP_STORAGE_DecodeOperations(pdev);
      MTP_DataLength.temp_length = MTP_DataLength.temp_length + MTP_DataLength.rx_length;

      if (MTP_DataLength.temp_length == MTP_DataLength.totallen) /* we received all data*/
      {
        hmtp->RECEIVE_DATA_STATUS = SEND_RESPONSE;
        (void)USBD_MTP_STORAGE_DecodeOperations(pdev);

        /* send response header after receiving all data successfully  */
        (void)USBD_MTP_STORAGE_SendContainer(pdev, DATA_TYPE);
      }
      break;

    default :
      break;
  }

  return (uint8_t)USBD_OK;
}


/**
  * @brief  USBD_MTP_STORAGE_DecodeOperations
  *         Parse the operations and Process operations
  * @param  pdev: device instance
  * @retval status value
  */
static uint8_t USBD_MTP_STORAGE_DecodeOperations(USBD_HandleTypeDef  *pdev)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  switch (hmtp->OperationsContainer.code)
  {
    case MTP_OP_GET_DEVICE_INFO:
      USBD_MTP_OPT_GetDeviceInfo(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    case MTP_OP_OPEN_SESSION:
      USBD_MTP_OPT_CreateObjectHandle(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    case MTP_OP_GET_STORAGE_IDS:
      USBD_MTP_OPT_GetStorageIDS(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    case MTP_OP_GET_STORAGE_INFO:
      USBD_MTP_OPT_GetStorageInfo(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    case MTP_OP_GET_OBJECT_HANDLES:
      USBD_MTP_OPT_GetObjectHandle(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    case MTP_OP_GET_OBJECT_INFO:
      USBD_MTP_OPT_GetObjectInfo(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    case MTP_OP_GET_OBJECT_PROP_REFERENCES:
      USBD_MTP_OPT_GetObjectReferences(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    case MTP_OP_GET_OBJECT_PROPS_SUPPORTED:
      USBD_MTP_OPT_GetObjectPropSupp(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    case MTP_OP_GET_OBJECT_PROP_DESC:
      USBD_MTP_OPT_GetObjectPropDesc(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    case MTP_OP_GET_OBJECT_PROPLIST:
      USBD_MTP_OPT_GetObjectPropList(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    case MTP_OP_GET_OBJECT_PROP_VALUE:
      USBD_MTP_OPT_GetObjectPropValue(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    case MTP_OP_GET_DEVICE_PROP_DESC:
      USBD_MTP_OPT_GetDevicePropDesc(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    case MTP_OP_GET_OBJECT:
      USBD_MTP_OPT_GetObject(pdev);
      hmtp->MTP_ResponsePhase = MTP_READ_DATA;
      break;

    case MTP_OP_SEND_OBJECT_INFO:
      USBD_MTP_OPT_SendObjectInfo(pdev, (uint8_t *)(hmtp->rx_buff), MTP_DataLength.prv_len);
      hmtp->MTP_ResponsePhase = MTP_RECEIVE_DATA;
      break;

    case MTP_OP_SEND_OBJECT:
      USBD_MTP_OPT_SendObject(pdev, (uint8_t *)(hmtp->rx_buff), MTP_DataLength.rx_length);
      hmtp->MTP_ResponsePhase = MTP_RECEIVE_DATA;
      break;

    case MTP_OP_DELETE_OBJECT:
      USBD_MTP_OPT_DeleteObject(pdev);
      hmtp->MTP_ResponsePhase = MTP_RESPONSE_PHASE;
      break;

    default:
      break;
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MTP_STORAGE_ReceiveContainer
  *         Receive the Data from USB BulkOut Buffer to Pointer
  * @param  pdev: device instance
  * @param  pDst: destination address to copy the buffer
  * @param  len: length of data to copy
  * @retval status value
  */
static uint8_t USBD_MTP_STORAGE_ReceiveContainer(USBD_HandleTypeDef  *pdev,
                                                 uint32_t *pDst, uint32_t len)
{
    USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  uint32_t Counter;
  uint32_t *pdst = pDst;

  for (Counter = 0; Counter < len; Counter++)
  {
    *pdst = (hmtp->rx_buff[Counter]);
    pdst++;
  }
  return (uint8_t)USBD_OK;
}
//////////////

/**
  * @}
  */

/** @defgroup USBD_MTP_Private_Functions
  * @{
  */

/**
  * @brief  USBD_MTP_Init
  *         Initialize the MTP interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static USBD_StatusTypeDef USBD_MTP_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
//  USBD_MTP_HandleTypeDef *hmtp;
//
//  hmtp = (USBD_MTP_HandleTypeDef *)USBD_malloc(sizeof(USBD_MTP_HandleTypeDef));
//
//  if (hmtp == NULL)
//  {
//    pdev->pClassDataCmsit[pdev->classId] = NULL;
//    return (uint8_t)USBD_EMEM;
//  }
//
//  /* Setup the pClassData pointer */
//  pdev->pClassDataCmsit[pdev->classId] = (void *)hmtp;
//  pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  MTPInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MTPOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MTPCmdEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */


  /* Initialize all variables */
  (void)USBD_memset(hmtp, 0, sizeof (USBD_MTP_HandleTypeDef));

  /* Setup the max packet size according to selected speed */
  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    hmtp->MaxPcktLen = MTP_DATA_MAX_PACKET_SIZE;
  }
  else
  {
    hmtp->MaxPcktLen = MTP_DATA_MAX_PACKET_SIZE;
  }

  /* Open EP IN */
  (void)USBD_LL_OpenEP(pdev, MTPInEpAdd, USBD_EP_TYPE_BULK, hmtp->MaxPcktLen);
  pdev->ep_in[MTPInEpAdd & 0xFU].is_used = 1U;

  /* Open EP OUT */
  (void)USBD_LL_OpenEP(pdev, MTPOutEpAdd, USBD_EP_TYPE_BULK, hmtp->MaxPcktLen);
  pdev->ep_out[MTPOutEpAdd & 0xFU].is_used = 1U;

  /* Open INTR EP IN */
  (void)USBD_LL_OpenEP(pdev, MTPCmdEpAdd, USBD_EP_TYPE_INTR, MTP_CMD_PACKET_SIZE);
  pdev->ep_in[MTPCmdEpAdd & 0xFU].is_used = 1U;

  /* Init the MTP  layer */
  (void)USBD_MTP_STORAGE_Init(pdev);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MTP_DeInit
  *         DeInitialize the MTP layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static USBD_StatusTypeDef USBD_MTP_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
  UNUSED(cfgidx);

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this MTP class instance */
  MTPInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MTPOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MTPCmdEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  /* Close EP IN */
  (void)USBD_LL_CloseEP(pdev, MTPInEpAdd);
  pdev->ep_in[MTPInEpAdd & 0xFU].is_used = 0U;

  /* Close EP OUT */
  (void)USBD_LL_CloseEP(pdev, MTPOutEpAdd);
  pdev->ep_out[MTPOutEpAdd & 0xFU].is_used = 0U;

  /* Close EP Command */
  (void)USBD_LL_CloseEP(pdev, MTPCmdEpAdd);
  pdev->ep_in[MTPCmdEpAdd & 0xFU].is_used = 0U;
//
//  /* Free MTP Class Resources */
//  if (pdev->pClassDataCmsit[pdev->classId] != NULL)
//  {
//    /* De-Init the MTP layer */
//    (void)USBD_MTP_STORAGE_DeInit(pdev);
//
//    (void)USBD_free(pdev->pClassDataCmsit[pdev->classId]);
//    pdev->pClassDataCmsit[pdev->classId] = NULL;
//    pdev->pClassData = NULL;
//  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MTP_Setup
  *         Handle the MTP specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static USBD_StatusTypeDef USBD_MTP_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
  //  USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  USBD_StatusTypeDef ret = USBD_OK;
  uint16_t len = 0U;

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this MTP class instance */
  MTPOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  if (hmtp == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }
  const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
  if (interfacev != INTERFACE_MTP_CONTROL)
		return USBD_OK;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    /* Class request */
    case USB_REQ_TYPE_CLASS :
      switch (req->bRequest)
      {
        case  MTP_REQ_CANCEL:
          len = MIN(hmtp->MaxPcktLen, req->wLength);
          (void)USBD_CtlPrepareRx(pdev, (uint8_t *)(hmtp->rx_buff), len);
          break;

        case MTP_REQ_GET_EXT_EVENT_DATA:
          break;

        case MTP_REQ_RESET:
          /* Stop low layer file system operations if any */
          USBD_MTP_STORAGE_Cancel(pdev, MTP_PHASE_IDLE);

          (void)USBD_LL_PrepareReceive(pdev, MTPOutEpAdd, (uint8_t *)&hmtp->rx_buff, hmtp->MaxPcktLen);
          break;

        case MTP_REQ_GET_DEVICE_STATUS:
          switch (hmtp->MTP_ResponsePhase)
          {
            case MTP_READ_DATA :
              len = 4U;
              USBD_poke_u32(hmtp->dev_status, ((uint32_t)MTP_RESPONSE_DEVICE_BUSY << 16) | len);
              break;

            case MTP_RECEIVE_DATA :
              len = 4U;
              USBD_poke_u32(hmtp->dev_status, ((uint32_t)MTP_RESPONSE_TRANSACTION_CANCELLED << 16) | len);
              break;

            case MTP_PHASE_IDLE :
              len = 4U;
              USBD_poke_u32(hmtp->dev_status, ((uint32_t)MTP_RESPONSE_OK << 16) | len);
              break;

            default:
              break;
          }
          (void)USBD_CtlSendData(pdev, (uint8_t *)&hmtp->dev_status, len);
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    /* Interface & Endpoint request */
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_INTERFACE :

          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            hmtp->alt_setting [0] = 0U;
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hmtp->alt_setting, 1U);
          }
          break;

        case USB_REQ_SET_INTERFACE :
          if (pdev->dev_state != USBD_STATE_CONFIGURED)
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_CLEAR_FEATURE:

          /* Re-activate the EP */
          (void)USBD_LL_CloseEP(pdev, (uint8_t)req->wIndex);

          if ((((uint8_t)req->wIndex) & 0x80U) == 0x80U)
          {
            (void)USBD_LL_OpenEP(pdev, ((uint8_t)req->wIndex), USBD_EP_TYPE_BULK, hmtp->MaxPcktLen);
          }
          else
          {
            (void)USBD_LL_OpenEP(pdev, ((uint8_t)req->wIndex), USBD_EP_TYPE_BULK, hmtp->MaxPcktLen);
          }
          break;

        default:
          break;
      }
      break;

    default:
      break;
  }
  return (uint8_t)ret;
}

/**
  * @brief  USBD_MTP_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static USBD_StatusTypeDef USBD_MTP_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
  UNUSED(epnum);
  //  USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  uint16_t len;

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this MTP class instance */
  MTPInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MTPOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  if (epnum == (MTPInEpAdd & 0x7FU))
  {
    switch (hmtp->MTP_ResponsePhase)
    {
      case MTP_RESPONSE_PHASE :
        (void)USBD_MTP_STORAGE_SendContainer(pdev, REP_TYPE);

        /* prepare to receive next operation */
        len = MIN(hmtp->MaxPcktLen, pdev->request.wLength);

        (void)USBD_LL_PrepareReceive(pdev, MTPOutEpAdd, (uint8_t *)&hmtp->rx_buff, len);
        hmtp->MTP_ResponsePhase = MTP_PHASE_IDLE;
        break;

      case MTP_READ_DATA :
        (void)USBD_MTP_STORAGE_ReadData(pdev);

        /* prepare to receive next operation */
        len = MIN(hmtp->MaxPcktLen, pdev->request.wLength);

        (void)USBD_LL_PrepareReceive(pdev, MTPInEpAdd, (uint8_t *)&hmtp->rx_buff, len);
        break;

      case MTP_PHASE_IDLE :
        /* prepare to receive next operation */
        len = MIN(hmtp->MaxPcktLen, pdev->request.wLength);

        (void)USBD_LL_PrepareReceive(pdev, MTPOutEpAdd, (uint8_t *)&hmtp->rx_buff, len);

        break;
      default:
        break;
    }
  }
  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MTP_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static USBD_StatusTypeDef USBD_MTP_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
  UNUSED(epnum);
  //  USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  USBD_MTP_HandleTypeDef * const hmtp = & mtpHandle;
  uint16_t len;

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this MTP class instance */
  MTPOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  if (epnum == (MTPOutEpAdd & 0x7FU))
  {
	  // 00000000  10 00 00 00 01 00 02 10 00 00 00 00 01 00 00 00                                   . . . . . . . . . . . . . . . .
	  //
	  (void)USBD_MTP_STORAGE_ReceiveOpt(pdev);

	  switch (hmtp->MTP_ResponsePhase)
	  {
	    case MTP_RESPONSE_PHASE :

	      if (hmtp->ResponseLength == MTP_CONT_HEADER_SIZE)
	      {
	        (void)USBD_MTP_STORAGE_SendContainer(pdev, REP_TYPE);
	        hmtp->MTP_ResponsePhase = MTP_PHASE_IDLE;
	      }
	      else
	      {
	        (void)USBD_MTP_STORAGE_SendContainer(pdev, DATA_TYPE);
	      }
	      break;

	    case MTP_READ_DATA :
	      (void)USBD_MTP_STORAGE_ReadData(pdev);
	      break;

	    case MTP_RECEIVE_DATA :
	      (void)USBD_MTP_STORAGE_ReceiveData(pdev);

	      /* prepare endpoint to receive operations */
	      len = MIN(hmtp->MaxPcktLen, pdev->request.wLength);

	      (void)USBD_LL_PrepareReceive(pdev, MTPOutEpAdd, (uint8_t *)&hmtp->rx_buff, len);
	      break;

	    case MTP_PHASE_IDLE :
	      /* prepare to receive next operation */
	      len = MIN(hmtp->MaxPcktLen, pdev->request.wLength);

	      (void)USBD_LL_PrepareReceive(pdev, MTPOutEpAdd, (uint8_t *)&hmtp->rx_buff, len);
	      break;

	    default:
	      break;
	  }
  }

  return (uint8_t)USBD_OK;
}

static void USBD_MTP_ColdInit(void)
{
}

const USBD_ClassTypeDef USBD_CLASS_MTP =
{
	USBD_MTP_ColdInit,
	USBD_MTP_Init,
	USBD_MTP_DeInit,
	USBD_MTP_Setup,
	NULL, /*EP0_TxSent*/
	NULL, /*EP0_RxReady*/
	USBD_MTP_DataIn,
	USBD_MTP_DataOut,
	NULL, /*SOF */
	NULL, /*ISOIn*/
	NULL, /*ISOOut*/
};

#endif /* WITHUSBHW && WITHUSBDMTP */
