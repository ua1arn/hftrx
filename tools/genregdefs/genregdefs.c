// genregdefs.c 
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>

#include "mslist.h"
#include "getopt_win.h"

/* debug stuff */
#if 0

	#define TP() do { \
		emitline(0, "%s/%d\n", __FILE__, __LINE__); \
		fflush(stdout); \
		} while (0)

#else

	#define TP() do { \
		} while (0)

#endif

struct regdfn
{
	LIST_ENTRY item;
	char * fldname;
	char * typname;
	unsigned fldsize;	/* 0 - need align to offset (end paddings) */
	unsigned fldoffs;	/* offset inside block */
	char * comment;
	unsigned fldrept;   // 0 - plain field, 1..n - array
};


enum { BASE_MAX = 32 };
enum { VNAME_MAX = 96 };

struct parsedfile
{
	LIST_ENTRY item;
	LIST_ENTRY regslist;
 //   size_t nregs;
 //   struct regdfn * regs;
	char bname [VNAME_MAX];
	int base_count;
	unsigned base_array [BASE_MAX];
	char base_names [BASE_MAX] [VNAME_MAX];

	int irq_count;
	int irq_array [BASE_MAX];
	char irq_names [BASE_MAX] [VNAME_MAX];

	int irqrv_count;
	int irqrv_array [BASE_MAX];
	char irqrv_names [BASE_MAX] [VNAME_MAX];
};

#define INDENT 4
#define COMMENTPOS 80

void emitline(int pos, const char * format, ...)
{
	static int emitpos;
	va_list ap;
	int n;
	FILE * fp = stdout;

	if (emitpos < pos)
	{
		const int pad = pos - emitpos;
		n = fprintf(fp, "%*.*s", pad, pad, "");
		if (n >= 0)
		{
			emitpos += pad;
		}
	}

	va_start(ap, format);
	n = vfprintf(fp, format, ap);
	va_end(ap);
	if (n >= 0)
	{
		emitpos += n;
	}
	if (strchr(format, '\n') != NULL)
		emitpos = 0;
}

/* Generate list of registers. Return last offset */
unsigned genreglist(int indent, const LIST_ENTRY * regslist)
{
	unsigned offs;
	PLIST_ENTRY t;

	offs = 0;
	for (t = regslist->Flink; t != regslist; t = t->Flink)
	{
		const struct regdfn * regp = CONTAINING_RECORD(t, struct regdfn, item);
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
        char fldtype [VNAME_MAX];
        if (regp->fldsize >= sizeof fldtypes / sizeof fldtypes [0])
        {
            _snprintf(fldtype, sizeof fldtype / sizeof fldtype [0], "typesize%u", regp->fldsize);
        }
        else
        {
             _snprintf(fldtype, sizeof fldtype / sizeof fldtype [0], "%s", fldtypes [regp->fldsize]);
		}

		if (regp->fldoffs > offs || regp->fldsize == 0)
		{
			// reserving
			const unsigned sz = regp->fldoffs - offs;


            if (sz == 4)
            {
			    emitline(indent + INDENT + 9, "uint32_t reserved_0x%03X;\n", offs);
            }
            else if ((sz % 4) == 0)
            {
			    emitline(indent + INDENT + 9, "uint32_t reserved_0x%03X [0x%04X];\n", offs, sz / 4);
            }
            else
            {
		    	emitline(indent + INDENT + 9, "uint8_t reserved_0x%03X [0x%04X];\n", offs, sz);
            }
			offs = regp->fldoffs;
		}
		if (regp->fldoffs == offs)
		{
			if (regp->fldsize != 0)
			{
				if (regp->fldrept)
				{
					// Array forming
					if (regp->typname != NULL)
					{
						emitline(indent + INDENT, "%s %s [0x%03X];", regp->typname, regp->fldname, regp->fldrept);
					}
					else
					{
						emitline(indent + INDENT, "volatile %s %s [0x%03X];", fldtype, regp->fldname, regp->fldrept);
					}

					offs += regp->fldsize * regp->fldrept;
				}
				else
				{
					// Plain field
					if (regp->typname != NULL)
					{
						emitline(indent + INDENT, "%s %s;", regp->typname, regp->fldname);
					}
					else
					{
						emitline(indent + INDENT, "volatile %s %s;", fldtype, regp->fldname);
					}
					offs += regp->fldsize;
				}
				emitline(COMMENTPOS, "/*!< Offset 0x%03X %s */\n", regp->fldoffs, regp->comment);
			}
		}
		else
		{
			emitline(0, "#error WRONG offset of field '%s' type '%s' at (0x%03X)\n",  regp->fldname, fldtype, regp->fldoffs);
			break;
		}
	}
	return offs;
}

