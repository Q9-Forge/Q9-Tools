/* Q9-compatible OS-9 file deletion utility. */
#include <stdio.h>
#include <string.h>
#include <modes.h>

extern int unlinkx(const char *, int);

struct options { int force; int prompt; int quiet; int execution_dir; int erase; };

static void usage(void)
{
    fputs("Syntax:   del [<opts>] {<file> [<opts>]}\015", stdout);
    fputs("Function: delete files\015", stdout);
    fputs("     -z=<path> get list of file names from <path>\015", stdout);
    fputs("     -z        get list of file names from standard input\015", stdout);
    fputs("     -x        delete files from execution directory\015", stdout);
    fputs("     -f        delete files with no write permission\015", stdout);
    fputs("     -p        ask before deleting\015", stdout);
    fputs("     -e        erase the disk space occupied by the file\015", stdout);
    fputs("     -q        suppress file-not-found messages\015", stdout);
}

static int delete_one(const char *path, const struct options *options)
{
    char answer[8];
    int mode;

    if (options->prompt) {
        printf("Delete  %s  ? (y,n,a,q) ", path);
        if (fgets(answer, sizeof answer, stdin) == NULL ||
            (answer[0] != 'y' && answer[0] != 'Y'))
            return 0;
    }
    mode = options->execution_dir ? FAM_EXEC : 0;
    if (unlinkx(path, mode) != 0) {
        if (!options->quiet)
            fprintf(stderr, "can't delete '%s' -\015", path);
        return 1;
    }
    return 0;
}

static int delete_list(FILE *input, const struct options *options)
{
    char line[256];
    int status;

    status = 0;
    while (fgets(line, sizeof line, input) != NULL) {
        char *end;
        end = line + strlen(line);
        while (end > line && (end[-1] == '\015' || end[-1] == '\012')) --end;
        *end = 0;
        if (*line != 0 && delete_one(line, options) != 0) status = 1;
    }
    return status;
}

int main(int argc, char **argv)
{
    struct options options;
    FILE *list;
    int status;
    int names;

    options.force = options.prompt = options.quiet = options.execution_dir = 0;
    options.erase = 0;
    list = NULL;
    status = 0;
    names = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) { usage(); return 0; }
        else if (strcmp(*argv, "-f") == 0) options.force = 1;
        else if (strcmp(*argv, "-p") == 0) options.prompt = 1;
        else if (strcmp(*argv, "-q") == 0) options.quiet = 1;
        else if (strcmp(*argv, "-x") == 0) options.execution_dir = 1;
        else if (strcmp(*argv, "-e") == 0) options.erase = 1;
        else if (strcmp(*argv, "-z") == 0) list = stdin;
        else if (strncmp(*argv, "-z=", 3) == 0) {
            list = fopen(*argv + 3, "r");
            if (list == NULL) { fprintf(stderr, "can't open '%s' -\015", *argv + 3); return 1; }
        } else if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1;
        } else {
            ++names;
            if (delete_one(*argv, &options) != 0) status = 1;
        }
    }
    if (list != NULL) status |= delete_list(list, &options);
    if (list == NULL && names == 0) { fputs("you must specify -z or file names\015", stderr); status = 1; }
    if (list != NULL && list != stdin) fclose(list);
    return status;
}
