/* Q9-compatible OS-9 file rename utility. */
#include <stdio.h>
#include <string.h>

static void usage(void)
{
    fputs("Syntax:   rename [<opts>] <path> <name> [<opts>]\015", stdout);
    fputs("Function: rename a file or directory\015", stdout);
    fputs("     -x        path starts from execution directory\015", stdout);
}

int main(int argc, char **argv)
{
    const char *old_name;
    const char *new_name;
    int names;

    old_name = NULL;
    new_name = NULL;
    names = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) { usage(); return 0; }
        if (strcmp(*argv, "-x") == 0) continue;
        if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        }
        ++names;
        if (old_name == NULL) old_name = *argv;
        else if (new_name == NULL) new_name = *argv;
        else { fputs("too many filenames\015", stderr); return 1; }
    }
    if (names != 2) {
        fputs("you must specify old and new names\015", stderr);
        return 1;
    }
    if (rename(old_name, new_name) != 0) {
        fputs("can't rename file.\015", stderr);
        return 1;
    }
    return 0;
}
