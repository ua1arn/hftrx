# Бесконечный аудиопоток без компрессии (Uncompressed FLAC) на базе lwIP

Архив содержит исходные файлы для организации бесконечной передачи несжатого звука (16 кГц, 16 бит, моно) с микроконтроллера в HTML5-браузер без ограничения WAV в 4 ГБ.

## Состав архива
- `flac_stream.h` / `flac_stream.c` — упаковщик сырых 16-битных PCM-семплов в фреймы FLAC режима `VERBATIM` (без сжатия) с расчетом CRC-8 и CRC-16.
- `audio_server.h` / `audio_server.c` — HTTP-сервер на базе lwIP Raw API, отдающий бесконечный поток по адресу `/stream` и веб-страницу с плеером по `/`.
- `index.html` — пример HTML5-клиента со стандартным тегом `<audio>`.
- `lwipopts_example.h` — рекомендуемые размеры TCP-буферов для `lwipopts.h`.

## Порядок интеграции
1. Добавьте `flac_stream.c` и `audio_server.c` в ваш проект.
2. Увеличьте `TCP_SND_BUF` и `TCP_SND_QUEUELEN` в `lwipopts.h` согласно `lwipopts_example.h`.
3. После инициализации сетевого стека lwIP вызовите `audio_server_init()`.
4. Каждые 32 мс (по мере накопления 512 семплов с I2S или АЦП) вызывайте `audio_server_push_pcm(buffer)`.
   *Примечание: при использовании FreeRTOS вызов `audio_server_push_pcm()` необходимо выполнять в контексте задачи `tcpip_thread` через `tcpip_callback()`.*
