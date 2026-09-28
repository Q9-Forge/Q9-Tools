/* Q9-compatible OS-9 module identification utility. */
#include <stdio.h>
#include <string.h>
#include <module.h>

static void usage(void)
{
    fputs("Syntax:   ident [<opts>] {<file> [<opts>]}\015", stdout);
    fputs("Function: display module information\015", stdout);
    fputs("     -z=<file> get list of module names from <file>\015", stdout);
    fputs("     -z        get list of module names from standard input\015", stdout);
    fputs("     -q        alternate concise information\015", stdout);
}

static const char *type_name(unsigned int type)
{
    static const char *names[] = { "Any", "Prog", "Subr", "Multi", "Data", "CData", "CData", "?", "?", "?", "?", "Trap", "Sys", "FMan", "Drvr", "Desc" };
    return type < 16 ? names[type] : "?";
}

static unsigned long get32(const unsigned char *p, int little)
{
    if (little) return ((unsigned long)p[3] << 24) | ((unsigned long)p[2] << 16) | ((unsigned long)p[1] << 8) | p[0];
    return ((unsigned long)p[0] << 24) | ((unsigned long)p[1] << 16) | ((unsigned long)p[2] << 8) | p[3];
}

static unsigned int get16(const unsigned char *p, int little)
{
    return little ? (unsigned int)(p[1] << 8) | p[0] : (unsigned int)(p[0] << 8) | p[1];
}

static int count_rof_signatures(FILE *input)
{
    int a, b, c, d, count = 0;
    rewind(input);
    a = fgetc(input); b = fgetc(input); c = fgetc(input); d = fgetc(input);
    while (d != EOF) {
        if (a == 0xde && b == 0xad && c == 0xfa && d == 0xce) ++count;
        a = b; b = c; c = d; d = fgetc(input);
    }
    rewind(input);
    return count;
}

/* Identify container/object signatures before interpreting an OS-9 header. */
static int identify_other(FILE *input, const char *path, int quiet)
{
    unsigned char b[64];
    size_t n;
    n = fread((char *)b, 1, sizeof b, input);
    if (n >= 4 && b[0] == 0x7f && b[1] == 'E' && b[2] == 'L' && b[3] == 'F') {
        if (n < 16) { fprintf(stderr, "incomplete ELF header in file \"%s\"\015", path); return 1; }
        if (quiet) printf("%s ELF%u %s-endian\015", path, b[4] == 2 ? 64 : 32, b[5] == 1 ? "little" : "big");
        else printf("Format: ELF%u, %s-endian, class %u, data encoding %u\015", b[4] == 2 ? 64 : 32, b[5] == 1 ? "little" : "big", b[4], b[5]);
        return 0;
    }
    if (n >= 4 && b[0] == 0xde && b[1] == 0xad && b[2] == 0xfa && b[3] == 0xce) {
        int count = count_rof_signatures(input);
        if (quiet) printf("%s %s\015", path, count > 1 ? "ROF-LIB?" : "ROF");
        else if (count > 1) printf("Format: possible concatenated ROF library (%d ROF signatures; heuristic)\015", count);
        else printf("Format: Microware ROF relocatable object\015");
        return 0;
    }
    if (n >= 4 && b[0] == 0x2d && b[1] == 0x00 && b[2] == 0xd5 && b[3] == 0xbc) {
        if (quiet) printf("%s LIB-GEN\015", path);
        else printf("Format: Microware LIB-GEN archive (signature 2D00D5BC); detailed index decoding not implemented\015");
        return 0;
    }
    return -1;
}

/* The classic OS-9/6809 header is the 9-byte common header, not mod_exec.
   Its $87CD sync word is distinct from the 68000/OS-9000 $4AFC header. */
static int identify_6809(FILE *input, const char *path, int quiet)
{
    unsigned char h[9], check;
    unsigned int size, name_offset, type, lang, attrrev;
    char name[64];
    int i, value;
    static const char *languages[] = { "data", "6809 object code", "BASIC09 I-code", "PASCAL P-code", "COBOL I-code" };

    rewind(input);
    if (fread((char *)h, 1, 2, input) != 2 || h[0] != 0x87 || h[1] != 0xcd) {
        rewind(input);
        return -1;
    }
    if (fread((char *)h + 2, 1, 7, input) != 7) {
        fprintf(stderr, "incomplete OS-9/6809 module header in file \"%s\"\015", path);
        return 1;
    }
    check = 0;
    for (i = 0; i < 8; ++i) check ^= h[i];
    if ((unsigned char)~check != h[8]) {
        fprintf(stderr, "OS-9/6809 header check wrong in file \"%s\"\015", path);
        return 1;
    }
    size = get16(h + 2, 0);
    name_offset = get16(h + 4, 0);
    type = h[6] >> 4;
    lang = h[6] & 0x0f;
    attrrev = h[7];
    if (size < 12 || name_offset < 9 || name_offset >= size - 3 ||
        fseek(input, (long)name_offset, 0) != 0) {
        fprintf(stderr, "invalid OS-9/6809 module header in file \"%s\"\015", path);
        return 1;
    }
    for (i = 0; i < (int)sizeof name - 1; ++i) {
        value = fgetc(input);
        if (value == EOF || name_offset + i >= size - 3) {
            fprintf(stderr, "can't read OS-9/6809 module name in \"%s\"\015", path);
            return 1;
        }
        name[i] = (char)(value & 0x7f);
        if (value & 0x80) break;
    }
    if (i >= (int)sizeof name - 1) {
        fprintf(stderr, "OS-9/6809 module name too long in \"%s\"\015", path);
        return 1;
    }
    name[i + 1] = 0;
    if (quiet) {
        printf("%s %04x %02x OS-9/6809\015", name, size, h[6]);
    } else {
        printf("Format: OS-9/6809 module (sync $87CD; 9-byte common header)\015");
        printf("Header for: %s\015", name);
        printf("Module size: $%04x  Name offset: $%04x\015", size, name_offset);
        printf("Type: %s (%u)  Language: %s (%u)\015", type_name(type), type,
               lang < sizeof languages / sizeof languages[0] ? languages[lang] : "reserved", lang);
        printf("Attributes/Revision: $%02x  Header check: valid\015", attrrev);
    }
    return 0;
}

