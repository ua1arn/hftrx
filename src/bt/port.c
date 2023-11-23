/*
 * Copyright (C) 2020 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BLUEKITCHEN
 * GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at
 * contact@bluekitchen-gmbh.com
 *
 */

#include "hardware.h"
#include "board.h"
#include "formats.h"

#if WITHUSEUSBBT


#include "port.h"

#include <stdio.h>
#include <stddef.h>

#include "port.h"
#include "btstack.h"
#include "btstack_debug.h"
#include "btstack_audio.h"
#include "btstack_run_loop_embedded.h"
#include "btstack_tlv.h"
#include "btstack_tlv_flash_bank.h"
#include "ble/le_device_db_tlv.h"
#include "classic/btstack_link_key_db_tlv.h"
//#include "hal_flash_bank_stm32.h"
#include "hci_transport.h"
#include "hci_transport_h2_stm32.h"
#include "hci_transport_h2_tinyusb.h"

#ifdef ENABLE_SEGGER_RTT
#include "SEGGER_RTT.h"
#include "hci_dump_segger_rtt_stdout.h"
#else
#include "hci_dump_embedded_stdout.h"
#endif

/**
  * @brief  Initializes wave recording.
  * @param  AudioFreq: Audio frequency to be configured for the I2S peripheral.
  * @param  BitRes: Audio frequency to be configured for the I2S peripheral.
  * @param  ChnlNbr: Audio frequency to be configured for the I2S peripheral.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_IN_Init(uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr)
{

//#if 0
//  // BK: only do PLL clock configuration in sink
//  /* Configure PLL clock */
//  BSP_AUDIO_IN_ClockConfig(&hAudioInI2s, AudioFreq, NULL);
//#endif
//
//  /* Configure the PDM library */
//  PDMDecoder_Init(AudioFreq, ChnlNbr);
//
//  /* Configure the I2S peripheral */
//  hAudioInI2s.Instance = I2S2;
//  if(HAL_I2S_GetState(&hAudioInI2s) == HAL_I2S_STATE_RESET)
//  {
//    /* Initialize the I2S Msp: this __weak function can be rewritten by the application */
//    BSP_AUDIO_IN_MspInit(&hAudioInI2s, NULL);
//  }
//
//  /* Configure the I2S2 */
//  I2S2_Init(AudioFreq);
//
  /* Return AUDIO_OK when all operations are correctly done */
  return 0;//AUDIO_OK;
}


/**
  * @brief  Stops audio recording.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_IN_Stop(void)
{
//  uint32_t ret = AUDIO_ERROR;
//
//  /* Call the Media layer pause function */
//  HAL_I2S_DMAStop(&hAudioInI2s);

  /* Return AUDIO_OK when all operations are correctly done */
//  ret = AUDIO_OK;

  return 0;//AUDIO_OK;
}


/**
  * @brief  Starts audio recording.
  * @param  pbuf: Main buffer pointer for the recorded data storing
  * @param  size: Current size of the recorded buffer
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_IN_Record(uint16_t* pbuf, uint32_t size)
{
//  uint32_t ret = AUDIO_ERROR;
//
//  /* Start the process receive DMA */
//  HAL_I2S_Receive_DMA(&hAudioInI2s, pbuf, size);
//
//  /* Return AUDIO_OK when all operations are correctly done */
//  ret = AUDIO_OK;

  return 0;//ret;
}

/**
  * @brief  Retrive the audio frequency.
  * @retval AudioFreq: Audio frequency used to play the audio stream.
  * @note   This API should be called after the BSP_AUDIO_OUT_Init() to adjust the
  *         audio frequency.
  */
uint32_t BSP_AUDIO_OUT_GetFrequency(uint32_t AudioFreq)
{
  return 8000;//bsp_audio_out_frequency;
}

static btstack_packet_callback_registration_t hci_event_callback_registration;
static btstack_tlv_flash_bank_t btstack_tlv_flash_bank_context;
//static hal_flash_bank_stm32_t   hal_flash_bank_context;

