# Проект HF Dream Receiver (КВ приёмник мечты)
## автор Гена Завидовский mgs2001@mail.ru UA1ARN

Инструкция по подготовке среды и компиляции:

## Микроконтроллер:

1. Настраиваем окружение (компилятор и утилиты для сборки проекта) <br>
1.1 **ARM:** GNU ARM Toolchain https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads (последняя проверенная сборка 7-2018-q2-update) <br>
1.2 **ATMEGA/ATXMEGA:** AVR 8-bit Toolchain https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers (распаковываем, прописываем путь к папке BIN в переменную окружения PATH)<br>
1.3 Утилиты для сборки отсюда: http://www.cygwin.com/ (устанавливаем пакет make, обновляем переменную PATH в системе, указав путь к подпапке bin, например C:\cygwin64\bin) <br>
1.4 Утилиты для работы с GIT репозиториями https://git-scm.com/downloads

2. Скачиваем дополнительные библиотеки, распаковываем их на уровень выше, чем папка проекта. <br>
2.1 **ARM:** CMSIS, используем оптимизированный форк, для этого в папке выше проекта вводим команду "git clone https://github.com/XGudron/CMSIS_5" <br>
2.2 **ATMEL ARM:** В случае использования Atmel ARM (SAM) процессоров, скачать и распаковать в папку "xdk-asf" пакет Advanced Software Framework (ASF) https://www.microchip.com/mplab/avr-support/advanced-software-framework

3. Устанавливаем IDE для разработки <br>
3.1 Скачиваем и устанавливаем Eclipse https://www.eclipse.org/downloads/ <br>
3.2 В верхнем меню Help -> Check for updates, устанавливаем обновления <br>
3.3 В верхнем меню Help -> Eclipse Marketplace, устанавливаем обновления <br>
3.4 В верхнем меню Help -> Eclipse Marketplace, используя поиск, устанавливаем расширение GNU MCU Eclipse

4. Собираем проект <br>
4.1 Скачаваем последнюю версию проекта командой "git clone https://github.com/ua1arn/hftrx" <br>
4.2 Открываем проект через File -> Open projects from File System
4.3 Копируем файл product.h.prototype в product.h <br>
4.4 Раскомментируем нужные нам константы с выбранной конфигурацией <br>
4.5 В меню Eclipse, Project -> Build configurations -> Set active выбираем необходимый процессор (или через выпадающее меню с молотком в ToolBox). Build target выбираем default.

5. Прошиваем <br>
5.1 Скопилированные прошивки находятся в папке /build/<процессор>/ <br>
5.2 Для обновления прошивки процессора RENESAS через bootloader в командных файлах используется уилилита из проекта https://sourceforge.net/projects/dfu-util/files/latest/download


## FPGA:

Используется Quartis II 13.1 (с апдейтом) Paid version http://download.altera.com/akdlm/software/acdsinst/13.1/162/ib_tar/Quartus-13.1.0.162-windows-complete.tar <br>
и  апдейт после http://download.altera.com/akdlm/software/acdsinst/13.1.4/182/update/QuartusSetup-13.1.4.182.exe

## Общее:

Выбор целевой конфигурации проекта производится в файле product.h <br>
На основании выбраной конфигурации и целевого процессора (arm/atmega) выбирается пара конфигурационных файлов <br>
.\board\*ctlstyle*.h и .\board\*cpustyle*.h. <br>
ctlstyle описывают внешние (по отношению к процессору) особенности аппаратуры - адреса устройств на шине spi, типы применённых микросхем и тип индикатора (и так далее).  <br>
cpustyle описывают назначение выводов процессора (распределение по портам ввода/вывода).

