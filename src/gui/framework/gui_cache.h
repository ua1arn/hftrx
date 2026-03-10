#ifndef GUI_CACHE_H_INCLUDED
#define GUI_CACHE_H_INCLUDED

#include "../gui_port_include.h"

#if WITHTOUCHGUI && GUI_USE_CACHE

#include "gui_settings.h"
#include "../gui_port.h"

/* ============================================================================
 * СТРУКТУРЫ КЭША
 * ========================================================================== */

/* Типы кэшируемых объектов */
typedef enum {
    GUI_CACHE_TYPE_NONE = 0,
    GUI_CACHE_TYPE_BUTTON,
    GUI_CACHE_TYPE_LABEL,
    GUI_CACHE_TYPE_ICON,
    GUI_CACHE_TYPE_CUSTOM
} gui_cache_type_t;

/* Структура кэша объекта (универсальная) */
typedef struct {
    gui_objbgbuf_t * buffer;          /* Кэш-буфер (абстракция через gui_port.h) */
    uint16_t w;                       /* Ширина */
    uint16_t h;                       /* Высота */
    uint8_t valid;                    /* Флаг: кэш действителен */
    gui_cache_type_t type;            /* Тип объекта */

    /* Состояние для проверки актуальности */
    uint8_t state;                    /* Состояние объекта (PRESSED, RELEASED, etc.) */
    uint8_t flags;                    /* Дополнительные флаги (is_locked, etc.) */
    char text[TEXT_ARRAY_SIZE];       /* Текст (для кнопок/меток) */

    /* Для будущей расширяемости */
    void * custom_data;               /* Пользовательские данные */
    uint32_t custom_hash;             /* Хэш для проверки изменений */
} gui_objects_cache_t;

/* ============================================================================
 * ФУНКЦИИ УПРАВЛЕНИЯ КЭШЕМ
 * ========================================================================== */

/* Создание кэша для объекта */
gui_objects_cache_t * gui_objects_cache_create(uint16_t w, uint16_t h, gui_cache_type_t type);

/* Уничтожение кэша */
void gui_objects_cache_destroy(gui_objects_cache_t * cache);

/* Инвалидация кэша (пометка на перерисовку) */
void gui_objects_cache_invalidate(gui_objects_cache_t * cache);

/* Проверка необходимости перерисовки */
int gui_objects_cache_needs_render(gui_objects_cache_t * cache,
                                    uint8_t state,
                                    uint8_t flags,
                                    const char * text);

/* Начало рендеринга в кэш */
int gui_objects_cache_begin_render(gui_objects_cache_t * cache);

/* Завершение рендеринга в кэш */
void gui_objects_cache_end_render(gui_objects_cache_t * cache,
                                   uint8_t state,
                                   uint8_t flags,
                                   const char * text);

/* Отрисовка кэша на экран */
void gui_objects_cache_draw(gui_objects_cache_t * cache,
                            uint16_t dst_x,
                            uint16_t dst_y);

/* Обновление состояния кэша */
void gui_objects_cache_update_state(gui_objects_cache_t * cache,
                                     uint8_t state,
                                     uint8_t flags,
                                     const char * text);

/* Очистка кэша (без уничтожения) */
void gui_objects_cache_clear(gui_objects_cache_t * cache);

/* Проверка валидности кэша */
static inline int gui_objects_cache_is_valid(gui_objects_cache_t * cache)
{
    return (cache != NULL && cache->buffer != NULL && cache->valid);
}

/* Получение размеров кэша */
static inline void gui_objects_cache_get_size(gui_objects_cache_t * cache,
                                              uint16_t * w,
                                              uint16_t * h)
{
    if (cache && w) *w = cache->w;
    if (cache && h) *h = cache->h;
}

#endif /* WITHTOUCHGUI && GUI_USE_CACHE */

#endif /* GUI_CACHE_H_INCLUDED */