//// hal_time_ms.h
//#include "hal_time_ms.h"
//uint32_t hal_time_ms(void){
//    return HAL_GetTick();
//}

// hal_cpu.h implementation
#include "hal_cpu.h"

void hal_cpu_disable_irqs(void){
    __disable_irq();
}

void hal_cpu_enable_irqs(void){
    __enable_irq();
}

void hal_cpu_enable_irqs_and_sleep(void){
    __enable_irq();
#if 0
    // temp disable until effect on RTT is clear
    // go to sleep if event flag isn't set. if set, just clear it. IRQs set event flag
    //  __asm__("wfe");
#endif
}

//#define HAL_FLASH_BANK_SIZE (128 * 1024)
//#define HAL_FLASH_BANK_0_ADDR 0x080C0000
//#define HAL_FLASH_BANK_1_ADDR 0x080E0000
//#define HAL_FLASH_BANK_0_SECTOR FLASH_SECTOR_10
//#define HAL_FLASH_BANK_1_SECTOR FLASH_SECTOR_11

int btstack_main(int argc, char ** argv);

// main.c
static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(size);
    UNUSED(channel);
    bd_addr_t local_addr;
	bd_addr_t address;
    if (packet_type != HCI_EVENT_PACKET) return;
    switch(hci_event_packet_get_type(packet)){
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) return;
        gap_local_bd_addr(local_addr);
        PRINTF("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));
        break;
    case HCI_EVENT_PIN_CODE_REQUEST:
        PRINTF("Pin code request - using '0000'\n");
        hci_event_pin_code_request_get_bd_addr(packet, address);
        gap_pin_code_response(address, "0000");
        break;
    case GAP_EVENT_PAIRING_COMPLETE:
        PRINTF("Paired!\n");
        break;
    case HCI_EVENT_USER_CONFIRMATION_REQUEST:
        // ssp: inform about user confirmation request
        PRINTF("User Confirmation Request with numeric value '%06" PRIu32 "'\n", little_endian_read_32(packet, 8));
        PRINTF("User Confirmation Auto accept\n");
        break;
	default:
		break;
    }
}

void btstack_assert_failed(const char * file, uint16_t line_nr){
    PRINTF("ASSERT in %s, line %u failed - HALT\n", file, line_nr);
    while(1);
}

static uint32_t hal_fram_get_size(void * context){
//	hal_flash_bank_t * self = (hal_flash_bank_t *) context;
	unsigned tlvbase;
	const unsigned tlvsize = nvram_tlv_getparam(& tlvbase);
	return tlvsize;;
}

static uint32_t hal_fram_get_alignment(void * context){
    UNUSED(context);
    return 1;
}

static void hal_fram_erase(void * context, int bank){
//	hal_flash_bank_t * self = (hal_flash_bank_t *) context;
	if (bank > 1) return;
//	PRINTF("hal_fram_erase: bank=%u\n", (unsigned) bank);
//	FLASH_EraseInitTypeDef eraseInit;
//	eraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
//	eraseInit.Sector = self->sectors[bank];
//	eraseInit.NbSectors = 1;
//	eraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_1;	// safe value
//	uint32_t sectorError;
//	HAL_FLASH_Unlock();
//	HAL_FLASHEx_Erase(&eraseInit, &sectorError);
//	HAL_FLASH_Lock();
}

static void hal_fram_read(void * context, int bank, uint32_t offset, uint8_t * buffer, uint32_t size){
//	hal_flash_bank_t * self = (hal_flash_bank_t *) context;
//
//	if (bank > 1) return;
//	if (offset > self->sector_size) return;
//	if ((offset + size) > self->sector_size) return;
//
//	memcpy(buffer, ((uint8_t *) self->banks[bank]) + offset, size);
//	memset(buffer, 0, size);
	//PRINTF("hal_fram_read: bank=%u, off=%u, size=%u\n", (unsigned) bank, (unsigned) offset, (unsigned) size);
	unsigned tlvbase;
	const unsigned tlvsize = nvram_tlv_getparam(& tlvbase);
	ASSERT(offset + size <= tlvsize);
	nvram_read(tlvbase + offset, buffer, size);
}

