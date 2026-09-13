/* Q9 wrapper for linking modules already present in OS-9 memory. */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <module.h>

static void usage(void)
{
    fputs("Syntax: link [<opts>] {<modname> [<opts>]}\015", stdout);
    fputs("Function: link a module in memory\015", stdout);
    fputs("     -z=<path> get list of module names from <path>\015", stdout);
    fputs("     -z        get list of module names from standard input\015", stdout);
}

static int link_one(const char *text)
{
    char *name;
    mh_com *header;
    void *data;
    u_int16 type_lang;
    u_int16 attr_rev;
    error_code error;

    name = (char *)text;
    type_lang = 0;
    attr_rev = 0;
    error = _os_link(&name, &header, &data, &type_lang, &attr_rev);
    if (error != 0) {
        fprintf(stderr, "can't link \"%s\".  Error #%03d\015", text, error);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    FILE *list;
    char line[256];
    int status;
    int module_count;

    list = NULL;
    status = 0;
    module_count = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) {
            usage();
            return 0;
        } else if (strcmp(*argv, "-z") == 0) {
            list = stdin;
        } else if (strncmp(*argv, "-z=", 3) == 0) {
            list = fopen(*argv + 3, "r");
            if (list == NULL) {
                fprintf(stderr, "can't open \"%s\".\015", *argv + 3);
                return 1;
            }
        } else if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        } else if (link_one(*argv) != 0) {
            status = 1;
            ++module_count;
        } else {
            ++module_count;
        }
    }
    if (list == NULL) {
        if (status == 0 && module_count == 0)
            fputs("you must specify -z or module names\015", stderr);
        return module_count == 0 ? 1 : status;
    }
    while (fgets(line, sizeof line, list) != NULL) {
        char *end;
        end = line + strlen(line);
        while (end > line && (end[-1] == '\015' || end[-1] == '\012'))
            --end;
        *end = 0;
        if (*line != 0 && link_one(line) != 0)
            status = 1;
    }
    if (list != stdin)
        fclose(list);
    return status;
}
