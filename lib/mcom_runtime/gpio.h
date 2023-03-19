#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "erlcommon.h"

typedef union {
    unsigned int _val;
    struct {
        unsigned int PIN0 : 1;
        unsigned int PIN1 : 1;
        unsigned int PIN2 : 1;
        unsigned int PIN3 : 1;
        unsigned int PIN4 : 1;
        unsigned int PIN5 : 1;
        unsigned int PIN6 : 1;
        unsigned int PIN7 : 1;
        unsigned int PIN8 : 1;
        unsigned int PIN9 : 1;
        unsigned int PIN10 : 1;
        unsigned int PIN11 : 1;
        unsigned int PIN12 : 1;
        unsigned int PIN13 : 1;
        unsigned int PIN14 : 1;
        unsigned int PIN15 : 1;
        unsigned int PIN16 : 1;
        unsigned int PIN17 : 1;
        unsigned int PIN18 : 1;
        unsigned int PIN19 : 1;
        unsigned int PIN20 : 1;
        unsigned int PIN21 : 1;
        unsigned int PIN22 : 1;
        unsigned int PIN23 : 1;
        unsigned int PIN24 : 1;
        unsigned int PIN25 : 1;
        unsigned int PIN26 : 1;
        unsigned int PIN27 : 1;
        unsigned int PIN28 : 1;
        unsigned int PIN29 : 1;
        unsigned int PIN30 : 1;
        unsigned int PIN31 : 1;
    };
} GPIO_REG;

typedef struct {
    GPIO_REG port_dr;
    GPIO_REG port_ddr;
    GPIO_REG port_ctl;

} gpio_port;

typedef struct {
    GPIO_REG port_edr;
} gpio_eport;

gpio_port *get_gpio_dev(char port);

enum ERL_ERROR gpio_pin_config(char port, int pin, int val, int dir, int ctl);

enum ERL_ERROR gpio_pin_set(char port, int pin, int val);

enum ERL_ERROR gpio_pin_invert(char port, int pin);

int gpio_pin_get(char port, int pin);

#define GPIO_BASE 0x38034000
/* Прерывание работает только на порту А*/
#define GPIO_INTEN 			(*(volatile GPIO_REG *)(GPIO_BASE + 0x30))
#define GPIO_INTMASK 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x34))
#define GPIO_INTTYPE 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x38))
#define GPIO_INTPOLARITY 	(*(volatile GPIO_REG *)(GPIO_BASE + 0x3C))
#define GPIO_INTSTATUS 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x40))
#define GPIO_RAWSTATUS 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x44))
#define GPIO_DEBOUNCE 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x48))
#define GPIO_PORTA_EOI 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x4C))
#define GPIO_PORTA_EXT 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x50))
/* Адресация портов */
#define GPIO_PORTA_DR 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x00))
#define GPIO_PORTA_DDR 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x04))
#define GPIO_PORTA_CTL 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x08))
/* Адресация портов - долбанный элвис и тут насрал.. */
#define GPIO_PORTB_DR 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x0C))
#define GPIO_PORTB_DDR 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x10))
#define GPIO_PORTB_CTL 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x14))
/* Адресация портов - долбанный элвис и тут насрал.. */
#define GPIO_PORTC_DR 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x18))
#define GPIO_PORTC_DDR 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x1C))
#define GPIO_PORTC_CTL 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x20))
/* Адресация портов - долбанный элвис и тут насрал.. */
#define GPIO_PORTD_DR 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x24))
#define GPIO_PORTD_DDR 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x28))
#define GPIO_PORTD_CTL 		(*(volatile GPIO_REG *)(GPIO_BASE + 0x2C))
/* Адресация портов - долбанный элвис и тут насрал.. */
#define GPIO_PORTA_EDR 		((GPIO_BASE + 0x50))
#define GPIO_PORTB_EDR 		((GPIO_BASE + 0x54))
#define GPIO_PORTC_EDR 		((GPIO_BASE + 0x58))
#define GPIO_PORTD_EDR 		((GPIO_BASE + 0x5C))

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H */
