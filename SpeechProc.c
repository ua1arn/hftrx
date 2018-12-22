/*
 * SpeechProc.c
 *
 *  Created on: 10 нояб. 2018 г.
 *      Author: Admin
 */
#include "SpeechProc.h"
arm_rfft_fast_instance_f32 RfftSp;
arm_rfft_fast_instance_f32 RfftSpectrum;
phase_rotator_t phrot;
symdet_t symmetry_detector_param;
compparam_t CB[N_POLOS];
NotchFilter NF;
NotchFilter NF2;
soft_kneecompp_t SoftKneeComp;
eqparam_t PostEqBand[N_POLOS];
limitparam_t Limitter;
compparam_t NoiseGate;

uint tmp_filter_bank_fmin[N_FILTER];
uint tmp_filter_bank_fmax[N_FILTER];
uint tmp_mb_filter_sharpness;//порядок фильтров компрессора - от 64 до 512
uint tmp_out_filter_sharpness;//порядок фильтров компрессора - от 64 до 512
__attribute__((section (".dtcmram"))) float Reverberator_Delay[REVERBERATOR_DELAY_MAX_LEN];
__attribute__((section (".dtcmram"))) filter_bank_t FiltBank;
dc_reject_t DcInReject, DcOutReject;
SpProcOpt_t SpOptions=
{
.input_type=0,

.notch_en=1,
.notch_frequency=140,
.notch_width=200,

.ng_en=0,
.ng_threshold_dB=75,
.ng_trel=2,
.ng_ratio=0.1,
.ng_tatt=2,

.ph_rot_en=1,
.ph_rot_freq=350,
.ph_rot_stage=25,
.pre_gain=10,

.mb_comp_en=1,
.mb_comp_threshold_dB[0]=-80,
.mb_comp_threshold_dB[1]=-80,
.mb_comp_threshold_dB[2]=-80,
.mb_comp_threshold_dB[3]=-80,
.mb_comp_threshold_dB[4]=-80,
.mb_comp_threshold_dB[5]=-80,

.mb_comp_tatt[0]=2,
.mb_comp_tatt[1]=2,
.mb_comp_tatt[2]=2,
.mb_comp_tatt[3]=2,
.mb_comp_tatt[4]=2,
.mb_comp_tatt[5]=2,

.mb_comp_trel[0]=10,
.mb_comp_trel[1]=10,
.mb_comp_trel[2]=10,
.mb_comp_trel[3]=10,
.mb_comp_trel[4]=10,
.mb_comp_trel[5]=10,

.mb_comp_ratio[0]=3,
.mb_comp_ratio[1]=3,
.mb_comp_ratio[2]=3,
.mb_comp_ratio[3]=3,
.mb_comp_ratio[4]=3,
.mb_comp_ratio[5]=3,

.eq_en=1,
.eq_gain[0]=10,
.eq_gain[1]=10,
.eq_gain[2]=10,
.eq_gain[3]=10,
.eq_gain[4]=10,
.eq_gain[5]=10,

.soft_knee_comp_en=1,
.soft_knee_comp_threshold_dB=-50,
.soft_knee_comp_tatt=5,
.soft_knee_comp_trel=20,
.soft_knee_comp_ratio=2,
.soft_knee=0.5,

.post_gain=10,

.limitter_level=10,
.limitter_tatt=1.5,
.limitter_trel=20,

.out_filter_en=1,

.filter_bank_fmin[0]=BAND1_MIN_FREQ,
.filter_bank_fmin[1]=BAND2_MIN_FREQ,
.filter_bank_fmin[2]=BAND3_MIN_FREQ,
.filter_bank_fmin[3]=BAND4_MIN_FREQ,
.filter_bank_fmin[4]=BAND5_MIN_FREQ,
.filter_bank_fmin[5]=BAND6_MIN_FREQ,
.filter_bank_fmin[6]=20,
.filter_bank_fmax[0]=BAND1_MAX_FREQ,
.filter_bank_fmax[1]=BAND2_MAX_FREQ,
.filter_bank_fmax[2]=BAND3_MAX_FREQ,
.filter_bank_fmax[3]=BAND4_MAX_FREQ,
.filter_bank_fmax[4]=BAND5_MAX_FREQ,
.filter_bank_fmax[5]=BAND6_MAX_FREQ,
.filter_bank_fmax[6]=10000,

.reverb_en=1,
.reverb_echo_gain=0.5f,
.reverb_delay=50,
.mb_filter_sharpness=512,
.out_filter_sharpness=512
};
void VarInit (void)
{
int i,k;
for (i=0;i<REVERBERATOR_DELAY_MAX_LEN;i++)
Reverberator_Delay[i]=0;

for (k=0;k<N_FILTER;k++)
{
for (i=0;i<FFT_FILTER_SIZE/2;i++)
FiltBank.filter[k][i].re=FiltBank.filter[k][i].im=0;
}
}
inline float dBToAmp (float in)
{
return powf(10.0f,in/20.0f);
}

