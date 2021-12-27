#ifndef FT8_H_
#define FT8_H_

#include "hardware.h"

#if WITHFT8

#include "audio.h"

enum {
	FT8_MSG_TIME_UPDATED = 1,
	FT8_MSG_TOGGLE,
	FT8_MSG_START_FILL_1,
	FT8_MSG_START_FILL_2,
	FT8_MSG_DECODE_1,
	FT8_MSG_DECODE_2,
	FT8_MSG_DECODE_DONE
};

enum {
	ft8_sample_rate = ARMI2SRATE,
	ft8_length = 15,
	ft8_text_length = 40,
	ft8_text_records = 50,
	ft8_interrupt_decode1 = 10,
	ft8_interrupt_decode2 = 11
};

typedef struct {
	float rx_buf1 [ft8_sample_rate * ft8_length];
	float rx_buf2 [ft8_sample_rate * ft8_length];
	char rx_text [ft8_text_records][ft8_text_length];
	uint8_t decoded_messages;
} ft8_t;

extern ft8_t ft8;

void ft8_decode(float * signal);

#endif /* WITHFT8 */
#endif /* FT8_H_ */
