#ifndef NAND_H
#define NAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "1892vm14ya.h"

    typedef union
    {
        volatile unsigned int value;
        struct
        {
            volatile unsigned int packet_size    : 11 ;  //  Размер пакета. Количество байт, которое должен обработать контроллер за один пакет. 11’h001 – 1 байт. 11’h002 – 2 байта. ...  Типичный размер пакета в режиме коррекции ошибок BCH 4, 8, 12 и 16 бит ? 512 байт. Типичный размер пакета в режиме коррекции ошибок BCH 24 бит - 1024 байт. 
            volatile unsigned int reserved 	     : 1  ;  //  Зарезервировано.                                                                                                                                                                                                                                                                           
            volatile unsigned int packet_count   : 12 ;  //  Количество пакетов. 12’h001 – количество пакетов = 1. ... 12’h800 - количество пакетов =2048.                                                                                                                                                                                             
            volatile unsigned int reserved2	     : 8  ;  //  Зарезервировано.                                                                                                                                                                                                                                                                           
        };
    }t_PACKET;

    typedef union
    {
        volatile unsigned int value;
        struct
        {
            volatile unsigned int column_addr     : 16 ;  //  Адрес столбца.                                                                                                                                                                                                           
            volatile unsigned int page_addr  	  : 7  ;  //  Адрес страницы.                                                                                                                                                                                                           
            volatile unsigned int lsb_block_addr  : 9  ;  //  Адрес блока (младшие разряды). Разряды [24:23] – имеют особое значение для interleaved/mulit-plane операций. 2’b00 – адрес/плоскость 0. 2’b01 – адрес/плоскость 1. 2’b10 – адрес/плоскость 2. 2’b11 – адрес/плоскость 3. 
        } v1;
        struct
        {
            volatile unsigned int page_addr  	  : 7  ;  //  Адрес страницы.                                                                                                                                                                                                           
            volatile unsigned int plane     	  : 2  ;  //  Плосткость
            volatile unsigned int block_addr      : 9  ;  //  Адрес блока
            volatile unsigned int lun		      : 1  ;  //  lun 0/1
            volatile unsigned int lsb_block_addr  : 13 ;  //  =0
        } v2;
    }t_MEMADDR1;
