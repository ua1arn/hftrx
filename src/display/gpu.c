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
	PRINTF("gpu_submit_job: head=%p, slot=%u\n", (void *) head, slot);
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
    	PRINTF("okay head %p:\n", (void *) head);
    	dcache_invalidate((uintptr_t) head, 128);
    	printhex32((uintptr_t) head, (void *) head, 128);
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
    uint8_t  job_barrier         : 1; // 0x11 (бит 0)  - Аппаратный барьер памяти
    uint8_t  unknown_flags       : 7; // 0x11 (биты 1-7) - Служебные флаги (обычно 0)

    uint16_t job_index;               // 0x12: Уникальный ID этой задачи для скорборда (например, 1)
    uint16_t job_dependency_index_1;  // 0x14: ID задачи, которую нужно дождаться перед запуском (0, если нет)
    uint16_t job_dependency_index_2;  // 0x16: ID второй зависимой задачи (0, если нет)

    uint64_t next_job;               // 0x18: Физический адрес следующего дескриптора в цепочке (0, если последний)
} mali_job_header;

#include <stdint.h>

#define MALI_WRITE_VALUE_ZERO     3 // Специальный флаг для обнуления

// Полезная нагрузка для записи значения
struct __attribute__((packed)) mali_payload_write_value {
	uint64_t address;
	uint32_t value_descriptor;
	uint32_t reserved;
	uint64_t immediate;
	uint64_t pad;
};

// Полный монолитный дескриптор задачи
// MGS! подтверждена работа заголовка
// https://android.googlesource.com/platform/external/mesa3d/+/e061bf004b5/src/panfrost/include/panfrost-job.h

struct __attribute__((packed, aligned(64))) mali_write_value_job {
    // 1. Стандартный заголовок (mali_job_header) - 32 байта
	mali_job_header header;
    // 2. Специфичный Payload - 32 байта
    struct mali_payload_write_value payload;
};

// Переменная-цель, куда будет писать GPU.
// Обязательно выравниваем по кэш-линии!
volatile uint64_t __attribute__((aligned(64))) gpu_test_target = 0x123456789abcdef;

int run_write_value_test(unsigned v) {
    // Создаем структуру дескриптора в памяти
	GPU_ALIGN static struct mali_write_value_job job;

    // Сбрасываем старую память
    memset(&job, 0, sizeof(job));

    // Заполняем заголовок
    job.header.exception_status = 0;
    job.header.job_descriptor_size = 1; // Используем 64-битные указатели
    job.header.job_type = MALI_JOB_TYPE_WRITE_VALUE; // Тип задачи = 2
    job.header.job_index = 1;           // Первая задача в Scoreboard
    job.header.next_job = 0;            // Цепочка заканчивается на ней
    job.header.job_barrier = 1;

    /**
     * value_descriptor:
     * 0 - timestamp?
     * 1 - timestamp?
     * 2 - ???
     * 3 - zero
     * 4 - 8 bit
     * 5 - 16 bit
     * 6 - 32 bit
     * 7 - 64 bit
     */
    // Заполняем Payload записи
    job.payload.address = (uintptr_t)&gpu_test_target; // Физический адрес цели
    job.payload.value_descriptor = v;                 // Бит [2] (Width): Разрядность данных Биты [1:0] (Type): Тип операции записи
    job.payload.immediate = 0xDEADBEEFABBA1980;               // Данные для записи

    // КРИТИЧЕСКИ ВАЖНО ДЛЯ BARE METAL:
    // Очищаем кэш CPU, чтобы GPU читал структуру из физического ОЗУ,
    // и инвалидируем регион gpu_test_target, чтобы CPU позже не прочитал старые данные из своего L1/L2.
    dcache_clean_invalidate((uintptr_t)&job, sizeof(job));
    gpu_test_target = 0x123456789abcdef;
    dcache_clean_invalidate((uintptr_t)&gpu_test_target, sizeof(gpu_test_target));

    // Важно: Адреса job и gpu_test_target должны быть предварительно
    // промаплены в MMU вашего Mali-G31 с правами Read/Write!

    if (gpu_submit_job(2, (uintptr_t) & job))
    	return 1;	// err
      // Проверяем результат выполнения
    dcache_invalidate((uintptr_t)&gpu_test_target, sizeof(gpu_test_target));

    if (/*job.header.exception_status == 0 && */gpu_test_target == 0xDEADBEEFABBA1980) {
        // УСПЕХ! GPU успешно прочитал дескриптор, записал значение через шину в ОЗУ и завершил задачу.
    	PRINTF("Okay write value! v=%02X gpu_test_target=%08X%08X\n", v, (unsigned) (gpu_test_target >> 32), (unsigned) gpu_test_target);
    } else {
        // ОШИБКА. Проверьте job.exception_status или глобальные регистры MMU FAULT.
    	PRINTF("No write value! v=%02X gpu_test_target=%08X%08X\n", v, (unsigned) (gpu_test_target >> 32), (unsigned) gpu_test_target);
    	return 1;	// err
    }
    return 0;
}

