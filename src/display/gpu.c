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

#if WITHGPUHW && CPUSTYLE_T507
//#define __OPENCL_VERSION__
//#include "panfrost_regs.h"

//#include "mali_bifrost_v6.h"
#include "pan_pack_helpers.h"
#include "bifrost_v7.h"

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
	if (local_wait32mask(& GPU_MMU->MMU_AS[as].AS_STATUS, 0x1, 0 * 0x1, 100))
		return;	// timeout

    GPU_MMU->MMU_AS[as].AS_COMMAND = cmd;
    // Ожидание завершения команды
	if (local_wait32mask(& GPU_MMU->MMU_AS[as].AS_STATUS, 0x1, 0 * 0x1, 100))
		return;	// timeout
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
	if (! local_wait32mask(& GPU_CONTROL->GPU_IRQ_RAWSTAT, mask, mask, 100))
		GPU_CONTROL->GPU_IRQ_CLEAR = GPU_IRQ_REG_ALL;
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
    GPU_JOB_CONTROL->JOB_INT_CLEAR = 0xFFFFFFFF;
    GPU_JOB_CONTROL->JOB_IRQ_CLEAR = 0xFFFFFFFF;
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
    PRINTF("   MMU Fault Status (as=%u) = 0x%08X\n", as, (unsigned)GPU_MMU->MMU_AS [as].AS_FAULTSTATUS);
    PRINTF("   MMU Fault Address = 0x%08X%08X\n",
           (unsigned)GPU_MMU->MMU_AS [as].AS_FAULTADDRESS_HI, (unsigned)GPU_MMU->MMU_AS [as].AS_FAULTADDRESS_LO);
}

static int gpu_submit_job(unsigned slot, uintptr_t job)
{
	unsigned as = 0;
	//PRINTF("gpu_submit_job: head=%p, slot=%u\n", (void *) head, slot);
//	PRINTF("Job before run:\n");
//	printhex32((uintptr_t) job, (void *) job, 512);
    // Записываем физический адрес начала структуры v_job в регистр указателя слота 0
    GPU_JOB_CONTROL->LOOP[slot].JS_HEAD_NEXT_HI = ptr_hi32(job);//(uint32_t)(((uintptr_t)&v_job >> 32) & 0xFFFFFFFF);
    GPU_JOB_CONTROL->LOOP[slot].JS_HEAD_NEXT_LO = ptr_lo32(job);//(uint32_t)((uintptr_t)&v_job & 0xFFFFFFFF);

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
    if (local_wait32mask2(& GPU_JOB_CONTROL->JOB_INT_RAWSTAT, (UINT32_C(0x10001) << slot), 1 * (UINT32_C(1) << slot), 1 * (UINT32_C(0x10000) << slot), 100))//Was: JOB_IRQ_RAWSTAT
    {
    	// timeout
    	PRINTF("error job @%p:\n", (void *) job);
    	dcache_invalidate((uintptr_t) job, 128);
    	printhex32(job, (void *) job, 128);
    	PRINTF("gpu timeout: GPU_JOB_CONTROL->JOB_INT_RAWSTAT=%08X\n", (unsigned) GPU_JOB_CONTROL->JOB_INT_RAWSTAT);
    	gpu_diagnose_slot_fault(slot, as);
        GPU_JOB_CONTROL->JOB_INT_CLEAR = 0xFFFFFFFF;
        return 1;
    }
    else
    {
//    	PRINTF("Okay job @%p:\n", (void *) job);
//    	dcache_invalidate((uintptr_t) job, 128);
//    	printhex32(job, (void *) job, 128);
        GPU_JOB_CONTROL->JOB_INT_CLEAR = (UINT32_C(1) << slot); // Сброс флага прерывания
        GPU_JOB_CONTROL->JOB_INT_CLEAR = 0xFFFFFFFF;
        return 0;
    }
}


//==============

//#include <stdint.h>
//#include <stddef.h>
//#include <stdbool.h>
//#include <string.h>
//#include "bifrost_v7.h"


