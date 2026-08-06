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

#include "mali_bifrost_v6.h"

/**
  * @brief  Writes values to the Common Job Header memory block for any Mali GPU job.
  * @param  pHeader: Pointer to the MALI_JobHeader_TypeDef structure in RAM.
  * @param  job_type: Type of the job (e.g. MALI_JOB_TYPE_FRAGMENT or MALI_JOB_TYPE_WRITE_VALUE).
  * @param  use_barrier: Enable hardware execution barrier (blocking next slots until done).
  * @param  job_index: Unique software identifier for tracking completion.
  * @param  next_job_gpu_address: 64-bit physical GPU address of the next job descriptor (or 0 for tail).
  * @retval None
  */
static void MALI_JobHeader_WriteValue(MALI_JobHeader_TypeDef *pHeader,
                               uint8_t job_type,
                               uint32_t use_barrier,
                               uint32_t job_index,
                               uint64_t next_job_gpu_address)
{
  /* 1. Сброс аппаратных статусных полей и адресов ошибок (должны быть 0 при отправке) */
  pHeader->STATUS.EXCEPTION_STATUS      = UINT32_C(0x00000000);
  pHeader->STATUS.FIRST_INCOMPLETE_TASK = UINT32_C(0x00000000);
  pHeader->FAULT_ADDRESS                = UINT64_C(0x0000000000000000);

  /* 2. Побитовая сборка нижнего 32-битного слова конфигурации (CONFIG_FLAGS) */
  uint32_t config = UINT32_C(0);

  /* Записываем размер дескриптора */
  config |= ((uint32_t)MALI_JOB_DESC_SIZE_BIFROST << MALI_JOB_CTRL_DESC_SIZE_POS) & MALI_JOB_CTRL_DESC_SIZE_MASK;

  /* Записываем тип задания (например, 0x13 или 0x02 / 0x03) */
  config |= ((uint32_t)job_type << MALI_JOB_CTRL_TYPE_POS) & MALI_JOB_CTRL_TYPE_MASK;

  /* Если требуется аппаратный барьер — выставляем бит 16 */
  if (use_barrier != UINT32_C(0))
  {
    config |= MALI_JOB_CTRL_BARRIER_MASK;
  }

  pHeader->CFG.CONFIG_FLAGS = config;

  /* 3. Запись индекса задачи для внутреннего трекера */
  pHeader->CFG.JOB_INDEX = job_index;

  /* 4. Запись 64-битного указателя на следующее задание в цепочке команд */
  pHeader->NEXT_JOB = next_job_gpu_address;
}

/**
  * @brief  Writes values to the Write Value Job Payload memory block for Mali Bifrost v6.
  * @param  pPayload: Pointer to the MALI_WriteValueJobPayload_TypeDef structure in RAM.
  * @param  target_gpu_address: 64-bit GPU physical address where data must be written.
  * @param  value_to_write: 64-bit (or 32-bit) immediate data to store.
  * @retval None
  */
static void MALI_WriteValueJobPayload_WriteValue(MALI_WriteValueJobPayload_TypeDef *pPayload,
                                          uint64_t target_gpu_address,
                                          uint64_t value_to_write)
{
  /* 1. Запись целевого 64-битного адреса памяти */
  pPayload->ADDRESS = target_gpu_address;

  /* 2. Запись флага типа операции (Immediate write) */
  pPayload->CFG.VALUE_DESCRIPTOR = MALI_WRITE_VALUE_TYPE_IMMEDIATE_64;

  /* 3. Обязательное зануление аппаратного резерва */
  pPayload->CFG.RESERVED_WORD1   = UINT32_C(0x00000000);

  /* 4. Запись полезных данных */
  pPayload->IMMEDIATE = value_to_write;
}

/* Выравнивание по стандарту CMSIS/GCC для кэш-линий */
__attribute__((aligned(64))) static MALI_FragmentJobPayload_TypeDef FragmentJobPayload;

/**
  * @brief  Initializes the Mali Bifrost Fragment Job Payload for a Clear Screen operation.
  * @param  pPayload: Pointer to a MALI_FragmentJobPayload_TypeDef structure to be initialized.
  * @param  fbd_address: GPU physical address of the Framebuffer Descriptor (fbd_frag).
  * @param  tile_meta_address: GPU physical address of the Tiler Heap (gpu_fragment_tile_meta).
  * @param  width: Screen width in pixels (e.g., 800).
  * @param  height: Screen height in pixels (e.g., 480).
  * @retval None
  */
static void MALI_FragmentJobPayload_ClearInit(MALI_FragmentJobPayload_TypeDef *pPayload,
                                       uint64_t fbd_address,
                                       uint64_t tile_meta_address,
                                       uint32_t width,
                                       uint32_t height)
{
  /* 1. Настройка тегированного указателя на дескриптор кадрового буфера */
  pPayload->FB_DESC = fbd_address | MALI_FBD_TYPE_MFBD;

  /* 2. Привязка кучи выделения тайлов */
  pPayload->TILE_ALLOC = tile_meta_address;

  /* 3. Для операции Clear бэкенд фрагментов зануляется через 64-битную константу */
  pPayload->FRAGMENT_BACKEND = UINT64_C(0x0000000000000000);

  /* 4. Расчет и упаковка координат тайлов 16x16 */
  uint32_t max_tile_x = (width / 16) - 1;
  uint32_t max_tile_y = (height / 16) - 1;

  pPayload->COORDS.MIN_TILE_COORD = MALI_PACK_TILE_COORD(0, 0);
  pPayload->COORDS.MAX_TILE_COORD = MALI_PACK_TILE_COORD(max_tile_x, max_tile_y);
#if 0
  /* 5. Отключение потайлового отсечения */
  pPayload->SCISSORED_TILE_BITMAP = UINT64_C(0x0000000000000000);

  /* 6. Конфигурация планировщика задач фрагментного процессора */
  pPayload->CTRL.TILES_IN_FLIGHT = MALI_TILES_IN_FLIGHT_DEFAULT;
  pPayload->CTRL.CTRL_FLAGS      = 0x0000;
  pPayload->CTRL.SHADING_RATE     = 0x00000000;

  /* 7. Очистка полей выравнивания */
  pPayload->RESERVED0 = UINT64_C(0x0000000000000000);
  pPayload->RESERVED1 = UINT64_C(0x0000000000000000);
#endif
}

