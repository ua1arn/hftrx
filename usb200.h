/* $Id$ */
/*
*   usb200.h
*
*   Definitions conforming to the USB 2.0 specification.
*
*   Adapted from Philips PDIUSBD12 firmware library - USB1.1 (USB100.H)
*   Revised for USB 2.0 compliance -- MJB Nov 2005
*/

#ifndef   __USB200_H__
#define   __USB200_H__


#define MAXIMUM_USB_STRING_LENGTH 255

/* values for the bits returned by the USB GET_STATUS command */
#define USB_GETSTATUS_SELF_POWERED                0x01
#define USB_GETSTATUS_REMOTE_WAKEUP_ENABLED       0x02

/* USB Descriptor Types */
#define USB_DEVICE_DESCRIPTOR_TYPE                  1
#define USB_CONFIGURATION_DESCRIPTOR_TYPE           2
#define USB_STRING_DESCRIPTOR_TYPE                  3
#define USB_INTERFACE_DESCRIPTOR_TYPE               4
#define USB_ENDPOINT_DESCRIPTOR_TYPE                5
#define USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE        6
#define USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE      7
#define USB_INTERFACE_POWER_DESCRIPTOR_TYPE         8
#define USB_OTG_DESCRIPTOR_TYPE                     9
#define USB_DEBUG_DESCRIPTOR_TYPE                  10
#define USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE  11 
#define USB_DEVICE_CAPABITY_DESCRIPTOR_TYPE                           0x10

/* Wireless USB extension Descriptor Type. */
#define USB_SECURITY_TYPE                          12
#define USB_KEY_TYPE                               13
#define USB_ENCRIPTION_TYPE                        14
#define USB_BOS_TYPE                               15
#define USB_DEVICE_CAPABILITY_TYPE                 16
#define USB_WIRELESS_ENDPOINT_COMPANION_TYPE       17

/* USB Device Classes */
#define USB_DEVICE_CLASS_RESERVED              0x00
#define USB_DEVICE_CLASS_AUDIO                 0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS        0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE       0x03
#define USB_DEVICE_CLASS_MONITOR               0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE    0x05
#define USB_DEVICE_CLASS_POWER                 0x06
#define USB_DEVICE_CLASS_PRINTER               0x07
#define USB_DEVICE_CLASS_STORAGE               0x08
#define USB_DEVICE_CLASS_HUB                   0x09
#define USB_DEVICE_CLASS_WIRELESS_CONTROLLER   0xE0
#define USB_DEVICE_CLASS_MISCELLANEOUS         0xEF
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC       0xFF

/* bMaxPower in Configuration Descriptor */
#define USB_CONFIG_POWER_MA(mA)                ((mA)/2)

/* bEndpointAddress in Endpoint Descriptor */
#define USB_ENDPOINT_DIRECTION_MASK            0x80
#define USB_ENDPOINT_OUT(addr)                 ((addr) & 0x7F)
#define USB_ENDPOINT_IN(addr)                  ((addr) | 0x80)

/* bmAttributes in Endpoint Descriptor */
#define USB_ENDPOINT_TYPE_MASK                 0x03
#define USB_ENDPOINT_TYPE_CONTROL              0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS          0x01
#define USB_ENDPOINT_TYPE_BULK                 0x02
#define USB_ENDPOINT_TYPE_INTERRUPT            0x03
#define USB_ENDPOINT_SYNC_MASK                 0x0C
#define USB_ENDPOINT_SYNC_NO_SYNCHRONIZATION   0x00
#define USB_ENDPOINT_SYNC_ASYNCHRONOUS         0x04
#define USB_ENDPOINT_SYNC_ADAPTIVE             0x08
#define USB_ENDPOINT_SYNC_SYNCHRONOUS          0x0C
#define USB_ENDPOINT_USAGE_MASK                0x30
#define USB_ENDPOINT_USAGE_DATA                0x00
#define USB_ENDPOINT_USAGE_FEEDBACK            0x10
#define USB_ENDPOINT_USAGE_IMPLICIT_FEEDBACK   0x20
#define USB_ENDPOINT_USAGE_RESERVED            0x30

#define USB_DESCRIPTOR_MAKE_TYPE_AND_INDEX(d, i) ((uint16_t)((uint16_t)d<<8 | i))


/* Endpoint direction bit, stored in address field, also wIndex of SET/CLEAR_FEATURE */
#define USB_ENDPOINT_DIRECTION_MASK               0x80

