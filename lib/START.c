#include "modules.h"

#ifdef PLATFORM_ARM_A9
#include "hardware.h"
#include "./../gpio.h"
#endif

#include "SWItypedef.h"
#ifdef PLATFORM_TMS6424
#include <cslr_edma3cc.h>
#include <cslr_edma3tc.h>
#include "global_regs.h"
#endif

#include "Structs.h"
#include <stdio.h>
#include <string.h>
#include "start.h"
#include "poisk.h"
#include "synhro.h"
#include "synhro_fn.h"
#include "macros.h"
#include "sko.h"
#include "filter.h"
#include "timer.h"
#include "aru.h"
#include "i2c.h"
#include "Secondary.h"
#include "FiltrCoeff.h"
#include "uart.h"
#include "serial_parse.h"
#include "config.h"
#include <math.h>
#include "vectorezed.h"
#include "info.h"
#include "Nacoplenie.h"
#include "rtcm.h"
#include "crc16.h"
#include "Results.h"
//#include "InitProcessor.h"
#include "FlashBlock.h"
#include "ChainControl.h"
#include "DSP_iir51.h"
#include "frns_coef.h"
#include "frns.h"
#include "binary.h"
#include "aru_frns.h"
#include "FRNS_impulse_filtering.h"
#include "GPIO.h"
#include "SerialFrnsSendData.h"
#include "modules.h"
#include "TimeSync.h"
#include "skpd.h"
#include "ZSmod.h"

#ifdef PLATFORM_TMS6424
extern CSL_Edma3ccRegsOvly Edma3ccRegsOvly;
#endif

extern void 	popraw(double fs, double ls, int y, int m1, int d, double t, int NF);
extern void 	prbank(int Freq);
void AnalizeFreqData2Coorsd(void);
/*
void Ex2(void)
{
  if ((TSR&1)==0)
  {
       asm("           NOP 5    ");
  }
}
*/
//#pragma DATA_SECTION(DMABuff, ".dma_buff")
void SynthInit1(void);

//#pragma DATA_SECTION(SAMPLES_PER_PERIOD, ".samples")
SINT32 SAMPLES_PER_PERIOD[CHAINS_NUMBER];/* кол-во отсчётов на период для каждой цепи*/

//#pragma DATA_SECTION(Command, ".samples")
#ifdef PLATFORM_TMS6424
#pragma DATA_ALIGN(4);
#endif
type_Command Command[CHAINS_NUMBER];/* управляющие слова (определяют режимы работы цепеи)*/

//#pragma DATA_SECTION(DispDWord, ".samples")
type_DispDWord DispDWord;/* 2 слова с запросами и ответами диспетчера */

//#pragma DATA_SECTION(Koeff, ".band_koeff")
//#pragma DATA_SECTION(Koeff_Low, ".band_koeff")
//#pragma DATA_SECTION(Debug, ".samples")
//short Debug[MCBSP1_IFRNS_BUFF_LEN];
//#pragma DATA_SECTION(LFilter, ".samples")
//#pragma DATA_SECTION(TmpBuff, ".samples")
//--------------------------
UINT32 ErrorStatus=0;     /* Статус-состояние канала*/
UINT32 ErrorFailCnt=0;     /* Статус-состояние канала*/
/* хлам */
UINT32    ulSys1=0;     /* Кол-во пропусков */
UINT32    ulSys2=0;     /* Кол-во переполнений*/
UINT32    ulSys3=0;     /* not used */
short 	  sSetBreak;             /* Активирование заданного по номеру бряка */
Mode_Zero mode_zero;
extern /*far*/ unsigned int iMainFreqFailDetected;
extern /*far*/ unsigned int uiDMAIFRNSInterruptCounter;
extern /*far*/ short   Koeff[];
/*
вычисляется диапазон отсчётов в буфере ДМА, который будет компенсироваться (posD[cn] и endD[cn]),
а так же точку posN[cn] в массиве компенсации, которая соответствует posD[cn], при чём
она определяется счётчиком NCnt[cn], который является общим для накопления и компенсации.
*/
#ifdef PLATFORM_TMS6424
#pragma DATA_SECTION(".myfastdata")
type_Hlam Help_cnt[FILTER_CHANNELS][CHAINS_NUMBER];
#else
type_Hlam Help_cnt[FILTER_CHANNELS][CHAINS_NUMBER] __attribute__ ((section (".VRam_data"))) ;//((section (".CPU_Internal")));
#endif
//
unsigned short bFirstStageDone;//, ResetDataCnt;
//

