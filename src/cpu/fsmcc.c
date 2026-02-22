#if __ARM_ARCH == 8

struct FSmcccRes {
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
};


void FSmcccHvcCall(unsigned long arg0, unsigned long arg1,
                                   unsigned long arg2, unsigned long arg3,
                                   unsigned long arg4, unsigned long arg5,
                                   unsigned long arg6, unsigned long arg7,
                                   struct FSmcccRes *res) ;



void FSmcccSmcCall(unsigned long arg0, unsigned long arg1,
                                   unsigned long arg2, unsigned long arg3,
                                   unsigned long arg4, unsigned long arg5,
                                   unsigned long arg6, unsigned long arg7,
                                   struct FSmcccRes *res) ;

void FSmcccSmcGetSocIdCall(struct FSmcccRes *res) ;

#ifdef __cplusplus
}
#endif


#if __aarch64__


/**
@name: FSmcccSmcCall
@msg: 执行SMC（Secure Monitor Call）指令。
@param {unsigned long} arg0: 第一个参数传递给SMC调用。
@param {unsigned long} arg1: 第二个参数传递给SMC调用。
@param {unsigned long} arg2: 第三个参数传递给SMC调用。
@param {unsigned long} arg3: 第四个参数传递给SMC调用。
@param {unsigned long} arg4: 第五个参数传递给SMC调用。
@param {unsigned long} arg5: 第六个参数传递给SMC调用。
@param {unsigned long} arg6: 第七个参数传递给SMC调用。
@param {unsigned long} arg7: 第八个参数传递给SMC调用。
@param {struct FSmcccRes*} res: 结构体指针，用于保存SMC调用的结果。
@note: 此函数执行SMC（Secure Monitor Call）指令，并将结果保存在提供的结构体指针中。
*/
void FSmcccSmcCall(unsigned long arg0, unsigned long arg1,
                                   unsigned long arg2, unsigned long arg3,
                                   unsigned long arg4, unsigned long arg5,
                                   unsigned long arg6, unsigned long arg7,
                                   struct FSmcccRes *res)
{
    register unsigned long x0 asm("x0") = arg0;
    register unsigned long x1 asm("x1") = arg1;
    register unsigned long x2 asm("x2") = arg2;
    register unsigned long x3 asm("x3") = arg3;
    register unsigned long x4 asm("x4") = arg4;
    register unsigned long x5 asm("x5") = arg5;
    register unsigned long x6 asm("x6") = arg6;
    register unsigned long x7 asm("x7") = arg7;

    asm volatile(
        "smc #0\n"
        : "+r"(x0), "+r"(x1), "+r"(x2), "+r"(x3),
          "+r"(x4), "+r"(x5), "+r"(x6), "+r"(x7)
        :
        : "memory");

    res->a0 = x0;
    res->a1 = x1;
    res->a2 = x2;
    res->a3 = x3;
}

/**
@name: FSmcccHvcCall
@msg: 执行HVC（虚拟化监管CALL）指令。
@param {unsigned long} arg0: 第一个参数传递给HVC调用。
@param {unsigned long} arg1: 第二个参数传递给HVC调用。
@param {unsigned long} arg2: 第三个参数传递给HVC调用。
@param {unsigned long} arg3: 第四个参数传递给HVC调用。
@param {unsigned long} arg4: 第五个参数传递给HVC调用。
@param {unsigned long} arg5: 第六个参数传递给HVC调用。
@param {unsigned long} arg6: 第七个参数传递给HVC调用。
@param {unsigned long} arg7: 第八个参数传递给HVC调用。
@param {struct FSmcccRes*} res: 结构体指针，用于保存HVC调用的结果。
@note: 此函数执行SMC（虚拟化监管CALL）指令，并将结果保存在提供的结构体指针中。
*/
void FSmcccHvcCall(unsigned long arg0, unsigned long arg1,
                                   unsigned long arg2, unsigned long arg3,
                                   unsigned long arg4, unsigned long arg5,
                                   unsigned long arg6, unsigned long arg7,
                                   struct FSmcccRes *res)
{
    register unsigned long x0 asm("x0") = arg0;
    register unsigned long x1 asm("x1") = arg1;
    register unsigned long x2 asm("x2") = arg2;
    register unsigned long x3 asm("x3") = arg3;
    register unsigned long x4 asm("x4") = arg4;
    register unsigned long x5 asm("x5") = arg5;
    register unsigned long x6 asm("x6") = arg6;
    register unsigned long x7 asm("x7") = arg7;