/**
  * @brief  Initializes the Mali Bifrost Framebuffer Descriptor for a Clear Screen operation.
  * @param  pFbd: Pointer to a MALI_FramebufferDescriptor_TypeDef structure to be initialized.
  * @param  tile_meta_address: GPU physical address of the Tiler Heap (gpu_fragment_tile_meta).
  * @param  rt_address: GPU physical address of the Render Target structure (render_target).
  * @param  width: Screen width in pixels (e.g., 800).
  * @param  height: Screen height in pixels (e.g., 480).
  * @retval None
  */
static void MALI_FramebufferDescriptor_ClearInit(MALI_FramebufferDescriptor_TypeDef *pFbd,
                                          uint64_t tile_meta_address,
                                          uint64_t rt_address,
                                          uint32_t width,
                                          uint32_t height)
{
  /* 1. Задаем физические размеры экрана за вычетом единицы */
  pFbd->WIDTH_MINUS_1  = width - UINT32_C(1);
  pFbd->HEIGHT_MINUS_1 = height - UINT32_C(1);

  /* 2. Настраиваем маску сэмплинга для стандартного 1x MSAA */
  pFbd->SAMPLE_MASK    = 1;//MALI_SAMPLE_MASK_1X;

  /* 3. Указываем количество Render Target (1 RT) */
  pFbd->RT_COUNT_AND_FLAGS = 0* 0x00040002 ;//MALI_RT_COUNT_1;

  /* 4. Прописываем 64-битный адрес кучи распределения памяти тайлов */
  pFbd->TILER_HEAP_START   = tile_meta_address;

  /* 5. Настраиваем тегированный указатель на первый дескриптор Render Target */
  pFbd->RENDER_TARGET_LIST = rt_address | 0*MALI_RT_TAG_MFBD;

  /* 6. Полностью зануляем вторую половину структуры (смещение с 32 байта) */
  /* Для bare-metal Clear-пассов здесь не должно быть флагов шейдеров и Z/Stencil */
  pFbd->FRAGMENT_FRAME_SHADER = UINT64_C(0x0000000000000000);
  pFbd->ZS_BLOCK              = UINT64_C(0x0000000000000000);
  pFbd->RESERVED_1            = UINT64_C(0x0000000000000000);
  pFbd->RESERVED_2            = UINT64_C(0x0000000000000000);
}
/**
  * @brief  Initializes the Mali Bifrost Render Target Descriptor for a Clear Screen operation.
  * @param  pRt: Pointer to a MALI_RenderTargetDescriptor_TypeDef structure to be initialized.
  * @param  fb_pointer: GPU physical address of the raw target framebuffer in RAM.
  * @param  width: Screen width in pixels (e.g., 800).
  * @param  r: Red color component (0.0f to 1.0f).
  * @param  g: Green color component (0.0f to 1.0f).
  * @param  b: Blue color component (0.0f to 1.0f).
  * @param  a: Alpha color component (0.0f to 1.0f).
  * @retval None
  */
static void MALI_RenderTargetDescriptor_ClearInit(MALI_RenderTargetDescriptor_TypeDef *pRt,
                                           uint64_t fb_pointer,
                                           uint32_t width,
                                           float r, float g, float b, float a)
{
  /* 1. Задаем формат пикселя и аппаратные флаги очистки фона */
  pRt->FORMAT_FLAGS = MALI_RT_FORMAT_RGBA8_UNORM;

  /* 2. Рассчитываем шаг строки (stride) в байтах с явным приведением типов */
  pRt->STRIDE = width * MALI_RT_BPP_RGBA8;

  /* 3. Прописываем чистый 64-битный указатель на область памяти вывода пикселей */
  pRt->FRAMEBUFFER_POINTER = fb_pointer;

  /* 4. Записываем цвет очистки экрана покомпонентно в формате FP32 */
//  pRt->CLEAR_COLOR.RGBA.R = r;
//  pRt->CLEAR_COLOR.RGBA.G = g;
//  pRt->CLEAR_COLOR.RGBA.B = b;
//  pRt->CLEAR_COLOR.RGBA.A = a;
}

/* Пример статической инициализации для вашего экрана 800x480 */
void MALI_FusedJobPayload_ClearInit(MALI_BifrostV6_FusedJobPayload_TypeDef *payload, uint64_t fbd_frag_phys_addr)
{
    payload->gl_enables = 0;
    payload->tiler_meta = 0;
    payload->min_tile_x = 0;
    payload->min_tile_y = 0;
    payload->max_tile_x = (DIM_X / 16) - 1;
    payload->max_tile_y = (DIM_Y / 16) - 1;

    // Привязываем FBD с флагом MFBD (0x1)
    payload->fb_desc = fbd_frag_phys_addr | 0*0x1;
}


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
static void gpu_diagnose_slot_fault(unsigned slot, unsigned as)
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