static void hal_fram_write(void * context, int bank, uint32_t offset, const uint8_t * data, uint32_t size){
//	hal_flash_bank_t * self = (hal_flash_bank_t *) context;

//	if (bank > 1) return;
//	if (offset > self->sector_size) return;
//	if ((offset + size) > self->sector_size) return;
//
//	unsigned int i;
//	HAL_FLASH_Unlock();
//	for (i=0;i<size;i++){
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, self->banks[bank] + offset +i, data[i]);
//	}
//	HAL_FLASH_Lock();
	//printhex(0, data, size);
	//PRINTF("hal_fram_write: bank=%u, off=%u, size=%u\n", (unsigned) bank, (unsigned) offset, (unsigned) size);
	unsigned tlvbase;
	const unsigned tlvsize = nvram_tlv_getparam(& tlvbase);
	ASSERT(offset + size <= tlvsize);
	nvram_write(tlvbase + offset, data, size);
}

static const hal_flash_bank_t hal_fram_bank_impl = {
	/* uint32_t (*get_size)() */         &hal_fram_get_size,
	/* uint32_t (*get_alignment)(..); */ &hal_fram_get_alignment,
	/* void (*erase)(..);             */ &hal_fram_erase,
	/* void (*read)(..);              */ &hal_fram_read,
	/* void (*write)(..);             */ &hal_fram_write,
};

//////////////////////////////////////////////

static int source_active;
static int sink_active;


static int btstack_audio_storch_sink_init(
    uint8_t channels,
    uint32_t samplerate,
    void (*playback)(int16_t * buffer, uint16_t num_samples)
){
	PRINTF("%s:\n", __func__);
    btstack_assert(playback != NULL);
    btstack_assert(channels != 0);

#ifdef HAVE_HAL_AUDIO_SINK_STEREO_ONLY
    // always use stereo from hal, duplicate samples if needed
    output_duplicate_samples = channels != 2;
    channels = 2;
#endif

//    playback_callback  = playback;
//    sink_samplerate = samplerate;
//    hal_audio_sink_init(channels, samplerate, &btstack_audio_audio_played);

    return 0;
}

static uint32_t btstack_audio_storch_sink_get_samplerate(void) {
    return 48000;//BSP_AUDIO_OUT_GetFrequency();
}

static uint32_t source_samplerate = 0;

static int btstack_audio_storch_source_init(
    uint8_t channels,
    uint32_t samplerate,
    void (*recording)(const int16_t * buffer, uint16_t num_samples)
){
	PRINTF("%s:\n", __func__);
    if (!recording){
        PRINTF("No recording callback\n");
        return 1;
    }

//    recording_callback  = recording;
//    source_samplerate = samplerate;
//    hal_audio_source_init(channels, samplerate, &btstack_audio_audio_recorded);

    return 0;
}

static uint32_t btstack_audio_storch_source_get_samplerate(void) {
	PRINTF("%s:\n", __func__);
    return source_samplerate;
}

static void btstack_audio_storch_sink_set_volume(uint8_t volume){
    UNUSED(volume);
	PRINTF("%s:\n", __func__);
}

static void btstack_audio_storch_source_set_gain(uint8_t gain){
    UNUSED(gain);
	PRINTF("%s:\n", __func__);
}

