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

static int flag_riscv = 0;
static int flag_svd = 0;
static int flag_debug = 0;
static int flag_cortexm3 = 0;
static int flag_cortexm4 = 0;
static int flag_cortexm7 = 0;

/* debug stuff */
#if 0

	#define TP() do { \
		fprintf(stderr, "%s/%d\n", __FILE__, __LINE__); \
		fflush(stderr); \
		} while (0)

#else

#define TP() do { \
		} while (0)

#endif

struct defdfn {
	LIST_ENTRY item;
	char *name;
	char *value;
	char *comment;
};

struct regdfn {
	LIST_ENTRY item;
	LIST_ENTRY aggregate;
	char *fldname;
	unsigned fldsize; /* 0 - need align to offset (end paddings) */
	unsigned fldoffs; /* offset inside block */
	char *comment;
	unsigned fldrept;   // 0 - plain field, 1..n - array
	unsigned resetvalue;
	int roflag;
	LIST_ENTRY bitfields; /* named bitfields in register */
};

struct bitfield {
	LIST_ENTRY item;
	char *bitfldname;
	unsigned bitfldwidth; /* width in bits */
	unsigned bitfldrpos; /* right bit position */
};

enum {
	BASE_MAX = 64
};
enum {
	VNAME_MAX = 96
};

enum {
	MAXIRQNUMBERS = 1024
};

struct parsedfile {
	LIST_ENTRY item;
	LIST_ENTRY regslist;
	LIST_ENTRY defineslist;

	char bname[VNAME_MAX];
	char *comment;
	char *file;

	int base_count;
	unsigned base_address[BASE_MAX];
	char *base_xnames[BASE_MAX];

	// ARM interrupt requests
	int irq_count;
	int irq_array[BASE_MAX];
	char *irq_xnames[BASE_MAX];
	char *irq_xcomments[BASE_MAX];

	// RISC-V interrupt requests
	int irqrv_count;
	int irqrv_array[BASE_MAX];
	char *irqrv_xnames[BASE_MAX];
	char *irqrv_xcomments[BASE_MAX];

};

#define INDENT 4
#define COMMENTPOS 54
#define COMMENTNEAR 54

void emitline(int pos, const char *format, ...) {
	static int emitpos;
	va_list ap;
	int n;
	FILE *fp = stdout;

	if (emitpos < pos) {
		const int pad = pos - emitpos;
		n = fprintf(fp, "%*.*s", pad, pad, "");
		if (n >= 0) {
			emitpos += pad;
		}
	}

	va_start(ap, format);
	n = vfprintf(fp, format, ap);
	va_end(ap);
	if (n >= 0) {
		emitpos += n;
	}
	if (strchr(format, '\n') != NULL)
		emitpos = 0;
}

/* XML escape characters */
/*
 "   &quot;
 '   &apos;
 <   &lt;
 >   &gt;
 &   &amp;

 */

static size_t escapedlen(const char *s) {
	size_t n = 0;
	for (;;) {
		switch (*s++) {
		case '\0':
			return n;
		case '"':
		case '\'':
			n += 6;
			continue;
		case '<':
		case '>':
			n += 4;
			continue;
		case '&':
			n += 5;
			continue;
		default:
			n += 1;
			continue;
		}
	}
}

static void desscape(char *dst, const char *src) {
	size_t n = 0;
	for (;;) {
		const char c = *src++;
		switch (c) {
		case '\0':
			dst[n++] = c;
			return;
		case '"':
			strcpy(dst + n, "&quot;");
			n += 6;
			continue;
		case '\'':
			strcpy(dst + n, "&apos;");
			n += 6;
			continue;
		case '<':
			strcpy(dst + n, "&lt;");
			n += 4;
			continue;
		case '>':
			strcpy(dst + n, "&gt;");
			n += 4;
			continue;
		case '&':
			strcpy(dst + n, "&amp;");
			n += 5;
			continue;
		default:
			dst[n++] = c;
			continue;
		}
	}
}

/* XML elements */
static void emitstring(int indent, const char *name, const char *value) {
	if (value == NULL)
		emitline(indent, "<%s></%s>" "\n", name, name);
	else {
		const size_t n = escapedlen(value);
		char *const s = malloc(n + 1);
		desscape(s, value);
		emitline(indent, "<%s>%s</%s>" "\n", name, s, name);
		free(s);
	}
}

static void emithex32(int indent, const char *name, unsigned value) {
	emitline(indent, "<%s>0x%08X</%s>" "\n", name, value, name);
}

static void emithex03(int indent, const char *name, unsigned value) {
	emitline(indent, "<%s>0x%03X</%s>" "\n", name, value, name);
}

static void emithex02(int indent, const char *name, unsigned value) {
	emitline(indent, "<%s>0x%02X</%s>" "\n", name, value, name);
}

static void emitudecimal(int indent, const char *name, unsigned value) {
	emitline(indent, "<%s>%u</%s>" "\n", name, value, name);
}

static void emitdecimal(int indent, const char *name, unsigned value) {
	emitline(indent, "<%s>%d</%s>" "\n", name, value, name);
}

static void emitcomment(int indent, const char *s) {
	emitline(indent, "<!-- %s -->\n", s);
}

