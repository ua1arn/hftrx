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


// Выравнивание для кэш-линий GPU
#define GPU_ALIGN __attribute__((aligned(64)))

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

// Регистры отправки команд в слот (сверьтесь со структурой GPU_JOB_CONTROL в panfrost_regs.h)
// Обычные имена регистров в драйвере Panfrost: JS_COMMAND, JS_HEAD_NEXT
void gpu_diagnose_slot_fault(unsigned slot, unsigned as)
{
    // Читаем статус ошибки Слота 1 (смещение 0x24 от 0x1880 -> адрес 0x018018A4)
    uint32_t slot1_status = GPU_JOB_CONTROL->LOOP[slot].JS_STATUS;

    // Физический адрес, на котором споткнулся Fragment-парсер
    uint32_t fault_lo = GPU_JOB_CONTROL->LOOP[slot].JS_HEAD_LO;
    uint32_t fault_hi = GPU_JOB_CONTROL->LOOP[slot].JS_HEAD_HI;
    // Физический адрес, на котором споткнулся Fragment-парсер
    uint32_t tail_lo = GPU_JOB_CONTROL->LOOP[slot].JS_TAIL_LO;
    uint32_t tail_hi = GPU_JOB_CONTROL->LOOP[slot].JS_TAIL_HI;

    PRINTF("\n-> FRAGMENT STAGE FAULT DIAGNOSIS:\n");
    PRINTF("   Slot %u JS_STATUS = 0x%08X\n", slot, (unsigned)slot1_status);
    PRINTF("   Slot %u Stopped at Address: 0x%08X%08X\n", slot, (unsigned)fault_hi, (unsigned)fault_lo);
    PRINTF("   Slot %u Tail Address:       0x%08X%08X\n", slot, (unsigned)tail_hi, (unsigned)tail_lo);

    // Проверяем, не ругнулся ли при этом MMU (адресное пространство AS0 на 0x400)
//    PRINTF("   MMU Fault Status (as=%u) = 0x%08X\n", as, (unsigned)GPU_MMU->MMU_AS [as].AS_FAULTSTATUS);
//    PRINTF("   MMU Fault Address = 0x%08X%08X\n",
//           (unsigned)GPU_MMU->MMU_AS [as].AS_FAULTADDRESS_HI, (unsigned)GPU_MMU->MMU_AS [as].AS_FAULTADDRESS_LO);
}

static int gpu_submit_job(unsigned slot, uintptr_t head)
{
//	PRINTF("gpu_submit_job: head=%p, slot=%u\n", (void *) head, slot);
//	printhex32((uintptr_t) job, job, 64);
    // Записываем физический адрес начала структуры v_job в регистр указателя слота 0
    GPU_JOB_CONTROL->LOOP[slot].JS_HEAD_NEXT_HI = ptr_hi32(head);//(uint32_t)(((uintptr_t)&v_job >> 32) & 0xFFFFFFFF);
    GPU_JOB_CONTROL->LOOP[slot].JS_HEAD_NEXT_LO = ptr_lo32(head);//(uint32_t)((uintptr_t)&v_job & 0xFFFFFFFF);

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
    	PRINTF("error head %p:\n", (void *) head);
    	dcache_invalidate((uintptr_t) head, 128);
    	printhex32((uintptr_t) head, (void *) head, 128);
    	PRINTF("gpu timeout: GPU_JOB_CONTROL->JOB_INT_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_INT_RAWSTAT);
    	gpu_diagnose_slot_fault(slot, 0);
        GPU_JOB_CONTROL->JOB_INT_CLEAR = ~ 0;
   	return 1;
    }
    else
    {
    	//PRINTF("okay head %p:\n", (void *) head);
    	//dcache_invalidate((uintptr_t) head, 128);
    	//printhex32((uintptr_t) head, (void *) head, 128);
        GPU_JOB_CONTROL->JOB_INT_CLEAR = (UINT32_C(1) << slot); // Сброс флага прерывания
        GPU_JOB_CONTROL->JOB_INT_CLEAR = ~ 0;
      return 0;
    }
}
#if 0
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
#endif
/**
 * @brief Типы задач (Job Types) для аппаратного планировщика ARM Mali Bifrost.
 *
 * Значения этих констант записываются в битовое поле `job_type` (биты [7:1])
 * байта по смещению 0x10 в заголовке дескриптора задачи.
 *
 * Формула для байта 0x10: (MALI_JOB_TYPE_XXX << 1) | JOB_DESCRIPTOR_SIZE
 */
