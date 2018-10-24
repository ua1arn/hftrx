Инструкция по подготовке среды компиляции для ARM процессоров находятся ниже.

ATMEGA/ATXMEGA:

Каталог проекта не должен собержать в пути русских (или других неанглийских) букв. Пробелы допустимы.

Для компиляции используется

AVR Studio 4.19 (build 730):
http://www.atmel.com/Images/AvrStudio4Setup.exe

В репозитории файл проекта для AVR Studio 4.19 хранится под именем tc1.aps.prototype, его надо переименовать в tc1.aps и открыть в среде как проект.
Так же требуется переименование файла product.h.prototype в product.h. В нем раскомментировать требуемую конфигурацию и закомментировант не нужную.

Должно быть установленно 
Atmel AVR 8-bit and 32-bit Toolchain 3.4.2 - Windows 
http://www.atmel.com/images/avr-toolchain-installer-3.4.2.1573-win32.win32.x86.exe

Для обеспечения работы под x64 версиями Windows 8.x, Windows 10 скачать
http://www.madwizard.org/download/electronics/msys-1.0-vista64.zip
И разархивировать msys-1.0.dll в тот же каталог, где находится make.exe
Или взять данный файл (или целиком пакет) с download page проекта MinGW - Minimalist GNU for Windows 
http://sourceforge.net/projects/mingw

Протестировано использование avr-gcc версии avr-gcc-6.1.1 - скачанный архив надо распаковать и настроить на использование avr-gcc из него. 
Утилита make используется из комплекта toolchain от ATMEL. 
Сборка это часть проекта https://sourceforge.net/projects/mobilechessboar/?source=directory
Ссылка для скачивания:
http://kent.dl.sourceforge.net/project/mobilechessboard/avr-gcc%20snapshots%20%28Win32%29/avr-gcc-6.1.1_2016-06-26_mingw32.zip

ARM:

Для компиляции используется комплект инструментов с сайта www.yagarto.de
Кроме того, при компиляции для просцессоров CORTEX-M0/M3/M4/M7 используется библиотека CMSIS от ARM LIMITED. Каталог находится на том же уровне, что и каталог проекта. 
Скачать CMSIS можно тут: 
https://silver.arm.com/download/ARM_and_AMBA_Architecture/CMSIS-SP-00300-r4p5-00rel0/CMSIS-SP-00300-r4p5-00rel0.zip
Информация о текущих версиях:
https://developer.arm.com/embedded/cmsis
https://github.com/ARM-software/CMSIS_5/releases/tag/5.2.0

Устанрвить две инсталляшки – 
yagarto-tools-20121018-setup.exe

Обновление компилятора - теперь тут: https://launchpad.net/gcc-arm-embedded
Последняя версия - 
https://developer.arm.com/-/media/Files/downloads/gnu-rm/6_1-2017q1/gcc-arm-none-eabi-6-2017-q1-update-win32.exe
Информация о текущих версиях:
https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
Old product page:
https://launchpad.net/gcc-arm-embedded

FPGA:

Используется Quartis II 13.1 (с апдейтом) Paid version

http://download.altera.com/akdlm/software/acdsinst/13.1/162/ib_tar/Quartus-13.1.0.162-windows-complete.tar

и  апдейт после

http://download.altera.com/akdlm/software/acdsinst/13.1.4/182/update/QuartusSetup-13.1.4.182.exe

ATMEL ARM:

Требуется Atmel Software Framework 3.27 

http://www.atmel.com/images/asf-standalone-archive-3.27.0.28.zip

Общее:

Выбор целевой конфигурации проекта производится в файле product.h
На основании выбраной конфигурации и целевого процессора (arm/atmega) выбирается пара конфигурационных файлов
.\board\*ctlstyle*.h и .\board\*cpustyle*.h.
ctlstyle описывают внешние (по отношению к процессору) особенности аппаратуры - адреса устройств на шине spi, 
типы применённых микросхем и тип индикатора (и так далее). 
cpustyle описывают назначение выводов процессора (распределение по портам ввода/вывода).

