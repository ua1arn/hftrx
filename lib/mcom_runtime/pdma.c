#include "pdma.h"
#include "gic.h"

pdma_mem_channel* get_pdma_dev(int dma_id)
{
	if(dma_id < 0 || dma_id > 7)
	{
		return 0;
	}
	return &(_MemCh[dma_id]);
}

unsigned long long set_cfg_reg(unsigned int dma_id, pdma_mem_channel * dma_ch, unsigned int FC_MODE)
{
   register PDMA_CFG_t CFG;
    CFG.value = 0;
	CFG.CH_PRIOR 	= dma_id; // приоритет равен номеру канала
	CFG.CH_SUSP     = 0; // приостанавливать не нужно
	CFG.FIFO_EMPTY  = 0; // Признак наличия данных в фифо 1 - свободно
	CFG.HS_SEL_DST  = 0; // интерфейс запросов 0 - hw 1 soft
	CFG.HS_SEL_SRC  = 0; // интерфейс запросов 0 - hw 1 soft
	CFG.LOCK_CH_L   = 0; // не реализовано
	CFG.LOCK_B_L    = 0; // не реализовано
	CFG.LOCK_CH	    = 0; // не реализовано
	CFG.LOCK_B	    = 0; // не реализовано
	CFG.DST_HS_POL  = 0; // активный уровен запроса : 0 - высокий 1 - низкий
	CFG.SRC_HS_POL  = 0; // активный уровен запроса : 0 - высокий 1 - низкий
	CFG.MAX_ABRST   = 0; // максимальный размер пакетной транзакции 0 - не ограничено
	CFG.RELOAD_SRC  = 0; // автоматическое обновление адреса - после транзакции загружается оригинал
	CFG.RELOAD_DST  = 0; // автоматическое обновление адреса - после транзакции загружается оригинал
	CFG.FCMODE      = FC_MODE; // 0- запросы  обслуживаются по мере поступления 1 - только если приемник готов
	CFG.FIFO_MODE   = 0; // 0 - должно быть достаточно места 1 должно быть свободным более чем на половину
	CFG.PROTCTL     = 0; // контроль протокола 1
	CFG.DS_UPD_EN   = 0; // разрешение считывания статуса
	CFG.SS_UPD_EN   = 0; // разрешение считывания статуса
	CFG.SRC_PER     = 0; // интерфейс 1 - программный 0 - аппаратный
	CFG.DEST_PER    = 0; // интерфейс 1 - программный 0 - аппаратный

    if (dma_id & 0x1) 
    {
        CFG.value &= ~(0x800);
        CFG.value =
            (CFG.value & ~((unsigned long long) 0xf << 39)) | (((unsigned long long) dma_id << 39) & ((unsigned long long) 0xf << 39));
    } 
    else 
    {
        CFG.value &= ~(0x400);
        CFG.value =
            (CFG.value & ~((unsigned long long) 0xf << 43)) | (((unsigned long long) dma_id << 43) & ((unsigned long long) 0xf << 43));
    }

	dma_ch->CFG = CFG.value;
	return CFG.value;
}

int pdma_configure(int dma_id, void *dst, const void *src, int size)
{
    if (size <= 0)
        return 0;

    if (dma_id < 0 || dma_id > 7)
        return 0;

    pdma_mem_channel *dma_ch = get_pdma_dev(dma_id);
    if (dma_ch == 0)
        return 0;

    if (pdma_interrupt_cfg_regs->ChEnReg & (1 << dma_id))
        return 0;

    dma_ch->SAR = (unsigned)src;
    dma_ch->DAR = (unsigned)dst;
    unsigned long long size_shifted = size >> 2;
    size_shifted = size_shifted << 32;
    dma_ch->CTL = size_shifted | 0x4824;
    pdma_interrupt_cfg_regs->DmaCfgReg = 1;
    return 1;
}

