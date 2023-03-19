#ifndef _scu_c_
#define  _scu_c_
typedef struct {
    volatile unsigned int SCU_Control;
    volatile unsigned int SCU_Config;
    volatile unsigned int SCU_State;
    volatile unsigned int InvalidateSM;
    volatile unsigned int FilterStart;
    volatile unsigned int FilterEnd;
    volatile unsigned int hole[2];
    volatile unsigned int SAC;
    volatile unsigned int SNSAC;
} scu_module_t;
void Write_SCU(void);
#endif