typedef enum {
    /* 0x00: Служебный маркер планировщика. Напрямую в слоты не отправляется */
    MALI_JOB_TYPE_NOT_STARTED   = 0x00,

    /**
     * 0x01: Задача-заглушка (Null Job).
     * Не выполняет никакой работы на GPU, завершается мгновенно.
     * Используется для отладки прерываний, тестирования шины/MMU
     * или в качестве барьера синхронизации между другими задачами.
     * Разрешена для отправки в ЛЮБОЙ слот (Slot 0, 1, 2).
     */
    MALI_JOB_TYPE_NULL          = 0x01,

    /**
     * 0x02: Запись значения в память (Write Value Job).
     * Аппаратно записывает 32-битное или 64-битное число по указанному адресу в ОЗУ.
     * Поддерживает режимы прямой записи константы, инкремента и атомарного сложения.
     * Допустима строго для сервисного слота: SLOT 2.
     * upd: проверено на T507-H: раьотает на всех трёх слотаж
     */
    MALI_JOB_TYPE_WRITE_VALUE   = 0x02,

    /**
     * 0x03: Очистка кэша (Cache Flush Job).
     * Принудительно заставляет GPU инвалидировать и сбросить свои внутренние
     * кэши данных (L2 cache) в системное ОЗУ. Используется для обеспечения
     * когерентности памяти между GPU и CPU перед чтением результатов.
     * Допустима строго для сервисного слота: SLOT 2.
     */
    MALI_JOB_TYPE_CACHE_FLUSH   = 0x03,

    /**
     * 0x04: Вычислительный шейдер (Compute Job).
     * Запускает одномерную, двумерную или трехмерную сетку вычислительных потоков
     * (OpenCL, Vulkan Compute). Требует заполнения структуры Thread Input и RSD.
     * Допустима строго для вычислительного слота: SLOT 0.
     */
    MALI_JOB_TYPE_COMPUTE       = 0x04,

    /**
     * 0x05: Вершинный шейдер (Vertex Job).
     * Классическая обработка вершин геометрии (трансформация координат, освещение).
     * Читает Vertex Buffers и выполняет Vertex Shader.
     * Допустима строго для вершинного слота: SLOT 0.
     */
    MALI_JOB_TYPE_VERTEX        = 0x05,

    /**
     * 0x06: Геометрический шейдер (Geometry Job).
     * Используется для шейдеров геометрии и тесселяции.
     * В архитектурах Bifrost (Mali-G31) в чистом виде практически не применяется,
     * так как эти этапы обычно объединяются с тайлингом.
     * Направляется в SLOT 0.
     */
    MALI_JOB_TYPE_GEOMETRY      = 0x06,

    /**
     * 0x07: Задача тайлинга (Tiler Job).
     * Принимает трансформированные вершины сцены, собирает их в примитивы (треугольники)
     * и распределяет по экранным плиткам (тайлам 16х16). Результат пишет в Polygon List.
     * Требует валидной структуры Tiler Heap и Framebuffer Descriptor (MFBD).
     * Допустима строго для слота геометрии: SLOT 0.
     */
    MALI_JOB_TYPE_TILER         = 0x07,

    /**
     * 0x08: Слитая задача (Fused Vertex + Tiler Job).
     * Оптимизированный аппаратный режим, выполняющий Vertex Shader и Tiler Job
     * одновременно в рамках одной задачи для экономии пропускной способности памяти.
     * Допустима строго для слота: SLOT 0.
     */
    MALI_JOB_TYPE_FUSED         = 0x08,

    /**
     * 0x09: Фрагментный шейдер (Fragment / Pixel Job).
     * Финальная стадия графического конвейера. Выполняет растеризацию, пиксельные шейдеры,
     * тесты глубины/трафарета и блендинг. Читает Polygon List и пишет в Framebuffer (картинка).
     * Допустима строго для фрагментного слота: SLOT 1.
     */
    MALI_JOB_TYPE_FRAGMENT      = 0x09

} mali_job_type;

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
// MGS! подтверждена работа заголовка
typedef struct __attribute__((packed)) {
    uint32_t exception_status;       // 0x00: Сюда GPU запишет код ошибки при FAULT (изначально 0)
    uint32_t first_incomplete_task;  // 0x04: Служебный внутренний статус GPU
    uint64_t fault_pointer;          // 0x08: Физический адрес буфера для дампа ошибок MMU/Bus

    // Битовые поля управления типом и разрядами адреса (занимают 1 байт):
    uint8_t  job_descriptor_size : 1; // 0x10 (бит 0)  - Выставляем 1 (64-битные указатели)
    uint8_t  job_type            : 7; // 0x10 (биты 1-7) - Тип задачи (TILER = 0x11, VERTEX = 0x12)

    // Битовые поля барьеров и флагов (занимают 1 байт):
    uint8_t  job_barrier         : 1; // 0x11 (бит 0)  - last job
    uint8_t  unknown_flags       : 7; // 0x11 (биты 1-7) - Служебные флаги (обычно 0)

    uint16_t job_index;               // 0x12: Уникальный ID этой задачи для скорборда (например, 1)
    uint16_t job_dependency_index_1;  // 0x14: ID задачи, которую нужно дождаться перед запуском (0, если нет)
    uint16_t job_dependency_index_2;  // 0x16: ID второй зависимой задачи (0, если нет)

    uint64_t next_job;               // 0x18: Физический адрес следующего дескриптора в цепочке (0, если последний)
} mali_job_header;

#include <stdint.h>

//#define MALI_WRITE_VALUE_ZERO     3 // Специальный флаг для обнуления

// Полный монолитный дескриптор задачи
// MGS! подтверждена работа заголовка
// mali_payload_write_value
// https://android.googlesource.com/platform/external/mesa3d/+/e061bf004b5/src/panfrost/include/panfrost-job.h
// https://github.com/dlehman-work/mesa/blob/master/src/panfrost/include/panfrost-job.h#L45

typedef struct __attribute__((packed, aligned(64))) {
    // 1. Стандартный заголовок (mali_job_header) - 32 байта
	mali_job_header header;
    // 2. Специфичный Payload - 32 байта
	uint64_t address;
	uint32_t value_descriptor;
	uint32_t reserved;
	uint64_t immediate;
	uint64_t pad;
} mali_write_value_job;
#if 0
// Расширенный дескриптор для Vertex/Tiler задач
typedef struct __attribute__((packed)) {
    // 1. Стандартный заголовок (mali_job_header) - 32 байта
    mali_job_header header;

    /* --- Слот 0x20 - 0x3F: Payload задачи тайлинга --- */
    uint64_t tiler_heap_desc;        // 0x20: Физический адрес структуры кучи (mali_bifrost_tiler_heap)
    uint64_t fb_desc;                // 0x28: Физический адрес Framebuffer-дескриптора (MFBD)
    uint64_t polygon_list;           // 0x30: Физический адрес буфера для Polygon List (выделите 4КБ нулей)
    uint32_t reserved;               // 0x38: 0
    uint32_t flags;                  // 0x3C: 0

    uint32_t padding_to_128[16];   // Еще 64 байта чистых нулей!
} mali_tiler_job;