inline float AmpTodB (float in)
{
return 20.0f*log10f(in);
}

void DC_OFFSET_REJECTION_Tx (float* input, int n, dc_reject_t* p)
{
int i;
//y(n)=x(n)-x(n-1)+R*y(n-1);
p->R=1.0f-(float)(DC_OFFSET_COEFF/(float)SAMPLE_RATE);//80Hz//63 - 10Hz, 126-20Hz, 190-30Hz, 250 - 40Hz
float tmp;
for(i=0;i<n;i++)
{
  tmp=input[i];
input[i]=input[i]-p->x_old+(p->R*p->y_old);
p->x_old=tmp;
p->y_old=input[i];
}
}

void PhRotator (float* in,
                uint cnt,
                uint fs,
                uint fc,
                uint nstages,
                phase_rotator_t* p)
{
float tmp=(float)fs;
p->fc=(float)fc;
p->nstages=nstages;
p->g=tanf(Pi*p->fc/tmp);
p->b[0] = (p->g - 1.0f) / (p->g + 1.0f);
p->b[1] = 1.0f;
p->a[1] = p->b[0];
int i, n;
		for (i = 0; i < cnt; i++)
		{
			p->x[0][0] = in[i];
			for (n = 0; n < p->nstages; n++)
			{
				if (n > 0) p->x[0][n] = p->y[0][n - 1];
				p->y[0][n]= p->b[0] * p->x[0][n]
							+ p->b[1] * p->x[1][n]
							- p->a[1] * p->y[1][n];
				p->y[1][n] = p->y[0][n];
				p->x[1][n] = p->x[0][n];
			}
			in[i] = p->y[0][p->nstages - 1];
		}
}

void SymmetryDetector (float *in, uint cnt, symdet_t* p)
{
int i;
p->alpha=SYMDETSMOOTH;
float tmp;
for (i=0;i<cnt;i++)
{
if(in[i]>=0)
p->pos_ave+=in[i]*in[i];//p->pos_ave=p->alpha*p->pos_ave+(1-p->alpha)*in[i];
else
p->neg_ave+=in[i]*in[i];//=p->alpha*p->neg_ave+(1-p->alpha)*in[i];
p->cnt++;
if(p->cnt==SYMMETRY_CNT)
{
p->cnt=0;
if(p->neg_ave!=0)
tmp=(p->pos_ave/p->neg_ave);
else
tmp=0;
p->symmetry=0.9*p->symmetry+0.1*tmp;
if(p->symmetry>=2.0)p->symmetry=2.0;
p->pos_ave=p->neg_ave=0;
}
}
}

void GetGaindB_f32 (float* in, uint cnt, int dB)
{
float tmp;
int i;
tmp=dBToAmp((float)dB);
for (i=0;i<cnt;i++)
in[i]*=tmp;
}

void SpeechCompressorInit(compparam_t* param)
{
  /*
  type_comp - 0 - обычный, 1 - софт кни
  Я не стал заморачиваться и делать разные структуры в
  зависимости от типа компрессора. Поэтому на оба типа - хард и софт
  функция одна
  */
  param->sr=SAMPLE_RATE;
    param->slope=1.0f-1.0f/param->ratio;
    param->max_level_dB=AmpTodB(MAX_24bit_FLOAT);
    param->threshold=dBToAmp(param->max_level_dB-fabs(param->threshold_dB));
    param->alpha_att = (param->tatt == 0.0) ? (0.0) : expf (-1.0 / ((float)param->sr * param->tatt*1e-3));
    param->beta_att=1.0f-param->alpha_att;
    param->alpha_rel = (param->trel == 0.0) ? (0.0) : expf (-1.0 / ((float)param->sr * (float)param->trel*1e-3));
    param->beta_rel=1.0f-param->alpha_rel;
}

