Каталог для сборки версии программы под процессор ST STM32MP157AAB3

Программа:
Выполнить make clean, затем make
Результатом работы является файл tc1_stm32mp157axx_app.stm32

Загрузчик:
Выполнить make clean, затем make bootloader
Результатом работы является файл tc1_stm32mp157axx_boot.hex и tc1_stm32mp157axx_boot.stm32
Требуются изменения в product.h

Командные файлы 
	flash_read_bin.cmd	чтение старой прошивки
	flash_write_bin.cmd	обновление прошивки из файла с расширением .bin
	flash_write_dfu.cmd	обновление прошивки из файла с расширением .dfu