/* test direction bit in the bEndpointAddress field of an endpoint descriptor. */
#define USB_ENDPOINT_DIRECTION_OUT(addr)      (!((addr) & USB_ENDPOINT_DIRECTION_MASK))
#define USB_ENDPOINT_DIRECTION_IN(addr)       ((addr) & USB_ENDPOINT_DIRECTION_MASK)

/* Bitmasks and defined values for bRequestType field of Setup pkt */
#define USB_RECIPIENT_MASK       (uint8_t)0x1F
#define USB_RECIPIENT_DEVICE     (uint8_t)0x00
#define USB_RECIPIENT_INTERFACE  (uint8_t)0x01
#define USB_RECIPIENT_ENDPOINT   (uint8_t)0x02

#define USB_REQUEST_TYPE_MASK    (uint8_t)0x60
#define USB_STANDARD_REQUEST     (uint8_t)0x00
#define USB_CLASS_REQUEST        (uint8_t)0x20
#define USB_VENDOR_REQUEST       (uint8_t)0x40

#define DEVICE_ADDRESS_MASK      0x7F

/* USB defined request codes */
/* see chapter 9 of the USB 2.0 specifcation for more information. */
/* These are the correct values based on the USB 2.0 specification */
#define USB_REQUEST_GET_STATUS                    0x00
#define USB_REQUEST_CLEAR_FEATURE                 0x01
#define USB_REQUEST_SET_FEATURE                   0x03
#define USB_REQUEST_SET_ADDRESS                   0x05
#define USB_REQUEST_GET_DESCRIPTOR                0x06
#define USB_REQUEST_SET_DESCRIPTOR                0x07
#define USB_REQUEST_GET_CONFIGURATION             0x08
#define USB_REQUEST_SET_CONFIGURATION             0x09
#define USB_REQUEST_GET_INTERFACE                 0x0A
#define USB_REQUEST_SET_INTERFACE                 0x0B
#define USB_REQUEST_SYNC_FRAME                    0x0C

/* defined USB device classes */
/*
#define USB_DEVICE_CLASS_RESERVED           0x00
#define USB_DEVICE_CLASS_AUDIO              0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS     0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE    0x03
#define USB_DEVICE_CLASS_MONITOR            0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE 0x05
#define USB_DEVICE_CLASS_POWER              0x06
#define USB_DEVICE_CLASS_PRINTER            0x07
#define USB_DEVICE_CLASS_STORAGE            0x08
#define USB_DEVICE_CLASS_HUB                0x09
#define USB_DEVICE_CLASS_TMC                0xFE
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC    0xFF

*/

#define CS_INTERFACE                 0x24

#define CDC_INTERFACE_DESCRIPTOR_TYPE                 0x24
#define CDC_ENDPOINT_DESCRIPTOR_TYPE                  0x25

#define USB_INTERFACE_ASSOC_DESCRIPTOR_TYPE			  0x0B

#define AUDIO_DESCRIPTOR_TYPE                         0x21
#define USB_DEVICE_CLASS_AUDIO                        0x01
#define AUDIO_SUBCLASS_AUDIOCONTROL                   0x01
#define AUDIO_SUBCLASS_AUDIOSTREAMING                 0x02
#define AUDIO_PROTOCOL_UNDEFINED                      0x00
#define AUDIO_STREAMING_GENERAL                       0x01
#define AUDIO_STREAMING_FORMAT_TYPE                   0x02

/* Audio Descriptor Types */
#define AUDIO_INTERFACE_DESCRIPTOR_TYPE               0x24
#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE                0x25

#define USB_ENDPOINT_TYPE_ASYNC_FEEDBACK  			  0x15
#define USB_ENDPOINT_TYPE_ISOCHRONOUS                 0x01
#define USB_ENDPOINT_TYPE_BULK		                  0x02
#define USB_ENDPOINT_TYPE_INTERRUPT	                  0x03
#define AUDIO_ENDPOINT_GENERAL                        0x01


/* USB 2.0 defined Feature Selectors (wValue in Set_Feature setup pkt) */
#define USB_FEATURE_EP_HALT                                0
#define USB_FEATURE_REMOTE_WAKEUP                          1
#define USB_FEATURE_TEST_MODE                              2

#define USB_OTG_HS_MAX_PACKET_SIZE           512U
#define USB_OTG_FS_MAX_PACKET_SIZE           64U
#define USB_OTG_MAX_EP0_SIZE                 64U