static int gpu_run_write_value_test(void) {
	PRINTF("gpu_run_write_value_test:\n");
	static volatile uint64_t __attribute__((aligned(64))) gpu_test_target [2];

	GPU_ALIGN static struct
	{
		MALI_JobHeader_TypeDef header;
		MALI_WriteValueJobPayload_TypeDef payload;
	} job, job2;

    memset((void *) gpu_test_target, 0xFF, sizeof gpu_test_target);

	MALI_JobHeader_WriteValue(& job2.header, MALI_JOB_TYPE_WRITE_VALUE, 1, 2, (uintptr_t) 0);
	MALI_WriteValueJobPayload_WriteValue(& job2.payload, (uintptr_t) & gpu_test_target [0], 0xDEADBEEFABBA1980);

	MALI_JobHeader_WriteValue(& job.header, MALI_JOB_TYPE_WRITE_VALUE, 0, 1, (uintptr_t) & job2);
	MALI_WriteValueJobPayload_WriteValue(& job.payload, (uintptr_t) & gpu_test_target [1], 0x0123456789ABCDEF);

	dcache_clean((uintptr_t) & job, sizeof job);
	dcache_clean((uintptr_t) & job2, sizeof job2);
	dcache_clean_invalidate((uintptr_t) & gpu_test_target, sizeof gpu_test_target);
//	PRINTF("job For test:\n");
//	printhex32(0, & job, sizeof job);
//	PRINTF("job2 For test:\n");
//	printhex32(0, & job2, sizeof job2);
	gpu_submit_job(2, (uintptr_t) & job);
	printhex64(0, (void *) & gpu_test_target, sizeof gpu_test_target);
	return 0;
}


static int gpu_run_write_value_test_old(void) {
	PRINTF("gpu_run_write_value_test_old:\n");

	// Переменная-цель, куда будет писать GPU.
	// Обязательно выравниваем по кэш-линии!
	static volatile uint64_t __attribute__((aligned(64))) gpu_test_target [2];
	unsigned v = MALI_WRITE_VALUE_TYPE_IMMEDIATE_64;
    // Создаем структуру дескриптора в памяти
	GPU_ALIGN static mali_write_value_job job;
	GPU_ALIGN static mali_write_value_job job2;

    // Сбрасываем старую память
    memset(&job, 0, sizeof(job));
    memset((void *) gpu_test_target, 0xFF, sizeof gpu_test_target);

    // Заполняем заголовок
    job.header.exception_status = 0;
    job.header.job_descriptor_size = 1; // Используем 64-битные указатели
    job.header.job_type = MALI_JOB_TYPE_WRITE_VALUE; // Тип задачи = 2
    job.header.job_barrier = 0;			// last
    job.header.job_index = 1;
    job.header.next_job = (uintptr_t) & job2;            // Цепочка заканчивается на ней

    job2.header.exception_status = 0;
    job2.header.job_descriptor_size = 1; // Используем 64-битные указатели
    job2.header.job_type = MALI_JOB_TYPE_WRITE_VALUE; // Тип задачи = 2
    job2.header.job_barrier = 1;			// last
    job2.header.job_index = 2;
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
    job2.immediate = 0x0123456789ABCDEF;               // Данные для записи

    // КРИТИЧЕСКИ ВАЖНО ДЛЯ BARE METAL:
    // Очищаем кэш CPU, чтобы GPU читал структуру из физического ОЗУ,
    // и инвалидируем регион gpu_test_target, чтобы CPU позже не прочитал старые данные из своего L1/L2.
    dcache_clean_invalidate((uintptr_t)&job, sizeof(job));
    dcache_clean_invalidate((uintptr_t)&job2, sizeof(job2));

    dcache_clean_invalidate((uintptr_t)&gpu_test_target, sizeof(gpu_test_target));

    // Важно: Адреса job и gpu_test_target должны быть предварительно
    // промаплены в MMU вашего Mali-G31 с правами Read/Write!
    //	PRINTF("job For test:\n");
    //	printhex32(0, & job, sizeof job);
    //	PRINTF("job2 For test:\n");
    //	printhex32(0, & job2, sizeof job2);

    if (gpu_submit_job(2, (uintptr_t) & job))
    	return 1;	// err
      // Проверяем результат выполнения
    printhex64(0, (void *) gpu_test_target, sizeof gpu_test_target);
    return 0;
}

#define GPU_ALIGN __attribute__((aligned(64)))

/**
 * [Биты 0-3]:   Swizzle (перестановка каналов). 0 = RGBA, 1 = BGRA/ARGB и т.д.
 * [Биты 4-11]:  Аппаратный формат (RGBA8888 = 0x14, RGB565 = 0x0C).
 * [Биты 12-31]: Управляющие флаги (тип памяти, линейная раскладка, write-enable).
 */
#define FORMAT_ARGB8888 0x15001000
#define FORMAT_RGBA8888 0x14001000
#define FORMAT_RGB565	0x0C001000

#define MALI_TILE_SHIFT 4
#define MALI_TILE_LENGTH (1 << MALI_TILE_SHIFT)

// https://android.googlesource.com/platform/external/mesa3d/+/e061bf004b5/src/panfrost/include/panfrost-job.h
#define MALI_MAKE_TILE_COORDS(X, Y) ((X) | ((Y) << 8))
#define MALI_BOUND_TO_TILE(B, bias) ((B - bias) >> MALI_TILE_SHIFT)
#define MALI_COORDINATE_TO_TILE(W, H, bias) MALI_MAKE_TILE_COORDS(MALI_BOUND_TO_TILE(W, bias), MALI_BOUND_TO_TILE(H, bias))
#define MALI_COORDINATE_TO_TILE_MIN(W, H) MALI_COORDINATE_TO_TILE(W, H, 0)
#define MALI_COORDINATE_TO_TILE_MAX(W, H) MALI_COORDINATE_TO_TILE(W, H, 1)

