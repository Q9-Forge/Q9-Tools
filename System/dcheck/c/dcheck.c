/* Q9 dcheck: read-only RBF volume sanity check. */
#include <stdio.h>
#include <string.h>
#include <modes.h>
#include <rbf.h>

static void usage(void)
{
    fputs("Syntax: dcheck [<opts>] {<device>}\015", stdout);
    fputs("Function: check basic RBF volume information\015", stdout);
    fputs("     -q        quiet mode\015", stdout);
    fputs("     -?        display this help\015", stdout);
    fputs("Note: this Q9 basis is read-only; repair is not enabled.\015", stdout);
}

static u_int32 be24(const u_char *p)
{
    return ((u_int32)p[0] << 16) | ((u_int32)p[1] << 8) | p[2];
}

static int check_one(const char *name, int quiet)
{
    path_id path;
    u_char sector[256];
    u_int32 count = 256;
    Sector0 s;
    error_code err;
    int i;

    err = _os_open(name, FAM_READ, &path);
    if (err) { fprintf(stderr, "can't open \"%s\" - Error #%03d\015", name, err); return 1; }
    err = _os_read(path, sector, &count);
    _os_close(path);
    if (err || count < 64) { fprintf(stderr, "can't read sector 0 of \"%s\"\015", name); return 1; }
    s = (Sector0)sector;
    if (!quiet) {
        fputs(name, stdout);
        fputs(": ", stdout);
        for (i = 0; i < 32 && s->dd_name[i]; ++i) putchar(s->dd_name[i]);
        printf(" sectors=%lu cluster=%u bitmap-bytes=%u\015",
               (unsigned long)be24(s->dd_tot), (unsigned)s->dd_bit,
               (unsigned)s->dd_map);
    }
    if (be24(s->dd_tot) == 0 || s->dd_bit == 0 || s->dd_map == 0) {
        fprintf(stderr, "invalid RBF volume parameters in \"%s\"\015", name);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    int quiet = 0, names = 0, status = 0;
    while (--argc > 0) {
        ++argv;
        if (!strcmp(*argv, "-?")) { usage(); return 0; }
        if (!strcmp(*argv, "-q")) { quiet = 1; continue; }
        if (**argv == '-') { fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1; }
        ++names;
        if (check_one(*argv, quiet)) status = 1;
    }
    if (!names) { fputs("no devices specified\015", stderr); return 1; }
    return status;
}
