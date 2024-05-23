Каталог для сборки версии программы под процессор Allwinner A64
Allwinner A64-H (Banana Pi BPI-M64)

Программа:
Закомментировать в product.h WITHISBOOTLOADER
Выполнить make clean, затем make
Результатом работы является файл tc1_a64_app.alw32

Загрузчик:
Раскомментировать в product.h WITHISBOOTLOADER
Выполнить make clean, затем make bootloader
Результатом работы является файл fsbl.alw32
Требуются изменения в product.h


xfel:
https://gitee.com/xboot/xfel/releases/tag/v1.3.2

Allwinner A64 (sun50iw1) SoC features a Quad-Core Cortex-A53 ARM CPU, and a Mali400 MP2 GPU from ARM.

ARM Cortex-A53 Quad-Core (r0p4, revidr=0x80)
512KiB L2-Cache
32KiB (Instruction) / 32KiB (Data) L1-Cache per core
SIMD NEON (including double-precision floating point), VFP4
Cryptography Extension (SHA and AES instructions)

Variants
Allwinner H64 is targetted for OTT boxes and A64 for the tablets. 
Both are quad core Cortex A53 processors with a Mali-400MP2 GPU, 
H.265 4K video playback with basically the same interfaces and peripherals, 
but H64 also supports H.264 at 4K resolutions, while A64 is limited to H.264 @1080p, and H64 adds a TS interface.