// Расширенный дескриптор для Vertex/Tiler задач
typedef struct __attribute__((packed)) {
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

typedef struct __attribute__((packed)) {
    uint32_t properties;             // +0x00: Тип стадии и количество регистров
    uint16_t stack_size;             // +0x04: Размер стека для ветвлений (0)
    uint16_t preload_regs;           // +0x06: Аппаратная предзагрузка (0)
    uint64_t shader_code_ptr;        // +0x08: 64-битный физический адрес кода шейдера
    uint64_t unk_bifrost_v6_ctrl1;   // +0x10: Скрытый системный контроль 1 (0)
    uint64_t unk_bifrost_v6_ctrl2;   // +0x18: Скрытый системный контроль 2 (0)
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

void gpu_init_program_state( void)
{
    // ... [Полное зануление структуры gpu_program_state] ...

    // 1. Инициализация дескриптора атрибутов вершин (gpu_vertex_input_meta)
    gpu_vertex_input_meta.buffer_ptr = (uintptr_t)triangle_vertices;
    gpu_vertex_input_meta.stride = 12; // 3 float * 4 байта = 12 байт
    gpu_vertex_input_meta.size = sizeof(triangle_vertices);
    gpu_vertex_input_meta.format = 0x0400030A; // Аппаратный код Vec3 Float32 для Bifrost v6

    // 2. Параметры стадий шейдеров (из вашего исходного кода)
    gpu_program_state.vertex_stage.properties = 0x00004000;
    gpu_program_state.vertex_stage.shader_code_ptr = (uintptr_t)bifrost_vertex_shader_code;

    gpu_program_state.fragment_stage.properties = 0x00004001;
    gpu_program_state.fragment_stage.shader_code_ptr = (uintptr_t)bifrost_fragment_shader_code;

    gpu_program_state.blend_equation = 0x00001200;

    // 3. ПРИМЕНЕНИЕ СИСТЕМНЫХ МАСОК И СУФФИКСОВ ЧЕРЕЗ UINT64_C
    // Направляем attribute_meta_ptr на нашу структуру gpu_vertex_input_meta с маской | 1
    gpu_program_state.attribute_meta_ptr = (uintptr_t)&gpu_vertex_input_meta | UINT64_C(1);

    // Направляем attribute_buffer_ptr на массив координат с маской | 3
    gpu_program_state.attribute_buffer_ptr = (uintptr_t)triangle_vertices | UINT64_C(3);

    gpu_program_state.varyings_meta_ptr = 0;

    // СИНХРОНИЗАЦИЯ КЭША (Канонические вызовы проекта hftrx)
    dcache_clean((uintptr_t)&gpu_vertex_input_meta, sizeof(gpu_vertex_input_meta));
    dcache_clean((uintptr_t)&gpu_program_state, sizeof(gpu_program_state));
    dcache_clean((uintptr_t)bifrost_vertex_shader_code, sizeof(bifrost_vertex_shader_code));
    dcache_clean((uintptr_t)bifrost_fragment_shader_code, sizeof(bifrost_fragment_shader_code));
    __DSB();

    PRINTF("gpu_vertex_input_meta @%p:\n", &gpu_vertex_input_meta);
    printhex32((uintptr_t) &gpu_vertex_input_meta, &gpu_vertex_input_meta, sizeof gpu_vertex_input_meta);

    PRINTF("gpu_program_state @%p:\n", &gpu_program_state);
    printhex32((uintptr_t) &gpu_program_state, &gpu_program_state, sizeof gpu_program_state);

    PRINTF("bifrost_vertex_shader_code @%p:\n", &bifrost_vertex_shader_code);
    printhex32((uintptr_t) &bifrost_vertex_shader_code, &bifrost_vertex_shader_code, sizeof bifrost_vertex_shader_code);

    PRINTF("bifrost_fragment_shader_code @%p:\n", &bifrost_fragment_shader_code);
    printhex32((uintptr_t) &bifrost_fragment_shader_code, &bifrost_fragment_shader_code, sizeof bifrost_fragment_shader_code);
}

// Номера слотов задач (обычно слот 0 - Vertex/Compute, слот 1 - Fragment)
#define MALI_FB_FORMAT_ARGB8888    0x18001000

/* --- МЛАДШИЕ 16 БИТ: Успешное завершение (Job Done) --- */
#define JOB_INT_BIT_SLOT_0_DONE    (1 << 0)  /* Задача в Слоте 0 выполнена успешно (Vertex/Tiler) */
#define JOB_INT_BIT_SLOT_1_DONE    (1 << 1)  /* Задача в Слоте 1 выполнена успешно (Fragment) */
#define JOB_INT_BIT_SLOT_2_DONE    (1 << 2)  /* Задача в Слоте 2 выполнена успешно (Compute) */

/* --- СТАРШИЕ 16 БИТ: Аппаратные сбои (Job Fault) --- */
#define JOB_INT_BIT_SLOT_0_FAULT   (1 << 16) /* Критическая ошибка выполнения в Слоте 0 */
#define JOB_INT_BIT_SLOT_1_FAULT   (1 << 17) /* Критическая ошибка выполнения в Слоте 1 */
#define JOB_INT_BIT_SLOT_2_FAULT   (1 << 18) /* Критическая ошибка выполнения в Слоте 2 */

#define MALI_BIFROST_PRIM_TRIANGLES          (0x4ULL << 0)   /* Тип примитива: GL_TRIANGLES */
#define MALI_BIFROST_PRIM_INDEX_NONE         (0x0ULL << 4)   /* Без индексного буфера (Arrays Mode) */
#define MALI_BIFROST_PRIM_INDEX_U16          (0x2ULL << 4)   /* Если бы использовался Index Buffer uint16_t */
#define MALI_BIFROST_PRIM_CULL_NONE          (0x0ULL << 8)   /* Выключить отсечение нелицевых граней (No Culling) */
#define MALI_BIFROST_PRIM_CULL_CCW           (0x1ULL << 8)   /* Отсекать Counter-Clockwise полигоны */
#define MALI_BIFROST_PRIM_CULL_CW            (0x2ULL << 8)   /* Отсекать Clockwise полигоны */

#if 0
mali_vertex_job v_job;
mali_tiler_job  t_job;

// 1. НАСТРОЙКА VERTEX JOB (Первое звено цепи)
v_job.job_descriptor_size = 1;
v_job.job_type = 5;                        // MALI_JOB_TYPE_VERTEX
v_job.job_index = 1;                       // Задаем уникальный ID = 1
v_job.job_dependency_index_1 = 0;          // Никого не ждет
v_job.next_job = (uint64_t)&t_job;         // Указываем на следующий Tiler джоб

v_job.thread_input_record = (uint64_t)&v_thread_input; // Сетка вершин (кол-во потоков = числу вершин)
v_job.renderer_state      = (uint64_t)&v_rsd;          // Параметры вершинного шейдера
v_job.attributes          = (uint64_t)&attr_table;     // Ссылка на геометрию (треугольник)
v_job.attribute_buffers   = (uint64_t)&attr_buf_table;

// 2. НАСТРОЙКА TILER JOB (Второе звено цепи)
t_job.header.job_descriptor_size = 1;
t_job.header.job_type = 7;                 // MALI_JOB_TYPE_TILER (В байте 0x10 будет 0x0F)
t_job.header.job_barrier = 1;              // Финальный барьер
t_job.header.job_index = 2;                // Задаем уникальный ID = 2
t_job.header.job_dependency_index_1 = 1;   // КРИТИЧЕСКИ ВАЖНО: Ждет завершения задачи ID = 1 (v_job)
t_job.header.next_job = 0;                 // Цепочка закончена

t_job.tiler_heap_desc = (uint64_t)&tiler_heap;
t_job.fb_desc         = (uint64_t)&fbd;
t_job.polygon_list    = (uint64_t)polygon_list_buffer;

#endif
//void gpu_draw_triangle2(uintptr_t framebuffer_phys_addr, uint32_t width, uint32_t height);
/**
 * @brief Структура параметров сетки потоков (Thread Input Record) для Mali Bifrost.
 * Размер: 64 байта. Выравнивание строго по 64 байтам.
 */
typedef struct __attribute__((packed, aligned(64))) {
    /**
     * 0x00: Флаги конфигурации сетки (Grid Control Flags).
     * Бит 0: Активация кастомного размера рабочих групп (обычно 1).
     * Бит 12: Формат индексов (0 - 16-бит, 1 - 32-бит, если используется Index Buffer).
     * Для базового некогерентного Vertex Job без сложных индексов пишем: 0x00000001.
     */
    uint32_t flags;

    /* --- Размеры рабочей группы (Workgroup Size) --- */
    uint32_t workgroup_size_x;   // 0x04: Количество потоков в одной группе по X (обычно 1)
    uint32_t workgroup_size_y;   // 0x08: Количество потоков по Y (всегда 1 для Vertex)
    uint32_t workgroup_size_z;   // 0x0C: Количество потоков по Z (всегда 1 для Vertex)

    /* --- Размеры глобальной сетки (Grid Size / Общее число вершин) --- */
    uint32_t grid_size_x;        // 0x10: Общее количество обрабатываемых вершин (для треугольника = 3)
    uint32_t grid_size_y;        // 0x14: Всегда 1 для Vertex
    uint32_t grid_size_z;        // 0x18: Всегда 1 для Vertex

    /* --- Специфичные для IDVS (Index-Driven Vertex Shading) параметры --- */
    uint32_t index_count;        // 0x1C: Количество индексов в буфере (если рисуем без Index Buffer - дублируем grid_size_x, т.е. 3)
    uint64_t index_buffer_ptr;   // 0x20: Физический адрес буфера индексов (0, если рисуем простым массивом без индексов)

    uint64_t unknown_fields;   // 0x28: Зануляем
    uint64_t padding[2];         // 0x30 - 0x3F: Зануляем остаток структуры до 64 байт
} mali_bifrost_thread_input;
/**
 * @brief Описатель одного буфера атрибутов в Bifrost v6.
 * Размер: 32 байта.
 */
struct __attribute__((packed)) mali_bifrost_attr_buffer_desc {
    /**
     * 0x00: Физический адрес начала буфера данных в системном ОЗУ.
     * Сюда мы запишем адрес нашего массива вершин (например, 0x40135AC0).
     */
    uint64_t pointer;

    /**
     * 0x08: Размер буфера в байтах, уменьшенный на 1 (Size - 1).
     * Защищает от выхода GPU за пределы памяти (Out-of-Bounds).
     * Для 3 вершин по 3 float (X,Y,Z) размер = 3 * 3 * 4 = 36 байт.
     * Значение: 36 - 1 = 35 (0x00000023).
     */
    uint32_t size_minus_1;

    /**
     * 0x0C: Шаг (Stride) между элементами в байтах.
     * Задает расстояние от начала одной вершины до начала следующей.
     * Если буфер содержит только плотные float3 координаты (X,Y,Z),
     * то шаг равен: 3 * 4 байта = 12 байт (0x0000000C).
     */
    uint32_t stride;

    /**
     * 0x10: Флаги конфигурации буфера (Аппаратный тип).
     * Бит 0: Признак валидности (1 = буфер активен).
     * Биты [4:1]: Тип аллокации/доступа.
     * Для стандартного плоского массива вершин на "голом железе" пишем: 0x00000001.
     */
    uint32_t flags;

    uint32_t reserved1; // 0x14: Зануляем
    uint64_t reserved2; // 0x18: Зануляем
};

// Выделяем 64 байта в ОЗУ под таблицу связей атрибутов
GPU_ALIGN static uint32_t attr_table_words[16] = {0};

void setup_attribute_table(void) {
    // Запись для Атрибута 0 (Координаты вершин):
    // Бит [0:2]   - Индекс буфера из таблицы attr_buf_table (у нас это Буфер 0) -> 0
    // Биты [11:3]  - Смещение внутри буфера в байтах (наш массив выровнен, смещение 0) -> 0
    // Биты [31:12] - Аппаратный формат Bifrost (для Vec3 Float32 это код 0x04000)

    // В ревизии Bifrost v6 (Mali-G31) точная маска для Vec3 Float32 из Буфера 0:
    attr_table_words[0] = 0x0400030A; // Формат данных + привязка к Buffer 0

    // Остальные элементы оставляем нулями (терминаторы)
    dcache_clean((uintptr_t)attr_table_words, sizeof(attr_table_words));

    PRINTF("attr_table_words @%p:\n", &attr_table_words);
    printhex32((uintptr_t) &attr_table_words, &attr_table_words, sizeof attr_table_words);
}

// Глобальные или статические буферы (выделенные в ОЗУ вашего проекта)
GPU_ALIGN static mali_vertex_job v_job;
GPU_ALIGN static mali_tiler_job  t_job; // Выделяем Tiler Job как честную переменную в ОЗУ

// Буфер под итоговый Polygon List (выделите с запасом, например, 4 КБ)
GPU_ALIGN static uint8_t polygon_list_mem[4096];

void gpu_draw_triangle(void)
{
    enum {
        COMMAND_SLOT_VERTEX = 0,
        COMMAND_SLOT_FRAGMENT
    };
    gpu_init_program_state();
    setup_attribute_table();

    // Область памяти для кучи тайлера (Mali Tiler требует буфер для сортировки геометрии)
    __attribute__((aligned(4096))) static uint8_t tiler_heap_mem[64 * 10 * 1024];
    struct mali_bifrost_tiler_heap {
        uint64_t tiler_heap_free; // Сюда пишем физический адрес START
        uint64_t tiler_heap_end;  // Физический адрес: START + SIZE - CHUNK_SIZE
        uint32_t padding;         // 0x0
        uint32_t flags;           // 0x00000001 (Флаг активации кучи)
        uint64_t chunk_size;      // 0x00008000 (Задаем размер чанка 32 КБ)
        uint64_t pad[4];          // 0x20: Зануляем остаток до 64 байт
    };
    static GPU_ALIGN struct mali_bifrost_tiler_heap tiler_heap =
    {
            .tiler_heap_free = (uintptr_t) tiler_heap_mem,
            .tiler_heap_end = (uintptr_t) tiler_heap_mem + 64 * 10 * 1024 - (32 * 1024),
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
    GPU_ALIGN static struct __attribute__((packed, aligned(64))) mali_bifrost_rsd {
        // Первое 32-битное слово содержит конфигурацию свойств шейдера
        uint32_t flags; // Запишем 0x00000002 (Минимальный флаг, указывающий на базовый тип шейдера)
        uint32_t unknown;
        uint64_t shader_code_ptr; // Указатель на код шейдера. Пока пишем 0x0 (пустой шейдер)
        uint8_t  pad[48]; // Зануляем остаток
    } rsd;
    rsd.flags = 2;
    rsd.unknown = 0x00000100;
    dcache_clean((uintptr_t)&rsd, sizeof rsd);
    dcache_clean((uintptr_t)&tiler_payload, sizeof tiler_payload);
    GPU_ALIGN static mali_bifrost_thread_input v_thread_input;

    // Сбрасываем старую память
    __builtin_memset(&v_thread_input, 0, sizeof(v_thread_input));

    // Инициализируем параметры под 3 вершины
    v_thread_input.flags = 0x00000001;          // Включаем валидацию сетки

    v_thread_input.workgroup_size_x = 1;        // Потоки бьются на группы по 1 вершине
    v_thread_input.workgroup_size_y = 1;
    v_thread_input.workgroup_size_z = 1;

    v_thread_input.grid_size_x = 3;             // Генерируем ровно 3 потока (по 1 на вершину треугольника)
    v_thread_input.grid_size_y = 1;
    v_thread_input.grid_size_z = 1;

    v_thread_input.index_count = 3;             // 3 вершины в неиндексированном режиме
    v_thread_input.index_buffer_ptr = 0;        // Буфер индексов отсутствует (рисуем последовательно)
    dcache_clean((uintptr_t)&v_thread_input, sizeof v_thread_input);

    PRINTF("v_thread_input @%p:\n", &v_thread_input);
    printhex32((uintptr_t) &v_thread_input, &v_thread_input, sizeof v_thread_input);

    // Полностью зануляем дескрипторы перед заполнением
    __builtin_memset(&v_job, 0, sizeof(v_job));
    __builtin_memset(&t_job, 0, sizeof(t_job));
    __builtin_memset(polygon_list_mem, 0, sizeof(polygon_list_mem));

    // =========================================================================
    // 1. СБОРКА VERTEX JOB (Первое звено цепи в Slot 0)
    // =========================================================================
    v_job.header.exception_status = 0;
    v_job.header.first_incomplete_task = 0;
    v_job.header.fault_pointer = 0;

    // (MALI_JOB_TYPE_VERTEX << 1) | MALI_JOB_64 -> (5 << 1) | 1 = 11 (0x0B)
    v_job.header.job_type = 5;
    v_job.header.job_descriptor_size = 1;
    v_job.header.job_barrier = 0;          // Не блокируем конвейер, даем течь к тайлеру
    v_job.header.job_index = 1;            // Уникальный ID задачи = 1
    v_job.header.job_dependency_index_1 = 0; // Никого не ждем

    // АВТОМАШТАБИРОВАНИЕ: next_job указывает строго на нашу структуру t_job в памяти
    v_job.header.next_job = (uintptr_t)&t_job;

    // Полезная нагрузка (Payload) вершинной стадии
    v_job.thread_input_record = (uintptr_t)&v_thread_input;
    v_job.renderer_state      = (uintptr_t)&gpu_program_state; // RSD (40373180)
    v_job.attributes          = 0; // Для Bifrost v6 эти поля строго зануляются
    v_job.attribute_buffers   = 0; // так как данные считываются из RSD выше

    // =========================================================================
    // 2. СБОРКА TILER JOB (Второе звено цепи в Slot 0)
    // =========================================================================
    t_job.header.exception_status = 0;
    t_job.header.first_incomplete_task = 0;
    t_job.header.fault_pointer = 0;

    // (MALI_JOB_TYPE_TILER << 1) | MALI_JOB_64 -> (7 << 1) | 1 = 15 (0x0F)
    t_job.header.job_type = 7;
    t_job.header.job_descriptor_size = 1;
    t_job.header.job_barrier = 1;          // Финальный аппаратный барьер в конце цепи
    t_job.header.job_index = 1;            // Уникальный ID задачи = 2

    // КРИТИЧНО: Явно заставляем тайлер дождаться выполнения задачи ID = 1 (v_job)
    t_job.header.job_dependency_index_1 = 0;//1;
    t_job.header.next_job = 0;             // Цепочка для Slot 0 на этом завершена

    // Полезная нагрузка (Payload) стадии тайлинга
    t_job.tiler_heap_desc = (uintptr_t)&tiler_heap; // Ваша структура кучи
    t_job.fb_desc         = (uintptr_t)&fbd;        // Ваша структура фреймбуфера кадра
    t_job.polygon_list    = (uintptr_t)polygon_list_mem; // Динамический буфер вывода геометрии

    // =========================================================================
    // 3. СИНХРОНИЗАЦИЯ КЭША CPU (Очистка перед отправкой на шину)
    // =========================================================================
    dcache_clean((uintptr_t)&v_thread_input, sizeof(v_thread_input));
    dcache_clean((uintptr_t)&gpu_program_state, sizeof(gpu_program_state));
    dcache_clean((uintptr_t)&gpu_vertex_input_meta, sizeof(gpu_vertex_input_meta));
    dcache_clean((uintptr_t)&tiler_heap, sizeof(tiler_heap));
    dcache_clean((uintptr_t)&fbd, sizeof(fbd));

    // Выталкиваем из кэша сами дескрипторы задач и буфер вывода
    dcache_clean_invalidate((uintptr_t)&v_job, sizeof(v_job));
    dcache_clean_invalidate((uintptr_t)&t_job, sizeof(t_job));
    dcache_clean_invalidate((uintptr_t)polygon_list_mem, sizeof(polygon_list_mem));

    __DSB(); // Жесткий системный барьер памяти процессора

    PRINTF("v_job @%p:\n", &v_job);
    printhex32((uintptr_t) &v_job, &v_job, sizeof v_job);

    PRINTF("t_job @%p:\n", &t_job);
    printhex32((uintptr_t) &t_job, &t_job, sizeof t_job);

    // =========================================================================
    // 4.ОТПРАВКА ЦЕПОЧКИ В СЛОТ 0 GPU
    // =========================================================================
    gpu_submit_job(0, (uintptr_t) &v_job);
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

#if 1
	unsigned v = 0;
	while (run_write_value_test(v ++))
		;
    TP();
    for (;;)
    	;
#endif
    uintptr_t fbaddr = (uintptr_t) colmain_fb_draw();
    memset32((void *) fbaddr, COLORPIP_DARKCYAN, DIM_X * DIM_Y * 4);
    gpu_draw_triangle();
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