/* Audio Interface Subclass Codes */
#define AUDIO_SUBCLASS_UNDEFINED                0x00
#define AUDIO_SUBCLASS_AUDIOCONTROL             0x01
#define AUDIO_SUBCLASS_AUDIOSTREAMING           0x02
#define AUDIO_SUBCLASS_MIDISTREAMING            0x03

/* Audio Interface Protocol Codes */
#define AUDIO_PROTOCOL_UNDEFINED                0x00


/* Audio Descriptor Types */
#define AUDIO_UNDEFINED_DESCRIPTOR_TYPE         0x20
#define AUDIO_DEVICE_DESCRIPTOR_TYPE            0x21
#define AUDIO_CONFIGURATION_DESCRIPTOR_TYPE     0x22
#define AUDIO_STRING_DESCRIPTOR_TYPE            0x23
#define AUDIO_INTERFACE_DESCRIPTOR_TYPE         0x24
#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE          0x25


/* Audio Control Interface Descriptor Subtypes */
#define AUDIO_CONTROL_UNDEFINED                 0x00
#define AUDIO_CONTROL_HEADER                    0x01
#define AUDIO_CONTROL_INPUT_TERMINAL            0x02
#define AUDIO_CONTROL_OUTPUT_TERMINAL           0x03
#define AUDIO_CONTROL_MIXER_UNIT                0x04
#define AUDIO_CONTROL_SELECTOR_UNIT             0x05
#define AUDIO_CONTROL_FEATURE_UNIT              0x06
#define AUDIO_CONTROL_PROCESSING_UNIT           0x07
#define AUDIO_CONTROL_EXTENSION_UNIT            0x08

/* Audio Streaming Interface Descriptor Subtypes */
#define AUDIO_STREAMING_UNDEFINED               0x00
#define AUDIO_STREAMING_GENERAL                 0x01
#define AUDIO_STREAMING_FORMAT_TYPE             0x02
#define AUDIO_STREAMING_FORMAT_SPECIFIC         0x03

/* Audio Endpoint Descriptor Subtypes */
#define AUDIO_ENDPOINT_UNDEFINED                0x00
#define AUDIO_ENDPOINT_GENERAL                  0x01

/* Audio Processing Unit Process Types */
#define AUDIO_UNDEFINED_PROCESS                 0x00
#define AUDIO_UP_DOWN_MIX_PROCESS               0x01
#define AUDIO_DOLBY_PROLOGIC_PROCESS            0x02
#define AUDIO_3D_STEREO_PROCESS                 0x03
#define AUDIO_REVERBERATION_PROCESS             0x04
#define AUDIO_CHORUS_PROCESS                    0x05
#define AUDIO_DYN_RANGE_COMP_PROCESS            0x06


/* Audio Request Codes */
#define AUDIO_REQUEST_UNDEFINED                 0x00
#define AUDIO_REQUEST_SET_CUR                   0x01
#define AUDIO_REQUEST_GET_CUR                   0x81
#define AUDIO_REQUEST_SET_MIN                   0x02
#define AUDIO_REQUEST_GET_MIN                   0x82
#define AUDIO_REQUEST_SET_MAX                   0x03
#define AUDIO_REQUEST_GET_MAX                   0x83
#define AUDIO_REQUEST_SET_RES                   0x04
#define AUDIO_REQUEST_GET_RES                   0x84
#define AUDIO_REQUEST_SET_MEM                   0x05
#define AUDIO_REQUEST_GET_MEM                   0x85
#define AUDIO_REQUEST_GET_STAT                  0xFF


/* Audio Control Selector Codes */
#define AUDIO_CONTROL_UNDEFINED                 0x00    /* Common Selector */

/*  Terminal Control Selectors */
#define AUDIO_COPY_PROTECT_CONTROL              0x01

/*  Feature Unit Control Selectors */
#define AUDIO_MUTE_CONTROL                      0x01
#define AUDIO_VOLUME_CONTROL                    0x02
#define AUDIO_BASS_CONTROL                      0x03
#define AUDIO_MID_CONTROL                       0x04
#define AUDIO_TREBLE_CONTROL                    0x05
#define AUDIO_GRAPHIC_EQUALIZER_CONTROL         0x06
#define AUDIO_AUTOMATIC_GAIN_CONTROL            0x07
#define AUDIO_DELAY_CONTROL                     0x08
#define AUDIO_BASS_BOOST_CONTROL                0x09
#define AUDIO_LOUDNESS_CONTROL                  0x0A