/**
 * @brief Заполняет прямоугольник в видеобуфере ARGB8888 однотонным цветом
 *        средствами аппаратного Clear в MALI_JOB_TYPE_FRAGMENT (Bifrost v7).
 *
 * @param fb_addr    Указатель на начало видеобуфера (физический адрес для GPU).
 * @param fb_width   Полная ширина видеобуфера в пикселях.
 * @param fb_height  Полная высота видеобуфера в пикселях.
 * @param fb_stride  Шаг строки видеобуфера в БАЙТАХ (должен быть кратен 64 байтам).
 * @param x          Координата X левого верхнего угла прямоугольника.
 * @param y          Координата Y левого верхнего угла прямоугольника.
 * @param width      Ширина прямоугольника в пикселях.
 * @param height     Высота прямоугольника в пикселях.
 * @param color_argb Цвет в формате ARGB8888 (0xAARRGGBB).
 * @return int       0 при успехе, ненулевой код при ошибке.
 */
static int gpu_fill_rect_fragment_v7(
    uint32_t *fb_addr,
    uint32_t fb_width,
    uint32_t fb_height,
    uint32_t fb_stride,
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height,
	PACKEDCOLORPIP_T color_argb
	)
{
	/**
	 * Multi-Target Framebuffer Descriptor (MFBD) для Bifrost v7:
	 * 1. MALI_FRAMEBUFFER_PACKED_T (128 байт: PARAMETERS + PADDING)
	 * 2. Массив MALI_RENDER_TARGET_PACKED_T (по 64 байта на каждый RT)
	 */
	typedef struct {
	    MALI_FRAMEBUFFER_PACKED_T fbd;
	    MALI_RGB_RENDER_TARGET_PACKED_T rt0;
	} GPU_ALIGN bifrost_v7_mfbd_t;

	static GPU_ALIGN bifrost_v7_mfbd_t fb_desc;
	static GPU_ALIGN MALI_FRAGMENT_JOB_PACKED_T frag_job;

	if (!fb_addr || width == 0 || height == 0) {
        return 0;
    }

    if (x >= fb_width || y >= fb_height) {
        return 0;
    }

    uint32_t max_x = (x + width > fb_width) ? (fb_width - 1) : (x + width - 1);
    uint32_t max_y = (y + height > fb_height) ? (fb_height - 1) : (y + height - 1);

    memset(&fb_desc, 0, sizeof(fb_desc));
    memset(&frag_job, 0, sizeof(frag_job));

    // =========================================================================
    // 1. Упаковка Framebuffer Descriptor (PARAMETERS + PADDING)
    // =========================================================================
    struct MALI_FRAMEBUFFER_PARAMETERS fbd_params = {
        MALI_FRAMEBUFFER_PARAMETERS_header,
        .pre_frame_0 = MALI_PRE_POST_FRAME_SHADER_MODE_NEVER,
        .pre_frame_1 = MALI_PRE_POST_FRAME_SHADER_MODE_NEVER,
        .post_frame  = MALI_PRE_POST_FRAME_SHADER_MODE_NEVER,
        .width = fb_width,
        .height = fb_height,
        .bound_min_x = x,
        .bound_min_y = y,
        .bound_max_x = max_x,
        .bound_max_y = max_y,
        .sample_count = 1,
        .sample_pattern = MALI_SAMPLE_PATTERN_SINGLE_SAMPLED,
        .tie_break_rule = MALI_TIE_BREAK_RULE_0_IN_180_OUT,
        .effective_tile_size = 16,
        .render_target_count = 1,
        .color_buffer_allocation = 1024, // 16 * 16 * 4 байта
        .has_zs_crc_extension = false
    };
    MALI_FRAMEBUFFER_PARAMETERS_pack(&fb_desc.fbd.PARAMETERS, &fbd_params);

    static GPU_ALIGN struct MALI_FRAMEBUFFER_PADDING fbd_pad = { MALI_FRAMEBUFFER_PADDING_header };
    MALI_FRAMEBUFFER_PADDING_pack(&fb_desc.fbd.PADDING, &fbd_pad);

    // =========================================================================
    // 2. Упаковка Render Target 0 (MALI_RGB_RENDER_TARGET -> rt0)
    // =========================================================================
    uint32_t a = (color_argb >> 24) & 0xFFU;
    uint32_t r = (color_argb >> 16) & 0xFFU;
    uint32_t g = (color_argb >> 8)  & 0xFFU;
    uint32_t b = (color_argb >> 0)  & 0xFFU;

    // Swizzle из внутреннего R8G8B8A8 в ARGB8888 (в Little-Endian памяти: байты B, G, R, A)
    // swizzle_bgra = 0000060A

    // bit 0..2 - byte 0
    // bit 3..5 - byte 1
    // bit 6..8 - byte 2
    // bit 9..1 - byte 3
    uint32_t swizzle_bgra = ((uint32_t)MALI_CHANNEL_B << 0) |
                            ((uint32_t)MALI_CHANNEL_G << 3) |
                            ((uint32_t)MALI_CHANNEL_R << 6) |
                            ((uint32_t)MALI_CHANNEL_A << 9);
    swizzle_bgra = 0x00000688;

    //PRINTHEX32(swizzle_bgra);
    struct MALI_RGB_RENDER_TARGET rt_rgb = {
        MALI_RGB_RENDER_TARGET_header,
        .internal_buffer_offset = 0,
        .yuv_enable = false,
        .dithered_clear = false,
        .internal_format = MALI_COLOR_BUFFER_INTERNAL_FORMAT_R8G8B8A8,
        .write_enable = true,
        .writeback_format = MALI_COLOR_FORMAT_R8G8B8A8,
        .writeback_block_format = MALI_BLOCK_FORMAT_LINEAR,
        .writeback_msaa = MALI_MSAA_SINGLE,
        .srgb = false,
        .dithering_enable = false,
        .swizzle = swizzle_bgra,
        .clean_tile_write_enable = true,
        .writeback_buffer = {
            .base = (uintptr_t)fb_addr,
            .row_stride = fb_stride,
            .surface_stride = (uint64_t)fb_stride * fb_height
        },
        .clear = {
            .color_0 = color_argb,//r * 0x01010101U,
            .color_1 = 0,//g * 0x01010101U,
            .color_2 = 0,//b * 0x01010101U,
            .color_3 = color_argb //a * 0x01010101U
        }
    };
    MALI_RGB_RENDER_TARGET_pack(&fb_desc.rt0, &rt_rgb);
    //PRINTHEX32(fb_desc.rt0);


    // =========================================================================
    // 4. Упаковка Fragment Job (HEADER и PAYLOAD)
    // =========================================================================
    struct MALI_JOB_HEADER jh = {
        MALI_JOB_HEADER_header,
        .type = MALI_JOB_TYPE_FRAGMENT,
        .exception_status = 0,
        .barrier = true,
        .index = 1,
        .next = 0
    };
    MALI_JOB_HEADER_pack(&frag_job.HEADER, &jh);

    struct MALI_FRAGMENT_JOB_PAYLOAD fjp = {
        MALI_FRAGMENT_JOB_PAYLOAD_header,
        .bound_min_x = x >> 4,
        .bound_min_y = y >> 4,
        .bound_max_x = max_x >> 4,
        .bound_max_y = max_y >> 4,
        .has_tile_enable_map = false,
        .framebuffer = (uint64_t)&fb_desc//(uint64_t)& fb_ptr_packed | 1
    };
    MALI_FRAGMENT_JOB_PAYLOAD_pack(&frag_job.PAYLOAD, &fjp);

    // =========================================================================
    // 5. Синхронизация D-Cache и отправка задачи в Job Slot 0
    // =========================================================================
    dcache_clean_invalidate((uintptr_t)&fb_desc, sizeof(fb_desc));
    dcache_clean_invalidate((uintptr_t)&frag_job, sizeof(frag_job));

    uintptr_t dirty_start = (uintptr_t)fb_addr + (uintptr_t)y * fb_stride;
    size_t dirty_size = (size_t)(max_y - y + 1) * fb_stride;
    dcache_clean_invalidate(dirty_start, dirty_size);

//    PRINTHEX32(fb_desc);
//    PRINTHEX32(frag_job);
//    PRINTHEX32(fb_ptr_packed);
//	PRINTF("Job before run:\n");
//	printhex32((uintptr_t)&frag_job, &frag_job, 512);
   if (gpu_submit_job(0, (uintptr_t)&frag_job)) {
        return -1;
    }

    dcache_clean_invalidate(dirty_start, dirty_size);
    return 0;
}

