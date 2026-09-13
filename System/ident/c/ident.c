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

static int identify(const char *path, int quiet)
{
    FILE *input;
    mod_exec header;
    char name[64];
    long name_offset;

    input = fopen(path, "r");
    if (input == NULL) { fprintf(stderr, "error reading file \"%s\"\015", path); return 1; }
    if (fread((char *)&header, 1, sizeof header, input) != sizeof header) {
        fclose(input); fprintf(stderr, "incomplete module header in file \"%s\"\015", path); return 1;
    }
    if (header._mh._msync != MODSYNC) {
        fclose(input); fprintf(stderr, "module sync wrong in file \"%s\"\015", path); return 1;
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
        printf("%s %08lx %04x\015", name, (unsigned long)header._mh._msize, header._mh._mtylan);
    else {
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