void SpeechCompressor
    (
        float*  wav_in,     // сигнал
        int     n,          // количество сэмплов
        compparam_t* param
    )
{
  float tmp;
  //float alpha=COMP_SMOOTH_FACTOR;
  //float beta=1.0f-alpha;
  float tmp_gain=0;
    for (int i = 0; i < n; ++i)
    {
      if(wav_in[i]<0.0f) tmp=(-1.0f)*wav_in[i];
      else tmp=wav_in[i];
  if(param->env < tmp)
  {
     param->env = param->env *param->alpha_att+param->beta_att*tmp;
  }
  else
  {
     param->env =param->env* param->alpha_rel+param->beta_rel*tmp;
  }
    // простой 1:N компрессор
    if (param->env> param->threshold)
    param->gain =powf((param->threshold/param->env),(param->slope));
    else param->gain=1;

 if(param->gain>1.0f)
    param->gain = 1.0f;
    wav_in[i]*= param->gain;
    tmp_gain+=param->gain;
   // param->ave_gain=param->ave_gain*alpha+param->gain*beta;
    }
    param->ave_gain=tmp_gain/n;
}

void NoiseGateProc
    (
        float*  wav_in,     // сигнал
        int     n,          // количество сэмплов
		compparam_t* param
    )
{
	float tmp;
	  //float alpha=COMP_SMOOTH_FACTOR;
	  //float beta=1.0f-alpha;
	  float tmp_gain=0;
	    for (int i = 0; i < n; ++i)
	    {
	      if(wav_in[i]<0.0f) tmp=(-1.0f)*wav_in[i];
	      else tmp=wav_in[i];
	  if(param->env < tmp)
	  {
	     param->env = param->env *param->alpha_att+param->beta_att*tmp;
	  }
	  else
	  {
	     param->env =param->env* param->alpha_rel+param->beta_rel*tmp;
	  }

	  if (param->env< param->threshold)
	    param->gain =powf((param->threshold/param->env),(param->slope));
	    else param->gain=1;
	 /* if (param->env< param->threshold)
	  	    param->gain = 0;//powf((param->threshold/param->env),(param->slope));
	  	    else param->gain=1;*/
	 if(param->gain>1.0f)
	    param->gain = 1.0f;
	    wav_in[i]*= param->gain;
	    tmp_gain+=param->gain;
	   // param->ave_gain=param->ave_gain*alpha+param->gain*beta;
	    }
	    param->ave_gain=tmp_gain/n;
    }

void Equalizer (float * in, uint n,
                eqparam_t* param,
                uint fs,
                uint f_min,
                uint f_max,
                int gain)
{
  int i;
  float y;
  float tmp;
  tmp=(float)fs;
  param->Df=(float)(f_max-f_min);
  param->f0=(float)(f_max+f_min)/2.0f;
 if(gain==0.0)
  {
  param->a[0]=param->b[0]=1.0f;
  param->a[1]=param->a[2]=param->b[1]=param->b[2]=0.0f;
  }
 else
 {
  param->G=dBToAmp((float)gain);
  param->GB=sqrtf(param->G);
  param->be=sqrtf((param->GB*param->GB-1)/(param->G*param->G-param->GB*param->GB))*
                  tanf(Pi*param->Df/tmp);
  param->c0=cosf(2*Pi*param->f0/tmp);
  param->a[0]=1.0f;
  param->a[1]=-2.0f*param->c0/(1.0f+param->be);
  param->a[2]=(1.0f-param->be)/(1.0f+param->be);
  param->b[0]=(1.0f+param->G*param->be)/(1.0f+param->be);
  param->b[1]=-2.0f*param->c0/(1.0f+param->be);
  param->b[2]=(1.0f-param->G*param->be)/(1.0f+param->be);
 }
  for(i=0;i<n;i++)
  {
param->w[0]=in[i]-param->a[1]*param->w[1]-param->a[2]*param->w[2];
y=param->b[0]*param->w[0]+param->b[1]*param->w[1]+param->b[2]*param->w[2];
param->w[2]=param->w[1];param->w[1]=param->w[0];
in[i]=y;
  }
}

void SoftKneeCompInit(soft_kneecompp_t* param)
{
  /*
  type_comp - 0 - обычный, 1 - софт кни
  Я не стал заморачиваться и делать разные структуры в
  зависимости от типа компрессора. Поэтому на оба типа - хард и софт
  функция одна
  */
  param->sr=SAMPLE_RATE;

    param->slope=1.0f-1.0f/param->ratio;
    param->max_level_dB=AmpTodB(MAX_24bit_FLOAT);
    param->threshold=dBToAmp(param->max_level_dB-fabs(param->threshold_dB));
    param->alpha_att = (param->tatt == 0.0) ? (0.0) : expf (-1.0 / ((float)param->sr * param->tatt*1e-3));
    param->beta_att=1.0f-param->alpha_att;
    param->alpha_rel = (param->trel == 0.0) ? (0.0) : expf (-1.0 / ((float)param->sr * (float)param->trel*1e-3));
    param->beta_rel=1.0f-param->alpha_rel;

    param->knee_width_dB=param->threshold_dB * param->knee * (-1.0f);
    param->knee_low_bound_dB = param->threshold_dB - (param->knee_width_dB / 2.0f);
    param->knee_up_bound_dB = param->threshold_dB + (param->knee_width_dB / 2.0f);

    param->knee_width=dBToAmp(param->max_level_dB-fabs(param->knee_width_dB));
    param->knee_low_bound=dBToAmp(param->max_level_dB-fabs(param->knee_low_bound_dB));
    param->knee_up_bound=dBToAmp(param->max_level_dB-fabs(param->knee_up_bound_dB));
    //param->lah_mask=DSP_SAMPLE_RATE/LIMITTER_LOOK_AHEAD;
}