/* Generate list of registers. Return last offset */
unsigned genreglist(int indent, const LIST_ENTRY *regslist, unsigned baseoffset) {
	unsigned offs;
	PLIST_ENTRY t;

	offs = 0;
	for (t = regslist->Flink; t != regslist; t = t->Flink) {
		const struct regdfn *const regp = CONTAINING_RECORD(t, struct regdfn, item);
		static const char *fldtypes[] = { "uint32_t", "uint8_t ", "uint16_t", "uint24_t", "uint32_t", "uint40_t", "uint48_t",
				"uint56_t", "uint64_t", };

		char fldtype[VNAME_MAX];

		//fprintf(stderr, "$generate field: fldsize=%u fldoffs=%04X fldrept=%u fldname=%s\n", regp->fldsize, regp->fldoffs,regp->fldrept,regp->fldname);

		if (!IsListEmpty(&regp->aggregate)) {
			fldtype[0] = '\0';
		} else if (regp->fldsize >= sizeof fldtypes / sizeof fldtypes[0]) {
			_snprintf(fldtype, sizeof fldtype / sizeof fldtype[0], "typesize%u", regp->fldsize);
		} else {
			_snprintf(fldtype, sizeof fldtype / sizeof fldtype[0], "%s", fldtypes[regp->fldsize]);
		}

		if (regp->fldoffs > offs || regp->fldsize == 0) {
			// reserving
			const unsigned sz = regp->fldoffs - offs;

			if (sz == 4) {
				emitline(indent + INDENT + 9, "uint32_t reserved_0x%03X;\n", offs);
			} else if (sz != 0 && (sz % 4) == 0) {
				emitline(indent + INDENT + 9, "uint32_t reserved_0x%03X [0x%04X];\n", offs, sz / 4);
			} else if (sz != 0) {
				emitline(indent + INDENT + 9, "uint8_t reserved_0x%03X [0x%04X];\n", offs, sz);
			}
			offs = regp->fldoffs;
		}

		if (regp->fldoffs != offs) {
			emitline(0, "#error Need offset 0x%03X of field '%s' type '%s' at (0x%03X)\n", offs, regp->fldname, fldtype,
					regp->fldoffs);
			//regp->fldoffs = offs;
		}

		if (1 /*regp->fldoffs == offs*/) {
			if (!IsListEmpty(&regp->aggregate)) {
				/* Emit aggregate type */
				emitline(indent + INDENT, "struct\n");
				emitline(indent + INDENT, "{\n");
				offs += regp->fldrept * genreglist(indent + INDENT, &regp->aggregate, offs); /* Emit fields list */
				emitline(indent + INDENT, "} %s [0x%03X];", regp->fldname, regp->fldrept);
				emitline(COMMENTPOS, "/*!< Offset 0x%03X %s */\n", regp->fldoffs + baseoffset, regp->comment);
			} else if (regp->fldsize != 0) {
				if (regp->fldrept) {
					// Array forming
					emitline(indent + INDENT, "%s %s %s [0x%03X];", regp->roflag ? "__I " : "__IO", fldtype, regp->fldname, regp->fldrept);

					offs += regp->fldsize * regp->fldrept;
				} else {
					// Plain field
					emitline(indent + INDENT, "%s %s %s;", regp->roflag ? "__I " : "__IO", fldtype, regp->fldname);
					offs += regp->fldsize;
				}
				emitline(COMMENTPOS, "/*!< Offset 0x%03X %s */\n", regp->fldoffs + baseoffset, regp->comment);
			}
		} else {
			emitline(0, "#error Need offset 0x%03X of field '%s' type '%s' at (0x%03X)\n", offs, regp->fldname, fldtype,
					regp->fldoffs);
			//break;
		}
	}
	return offs;
}

void gendefines(struct parsedfile *pfl) {
	PLIST_ENTRY const p = &pfl->defineslist;
	PLIST_ENTRY t;
	for (t = p->Flink; t != p; t = t->Flink) {
		struct defdfn *const defp = CONTAINING_RECORD(t, struct defdfn, item);
		if (defp->comment)
			emitline(0, "#define %s %s /* %s */\n", defp->name, defp->value, defp->comment);
		else
			emitline(0, "#define %s %s\n", defp->name, defp->value);
	}
}

void genstruct(struct parsedfile *pfl) {
	unsigned offs;

	emitline(0, "/*\n");
	emitline(0, " * @brief %s\n", pfl->bname);
	emitline(0, " */\n");

	emitline(0, "/*!< %s %s */\n", pfl->bname, pfl->comment ? pfl->comment : "");
	emitline(0, "typedef struct %s_Type\n", pfl->bname);
	emitline(0, "{\n");

	offs = genreglist(0, &pfl->regslist, 0);

	emitline(0, "} %s_TypeDef; /* size of structure = 0x%03X */\n", pfl->bname, offs);
}

void genstructprint(struct parsedfile *pfl) {
	PLIST_ENTRY t;

	emitline(0, "/* Print %s */\n", pfl->bname);
	emitline(0, "static void %s_Type_print(const %s_TypeDef * p, const char * base)\n", pfl->bname, pfl->bname);
	emitline(0, "{\n");
	for (t = pfl->regslist.Flink; t != &pfl->regslist; t = t->Flink) {
		const struct regdfn *const p = CONTAINING_RECORD(t, struct regdfn, item);

		if (p->fldsize != 0) {
			if (p->fldrept) {
				// Array forming
				unsigned i;
				for (i = 0; i < 4 && i < p->fldrept; ++i) {
					emitline(
					INDENT,
							"PRINTF(\"%%s->%s [%u] = 0x%%08X; /* 0x%%08X */\\n\", base, (unsigned) p->%s [%u], (unsigned) p->%s [%u]);",
							p->fldname, i, p->fldname, i, p->fldname, i);
					emitline(COMMENTPOS, "/*!< Offset 0x%03X %s */\n", p->fldoffs + i * p->fldsize, p->comment);
				}
			} else if (p->fldrept == 0) {
				// Plain field
				emitline(INDENT, "PRINTF(\"%%s->%s = 0x%%08X; /* 0x%%08X */\\n\", base, (unsigned) p->%s, (unsigned) p->%s );",
						p->fldname, p->fldname, p->fldname);
				emitline(COMMENTPOS, "/*!< Offset 0x%03X %s */\n", p->fldoffs, p->comment);
			}
		}
	}
	emitline(0, "}\n");
}

static LIST_ENTRY parsedfiles;

struct basemap {
	unsigned base;
	char *xname;
	struct parsedfile *pfl;
};

struct irqmap {
	int irq;
	char *xname;
	char *xcomment;
	struct parsedfile *pfl;
};

/* qsort parameter */
static int compare_base(const void *v1, const void *v2) {
	const struct basemap *p1 = v1;
	const struct basemap *p2 = v2;
	if (p1->base == p2->base) {
		return strcmp(p1->xname, p2->xname);
	}
	return p1->base - p2->base;
}

/* qsort parameter */
static int compare_irq(const void *v1, const void *v2) {
	const struct irqmap *p1 = v1;
	const struct irqmap *p2 = v2;

	if (p1->irq == p2->irq) {
		return strcmp(p1->xname, p2->xname);
	}
	return p1->irq - p2->irq;
}

/* qsort parameter */
static int compare_pfltypes(const void *v1, const void *v2) {
	const struct parsedfile **p1 = v1;
	const struct parsedfile **p2 = v2;

	return strcmp((*p1)->bname, (*p2)->bname);
}

/* source line for parse */