int pdma_hw_configure(int dma_id, void * dst, const void * src, int size, pdma_mem_chain * link, unsigned int dir)
{
    register PDMA_CTL_t CTL;
    //
    if(size <= 0) return 0;
    if (dma_id <0 || dma_id >7) return 0;
    pdma_mem_channel* dma_ch = get_pdma_dev(dma_id);
    if (dma_ch == 0) return 0;
    //unsigned long long dma_tst = 0;
    //
    //dma_tst = pdma_interrupt_cfg_regs->ChEnReg;
    //
    if (pdma_interrupt_cfg_regs->ChEnReg & (1<<dma_id)) return 0;
    CTL.value = 0;
    dma_ch->SAR = (unsigned)src;
    dma_ch->DAR = (unsigned)dst;
    
    dma_ch->LLP = (int)link;

    unsigned long long size_shifted = size;//>>2;
    //size_shifted = size_shifted<<32;
    //dma_ch->CTL.value = size_shifted | 0x4824;
    //dma_ch->CTL.value = 0;
    CTL.INT_EN 	        = 1;//Разрешение прерываний (Interrupt Enable Bit).  
									//Если установлено в 1, то включаются все возможные источники 
									//прерываний. Работает как глобальная маска для всех прерываний Канала. 
    CTL.DST_TR_WIDTH    = 0;//разрядность ввыхода
    CTL.SRC_TR_WIDTH    = 0;//разрядность входа
    //Ниже в switch
    //CTL.DINC 			= 1;// 00=+1 01=-1 1x= 0	назначение
    //CTL.SINC 			= 0;// 00=+1 01=-1 1x= 0	источник
    CTL.DEST_MSIZE 	    = 0;//Разрядность одиночного обращения AHB Приемника
    CTL.SRC_MSIZE  	    = 0;//Разрядность одиночного обращения AHB Источника(Source Transfer Width). 
    CTL.SRC_GATHER_EN   = 0;//Разрешение режима Сбора данных Источника. 0 - Сбор запрещён. 1 - Сбор разрешён. 
							//Режим Сбора применим только в том случае, если поле CTLx.SINC настроено на увеличение или уменьшение адреса Источника. 
    CTL.DST_SCATTER_EN  = 0;//Разрешение режима Разброса данных Приёмника. 
									//0 - Разброс запрещён.   1 - Разброс разрешён. 
									//Режим Разброса применим только в том случае, если поле CTLx.DINC настроено на увеличение или уменьшение адреса Приёмника. 
									//Примечание. Поле не реализовано и при чтении возвращает ноль, если не задан аппаратный параметр DMAH_CHx_DST_SCA_EN. 
 									//В данной реализации PDMA аппаратный параметр  DMAH_CHx_DST_SCA_EN задан для всех Каналов. 
 	switch(dir)
 	{
 	   case 1: CTL.TT_FC = 1;CTL.SINC = 2;break;//uart-> ram
 	   case 3: CTL.TT_FC = 4;CTL.SINC = 2;break;//uart-> ram
 	   case 2: CTL.TT_FC = 2;CTL.DINC = 2;break;//ram -> uart
 	   case 4: CTL.TT_FC = 6;CTL.DINC = 2;break;//ram -> uart
 	}
  //dma_ch->CTL.TT_FC 		    =   //Тип передачи и Управление размером Блока (Transfer Type and Flow Control). Возможны следующие варианты:  
//TT_FC  Тип передачи  Управление размером Блока 
//000  Память-Память  PDMA          001  Память-Периферия  PDMA        010  Периферия-Память  PDMA  	  011  Периферия-Периферия  PDMA 
//100  Периферия-Память  Периферия  101  Периферия-Периферия  Источник 110  Память-Периферия  Периферия   111  Периферия-Периферия  Приёмник 
//При использовании цепочки Блоков поле TT_FC должно оставаться неизменным для всех Блоков цепочки. 
//
//Примечание. Для каждого Канала аппаратно задан тип управления размером Блока: 
//Размером Блока управляет только PDMA (DMA_FC_ONLY)      Размером Блока управляет только Источник (SRC_FC_ONLY) 
//Размером Блока управляет только Приёмник (DST_FC_ONLY)  Размером Блока управляет или Периферия или PDMA (ANY_FC) 
//Значение по сбросу: 
//TT_FC[0] = 1 
//TT_FC[1] = 1 если тип отличен от SRC_FC_ONLY 
//TT_FC[2] = 1 при SRC_FC_ONLY или DST_FC_ONLY 
// 
//Поле TT_FC[2] не реализовано и при чтении возвращает 0 при DMA_FC_ONLY. 
//Поле TT_FC[2:1] не реализовано и при чтении возвращает 10 при SRC_FC_ONLY. 
//Поле TT_FC[2:1] не реализовано и при чтении возвращает 11 при DST_FC_ONLY. 
// 
//В данной реализации PDMA для всех каналов аппаратно задан тип управления передачей ANY_FC. Соответственно состояние по сбросу: TT_FC[2:0] = 011. 

    //dma_ch->CTL.DMS 			= 0;//Destination Master Select. Выбор AHB интерфейса, по которому PDMA обращается к Приёмнику.  00 — AHB Master 1 01 — AHB Master 2 10 — AHB Master 3 11 — AHB Master 4 
    //dma_ch->CTL.SMS 			= 0;//Source Master Select. Выбор AHB интерфейса, по которому PDMA обращается к Источнику.  00 — AHB Master 1 01 — AHB Master 2 10 — AHB Master 3 11 — AHB Master 4
    if(link !=0)
    if(dir == 1 || dir == 3) {CTL.LLP_DST_EN = 1;}
    if(dir == 2 || dir == 4) {CTL.LLP_SRC_EN = 1;}
    //dma_ch->CTL.LLP_DST_EN 	    =  0;//Разрешение цепочки Блоков Приёмника. Цепочки Блоков Приёмника разрешены при LLPx.LOC != 0 и LLP_DST_EN в 1.
    //dma_ch->CTL.LLP_SRC_EN 	    =  0;//Разрешение цепочки Блоков Источника. Цепочки Блоков Источника разрешены при LLPx.LOC != 0 и LLP_SRC_EN в 1.
    CTL.BLOCK_TS 	 	= size_shifted;//Размер Блока (Block Transfer Size). Если размером Блока управляет PDMA, то это поле устанавливается пользователем до запуска Канала. Значение 
//поля задаёт количество Одиночных обращений на шине AHB к Источнику, которое будет произведено за один Блок. Одиночное обращение 
//соответствует одному «beat» AHB. Разрядность Одиночного обращения определяется полем CTLx.SRC_TR_WIDTH. 
    //dma_ch->CTL.DONE 			=  ;//При использовании возможности обратной записи разряды CTLx[63:32] 
//записываются в LLI в конце обработки Блока с установленным разрядом DONE. Программно опрашивая разряд DONE LLI.CTL, можно определить 
//выполнен ли конкретный Блок в цепочке или нет. При настройке цепочки Блоков разряд DONE регистра LLI.CTL должен быть сброшен до запуска Канала. 
  dma_ch->CTL = CTL.value;
  set_cfg_reg(dma_id, dma_ch, 0);
  pdma_interrupt_cfg_regs->DmaCfgReg = 1;
  return 1;
}

