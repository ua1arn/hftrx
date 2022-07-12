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
	char * typname;
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
			    printf("\t" "uint32_t reserved%u; /* at 0x%03X */\n", ++ reservers, offs);
            }
            else if ((sz % 4) == 0)
            {
			    printf("\t" "uint32_t reserved%u [0x%04X]; /* at 0x%03X */\n", ++ reservers, sz / 4, offs);
            }
            else
            {
		    	printf("\t" "uint8_t reserved%u [0x%04X]; /* at 0x%03X */\n", ++ reservers, sz, offs);
            }
			offs = p->fldoffs;
		}
		if (p->fldoffs == offs)
		{
			int eolpos;
            if (p->fldrept)
            {
                // Array forming
				if (p->typname != NULL)
				{
	  			    printf("\t" "%s %s [0x%03X];%n", p->typname, p->fldname, p->fldrept, & eolpos);
				}
				else
				{
	  			    printf("\t" "__IO %s %s [0x%03X];%n", fldtype, p->fldname, p->fldrept, & eolpos);
				}

 			    offs += p->fldsize * p->fldrept;
            }
            else
            {
                // Plain field
				if (p->typname != NULL)
				{
	  			    printf("\t" "%s %s;%n", p->typname, p->fldname, & eolpos);
				}
				else
				{
	  			    printf("\t" "__IO %s %s;%n", fldtype, p->fldname, & eolpos);
				}
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
	printf("} %s_TypeDef; /* size of structure = 0x%03X */\n", bname, offs);
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
	//fprintf(stderr, "Parse file\n");
    size_t nregs;
    for (nregs = 0; nregs < maxrows; ++ nregs)
    {
        char fldname [1024];
        char typname [1024];
        int fldsize;
        char buff [1024];
        char * s0 = fgets(buff, sizeof buff / sizeof buff [0], fp);
        if (s0 == NULL)
            break;
        int f1 = sscanf(s0, "%s %i %[*a-zA-Z_]s ", fldname, & fldsize, typname);
        if (f1 == 3)
        {
			//fprintf(stderr, "fld3 '%s' '%s'\n", s0, typname);
			regs [nregs].typname = strdup(typname);
        }
        else if (f1 == 2)
        {
			//fprintf(stderr, "fld2 '%s'\n", s0);
			regs [nregs].typname = NULL;
        }
        else if (f1 == 1)
        {
			//fprintf(stderr, "fld1 '%s'\n", s0);
			regs [nregs].typname = NULL;
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
	//fprintf(stderr, "Generate structure\n");
	genstruct(regs, nregs, bname);

	//fprintf(stderr, "Release memory\n");
    unsigned i;
    for (i = 0; i < nregs; ++ i)
    {
        free(regs [i].fldname);
        free(regs [i].typname);
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