void genstruct(struct parsedfile * pfl)
{
	unsigned offs;

	emitline(0, "/*\n");
	emitline(0, " * @brief %s\n", pfl->bname);
	emitline(0, " */\n");

	emitline(0, "/*!< %s Controller Interface */\n", pfl->bname);
	emitline(0, "typedef struct %s_Type\n", pfl->bname);
	emitline(0, "{\n");

	offs = genreglist(0, & pfl->regslist);

	emitline(0, "} %s_TypeDef; /* size of structure = 0x%03X */\n", pfl->bname, offs);
}


void genstructprint(struct parsedfile * pfl)
{
	PLIST_ENTRY t;

	emitline(0, "/* Print %s */\n", pfl->bname);
	emitline(0, "static void %s_Type_print(const %s_TypeDef * p, const char * base)\n", pfl->bname, pfl->bname);
	emitline(0, "{\n");
	for (t = pfl->regslist.Flink; t != & pfl->regslist; t = t->Flink)
	{
		const struct regdfn * p = CONTAINING_RECORD(t, struct regdfn, item);

		if (p->fldsize != 0)
		{
			if (p->fldrept && p->typname == NULL)
			{
				// Array forming
				unsigned i;
				for (i = 0; i < 4 && i < p->fldrept; ++ i)
				{
					emitline(INDENT, "PRINTF(\"%%s->%s [%u] = 0x%%08X; /* 0x%%08X */\\n\", base, (unsigned) p->%s [%u], (unsigned) p->%s [%u]);",
							p->fldname, i, p->fldname, i, p->fldname, i);
					emitline(COMMENTPOS, "/*!< Offset 0x%03X %s */\n", p->fldoffs + i * p->fldsize, p->comment);
				}
			}
			else if (p->fldrept == 0 &&p->typname == NULL)
			{
				// Plain field
				emitline(INDENT, "PRINTF(\"%%s->%s = 0x%%08X; /* 0x%%08X */\\n\", base, (unsigned) p->%s, (unsigned) p->%s );", p->fldname, p->fldname, p->fldname);
				emitline(COMMENTPOS, "/*!< Offset 0x%03X %s */\n", p->fldoffs, p->comment);
			}
		}
	}
	emitline(0, "}\n");
}


LIST_ENTRY parsedfiles;

struct basemap
{
	unsigned base;
	char name [VNAME_MAX];
};

struct irqmap
{
	int irq;
	char name [VNAME_MAX];
};

struct irqmaprv
{
	int irqrv;
	char name [VNAME_MAX];
};

/* qsort parameter */
int compare_base(const void * v1, const void * v2)
{
	const struct basemap * p1 = v1;
	const struct basemap * p2 = v2;
	if (p1->base == p2->base)
	{
		return strcmp(p1->name, p2->name);
	}
	return p1->base - p2->base;
}

/* qsort parameter */
int compare_irq(const void * v1, const void * v2)
{
	const struct irqmap * p1 = v1;
	const struct irqmap * p2 = v2;

	if (p1->irq == p2->irq)
	{
		return strcmp(p1->name, p2->name);
	}
	return p1->irq - p2->irq;
}

/* qsort parameter */
int compare_irqrv(const void * v1, const void * v2)
{
	const struct irqmaprv * p1 = v1;
	const struct irqmaprv * p2 = v2;

	if (p1->irqrv == p2->irqrv)
	{
		return strcmp(p1->name, p2->name);
	}
	return p1->irqrv - p2->irqrv;
}