void SoftKneeCompressor
    (
        float*  wav_in,     // сигнал
        int     n,          // количество сэмплов
        soft_kneecompp_t* param
    )
{
  float tmp;
  //float alpha=COMP_SMOOTH_FACTOR;
  //float beta=1.0f-alpha;
  float tmp_gain=0;
  for (int i = 0; i < n; ++i)
    {
      if(wav_in[i]<0) tmp=(-1)*wav_in[i];
      else tmp=wav_in[i];

      //delay=param->lah_delay[param->lah_ptr];
   //   param->lah_delay[param->lah_ptr]=wav_in[i];
     // param->lah_ptr++;
    //  if(param->lah_ptr>(param->lah_mask-1))param->lah_ptr=0;


  if(param->env < tmp)
  {
     param->env = param->env *param->alpha_att+param->beta_att*tmp;
  }
  else
  {
      param->env =param->env* param->alpha_rel+param->beta_rel*tmp;
  }

param->env_dB=(AmpTodB(param->env)-param->max_level_dB);
if (param->knee_width_dB > 0.0f && param->env_dB > param->knee_low_bound_dB && param->env_dB < param->knee_up_bound_dB)  // Soft knee
 {
  tmp=param->slope *( ((param->env_dB - param->knee_low_bound_dB) / param->knee_width_dB) * 0.5f );
  param->gain_dB = tmp * (param->knee_low_bound_dB - param->env_dB);
  param->gain=dBToAmp(param->gain_dB);
 }
  else
  {
  if (param->env> param->threshold)
    param->gain =powf((param->threshold/param->env),(param->slope));
  else param->gain=1;
  }
 if(param->gain>1.0f)
    param->gain = 1.0f;
    wav_in[i]*= param->gain;
    tmp_gain+=param->gain;
   //param->ave_gain=param->ave_gain*alpha+param->gain*beta;
    }
  param->ave_gain=tmp_gain/n;
}

void LimitterInit(limitparam_t* param)
{
  /*
  type_comp - 0 - обычный, 1 - софт кни
  Я не стал заморачиваться и делать разные структуры в
  зависимости от типа компрессора. Поэтому на оба типа - хард и софт
  функция одна
  */
  param->threshold_dB=(-1)*SpOptions.limitter_level;//LIMITTER_UP_THRESHOLD;
  if(param->tatt>=20)param->tatt=20;
  param->sr=SAMPLE_RATE;
    param->slope=1.0f-1.0f/param->ratio;
    param->max_level_dB=AmpTodB(MAX_24bit_FLOAT);
    param->threshold=dBToAmp(param->max_level_dB-fabs(param->threshold_dB));
    param->alpha_att = (param->tatt == 0.0) ? (0.0) : expf (-1.0 / ((float)param->sr * param->tatt*1e-3));
    param->beta_att=1.0f-param->alpha_att;
    param->alpha_rel = (param->trel == 0.0) ? (0.0) : expf (-1.0 / ((float)param->sr * (float)param->trel*1e-3));
    param->beta_rel=1.0f-param->alpha_rel;
    param->lah_mask=(int)LIM_LAH_DELAY;//(param->sr * param->tatt*1e-3);
}