static char token0[1024];
#define TKSZ (sizeof token0 / sizeof token0 [0])

static int istokencomment(void) {
	return 0 == memcmp(token0, "##", 2) || 0 == memcmp(token0, "# ", 2) || 0 == strcmp(token0, "#\n") || 0 == strcmp(token0, "\n")
			|| 0;
}

/* trim field name */
static void trimname(char *s) {
	if (strchr(s, '\n') != NULL)
		*strchr(s, '\n') = '\0';
	if (strchr(s, '/') != NULL)
		*strchr(s, '/') = '_';
	if (strchr(s, '/') != NULL)
		*strchr(s, '/') = '_';
	if (strchr(s, '-') != NULL)
		*strchr(s, '-') = '_';
	if (strchr(s, '-') != NULL)
		*strchr(s, '-') = '_';
}

/* trim spaces */
static void trimheadspaces(char *s) {
	char *dst = s;
	while (*s == ' ' || *s == '\t')
		++s;
	while ((*dst++ = *s++) != '\0')
		;
}

static int nextline(FILE *fp) {
	for (;;) {
		char *s = fgets(token0, TKSZ, fp);
		if (s != NULL) {
			trimheadspaces(s);
			if (istokencomment())
				continue;
		}
		return s != NULL;
	}

}

static struct regdfn*
parseregdef(char *s0, char *fldname, unsigned fldsize, int ro, const char *file) {
	unsigned fldoffset;
	unsigned fldrept;
	struct regdfn *regp = calloc(1, sizeof *regp);

	static const char SEP[] = ";";
	//	#regdef; RISC_STA_ADD0_REG; 0x0004; RISC Start Address0 Register

	//fprintf(stderr, "Parsed regdef='%s' 0x%08X '%s'\n", fldname, fldoffset, comment);
	char *s = strtok(s0, SEP);
	char *s2 = strtok(NULL, SEP);

	InitializeListHead(&regp->aggregate);
	InitializeListHead(&regp->bitfields);

	trimname(fldname);

	if (s2 != NULL) {
		/* trim comments */
		while (isspace((unsigned char ) *s2))
			++s2;
		if (strchr(s2, '\n') != NULL)
			*strchr(s2, '\n') = '\0';
	}

	//fprintf(stderr, "parse: '%s' '%s'\n", s, s2);
	regp->roflag = ro;
	if (2 == sscanf(s, "%i %i", &fldoffset, &fldrept)) {
		//fprintf(stderr, "fParsed 2 regdef='%s' offs=0x%08X rept=0x%08X\n", fldname, fldoffset, fldrept);
		regp->fldname = strdup(fldname);
		regp->fldoffs = fldoffset;
		regp->fldrept = fldrept;
		regp->fldsize = fldsize;
		regp->comment = strdup(s2);
		regp->resetvalue = 0;
	} else if (1 == sscanf(s, "%i", &fldoffset)) {
		//fprintf(stderr, "fParsed 1 regdef='%s' offs=0x%08X\n", fldname, fldoffset);
		regp->fldname = strdup(fldname);
		regp->fldoffs = fldoffset;
		regp->fldrept = 0;
		regp->fldsize = fldsize;
		regp->comment = strdup(s2);
		regp->resetvalue = 0;

		/* parsed */
	} else {
		regp->comment = strdup("no comment");

		fprintf(stderr, "fstrange '%s' token0='%s' regdef='%s'\n", file, token0, s);

		regp->fldname = strdup(fldname);
		regp->fldoffs = 0;
		regp->fldrept = 0;
		regp->fldsize = 4;
		regp->resetvalue = 0;

		/* parsed */
	}
	return regp;
}

// registers list bay be empty
static void parsereglist(FILE *fp, const char *file, PLIST_ENTRY listhead) {
	char fldname[VNAME_MAX];
	unsigned fldsize;
	int pos; /* end of parsed field position */

	for (;;) {
		//fprintf(stderr, "token0=%s\n", token0);
		if (2 == sscanf(token0, "#regdefr; %[a-zA-Z_0-9/-] %i %n", fldname, &fldsize, &pos)) {
			struct regdfn *regp = parseregdef(token0 + pos, fldname, fldsize, 1, file);
			//fprintf(stderr, "Parsed 2 regdef fldname='%s' fldszie=%u\n", fldname, fldsize);
			/* parsed */
			InsertTailList(listhead, &regp->item);
			if (nextline(fp) == 0)
				break;

		} else if (1 == sscanf(token0, "#regdefr; %[a-zA-Z_0-9/-] %n", fldname, &pos)) {

			struct regdfn *regp = parseregdef(token0 + pos, fldname, 4, 1, file);
			//	fprintf(stderr, "Parsed 1 regdef fldname='%s' \n", fldname);
			/* parsed */
			InsertTailList(listhead, &regp->item);
			if (nextline(fp) == 0)
				break;
		} else if (2 == sscanf(token0, "#regdef; %[a-zA-Z_0-9/-] %i %n", fldname, &fldsize, &pos)) {
			struct regdfn *regp = parseregdef(token0 + pos, fldname, fldsize, 0, file);
			//fprintf(stderr, "Parsed 2 regdef fldname='%s' fldszie=%u\n", fldname, fldsize);
			/* parsed */
			InsertTailList(listhead, &regp->item);
			if (nextline(fp) == 0)
				break;

		} else if (1 == sscanf(token0, "#regdef; %[a-zA-Z_0-9/-] %n", fldname, &pos)) {

			struct regdfn *regp = parseregdef(token0 + pos, fldname, 4, 0, file);
			//	fprintf(stderr, "Parsed 1 regdef fldname='%s' \n", fldname);
			/* parsed */
			InsertTailList(listhead, &regp->item);
			if (nextline(fp) == 0)
				break;

		} else if (1 == sscanf(token0, "#aggreg; %[a-zA-Z_0-9/-] %n", fldname, &pos)) {

			struct regdfn *regp = parseregdef(token0 + pos, fldname, 4, 0, file);
			//fprintf(stderr, "x Parsed 1 aggreg fldname='%s' \n", fldname);
			/* parsed */
			InsertTailList(listhead, &regp->item);

			/* parse other fields for this aggregate */
			if (nextline(fp) == 0)
				break;
			parsereglist(fp, file, &regp->aggregate);

		} else if (strcmp(token0, "#aggregend;") == 0 || strcmp(token0, "#aggregend;\n") == 0) {
			/* parsed */
			//fprintf(stderr, "#aggregend: token0=%s", token0);
			nextline(fp);
			return;

		} else if (strcmp(token0, "#typeend;") == 0 || strcmp(token0, "#typeend;\n") == 0) {
			/* parsed */
			nextline(fp);
			return;

		} else {
			break;
		}
	}
}