typedef struct __attribute__((packed)) {
    // 1. Стандартный заголовок (mali_job_header) - 32 байта
    mali_job_header header;

    /* --- Смещение 0x20 - 0x3F: Payload задачи вершинного шейдера --- */
     uint64_t thread_input_record;    // 0x20: Физический адрес структуры параметров сетки потоков
     uint64_t renderer_state;         // 0x28: Физический адрес RSD (Render State Descriptor) для Vertex-стадии
     uint64_t attributes;             // 0x30: Физический адрес таблицы вершинных буферов (Attribute Table)
     uint64_t attribute_buffers;      // 0x38: Физический адрес описателей самих буферов (Attribute Buffer Table)

     uint32_t padding_to_128[16];   // Еще 64 байта чистых нулей!
} mali_vertex_job;

// Расширенный дескриптор для Fragment задач
typedef struct __attribute__((packed)) {
    // 1. Стандартный заголовок (mali_job_header) - 32 байта
    mali_job_header header;

    __IO uint64_t framebuffer_desc;       /* +0x20: 64-бит физ. адрес дескриптора Framebuffer (fb_desc) */
    __IO uint64_t tile_render_list;        /* +0x28: 64-бит физ. адрес списка тайлов дисплея (tiler_heap_mem) */

    __IO uint32_t stride_and_format;      /* +0x30: Внутренние аппаратные флаги шага и формата тайлера */
    __IO uint16_t width_minus_1;          /* +0x34: Ширина зоны рендеринга МИНУС 1 (например, 799 для 800) */
    __IO uint16_t height_minus_1;         /* +0x36: Высота зоны рендеринга МИНУС 1 (например, 479 для 480) */

    __IO uint32_t clear_color;            /* +0x38: Цвет очистки экрана в формате ARGB8888 (если включен флаг) */
    __IO uint32_t fragment_flags;         /* +0x3C: Флаги фрагментного конвейера (биты очистки, глубина, трафарет) */
    __IO uint32_t r1 [2];
    uint32_t padding_to_128[16];   // !!!!! Еще 64 байта чистых нулей!
} mali_fragment_job;

// Дескриптор Vertex Job (128 байт под стандарты пакетного чтения Bifrost v6)
typedef struct __attribute__((packed, aligned(64))) {
    mali_job_header header;                 // 0x00 - 0x1F: Заголовок задачи (32 байта)

    /* --- Специфичный Payload для Bifrost v6 (Строго 32 байта) --- */
    uint64_t thread_input_record;           // 0x20: Физический адрес структуры v_thread_input
    uint64_t renderer_state;                // 0x28: Физический адрес структуры gpu_program_state (RSD)
    uint64_t attributes;                    // 0x30: В Bifrost v6 СТРОГО зануляется (0x0)
    uint64_t attribute_buffers;             // 0x38: В Bifrost v6 СТРОГО зануляется (0x0)

    /* --- Паддинг безопасности (Добивка до 128 байт) --- */
    uint32_t padding_to_128[16];            // 0x40 - 0x7F: Скрытая зона регистров отсечения (зануляем)
} mali_vertex_job_bifrost;
#endif
// Переменная-цель, куда будет писать GPU.
// Обязательно выравниваем по кэш-линии!
#if 1
static volatile uint64_t __attribute__((aligned(64))) gpu_test_target [2];
int run_write_value_test(void) {
	unsigned v = 0x07;
    // Создаем структуру дескриптора в памяти
	GPU_ALIGN static mali_write_value_job job;
	GPU_ALIGN static mali_write_value_job job2;

    // Сбрасываем старую память
    memset(&job, 0, sizeof(job));
    memset(gpu_test_target, 0xFF, sizeof gpu_test_target);

    // Заполняем заголовок
    job.header.exception_status = 0;
    job.header.job_descriptor_size = 1; // Используем 64-битные указатели
    job.header.job_type = MALI_JOB_TYPE_WRITE_VALUE; // Тип задачи = 2
    job.header.job_barrier = 1;			// last
    job.header.job_index = 1;
    job.header.next_job = (uintptr_t) & job2;            // Цепочка заканчивается на ней

    job2.header.exception_status = 0;
    job2.header.job_descriptor_size = 1; // Используем 64-битные указатели
    job2.header.job_type = MALI_JOB_TYPE_WRITE_VALUE; // Тип задачи = 2
    job2.header.job_barrier = 1;			// last
    job2.header.job_index = 1;
    job2.header.next_job = (uintptr_t) 0;            // Цепочка заканчивается на ней

    /**
     * value_descriptor:
     * 0 - fault operation
     * 1 - timestamp?
     * 2 - timestamp?
     * 3 - zero
     * 4 - 8 bit
     * 5 - 16 bit
     * 6 - 32 bit
     * 7 - 64 bit
     */
    // Заполняем Payload записи
    job.address = (uintptr_t) & gpu_test_target [0]; // Физический адрес цели
    job.value_descriptor = v;                 // Бит [2] (Width): Разрядность данных Биты [1:0] (Type): Тип операции записи
    job.immediate = 0xDEADBEEFABBA1980;               // Данные для записи
    job2.address = (uintptr_t) & gpu_test_target [1]; // Физический адрес цели
    job2.value_descriptor = v;                 // Бит [2] (Width): Разрядность данных Биты [1:0] (Type): Тип операции записи
    job2.immediate = 0x123456789abcdef;               // Данные для записи

    // КРИТИЧЕСКИ ВАЖНО ДЛЯ BARE METAL:
    // Очищаем кэш CPU, чтобы GPU читал структуру из физического ОЗУ,
    // и инвалидируем регион gpu_test_target, чтобы CPU позже не прочитал старые данные из своего L1/L2.
    dcache_clean_invalidate((uintptr_t)&job, sizeof(job));
    dcache_clean_invalidate((uintptr_t)&job2, sizeof(job2));

    dcache_clean_invalidate((uintptr_t)&gpu_test_target, sizeof(gpu_test_target));

    // Важно: Адреса job и gpu_test_target должны быть предварительно
    // промаплены в MMU вашего Mali-G31 с правами Read/Write!

    if (gpu_submit_job(2, (uintptr_t) & job))
    	return 1;	// err
      // Проверяем результат выполнения
    printhex64(0, gpu_test_target, sizeof gpu_test_target);
    return 0;
}
#endif