/*
Примечание.  Для  команд  Block  Erase  и  Read  Status  Enhanced  регистр  MEMADDR1 
должен быть запрограммирован следующим образом: 
Разряды [6:0] – адрес страницы. 
Разряды [8:7] – выбор одного из чередующихся адресов: 
2’b00 – адрес/плоскость 0. 
2’b01 – адрес/плоскость 1. 
2’b10 – адрес/плоскость 2. 
2’b11 – адрес/плоскость 3. 
Разряды [17:9] – адрес блока. 
Разряды [18] – выбор логического устройства (LUN). 
1’b0 - выбран LUN0 . 
1’b1 - выбран LUN1. 
Разряды [31:19] – должны быть равны нулю. 
*/
    typedef union//Для  команд  Block  Erase  и  Read  Status  Enhanced  биты  [7:0]  регистра MEMADDR2 должны быть равны нулю. 
    {
        volatile unsigned int value;
        struct
        {
            volatile unsigned int msb_block_addr  : 1  ;  //  Адрес блока (старшие разряды).                                                                                                                         
            volatile unsigned int lun_addr  	  : 6  ;  //  Адрес LUN. 6'h0 – выбор LUN0. 6'h1 – выбор LUN1.                                                                                                        
            volatile unsigned int reserved  	  : 17 ;  //  Зарезервировано                                                                                                                                        
            volatile unsigned int nfc_bch_mode    : 3  ;  //  Режим работы BCH: 3’b000 – 16-битный код ECC 3’b001 – 12-битный код ECC 3’b010 – 8-битный код ECC 3’b011 – 4-битный код ECC 3’b100 – 24-битный код ECC  
            volatile unsigned int mode  		  : 2  ;  //  Режим работы с флэш-памятями. 2’b00 - Последовательный режим. 2’b01 - Параллельный режим.  2’b10 - Зарезервировано. 2’b11 - Зарезервировано.            
            volatile unsigned int chip_select  	  : 2  ;  //  Выбор флэш-памяти. 2’b00 - флэш-память 0. 2’b01 - флэш-память 1. 2’b10 - флэш-память 2. 2’b11 - флэш-память 3.                                          
        };
    }t_MEMADDR2;

    typedef union
    {
        volatile unsigned int value;
        struct
        {
            volatile unsigned int command1  	  : 8  ;  //  Код операции первой команды для флэш-памяти.                                                                                                            
            volatile unsigned int command2  	  : 8  ;  //  Код операции второй команды для флэш-памяти.                                                                                                             
            volatile unsigned int asyn_modes  	  : 3  ;  //  Асинхронный режим. 3’b000- режим 0. 3’b001- режим 1. 3’b010- режим 2. 3’b011- режим 3. 3’b100- режим 4. 3’b101- режим 5. 3’b110- зарезервировано.       
            volatile unsigned int asyn_syn     	  : 1  ;  //  Выбор режима синхронизации 1’d0 - асинхронный режим. 1’d1 – синхронный режим.                                                                            
            volatile unsigned int syn_modes  	  : 3  ;  //  Синхронный режим. 3’b000- режим 0. 3’b001- режим 1. 3’b010- режим 2. 3’b011- режим 3. 3’b100- режим 4. 3’b101- режим 5. 3’b110- зарезервировано.         
            volatile unsigned int page_size  	  : 3  ;  //  Размер страницы. 3’d0 – 512 байт. 3’d1 – 2K. 3’d2 – 4K. 3’d3 - 8K. 4-7 - Зарезервировано.                                                                
            volatile unsigned int dma_enable  	  : 2  ;  //  Режим обмена данными. 2’d0 - Режим PIO. 2’d1 - Режим Slave DMA. 2’d2 - Режим MDMA. 2’d3 - Зарезервировано.                                               
            volatile unsigned int num_addr_cycles : 3  ;  //  Число циклов адреса в команде для флэш-памяти. 3’d0 – Не используется. 3’d1 – Один цикл адреса. 3’d2- Два цикла адреса. .... 3’d7 – Семь циклов адреса.  
            volatile unsigned int ecc_on_off   	  : 1  ;  //  Включение/отключение коррекции ошибок. 1 – ECC включён. 0 - ECC выключен.                                                                                
        };
    }t_COMMAND;


    typedef union
    {
        volatile unsigned int value;
        struct
        {
            volatile unsigned int read   						  : 1  ;  //  Операция чтения из флэш-памяти. 
            volatile unsigned int multi_die   					  : 1  ;  //  Признак Interleaved/Multiplane операции (Page Program, Read, Block Erase). 
            volatile unsigned int block_erase   				  : 1  ;  //  Операция Block Erase. 
            volatile unsigned int read_status   				  : 1  ;  //  Операция Read Status. 
            volatile unsigned int page_program   				  : 1  ;  //  Операция Page Program. 
            volatile unsigned int multi_die_rd   				  : 1  ;  //  Операция чтения из разных LUN. 
            volatile unsigned int read_id  						  : 1  ;  //  Операция Read ID. 
            volatile unsigned int read_parameter_page   		  : 1  ;  //  Операция Read Parameter Page. 
            volatile unsigned int reset   						  : 1  ;  //  Операция Reset. 
            volatile unsigned int get_features   				  : 1  ;  //  Операция Get Features. 
            volatile unsigned int set_features  				  : 1  ;  //  Операция Set Features. 
            volatile unsigned int read_unique_id  				  : 1  ;  //  Операция Read Unique ID. 
            volatile unsigned int read_status_enhanced   		  : 1  ;  //  Операция Read Status Enhanced. 
            volatile unsigned int read_interleaved   			  : 1  ;  //  Операция Interleaved/Multiplane чтения. 
            volatile unsigned int change_read_column_enhanced     : 1  ;  //  Операция Change Read Column Enhanced. 
            volatile unsigned int copy_back_interleaved  		  : 1  ;  //  Операция Copy Back Interleaved. 
            volatile unsigned int read_cache_start   			  : 1  ;  //  Контроллер выполняет операцию Read. 
            volatile unsigned int read_cache_sequential   		  : 1  ;  //  Операция Read Cache Sequential. 
            volatile unsigned int read_cache_random   			  : 1  ;  //  Операция Read Cache Random. 
            volatile unsigned int read_cache_end  				  : 1  ;  //  Операция Read Cache End. 
            volatile unsigned int small_data_move   			  : 1  ;  //  Операция Small Data Move. 
            volatile unsigned int change_row_addr  				  : 1  ;  //  Операция Change Row Address. 
            volatile unsigned int change_row_addr_end  			  : 1  ;  //  1’b1 - контроллер завершает операцию Change Row Address. 
            volatile unsigned int reset_lun  					  : 1  ;  //  1’b1 - контроллер выполняет операцию сброса заданного логического устройства (LUN)  
            volatile unsigned int pgm_pg_reg_clr  				  : 1  ;  //  1’b1 - контроллер выполняет операцию c включённым свойством Page Register Clear Enhancement. 
            volatile unsigned int reserved   					  : 7  ;  //  Зарезервировано. 
        };
    }t_PROGRAM;

    typedef union
    {
        volatile unsigned int value;
        struct
        {
            volatile unsigned int buff_wr_rdy_sts_en  	: 1    ;  //  Разрешение признака прерывания «Buffer Write Ready» в регистре INTERRUPT_STATUS (разряд 0).  1'b1 – разрешено. 1'b0 – запрещено. 
            volatile unsigned int buff_rd_rdy_sts_en  	: 1    ;  //  Разрешение признака прерывания «Buffer Read Ready» в регистре INTERRUPT_STATUS (разряд 1).  1'b1 – разрешено. 1'b0 – запрещено. 
            volatile unsigned int trans_comp_sts_en  	: 1    ;  //  Разрешение признака прерывания «Transfer Complete» в регистре INTERRUPT_STATUS (разряд 2).  1'b1 – разрешено. 1'b0 – запрещено. 
            volatile unsigned int mul_bit_err_sts_en  	: 1    ;  //  Разрешение признака прерывания «Multi Bit Error» в регистре INTERRUPT_STATUS (разряд 3).  1'b1 – разрешено. 1'b0 – запрещено. Значение данного поля используется при коррекции ошибок по алгоритму Хемминга (HAM), иначе заменяется нулем. 
            volatile unsigned int err_intrpt_sts_en  	: 1    ;  //  Разрешение признака прерывания «Single Bit Error» в режиме HAM или «BCH Detect Error» в режиме BCH в регистре INTERRUPT_STATUS (разряд 4).  1'b1 – разрешено. 1'b0 – запрещено. 
            volatile unsigned int reserved  			: 1    ;  //  Зарезервировано. 
            volatile unsigned int dma_int_sts_en  		: 1    ;  //  Разрешение признака прерывания «dma_int» в регистре INTERRUPT_STATUS (разряд 6).  1'b1 – разрешено. 1'b0 – запрещено. Используется только при пересылке в режиме MDMA, иначе заменяется нулем. 
            volatile unsigned int error_ahb_sts_en  	: 1    ;  //  Разрешение признака прерывания «error_ahb» в регистре INTERRUPT_STATUS (разряд 7).  1'b1 – разрешено. 1'b0 – запрещено. Используется только при пересылке в режиме MDMA, иначе заменяется нулем. 
            volatile unsigned int reserved2  			: 24   ;  //   Зарезервировано.
        };
    }t_INTERRUPT_STATUS_EN;


    typedef union
    {
        volatile unsigned int value;
        struct
        {
            volatile unsigned int buff_wr_rdy_sig_en  	: 1     ;  //  Разрешение установки сигнала прерывания по событию «Buffer Write Ready».  1'b1 – разрешено. 1'b0 – запрещено. 
            volatile unsigned int buff_rd_rdy_sig_en  	: 1     ;  //  Разрешение установки сигнала прерывания по событию «Buffer Read Ready».  1'b1 – разрешено. 1'b0 – запрещено. 
            volatile unsigned int trans_comp_sig_en  	: 1     ;  //  Разрешение установки сигнала прерывания по событию «Transfer Complete».  1'b1 – разрешено. 1'b0 – запрещено. 
            volatile unsigned int mul_bit_err_sig_en  	: 1     ;  //  Разрешение установки сигнала прерывания по событию «Multi Bit Error».  1'b1 – разрешено. 1'b0 – запрещено. Значение данного поля используется при коррекции ошибок по алгоритму Хемминга (HAM), иначе заменяется нулем. 
            volatile unsigned int err_intrpt_sig_en  	: 1     ;  //  Разрешение установки сигнала прерывания по событию «Single Bit Error» в режиме HAM или «BCH Detect Error» в режиме BCH.  1'b1 – разрешено. 1'b0 – запрещено. 
            volatile unsigned int reserved  			: 1     ;  //  Зарезервировано. 
            volatile unsigned int dma_int_sig_en  		: 1     ;  //  Разрешение установки сигнала прерывания по событию «dma_int».  1'b1 – разрешено. 1'b0 – запрещено. Используется только при пересылке в режиме MDMA, иначе заменяется нулём. 
            volatile unsigned int error_ahb_sig_en  	: 1     ;  //  Разрешение установки сигнала прерывания по событию «error_ahb».  1'b1 – разрешено. 1'b0 – запрещено. Используется только при пересылке в режиме MDMA, иначе заменяется нулём. 
            volatile unsigned int reserved2  			: 24    ;  //   Зарезервировано.
        };
    }t_INTERRUPT_SIGNAL_EN;


    typedef union
    {
        volatile unsigned int value;
        struct
        {
            volatile unsigned int buff_wr_rdy_reg  	: 1      ;  //  «Buffer Write Ready». Готовность FIFO к приёму данных от процессора. Устанавливается при условии, что в буфере достаточно места для получения блока данных от процессора. 
            volatile unsigned int buff_rd_rdy_reg  	: 1      ;  //  «Buffer Read Ready». Готовность FIFO к передаче данных процессору. Устанавливается при условии, что считано достаточно данных из флэш-памяти. 
            volatile unsigned int trans_comp_reg  	: 1      ;  //  «Transfer Complete». Пересылка завершена. Устанавливается после завершения выполнения заданной команды. 
            volatile unsigned int mul_bit_err_reg  	: 1      ;  //  «Multi Bit Error». Устанавливается при условии, что подтверждается ошибка в нескольких битах. Используется только в режиме HAM, иначе считается нулём. 
            volatile unsigned int err_intrpt_reg  	: 1      ;  //  «Single Bit Error»/«BCH Detect Error». Устанавливается при условии, что обнаруживается ошибка BCH или одиночная ошибка в HAM режиме. 
            volatile unsigned int reserved  		: 1      ;  //  Зарезервировано. 
            volatile unsigned int dma_int_reg  		: 1      ;  //  «dma_int». Установка бита происходит при достижении границы буфера в режиме MDMA. Используется только при пересылке в режиме MDMA, иначе заменяется нулём. 
            volatile unsigned int error_ahb_reg  	: 1      ;  //   «error ahb». Устанавливается при выполнении пересылки данных на системной шине в режиме MDMA, если получен отклик с признаком ошибки. Используется только при пересылке в режиме MDMA, иначе заменяется нулём. 
            volatile unsigned int reserved2  		: 24     ;  //   Зарезервировано.
        };
    }t_INTERRUPT_STATUS;


    typedef union
    {
        volatile unsigned int value;
        struct
        {
            volatile unsigned int tccs_time  		: 2	;   //  	0  tccs (время установки после смены столбца, см. ONFI). 2'b00 - 500 ns 2'b01 - 100 ns 2'b10 - 200 ns 2'b11 - 300 ns Поле tccs time настраивается на основе значений байтов [139:140] страницы параметров. 
            volatile unsigned int slow_fast_tcad  	: 1	;   //  	0  tcad (Command, Address, Data delay, см. ONFI). 0 – медленное устройство (tCADs). 1 – быстрое устройство (tCADf). Поле slow_fast_tcad настраивается на основе значения байта [143] страницы параметров. 
            volatile unsigned int s_buff_sel  		: 4	;   //  	6  Задержка двунаправленного сигнал dqs. Входной и выходной буфера двунаправленного сигнала dqs имеют настраиваемую задержку. Поле dqs_buff_sel 
															//	позволяет изменять время задержки с шагом в 370-480 пикосекунд. Для вычисления задержки сигнала dqs необходимо умножить значение поля dqs_buff_sel на шаг задержки. 
															//	Для операций чтения необходимо настроить значение данного поля, основываясь на значении tDQSQ (см. ONFI).  Для операций записи необходимо настроить значение данного поля, основываясь на значении tDS (см. ONFI). 
															//	Рекомендуемые значения dqs_buff_sel для операции чтения данных: 4’h6 — синхронный режим 0 4’h4 — синхронный режим 1 4’h3 — синхронный режим 2 и 3 
															//	4’h2 — синхронный режим 4 и 5 Рекомендуемые значения dqs_buff_sel для операции записи данных: 4’h6 — синхронный режим 0 и 1 4’h2 — синхронный режим 2, 3, 4 4’h1 — синхронный режим 5 
            volatile unsigned int tadl_time  		: 8	 ;  //  	0  tadl (см. ONFI). Время между циклом адреса и загрузкой данных. Данное поля должно быть запрограммировано при операции программирования флэш-памяти, 
															//	если установлен бит pgm_pg_reg_clr. Значения поля tadl_time определяется значениями байтов [154:155] страницы параметров. 
            volatile unsigned int reserved  		: 17 ;  //  	Зарезервировано. 
        };
    }t_TIMING;

    typedef union
    {
        volatile unsigned int value;
        struct
        {                                           //  4096 + 224 - 52(34h)) = 10ACh
            volatile unsigned int ecc_addr  : 16 ;  //  	’h104c  Адрес столбца (Column Address) страницы флэш-памяти, определяющий начальное местоположение кодов коррекции. Используется для формирования адреса ECC с командами Change Read Column, Change Write Column. Рекомендуется выбирать значение ecc_addr = *размер страницы в байтах* + *размер spare-области* - ecc_size. 
            volatile unsigned int ecc_size  : 9	 ;  //  	’h34  Задаёт количество транзакций записи или чтения для области памяти с данными ECC. 
            volatile unsigned int ham_bch   : 1	 ;  //  	’b1  Выбор способа коррекции. 1’b0 - HAM 1’b1 - BCH 
            volatile unsigned int reserved  : 6	 ;  //  	’h0  Зарезервировано. 
        };
    }t_ECC;


    typedef union
    {
        volatile unsigned int value;
        struct
        {                                           //  4096 + 224 - 52(34h)) = 10ACh
            volatile unsigned int packet_bound_err_count  : 8   ;  //  	Счётчик ошибок при чтении пакета данных. Процессор должен считывать значение данного регистра каждый раз после чтения данных размером packet_size. 
            volatile unsigned int page_bound_err_count    : 8   ;  //  	Общий счётчик ошибок при чтении страницы. Процессор должен считывать значение данного регистра после прочтения каждой страницы. 
            volatile unsigned int reserved  			  : 16  ;  //  	Зарезервировано 
        };
    }t_ECC_ERROR_CNT;


    typedef union
    {
        volatile unsigned int value;
        struct
        {                                           //  4096 + 224 - 52(34h)) = 10ACh
            volatile unsigned int ecc_spare_cmd  				: 16   ;  //  Используется для программирования кода операций ECC, которые выполняются совместно с командами Change Write Column, Change Read Column. 
            volatile unsigned int reserved  					: 12   ;  //  Зарезервировано  
            volatile unsigned int ecc_spare_num_addr_cycles   	: 3    ;  //  Число циклов адреса в команде ECC для флэш-памяти. 3'b000 – Не используется 3'b001 – Один цикл ... 3'b111 — Семь циклов 
            volatile unsigned int reserved2  					: 1    ;  //  Зарезервировано
        };
    }t_ECC_SPARE_CMD;

    typedef union
    {
        volatile unsigned int value;
        struct
        {
            volatile unsigned int sdma_tx_rx      : 1   ;  //  Направление передачи данных (Запись/Чтение). 
														   //  0 - Данные считываются из системной памяти во 
														   //  внутреннее TX FIFO, а затем записываются во флэш-память контроллером Slave DMA. 
														   //  1 - Данные считываются из флэш-памяти во внутреннее RX FIFO, а затем записываются в системную память контроллером Slave DMA. 
            volatile unsigned int transfer_count  : 20  ;  //  Общий счётчик операций DMA. Значение счётчика декрементируется после каждой передачи данных размером в одно двойное слово 
														   //  (DWORD). Slave DMA инициирует передачу данных при условии, что значение данного поля не равно нулю. 
														   //  20'h00001 - 1 двойное слово 20'h00002 - 2 двойных слова ... 20'hFFFFF - 1048575 двойных слов 
            volatile unsigned int burst_size  	  :	3   ;  //  Размер пакета данных Slave DMA. Поле задаёт количество данных в пакете Slave DMA. 
														   //  3’b000 - 4 двойных слова 3’b001 - 8 двойных слов 3’b010 - 16 двойных слов 3’b011 - 32 двойных слова 3’b100-3’b111- Зарезервировано 
														   //  Если размер данных, содержащихся в FIFO меньше, чем установленный размер пакета , контроллер установит запрос на единичную передачу. 
            volatile unsigned int timeout_value   :	4   ;  //  ’b0  Значение счётчика ожидания DMA  4'b0000 - 2^13 тактов 4'b0001 - 2^14 тактов ... 4'b1110 - 2^27 тактов 
														   //  Данное значение определяет максимальное время ожидания данных. Логика подсчёта превышения времени ожидания используется только при приёме данных.  
														   //  Счётчик начинает отсчёт, когда одно двойное слово находится в RX FIFO, и пороговый уровень не достигнут. 
														   //  Сбрасывается в ноль, когда достигнуто пороговое значение, или счётчик переполняется 
            volatile unsigned int sdma_enable  	  :	1   ;  //  ’b0  Разрешение режима Slave DMA. 0 – режим запрещён. 1 – режим разрешён. 
            volatile unsigned int reserved  	  :	3   ;  //  ’b0  Зарезервировано. 
        };
    }t_SLAVE_DMA_CFG;