/* Parse line. NULL - unrecognized format */
static char * commentfgets(struct parsedfile * pfl, char * buff, size_t n, FILE * fp)
{
	char dummy [VNAME_MAX];
	char * s;
	for (;;)
	{
        int f2;
		s = fgets(buff, n, fp);
		if (s == NULL)
			break;
		if (s [0] != '#')
		{
			fprintf(stderr, "# # undeined %s", s);
			return NULL;
		}

		f2 = sscanf(s + 1, "regdef; %[*a-zA-Z_0-9]s", dummy);
		if (f2 == 1)
		{
			//emitline(0, "type %s processrd\n", pfl->bname);
			return s + 7;
		}

		f2 = sscanf(s + 1, "type; %[*a-zA-Z_0-9]s", pfl->bname);
		if (f2 == 1)
		{
			//emitline(0, "type %s processrd\n", pfl->bname);
			continue;
		}

		if (pfl->base_count < BASE_MAX)
		{
			f2 = sscanf(s + 1, "base; %s %x", pfl->base_names [pfl->base_count], & pfl->base_array [pfl->base_count]);
			if (f2 == 2)
			{
				++ pfl->base_count;
				//emitline(0, "base %s processrd\n", pfl->bname);
				continue;
			}
		}

		if (pfl->irq_count < BASE_MAX)
		{
			f2 = sscanf(s + 1, "irq; %s %d", pfl->irq_names [pfl->irq_count], & pfl->irq_array [pfl->irq_count]);
			if (f2 == 2)
			{
				++ pfl->irq_count;
				//emitline(0, "irq %s processrd\n", pfl->bname);
				continue;
			}
		}

		if (pfl->irqrv_count < BASE_MAX)
		{
			f2 = sscanf(s + 1, "irqrv; %s %d", pfl->irqrv_names [pfl->irqrv_count], & pfl->irqrv_array [pfl->irqrv_count]);
			if (f2 == 2)
			{
				++ pfl->irqrv_count;
				//emitline(0, "irq %s processrd\n", pfl->bname);
				continue;
			}
		}
		fprintf(stderr, "# # f2=%d undeined %s", f2, s);
	}
	return NULL;
}

// return 0: 0k
static int parseregister(struct parsedfile * pfl, struct regdfn * regp, FILE * fp, const char * file)
{
    char fldname [VNAME_MAX];
    char typname [VNAME_MAX];
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
	//emitline(0, "#R %s", s0z);
	TP();
	token = strtok(s0z, SEP);
	if (token == NULL)
		return 1;

	TP();
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
        emitline(0, "#error: wrong format f1=%d, at parse file '%s': '%s'\n", f1, file, s0);
        exit(1);
    }
    regp->fldsize = fldsize;
	TP();

     if (strchr(fldname, '\n') != NULL)
        * strchr(fldname, '\n') = '\0';
     if (strchr(fldname, '/') != NULL)
        * strchr(fldname, '/') = '_';
     if (strchr(fldname, '/') != NULL)
        * strchr(fldname, '/') = '_';
	TP();

	regp->fldname = strdup(fldname);
	TP();

 	s1 = strtok(NULL, SEP);
	//char * const s1 = commentfgets(b1, sizeof b1 / sizeof b1 [0], fp);
    if (s1 == NULL)
        return 1;

	while (isspace((unsigned char) * s1))
		++ s1;

	TP();
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
			emitline(0, "WRONG offset format '%s'\n", regp->fldname);
        break;
    }
	TP();

  	s2 = strtok(NULL, SEP);
	//char * const s2 = commentfgets(b2, sizeof b2 / sizeof b2 [0], fp);
    if (s2 == NULL)
        return 1;
	TP();
	while (isspace((unsigned char) * s2))
		++ s2;

    if (strchr(s2, '\n') != NULL)
        * strchr(s2, '\n') = '\0';
   regp->comment = strdup(s2);

//   const size_t n0 = strleneol(s0);
//   const size_t n1 = strleneol(s1);
//   const size_t n2 = strleneol(s2);
//   fprintf(fpregen, "%*.*s; %*.*s; %*.*s\n", n0, n0, s0, n1, n1, s1, n2, n2, s2);
 	TP();
   return 0;

}

static int loadregs(struct parsedfile * pfl, const char * file)
{
    const size_t maxrows = 256;
    FILE * fp = fopen(file, "rt");
	TP();
	strcpy(pfl->bname, "");
	pfl->base_count = 0;
	pfl->irq_count = 0;
	pfl->irqrv_count = 0;

	InitializeListHead(& pfl->regslist);
	//pfl->regs = NULL; 
	//pfl->nregs = 0; 
	
	TP();
	if (fp == NULL)
    {
        emitline(0, "#error Can not open file '%s'\n", file);
        return 1;
    }

	TP();
    for (;;)
    {
		struct regdfn * regp = calloc(1, sizeof (struct regdfn));
		if (parseregister(pfl, regp, fp, file))
		{
			free(regp);
			break;
		}
		InsertTailList(& pfl->regslist, & regp->item);
    }
	TP();

	//pfl->regs = regs; 
	//pfl->nregs = nregs; 

	return 0;
}