void IndNormWork(void)
{
   GP13_ONm();   //0 RED
   GP14_OFFm();  //1 GREEN
}
void IndControl(void)
{
   GP13_ONm();  //0 RED  
   GP14_ONm();  //0 GREEN
}
void IndSearch(register int i)
{
   GP13_ONm();  //0 RED  
   if(i==1)
   {
     GP14_OFFm(); //1 GREEN мерцает
   }
   else
   {
     GP14_ONm(); //1 GREEN мерцает
   }
}
void IndFail(void)
{
   GP13_OFFm();  //1 RED  
   GP14_ONm();   //0 GREEN
}

/* инициализация хлама */
void InitHlam(void)
{
  short cn;
  register type_Hlam * hlp;

  hlp = (type_Hlam *)&(Help_cnt[0]);
  memset(hlp, 0, sizeof(type_Hlam)*CHAINS_NUMBER);
  for(cn = 0; cn < CHAINS_NUMBER; cn++)
  {
     //if(m_Config.chain_id[cn]==0) continue;
     hlp->prevSCnt = -SHAG_NAKOPL;
     hlp->search = 1; 
     hlp->ready_sn = -1;
     hlp->last_sn = -1;
     hlp->cn = -100;
     //hlp->last_pos=-1;
     hlp++;
  }
  //pDelayBuff  = &DelayBuff[0];
  //pLDelayBuff = &LDelayBuff[0];
  bFirstStageDone=0;//ResetDataCnt=0;
  memset(&PhaseControl, 0, sizeof(type_PhaseControl));
  memset(&mode_zero, 0, sizeof(Mode_Zero));
}

void InitCnt(short cn)
{
  register type_Hlam * hlp;
  hlp = (type_Hlam *)&(Help_cnt[cn]);
  hlp->cn=-100;
}

void Resync(void)
{
	InitHlam();
     memset(&Search, 0, sizeof(m_Poisk)*CHAINS_NUMBER);
     memset(&PoiskRes, 0, sizeof(type_PoiskRes)*CHAINS_NUMBER);
     memset(&Synh,0,sizeof(m_Synhro)*MAX_STATIONS);
     memset(Ar_mass, 0, sizeof(Ar_)*MAX_STATIONS);
     for(int cn=0;cn<MAX_STATIONS;cn++)
     {
        Synh[cn].Ar=&Ar_mass[cn];
     }
     for(int cn=0;cn<CHAINS_NUMBER;cn++)
     {
          	for(int i=0;i<FILTER_CHANNELS;i++)
          	{
       			Nakopl[i][cn].SamplCount = 0;
       		}	
        //Nakopl[cn].SamplCount = 0;
     }
}

