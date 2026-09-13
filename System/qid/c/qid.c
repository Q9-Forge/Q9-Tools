/* qid.c -- extended OS-9 module / relocatable-object / ELF identification.
 *
 * qid is ident's bigger sibling, not a replacement -- ident stays exactly
 * as it is. Where ident only ever prints the mod_exec-shaped fields (wrong
 * for Drvr/Desc/Systm/FMan modules -- see module.h), qid parses the correct
 * type-specific header extension for every OS-9 module type, and on top of
 * that recognizes relocatable object files (ROF, what r68 writes before
 * l68 links things into a module) and ELF files.
 *
 * Locations searched for the requested name pattern(s):
 *   (default)  current DATA directory, wildcard-aware ('*' / '?')
 *   -x         current EXECUTION directory instead of the data directory
 *              (opened via the FAM_EXEC access-mode bit, same mechanism
 *              F$Link/F$Load use for relative pathnames)
 *   -m         modules resident in memory (F$GModDr's directory, then
 *              F$CpyMem to copy each header -- the Technical Reference
 *              Manual is explicit that a module pointer must never be
 *              dereferenced directly: "you should never directly access
 *              the modules... use F$CpyMem")
 *   -a         all three locations combined
 *
 * Output:
 *   (default)  one detailed block per item, ident-style
 *   -q         one concise line per item (name, size, format -- same idea
 *              as ident's -q)
 *   -t         one table row per item with the important fields side by
 *              side
 */
#include <stdio.h>
#include <string.h>
#include <module.h>
#include <modes.h>
#include <direct.h>
#include <dir.h>	/* DIRBLKSIZ -- RBF hands back directory data in blocks
			   of this size, not in single struct dirent records */

/* ---- formats module.h doesn't know about -------------------------------- */

#define ROF_SYNC0 0xDEu
#define ROF_SYNC1 0xADu
#define ROF_SYNC2 0xFAu
#define ROF_SYNC3 0xCEu

#define ELF_MAG0 0x7Fu
#define ELF_MAG1 'E'
#define ELF_MAG2 'L'
#define ELF_MAG3 'F'
#define ELFCLASS32 1
#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define QID_NAMEMAX 64

/* Relocatable Object File header, fixed part (56 bytes), per the OS-9
   Assembler/Linker User Manual, chapter "Relocatable Program Sections" ->
   "Relocatable Object File Format". The module name (null terminated,
   variable length) immediately follows this struct in the file. */
struct rof_header {
	unsigned char	sync[4];
	short		type_lang;
	short		attr_rev;
	short		asm_valid;
	short		series;
	unsigned char	assembled[6];
	short		edition;
	long		static_size;
	long		idata_size;
	long		code_size;
	long		stack_size;
	long		entry_offset;
	long		trap_offset;
	long		remote_static_size;
	long		remote_idata_size;
	long		debug_size;
};

/* raw F$CpyMem ($1B, "Copy External Memory") wrapper -- no library wrapper
   for this syscall exists in module.h/modes.h, so it's hand-written in
   qid_cpymem.a. error_code, 0 on success. */
extern error_code _qid_cpymem(u_int16 owner_pid, u_int32 count, void *src, void *dst);

/* Module-directory memory belongs to the system, not to our own process;
   0 is used here to mean "the system" -- see README.md for how this was
   verified against the running emulator. */
#define QID_MEM_OWNER 0

/* ---- options -------------------------------------------------------------*/

struct options {
	int use_exec;	/* -x */
	int use_mem;	/* -m */
	int use_all;	/* -a */
	int quiet;	/* -q : concise one-liner */
	int table;	/* -t : tabular report */
};

/* ---- a single I/O source, file (disk/exec dir) or live memory ------------ */

typedef struct {
	int is_memory;
	path_id fd;		/* valid when !is_memory */
	void *base;		/* valid when is_memory: module's memory address */
} qid_src;

static int qid_read(qid_src *src, u_int32 offset, void *buf, u_int32 n)
{
	if (src->is_memory)
		return _qid_cpymem(QID_MEM_OWNER, n, (char *)src->base + offset, buf) == 0;
	if (_os_seek(src->fd, offset) != 0) return 0;
	{
		u_int32 got = n;
		if (_os_read(src->fd, buf, &got) != 0 || got != n) return 0;
	}
	return 1;
}