// 0 - end of file
// 1 - register definition ok
static int parseregfile(struct parsedfile *pfl, FILE *fp, const char *file) {
	char comment[TKSZ];
	char typname[VNAME_MAX];
	char irqname[VNAME_MAX];
	int irq;
	unsigned base;

	memset(comment, 0, sizeof comment);

	// #type should be 1-st in register definitions
	if (2 == sscanf(token0, "#type; %[a-zA-Z0-9_]; %1023[^\n]", typname, comment)) {
		//fprintf(stderr, "Parsed [%s]: typname='%s', comment='%s'\n", token0, typname, comment);
		trimname(typname);
		strcpy(pfl->bname, typname);
		pfl->comment = strdup(comment);

		/* parsed */
		if (nextline(fp) == 0)
			return 0;
	} else if (1 == sscanf(token0, "#type; %[a-zA-Z0-9_]\n", typname)) {
		//fprintf(stderr, "Parsed [%s]: typname='%s'\n", token0, typname);
		trimname(typname);
		strcpy(pfl->bname, typname);

		/* parsed */
		if (nextline(fp) == 0)
			return 0;
	} else {
		return 0;
	}

	for (;;) {
		//fprintf(stderr, "0 token0=%s\n", token0);
		memset(comment, 0, sizeof comment);
		if (1 == sscanf(token0, "#comment; %1023[^\n]\n", comment)) {
			//fprintf(stderr, "Parsed comment='%s'\n", comment);
			pfl->comment = strdup(comment);
			if (nextline(fp) == 0)
				break;
		} else if (3 == sscanf(token0, "#irq; %s %i; %1023[^\n]", irqname, &irq, comment)) {
			trimname(irqname);
			//fprintf(stderr, "Parsed irq='%s' %d\n", irqname, irq);
			if (pfl->irq_count < BASE_MAX) {
				pfl->irq_array[pfl->irq_count] = irq;
				pfl->irq_xnames[pfl->irq_count] = strdup(irqname);
				pfl->irq_xcomments[pfl->irq_count] = strdup(comment);
				//
				++pfl->irq_count;
			}

			/* parsed */
			if (nextline(fp) == 0)
				break;
		} else if (2 == sscanf(token0, "#irq; %s %i\n", irqname, &irq)) {
			trimname(irqname);
			//fprintf(stderr, "Parsed irq='%s' %d\n", irqname, irq);
			if (pfl->irq_count < BASE_MAX) {
				pfl->irq_array[pfl->irq_count] = irq;
				pfl->irq_xnames[pfl->irq_count] = strdup(irqname);
				pfl->irq_xcomments[pfl->irq_count] = NULL;
				//
				++pfl->irq_count;
			}

			/* parsed */
			if (nextline(fp) == 0)
				break;
		} else if (3 == sscanf(token0, "#irqrv; %s %i; %1023[^\n]\n", irqname, &irq, comment)) {
			trimname(irqname);
			//fprintf(stderr, "Parsed irqrv='%s' %d\n", irqname, irqrv);
			if (pfl->irqrv_count < BASE_MAX) {
				pfl->irqrv_array[pfl->irqrv_count] = irq;
				pfl->irqrv_xnames[pfl->irqrv_count] = strdup(irqname);
				pfl->irqrv_xcomments[pfl->irqrv_count] = strdup(comment);
				//
				++pfl->irqrv_count;
			}

			/* parsed */
			if (nextline(fp) == 0)
				break;
		} else if (2 == sscanf(token0, "#irqrv; %s %i\n", irqname, &irq)) {
			trimname(irqname);
			//fprintf(stderr, "Parsed irqrv='%s' %d\n", irqname, irqrv);
			if (pfl->irqrv_count < BASE_MAX) {
				pfl->irqrv_array[pfl->irqrv_count] = irq;
				pfl->irqrv_xnames[pfl->irqrv_count] = strdup(irqname);
				pfl->irqrv_xcomments[pfl->irqrv_count] = NULL;
				//
				++pfl->irqrv_count;
			}

			/* parsed */
			if (nextline(fp) == 0)
				break;
		} else if (2 == sscanf(token0, "#base; %s%i\n", typname, &base)) {
			//fprintf(stderr, "Parsed base='%s' 0x%08X\n", typname, base);
			if (pfl->base_count < BASE_MAX) {
				pfl->base_address[pfl->base_count] = base;
				trimname(typname);
				pfl->base_xnames[pfl->base_count] = strdup(typname);
				++pfl->base_count;
			}

			/* parsed */
			if (nextline(fp) == 0)
				break;
		} else {
			parsereglist(fp, file, &pfl->regslist);
			return 1;
		}
	}
	return 1;
}

// parse file section
// 0 - end of file
// 1 - register definition ok
static int loadregs(struct parsedfile *pfl, FILE *fp, const char *file) {
	const size_t maxrows = 256;
	//TP();
	strcpy(pfl->bname, "");
	pfl->base_count = 0;
	pfl->irq_count = 0;
	pfl->irqrv_count = 0;

	InitializeListHead(&pfl->regslist);
	InitializeListHead(&pfl->defineslist);

	pfl->file = strdup(file);
	pfl->comment = NULL;

	//fprintf(stderr, "#error Opened file '%s'\n", file);

	// 0 - end of file
	// 1 - register definition ok
	return parseregfile(pfl, fp, file);

}

static struct parsedfile* findregs(const char *name) {
	PLIST_ENTRY t;
	for (t = parsedfiles.Flink; t != &parsedfiles; t = t->Flink) {
		struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
		if (!strcmp(pfl->bname, name))
			return pfl;
	}
	return NULL;
}

/* release memory of bitfields */
static void freebitfields(PLIST_ENTRY p) {
	PLIST_ENTRY t;
	//fprintf(stderr, "Release memory\n");
	for (t = p->Flink; t != p;) {
		struct bitfield *const fieldp = CONTAINING_RECORD(t, struct bitfield, item);
		t = t->Flink;
		free(fieldp->bitfldname);
	}
}