static void btstack_audio_storch_sink_start_stream(void){
	PRINTF("%s:\n", __func__);
//    output_buffer_count   = hal_audio_sink_get_num_output_buffers();
//    output_buffer_samples = hal_audio_sink_get_num_output_buffer_samples();
//
//    btstack_assert(output_buffer_samples > 0);
//
//    // pre-fill HAL buffers
//    uint16_t i;
//    for (i=0;i<output_buffer_count;i++){
//        int16_t * buffer = hal_audio_sink_get_output_buffer(i);
//        (*playback_callback)(buffer, output_buffer_samples);
//    }
//
//    output_buffer_to_play = 0;
//    output_buffer_to_fill = 0;
//
//    // start playback
//    hal_audio_sink_start();
//
//    // start timer
//    btstack_run_loop_set_timer_handler(&driver_timer_sink, &driver_timer_handler_sink);
//    btstack_run_loop_set_timer(&driver_timer_sink, DRIVER_POLL_INTERVAL_MS);
//    btstack_run_loop_add_timer(&driver_timer_sink);

    // state
    sink_active = 1;
}

static void btstack_audio_storch_source_start_stream(void){
    // just started, no data ready
	PRINTF("%s:\n", __func__);
//    input_buffer_ready = 0;
//
//    // start recording
//    hal_audio_source_start();
//
//    // start timer
//    btstack_run_loop_set_timer_handler(&driver_timer_source, &driver_timer_handler_source);
//    btstack_run_loop_set_timer(&driver_timer_source, DRIVER_POLL_INTERVAL_MS);
//    btstack_run_loop_add_timer(&driver_timer_source);

    // state
    source_active = 1;
}

static void btstack_audio_storch_sink_stop_stream(void){
    // stop stream
	PRINTF("%s:\n", __func__);
//    hal_audio_sink_stop();
//    // stop timer
//    btstack_run_loop_remove_timer(&driver_timer_sink);
    // state
    sink_active = 0;
}

static void btstack_audio_storch_source_stop_stream(void){
    // stop stream
	PRINTF("%s:\n", __func__);
//    hal_audio_source_stop();
//    // stop timer
//    btstack_run_loop_remove_timer(&driver_timer_source);
    // state
    source_active = 0;
}

static void btstack_audio_storch_sink_close(void){
    // stop stream if needed
    if (sink_active){
        btstack_audio_storch_sink_stop_stream();
    }
    // close HAL
//    hal_audio_sink_close();
}

static void btstack_audio_storch_source_close(void){
    // stop stream if needed
	PRINTF("%s:\n", __func__);
    if (source_active){
        btstack_audio_storch_source_stop_stream();
    }
    // close HAL
//    hal_audio_source_close();
}

static const btstack_audio_sink_t btstack_audio_storch_sink = {
        .init           = &btstack_audio_storch_sink_init,
        .get_samplerate = &btstack_audio_storch_sink_get_samplerate,
        .set_volume     = &btstack_audio_storch_sink_set_volume,
        .start_stream   = &btstack_audio_storch_sink_start_stream,
        .stop_stream    = &btstack_audio_storch_sink_stop_stream,
        .close          = &btstack_audio_storch_sink_close
};

static const btstack_audio_source_t btstack_audio_storch_source = {
        .init           = &btstack_audio_storch_source_init,
        .get_samplerate = &btstack_audio_storch_source_get_samplerate,
        .set_gain       = &btstack_audio_storch_source_set_gain,
        .start_stream   = &btstack_audio_storch_source_start_stream,
        .stop_stream    = &btstack_audio_storch_source_stop_stream,
        .close          = &btstack_audio_storch_source_close
};

const btstack_audio_sink_t * btstack_audio_storch_sink_get_instance(void){
    return &btstack_audio_storch_sink;
}

const btstack_audio_source_t * btstack_audio_storch_source_get_instance(void){
    return &btstack_audio_storch_source;
}