int pdma_run(int dma_id)
{
//MaskTfr
//MaskBlock  
//MaskSrcTran
//MaskDstTran
//MaskErr
    //
    register int run_code = (1<<dma_id) | ((1<<8)<<(dma_id));// ((1<<8)<<(dma_id)) - разрешение записи в соответствующие разряды
    //
	pdma_interrupt_regs->MaskTfr |= run_code;
	//
	pdma_interrupt_cfg_regs->ChEnReg = run_code;
	//
    return 1;
}

void Init_Pdma_dev(void)
{
	for(int i=0 ; i<6;i++) pdma_stop(i);
	pdma_interrupt_regs->ClearTfr = 0xff;
	pdma_interrupt_regs->ClearBlock = 0xff;
	pdma_interrupt_regs->ClearSrcTran = 0xff;
	pdma_interrupt_regs->ClearDstTran = 0xff;
	pdma_interrupt_regs->ClearErr  = 0xff;
}

int pdma_stop(int dma_id)
{
   register PDMA_CFG_t CFG;
   //register unsigned long long dma_status;
    CFG.value = 0;
    pdma_mem_channel *dma_ch = get_pdma_dev(dma_id);
    if (dma_ch == 0)
        return 0;

      int is_run = pdma_interrupt_cfg_regs->ChEnReg&(1<<dma_id);
    //
      CFG.value = dma_ch->CFG;
    //
    if (is_run && CFG.FIFO_EMPTY == 0)//dma_ch->CFG ???
    {
		CFG.CH_SUSP     = 1; // приостанавливать нужно
		dma_ch->CFG = CFG.value;
		do
		{
		    CFG.value = dma_ch->CFG;
		}
		while(!CFG.FIFO_EMPTY);
	}

	//dma_status = pdma_interrupt_cfg_regs->ChEnReg;
	//dma_status &= (~(1<<dma_id))&0x07F;
	pdma_interrupt_cfg_regs->ChEnReg = ((1<<8)<<(dma_id));//dma_status;
    return 1;
}

int pdma_wait(int dma_id)
{
    while(1)
    {
      int is_run = pdma_interrupt_cfg_regs->ChEnReg&(1<<dma_id);
      if (!is_run) break;
    }
    return 1;
}