/*  Processing Unit Control Selectors: */
#define AUDIO_ENABLE_CONTROL                    0x01    /* Common Selector */
#define AUDIO_MODE_SELECT_CONTROL               0x02    /* Common Selector */

/*  - Up/Down-mix Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */
/*      AUDIO_MODE_SELECT_CONTROL               0x02       Common Selector */

/*  - Dolby Prologic Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */
/*      AUDIO_MODE_SELECT_CONTROL               0x02       Common Selector */

/*  - 3D Stereo Extender Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */
#define AUDIO_SPACIOUSNESS_CONTROL              0x02

/*  - Reverberation Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */
#define AUDIO_REVERB_LEVEL_CONTROL              0x02
#define AUDIO_REVERB_TIME_CONTROL               0x03
#define AUDIO_REVERB_FEEDBACK_CONTROL           0x04

/*  - Chorus Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */
#define AUDIO_CHORUS_LEVEL_CONTROL              0x02
#define AUDIO_SHORUS_RATE_CONTROL               0x03
#define AUDIO_CHORUS_DEPTH_CONTROL              0x04

/*  - Dynamic Range Compressor Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */
#define AUDIO_COMPRESSION_RATE_CONTROL          0x02
#define AUDIO_MAX_AMPL_CONTROL                  0x03
#define AUDIO_THRESHOLD_CONTROL                 0x04
#define AUDIO_ATTACK_TIME_CONTROL               0x05
#define AUDIO_RELEASE_TIME_CONTROL              0x06

/*  Extension Unit Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */

/*  Endpoint Control Selectors */
#define AUDIO_SAMPLING_FREQ_CONTROL             0x01
#define AUDIO_PITCH_CONTROL                     0x02


/* Audio Format Specific Control Selectors */

/*  MPEG Control Selectors */
#define AUDIO_MPEG_CONTROL_UNDEFINED            0x00
#define AUDIO_MPEG_DUAL_CHANNEL_CONTROL         0x01
#define AUDIO_MPEG_SECOND_STEREO_CONTROL        0x02
#define AUDIO_MPEG_MULTILINGUAL_CONTROL         0x03
#define AUDIO_MPEG_DYN_RANGE_CONTROL            0x04
#define AUDIO_MPEG_SCALING_CONTROL              0x05
#define AUDIO_MPEG_HILO_SCALING_CONTROL         0x06

/*  AC-3 Control Selectors */
#define AUDIO_AC3_CONTROL_UNDEFINED             0x00
#define AUDIO_AC3_MODE_CONTROL                  0x01
#define AUDIO_AC3_DYN_RANGE_CONTROL             0x02
#define AUDIO_AC3_SCALING_CONTROL               0x03
#define AUDIO_AC3_HILO_SCALING_CONTROL          0x04


/* Audio Format Types */
#define AUDIO_FORMAT_TYPE_UNDEFINED             0x00
#define AUDIO_FORMAT_TYPE_I                     0x01
#define AUDIO_FORMAT_TYPE_II                    0x02
#define AUDIO_FORMAT_TYPE_III                   0x03

/* Audio Data Format Codes */

/*  Audio Data Format Type I Codes */
#define AUDIO_FORMAT_TYPE_I_UNDEFINED           0x0000
#define AUDIO_FORMAT_PCM                        0x0001
#define AUDIO_FORMAT_PCM8                       0x0002
#define AUDIO_FORMAT_IEEE_FLOAT                 0x0003
#define AUDIO_FORMAT_ALAW                       0x0004
#define AUDIO_FORMAT_MULAW                      0x0005

/*  Audio Data Format Type II Codes */
#define AUDIO_FORMAT_TYPE_II_UNDEFINED          0x1000
#define AUDIO_FORMAT_MPEG                       0x1001
#define AUDIO_FORMAT_AC3                        0x1002

/*  Audio Data Format Type III Codes */
#define AUDIO_FORMAT_TYPE_III_UNDEFINED         0x2000
#define AUDIO_FORMAT_IEC1937_AC3                0x2001
#define AUDIO_FORMAT_IEC1937_MPEG1_L1           0x2002
#define AUDIO_FORMAT_IEC1937_MPEG1_L2_3         0x2003
#define AUDIO_FORMAT_IEC1937_MPEG2_NOEXT        0x2003
#define AUDIO_FORMAT_IEC1937_MPEG2_EXT          0x2004
#define AUDIO_FORMAT_IEC1937_MPEG2_L1_LS        0x2005
#define AUDIO_FORMAT_IEC1937_MPEG2_L2_3         0x2006


