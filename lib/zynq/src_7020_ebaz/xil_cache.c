#include "hardware.h"

void Xil_DCacheEnable(void)
{

}

void Xil_DCacheDisable(void)
{

}

void Xil_DCacheInvalidate(void)
{

}

void Xil_DCacheInvalidateRange(uintptr_t adr, uint32_t len)
{
	arm_hardware_invalidate(adr, len);
}

void Xil_DCacheFlush(void)
{
	arm_hardware_flush_all();
}

void Xil_DCacheFlushRange(uintptr_t adr, uint32_t len)
{
	arm_hardware_flush(adr, len);
}

void Xil_ICacheEnable(void)
{

}

void Xil_ICacheDisable(void)
{

}
void Xil_ICacheInvalidate(void)
{

}
void Xil_ICacheInvalidateRange(uintptr_t adr, uint32_t len)
{
	arm_hardware_flush_invalidate(adr, len);
}