#define GPU_ALIGN __attribute__((aligned(64)))

//++++++++++++++++++
#ifndef GPU_BIFROST_V6_H
#define GPU_BIFROST_V6_H

#include <stdint.h>

/* =========================================================================
 * 1. АППАРАТНЫЕ КОНСТАНТЫ И КОДЫ ТИПОВ ЗАДАЧ MALI BIFROST
 * ========================================================================= */
//#define MALI_JOB_TYPE_NULL          UINT8_C(1)
//#define MALI_JOB_TYPE_WRITE_VALUE   UINT8_C(2)
//#define MALI_JOB_TYPE_CACHE_FLUSH   UINT8_C(3)
//#define MALI_JOB_TYPE_COMPUTE       UINT8_C(4)
//#define MALI_JOB_TYPE_VERTEX        UINT8_C(5)
//#define MALI_JOB_TYPE_TILER         UINT8_C(7)
//#define MALI_JOB_TYPE_FUSED         UINT8_C(8)
//#define MALI_JOB_TYPE_FRAGMENT      UINT8_C(9)

/* Флаги управления разрядностью и барьерами */
#define MALI_JOB_64                 UINT8_C(1)  /* 1 = 64-битная адресация указателей */
#define MALI_JOB_BARRIER            UINT8_C(1)  /* 1 = аппаратный барьер в слоте */

/* Аппаратные макросы выравнивания для шины Mali */
#define GPU_ALIGN_64  __attribute__((aligned(64)))
#define GPU_ALIGN_128 __attribute__((aligned(128)))
#define GPU_PACKED    __attribute__((packed))

/* =========================================================================
 * 2. УНИВЕРСАЛЬНЫЙ ЗАГОЛОВОК ЗАДАЧИ (32 БАЙТА)
 * ========================================================================= */
typedef struct GPU_PACKED {
    uint32_t exception_status;       // 0x00: Сюда GPU пишет код ошибки при падении (0 = DONE)
    uint32_t first_incomplete_task;  // 0x04: Служебный статус планировщика (0)
    uint64_t fault_pointer;          // 0x08: Сюда GPU пишет адрес сбоя на шине или MMU

    // Смещение 0x10: Упаковывается компилятором как (type << 1) | descriptor_size
    uint8_t  job_descriptor_size : 1; // Бит 0: Ставим 1 (MALI_JOB_64)
    uint8_t  job_type            : 7; // Биты 1-7: Код типа задачи (VERTEX, TILER и т.д.)

    uint8_t  job_barrier         : 1; // Бит 0 смещения 0x11: Включение барьера (MALI_JOB_BARRIER)
    uint8_t  unknown_flags       : 7; // Биты 1-7 смещения 0x11: Системный резерв (0)

    uint16_t job_index;               // 0x12: ID задачи в окне планировщика (Scoreboard)
    uint16_t job_dependency_index_1;  // 0x14: ID задачи, которую нужно дождаться
    uint16_t job_dependency_index_2;  // 0x16: Второй ID зависимости

    uint64_t next_job;               // 0x18: Физический адрес следующего 128-байтного джоба в цепи
} mali_job_headeXr;

/* =========================================================================
 * 3. ДЕСКРИПТОРЫ ОЧЕРЕДЕЙ ЗАДАЧ (ПО 128 БАЙТ ИЗ-ЗА BURST-ЧТЕНИЯ BIFROST)
 * ========================================================================= */

/* Задача вершинного шейдера (Vertex Job) — Slot 0 */
typedef struct GPU_PACKED GPU_ALIGN_64 {
    mali_job_header header;          // 0x00 - 0x1F: Базовый заголовок задачи (32 байта)

    /* Специфичный Payload для Bifrost v6 (32 байта) */
    uint64_t thread_input_record;   // 0x20: Физический адрес структуры mali_bifrost_thread_input
    uint64_t renderer_state;        // 0x28: Физический адрес RSD (mali_renderer_state_bifrost_v6_t)
    uint64_t attributes;            // 0x30: В Bifrost v6 СТРОГО 0 (Удалено из Midgard)
    uint64_t attribute_buffers;     // 0x38: В Bifrost v6 СТРОГО 0 (Удалено из Midgard)

    /* Паддинг безопасности (64 байта чистых нулей) */
    uint8_t  padding_to_128[64];    // 0x40 - 0x7F: Сюда GPU burst-чтением мапит движок Clipping/Culling [0x1.1]
} mali_vertex_job_bifrost;