/* release memory of register definitions */
static void freeregdfn(PLIST_ENTRY p) {
	PLIST_ENTRY t;
	//fprintf(stderr, "Release memory\n");
	for (t = p->Flink; t != p;) {
		struct regdfn *const regp = CONTAINING_RECORD(t, struct regdfn, item);
		t = t->Flink;
		free(regp->fldname);
		free(regp->comment);
		freeregdfn(&regp->aggregate);
		freebitfields(&regp->bitfields);
		free(regp);
	}
}

/* release memory of defines */
static void freedefines(PLIST_ENTRY p) {
	PLIST_ENTRY t;
	//fprintf(stderr, "Release memory\n");
	for (t = p->Flink; t != p;) {
		struct defdfn *const defp = CONTAINING_RECORD(t, struct defdfn, item);
		t = t->Flink;
		free(defp->name);
		free(defp->value);
		free(defp->comment);
		free(defp);
	}
}

static void freeregs(struct parsedfile *pfl) {
	int i;
	for (i = 0; i < pfl->irqrv_count; ++i) {
		free(pfl->irqrv_xnames[i]);
		free(pfl->irqrv_xcomments[i]);
	}
	for (i = 0; i < pfl->irq_count; ++i) {
		free(pfl->irq_xnames[i]);
		free(pfl->irq_xcomments[i]);
	}
	for (i = 0; i < pfl->base_count; ++i) {
		free(pfl->base_xnames[i]);
		//free(pfl->base_xcomments [i]);
	}
	//free(pfl->sss);
	freeregdfn(&pfl->regslist);
	freedefines(&pfl->defineslist);
	free(pfl->comment);
	free(pfl->file);
}

static void movelist(PLIST_ENTRY dst, PLIST_ENTRY src) {
	while (!IsListEmpty(src)) {
		PLIST_ENTRY const t = RemoveHeadList(src)
		;
		InsertTailList(dst, t);
	}
}

static void mergestrings(char **dst, char **src) {
	if (*dst == NULL) {
		*dst = *src;
		*src = NULL;
	} else if (*dst != NULL && *src != NULL) {
		free(*dst);
		*dst = *src;
		*src = NULL;
	} else {

	}
}

static void mergeregs(struct parsedfile *old, struct parsedfile *pfl) {
	int t;
	int i;

	for (t = old->irqrv_count, i = 0; i < pfl->irqrv_count && t < BASE_MAX; ++i, ++t) {
		old->irqrv_array[t] = pfl->irqrv_array[i];
		old->irqrv_xnames[t] = pfl->irqrv_xnames[i];
		old->irqrv_xcomments[t] = pfl->irqrv_xcomments[i];
	}
	old->irqrv_count = t;
	pfl->irqrv_count = 0;

	for (t = old->irq_count, i = 0; i < pfl->irq_count && t < BASE_MAX; ++i, ++t) {
		old->irq_array[t] = pfl->irq_array[i];
		old->irq_xnames[t] = pfl->irq_xnames[i];
		old->irq_xcomments[t] = pfl->irq_xcomments[i];
	}
	old->irq_count = t;
	pfl->irq_count = 0;

	for (t = old->base_count, i = 0; i < pfl->base_count && t < BASE_MAX; ++i, ++t) {
		old->base_address[t] = pfl->base_address[i];
		old->base_xnames[t] = pfl->base_xnames[i];
		//old->base_xcomments [t] = pfl->base_xcomments[i];
	}
	old->base_count = t;
	pfl->base_count = 0;

	//free(pfl->sss);
	movelist(&old->regslist, &pfl->regslist);
	movelist(&old->defineslist, &pfl->defineslist);

	mergestrings(&old->comment, &pfl->comment);
	mergestrings(&old->file, &pfl->file);
}

static void loadfile(const char *file) {
	FILE *fp = fopen(file, "rt");

	if (fp == NULL) {
		fprintf(stderr, "#error Can not open file '%s'\n", file);
		return;
	}
	// parser initialize
	if (nextline(fp) == 0) {
		fclose(fp);
		return;
	}
	// parse file sections
	for (;;) {
		struct parsedfile *const pfl = calloc(1, sizeof(struct parsedfile));
		if (loadregs(pfl, fp, file)) {
			struct parsedfile *const old = findregs(pfl->bname);
			if (old == NULL) {
				InsertTailList(&parsedfiles, &pfl->item);
			} else {
				mergeregs(old, pfl);
				freeregs(pfl);
				free(pfl);
			}
		} else {
			free(pfl);
			break;
		}
	}
	fclose(fp);
}

static void processfile_periphregs(struct parsedfile *pfl) {

	if (!IsListEmpty(&pfl->regslist)) {
		genstruct(pfl);
	} else {
		//fprintf(stderr, "#error No registers in '%s'\n", pfl->bname);
	}
}

static void processfile_defines(struct parsedfile *pfl) {

	if (!IsListEmpty(&pfl->defineslist)) {
		gendefines(pfl);
	} else {
		//fprintf(stderr, "#error No registers in '%s'\n", pfl->bname);
	}
}

static void processfile_periphregsdebug(struct parsedfile *pfl) {

	if (!IsListEmpty(&pfl->regslist)) {
		genstructprint(pfl);
	}
}

static int collect_base(struct parsedfile *pfl, int n, struct basemap *v) {
	/* collect base pointers */
	int i;
	int score = 0;
	for (i = 0; i < pfl->base_count && n--; ++i, ++v, ++score) {
		v->xname = pfl->base_xnames[i];
		v->base = pfl->base_address[i];
		v->pfl = pfl;
	}
	return score;
}

static int collect_irq(struct parsedfile *pfl, int n, struct irqmap *v) {
	/* collect irq vectors */
	int i;
	int score = 0;
	for (i = 0; i < pfl->irq_count && n--; ++i, ++v, ++score) {
		v->xname = pfl->irq_xnames[i];
		v->xcomment = pfl->irq_xcomments[i];
		v->irq = pfl->irq_array[i];
		v->pfl = pfl;
	}
	return score;
}

static int collect_irqrv(struct parsedfile *pfl, int n, struct irqmap *v) {
	/* collect irq vectors */
	int i;
	int score = 0;
	for (i = 0; i < pfl->irqrv_count && n--; ++i, ++v, ++score) {
		v->xname = pfl->irqrv_xnames[i];
		v->xcomment = pfl->irqrv_xcomments[i];
		v->irq = pfl->irqrv_array[i];
		v->pfl = pfl;
	}
	return score;
}

