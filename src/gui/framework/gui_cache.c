/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "../gui_port_include.h"

#if WITHTOUCHGUI && GUI_USE_CACHE

#include "gui.h"
#include "gui_system.h"
#include "gui_structs.h"
#include "gui_settings.h"
#include "gui_windows.h"
#include "gui_cache.h"
#include "utils.h"
#include "../gui_port.h"

/* ============================================================================
 * СОЗДАНИЕ/УНИЧТОЖЕНИЕ КЭША
 * ========================================================================== */

gui_objects_cache_t * gui_objects_cache_create(uint16_t w, uint16_t h, gui_cache_type_t type)
{
    gui_objects_cache_t * cache = NULL;

    cache = (gui_objects_cache_t *)calloc(1, sizeof(gui_objects_cache_t));
    if (cache == NULL) {
        return NULL;
    }

    cache->w = w;
    cache->h = h;
    cache->type = type;
    cache->valid = 0;
    cache->state = 0;
    cache->flags = 0;
    cache->text[0] = '\0';
    cache->custom_data = NULL;
    cache->custom_hash = 0;

    cache->buffer = __gui_cache_texture_create(w, h);
    if (cache->buffer == NULL) {
        free(cache);
        return NULL;
    }

    return cache;
}

void gui_objects_cache_destroy(gui_objects_cache_t * cache)
{
    if (cache == NULL) {
        return;
    }

    if (cache->buffer != NULL) {
        __gui_cache_texture_destroy(cache->buffer);
        cache->buffer = NULL;
    }

    if (cache->custom_data != NULL) {
        free(cache->custom_data);
        cache->custom_data = NULL;
    }

    free(cache);
}

/* ============================================================================
 * УПРАВЛЕНИЕ СОСТОЯНИЕМ КЭША
 * ========================================================================== */

void gui_objects_cache_invalidate(gui_objects_cache_t * cache)
{
    if (cache == NULL) {
        return;
    }
    cache->valid = 0;
}

void gui_objects_cache_clean(gui_objects_cache_t * cache)
{
    if (cache == NULL) {
        return;
    }
    cache->valid = 0;
    cache->state = 0;
    cache->flags = 0;
    cache->text[0] = '\0';
    if (cache->custom_data != NULL) {
        free(cache->custom_data);
        cache->custom_data = NULL;
    }
    cache->custom_hash = 0;
}

/* ============================================================================
 * ПРОВЕРКА НЕОБХОДИМОСТИ ПЕРЕРИСОВКИ
 * ========================================================================== */

int gui_objects_cache_needs_render(gui_objects_cache_t * cache,
                                    uint8_t state,
                                    uint8_t flags,
                                    const char * text)
{
    /* Кэш не создан */
    if (cache == NULL || cache->buffer == NULL) {
        return 1;
    }

    /* Кэш недействителен */
    if (!cache->valid) {
        return 1;
    }

    /* Изменилось состояние */
    if (cache->state != state) {
        return 1;
    }

    /* Изменились флаги */
    if (cache->flags != flags) {
        return 1;
    }

    /* Изменился текст */
    if (safe_strcmp(cache->text, text) != 0)
        return 1;

    /* Кэш актуален - перерисовка не требуется */
    return 0;
}

/* ============================================================================
 * РЕНДЕРИНГ В КЭШ
 * ========================================================================== */

int gui_objects_cache_begin_render(gui_objects_cache_t * cache)
{
    if (cache == NULL || cache->buffer == NULL) {
        return 0;
    }

    /* Начинаем отрисовку в кэш-текстуру */
    __gui_cache_texture_begin(cache->buffer);

    return 1;
}

void gui_objects_cache_end_render(gui_objects_cache_t * cache,
                                   uint8_t state,
                                   uint8_t flags,
                                   const char * text)
{
    if (cache == NULL || cache->buffer == NULL) {
        return;
    }

    /* Завершаем отрисовку в кэш-текстуру */
    __gui_cache_texture_end(cache->buffer);

    /* Обновляем состояние кэша */
    cache->valid = 1;
    cache->state = state;
    cache->flags = flags;
    if (text != NULL) {
        strncpy(cache->text, text, TEXT_ARRAY_SIZE - 1);
        cache->text[TEXT_ARRAY_SIZE - 1] = '\0';
    } else {
        cache->text[0] = '\0';
    }
}

/* ============================================================================
 * ОТРИСОВКА КЭША НА ЭКРАН
 * ========================================================================== */

void gui_objects_cache_draw(gui_objects_cache_t * cache,
                             uint16_t dst_x,
                             uint16_t dst_y)
{
    if (cache == NULL || cache->buffer == NULL || !cache->valid) {
        return;
    }

    /* Отрисовываем кэш-текстуру на экран в указанных координатах */
    __gui_cache_texture_draw(cache->buffer, dst_x, dst_y);
}

/* ============================================================================
 * ОБНОВЛЕНИЕ СОСТОЯНИЯ
 * ========================================================================== */

void gui_objects_cache_update_state(gui_objects_cache_t * cache,
                                     uint8_t state,
                                     uint8_t flags,
                                     const char * text)
{
    if (cache == NULL) {
        return;
    }
    cache->state = state;
    cache->flags = flags;
    if (text != NULL) {
        strncpy(cache->text, text, TEXT_ARRAY_SIZE - 1);
        cache->text[TEXT_ARRAY_SIZE - 1] = '\0';
    } else {
        cache->text[0] = '\0';
    }
}

/* ============================================================================
 * ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
 * ========================================================================== */

void gui_objects_cache_set_custom_data(gui_objects_cache_t * cache,
                                        void * data,
                                        uint32_t hash)
{
    if (cache == NULL) {
        return;
    }
    if (cache->custom_data != NULL) {
        free(cache->custom_data);
    }
    cache->custom_data = data;
    cache->custom_hash = hash;
}

void * gui_objects_cache_get_custom_data(gui_objects_cache_t * cache,
                                          uint32_t * hash)
{
    if (cache == NULL) {
        if (hash) *hash = 0;
        return NULL;
    }
    if (hash) *hash = cache->custom_hash;
    return cache->custom_data;
}

#endif /* WITHTOUCHGUI && GUI_USE_CACHE */