static LCLSPINLOCK_t gpulock;

// dcache_clean_invalidate уже выполнено
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
	IRQL_t irql;
	IRQLSPIN_LOCK(& gpulock, & irql, BRDSYS_IRQL);

    // Заливка прямоугольника 200x200 в позиции (x = 100, y = 100)
    int res = gpu_fill_rect_fragment_v7(
    	buffer, // Указатель на начало видеобуфера (физический адрес)
        dx,           // 800
        DIM_Y,          // 480
		tstride,          // 3200 байт
        0,                // x
        0,                // y
		w,                // width
		h,                // height
		color           // 0xFFFF0000
    );
    ASSERT(res == 0);
	IRQLSPIN_UNLOCK(& gpulock, irql);
}

void draw_example(void) {
    // Цвет в формате ARGB8888: 0xAARRGGBB
    // Например, непрозрачный красный цвет: A=0xFF, R=0xFF, G=0x00, B=0x00
	COLORPIP_T color_red = TFTRGB(255, 0, 0);
	PRINTF("color_red=0x%08X\n", (unsigned) color_red);
	uintptr_t fbaddr = (uintptr_t) colmain_fb_draw();
    memset32((void *) fbaddr, COLORPIP_DARKCYAN, DIM_X * DIM_Y * LCDMODE_PIXELSIZE);

    dcache_clean_invalidate(fbaddr, DIM_Y * GXADJ(DIM_X) * LCDMODE_PIXELSIZE);
    // Заливка прямоугольника 200x200 в позиции (x = 100, y = 100)
    int res = gpu_fill_rect_fragment_v7(
        (void *) fbaddr, // Указатель на начало видеобуфера (физический адрес)
        DIM_X,           // 800
        DIM_Y,          // 480
        GXADJ(DIM_X) * LCDMODE_PIXELSIZE,          // 3200 байт
        5,                // x
        0,                // y
		202,                // width
		203,                // height
        color_red           // 0xFFFF0000
    );
	PRINTF("framebuffer:\n");
    printhex32(0, (void *) fbaddr, 256);
    if (res != 0) {
        PRINTF("gpu_fill_rect_fragment_v7 failed: %d\n", res);
    }
    colmain_nextfb();
    for (;;)
    	;
}