int pdma_get_free(void)
{
  register int dma_id, is_run;
    for(dma_id = 0 ; dma_id < 8 ; dma_id++)
    {
      is_run = pdma_interrupt_cfg_regs->ChEnReg&(1<<dma_id);
      if (!is_run) return dma_id;
    }

    return -1;
}

int pdma_is_free(int dma_id)
{
      int is_run = pdma_interrupt_cfg_regs->ChEnReg&(1<<dma_id);
    if (!is_run) return 1;

    return 0;
}

int pdma_copy(unsigned int dma_id, void * dst, const void * src, unsigned int size)
{
    unsigned int src_t, dst_t;
    int n_blocks = 0;
    unsigned int current_shift = 0;
    int i = 0;
    int size_w = size>>2;

    n_blocks = size_w/PDMA_MAX_BLOCK_SIZE;


    for (i=0;i<n_blocks;i++)
    {
        current_shift = (i*PDMA_MAX_BLOCK_SIZE*4);
        dst_t = (unsigned int)dst + current_shift;
        src_t = (unsigned int)src + current_shift;
        if(pdma_configure(dma_id, (void*)(dst_t), (void*)(src_t), PDMA_MAX_BLOCK_SIZE*4))
        {
            pdma_run(dma_id);
            pdma_wait(dma_id);
        }
        else return 0;
    }
    //
    if ((size_w%PDMA_MAX_BLOCK_SIZE)!=0)
    {
        current_shift = (n_blocks*PDMA_MAX_BLOCK_SIZE*4);
        dst_t = (unsigned int)dst + current_shift;
        src_t = (unsigned int)src + current_shift;
        int wsize =  (size_w%PDMA_MAX_BLOCK_SIZE)<<2;
        if(pdma_configure(dma_id, (void*)(dst_t), (void*)(src_t), wsize))
        {
            pdma_run(dma_id);
            pdma_wait(dma_id);
        }
        else return 0;
    }

    return 1;
}

void pdma_handshake_enable(int channel, int handshake_num)
{
    pdma_mem_channel *channel_regs = (pdma_mem_channel *)(PDMA_BASE + channel * 0x58);

    if (handshake_num & 0x1) 
    {
        channel_regs->CFG &= ~(0x800);
        channel_regs->CFG =
            (channel_regs->CFG & ~((unsigned long long) 0xf << 39)) | (((unsigned long long) handshake_num << 39) & ((unsigned long long) 0xf << 39));
    } 
    else 
    {
        channel_regs->CFG &= ~(0x400);
        channel_regs->CFG =
            (channel_regs->CFG & ~((unsigned long long) 0xf << 43)) | (((unsigned long long) handshake_num << 43) & ((unsigned long long) 0xf << 43));
    }
}

int pdma_enable_interrupt(int channel, dma_irq irq, handler_funct_ptr funct_pointer)
{
	volatile unsigned long long *mask_reg;
	int irq_num;

	switch (irq)
	{
	case IntTfr:
		mask_reg = &(pdma_interrupt_regs->MaskTfr);
		irq_num = PDMA_INT_FLAG0;
		break;
	case IntBlock:
		mask_reg = &(pdma_interrupt_regs->MaskBlock);
		irq_num = PDMA_INT_FLAG1;
		break;
	case IntDstTran:
		mask_reg = &(pdma_interrupt_regs->MaskDstTran);
		irq_num = PDMA_INT_FLAG3;
		break;
	case IntSrcTran:
		mask_reg = &(pdma_interrupt_regs->MaskSrcTran);
		irq_num = PDMA_INT_FLAG2;
		break;
	case IntError:
		mask_reg = &(pdma_interrupt_regs->MaskErr);
		irq_num = PDMA_INT_FLAG4;		
		break;

	default:
		return 1;
	}

	risc_enable_interrupt(irq_num, GIC_PL390_TARGET_CPU0, 0xF1);
	risc_register_interrupt(funct_pointer, irq_num, 0);

	*mask_reg |= (1 << (channel + 8)) | (1 << channel);

    return 0;
}