static void processfile_periphregs(struct parsedfile * pfl)
{

	if (! IsListEmpty(& pfl->regslist))
	{
		genstruct(pfl);
	}
}

static void processfile_periphregsdebug(struct parsedfile * pfl)
{

	if (! IsListEmpty(& pfl->regslist))
	{
		genstructprint(pfl);
	}
}


static int collect_base(struct parsedfile * pfl, int n, struct basemap * v)
{
	/* collect base pointers */
	int i;
	int score = 0;
	for (i = 0; i < pfl->base_count && n --; ++ i, ++ v, ++ score)
	{
		strcpy(v->name, pfl->base_names [i]);
		v->base = pfl->base_array [i];
	}
	return score;
}


static void processfile_access(struct parsedfile * pfl)
{

	/* print acces pointers */
	int i;

	if (IsListEmpty(& pfl->regslist))
		return;
	for (i = 0; i < pfl->base_count; ++ i)
	{
		emitline(0,			"#define %s ((%s_TypeDef *) %s_BASE)", pfl->base_names [i], pfl->bname, pfl->base_names [i]);
		emitline(COMMENTPOS, "/*!< \\brief %s Interface register set access pointer */\n", pfl->base_names [i]);
	}
}


static int collect_irq(struct parsedfile * pfl, int n, struct irqmap * v)
{
	/* collect irq vectors */
	int i;
	int score = 0;
	for (i = 0; i < pfl->irq_count && n --; ++ i, ++ v, ++ score)
	{
		strcpy(v->name, pfl->irq_names [i]);
		v->irq = pfl->irq_array [i];
	}
	return score;
}

static int collect_irqrv(struct parsedfile * pfl, int n, struct irqmaprv * v)
{
	/* collect irq vectors */
	int i;
	int score = 0;
	for (i = 0; i < pfl->irqrv_count && n --; ++ i, ++ v, ++ score)
	{
		strcpy(v->name, pfl->irqrv_names [i]);
		v->irqrv = pfl->irqrv_array [i];
	}
	return score;
}

static void freeregs(struct parsedfile * pfl)
{
	PLIST_ENTRY t;
	//fprintf(stderr, "Release memory\n");
	for (t = pfl->regslist.Flink; t != & pfl->regslist; )
    {
 		struct regdfn * regp = CONTAINING_RECORD(t, struct regdfn, item);
		t = t->Flink;
		free(regp->fldname);
		free(regp->typname);
		free(regp->comment);
		free(regp);
    }
}