//==============

static int gpu_run_write_value_test_mesa(void) {
	PRINTF("gpu_run_write_value_test_mesa:\n");
	// Переменная-цель, куда будет писать GPU.
	GPU_ALIGN static volatile uint64_t gpu_test_target [2];

	GPU_ALIGN static MALI_WRITE_VALUE_JOB_PACKED_T job_p;
	GPU_ALIGN static MALI_WRITE_VALUE_JOB_PACKED_T job2_p;

	pan_section_pack(& job_p, WRITE_VALUE_JOB, HEADER, header) {
		header.type = MALI_JOB_TYPE_WRITE_VALUE;
		header.index = 1;
		header.next = (uintptr_t) & job2_p;
	}
	pan_section_pack(& job2_p, WRITE_VALUE_JOB, HEADER, header) {
		header.type = MALI_JOB_TYPE_WRITE_VALUE;
		header.index = 2;
		header.barrier = 1;
		header.next = (uintptr_t) 0;
	}

	pan_section_pack(& job_p, WRITE_VALUE_JOB, PAYLOAD, payload) {
		payload.address = (uintptr_t) & gpu_test_target [0];
		payload.type = MALI_WRITE_VALUE_TYPE_IMMEDIATE_64;
		payload.immediate_value = 0xDEADBEEFABBA1980;
	}
	pan_section_pack(& job2_p, WRITE_VALUE_JOB, PAYLOAD, payload) {
		payload.address = (uintptr_t) & gpu_test_target [1];
		payload.type = MALI_WRITE_VALUE_TYPE_IMMEDIATE_64;
		payload.immediate_value = 0x0123456789ABCDEF;
	}
//    PRINTHEX32(job_p);
//    PRINTHEX32(job2_p);

    dcache_clean_invalidate((uintptr_t) & job_p, sizeof job_p);
    dcache_clean_invalidate((uintptr_t) & job2_p, sizeof job2_p);
    memset((void *) gpu_test_target, 0xE5, sizeof gpu_test_target);
    dcache_clean_invalidate((uintptr_t) & gpu_test_target, sizeof gpu_test_target);
    if (gpu_submit_job(2, (uintptr_t) & job_p))
    	return 1;	// err
      // Проверяем результат выполнения
    printhex64((uintptr_t) gpu_test_target, (void *)gpu_test_target, sizeof gpu_test_target);
    return 0;
}

