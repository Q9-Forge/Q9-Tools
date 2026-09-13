/* Q9-compatible OS-9 sleep utility. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

static void usage(void)
{
    fputs("Syntax: sleep [<opts>] [<count>] [<opts>]\015", stdout);
    fputs("Function: suspend process for ticks/seconds/until signalled\015", stdout);
    fputs("     -s        count represents seconds\015", stdout);
}

int main(int argc, char **argv)
{
    int seconds_mode;
    u_int32 count;
    u_int32 ticks;
    char *end;
    int count_seen;

    seconds_mode = 0;
    count = 0;
    count_seen = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) {
            usage();
            return 0;
        } else if (strcmp(*argv, "-s") == 0) {
            seconds_mode = 1;
        } else if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        } else {
            if (count_seen) {
                fprintf(stderr, "unknown parameter \"%s\"\015", *argv);
                return 1;
            }
            count = (u_int32)strtoul(*argv, &end, 0);
            if (*end != 0) {
                fprintf(stderr, "unknown parameter \"%s\"\015", *argv);
                return 1;
            }
            count_seen = 1;
        }
    }
    ticks = seconds_mode ? count * 60 : count;
    if (_os9_sleep(&ticks) != 0) {
        fputs("sleep interrupted\015", stderr);
        return 1;
    }
    return 0;
}