/* Predefined Audio Channel Configuration Bits */
#define AUDIO_CHANNEL_M                         0x0000  /* Mono */
#define AUDIO_CHANNEL_L                         0x0001  /* Left Front */
#define AUDIO_CHANNEL_R                         0x0002  /* Right Front */
#define AUDIO_CHANNEL_C                         0x0004  /* Center Front */
#define AUDIO_CHANNEL_LFE                       0x0008  /* Low Freq. Enhance. */
#define AUDIO_CHANNEL_LS                        0x0010  /* Left Surround */
#define AUDIO_CHANNEL_RS                        0x0020  /* Right Surround */
#define AUDIO_CHANNEL_LC                        0x0040  /* Left of Center */
#define AUDIO_CHANNEL_RC                        0x0080  /* Right of Center */
#define AUDIO_CHANNEL_S                         0x0100  /* Surround */
#define AUDIO_CHANNEL_SL                        0x0200  /* Side Left */
#define AUDIO_CHANNEL_SR                        0x0400  /* Side Right */
#define AUDIO_CHANNEL_T                         0x0800  /* Top */


/* Feature Unit Control Bits */
#define AUDIO_CONTROL_MUTE                      0x0001
#define AUDIO_CONTROL_VOLUME                    0x0002
#define AUDIO_CONTROL_BASS                      0x0004
#define AUDIO_CONTROL_MID                       0x0008
#define AUDIO_CONTROL_TREBLE                    0x0010
#define AUDIO_CONTROL_GRAPHIC_EQUALIZER         0x0020
#define AUDIO_CONTROL_AUTOMATIC_GAIN            0x0040
#define AUDIO_CONTROL_DEALY                     0x0080
#define AUDIO_CONTROL_BASS_BOOST                0x0100
#define AUDIO_CONTROL_LOUDNESS                  0x0200

/* Processing Unit Control Bits: */
#define AUDIO_CONTROL_ENABLE                    0x0001  /* Common Bit */
#define AUDIO_CONTROL_MODE_SELECT               0x0002  /* Common Bit */

/* - Up/Down-mix Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */
/*      AUDIO_CONTROL_MODE_SELECT               0x0002     Common Bit */

/* - Dolby Prologic Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */
/*      AUDIO_CONTROL_MODE_SELECT               0x0002     Common Bit */

/* - 3D Stereo Extender Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */
#define AUDIO_CONTROL_SPACIOUSNESS              0x0002

/* - Reverberation Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */
#define AUDIO_CONTROL_REVERB_TYPE               0x0002
#define AUDIO_CONTROL_REVERB_LEVEL              0x0004
#define AUDIO_CONTROL_REVERB_TIME               0x0008
#define AUDIO_CONTROL_REVERB_FEEDBACK           0x0010

/* - Chorus Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */
#define AUDIO_CONTROL_CHORUS_LEVEL              0x0002
#define AUDIO_CONTROL_SHORUS_RATE               0x0004
#define AUDIO_CONTROL_CHORUS_DEPTH              0x0008

/* - Dynamic Range Compressor Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */
#define AUDIO_CONTROL_COMPRESSION_RATE          0x0002
#define AUDIO_CONTROL_MAX_AMPL                  0x0004
#define AUDIO_CONTROL_THRESHOLD                 0x0008
#define AUDIO_CONTROL_ATTACK_TIME               0x0010
#define AUDIO_CONTROL_RELEASE_TIME              0x0020

/* Extension Unit Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */

/* Endpoint Control Bits */
#define AUDIO_CONTROL_SAMPLING_FREQ             0x01
#define AUDIO_CONTROL_PITCH                     0x02
#define AUDIO_MAX_PACKETS_ONLY                  0x80


/* Audio Terminal Types */

/*  USB Terminal Types */
#define AUDIO_TERMINAL_USB_UNDEFINED            0x0100
#define AUDIO_TERMINAL_USB_STREAMING            0x0101
#define AUDIO_TERMINAL_USB_VENDOR_SPECIFIC      0x01FF

