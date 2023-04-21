#include "gpio.h"

gpio_port *get_gpio_dev(char port)
{
    switch (port) 
    {
    case 'a':
    case 'A':
        return ((gpio_port *)(GPIO_BASE));
    case 'b':
    case 'B':
        return ((gpio_port *)(GPIO_BASE + 0xC));
    case 'c':
    case 'C':
        return ((gpio_port *)(GPIO_BASE + 0x18));
    case 'd':
    case 'D':
        return ((gpio_port *)(GPIO_BASE + 0x24));
    };
    return 0;
}

gpio_eport *get_gpio_edev(char port)
{
    switch (port) 
    {
    case 'a':
    case 'A':
        return ((gpio_eport *)(GPIO_PORTA_EDR));
    case 'b':
    case 'B':
        return ((gpio_eport *)(GPIO_PORTB_EDR));
    case 'c':
    case 'C':
        return ((gpio_eport *)(GPIO_PORTC_EDR));
    case 'd':
    case 'D':
        return ((gpio_eport *)(GPIO_PORTD_EDR));
    };
    return 0;
}

//Исправлена ошибка в этой функции. Оригинал - от элвиса - говна кусок
enum ERL_ERROR gpio_pin_config(char port, int pin, int val, int dir, int ctl)
{
    gpio_port *gport = get_gpio_dev(port);
    if (!gport) 
    {
        return ERL_SYSTEM_ERROR;
    }
    if ((pin < 0) || (pin > 31)) 
    {
        return ERL_SYSTEM_ERROR;
    }

    if (val)
        gport->port_dr._val |= (1 << pin);
    else
        gport->port_dr._val &= ~(1 << pin);

    if (dir)
        gport->port_ddr._val |= (1 << pin);
    else
        gport->port_ddr._val &= ~(1 << pin);

    if (ctl)
        gport->port_ctl._val |= (1 << pin);
    else
        gport->port_ctl._val &= ~(1 << pin);

    return ERL_NO_ERROR;
}

enum ERL_ERROR gpio_pin_set(char port, int pin, int val)
{
    gpio_port *gport = get_gpio_dev(port);
    if (!gport) 
    {
        return ERL_SYSTEM_ERROR;
    }
    if ((pin < 0) || (pin > 31)) 
    {
        return ERL_SYSTEM_ERROR;
    }

    if (val)
        gport->port_dr._val |= (1 << pin);
    else
        gport->port_dr._val &= ~(1 << pin);
    return ERL_NO_ERROR;
}

//Чтение происходит из других регистров... get_gpio_edev возвращает их.
int gpio_pin_get(char port, int pin)
{
    gpio_eport *gport = get_gpio_edev(port);
    if (!gport) 
    {
        return ERL_SYSTEM_ERROR;
    }
    if ((pin < 0) || (pin > 31)) 
    {
        return ERL_SYSTEM_ERROR;
    }

    return ( (gport->port_edr._val >> pin) & 0x01 );
}

enum ERL_ERROR gpio_pin_invert(char port, int pin)
{
    gpio_port *gport = get_gpio_dev(port);
    if (!gport) 
    {
        return ERL_SYSTEM_ERROR;
    }
    if ((pin < 0) || (pin > 31)) 
    {
        return ERL_SYSTEM_ERROR;
    }

    gport->port_dr._val ^= (1 << pin);
    return ERL_NO_ERROR;
}