void LimitterCompressor
    (
        float*  wav_in,     // сигнал
        int     n,          // количество сэмплов
        limitparam_t* param
    )
{
  float tmp;
  float alpha=COMP_SMOOTH_FACTOR;
  float beta=1-alpha;
  float delay;
    for (int i = 0; i < n; ++i)
    {
      if(wav_in[i]<0.0f) tmp=(-1.0f)*wav_in[i];
      else tmp=wav_in[i];

      delay=param->lah_delay[param->lah_ptr];
      param->lah_delay[param->lah_ptr]=wav_in[i];
      param->lah_ptr++;
      if(param->lah_ptr>(param->lah_mask-1))param->lah_ptr=0;
  if(param->env < tmp)
  {
     param->env = param->env *param->alpha_att+param->beta_att*tmp;
  }
  else
  {
      param->env =param->env* param->alpha_rel+param->beta_rel*tmp;
  }
    // простой 1:N компрессор
  if (param->env> param->threshold)
    param->gain =powf((param->threshold/param->env),(param->slope));
  else param->gain=1;
 if(param->gain>1) param->gain = 1;
    wav_in[i]=delay* param->gain;

   if(fabs(wav_in[i])>param->threshold)
    {
    if(wav_in[i]>0.0f)
      wav_in[i]=param->threshold;
    else wav_in[i]=(-1.0f)*param->threshold;

    }

    param->ave_gain=param->ave_gain*alpha+param->gain*beta;
    }
}

void ManNotchInit (uint F0, uint Df, NotchFilter* ptr)
{
ptr->F0=F0;
ptr->Fs=SAMPLE_RATE;
ptr->Df=(float)Df;
ptr->be=tanf((Pi*ptr->Df)/ptr->Fs);
ptr->c0 = arm_cos_f32(2*Pi*ptr->F0/ptr->Fs);
ptr->a[0] = 1;
ptr->a[1] = -2*ptr->c0/(1+ptr->be);
ptr->a[2] = (1-ptr->be)/(1+ptr->be);
ptr->b[0] = 1/(1+ptr->be);
ptr->b[1] = -2*ptr->c0/(1+ptr->be);
ptr->b[2] = 1/(1+ptr->be);
/*for (int i=0;i<3;i++)
ptr->w[i] = 0;*/
}

void ManualNotch(uint F0, uint Df, float* inp,int size,NotchFilter* ptr)//ручной ночь, не используется.
{
float x;
int i;

ManNotchInit (F0,Df,ptr);

for (i=0;i<size;i++)
{
x = (inp[i]); // work with left input only
ptr->w[0] =( x - (ptr->a[1]*ptr->w[1]) - (ptr->a[2]*ptr->w[2]));
inp[i] = ((ptr->b[0]*ptr->w[0]) + (ptr->b[1]*ptr->w[1]) + (ptr->b[2]*ptr->w[2]));
ptr->w[2] = ptr->w[1];
ptr->w[1] = ptr->w[0];
}
ptr=&NF2;
ManNotchInit (F0,Df,ptr);
for (i=0;i<size;i++)
{
x = (inp[i]); // work with left input only
ptr->w[0] =( x - (ptr->a[1]*ptr->w[1]) - (ptr->a[2]*ptr->w[2]));
inp[i] = ((ptr->b[0]*ptr->w[0]) + (ptr->b[1]*ptr->w[1]) + (ptr->b[2]*ptr->w[2]));
ptr->w[2] = ptr->w[1];
ptr->w[1] = ptr->w[0];
}
}

volatile float gain_comp[N_FILTER];

