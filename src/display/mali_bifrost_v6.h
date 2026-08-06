#ifndef __MALI_BIFROST_V6_H
#define __MALI_BIFROST_V6_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

 /** @addtogroup MALI_Bifrost_Peripheral_Structures
   * @{
   */

 /**
   * @brief  Mali Bifrost Common Job Header Structure definition
   * @note   Direct translation of <struct name="Job Descriptor Header"> from bifrost.xml.
   *         This header prefixes every Job type (Vertex, Fragment, Tiler, Write Value).
   *         Must be aligned and takes exactly 32 bytes (4 words x 64-bit).
   */
 typedef struct
 {
   /* Word 0 (Bytes 0x00 - 0x07): <field name="Exception Status" start="0:0" end="0:31" type="uint"/>
                                  <field name="First Incomplete Task" start="0:32" end="0:63" type="uint"/> */
   union {
     __IO uint64_t WORD0;
     struct {
       __IO uint32_t EXCEPTION_STATUS;     /*!< Hardware fault/exception status code (0 on submission) */
       __IO uint32_t FIRST_INCOMPLETE_TASK;/*!< Index of the first incomplete task in multi-task jobs */
     };
   } STATUS;

   /* Word 1 (Bytes 0x08 - 0x0F): <field name="Fault Address" start="1:0" end="1:63" type="address"/> */
   __IO uint64_t FAULT_ADDRESS;            /*!< GPU MMU fault address populated by hardware on crash */

   /* Word 2 (Bytes 0x10 - 0x17):
      <field name="Job Descriptor Size" start="2:0" end="2:7" type="uint"/>
      <field name="Job Type" start="2:8" end="2:15" type="uint"/>
      <field name="Job Barrier" start="2:16" end="2:16" type="bool"/>
      <field name="Reserved" start="2:17" end="2:31" type="uint"/>
      <field name="Job Index" start="2:32" end="2:63" type="uint"/> */
   union {
     __IO uint64_t WORD2;
     struct {
       __IO uint32_t CONFIG_FLAGS;         /*!< Lower 32-bit: combined Size, Type, and Barrier flags */
       __IO uint32_t JOB_INDEX;            /*!< Upper 32-bit: Unique tracking index of the job */
     };
   } CFG;

   /* Word 3 (Bytes 0x18 - 0x1F): <field name="Next Job" start="3:0" end="3:63" type="address"/> */
   __IO uint64_t NEXT_JOB;                 /*!< 64-bit GPU address of the next job in the hardware chain (0 if tail) */

 } MALI_JobHeader_TypeDef;

 /**
   * @}
   */

 /** @defgroup MALI_JobHeader_Constants
   * @{
   */
 /* Константы для поля CONFIG_FLAGS (Смещение битов внутри 32-битного слова конфигурации) */
 #define MALI_JOB_CTRL_DESC_SIZE_POS        UINT32_C(0)
 #define MALI_JOB_CTRL_DESC_SIZE_MASK        (UINT32_C(0x00000001) << MALI_JOB_CTRL_DESC_SIZE_POS)

 #define MALI_JOB_CTRL_TYPE_POS             UINT32_C(1)
 #define MALI_JOB_CTRL_TYPE_MASK             (UINT32_C(0x0000007F) << MALI_JOB_CTRL_TYPE_POS)

 #define MALI_JOB_CTRL_BARRIER_POS          UINT32_C(16)
 #define MALI_JOB_CTRL_BARRIER_MASK          (UINT32_C(0x00000001) << MALI_JOB_CTRL_BARRIER_POS)


 /* Коды размеров дескрипторов (Job Descriptor Size) */
 #define MALI_JOB_DESC_SIZE_BIFROST         UINT8_C(1)    /*!< Standard layout size for Bifrost hardware */
 /**
   * @}
   */

 /** @addtogroup MALI_Bifrost_Peripheral_Structures
   * @{
   */

 /**
   * @brief  Mali Bifrost Write Value Job Payload Structure definition
   * @note   Direct translation of <struct name="Write Value Job Payload"> from XML.
   *         Must be aligned according to Mali hardware requirements.
   */
 typedef struct
 {
   /* Word 0 (Bytes 0x00 - 0x07): <field name="Address" start="0:0" end="0:63" type="address"/> */
   __IO uint64_t ADDRESS;              /*!< Target 64-bit physical/virtual GPU address to write the data into */

   /* Word 1 (Bytes 0x08 - 0x0F):
      <field name="Value Descriptor" start="1:0" end="1:31" type="uint"/>
      <field name="Reserved" start="1:32" end="1:63" type="uint"/> */
   union {
     __IO uint64_t WORD1;
     struct {
       __IO uint32_t VALUE_DESCRIPTOR; /*!< Write mode configuration flags (e.g. 32-bit or 64-bit immediate write) */
       __IO uint32_t RESERVED_WORD1;   /*!< Strict hardware padding, must be zero */
     };
   } CFG;

   /* Word 2 (Bytes 0x10 - 0x17): <field name="Immediate" start="2:0" end="2:63" type="uint"/> */
   __IO uint64_t IMMEDIATE;            /*!< The actual 64-bit value or 32-bit dword data to be written to ADDRESS */
   uint64_t pad;

 } MALI_WriteValueJobPayload_TypeDef;

 /**
   * @}
   */

 /** @defgroup MALI_WriteValue_Constants
   * @{
   */
 /* Константы для поля VALUE_DESCRIPTOR строго через макрос UINT32_C() */
 #define MALI_WRITE_VALUE_TYPE_IMMEDIATE_64    UINT32_C(0x00000007) /* Запись непосредственного значения 64 bit */
 #define MALI_WRITE_VALUE_TYPE_ZERO        		 UINT32_C(0x00000003) /* Специфичный аппаратный флаг записи нуля */

 /**
   * @}
   */

