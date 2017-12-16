/* $Id$ */
// ѕроект HF Dream Receiver ( ¬ приЄмник мечты)
// автор √ена «авидовский mgs2001@mail.ru
// UA1ARN
//



#ifndef R820T_H_INCLUDED
#define R820T_H_INCLUDED

// вз€то с 
// https://github.com/airspy/airspyone_firmware/blob/master/common/r820t.c
//
/*
 * Rafael Micro R820T driver for AIRSPY
 *
 * Copyright 2013 Youssef Touil <youssef@airspy.com>
 * Copyright 2014-2016 Benjamin Vernoux <bvernoux@airspy.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//#include <stdio.h>
#include <stdint.h>
#include <string.h>

//#include "airspy_commands.h"

#define R820T_I2C_ADDR 0x34	//(0x1A << 1)

#define I2C_READ 0x01	// 0x35 as result
#define I2C_WRITE 0x00	// 0x34 as result

#define REG_SHADOW_START 5
#define NUM_REGS 30

/* R820T Clock */
#define CALIBRATION_LO 88000

typedef struct
{
  uint32_t xtal_freq; /* XTAL_FREQ_HZ */
  uint32_t freq;
  uint32_t if_freq;
  uint8_t regs[NUM_REGS];
  uint16_t padding;
  //uint8_t has_lock;
} r820t_priv_t;

void airspy_r820t_write_single(r820t_priv_t *priv, uint8_t reg, uint8_t val);
uint8_t airspy_r820t_read_single(r820t_priv_t *priv, uint8_t reg);

void r820t_startup(r820t_priv_t *priv);
int r820t_init(r820t_priv_t *priv, const uint32_t if_freq);
int r820t_set_freq(r820t_priv_t *priv, uint32_t freq);
int r820t_set_lna_gain(r820t_priv_t *priv, uint8_t gain_index);
int r820t_set_mixer_gain(r820t_priv_t *priv, uint8_t gain_index);
int r820t_set_vga_gain(r820t_priv_t *priv, uint8_t gain_index);
int r820t_set_vga_voltagecontrol(r820t_priv_t *priv);
int r820t_set_lna_agc(r820t_priv_t *priv, uint8_t value);
int r820t_set_mixer_agc(r820t_priv_t *priv, uint8_t value);
void r820t_set_if_bandwidth(r820t_priv_t *priv, uint8_t bw);
int r820t_standby(void);

static int r820t_read_cache_reg(r820t_priv_t *priv, int reg);

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Tuner frequency ranges */
struct r820t_freq_range
{
  uint8_t open_d;
  uint8_t rf_mux_ploy;
  uint8_t tf_c;
};

#define R820T_READ_MAX_DATA 32
#define R820T_INIT_NB_REGS (32-5)
uint8_t r820t_read_data[R820T_READ_MAX_DATA]; /* Buffer for data read from I2C */
uint8_t r820t_state_standby = 1; /* 1=standby/power off 0=r820t initialized/power on */


/* initial freq @ 128MHz -> ~5MHz IF due to xtal mismatch */
static const uint8_t xxr82xx_init_array[NUM_REGS] =
{
	/* 05 */ 0x90, // 0x90 LNA manual gain mode, init to 0
	/* 06 */ 0x80,
	/* 07 */ 0x60,
	/* 08 */ 0x80, // Image Gain Adjustment
	/* 09 */ 0x40, //  40 Image Phase Adjustment
	/* 0A */ 0xA0, //  A8 Channel filter [0..3]: 0 = widest, f = narrowest - Optimal. Don't touch!
	/* 0B */ 0x6F, //  0F High pass filter - Optimal. Don't touch!
	/* 0C */ 0x40, // 0x48 0x40 VGA control by code, init at 0
	/* 0D */ 0x63, // LNA AGC settings: [0..3]: Lower threshold; [4..7]: High threshold
	/* 0E */ 0x75,
	/* 0F */ 0xF8, // F8 Filter Widest, LDO_5V OFF, clk out OFF,
	/* 10 */ 0x7C,
	/* 11 */ 0x83,
	/* 12 */ 0x80, 
	/* 13 */ 0x00,
	/* 14 */ 0x0F,
	/* 15 */ 0x00,
	/* 16 */ 0xC0,
	/* 17 */ 0x30,
	/* 18 */ 0x48,
	/* 19 */ 0xCC,
	/* 1A */ 0x62, //0x60
	/* 1B */ 0x00,
	/* 1C */ 0x54,
	/* 1D */ 0xAE,
	/* 1E */ 0x0A,
	/* 1F */ 0xC0
};

// http://elixir.free-electrons.com/linux/v3.10.107/source/drivers/media/tuners/r820t.c
/* Those initial values start from REG_SHADOW_START */
static const uint8_t r82xx_init_array[NUM_REGS] = {
	0x83, 0x30, 0x65,			/* 05 to 07 */
	0x80, 0x00, 0x96, 0x6c,			/* 08 to 0b */
	0xf5, 0x63, 0x75, 0x68,			/* 0c to 0f */
	0x6c, 0x83, 0x80, 0x00,			/* 10 to 13 */
	0x0f, 0x00, 0xc0, 0x30,			/* 14 to 17 */
	0x48, 0xcc, 0x60, 0x00,			/* 18 to 1b */
	0x54, 0xae, 0x4a, 0xc0			/* 1c to 1f */
};

