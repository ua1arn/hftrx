Каталог для сборки версии программы под процессор Allwinner T133-S3

sun8i

Dual-core ARM CortexTM-A7
32 KB L1 I-cache + 32 KB L1 D-cache per core, and 256 KB L2 cache

xfel:
https://gitee.com/xboot/xfel/releases/tag/v1.3.2

Программа:
Выполнить make clean, затем make
Результатом работы является файл tc1_t113s3_app.alw32

Загрузчик:
Выполнить make clean, затем make bootloader
Результатом работы является файл fsbl.alw32
Требуются изменения в product.h

