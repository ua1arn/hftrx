#include "hardware.h"

#if WITHINTEGRATEDDSP && 0

#include "audio.h"
#include "SpeechProc.h"
//#include "SpeechProcIIRcoef.h"
//#include "dsp_to_system.h"

phase_rotator_t ph_rot;
symdet_t symmetry_detector_param;
compparam_t CB [N_POLOS];
compparam_t SoftKneeComp;
compparam_t Limitter;
float WHistoryIir [N_POLOS] [MWSPT_NSEC] [2];
eqparam_t EqBand [N_POLOS];
eqparam_t PostEqBand [N_POLOS];
const unsigned eq_fmin [N_POLOS] = { BAND1_MIN_FREQ, BAND2_MIN_FREQ, BAND3_MIN_FREQ, BAND4_MIN_FREQ, BAND5_MIN_FREQ };
const unsigned eq_fmax [N_POLOS] = { BAND1_MAX_FREQ, BAND2_MAX_FREQ, BAND3_MAX_FREQ, BAND4_MAX_FREQ, BAND5_MAX_FREQ };
float eq_gain [N_POLOS] = { 0, 0, 0, 0, 0 };
compparam_t FinalCB;
phase_rotator_t phrot;
float post_eq_gain [N_POLOS] = { - 20, - 10, 30, 30, 30 };
//const float BeTanParam[N_POLOS]={0.01439996f,0.02487607f, 0.06554346f,0.15838444f,0.19891237};

inline float AmpTodB(float in)
{
	return 20 * log10f(in);
}

inline float dBToAmp(float in)
{
	return powf(10.0f, in / 20);
}

void PhRotator(float *in, unsigned cnt, unsigned fs, unsigned fc, unsigned nstages, phase_rotator_t *p)
{
	float tmp = (float) fs;
	p->fc = (float) fc;
	p->nstages = nstages;
	p->g = tanf(Pi * p->fc / tmp);
	p->b [0] = (p->g - 1) / (p->g + 1);
	p->b [1] = 1;
	p->a [1] = p->b [0];
	int i, n;
	for (i = 0; i < cnt; i ++)
	{
		p->x [0] [0] = in [i];
		for (n = 0; n < p->nstages; n ++)
		{
			if (n > 0)
				p->x [0] [n] = p->y [0] [n - 1];
			p->y [0] [n] = p->b [0] * p->x [0] [n] + p->b [1] * p->x [1] [n] - p->a [1] * p->y [1] [n];
			p->y [1] [n] = p->y [0] [n];
			p->x [1] [n] = p->x [0] [n];
		}
		in [i] = p->y [0] [p->nstages - 1];
	}
}

void GetGaindB_f32(float *in, unsigned cnt, int dB)
{
	float tmp;
	int i;
	tmp = dBToAmp((float) dB);
	for (i = 0; i < cnt; i ++)
		in [i] *= tmp;
}
void SpeechCompressorInit(compparam_t *param, unsigned type_comp)
{
	/*
	 type_comp - 0 - обычный, 1 - софт кни
	 Я не стал заморачиваться и делать разные структуры в
	 зависимости от типа компрессора. Поэтому на оба типа - хард и софт
	 функция одна
	 */
	param->sr = ARMI2SRATE;

	param->slope = 1 - 1 / param->ratio;
	param->max_level_dB = AmpTodB(MAX_16bit_FLOAT);
	param->threshold = dBToAmp(param->max_level_dB - fabs(param->threshold_dB));
	param->att = (param->tatt == 0.0) ? (0.0) : expf( - 1.0 / ((float) param->sr * param->tatt * 1e-3));
	param->rel = (param->trel == 0.0) ? (0.0) : expf( - 1.0 / ((float) param->sr * (float) param->trel * 1e-3));

	if (type_comp == 1)
	{
		param->knee_width_dB = param->threshold_dB * param->knee * ( - 1.0f);
		param->knee_low_bound_dB = param->threshold_dB - (param->knee_width_dB / 2.0f);
		param->knee_up_bound_dB = param->threshold_dB + (param->knee_width_dB / 2.0f);

		param->knee_width = dBToAmp(param->max_level_dB - fabs(param->knee_width_dB));
		param->knee_low_bound = dBToAmp(param->max_level_dB - fabs(param->knee_low_bound_dB));
		param->knee_up_bound = dBToAmp(param->max_level_dB - fabs(param->knee_up_bound_dB));

	}
}

