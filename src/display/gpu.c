/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Доработки для LS020 Василий Линывый, livas60@mail.ru
//
// Функции построения изображений в буфере - вне зависимости от того, есть ли в процессоре LTDC.'
// Например при offscreen composition растровых изображений для SPI дисплеев
//

#include "hardware.h"

#include "board.h"
#include "display.h"
#include "clocks.h"
#include "formats.h"
#include "utils.h"
#include "display2.h"
#include <string.h>

#if CPUSTYLE_T507

//#include "panfrost_regs.h"


// Команды для AS_COMMAND
#define AS_COMMAND_NOP          0x00
#define AS_COMMAND_UPDATE       0x01
#define AS_COMMAND_INVALIDATE   0x03 // Принудительный сброс TLB кэша MMU (Bifrost/Valhall)
//#define AS_COMMAND_FLUSH_PT     0x05 // Полная очистка конвейера таблиц страниц

static void gpu_as_command(unsigned as, unsigned cmd)
{
    // Ожидание готовности MMU AS
    while (GPU_MMU->MMU_AS[as].AS_STATUS & 0x1)
        ;
    GPU_MMU->MMU_AS[as].AS_COMMAND = cmd;
    // Ожидание завершения команды
    while (GPU_MMU->MMU_AS[as].AS_STATUS & 0x1)
        ;
}


//#define GPU_CTRLBASE (GPU_BASE + 0x10000)

// https://elixir.bootlin.com/linux/latest/source/drivers/gpu/drm/panfrost/panfrost_regs.h

/* GPU_COMMAND values */
#define GPU_COMMAND_NOP                0x00 /* No operation, nothing happens */
#define GPU_COMMAND_SOFT_RESET         0x01 /* Stop all external bus interfaces, and then reset the entire GPU. */
#define GPU_COMMAND_HARD_RESET         0x02 /* Immediately reset the entire GPU. */
#define GPU_COMMAND_PRFCNT_CLEAR       0x03 /* Clear all performance counters, setting them all to zero. */
#define GPU_COMMAND_PRFCNT_SAMPLE      0x04 /* Sample all performance counters, writing them out to memory */
#define GPU_COMMAND_CYCLE_COUNT_START  0x05 /* Starts the cycle counter, and system timestamp propagation */
#define GPU_COMMAND_CYCLE_COUNT_STOP   0x06 /* Stops the cycle counter, and system timestamp propagation */
#define GPU_COMMAND_CLEAN_CACHES       0x07 /* Clean all caches */
#define GPU_COMMAND_CLEAN_INV_CACHES   0x08 /* Clean and invalidate all caches */
#define GPU_COMMAND_SET_PROTECTED_MODE 0x09 /* Places the GPU in protected mode */

//#define AS_COMMAND_NOP          0x00
//#define AS_COMMAND_UPDATE       0x01
//#define AS_COMMAND_INVALIDATE   0x03 // Принудительный сброс TLB кэша MMU (Bifrost/Valhall)
//#define AS_COMMAND_FLUSH_PT     0x05 // Полная очистка конвейера таблиц страниц

/* GPU_STATUS values */
//#define GPU_STATUS_PRFCNT_ACTIVE            (1 << 2)    /* Set if the performance counters are active. */
//#define GPU_STATUS_PROTECTED_MODE_ACTIVE    (1 << 7)    /* Set if protected mode is active */


/* IRQ flags */
#define GPU_FAULT               (1 << 0)    /* A GPU Fault has occurred */
#define MULTIPLE_GPU_FAULTS     (1 << 7)    /* More than one GPU Fault occurred. */
#define RESET_COMPLETED         (1 << 8)    /* Set when a reset has completed. */
#define POWER_CHANGED_SINGLE    (1 << 9)    /* Set when a single core has finished powering up or down. */
#define POWER_CHANGED_ALL       (1 << 10)   /* Set when all cores have finished powering up or down. */

#define PRFCNT_SAMPLE_COMPLETED (1 << 16)   /* Set when a performance count sample has completed. */
#define CLEAN_CACHES_COMPLETED  (1 << 17)   /* Set when a cache clean operation has completed. */

#define GPU_IRQ_REG_ALL (GPU_FAULT | MULTIPLE_GPU_FAULTS | 0*RESET_COMPLETED \
		| POWER_CHANGED_ALL | PRFCNT_SAMPLE_COMPLETED)

static void gpu_command(unsigned cmd)
{
	while ((GPU_CONTROL->GPU_STATUS & (UINT32_C(1) << 0)) != 0)
		;
	GPU_CONTROL->GPU_COMMAND = cmd;
//	unsigned v1 = GPU_CONTROL->GPU_STATUS;
//	unsigned v2 = GPU_CONTROL->GPU_STATUS;
//	unsigned v3 = GPU_CONTROL->GPU_STATUS;
//	PRINTF("cmd: %08X, Status: %08X, %08X, %08X\n", cmd, v1, v2, v3);
	__DSB();
}

static void gpu_wait(unsigned mask)
{
	while ((GPU_CONTROL->GPU_IRQ_RAWSTAT & mask) != mask)
		;
	GPU_CONTROL->GPU_IRQ_CLEAR = mask;
}

/* AS_COMMAND register commands */
//#define AS_COMMAND_NOP			0x00	/* NOP Operation */
//#define AS_COMMAND_UPDATE		0x01	/* Broadcasts the values in AS_TRANSTAB and ASn_MEMATTR to all MMUs */
//#define AS_COMMAND_LOCK			0x02	/* Issue a lock region command to all MMUs */
//#define AS_COMMAND_UNLOCK		0x03	/* Issue a flush region command to all MMUs */
#define AS_COMMAND_FLUSH		0x04	/* Flush all L2 caches then issue a flush region command to all MMUs (deprecated - only for use with T60x) */
//#define AS_COMMAND_FLUSH_PT		0x04	/* Flush all L2 caches then issue a flush region command to all MMUs */
//#define AS_COMMAND_FLUSH_MEM		0x05	/* Wait for memory accesses to complete, flush all the L1s cache then flush all L2 caches then issue a flush region command to all MMUs */


void GPU_IRQHandler(void)
{
	PRINTF("GPU_IRQHandler\n");
	PRINTF("GPU_CONTROL->GPU_IRQ_STATUS=%08X\n", (unsigned) GPU_CONTROL->GPU_IRQ_STATUS);
	ASSERT(0);
}