#define GPU_ALIGN_64  __attribute__((aligned(64)))
#define GPU_ALIGN_128 __attribute__((aligned(128)))
#define GPU_PACKED    __attribute__((packed))
/* Бинарный код пустого фрагментного шейдера Bifrost v6 (Mali-G31) */
//__attribute__((aligned(64))) static const uint32_t bifrost_dummy_fs[] = {
//    UINT32_C(0x00000000), UINT32_C(0x00000000), /* Системный префикс */
//    UINT32_C(0x7C003C00), UINT32_C(0x00000000), /* Команда останова и blend-прохода */
//};
//__attribute__((aligned(64))) static const uint32_t bifrost_dummy_fs[] = {
//    UINT32_C(0x00007800), UINT32_C(0x00000000), /* Первое 64-битное слово - зануленный заголовок */
//    UINT32_C(0x00007800), UINT32_C(0x00000000)  /* Второе 64-битное слово - безусловный END/NOP для v6 */
//};
__attribute__((aligned(64))) static const uint32_t bifrost_dummy_fs[] = {
    UINT32_C(0x00000000), UINT32_C(0x00000000), /* Системный заголовок / Клауза */
    UINT32_C(0x00000004), UINT32_C(0x00001000)  /* Аппаратная v6 команда Blend Writeout RT0 + END */
};

/** @addtogroup MALI_Bifrost_Renderer_Structures
  * @{
  */

/**
  * @brief  Mali Bifrost Blend Descriptor Structure definition
  * @note   Takes exactly 8 bytes per Render Target.
  */
typedef struct
{
  __IO uint32_t BLEND_FLAGS;          /*!< Blending equation and format configuration flags */
  __IO uint32_t BLEND_CONSTANT;       /*!< Fixed blend color constant (if used) */
} MALI_BlendDescriptor_TypeDef;

/**
  * @brief  Mali Bifrost Renderer State Descriptor Structure definition
  * @note   Combined shader metadata layout according to bifrost.xml specification.
  *         Must be 16-byte aligned in RAM. Total size with 1 RT = 24 bytes.
  */
typedef struct
{
  /* Word 0 (Bytes 0x00 - 0x07): <field name="Shader Code Ptr" start="0:0" end="0:63" type="address"/> */
  __IO uint64_t SHADER_CODE_PTR;      /*!< 64-bit GPU virtual/physical pointer to raw shader binary program */

  /* Word 1 (Bytes 0x08 - 0x0F): Аппаратные свойства и счетчики ресурсов */
  union {
    __IO uint64_t PROPERTIES;         /*!< Полный 64-битный регистр свойств для атомарной записи */
    struct {
      __IO uint16_t SAMPLER_COUNT;    /*!< Number of samplers used by shader (0 for clear) */
      __IO uint16_t TEXTURE_COUNT;    /*!< Number of textures bound (0 for clear) */
      __IO uint16_t ATTRIBUTE_COUNT;  /*!< Number of vertex attributes (0 for fragment shader) */
      __IO uint16_t VARYING_COUNT;    /*!< Number of varying interpolators (0 for clear) */
    };
  } PROP;

  /* Секция дескрипторов блендинга (Смещение 16 байт) */
  MALI_BlendDescriptor_TypeDef BLEND[UINT32_C(1)]; /*!< Blend descriptor for Render Target 0 */

} MALI_RendererState_TypeDef;

/**
  * @}
  */

/** @defgroup MALI_RendererState_Constants
  * @{
  */
/* Тег для указателя на бинарный код шейдера в Bifrost (обычно 0) */
#define MALI_SHADER_CODE_TAG               UINT64_C(0x0000000000000000)

/**
  * @brief  Аппаратные флаги блендинга для безусловной перезаписи (Replace mode).
  * @note   Маска 0x00011E00 кодирует в Bifrost v6 отключение альфа-смешивания
  *         и принудительный сброс сырых пикселей тайл-буфера в RAM (Raw write-out).
  */
#define MALI_BLEND_FLAGS_REPLACE_RAW8      UINT32_C(0x00011E00)
/**
  * @}
*/

/**
  * @brief  Initializes the Renderer State and Blend metadata for a Clear Screen operation.
  * @note   Configures the descriptor for a dummy fragment shader pass on Mali-G31.
  * @param  pState: Pointer to a MALI_RendererState_TypeDef structure in RAM to be initialized.
  * @param  shader_binary_gpu_address: Pure 64-bit physical/virtual GPU address of bifrost_dummy_fs.
  * @retval None
  */
void MALI_RendererState_ClearInit(MALI_RendererState_TypeDef *pState, uint64_t shader_binary_gpu_address)
{
  /* 1. Записываем 64-битный адрес кода шейдера с аппаратным тегом вызова (0x4) */
  pState->SHADER_CODE_PTR = shader_binary_gpu_address | 0*MALI_SHADER_CODE_TAG_BIFROST_V6;

  /* 2. Побитовая сборка 64-битного поля Properties через анонимное объединение */
  uint64_t props = UINT64_C(0);

  /* Для операции Clear Screen счётчики ресурсов шейдера равны нулю */
  props |= (UINT64_C(0) << MALI_RENDER_PROP_SAMPLER_COUNT_POS);
  props |= (UINT64_C(0) << MALI_RENDER_PROP_TEXTURE_COUNT_POS);
  props |= (UINT64_C(0) << MALI_RENDER_PROP_ATTRIBUTE_COUNT_POS);
  props |= (UINT64_C(0) << MALI_RENDER_PROP_VARYING_COUNT_POS);

  /* Принудительно взводим аппаратный бит аллокации потоков (Thread/Register Pre-allocation) */
  props |= MALI_RENDER_PROP_FRAG_THREAD_CONFIG; // Сгенерирует маску 0x0000200000000000

  /* Записываем собранную маску в 64-битное поле объединения */
  pState->PROP.PROPERTIES = props;

  /* 3. Настраиваем дескриптор блендинга для Render Target 0 (Подавление смешивания, чистый Replace) */
  pState->BLEND[UINT32_C(0)].BLEND_FLAGS    = MALI_BLEND_FLAGS_REPLACE_RAW32;
  pState->BLEND[UINT32_C(0)].BLEND_CONSTANT = UINT32_C(0x00000000);
}