void FilterBankCoefInit(void)
{
static int update=0;
int i;
for(i=0;i<N_FILTER;i++)
{
if (tmp_filter_bank_fmin[i]!=SpOptions.filter_bank_fmin[i])
{
	tmp_filter_bank_fmin[i]=SpOptions.filter_bank_fmin[i];
	update=1;
}
if (tmp_filter_bank_fmax[i]!=SpOptions.filter_bank_fmax[i])
{
	tmp_filter_bank_fmax[i]=SpOptions.filter_bank_fmax[i];
	update=1;
}
}
if(tmp_mb_filter_sharpness!=SpOptions.mb_filter_sharpness)
{
	tmp_mb_filter_sharpness=SpOptions.mb_filter_sharpness;
	update=1;
}
if(tmp_out_filter_sharpness!=SpOptions.out_filter_sharpness)
{
	tmp_out_filter_sharpness=SpOptions.out_filter_sharpness;
	update=1;
}

 if(update)
 {
design_LPF_FIR_coefs_var_order(SpOptions.filter_bank_fmax[0],
		                      SpOptions.mb_filter_sharpness,
		                       (COMPLEX*)&FiltBank.filter[0][0].re);

for(i=0;i<N_FILTER-1;i++)
	gain_comp[i]=-10000.0f;
for(i=1;i<N_FILTER-2;i++)
design_FIR_coefs_real_var_order (SpOptions.filter_bank_fmin[i],SpOptions.filter_bank_fmax[i],
		                          SpOptions.mb_filter_sharpness,
								 (COMPLEX*)&FiltBank.filter[i][0].re);//функция вычисляет коэффициенты полосового фильтра

design_FIR_coefs_real_var_order(SpOptions.filter_bank_fmin[N_FILTER-2],
								11000,
								SpOptions.mb_filter_sharpness,
								(COMPLEX*)&FiltBank.filter[N_FILTER-2][0].re);
if(SpOptions.filter_bank_fmin[N_FILTER-1]!=0)//если не ФНЧ
design_FIR_coefs_real_var_order(SpOptions.filter_bank_fmin[N_FILTER-1],SpOptions.filter_bank_fmax[N_FILTER-1],
			SpOptions.out_filter_sharpness,
			(COMPLEX*)&FiltBank.filter[N_FILTER-1][0].re);


	else
design_LPF_FIR_coefs_var_order(SpOptions.filter_bank_fmax[N_FILTER-1],
								SpOptions.out_filter_sharpness,
								(COMPLEX*)&FiltBank.filter[N_FILTER-1][0].re);
/*
for(i=0;i<N_FILTER;i++)
{
	for(k=0;k<FFT_FILTER_SIZE/2;k++)
	{
tmp= (SoundProcessor.filter_bank[i][k].re*SoundProcessor.filter_bank[i][k].re+
		SoundProcessor.filter_bank[i][k].im*SoundProcessor.filter_bank[i][k].im);
tmp=(10.0f*log10f(tmp));
if(tmp>gain_comp[i])gain_comp[i]=tmp;

if (tmp<-127)tmp=-127;
if (tmp>127)tmp=127;
SpOptions.FilterSpectrum[i][k]=(int8_t)tmp;
	}
}
*/
//Теперь надо компенсировать усиление по фильтрам
/*
for(i=0;i<N_FILTER;i++)
{
	tmp=dBToAmp(fabs(gain_comp[i]));
	for(k=0;k<FFT_FILTER_SIZE/2;k++)
	{
SoundProcessor.filter_bank[i][k].re*=tmp;
SoundProcessor.filter_bank[i][k].im*=tmp;

tmp2= (SoundProcessor.filter_bank[i][k].re*SoundProcessor.filter_bank[i][k].re+
		SoundProcessor.filter_bank[i][k].im*SoundProcessor.filter_bank[i][k].im);
tmp2=(10.0f*log10f(tmp2));
if (tmp2<-127)tmp2=-127;
if (tmp2>127)tmp2=127;
SpOptions.FilterSpectrum[i][k]=(int8_t)tmp2;
	}
}
*/
update=0;
 }
}

void Reverberator (float* input, int n)
{
int i;
static int Dp=0;
uint delay_len;//длина задержки, переводит мс в сэмплы
float y, x, sD,u;
float b1=0.07f;
float b0=0.1f;
static float v1, v0;
float a=SpOptions.reverb_echo_gain;
a*=0.8f;//Для "подгонки масштабирования", т.к. входное значение 1-100 (в итоге 0,01-1,0), а при коэфф.0,8 алгоритм работает нестабильно.
if (SpOptions.reverb_delay<=REVERBERATOR_MAX_DELAY)
delay_len=SpOptions.reverb_delay*SAMPLE_RATE/1000;
else
delay_len=REVERBERATOR_MAX_DELAY*SAMPLE_RATE/1000;
for(i=0;i<n;i++)
{
sD=Reverberator_Delay[Dp];
x=input[i];
v0 = a*v1 + sD;
u = b0*v0 + b1*v1;
v1 = v0;
y = x+u;
Reverberator_Delay[Dp] = y;
input[i]=y;
Dp++;
if (Dp>=(delay_len))Dp=0;
}
}