/* Задача тайлинга (Tiler Job) — Slot 0 */
typedef struct GPU_PACKED GPU_ALIGN_64 {
    mali_job_header header;          // 0x00 - 0x1F: Базовый заголовок задачи (32 байта)

    /* Специфичный Payload для Bifrost v6 (32 байта) */
    uint64_t tiler_heap_desc;       // 0x20: Физический адрес кучи (mali_bifrost_tiler_heap)
    uint64_t fb_desc;               // 0x28: Физический адрес Framebuffer-дескриптора (mali_bifrost_fb_desc)
    uint64_t polygon_list;          // 0x30: Физический адрес выходного буфера под Polygon List
    uint64_t reserved_fields;       // 0x38: Системный резерв (зануляется)

    /* Паддинг безопасности (64 байта чистых нулей) */
    uint8_t  padding_to_128[64];    // 0x40 - 0x7F
} mali_tiler_job_bifrost;

/* Сервисная задача записи значения (Write Value Job) — Slot 2 */
typedef struct GPU_PACKED GPU_ALIGN_64 {
    mali_job_header header;          // 0x00 - 0x1F: Базовый заголовок задачи (32 байта)

    /* Специфичный Payload из panfrost-job.h (32 байта) */
    uint64_t address;               // 0x20: Физический адрес ОЗУ, КУДА пишем данные
    uint32_t value_descriptor;      // 0x28: Режим записи (0 = безусловный immediate константный маркер)
    uint32_t reserved_field;        // 0x2C: Выравнивающий резерв (0)
    uint64_t immediate;             // 0x30: ЧТО именно пишем (например, 0xDEADBEEF)

    /* Паддинг безопасности (64 байта чистых нулей) */
    uint8_t  padding_to_128[64];    // 0x40 - 0x7F
} mali_write_value_job_bifrost;

/* =========================================================================
 * 4. ВНУТРЕННИЕ СТРУКТУРЫ УПРАВЛЕНИЯ КОНВЕЙЕРОМ ГЕОМЕТРИИ (IDVS)
 * ========================================================================= */

/* Параметры сетки потоков вершин (64 байта) */
typedef struct GPU_PACKED GPU_ALIGN_64 {
    uint32_t flags;                  // 0x00: Режим сетки (Ставим 0x00000001 — активация параметров)
    uint32_t workgroup_size_x;       // 0x04: Разбиение на группы по X (пишем 1)
    uint32_t workgroup_size_y;       // 0x08: Всегда 1 для геометрии
    uint32_t workgroup_size_z;       // 0x0C: Всегда 1 для геометрии
    uint32_t grid_size_x;            // 0x10: Глобальное число вершин в сцене (треугольник = 3)
    uint32_t grid_size_y;            // 0x14: Всегда 1
    uint32_t grid_size_z;            // 0x18: Всегда 1
    uint32_t index_count;            // 0x1C: Число индексов (в неиндексированном режиме дублируем = 3)
    uint64_t index_buffer_ptr;       // 0x20: Физический адрес буфера индексов (0 = рисуем сырым массивом)
    uint8_t  reserved[24];           // 0x28 - 0x3F: Зануленный хвост до 64 байт
} mali_bifrost_thread_input;

/* Метаданные формата и шага входного атрибута (32 байта) */
typedef struct GPU_PACKED {
    uint64_t buffer_ptr;             // 0x00: Физический адрес массива координат triangle_vertices в ОЗУ
    uint32_t stride;                 // 0x08: Расстояние от начала одной вершины до следующей (3 float * 4 = 12 байт)
    uint32_t size;                   // 0x0C: Общий размер буфера вершин в байтах (3 вершины * 12 = 36 байт)
    uint32_t format;                 // 0x10: Аппаратная кодировка Bifrost v6 (0x0400030A = Vec3 Float32)
    uint8_t  reserved[12];           // 0x14 - 0x1F: Зануленный остаток
} mali_vertex_input_meta;

/* Описание одной стадии выполнения шейдера (32 байта — Строгий стандарт Bifrost v6) */
typedef struct GPU_PACKED {
    uint32_t properties;             // 0x00: Тип стадии (0x4000 = Vertex/Compute, 0x4001 = Fragment)
    uint16_t stack_size;             // 0x04: Размер аппаратного стека для ветвлений шейдера (0)
    uint16_t preload_regs;           // 0x06: Маска предзагрузки атрибутов/текстур (0)
    uint64_t shader_code_ptr;        // 0x08: Физический 64-битный адрес бинарного кода шейдера (Bifrost ISA) в ОЗУ
    uint64_t unk_bifrost_v6_ctrl1;   // 0x10: Системное управление кэшем команд Bifrost v6 (0)
    uint64_t unk_bifrost_v6_ctrl2;   // 0x18: Системный резерв управления ядрами (0)
} mali_shader_stage_t;