/**
  * @brief  Writes values to the Tiler Heap descriptor for Mali-G31 (Bifrost v6).
  * @param  pHeap: Pointer to the MALI_TilerHeapDescriptor_TypeDef structure in RAM.
  * @param  heap_gpu_base_address: Physical 64-bit address where gpu_fragment_tile_meta is located.
  * @retval None
  */
void MALI_TilerHeapDescriptor_WriteValue(MALI_TilerHeapDescriptor_TypeDef *pHeap,
                                         uint64_t heap_gpu_base_address)
{
  /* 1. Создаем loopback-петлю: указываем аппаратной каретке свободных блоков
        на адрес конца нашей собственной 64-байтной структуры метаданных */
  pHeap->FREE_LIST_HEAD = heap_gpu_base_address + UINT64_C(64);

  /* 2. Задаем валидный размер кучи, отличный от нуля */
  pHeap->CFG.HEAP_SIZE  = MALI_TILER_HEAP_SIZE_64KB;
  pHeap->CFG.FLAGS      = UINT32_C(0x00000000);

  /* 3. Базовый адрес массива кучи указывает на начало нашей мета-области */
  pHeap->BASE_ADDRESS   = heap_gpu_base_address;

  /* 4. Безусловно зануляем весь аппаратный остаток 64-байтного дескриптора */
  pHeap->HARDWARE_PADDING[UINT32_C(0)] = UINT64_C(0x0000000000000000);
  pHeap->HARDWARE_PADDING[UINT32_C(1)] = UINT64_C(0x0000000000000000);
  pHeap->HARDWARE_PADDING[UINT32_C(2)] = UINT64_C(0x0000000000000000);
  pHeap->HARDWARE_PADDING[UINT32_C(3)] = UINT64_C(0x0000000000000000);
  pHeap->HARDWARE_PADDING[UINT32_C(4)] = UINT64_C(0x0000000000000000);
}


/* Выделение управляющих дескрипторов в системном ОЗУ с выравниванием по кэш-линиям */
__attribute__((aligned(64))) static MALI_FramebufferDescriptor_TypeDef    fbd_frag;
__attribute__((aligned(64))) static MALI_RenderTargetDescriptor_TypeDef   render_target;

/* Фиктивная область метаданных тайлов */
//__attribute__((aligned(4096))) static uint8_t gpu_fragment_tile_meta_buff[MALI_TILER_HEAP_SIZE_64KB];
//MALI_TilerHeapDescriptor_TypeDef gpu_fragment_heap;
/* Метаданные фрагментного шейдера */
__attribute__((aligned(16))) static MALI_RendererState_TypeDef            fragment_renderer_state;

