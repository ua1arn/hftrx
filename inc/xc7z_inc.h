#if defined(XC7Z010) && WITHPS7BOARD_ZM10
	#include "lib/zynq/src_7010_zm/xparameters.h"
	#include "lib/zynq/src_7010_zm/xil_exception.h"
	#include "lib/zynq/src_7010_zm/xil_types.h"
	#if WITHETHHW
		#include "lib/zynq/src_7010_zm/emacps_v3_14/xemacps.h"
	#endif /* WITHETHHW */
	#include "lib/zynq/src_7010_zm/axivdma_v6_9/xaxivdma.h"
	#include "lib/zynq/src_7010_zm/gpiops_v3_9/xgpiops.h"
	#include "lib/zynq/src_7010_zm/iicps_v3_13/xiicps.h"
	#include "lib/zynq/src_7010_zm/vtc_v8_3/xvtc.h"
	#include "lib/zynq/src_7010_zm/xadcps_v2_6/xadcps.h"
	#include "lib/zynq/src_7010_zm/dmaps_v2_8/xdmaps.h"
#elif defined(XC7Z010)
	#include "lib/zynq/src_7010/xparameters.h"
	#include "lib/zynq/src_7010/xil_exception.h"
	#include "lib/zynq/src_7010/xil_types.h"
	#if WITHTX || WITHDSPEXTFIR
		#include "lib/zynq/src_7010/axidma_v9_13/xaxidma.h"
	#endif /* WITHTX || WITHDSPEXTFIR */
	#if WITHETHHW
		#include "lib/zynq/src_7010/emacps_v3_14/xemacps.h"
	#endif /* WITHETHHW */
	#include "lib/zynq/src_7010/axivdma_v6_9/xaxivdma.h"
	#include "lib/zynq/src_7010/gpiops_v3_9/xgpiops.h"
	#include "lib/zynq/src_7010/iicps_v3_13/xiicps.h"
	#include "lib/zynq/src_7010/llfifo_v5_5/xllfifo.h"
	#include "lib/zynq/src_7010/vtc_v8_3/xvtc.h"
	#include "lib/zynq/src_7010/xadcps_v2_6/xadcps.h"
	#include "lib/zynq/src_7010/dmaps_v2_8/xdmaps.h"
#elif defined(XC7Z020) && WITHPS7BOARD_MYC_Y7Z020			// MYC-Y7Z020-4E512D-766-I
	#include "lib/zynq/src_7020_myir/xparameters.h"
	#include "lib/zynq/src_7020_myir/xil_exception.h"
	#include "lib/zynq/src_7020_myir/xil_types.h"
	#if WITHETHHW
		#include "lib/zynq/src_7020_myir/emacps_v3_14/xemacps.h"
	#endif /* WITHETHHW */
	#if WIHSPIDFHW
		#include "lib\zynq\src_7020_myir\qspips_v3_9\xqspips.h"
	#endif /* WIHSPIDFHW */
	#include "lib/zynq/src_7020_myir/axivdma_v6_9/xaxivdma.h"
	#include "lib/zynq/src_7020_myir/gpiops_v3_9/xgpiops.h"
	#include "lib/zynq/src_7020_myir/iicps_v3_13/xiicps.h"
	#include "lib/zynq/src_7020_myir/vtc_v8_3/xvtc.h"
	#include "lib/zynq/src_7020_myir/xadcps_v2_6/xadcps.h"
	#include "lib/zynq/src_7020_myir/dmaps_v2_8/xdmaps.h"
#elif defined(XC7Z020) && WITHPS7BOARD_EBAZ_7020			// EBAZ4205 with xc7z020
	#include "lib/zynq/src_7020_ebaz/xparameters.h"
	#include "lib/zynq/src_7020_ebaz/xil_exception.h"
	#include "lib/zynq/src_7020_ebaz/xil_types.h"
	#if WITHETHHW
		#include "lib/zynq/src_7020_ebaz/emacps_v3_14/xemacps.h"
	#endif /* WITHETHHW */
	#include "lib/zynq/src_7020_ebaz/axivdma_v6_9/xaxivdma.h"
	#include "lib/zynq/src_7020_ebaz/gpiops_v3_9/xgpiops.h"
	#include "lib/zynq/src_7020_ebaz/iicps_v3_13/xiicps.h"
	#include "lib/zynq/src_7020_ebaz/vtc_v8_3/xvtc.h"
	#include "lib/zynq/src_7020_ebaz/xadcps_v2_6/xadcps.h"
	#include "lib/zynq/src_7020_ebaz/dmaps_v2_8/xdmaps.h"
#endif /*  */