/* Состояние программы рендерера / Renderer State Descriptor (RSD — 128 байт) */
typedef struct GPU_PACKED GPU_ALIGN_128 {
    mali_shader_stage_t vertex_stage;   // 0x00 - 0x1F: Вершинная стадия (32 байта)
    mali_shader_stage_t fragment_stage; // 0x20 - 0x3F: Фрагментная стадия (32 байта)

    uint32_t blend_equation;            // 0x40: Маска блендинга (0x00001200 = Режим Opaque, перезапись пикселя)
    uint32_t blend_constant;            // 0x44: Константный цвет смешивания (0)
    uint64_t reserved_blend_fields;     // 0x48: Служебные поля блендинга Bifrost (0)

    // Аппаратные поинтеры с обязательными флагами-суффиксами (модифицируются через | UINT64_C)
    uint64_t attribute_meta_ptr;        // 0x50: Адрес структуры mali_vertex_input_meta | UINT64_C(1) [0x1.1]
    uint64_t varyings_meta_ptr;         // 0x58: Адрес описания интерполяции (0 = транзитный проброс)
    uint64_t attribute_buffer_ptr;      // 0x60: Прямой адрес начала triangle_vertices | UINT64_C(3) [0x1.1]

    uint64_t padding_to_128;            // 0x68 - 0x7F: Хвост структуры зануляется
} mali_renderer_state_bifrost_v6_t;

/* Динамическая куча тайлера (64 байта) */
typedef struct GPU_PACKED GPU_ALIGN_64 {
    uint64_t tiler_heap_free;        // 0x00: Текущий адрес старта свободной памяти кучи (GPU инкрементирует сам)
    uint64_t tiler_heap_end;         // 0x08: Физический адрес КОНЦА кучи МИНУС размер одного чанка (Start + Size - Chunk)
    uint32_t unknown;                // 0x10: Системный резерв (0)
    uint32_t flags;                  // 0x14: Ставим 0x00000001 (Аппаратный флаг активации кучи Bifrost)
    uint64_t chunk_size;             // 0x18: Атомарный размер блока памяти под тайл (для теста выставим 0x00000200 = 512Б)
    uint8_t  reserved[32];           // 0x20 - 0x3F: Занулено
} mali_bifrost_tiler_heap;

/* Дескриптор геометрии целевого экрана / Framebuffer Descriptor (MFBD — 64 байта) */
typedef struct GPU_PACKED GPU_ALIGN_64 {
    uint32_t width_minus_1;          // 0x00: Ширина экрана в пикселях - 1 (для микро-теста 16х16 пикселей = 15)
    uint32_t height_minus_1;         // 0x04: Высота экрана в пикселях - 1 (для теста = 15)
    uint32_t sample_mask;            // 0x08: Маска сглаживания и разметки (Ставим 0x0000FFFF — плитки 16х16)
    uint32_t unk_flags;              // 0x0C: Системный флаг формата Bifrost (Ставим 0x00000002)
    uint64_t tiler_heap_ptr;         // 0x10: Физический адрес структуры управления кучей mali_bifrost_tiler_heap
    uint8_t  reserved[32];           // 0x18 - 0x3F: Занулено
} mali_bifrost_fb_desc;
#endif /* GPU_BIFROST_V6_H */

#include <stdint.h>
#include <string.h>

/* Аппаратные коды типов задач */
//#define MALI_JOB_TYPE_VERTEX 5
//#define MALI_JOB_TYPE_TILER  7

/* Внешние бинарные массивы с кодом шейдеров (из вашего проекта) */

/* =========================================================================
 * 2. ГЕОМЕТРИЯ И БИНАРНЫЙ КОД ШЕЙДЕРОВ BIFROST V6
 * ========================================================================= */

/* Массив координат 3 вершин треугольника (X, Y, Z, W) — строго выровнен по 64 байтам */
GPU_ALIGN_64 const float triangle_vertices[] = {
    -0.5f, -0.5f, 0.0f, 1.0f,  // Вершина 0
     0.5f, -0.5f, 0.0f, 1.0f,  // Вершина 1
     0.0f,  0.5f, 0.0f, 1.0f   // Вершина 2
};

/*
 * Бинарный код Vertex-шейдера Bifrost (Identity/Transit Shader).
 * Просто перекладывает входные атрибуты координат в аппаратный кэш tiler-а.
 */
GPU_ALIGN_64 const uint32_t bifrost_vertex_shader_code[] = {
    0x00000000, 0x00000000, 0x7C002008, 0x00000000,
    0x80004000, 0x00000000, 0x00100002, 0x5E000000,
    0x00200002, 0x5E000000, 0x00300002, 0x5E000000,
    0x00400002, 0x5E000000, 0x00000000, 0x000002FF
};

/*
 * Бинарный код Fragment-шейдера Bifrost (Solid Fill Shader).
 * Окрашивает треугольник в сплошной цвет (в данном случае Red/Белый в зависимости от маски).
 */
GPU_ALIGN_64 const uint32_t bifrost_fragment_shader_code[] = {
    0x00000000, 0x00000000, 0x000000FF, 0x00000000,
    0x000000FF, 0x00000000, 0x000000FF, 0x00000000,
    0x000000FF, 0x00000000, 0x00001002, 0x7E000000,
    0x00002002, 0x00001DFF, 0x00000000, 0x000002FF
};

/* Выделение памяти под управляющие дескрипторы задач (128 байт для безопасности шины Bifrost) */
__attribute__((aligned(64))) static mali_vertex_job_bifrost v_job;
__attribute__((aligned(64))) static mali_tiler_job_bifrost  t_job;

/* Выделение памяти под внутренние структуры конвейера Bifrost v6 */
__attribute__((aligned(64))) static mali_bifrost_thread_input      v_thread_input;
__attribute__((aligned(128))) static mali_renderer_state_bifrost_v6_t gpu_program_state;
__attribute__((aligned(64))) static mali_bifrost_tiler_heap        tiler_heap;
__attribute__((aligned(64))) static mali_bifrost_fb_desc           fbd;
__attribute__((aligned(64))) static mali_vertex_input_meta         gpu_vertex_input_meta;

