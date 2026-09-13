/* Q9-compatible OS-9 file listing utility. */
#include <stdio.h>
#include <string.h>

static void usage(void)
{
    fputs("Syntax:   list [<opts>] {<path> [<opts>]}\015", stdout);
    fputs("Function: list a file\015", stdout);
    fputs("     -z=<path> get list of file names from <path>\015", stdout);
    fputs("     -z        get list of file names from standard input\015", stdout);
    fputs("     -x        look in execution directory\015", stdout);
}

static int list_one(const char *path)
{
    FILE *input;
    char buffer[512];
    int count;

    input = fopen(path, "r");
    if (input == NULL) {
        fprintf(stderr, "can't open '%s'\015", path);
        return 1;
    }
    while ((count = fread(buffer, 1, sizeof buffer, input)) > 0)
        if (fwrite(buffer, 1, count, stdout) != count) {
            fclose(input);
            return 1;
        }
    fclose(input);
    return 0;
}

static int list_names(FILE *input)
{
    char line[256];
    int status;

    status = 0;
    while (fgets(line, sizeof line, input) != NULL) {
        char *end;
        end = line + strlen(line);
        while (end > line && (end[-1] == '\015' || end[-1] == '\012')) --end;
        *end = 0;
        if (*line != 0 && list_one(line) != 0) status = 1;
    }
    return status;
}

int main(int argc, char **argv)
{
    FILE *list;
    int status;
    int names;

    list = NULL;
    status = 0;
    names = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) { usage(); return 0; }
        else if (strcmp(*argv, "-z") == 0) list = stdin;
        else if (strncmp(*argv, "-z=", 3) == 0) {
            list = fopen(*argv + 3, "r");
            if (list == NULL) { fprintf(stderr, "can't open '%s'\015", *argv + 3); return 1; }
        } else if (strcmp(*argv, "-x") == 0) {
            /* Kept for command-line compatibility; path resolution follows later. */
        } else if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1;
        } else {
            ++names;
            if (list_one(*argv) != 0) status = 1;
        }
    }
    if (list != NULL) status |= list_names(list);
    if (list == NULL && names == 0) { fputs("you must specify -z or file names\015", stderr); status = 1; }
    if (list != NULL && list != stdin) fclose(list);
    return status;
}