void InitPO1(void)
{
UINT16 cn;
short * coeffPtr2;
     PreInitSynhro();
     InitSecondaryVars();
     for(cn = 0; cn < CHAINS_NUMBER; cn++)
     {
       Command[cn].word = REJIM_NAC;
       InitStruct(cn);
     }
     InitFlash();
//       asm("           DINT    ");
     sCalibration=ARU_CAL_MIDDLE_POINT;
     for(int k=0; k < FILTER_CHANNELS; k++) sADCvalue[k]=0;
     memset(&SearchMode, 0, sizeof(type_Mode));
     memset(&SAMPLES_PER_PERIOD, 0, sizeof(int)*CHAINS_NUMBER);
     memset(&Search, 0, sizeof(m_Poisk)*CHAINS_NUMBER*FILTER_CHANNELS);
     memset(&DispDWord, 0, sizeof(DispDWord));
     //memset(DelayBuff, 0, sizeof(DelayBuff));
     memset(LDelayBuff, 0, 2*DL_SIZE*sizeof(short)*FILTER_CHANNELS);
     memset(&PoiskRes, 0, sizeof(type_PoiskRes)*CHAINS_NUMBER);
     memset(&Nakopl, 0, sizeof(type_Nakopl)*CHAINS_NUMBER*FILTER_CHANNELS);
     memset(&Synh,0,sizeof(m_Synhro)*MAX_STATIONS);
     /*
     coeffPtr2=&(Koeff_Low[0]);
     *coeffPtr2++ = (short)BL00;
     *coeffPtr2++ = (short)BL01;
     *coeffPtr2++ = (short)BL02;
     *coeffPtr2++ = (short)AL01;
     *coeffPtr2++ = (short)AL02;
     *coeffPtr2++ = (short)0;
     *coeffPtr2++ = (short)0;
     *coeffPtr2++ = (short)0;
     coeffPtr2=&(Koeff_Low[BQ_SIZE]);
     *coeffPtr2++ = (short)BL10;
     *coeffPtr2++ = (short)BL11;
     *coeffPtr2++ = (short)BL12;
     *coeffPtr2++ = (short)AL11;
     *coeffPtr2++ = (short)AL12;
     *coeffPtr2++ = (short)0;
     *coeffPtr2++ = (short)0;
     *coeffPtr2++ = (short)0;
     */
     //http://www.earlevel.com/main/2013/10/13/biquad-calculator-v2/
     //широкополосная ------------------------------------------------------------
     coeffPtr2=&(Koeff[0]);
     *coeffPtr2++ = (short)(LPa0*32767.0);
     *coeffPtr2++ = (short)(LPa1*32767.0);
     *coeffPtr2++ = (short)(LPa2*32767.0);
     *coeffPtr2++ = (short)(LPb1*32767.0);
     *coeffPtr2++ = (short)(LPb2*32767.0);
#if BQ_SIZE == 8
     *coeffPtr2++ = (short)0;
     *coeffPtr2++ = (short)0;
#endif
     *coeffPtr2   = (short)0;
     coeffPtr2=&(Koeff[BQ_SIZE]);
     *coeffPtr2++ = (short)(HPa0*32767.0);
     *coeffPtr2++ = (short)(HPa1*32767.0);
     *coeffPtr2++ = (short)(HPa2*32767.0);
     *coeffPtr2++ = (short)(HPb1*32767.0);
     *coeffPtr2++ = (short)(HPb2*32767.0);
#if BQ_SIZE == 8
     *coeffPtr2++ = (short)0;
     *coeffPtr2++ = (short)0;
#endif
     *coeffPtr2   = (short)0;

/*
     coeffPtr2=&(Koeff[0]);
     *coeffPtr2++ = (short)(B01);
     *coeffPtr2++ = (short)(B11);
     *coeffPtr2++ = (short)(B21);
     *coeffPtr2++ = (short)(A11);
     *coeffPtr2++ = (short)(A21);
     *coeffPtr2++ = (short)0;
     *coeffPtr2++ = (short)0;
     *coeffPtr2++ = (short)0;
     coeffPtr2=&(Koeff[BQ_SIZE]);
     *coeffPtr2++ = (short)(B02);
     *coeffPtr2++ = (short)(B12);
     *coeffPtr2++ = (short)(B22);
     *coeffPtr2++ = (short)(A12);
     *coeffPtr2++ = (short)(A22);
     *coeffPtr2++ = (short)0;
     *coeffPtr2++ = (short)0;
     *coeffPtr2++ = (short)0;
*/
     //для поиска -----------------------------------------------------------------
     coeffPtr2=&(Koeff_Low[0]);
     *coeffPtr2++ = (short)(a00*32767.0);
     *coeffPtr2++ = (short)(a01*32767.0);
     *coeffPtr2++ = (short)(a02*32767.0);
     *coeffPtr2++ = (short)(b01*32767.0);
     *coeffPtr2++ = (short)(b02*32767.0);
#if BQ_SIZE == 8
     *coeffPtr2++ = (short)0;
     *coeffPtr2++ = (short)0;
#endif
     *coeffPtr2   = (short)0;
     coeffPtr2=&(Koeff_Low[BQ_SIZE]);
     *coeffPtr2++ = (short)(a10*32767.0);
     *coeffPtr2++ = (short)(a11*32767.0);
     *coeffPtr2++ = (short)(a12*32767.0);
     *coeffPtr2++ = (short)(b11*32767.0);
     *coeffPtr2++ = (short)(b12*32767.0);
#if BQ_SIZE == 8
     *coeffPtr2++ = (short)0;
     *coeffPtr2++ = (short)0;
#endif
     *coeffPtr2   = (short)0;

     ulSys1=0;ulSys2=0;ulSys3=0;
     //
     ErrorStatus |= ((unsigned int)1 << PILOT_IFRNS_ERROR);
     //
//       asm("           RINT    ");
     memset(Synh, 0, sizeof(m_Synhro)*MAX_STATIONS);
     memset(Ar_mass, 0, sizeof(Ar_)*MAX_STATIONS);
     for(cn=0;cn<MAX_STATIONS;cn++)
     {
        Synh[cn].Ar=&Ar_mass[cn];
     }
     PreInitSecondary();
     //InitConfig(); //RESET CONFIG !!!!
     LoadConfig();
     SetArtificialChain();
     InitHlam();
     //
     //FindChainL(79500, 4); // Так больше делать нельзя!
     //
     memset(&Result,0,sizeof(Result));
     InitFilter();
     InitAru();
     Init_inertial();
}

void InitPO2(void)
{
     short  cn;
     InitNmeaNakopl();
     //SetMyLevel(200);//01** - аттеюация выкльчена
     //ConfigurePortA(m_Config.portA_speed);ConfigurePortB(m_Config.portB_speed);
#ifdef PLATFORM_TMS6424
       			 asm("           DINT    ");
#elif PLATFORM_ARM_A9
                 global_disableIRQ();
#endif
     for(cn = 0; cn < CHAINS_NUMBER; cn++)
     {
        Command[cn].word = REJIM_NAC;
        // установка режима поиска
        PoiskRes[cn].RejCount = 0;
        Command[cn].aword.Rejim = REJIM_COLIBRATE;
     }
#ifdef PLATFORM_TMS6424
       			 asm("           RINT    ");
#elif PLATFORM_ARM_A9
                 global_enableIRQ();
#else
                 asm ("  RSBX    INTM");
#endif
       bLockUpdate=0;
}