static void gpu_clear_screen(uintptr_t framebuffer_phys_addr, uint32_t width, uint32_t height, uint32_t stride)
{
	/* Монолитная структура фрагментного задания для отправки в Job Slot 1 */
	typedef struct __attribute__((packed, aligned(64))) {
	    MALI_JobHeader_TypeDef           HEADER;   /*!< Общий аппаратный заголовок задания */
	    MALI_BifrostV6_FusedJobPayload_TypeDef  PAYLOAD;  /*!< Полезная нагрузка фрагментного процессора Bifrost v6 */
	} MALI_FragmentJob_TypeDef;

	__attribute__((aligned(64))) static GPU_ALIGN MALI_FragmentJob_TypeDef f_job_monolithic;
    // ... [Инициализация памяти __builtin_memset] ...

	//MALI_TilerHeapDescriptor_WriteValue(& gpu_fragment_heap, (uintptr_t) gpu_fragment_tile_meta_buff);
    // 1. ИНИЦИАЛИЗАЦИЯ МЕТАДАННЫХ ШЕЙДЕРА (Renderer State)
	MALI_RendererState_ClearInit(& fragment_renderer_state, (uint64_t)&bifrost_dummy_fs);

    // 2. ИНИЦИАЛИЗАЦИЯ RENDER TARGET
    MALI_RenderTargetDescriptor_ClearInit(& render_target, framebuffer_phys_addr, width, 1, 1, 1, 1);
    render_target.FRAMEBUFFER_POINTER = framebuffer_phys_addr;
    //render_target.CLEAR_COLOR.RGBA.R = 1.0f; // Пурпурный цвет
    // ... [Настройка формата и STRIDE] ...
    render_target.FRAMEBUFFER_POINTER = framebuffer_phys_addr;
    render_target.FORMAT_FLAGS = FORMAT_ARGB8888;
    render_target.STRIDE = stride;

    // 3. ИНИЦИАЛИЗАЦИЯ FRAMEBUFFER DESCRIPTOR (MFBD)
	MALI_FramebufferDescriptor_ClearInit(& fbd_frag,
			0,//(uintptr_t) & gpu_fragment_heap,
			(uintptr_t) & render_target, width, height);
    fbd_frag.FRAGMENT_FRAME_SHADER = (uint64_t)&fragment_renderer_state;
    // ... [Настройка размеров, TILER_HEAP_START и т.д.] ...

    // 4. СБОРКА МОНОЛИТНОГО FRAGMENT JOB (Заголовок + Полезная нагрузка)
    MALI_JobHeader_WriteValue(&f_job_monolithic.HEADER, MALI_JOB_TYPE_FUSED, 1, 1, 0);
//    MALI_FragmentJobPayload_ClearInit(& f_job_monolithic.PAYLOAD,
//    		(uint64_t)&fbd_frag,
//			0,//(uintptr_t) & gpu_fragment_heap,
//			width, height);
    MALI_FusedJobPayload_ClearInit(& f_job_monolithic.PAYLOAD,
    		(uint64_t)&fbd_frag);
    //f_job_monolithic.PAYLOAD.FB_DESC = (uint64_t)&fbd_frag | MALI_FBD_TYPE_MFBD;
    // ... [Расчет тайлов и заполнение остальных полей] ...

    // 5. ВЫТАЛКИВАНИЕ ДАННЫХ ИЗ КЭША В ОЗУ
    // ... [dcache_clean для всех дескрипторов] ...
    dcache_clean_invalidate((uintptr_t)&f_job_monolithic, sizeof f_job_monolithic );

    dcache_clean_invalidate((uintptr_t)&fbd_frag, sizeof fbd_frag );
    dcache_clean_invalidate((uintptr_t)&render_target, sizeof render_target );
//    dcache_clean_invalidate((uintptr_t)&gpu_fragment_tile_meta_buff, sizeof gpu_fragment_tile_meta_buff );
//    dcache_clean_invalidate((uintptr_t)&gpu_fragment_heap, sizeof gpu_fragment_heap );
    dcache_clean_invalidate((uintptr_t)&fragment_renderer_state, sizeof fragment_renderer_state );

    PRINTHEX32(bifrost_dummy_fs);
    PRINTHEX32(f_job_monolithic);
    PRINTHEX32(fbd_frag);
    PRINTHEX32(render_target);
    //PRINTHEX32(gpu_fragment_heap);
    PRINTHEX32(fragment_renderer_state);

    __DSB(); __ISB();

    gpu_submit_job(0, (uintptr_t)&f_job_monolithic);
}


static void gpu_clear_screen_on_fragment(uintptr_t framebuffer_phys_addr, uint32_t width, uint32_t height, uint32_t stride)
{
	/* Монолитная структура фрагментного задания для отправки в Job Slot 1 */
	typedef struct __attribute__((packed, aligned(64))) {
	    MALI_JobHeader_TypeDef           HEADER;   /*!< Общий аппаратный заголовок задания */
	    MALI_FragmentJobPayload_TypeDef  PAYLOAD;  /*!< Полезная нагрузка фрагментного процессора Bifrost v6 */
	} MALI_FragmentJob_TypeDef;

	__attribute__((aligned(64))) static GPU_ALIGN MALI_FragmentJob_TypeDef f_job_monolithic;
    // ... [Инициализация памяти __builtin_memset] ...

	//MALI_TilerHeapDescriptor_WriteValue(& gpu_fragment_heap, (uintptr_t) gpu_fragment_tile_meta_buff);
    // 1. ИНИЦИАЛИЗАЦИЯ МЕТАДАННЫХ ШЕЙДЕРА (Renderer State)
	MALI_RendererState_ClearInit(& fragment_renderer_state, (uint64_t)&bifrost_dummy_fs);

    // 2. ИНИЦИАЛИЗАЦИЯ RENDER TARGET
    MALI_RenderTargetDescriptor_ClearInit(& render_target, framebuffer_phys_addr, width, 1, 1, 1, 1);
    render_target.FRAMEBUFFER_POINTER = framebuffer_phys_addr;
    //render_target.CLEAR_COLOR.RGBA.R = 1.0f; // Пурпурный цвет
    // ... [Настройка формата и STRIDE] ...
    render_target.FRAMEBUFFER_POINTER = framebuffer_phys_addr;
    render_target.FORMAT_FLAGS = FORMAT_ARGB8888;
    render_target.STRIDE = stride;

    // 3. ИНИЦИАЛИЗАЦИЯ FRAMEBUFFER DESCRIPTOR (MFBD)
	MALI_FramebufferDescriptor_ClearInit(& fbd_frag,
			0,//(uintptr_t) & gpu_fragment_heap,
			(uintptr_t) & render_target, width, height);
    fbd_frag.RENDER_TARGET_LIST = (uint64_t)&render_target | MALI_RT_TAG_MFBD;
    fbd_frag.FRAGMENT_FRAME_SHADER = (uint64_t)&fragment_renderer_state;
    // ... [Настройка размеров, TILER_HEAP_START и т.д.] ...

    // 4. СБОРКА МОНОЛИТНОГО FRAGMENT JOB (Заголовок + Полезная нагрузка)
    MALI_JobHeader_WriteValue(&f_job_monolithic.HEADER, MALI_JOB_TYPE_FRAGMENT, 1, 1, 0);
    MALI_FragmentJobPayload_ClearInit(& f_job_monolithic.PAYLOAD,
    		(uint64_t)&fbd_frag,
			0,//(uintptr_t) & gpu_fragment_heap,
			width, height);
    //f_job_monolithic.PAYLOAD.FB_DESC = (uint64_t)&fbd_frag | MALI_FBD_TYPE_MFBD;
    // ... [Расчет тайлов и заполнение остальных полей] ...

    // 5. ВЫТАЛКИВАНИЕ ДАННЫХ ИЗ КЭША В ОЗУ
    // ... [dcache_clean для всех дескрипторов] ...
    dcache_clean_invalidate((uintptr_t)&f_job_monolithic, sizeof f_job_monolithic );

    dcache_clean_invalidate((uintptr_t)&fbd_frag, sizeof fbd_frag );
    dcache_clean_invalidate((uintptr_t)&render_target, sizeof render_target );
//    dcache_clean_invalidate((uintptr_t)&gpu_fragment_tile_meta_buff, sizeof gpu_fragment_tile_meta_buff );
//    dcache_clean_invalidate((uintptr_t)&gpu_fragment_heap, sizeof gpu_fragment_heap );
    dcache_clean_invalidate((uintptr_t)&fragment_renderer_state, sizeof fragment_renderer_state );

    PRINTHEX32(bifrost_dummy_fs);
    PRINTHEX32(f_job_monolithic);
    PRINTHEX32(fbd_frag);
    PRINTHEX32(render_target);
    //PRINTHEX32(gpu_fragment_heap);
    PRINTHEX32(fragment_renderer_state);

    __DSB(); __ISB();

    // Запуск в аппаратный слот фрагментов (Слот 1)
    gpu_submit_job(1, (uintptr_t)&f_job_monolithic);
}


