Каталог для сборки версии программы под процессор RENESAS RZ/A1L R7S721020

Программа:
Выполнить make clean, затем make
Результатом работы является файл tc1_r7s721020_app.stm32

Загрузчик:
Выполнить make clean, затем make bootloader
Результатом работы является файл tc1_r7s721020_boot.hex и tc1_r7s721020_boot.bin
Требуются изменения в product.h


memory allocation:

BOOT0
  FLASH (rx) : ORIGIN = 0x18000000, LENGTH = 256K
  RAM (rwx)  : ORIGIN = 0x20200000, LENGTH = 256K - 16k
  TTB (rwx)  : ORIGIN = 0x20200000 + 256K - 16k, LENGTH = 16k

BOOT
  RAM (rwx)  : ORIGIN = 0x20200000 + 256K + 256,	LENGTH = 1024M - 256K - 16k - 256
  TTB (rwx)  : ORIGIN = 0x20200000 + 256K + 1024M - 16k, 	LENGTH = 16k

APP: 
  RAM (rwx)  : ORIGIN = 0x20000000 + 256,	LENGTH = 3072k - 16k - 256
  TTB (rwx)  : ORIGIN = 0x20000000 + 3072k - 16k, 	LENGTH = 16k

  