/* OS-9 module names historically end with a high-bit-set character
   instead of a NUL (ident.c decodes only that form). A real xcc-built
   module inspected during development ("qid" itself) turned out to be
   plain NUL-terminated instead -- so this accepts either: it stops on
   a NUL (nothing stored for that byte) or on a byte with the high bit
   set (its low 7 bits are stored as the last character), whichever
   comes first. Works for memory (F$CpyMem chunks) as well as files. */
static int qid_read_os9_name(qid_src *src, u_int32 offset, char *out, size_t outsz)
{
	unsigned char chunk[40];
	size_t have, i, len;

	len = 0;
	while (len + 1 < outsz) {
		have = sizeof chunk;
		if (len + have > 4096) have = 8; /* sanity stop, never used in practice */
		if (!qid_read(src, offset + (u_int32)len, chunk, (u_int32)have)) {
			if (len == 0) return 0;
			break;
		}
		for (i = 0; i < have && len + 1 < outsz; ++i) {
			if (chunk[i] == 0) { out[len] = 0; return 1; }
			out[len++] = (char)(chunk[i] & 0x7f);
			if (chunk[i] & 0x80) { out[len] = 0; return 1; }
		}
		if (i < have) break; /* out buffer got full mid-chunk */
	}
	out[len] = 0;
	return 1; /* truncated: no terminator seen within outsz-1 chars */
}

/* plain null terminated string (ROF module name, ELF section names) */
static int qid_read_cstr(qid_src *src, u_int32 offset, char *out, size_t outsz)
{
	unsigned char ch;
	size_t len = 0;
	for (;;) {
		if (!qid_read(src, offset + (u_int32)len, &ch, 1)) { out[len] = 0; return len > 0; }
		if (ch == 0 || len + 1 >= outsz) { out[len] = 0; return 1; }
		out[len++] = (char)ch;
	}
}

/* ---- small generic helpers ------------------------------------------------*/

static int wildmatch(const char *pattern, const char *name)
{
	for (;;) {
		if (*pattern == 0) return *name == 0;
		if (*pattern == '*') {
			while (*pattern == '*') ++pattern;
			if (*pattern == 0) return 1;
			for (; *name != 0; ++name) if (wildmatch(pattern, name)) return 1;
			return 0;
		}
		if (*name == 0) return 0;
		if (*pattern != '?' && *pattern != *name) return 0;
		++pattern; ++name;
	}
}

/* Directory entry names use the same high-bit-terminated encoding as
   module names (verified on the real thing: raw bytes for "BOOTOBJ" came
   back as plain ASCII with the final 'J' OR-ed with 0x80, not NUL
   terminated). Tolerates a plain NUL too, same as qid_read_os9_name. */
static void decode_dirname(const char *raw, int rawlen, char *out, size_t outsz)
{
	int i;
	size_t len = 0;
	for (i = 0; i < rawlen && len + 1 < outsz; ++i) {
		unsigned char c = (unsigned char)raw[i];
		if (c == 0) break;
		out[len++] = (char)(c & 0x7f);
		if (c & 0x80) break;
	}
	out[len] = 0;
}

static u_int32 read_be32(const unsigned char *p) { return ((u_int32)p[0] << 24) | ((u_int32)p[1] << 16) | ((u_int32)p[2] << 8) | (u_int32)p[3]; }
static u_int32 read_le32(const unsigned char *p) { return ((u_int32)p[3] << 24) | ((u_int32)p[2] << 16) | ((u_int32)p[1] << 8) | (u_int32)p[0]; }
static u_int16 read_be16(const unsigned char *p) { return (u_int16)(((u_int32)p[0] << 8) | p[1]); }
static u_int16 read_le16(const unsigned char *p) { return (u_int16)(((u_int32)p[1] << 8) | p[0]); }

static u_int32 read_u32(const unsigned char *p, int le) { return le ? read_le32(p) : read_be32(p); }
static u_int16 read_u16(const unsigned char *p, int le) { return le ? read_le16(p) : read_be16(p); }

/* 64-bit field, shown as plain hex -- avoids relying on `long long`, which
   the 68k Ultra-C target does not support. */
