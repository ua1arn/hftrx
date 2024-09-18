#include "hardware.h"

#if WITHTX && AUTOTUNER && WITHAUTOTUNER_N7DDCALGO

#include "formats.h"
#include "board.h"

static unsigned char ind = 0, cap = 0, SW = 0, step_cap = 0, step_ind = 0,
		L_linear = 0;
static unsigned char C_linear = 0, L_q = 7, C_q = 7;
unsigned char L_mult = 1, C_mult = 1, P_High = 0, K_Mult = 32;
unsigned char Overload = 0, Loss_ind = 0, Relay_off = 0;

static int min_for_start, max_for_start, max_swr;
static int SWR, P_max, swr_a;

static unsigned lastout_ind;
static unsigned lastout_cap;
static unsigned lastout_SW;

static void set_ind(unsigned char Ind) {
	lastout_ind = Ind;
	n7ddc_settuner(lastout_ind, lastout_cap, lastout_SW);
}

static unsigned lastout_cap;

static void set_cap(unsigned char Cap) {
	lastout_cap = Cap;
	n7ddc_settuner(lastout_ind, lastout_cap, lastout_SW);
}

static void set_indcap(unsigned char Ind, unsigned char Cap) {
	lastout_ind = Ind;
	lastout_cap = Cap;
	n7ddc_settuner(lastout_ind, lastout_cap, lastout_SW);
}

static void set_sw(unsigned char SW) {  // 0 - IN,  1 - OUT
	lastout_SW = SW;
	n7ddc_settuner(lastout_ind, lastout_cap, lastout_SW);
}

static void atu_reset(void) {
	lastout_SW = 0;
	lastout_cap = 0;
	lastout_ind = 0;
	ind = 0;
	cap = 0;
	n7ddc_settuner(lastout_ind, lastout_cap, lastout_SW);
}

//измерение КСВ
static void local_get_swr(void) {
	SWR = n7ddc_get_swr();
	//PRINTF("local_get_swr: SWR=%d\n", SWR);
	return;
}

static void sharp_cap(void) {
	unsigned char range, count, max_range, min_range;
	int min_SWR;
	range = step_cap * C_mult;
	//
	max_range = cap + range;
	if (max_range > 32 * C_mult - 1)
		max_range = 32 * C_mult - 1;
	if (cap > range)
		min_range = cap - range;
	else
		min_range = 0;
	cap = min_range;
	set_cap(cap);
	local_get_swr();
	if (SWR == 0)
		return;
	min_SWR = SWR;
	for (count = min_range + C_mult; count <= max_range; count += C_mult) {
		set_cap(count);
		local_get_swr();
		if (SWR == 0)
			return;

		if (SWR >= min_SWR) {
			local_delay_ms(10);
			local_get_swr();
		}

		if (SWR >= min_SWR) {
			local_delay_ms(10);
			local_get_swr();
		}

		if (SWR < min_SWR) {
			min_SWR = SWR;
			cap = count;
			if (SWR < 120)
				break;
		} else
			break;
	}
	set_cap(cap);
	return;
}

static void sharp_ind(void) {
	unsigned char range, count, max_range, min_range;
	int min_SWR;
	range = step_ind * L_mult;
	//
	max_range = ind + range;
	if (max_range > 32 * L_mult - 1)
		max_range = 32 * L_mult - 1;

	if (ind > range)
		min_range = ind - range;
	else
		min_range = 0;

	ind = min_range;
	set_ind(ind);

	local_get_swr();

	if (SWR == 0)
		return;
	min_SWR = SWR;
	for (count = min_range + L_mult; count <= max_range; count += L_mult) {
		set_ind(count);
		local_get_swr();
		if (SWR == 0)
			return;
		if (SWR >= min_SWR) {
			local_delay_ms(10);
			local_get_swr();
		}

		if (SWR >= min_SWR) {
			local_delay_ms(10);
			local_get_swr();
		}

		if (SWR < min_SWR) {
			min_SWR = SWR;
			ind = count;

			if (SWR < 120)
				break;
		} else
			break;
	}
	set_ind(ind);
	return;
}

static void coarse_cap(void) {
	unsigned char step = 3;
	unsigned char count;
	int min_swr;

	cap = 0;
	set_cap(cap);
	step_cap = step;
	local_get_swr();

	if (SWR == 0)
		return;
	min_swr = SWR + SWR / 20;
	for (count = step; count <= 31;) {
		set_cap(count * C_mult);
		local_get_swr();
		if (SWR == 0)
			return;
		if (SWR < min_swr) {
			min_swr = SWR + SWR / 20;
			cap = count * C_mult;
			step_cap = step;
			if (SWR < 120)
				break;
			count += step;
			if (C_linear == 0 && count == 9)
				count = 8;
			else if (C_linear == 0 && count == 17) {
				count = 16;
				step = 4;
			}
		} else
			break;
	}
	set_cap(cap);
	return;
}