    asm volatile(
        "hvc #0\n"
        : "+r"(x0), "+r"(x1), "+r"(x2), "+r"(x3),
          "+r"(x4), "+r"(x5), "+r"(x6), "+r"(x7)
        :
        : "memory");

    res->a0 = x0;
    res->a1 = x1;
    res->a2 = x2;
    res->a3 = x3;
}

#else


/**
@name: FSmcccHvcCall
@msg: 执行HVC（虚拟化监管CALL）指令。
@param {unsigned long} arg0: 第一个参数传递给HVC调用。
@param {unsigned long} arg1: 第二个参数传递给HVC调用。
@param {unsigned long} arg2: 第三个参数传递给HVC调用。
@param {unsigned long} arg3: 第四个参数传递给HVC调用。
@param {unsigned long} arg4: 第五个参数传递给HVC调用。
@param {unsigned long} arg5: 第六个参数传递给HVC调用。
@param {unsigned long} arg6: 第七个参数传递给HVC调用。
@param {unsigned long} arg7: 第八个参数传递给HVC调用。
@param {struct FSmcccRes*} res: 结构体指针，用于保存HVC调用的结果。
@note: 此函数执行SMC（虚拟化监管CALL）指令，并将结果保存在提供的结构体指针中。
*/
void FSmcccHvcCall(unsigned long arg0, unsigned long arg1,
                                   unsigned long arg2, unsigned long arg3,
                                   unsigned long arg4, unsigned long arg5,
                                   unsigned long arg6, unsigned long arg7,
                                   struct FSmcccRes *res)
{
    register unsigned long r0 asm("r0") = arg0;
    register unsigned long r1 asm("r1") = arg1;
    register unsigned long r2 asm("r2") = arg2;
    register unsigned long r3 asm("r3") = arg3;
    register unsigned long r4 asm("r4") = arg4;
    register unsigned long r5 asm("r5") = arg5;
    register unsigned long r6 asm("r6") = arg6;
    register unsigned long r7 asm("r7") = arg7;

    asm volatile(
        "hvc #0\n"
        : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),
          "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)
        :
        : "memory");

    res->a0 = r0;
    res->a1 = r1;
    res->a2 = r2;
    res->a3 = r3;
}


/**
@name: FSmcccSmcCall
@msg: 执行SMC（Secure Monitor Call）指令。
@param {unsigned long} arg0: 第一个参数传递给SMC调用。
@param {unsigned long} arg1: 第二个参数传递给SMC调用。
@param {unsigned long} arg2: 第三个参数传递给SMC调用。
@param {unsigned long} arg3: 第四个参数传递给SMC调用。
@param {unsigned long} arg4: 第五个参数传递给SMC调用。
@param {unsigned long} arg5: 第六个参数传递给SMC调用。
@param {unsigned long} arg6: 第七个参数传递给SMC调用。
@param {unsigned long} arg7: 第八个参数传递给SMC调用。
@param {struct FSmcccRes*} res: 结构体指针，用于保存SMC调用的结果。
@note: 此函数执行SMC（Secure Monitor Call）指令，并将结果保存在提供的结构体指针中。
*/
void FSmcccSmcCall(unsigned long arg0, unsigned long arg1,
                                   unsigned long arg2, unsigned long arg3,
                                   unsigned long arg4, unsigned long arg5,
                                   unsigned long arg6, unsigned long arg7,
                                   struct FSmcccRes *res)
{
    register unsigned long r0 asm("r0") = arg0;
    register unsigned long r1 asm("r1") = arg1;
    register unsigned long r2 asm("r2") = arg2;
    register unsigned long r3 asm("r3") = arg3;
    register unsigned long r4 asm("r4") = arg4;
    register unsigned long r5 asm("r5") = arg5;
    register unsigned long r6 asm("r6") = arg6;
    register unsigned long r7 asm("r7") = arg7;

    asm volatile(
        "smc #0\n"
        : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),
          "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)
        :
        : "memory");

    res->a0 = r0;
    res->a1 = r1;
    res->a2 = r2;
    res->a3 = r3;
}

#endif

#endif /* __ARM_ARCH == 8 */