/* Динамический буфер, куда тайлер запишет итоговую разметку полигонов (минимум 4КБ) */
__attribute__((aligned(4096))) static uint8_t polygon_list_mem[4096];

void gpu_run_geometric_pipeline_test(void)
{
    /* 1. Полностью очищаем ОЗУ под все структуры перед заполнением */
    __builtin_memset(&v_job, 0, sizeof(v_job));
    __builtin_memset(&t_job, 0, sizeof(t_job));
    __builtin_memset(&v_thread_input, 0, sizeof(v_thread_input));
    __builtin_memset(&gpu_program_state, 0, sizeof(gpu_program_state));
    __builtin_memset(&tiler_heap, 0, sizeof(tiler_heap));
    __builtin_memset(&fbd, 0, sizeof(fbd));
    __builtin_memset(&gpu_vertex_input_meta, 0, sizeof(gpu_vertex_input_meta));
    __builtin_memset(polygon_list_mem, 0, sizeof(polygon_list_mem));

    // =========================================================================
    // 2. ИНИЦИАЛИЗАЦИЯ ВНУТРЕННИХ СТРУКТУР И МЕТАДАННЫХ
    // =========================================================================

    /* А. Параметры сетки потоков (v_thread_input) — под 3 вершины треугольника */
    v_thread_input.flags = 0x00000001;          // Включаем валидацию сетки
    v_thread_input.workgroup_size_x = 1;        // Потоки бьются по 1 вершине
    v_thread_input.workgroup_size_y = 1;
    v_thread_input.workgroup_size_z = 1;
    v_thread_input.grid_size_x = 3;             // Ровно 3 потока выполнения
    v_thread_input.grid_size_y = 1;
    v_thread_input.grid_size_z = 1;
    v_thread_input.index_count = 3;             // Без индексного буфера дублируем размер
    v_thread_input.index_buffer_ptr = 0;

    /* Б. Метаданные формата входных атрибутов (gpu_vertex_input_meta) */
    gpu_vertex_input_meta.buffer_ptr = (uintptr_t)triangle_vertices;
    gpu_vertex_input_meta.stride = 12;          // 3 float * 4 байта = 12 байт шаг
    gpu_vertex_input_meta.size = 36;            // 3 вершины * 12 байт = 36 байт общий размер
    gpu_vertex_input_meta.format = 0x0400030A;  // Аппаратный код Vec3 Float32 для Bifrost v6

    /* В. Описание программы выполнения (gpu_program_state / RSD — 128 байт) */
    // Вершинная стадия (32 байта)
    gpu_program_state.vertex_stage.properties = 0x00004000; // Базовый тип + дефолтные регистры
    gpu_program_state.vertex_stage.shader_code_ptr = (uintptr_t)bifrost_vertex_shader_code;

    // Фрагментная стадия (32 байта)
    gpu_program_state.fragment_stage.properties = 0x00004001;
    gpu_program_state.fragment_stage.shader_code_ptr = (uintptr_t)bifrost_fragment_shader_code;

    // Блендинг и интерполяция
    gpu_program_state.blend_equation = 0x00001200; // Режим Opaque (запись поверх)

    // Настраиваем внутренние указатели атрибутов RSD с обязательными суффиксами
    gpu_program_state.attribute_meta_ptr = (uintptr_t)&gpu_vertex_input_meta | UINT64_C(1);
    gpu_program_state.attribute_buffer_ptr = (uintptr_t)triangle_vertices | UINT64_C(3);
    gpu_program_state.varyings_meta_ptr = 0;

    /* Г. Управление кучей тайлера (tiler_heap) */
    // Считаем кучу размером 1 МБ, выделенную в ОЗУ (например, ваш регион кучи)
    // Пусть старт кучи будет равен адресу начала polygon_list_mem для изоляции тестов
    uintptr_t heap_start = (uintptr_t)polygon_list_mem;
    tiler_heap.tiler_heap_free = heap_start;
    tiler_heap.tiler_heap_end  = heap_start + 4096 - 512; // Конец буфера минус чанк безопасности
    tiler_heap.flags = 0x00000001;               // Активация кучи
    tiler_heap.chunk_size = 0x00000200;          // Выделяем маленькие чанки для теста (512 байт)

    /* Д. Описание параметров кадра экрана (fbd / Multi-Target Framebuffer Descriptor) */
    fbd.width_minus_1 = 15;                     // Маленький тестовый экран 16х16 (1 тайл)
    fbd.height_minus_1 = 15;
    fbd.sample_mask = 0x0000FFFF;               // Разметка под тайлы 16х16
    fbd.unk_flags = 0x00000002;                 // Минимальный флаг формата Bifrost
    fbd.tiler_heap_ptr = (uintptr_t)&tiler_heap; // Связываем с дескриптором кучи

    // =========================================================================
    // 3. СБОРКА ЦЕПОЧКИ ЗАДАЧ В СЛОТ 0 (VERTEX -> TILER)
    // =========================================================================

    /* Конфигурация Vertex Job */
    v_job.header.exception_status = 0;
    v_job.header.first_incomplete_task = 0;
    v_job.header.fault_pointer = 0;
    v_job.header.job_type = MALI_JOB_TYPE_VERTEX;
    v_job.header.job_descriptor_size = 1;       // 64-битные указатели
    v_job.header.job_barrier = 0;                // Барьер снят, конвейер течет дальше
    v_job.header.job_index = 1;                  // Индекс скорборда = 1
    v_job.header.job_dependency_index_1 = 0;
    v_job.header.next_job = (uintptr_t)&t_job;  // КРИТИЧНО: Указываем на следующий Tiler Job

    v_job.thread_input_record = (uintptr_t)&v_thread_input;
    v_job.renderer_state      = (uintptr_t)&gpu_program_state;
    v_job.attributes          = 0;               // Строго 0 для Bifrost v6
    v_job.attribute_buffers   = 0;               // Строго 0 для Bifrost v6

    /* Конфигурация Tiler Job */
    t_job.header.exception_status = 0;
    t_job.header.first_incomplete_task = 0;
    t_job.header.fault_pointer = 0;
    t_job.header.job_type = MALI_JOB_TYPE_TILER;
    t_job.header.job_descriptor_size = 1;
    t_job.header.job_barrier = 1;                // ФИНАЛЬНЫЙ барьер, закрывающий всю геометрию
    t_job.header.job_index = 1;                  // Тот же индекс контекста скорборда
    t_job.header.job_dependency_index_1 = 0;       // Зависимость 0, так как связь задана через next_job
    t_job.header.next_job = 0;                   // Конец цепочки геометрической фазы

    t_job.tiler_heap_desc = (uintptr_t)&tiler_heap;
    t_job.fb_desc         = (uintptr_t)&fbd;
    t_job.polygon_list    = (uintptr_t)polygon_list_mem;

    // =========================================================================
    // 4. СИНХРОНИЗАЦИЯ КЭША CPU (Очистка перед отправкой на шину)
    // =========================================================================

    /* Сбрасываем внутренние структуры из L1/L2 кэша процессора ARM в ОЗУ */
    dcache_clean((uintptr_t)&v_thread_input, sizeof(v_thread_input));
    dcache_clean((uintptr_t)&gpu_vertex_input_meta, sizeof(gpu_vertex_input_meta));
    dcache_clean((uintptr_t)&gpu_program_state, sizeof(gpu_program_state));
    dcache_clean((uintptr_t)&tiler_heap, sizeof(tiler_heap));
    dcache_clean((uintptr_t)&fbd, sizeof(fbd));
    dcache_clean((uintptr_t)bifrost_vertex_shader_code, 64); // С запасом под код
    dcache_clean((uintptr_t)triangle_vertices, 64);

    /* Сбрасываем и инвалидируем сами 128-байтные управляющие джобы */
    dcache_clean_invalidate((uintptr_t)&v_job, 128);
    dcache_clean_invalidate((uintptr_t)&t_job, 128);
    dcache_clean_invalidate((uintptr_t)polygon_list_mem, sizeof(polygon_list_mem));

    __DSB(); // Барьер системного интерконнекта ядра ARM

    // =========================================================================
    // 5. ОТПРАВКА СТРОГО ЧЕРЕЗ ВАШУ ФУНКЦИЮ В СЛОТ 0
    // =========================================================================
    /*
     * Передаем адрес головы цепочки (v_job).
     * Благодаря отсутствию абсолютных адресов компилятор соберет связи идеально,
     * дедлок Scoreboard убран, а скрытые поля Clipping-движка занулены паддингом.
     */
    int result = gpu_submit_job(0, (uintptr_t)&v_job);

    if (result == 0) {
        // ПОЛНЫЙ УСПЕХ! Геометрия просчитана, тайлы размечены.
        // Буфер polygon_list_mem теперь содержит списки примитивов треугольника.
    } else {
        // Ошибка. Анализируйте v_job.header.exception_status
    }
}