/** @addtogroup Exported_types
  * @{
  */

/**
  * @brief  Mali Bifrost v6 Fragment Job Payload Structure definition
  * @note   According to Mali GPU Job Manager Hardware Specification.
  *         Must be 64-byte aligned for L2 Cache / Bus master visibility.
  */
typedef struct
{
  __IO uint64_t FB_DESC;              /*!< Framebuffer Descriptor Address. Lower bits [1:0] tag the FBD type (MFBD = 0x1) */
  __IO uint64_t TILE_ALLOC;           /*!< Tile Allocation / Tiler Heap Memory Pointer */
  __IO uint64_t FRAGMENT_BACKEND;     /*!< Fragment Backend Descriptor Pointer (0 for pure Clear passes) */

  union {
    __IO uint64_t WORD3;
    struct {
      __IO uint32_t MIN_TILE_COORD;   /*!< Minimum Tile Coordinates. Format: (Y << 8) | X */
      __IO uint32_t MAX_TILE_COORD;   /*!< Maximum Tile Coordinates. Format: (Y << 8) | X */
    };
  } COORDS;

  __IO uint64_t SCISSORED_TILE_BITMAP;/*!< Scissored Tile Bitmap Base Address (0 if disabled) */

  union {
    __IO uint64_t WORD5;
    struct {
      __IO uint16_t TILES_IN_FLIGHT;  /*!< Maximum outstanding tiles allowed in flight */
      __IO uint16_t CTRL_FLAGS;       /*!< Fragment Control Flags / Cache optimizations */
      __IO uint32_t SHADING_RATE;     /*!< Variable Rate Shading (VRS) Configuration */
    };
  } CTRL;

  __IO uint64_t RESERVED0;            /*!< Reserved for hardware alignment padding */
  __IO uint64_t RESERVED1;            /*!< Reserved for hardware alignment padding */

} MALI_FragmentJobPayload_TypeDef;

/**
  * @}
  */

/** @defgroup MALI_Fragment_Constants
  * @{
  */
/* Использование UINT64_C гарантирует правильное приведение констант на уровне препроцессора */
#define MALI_FBD_TYPE_MFBD                 UINT64_C(0x0000000000000001)
#define MALI_FRAGMENT_JOB_TYPE             ((uint8_t)0x13)

/* Tiles In Flight configurations */
#define MALI_TILES_IN_FLIGHT_DEFAULT       ((uint16_t)0x003F)

