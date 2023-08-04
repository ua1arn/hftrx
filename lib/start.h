#ifndef __START_H
#define __START_H
#include "structs.h"

extern int BINARY_PORT;
extern int NMEA_PORT;

//-------------------------------------------------------
#define fLog10 (double)2.30258509299404568401799145468436
#ifndef PI
#define PI     (double)3.1415926535897932384626433832795
#endif

#define FRIQ                 100     /* KHz */
#define DISKR_FRIQ           4*FRIQ  /* KHz */
#define MAX_NAKOPL           2500    /* максимальный размер буфера накопления в поиске (для периода следования 0.02 сек)*/
#define CHISLO_NAKOPL        16      /* кол-во накапливаемых двойных периодов в поиске (должно быть степенью 2-ух) */
#define OGR_NAKOPL           0x07FF  /* максимальное ограничение сигнала при накоплении в поиске (0x7FFF >> log(CHISLO_NAKOPL)) */


#define N1                   8       /* число импульсов в пачке */
#define SHIFT_N1             3       /* log(N1) */
#define IMP_PERIOD           400     /* интервал между импульсами в пачке (отсчётов)*/
//#define MRK_PERIOD           480     /* интервал между маркирующими импульсами в пачке (отсчётов)*/
#define MRK_OFFSET           ((int)3280)    /* интервал до маркирующих импульсов в пачке от SPoint  (400*(N1-1)+ MRK_PERIOD*/
#define MRK_IMPULSE          4       /* количество маркирующих импульсов*/
#define SHAG_NAKOPL          40      /* шаг накопления в поиске (отсчётов)*/
//#define DMA_SIZE             128     /* размер буфера DMA*/
#define IMP_CODE_SIZE        16      /* размер кода станции (16 импульсов на двух периодах)*/

/* интервал рассчёта бокового лепестка */
#define INTERVAL_BL          2*DISKR_FRIQ/SHAG_NAKOPL /* 2 мс */
/* диапазон рассчёта бокового лепестка */
#define RANGE_BL             6*DISKR_FRIQ/SHAG_NAKOPL /* 6 мс */

#define MASTER_CODE          (unsigned short)0xCA9F   /* код ведущей */
#define SLAVE_CODE           (unsigned short)0xF9AC   /* код ведомой */

/* режимы работы */
#define REJIM_COLIBRATE      5         /* режим Колибровки входного тракта и настройки фильтров */
#define REJIM_SKO            4         /* режим оценки СКО шума */
#define REJIM_NAC            0         /* режим начального накопления */
#define REJIM_POISK          1         /* режим грубого поиска */
#define REJIM_SYNHRO         2         /* режим синхронизации и слежения */
#define REJIM_TEST           6         /* режим проверки ару */
#define REJIM_NONE           7         /* режим блокировки обработки */
#define REJIM_WAIT4START     8         /* режим ожидания старта */

#define NAKOPL_POISK_STN_SIZE 300   // накопление поиска станции 400-DMA_SIZE/2    //150
                                    //NAKOPL_POISK_STN_SIZE*SHAG_NAKOPL_STN*2.5 = мкс
#define SHAG_NAKOPL_STN       200         // шаг накопления при поиске отдельной станции  100

#define SYNH_SIZE             200 /* размер буфера накопления в слежении */

#define FILTER_CHANNELS2 2

typedef struct {
        short  SinArr[MAX_NAKOPL];    /* массив накопленных синфазных отсчётов */
        short  CosArr[MAX_NAKOPL];    /* массив накопленных квадратурных отсчётов */
}m_Poisk;

typedef struct {
        SINT32 KArr[SYNH_SIZE];      /* буфер накопления станции */
        short  SArr[FILTER_CHANNELS2][SYNH_SIZE];      /* буфер накопления для компенсации станции */
}m_s;
typedef struct {// массив для накопления импулса станции
        SINT32 ASC[NAKOPL_POISK_STN_SIZE];
}m_a;

typedef struct{
                float          A5;          /* ECM  */
                UINT32  	   n;
                unsigned short k;
                unsigned short nac;
}ECM;

typedef         union
                {
                        m_s S;
                        m_a A;
                }Ar_;

