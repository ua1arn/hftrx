Каталог для сборки версии программы под процессор Allwinner V851S
https://github.com/YuzukiHD/Yuzukilizard


xfel:
https://gitee.com/xboot/xfel/releases/tag/v1.3.2


Dual-core ARM CortexTM-A7
32 KB L1 I-cache + 32 KB L1 D-cache per core, and 256 KB L2 cache

Программа:
Выполнить make clean, затем make
Результатом работы является файл tc1_alwnrt113s3.alw32

Загрузчик:
Выполнить make clean, затем make bootloader
Результатом работы является файл tc1_v3s_boot.hex и tc1_v3s_boot.alw32
Требуются изменения в product.h