/* Coordinate Helper Macros */
#define MALI_PACK_TILE_COORD(X, Y)         ((uint32_t)(((Y) << 8) | ((X) & 0xFF)))
/**
  * @}
  */

/**
  * @brief  Mali Bifrost v6 Framebuffer Descriptor Structure definition
  * @note   According to Mali GPU Multi-Target Framebuffer Descriptor Specification.
  *         Must be 64-byte aligned for L2 Cache / Bus master visibility.
  */
typedef struct
{
  /* --- Первая строка дескриптора (32 байта) --- */
  __IO uint32_t WIDTH_MINUS_1;        /*!< Screen Width minus 1 (e.g. 799) */
  __IO uint32_t HEIGHT_MINUS_1;       /*!< Screen Height minus 1 (e.g. 479) */
  __IO uint32_t SAMPLE_MASK;          /*!< MSAA sample mask. For 1x MSAA always 0x0000FFFF */
  __IO uint32_t RT_COUNT_AND_FLAGS;   /*!< Number of Render Targets and Tiler flags */

  __IO uint64_t TILER_HEAP_START;     /*!< Pointer to the Tiler Heap (gpu_fragment_tile_meta) */
  __IO uint64_t RENDER_TARGET_LIST;   /*!< Pointer to the first Render Target descriptor. Tagged [1:0] = 0x1 */

  /* --- Вторая строка дескриптора (32 байта) --- */
  __IO uint64_t FRAGMENT_FRAME_SHADER;/*!< Pointer to Fragment Frame Shader (0 for pure Clear passes) */
  __IO uint64_t ZS_BLOCK;             /*!< Depth/Stencil buffer configuration block pointer (0 if unused) */
  __IO uint64_t RESERVED_1;           /*!< Hardware alignment padding / System flags */
  __IO uint64_t RESERVED_2;           /*!< Hardware alignment padding / System flags */

} MALI_FramebufferDescriptor_TypeDef;

/** @defgroup MALI_Framebuffer_Constants
  * @{
  */
#define MALI_SAMPLE_MASK_1X                UINT32_C(0x0000FFFF)

/* По спецификации Bifrost v6: (rt_count << 1). Для 1 RT это (1 << 1) = 2 */
#define MALI_RT_COUNT_1                    UINT32_C(0x00000002)

/* Тег для указателя на структуру рендер-таргета */
#define MALI_RT_TAG_MFBD                   UINT64_C(0x0000000000000001)
/**
  * @}
  */
/**
  * @brief  Mali Bifrost v6 Render Target Descriptor Structure definition
  * @note   According to Mali GPU Render Target Descriptor Specification.
  *         Must be 32-byte aligned in memory.
  */
typedef struct
{
  __IO uint32_t FORMAT_FLAGS;         /*!< Pixel format, blending controls, and write-back flags */
  __IO uint32_t STRIDE;               /*!< Line stride in bytes for linear buffers (Width * BytesPerPixel) */
  __IO uint64_t FRAMEBUFFER_POINTER;  /*!< Pure 64-bit GPU physical/virtual address of raw VRAM buffer */

  union {
    __IO float COMPONENT[4];          /*!< Array access to RGBA components [0]=R, [1]=G, [2]=B, [3]=A */
    struct {
      __IO float R;                   /*!< Red color component component (FP32) */
      __IO float G;                   /*!< Green color component component (FP32) */
      __IO float B;                   /*!< Blue color component component (FP32) */
      __IO float A;                   /*!< Alpha color component component (FP32) */
    } RGBA;
  } CLEAR_COLOR;                      /*!< Clear color values in FP32 format for Bifrost v6 hardware */

} MALI_RenderTargetDescriptor_TypeDef;

/** @defgroup MALI_RenderTarget_Constants
  * @{
  */
/* Внутренний сырой формат RGBA8_UNORM + флаг фоновой операции (Clear/Background) */
#define MALI_RT_FORMAT_RGBA8_UNORM         UINT32_C(0x15011000)

/* Количество байт на пиксель для формата RGBA8 */
#define MALI_RT_BPP_RGBA8                  UINT32_C(4)
/**
  * @}
  */

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


#ifdef __cplusplus
}
#endif

#endif /* __MALI_BIFROST_V6_H */