static void format_hex64(const unsigned char *p, int le, int is64, char *out)
{
	u_int32 hi, lo;
	if (!is64) { sprintf(out, "%08lx", (unsigned long)read_u32(p, le)); return; }
	if (le) { lo = read_u32(p, 1); hi = read_u32(p + 4, 1); }
	else    { hi = read_u32(p, 0); lo = read_u32(p + 4, 0); }
	sprintf(out, "%08lx%08lx", (unsigned long)hi, (unsigned long)lo);
}

/* ---- OS-9 module type/language/attribute decoding ------------------------*/

static const char *module_type_name(unsigned int type)
{
	switch (type) {
	case MT_ANY:     return "Any";
	case MT_PROGRAM: return "Prgrm";
	case MT_SUBROUT: return "Sbrtn";
	case MT_MULTI:   return "Multi";
	case MT_DATA:    return "Data";
	case MT_CSDDATA: return "CSDData";
	case MT_TRAPLIB: return "TrapLib";
	case MT_SYSTEM:  return "Systm";
	case MT_FILEMAN: return "FlMgr";
	case MT_DEVDRVR: return "Drvr";
	case MT_DEVDESC: return "Devic";
	default:         return "?";
	}
}

static const char *module_lang_name(unsigned int lang)
{
	switch (lang) {
	case ML_ANY:      return "Any";
	case ML_OBJECT:   return "Objct";
	case ML_ICODE:    return "ICode";
	case ML_PCODE:    return "PCode";
	case ML_CCODE:    return "CCode";
	case ML_CBLCODE:  return "CblCode";
	case ML_FRTNCODE: return "FrtnCode";
	case ML_JAVACODE: return "JavaCode";
	default:          return "?";
	}
}

static void format_attrs(unsigned int attr, char *out)
{
	out[0] = 0;
	if (attr & MA_REENT) strcat(out, "ReEnt ");
	if (attr & MA_GHOST) strcat(out, "Ghost ");
	if (attr & MA_SUPER) strcat(out, "SupStat ");
	if (out[0] == 0) strcpy(out, "none");
}

/* ---- usage -----------------------------------------------------------------*/

static void usage(void)
{
	fputs("Syntax:   qid [<opts>] {<pattern> [<opts>]}\015", stdout);
	fputs("Function: identify OS-9 modules, relocatable object files and ELF files\015", stdout);
	fputs("     -x        search the execution directory instead of the data directory\015", stdout);
	fputs("     -m        search modules resident in memory (module directory)\015", stdout);
	fputs("     -a        search data directory, execution directory and memory\015", stdout);
	fputs("     -q        concise one-line output (like ident -q)\015", stdout);
	fputs("     -t        tabular output, one row per item\015", stdout);
	fputs("<pattern> may contain '*' and '?' wildcards; default is '*'.\015", stdout);
}

/* ---- module report ---------------------------------------------------------*/