/*  Input Terminal Types */
#define AUDIO_TERMINAL_INPUT_UNDEFINED          0x0200
#define AUDIO_TERMINAL_MICROPHONE               0x0201
#define AUDIO_TERMINAL_DESKTOP_MICROPHONE       0x0202
#define AUDIO_TERMINAL_PERSONAL_MICROPHONE      0x0203
#define AUDIO_TERMINAL_OMNI_DIR_MICROPHONE      0x0204
#define AUDIO_TERMINAL_MICROPHONE_ARRAY         0x0205
#define AUDIO_TERMINAL_PROCESSING_MIC_ARRAY     0x0206

/*  Output Terminal Types */
#define AUDIO_TERMINAL_OUTPUT_UNDEFINED         0x0300
#define AUDIO_TERMINAL_SPEAKER                  0x0301
#define AUDIO_TERMINAL_HEADPHONES               0x0302
#define AUDIO_TERMINAL_HEAD_MOUNTED_AUDIO       0x0303
#define AUDIO_TERMINAL_DESKTOP_SPEAKER          0x0304
#define AUDIO_TERMINAL_ROOM_SPEAKER             0x0305
#define AUDIO_TERMINAL_COMMUNICATION_SPEAKER    0x0306
#define AUDIO_TERMINAL_LOW_FREQ_SPEAKER         0x0307

/*  Bi-directional Terminal Types */
#define AUDIO_TERMINAL_BIDIRECTIONAL_UNDEFINED  0x0400
#define AUDIO_TERMINAL_HANDSET                  0x0401
#define AUDIO_TERMINAL_HEAD_MOUNTED_HANDSET     0x0402
#define AUDIO_TERMINAL_SPEAKERPHONE             0x0403
#define AUDIO_TERMINAL_SPEAKERPHONE_ECHOSUPRESS 0x0404
#define AUDIO_TERMINAL_SPEAKERPHONE_ECHOCANCEL  0x0405

/*  Telephony Terminal Types */
#define AUDIO_TERMINAL_TELEPHONY_UNDEFINED      0x0500
#define AUDIO_TERMINAL_PHONE_LINE               0x0501
#define AUDIO_TERMINAL_TELEPHONE                0x0502
#define AUDIO_TERMINAL_DOWN_LINE_PHONE          0x0503

/*  External Terminal Types */
#define AUDIO_TERMINAL_EXTERNAL_UNDEFINED       0x0600
#define AUDIO_TERMINAL_ANALOG_CONNECTOR         0x0601
#define AUDIO_TERMINAL_DIGITAL_AUDIO_INTERFACE  0x0602
#define AUDIO_TERMINAL_LINE_CONNECTOR           0x0603
#define AUDIO_TERMINAL_LEGACY_AUDIO_CONNECTOR   0x0604
#define AUDIO_TERMINAL_SPDIF_INTERFACE          0x0605
#define AUDIO_TERMINAL_1394_DA_STREAM           0x0606
#define AUDIO_TERMINAL_1394_DA_STREAM_TRACK     0x0607

/*  Embedded Function Terminal Types */
#define AUDIO_TERMINAL_EMBEDDED_UNDEFINED       0x0700
#define AUDIO_TERMINAL_CALIBRATION_NOISE        0x0701
#define AUDIO_TERMINAL_EQUALIZATION_NOISE       0x0702
#define AUDIO_TERMINAL_CD_PLAYER                0x0703
#define AUDIO_TERMINAL_DAT                      0x0704
#define AUDIO_TERMINAL_DCC                      0x0705
#define AUDIO_TERMINAL_MINI_DISK                0x0706
#define AUDIO_TERMINAL_ANALOG_TAPE              0x0707
#define AUDIO_TERMINAL_PHONOGRAPH               0x0708
#define AUDIO_TERMINAL_VCR_AUDIO                0x0709
#define AUDIO_TERMINAL_VIDEO_DISC_AUDIO         0x070A
#define AUDIO_TERMINAL_DVD_AUDIO                0x070B
#define AUDIO_TERMINAL_TV_TUNER_AUDIO           0x070C
#define AUDIO_TERMINAL_SATELLITE_RECEIVER_AUDIO 0x070D
#define AUDIO_TERMINAL_CABLE_TUNER_AUDIO        0x070E
#define AUDIO_TERMINAL_DSS_AUDIO                0x070F
#define AUDIO_TERMINAL_RADIO_RECEIVER           0x0710
#define AUDIO_TERMINAL_RADIO_TRANSMITTER        0x0711
#define AUDIO_TERMINAL_MULTI_TRACK_RECORDER     0x0712
#define AUDIO_TERMINAL_SYNTHESIZER              0x0713