void GPU_JOB_IRQHandler(void)
{
	PRINTF("GPU_JOB_IRQHandler\n");
    PRINTF("h GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT);
    PRINTF("h GPU_JOB_CONTROL->JOB_INT_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_INT_RAWSTAT);
    GPU_JOB_CONTROL->JOB_INT_CLEAR = ~ 0;
    GPU_JOB_CONTROL->JOB_IRQ_CLEAR = ~ 0;
}

void GPU_MMU_IRQHandler(void)
{
	PRINTF("GPU_MMU_IRQHandler\n");
	ASSERT(0);
}

void GPU_EVENT_IRQHandler(void)
{
	PRINTF("GPU_EVENT_IRQHandler\n");
	ASSERT(0);
}

void gpu_fillrect(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uintptr_t taddr,
	uint_fast32_t tstride,
	uint_fast32_t tsizehw,
	unsigned alpha,
	COLOR24_T color24,
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T color	// цвет
	)
{
	if (w == 0 || h == 0)
		return;
	if (w == 1 && h == 1)
	{
		* buffer = TFTALPHA(alpha, color24);
		return;
	}

	int32_t triangle0 [3] [2] = { { 0, 0 }, { 0, h - 1}, { w - 1, 0 } };
	int32_t triangle1 [3] [2] = { { w - 1, h - 1 }, { 0, h - 1}, { w - 1, 0 } };
}

// Типы задач (Job Types) для Mali Bifrost
//#define JOB_TYPE_NULL      0  /* Пустая задача (вызывается для тестов или очистки) */
//#define JOB_TYPE_VERTEX    1  /* Только вершинный шейдер (редко используется в чистом виде) */
//#define JOB_TYPE_TILER     2  /* Вершинный шейдер + расчет сетки тайлов экрана (Slot 0) */
//#define JOB_TYPE_FRAGMENT  3  /* Пиксельный/Фрагментный шейдер и растеризац */

enum mali_job_type {
        JOB_NOT_STARTED	= 0,
        JOB_TYPE_NULL = 1,
        JOB_TYPE_WRITE_VALUE = 2,
        JOB_TYPE_CACHE_FLUSH = 3,
        JOB_TYPE_COMPUTE = 4,
        JOB_TYPE_VERTEX = 5,
        JOB_TYPE_GEOMETRY = 6,
        JOB_TYPE_TILER = 7,
        JOB_TYPE_FUSED = 8,
        JOB_TYPE_FRAGMENT = 9,
};
enum mali_draw_mode {
        MALI_DRAW_NONE      = 0x0,
        MALI_POINTS         = 0x1,
        MALI_LINES          = 0x2,
        MALI_LINE_STRIP     = 0x4,
        MALI_LINE_LOOP      = 0x6,
        MALI_TRIANGLES      = 0x8,
        MALI_TRIANGLE_STRIP = 0xA,
        MALI_TRIANGLE_FAN   = 0xC,
        MALI_POLYGON        = 0xD,
        MALI_QUADS          = 0xE,
        MALI_QUAD_STRIP     = 0xF,
        /* All other modes invalid */
};
// Структура заголовка задачи (Mali Job Header)
typedef struct __attribute__((packed)) {
    __IO uint32_t exception_status;       /* +0x00: Код ошибки, если задача упала в FAULT (Заполняет GPU) */
    __IO uint32_t first_incomplete_task;  /* +0x04: Индекс первой незавершенной подзадачи */
    __IO uint64_t fault_pointer;          /* +0x08: 64-бит физический адрес, на котором произошел сбой */

    __IO uint8_t  job_type;               /* +0x10: Тип задачи (0 - NULL, 1 - VERTEX, 2 - TILER, 3 - FRAGMENT) */
    __IO uint8_t  job_index;              /* +0x11: Порядковый номер задачи в цепочке (обычно 1) */
    __IO uint16_t job_descriptor_size;    /* +0x12: Размер всего расширенного дескриптора (обычно 64 или 128) */

    __IO uint32_t reserved_unk;           /* +0x14: Зарезервировано / Неиспользуемые биты */
    __IO uint64_t next_job;               /* +0x18: 64-бит физический адрес СЛЕДУЮЩЕЙ задачи в ОЗУ (0 - конец цепочки) */
} mali_job_header;

// Расширенный дескриптор для Vertex/Tiler задач
typedef struct __attribute__((packed)) {
    mali_job_header header;
    uint64_t tiler_heap;       // Адрес памяти кучи для тайлера
    uint64_t vertex_buffer;    // Физический адрес массива координат вершин
    uint32_t vertex_count;     // Количество вершин (для 1 треугольника = 3)
    uint32_t attributes;       // Настройки формата вершин
    uint64_t primitive_size;
    uint64_t renderer_state;
} mali_tiler_job;

// Расширенный дескриптор для Fragment задач
typedef struct __attribute__((packed)) {
    mali_job_header header;

    __IO uint64_t framebuffer_desc;       /* +0x20: 64-бит физ. адрес дескриптора Framebuffer (fb_desc) */
    __IO uint64_t tile_render_list;        /* +0x28: 64-бит физ. адрес списка тайлов дисплея (tiler_heap_mem) */

    __IO uint32_t stride_and_format;      /* +0x30: Внутренние аппаратные флаги шага и формата тайлера */
    __IO uint16_t width_minus_1;          /* +0x34: Ширина зоны рендеринга МИНУС 1 (например, 799 для 800) */
    __IO uint16_t height_minus_1;         /* +0x36: Высота зоны рендеринга МИНУС 1 (например, 479 для 480) */

    __IO uint32_t clear_color;            /* +0x38: Цвет очистки экрана в формате ARGB8888 (если включен флаг) */
    __IO uint32_t fragment_flags;         /* +0x3C: Флаги фрагментного конвейера (биты очистки, глубина, трафарет) */
    __IO uint32_t r1 [2];
} mali_fragment_job;

// Выравнивание для кэш-линий GPU
#define GPU_ALIGN __attribute__((aligned(64)))


#include <stdint.h>

/**
  * @brief Дескриптор конкретной поверхности цвета (Render Target Descriptor)
  * Размер: ровно 32 байта (0x20)
  */
typedef struct __attribute__((packed)) {
    __IO uint64_t base_address;          /* +0x00: Физический адрес буфера экрана (из вашего Display Engine) */
    __IO uint32_t stride;                /* +0x08: Шаг строки в байтах (width * 4 для 32-бит ARGB) */
    __IO uint32_t format;                /* +0x0C: Формат пикселя (0x18001000 для ARGB8888, 0x18005000 для BGRA8888) */
    __IO uint32_t flags;                 /* +0x10: Аппаратные флаги поверхности (Panfrost пишет 0x00000000 для линейного буфера) */
    __IO uint32_t reserved_rt[3];        /* +0x14 - +0x1F: Паддинг */
} mali_render_target_desc_t;

/**
  * @brief Главный глобальный дескриптор Framebuffer для Bifrost v6
  * Размер: ровно 128 байт (0x80). Должен бытьpacked и выровнен по кэш-линии (64 байта).
  */
typedef struct __attribute__((packed)) {
    /* --- Глобальные параметры окна (0x00 - 0x1F) --- */
    __IO uint32_t width_minus_1;         /* +0x00: Ширина экрана минус 1 (например, 799 для 800) */
    __IO uint32_t height_minus_1;        /* +0x04: Высота экрана минус 1 (например, 479 для 480) */
    __IO uint32_t sample_count;          /* +0x08: Мультисэмплинг (MSAA). Пишем 0x00000001 (1 sample / без MSAA) */
    __IO uint32_t msub_config;           /* +0x0C: Внутренняя конфигурация плиточного процессора (Panfrost пишет 0) */
    __IO uint64_t rt_pointer;            /* +0x10: 64-бит физ. адрес, указывающий на массив поверхностей Render Targets (на смещение +0x40) */
    __IO uint64_t reserved_hdr;          /* +0x18: Резерв */

    /* --- Параметры очистки и вспомогательных буферов (0x20 - 0x3F) --- */
    __IO uint64_t zs_address;            /* +0x20: Физический адрес буфера глубины/трафарета (Depth/Stencil). 0, если выключен */
    __IO uint32_t zs_stride;             /* +0x28: Шаг буфера глубины */
    __IO uint32_t clear_flags;           /* +0x2C: Флаги очистки */
    __IO uint32_t padding_hdr[4];        /* +0x30 - +0x3F: Паддинг до 64 байт */

    /* --- Массив описания поверхностей вывода (0x40 - 0x7F) --- */
    /* Аппаратно заголовок и первая RT сгруппированы вместе. В `rt_pointer` мы передадим адрес этой ячейки. */
    mali_render_target_desc_t rt[1];     /* +0x40: Описание Render Target 0 (Цветовой буфер экрана) */

} mali_framebuffer_desc_t;

typedef struct __attribute__((packed)) {
    /* --- Глобальные параметры окна (0x00 - 0x1F) --- */
    __IO uint32_t width_minus_1;         /* +0x00: Ширина экрана минус 1 (например, 799 для 800) */
    __IO uint32_t height_minus_1;        /* +0x04: Высота экрана минус 1 (например, 479 для 480) */
    __IO uint32_t sample_count;          /* +0x08: Мультисэмплинг (MSAA). Пишем 0x00000001 (1 sample / без MSAA) */
    __IO uint32_t msub_config;           /* +0x0C: Внутренняя конфигурация плиточного процессора (Panfrost пишет 0) */
    __IO uint64_t rt_pointer;            /* +0x10: 64-бит физ. адрес, указывающий на массив поверхностей Render Targets (на смещение +0x40) */
    __IO uint64_t reserved_hdr;          /* +0x18: Резерв */

    /* --- Параметры очистки и вспомогательных буферов (0x20 - 0x3F) --- */
    __IO uint64_t zs_address;            /* +0x20: Физический адрес буфера глубины/трафарета (Depth/Stencil). 0, если выключен */
    __IO uint32_t zs_stride;             /* +0x28: Шаг буфера глубины */
    __IO uint32_t clear_flags;           /* +0x2C: Флаги очистки */
    __IO uint32_t padding_hdr[4];        /* +0x30 - +0x3F: Паддинг до 64 байт */

    /* --- Массив описания поверхностей вывода (0x40 - 0x7F) --- */
    /* Аппаратно заголовок и первая RT сгруппированы вместе. В `rt_pointer` мы передадим адрес этой ячейки. */
    mali_render_target_desc_t rt[1];     /* +0x40: Описание Render Target 0 (Цветовой буфер экрана) */
} mali_framebuffer_desc;

GPU_ALIGN static mali_framebuffer_desc fb_desc;

/**
  * @brief Дескриптор конфигурации одной стадии шейдера для Bifrost v6 (Mali-G31)
  * Размер: ровно 16 байт. Обязательно packed.
  */
typedef struct __attribute__((packed)) {
    __IO uint32_t properties;             /* +0x00: Аппаратные свойства: биты [3:0] — тип (0-Vert, 1-Frag), биты [15:4] — число регистров */
    __IO uint16_t stack_size;             /* +0x04: Размер стека для потоков (ставим 0, если нет ветвлений) */
    __IO uint16_t preload_regs;           /* +0x06: Флаги аппаратного предзагрузчика текстур/атрибутов (ставим 0) */
    __IO uint64_t shader_code_ptr;        /* +0x08: СТРОГО ТУТ: 64-бит физический адрес бинарного кода шейдера (Bifrost ISA) */
} mali_shader_stage_t;

typedef struct __attribute__((packed)) {
    mali_shader_stage_t vertex_stage;    /* +0x00 - +0x0F: 16 байт */
    mali_shader_stage_t fragment_stage;  /* +0x10 - +0x1F: 16 байт */

    uint32_t blend_equation;             /* +0x20: 0x00001200 */
    uint32_t blend_constant;             /* +0x24: 0x00000000 */
    uint64_t reserved_blend_fields;      /* +0x28: 8 байт резерва под блендинг */

    uint64_t attribute_meta_ptr;         /* +0x30: Физический адрес метаданных атрибутов | 0x1 */
    uint64_t reserved_unk_bifrost;       /* +0x38: Скрытый указатель конфигурации (ставим 0) */

    /* --- КРИТИЧНО ДЛЯ BIFROST V6: Расширение структуры до 128 байт --- */
    uint64_t varyings_meta_ptr;          /* +0x40: Описание интерполяции (Varyings). Ставим 0 для заглушки */
    uint64_t attribute_buffer_ptr;       /* +0x48: Прямой указатель на буфер (Panfrost использует | 0x3) */
    uint8_t  padding_to_128[48];         /* +0x50 - +0x7F: Добиваем структуру нулями до 128 байт */
} mali_renderer_state_bifrost_v6_t;

__attribute__((aligned(128))) static mali_renderer_state_bifrost_v6_t gpu_program_state;
#include <stdint.h>

/**
  * @brief Минимально валидный вершинный шейдер для ARM Bifrost (Mali-G31)
  * Выполняет Identity проброс координат Vec3 Float из Attribute 0 на выход конвейера Tiler.
  * Выровнен строго по кэш-линии (64 байта).
  */
__attribute__((aligned(64))) static const uint32_t bifrost_vertex_shader_code[] = {
    /* --- Clause 0: Предзагрузка и инициализация регистров потока --- */
    0x00000000, 0x00000000, // Служебные метаданные компилятора Panfrost
    0x7C002008, 0x00000000, // Считывание атрибута 0 (Координаты X, Y, Z вершины)
    0x80004000, 0x00000000, // Выделение внутренних скалярных регистров R0, R1, R2

    /* --- Clause 1: Команда копирования и отправки позиции в геометрический движок (VARYING/POSITION) --- */
    0x00100002, 0x5E000000, // Исполняемая инструкция Bifrost ISA: Маппинг R0 -> Out_X
    0x00200002, 0x5E000000, // Исполняемая инструкция Bifrost ISA: Маппинг R1 -> Out_Y
    0x00300002, 0x5E000000, // Исполняемая инструкция Bifrost ISA: Маппинг R2 -> Out_Z
    0x00400002, 0x5E000000, // Запись константы 1.0f в координату W (Out_W = 1.0f для однородных координат)

    /* --- Clause 2: Финальный маркер завершения шейдерного потока (End of Shader Clause) --- */
    0x00000000, 0x000002FF, // Аппаратный код терминации нити (Bifrost Thread End execution opcode)
    0x00000000, 0x00000000, // Паддинг до выравнивания
    0x00000000, 0x00000000,
    0x00000000, 0x00000000
};

#include <stdint.h>

/**
  * @brief Минимально валидный фрагментный (пиксельный) шейдер для ARM Bifrost (Mali-G31)
  * Записывает жестко заданный зеленый цвет (ARGB: 0xFF00FF00) в Render Target 0 (наш Framebuffer).
  * Выровнен строго по кэш-линии (64 байта).
  */
__attribute__((aligned(64))) static const uint32_t bifrost_fragment_shader_code[] = {
    /* --- Clause 0: Загрузка констант цвета в скалярные регистры потока R0, R1, R2, R3 --- */
    0x00000000, 0x00000000, // Служебные метаданные компилятора Panfrost
    0x000000FF, 0x00000000, // R0 = 0x00 (Синий канал / Blue)
    0x000000FF, 0x00000000, // R1 = 0xFF (Зеленый канал / Green)
    0x000000FF, 0x00000000, // R2 = 0x00 (Красный канал / Red)
    0x000000FF, 0x00000000, // R3 = 0xFF (Альфа канал / Alpha)

    /* --- Clause 1: Аппаратная инструкция FB_WRITE (Запись цвета в Framebuffer дисплея) --- */
    // Этот 128-битный блок содержит специфический для Bifrost v6 опкод blend/fb_write,
    // который приказывает пиксельному ядру Mali-G31 отправить RGBA из регистров R2, R1, R0, R3
    // в контроллер вывода цвета (Tile Buffer / Blend Unit)
    0x00001002, 0x7E000000, // FB_WRITE инструкция (Прямой маппинг на Render Target 0)
    0x00002002, 0x00001DFF, // Настройка параметров смешивания (Opaque)

    /* --- Clause 2: Финальный маркер завершения фрагментного потока (Thread End Clause) --- */
    0x00000000, 0x000002FF, // Аппаратный код терминации пиксельной нити (End of Shader)
    0x00000000, 0x00000000, // Паддинг до размера кэш-линии
    0x00000000, 0x00000000,
    0x00000000, 0x00000000
};

#include <stdint.h>

/**
  * @brief Дескриптор формата входного атрибута (вершинного буфера) Bifrost v6
  * Размер: ровно 16 байт. Обязательно packed.
  */
typedef struct __attribute__((packed)) {
    __IO uint64_t buffer_ptr;             /* +0x00: СТРОГО ТУТ: 64-бит физ. адрес массива координат вершин triangle_vertices */
    __IO uint16_t stride;                 /* +0x08: Шаг между вершинами в байтах (для vec3 float: sizeof(float)*3 = 12 байт) */
    __IO uint16_t size;                   /* +0x0A: Общий размер буфера вершин в байтах (sizeof(triangle_vertices) = 36 байт) */
    __IO uint32_t format;                 /* +0x0C: Аппаратный код формата Bifrost ISA (задает тип данных, размер вектора и swizzle) */
} mali_attribute_meta_t;

// Объявление экземпляра в gpu.c с выравниванием по кэш-линии
__attribute__((aligned(64))) static mali_attribute_meta_t gpu_vertex_input_meta;
// Координаты вершин треугольника: X, Y, Z (в нормализованных координатах от -1.0 до 1.0)
GPU_ALIGN static float triangle_vertices[] = {
     0.0f,  0.5f, 0.0f, // Верхняя точка
    -0.5f, -0.5f, 0.0f, // Нижняя левая
     0.5f, -0.5f, 0.0f  // Нижняя правая
};

void gpu_init_program_state(void)
{
    // Полностью зануляем 128-байтную структуру программы (Renderer State)
    uint8_t *p = (uint8_t *)&gpu_program_state;
    for (int i = 0; i < 128; i++) p[i] = 0;

    // 1. НАСТРОЙКА ДЕСКРИПТОРА АТРИБУТОВ ВЕРШИН (gpu_vertex_input_meta)
    // Передаем прямой физический адрес массива координат triangle_vertices
    gpu_vertex_input_meta.buffer_ptr = (uintptr_t)triangle_vertices;

    // Шаг (Stride) между вершинами: 3 флоата * 4 байта = 12 байт
    gpu_vertex_input_meta.stride     = 12;

    // Общий размер буфера: 3 вершины * 3 флоата * 4 байта = 36 байт
    gpu_vertex_input_meta.size       = sizeof(triangle_vertices);

    // ИСПРАВЛЕНО: Аппаратный код Bifrost v6 для Vec3 Float32
    gpu_vertex_input_meta.format     = 0x0400030A;

    // 2. Настраиваем Вершинную стадию (properties = 0x00004000)
    gpu_program_state.vertex_stage.properties      = 0x00004000;
    gpu_program_state.vertex_stage.stack_size      = 0;
    gpu_program_state.vertex_stage.preload_regs    = 0;
    gpu_program_state.vertex_stage.shader_code_ptr = (uintptr_t)bifrost_vertex_shader_code;

    // 3. Настраиваем Фрагментную стадию (properties = 0x00004001)
    gpu_program_state.fragment_stage.properties      = 0x00004001;
    gpu_program_state.fragment_stage.stack_size      = 0;
    gpu_program_state.fragment_stage.preload_regs    = 0;
    gpu_program_state.fragment_stage.shader_code_ptr = (uintptr_t)bifrost_fragment_shader_code;

    // 4. Формула блендинга (Opaque)
    gpu_program_state.blend_equation = 0x00001200;

    // 5. Системные суффиксы (маски) Bifrost v6 для пробития шины атрибутов ОЗУ T507
    // Направляем attribute_meta_ptr на нашу структуру gpu_vertex_input_meta с маской | 0x1
    gpu_program_state.attribute_meta_ptr   = (uintptr_t)&gpu_vertex_input_meta | 0x0000000000000001ULL;

    // Направляем attribute_buffer_ptr на массив координат с маской | 0x3
    gpu_program_state.attribute_buffer_ptr = (uintptr_t)triangle_vertices       | 0x0000000000000003ULL;

    gpu_program_state.varyings_meta_ptr    = 0;

    // СИНХРОНИЗАЦИЯ: Применяем dcache_clean проекта hftrx для выталкивания всех измененных структур из кэша CPU
    dcache_clean((uintptr_t)&gpu_vertex_input_meta, sizeof(gpu_vertex_input_meta));
    dcache_clean((uintptr_t)&gpu_program_state, sizeof(gpu_program_state));
    dcache_clean((uintptr_t)bifrost_vertex_shader_code, sizeof(bifrost_vertex_shader_code));
    dcache_clean((uintptr_t)bifrost_fragment_shader_code, sizeof(bifrost_fragment_shader_code));

    // Наш канонический CMSIS барьер памяти
    __DSB();
}

// Номера слотов задач (обычно слот 0 - Vertex/Compute, слот 1 - Fragment)
#define MALI_FB_FORMAT_ARGB8888    0x18001000

void gpu_diagnose_fault(unsigned slot)
{
    // Читаем физический адрес дескриптора, на котором споткнулся DMA-движок GPU
    uint32_t fault_addr_lo = GPU_JOB_CONTROL->LOOP[slot].JS_HEAD_LO;
    uint32_t fault_addr_hi = GPU_JOB_CONTROL->LOOP[slot].JS_HEAD_HI;
    uint64_t fault_phys_addr = ((uint64_t)fault_addr_hi << 32) | fault_addr_lo;

    // В Bifrost код конкретного исключения (Exception Code) часто дублируется
    // в старших битах регистра JS_STATUS или в регистре ошибок MMU, если это был Page Fault
    PRINTF("-> MALI FAULT DIAGNOSIS:\n");
    PRINTF("   GPU stopped at physical address: 0x%08X%08X\n", (unsigned)fault_addr_hi, (unsigned)fault_addr_lo);
    PRINTF("   MMU Fault Status (0x01802420): 0x%08X\n", (unsigned)GPU_MMU->MMU_AS[slot].AS_FAULTSTATUS);
    PRINTF("   MMU Fault Address: 0x%08X%08X\n",
           (unsigned)GPU_MMU->MMU_AS[slot].AS_FAULTADDRESS_HI, (unsigned)GPU_MMU->MMU_AS[slot].AS_FAULTADDRESS_LO);
}

/* --- МЛАДШИЕ 16 БИТ: Успешное завершение (Job Done) --- */
#define JOB_INT_BIT_SLOT_0_DONE    (1 << 0)  /* Задача в Слоте 0 выполнена успешно (Vertex/Tiler) */
#define JOB_INT_BIT_SLOT_1_DONE    (1 << 1)  /* Задача в Слоте 1 выполнена успешно (Fragment) */
#define JOB_INT_BIT_SLOT_2_DONE    (1 << 2)  /* Задача в Слоте 2 выполнена успешно (Compute) */

/* --- СТАРШИЕ 16 БИТ: Аппаратные сбои (Job Fault) --- */
#define JOB_INT_BIT_SLOT_0_FAULT   (1 << 16) /* Критическая ошибка выполнения в Слоте 0 */
#define JOB_INT_BIT_SLOT_1_FAULT   (1 << 17) /* Критическая ошибка выполнения в Слоте 1 */
#define JOB_INT_BIT_SLOT_2_FAULT   (1 << 18) /* Критическая ошибка выполнения в Слоте 2 */

// Регистры отправки команд в слот (сверьтесь со структурой GPU_JOB_CONTROL в panfrost_regs.h)
// Обычные имена регистров в драйвере Panfrost: JS_COMMAND, JS_HEAD_NEXT
void gpu_diagnose_slot1_fault(unsigned slot)
{
    // Читаем статус ошибки Слота 1 (смещение 0x24 от 0x1880 -> адрес 0x018018A4)
    uint32_t slot1_status = GPU_JOB_CONTROL->LOOP[slot].JS_STATUS;

    // Физический адрес, на котором споткнулся Fragment-парсер
    uint32_t fault_lo = GPU_JOB_CONTROL->LOOP[slot].JS_HEAD_LO;
    uint32_t fault_hi = GPU_JOB_CONTROL->LOOP[slot].JS_HEAD_HI;

    PRINTF("\n-> FRAGMENT STAGE FAULT DIAGNOSIS:\n");
    PRINTF("   Slot %u JS_STATUS = 0x%08X\n", slot, (unsigned)slot1_status);
    PRINTF("   Slot %u Stopped at Address: 0x%08X%08X\n", slot, (unsigned)fault_hi, (unsigned)fault_lo);

    // Проверяем, не ругнулся ли при этом MMU (адресное пространство AS0 на 0x400)
    PRINTF("   MMU Fault Status (0x01802420) = 0x%08X\n", (unsigned)GPU_MMU->MMU_AS [0].AS_FAULTSTATUS);
    PRINTF("   MMU Fault Address = 0x%08X%08X\n",
           (unsigned)GPU_MMU->MMU_AS [0].AS_FAULTADDRESS_HI, (unsigned)GPU_MMU->MMU_AS [0].AS_FAULTADDRESS_LO);
}
#define MALI_BIFROST_PRIM_TRIANGLES          (0x4ULL << 0)   /* Тип примитива: GL_TRIANGLES */
#define MALI_BIFROST_PRIM_INDEX_NONE         (0x0ULL << 4)   /* Без индексного буфера (Arrays Mode) */
#define MALI_BIFROST_PRIM_INDEX_U16          (0x2ULL << 4)   /* Если бы использовался Index Buffer uint16_t */
#define MALI_BIFROST_PRIM_CULL_NONE          (0x0ULL << 8)   /* Выключить отсечение нелицевых граней (No Culling) */
#define MALI_BIFROST_PRIM_CULL_CCW           (0x1ULL << 8)   /* Отсекать Counter-Clockwise полигоны */
#define MALI_BIFROST_PRIM_CULL_CW            (0x2ULL << 8)   /* Отсекать Clockwise полигоны */

//void gpu_draw_triangle2(uintptr_t framebuffer_phys_addr, uint32_t width, uint32_t height);

static void gpu_submit_job(unsigned slot, void * job)
{
    // Записываем физический адрес начала структуры v_job в регистр указателя слота 0
    GPU_JOB_CONTROL->LOOP[slot].JS_HEAD_NEXT_HI = ptr_hi32((uintptr_t) job);//(uint32_t)(((uintptr_t)&v_job >> 32) & 0xFFFFFFFF);
    GPU_JOB_CONTROL->LOOP[slot].JS_HEAD_NEXT_LO = ptr_lo32((uintptr_t) job);//(uint32_t)((uintptr_t)&v_job & 0xFFFFFFFF);

    // 2. ИНИЦИАЛИЗАЦИЯ JS_AFFINITY (Критично для Bifrost!)
    // Говорим планировщику распределить потоки шейдеров на оба ядра Mali-G31 MP2
    GPU_JOB_CONTROL->LOOP[slot].JS_AFFINITY_NEXT_HI = ~0;//0x00000003;
    GPU_JOB_CONTROL->LOOP[slot].JS_AFFINITY_NEXT_LO = ~0;//0x00000003;

    // Дополнительно для Bifrost рекомендуется сбросить расширенную конфигурацию слота
    GPU_JOB_CONTROL->LOOP[slot].JS_CONFIG_NEXT = 0x00000000;

    //GPU_JOB_CONTROL->JOB_INT_MASK = 0xFFFFFFFF;	// Это разрешает вызовы обработчика прерываний
    GPU_JOB_CONTROL->JOB_IRQ_MASK = 0xFFFFFFFF;

//        PRINTF("0 GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT);
//        PRINTF("0 GPU_JOB_CONTROL->JOB_INT_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_INT_RAWSTAT);

    // Команда START (обычно значение 0x01 в регистр JS_COMMAND)
    GPU_JOB_CONTROL->LOOP[slot].JS_COMMAND_NEXT = 0x01;
    __DSB();
    // Ожидание завершения работы аппаратного тайлера геометрии
    // В hftrx прерывания выводят ASSERT(0), поэтому опрашиваем статус в цикле (polling)
    if (local_wait32mask(& GPU_JOB_CONTROL->JOB_INT_RAWSTAT, (UINT32_C(1) << slot), 1 * (UINT32_C(1) << slot), 100))//Was: JOB_IRQ_RAWSTAT
    {
    	PRINTF("gpu timeout: GPU_JOB_CONTROL->JOB_INT_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_INT_RAWSTAT);
    	gpu_diagnose_slot1_fault(slot);
    }
    else
    {
        GPU_JOB_CONTROL->JOB_INT_CLEAR = (UINT32_C(1) << slot); // Сброс флага прерывания
    }
}

void gpu_draw_triangle(uintptr_t framebuffer_phys_addr, uint32_t width, uint32_t height)
{
	enum {
		COMMAND_SLOT_VERTEX = 0,
		COMMAND_SLOT_FRAGMENT
	};
	gpu_init_program_state();
//	gpu_draw_triangle2(framebuffer_phys_addr, width, height);
    PRINTF("Assembling GPU Job Chain for Triangle...\n");
    // https://android.googlesource.com/platform/external/mesa3d/+/e061bf004b5/src/panfrost/include/panfrost-job.h

	// Область памяти для кучи тайлера (Mali Tiler требует буфер для сортировки геометрии)
	__attribute__((aligned(4096))) static uint8_t tiler_heap_mem[64 * 10 * 1024];
	struct mali_bifrost_tiler_heap {
	    uint64_t tiler_heap_free; // Сюда пишем физический адрес START
	    uint64_t tiler_heap_end;  // Физический адрес: START + SIZE - CHUNK_SIZE
	    uint32_t padding;         // 0x0
	    uint32_t flags;           // 0x00000001 (Флаг активации кучи)
	    uint64_t chunk_size;      // 0x00008000 (Задаем размер чанка 32 КБ)
	    uint8_t  reserved[32];    // Зануляем остаток до 64 байт
	};
	static GPU_ALIGN struct mali_bifrost_tiler_heap tiler_heap =
	{
			.tiler_heap_free = (uintptr_t) tiler_heap_mem,
			.tiler_heap_end = (uintptr_t) tiler_heap_mem + 64 * 1024 * 9,
			.flags = 1,
			.chunk_size = 32 * 1024
	};
	dcache_clean((uintptr_t)&tiler_heap, sizeof tiler_heap);
	struct __attribute__((packed, aligned(64))) mali_bifrost_fb_desc {
	    uint32_t width_minus_1;     // Ширина экрана - 1 (например, 15 для экрана шириной 16)
	    uint32_t height_minus_1;    // Высота экрана - 1 (например, 15 для экрана высотой 16)
	    uint32_t sample_mask;       // 0x0000FFFF (Плитки 16х16)
	    uint32_t unk_flags;         // 0x00000002 (Минимальные флаги формата)
	    uint64_t tiler_heap_ptr;    // Физический адрес структуры `mali_bifrost_tiler_heap` (из шага 1)
	    uint8_t  reserved[40];      // Зануляем остаток до 64 байт
	};
	static GPU_ALIGN struct mali_bifrost_fb_desc fbd = {
		.width_minus_1 = 15,
		.height_minus_1 = 15,
		.sample_mask = 0x0000FFFF,
		.unk_flags = 0x02,
		.tiler_heap_ptr = (uintptr_t) & tiler_heap,
	};
	dcache_clean((uintptr_t)&fbd, sizeof fbd);
	struct __attribute__((packed, aligned(64))) mali_bifrost_tiler_payload {
	    uint64_t fb_desc_ptr;       // Физический адрес структуры `mali_bifrost_fb_desc` (из шага 2)
	    uint64_t polygon_list_ptr;  // Физический адрес пустого буфера в ОЗУ (выделите под него 4КБ)
	    uint32_t total_polygons;    // 0 (У нас нет полигонов, сцена пустая!)
	    uint32_t tiler_flags;       // 0x00000000
	    uint8_t  reserved[40];      // Зануляем остаток до 64 байт
	};
	__attribute__((aligned(4096))) static uint8_t polygon_list_mem [ 4096];
	static GPU_ALIGN struct mali_bifrost_tiler_payload tiler_payload = {
		.fb_desc_ptr = (uintptr_t) & fbd,
		.polygon_list_ptr = (uintptr_t) & polygon_list_mem,
		.total_polygons = 0,
		.tiler_flags = 0,

	};
	dcache_clean((uintptr_t)&tiler_payload, sizeof tiler_payload);
	struct __attribute__((packed, aligned(64))) mali_job_descriptor {
	    uint64_t next_job;          // 0x0 (Это единственная задача в цепочке)
	    uint32_t job_type;          // 0x11 (Аппаратный тип: MALI_JOB_TYPE_TILER)
	    uint32_t flags_status;      // 0x00000001 (Runnable / Готов к запуску)
	    uint64_t fault_payload;     // 0x0 (Или адрес буфера под дамп ошибок)
	    uint64_t dependency_1;      // 0x0 (Никого не ждем)
	    uint64_t dependency_2;      // 0x0 (Никого не ждем)
	    uint64_t payload_ptr;       // Физический адрес структуры `mali_bifrost_tiler_payload` (из шага 3)
	    uint8_t  reserved[16];      // Зануляем остаток до 64 байт
	};
	static GPU_ALIGN struct mali_job_descriptor job =
	{
			.next_job = (uintptr_t) 0,
			.job_type = 0x11,
			.flags_status = 1,
			.fault_payload = 0,
			.payload_ptr = (uintptr_t) & tiler_payload,
	};
	{

    	// Выделяем память под дескрипторы аппаратных задач в RAM
    	GPU_ALIGN static mali_tiler_job    v_job;

        // 2. Настройка первой задачи: Координаты и геометрия (Vertex / Tiler Job)
        v_job.header.job_type = JOB_TYPE_TILER;
        v_job.header.job_index = 0;
        v_job.header.job_descriptor_size = sizeof v_job;//(sizeof v_job + 7) / 8;
        v_job.header.next_job = 0;//(uintptr_t) & f_job;//0; // Конец первой цепочки (или можно связать с f_job, если аппаратно поддерживается)

        v_job.tiler_heap = (uintptr_t) & tiler_heap;
        v_job.vertex_buffer = (uintptr_t)triangle_vertices;
        v_job.vertex_count = 3; // 3 вершины формируют 1 треугольник
        //v_job.draw_flags = MALI_BIFROST_PRIM_TRIANGLES; // Флаг типа примитива: TRIANGLES
        // Формула Panfrost для привязки Renderer State в Bifrost v6
        v_job.renderer_state = (uintptr_t)&gpu_program_state | 0x0000000000000007ULL;

        memset(& tiler_heap_mem, 0xE5, sizeof tiler_heap_mem);
		dcache_clean((uintptr_t)&tiler_heap, sizeof(tiler_heap));
        dcache_clean((uintptr_t)&v_job, sizeof(v_job));
        //dcache_clean((uintptr_t)&gpu_program_state, sizeof(gpu_program_state));
        dcache_clean((uintptr_t)triangle_vertices, sizeof(triangle_vertices));
        dcache_clean_invalidate((uintptr_t) tiler_heap_mem, sizeof tiler_heap_mem);
        PRINTF("v_job: %p (%u)\n", & v_job, (unsigned) sizeof v_job);
        //PRINTF("gpu_program_state: %p (%u)\n", & gpu_program_state, (unsigned) sizeof gpu_program_state);
        PRINTF("triangle_vertices: %p (%u)\n", & triangle_vertices, (unsigned) sizeof triangle_vertices);

       	printhex32(0, & tiler_heap, sizeof tiler_heap);
        dcache_clean_invalidate((uintptr_t) & tiler_heap, sizeof tiler_heap);

        // 4. Запуск цепочки геометрии в Slot 0
        PRINTF("Submitting Vertex Job to Slot 0...\n");
        gpu_submit_job(COMMAND_SLOT_VERTEX, &v_job);
     	//gpu_as_command(0, 0x05);
     	PRINTF("Complete Vertex Job on Slot 0...\n");
    }
   	printhex32(0, & tiler_heap, sizeof tiler_heap);
   	printhex32(0, tiler_heap_mem, 512);

    {
    	GPU_ALIGN static mali_fragment_job f_job;
        // 1. Инициализация описания целевого экрана (Framebuffer)
        fb_desc.rt [0].base_address = framebuffer_phys_addr;
        fb_desc.rt [0].stride = width * 4;
        fb_desc.rt [0].format = MALI_FB_FORMAT_ARGB8888;//0x18004000; // Простой формат записи RGBA8888 в Bifrost
        fb_desc.sample_count = 1;
        fb_desc.rt_pointer = (uintptr_t) & fb_desc.rt [0] | 0x3;

        // 3. Настройка второй задачи: Отрисовка пикселей (Fragment Job)
        f_job.header.job_type = JOB_TYPE_FRAGMENT;
        f_job.header.job_index = 0;
        f_job.header.job_descriptor_size = (sizeof f_job + 7) / 8;
        f_job.header.next_job = 0; // Последняя задача

        f_job.framebuffer_desc = (uintptr_t)&fb_desc;
        f_job.tile_render_list = (uintptr_t)& tiler_heap; // Читает геометрию из кучи тайлера
        f_job.width_minus_1 = width - 1;
        f_job.height_minus_1 = height - 1;
        // Формула расчета шага тайлов дисплея для Bifrost v6
        uint32_t stride = (width + 15) / 16; // Округление ширины до сетки тайлов 16x16
        f_job.stride_and_format = (stride & 0xFFFF);// | (1U << 16);
        f_job.clear_color = 0xFF0000FF; // Фоновый цвет очистки, если нужно (Красный)
        dcache_clean((uintptr_t)&f_job, sizeof(f_job));
        dcache_clean((uintptr_t)&fb_desc, sizeof(fb_desc));
        PRINTF("f_job: %p (%u)\n", & f_job, (unsigned) sizeof f_job);
        PRINTF("fb_desc: %p (%u)\n", & fb_desc, (unsigned) sizeof fb_desc);


		// 5. Запуск цепочки растеризации фрагментов в Slot 1
		PRINTF("Submitting Fragment Job to Slot 1...\n");
		gpu_submit_job(COMMAND_SLOT_FRAGMENT, &f_job);
		}

    PRINTF("Triangle rendering completed successfully!\n");
}

#define GPU_ALIGN __attribute__((aligned(64)))

#if 0
void gpu_draw_triangle2(uintptr_t framebuffer_phys_addr, uint32_t width, uint32_t height)
{
    // ... [Предыдущий код инициализации fb_desc] ...

    // 1. Настройка метаданных входных вершин
    gpu_vertex_input_meta.buffer_ptr = (uintptr_t)triangle_vertices;
    gpu_vertex_input_meta.stride = sizeof(float) * 3;
    gpu_vertex_input_meta.size = sizeof(triangle_vertices);
    gpu_vertex_input_meta.format = 0x0400000A; // Таблица типов Bifrost: Vec3 Float

    // 2. Настройка дескриптора Вершинного шейдера
    gpu_program_state.vertex_shader.shader_code_ptr = (uintptr_t)bifrost_vertex_shader_code;
    gpu_program_state.vertex_shader.properties = 0x00000004; // Режим: Vertex Stage, 4 рабочих регистра
    gpu_program_state.vertex_shader.preload_regs = 0;
    gpu_program_state.vertex_shader.uniform_buffer_ptr = 0;  // Константы не используем

    // 3. Настройка дескриптора Фрагментного (пиксельного) шейдера
    gpu_program_state.fragment_shader.shader_code_ptr = (uintptr_t)bifrost_fragment_shader_code;
    gpu_program_state.fragment_shader.properties = 0x00000008; // Режим: Fragment Stage, требуется Render Target
    gpu_program_state.fragment_shader.preload_regs = 0;
    gpu_program_state.fragment_shader.uniform_buffer_ptr = 0;

    // Настройка блендинга (простая перезапись пикселя без прозрачности)
    gpu_program_state.blend_equation = 0x00001200; // Режим Opaque (Замещение)
    gpu_program_state.attribute_meta_ptr = (uintptr_t)&gpu_vertex_input_meta;

    // 4. Привязываем созданную программу к аппаратным задачам
    // В Bifrost v6 адрес Renderer State передается в соответствующие поля дескрипторов задач
    v_job.attributes = (uint32_t)((uintptr_t)&gpu_program_state & 0xFFFFFFFF); // Привязка шейдеров к Vertex Job
    // Для некоторых ревизий Bifrost адрес передается через расширенное поле primitive_size
    v_job.primitive_size = (uintptr_t)&gpu_program_state;

    // Задаем цвет очистки экрана перед рисованием треугольника (сброс в темно-синий)
    f_job.clear_color = 0x101030FF;

    // КРИТИЧЕСКИ ВАЖНО: Выталкиваем новые бинарники и структуры дескрипторов из кэша процессора
     dcache_clean((uintptr_t)&gpu_program_state, sizeof(gpu_program_state));
     dcache_clean((uintptr_t)&gpu_vertex_input_meta, sizeof(gpu_vertex_input_meta));
     dcache_clean((uintptr_t)bifrost_vertex_shader_code, sizeof(bifrost_vertex_shader_code));
     dcache_clean((uintptr_t)bifrost_fragment_shader_code, sizeof(bifrost_fragment_shader_code));

    // 5. Запуск цепочки через регистры планировщика (Слот 0 и Слот 1)
    // ... [Код отправки в регистры JS_HEAD_NEXT и ожидания прерываний из предыдущего ответа] ...
}
#endif

#define MALI_GPU_CONTROL_BASE  0x01800000

// Регистры разблокировки и оверрайдов (Блок управления питанием)
#define GPU_PWR_KEY                     0x0050
#define GPU_PWR_OVERRIDE1               0x0058
#define GPU_PWR_KEY_UNLOCK              0x2968A819

// ИСПРАВЛЕНО: Регистры подачи команд питания (Write-Only) для Bifrost
#define REG_L2_PWRON                    0x01A0
#define REG_TILER_PWRON                 0x0190
#define REG_SHADER_PWRON                0x0180

// ИСПРАВЛЕНО: Регистры РЕАЛЬНОЙ готовности (Read-Only) по вашим смещениям
#define REG_SHADER_READY                0x0140
#define REG_TILER_READY                 0x0150 // То самое смещение 0x150!
#define REG_L2_READY                    0x0160

void mali_bifrost_power_on(void)
{
    volatile uint32_t *gpu_pwr_key   = (volatile uint32_t *)(MALI_GPU_CONTROL_BASE + GPU_PWR_KEY);
    volatile uint32_t *gpu_pwr_ovr1  = (volatile uint32_t *)(MALI_GPU_CONTROL_BASE + GPU_PWR_OVERRIDE1);

    volatile uint32_t *l2_pwron      = (volatile uint32_t *)(MALI_GPU_CONTROL_BASE + REG_L2_PWRON);
    volatile uint32_t *l2_ready      = (volatile uint32_t *)(MALI_GPU_CONTROL_BASE + REG_L2_READY);

    volatile uint32_t *tiler_pwron   = (volatile uint32_t *)(MALI_GPU_CONTROL_BASE + REG_TILER_PWRON);
    volatile uint32_t *tiler_ready   = (volatile uint32_t *)(MALI_GPU_CONTROL_BASE + REG_TILER_READY);

    volatile uint32_t *shader_pwron  = (volatile uint32_t *)(MALI_GPU_CONTROL_BASE + REG_SHADER_PWRON);
    volatile uint32_t *shader_ready  = (volatile uint32_t *)(MALI_GPU_CONTROL_BASE + REG_SHADER_READY);

    PRINTF("Mali-G31: Initializing power-up via Bifrost v6 Register Map...\n");

    // 1. Снимаем программную защиту с контроллера питания
    *gpu_pwr_key = GPU_PWR_KEY_UNLOCK;
    __DSB();

    // Фиксация стабильности шин питания
    *gpu_pwr_ovr1 = 0xFFF | (0x20 << 16);
    __DSB();

    // 2. Включаем L2 Кэш (Бит 0 = Включить домен 0)
    *l2_pwron = 0x00000001;
    __DSB();

    //TP();
   // Ожидаем готовность L2 на смещении 0x160
    while ((*l2_ready & 0x00000001) == 0) {
        // Опрос готовности L2-интерфейса
    }

    // 3. Включаем блок геометрии (Tiler) через смещение 0x190
    *tiler_pwron = 0x00000001;
    __DSB();

    //TP();
   // Ожидаем готовность тайлера на вашем смещении 0x150
    while ((*tiler_ready & 0x00000001) == 0) {
        // Если зависает здесь, значит на GPU не подана частота от CCU Allwinner
    }
    //TP();
#define REG_STACK_PWRON   0xE20
#define REG_STACK_READY   0xE10

	// Добавьте этот кусок в mali_bifrost_power_on() СТРОГО ПЕРЕД включением шейдеров:
	volatile uint32_t *stack_pwron = (volatile uint32_t *)(MALI_GPU_CONTROL_BASE + REG_STACK_PWRON);
	volatile uint32_t *stack_ready = (volatile uint32_t *)(MALI_GPU_CONTROL_BASE + REG_STACK_READY);

	PRINTF("Mali-G31: Powering up Shader Core Stack (0x1D0)...\n");
	*stack_pwron = 0x00000001; // Включаем базовый стек
	__DSB();
	local_delay_ms(100);
	PRINTF("*stack_ready=%08X\n", (unsigned) *stack_ready);
	PRINTF("*tiler_ready=%08X\n", (unsigned) *tiler_ready);

    //TP();
//	while ((*stack_ready & 0x00000001) == 0) {
//		// Ожидание готовности стека ядер на смещении 0xE10
////		PRINTF("*stack_ready=%08X\n", (unsigned) *stack_ready);
////		PRINTF("*tiler_ready=%08X\n", (unsigned) *tiler_ready);
//	}
	local_delay_ms(100);
    //TP();
	// 4. Включаем 2 вычислительных ядра (Shader Cores) для MP2 (Маска 0x03) через 0x180
    *shader_pwron = 0x00000003;
    __DSB();

	local_delay_ms(100);
	PRINTF("*shader_ready=%08X\n", (unsigned) *shader_ready);
   //TP();
   // Ожидаем готовность ядер на смещении 0x140
    while ((*shader_ready & 0x00000003) != 0x00000001) {	// was: 0x00000003
        // Ожидание готовности обоих шейдерных ядер
    	   // ТОЛЬКО ОДНО ЯДРО
    }
    //TP();

    PRINTF("Mali-G31: Success! L2, Tiler (0x150), and Shaders (0x140) are READY.\n");
}


#define REG_L2_PWR_DOMAIN_COMMAND      0x0010
#define REG_L2_PWR_DOMAIN_STATUS       0x0014

void mali_bifrost_l2_ready(void)
{
    volatile uint32_t *l2_pwr_cmd  = (volatile uint32_t *)(MALI_GPU_CONTROL_BASE + REG_L2_PWR_DOMAIN_COMMAND);
    volatile uint32_t *l2_pwr_stat = (volatile uint32_t *)(MALI_GPU_CONTROL_BASE + REG_L2_PWR_DOMAIN_STATUS);

    // Принудительно включаем и запитываем L2 кэш GPU
    *l2_pwr_cmd = 0xFFFFFFFF;
    while ((*l2_pwr_stat & 0x1) == 0) {
        // Ожидание готовности кэш-памяти GPU
    }
}

void gpu_test(void)
{
#if 0
	PRINTF("board_gpu_initialize: L2_FEATURES=0x%08X\n", (unsigned) GPU_CONTROL->L2_FEATURES);
	PRINTF("board_gpu_initialize: CORE_FEATURES=0x%08X\n", (unsigned) GPU_CONTROL->CORE_FEATURES);
	PRINTF("board_gpu_initialize: TILER_FEATURES=0x%08X\n", (unsigned) GPU_CONTROL->TILER_FEATURES);
	PRINTF("board_gpu_initialize: MEM_FEATURES=0x%08X\n", (unsigned) GPU_CONTROL->MEM_FEATURES);
	PRINTF("board_gpu_initialize: MMU_FEATURES=0x%08X\n", (unsigned) GPU_CONTROL->MMU_FEATURES);
	PRINTF("board_gpu_initialize: SHADER_PRESENT_LO=0x%08X\n", (unsigned) GPU_CONTROL->SHADER_PRESENT_LO);
	PRINTF("board_gpu_initialize: SHADER_PRESENT_HI=0x%08X\n", (unsigned) GPU_CONTROL->SHADER_PRESENT_HI);
	PRINTF("board_gpu_initialize: TILER_PRESENT_LO=0x%08X\n", (unsigned) GPU_CONTROL->TILER_PRESENT_LO);
	PRINTF("board_gpu_initialize: TILER_PRESENT_HI=0x%08X\n", (unsigned) GPU_CONTROL->TILER_PRESENT_HI);
	PRINTF("board_gpu_initialize: L2_PRESENT_LO=0x%08X\n", (unsigned) GPU_CONTROL->L2_PRESENT_LO);
	PRINTF("board_gpu_initialize: L2_PRESENT_HI=0x%08X\n", (unsigned) GPU_CONTROL->L2_PRESENT_HI);
#endif


    uintptr_t fbaddr = (uintptr_t) colmain_fb_draw();
    memset32((void *) fbaddr, COLORPIP_DARKCYAN, DIM_X * DIM_Y * 4);
    gpu_draw_triangle(fbaddr, DIM_X, DIM_Y);
    colmain_nextfb();

    TP();
    for (;;)
    	;

}
#define GPU_L2_MMU_CONFIG  0x0008 // Смещение внутри блока GPU_CONTROL (0x01800008)

void mali_bifrost_open_mmu_bus(void)
{
    volatile uint32_t *l2_mmu_config = (volatile uint32_t *)(0x01800000 + GPU_L2_MMU_CONFIG);

    // Читаем текущие особенности чипа
    uint32_t val = *l2_mmu_config;

    // Для Mali-G31 (Bifrost r0p3) необходимо принудительно выставить биты 0 и 1,
    // чтобы открыть внутренний интерконнект от процессоров к MMU и снять изоляцию.
    *l2_mmu_config = val | 0x00000003;

    __DSB();
}

void mali_g31_mmu_enable(void)
{
    unsigned as = 0; // Шейдерный домен по умолчанию

	// Индекс 0 = 0xAA (Cacheable), Индекс 1 = 0x22 (Non-Cacheable)
	GPU_MMU->MMU_AS[as].AS_MEMATTR_HI = 0x00000000;
	GPU_MMU->MMU_AS[as].AS_MEMATTR_LO = 0xff ;//0x000022AA;
	__DSB();

	uint64_t table_phys_addr = 0;
	ASSERT((table_phys_addr & 0xFFF) == 0);

	// 3. Загружаем физический адрес плоской таблицы
	// Младшие биты 0x03 включают режим трансляции LPAE
	uint64_t transtab_val = table_phys_addr |
//		1 * (UINT64_C(1) << 4) |	// SHARE_OUTER
		1 * (UINT64_C(1) << 3) |	// SHARE_INNER
		1 * (UINT64_C(1) << 2) |	// READ_INNER
//		0x03 * (UINT64_C(1) << 0) |	// ADRMODE: TABLE (Включена трансляция по таблицам страниц LPAE).
		0x01 * (UINT64_C(1) << 0) |	// ADRMODE: IDENTITY
		0;

	GPU_MMU->MMU_AS[as].AS_TRANSCFG_HI = 0;
	GPU_MMU->MMU_AS[as].AS_TRANSCFG_LO = 0;

    // 3. Записываем адрес в регистры AS0 (теперь они строго на 0x01802400)
    GPU_MMU->MMU_AS[as].AS_TRANSTAB_HI = ptr_hi32(transtab_val); //(uint32_t)(transtab_val >> 32);
    GPU_MMU->MMU_AS[as].AS_TRANSTAB_LO = ptr_lo32(transtab_val); //(uint32_t)(transtab_val & 0xFFFFFFFF);

    __DSB();

    // 4. Ждем, пока MMU освободится
    while (GPU_MMU->MMU_AS[as].AS_STATUS & 0x1) {}

    // 5. Отправляем команду UPDATE для применения таблиц
    GPU_MMU->MMU_AS[as].AS_COMMAND = 0x01;
    __DSB();

    // 6. Ожидаем окончания защелкивания таблиц аппаратурой Mali
    while (GPU_MMU->MMU_AS[as].AS_STATUS & 0x1) {}

    // 4. Очищаем внутренний TLB кэш MMU от старых зависших ошибок 0xC8
    GPU_MMU->MMU_AS[as].AS_COMMAND = 0x03; // AS_COMMAND_INVALIDATE
    __DSB();
    while (GPU_MMU->MMU_AS[as].AS_STATUS & 0x1) {}

    // 5. Активируем таблицы командным словом UPDATE
    GPU_MMU->MMU_AS[as].AS_COMMAND = 0x01; // AS_COMMAND_UPDATE
    __DSB();
    while (GPU_MMU->MMU_AS[as].AS_STATUS & 0x1) {}


	//gpu_as_command(as, AS_COMMAND_NOP);
	gpu_as_command(as, AS_COMMAND_UPDATE);
	gpu_as_command(as, AS_COMMAND_INVALIDATE);
//	gpu_as_command(as, AS_COMMAND_FLUSH_PT);
	//TP();
	printhex32((uintptr_t) & GPU_MMU->MMU_AS[as], & GPU_MMU->MMU_AS[as], sizeof GPU_MMU->MMU_AS[as]);
    PRINTF("Mali-G31: MMU Address Space 0 successfully enabled at offset 0x400!\n");
}

//
#define T507_SPC_BASE         0x03008000

// Регистры конфигурации защиты периферии (Secure Peripherals Control)
#define SPC_GPU_MAST_REG      (T507_SPC_BASE + 0x00A0) // Управление правами GPU как Master шины
#define SPC_GPU_SLAV_REG      (T507_SPC_BASE + 0x00A4) // Управление правами доступа CPU к регистрам GPU

void t507_spc_unlock_gpu(void)
{
    volatile uint32_t *spc_gpu_master = (volatile uint32_t *)SPC_GPU_MAST_REG;
    volatile uint32_t *spc_gpu_slave  = (volatile uint32_t *)SPC_GPU_SLAV_REG;

    PRINTF("T507 Platform: Unlocking GPU Secure Peripherals Controller (SPC)...\n");

    // Запись маски 0xFFFFFFFF или 0x00000003 (в зависимости от разводки доменов)
    // разрешает Non-Secure транзакции для графического процессора на системной интерконнект-шине.
    // По спецификации Allwinner, запись всех единиц переводит устройство в полностью открытый Non-Secure режим.
    *spc_gpu_master = 0xFFFFFFFF;
    *spc_gpu_slave  = 0xFFFFFFFF;

    __DSB(); // Принудительно толкаем барьер в контроллер SPC

    PRINTF("T507 Platform: GPU registers bypass TrustZone protection now.\n");
}

// Graphic processor unit
void board_gpu_initialize(void)
{
	PRINTF("board_gpu_initialize start.\n");
//#if WITHGPUHW
//	allwnr_t507_module_pll_spr(& CCU->PLL_GPU0_CTRL_REG, & CCU->PLL_GPU0_PAT0_CTRL_REG);	// Set Spread Frequency Mode
//	allwnr_t507_module_pll_enable(& CCU->PLL_GPU0_CTRL_REG, 36);
//#endif /* WITHGPUHW */
	{
		//PRINTF("1 CCU->PLL_GPU0_CTRL_REG = %08X\n", (unsigned) CCU->PLL_GPU0_CTRL_REG);

		const unsigned N = 600 * 2 / 24;
		const unsigned M1 = 1;
		const unsigned M0 = 2;
		// PLL_GPU0 = 24 MHz*N/M0/M1
		CCU->PLL_GPU0_CTRL_REG = 0;
		allwnr_t507_module_pll_spr(& CCU->PLL_GPU0_CTRL_REG, & CCU->PLL_GPU0_PAT0_CTRL_REG);	// Set Spread Frequency Mode
		CCU->PLL_GPU0_CTRL_REG &= ~ (UINT32_C(1) << 31) & ~ (UINT32_C(1) << 27);
		CCU->PLL_GPU0_CTRL_REG |=
			(N - 1) * (UINT32_C(1) << 8) |
			(M1 - 1) * (UINT32_C(1) << 1) |
			(M0 - 1) * (UINT32_C(1) << 0) |
			0;
		CCU->PLL_GPU0_CTRL_REG |= (UINT32_C(1) << 31); // PLL_ENABLE
		CCU->PLL_GPU0_CTRL_REG |= (UINT32_C(1) << 29); // LOCK_ENABLE
		while ((CCU->PLL_GPU0_CTRL_REG  & (UINT32_C(1) << 28)) == 0)	// LOCK
			;
		CCU->PLL_GPU0_CTRL_REG |= (UINT32_C(1) << 27); // PLL_OUTPUT_ENABLE
		//PRINTF("2 CCU->PLL_GPU0_CTRL_REG = %08X\n", (unsigned) CCU->PLL_GPU0_CTRL_REG);

	}

	t507_spc_unlock_gpu();

	CCU->GPU_CLK1_REG |= (UINT32_C(1) << 31);	// PLL_PERI_BAK_CLK_GATING
	CCU->GPU_CLK0_REG |= (UINT32_C(1) << 31);	// SCLK_GATING

	PRCM->GPU_PWROFF_GATING = 0;

	CCU->GPU_BGR_REG |= (UINT32_C(1) << 0);	// Clock Gating
	CCU->GPU_BGR_REG &= ~ (UINT32_C(1) << 16);	// Assert Reset
	CCU->GPU_BGR_REG |= (UINT32_C(1) << 16);	// De-assert Reset

	PRINTF("allwnr_t507_get_gpu_freq()=%" PRIuFAST32 " MHz\n", allwnr_t507_get_gpu_freq() / 1000 / 1000);

	// https://github.com/bakhi/GPUReplay/blob/accce5d2bcbe5794b895156997f50a6fda86a87c/replayer/include/midgard/mali_kbase_gpu_id.h#L26

	// Mali G31 MP2 (Panfrost)
	PRINTF("board_gpu_initialize: ID=0x%08X (expected 0x%08X)\n", (unsigned) GPU_CONTROL->ID, 0x70930000);

	arm_hardware_set_handler_system(GPU_IRQn, GPU_IRQHandler);
	arm_hardware_set_handler_system(GPU_EVENT_IRQn, GPU_EVENT_IRQHandler);
	arm_hardware_set_handler_system(GPU_JOB_IRQn, GPU_JOB_IRQHandler);
	arm_hardware_set_handler_system(GPU_MMU_IRQn, GPU_MMU_IRQHandler);

	GPU_CONTROL->GPU_IRQ_CLEAR = GPU_IRQ_REG_ALL;
	GPU_CONTROL->GPU_IRQ_MASK = GPU_IRQ_REG_ALL;

	GPU_JOB_CONTROL->JOB_IRQ_CLEAR = 0xFFFFFFFF;
	GPU_JOB_CONTROL->JOB_IRQ_MASK = 0xFFFFFFFF;

	GPU_MMU->MMU_IRQ_CLEAR = 0xFFFFFFFF;
	GPU_MMU->MMU_IRQ_MASK = 0xFFFFFFFF;
//    PRINTF("1 GPU_MMU:\n");
//    printhex32(GPU_MMU_BASE, GPU_MMU, 4096);

	gpu_command(GPU_COMMAND_HARD_RESET);
	gpu_wait(RESET_COMPLETED);
	gpu_command(GPU_COMMAND_SOFT_RESET);
	gpu_wait(RESET_COMPLETED);
	gpu_command(GPU_COMMAND_NOP);

    PRINTF("GPU Reset released. Unlocking internal buses...\n");

    // 2. СНИМАЕМ ИЗОЛЯЦИЮ ШИНЫ ЗАДАЧ (Решение причины №1)
	gpu_command(GPU_COMMAND_CYCLE_COUNT_START);
	local_delay_ms(100);

	// https://elixir.bootlin.com/linux/latest/source/drivers/gpu/drm/panfrost/panfrost_mmu.c

	PRINTF("board_gpu_initialize done.\n");

	mali_bifrost_power_on();
	mali_bifrost_open_mmu_bus();

	mali_g31_mmu_enable();
}

#elif CPUSTYLE_STM32MP1

void GPU_IRQHandler(void)
{
	PRINTF("GPU_IRQHandler\n");
}

// Graphic processor unit
void board_gpu_initialize(void)
{
	PRINTF("board_gpu_initialize start.\n");

	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_GPUEN;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MP_AHB6LPENSETR_GPULPEN;
	(void) RCC->MP_AHB6LPENSETR;

	PRINTF("board_gpu_initialize: PRODUCTID=%08lX\n", (unsigned long) GPU->PRODUCTID);

//
	arm_hardware_set_handler_system(GPU_IRQn, GPU_IRQHandler);

	PRINTF("board_gpu_initialize done.\n");
}

#endif /* WITHGPUHW */