int main(int argc, char* argv[], char* envp[])
{
	//struct parsedfile pfls [MAXPARSEDFILES];
	int i = 1;
	//int nperoiph;

    if (argc < 2)
        return 1;

	InitializeListHead(& parsedfiles);

	/* Load files */
	for (;  i < argc;)
	{
		struct parsedfile * const pfl = calloc(1, sizeof (struct parsedfile));
		if (loadregs(pfl, argv [i]) != 0)
		{
			free(pfl);
			break;
		}
		InsertTailList(&parsedfiles, & pfl->item);
		++ i;
	}

	{
		PLIST_ENTRY t;
		for (t = parsedfiles.Flink; t != & parsedfiles; t = t->Flink)
		{
			struct parsedfile * const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
			//fprintf(stderr, "$");
		}
	}


	if (1)
	{
		/* CMSIS header forming */
		char headrname [128];
		_snprintf(headrname, sizeof headrname / sizeof headrname [0], "HEADER_%08X_INCLUDED", (unsigned) time(NULL));

		emitline(0, "#ifndef %s" "\n", headrname);
		emitline(0, "#define %s" "\n", headrname);

		emitline(0, "#include <stdint.h>" "\n");
		emitline(0, "\n");

		if (0)
		{
			/* collect ARM IRQ vectors */
			int nitems = 0;
			struct irqmap irqs [1024];

			{
				PLIST_ENTRY t;
				for (t = parsedfiles.Flink; t != & parsedfiles && nitems < sizeof irqs / sizeof irqs [0]; t = t->Flink)
				{
					struct parsedfile * const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
					nitems += collect_irq(pfl, sizeof irqs / sizeof irqs [0] - nitems, irqs + nitems);
				}
			}

			qsort(irqs, nitems, sizeof irqs [0], compare_irq);

			emitline(0, "\n");
			emitline(0, "/* IRQs */\n");
			emitline(0, "\n");
			emitline(0, "typedef enum IRQn\n");
			emitline(0, "{\n");
			for (i = 0; i < nitems; ++ i)
			{
				struct irqmap * const p = & irqs [i];

				emitline(0, "\t%s_IRQn\t= %d,\n", p->name, p->irq);
			}
			emitline(0, "\n");
			emitline(0, "\t%MAX_IRQ_n,\n");
			emitline(0, "\tForce_IRQn_enum_size\t= %d\t/* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */\n", 1048);
			emitline(0, "} IRQn_Type;\n");
			emitline(0, "\n");
		}

		if (0)
		{
			/* collect RISC-V IRQ vectors */
			int nitems = 0;
			struct irqmaprv irqs [1024];

			{
				PLIST_ENTRY t;
				for (t = parsedfiles.Flink; t != & parsedfiles && nitems < sizeof irqs / sizeof irqs [0]; t = t->Flink)
				{
					struct parsedfile * const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
					nitems += collect_irqrv(pfl, sizeof irqs / sizeof irqs [0] - nitems, irqs + nitems);
				}
			}

			qsort(irqs, nitems, sizeof irqs [0], compare_irqrv);

			emitline(0, "\n");
			emitline(0, "/* IRQs */\n");
			emitline(0, "\n");
			emitline(0, "typedef enum IRQn\n");
			emitline(0, "{\n");
			for (i = 0; i < nitems; ++ i)
			{
				struct irqmaprv * const p = & irqs [i];

				emitline(0, "\t%s_IRQn\t= %d,\n", p->name, p->irqrv);
			}
			emitline(0, "\n");
			emitline(0, "\t%MAX_IRQ_n,\n");
			emitline(0, "\tForce_IRQn_enum_size\t= %d\t/* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */\n", 1048);
			emitline(0, "} IRQn_Type;\n");
			emitline(0, "\n");
		}

		if (1)
		{

			/* collect base addresses */
			int nitems = 0;
			struct basemap maps [256];
			PLIST_ENTRY t;

			for (t = parsedfiles.Flink; t != & parsedfiles; t = t->Flink)
			{
				struct parsedfile * const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
				nitems += collect_base(pfl, 1024 - nitems, maps + nitems);
			}

			qsort(maps, nitems, sizeof maps [0], compare_base);

			emitline(0, "\n");
			emitline(0, "/* Peripheral and RAM base address */\n");
			emitline(0, "\n");

			for (i = 0; i < nitems; ++ i)
			{
				struct basemap * const p = & maps [i];

				emitline(0, "#define\t%s_BASE\t ((uintptr_t) 0x%08X)\n", p->name, p->base);
			}
		}

		if (1)
		{
			/* structures */
			PLIST_ENTRY t;

			for (t = parsedfiles.Flink; t != & parsedfiles; t = t->Flink)
			{
				struct parsedfile * const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
				processfile_periphregs(pfl);
			}
		}

		if (1)
		{
			PLIST_ENTRY t;

			emitline(0, "\n");
			emitline(0, "/* Access pointers */\n");
			emitline(0, "\n");


			for (t = parsedfiles.Flink; t != & parsedfiles; t = t->Flink)
			{
				struct parsedfile * const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
				processfile_access(pfl);
			}
		}

		emitline(0, "#endif /* %s */" "\n", headrname);
	}
	else
	{
		/* Debug header forming */
		char headrname [128];
		PLIST_ENTRY t;

		/* print structire debug */

		_snprintf(headrname, sizeof headrname / sizeof headrname [0], "HEADER_%08X_INCLUDED", (unsigned) time(NULL));

		emitline(0, "#ifndef %s" "\n", headrname);
		emitline(0, "#define %s" "\n", headrname);
		emitline(0, "#ifdef PRINTF\n");

		/* structures */

		for (t = parsedfiles.Flink; t != & parsedfiles; t = t->Flink)
		{
			struct parsedfile * const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
			processfile_periphregsdebug(pfl);
		}
		emitline(0, "#endif /* PRINTF */\n");
		emitline(0, "#endif /* %s */" "\n", headrname);
	}

	{
		PLIST_ENTRY t;
		/* release memory */

		for (t = parsedfiles.Flink; t != & parsedfiles; )
		{
			struct parsedfile * const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
			t = t->Flink;

			freeregs(pfl);
		}
	}

	return 0;
}