typedef struct {
		t_PACKET 			  PACKET; 			// 0x0  RW  Регистр настройки пакетов 
		t_MEMADDR1 			  MEMADDR1; 		//  0x4  RW  Регистр адреса 1 
		t_MEMADDR2 			  MEMADDR2; 		//  0x8  RW  Регистр адреса 2 
		t_COMMAND  			  COMMAND; 			//  0xC  RW  Регистр команды 
		t_PROGRAM 			  PROGRAM; 			//  0x10  RW  Регистр программы 
		t_INTERRUPT_STATUS_EN INTERRUPT_STATUS_EN; //  0x14  RW  Регистр разрешения статуса прерываний 
		t_INTERRUPT_SIGNAL_EN INTERRUPT_SIGNAL_EN; //  0x18  RW  Регистр разрешения сигнала прерываний 
		t_INTERRUPT_STATUS 	  INTERRUPT_STATUS; //  0x1C  RW  Регистр статуса прерываний 
		volatile unsigned int ID1; 				//  0x20  RO  Регистр ID1 
		volatile unsigned int ID2; 				//  0x24  RO  Регистр ID2 
		volatile unsigned int FLASH_STATUS; 	//  0x28  RO  Регистр статуса флэш-памяти 
		t_TIMING 			  TIMING; 			//  0x2C  RW  Регистр временных параметров 
		volatile unsigned int BUFFER_DATA; 		//  0x30  RW  Регистр данных буфера 
		t_ECC 				  ECC; 				//  0x34  RW  Регистр ECC 
		t_ECC_ERROR_CNT 	  ECC_ERROR_CNT; 	//  0x38  RO  Счётчик ошибок ECC 
		t_ECC_SPARE_CMD 	  ECC_SPARE_CMD; 	//  0x3c  RW  Регистр команд ECC  
		volatile unsigned int ERROR_CNT_1BIT; 	//  0x40  RW  Счётчик 1-битных ошибок  
		volatile unsigned int ERROR_CNT_2BIT; 	//  0x44  RW  Счётчик 2-битных ошибок  
		volatile unsigned int ERROR_CNT_3BIT; 	//  0x48  RW  Счётчик 3-битных ошибок  
		volatile unsigned int ERROR_CNT_4BIT; 	//  0x4C  RW  Счётчик 4-битных ошибок  
		volatile unsigned int DMA_SYS_ADDR; 	//  0x50  RW  Системный адрес DMA 
		volatile unsigned int DMA_BUFFER_BOUNDARY; //  0x54  RW  Граница DMA пересылки 
		volatile unsigned int CPU_STATE; 		//  0x58  RW  Регистр состояния CPU 
		volatile unsigned int ERROR_COUNT_5BIT; //  0x5c  RW  Счётчик 5-битных ошибок  
		volatile unsigned int ERROR_COUNT_6BIT; //  0x60  RW  Счётчик 6-битных ошибок  
		volatile unsigned int ERROR_COUNT_7BIT; //  0x64  RW  Счётчик 7-битных ошибок  
		volatile unsigned int ERROR_COUNT_8BIT; //  0x68  RW  Счётчик 8-битных ошибок  
		volatile unsigned int RESERVED0[4]; 	//  0x6c-0x7c  -  Зарезервировано 
		t_SLAVE_DMA_CFG 	  SLAVE_DMA_CFG; 	//  0x80  RW  Регистр конфигурации Slave DMA 
		volatile unsigned int RESERVED1; 		//  0x84 - 0xFF -  Зарезервировано 
} nand_control_regs;

//17.5  Настройка тактирования NANDMPORT 
//Для работы NANDMPORT'а необходимо настроить следующие частоты: 
//  NFC_SCLK – рабочая частота Nand Flash Интерфейса; 
//  NFC_HLCK – системная частота. 

typedef volatile  nand_control_regs    *NandRegsOvly;
extern  NandRegsOvly     NandRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H */
