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
#define MAXHARMONICS 50

static int
checkvisible(unsigned freqmin, unsigned freqmax, unsigned fdcdc)
{
    unsigned harmonics;
    for (harmonics = 1; harmonics < MAXHARMONICS; ++ harmonics)
    {
        const unsigned f = fdcdc * harmonics;
        if (f >= freqmin && f <= freqmax)
            return 1;
    }
    return 0;
}

/* if no suitable dcdc frequency - return n */
static unsigned finddivider(
    unsigned freqmin, unsigned freqmax,
    const unsigned * frequs,    /* frequencies table */
    unsigned n
    )
{
    unsigned i;
    if (freqmin >= freqmax)
    {
        fprintf(stderr, "Strange check range (%u..%u)\n", freqmin, freqmax);
        exit(1);
    }
    for (i = 0; i < n; ++ i)
    {
        if (! checkvisible(freqmin, freqmax, frequs [i]))
            break;
    }
    return i;
}

static void generaterow(
    unsigned divider,
    unsigned f1, unsigned f2,
    unsigned dcdc,
    int visible,
    FILE * fp
    )

{
    fprintf(fp, "\t{ %-3u, %-8u, %-8u, },\t/* dcdc=%u Hz visible=%s */\n", divider, f1, f2, dcdc, visible ? "yes" : "no");
}

void buildtable(
    unsigned fsync,             /* dcdc synchronisation divider input frewquency */
    unsigned dcdcfmin, unsigned dcdcfmax,   /* dc-dc frequencies range */
    unsigned rxmin, unsigned rxmax,     /* work frrequencies range */
    unsigned wflwidth,
    FILE * fp
    )          /* spectrum width */
{
    unsigned i;
    // допустимые делители и результирующие частоты преобразования
    unsigned divs [DCDCPOINTS];     // in order frequency maximal to minimal
    unsigned dcdcfrequs [DCDCPOINTS];   // in order frequency maximal to minimal
    
    const unsigned dcdcdivmin = calcdivround2(fsync, dcdcfmin);
    const unsigned dcdcdivmax = calcdivround2(fsync, dcdcfmax);
    const unsigned dcdcdivn = dcdcdivmin - dcdcdivmax + 1;

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
    fprintf(fp, "/* fsync=%u, wflwidth=%u */\n", fsync, wflwidth);
    fprintf(fp, "/* number of dividers=%u %u..%u */\n", dcdcdivn, dcdcdivmax, dcdcdivmin);
    fprintf(fp, "/* Analyze up to %u harmonics. */\n", MAXHARMONICS);
	fprintf(fp, "static const FLASHMEM struct FREQ freqs [] = {\n");

    // Fill frequencies tble
    // in order frequency maximal to minimal
    for (i = 0; i < dcdcdivn; ++ i)
    {

        divs [i] = (dcdcdivmax + i);
        dcdcfrequs [i] = fsync / divs [i];
        //printf("freq: divider=%-4u freq=%u\n", divs [i], dcdcfrequs [i]);
    }

    // 
    unsigned actindex = UINT_MAX;
    unsigned fbottom = rxmin;
    unsigned ftop = rxmax;
    unsigned f;
    unsigned df = wflwidth / 2;
    for (f = rxmin; f < rxmax;)
    {
        const unsigned fmin = f < df ? f : f - df;
        const unsigned fmax = (f + df) >= rxmax ? rxmax : f + df;
        const unsigned index = finddivider(fmin, fmax, dcdcfrequs, dcdcdivn);

        //printf("Analyze %u..%u (%u)\n", fmin, fmax, actindex);
        if (index == dcdcdivn)
        {
            // no suitable dividers
            if (actindex != UINT_MAX)
            {
                /* end previously opened interval, found next */
                generaterow(divs [actindex], fbottom, ftop, dcdcfrequs [actindex], checkvisible(fbottom, ftop, dcdcfrequs [actindex]), fp);
                actindex = UINT_MAX;
            }
            f += 1; 
        }
        else if (actindex == UINT_MAX)
        {
            //printf("Open interval\n");
            // have suitable divider, no opened interval
            fbottom = f;
            ftop = f;
            actindex = index;
            f = fmax;
        }
        else if (actindex != index)
        {
            /* end previously opened interval, found next */
            generaterow(divs [actindex], fbottom, ftop, dcdcfrequs [actindex], checkvisible(fbottom, ftop, dcdcfrequs [actindex]), fp);
            fbottom = f;
            ftop = f;
            actindex = index;
            //f = ftop;

        }
        else
        {
            //printf("Keep interval, f=%u, actindex=%u, index=%u\n", f, actindex, index);
            // keep range
            ftop = fmax;
            f = fmax;
       }
    }
    if (actindex != UINT_MAX)
    {
        generaterow(divs [actindex], fbottom, rxmax, dcdcfrequs [actindex], checkvisible(fbottom, rxmax, dcdcfrequs [actindex]), fp);
    }
	fprintf(fp, "};\n");
    //fprintf(fp, "#define DCDC_DEFAULTDIV %u /* divider for ranges without silent dividers */\n", divs [0]);
}

int main(int argc, char* argv[])
{
    const unsigned bw = 96000 * 1;
     buildtable(50000000, 400000, 1200000, 30000, 54000000, bw, stdout);    /* Allwinner t113-s3 */
//    buildtable(16000000, 400000, 1200000, 30000, 54000000, bw, stdout);  /* STM32H7 */
//   buildtable(15000000, 400000, 1200000, 30000, 54000000, bw, stdout);  /* Renesas RZA1/L @360 MHz */
 //  buildtable(122880000 / 2, 400000, 1200000, 30000, 54000000, bw, stdout);  /* Zynq 7020 */

	return 0;
}