/*----------------------------------------------------------------------------
 *      Definitions  based on usbcdc11.pdf (www.usb.org)
 *---------------------------------------------------------------------------*/
// Communication device class specification version 1.10
#define CDC_V1_10                               0x0110

// Communication interface class code
// (usbcdc11.pdf, 4.2, Table 15)
#define CDC_COMMUNICATION_INTERFACE_CLASS       0x02

// Communication interface class subclass codes
// (usbcdc11.pdf, 4.3, Table 16)
#define CDC_DIRECT_LINE_CONTROL_MODEL           0x01
#define CDC_ABSTRACT_CONTROL_MODEL              0x02
#define CDC_TELEPHONE_CONTROL_MODEL             0x03
#define CDC_MULTI_CHANNEL_CONTROL_MODEL         0x04
#define CDC_CAPI_CONTROL_MODEL                  0x05
#define CDC_ETHERNET_NETWORKING_CONTROL_MODEL   0x06
#define CDC_ATM_NETWORKING_CONTROL_MODEL        0x07
#define CDC_ETHERNET_EMULATION_MODEL			0x0C

// Communication interface class control protocol codes
// (usbcdc11.pdf, 4.4, Table 17)
#define CDC_PROTOCOL_COMMON_AT_COMMANDS         0x01

// Data interface class code
// (usbcdc11.pdf, 4.5, Table 18)
#define CDC_DATA_INTERFACE_CLASS                0x0A

// Data interface class protocol codes
// (usbcdc11.pdf, 4.7, Table 19)
#define CDC_PROTOCOL_ISDN_BRI                   0x30
#define CDC_PROTOCOL_HDLC                       0x31
#define CDC_PROTOCOL_TRANSPARENT                0x32
#define CDC_PROTOCOL_Q921_MANAGEMENT            0x50
#define CDC_PROTOCOL_Q921_DATA_LINK             0x51
#define CDC_PROTOCOL_Q921_MULTIPLEXOR           0x52
#define CDC_PROTOCOL_V42                        0x90
#define CDC_PROTOCOL_EURO_ISDN                  0x91
#define CDC_PROTOCOL_V24_RATE_ADAPTATION        0x92
#define CDC_PROTOCOL_CAPI                       0x93
#define CDC_PROTOCOL_HOST_BASED_DRIVER          0xFD
#define CDC_PROTOCOL_DESCRIBED_IN_PUFD          0xFE

// Type values for bDescriptorType field of functional descriptors
// (usbcdc11.pdf, 5.2.3, Table 24)
#define CDC_CS_INTERFACE                        0x24
#define CDC_CS_ENDPOINT                         0x25

// Type values for bDescriptorSubtype field of functional descriptors
// (usbcdc11.pdf, 5.2.3, Table 25)
#define CDC_HEADER                              0x00
#define CDC_CALL_MANAGEMENT                     0x01
#define CDC_ABSTRACT_CONTROL_MANAGEMENT         0x02
#define CDC_DIRECT_LINE_MANAGEMENT              0x03
#define CDC_TELEPHONE_RINGER                    0x04
#define CDC_REPORTING_CAPABILITIES              0x05
#define CDC_UNION                               0x06
#define CDC_COUNTRY_SELECTION                   0x07
#define CDC_TELEPHONE_OPERATIONAL_MODES         0x08
#define CDC_USB_TERMINAL                        0x09
#define CDC_NETWORK_CHANNEL                     0x0A
#define CDC_PROTOCOL_UNIT                       0x0B
#define CDC_EXTENSION_UNIT                      0x0C
#define CDC_MULTI_CHANNEL_MANAGEMENT            0x0D
#define CDC_CAPI_CONTROL_MANAGEMENT             0x0E
#define CDC_ETHERNET_NETWORKING                 0x0F
#define CDC_ATM_NETWORKING                      0x10

