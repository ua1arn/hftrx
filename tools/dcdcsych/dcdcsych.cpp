// dcdcsych.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


static 
unsigned calcdivround2(
	unsigned ref,	/* частота на входе делителя, в герцах. */
	unsigned freq	/* требуемая частота на выходе делителя, в герцах. */
	)
{
	return (ref < freq) ? 1 : ((ref + freq / 2) / freq);
}

#define DCDCPOINTS 256

static int
checkvisible(unsigned freqmin, unsigned freqmax, unsigned fdcdc)
{
    unsigned harmonics;
    for (harmonics = 1; harmonics < 50; ++ harmonics)
    {
        const unsigned f = fdcdc * harmonics;
        if (f >= freqmin && f <= freqmax)
            return 1;
    }
    return 0;
}

void buildtable(
    unsigned fsync,             /* dcdc synchronisation divider input frewquency */
    unsigned dcdcfmin, unsigned dcdcfmax,   /* dc-dc frequencies range */
    unsigned rxmin, unsigned rxmax,     /* work frrequencies range */
    unsigned wflwidth)          /* spectrum width */
{
    unsigned i;
    // допустимые делители и результирующие частоты преобразования
    unsigned divs [DCDCPOINTS];     // in order frequency maximal to minimal
    unsigned dcdcfrequs [DCDCPOINTS];   // in order frequency maximal to minimal
    
    unsigned dcdcdivmin = calcdivround2(fsync, dcdcfmin);
    unsigned dcdcdivmax = calcdivround2(fsync, dcdcfmax);
    unsigned dcdcdivn = dcdcdivmin - dcdcdivmax;
    if (dcdcdivn == 0)
    {
        fprintf(stderr, "no divider control possible\n");
        return;
    }
    if (dcdcdivn >= DCDCPOINTS)
    {
        fprintf(stderr, "no divider control possible (increase DCDCPOINTS)\n");
        return;
    }
    if (wflwidth > dcdcfmax)
    {
        fprintf(stderr, "Too wide spectum display\n");
        return;
    }
    fprintf(stderr, "dcdcdivn=%u dcdcdivmin=%u dcdcdivmax=%u\n", dcdcdivn, dcdcdivmin, dcdcdivmax);

    // Fill frequencies tble
    // in order frequency maximal to minimal
    for (i = 0; i < dcdcdivn; ++ i)
    {

        divs [i] = (dcdcdivmin - i);
        dcdcfrequs [i] = fsync / divs [i];
        //printf("freq: divider=%-4u freq=%u\n", divs [i], dcdcfrequs [i]);
    }

    // 

}

int main(int argc, char* argv[])
{
    buildtable(50000000, 400000, 1200000, 30000, 54000000, 96000);

	return 0;
}
