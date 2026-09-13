/* Q9-compatible OS-9 date display utility. */
#include <stdio.h>
#include <string.h>
#include <time.h>

static unsigned long julian_day(const struct tm *value)
{
    int year;
    int month;
    int day;
    int a;
    int y;
    int m;

    year = value->tm_year + 1900;
    month = value->tm_mon + 1;
    day = value->tm_mday;
    a = (14 - month) / 12;
    y = year + 4800 - a;
    m = month + 12 * a - 3;
    return (unsigned long)(day + (153 * m + 2) / 5 + 365 * y +
                           y / 4 - y / 100 + y / 400 - 32045);
}

static void usage(void)
{
    fputs("Syntax: date [<opts>]\015", stdout);
    fputs("Function: display system date and time\015", stdout);
    fputs("     -m        print hour:minute:sec in military format\015", stdout);
    fputs("     -j        print day, seconds past midnight in julian time\015", stdout);
}

int main(int argc, char **argv)
{
    int military;
    int julian;
    time_t now;
    struct tm *local;

    military = 0;
    julian = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) {
            usage();
            return 0;
        } else if (strcmp(*argv, "-m") == 0) {
            military = 1;
        } else if (strcmp(*argv, "-j") == 0) {
            julian = 1;
        } else {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        }
    }

    if (military && julian) {
        fputs("conflicting options.\015", stderr);
        return 1;
    }

    now = time((time_t *)0);
    local = localtime(&now);
    if (local == NULL) {
        fputs("can't get system time\015", stderr);
        return 1;
    }
    if (julian) {
        printf("Julian date: %lu time: %lu\015", julian_day(local),
               (unsigned long)(local->tm_hour * 3600 + local->tm_min * 60 +
                               local->tm_sec));
        return 0;
    }
    if (military)
        printf("%02d:%02d:%02d\015", local->tm_hour, local->tm_min,
               local->tm_sec);
    else
        printf("%s %d, %d %02d:%02d:%02d\015", local->tm_mon == 0 ? "January" :
               local->tm_mon == 1 ? "February" : local->tm_mon == 2 ? "March" :
               local->tm_mon == 3 ? "April" : local->tm_mon == 4 ? "May" :
               local->tm_mon == 5 ? "June" : local->tm_mon == 6 ? "July" :
               local->tm_mon == 7 ? "August" : local->tm_mon == 8 ? "September" :
               local->tm_mon == 9 ? "October" : local->tm_mon == 10 ? "November" :
               "December", local->tm_mday, local->tm_year + 1900,
               local->tm_hour, local->tm_min, local->tm_sec);
    return 0;
}
