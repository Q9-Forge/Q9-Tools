/* Q9 paths: inspect the OS-9 path descriptor of named paths. */
#include <stdio.h>
#include <string.h>
#include <modes.h>
#include <sg_codes.h>

static void usage(void)
{
    fputs("Syntax: paths [<opts>] {<path>}\015", stdout);
    fputs("Function: display path/device information\015", stdout);
    fputs("     -h        display numeric values in hexadecimal\015", stdout);
    fputs("     -?        display this help\015", stdout);
}

static int inspect(const char *name, int hex)
{
    path_id path;
    char dev[64];
    u_int32 pos = 0, size = 0;
    error_code e;

    e = _os_open(name, FAM_READ, &path);
    if (e) {
        fprintf(stderr, "can't open '%s' - Error #%03d\015", name, e);
        return 1;
    }
    dev[0] = 0;
    _os_gs_devnm(path, dev);
    _os_gs_pos(path, &pos);
    _os_gs_size(path, &size);
    if (hex)
        printf("%s: path=%04lx device=%s pos=%08lx size=%08lx\015",
               name, (unsigned long)path, dev,
               (unsigned long)pos, (unsigned long)size);
    else
        printf("%s: path=%lu device=%s pos=%lu size=%lu\015",
               name, (unsigned long)path, dev,
               (unsigned long)pos, (unsigned long)size);
    _os_close(path);
    return 0;
}

int main(int argc, char **argv)
{
    int hex = 0, status = 0, names = 0;
    while (--argc > 0) {
        ++argv;
        if (!strcmp(*argv, "-?")) { usage(); return 0; }
        if (!strcmp(*argv, "-h")) { hex = 1; continue; }
        if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        }
        ++names;
        if (inspect(*argv, hex)) status = 1;
    }
    if (!names) { fputs("no paths specified\015", stderr); return 1; }
    return status;
}
