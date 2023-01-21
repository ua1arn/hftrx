#ifndef FT8_H_
#define FT8_H_

#include "hardware.h"

#if WITHFT8

#include "audio.h"

enum {
	FT8_MSG_TIME_UPDATED = 1,
	FT8_MSG_DISABLE,
	FT8_MSG_ENABLE,
	FT8_MSG_START_FILL,
	FT8_MSG_DECODE_1,
	FT8_MSG_DECODE_2,
	FT8_MSG_DECODE_DONE,
	FT8_MSG_ENCODE,
	FT8_MSG_ENCODE_DONE,
	FT8_MSG_TX_DONE,
};

enum {
	ft8_sample_rate = ARMI2SRATE,
	ft8_length = 15,
	ft8_text_length = 40,
	ft8_text_records = 50,
#if ! LINUX_SUBSYSTEM
	ft8_interrupt_core0 = SGI10_IRQn,
	ft8_interrupt_core1 = SGI11_IRQn,
#endif /* ! LINUX_SUBSYSTEM */
};

typedef struct {
	float rx_buf1 [ft8_sample_rate * ft8_length];
	float rx_buf2 [ft8_sample_rate * ft8_length];
	uint8_t int_core0;
	uint8_t int_core1;
	char rx_text [ft8_text_records][ft8_text_length];
	uint8_t decoded_messages;
	char tx_text [ft8_text_length];
	float tx_freq;
	float tx_buf [ft8_sample_rate * ft8_length];
} ft8_t;

typedef struct {
	uint_fast8_t hour;
	uint_fast8_t minute;
	uint_fast8_t second;
} timestamp_t;

extern ft8_t ft8;

void ft8_decode_buf(float * signal, timestamp_t ts);
void ft8_fill1(float sample);
void ft8_fill2(float sample);
void ft8_txfill(float * sample);
void ft8_start_fill(void);
void ft8_stop_fill(void);
void ft8_set_state(uint8_t v);
uint8_t get_ft8_state(void);
void ft8_tx_enable(void);
void ft8_initialize(void);
void ft8_walkthrough_core0(uint_fast8_t rtc_secounds);

void xcz_ipi_sendmsg_c0(uint8_t msg);
void xcz_ipi_sendmsg_c1(uint8_t msg);

void hamradio_gui_parse_ft8buf(void);

#endif /* WITHFT8 */
#endif /* FT8_H_ */