static void print_module_detail(qid_src *src, struct modhcom *common, const char *name)
{
	unsigned int type, lang, attr, revs;
	char attrtext[32];

	type = ((unsigned int)common->_mtylan >> 8) & 0xff;
	lang = (unsigned int)common->_mtylan & 0xff;
	attr = ((unsigned int)common->_mattrev >> 8) & 0xff;
	revs = (unsigned int)common->_mattrev & 0xff;
	format_attrs(attr, attrtext);

	printf("Header for: %s\015", name);
	printf("Type: %s (%u)   Language: %s (%u)   Access: $%04x\015",
		module_type_name(type), type, module_lang_name(lang), lang,
		(unsigned int)common->_maccess);
	printf("Attributes: %s  Revision: %u   Edition: %u\015",
		attrtext, revs, (unsigned int)common->_medit);
	printf("Module size: $%08lx\015", (unsigned long)(u_int32)common->_msize);

	switch (type) {
	case MT_PROGRAM:
	case MT_SUBROUT:
	case MT_MULTI:
	case MT_DATA:
	case MT_CSDDATA:
	case MT_TRAPLIB:
		{
			mod_exec ex;
			if (qid_read(src, 0, &ex, sizeof ex)) {
				printf("Execution offset: $%08lx   Exception offset: $%08lx\015",
					(unsigned long)(u_int32)ex._mexec, (unsigned long)(u_int32)ex._mexcpt);
				printf("Data size: $%08lx   Stack size: $%08lx\015",
					(unsigned long)(u_int32)ex._mdata, (unsigned long)(u_int32)ex._mstack);
				printf("Init-data offset: $%08lx   Data-ref offset: $%08lx\015",
					(unsigned long)(u_int32)ex._midata, (unsigned long)(u_int32)ex._midref);
			}
		}
		break;
	case MT_FILEMAN:
		{
			mh_fman fm;
			if (qid_read(src, 0, &fm, sizeof fm))
				printf("Execution offset: $%08lx   Exception offset: $%08lx  (file managers carry no data/stack)\015",
					(unsigned long)(u_int32)fm._mexec, (unsigned long)(u_int32)fm._mexcpt);
		}
		break;
	case MT_DEVDRVR:
		{
			mod_driver dr;
			if (qid_read(src, 0, &dr, sizeof dr)) {
				printf("Execution offset: $%08lx   Exception offset: $%08lx   Data size: $%08lx\015",
					(unsigned long)(u_int32)dr._mexec, (unsigned long)(u_int32)dr._mexcpt,
					(unsigned long)(u_int32)dr._mdata);
				printf("Routines -> Init:$%04x Read:$%04x Write:$%04x GetStat:$%04x SetStat:$%04x Term:$%04x Error:$%04x\015",
					(unsigned int)dr._mdinit, (unsigned int)dr._mdread, (unsigned int)dr._mdwrite,
					(unsigned int)dr._mdgetstat, (unsigned int)dr._mdsetstt,
					(unsigned int)dr._mdterm, (unsigned int)dr._mderror);
			}
		}
		break;
	case MT_DEVDESC:
		{
			mod_dev dv;
			if (qid_read(src, 0, &dv, sizeof dv)) {
				char fmgr[QID_NAMEMAX], pdev[QID_NAMEMAX];
				if (!qid_read_os9_name(src, (u_int32)(unsigned short)dv._mfmgr, fmgr, sizeof fmgr)) strcpy(fmgr, "?");
				if (!qid_read_os9_name(src, (u_int32)(unsigned short)dv._mpdev, pdev, sizeof pdev)) strcpy(pdev, "?");
				printf("Port: $%08lx   Vector: %u   IRQ-Level: %u   Priority: %u   Mode: $%02x\015",
					(unsigned long)dv._mport, (unsigned int)dv._mvector,
					(unsigned int)dv._mirqlvl, (unsigned int)dv._mpriority, (unsigned int)dv._mmode);
				printf("File Manager: %s   Driver: %s   Device type: %u\015", fmgr, pdev, (unsigned int)dv._mdtype);
				printf("DevCon offset: $%04x   Option table size: %u\015",
					(unsigned int)(unsigned short)dv._mdevcon, (unsigned int)dv._mopt);
			}
		}
		break;
	case MT_SYSTEM:
		{
			mod_config cf;
			if (qid_read(src, 0, &cf, sizeof cf)) {
				char sysgo[QID_NAMEMAX], sysdrive[QID_NAMEMAX], consol[QID_NAMEMAX];
				char clock[QID_NAMEMAX], ioman[QID_NAMEMAX];
				if (!qid_read_os9_name(src, (u_int32)cf._msysgo, sysgo, sizeof sysgo)) strcpy(sysgo, "?");
				if (!qid_read_os9_name(src, (u_int32)cf._msysdrive, sysdrive, sizeof sysdrive)) strcpy(sysdrive, "?");
				if (!qid_read_os9_name(src, (u_int32)cf._mconsol, consol, sizeof consol)) strcpy(consol, "?");
				if (!qid_read_os9_name(src, (u_int32)cf._mclock, clock, sizeof clock)) strcpy(clock, "?");
				if (!qid_read_os9_name(src, (u_int32)cf._mioman, ioman, sizeof ioman)) strcpy(ioman, "?");
				printf("Boot module: %s   System drive: %s   Console: %s\015", sysgo, sysdrive, consol);
				printf("Clock module: %s   IOMan module: %s\015", clock, ioman);
				printf("CPU type: %ld (raw)   Initial priority: %u   Time slice: %u ticks\015",
					(long)cf._mcputyp, (unsigned int)cf._msyspri, (unsigned int)cf._mslice);
				printf("Process table: %u   Path table: %u   Module dir size: %u\015",
					(unsigned int)cf._mprocs, (unsigned int)cf._mpaths, (unsigned int)cf._mmdirsz);
				printf("IRQ stack size: %u longwords   Config flags: $%04x\015",
					(unsigned int)cf._mstacksz, (unsigned int)cf._msysconf);
			}
		}
		break;
	default:
		fputs("(no further fields known for this module type)\015", stdout);
		break;
	}
}