static void emitcpu(int indent) {
	emitstring(indent, "name", "XXXX");
	emitstring(indent, "version", "1.00");
	emitstring(indent, "description", "for test");
	emitudecimal(indent, "addressUnitBits", 8);
	emitudecimal(indent, "width", 32);
	emitudecimal(indent, "size", 32);
	emithex32(indent, "resetValue", 0x0000000);
	emithex32(indent, "resetMask", 0xFFFFFFFF);

}

static unsigned emitregister000(int indent, const struct regdfn *const regp, unsigned baseoffset) {
	unsigned offs;
	unsigned regsizebits = regp->fldsize * 8;

	offs = 0;
	if (regp->fldsize == 0) {
		/* set to required size */
		offs = regp->fldoffs - baseoffset;
	} else {

		emitline(indent + 0, "<register>" "\n");

		emitstring(indent + 1, "name", regp->fldname);
		emitstring(indent + 1, "displayName", regp->fldname);
		emitstring(indent + 1, "description", regp->comment);
		emithex03(indent + 1, "addressOffset", regp->fldoffs - baseoffset);
		emithex02(indent + 1, "size", regsizebits);
		emitstring(indent + 1, "access", "read-write");
		emithex32(indent + 1, "resetValue", regp->resetvalue);

		emitline(indent + 0, "</register>" "\n");

		offs += regp->fldsize;
	}

	return offs;
}

unsigned
emitregisters(int indent, const LIST_ENTRY *regslist, unsigned baseoffset);

/* return total size of emitted registers */
unsigned emitregister(int indent, const struct regdfn *const regp, unsigned baseoffset) {
	unsigned offs = 0;

//	char buff [128];
//
//	_snprintf(buff, sizeof buff / sizeof buff [0], "%s: fldsize=%u, fldrept=%u, fldoffs=0x%03X", regp->fldname, regp->fldsize, regp->fldrept, regp->fldoffs);
//	emitcomment(indent, buff);

	if (!IsListEmpty(&regp->aggregate)) {
		/* Emit aggregate type */
		if (regp->fldrept) {
			unsigned size;
			emitline(indent, "<cluster>" "\n");
			emitudecimal(indent + 1, "dim", regp->fldrept);
			emithex32(indent + 1, "addressOffset", regp->fldoffs);
			size = emitregisters(indent + 1, &regp->aggregate, 0) * regp->fldrept;
			emithex03(indent + 1, "dimIncrement", size);
			offs += size;
			emitline(indent, "</cluster>" "\n");
		} else {
			offs += emitregisters(indent, &regp->aggregate, 0);
		}

	} else if (regp->fldsize != 0) {
		if (regp->fldrept) {
			unsigned size;
			// Array forming
			//emitline(indent + INDENT, "__IO %s %s [0x%03X];",
			//		fldtype, regp->fldname, regp->fldrept);
			emitline(indent, "<cluster>" "\n");
			emitudecimal(indent + 1, "dim", regp->fldrept);
			emithex32(indent + 1, "addressOffset", regp->fldoffs);
			size = emitregister000(indent + 1, regp, regp->fldoffs) * regp->fldrept;
			emithex03(indent + 1, "dimIncrement", size);
			offs += size;
			emitline(indent, "</cluster>" "\n");

		} else {
			// Plain field
			//emitline(indent + INDENT, "__IO %s %s;", fldtype,
			//		regp->fldname);
			offs += emitregister000(indent, regp, 0);
		}
	} else {
		// fldsize == 0: align
		//fprintf(stderr, "align to %u %s\n", regp->fldoffs, regp->fldname);
		offs = regp->fldoffs - baseoffset;
	}

	return offs;
}

unsigned emitregisters(int indent, const LIST_ENTRY *regslist, unsigned baseoffset) {
	unsigned offs;
	PLIST_ENTRY t;

	emitline(indent, "<registers>" "\n");

	offs = 0;
	for (t = regslist->Flink; t != regslist; t = t->Flink) {
		const struct regdfn *const regp = CONTAINING_RECORD(t, struct regdfn, item);
		unsigned regsizebits = regp->fldsize * 8;

		offs += emitregister(indent + 1, regp, offs);
	}
	emitline(indent, "</registers>" "\n");
	return offs;
}

static void emitinterrupt(int indent, const char *name, int value) {
	emitline(indent, "<interrupt>" "\n");
	emitstring(indent + 1, "name", name);
	emitdecimal(indent + 1, "value", value);
	emitstring(indent + 1, "description", "");
	emitline(indent, "</interrupt>" "\n");
}

static void emitinterrupts(int indent, const struct parsedfile *pfl) {
	/* interrupts */
	if (!flag_riscv) {
		/* collect ARM IRQ vectors */
		int i;
		for (i = 0; i < pfl->irq_count; ++i) {
			emitinterrupt(indent, pfl->irq_xnames[i], pfl->irq_array[i]);
		}
	} else {
		/* collect RISC-V IRQ vectors */
		int i;
		for (i = 0; i < pfl->irqrv_count; ++i) {
			emitinterrupt(indent, pfl->irqrv_xnames[i], pfl->irqrv_array[i]);
		}
	}
}

static void emitperipherial(int indent, const struct parsedfile *pfl) {
	int i;

	if (IsListEmpty(&pfl->regslist)) {
		return;
	}
	if (strlen(pfl->bname) == 0)
		fprintf(stderr, "Not named object in file '%s'\n", pfl->file);

	/* name */
	if (pfl->base_count >= 1) {
		emitline(indent, "<peripheral>" "\n");
		emitstring(indent, "name", pfl->base_xnames[0]);
		emitstring(indent, "description", pfl->comment);
		//emitstring(indent, "<groupName>%s</groupName>" "\n", pfl->base_names [0]);
		emithex32(indent, "baseAddress", pfl->base_address[0]);
		emitinterrupts(indent, pfl);
		emitregisters(indent, &pfl->regslist, 0);
		emitline(indent, "</peripheral>" "\n");
	}
	/* base address */
	for (i = 1; i < pfl->base_count; ++i) {

		emitline(indent, "<peripheral derivedFrom=\"%s\">" "\n", pfl->base_xnames[0]);
		emitstring(indent, "name", pfl->base_xnames[i]);
		emithex32(indent, "baseAddress", pfl->base_address[i]);
		emitinterrupts(indent, pfl);
		emitline(indent, "</peripheral>" "\n");
	}
}

