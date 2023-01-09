// genregdefs.c 
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct ddd
{
	char * fldname;
	char * typname;
	unsigned fldsize;	/* 0 - need align to offset (end paddings) */
	unsigned fldoffs;
	char * comment;
	unsigned fldrept;   // 0 - plain field, 1..n - array
};

void genstruct(const struct ddd * regs, unsigned szregs, const char * bname)
{
	unsigned i;
	unsigned offs;

	printf("/*\n");
	printf(" * @brief %s\n", bname);
	printf(" */\n");

	printf("/*!< %s Controller Interface */\n", bname);
	printf("typedef struct %s_Type\n", bname);
	printf("{\n");
	offs = 0;
	for (i = 0; i < szregs; ++ i)
	{
        static const char * fldtypes [] =
        {
            "uint32_t",
            "uint8_t ",
            "uint16_t",
            "uint24_t",
            "uint32_t",
            "uint40_t",
            "uint48_t",
            "uint56_t",
            "uint64_t",
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

		if (p->fldoffs > offs || p->fldsize == 0)
		{
			// reserving
			const unsigned sz = p->fldoffs - offs;


            if (sz == 4)
            {
			    printf("\t" "uint32_t reserved_0x%03X;\n", offs);
            }
            else if ((sz % 4) == 0)
            {
			    printf("\t" "uint32_t reserved_0x%03X [0x%04X];\n", offs, sz / 4);
            }
            else
            {
		    	printf("\t" "uint8_t reserved_0x%03X [0x%04X];\n", offs, sz);
            }
			offs = p->fldoffs;
		}
		if (p->fldoffs == offs)
		{
			if (p->fldsize != 0)
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
		}
		else
		{
			printf("#error WRONG offset of field '%s' type '%s' at (0x%03X)\n",  p->fldname, fldtype, p->fldoffs);
			break;
		}
	}
	printf("} %s_TypeDef; /* size of structure = 0x%03X */\n", bname, offs);
}

enum { BASE_MAX = 32 };

struct parsedfile
{
    size_t nregs;
    struct ddd * regs;
	char bname [64];
	int base_count;
	unsigned base_array [BASE_MAX];
	char base_names [BASE_MAX] [64];
	int irq_count;
	//unsigned irq_array [BASE_MAX];
	//char irq_names [BASE_MAX] [64];
};

static char * commentfgets(struct parsedfile * pfl, char * buff, size_t n, FILE * fp)
{
	char * s;
	for (;pfl->base_count < BASE_MAX;)
	{
        int f1;
		s = fgets(buff, n, fp);
		if (s == NULL)
			break;
		if (s [0] != '#')
			break;
		f1 = sscanf(s + 1, "type %[*a-zA-Z_0-9]s", pfl->bname);
		if (f1 != 1)
		{
			int f2 = sscanf(s + 1, "base %s %x", pfl->base_names [pfl->base_count], & pfl->base_array [pfl->base_count]);
			if (f2 == 2)
			{
				++ pfl->base_count;
			}
		}
	}
	return s;
}

// return 0: 0k
static int parseregister(struct parsedfile * pfl, struct ddd * regp, FILE * fp, const char * file)
{
    char fldname [64];
    char typname [64];
    int fldsize;
    char b0 [1024];
	static const char SEP [] = ";";
    char * token;
    char * s0;
    int f1;
    char * s1;
    int nargs;
    char * s2;

    char * const s0z = commentfgets(pfl, b0, sizeof b0 / sizeof b0 [0], fp);
    if (s0z == NULL)
        return 1;
	token = strtok(s0z, SEP);
	if (token == NULL)
		return 1;

	s0 = token;
 	while (isspace((unsigned char) * s0))
		++ s0;

    f1 = sscanf(s0, "%s %i %[*a-zA-Z_0-9]s ", fldname, & fldsize, typname);
    if (f1 == 3)
    {
		//fprintf(stderr, "fld3 '%s' '%s'\n", s0, typname);
		regp->typname = strdup(typname);
    }
    else if (f1 == 2)
    {
		//fprintf(stderr, "fld2 '%s'\n", s0);
		regp->typname = NULL;
    }
    else if (f1 == 1)
    {
		//fprintf(stderr, "fld1 '%s'\n", s0);
		regp->typname = NULL;
        fldsize = 4;
    }
    else
    {
        printf("#error: wrong format f1=%d, at parse file '%s': '%s'\n", f1, file, s0);
        exit(1);
    }
    regp->fldsize = fldsize;

     if (strchr(fldname, '\n') != NULL)
        * strchr(fldname, '\n') = '\0';
     if (strchr(fldname, '/') != NULL)
        * strchr(fldname, '/') = '_';
     if (strchr(fldname, '/') != NULL)
        * strchr(fldname, '/') = '_';

	regp->fldname = strdup(fldname);

 	s1 = strtok(NULL, SEP);
	//char * const s1 = commentfgets(b1, sizeof b1 / sizeof b1 [0], fp);
    if (s1 == NULL)
        return 1;

	while (isspace((unsigned char) * s1))
		++ s1;

    nargs = sscanf(s1, "%i %i", & regp->fldoffs, & regp->fldrept);
    switch (nargs)
    {
    case 1:
        regp->fldrept = 0;
        break;
    case 2:
        break;
    default:
        if (1 != sscanf(s1, "%i", & regp->fldoffs))
			printf("WRONG offset format '%s'\n", regp->fldname);
        break;
    }

  	s2 = strtok(NULL, SEP);
	//char * const s2 = commentfgets(b2, sizeof b2 / sizeof b2 [0], fp);
    if (s2 == NULL)
        return 1;
	while (isspace((unsigned char) * s2))
		++ s2;

    if (strchr(s2, '\n') != NULL)
        * strchr(s2, '\n') = '\0';
   regp->comment = strdup(s2);

//   const size_t n0 = strleneol(s0);
//   const size_t n1 = strleneol(s1);
//   const size_t n2 = strleneol(s2);
//   fprintf(fpregen, "%*.*s; %*.*s; %*.*s\n", n0, n0, s0, n1, n1, s1, n2, n2, s2);
    return 0;

}

static int loadregs(struct parsedfile * pfl, const char * file)
{
    size_t nregs;
    struct ddd * regs;


    const size_t maxrows = 512;
    FILE * fp = fopen(file, "rt");
 
	strcpy(pfl->bname, "UNNAMED");
	pfl->base_count = 0;
	pfl->irq_count = 0;
	pfl->regs = NULL; 
	pfl->nregs = 0; 
	
	if (fp == NULL)
    {
        printf("#error Can not open file '%s'\n", file);
        return 1;
    }

    regs = (struct ddd *) calloc(sizeof (struct ddd), maxrows);
    if (regs == NULL)
    {
        printf("#error Can not allocate memory for file '%s'\n", file);
        return 1;
    }

	//fprintf(stderr, "Parse file\n");
    for (nregs = 0; nregs < maxrows; ++ nregs)
    {
		struct ddd * regp = & regs [nregs];
		if (parseregister(pfl, regp, fp, file))
			break;
    }

	pfl->regs = regs; 
	pfl->nregs = nregs; 

	return 0;
}


static void processfile_periphregs(struct parsedfile * pfl)
{

	if (pfl->nregs != 0)
	{

		//fprintf(stderr, "Generate structure\n");
		genstruct(pfl->regs, pfl->nregs, pfl->bname);
	}
}

static void processfile_base(struct parsedfile * pfl)
{

	/* print base addresses */
	int i;

	for (i = 0; i < pfl->base_count; ++ i)
	{
		fprintf(stdout, "#define\t%s_BASE\t0x%08X\n", pfl->base_names [i], pfl->base_array [i]);
	}
}


static void processfile_access(struct parsedfile * pfl)
{

	/* print acces pointers */
	int i;

	for (i = 0; i < pfl->base_count; ++ i)
	{
		fprintf(stdout, "#define\t%s\t((%s_TypeDef *) %s_BASE)\t/*!< \\brief %s Interface register set access pointer */\n", pfl->base_names [i], pfl->bname, pfl->base_names [i], pfl->base_names [i]);
	}
}


static void processfile_irq(struct parsedfile * pfl)
{

	/* print base pointers */
	int i;
	for (i = 0; i < pfl->irq_count; ++ i)
	{
		//fprintf(stdout, "#define\t%s\t((%s_TypeDef *) %s_BASE)\t/*!< \\brief %s Interface register set access pointer */\n", pfl->base_names [i], pfl->bname, pfl->base_names [i], pfl->base_names [i]);
	}
}

static void freeregs(struct parsedfile * pfl)
{
    unsigned i;
	//fprintf(stderr, "Release memory\n");
    for (i = 0; i < pfl->nregs; ++ i)
    {
        free(pfl->regs [i].fldname);
        free(pfl->regs [i].typname);
        free(pfl->regs [i].comment);
    }
    free(pfl->regs);
}

#define MAXPARSEDFILES 256

int main(int argc, char* argv[], char* envp[])
{
	struct parsedfile pfls [MAXPARSEDFILES];
	int i = 1;
	int nperoiph;

    if (argc < 2)
        return 1;

	/* Load files */
	for (nperoiph = 0; nperoiph < MAXPARSEDFILES && i < argc; ++ i, ++ nperoiph)
	{
		struct parsedfile * const pfl = & pfls [nperoiph];

		if (loadregs(pfl, argv [i]))
			continue;
	}

	/* structures */
	for (i = 0; i < nperoiph; ++ i)
	{
		struct parsedfile * const pfl = & pfls [i];

		processfile_periphregs(pfl);
	}

	if (0)
	{

		fprintf(stdout, "\n");
		fprintf(stdout, "/* IRQs */\n");
		fprintf(stdout, "\n");

		for (i = 0; i < nperoiph; ++ i)
		{
			struct parsedfile * const pfl = & pfls [i];

			processfile_irq(pfl);
		}
	}

	if (0)
	{

		fprintf(stdout, "\n");
		fprintf(stdout, "/* Peripheral and RAM base address */\n");
		fprintf(stdout, "\n");

		for (i = 0; i < nperoiph; ++ i)
		{
			struct parsedfile * const pfl = & pfls [i];

			processfile_base(pfl);
		}
	}

	if (0)
	{
		fprintf(stdout, "\n");
		fprintf(stdout, "/* Access pointers */\n");
		fprintf(stdout, "\n");

		for (i = 0; i < nperoiph; ++ i)
		{
			struct parsedfile * const pfl = & pfls [i];

			processfile_access(pfl);
		}
	}

	/* release memory */
	for (i = 0; i < nperoiph; ++ i)
	{
		struct parsedfile * const pfl = & pfls [i];

		freeregs(pfl);
	}
	return 0;
}


