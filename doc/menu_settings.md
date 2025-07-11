# Меню настроек трансивера

## Вход в меню
Вход в меню -- удержанное (более 0.6 секунды) нажатие кнопки MENU.

## Сброс параметров
При включении питания с нажатой кнопкой SPLIT на дисплее появляется приглашение подтвердить необходимость сброса всех параметров к значениям «по умолчанию». Подтверждение выполняется повторным нажатием кнопки SPLIT. Если вход в это состояние ошибочен -- просто выключить питание. Нажатия остальных кнопок игнорируются.

Если какого-то из пунктов, перечисленных в данной таблице в конкретной модификации нет -- это нормально, значит, в Вашей конфигурации оборудования он не имеет смысла.

## Таблица параметров

| Параметр         | Описание |
|------------------|----------|
| **LCD CONT**     | Контрастность индикатора (если есть возможность управления). |
| **LCD LIGH**     | Установка яркости подсветки |
| **KEY BEEP**     | Тональность звукового сигнала по нажатиям клавиатуры |
| **CW WDT W**     | Регулировка полосы пропускания широкого телеграфного фильтра |
| **CW WDT N**     | Регулировка полосы пропускания узкого телеграфного фильтра |
| **SSB HI W**     | Высокочастотный срез широкого SSB фильтра |
| **SSB LO W**     | Низкочастотный срез широкого SSB фильтра |
| **SSB SFTW**     | Регулировка скатов широкого SSB фильтра<br>0 -- максимальная прямоугольность фильтра<br>100 -- минимальная прямоугольность фильтра (используется 50% от порядка фильтра) |
| **SSB AFRW**     | Изменение тембра звука - на 24 кГц АЧХ изменяется на столько децибел для широкого SSB фильтра |
| **SSB AFRN**     | Изменение тембра звука - на 24 кГц АЧХ изменяется на столько децибел для узкого SSB фильтра |
| **AM AFR W**     | Изменение тембра звука - на 24 кГц АЧХ изменяется на столько децибел для широкого AM фильтра |
| **AM AFR N**     | Изменение тембра звука - на 24 кГц АЧХ изменяется на столько децибел для узкого AM фильтра |
| **SSB HI N**     | Высокочастотный срез узкого SSB фильтра |
| **SSB LO N**     | Низкочастотный срез узкого SSB фильтра |
| **SSB SFTN**     | Регулировка скатов узкого SSB фильтра<br>0 -- максимальная прямоугольность фильтра<br>100 -- минимальная прямоугольность фильтра (используется 50% от порядка фильтра) |
| **AM HI W**      | Высокочастотный срез широкого AM/FM фильтра |
| **AM LO W**      | Низкочастотный срез широкого AM/FM фильтра |
| **AM HI N**      | Высокочастотный срез узкого AM/FM фильтра |
| **AM LO N**      | Низкочастотный срез узкого AM/FM фильтра |
| **SDTN LVL**     | Относительная громкость сигнала самоконтроля или отклика клавиш. При 100% оператор слышит только звук по нажатиям клавиатуры, сигнал с приёмника не слышит. |
| **KBD LIGH**     | Включение подсветки клавиатуры |
| **DIMM TIM**     | Автоматическое снижение яркости подсветки до минимальной.<br>Время неактивности пользователя устанавливается в пределах 1..240 секунд (с дискретностью 5 секунд).<br>Значение параметра 0 для выключения этой функции (подсветка от активности пользователя не зависит). |
| **SLEEPTIM**     | Автоматическое выключение подсветки и звука.<br>Время неактивности пользователя устанавливается в пределах 1..240 минут. 0 для выключения этой функции. |
| **BLUE BG**      | На цветном дисплее сделать цвет фона синим вместо чёрного |
| **SHOW dBm**     | Показывать уровень сигнала в единицах шкалы S (S9 = -73 dBm, 50 uV @50 Ohm) или как мощность. |
| **RXBW IND**     | Насыщенность цвета «шторки» - индикатор полосы пропускания приёмника на спектре. |
| **PBT**          | Pass Band Tune -- плавное уменьшение полосы пропускания взаимной расстройкой гетеродинов. |
| **IF SHIFT**     | Смещение частоты третьего гетеродина для изменения тембра принимаемых сигналов. Увеличение значения параметра смещает слышимую часть спектра в более высокие частоты. |
| **RFSG MOD**     | Режим сигнал-генератора |
| **CW PITCH**     | Тон самоконтроля |
| **CW SPEED**     | Скорость передачи автоматического телеграфного ключа (в единицах WPM по системе PARIS) |
| **VIBROPLX**     | Скорость ускорения передачи последовательности точек (имитация передачи на механическом ключе Vibroplex).<br>Значение 0 -- отключение данной функции |
| **BREAK-IN**     | Включение CW BREAK-IN-- автоматического перехода на передачу в телеграфе при нажатии на ключ. |
| **CW DELAY**     | Задержка перехода на приём после последней посылки в телеграфе |
| **EDGE TIM**     | Время нарастания/спада огибающей телеграфной посылки при передаче |
| **SD RECRD**     | Автоматическое начало записи на SD CARD при включении. |
| **AM DEPTH**     | Подстройка глубины модуляции в АМ - 0..100% |
| **DACSCALE**     | Подстройка амплитуды сигнала с ЦАП передатчика - 0..100% |
| **FT8BOOST**     | Увеличение усиления при передаче в цифровых режимах 100..400% |
| **PA BIAS**      | Подстройка тока оконечного каскада передатчика |
| **FAN TIME**     | Время в секундах, сколько держать включенным вентилятор после перехода на прием |
| **FAN FLOW**     | Условная скорость вентилятора |
| **TEMP LIM**     | Температура, при которой срабатывает защита оконечного каскада усилителя мощности от перегрева |
| **IGNO SWR**     | Игнорирование превышения КСВ как аварийного состояния. |
| **TX GATE**      | On -- предварительный усилитель передатчика и оконечный усилитель включаются на передаче. Off - никогда не включаются. На антенное реле и фильтры не действует. Использовать, например, при работе с трансверторами. |
| **CLASSA**       | Включение оконечного каскада в режим с улучшенной линейностью и уменьшенной мощностью. |
| **CLASSA P**     | Дополнительная коррекция мощности (уменьшение) при работе в режиме с повышенной линейностью. |
| **ATU PWR**      | Уменьшенная мощность при работе автоматического согласующего устройства |
| **DAC TEST**     | Режим тестирования ЦАП -- непрерывная несущая почти на частоте передачи -- для тестирования аппаратуры. Использование может повредить каскады усилителя мощности. |
| **TX MW/LW**     | Корректировка выходной мощности в диапазонах средних и длинных волн |
| **TX 160m**      | Корректировка мощности в коротковолновых диапазонах |
| **TX 80m**       | |
| **TX 6m**        | |
| **TX 2m**        | |
| **TX 0.7m**      | |
| **TX ACC13**     | |
| **TX ACC14**     | |
| **TX ACC15**     | |
| **KEYER**        | Тип электронного ключа.<br>- ACS -- с автоматическим формированием паузы между знаками,<br>- ELE -- без этого,<br>- OFF -- простой вертикальный ключ, подключённый к любому контакту разъёма манипулятора.<br>- BUG -- полуавтоматический ключ -- тире вручную, точки автоматические. |
| **CWKEYREV**     | Поменять местами контакты точек и тире на электронном ключе |
| **CWWEIGHT**     | Отношение длительности тире к длительности точки |
| **SPACEWGH**     | Отношение длительности паузы между элементами знака к длительности точки. |
| **TX POWER**     | Мощность передатчика |
| **MUTE ALL**     | Отключение микрофонного усилителя во всех режимах |
| **VOX**          | Включение VOX |
| **VOXDELAY**     | Задержка перехода на приём при VOX |
| **VOX LEVL**     | Уровень чувствительности VOX |
| **AVOX LEV**     | Уровень чувствительности ANTI VOX |
| **ENC1 DIV**     | Уменьшение чувствительности основного валкодера |
| **ENC2 DIV**     | Уменьшение чувствительности дополнительного валкодера |
| **ENC1 RES**     | Выбор типа основного валкодера (количество прорезей в диске) |
| **ENC1 DYN**     | Включение режима динамического изменения шага перестройки в зависимости от скорости вращения основного валкодера. |
| **BIG STEP**     | Грубый шаг перестройки (50 Герц для CW и SSB, 1 кГц для AM). Применять с валкодерами с небольшим количеством циклов на оборот -- например, с 24-х «дырочковыми» валкодерами. |
| **STAYFREQ**     | При изменении режима работы частота настройки фиксируется, а не корректируется для сохранения слышимого эфира при переключении LSB-CWR и USB-CW. |
| **NB LEVEL**     | NFM -- уровень срабатывания шумоподавителя |
| **MUTE ALL**     | Во всех режимах (а не только в CW) отключение микрофона. Используется при digital modes в режиме SSB. |
| **LINE LVL**     | Использовать LINE IN вместо микрофона |
| **MIC LEVL**     | Уровень передачи при выбранном источнике LINE IN |
| **MIC BUST**     | Включение 20 дБ предусилителя сигнала микрофона в кодеке |
| **MIC AM, MIC SSB, MIC DIG, MIC FM** | Выбор источника сигнала при передаче в режимах SSB/AM/FM/DIG:<br>- MIKE -- микрофон<br>- LINE -- Вход LINE IN (левый канал)<br>- USB -- аудиоданные с USB устройства Transmitter Input<br>- 2TONE -- двухтональный сигнал<br>- 1TONE -- синусоида 700 герц<br>- NOISE -- шум<br>- MUTE - тишина |
| **MIC AGC**      | Включение программной АРУ в тракте НЧ сигнала перед модулятором (только в тракте от микрофона). |
| **MK AGCSC**     | На какую часть (в процентах) от полной амплитуды выхода настроена АРУ микрофона. |
| **MICAGCGN**     | Максимальное усиление АРУ микрофона в дБ |
| **MIC CLIP**     | Параметр ограничителя микрофона (после АРУ). В процентах (0..90): 0 -- не действует, 90 -- ограничение наступает на 10 процентах от полной амплитуды, потом масштабируется до полной амплитуды.<br>Начальное состояние -- 20. Ограничитель работает после АРУ микрофона. |
| **PLAY USB**     | Можно использовать трансивер как USB SPEAKER для прослушивания музыки с компьютера. Использовалось при отладке программного обеспечения. |
| **I/Q SWAP**     | Поменять местами I и Q выборки в потоке для анализатора спектра в USB AUDIO. |
| **CAT SPD**      | Скорость обмена командами интерфейса CAT. |
| **CAT ENAB**     | Разрешение работы интерфейса CAT |
| **CAT PTT**      | Управление переходом на передачу от интерфейса CAT.<br>Значения:<br>NONE,<br>SER1 DTR,<br>SER1 RTS,<br>SER2 DTR,<br>SER2 RTS. |
| **CAT KEY**      | Управление манипуляцией от от интерфейса CAT.<br>Значения:<br>NONE,<br>SER1 DTR,<br>SER1 RTS,<br>SER2 DTR,<br>SER2 RTS. |
| **DATA VOX**     | Разрешение автоматического перехода на передачу в режиме DATA при появлении потока аудиоданных на USB AUDIO интерфейсе. |
| **DIG MARK**     | Частота MARK для работы в режиме RTTY (соответствует частоте настройки на дисплее) |
| **RXTX dly**     | Задержка подачи ВЧ при переходе на передачу в миллисекундах |
| **TXRX dly**     | Задержка переключения на приём после окончания передачи (например, задержка в формирователе огибающей телеграфного сигнала) в миллисекундах |
| **PRETXdly**     | Задержка на выключение обратимого тракта при переходе на передачу -- для исключения формирования короткой посылки несущей перед первым знаком. в миллисекундах |
| **SWR SHOW**     | Выбор отображение SWR вместо уровня мощности |
| **SWR cali**     | Калибровочный параметр SWR-метра (выставить по КСВ=2 на образцовую нагрузку) -- выравнивание уровней с датчиков прямой и отражённой волны. |
| **FWD lowr**     | Код с АЦП прямой волны КСВ-метра, с которого начинает работать расчёт КСВ. |
| **PWR cali**     | Калибровочный параметр PWR-метра (код АЦП, соответствующий полностью заполненной шкале измерителя) |
| **BAT cali**     | Калибровочный параметр измерителя напряжения питания. Соответствует напряжению в измеряемой цепи при максимальном напряжении на измеряющем входе процессора -- обычно равно питанию процессора -- 3.3 или 5.0 вольт. 17.5 вольт через делитель 4.3 килоома и 1 килоом дают 3.3 вольта на входе процессора. |
| **DAC LEVL**     | При наличии цепей управления частотой опорного генератора -- управление этим напряжением. |
| **USE TXFL**     | Выбор используемого фильтра при передаче. Если нет выбора, отображается единственный используемый фильтр. |
| **REF FREQ**     | Точная частота опорного генератора (настраивается по нулевым биениям при приёме эталонных частот в режиме CWZ). |
| **LFM MODE**     | Работа в режиме приёма станций наклонного зондирования ионосферы |
| **LFM STRT**     | Начальная частота |
| **LFM STOP**     | Конечная частота |
| **LFM SPD**      | Скорость перестройки (кГц за секунду) |
| **LFM OFST**     | Время начала (секунды) цикла зондирования в пределах часа |
| **LFM PERI**     | Период повторения (секунды) циклов зондирования в пределах часа |
| **LO1 LEVEL**    | Амплитуда сигнала на выходе DDS генератора первого гетеродина. 0%..100% |
| **REFSI570**     | Точная частота внутреннего опорного генератора синтезатора Si570 (настраивается по нулевым биениям при приёме эталонных частот в режиме CWZ). |
| **BFO USB**      | Частота третьего гетеродина в режиме приёма SSB на верхней боковой полосе (частота ската фильтра) |
| **BFO LSB**      | Частота третьего гетеродина в режиме приёма SSB на нижней боковой полосе (частота ската фильтра) |
| **LAST LSB**     | В трансиверах без управления частотой DDS в смесительном детекторе -- указать боковую полосу, которая фильтруется фильтром основной селекции. YES, если фильтр на нижнюю боковую. |
| **BFO FREQ**     | В трансиверах без управления частотой DDS в смесительном детекторе -- значение частоты опорного генератора. |
| **HAVE 0.3**     | Наличие фильтра ПЧ 0.3 кГц |
| **CNTR 0.3**     | Частота середины полосы пропускания телеграфного фильтра 0.3 кГц |
| **HAVE 0.5**     | Наличие в приёмнике фильтра ПЧ 0.5 кГц |
| **CNTR 0.5**     | Частота середины полосы пропускания телеграфного фильтра 0.5 кГц |
| **HAVE 6.0**     | Наличие в приёмнике фильтра ПЧ 6.0 кГц |
| **6K OFFS**      | Подстройка центральной частоты фильтра ПЧ 6.0 кГц |
| **CAR USB**      | Частота третьего гетеродина в режиме передачи SSB на верхней боковой полосе (частота ската фильтра) |
| **CAR LSB**      | Частота третьего гетеродина в режиме передачи SSB на нижней боковой полосе (частота ската фильтра) |
| **DC TX**        | Формирование сигнала передачи в режиме CW без участия третьего гетеродина |
| **LO1DV RX**     | Указание синтезатору, есть ли делитель в формировании сигнала для первого смесителя в режиме приёма. |
| **LO1DV TX**     | Указание синтезатору, есть ли делитель в формировании сигнала для первого смесителя при передаче. |
| **S9 LEVEL**     | Калибровочный параметр S-метра -- уровень сигнала S9 (калибруется первым) |
| **S9 DELTA**     | Калибровочный параметр S-метра -- диапазон уровней от S0 до S9 |
| **+60DELTA**     | Калибровочный параметр S-метра -- диапазон уровней от S9 до S9 + 60 дБ |
| **ATTPRESH**     | Корректировка показаний с-метра по включенному аттенюатору и предварительному усилителю |
| **BANDONLY**     | Перестройка ограничена любительскими диапазонами |
| **FORCEXVR**     | Принудительно включить коммутацию как при работе трансвертора |
| **FREQ FPS**     | Частота обновления показаний частоты на дисплее |
| **SMTR FPS**     | Частота обновления показаний S-метра на дисплее |
| **BAND BC**      | Запоминание частот в пределах вещательных, а не любительских диапазонов |
| **BAND 27**      | Запоминание одной частоты в пределах CB диапазона |
| **BAND 50**      | Запоминание одной частоты в пределах 50 МГц диапазона |
| **BAND 70**      | Запоминание одной частоты в пределах 70 МГц диапазона |
| **BAND144**      | Запоминание частот в пределах 144 МГц диапазона |
| **ADC RAND**     | отладочный пункт, меняет протокол передачи между радио-АЦП и ПЛИС. Может привести к незначительному увеличению шума. Сильный шум появляется, если есть ошибки (непропай) при монтаже АЦП или ПЛИС. |
| **ADC DITH**     | включает внутренний генератор шума в радио-АЦП. |
| **ADC FIFO**     | включает очередь данных между радио=АЦП и ПЛИС (держать включенным, выключение может проявляться как треск при приеме) |
| **ADC OFFS**     | Корректировка постоянной составляющей АЦП |
| **ADC FS**       | Мощность, соответствующая максимальному значению на входе АЦП приёмника. |
| **ADC FSXV**     | Мощность, соответствующая максимальному значению на входе АЦП приёмника в режиме приема с конвертором |
| **AGC T1**       | Время срабатывания медленной цепи АРУ (мс) |
| **AGC T2**       | Время отпускания медленной цепи АРУ (с) |
| **AGC T3**       | Время срабатывания быстрой цепи АРУ (мс). Поскольку в DDC трансивере «АРУ вперед» - оставить ноль. |
| **AGS T4**       | Время отпускания быстрой цепи АРУ (мс) |
| **AGC HUNG**     | Время, в течении которого при уменьшении сигнала не происходит изменение усиления. |
| **AGC RATE**     | На N децибел изменения входного сигнала происходит 1 дБ выходного |
| **IFGN MAX**     | диапазон ручной (или автоматической) регулировки цифрового усиления - максимальное значение |
| **TUNER L**      | Код значения емкости для ручной подстройки согласующего устройства. |
| **TUNER C**      | Код значения емкости для ручной подстройки согласующего устройства. |
| **TUNER TY**     | Код типа (понижающий/повышающий) для ручной подстройки согласующего устройства. |
| **TUNER WT**     | Время ожидания в миллисекундах после переключения параметров тюнера перед получением значения КСВ |
| **CTCSS**        | Передача сабтона (вкл/выкл) в режиме NFM |
| **CTCSS FQ**     | Частота сабтона |
| **CTCSSLVL**     | Уровень (в процентах от максимальной амплитуды НЧ сигнала поступающего на модулятор). |
| **SQUELCH**      | Уровень включения порогового шумоподавителя |
| **AGC FS**       | Максимальная мощность сигнала на входе приёмника -- используется для калибровки S-метра |
| **AGC FS XV**    | Максимальная мощность сигнала на входе приёмника -- используется для калибровки S-метра при работе с трансвертором