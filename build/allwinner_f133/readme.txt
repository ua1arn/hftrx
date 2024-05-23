Каталог для сборки версии программы под процессор Allwinner F133

xfel:
https://gitee.com/xboot/xfel/releases/tag/v1.3.2

The D1s features a single RV64GCV core XuanTie C906 from T-Head Semiconductor (subsidiary of Alibaba).

Программа:
Выполнить make clean, затем make
Результатом работы является файл tc1_aw_d1s_app.alw32

Загрузчик:
Выполнить make clean, затем make bootloader
Результатом работы является файл tc1_aw_d1s_boot.hex и tc1_aw_d1s_boot.alw32
Требуются изменения в product.h

В CMSIS_5 внести изменения в соответствии с cmsis_gcc_h_mods.png

#if ! defined (__riscv)
...
#endif /* ! defined (__riscv) */



