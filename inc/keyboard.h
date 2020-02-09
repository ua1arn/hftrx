/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED 1

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum
{
	KBD_CODE_SPLIT = 0, 
	KBD_CODE_SPLIT_HOLDED,
	KBD_CODE_A_EX_B,		// A/B symbol on key
	KBD_CODE_A_EQ_B,		// A=B sy,bol on key
	KBD_CODE_ATT,
	KBD_CODE_BW,	// IF Bandwidth
	KBD_CODE_NR,	// AF Noise Reduction
	KBD_CODE_IFSHIFT,
	KBD_CODE_BAND_UP,
	KBD_CODE_BAND_DOWN,
	KBD_CODE_BKIN,
	KBD_CODE_BKIN_HOLDED,

	KBD_CODE_MENU,
	KBD_CODE_MODE, KBD_CODE_MODEMOD,	// переключение режимов - первый код при коротком нажатии, второй - при удержанном
#if WITHDIRECTBANDS
	KBD_CODE_MODE_3, KBD_CODE_MODEMOD_3,
	KBD_CODE_MODE_2, KBD_CODE_MODEMOD_2,
	KBD_CODE_MODE_1, KBD_CODE_MODEMOD_1,
	KBD_CODE_MODE_0, KBD_CODE_MODEMOD_0,
#endif /* WITHDIRECTBANDS */
	KBD_CODE_LOCK,
	KBD_CODE_USEFAST,
	KBD_CODE_AFNARROW,		// переключить УНЧ в режим узкой полосы
	KBD_CODE_TXTUNE,
	KBD_CODE_ATUSTART,
	KBD_CODE_ATUBYPASS,
	KBD_CODE_DISPMODE,		// переключение набора отображаемой на дисплее информации

	KBD_CODE_ENTERFREQ,		// переход в режим прямого ввода частоты
	KBD_CODE_ENTERFREQDONE,	// выход

	KBD_CODE_ERASECONFIG,

	KBD_CODE_VOXTOGGLE,	
	KBD_CODE_PWRTOGGLE,
	KBD_CODE_NOTCHTOGGLE,
	KBD_CODE_NOTCHFREQ,
	KBD_CODE_COMPTOGGLE,
	KBD_CODE_ALTTOGGLE,
	KBD_CODE_LDSPTGL,
	KBD_CODE_DATATOGGLE,
	KBD_CODE_DWATCHTOGGLE,
	KBD_CODE_DWATCHHOLD,
	KBD_CODE_AMBANDPASSUP,	// Подстройка верхней частоты среза НЧ фильтра в режиме АМ
	KBD_CODE_AMBANDPASSDOWN,	// Подстройка верхней частоты среза НЧ фильтра в режиме АМ

	KBD_CODE_LOCK_HOLDED,
	KBD_CODE_MENU_CWSPEED,
	KBD_CODE_AGC,
	KBD_CODE_PAMP,
	KBD_CODE_GEN_HAM,
	KBD_CODE_ANTENNA,
	KBD_CODE_MOX,
	KBD_CODE_DIGI,

	// Ручная перестройка параметров тюнера
	KBD_CODE_TUNERTYPE,
	KBD_CODE_CAP_UP,
	KBD_CODE_CAP_DOWN,
	KBD_CODE_IND_UP,
	KBD_CODE_IND_DOWN,

	KBD_CODE_RECORDTOGGLE,		// SD CARD audio recording
	KBD_CODE_RECORD_HOLDED,

	KBD_CODE_VIBROCTL,		// 328
	KBD_CODE_CWSPEEDUP,
	KBD_CODE_CWSPEEDDOWN,
	//
	ENC_CODE_STEP_UP,		// относится только к енкодеру - обрабатывается его функциями
	ENC_CODE_STEP_DOWN,		// относится только к енкодеру - обрабатывается его функциями
	//
	KBD_CODE_0,
	KBD_CODE_1,
	KBD_CODE_2,
	KBD_CODE_3,
	KBD_CODE_4,
	KBD_CODE_5,
	KBD_CODE_6,
	KBD_CODE_7,
	KBD_CODE_8,
	KBD_CODE_9,
	KBD_CODE_10,
	KBD_CODE_11,
	KBD_CODE_12,
	KBD_CODE_13,
	KBD_CODE_14,
	KBD_CODE_15,
	KBD_CODE_16,
	KBD_CODE_17,
	KBD_CODE_18,
	KBD_CODE_19,
	KBD_CODE_20,
	KBD_CODE_21,
	KBD_CODE_22,
	KBD_CODE_23,

	KBD_CODE_USER1,
	KBD_CODE_USER2,
	KBD_CODE_USER3,
	KBD_CODE_USER4,
	KBD_CODE_USER5,
	//
	KBD_CODE_POWEROFF,
	//
#if WITHDIRECTBANDS
	KBD_CODE_BAND_1M8, KBD_CODE_BAND_3M5, KBD_CODE_BAND_5M3, KBD_CODE_BAND_7M0, 
	KBD_CODE_BAND_10M1, KBD_CODE_BAND_14M0, KBD_CODE_BAND_18M0, 
	KBD_CODE_BAND_21M0, KBD_CODE_BAND_24M9, KBD_CODE_BAND_28M0, KBD_CODE_BAND_50M0,
#endif /* WITHDIRECTBANDS */
	//
	KBD_ENC2_PRESS, KBD_ENC2_HOLD,

#if WITHWAVPLAYER || WITHSENDWAV
	KBD_CODE_PLAYFILE1,
	KBD_CODE_PLAYFILE2,
	KBD_CODE_PLAYFILE3,
	KBD_CODE_PLAYFILE4,
	KBD_CODE_PLAYFILE5,
	KBD_CODE_PLAYSTOP,
	KBD_CODE_PLAYLOUD,	// громче
	KBD_CODE_PLAYQUITE,	// тише
#endif /* WITHWAVPLAYER || WITHSENDWAV */
	//
	KBD_CODE_MAX		// на этот код назначаются неиспользуемые клавиши.
};

