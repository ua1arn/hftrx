#include "xparameters.h"
#include "xil_exception.h"
#include "xil_types.h"

#if defined(XC7Z010)
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
#elif defined(XC7Z020)
	#if WITHTX || WITHDSPEXTFIR
		#include "lib/zynq/src_7020/axidma_v9_13/xaxidma.h"
	#endif /* WITHTX || WITHDSPEXTFIR */
	#if WITHETHHW
		#include "lib/zynq/src_7020/emacps_v3_14/xemacps.h"
	#endif /* WITHETHHW */
	#include "lib/zynq/src_7020/axivdma_v6_9/xaxivdma.h"
	#include "lib/zynq/src_7020/gpiops_v3_9/xgpiops.h"
	#include "lib/zynq/src_7020/iicps_v3_13/xiicps.h"
	#include "lib/zynq/src_7020/llfifo_v5_5/xllfifo.h"
	#include "lib/zynq/src_7020/vtc_v8_3/xvtc.h"
	#include "lib/zynq/src_7020/xadcps_v2_6/xadcps.h"
#endif /*  */