/* Tuner frequency ranges
"Copyright (C) 2013 Mauro Carvalho Chehab"
https://stuff.mit.edu/afs/sipb/contrib/linux/drivers/media/tuners/r820t.c
part of freq_ranges()
*/
static const struct r820t_freq_range freq_ranges[] =
{
  {
  /* 0 MHz */
  /* .open_d = */     0x08, /* low */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0xdf, /* R27[7:0]  band2,band0 */
  }, {
  /* 50 MHz */
  /* .open_d = */     0x08, /* low */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0xbe, /* R27[7:0]  band4,band1  */
  }, {
  /* 55 MHz */
  /* .open_d = */     0x08, /* low */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x8b, /* R27[7:0]  band7,band4 */
  }, {
  /* 60 MHz */
  /* .open_d = */     0x08, /* low */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x7b, /* R27[7:0]  band8,band4 */
  }, {
  /* 65 MHz */
  /* .open_d = */     0x08, /* low */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x69, /* R27[7:0]  band9,band6 */
  }, {
  /* 70 MHz */
  /* .open_d = */     0x08, /* low */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x58, /* R27[7:0]  band10,band7 */
  }, {
  /* 75 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x44, /* R27[7:0]  band11,band11 */
  }, {
  /* 80 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x44, /* R27[7:0]  band11,band11 */
  }, {
  /* 90 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x34, /* R27[7:0]  band12,band11 */
  }, {
  /* 100 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x34, /* R27[7:0]  band12,band11 */
  }, {
  /* 110 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x24, /* R27[7:0]  band13,band11 */
  }, {
  /* 120 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x24, /* R27[7:0]  band13,band11 */
  }, {
  /* 140 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x14, /* R27[7:0]  band14,band11 */
  }, {
  /* 180 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x13, /* R27[7:0]  band14,band12 */
  }, {
  /* 220 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x13, /* R27[7:0]  band14,band12 */
  }, {
  /* 250 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x11, /* R27[7:0]  highest,highest */
  }, {
  /* 280 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x02, /* R26[7:6]=0 (LPF)  R26[1:0]=2 (low) */
  /* .tf_c = */     0x00, /* R27[7:0]  highest,highest */
  }, {
  /* 310 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x41, /* R26[7:6]=1 (bypass)  R26[1:0]=1 (middle) */
  /* .tf_c = */     0x00, /* R27[7:0]  highest,highest */
  }, {
  /* 450 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x41, /* R26[7:6]=1 (bypass)  R26[1:0]=1 (middle) */
  /* .tf_c = */     0x00, /* R27[7:0]  highest,highest */
  }, {
  /* 588 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x40, /* R26[7:6]=1 (bypass)  R26[1:0]=0 (highest) */
  /* .tf_c = */     0x00, /* R27[7:0]  highest,highest */
  }, {
  /* 650 MHz */
  /* .open_d = */     0x00, /* high */
  /* .rf_mux_ploy = */  0x40, /* R26[7:6]=1 (bypass)  R26[1:0]=0 (highest) */
  /* .tf_c = */     0x00, /* R27[7:0]  highest,highest */
  }
};