static int identify_module(qid_src *src, const char *location, const char *display, struct options *opt)
{
	struct modhcom common;
	char name[QID_NAMEMAX];

	if (!qid_read(src, 0, &common, sizeof common)) {
		fprintf(stderr, "error reading module header in \"%s\"\015", display);
		return 1;
	}
	if (common._msync != MODSYNC) return -1; /* not a module; caller tries other formats */
	if (!qid_read_os9_name(src, (u_int32)common._mname, name, sizeof name)) strcpy(name, "?");

	if (opt->table) {
		unsigned int type = ((unsigned int)common._mtylan >> 8) & 0xff;
		unsigned int lang = (unsigned int)common._mtylan & 0xff;
		printf("%-6s %-20s %-6s %-16s %10lu\015", location, name, "Modul",
			module_type_name(type), (unsigned long)(u_int32)common._msize);
		(void)lang;
	} else if (opt->quiet) {
		printf("%s %08lx %04x  [%s] %s\015", name, (unsigned long)(u_int32)common._msize,
			(unsigned int)common._mtylan, "Modul", location);
	} else {
		printf("--- %s (%s) ---\015", display, location);
		print_module_detail(src, &common, name);
	}
	return 0;
}

/* ---- ROF report --------------------------------------------------------- */

static int identify_rof(qid_src *src, const char *location, const char *display, struct options *opt)
{
	struct rof_header hdr;
	char name[QID_NAMEMAX];
	unsigned int type, lang, attr, revs;

	if (!qid_read(src, 0, &hdr, sizeof hdr)) return 1;
	if (hdr.sync[0] != ROF_SYNC0 || hdr.sync[1] != ROF_SYNC1 ||
	    hdr.sync[2] != ROF_SYNC2 || hdr.sync[3] != ROF_SYNC3) return -1;
	if (!qid_read_cstr(src, sizeof hdr, name, sizeof name)) strcpy(name, "?");

	type = ((unsigned int)hdr.type_lang >> 8) & 0xff;
	lang = (unsigned int)hdr.type_lang & 0xff;
	attr = ((unsigned int)hdr.attr_rev >> 8) & 0xff;
	revs = (unsigned int)hdr.attr_rev & 0xff;

	if (opt->table) {
		printf("%-6s %-20s %-6s %-16s %10ld\015", location, name, "ROF",
			hdr.type_lang ? module_type_name(type) : "(subr)", (long)hdr.code_size);
	} else if (opt->quiet) {
		printf("%s %08lx ROF  [%s]\015", name, (unsigned long)hdr.code_size, location);
	} else {
		char attrtext[32];
		format_attrs(attr, attrtext);
		printf("--- %s (%s) ---\015", display, location);
		printf("Relocatable Object File (ROF)   Module name: %s\015", name);
		if (hdr.type_lang)
			printf("Type: %s (%u)   Language: %s (%u)   Attributes: %s  Revision: %u\015",
				module_type_name(type), type, module_lang_name(lang), lang, attrtext, revs);
		else
			fputs("Type/Language word is zero -- subroutine-type module (only the mainline segment sets this)\015", stdout);
		printf("Assembly valid: %s   Series: %d   Edition: %u\015",
			hdr.asm_valid ? "NO (errors occurred)" : "yes", (int)hdr.series, (unsigned int)hdr.edition);
		printf("Static size: $%08lx   Init-data size: $%08lx   Object code size: $%08lx\015",
			(unsigned long)hdr.static_size, (unsigned long)hdr.idata_size, (unsigned long)hdr.code_size);
		printf("Stack size: $%08lx   Entry offset: $%08lx   Trap entry offset: $%08lx\015",
			(unsigned long)hdr.stack_size, (unsigned long)hdr.entry_offset, (unsigned long)hdr.trap_offset);
		printf("Remote static: $%08lx   Remote init-data: $%08lx   Debug size: $%08lx\015",
			(unsigned long)hdr.remote_static_size, (unsigned long)hdr.remote_idata_size, (unsigned long)hdr.debug_size);
	}
	return 0;
}

/* ---- ELF report ---------------------------------------------------------- */

static const char *elf_type_name(unsigned int t)
{
	switch (t) {
	case 0: return "NONE"; case 1: return "REL"; case 2: return "EXEC";
	case 3: return "DYN";  case 4: return "CORE";
	default: return "?";
	}
}

