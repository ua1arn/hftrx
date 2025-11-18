
# Инструкция по сборке проекта

## Подготовка окружения для микроконтроллера

### Для Windows

#### 1. Установка инструментов сборки

1. **Компиляторы:**
  - [ARM GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)  
    Проверенная версия: `arm-none-eabi-gcc 14.2.1`
  - [RISC-V Toolchain](https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/)  
    Проверенная версия: `riscv-none-elf-gcc (xPack GNU RISC-V Embedded GCC x86_64) 15.2.0`

2. **Вспомогательные утилиты:**
  - [Windows Build Tools](https://xpack-dev-tools.github.io/windows-build-tools-xpack/docs/releases/)
  - [Git](https://git-scm.com/downloads)

---

#### 2. Настройка IDE

1. Установите [Eclipse IDE](https://www.eclipse.org/downloads/)
2. Обновите IDE через `Help → Check for Updates`
3. Установите плагины через Eclipse Marketplace:
  - `GNU MCU Eclipse` (**обязательно**)
  - Дополнительные по необходимости

---

#### 3. Сборка проекта

```bash
git clone https://github.com/ua1arn/hftrx hftrx
cd hftrx
git submodule update --init --recursive
cp product_template.h product.h
```

Настройте конфигурацию в `product.h`, раскомментировав нужные параметры.

В Eclipse:

1. Откройте проект через `File → Open Projects from File System`
2. Выберите сборочную конфигурацию:  
   `Project → Build Configurations → Set Active`
3. Соберите проект (значок молотка)

---

#### 4. Прошивка

Скомпилированные файлы находятся в каталоге:

```
/build/<процессор>/
```

Для прошивки Renesas используйте `dfu-util`:  
[Скачать dfu-util](https://sourceforge.net/projects/dfu-util/files/latest/download)

---

## Для Linux

### Подготовка
Для начала необходимо установить утилиты `git`, `binutils` и `gcc`
```
sudo apt install git-all
sudo apt install binutils
sudo apt install build-essential
```

### Установка зависимостей

```bash
sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi dfu-util
sudo apt install pkg-config
sudo apt install libusb-1.0-0-dev
```

---

### Сборка утилит

```bash
# stm32image
cd tools/stm32image
make
cp stm32image $BUILD_DIR

# bin2ihex
cd ../bin2ihex
make
cp bin2ihex $BUILD_DIR
```

---

### Сборка образов

#### Основной образ

```bash
cp product_template.h product.h
git submodule update --init --recursive
cd $BUILD_DIR
make
```

#### Загрузчик

В `product.h` раскомментируйте:

```c
#define WITHISBOOTLOADER 1
```

Соберите загрузчик:

```bash
cd $BUILD_DIR
make bootloader
```

---

## Сборка для FPGA

### Требования

- Quartus II 13.1 (платная версия) + обновление
  - [Основной пакет](http://download.altera.com/akdlm/software/acdsinst/13.1/162/ib_tar/Quartus-13.1.0.162-windows-complete.tar)
  - [Обновление](http://download.altera.com/akdlm/software/acdsinst/13.1.4/182/update/QuartusSetup-13.1.4.182.exe)

---

## Конфигурация проекта

Файл `product.h` определяет конфигурацию и целевой процессор.  
В зависимости от конфигурации используются:

```
./board/*ctlstyle*.h  # Аппаратные настройки (SPI, дисплеи, микросхемы)
./board/*cpustyle*.h  # Назначения портов ввода/вывода процессора
```
