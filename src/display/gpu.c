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

#define MALI_PTE_VALID          (1ULL << 0)
#define MALI_PTE_BLOCK          (0ULL << 1) // Финальный блок (разрешен на L1 для 1GB и на L2 для 2MB)
#define MALI_PTE_PAGE           (1ULL << 1) // Указатель на следующую таблицу (на L0, L1, L2)
#define MALI_PTE_USER           (1ULL << 6)
#define MALI_PTE_SHARE_OUTER    (2ULL << 8)
#define MALI_PTE_AF             (1ULL << 10) // Access Flag
#define MALI_PTE_ATTR_IDX_1     (1ULL << 2)  // Non-Cacheable домен (0x22 из MEMATTR)

// Корневая таблица Уровня 0 (Level 0)
__attribute__((aligned(4096))) static uint64_t gpu_mmu_l0_strict_table [512];

// Плоская таблица Уровня 1 (Level 1) на 4 гигабайтных блока
__attribute__((aligned(4096))) static uint64_t gpu_mmu_l1_block_table [512];


void gpu_mmu_build_4gb_l1_blocks(void)
{
    PRINTF("Mali-G31: Building 4GB Identity Mapping via L0->L1 block structure...\n");

    // Полностью зануляем таблицы
    for (int i = 0; i < 512; i++) {
        gpu_mmu_l0_strict_table[i] = 0;
        gpu_mmu_l1_block_table[i] = 0;
    }

    uint64_t phys_addr = 0x00000000;

    // 1. Заполняем таблицу Уровня 1 (L1) четырьмя честными блоками по 1 Гигабайту
    for (int gb = 0; gb < 4; gb++) {
        // На Уровне 1 бит 1 равен 0 (BLOCK) — это легитимный конечный 1ГБ кусок памяти в LPAE
        gpu_mmu_l1_block_table[gb] = phys_addr |
                                     MALI_PTE_VALID |
                                     MALI_PTE_BLOCK |
                                     MALI_PTE_USER |	// if commented - MMU Fault Status (0x01802420): 0x7C0002C8
                                     MALI_PTE_SHARE_OUTER |
                                     MALI_PTE_AF |          /* Бит 10 взведен */
                                     MALI_PTE_ATTR_IDX_1 |
									 0;

        phys_addr += (1ULL * 1024ULL * 1024ULL * 1024ULL); // Шаг 1 ГБ
    }

    // 2. Связываем корневую таблицу L0 с нашей L1 таблицей блоков
    // Индекс 0 покрывает первые 512 Гигабайт, что с избытком накрывает наши 4 ГБ.
    // На Уровне 0 дескриптор ОБЯЗАН быть Table Pointer (VALID + PAGE)
    uint64_t l1_table_phys = (uintptr_t)gpu_mmu_l1_block_table;
    gpu_mmu_l0_strict_table [0] = l1_table_phys | MALI_PTE_VALID | MALI_PTE_PAGE;

    // 3. Синхронизируем память через dcache_clean вашего проекта hftrx
    dcache_clean((uintptr_t)gpu_mmu_l0_strict_table, sizeof(gpu_mmu_l0_strict_table));
    dcache_clean((uintptr_t)gpu_mmu_l1_block_table, sizeof(gpu_mmu_l1_block_table));

    __DSB();
}


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
    uint64_t draw_flags;
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
} mali_fragment_job;

// Выравнивание для кэш-линий GPU
#define GPU_ALIGN __attribute__((aligned(64)))

// Координаты вершин треугольника: X, Y, Z (в нормализованных координатах от -1.0 до 1.0)
GPU_ALIGN static float triangle_vertices[] = {
     0.0f,  0.5f, 0.0f, // Верхняя точка
    -0.5f, -0.5f, 0.0f, // Нижняя левая
     0.5f, -0.5f, 0.0f  // Нижняя правая
};