void tuh_bth_mount_cb(uint8_t idx)
{
//	PRINTF("tuh_bth_mount_cb: idx=%u\n", idx);
    // start with BTstack init - especially configure HCI Transport
    btstack_memory_init();
    btstack_run_loop_init(btstack_run_loop_embedded_get_instance());


    //PRINTF("BTstack on STM32 F4 Discovery with USB support starting...\n");


    // uncomment to enable packet logger
#ifdef ENABLE_SEGGER_RTT
    // hci_dump_init(hci_dump_segger_rtt_stdout_get_instance());
#else
    // hci_dump_init(hci_dump_embedded_stdout_get_instance());
#endif

    // init HCI
#if WITHTINYUSB
    hci_init(hci_transport_h2_tinyusb_instance(idx), NULL);
#else /* WITHTINYUSB */
    hci_init(hci_transport_h2_stm32_instance(), NULL);
#endif /* WITHTINYUSB */

    // setup TLV Flash Sector implementation
//    const hal_flash_bank_t * hal_flash_bank_impl = hal_flash_bank_stm32_init_instance(
//            &hal_flash_bank_context,
//            0,//HAL_FLASH_BANK_SIZE,
//			0,//HAL_FLASH_BANK_0_SECTOR,
//			0,//HAL_FLASH_BANK_1_SECTOR,
//			0,//HAL_FLASH_BANK_0_ADDR,
//			0//HAL_FLASH_BANK_1_ADDR);
    const btstack_tlv_t * btstack_tlv_impl = btstack_tlv_flash_bank_init_instance(
            &btstack_tlv_flash_bank_context,
			& hal_fram_bank_impl,
            NULL);

    // setup global tlv
    btstack_tlv_set_instance(btstack_tlv_impl, &btstack_tlv_flash_bank_context);

    // setup Link Key DB using TLV
    const btstack_link_key_db_t * btstack_link_key_db = btstack_link_key_db_tlv_get_instance(btstack_tlv_impl, &btstack_tlv_flash_bank_context);
    hci_set_link_key_db(btstack_link_key_db);

    // setup LE Device DB using TLV
    ////le_device_db_tlv_configure(btstack_tlv_impl, &btstack_tlv_flash_bank_context);

    // setup audio
	btstack_audio_sink_set_instance(btstack_audio_storch_sink_get_instance());
	btstack_audio_source_set_instance(btstack_audio_storch_source_get_instance());

//    // inform about BTstack state
//    hci_event_callback_registration.callback = &packet_handler;
//    hci_add_event_handler(&hci_event_callback_registration);
//
//    sdp_init();		// везде в примерах убрать
//    l2cap_init();	// везде в примерах убрать
//    rfcomm_init();	// везде в примерах убрать
//    // Init profiles
//    a2dp_sink_init();
//    avrcp_init();
//    avrcp_controller_init();
//    avrcp_target_init();

    // hand over to btstack embedded code
    //VERIFY(! spp_counter_btstack_main(0, NULL));
    //VERIFY(! a2dp_source_btstack_main(0, NULL));
    //VERIFY(! a2dp_sink_btstack_main(0, NULL));
    VERIFY(! hfp_hf_btstack_main(0, NULL));
    //VERIFY(! hsp_hs_btstack_main(0, NULL));

    //gap_set_local_name(WITHBRANDSTR " TRX 00:00:00:00:00:00");
    gap_set_local_name(WITHBRANDSTR " BTx");
    gap_discoverable_control(1);
//    //gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);
//    gap_ssp_set_io_capability(SSP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
//    // turn on!
    hci_power_control(HCI_POWER_ON);

    // go
    //btstack_run_loop_execute();
}


void tuh_bth_umount_cb(uint8_t idx)
{
	//PRINTF("tuh_bth_umount_cb: idx=%u\n", idx);
    //hci_power_control(HCI_POWER_OFF);
	hci_remove_event_handler(&hci_event_callback_registration);
	hci_deinit();
//	btstack_run_loop_deinit();
//	btstack_memory_deinit();
	ASSERT(0);
}

/* Bluetooth initialize */
void bt_initialize(void)
{
#if ! WITHTINYUSB
	PRINTF("bt_initialize start\n");
	tuh_bth_mount_cb(1);
	PRINTF("bt_initialize done\n");
#endif
}

/* Bluetooth enable */
void bt_enable(uint_fast8_t v)
{

}


#else  /* WITHUSEUSBBT */

void bt_initialize(void)
{

}

void bt_enable(uint_fast8_t v)
{

}

#endif /* WITHUSEUSBBT */