typedef struct {
                short  		   chain_id;    /* идентификатор цепи - кому принадлежит данная станция.*/
                short  		   station_num; /* номер станции цепи. 0-5*/
                char           SubRej;      /* подрежим станции */
                unsigned short code;        /* бинарный код станции */
                unsigned short TCnt;        /* счётчик циклов накопления */
                unsigned short TMax;        /* количество накоплений */
                short          Step;        /* количество пройденых шагов в поиске */
                short          SMax;        /* максимальное количество шагов в поиске */
                SINT32         SPoint;      /* номер отсчёта с которого начинается накопление*/
                SINT32         SPoint_old;      /* номер отсчёта с которого начинается накопление*/
                SINT32         Koeff;       /* коэфициент масштабирования амплитуды */
                unsigned short ACnt;        /* счетчик проходов слежения за амплитудой */
                unsigned short AAlfa;       /* коэфициент фильтра амплитуды */
                unsigned short FrontDinamicPorog;       				
                unsigned char  AReady;      /* признак входа в слежение уровня амплитуды */
                unsigned char  AReady2;     /* признак входа в слежение уровня амплитуды */
                SINT32         Phase;       /* отслеживаемая фаза */
                SINT32         PSpeed;      /* отслеживаемая скорость фазы */
                unsigned short SS1PAlfa;    /* коэфициент фильтра фазы SS1 */
                unsigned short SS1PAlfaInit;/* коэфициент фильтра фазы SS1 */
                unsigned short PAlfa;       /* коэфициент фильтра фазы */
                unsigned short PBeta;       /* коэфициент фильтра фазы */
                unsigned short PCnt;        /* счетчик проходов слежения за фазой */
                unsigned short PReady;      /* признак входа в слежение за фазой */
                short          SSF2Point;   /* точка слежения за фазой2 */
                SINT32         Phase2;      /* отслеживаемая фаза2 */
                SINT32         PSpeed2;     /* отслеживаемая скорость фазы2 */
                SINT32         initPSpeed2; /* начальная скорость фазы2 */
                unsigned short PCnt2;       /* счетчик проходов слежения за фазой2 */
                unsigned short PReady2;     /* признак входа в слежение за фазой2 */
				
                char           PhaseReady;  /* признак готовности измерения текущего периода */

                unsigned short RejCnt;      /* счётчик режима */
                unsigned short FailCnt;     /* счётчик обломов С/Ш при накоплении */
                unsigned short LevelFailCnt;/* счётчик обломов уровня станции при накоплении */
                unsigned short CorrPCnt;    /* счётчик правильно накопленных фаз */
                short          ReadyRM;     /* признак готовности РМ */
                SINT32         PorogRM;     /* порог РМ Q16.15 */
                SINT32         A1;          /* рез-ты формирования характерной точки огибающей */
                SINT32         A2;          /* рез-ты формирования характерной точки огибающей */
                SINT32         r1, r2;
                SINT32         A4, A4s;          /* рез-ты формирования характерной точки огибающей */
                ECM            A5;          /* ECM  */
                short          ht4;          /* Коэффициенты для формирования характерной точки огибающей */
                short          ht5;          /* Коэффициенты для формирования характерной точки огибающей */
                SINT32         LastPoint;   /* точка последнего накопленного отсчёта */
                short          SigNoiseCnt; /* счётчик срабатывания порога с/ш */
                SINT32         LastCorVal;  /* последняя величена корректирования РМ*/
                short          CorPTCnt;    /* счётчик накопленных периодов для вычисления порога РМ */
                SINT32         HTO2Arr[10];  /* массив накопленных отсчётов для ФХТО в максимуме сигнала */
                SINT32         AmpArr[6];   /* массив отсчётов в максимуме сигнала для слежения за амплитудой */
                short          TNakMax;     /* кол-во накоплений для поска - коррккции максимума*/
                unsigned short CompReady;   /* готовность буфера накопления для компенсации станции */
                unsigned short CompCnt;     /* количество накопления для компенсации станции */
                short          NakShift;    /* сдвиг накопленного сигнала относительно точки накопления */
                char           ClearStation;/*Признак необходимости очистить станцию*/  
				unsigned char  Lock_IVP;
				unsigned char  Lock_IMP;
				unsigned char  Lock_1Min;
				unsigned char  Lock_5Min;
				char  		   simple_snr;
				char 		   rezerv;
				//
                short          DelayNacoplenie;/*Сдвиг накопления при решении положения станции*/
                /*Расчет скп*/
                short          skpPhase;    /* ошибка фазы */
                float          skpNum;      /* номер фильтрованного скп*/
                float          skpNac;      /* накопленныая скп*/
                /*Данные по сигналу*/
                float          fSNR;        /* отношение сигнал/шум */
                SINT32         lLevel;      /* уровень сигнала */
                /*проверка станции*/
                SINT32         lRMNak;      /* Накопление в точке РМ - слежение за присутствием сигнала*/
                short          sRMNakNum;   /* номер Накопления в точке РМ - слежение за присутствием сигнала*/
                double	       OneHz_offset;
                short 		   OneHz_offset_St;
                short		   imp_8_sec;
                short		   imp_8_hug;
                short		   imp_8_min;
                unsigned short Status;
                //Синхронизация времени
                short 		   Mark_Imp_Data[16];
                int   		   Mark_Imp_Phase[4];//паралельно по трём возможным импульсам
                int   		   Mark_Imp_Power[4];//паралельно по трём возможным импульсам
                int   		   Main_Imp_Phase;//среднее по 8-ми
                int   		   Main_Imp_Power;//среднее по 8-ми
                int  		   TimeSyncStatus;
                unsigned int   o1, o2, o3, o4, o5, o5cnt, summ, ImpCnt, ImpCnt2, ImpCnt3, E_interval,  B_Fix, Live2HzCnt;
                int 		   ImpMarkCnt;
                unsigned int   rns_e_5min_cnt;
                unsigned int   catch_time_next_imp;
                int   			rns_e_time_offset_5min;
                int   			rns_e_time_offset_1min;
                int   			rns_e_time_offset_1hz;

                char 		   szSysInfo[32];
                int   		   Scan1[50];
                int			   Scan2[30*11];
                unsigned int   uiSysInfoIdx, uiSyncIdx, Scan1idx, Scan2idx, Scan3idx, Scan4idx, Scan2MinIdx, Scan2MinCnt;
                int			   i5MinImpulse, i30BitLock, iImpHLockCnt, iOneMinCnt, i1MinImpulse, i2HzImpulse;
                unsigned int   uiStartCopy, iTestCnt;
                unsigned char  uiLock, uiLockErr;
                unsigned short usTimeTimer[4];	
                short		   iPeriodCopyed;
                unsigned char  uiTimeReady;
                long           PhaseSpeedBySygnal;
                nav_time       ti;
                //
                float 			Ant_Angle;	
                int			   Ant_part;
                //
/*
	Эта структура должна быть скопирована из binary.h : _TimeInfo
*/
				/*union
                {
                        m_s S;
                        m_a A;
                }Ar_;*/
                Ar_            *Ar;
}m_Synhro;

