/* Q9-compatible OS-9 environment display utility. */
#include <stdio.h>
#include <string.h>

/* Microware's C runtime exports the process environment as _environ. */
extern char **_environ;

int main(int argc, char **argv)
{
    char **place;
    if (argc > 1) {
        if (argc == 2 && strcmp(argv[1], "-?") == 0) {
            fputs("Syntax:   printenv [<opt>]\015Function: display list of environment variables\015Options:  none.\015", stdout);
            return 0;
        }
        fputs("unknown option\015", stderr); return 1;
    }
    for (place = _environ; place != NULL && *place != NULL; ++place)
        printf("%s\015", *place);
    return 0;
}
