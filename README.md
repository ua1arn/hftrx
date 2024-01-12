# Проект HF Dream Receiver (КВ приёмник мечты)
## автор Гена Завидовский mgs2001@mail.ru UA1ARN

Инструкция по подготовке среды и компиляции:

## Микроконтроллер:

### Windows

1. Настраиваем окружение (компилятор и утилиты для сборки проекта) <br>
1.1 **ARM:** ARM GNU Toolchain https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads (последняя проверенная сборка (Arm GNU Toolchain 11.3.Rel1) 11.3.1 20220712) <br>
1.2 **ATMEGA/ATXMEGA:** AVR 8-bit Toolchain https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers или https://blog.zakkemble.net/avr-gcc-builds/<br>

Прямая ссылка https://github.com/ZakKemble/avr-gcc-build/releases/download/v12.1.0-1/avr-gcc-12.1.0-x64-windows.zip (распаковываем, прописываем путь к папке BIN в переменную окружения PATH)<br>
1.3 **RISC-V:** riscv-none-elf-gcc.exe (xPack GNU RISC-V Embedded GCC x86_64) 12.2.0 https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/
https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/download/v12.2.0-1/xpack-riscv-none-elf-gcc-12.2.0-1-win32-x64.zip

1.4 Утилиты для сборки отсюда: tools/gnu-mcu-eclipse-build-tools-2.11-20180428-1604-win64.zip (разархивировать, обновляем переменную PATH в системе, указав путь к подпапке bin, например C:\user\hftrx\tools\gnu-mcu-eclipse-build-tools-2.11-20180428-1604-win64\GNU MCU Eclipse\Build Tools\2.11-20180428-1604\bin) <br>
1.4 Утилиты для работы с GIT репозиториями https://git-scm.com/downloads <br>

2. Скачиваем дополнительные библиотеки, распаковываем их на уровень выше, чем папка проекта. <br>
2.1 **ATMEL ARM:** В случае использования Atmel ARM (SAM) процессоров, скачать и распаковать в папку "xdk-asf" пакет Advanced Software Framework (ASF) <https://www.microchip.com/mplab/avr-support/advanced-software-framework <br>

3. Устанавливаем IDE для разработки <br>
3.1 Скачиваем и устанавливаем Eclipse https://www.eclipse.org/downloads/ <br>
3.2 В верхнем меню Help -> Check for updates, устанавливаем обновления <br>
3.3 В верхнем меню Help -> Eclipse Marketplace, устанавливаем обновления <br>
3.4 В верхнем меню Help -> Eclipse Marketplace, используя поиск, устанавливаем расширение GNU MCU Eclipse

4. Собираем проект <br>
4.1 Скачаваем последнюю версию проекта командой "git clone https://github.com/ua1arn/hftrx hftrx" <br>
4.2 Перходим в каталог hftrx "cd hftrx"<br>
4.3 Скачиваем субмодули "git submodule update --init --recursive" <br>
4.4 Открываем проект через File -> Open projects from File System
4.5 Копируем файл product_template.h в product.h <br>

4.4 Раскомментируем нужные нам константы с выбранной конфигурацией <br>
4.5 В меню Eclipse, Project -> Build configurations -> Set active выбираем необходимый процессор (или через выпадающее меню с молотком в ToolBox). Build target выбираем default.

5. Прошиваем <br>
5.1 Скопилированные прошивки находятся в папке /build/<процессор>/ <br>
5.2 Для обновления прошивки процессора RENESAS через bootloader в командных файлах используется уилилита из проекта https://sourceforge.net/projects/dfu-util/files/latest/download

### Linux

Пакеты, которые необходимо установить:
- arm-none-eabi-gcc (gcc-arm-none-eabi for Ubuntu)
- arm-none-eabi-newlib (libnewlib-arm-none-eabi for Ubuntu)
- dfu-util

В зависимости от микроконтроллера выбирается каталог сборки образа (для Сокол Про `build/stm32mp157axx`). Дальше каталог сборки образа подразумевается в переменной `BUILD_DIR`

#### Сборка утилит

Все скрипты приведены для запуска из корня репозитория

- stm32image
  ```shell
  cd tools/stm32image
  make
  cp stm32image $BUILD_DIR
  ```
- bin2ihex
  ```shell
  cd tools/bin2ihex
  make
  cp bin2ihex $BUILD_DIR
  ```

#### Сборка образов

##### Образ приложения
- Скопировать `product_template.h` в `product.h`
- Затянуть все подмодули командой `git submodule update --init --recursive`
- В каталоге `$BUILD_DIR` выполнить `make`

##### Загрузчик
- Скопировать `product_template.h` в `product.h`
- В `product.h` раскомментировать строку 
  ```C
  #define WITHISBOOTLOADER	1	/* соответствующим Build Target компилируем и собираем bootloader */
  ```
- Затянуть все подмодули командой `git submodule update --init --recursive`
- В каталоге `$BUILD_DIR` выполнить `make bootloader`

## FPGA:

Используется Quartis II 13.1 (с апдейтом) Paid version http://download.altera.com/akdlm/software/acdsinst/13.1/162/ib_tar/Quartus-13.1.0.162-windows-complete.tar <br>
и  апдейт после http://download.altera.com/akdlm/software/acdsinst/13.1.4/182/update/QuartusSetup-13.1.4.182.exe

## Общее:

Выбор целевой конфигурации проекта производится в файле product.h <br>
На основании выбраной конфигурации и целевого процессора (arm/atmega) выбирается пара конфигурационных файлов <br>
.\board\*ctlstyle*.h и .\board\*cpustyle*.h. <br>
ctlstyle описывают внешние (по отношению к процессору) особенности аппаратуры - адреса устройств на шине spi, типы применённых микросхем и тип индикатора (и так далее).  <br>
cpustyle описывают назначение выводов процессора (распределение по портам ввода/вывода).