extern  m_Poisk  Search[][CHAINS_NUMBER];
extern  m_Synhro Synh[MAX_STATIONS];
extern  Ar_      Ar_mass[MAX_STATIONS];

typedef struct
{
        short 	T2Cnt;      /* Счётчик двойных периодов */
        short 	T2Lenght;   /* Длительность двойного периода в парах отсчётов*/
        short 	T2Max;      /* Количество накапливаемых периодов */
        short 	SamplCount; /* Счётчик пар отсчётов */
        short 	PorogPoisk; /* ограничение сигнала при накоплении в поиске */
        short 	Noise;
        UINT32  Porog;
}type_Nakopl;

typedef struct{
        char   cn;
        char   test;      /* признак того, что процедура обработки накопленных на периоде отсчётов станции не завершилась */
        char   test_n;    /* признак того, что промежуточный массив накопления в режиме слежения ещё не обработан */
        char   even;      /* чёт/нечет. период повторения */
        //int     offset;
        SINT32  prevSCnt;  /* предыдущий номер накопленного отсчёта в поиске */
        SINT32  SCnt;      /* счётчик отсчётов на периоде */
        SINT32  SCnt_old;      /* счётчик отсчётов на периоде */
        short   NCnt;      /* кол-во накопленных отсчётов станции в промежуточный массив в реж. слежения */
        short   pos_dma;   /* точка в буфере DMA, с которой следует начинать накопление отсчётов импульса станции */
        short   ready_sn;
        short   search;    /* признак отсутствия импульса в текущем буфере DMA */
        short   posD, posN, endD;
        short   work_sn;   // накапливаемая в данный момент станция
        short   last_sn;   // Последняя станция для которой копился сигнал
        //short   last_sn;
        short   work_pos;  // номер импульса накапливаемой станции
        //short   last_pos;  // номер импульса накапливаемой станции
        short   fail;      // был пропуск цикла
        short   overload;  // было переполнение при накоплении пачки импульсов
        short   NoInterfire;
        short   mrk_pos;
        short   mrk_idx;
        short   mrk_get;
        m_Synhro * station;
        char    even_old;      /* чёт/нечет. период повторения */
        long long      llPeriods;
}type_Hlam;

/* пара - величина, индекс */
typedef struct
{
        short  val;
        short index;
}type_pair;