static int gpu_run_write_value_test_mesa2(void) {
	PRINTF("gpu_run_write_value_test_mesa2:\n");

//	volatile uint32_t * const p = (volatile uint32_t *) 0x40;
//	* p = 1234;

	// Переменная-цель, куда будет писать GPU.
	static volatile uint64_t GPU_ALIGN gpu_test_target [2];

	MALI_WRITE_VALUE_JOB_SECTION_PAYLOAD_TYPE pjob = {
			MALI_WRITE_VALUE_JOB_SECTION_PAYLOAD_header,
			.type = MALI_WRITE_VALUE_TYPE_IMMEDIATE_64
	};
	MALI_WRITE_VALUE_JOB_SECTION_PAYLOAD_TYPE pjob2 = {
			MALI_WRITE_VALUE_JOB_SECTION_PAYLOAD_header,
			.type = MALI_WRITE_VALUE_TYPE_IMMEDIATE_64
	};

	GPU_ALIGN static MALI_WRITE_VALUE_JOB_PACKED_T job_p;
	GPU_ALIGN static MALI_WRITE_VALUE_JOB_PACKED_T job2_p;

    MALI_WRITE_VALUE_JOB_SECTION_HEADER_TYPE jh = {
    		MALI_WRITE_VALUE_JOB_SECTION_HEADER_header,
			.type = MALI_JOB_TYPE_WRITE_VALUE
    };
    MALI_WRITE_VALUE_JOB_SECTION_HEADER_TYPE jh2 = {
    		MALI_WRITE_VALUE_JOB_SECTION_HEADER_header,
			.type = MALI_JOB_TYPE_WRITE_VALUE
    };
    // Заполняем заголовок
    jh.exception_status = 0;
    jh.barrier = 0;			// last
    jh.index = 1;
    jh.next = (uintptr_t) & job2_p;            // Цепочка заканчивается на ней
    MALI_JOB_HEADER_pack(& job_p.HEADER, & jh);

    jh2.exception_status = 0;
    jh2.barrier = 1;			// last
    jh2.index = 2;
    jh2.next = (uintptr_t) 0;            // Цепочка заканчивается на ней
    MALI_JOB_HEADER_pack(& job2_p.HEADER, & jh2);

    // Заполняем Payload записи
    pjob.address = (uintptr_t) & gpu_test_target [0]; // Физический адрес цели
    pjob.immediate_value = 0xDEADBEEFABBA1980;               // Данные для записи
	MALI_WRITE_VALUE_JOB_PAYLOAD_pack(& job_p.PAYLOAD, & pjob);

    pjob2.address = (uintptr_t) & gpu_test_target [1]; // Физический адрес цели
    pjob2.immediate_value = 0x0123456789ABCDEF;               // Данные для записи
	MALI_WRITE_VALUE_JOB_PAYLOAD_pack(& job2_p.PAYLOAD, & pjob2);

//    PRINTHEX32(job_p);
//    PRINTHEX32(job2_p);

    dcache_clean_invalidate((uintptr_t) & job_p, sizeof job_p);
    dcache_clean_invalidate((uintptr_t) & job2_p, sizeof job2_p);
    memset((void *) gpu_test_target, 0xE5, sizeof gpu_test_target);
    dcache_clean_invalidate((uintptr_t) & gpu_test_target, sizeof gpu_test_target);
    if (gpu_submit_job(2, (uintptr_t) & job_p))
    	return 1;	// err
      // Проверяем результат выполнения
    printhex64((uintptr_t) gpu_test_target, (void *)gpu_test_target, sizeof gpu_test_target);
    return 0;
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
// не требуется
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
#if 1
	draw_example();
	gpu_run_write_value_test_mesa();
	//gpu_computejob();
	gpu_run_write_value_test_mesa2();
	//gpu_computejob();
	gpu_run_write_value_test_mesa();
	//gpu_computejob();
	return;
#endif
}
#define GPU_L2_MMU_CONFIG  0x0008 // Смещение внутри блока GPU_CONTROL (0x01800008)
// не требуется
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
	const uint64_t table_phys_addr = hardware_gpu_ttb();

	ASSERT((table_phys_addr & 0xFFF) == 0);

	/**
	 * БитыНазвание конфигурацииОписание и назначение
	 * 0:1 Address Mode (ADRMODE) Задает базовый режим адресации:• 0x0 — Запрещено / Неактивно • 0x2 — Режим AArch64 (LPAE) с размером страницы 4 КБ • 0x3 — Режим AArch64 (LPAE) с размером страницы 64 КБ
	 * 2:5 Input Address Size (IAS) Определяет ширину входного виртуального адреса (VA Bits). Задается в виде закодированного смещения (например, для 48-битного адресного пространства VA_BITS=48).
	 * 6:9 Output Address Size (OAS) Определяет ширину выходного физического адреса (PA Bits) системы. На Bifrost v7 обычно аппаратно ограничена шиной SoC (например, 40 или 44 бита).
	 * 10 Disable Translation (DT) Флаг отключения трансляции (Bypass MMU). Если выставлен в 1, запросы идут напрямую по физическим адресам без таблиц страниц (используется для отладки).
	 * 11 Unprivileged Access Control Флаг, управляющий разделением прав доступа на уровне пользователя и суперпользователя (User/Privileged).
	 * 12:15 Starting Level (SL) Задает начальный уровень обхода таблиц страниц (Level 0, Level 1 или Level 2) в зависимости от размера страницы и размера виртуального адреса.
	 * 16:31 ReservedЗарезервировано под аппаратные нужды архитектуры (обычно заполняется нулями 0).
	 *
	 */
	const uint64_t transflags_val =