#define FREQ_TO_IDX_SIZE (600)
const uint8_t freq_to_idx[FREQ_TO_IDX_SIZE]=
{
  /* 50 */ 1,/* 51 */ 1,/* 52 */ 1,/* 53 */ 1,/* 54 */ 1,
  /* 55 */ 2,/* 56 */ 2,/* 57 */ 2,/* 58 */ 2,/* 59 */ 2,
  /* 60 */ 3,/* 61 */ 3,/* 62 */ 3,/* 63 */ 3,/* 64 */ 3,
  /* 65 */ 4,/* 66 */ 4,/* 67 */ 4,/* 68 */ 4,/* 69 */ 4,
  /* 70 */ 5,/* 71 */ 5,/* 72 */ 5,/* 73 */ 5,/* 74 */ 5,
  /* 75 */ 6,/* 76 */ 6,/* 77 */ 6,/* 78 */ 6,/* 79 */ 6,
  /* 80 */ 7,/* 81 */ 7,/* 82 */ 7,/* 83 */ 7,/* 84 */ 7,/* 85 */ 7,/* 86 */ 7,/* 87 */ 7,/* 88 */ 7,/* 89 */ 7,
  /* 90 */ 8,/* 91 */ 8,/* 92 */ 8,/* 93 */ 8,/* 94 */ 8,/* 95 */ 8,/* 96 */ 8,/* 97 */ 8,/* 98 */ 8,/* 99 */ 8,
  /* 100 */ 9,/* 101 */ 9,/* 102 */ 9,/* 103 */ 9,/* 104 */ 9,/* 105 */ 9,/* 106 */ 9,/* 107 */ 9,/* 108 */ 9,/* 109 */ 9,
  /* 110 */ 10,/* 111 */ 10,/* 112 */ 10,/* 113 */ 10,/* 114 */ 10,/* 115 */ 10,/* 116 */ 10,/* 117 */ 10,/* 118 */ 10,/* 119 */ 10,
  /* 120 */ 11,/* 121 */ 11,/* 122 */ 11,/* 123 */ 11,/* 124 */ 11,/* 125 */ 11,/* 126 */ 11,/* 127 */ 11,/* 128 */ 11,/* 129 */ 11,
  /* 130 */ 11,/* 131 */ 11,/* 132 */ 11,/* 133 */ 11,/* 134 */ 11,/* 135 */ 11,/* 136 */ 11,/* 137 */ 11,/* 138 */ 11,/* 139 */ 11,
  /* 140 */ 12,/* 141 */ 12,/* 142 */ 12,/* 143 */ 12,/* 144 */ 12,/* 145 */ 12,/* 146 */ 12,/* 147 */ 12,/* 148 */ 12,/* 149 */ 12,
  /* 150 */ 12,/* 151 */ 12,/* 152 */ 12,/* 153 */ 12,/* 154 */ 12,/* 155 */ 12,/* 156 */ 12,/* 157 */ 12,/* 158 */ 12,/* 159 */ 12,
  /* 160 */ 12,/* 161 */ 12,/* 162 */ 12,/* 163 */ 12,/* 164 */ 12,/* 165 */ 12,/* 166 */ 12,/* 167 */ 12,/* 168 */ 12,/* 169 */ 12,
  /* 170 */ 12,/* 171 */ 12,/* 172 */ 12,/* 173 */ 12,/* 174 */ 12,/* 175 */ 12,/* 176 */ 12,/* 177 */ 12,/* 178 */ 12,/* 179 */ 12,
  /* 180 */ 13,/* 181 */ 13,/* 182 */ 13,/* 183 */ 13,/* 184 */ 13,/* 185 */ 13,/* 186 */ 13,/* 187 */ 13,/* 188 */ 13,/* 189 */ 13,
  /* 190 */ 13,/* 191 */ 13,/* 192 */ 13,/* 193 */ 13,/* 194 */ 13,/* 195 */ 13,/* 196 */ 13,/* 197 */ 13,/* 198 */ 13,/* 199 */ 13,
  /* 200 */ 13,/* 201 */ 13,/* 202 */ 13,/* 203 */ 13,/* 204 */ 13,/* 205 */ 13,/* 206 */ 13,/* 207 */ 13,/* 208 */ 13,/* 209 */ 13,
  /* 210 */ 13,/* 211 */ 13,/* 212 */ 13,/* 213 */ 13,/* 214 */ 13,/* 215 */ 13,/* 216 */ 13,/* 217 */ 13,/* 218 */ 13,/* 219 */ 13,
  /* 220 */ 14,/* 221 */ 14,/* 222 */ 14,/* 223 */ 14,/* 224 */ 14,/* 225 */ 14,/* 226 */ 14,/* 227 */ 14,/* 228 */ 14,/* 229 */ 14,
  /* 230 */ 14,/* 231 */ 14,/* 232 */ 14,/* 233 */ 14,/* 234 */ 14,/* 235 */ 14,/* 236 */ 14,/* 237 */ 14,/* 238 */ 14,/* 239 */ 14,
  /* 240 */ 14,/* 241 */ 14,/* 242 */ 14,/* 243 */ 14,/* 244 */ 14,/* 245 */ 14,/* 246 */ 14,/* 247 */ 14,/* 248 */ 14,/* 249 */ 14,
  /* 250 */ 15,/* 251 */ 15,/* 252 */ 15,/* 253 */ 15,/* 254 */ 15,/* 255 */ 15,/* 256 */ 15,/* 257 */ 15,/* 258 */ 15,/* 259 */ 15,
  /* 260 */ 15,/* 261 */ 15,/* 262 */ 15,/* 263 */ 15,/* 264 */ 15,/* 265 */ 15,/* 266 */ 15,/* 267 */ 15,/* 268 */ 15,/* 269 */ 15,
  /* 270 */ 15,/* 271 */ 15,/* 272 */ 15,/* 273 */ 15,/* 274 */ 15,/* 275 */ 15,/* 276 */ 15,/* 277 */ 15,/* 278 */ 15,/* 279 */ 15,
  /* 280 */ 16,/* 281 */ 16,/* 282 */ 16,/* 283 */ 16,/* 284 */ 16,/* 285 */ 16,/* 286 */ 16,/* 287 */ 16,/* 288 */ 16,/* 289 */ 16,
  /* 290 */ 16,/* 291 */ 16,/* 292 */ 16,/* 293 */ 16,/* 294 */ 16,/* 295 */ 16,/* 296 */ 16,/* 297 */ 16,/* 298 */ 16,/* 299 */ 16,
  /* 300 */ 16,/* 301 */ 16,/* 302 */ 16,/* 303 */ 16,/* 304 */ 16,/* 305 */ 16,/* 306 */ 16,/* 307 */ 16,/* 308 */ 16,/* 309 */ 16,
  /* 310 */ 17,/* 311 */ 17,/* 312 */ 17,/* 313 */ 17,/* 314 */ 17,/* 315 */ 17,/* 316 */ 17,/* 317 */ 17,/* 318 */ 17,/* 319 */ 17,
  /* 320 */ 17,/* 321 */ 17,/* 322 */ 17,/* 323 */ 17,/* 324 */ 17,/* 325 */ 17,/* 326 */ 17,/* 327 */ 17,/* 328 */ 17,/* 329 */ 17,
  /* 330 */ 17,/* 331 */ 17,/* 332 */ 17,/* 333 */ 17,/* 334 */ 17,/* 335 */ 17,/* 336 */ 17,/* 337 */ 17,/* 338 */ 17,/* 339 */ 17,
  /* 340 */ 17,/* 341 */ 17,/* 342 */ 17,/* 343 */ 17,/* 344 */ 17,/* 345 */ 17,/* 346 */ 17,/* 347 */ 17,/* 348 */ 17,/* 349 */ 17,
  /* 350 */ 17,/* 351 */ 17,/* 352 */ 17,/* 353 */ 17,/* 354 */ 17,/* 355 */ 17,/* 356 */ 17,/* 357 */ 17,/* 358 */ 17,/* 359 */ 17,
  /* 360 */ 17,/* 361 */ 17,/* 362 */ 17,/* 363 */ 17,/* 364 */ 17,/* 365 */ 17,/* 366 */ 17,/* 367 */ 17,/* 368 */ 17,/* 369 */ 17,
  /* 370 */ 17,/* 371 */ 17,/* 372 */ 17,/* 373 */ 17,/* 374 */ 17,/* 375 */ 17,/* 376 */ 17,/* 377 */ 17,/* 378 */ 17,/* 379 */ 17,
  /* 380 */ 17,/* 381 */ 17,/* 382 */ 17,/* 383 */ 17,/* 384 */ 17,/* 385 */ 17,/* 386 */ 17,/* 387 */ 17,/* 388 */ 17,/* 389 */ 17,
  /* 390 */ 17,/* 391 */ 17,/* 392 */ 17,/* 393 */ 17,/* 394 */ 17,/* 395 */ 17,/* 396 */ 17,/* 397 */ 17,/* 398 */ 17,/* 399 */ 17,
  /* 400 */ 17,/* 401 */ 17,/* 402 */ 17,/* 403 */ 17,/* 404 */ 17,/* 405 */ 17,/* 406 */ 17,/* 407 */ 17,/* 408 */ 17,/* 409 */ 17,
  /* 410 */ 17,/* 411 */ 17,/* 412 */ 17,/* 413 */ 17,/* 414 */ 17,/* 415 */ 17,/* 416 */ 17,/* 417 */ 17,/* 418 */ 17,/* 419 */ 17,
  /* 420 */ 17,/* 421 */ 17,/* 422 */ 17,/* 423 */ 17,/* 424 */ 17,/* 425 */ 17,/* 426 */ 17,/* 427 */ 17,/* 428 */ 17,/* 429 */ 17,
  /* 430 */ 17,/* 431 */ 17,/* 432 */ 17,/* 433 */ 17,/* 434 */ 17,/* 435 */ 17,/* 436 */ 17,/* 437 */ 17,/* 438 */ 17,/* 439 */ 17,
  /* 440 */ 17,/* 441 */ 17,/* 442 */ 17,/* 443 */ 17,/* 444 */ 17,/* 445 */ 17,/* 446 */ 17,/* 447 */ 17,/* 448 */ 17,/* 449 */ 17,
  /* 450 */ 18,/* 451 */ 18,/* 452 */ 18,/* 453 */ 18,/* 454 */ 18,/* 455 */ 18,/* 456 */ 18,/* 457 */ 18,/* 458 */ 18,/* 459 */ 18,
  /* 460 */ 18,/* 461 */ 18,/* 462 */ 18,/* 463 */ 18,/* 464 */ 18,/* 465 */ 18,/* 466 */ 18,/* 467 */ 18,/* 468 */ 18,/* 469 */ 18,
  /* 470 */ 18,/* 471 */ 18,/* 472 */ 18,/* 473 */ 18,/* 474 */ 18,/* 475 */ 18,/* 476 */ 18,/* 477 */ 18,/* 478 */ 18,/* 479 */ 18,
  /* 480 */ 18,/* 481 */ 18,/* 482 */ 18,/* 483 */ 18,/* 484 */ 18,/* 485 */ 18,/* 486 */ 18,/* 487 */ 18,/* 488 */ 18,/* 489 */ 18,
  /* 490 */ 18,/* 491 */ 18,/* 492 */ 18,/* 493 */ 18,/* 494 */ 18,/* 495 */ 18,/* 496 */ 18,/* 497 */ 18,/* 498 */ 18,/* 499 */ 18,
  /* 500 */ 18,/* 501 */ 18,/* 502 */ 18,/* 503 */ 18,/* 504 */ 18,/* 505 */ 18,/* 506 */ 18,/* 507 */ 18,/* 508 */ 18,/* 509 */ 18,
  /* 510 */ 18,/* 511 */ 18,/* 512 */ 18,/* 513 */ 18,/* 514 */ 18,/* 515 */ 18,/* 516 */ 18,/* 517 */ 18,/* 518 */ 18,/* 519 */ 18,
  /* 520 */ 18,/* 521 */ 18,/* 522 */ 18,/* 523 */ 18,/* 524 */ 18,/* 525 */ 18,/* 526 */ 18,/* 527 */ 18,/* 528 */ 18,/* 529 */ 18,
  /* 530 */ 18,/* 531 */ 18,/* 532 */ 18,/* 533 */ 18,/* 534 */ 18,/* 535 */ 18,/* 536 */ 18,/* 537 */ 18,/* 538 */ 18,/* 539 */ 18,
  /* 540 */ 18,/* 541 */ 18,/* 542 */ 18,/* 543 */ 18,/* 544 */ 18,/* 545 */ 18,/* 546 */ 18,/* 547 */ 18,/* 548 */ 18,/* 549 */ 18,
  /* 550 */ 18,/* 551 */ 18,/* 552 */ 18,/* 553 */ 18,/* 554 */ 18,/* 555 */ 18,/* 556 */ 18,/* 557 */ 18,/* 558 */ 18,/* 559 */ 18,
  /* 560 */ 18,/* 561 */ 18,/* 562 */ 18,/* 563 */ 18,/* 564 */ 18,/* 565 */ 18,/* 566 */ 18,/* 567 */ 18,/* 568 */ 18,/* 569 */ 18,
  /* 570 */ 18,/* 571 */ 18,/* 572 */ 18,/* 573 */ 18,/* 574 */ 18,/* 575 */ 18,/* 576 */ 18,/* 577 */ 18,/* 578 */ 18,/* 579 */ 18,
  /* 580 */ 18,/* 581 */ 18,/* 582 */ 18,/* 583 */ 18,/* 584 */ 18,/* 585 */ 18,/* 586 */ 18,/* 587 */ 18,
  /* 588 */ 19,/* 589 */ 19,/* 590 */ 19,/* 591 */ 19,/* 592 */ 19,/* 593 */ 19,/* 594 */ 19,/* 595 */ 19,/* 596 */ 19,/* 597 */ 19,
  /* 598 */ 19,/* 599 */ 19,/* 600 */ 19,/* 601 */ 19,/* 602 */ 19,/* 603 */ 19,/* 604 */ 19,/* 605 */ 19,/* 606 */ 19,/* 607 */ 19,
  /* 608 */ 19,/* 609 */ 19,/* 610 */ 19,/* 611 */ 19,/* 612 */ 19,/* 613 */ 19,/* 614 */ 19,/* 615 */ 19,/* 616 */ 19,/* 617 */ 19,
  /* 618 */ 19,/* 619 */ 19,/* 620 */ 19,/* 621 */ 19,/* 622 */ 19,/* 623 */ 19,/* 624 */ 19,/* 625 */ 19,/* 626 */ 19,/* 627 */ 19,
  /* 628 */ 19,/* 629 */ 19,/* 630 */ 19,/* 631 */ 19,/* 632 */ 19,/* 633 */ 19,/* 634 */ 19,/* 635 */ 19,/* 636 */ 19,/* 637 */ 19,
  /* 638 */ 19,/* 639 */ 19,/* 640 */ 19,/* 641 */ 19,/* 642 */ 19,/* 643 */ 19,/* 644 */ 19,/* 645 */ 19,/* 646 */ 19,/* 647 */ 19,
  /* 648 */ 19,/* 649 */ 19
};