pdma_mem_chain * pdma_fill_link(pdma_mem_chain * link, void * dst, void * src, unsigned int size, pdma_mem_chain * link2, unsigned int dir)
{
    //register
	PDMA_CTL_t CTL;
    link->DAR = (unsigned int)dst;
    link->SAR = (unsigned int)src;
    //
    if(link2 == 0)
      link->LLP = 0;//(unsigned int)(link + 1);
    else
      link->LLP = (unsigned int)link2;
    //
    link->SSTAT = 0;
    link->DSTAT = 0;
    unsigned long long size_shifted = size;//>>2;
    //size_shifted = size_shifted<<32;
    //link->CTL.value = size_shifted | 0x4824;
    CTL.value = 0;
    CTL.INT_EN 	        = 1;//Разрешение прерываний (Interrupt Enable Bit).  
								//Если установлено в 1, то включаются все возможные источники 
								//прерываний. Работает как глобальная маска для всех прерываний Канала. 
    CTL.DST_TR_WIDTH    = 0;//разрядность ввыхода
    CTL.SRC_TR_WIDTH    = 0;//разрядность входа
    //Ниже в switch
    //CTL.DINC 			= 1;// 00=+1 01=-1 1x= 0	назначение
    //CTL.SINC 			= 0;// 00=+1 01=-1 1x= 0	источник
    CTL.DEST_MSIZE 	    = 0;//Разрядность одиночного обращения AHB Приемника
    CTL.SRC_MSIZE  	    = 0;//Разрядность одиночного обращения AHB Источника(Source Transfer Width). 
    CTL.SRC_GATHER_EN   = 0;//Разрешение режима Сбора данных Источника. 0 - Сбор запрещён. 1 - Сбор разрешён. 
								//Режим Сбора применим только в том случае, если поле CTLx.SINC настроено на увеличение или уменьшение адреса Источника. 
    CTL.DST_SCATTER_EN  = 0;//Разрешение режима Разброса данных Приёмника. 
									//0 - Разброс запрещён.   1 - Разброс разрешён. 
									//Режим Разброса применим только в том случае, если поле CTLx.DINC настроено на увеличение или уменьшение адреса Приёмника. 
									//Примечание. Поле не реализовано и при чтении возвращает ноль, если не задан аппаратный параметр DMAH_CHx_DST_SCA_EN. 
 									//В данной реализации PDMA аппаратный параметр  DMAH_CHx_DST_SCA_EN задан для всех Каналов. 
 	switch(dir)
 	{
 	   case 1: CTL.TT_FC = 1;CTL.SINC = 2;break;//uart-> ram
 	   case 3: CTL.TT_FC = 4;CTL.SINC = 2;break;//uart-> ram
 	   case 2: CTL.TT_FC = 2;CTL.DINC = 2;break;//ram -> uart
 	   case 4: CTL.TT_FC = 6;CTL.DINC = 2;break;//ram -> uart
 	}
  //link->CTL.TT_FC 		    =   //Тип передачи и Управление размером Блока (Transfer Type and Flow Control). Возможны следующие варианты:  
//TT_FC  Тип передачи  Управление размером Блока 
//000  Память-Память  PDMA          001  Память-Периферия  PDMA        010  Периферия-Память  PDMA  	  011  Периферия-Периферия  PDMA 
//100  Периферия-Память  Периферия  101  Периферия-Периферия  Источник 110  Память-Периферия  Периферия   111  Периферия-Периферия  Приёмник 
//При использовании цепочки Блоков поле TT_FC должно оставаться неизменным для всех Блоков цепочки. 
//
//Примечание. Для каждого Канала аппаратно задан тип управления размером Блока: 
//Размером Блока управляет только PDMA (DMA_FC_ONLY)      Размером Блока управляет только Источник (SRC_FC_ONLY) 
//Размером Блока управляет только Приёмник (DST_FC_ONLY)  Размером Блока управляет или Периферия или PDMA (ANY_FC) 
//Значение по сбросу: 
//TT_FC[0] = 1 
//TT_FC[1] = 1 если тип отличен от SRC_FC_ONLY 
//TT_FC[2] = 1 при SRC_FC_ONLY или DST_FC_ONLY 
// 
//Поле TT_FC[2] не реализовано и при чтении возвращает 0 при DMA_FC_ONLY. 
//Поле TT_FC[2:1] не реализовано и при чтении возвращает 10 при SRC_FC_ONLY. 
//Поле TT_FC[2:1] не реализовано и при чтении возвращает 11 при DST_FC_ONLY. 
// 
//В данной реализации PDMA для всех каналов аппаратно задан тип управления передачей ANY_FC. Соответственно состояние по сбросу: TT_FC[2:0] = 011. 

    //link->CTL.DMS 			= 0;//Destination Master Select. Выбор AHB интерфейса, по которому PDMA обращается к Приёмнику.  00 — AHB Master 1 01 — AHB Master 2 10 — AHB Master 3 11 — AHB Master 4 
    //link->CTL.SMS 			= 0;//Source Master Select. Выбор AHB интерфейса, по которому PDMA обращается к Источнику.  00 — AHB Master 1 01 — AHB Master 2 10 — AHB Master 3 11 — AHB Master 4
    if(link !=0)
    if(dir == 1 || dir == 3) {CTL.LLP_DST_EN = 1;}
    if(dir == 2 || dir == 4) {CTL.LLP_SRC_EN = 1;CTL.LLP_DST_EN = 1;}//CTL.LLP_DST_EN = 1;
    //link->CTL.LLP_DST_EN 	    =  0;//Разрешение цепочки Блоков Приёмника. Цепочки Блоков Приёмника разрешены при LLPx.LOC != 0 и LLP_DST_EN в 1.
    //link->CTL.LLP_SRC_EN 	    =  0;//Разрешение цепочки Блоков Источника. Цепочки Блоков Источника разрешены при LLPx.LOC != 0 и LLP_SRC_EN в 1.
    CTL.BLOCK_TS 	 	= size_shifted;//Размер Блока (Block Transfer Size). Если размером Блока управляет PDMA, то это поле устанавливается пользователем до запуска Канала. Значение 
//поля задаёт количество Одиночных обращений на шине AHB к Источнику, которое будет произведено за один Блок. Одиночное обращение 
//соответствует одному «beat» AHB. Разрядность Одиночного обращения определяется полем CTLx.SRC_TR_WIDTH. 
    //link->CTL.DONE 			=  ;//При использовании возможности обратной записи разряды CTLx[63:32] 
//записываются в LLI в конце обработки Блока с установленным разрядом DONE. Программно опрашивая разряд DONE LLI.CTL, можно определить 
//выполнен ли конкретный Блок в цепочке или нет. При настройке цепочки Блоков разряд DONE регистра LLI.CTL должен быть сброшен до запуска Канала. 
    //if (llp_en == 1) CTL.value |= 0x18000000;
    link->CTL = CTL.value;
    return link;
}