static int identify_elf(qid_src *src, const char *location, const char *display, struct options *opt)
{
	unsigned char ident[16];
	unsigned char ehdr[64]; /* big enough for either Elf32_Ehdr (52) or Elf64_Ehdr (64) */
	int is64, le;
	u_int32 entry_off, phoff_off, shoff_off; /* byte offsets within ehdr[] */
	unsigned int etype, machine, ehsize, phentsize, phnum, shentsize, shnum, shstrndx;
	char entry_hex[20], off_hex[20];
	u_int32 phoff32, shoff32;
	int i;

	if (!qid_read(src, 0, ident, sizeof ident)) return 1;
	if (ident[0] != ELF_MAG0 || ident[1] != ELF_MAG1 || ident[2] != ELF_MAG2 || ident[3] != ELF_MAG3) return -1;

	is64 = ident[4] == ELFCLASS64;
	le = ident[5] == ELFDATA2LSB;

	if (!qid_read(src, 0, ehdr, is64 ? 64 : 52)) return 1;

	if (!is64) {
		entry_off = 24; phoff_off = 28; shoff_off = 32;
		etype     = read_u16(ehdr + 16, le);
		machine   = read_u16(ehdr + 18, le);
		ehsize    = read_u16(ehdr + 40, le);
		phentsize = read_u16(ehdr + 42, le);
		phnum     = read_u16(ehdr + 44, le);
		shentsize = read_u16(ehdr + 46, le);
		shnum     = read_u16(ehdr + 48, le);
		shstrndx  = read_u16(ehdr + 50, le);
		phoff32 = read_u32(ehdr + phoff_off, le);
		shoff32 = read_u32(ehdr + shoff_off, le);
	} else {
		entry_off = 24; phoff_off = 32; shoff_off = 40;
		etype     = read_u16(ehdr + 16, le);
		machine   = read_u16(ehdr + 18, le);
		ehsize    = read_u16(ehdr + 52, le);
		phentsize = read_u16(ehdr + 54, le);
		phnum     = read_u16(ehdr + 56, le);
		shentsize = read_u16(ehdr + 58, le);
		shnum     = read_u16(ehdr + 60, le);
		shstrndx  = read_u16(ehdr + 62, le);
		phoff32 = read_u32(ehdr + phoff_off, le); /* low 32 bits; ELF64 offsets in practice never exceed 4G for these tools */
		shoff32 = read_u32(ehdr + shoff_off, le);
	}
	format_hex64(ehdr + entry_off, le, is64, entry_hex);

	if (opt->table) {
		printf("%-6s %-20s %-6s %-16s %10u\015", location, display, is64 ? "ELF64" : "ELF32",
			elf_type_name(etype), (unsigned int)0);
	} else if (opt->quiet) {
		printf("%s ELF%d %s entry=$%s [%s]\015", display, is64 ? 64 : 32, elf_type_name(etype), entry_hex, location);
	} else {
		printf("--- %s (%s) ---\015", display, location);
		printf("Class: ELF%d   Data: %s-endian   Machine: 0x%x   Type: %s (%u)\015",
			is64 ? 64 : 32, le ? "little" : "big", machine, elf_type_name(etype), etype);
		printf("Entry: $%s   Header size: %u\015", entry_hex, ehsize);
		sprintf(off_hex, "%08lx", (unsigned long)phoff32);
		printf("Program headers: %u @ $%s (entsize %u)\015", phnum, off_hex, phentsize);
		sprintf(off_hex, "%08lx", (unsigned long)shoff32);
		printf("Section headers: %u @ $%s (entsize %u, string table index %u)\015", shnum, off_hex, shentsize, shstrndx);

		if (shnum > 0 && shnum < 512) {
			unsigned char shstr_hdr[64];
			u_int32 shstr_off = 0, shstr_size = 0;
			if (qid_read(src, shoff32 + (u_int32)shstrndx * shentsize, shstr_hdr, is64 ? 64 : 40)) {
				shstr_off  = read_u32(shstr_hdr + (is64 ? 24 : 16), le);
				shstr_size = read_u32(shstr_hdr + (is64 ? 32 : 20), le);
			}
			fputs("Sections:\015", stdout);
			for (i = 0; i < (int)shnum; ++i) {
				unsigned char sh[64];
				u_int32 nameoff, type, size, addr;
				char secname[40];
				if (!qid_read(src, shoff32 + (u_int32)i * shentsize, sh, is64 ? 64 : 40)) break;
				nameoff = read_u32(sh + 0, le);
				type    = read_u32(sh + 4, le);
				if (is64) { addr = read_u32(sh + 16, le); size = read_u32(sh + 32, le); }
				else      { addr = read_u32(sh + 12, le); size = read_u32(sh + 20, le); }
				secname[0] = 0;
				if (shstr_size != 0 && nameoff < shstr_size)
					qid_read_cstr(src, shstr_off + nameoff, secname, sizeof secname);
				printf("  [%2d] %-16s type=%lu size=$%08lx addr=$%08lx\015",
					i, secname[0] ? secname : "(unnamed)", (unsigned long)type,
					(unsigned long)size, (unsigned long)addr);
			}
		}
		if (phnum > 0 && phnum < 512) {
			fputs("Segments:\015", stdout);
			for (i = 0; i < (int)phnum; ++i) {
				unsigned char ph[56];
				u_int32 ptype, poff, pvaddr, pfilesz, pmemsz, pflags;
				if (!qid_read(src, phoff32 + (u_int32)i * phentsize, ph, is64 ? 56 : 32)) break;
				if (is64) {
					ptype = read_u32(ph + 0, le); pflags = read_u32(ph + 4, le);
					poff  = read_u32(ph + 8, le); pvaddr = read_u32(ph + 16, le);
					pfilesz = read_u32(ph + 32, le); pmemsz = read_u32(ph + 40, le);
				} else {
					ptype = read_u32(ph + 0, le); poff = read_u32(ph + 4, le);
					pvaddr = read_u32(ph + 8, le); pfilesz = read_u32(ph + 16, le);
					pmemsz = read_u32(ph + 20, le); pflags = read_u32(ph + 24, le);
				}
				printf("  [%d] type=%lu off=$%08lx vaddr=$%08lx filesz=$%08lx memsz=$%08lx flags=%c%c%c\015",
					i, (unsigned long)ptype, (unsigned long)poff, (unsigned long)pvaddr,
					(unsigned long)pfilesz, (unsigned long)pmemsz,
					(pflags & 4) ? 'R' : '-', (pflags & 2) ? 'W' : '-', (pflags & 1) ? 'X' : '-');
			}
		}
	}
	return 0;
}