// флаги, определяют модификации нажаьой клавиши:
// KIF_NONE - обычгная клавиша, без автоповтора (чувствительна к удержанию)
// KIF_POWER - клвыиша реагирует после окончания длителного нажатия
// KIF_SLOW - с медленным автоповтором
// KIF_FAST - с быстрым автоповтором
// KIF_ERASE - нажатие этой клавиши на запуске распознаётся как команда на стирание NVRAM
// KIF_EXTMENU - нажатие этой клавиши на запуске даёт разрешение доступа к расширенному списку пунктов меню.

enum
{
	KIF_SLOW = 0x01, 
	KIF_FAST = 0x02, 
	KIF_POWER = 0x04,	/* кнопка выклбчения питания - срабаьывание по отпусканию после длинного нажатия */
	KIF_ERASE = 0x08,
	KIF_EXTMENU = 0x10,
	KIF_SLOW4 = 0x20,
	//
	KIF_USER2 = 0x40,	// rec
	KIF_USER3 = 0x80,	// play

	KIF_NONE = 0x00
};

#define HOLDED_SPEEDUP 5	/* Во сколько раз быстрее работает перестройка клавишами при удержанном нажатии */

/* получение скан-кода клавиши или 0 в случае отсутствия.
 * если клавиша удержана, возвращается скан-код с добавленным битом 0x80
 */
/* инициализация переменных работы с клавиатурой */
/* возврат не-ноля - удержана после сброса клавиша */
void  kbd_initialize(void);

uint_fast8_t kbd_is_tready(void);
/* получение скан-кода клавиши, возврат 0 в случае отсутствия.
 */
uint_fast8_t kbd_scan(uint_fast8_t * key);
uint_fast8_t kbd_get_ishold(uint_fast8_t flag);
uint_fast8_t kbd_getnumpad(uint_fast8_t key);

struct qmkey 
{
	uint_fast8_t flags;
	uint_fast8_t code, holded;
	uint_fast8_t numpad;		/* код клавиши в режиме цифрового ввода */
};
extern const struct qmkey qmdefs [];
uint_fast8_t getexitkey(void);


#define KEYBOARD_NOKEY UINT8_MAX

uint_fast8_t board_get_pressed_key(void);	// Если ничего - возвращаем KEYBOARD_NOKEY

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* KEYBOARD_H_INCLUDED */
