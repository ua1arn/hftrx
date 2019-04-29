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

1. Настраиваем окружение (компилятор и утилиты для сборки проекта)
1.1 GNU ARM Toolchain https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads (последняя проверенная сборка 7-2018-q2-update)
1.2 Утилиты для сборки (напр. make) отсюда: http://www.yagarto.org/ или отсюда: http://www.cygwin.com/ (проверяем что в командной строке работает команда make, иначе обновляем переменную PATH в системе)

2. Скачиваем дополнительные библиотеки, распаковываем их на уровень выше, чем папка проекта
2.1 CMSIS https://github.com/ARM-software/CMSIS_5/releases
2.2 В случае использования процессора Cortex-A9 используем форк https://github.com/XGudron/CMSIS_5

3. Устанавливаем IDE для разработки
3.1 Скачиваем и устанавливаем Eclipse https://www.eclipse.org/downloads/
3.2 В верхнем меню Help -> Check for updates, устанавливаем обновления
3.3 В верхнем меню Help -> Eclipse Marketplace, устанавливаем обновления
3.4 В верхнем меню Help -> Eclipse Marketplace, используя поиск, устанавливаем расширение GNU MCU Eclipse

4. Выбираем тип аппаратуры и собираем проект
4.1 Копируем файл product.h.prototype в product.h
4.2 Раскомментируем нужные нам константы с выбранной конфигурацией
4.3 В меню Eclipse, Project -> Build configurations -> Set active выбираем необходимый процессор (или через выпадающее меню с молотком в ToolBox)

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