/* ---- top-level dispatch ---------------------------------------------------*/

static int identify(qid_src *src, const char *location, const char *display, struct options *opt)
{
	int r;
	if ((r = identify_module(src, location, display, opt)) >= 0) return r;
	if ((r = identify_rof(src, location, display, opt)) >= 0) return r;
	if ((r = identify_elf(src, location, display, opt)) >= 0) return r;
	fprintf(stderr, "\"%s\": unrecognized format (not a module, ROF or ELF file)\015", display);
	return 1;
}

/* ---- scanning the data / execution directory ------------------------------*/

/* RBF hands back raw directory sectors, not individual 32-byte records --
   a read shorter than a full DIRBLKSIZ (256 byte) block reads nothing (an
   empty scan, no error). Verified against the real thing: opendir()'s own
   DIR struct (dir.h) buffers in DIRBLKSIZ chunks for exactly this reason. */
static int scan_directory(int use_exec, const char **patterns, int npatterns, struct options *opt)
{
	path_id dirfd;
	unsigned char block[DIRBLKSIZ];
	u_int32 n;
	int status = 0, matched_any = 0;
	const char *location = use_exec ? "exec" : "disk";
	u_int32 open_mode = FAM_READ | FAM_DIR | (use_exec ? FAM_EXEC : 0);

	if (_os_open(".", open_mode, &dirfd) != 0) {
		fprintf(stderr, "can't open %s directory\015", location);
		return 1;
	}
	for (;;) {
		unsigned int slot, slots;
		error_code err;
		n = sizeof block;
		err = _os_read(dirfd, block, &n);
		if (err != 0 || n == 0) break;
		slots = n / sizeof(struct dirent);
		for (slot = 0; slot < slots; ++slot) {
			struct dirent *entry = (struct dirent *)(block + slot * sizeof(struct dirent));
			char name[29];
			int p;

			if (entry->dir_addr == 0) continue; /* empty/deleted slot */
			decode_dirname(entry->dir_name, 28, name, sizeof name);
			if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;
			for (p = 0; p < npatterns; ++p) {
				if (!wildmatch(patterns[p], name)) continue;
				matched_any = 1;
				{
					path_id fd;
					u_int32 fmode = FAM_READ | (use_exec ? FAM_EXEC : 0);
					if (_os_open(name, fmode, &fd) != 0) {
						fprintf(stderr, "can't open \"%s\"\015", name);
						status = 1;
						break;
					}
					{
						qid_src src;
						src.is_memory = 0; src.fd = fd;
						status |= identify(&src, location, name, opt);
					}
					_os_close(fd);
				}
				break; /* one match per entry is enough */
			}
		}
		if (n < sizeof block) break; /* short block: end of directory */
	}
	_os_close(dirfd);
	if (!matched_any) fprintf(stderr, "no matching files in %s directory\015", location);
	return status;
}