void InitPO_IFRNS(void)
{
    sSetBreak=0;
	InitPO1();
	InitPO2();
#ifdef PLATFORM_ARM_A9
	SetbFilterOn(0);
#endif
    skp_reset_all(-1);
    ShowVersion();
    bStartInitCompleted = 1;
}

void REJIM_PROCESS(void)
{
    short cn, Longest, i, m, bReinitDonde=0;
    register SINT32  k=0, *l;
    Longest=0;
    l=SAMPLES_PER_PERIOD;
    for(cn = 0; cn < CHAINS_NUMBER; cn++)
    {
       if(m_Config.chain_id[cn]!=0)
         if(k<*l){ k=*l; Longest=cn; };
       l++;
    }

    for(cn = 0; cn < CHAINS_NUMBER; cn++)
    {
       if(m_Config.chain_id[cn]==0) continue;
       switch(Command[cn].aword.Rejim)
       {
           case REJIM_WAIT4START:
           {
		     if(Command[cn].aword.Init)
			 {
			   //InitHlam();
			   /*if(bReinitDonde == 0) 
			   {
			   	 Resync();
			   	 bReinitDonde = 1;
			   }*/
			   Command[cn].aword.Init = 0;
			 }
			 break;
		   }
           case REJIM_TEST:
           {
               if(Command[cn].aword.Init) 
			   if(bReinitDonde == 0) 
			   {
                 InitPO1();
			   	 bReinitDonde = 1;
			   }
               Command[cn].aword.Init=0;
               if(sCalibration==RAW_MODE)
               {
                      //ResetCtrl;
                      mode_zero.bLockNacopl=0;
                      sSetBreak=1;
                      do{
#ifdef PLATFORM_TMS6424
                         asm(" NOP 4");
                         asm(" NOP 4");
                         asm(" NOP 4");
#endif
#ifdef PLATFORM_ARM_A9
                         asm(" NOP");
                         asm(" NOP");
                         asm(" NOP");
#endif
                      }while(mode_zero.bBufferFilled==0);
                          /*for(i=0;i<(0x8000/(MCBSP1_IFRNS_BUFF_LEN));i++){
                            //ReadDataA((ARU_MASS_DATA+(SINT32)(i*(MCBSP1_IFRNS_BUFF_LEN))),MCBSP1_IFRNS_BUFF_LEN, Debug);
                            memcpy(ARU_MASS_DATA+(SINT32)(i*(MCBSP1_IFRNS_BUFF_LEN)*sizeof(short)), TmpDMABuff,(MCBSP1_IFRNS_BUFF_LEN)*sizeof(short));
                            OutData((SINT16 *)TmpDMABuff,MCBSP1_IFRNS_BUFF_LEN, 0);
                          } */
                          //SetCtrl;
                          Command[cn].aword.Rejim=REJIM_NONE;
               }
             return;
           }
           case REJIM_SKO:
           {
               if(Command[cn].aword.Init) InitSKO(cn);
			   //Command[cn].aword.Init = 0;
               SKO(cn);
               m=PoiskRes[cn].RejCount;
               if(cn==Longest && m>0 && m%10==0)
               {
                 //Смотрим состояние остальных:
                 m=0;
                  for(i=0;i<CHAINS_NUMBER;i++)
                  {
                    if(m_Config.chain_id[i]==0) continue;
                    if(i!=Longest)
                    {
                      if(Command[i].word != REJIM_SYNHRO) m++;
                    }
                  }
                 if(m==CHAINS_NUMBER-1)
                 {//Нет ни одной рабочей
                    for(m=0;m<CHAINS_NUMBER;m++)
                    {
                     if(m_Config.chain_id[m]==0) continue;
                     Command[m].aword.Rejim = REJIM_COLIBRATE;
                     Command[m].aword.Init=1;
                    }
                 }
               }
               break;
           }
           case REJIM_POISK:
           {
               if(Command[cn].aword.Init) InitPoisk(cn);
               //
               if(bFirstStageDone==0) Poisk(cn);
               //
               break;
               }
           case REJIM_SYNHRO:
           {
               
               if(Command[cn].aword.Init) 
               {
                 InitSynhro(cn);
                 skp_reset_all(cn);
               }
               //
               if(bFirstStageDone==0) Synhro(cn);
               //
               break;
           }
           case REJIM_COLIBRATE:
           {
                    if(Command[cn].aword.Init==1)
                    {
                     sCalibration=ARU_CAL_MIDDLE_POINT;
                     for(m=0;m<CHAINS_NUMBER;m++)
                     {
                      if(m_Config.chain_id[m]==0) continue;
                      Command[m].aword.Init=0;
                     }
                    }
           break;
           }
            default:;
       }
    }
}

