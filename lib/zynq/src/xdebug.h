#ifndef XDEBUG  /* prevent circular inclusions */
#define XDEBUG  /* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

#include "formats.h"

#define xil_printf	debug_printf_P
#define xdbg_printf(type, ...) (((type) & xdbg_current_types) ? debug_printf_P (__VA_ARGS__) : 0)

#define XDBG_DEBUG_ERROR             0x00000001U    /* error  condition messages */
#define XDBG_DEBUG_GENERAL           0x00000002U    /* general debug  messages */
#define XDBG_DEBUG_ALL               0xFFFFFFFFU    /* all debugging data */

#define xdbg_current_types (XDBG_DEBUG_ERROR)

#ifdef __cplusplus
}
#endif

#endif /* XDEBUG */