static void emitperipherials(int indent) {
	PLIST_ENTRY t;

	emitline(indent, "<peripherals>" "\n");

	/* structures */
	for (t = parsedfiles.Flink; t != &parsedfiles; t = t->Flink) {
		struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
		emitperipherial(indent + 1, pfl);
	}

	emitline(indent, "</peripherals>" "\n");
}

static void emitvendorext(int indent) {
	emitline(indent, "<vendor>" "\n");
	emitline(indent, "</vendor>" "\n");

}

static void emitdevice(int indent) {
	emitline(indent,
			"<device schemaVersion=\"1.1\" xmlns:xs=\"http://www.w3.org/2001/XMLSchema-instance\" xs:noNamespaceSchemaLocation=\"CMSIS-SVD_Schema_1_1.xsd\" >" "\n");

	emitcpu(indent + 1);
	emitperipherials(indent + 1);
	emitvendorext(indent + 1);

	emitline(indent, "</device >" "\n");
}

// See https://github.com/Open-CMSIS-Pack/devtools/SVDConv

static void generate_svd(void) {
	emitline(0, "<?xml version=\"1.0\" encoding=\"utf-8\"?>" "\n");
	emitdevice(0);
}

static void generate_debug(void) {

	/* Debug header forming */
	char headrname[128];
	PLIST_ENTRY t;

	/* print structire debug */

	_snprintf(headrname, sizeof headrname / sizeof headrname[0], "HEADER_%08X_INCLUDED", (unsigned) time(NULL));

	emitline(0, "#ifndef %s" "\n", headrname);
	emitline(0, "#define %s" "\n", headrname);
	emitline(0, "#ifdef PRINTF\n");

	/* structures */

	for (t = parsedfiles.Flink; t != &parsedfiles; t = t->Flink) {
		struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
		processfile_periphregsdebug(pfl);
	}
	emitline(0, "#endif /* PRINTF */\n");
	emitline(0, "#endif /* %s */" "\n", headrname);

}

static void generate_cmsis(void) {

	/* CMSIS header forming */
	char headrname[128];
	_snprintf(headrname, sizeof headrname / sizeof headrname[0], "HEADER_%08X_INCLUDED", (unsigned) 12345 /*time(NULL) */);

	emitline(0, "#pragma once" "\n");
	emitline(0, "#ifndef %s" "\n", headrname);
	emitline(0, "#define %s" "\n", headrname);

	emitline(0, "#include <stdint.h>" "\n");
	emitline(0, "\n");

	if (!flag_riscv) {
		/* collect ARM IRQ vectors */
		const int maxbases = MAXIRQNUMBERS;
		int nitems = 0;
		int i;
		struct irqmap *irqs;

		irqs = calloc(maxbases, sizeof(struct irqmap));
		{
			PLIST_ENTRY t;
			for (t = parsedfiles.Flink; t != &parsedfiles && nitems < maxbases; t = t->Flink) {
				struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
				nitems += collect_irq(pfl, maxbases - nitems, irqs + nitems);
			}
		}
		if (maxbases == nitems) {
			fprintf(stderr, "Too large data (struct irqmap)\n");
			return;
		}

		qsort(irqs, nitems, sizeof irqs[0], compare_irq);

		/* generate ARM IRQ vectors */
		emitline(0, "\n");
		emitline(0, "/* IRQs */\n");
		emitline(0, "\n");
		emitline(0, "typedef enum IRQn\n");
		emitline(0, "{\n");
		for (i = 0; i < nitems; ++i) {
			struct irqmap *const p = &irqs[i];
			const char *comment = p->xcomment ? p->xcomment : p->pfl->comment ? p->pfl->comment : "";

			emitline(INDENT, "%s_IRQn = %d,", p->xname, p->irq);
			emitline(COMMENTNEAR, "/*!< %s %s */\n", p->pfl->bname, comment);
		}
		emitline(0, "\n");
		emitline(INDENT, "MAX_IRQ_n,\n");
		emitline(
		INDENT,
				"Force_IRQn_enum_size = %d /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */\n",
				1048);
		emitline(0, "} IRQn_Type;\n");
		emitline(0, "\n");
		free(irqs);
	}

	if (flag_riscv) {
		/* collect RISC-V IRQ vectors */
		const int maxbases = MAXIRQNUMBERS;
		int nitems = 0;
		int i;
		struct irqmap *irqs;

		irqs = calloc(maxbases, sizeof(struct irqmap));

		{
			PLIST_ENTRY t;
			for (t = parsedfiles.Flink; t != &parsedfiles && nitems < maxbases; t = t->Flink) {
				struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
				nitems += collect_irqrv(pfl, maxbases - nitems, irqs + nitems);
			}
		}
		if (maxbases == nitems) {
			fprintf(stderr, "Too large data (struct irqmap)\n");
			return;
		}

		qsort(irqs, nitems, sizeof irqs[0], compare_irq);

		/* generate RISC-V IRQ vectors */
		emitline(0, "\n");
		emitline(0, "/* IRQs */\n");
		emitline(0, "\n");
		emitline(0, "typedef enum IRQn\n");
		emitline(0, "{\n");
		for (i = 0; i < nitems; ++i) {
			struct irqmap *const p = &irqs[i];
			const char *comment = p->xcomment ? p->xcomment : p->pfl->comment ? p->pfl->comment : "";
			emitline(INDENT, "%s_IRQn = %d,", p->xname, p->irq);
			emitline(COMMENTNEAR, "/*!< %s %s */\n", p->pfl->bname, comment);
		}
		emitline(0, "\n");
		emitline(INDENT, "MAX_IRQ_n,\n");
		emitline(INDENT, "Force_IRQn_enum_size = %d /* Dummy entry to ensure IRQn_Type is more than 8 bits. "
				"Otherwise GIC init loop would fail */\n", 1048);
		emitline(0, "} IRQn_Type;\n");
		emitline(0, "\n");
		free(irqs);
	}

	if (1) {

		/* collect base addresses */
		const int maxbases = 1024;
		int nitems = 0;
		int i;
		struct basemap *maps;
		PLIST_ENTRY t;

		maps = calloc(maxbases, sizeof(struct basemap));
		for (t = parsedfiles.Flink; t != &parsedfiles; t = t->Flink) {
			struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
			nitems += collect_base(pfl, maxbases - nitems, maps + nitems);
		}
		if (maxbases == nitems) {
			fprintf(stderr, "Too large data (struct basemap)\n");
			return;
		}

		qsort(maps, nitems, sizeof maps[0], compare_base);

		/* generate base addresses */
		emitline(0, "\n");
		emitline(0, "/* Peripheral and RAM base address */\n");
		emitline(0, "\n");

		for (i = 0; i < nitems; ++i) {
			struct basemap *const p = &maps[i];

			emitline(0, "#define %s_BASE ((uintptr_t) 0x%08X)", p->xname, p->base);
			emitline(COMMENTNEAR, "/*!< %s %s Base */\n", p->pfl->bname, p->pfl->comment ? p->pfl->comment : "");
		}
		emitline(0, "\n");
		free(maps);
	}

	{
		/* CMSIS includes here */
		if (flag_riscv) {
			emitline(0, "#include <core_rv64.h>\n");
		} else if (flag_cortexm7) {
			emitline(0, "#include <core_cm7.h>\n");
		} else if (flag_cortexm4) {
			emitline(0, "#include <core_cm4.h>\n");
		} else if (flag_cortexm3) {
			emitline(0, "#include <core_cm3.h>\n");
		} else {
			emitline(0, "#include <core_ca.h>\n");
		}

		//emitline(0, "#include <stdint.h>\n");
		emitline(0, "\n");
	}

	if (1) {
		/* structures */
		PLIST_ENTRY t;

		for (t = parsedfiles.Flink; t != &parsedfiles; t = t->Flink) {
			struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
			processfile_periphregs(pfl);
		}
		emitline(0, "\n");
	}

	if (1) {
		/* defines */
		PLIST_ENTRY t;
		emitline(0, "\n");
		emitline(0, "/* Defines */\n");
		emitline(0, "\n");

		for (t = parsedfiles.Flink; t != &parsedfiles; t = t->Flink) {
			struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
			processfile_defines(pfl);
		}
		emitline(0, "\n");
	}

	if (1) {
		const int maxbases = 1024;
		int nitems = 0;
		int i;
		struct basemap *maps;
		PLIST_ENTRY t;

		emitline(0, "\n");
		emitline(0, "/* Access pointers */\n");
		emitline(0, "\n");
		maps = calloc(maxbases, sizeof(struct basemap));
		for (t = parsedfiles.Flink; t != &parsedfiles; t = t->Flink) {
			struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
			nitems += collect_base(pfl, maxbases - nitems, maps + nitems);
		}
		if (maxbases == nitems) {
			fprintf(stderr, "Too large data (struct basemap)\n");
			return;
		}

		qsort(maps, nitems, sizeof maps[0], compare_base);

		for (i = 0; i < nitems; ++i) {
			struct parsedfile *const pfl = maps[i].pfl;
			if (IsListEmpty(&pfl->regslist)) {
				/* no structure defined */
				continue;
			}
			emitline(0, "#define %s ((%s_TypeDef *) %s_BASE)", maps[i].xname, maps[i].pfl->bname, maps[i].xname);
			emitline(COMMENTNEAR, "/*!< %s %s register set access pointer */\n", maps[i].xname, pfl->comment ? pfl->comment : "");
		}
		emitline(0, "\n");
		free(maps);
	}

	emitline(0, "\n");
	emitline(0, "#endif /* %s */" "\n", headrname);

}