static int identify(const char *path, int quiet)
{
    FILE *input;
    mod_exec header;
    unsigned char raw[sizeof(mod_exec)];
    char name[64];
    long name_offset;
    int little;

    input = fopen(path, "r");
    if (input == NULL) { fprintf(stderr, "error reading file \"%s\"\015", path); return 1; }
    {
        int other = identify_other(input, path, quiet);
        if (other >= 0) { fclose(input); return other; }
    }
    {
        int legacy = identify_6809(input, path, quiet);
        if (legacy >= 0) { fclose(input); return legacy; }
    }
    rewind(input);
    if (fread((char *)raw, 1, sizeof raw, input) != sizeof raw) {
        fclose(input); fprintf(stderr, "incomplete module header in file \"%s\"\015", path); return 1;
    }
    little = raw[0] == 0xfc && raw[1] == 0x4a;
    if (!(raw[0] == 0x4a && raw[1] == 0xfc) && !little) {
        fclose(input); fprintf(stderr, "module sync wrong in file \"%s\"\015", path); return 1;
    }
    memcpy((char *)&header, (char *)raw, sizeof header);
    if (little) {
        header._mh._msysrev = (short)get16(raw + 2, 1);
        header._mh._msize = (int)get32(raw + 4, 1);
        header._mh._mname = (int)get32(raw + 12, 1);
        header._mh._maccess = (short)get16(raw + 16, 1);
        header._mh._mtylan = (short)get16(raw + 18, 1);
        header._mh._mattrev = (short)get16(raw + 20, 1);
        header._mh._medit = (short)get16(raw + 22, 1);
        header._mexec = (int)get32(raw + 48, 1);
        header._mexcpt = (int)get32(raw + 52, 1);
        header._mdata = (int)get32(raw + 56, 1);
        header._mstack = (int)get32(raw + 60, 1);
    }
    name_offset = header._mh._mname;
    if (fseek(input, name_offset, 0) != 0) {
        fclose(input); fprintf(stderr, "can't read module name in \"%s\"\015", path); return 1;
    }
    {
        int value;
        int length = 0;
        do {
            value = fgetc(input);
            if (value == EOF || length >= (int)sizeof name - 1) {
                fclose(input); fprintf(stderr, "can't read module name in \"%s\"\015", path); return 1;
            }
            name[length++] = (char)(value & 0x7f);
        } while ((value & 0x80) == 0);
        name[length] = 0;
    }
    if (quiet)
        printf("%s %08lx %04x %s\015", name, (unsigned long)header._mh._msize, header._mh._mtylan, little ? "OS-9000-LE" : "OS-9-BE");
    else {
        printf("Format: %s-endian OS-9 module header (6809/68k family; CPU is not encoded in this header)\015", little ? "OS-9000 little" : "big");
        printf("Header for: %s\015", name);
        printf("Module size: $%08lx  Edition: %u\015", (unsigned long)header._mh._msize, (unsigned int)header._mh._medit);
        printf("Type: %s  Language: %u  Access: $%04x\015", type_name(((unsigned int)header._mh._mtylan >> 8) & 0xff), (unsigned int)header._mh._mtylan & 0xff, (unsigned int)header._mh._maccess);
        printf("Execution offset: $%08lx  Data size: $%08lx  Stack: $%08lx\015", (unsigned long)header._mexec, (unsigned long)header._mdata, (unsigned long)header._mstack);
    }
    fclose(input);
    return 0;
}

static int identify_list(FILE *input, int quiet)
{
    char line[256];
    int status;
    status = 0;
    while (fgets(line, sizeof line, input) != NULL) {
        char *end = line + strlen(line);
        while (end > line && (end[-1] == '\015' || end[-1] == '\012')) --end;
        *end = 0;
        if (*line != 0 && identify(line, quiet) != 0) status = 1;
    }
    return status;
}

int main(int argc, char **argv)
{
    FILE *list;
    int quiet, status, names;
    list = NULL; quiet = status = names = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) { usage(); return 0; }
        else if (strcmp(*argv, "-q") == 0) quiet = 1;
        else if (strcmp(*argv, "-z") == 0) list = stdin;
        else if (strncmp(*argv, "-z=", 3) == 0) {
            list = fopen(*argv + 3, "r");
            if (list == NULL) { fprintf(stderr, "can't open '%s'\015", *argv + 3); return 1; }
        } else if (**argv == '-') { fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1; }
        else { ++names; if (identify(*argv, quiet) != 0) status = 1; }
    }
    if (list != NULL) status |= identify_list(list, quiet);
    if (list == NULL && names == 0) { fputs("you must specify -z or file/module names\015", stderr); status = 1; }
    if (list != NULL && list != stdin) fclose(list);
    return status;
}