int pdma_start_chain(int dma_id, pdma_mem_chain *link, pdma_mem_chain *link2)
{
    unsigned long long cfg;
    pdma_mem_channel* dma_ch = get_pdma_dev(dma_id);
    if (dma_ch == 0) return 0;

    if (pdma_interrupt_cfg_regs->ChEnReg & (1<<dma_id)) return 0;
    cfg = set_cfg_reg(dma_id, dma_ch, 0);
    //link->CFG = cfg;
	pdma_ctl_reg ctl = {
		.BLOCK_TS 		= 0,
		.SRC_TR_WIDTH 	= 0,
		.DST_TR_WIDTH 	= 0,
		.TT_FC 			= PeripheralToMemory,
		.SRC_GATHER_EN 	= 0,
		.DST_SCATTER_EN = 0,
		.SRC_MSIZE 		= DMA_BurstSize1,
		.DEST_MSIZE 	= DMA_BurstSize1,
		.SINC 			= NoChange,
		.DINC 			= Incr,
		.INT_EN 		= 0,
		.LLP_DST_EN 	= 1,
		.LLP_SRC_EN		= 1
	};
	//
    //link2->CFG = cfg;
	dma_ch->LLP = (unsigned int)link;
    dma_ch->DAR = 0;
    dma_ch->SAR = 0;
    dma_ch->CTL = ctl.value;
    pdma_interrupt_cfg_regs->DmaCfgReg = 1;
    //pdma_interrupt_regs->MaskTfr = ((1<<8) << dma_id) | (1 << dma_id);
    pdma_run(dma_id);
   return 1;
}

/*
int pdma_start_chain(int dma_id, pdma_mem_chain *link, pdma_mem_chain *link2)
{
    unsigned long long cfg;
    pdma_mem_channel* dma_ch = get_pdma_dev(dma_id);
    if (dma_ch == 0) return 0;

    if (pdma_interrupt_cfg_regs->ChEnReg & (1<<dma_id)) return 0;
    cfg = set_cfg_reg(dma_id, dma_ch, 0);
    //link->CFG = cfg;
    //link2->CFG = cfg;
	dma_ch->LLP = link2;
    dma_ch->DAR = link->DAR;
    dma_ch->SAR = link->SAR;
    dma_ch->CTL = link->CTL;
    pdma_interrupt_cfg_regs->DmaCfgReg = 1;
    //pdma_interrupt_regs->MaskTfr = ((1<<8) << dma_id) | (1 << dma_id);
    pdma_run(dma_id);
   return 1;
}
*/