//----------------------

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

static void mali_bifrost_power_on(void)
{
    volatile uint32_t *gpu_pwr_key   = (volatile uint32_t *)(GPU_CONTROL_BASE + GPU_PWR_KEY);
    volatile uint32_t *gpu_pwr_ovr1  = (volatile uint32_t *)(GPU_CONTROL_BASE + GPU_PWR_OVERRIDE1);

    volatile uint32_t *l2_pwron      = (volatile uint32_t *)(GPU_CONTROL_BASE + REG_L2_PWRON);
    volatile uint32_t *l2_ready      = (volatile uint32_t *)(GPU_CONTROL_BASE + REG_L2_READY);

    volatile uint32_t *tiler_pwron   = (volatile uint32_t *)(GPU_CONTROL_BASE + REG_TILER_PWRON);
    volatile uint32_t *tiler_ready   = (volatile uint32_t *)(GPU_CONTROL_BASE + REG_TILER_READY);

    volatile uint32_t *shader_pwron  = (volatile uint32_t *)(GPU_CONTROL_BASE + REG_SHADER_PWRON);
    volatile uint32_t *shader_ready  = (volatile uint32_t *)(GPU_CONTROL_BASE + REG_SHADER_READY);

//    PRINTF("Mali-G31: Initializing power-up via Bifrost v6 Register Map...\n");

    // 1. Снимаем программную защиту с контроллера питания
    *gpu_pwr_key = GPU_PWR_KEY_UNLOCK;
    __DSB();

    // Фиксация стабильности шин питания
    *gpu_pwr_ovr1 = 0xFFF | (UINT32_C(0x20) << 16);
    __DSB();

    // 2. Включаем L2 Кэш (Бит 0 = Включить домен 0)
    *l2_pwron = 0x00000001;
    __DSB();

    //TP();
   // Ожидаем готовность L2 на смещении 0x160
    while ((*l2_ready & UINT32_C(1)) == 0) {
        // Опрос готовности L2-интерфейса
    }

    // 3. Включаем блок геометрии (Tiler) через смещение 0x190
    *tiler_pwron = UINT32_C(1);
    __DSB();

    //TP();
   // Ожидаем готовность тайлера на вашем смещении 0x150
    while ((*tiler_ready & UINT32_C(1)) == 0) {
        // Если зависает здесь, значит на GPU не подана частота от CCU Allwinner
    }
    //TP();
#define REG_STACK_PWRON   0xE20
#define REG_STACK_READY   0xE10

	// Добавьте этот кусок в mali_bifrost_power_on() СТРОГО ПЕРЕД включением шейдеров:
	volatile uint32_t *stack_pwron = (volatile uint32_t *)(GPU_CONTROL_BASE + REG_STACK_PWRON);
	volatile uint32_t *stack_ready = (volatile uint32_t *)(GPU_CONTROL_BASE + REG_STACK_READY);

//	PRINTF("Mali-G31: Powering up Shader Core Stack (0x1D0)...\n");
	*stack_pwron =  UINT32_C(1); // Включаем базовый стек
	__DSB();
	local_delay_ms(100);
//	PRINTF("*stack_ready=%08X\n", (unsigned) *stack_ready);
//	PRINTF("*tiler_ready=%08X\n", (unsigned) *tiler_ready);

//	while ((*stack_ready & 0x00000001) == 0) {
//		// Ожидание готовности стека ядер на смещении 0xE10
////		PRINTF("*stack_ready=%08X\n", (unsigned) *stack_ready);
////		PRINTF("*tiler_ready=%08X\n", (unsigned) *tiler_ready);
//	}
	local_delay_ms(100);

	// 4. Включаем 2 вычислительных ядра (Shader Cores) для MP2 (Маска 0x03) через 0x180
    *shader_pwron =  UINT32_C(0x03);
    __DSB();

	local_delay_ms(100);
//	PRINTF("*shader_ready=%08X\n", (unsigned) *shader_ready);
   //TP();
   // Ожидаем готовность ядер на смещении 0x140
    while ((*shader_ready & 0x00000001) != 0x00000001) {	// was: 0x00000003
        // Ожидание готовности обоих шейдерных ядер
    	   // ТОЛЬКО ОДНО ЯДРО
    }
//    PRINTF("Mali-G31: Success! L2, Tiler (0x150), and Shaders (0x140) are READY.\n");
}