typedef struct
{
        UINT32  val;
        short index;
}type_lpair;


typedef struct
{
        double pos;
        short  got;
} type_Res;

/* управляющая структура цепи (определяет режим работы цепи)*/
union _Command
{
        unsigned int word;
        struct
        {
                unsigned int Rejim : 31;  /* номер режима */
                unsigned int Init  : 1; /* признак необходимости инициализации режима */
        } aword;
};
typedef union _Command type_Command;
extern type_Command Command[CHAINS_NUMBER]; /* управляющие слова */

typedef struct
{
        struct
        {
                int zInitPO  : 1;
                int zPO      : 1;
                int zCalcRes : 1;
                int zCalibrateAru  : 1;
                int zAruOnLine  : 1;
                int OneHz    : 1;
                int ThreeHz  : 1;
                int OneMinute  : 1;
                int CorrectionGet  : 1;
                int NextOneHz  : 1;
				int FRNS_poisk : 1;
				int AruCorrection : 1;
				int AruPreCorrection : 1;
				int FindPartsTreshold : 1;
                int FrnsForPilotAruCorrection  : 1;
                int FrnsAfterPilotAruCorrection  : 1;
				int StartGraphicTransmit : 1;
				int FindPilotLvl : 1;
				int FindPilot	: 1;
                int TenHz : 1;
                int StartFRNSPilot : 1;
                int StopFRNSPilot : 1;
                int AnalizeTime : 1;
                int AnalizeFRNSPhases : 1;
                int ResyncStation : 1;
                int SWReset : 1;
                int vpRememberARU : 1;
                int vpDisablePilot : 1;
                int vpWaitTimeout : 1;
                int vpRestoreARU : 1;
                int vpStartPilot : 1;
                int RestartWork  : 1;
        } Zapros;
        struct
        {
                int oInitPO  : 1;
                int oPO      : 1;
                int obt2  : 1;
                int obt3  : 1;
                int obt4  : 1;
                int obt5  : 1;
                int obt6  : 1;
                int obt7  : 1;
                int obt8  : 1;
                int obt9  : 1;
                int obt10 : 1;
                int obt11 : 1;
                int obt12 : 1;
                int obt13 : 1;
                int obt14 : 1;
                int obt15 : 1;
        } Otvet;
        unsigned short tim0;
        unsigned short timPO;
        unsigned short tim2;
        unsigned short tim3;
        unsigned short tim4;
        unsigned short tim5;
        unsigned short tim6;
        unsigned short tim7;
        unsigned short tim8;
        unsigned short tim9;
        unsigned short tim10;
        unsigned short tim11;
        unsigned short tim12;
        unsigned short tim13;
        unsigned short tim14;
        unsigned short tim15;
} type_DispDWord;


typedef struct{
       volatile short  bLockNacopl;
       volatile short  bBufferFilled;
       volatile SINT32 dCnt;
       volatile SINT32 start;
}Mode_Zero;

extern short sCalibration;
extern Mode_Zero mode_zero;
extern type_DispDWord DispDWord;/* 2 слова с запросами и ответами диспетчера */
//extern SINT32 Acum[CHAINS_NUMBER][SYNH_SIZE];/* массив для накопления пачки (8) импульсов станции на одном периоде*/
//extern SINT32 AcumComp[CHAINS_NUMBER][SYNH_SIZE];/* массив для накопления пачки (8) импульсов станции на одном периоде*/
//extern short  tAcumComp[CHAINS_NUMBER][SYNH_SIZE];/* массив для накопления (1) импульсов станции на одном периоде*/
extern SINT32 SAMPLES_PER_PERIOD[CHAINS_NUMBER];/* кол-во отсчётов на период для каждой цепи*/
extern type_Hlam Help_cnt[][CHAINS_NUMBER];
extern char  str[];
//extern short Debug[];
extern UINT32    ulSys1;     /* Кол-во пропусков из-за неразблокировки буффера*/
extern UINT32    ulSys2;     /* Кол-во пропусков из-за невыхода из процедуры*/
extern UINT32    ulSys3;     /* Кол-во пропусков*/
extern short     sSetBreak;
extern UINT32    ErrorStatus;
extern volatile short  ActiveChainNumber; 
//extern void Nakopl_DMA_IFRNS(short *DMABuff);
extern void InitPO_IFRNS(void);
extern void ifrns_task(void);
extern void InitCnt(short cn);
void IndNormWork(void);
void IndControl(void);
void IndSearch(register int i);
void IndFail(void);

#endif