//---------------------

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

#if 1
	run_write_value_test();
	return;
//	unsigned v = 0;
//	while (run_write_value_test(v ++))
//		;
//    TP();
//    for (;;)
//    	;
#endif
    uintptr_t fbaddr = (uintptr_t) colmain_fb_draw();
    memset32((void *) fbaddr, COLORPIP_DARKCYAN, DIM_X * DIM_Y * 4);
    gpu_run_geometric_pipeline_test();
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

    /*
     *
     * 2. Как расшифровывается эта маска по байтам (Индексы от 0 до 7)
     * Mali считывает младшие 32 бита (LO) как 4 независимых правила кэширования:
     * Байт 0 (Индекс 0) = 0xFF: Традиционная кэшируемая память (Write-Back, Read/Write-Allocate). Основной тип для кода, текстур и дескрипторов.
     * Байт 1 (Индекс 1) = 0x88: Память с типом Write-Through (прямая запись, без аллокации кэша на запись).
     * Байт 2 (Индекс 2) = 0x44: Non-Cacheable (некэшируемая память). Именно этот индекс критически важен, если вы хотите отключить кэширование GPU для буфера результатов (как в тесте WRITE_VALUE).
     * Байт 3 (Индекс 3) = 0x00: Память типа Device (для регистров или специфического MMIO, если применимо).
     * Байты 4–7 (Индекс 4-7) = 0x00: Зарезервированы / не используются для стандартных буферов.
     *
     */
	// Индекс 0 = 0xAA (Cacheable), Индекс 1 = 0x22 (Non-Cacheable)
	GPU_MMU->MMU_AS[as].AS_MEMATTR_HI = 0x00000000;
	GPU_MMU->MMU_AS[as].AS_MEMATTR_LO = ~0;//0x004488ff;
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

	GPU_MMU->MMU_AS[as].AS_TRANSCFG_HI = 0x00;
	GPU_MMU->MMU_AS[as].AS_TRANSCFG_LO = 0x02;	//  (Включает режим адресации ARM 64-bit LPAE с размером страницы 4 КБ).

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