// Выделяем память под дескрипторы аппаратных задач в RAM
GPU_ALIGN static mali_tiler_job    v_job;
GPU_ALIGN static mali_fragment_job f_job;

// Область памяти для кучи тайлера (Mali Tiler требует буфер для сортировки геометрии)
__attribute__((aligned(4096))) static uint8_t tiler_heap_mem[64 * 00 * 1024];

typedef struct __attribute__((packed)) {
    uint64_t base_address;  // Физический адрес экрана (из вашего Display Engine)
    uint32_t width;
    uint32_t height;
    uint32_t stride;        // Шаг строки в байтах (width * 4 для 32-бит RGBA)
    uint32_t format;        // Формат пикселя (например, 0x18004000 для RGBA8888)
} mali_framebuffer_desc;

GPU_ALIGN static mali_framebuffer_desc fb_desc;

// Номера слотов задач (обычно слот 0 - Vertex/Compute, слот 1 - Fragment)
#define COMMAND_SLOT_VERTEX   0
#define COMMAND_SLOT_FRAGMENT 1
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

void gpu_draw_triangle(uintptr_t framebuffer_phys_addr, uint32_t width, uint32_t height)
{
    PRINTF("Assembling GPU Job Chain for Triangle...\n");
    // https://android.googlesource.com/platform/external/mesa3d/+/e061bf004b5/src/panfrost/include/panfrost-job.h

    // 1. Инициализация описания целевого экрана (Framebuffer)
    fb_desc.base_address = framebuffer_phys_addr;
    fb_desc.width = width;
    fb_desc.height = height;
    fb_desc.stride = width * 4;
    fb_desc.format = MALI_FB_FORMAT_ARGB8888;//0x18004000; // Простой формат записи RGBA8888 в Bifrost

    // 2. Настройка первой задачи: Координаты и геометрия (Vertex / Tiler Job)
    v_job.header.job_type = JOB_TYPE_TILER;
    v_job.header.job_index = 0;
    v_job.header.job_descriptor_size = (sizeof v_job + 7) / 8;
    v_job.header.next_job = 0;//(uintptr_t) & f_job;//0; // Конец первой цепочки (или можно связать с f_job, если аппаратно поддерживается)

    v_job.tiler_heap = (uintptr_t)tiler_heap_mem;
    v_job.vertex_buffer = (uintptr_t)triangle_vertices;
    v_job.vertex_count = 3; // 3 вершины формируют 1 треугольник
    v_job.draw_flags = 0x00000004; // Флаг типа примитива: TRIANGLES

    // 3. Настройка второй задачи: Отрисовка пикселей (Fragment Job)
    f_job.header.job_type = JOB_TYPE_FRAGMENT;
    f_job.header.job_index = 0;
    f_job.header.job_descriptor_size = (sizeof f_job + 7) / 8;
    f_job.header.next_job = 0; // Последняя задача

    f_job.framebuffer_desc = (uintptr_t)&fb_desc;
    f_job.tile_render_list = (uintptr_t)tiler_heap_mem; // Читает геометрию из кучи тайлера
    f_job.width_minus_1 = width - 1;
    f_job.height_minus_1 = height - 1;
    f_job.clear_color = 0xFF0000FF; // Фоновый цвет очистки, если нужно (Красный)

    // КРИТИЧЕСКИ ВАЖНО: Выталкиваем структуры из кэша ядер процессора (CPU L1/L2) в ОЗУ,
    // чтобы GPU увидел актуальные данные через шину AXI.
    // Если у вас в hftrx есть функции работы с кэшем, вызовите их:
     dcache_clean((uintptr_t)&v_job, sizeof(v_job));
     dcache_clean((uintptr_t)&f_job, sizeof(f_job));
     dcache_clean((uintptr_t)&fb_desc, sizeof(fb_desc));
     dcache_clean((uintptr_t)triangle_vertices, sizeof(triangle_vertices));

    // 4. Запуск цепочки геометрии в Slot 0
    PRINTF("Submitting Vertex Job to Slot 0...\n");

    // Записываем физический адрес начала структуры v_job в регистр указателя слота 0
    GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_VERTEX].JS_HEAD_NEXT_HI = ptr_hi32((uintptr_t)&v_job);//(uint32_t)(((uintptr_t)&v_job >> 32) & 0xFFFFFFFF);
    GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_VERTEX].JS_HEAD_NEXT_LO = ptr_lo32((uintptr_t)&v_job);//(uint32_t)((uintptr_t)&v_job & 0xFFFFFFFF);

    // 2. ИНИЦИАЛИЗАЦИЯ JS_AFFINITY (Критично для Bifrost!)
    // Говорим планировщику распределить потоки шейдеров на оба ядра Mali-G31 MP2
    GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_VERTEX].JS_AFFINITY_NEXT_HI = ~0;//0x00000003;
    GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_VERTEX].JS_AFFINITY_NEXT_LO = ~0;//0x00000003;

    // Дополнительно для Bifrost рекомендуется сбросить расширенную конфигурацию слота
    GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_VERTEX].JS_CONFIG_NEXT = 0x00000000;

    //GPU_JOB_CONTROL->JOB_INT_MASK = 0xFFFFFFFF;	// Это разрешает вызовы обработчика прерываний
    GPU_JOB_CONTROL->JOB_IRQ_MASK = 0xFFFFFFFF;

    PRINTF("0 GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT);
    PRINTF("0 GPU_JOB_CONTROL->JOB_INT_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_INT_RAWSTAT);

    // Команда START (обычно значение 0x01 в регистр JS_COMMAND)
    GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_VERTEX].JS_COMMAND_NEXT = 0x01;
    __DSB();
    TP();

    local_delay_ms(200);
//    PRINTF("GPU_MMU:\n");
//    printhex32(GPU_MMU_BASE, GPU_MMU, 4096);
    PRINTF("v_job: %p (%u)\n", & v_job, (unsigned) sizeof v_job);
    PRINTF("f_job: %p(%u)\n", & f_job, (unsigned) sizeof f_job);
    //memset32(GPU_JOB_CONTROL, ~0, 4096);
//	PRINTF("GPU_JOB_CONTROL:\n");
//	printhex32(GPU_JOB_CONTROL_BASE, GPU_JOB_CONTROL, 4096);
//    for (;;)
//    	;
//	PRINTF("GPU_MMU:\n");
//	printhex32(GPU_MMU_BASE, GPU_MMU, 4096);
   PRINTF("GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT);
   PRINTF("GPU_JOB_CONTROL->JOB_INT_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_INT_RAWSTAT);
    gpu_diagnose_fault(COMMAND_SLOT_VERTEX);
    PRINTF("SLOT_STATUS: 0x%08X, RAW_STATUS: 0x%08X\n", (unsigned) GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_VERTEX].JS_STATUS, (unsigned) GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_VERTEX].JS_STATUS);
    // Ожидание завершения работы аппаратного тайлера геометрии
    // В hftrx прерывания выводят ASSERT(0), поэтому опрашиваем статус в цикле (polling)
    while ((GPU_JOB_CONTROL->JOB_INT_RAWSTAT & (1 << COMMAND_SLOT_VERTEX)) == 0) { //Was: JOB_IRQ_RAWSTAT
        // Если произошел сбой, сработает MMU или Job Fault прерывание
    }
    local_delay_ms(200);
    TP();
    GPU_JOB_CONTROL->JOB_INT_CLEAR = (1 << COMMAND_SLOT_VERTEX); // Сброс флага прерывания
    PRINTF("a GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT);
    PRINTF("a GPU_JOB_CONTROL->JOB_INT_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_INT_RAWSTAT);

    // 5. Запуск цепочки растеризации фрагментов в Slot 1
    PRINTF("Submitting Fragment Job to Slot 1...\n");

    GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_FRAGMENT].JS_HEAD_NEXT_HI = ptr_hi32((uintptr_t)&f_job);//(uint32_t)(((uintptr_t)&f_job >> 32) & 0xFFFFFFFF);
    GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_FRAGMENT].JS_HEAD_NEXT_LO = ptr_lo32((uintptr_t)&f_job);//(uint32_t)((uintptr_t)&f_job & 0xFFFFFFFF);

    // 2. ИНИЦИАЛИЗАЦИЯ JS_AFFINITY (Критично для Bifrost!)
    // Говорим планировщику распределить потоки шейдеров на оба ядра Mali-G31 MP2
    GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_FRAGMENT].JS_AFFINITY_NEXT_HI = ~0;//0x00000003;
    GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_FRAGMENT].JS_AFFINITY_NEXT_LO = ~0;//0x00000003;

    // Дополнительно для Bifrost рекомендуется сбросить расширенную конфигурацию слота
    GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_FRAGMENT].JS_CONFIG_NEXT = 0x00000000;

    GPU_JOB_CONTROL->LOOP[COMMAND_SLOT_FRAGMENT].JS_COMMAND_NEXT = 0x01;

    local_delay_ms(200);
    PRINTF("4 GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT);
    PRINTF("4 GPU_JOB_CONTROL->JOB_INT_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_INT_RAWSTAT);
    gpu_diagnose_fault(COMMAND_SLOT_FRAGMENT);
    PRINTF("v_job:\n");
    printhex32((uintptr_t) & v_job, & v_job, sizeof v_job);
    PRINTF("f_job:\n");
    printhex32((uintptr_t) & f_job, & f_job, sizeof f_job);
    // Ожидание завершения отрисовки пикселей в память кадра
    while ((GPU_JOB_CONTROL->JOB_IRQ_RAWSTAT & (1 << COMMAND_SLOT_FRAGMENT)) == 0) {
        // Опрос статуса
    }
    local_delay_ms(200);
    TP();
    GPU_JOB_CONTROL->JOB_INT_CLEAR = (1 << COMMAND_SLOT_FRAGMENT);

    PRINTF("Triangle rendering completed successfully!\n");
}

#define GPU_ALIGN __attribute__((aligned(64)))

// Минимальный вершинный шейдер (Vertex Shader) для Bifrost
// Задача: Пропустить координаты вершин (X, Y, Z, W) без изменений на стадию растеризации
RAMNC GPU_ALIGN static const uint32_t bifrost_vertex_shader_code[] = {
    0x7C003C00, 0x00000000, 0x00000000, 0x00000000, // Инструкция прохода позиции (Pass-through)
    0x00000000, 0x00000000, 0x00000000, 0x00000000  // Конец шейдера (Команда терминации потока)
};

// Минимальный фрагментный/пиксельный шейдер (Fragment Shader) для Bifrost
// Задача: Записать фиксированный цвет RGBA (0.0, 1.0, 0.0, 1.0) - Чистый зеленый
RAMNC GPU_ALIGN static const uint32_t bifrost_fragment_shader_code[] = {
    0x9C003C00, 0x0000FFFF, 0x00000000, 0x3F800000, // Запись зеленого компонента в квад-вектор
    0x00000000, 0x00000000, 0x00000000, 0x00000000  // Команда вывода в Render Target 0 и выход
};

// Структура описания шейдера для планировщика задач Bifrost
typedef struct __attribute__((packed)) {
    uint64_t shader_code_ptr;    // Физический адрес бинарного кода шейдера
    uint32_t properties;         // Флаги: количество регистров, тип шейдера
    uint16_t stack_size;         // Размер стека для потоков (0 для заглушки)
    uint16_t reserved;
    uint64_t preload_regs;       // Настройки предварительной загрузки регистров
    uint64_t uniform_buffer_ptr; // Адрес констант (Uniforms), если есть
} mali_shader_state;

// Контекст состояния отрисовки (Renderer State)
typedef struct __attribute__((packed)) {
    mali_shader_state vertex_shader;
    mali_shader_state fragment_shader;
    uint32_t blend_equation;     // Формула смешивания цветов (Alpha blending)
    uint32_t blend_constant;     // Константа прозрачности
    uint64_t attribute_meta_ptr; // Описание формата входных вершин (X, Y, Z)
} mali_renderer_state;

GPU_ALIGN static mali_renderer_state gpu_program_state;

typedef struct __attribute__((packed)) {
    uint64_t buffer_ptr;  // Физический адрес массива координат triangle_vertices
    uint32_t stride;      // Шаг между вершинами: sizeof(float) * 3 = 12 байт
    uint32_t size;        // Общий размер буфера вершин в байтах
    uint32_t format;      // Код формата Bifrost: 0x0400000A (3D Float вектор)
} mali_attribute_meta;

GPU_ALIGN static mali_attribute_meta gpu_vertex_input_meta;


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

void mali_g31_mmu_enable(uintptr_t table_phys_addr)
{
    unsigned as = 0; // Шейдерный домен по умолчанию

	// Индекс 0 = 0xAA (Cacheable), Индекс 1 = 0x22 (Non-Cacheable)
	GPU_MMU->MMU_AS[as].AS_MEMATTR_HI = 0x00000000;
	GPU_MMU->MMU_AS[as].AS_MEMATTR_LO = 0xff ;//0x000022AA;
	__DSB();

	table_phys_addr = 0;
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
	TP();
	printhex32((uintptr_t) & GPU_MMU->MMU_AS[as], & GPU_MMU->MMU_AS[as], sizeof GPU_MMU->MMU_AS[as]);
    PRINTF("Mali-G31: MMU Address Space 0 successfully enabled at offset 0x400!\n");
}

static void malimmu_initialize(void)
{
//    // 4. Проверяем! Теперь регистры MMU обязаны ожить
//    volatile uint32_t *mmu_int_rawstat = (volatile uint32_t *)0x01802000;
//    PRINTF("MMU Raw Interrupt Status: 0x%08X\n", (unsigned)*mmu_int_rawstat);
//    // (Если мост открылся, здесь вместо 0x00000000 перестанет падать мертвый ноль шины,
//    // и вы сможете писать в TRANSTAB и подавать команду UPDATE).
//    PRINTF("GPU_CONTROL->GPU_STATUS=%08X\n", (unsigned) GPU_CONTROL->GPU_STATUS);

//	gpu_as_command(0, AS_COMMAND_NOP);
//	gpu_as_command(0, AS_COMMAND_UPDATE);
//	gpu_as_command(0, AS_COMMAND_INVALIDATE);
//	gpu_as_command(0, AS_COMMAND_FLUSH_PT);
//	gpu_as_command(0, AS_COMMAND_FLUSH_MEM);

    // 1. Создаем таблицы страниц в ОЗУ
    gpu_mmu_build_4gb_l1_blocks();


	//mali_g31_mmu_enable((uintptr_t) gpu_mmu_l0_strict_table);
	mali_g31_mmu_enable((uintptr_t) hardware_get_g31_mmutable());

	//mali_bifrost_l2_ready();

    PRINTF("Mali-G31: MMU Coherency initialized successfully.\n");
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

	malimmu_initialize();

    // Тест чтения регистров возможностей
    //gpu_test();

#if 0
	unsigned i;

	memset32(GPU_MMU->MMU_AS, ~ UINT32_C(0), sizeof GPU_MMU->MMU_AS);
	for (i = 0; i < ARRAY_SIZE(GPU_MMU->MMU_AS); ++ i)
	{
		printhex32((uintptr_t) & GPU_MMU->MMU_AS [i], & GPU_MMU->MMU_AS [i], sizeof GPU_MMU->MMU_AS [i]);
	}
#endif
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

