/* Q9 touch: update the RBF file descriptor date. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <rbf.h>
#include <modes.h>

static void usage(void)
{
    fputs("Syntax: touch [<opts>] {<path> [<opts>]}\015", stdout);
    fputs("Function: update the date of a file\015", stdout);
    fputs("     -c        don't create files\015", stdout);
    fputs("     -q        don't quit on error\015", stdout);
    fputs("     -?        display this help\015", stdout);
}

static int touch_one(const char *name, int no_create)
{
    path_id path;
    fd_stats fd;
    time_t now;
    struct tm *tmv;
    error_code err;

    err = _os_open(name, FAM_READ | FAM_WRITE, &path);
    if (err != 0 && !no_create)
        err = _os_create(name, FAM_READ | FAM_WRITE, &path, 0);
    if (err != 0) {
        fprintf(stderr, "can't open \"%s\"\015", name);
        return 1;
    }
    err = _os_gs_fd(path, 0, &fd);
    if (err == 0) {
        now = time((time_t *)0);
        tmv = localtime(&now);
        if (tmv == (struct tm *)0)
            err = 1;
        else {
            /* OS-9 stores the year as years since 1900. */
            fd.fd_date[0] = (u_char)tmv->tm_year;
            fd.fd_date[1] = (u_char)(tmv->tm_mon + 1);
            fd.fd_date[2] = (u_char)tmv->tm_mday;
            fd.fd_date[3] = (u_char)tmv->tm_hour;
            fd.fd_date[4] = (u_char)tmv->tm_min;
            err = _os_ss_fd(path, &fd);
        }
    }
    _os_close(path);
    if (err != 0) {
        fprintf(stderr, "can't set file date for \"%s\"\015", name);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    int no_create = 0, quiet = 0, status = 0, names = 0;
    while (--argc > 0) {
        ++argv;
        if (!strcmp(*argv, "-?")) { usage(); return 0; }
        if (!strcmp(*argv, "-c")) { no_create = 1; continue; }
        if (!strcmp(*argv, "-q")) { quiet = 1; continue; }
        if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        }
        ++names;
        if (touch_one(*argv, no_create) != 0) {
            status = 1;
            if (!quiet) break;
        }
    }
    if (names == 0) { fputs("no input files\015", stderr); return 1; }
    return status;
}