#define FREQ_50MHZ (50)
#define FREQ_TO_IDX_0_TO_49MHZ (0)
#define FREQ_TO_IDX_650_TO_1800MHZ (20)

int r820t_freq_get_idx(uint32_t freq_mhz)
{
  uint32_t freq_mhz_fix;

  if(freq_mhz < FREQ_50MHZ)
  {
    /* Frequency Less than 50MHz */
    return FREQ_TO_IDX_0_TO_49MHZ;
  }else
  {
    /* Frequency Between 50 to 649MHz use table */
    /* Fix the frequency for the table */
    freq_mhz_fix = freq_mhz - FREQ_50MHZ;
    if(freq_mhz_fix < FREQ_TO_IDX_SIZE)
    {

      return freq_to_idx[freq_mhz_fix];
    }else
    {
      /* Frequency Between 650 to 1800MHz */
      return FREQ_TO_IDX_650_TO_1800MHZ;
    }
  }
}

#if 1

static bool r820t_is_power_enabled(void)
{
  uint8_t value;
  value = 1;
  if(value == 1)
  {
    if(r820t_state_standby == 0)
    {
      return true;
    } else
    {
      return false;
    }
  } else
  {
    return false;
  }
}

#else

__attribute__ ((always_inline)) static inline bool r820t_is_power_enabled(void)
{
  uint32_t port_num;
  uint32_t pin_num;
  uint8_t value;

  port_num = PORT_EN_R820T;
  pin_num = PIN_EN_R820T;

  /* GPIO DIR is set to OUT read the GPIO_SET reg */
  value = ((GPIO_SET(port_num) & pin_num) != 0);
  if(value == 1)
  {
    if(r820t_state_standby == 0)
    {
      return true;
    } else
    {
      return false;
    }
  } else
  {
    return false;
  }
}