//		0x01 * (UINT64_C(1) << 11) |
//	    0x01 * (UINT64_C(1) << 10) |	// DT
		0x02 * (UINT64_C(1) << 0) |	//  (Включает режим адресации ARM 64-bit LPAE с размером страницы 4 КБ).
		//0x03 * (UINT64_C(1) << 0) |	// ВЫЗЫВАЕТ ОШИБКУ ДАЖЕ ПРИ IDENTITY (Включает режим адресации ARM 64-bit LPAE с размером страницы 64 КБ).
		0;


    	// 3. Загружаем физический адрес плоской таблицы
    	// Младшие биты 0x03 включают режим трансляции LPAE
	const uint64_t transtab_val = table_phys_addr |
		1 * (UINT64_C(1) << 4) |	// SHARE_OUTER
//		1 * (UINT64_C(1) << 3) |	// SHARE_INNER - не устанавливается
		1 * (UINT64_C(1) << 2) |	// READ_INNER
//   	0x03 * (UINT64_C(1) << 0) |	// ADRMODE: TABLE (Включена трансляция по таблицам страниц LPAE).
		0x01 * (UINT64_C(1) << 0) |	// ADRMODE: IDENTITY
		0;
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
//	enum aarch64_attrindex
//	{
//		AARCH64_ATTR_INDEX_CACHED = 0,
//		AARCH64_ATTR_INDEX_NCACHED,
//		AARCH64_ATTR_INDEX_DEVICE
//
//	};
	// Индекс 0 = 0xAA (Cacheable), Индекс 1 = 0x22 (Non-Cacheable)

    const uint64_t memattrval =
    		0x000044FF |
    		0;

    unsigned as = 0; // Шейдерный домен по умолчанию
    for (as = 0; as < ARRAY_SIZE(GPU_MMU->MMU_AS); ++ as)
    {
    	GPU_MMU->MMU_AS[as].AS_MEMATTR_HI = 0xFFFFFFFF & (memattrval >> 32);
    	GPU_MMU->MMU_AS[as].AS_MEMATTR_LO = 0xFFFFFFFF & (memattrval >> 0);

    	GPU_MMU->MMU_AS[as].AS_TRANSCFG_HI = 0xFFFFFFFF & (transflags_val >> 32);
    	GPU_MMU->MMU_AS[as].AS_TRANSCFG_LO = 0xFFFFFFFF & (transflags_val >> 0);

        GPU_MMU->MMU_AS[as].AS_TRANSTAB_HI = 0xFFFFFFFF & (transtab_val >> 32);
        GPU_MMU->MMU_AS[as].AS_TRANSTAB_LO = 0xFFFFFFFF & (transtab_val >> 0);

        __DSB();

//        PRINTF("mali_g31_mmu_enable: as=%u: transtab_val=%08X%08X\n", as, (unsigned) (0xFFFFFFFF & (transtab_val >> 32)), (unsigned) (0xFFFFFFFF & (transtab_val >> 0)));
//        PRINTHEX32(GPU_MMU->MMU_AS[as].AS_MEMATTR_HI);
//        PRINTHEX32(GPU_MMU->MMU_AS[as].AS_MEMATTR_LO);
//        PRINTHEX32(GPU_MMU->MMU_AS[as].AS_TRANSCFG_HI);
//        PRINTHEX32(GPU_MMU->MMU_AS[as].AS_TRANSCFG_LO);
//        PRINTHEX32(GPU_MMU->MMU_AS[as].AS_TRANSTAB_HI);
//        PRINTHEX32(GPU_MMU->MMU_AS[as].AS_TRANSTAB_LO);
        ASSERT(GPU_MMU->MMU_AS[as].AS_MEMATTR_HI == (0xFFFFFFFF & (memattrval >> 32)));
        ASSERT(GPU_MMU->MMU_AS[as].AS_MEMATTR_LO == (0xFFFFFFFF & (memattrval >> 0)));
        ASSERT(GPU_MMU->MMU_AS[as].AS_TRANSCFG_HI == (0xFFFFFFFF & (transflags_val >> 32)));
        ASSERT(GPU_MMU->MMU_AS[as].AS_TRANSCFG_LO == (0xFFFFFFFF & (transflags_val >> 0)));
        ASSERT(GPU_MMU->MMU_AS[as].AS_TRANSTAB_HI == (0xFFFFFFFF & (transtab_val >> 32)));
        ASSERT(GPU_MMU->MMU_AS[as].AS_TRANSTAB_LO == (0xFFFFFFFF & (transtab_val >> 0)));

    //    gpu_as_command(as, 0x01);	// Отправляем команду UPDATE для применения таблиц
    //    gpu_as_command(as, 0x03);	// Очищаем внутренний TLB кэш MMU от старых зависших ошибок 0xC8 AS_COMMAND_INVALIDATE

    	gpu_as_command(as, AS_COMMAND_UPDATE);
    	gpu_as_command(as, AS_COMMAND_INVALIDATE);
    //	gpu_as_command(as, AS_COMMAND_FLUSH_PT);

    }
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

#elif  WITHGPUHW && CPUSTYLE_STM32MP1

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