// CDC class-specific request codes
// (usbcdc11.pdf, 6.2, Table 46)
// see Table 45 for info about the specific requests.
#define CDC_SEND_ENCAPSULATED_COMMAND           0x00
#define CDC_GET_ENCAPSULATED_RESPONSE           0x01
#define CDC_SET_COMM_FEATURE                    0x02
#define CDC_GET_COMM_FEATURE                    0x03
#define CDC_CLEAR_COMM_FEATURE                  0x04
#define CDC_SET_AUX_LINE_STATE                  0x10
#define CDC_SET_HOOK_STATE                      0x11
#define CDC_PULSE_SETUP                         0x12
#define CDC_SEND_PULSE                          0x13
#define CDC_SET_PULSE_TIME                      0x14
#define CDC_RING_AUX_JACK                       0x15
#define CDC_SET_LINE_CODING                     0x20
#define CDC_GET_LINE_CODING                     0x21
#define CDC_SET_CONTROL_LINE_STATE              0x22
#define CDC_SEND_BREAK                          0x23
#define CDC_SET_RINGER_PARMS                    0x30
#define CDC_GET_RINGER_PARMS                    0x31
#define CDC_SET_OPERATION_PARMS                 0x32
#define CDC_GET_OPERATION_PARMS                 0x33
#define CDC_SET_LINE_PARMS                      0x34
#define CDC_GET_LINE_PARMS                      0x35
#define CDC_DIAL_DIGITS                         0x36
#define CDC_SET_UNIT_PARAMETER                  0x37
#define CDC_GET_UNIT_PARAMETER                  0x38
#define CDC_CLEAR_UNIT_PARAMETER                0x39
#define CDC_GET_PROFILE                         0x3A
#define CDC_SET_ETHERNET_MULTICAST_FILTERS      0x40
#define CDC_SET_ETHERNET_PMP_FILTER             0x41
#define CDC_GET_ETHERNET_PMP_FILTER             0x42
#define CDC_SET_ETHERNET_PACKET_FILTER          0x43
#define CDC_GET_ETHERNET_STATISTIC              0x44
#define CDC_SET_ATM_DATA_FORMAT                 0x50
#define CDC_GET_ATM_DEVICE_STATISTICS           0x51
#define CDC_SET_ATM_DEFAULT_VC                  0x52
#define CDC_GET_ATM_VC_STATISTICS               0x53

// Communication feature selector codes
// (usbcdc11.pdf, 6.2.2..6.2.4, Table 47)
#define CDC_ABSTRACT_STATE                      0x01
#define CDC_COUNTRY_SETTING                     0x02

// Feature Status returned for ABSTRACT_STATE Selector
// (usbcdc11.pdf, 6.2.3, Table 48)
#define CDC_IDLE_SETTING                        (1 << 0)
#define CDC_DATA_MULTPLEXED_STATE               (1 << 1)


// Control signal bitmap values for the SetControlLineState request
// (usbcdc11.pdf, 6.2.14, Table 51)
#define CDC_DTE_PRESENT                         (1 << 0)
#define CDC_ACTIVATE_CARRIER                    (1 << 1)

// CDC class-specific notification codes
// (usbcdc11.pdf, 6.3, Table 68)
// see Table 67 for Info about class-specific notifications
#define CDC_NOTIFICATION_NETWORK_CONNECTION     0x00
#define CDC_RESPONSE_AVAILABLE                  0x01
#define CDC_AUX_JACK_HOOK_STATE                 0x08
#define CDC_RING_DETECT                         0x09
#define CDC_NOTIFICATION_SERIAL_STATE           0x20
#define CDC_CALL_STATE_CHANGE                   0x28
#define CDC_LINE_STATE_CHANGE                   0x29
#define CDC_CONNECTION_SPEED_CHANGE             0x2A

// UART state bitmap values (Serial state notification).
// (usbcdc11.pdf, 6.3.5, Table 69)
#define CDC_SERIAL_STATE_OVERRUN                (1 << 6)  // receive data overrun error has occurred
#define CDC_SERIAL_STATE_PARITY                 (1 << 5)  // parity error has occurred
#define CDC_SERIAL_STATE_FRAMING                (1 << 4)  // framing error has occurred
#define CDC_SERIAL_STATE_RING                   (1 << 3)  // state of ring signal detection
#define CDC_SERIAL_STATE_BREAK                  (1 << 2)  // state of break detection
#define CDC_SERIAL_STATE_TX_CARRIER             (1 << 1)  // state of transmission carrier
#define CDC_SERIAL_STATE_RX_CARRIER             (1 << 0)  // state of receiver carrier


// HID

#define HID_DESCRIPTOR_TYPE           0x21
#define HID_REPORT_DESC               0x22

#endif   /* __USB200_H__ */