#endif

/*
 * Write regs 5 to 32 (R820T_INIT_NB_REGS values) using data parameter and write last reg to 0
 */
void airspy_r820t_write_init(const uint8_t* data)
{
  uint_fast8_t i;

  if(r820t_is_power_enabled() == true)
  {
    //i2c1_tx_start();
    i2c_start(R820T_I2C_ADDR | I2C_WRITE);
    i2c_write(REG_SHADOW_START); /* Start reg */

    for (i = 0; i < R820T_INIT_NB_REGS; i++)
      i2c_write(data[i]);

    i2c_write(0); /* Set last reg to 0 (errata r820t) */

	i2c_waitsend();
	i2c_stop();
  }
}

/*
 * Read from one or more contiguous registers. data[0] should be the first
 * register number, one or more values follow.
 */
 const uint8_t lut[16] = { 0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
      0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf };

static uint8_t r82xx_bitrev(uint8_t byte)
{
 return (lut[byte & 0xf] << 4) | lut[byte >> 4];
}

void airspy_r820t_read(r820t_priv_t *priv, uint8_t* const data, const uint8_t data_count)
{
  int i;
  uint_fast8_t val;

  if(r820t_is_power_enabled() == true)
  {
    /* read the value */
    //i2c1_tx_start();
    i2c_start(R820T_I2C_ADDR | I2C_READ);

    if(data_count > 1)
    {
      for(i = 0; i < (data_count-1); i++)
      {
		i2c_read(& val, i == 0 ? I2C_READ_ACK_1 : I2C_READ_ACK);	/* чтение промежуточного байта ответа */
		//val = i2c1_rx_byte(1); /* ACK each byte */
		data[i] = r82xx_bitrev(val); /* Swap data bits as they are received LSB to MSB */
      }

 		i2c_read(& val, I2C_READ_NACK);	/* чтение последнего байта ответа */
     //val = i2c1_rx_byte(0); /* NACK last byte */
      data[i] = r82xx_bitrev(val); /* Swap data bits as they are received LSB to MSB */

    }
	else
    {
		i2c_read(& val, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
      //val = i2c1_rx_byte(0); /* NACK last byte */
      data[0] = r82xx_bitrev(val); /* Swap data bits as they are received LSB to MSB */
    }

	//i2c_waitsend();
	i2c_stop();
  }else
  {
    /* Data cannot be read when R820T is OFF, just read cache */
    for(i=0; i<data_count; i++)
    {
      data[i] = r820t_read_cache_reg(priv, i);
    }
  }
  
}

/* write to single register (return 0 if success) */
void airspy_r820t_write_single(r820t_priv_t *priv, uint8_t reg, uint8_t val)
{
  if(r820t_is_power_enabled() == true)
  {
    //i2c1_tx_start();
    i2c_start(R820T_I2C_ADDR | I2C_WRITE);
    i2c_write(reg);
    i2c_write(val);
	i2c_waitsend();
	i2c_stop();
  }
  priv->regs[reg - REG_SHADOW_START] = val;
}

/* read single register */
uint8_t airspy_r820t_read_single(r820t_priv_t *priv, uint8_t reg)
{
  uint8_t val;

  /* read the value */
  airspy_r820t_read(priv, r820t_read_data, reg+1);
  val = r820t_read_data[reg];
  return val;
}

static int r820t_write_reg(r820t_priv_t *priv, uint8_t reg, uint8_t val)
{
  if (r820t_read_cache_reg(priv, reg) == val)
    return 0;
  airspy_r820t_write_single(priv, reg, val);
  return 0;
}

static int r820t_read_cache_reg(r820t_priv_t *priv, int reg)
{
  reg -= REG_SHADOW_START;

  if (reg >= 0 && reg < NUM_REGS)
    return priv->regs[reg];
  else
  {
	  TP();
    return -1;
  }
}

static int r820t_write_reg_mask(r820t_priv_t *priv, uint8_t reg, uint8_t val, uint8_t bit_mask)
{
  int rc = r820t_read_cache_reg(priv, reg);

  if (rc < 0)
    return rc;

  val = (rc & ~bit_mask) | (val & bit_mask);

  return r820t_write_reg(priv, reg, val);
}

static int r820t_read(r820t_priv_t *priv, uint8_t *val, int len)
{
  /* reg not used and assumed to be always 0 because start from reg0 to reg0+len */
  airspy_r820t_read(priv, val, len);

  return 0;
}

/*
 * r820t tuning logic
 */
#ifdef OPTIM_SET_MUX
int r820t_set_mux_freq_idx = -1; /* Default set to invalid value in order to force set_mux */
#endif

/*
"inspired by Mauro Carvalho Chehab set_mux technique"
https://stuff.mit.edu/afs/sipb/contrib/linux/drivers/media/tuners/r820t.c
part of r820t_set_mux() (set tracking filter)
*/
static int r820t_set_tf(r820t_priv_t *priv, uint32_t freq)
{
  const struct r820t_freq_range *range;
  int freq_idx;
  int rc = 0;

  /* Get the proper frequency range in MHz instead of Hz */
  /* Fast divide freq by 1000000 */
  freq = (uint32_t)((uint64_t)freq * 4295 >> 32);

  freq_idx = r820t_freq_get_idx(freq);
  range = &freq_ranges[freq_idx];

  /* Only reconfigure mux freq if modified vs previous range */
#ifdef OPTIM_SET_MUX
  if(freq_idx != r820t_set_mux_freq_idx)
  {
#endif
    /* Open Drain */
    rc = r820t_write_reg_mask(priv, 0x17, range->open_d, 0x08);
    if (rc < 0)
      return rc;

    /* RF_MUX,Polymux */
    rc = r820t_write_reg_mask(priv, 0x1a, range->rf_mux_ploy, 0xc3);
    if (rc < 0)
      return rc;

    /* TF BAND */
    rc = r820t_write_reg(priv, 0x1b, range->tf_c);
    if (rc < 0)
      return rc;

    /* XTAL CAP & Drive */
    rc = r820t_write_reg_mask(priv, 0x10, 0x08, 0x0b);
    if (rc < 0)
      return rc;

    rc = r820t_write_reg_mask(priv, 0x08, 0x00, 0x3f);
    if (rc < 0)
      return rc;

    rc = r820t_write_reg_mask(priv, 0x09, 0x00, 0x3f);
#ifdef OPTIM_SET_MUX
  }
  r820t_set_mux_freq_idx = freq_idx;
#endif

  return rc;
}

#define VCO_POWER_REF   0x02

// http://elixir.free-electrons.com/linux/v3.10.52/source/drivers/media/tuners/r820t.c#L539

int r820t_set_pll(r820t_priv_t *priv, uint32_t freq)
{
	debug_printf_P(PSTR("r820t_set_pll: freq=%u Hz (%u kHz)\n"), freq, freq / 1000);

	uint32_t vco_freq;
	int rc, i;
	//unsigned sleep_time = 10000;
	uint32_t vco_fra;		/* VCO contribution by SDM (kHz) */
	uint32_t vco_min  = 1770000;
	uint32_t vco_max  = vco_min * 2;
	uint32_t pll_ref;
	uint16_t n_sdm = 2;
	uint16_t sdm = 0;
	uint8_t mix_div = 2;
	uint8_t div_buf = 0;
	uint8_t div_num = 0;
	uint8_t refdiv2 = 0;
	uint8_t ni, si, nint, vco_fine_tune, val;
	uint8_t data[5];

	/* Frequency in kHz */
	freq = (freq + 500) / 1000;
	pll_ref = (priv->xtal_freq + 500) / 1000;

#if 0
	/* Doesn't exist on rtl-sdk, and on field tests, caused troubles */
	if ((priv->cfg->rafael_chip == CHIP_R620D) ||
	   (priv->cfg->rafael_chip == CHIP_R828D) ||
	   (priv->cfg->rafael_chip == CHIP_R828)) {
		/* ref set refdiv2, reffreq = Xtal/2 on ATV application */
		if (type != V4L2_TUNER_DIGITAL_TV) {
			pll_ref /= 2;
			refdiv2 = 0x10;
			sleep_time = 20000;
		}
	} else {
		if (priv->cfg->xtal > 24000000) {
			pll_ref /= 2;
			refdiv2 = 0x10;
		}
	}
#endif

	rc = r820t_write_reg_mask(priv, 0x10, refdiv2, 0x10);
	if (rc < 0)
		return rc;

	/* set pll autotune = 128kHz */
	rc = r820t_write_reg_mask(priv, 0x1a, 0x00, 0x0c);
	if (rc < 0)
		return rc;

	/* set VCO current = 100 */
	rc = r820t_write_reg_mask(priv, 0x12, 0x80, 0xe0);
	if (rc < 0)
		return rc;

	/* Calculate divider */
	while (mix_div <= 64) {
		if (((freq * mix_div) >= vco_min) &&
		   ((freq * mix_div) < vco_max)) {
			div_buf = mix_div;
			while (div_buf > 2) {
				div_buf = div_buf >> 1;
				div_num++;
			}
			break;
		}
		mix_div = mix_div << 1;
	}

	rc = r820t_read(priv, data, sizeof data / sizeof data [0]);
	if (rc < 0)
		return rc;

	debug_printf_P(PSTR("r820t_set_pll 3: data[5]={%02x, %02x, %02x, %02x, %02x };\n"), data[0],data[1],data[2],data[3],data[4]);

	vco_fine_tune = (data[4] & 0x30) >> 4;

	if (vco_fine_tune > VCO_POWER_REF)
		div_num = div_num - 1;
	else if (vco_fine_tune < VCO_POWER_REF)
		div_num = div_num + 1;

	rc = r820t_write_reg_mask(priv, 0x10, div_num << 5, 0xe0);
	if (rc < 0)
		return rc;

	vco_freq = freq * mix_div;
	nint = vco_freq / (2 * pll_ref);
	vco_fra = vco_freq - 2 * pll_ref * nint;

	debug_printf_P(PSTR("r820t_set_pll: vco_freq=%u kHz (%u MHz)\n"), vco_freq, vco_freq / 1000);

	/* boundary spur prevention */
	if (vco_fra < pll_ref / 64) {
		vco_fra = 0;
	} else if (vco_fra > pll_ref * 127 / 64) {
		vco_fra = 0;
		nint++;
	} else if ((vco_fra > pll_ref * 127 / 128) && (vco_fra < pll_ref)) {
		vco_fra = pll_ref * 127 / 128;
	} else if ((vco_fra > pll_ref) && (vco_fra < pll_ref * 129 / 128)) {
		vco_fra = pll_ref * 129 / 128;
	}

	if (nint > 63) {
		debug_printf_P("No valid PLL values for %u kHz!\n", freq);
		return -1;
	}

	ni = (nint - 13) / 4;
	si = nint - 4 * ni - 13;

	rc = r820t_write_reg(priv, 0x14, ni + (si << 6));
	if (rc < 0)
		return rc;

	/* pw_sdm */
	if (!vco_fra)
		val = 0x08;
	else
		val = 0x00;

	rc = r820t_write_reg_mask(priv, 0x12, val, 0x08);
	if (rc < 0)
		return rc;

	/* sdm calculator */
	while (vco_fra > 1) {
		if (vco_fra > (2 * pll_ref / n_sdm)) {
			sdm = sdm + 32768 / (n_sdm / 2);
			vco_fra = vco_fra - 2 * pll_ref / n_sdm;
			if (n_sdm >= 0x8000)
				break;
		}
		n_sdm = n_sdm << 1;
	}

	debug_printf_P(PSTR("freq %d kHz, pll ref %d%s, sdm=0x%04x\n"),
		  freq, pll_ref, refdiv2 ? " / 2" : "", sdm);

	rc = r820t_write_reg(priv, 0x16, sdm >> 8);
	if (rc < 0)
		return rc;
	rc = r820t_write_reg(priv, 0x15, sdm & 0xff);
	if (rc < 0)
		return rc;

	for (i = 0; i < 2; i++) {
		local_delay_ms(20);

		/* Check if PLL has locked */
		rc = r820t_read(priv, data, 3);
		if (rc < 0)
			return rc;
		debug_printf_P(PSTR("r820t_set_pll 4: data[3]={%02x, %02x, %02x };\n"), data[0],data[1],data[2]);
		if (data[2] & 0x40)
			break;

		if (!i) {
			/* Didn't lock. Increase VCO current */
			rc = r820t_write_reg_mask(priv, 0x12, 0x60, 0xe0);
			if (rc < 0)
				return rc;
		}
	}

	if (!(data[2] & 0x40)) {
		//priv->has_lock = false;
		return 0;
	}

	//priv->has_lock = true;
	debug_printf_P("r820t_set_pll: tuner has lock at frequency %d kHz\n", freq);

	/* set pll autotune = 8kHz */
	rc = r820t_write_reg_mask(priv, 0x1a, 0x08, 0x08);

	return rc;
}

int r820t_set_freq(r820t_priv_t *priv, uint32_t freq)
{
  int rc;
  uint32_t lo_freq = freq + priv->if_freq;

  rc = r820t_set_tf(priv, freq);
  if (rc < 0)
    return rc;

  rc = r820t_set_pll(priv, lo_freq);
  if (rc < 0)
    return rc;

  priv->freq = freq;

  return 0;
}

int r820t_set_lna_gain(r820t_priv_t *priv, uint8_t gain_index)
{
  return r820t_write_reg_mask(priv, 0x05, gain_index, 0x0f);
}

int r820t_set_mixer_gain(r820t_priv_t *priv, uint8_t gain_index)
{
  return r820t_write_reg_mask(priv, 0x07, gain_index, 0x0f);
}

int r820t_set_vga_gain(r820t_priv_t *priv, uint8_t gain_index) // 0x10: IF vga gain controlled by vagc pin 
{
  return r820t_write_reg_mask(priv, 0x0c, 0x40 | gain_index, 0x5f);
}

int r820t_set_vga_voltagecontrol(r820t_priv_t *priv)
{
  return r820t_write_reg_mask(priv, 0x0c, 0x50, 0x50);
}

int r820t_set_lna_agc(r820t_priv_t *priv, uint8_t value)	// 1 - auto, 0 - manual
{
  value = value != 0 ? 0x00 : 0x10;
  return r820t_write_reg_mask(priv, 0x05, value, 0x10);	
}

int r820t_set_mixer_agc(r820t_priv_t *priv, uint8_t value)	// 1 - auto, 0 - manual
{
  value = value != 0 ? 0x10 : 0x00;
  return r820t_write_reg_mask(priv, 0x07, value, 0x10);
}

/* 
"inspired by Mauro Carvalho Chehab calibration technique"
https://stuff.mit.edu/afs/sipb/contrib/linux/drivers/media/tuners/r820t.c
part of r820t_set_tv_standard()
*/
int r820t_calibrate(r820t_priv_t *priv)
{
	  debug_printf_P(PSTR("r820t_calibrate: start\n"));
  int i, rc, cal_code;
  uint8_t data [5];

  for (i = 0; i < 5; i++)
  {
    /* Set filt_cap */
    rc = r820t_write_reg_mask(priv, 0x0b, 0x08, 0x60);
    if (rc < 0)
      return rc;

    /* set cali clk =on */
    rc = r820t_write_reg_mask(priv, 0x0f, 0x04, 0x04);
    if (rc < 0)
      return rc;

    /* X'tal cap 0pF for PLL */
    rc = r820t_write_reg_mask(priv, 0x10, 0x00, 0x03);
    if (rc < 0)
      return rc;

    rc = r820t_set_pll(priv, CALIBRATION_LO * 1000);
    if (rc < 0)
      return rc;

    /* Start Trigger */
    rc = r820t_write_reg_mask(priv, 0x0b, 0x10, 0x10);
    if (rc < 0)
      return rc;

    local_delay_us(10000);

    /* Stop Trigger */
    rc = r820t_write_reg_mask(priv, 0x0b, 0x00, 0x10);
    if (rc < 0)
      return rc;

    /* set cali clk =off */
    rc = r820t_write_reg_mask(priv, 0x0f, 0x00, 0x04);
    if (rc < 0)
      return rc;

    /* Check if calibration worked */
    rc = r820t_read(priv, data, sizeof data / sizeof data [0]);
    if (rc < 0)
      return rc;

    cal_code = data[4] & 0x0f;
    if (cal_code && cal_code != 0x0f)
	{
	  debug_printf_P(PSTR("r820t_calibrate: okay\n"));
      return 0;
	}
  }

	debug_printf_P(PSTR("r820t_calibrate: error\n"));
  return -1;
}

int r820t_init(r820t_priv_t *priv, const uint32_t if_freq)
{
  int rc;
  uint32_t saved_freq;

  r820t_state_standby = 0;
  priv->if_freq = if_freq;
  /* Initialize registers */
  airspy_r820t_write_init(priv->regs);

  r820t_set_freq(priv, priv->freq);

  /* Calibrate the IF filter */
  saved_freq = priv->freq;
  rc = r820t_calibrate(priv);
  priv->freq = saved_freq;
  if (rc < 0)
  {
	  debug_printf_P(PSTR("r820t_init: calibrate retray\n"));
    saved_freq = priv->freq;
    r820t_calibrate(priv);
    priv->freq = saved_freq;
  }
  else
  {
	  debug_printf_P(PSTR("r820t_init: calibrate okay\n"));
  }

  /* Restore freq as it has been modified by r820t_calibrate() */
  rc = r820t_set_freq(priv, priv->freq);
  return rc;
}

void r820t_startup(r820t_priv_t *priv)
{
  r820t_state_standby = 0;
  /* Initialize registers */
  airspy_r820t_write_init(priv->regs);
  /* Enter in Standby mode */
  r820t_standby();
}

void r820t_set_if_bandwidth(r820t_priv_t *priv, uint8_t bw)
{
    const uint8_t modes[] = { 0xE0, 0x80, 0x60, 0x00 };
    const uint8_t opt[] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    uint8_t a = 0xB0 | opt[bw & 0x0F];
    uint8_t b = 0x0F | modes[bw >> 4];
    r820t_write_reg(priv, 0x0A, a);
    r820t_write_reg(priv, 0x0B, b);
}

/* write to single register but do not update priv (return 0 if success) */
static void airspy_r820t_write_direct(uint8_t reg, uint8_t val)
{
  if(r820t_is_power_enabled() == true)
  {
    //i2c1_tx_start();
    i2c_start(R820T_I2C_ADDR | I2C_WRITE);
    i2c_write(reg);
    i2c_write(val);
	i2c_waitsend();
	i2c_stop();
  }
}

int r820t_standby(void)
{
	return 0;
  airspy_r820t_write_direct(0x05, 0xA0);
  airspy_r820t_write_direct(0x06, 0xD0);
  airspy_r820t_write_direct(0x07, 0x00);
  airspy_r820t_write_direct(0x08, 0x40);
  airspy_r820t_write_direct(0x09, 0xC0);
  airspy_r820t_write_direct(0x0a, 0x70);
  airspy_r820t_write_direct(0x0c, 0xA0);
  airspy_r820t_write_direct(0x0f, 0x2A);
  airspy_r820t_write_direct(0x11, 0x03);
  airspy_r820t_write_direct(0x17, 0xF4);
  airspy_r820t_write_direct(0x19, 0x0C);
  r820t_state_standby = 1;

  return 0;
}


static r820t_priv_t r820t0;

static uint_fast8_t gcalibrated;
static uint_fast8_t genable;



static uint_fast8_t lnagain = 14;
static uint_fast8_t mixergain = 15;
static uint_fast8_t vgagain = 4;

static uint_fast8_t lnaagc = 0;
static uint_fast8_t mixeragc = 0;

static void 
r820t_enable(
	uint_fast8_t enable
	)
{
	genable = enable;
	return;
	if (enable)
		;
	else
		r820t_standby();
}

static void 
r820t_setfreq(
	uint_fast32_t f
	)
{

	if (genable)
	{
		if (gcalibrated == 0)
		{
			gcalibrated = 1;

			//r820t0.xtal_freq = 16000000uL;
			r820t0.xtal_freq = 28800000uL;
			///r820t0.xtal_freq = 28808844uL;
			r820t0.xtal_freq = R820T_REFERENCE;
			r820t0.if_freq = R820T_IFFREQ;
			r820t0.freq = 100000000uL;
			memcpy(r820t0.regs, r82xx_init_array, sizeof r820t0.regs);

			r820t_startup(& r820t0);	// and go to standbay mode
			r820t_init(& r820t0, R820T_IFFREQ);
			r820t_set_if_bandwidth(& r820t0, 7);

			r820t_set_lna_gain(& r820t0, lnagain);
			r820t_set_mixer_gain(& r820t0, mixergain);
			r820t_set_vga_gain(& r820t0, vgagain);
			//r820t_set_vga_voltagecontrol(& r820t0);
			r820t_set_lna_agc(& r820t0, lnaagc);
			r820t_set_mixer_agc(& r820t0, mixeragc);

		}
		//r820t_set_pll(& r820t0, f);
		//r820t_set_tf(& r820t0, f - R820T_IFFREQ);
		r820t_set_freq(& r820t0, f - R820T_IFFREQ);
	}
}

static void 
r820t_initialize(void)
{
	uint_fast8_t val;

    i2c_start(R820T_I2C_ADDR | I2C_READ);
	i2c_read(& val, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
	i2c_stop();

	debug_printf_P(PSTR("r820t_initialize: signature=0x%02X (0x96 expected)\n"), r82xx_bitrev(val));

    //i2c_start(R820T_I2C_ADDR | I2C_READ);
	//i2c_read(& val, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
	//i2c_stop();
	//debug_printf_P(PSTR("r820t_initialize2: signature=0x%02X (0x96 expected)\n"), r82xx_bitrev(val));
}


#if WITHDEBUG


static void
r820t_update(void)
{
	r820t_set_lna_gain(& r820t0, lnagain);
	r820t_set_mixer_gain(& r820t0, mixergain);
	r820t_set_vga_gain(& r820t0, vgagain);
	//r820t_set_vga_voltagecontrol(& r820t0);
	r820t_set_lna_agc(& r820t0, lnaagc);
	r820t_set_mixer_agc(& r820t0, lnaagc);

	debug_printf_P(PSTR("lna=%u, mixer=%u, vga=%u, lnaagc=%s, mixeragc=%s\n"),
		lnagain, mixergain, vgagain, lnaagc ? "on" : "off", mixeragc ? "on" : "off");

}
void
r820t_spool(void)
{
	char kbch;
	if (dbg_getchar(& kbch) != 0)
	{
		enum { gainmax = 15 };
		switch (kbch)
		{
		case ' ':	
			r820t_update();
			break;

		case '1':	
			lnagain = lnagain ? lnagain - 1 : 0;
			r820t_update();
			break;

		case '2':	
			lnagain = lnagain < gainmax ? lnagain + 1 : lnagain;
			r820t_update();
			break;

		case '3':	
			mixergain = mixergain ? mixergain - 1 : 0;
			r820t_update();
			break;

		case '4':	
			mixergain = mixergain < gainmax ? mixergain + 1 : mixergain;
			r820t_update();
			break;

		case '5':	
			vgagain = vgagain ? vgagain - 1 : 0;
			r820t_update();
			break;

		case '6':	
			vgagain = vgagain < gainmax ? vgagain + 1 : vgagain;
			r820t_update();
			break;

		case 'q':	
			lnaagc = ! lnaagc;
			r820t_update();
			break;

		case 'w':	
			mixeragc = ! mixeragc;
			r820t_update();
			break;

		}
	}

}

#endif /* WITHDEBUG */

#endif /* R820T_H_INCLUDED */