#define REG_L2_PWR_DOMAIN_COMMAND      0x0010
#define REG_L2_PWR_DOMAIN_STATUS       0x0014

static void mali_bifrost_l2_ready(void)
{
    volatile uint32_t *l2_pwr_cmd  = (volatile uint32_t *)(GPU_CONTROL_BASE + REG_L2_PWR_DOMAIN_COMMAND);
    volatile uint32_t *l2_pwr_stat = (volatile uint32_t *)(GPU_CONTROL_BASE + REG_L2_PWR_DOMAIN_STATUS);

    // Принудительно включаем и запитываем L2 кэш GPU
    *l2_pwr_cmd = UINT32_C(0xFFFFFFFF);
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

#if 0
	gpu_run_write_value_test_old();
	gpu_run_write_value_test();
	gpu_run_write_value_test_old();
	gpu_run_write_value_test();
	gpu_run_write_value_test_old();
	gpu_run_write_value_test();
	gpu_run_write_value_test_old();
	gpu_run_write_value_test();
	return;
//	unsigned v = 0;
//	while (run_write_value_test(v ++))
//		;
//    TP();
//    for (;;)
//    	;
#endif
	{
		PRINTF("gpu_clear_screen test:\n");
		ASSERT(LCDMODE_PIXELSIZE == 4);
		uintptr_t fbaddr = (uintptr_t) colmain_fb_draw();
	    memset32((void *) fbaddr, COLORPIP_DARKCYAN, DIM_X * DIM_Y * LCDMODE_PIXELSIZE);
	    gpu_clear_screen(fbaddr, DIM_X, DIM_Y, DIM_X * LCDMODE_PIXELSIZE);
	    colmain_nextfb();

	    TP();
	    for (;;)
	    	;
	}

}
#define GPU_L2_MMU_CONFIG  0x0008 // Смещение внутри блока GPU_CONTROL (0x01800008)

static void mali_bifrost_open_mmu_bus(void)
{
    volatile uint32_t *l2_mmu_config = (volatile uint32_t *)(GPU_BASE + GPU_L2_MMU_CONFIG);

    // Читаем текущие особенности чипа
    uint32_t val = *l2_mmu_config;

    // Для Mali-G31 (Bifrost r0p3) необходимо принудительно выставить биты 0 и 1,
    // чтобы открыть внутренний интерконнект от процессоров к MMU и снять изоляцию.
    *l2_mmu_config = val | 0x00000003;

    __DSB();
}

static void mali_g31_mmu_enable(void)
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

//    gpu_as_command(as, 0x01);	// Отправляем команду UPDATE для применения таблиц
//    gpu_as_command(as, 0x03);	// Очищаем внутренний TLB кэш MMU от старых зависших ошибок 0xC8 AS_COMMAND_INVALIDATE

	gpu_as_command(as, AS_COMMAND_UPDATE);
	gpu_as_command(as, AS_COMMAND_INVALIDATE);
//	gpu_as_command(as, AS_COMMAND_FLUSH_PT);

//	printhex32((uintptr_t) & GPU_MMU->MMU_AS[as], & GPU_MMU->MMU_AS[as], sizeof GPU_MMU->MMU_AS[as]);
//    PRINTF("Mali-G31: MMU Address Space 0 successfully enabled at offset 0x400!\n");
}

//
#define T507_SPC_BASE         SPC_BASE

// Регистры конфигурации защиты периферии (Secure Peripherals Control)
#define SPC_GPU_MAST_REG      (T507_SPC_BASE + 0x00A0) // Управление правами GPU как Master шины
#define SPC_GPU_SLAV_REG      (T507_SPC_BASE + 0x00A4) // Управление правами доступа CPU к регистрам GPU

// бред от AI
static void t507_spc_unlock_gpu(void)
{
    volatile uint32_t *spc_gpu_master = (volatile uint32_t *)SPC_GPU_MAST_REG;
    volatile uint32_t *spc_gpu_slave  = (volatile uint32_t *)SPC_GPU_SLAV_REG;

    PRINTF("T507 Platform: Unlocking GPU Secure Peripherals Controller (SPC)...\n");
    printhex32_titled(SPC_BASE, SPC, 1024, "SPC");

    // Запись маски 0xFFFFFFFF или 0x00000003 (в зависимости от разводки доменов)
    // разрешает Non-Secure транзакции для графического процессора на системной интерконнект-шине.
    // По спецификации Allwinner, запись всех единиц переводит устройство в полностью открытый Non-Secure режим.
    *spc_gpu_master = 0xFFFFFFFF;
    *spc_gpu_slave  = 0xFFFFFFFF;

    __DSB(); // Принудительно толкаем барьер в контроллер SPC

    PRINTF("T507 Platform: GPU registers bypass TrustZone protection now.\n");
    printhex32_titled(SPC_BASE, SPC, 1024, "SPC 2");
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

	//t507_spc_unlock_gpu();

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

//    PRINTF("GPU Reset released. Unlocking internal buses...\n");

    // 2. СНИМАЕМ ИЗОЛЯЦИЮ ШИНЫ ЗАДАЧ (Решение причины №1)
//	gpu_command(GPU_COMMAND_CYCLE_COUNT_START);
//	local_delay_ms(100);

	// https://elixir.bootlin.com/linux/latest/source/drivers/gpu/drm/panfrost/panfrost_mmu.c

	mali_bifrost_power_on();	// бязательно нужно
	mali_bifrost_open_mmu_bus();	// не требуется
	mali_bifrost_l2_ready();	// не требуется

	mali_g31_mmu_enable();

	PRINTF("board_gpu_initialize done.\n");
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

