#include "1892vm14ya.h"
#include "scu.h"

/*
typedef struct {
    volatile unsigned int SCU_Control;
    volatile unsigned int SCU_Config;
    volatile unsigned int SCU_State;
    //volatile unsigned int IntrStatus;
} scu_module_t;
*/

/* ÌÎÄÓËÜ ÍÅ ÐÀÁÎ×ÈÉ */

void Write_SCU(void)
{
    scu_module_t  * scu_module = (scu_module_t *)(BASE_ADDR_MPU);
    scu_module -> SAC = 3;
    //scu_module -> SNSAC = 3;
    scu_module -> SCU_Control = (1<<3) | (1<<2) | (1<<0);
    scu_module -> SCU_Config = 0x0A<<8;// 1010b << 8
    scu_module -> SAC = 0;
}