static void coarse_tune(void) {
	unsigned char step = 3;
	unsigned char count;
	unsigned char mem_cap, mem_step_cap;
	int min_swr;

	mem_cap = 0;
	step_ind = step;
	mem_step_cap = 3;
	min_swr = SWR + SWR / 20;
	for (count = 0; count <= 31;) {
		set_ind(count * L_mult);
		coarse_cap();
		local_get_swr();

		if (SWR == 0)
			return;
		if (SWR < min_swr) {
			min_swr = SWR + SWR / 20;
			ind = count * L_mult;
			mem_cap = cap;
			step_ind = step;
			mem_step_cap = step_cap;

			if (SWR < 120)
				break;
			count += step;
			if (L_linear == 0 && count == 9)
				count = 8;
			else if (L_linear == 0 && count == 17) {
				count = 16;
				step = 4;
			}
		} else
			break;
	}
	cap = mem_cap;
	set_indcap(ind, cap);
	step_cap = mem_step_cap;
	return;
}

static void sub_tune(void) {
	int swr_mem, ind_mem, cap_mem;
	//
	swr_mem = SWR;
	coarse_tune();
	if (SWR == 0) {
		atu_reset();
		return;
	}

	local_delay_ms(10);
	local_get_swr();
	if (SWR < 120)
		return;

	sharp_ind();
	if (SWR == 0) {
		atu_reset();
		return;
	}
	local_get_swr();
	if (SWR < 120)
		return;

	sharp_cap();
	if (SWR == 0) {
		atu_reset();
		return;
	}
	local_get_swr();

	if (SWR < 120)
		return;
	//
	if (SWR < 200 && SWR < swr_mem && (swr_mem - SWR) > 100)
		return;
	swr_mem = SWR;
	ind_mem = ind;
	cap_mem = cap;
	//
	if (SW == 1)
		SW = 0;
	else
		SW = 1;

	atu_reset();
	set_sw(SW);

	local_delay_ms(50);
	local_get_swr();
	if (SWR < 120)
		return;
	//
	coarse_tune();
	if (SWR == 0) {
		atu_reset();
		return;
	}

	local_delay_ms(10);
	local_get_swr();
	if (SWR < 120)
		return;
	sharp_ind();

	if (SWR == 0) {
		atu_reset();
		return;
	}

	local_get_swr();
	if (SWR < 120)
		return;

	sharp_cap();
	if (SWR == 0) {
		atu_reset();
		return;
	}
	local_get_swr();
	if (SWR < 120)
		return;
	//
	if (SWR > swr_mem) {
		if (SW == 1)
			SW = 0;
		else
			SW = 1;
		set_sw(SW);
		ind = ind_mem;
		cap = cap_mem;
		set_indcap(ind, cap);
		SWR = swr_mem;
	}
	return;
}

static void tune(void) {
	P_max = 0;
	//
	local_get_swr();
	if (SWR < 110)
		return;

	atu_reset();

	local_delay_ms(50);
	local_get_swr();
	swr_a = SWR;
	if (SWR < 110)
		return;
	if (max_swr > 110 && SWR > max_swr)
		return;

	sub_tune();

	if (SWR == 0) {
		atu_reset();
		return;
	}
	if (SWR < 120)
		return;

	if (C_q == 5 && L_q == 5)
		return;

	if (L_q > 5) {
		step_ind = L_mult;
		L_mult = 1;
		sharp_ind();
	}
	if (SWR < 120)
		return;
	if (C_q > 5) {
		step_cap = C_mult;  // = C_mult
		C_mult = 1;
		sharp_cap();
	}
	if (L_q == 5)
		L_mult = 1;
	else if (L_q == 6)
		L_mult = 2;
	else if (L_q == 7)
		L_mult = 4;
	if (C_q == 5)
		C_mult = 1;
	else if (C_q == 6)
		C_mult = 2;
	else if (C_q == 7)
		C_mult = 4;
	return;
}

void n7ddc_tune(void) {
	unsigned i;
	PRINTF("n7ddc_tune:\n");
	atu_reset();

	for (i = 0; i < 32; ++ i)
	{
		local_get_swr();
	}

	for (i = 0; i <= 5; i++) //на всякий случай 5 проходов
			{
		PRINTF("n7ddc_tune: START LOOP\n");
		local_get_swr();
		if (SWR <= 120)
			break;
		else
			tune();
	}
	PRINTF("n7ddc_tune: DONE\n");

}

#endif /* WITHTX && AUTOTUNER && WITHAUTOTUNER_N7DDCALGO */
