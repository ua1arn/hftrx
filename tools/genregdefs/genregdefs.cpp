// genregdefs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "genregdefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;


struct ddd
{
	char * fldname;
	unsigned fldsize;
	unsigned fldoffs;
	char * comment;
	unsigned fldrept;   // 0 - plain field, 1..n - array
};


void genstruct(const struct ddd * regs, unsigned szregs, const char * bname)
{
	unsigned reservers = 0;
	unsigned i;

	printf("/*\n");
	printf(" * @brief %s\n", bname);
	printf(" */\n");

	printf("/*!< %s Controller Interface */\n", bname);
	printf("typedef struct %s_Type\n", bname);
	printf("{\n");
	unsigned offs = 0;
	for (i = 0; i < szregs; ++ i)
	{
        static const char * fldtypes [] =
        {
            "uint32_t",
            "uint8_t ",
            "uint16_t",
            "uint24_t",
            "uint32_t",
        };

		int commentspos = 54;
		const struct ddd * p = & regs [i];
        char fldtype [256];
        if (p->fldsize >= sizeof fldtypes / sizeof fldtypes [0])
        {
            _snprintf(fldtype, sizeof fldtype / sizeof fldtype [0], "typesize%u", p->fldsize);
        }
        else
        {
             _snprintf(fldtype, sizeof fldtype / sizeof fldtype [0], "%s", fldtypes [p->fldsize]);
       }

		if (p->fldoffs > offs)
		{
			// reserving
			unsigned sz = p->fldoffs - offs;


            if (sz == 4)
            {
			    printf("\t" "uint32_t reserved%u;\n", ++ reservers);
            }
            else if ((sz % 4) == 0)
            {
			    printf("\t" "uint32_t reserved%u [0x%04X];\n", ++ reservers, sz / 4);
            }
            else
            {
		    	printf("\t" "uint8_t reserved%u [0x%04X];\n", ++ reservers, sz);
            }
			offs = p->fldoffs;
		}
		if (p->fldoffs == offs)
		{
			int eolpos;
            if (p->fldrept)
            {
                // Array forming
  			    printf("\t" "__IO %s %s [0x%03X];%n", fldtype, p->fldname, p->fldrept, & eolpos);

 			    offs += p->fldsize * p->fldrept;
            }
            else
            {
                // Plain field
  			    printf("\t" "__IO %s %s;%n", fldtype, p->fldname, & eolpos);
 			    offs += p->fldsize;
            }
			if (eolpos < commentspos)
			{
				int pad = commentspos - eolpos;
				printf("%*.*s", pad, pad, "");
			}
			printf("/*!< Offset 0x%03X %s */\n", p->fldoffs, p->comment);
		}
		else
		{
			printf("#error WRONG offset of field '%s' type '%s' at (0x%03X)\n",  p->fldname, fldtype, p->fldoffs);
			break;
		}
	}
	printf("} %s_TypeDef;\n", bname);
}

static int processfile(const char * file, const char * bname)
{
    FILE * fp = fopen(file, "rt");
    if (fp == NULL)
    {
        printf("#error Can not open file '%s'\n", file);
        return 1;
    }

    size_t maxrows = 1024;
    struct ddd * regs = (struct ddd *) calloc(sizeof (struct ddd), maxrows);
    if (regs == NULL)
    {
        printf("#error Can not allocate memory for file '%s'\n", file);
        return 1;
    }
    size_t nregs;
    for (nregs = 0; nregs < maxrows; ++ nregs)
    {
        char fldname [1024];
        int fldsize;
        char buff [1024];
        char * s0 = fgets(buff, sizeof buff / sizeof buff [0], fp);
        if (s0 == NULL)
            break;
        int f1 = sscanf(s0, "%s %i", fldname, & fldsize);
        if (f1 == 2)
        {
        }
        else if (f1 == 1)
        {
            fldsize = 4;
        }
        else
        {
            printf("#error: wrong format f1=%d, at parse file '%s': '%s'\n", f1, file, buff);
            exit(1);
        }
        regs [nregs].fldsize = fldsize;

         if (strchr(fldname, '\n') != NULL)
            * strchr(fldname, '\n') = '\0';
         if (strchr(fldname, '/') != NULL)
            * strchr(fldname, '/') = '_';
         if (strchr(fldname, '/') != NULL)
            * strchr(fldname, '/') = '_';

         regs [nregs].fldname = strdup(fldname);

        char * s1 = fgets(buff, sizeof buff / sizeof buff [0], fp);
        if (s1 == NULL)
            break;

        int nargs = sscanf(buff, "%i %i", & regs [nregs].fldoffs, & regs [nregs].fldrept);
        switch (nargs)
        {
        case 1:
            regs [nregs].fldrept = 0;
            break;
        case 2:
            break;
        default:
            if (1 != sscanf(buff, "%i", & regs [nregs].fldoffs))
			    printf("WRONG offset format '%s'\n", regs [nregs].fldname);
            break;
        }

        char * s2 = fgets(buff, sizeof buff / sizeof buff [0], fp);
        if (s2 == NULL)
            break;
        if (strchr(s2, '\n') != NULL)
            * strchr(s2, '\n') = '\0';
       regs [nregs].comment = strdup(s2);

    }
    if (nregs == 0)
    {
        regs [nregs].fldoffs = 0;
        regs [nregs].fldsize = 4;
        regs [nregs].fldname = strdup("DUMMY");
       regs [nregs].comment = strdup("Dummy field definition");

        nregs = 1;
    }
	genstruct(regs, nregs, bname);

    unsigned i;
    for (i = 0; i < nregs; ++ i)
    {
        free(regs [i].fldname);
        free(regs [i].comment);
    }
    free(regs);
    return 0;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    if (argc < 3)
        return 1;
    processfile(argv [1], argv [2]);
	return 0;
}