void SpeechCompressor(float *wav_in,     // сигнал
		int n,          // количество сэмплов
		compparam_t *param, unsigned comp_type)
{
	float tmp;
	float alpha = COMP_SMOOTH_FACTOR;
	float beta = 1 - alpha;
	for (int i = 0; i < n; ++ i)
	{
		if (wav_in [i] < 0)
			tmp = ( - 1) * wav_in [i];
		else
			tmp = wav_in [i];
		if (param->env < tmp)
		{
			param->env = param->env * param->att + (1 - param->att) * tmp;
		}
		else
		{
			param->env = param->env * param->rel + (1 - param->rel) * tmp;
		}
		switch (comp_type)
		{
		case 0:
		{
			// простой 1:N компрессор
			if (param->env > param->threshold)
				param->gain = powf((param->threshold / param->env), (param->slope));
			else
				param->gain = 1;
		}
			break;
		case 1:
		{

			param->env_dB = (AmpTodB(param->env) - param->max_level_dB);
			if (param->knee_width_dB > 0.0f && param->env_dB > param->knee_low_bound_dB
					&& param->env_dB < param->knee_up_bound_dB)  // Soft knee
			{
				tmp = param->slope * (((param->env_dB - param->knee_low_bound_dB) / param->knee_width_dB) * 0.5f);
				param->gain_dB = tmp * (param->knee_low_bound_dB - param->env_dB);
				param->gain = dBToAmp(param->gain_dB);
			}
			else
				param->gain = powf((param->threshold / param->env), (param->slope));

		}
			break;
		default:
			break;
		}
		if (param->gain > 1)
			param->gain = 1;
		wav_in [i] *= param->gain;
		param->ave_gain = param->ave_gain * alpha + param->gain * beta;
	}
}

void Equalizer(float *in, unsigned n, eqparam_t *param, unsigned fs, unsigned f_min, unsigned f_max, int gain)
{
	int i;
	float y;
	float tmp;
	tmp = (float) fs;
	param->Df = (float) (f_max - f_min);
	param->f0 = (float) (f_max + f_min) / 2;
	if (gain == 0)
	{
		param->a [0] = param->b [0] = 1;
		param->a [1] = param->a [2] = param->b [1] = param->b [2] = 0;
	}
	else
	{
		param->G = dBToAmp((float) gain);
		param->GB = sqrtf(param->G);
		param->be = sqrtf((param->GB * param->GB - 1) / (param->G * param->G - param->GB * param->GB))
				* tanf(Pi * param->Df / tmp);
		param->c0 = cosf(2 * Pi * param->f0 / tmp);
		param->a [0] = 1;
		param->a [1] = - 2 * param->c0 / (1 + param->be);
		param->a [2] = (1 - param->be) / (1 + param->be);
		param->b [0] = (1 + param->G * param->be) / (1 + param->be);
		param->b [1] = - 2 * param->c0 / (1 + param->be);
		param->b [2] = (1 - param->G * param->be) / (1 + param->be);
	}
	for (i = 0; i < n; i ++)
	{
		param->w [0] = in [i] - param->a [1] * param->w [1] - param->a [2] * param->w [2];
		y = param->b [0] * param->w [0] + param->b [1] * param->w [1] + param->b [2] * param->w [2];
		param->w [2] = param->w [1];
		param->w [1] = param->w [0];
		in [i] = y;
	}
}

float32_t iir_filter(float new_sample, float (*WHistory) [2], const float (*aNUM) [3], const float (*aDEN) [3])
{
	int i;
	for (i = 0; i < MWSPT_NSEC; i ++)
	{
		float32_t buf;
		/*
		 Ak - Denominator
		 Bk-Numerator
		 */
		int j;
		buf = new_sample;
		for (j = 1; j < DL [i]; j ++)
		{
			buf -= aDEN [i] [j] * WHistory [i] [j - 1];
		}
		//buf *= 1 / aDEN[i][0];
		buf /= aDEN [i] [0];
		new_sample = buf * aNUM [i] [0];
		for (j = 1; j < NL [i]; j ++)
		{
			new_sample += aNUM [i] [j] * WHistory [i] [j - 1];
		}
		//*(float32_t*)(WHistory+(2*i)+1)=*(float32_t*)(WHistory+(2*i));
		WHistory [i] [1] = WHistory [i] [0];
		//*(float32_t*)(WHistory+(2*i)) = BUF;
		WHistory [i] [0] = buf;
	}
	return new_sample;
}

