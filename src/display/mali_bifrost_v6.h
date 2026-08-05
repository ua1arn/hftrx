#ifndef __MALI_BIFROST_V6_H
#define __MALI_BIFROST_V6_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* __MALI_BIFROST_V6_H */