void Disp(void)
{
  short cn, i;
  /*if (DispDWord.Zapros.Poisk)
  {
	DispDWord.Zapros.Poisk = 0;
	FRNS_poisk();
  }*/
  if (DispDWord.Zapros.StartFRNSPilot)
  {
      if(IsCounted(PILOTF_WAIT))
      {
       DispDWord.Zapros.StartFRNSPilot=0;
       StartFPilot();
      }
  }
  /*if (DispDWord.Zapros.StopFRNSPilot)
  {
      DispDWord.Zapros.StopFRNSPilot=0;
      StopFPilot();
  }*/
  if (DispDWord.Zapros.AruCorrection)
  {
#ifdef PLATFORM_TMS6424
        if((Edma3ccRegsOvly->SER & (((Uint32)1)<<FRNS_DMA_ARU)) == 0)
#endif
#ifdef PLATFORM_ARM_A9
        	  if(iFRNS_IO_Complete)
#endif
        {
                DispDWord.Zapros.AruCorrection = false;
                //FAruCorrection();
                AruOnlineF(AruMode);
        }
  }
  if (DispDWord.Zapros.AruPreCorrection)
  {
#ifdef PLATFORM_TMS6424
        if((Edma3ccRegsOvly->SER & (((Uint32)1)<<FRNS_DMA_ARU)) == 0)
#endif
#ifdef PLATFORM_ARM_A9
        	  if(iFRNS_IO_Complete)
#endif
        {
                DispDWord.Zapros.AruPreCorrection = false;
                //FAruCorrection();
                AruPreOnlineF(Aru_nak_frns1);
        }
  }  
  if(DispDWord.Zapros.RestartWork)
  {
     DispDWord.Zapros.RestartWork = false;
     if(IsCounted(LOADER_WAIT))
     {
#ifdef PLATFORM_TMS6424
         SetWait(LOADER_WAIT, 200);
         SynthInit1();
         SetWait(PILOT_RM_WAIT, 120000);
#endif
         if(iVirtualPilotTimerSet)
         {
            iVirtualPilotTimerSet = 0;
			ResetPsevdoPilot();
         }
         //Resync();
         Send_To_MCU_cmd(6);
#ifdef PLATFORM_TMS6424
       			 asm("           DINT    ");
#elif PLATFORM_ARM_A9
                 global_disableIRQ();
#endif
         InitHlam();
         for(i = 0; i < CHAINS_NUMBER; i++)
         {
          if(m_Config.chain_id[i]!=0)
		  {
            Command[i].aword.Init = 1;
            // установка режима поиска 
            Command[i].aword.Rejim = REJIM_WAIT4START;
		  }
         }
#ifdef PLATFORM_TMS6424
       			 asm("           RINT    ");
#elif PLATFORM_ARM_A9
                 global_enableIRQ();
#else
                 asm ("  RSBX    INTM");
#endif
     }
  }
  if (DispDWord.Zapros.FindPilot)
  {
	DispDWord.Zapros.FindPilot = false;
	FindPilot();
  }
  if (DispDWord.Zapros.FindPilotLvl)
  {
	DispDWord.Zapros.FindPilotLvl = false;
	FindPilotLvl();
  }
  if (DispDWord.Zapros.FrnsAfterPilotAruCorrection)
  {
    DispDWord.Zapros.FrnsAfterPilotAruCorrection = false;
    /*aru2.adc = AruOptLvl;
    SetLevelF();*/
    StopFPilot();
	//Выключаем децибельник (умножаем на 10)
	if ( aru2.adc&0x0100 )
	{
	    AMP2_OFF();
    }
    //aru2.adc = 0x01FF;
  }
  if (DispDWord.Zapros.FrnsForPilotAruCorrection)
  {
    DispDWord.Zapros.FrnsForPilotAruCorrection = false;
    //AruOptLvl = aru2.adc;
    /*aru2.adc = PILOT_OPT_LVL;
    SetLevelF();*/
	//Включаем децибельник (делим на 10)
    if ( aru2.adc&0x0100 )
	{
	    AMP2_ON();
    }
    //aru2.adc = PILOT_OPT_LVL;
  }
  if (DispDWord.Zapros.StartGraphicTransmit)
  {
    DispDWord.Zapros.StartGraphicTransmit = false;
    SendFrnsData(0);
  }
  if(DispDWord.Zapros.zInitPO)
  {
     DispDWord.Zapros.zInitPO = 0;
     InitPO_IFRNS();
  }
  if(DispDWord.Zapros.zPO)
  {
     DispDWord.Zapros.zPO = 0;
     REJIM_PROCESS();
       do
       {
        i=0;
        for(cn = 0; cn < CHAINS_NUMBER; cn++)
        {
          if(m_Config.chain_id[cn] == 0) continue;
          if(Command[cn].aword.Init == 1) 
          { 
          	REJIM_PROCESS(); 
          	i++; 
          }
        }
       }while(i>0);
  }
  /*for(cn=0;cn<CHAINS_NUMBER;cn++){
   if(m_Config.chain_id[cn]==0) continue;
   if(Command[cn].aword.Rejim == REJIM_TEST || Command[cn].aword.Rejim == REJIM_NONE) return;
  } */
  if(DispDWord.Zapros.zCalibrateAru)
  {
     if(sCalibration==ARU_CAL_DELITEL)
     {
#ifdef PLATFORM_TMS6424
        if((m_Config.iGlobal)&0x4000) AruCalibrate1();
#endif
        SetMyLevel(ARU_BITS-1);SetWait(ARU_TIMER, 20);Wait(ARU_TIMER);
        sCalibration=ARU_CAL_NAC_SEARCH;
     }
     //
     if(sCalibration==ARU_CAL_NAC_SEARCH)
     {
       if(GetbBufferFilled()==TRUE)
        if(AruNacoplenie()) 
        {
           sCalibration=ARU_CAL_SEARCH;
           //DispDWord.Zapros.zCalibrateAru = 1;
        }
     }
     //
     if(sCalibration==ARU_CAL_SEARCH)
     {
        AruCalibrate2();
        sCalibration=ARU_CAL_STABILIZE;
        DispDWord.Zapros.zCalibrateAru = 0;
     }
  }
  //Дальше не выполняем если NONE
  for(cn = 0; cn < CHAINS_NUMBER; cn++)
  {
   if(m_Config.chain_id[cn]==0) continue;
   if(Command[cn].aword.Rejim == REJIM_TEST || Command[cn].aword.Rejim == REJIM_NONE) return;
  }
  if(DispDWord.Zapros.zAruOnLine)
  {
     DispDWord.Zapros.zAruOnLine=0;
     if(sCalibration == ARU_CAL_STABILIZE)
     {
      if(AruPreOnLine() == 1)
      {
         for(cn = 0; cn < CHAINS_NUMBER; cn++)
         {
	   		InitSKO(cn);
	   		PoiskRes[cn].RejCount = 0;
		 }
            //PoiskRes[cn].RejCount = 0;
            sCalibration = ARU_CAL_FINISH;
	  }
      /*   for(cn = 0; cn < CHAINS_NUMBER; cn++)
         {
            if(m_Config.chain_id[cn]==0) continue;
            Command[cn].aword.Init = 1;
            // установка режима поиска 
            Command[cn].aword.Rejim = REJIM_SKO;
         }
		 */
      //}
     }
  }
  if(DispDWord.Zapros.AnalizeTime)
  {
    DispDWord.Zapros.AnalizeTime = 0;
    AdjustTime();
  }
  if(DispDWord.Zapros.OneHz)//0.75 от метки времени
  {
    DispDWord.Zapros.OneHz=0;
    SS_Amp3();
    rtcmAutomat();
  		//
     		PsevdoPilot();
     		InitCheck_on_VirtualData();
  		//    
    //ResetWDT();rtcmNacoplenie();ResetWDT();
  }
  if(DispDWord.Zapros.ThreeHz)
  {
    DispDWord.Zapros.ThreeHz=0;
#ifdef MODULE_241
     if((ErrorStatus&((int)1 << PILOT_IFRNS_ERROR)) != 0)//смотрим по пилоту
     {
       IndSearch(0);
     }
#endif
    SS_Amp2();
    
  }
  if(DispDWord.Zapros.TenHz)
  {
    DispDWord.Zapros.TenHz=0;
    if(sCalibration == ARU_CAL_FINISH) AruOnline();
  }
  //В самый последний момент =)
  if(DispDWord.Zapros.NextOneHz)//Отстоит на пол секунды от метки времени.
  {
    DispDWord.Zapros.NextOneHz=0;
    if(I2C_ERROR == 1)
    {
         ErrorStatus|=(1<<REOSTAT_IFRNS_ERROR);
         ErrorStatus|=(1<<EEPROM_ERROR);
    }
    else
    {
         ErrorStatus&=~(1<<REOSTAT_IFRNS_ERROR);
         ErrorStatus&=~(1<<EEPROM_ERROR);
    }
    //
    if(iMainFreqFailDetected == 1)
    {
         if(iVirtualPilotTimerSet)
         {
            iVirtualPilotTimerSet = 0;
			ResetPsevdoPilot();
         }
         //
         //Если это не делать - то порт может не подняться
#ifdef PLATFORM_TMS6424
         SynthInit1();
#endif
         //
    }
    //ResetWDT();
            register m_Synhro * synh = &Synh[0];
            for(int i=0;i<MAX_STATIONS;i++)       
            {
  				if((synh->ti.TimeMode&(~(1<<15))) != 0)
  				{
  				   for(int m = 0; m < 4; m++)
  				   {
  				       if(synh->usTimeTimer[m] > 0) synh->usTimeTimer[m]--;
  				   }
  				}
               synh++;
  			}
    rtcmNacoplenie();
    if (DispDWord.Zapros.FindPartsTreshold)
    {
#ifdef PLATFORM_TMS6424
          if((Edma3ccRegsOvly->SER & (((Uint32)1)<<FRNS_DMA_ARU)) == 0)
#endif
#ifdef PLATFORM_ARM_A9
        	  if(iFRNS_IO_Complete)
#endif
          {
                  if (BlockFindPartsTrsh == 0)
                  {
                  	DispDWord.Zapros.FindPartsTreshold = false;
					BlockFindPartsTrsh = 1;
					FindPartsTreshold();
					BlockFindPartsTrsh = 0;
				  }
                  //FAruCorrection();
          }
    }
    //
#ifdef MODULE_219
    if( ((ErrorStatus & ((unsigned int)1<<FIVE_MHZ_ERROR)) != 0) ||
        ((ErrorStatus & ((unsigned int)1<<REOSTAT_IFRNS_ERROR)) != 0) ||
        //((ErrorStatus & ((unsigned int)1<<REOSTAT_FRNS_ERROR)) != 0) ||
        ((ErrorStatus & ((unsigned int)1<<EEPROM_ERROR)) != 0)
      )
    {
       if(ErrorFailCnt>1)
       {
          IndFail();
       }
       ErrorFailCnt++;
    }
    else
    {
       IndNormWork();
       ErrorFailCnt = 0;
    }
#endif
    //ResetWDT();
  }
  if(DispDWord.Zapros.zCalcRes)//Привязана к секундной метке, если есть..
  {
    /*
    if((uiDMAIFRNSInterruptCounter < 30) || (uiDMAIFRNSInterruptCounter > 33)) 
    {
    	ErrorStatus|=((unsigned int)1 << FIVE_MHZ_ERROR);
    	iMainFreqFailDetected = 1;
    }
	else
    {
    	ErrorStatus&=~((unsigned int)1 << FIVE_MHZ_ERROR);
    	if(iMainFreqFailDetected == 1)
    	{
             for(cn=0;cn<CHAINS_NUMBER;cn++)
             {
               if(m_Config.chain_id[cn]!=0)
			   {
                 Command[cn].aword.Init = 1;
                 // установка режима поиска 
                 Command[cn].aword.Rejim = REJIM_WAIT4START;
			   }
             }    		 		
    	}
    	iMainFreqFailDetected = 0;
    }
    */
#ifdef MODULE_241
     if(ErrorStatus&((unsigned int)1 << PILOT_IFRNS_ERROR))//смотрим по пилоту
     {
       IndSearch(1);
     } 
     else
     {
       IndNormWork();
     }
#endif
  //
     if(bFirstStageDone==0)
     {
       DispDWord.Zapros.CorrectionGet=0;
	   if(bFirstStageDone == 0)
       		FirstStageOutput();
       bFirstStageDone=1;
       if(AdjustmentData_Present && Hz_Present/* && (m_Config.iGlobal&64)*/)//Если есть - ждём всегда
       {
        SetWait(WAIT_DATA_TIMER, 0);
       }
       else
       {
        SetWait(WAIT_DATA_TIMER, 60);
       }
       //if(AdjustmentData_Present && Hz_Present && (m_Config.iGlobal&64))
     }
     else
     {
       if(IsCounted(WAIT_DATA_TIMER)==1 || DispDWord.Zapros.CorrectionGet)
       {
          //ResetDataCnt=0;
          SetWait(WAIT_DATA_TIMER, 0);//ok! Data Get!
          
          SecondStageOutput();
          Desition_on_VirtualData();
          //if(ResetDataCnt>4) RMC.status='V';
          //RMC.status='V';
          //sprintf(str, "$DON,%.2d,%.3d", sys_time.sec,(INERRUPTS_IN_SEC - sys_time.hung)*5);SetCotrol(str);OutString(str);
          DispDWord.Zapros.CorrectionGet=0;
          DispDWord.Zapros.zCalcRes = 0;
          bFirstStageDone=0;
       }
     }
  }

  if(DispDWord.Zapros.OneMinute)
  {
     DispDWord.Zapros.OneMinute=0;
        //if(!bFlashBUSY)
#ifdef MODULE_205
          register type_time * st=&sys_time;
	      double Time_h=((double)(st->hour*3600+st->min*60+st->sec))/3600.0;
          popraw(cinfo[CHAIN_PILOT].last.f,cinfo[CHAIN_PILOT].last.l,st->year,st->month,st->day,Time_h,Station_num);
#endif
          SecondaryOneMinuteTask();
        
  }
  if (DispDWord.Zapros.AnalizeFRNSPhases)
  {
	DispDWord.Zapros.AnalizeFRNSPhases = 0;
	AnalizeFreqData2Coorsd();
  }
  if(DispDWord.Zapros.SWReset)
  {
		  if(SM_BUSY == 0)
		  {
			//WatchDogTimerInit(1.5);
			//WatchDogReset();
			//while(1){continue;}
			ResetModule();
			DispDWord.Zapros.SWReset = 0;
		  }	
  }
  if(DispDWord.Zapros.ResyncStation)
  {
     DispDWord.Zapros.ResyncStation = 0;
	 /*
         for(i=0;i<(CHAINS_NUMBER);i++)//Перезапускаем всё
         {
           if(m_Config.chain_id[i]==0) continue;
            Command[i].aword.Init = 1;
            // установка режима поиска 
            Command[i].aword.Rejim = REJIM_SKO;
         }
         ResetPsevdoPilot();
	 */
  }
  if (DispDWord.Zapros.FRNS_poisk)
  {
  	//DispDWord.Zapros.FRNS_poisk = false;
#ifdef PLATFORM_TMS6424
  	if (Edma3ccRegsOvly->QSER == 0)
#endif
  	{
  		DispDWord.Zapros.FRNS_poisk = false;
#ifdef MODULE_205
  		FRNS_poisk();	
#endif
  	}	
  	//FRNS_poisk();		
  }
  // Управление пилотом - выключаем если выключается штырь на время пилотирования. Иначе не будет работать компенсация.
  if (DispDWord.Zapros.vpRememberARU)
  {
      DispDWord.Zapros.vpRememberARU = 0;
      aru.vp_adc     = aru.adc;    
      aru.vp_adc_val = aru.adc_val;
      DispDWord.Zapros.vpDisablePilot = 1;

  }
  if (DispDWord.Zapros.vpDisablePilot)
  {
      DispDWord.Zapros.vpDisablePilot = 0;
      Send_To_MCU_cmd(5);
      DispDWord.Zapros.vpWaitTimeout = 1;
  }
  if(DispDWord.Zapros.vpWaitTimeout)
  {
     if(iVirtualPilotTimerSet == 0)
     {
        DispDWord.Zapros.vpWaitTimeout = 0;
     }
     if(iVirtualPilotTimerSet && IsCounted(PILOT_VIRTUAL_WAIT) )
     {
     	DispDWord.Zapros.vpWaitTimeout = 0;
     	DispDWord.Zapros.vpRestoreARU = 1;
     }
  }
  if (DispDWord.Zapros.vpRestoreARU)
  {
      if( aru.adc < aru.vp_adc )
      {
      	if(IsCounted(ARU_TIMER))
      	{
      		AddLevel(1);// Плавненько выводим на уровень
      		SetWait(ARU_TIMER, 5);
      	}
      }
      else
      {
      	DispDWord.Zapros.vpRestoreARU = 0;
      	DispDWord.Zapros.vpStartPilot = 1;
      }
  }
  if (DispDWord.Zapros.vpStartPilot)
  {
      DispDWord.Zapros.vpStartPilot = 0;
      Send_To_MCU_cmd(6);
      SetWait(PILOT_RM_WAIT, 120000);
  }

}

void TestARU(void);

const char *alive_str = ".";
void ImAlive(void)
{
Send101();
//WriteUart(NMEA_PORT, (unsigned char *)alive_str, 1, 6);
WatchDogReset();
}


void ifrns_task(void)
{
#ifdef MODULE_241
        Serial_Nmea_241();
#else
        Serial_Nmea_205();
        ReceiveBynary();
#endif
        Disp();
        FlashJob();
		if( IsCounted(PILOT_RM_WAIT) && ( iVirtualPilotTimerSet == 0 ) && ( (module_ctrl.ctrl.control&(1<<5)) != 0 ) )
		{
		  if(SM_BUSY == 0)
		  {
			//WatchDogTimerInit(1.5);
			//WatchDogReset();
			//while(1){continue;}
			ResetModule();
		  }	
		}
       if(sCalibration==ARU_CAL_FINISH)
       {
          if(bFirstStageDone==0)
          {
            //
            FilterProc();
            //
            CalcSKPD();
            //
          }
          if(sys_time.hung<100 || bFirstStageDone==0)
          {
            //
            SS_Amp();
            //
          }
       }
       //TestARU();
//       ResetWDT();
}