void multiband_compressor(float *in, unsigned n, int threshold_dB, float ratio)
{
	int i;
	for (i = 0; i < N_POLOS; i ++)
	{

		CB [i].threshold_dB = options.mb_comp_threshold_dB [i];
		CB [i].tatt = options.mb_comp_tatt [i];
		CB [i].trel = options.mb_comp_trel [i];
		CB [i].ratio = options.mb_comp_ratio [i];
		SpeechCompressorInit( & CB [i], 0);
	}

	float tmp [N_POLOS];
	int j;
	for (j = 0; j < n; j ++)
	{
		tmp [0] = iir_filter(in [j], WHistoryIir [0], NUM0_110, DEN0_110);
		tmp [1] = iir_filter(in [j], WHistoryIir [1], NUM110_300, DEN110_300);
		tmp [2] = iir_filter(in [j], WHistoryIir [2], NUM300_800, DEN300_800);
		tmp [3] = iir_filter(in [j], WHistoryIir [3], NUM800_2000, DEN800_2000);
		tmp [4] = iir_filter(in [j], WHistoryIir [4], NUM2000_, DEN2000_);
		for (i = 0; i < N_POLOS; i ++)
			SpeechCompressor( & tmp [i], 1, & CB [i], 0);

		in [j] = tmp [0];
		for (i = 1; i < N_POLOS; i ++)
			in [j] += tmp [i];
	}
}

void SymmetryDetector(float *in, unsigned cnt, symdet_t *p)
{
	int i;
	p->alpha = SYMDETSMOOTH;
	float tmp;
	for (i = 0; i < cnt; i ++)
	{

		if (in [i] >= 0)
			p->pos_ave += in [i] * in [i];  //p->pos_ave=p->alpha*p->pos_ave+(1-p->alpha)*in[i];
		else
			p->neg_ave += in [i] * in [i];  //=p->alpha*p->neg_ave+(1-p->alpha)*in[i];
		p->cnt ++;
		if (p->cnt == ARMI2SRATE / 10)
		{
			p->cnt = 0;
			if (p->neg_ave != 0)
				tmp = (p->pos_ave / p->neg_ave);
			else
				tmp = 0;

			p->symmetry = 0.9 * p->symmetry + 0.1 * tmp;

			p->pos_ave = p->neg_ave = 0;
		}
	}
}

void SpeechProc(float *input, unsigned n)
{
	int i;

	SoftKneeComp.threshold_dB = options.soft_knee_comp_threshold_dB;
	SoftKneeComp.tatt = options.soft_knee_comp_tatt;
	SoftKneeComp.trel = options.soft_knee_comp_trel;
	SoftKneeComp.ratio = options.soft_knee_comp_ratio;
	SoftKneeComp.knee = options.soft_knee;
	SpeechCompressorInit( & SoftKneeComp, 1);

	Limitter.threshold_dB = - 3;
	Limitter.tatt = 0.1;  //options.soft_knee_comp_tatt;
	Limitter.trel = 50;  //options.soft_knee_comp_trel;
	Limitter.ratio = 100.0f;  //options.soft_knee_comp_ratio;
	SpeechCompressorInit( & Limitter, 0);

	if (options.ph_rot_en)
	{
		PhRotator(input, n,
		ARMI2SRATE, options.ph_rot_freq, options.ph_rot_stage, & phrot);
		SymmetryDetector(input, n, & symmetry_detector_param);
		ObmSpi.SymmetrySignalCoef = symmetry_detector_param.symmetry;  //передал значение для вывода на дисплей
	}

	GetGaindB_f32(input, n, options.pre_gain);

	if (options.mb_comp_en)
	{
		multiband_compressor(input, n, ( - 1) * options.Tx_compressor_max_gain, (float) options.Tx_compressor_coef);
		for (i = 0; i < N_POLOS; i ++)
		{
			CB [i].ave_gain_dB = (int) (AmpTodB(CB [i].ave_gain));
			ObmSpi.mb_comp_env [i] = CB [i].ave_gain_dB;
		}
	}

//GetGaindB_f32 (input, n, options.post_gain);

	if (options.eq_en)
		for (i = 0; i < N_POLOS; i ++)
			Equalizer(input, n, & PostEqBand [i], ARMI2SRATE, eq_fmin [i], eq_fmax [i], options.eq_gain [i]);

	if (options.soft_knee_comp_en)
	{
		SpeechCompressor(input, n, & SoftKneeComp, 1);
		SoftKneeComp.ave_gain_dB = (int) (AmpTodB(SoftKneeComp.ave_gain));
		ObmSpi.soft_knee_comp_env = SoftKneeComp.ave_gain_dB;
	}
	GetGaindB_f32(input, n, options.post_gain);

	SpeechCompressor(input, n, & Limitter, 0);
}

#endif /* WITHINTEGRATEDDSP */
