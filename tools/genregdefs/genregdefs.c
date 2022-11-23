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

static int option_base;	/* base address generate */

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

static char bname [1024];
static int base_count = 0;
enum { BASE_MAX = 32 };
static unsigned base_array [BASE_MAX];

static char * commentfgets(char * buff, size_t n, FILE * fp)
{
	char * s;
	for (;;)
	{
        int f1;
		s = fgets(buff, n, fp);
		if (s == NULL)
			break;
		if (s [0] != '#')
			break;
		f1 = sscanf(s + 1, "type %[*a-zA-Z_0-9]s", bname);
		if (f1 != 1)
		{
			int f2 = sscanf(s + 1, "base %x", & base_array [base_count]);
			if (f2 == 1 && base_count < BASE_MAX)
			{
				++ base_count;
			}
		}
	}
	return s;
}

// return 0: 0k
static int parseregister(struct ddd * regp, FILE * fp, const char * file)
{
    char fldname [1024];
    char typname [1024];
    int fldsize;
    char b0 [1024];
	static const char SEP [] = ";";
    char * token;
    char * s0;
    int f1;
    char * s1;
    int nargs;
    char * s2;

    char * const s0z = commentfgets(b0, sizeof b0 / sizeof b0 [0], fp);
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

static int processfile(const char * file)
{
    unsigned i;
    size_t nregs;
    struct ddd * regs;
    const size_t maxrows = 1024;
    FILE * fp = fopen(file, "rt");
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
		if (parseregister(regp, fp, file))
			break;
    }
    if (nregs == 0)
    {
        regs [nregs].fldoffs = 0;
        regs [nregs].fldsize = 4;
        regs [nregs].fldname = strdup("DUMMY");
		regs [nregs].comment = strdup("Dummy field definition");

        nregs = 1;
    }

	if (option_base != 0)
	{
		int i;
		for (i = 0; i < base_count; ++ i)
		{
			if (base_count > 1)
			{
				fprintf(stdout, "#define %s%i_BASE 0x%08X\n", bname, i, base_array [i]);
			}
			else
			{
				fprintf(stdout, "#define %s_BASE 0x%08X\n", bname, base_array [i]);
			}
		}
	}
	//fprintf(stderr, "Generate structure\n");
	genstruct(regs, nregs, bname);

	//fprintf(stderr, "Release memory\n");
    for (i = 0; i < nregs; ++ i)
    {
        free(regs [i].fldname);
        free(regs [i].typname);
        free(regs [i].comment);
    }
    free(regs);
    return 0;
}

int main(int argc, char* argv[], char* envp[])
{
	int i = 1;
	strcpy(bname, "UNNAMED");
    if (argc < 2)
        return 1;
	if (strcmp(argv [i], "-b") == 0)
	{
		++ i;
		option_base = 1;
		if (argc < 3)
			return 1;
	}
    processfile(argv [i]);
	return 0;
}