void SpeechProcessor (int* in,//вход от кодека
					int* out //выход кодека
		)
{
int i,k;
int32_t* tmp_out;
float tmp;
float* CompTmpBuf;
static int sample_count=0;
arm_rfft_fast_init_f32(&RfftSp,FFT_FILTER_SIZE);
arm_rfft_fast_init_f32(&RfftSpectrum,2*FFT_FILTER_SIZE);
FilterBankCoefInit();
CompTmpBuf=(float*)malloc((FFT_FILTER_SIZE/2)*sizeof(float));
if(CompTmpBuf==NULL) while (1);
GetNormalized24BitAdcData(in, FFT_FILTER_SIZE);
if(SpOptions.input_type==0)
for(i=0;i<FFT_FILTER_SIZE;i+=2) SoundProcessor.TmpBuf[i/2]=(float)in[i];
else for(i=0;i<FFT_FILTER_SIZE;i+=2) SoundProcessor.TmpBuf[i/2]=(float)in[i+1];

DC_OFFSET_REJECTION_Tx (SoundProcessor.TmpBuf, FFT_FILTER_SIZE/2,&DcInReject);//вырезал постоянку

if(SpOptions.notch_en)
ManualNotch(SpOptions.notch_frequency, SpOptions.notch_width,
		SoundProcessor.TmpBuf,FFT_FILTER_SIZE/2,&NF);

if(SpOptions.ng_en)
{
	NoiseGate.threshold_dB=(-1)*SpOptions.ng_threshold_dB;
	NoiseGate.trel=SpOptions.ng_trel;
	NoiseGate.ratio=SpOptions.ng_ratio;
	NoiseGate.tatt=SpOptions.ng_tatt;
	SpeechCompressorInit(&NoiseGate);
	NoiseGateProc(SoundProcessor.TmpBuf,     // сигнал
				FFT_FILTER_SIZE/2,          // количество сэмплов
				&NoiseGate);
}

if(SpOptions.ph_rot_en)//если фазовый ротатор включен
{
PhRotator ( SoundProcessor.TmpBuf,//фазовращатель
		    FFT_FILTER_SIZE/2,
            SAMPLE_RATE,
			SpOptions.ph_rot_freq,
			SpOptions.ph_rot_stage,
                &phrot);
SymmetryDetector (SoundProcessor.TmpBuf,
				FFT_FILTER_SIZE/2,
				&symmetry_detector_param);
SpOptions.symmetry=symmetry_detector_param.symmetry;
}

GetGaindB_f32 (SoundProcessor.TmpBuf,
			   FFT_FILTER_SIZE/2,
			   SpOptions.pre_gain);

if(SpOptions.mb_comp_en)
{
	for (i=0;i<N_POLOS;i++)
	{

	CB[i].threshold_dB=(-1)*SpOptions.mb_comp_threshold_dB[i];
	CB[i].tatt=SpOptions.mb_comp_tatt[i];
	CB[i].trel=SpOptions.mb_comp_trel[i];
	CB[i].ratio=SpOptions.mb_comp_ratio[i];
	SpeechCompressorInit(&CB[i]);
	}

for(i=0;i<FFT_FILTER_SIZE/2;i++)
SoundProcessor.CompRfftBuff[i+FFT_FILTER_SIZE/2]=SoundProcessor.TmpBuf[i];
arm_rfft_fast_f32(&RfftSp, SoundProcessor.CompRfftBuff, (float*)SoundProcessor.CompCfftBuff,0);  //Here happens
for(i=0;i<FFT_FILTER_SIZE/2;i++)
CompTmpBuf[i]=0;
for (k=0;k<N_POLOS;k++)
{
	arm_cmplx_mult_cmplx_f32((float*)SoundProcessor.CompCfftBuff,
			(float*)&FiltBank.filter[k][0].re,
			(float*)SoundProcessor.CompTmpCfftBuff,FFT_FILTER_SIZE/2);//Здесь сразу фильтрую
	arm_rfft_fast_f32(&RfftSp, (float*)SoundProcessor.CompTmpCfftBuff, SoundProcessor.CompRfftBuff,1);  //Here happens

	SpeechCompressor(&SoundProcessor.CompRfftBuff[FFT_FILTER_SIZE/2],FFT_FILTER_SIZE/2,&CB[k]);

	for(i=0;i<FFT_FILTER_SIZE/2;i++)
		CompTmpBuf[i]+=SoundProcessor.CompRfftBuff[FFT_FILTER_SIZE/2 + i];
}
for(i=0;i<N_POLOS;i++)
{
CB[i].ave_gain_dB=(int)(AmpTodB(CB[i].ave_gain));
SpOptions.mb_comp_env[i]=CB[i].ave_gain_dB;
}

for(i=0;i<FFT_FILTER_SIZE/2;i++)
	SoundProcessor.CompRfftBuff[i]	=SoundProcessor.TmpBuf[i];
for(i=0;i<FFT_FILTER_SIZE/2;i++)
		SoundProcessor.TmpBuf[i]=CompTmpBuf[i];
}

if(SpOptions.eq_en)
	for(i=0;i<N_POLOS;i++)
	Equalizer(SoundProcessor.TmpBuf,FFT_FILTER_SIZE/2,
			&PostEqBand[i],SAMPLE_RATE,
			SpOptions.filter_bank_fmin[i],SpOptions.filter_bank_fmax[i], SpOptions.eq_gain[i]);

if(SpOptions.soft_knee_comp_en)
{
	SoftKneeComp.threshold_dB=(-1)*SpOptions.soft_knee_comp_threshold_dB;
	SoftKneeComp.tatt=SpOptions.soft_knee_comp_tatt;
	SoftKneeComp.trel=SpOptions.soft_knee_comp_trel;
	SoftKneeComp.ratio=SpOptions.soft_knee_comp_ratio;
	SoftKneeComp.knee=SpOptions.soft_knee;
	SoftKneeCompInit(&SoftKneeComp);

	SoftKneeCompressor(SoundProcessor.TmpBuf,FFT_FILTER_SIZE/2,&SoftKneeComp);
	SoftKneeComp.ave_gain_dB=(int)(AmpTodB(SoftKneeComp.ave_gain));
	SpOptions.soft_knee_comp_env=SoftKneeComp.ave_gain_dB;

}

GetGaindB_f32 (SoundProcessor.TmpBuf, FFT_FILTER_SIZE/2, SpOptions.post_gain);

if(SpOptions.reverb_en)
	Reverberator (SoundProcessor.TmpBuf,FFT_FILTER_SIZE/2);

Limitter.tatt=SpOptions.limitter_tatt;//options.soft_knee_comp_tatt;
Limitter.trel=SpOptions.limitter_trel;//options.soft_knee_comp_trel;
Limitter.ratio=1000000.0f;//options.soft_knee_comp_ratio;
LimitterInit(&Limitter);
LimitterCompressor(SoundProcessor.TmpBuf,FFT_FILTER_SIZE/2,&Limitter);

	for(i=0;i<FFT_FILTER_SIZE/2;i++)
	SoundProcessor.OutRfftBuff[i+FFT_FILTER_SIZE/2]=SoundProcessor.TmpBuf[i];
	arm_rfft_fast_f32(&RfftSp, SoundProcessor.OutRfftBuff, (float*)SoundProcessor.CompCfftBuff,0);  //Here happens
	if(SpOptions.out_filter_en)
	{
    arm_cmplx_mult_cmplx_f32((float*)SoundProcessor.CompCfftBuff,
				(float*)&FiltBank.filter[N_FILTER-1][0].re,
				(float*)SoundProcessor.CompCfftBuff,FFT_FILTER_SIZE/2);//Здесь сразу фильтрую
	}
	arm_rfft_fast_f32(&RfftSp, (float*)SoundProcessor.CompCfftBuff, SoundProcessor.OutRfftBuff,1);
	for(i=0;i<FFT_FILTER_SIZE/2;i++)
	SoundProcessor.OutRfftBuff[i]	=SoundProcessor.TmpBuf[i];
	for(i=0;i<FFT_FILTER_SIZE/2;i++)
	SoundProcessor.TmpBuf[i]=SoundProcessor.OutRfftBuff[i+FFT_FILTER_SIZE/2];


	for(i=0;i<FFT_FILTER_SIZE/2;i++)
	{
	SoundProcessor.SpectrumRealIn[sample_count]=SoundProcessor.TmpBuf[i];
	sample_count++;
	}
	if (sample_count>=(2*FFT_FILTER_SIZE))
	{
		sample_count=0;
	for(i=0;i<2*FFT_FILTER_SIZE;i++)
		SoundProcessor.SpectrumRealIn[i]*=WindowCoefs_2048[i];
	arm_rfft_fast_f32(&RfftSpectrum, SoundProcessor.SpectrumRealIn, (float*)SoundProcessor.CompCfftBuff,0);
	for(i=0;i<FFT_FILTER_SIZE;i++)
		{
			tmp= (SoundProcessor.CompCfftBuff[i].re*SoundProcessor.CompCfftBuff[i].re+
			SoundProcessor.CompCfftBuff[i].im*SoundProcessor.CompCfftBuff[i].im)/(4*FFT_FILTER_SIZE*FFT_FILTER_SIZE/4);
			tmp=(10.0f*log10f(tmp));
			SpOptions.MAG[i]=(int8_t)(10.0f*log10f(tmp));
			if (tmp<0)tmp=0;
			if (tmp>127)tmp=127;
			SpOptions.MAG[i]=(int8_t)tmp;
		}
	}

DC_OFFSET_REJECTION_Tx (SoundProcessor.TmpBuf, FFT_FILTER_SIZE/2, &DcOutReject);
free(CompTmpBuf);
tmp_out=(int32_t*)malloc((FFT_FILTER_SIZE/2)*sizeof(int32_t));
if(tmp_out==NULL) while (1);

for(i=0;i<FFT_FILTER_SIZE/2;i++)
{
out[i]=(int)SoundProcessor.TmpBuf[i];//SoundProcessor.RfftBuff[FFT_FILTER_SIZE/2 + i/2];
tmp_out[i]=(int32_t)SoundProcessor.TmpBuf[i];
}

free(tmp_out);
GetFillUacOutBuffer ((int8_t*)&tmp_out[0]);

}