int main(int argc, char *argv[], char *envp[]) {
	//struct parsedfile pfls [MAXPARSEDFILES];
	int i = 1;

	if (argc > 1 && strcmp(argv[1], "--riscv") == 0) {
		flag_riscv = 1;
		--argc;
		++argv;
	}
	if (argc > 1 && strcmp(argv[1], "--cortexm3") == 0) {
		flag_cortexm3 = 1;
		--argc;
		++argv;
	}
	if (argc > 1 && strcmp(argv[1], "--cortexm4") == 0) {
		flag_cortexm4 = 1;
		--argc;
		++argv;
	}
	if (argc > 1 && strcmp(argv[1], "--cortexm7") == 0) {
		flag_cortexm7 = 1;
		--argc;
		++argv;
	}
	if (argc > 1 && strcmp(argv[1], "--svd") == 0) {
		flag_svd = 1;
		--argc;
		++argv;
	}
	if (argc > 1 && strcmp(argv[1], "--debug") == 0) {
		flag_debug = 1;
		--argc;
		++argv;
	}

	if (argc < 2)
		return 1;

	/* Load files */
	{
		InitializeListHead(&parsedfiles);

		for (; i < argc;) {
			loadfile(argv[i]);
			++i;
		}
	}
	/* demo */
	{
		PLIST_ENTRY t;
		for (t = parsedfiles.Flink; t != &parsedfiles; t = t->Flink) {
			struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
			/* any actions... */
		}
	}

	/* Sort objects */
	{
		int nitems;
		int i;
		struct parsedfile **pflarray;
		PLIST_ENTRY t;

		for (nitems = 0, t = parsedfiles.Flink; t != &parsedfiles; t = t->Flink) {
			struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
			++nitems;
		}
		pflarray = calloc(nitems, sizeof(struct parsedfile*));
		for (i = 0, t = parsedfiles.Flink; t != &parsedfiles; t = t->Flink, ++i) {
			struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
			pflarray[i] = pfl;
		}
		qsort(pflarray, nitems, sizeof pflarray[0], compare_pfltypes);
		/* rebuild list */
		InitializeListHead(&parsedfiles);
		for (i = 0; i < nitems; ++i) {
			struct parsedfile *const pfl = pflarray[i];
			InsertTailList(&parsedfiles, &pfl->item);
		}
		free(pflarray);
	}

	/* Generate one of required output files */
	if (flag_svd) {
		generate_svd();
	} else if (flag_debug) {
		generate_debug();
	} else {
		generate_cmsis();
	}

	{
		PLIST_ENTRY t;
		/* release memory */

		for (t = parsedfiles.Flink; t != &parsedfiles;) {
			struct parsedfile *const pfl = CONTAINING_RECORD(t, struct parsedfile, item);
			t = t->Flink;

			freeregs(pfl);
			free(pfl);
		}
	}

	return 0;
}