/* ---- scanning the module directory (memory) ------------------------------*/

static int scan_memory(const char **patterns, int npatterns, struct options *opt)
{
	static mod_dir directory[256];
	u_int32 dirsize;
	unsigned int count, i;
	int status = 0, matched_any = 0;

	dirsize = sizeof directory;
	if (_os_get_moddir(directory, &dirsize) != 0) {
		fprintf(stderr, "can't get module directory\015");
		return 1;
	}
	count = dirsize / sizeof directory[0];
	if (count > 256) count = 256;

	for (i = 0; i < count; ++i) {
		qid_src src;
		char name[QID_NAMEMAX];
		struct modhcom common;
		int p;

		if (directory[i].md_mptr == NULL) continue;
		src.is_memory = 1; src.fd = 0; src.base = directory[i].md_mptr;

		if (!qid_read(&src, 0, &common, sizeof common) || common._msync != MODSYNC) continue;
		if (!qid_read_os9_name(&src, (u_int32)common._mname, name, sizeof name)) continue;

		for (p = 0; p < npatterns; ++p) {
			char addrtext[16];
			if (!wildmatch(patterns[p], name)) continue;
			matched_any = 1;
			sprintf(addrtext, "$%08lx", (unsigned long)directory[i].md_mptr);
			status |= identify(&src, "mem", name, opt);
			break;
		}
	}
	if (!matched_any) fprintf(stderr, "no matching modules resident in memory\015");
	return status;
}

/* ---- main ------------------------------------------------------------------*/

int main(int argc, char **argv)
{
	struct options opt;
	/* The shell expands '*' itself before qid ever sees it -- a directory
	   with a few hundred files means a few hundred argv entries, not one
	   literal "*". Sized generously for that. */
	const char *patterns[512];
	int npatterns = 0;
	int status = 0;

	opt.use_exec = opt.use_mem = opt.use_all = opt.quiet = opt.table = 0;

	while (--argc > 0) {
		++argv;
		if (strcmp(*argv, "-?") == 0) { usage(); return 0; }
		else if (strcmp(*argv, "-x") == 0) opt.use_exec = 1;
		else if (strcmp(*argv, "-m") == 0) opt.use_mem = 1;
		else if (strcmp(*argv, "-a") == 0) opt.use_all = 1;
		else if (strcmp(*argv, "-q") == 0) opt.quiet = 1;
		else if (strcmp(*argv, "-t") == 0) opt.table = 1;
		else if (**argv == '-') { fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1; }
		else if (npatterns < 512) patterns[npatterns++] = *argv;
		else { fprintf(stderr, "too many names/patterns, ignoring \"%s\"\015", *argv); }
	}
	if (npatterns == 0) patterns[npatterns++] = "*";

	if (opt.table) fputs("Loc    Name                 Format Type/Class            Size\015", stdout);

	if (opt.use_all) {
		status |= scan_directory(0, patterns, npatterns, &opt);
		status |= scan_directory(1, patterns, npatterns, &opt);
		status |= scan_memory(patterns, npatterns, &opt);
	} else if (opt.use_exec || opt.use_mem) {
		if (opt.use_exec) status |= scan_directory(1, patterns, npatterns, &opt);
		if (opt.use_mem)  status |= scan_memory(patterns, npatterns, &opt);
	} else {
		status |= scan_directory(0, patterns, npatterns, &opt);
	}
	return status;
}
