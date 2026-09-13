/* Q9 device-table display utility. */
#include <stdio.h>
#include <string.h>
#include <setsys.h>

extern int _getsys(int, int);

static void usage(void)
{
    fputs("Syntax:   devs [<opts>]\015", stdout);
    fputs("Function: print system device table\015", stdout);
    fputs("Options:\015     (none available)\015", stdout);
}

int main(int argc, char **argv)
{
    int count;
    int entry_size;
    long table_address;
    int index;

    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) {
            usage();
            return 0;
        }
        if (**argv == '-')
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
        else
            fprintf(stderr, "unknown parameter - '%s'\015", *argv);
        return 1;
    }
    count = _getsys(D_DevCnt, 0x80000002);
    entry_size = _getsys(D_DevSiz, 0x80000002);
    table_address = _getsys(D_DevTbl, 0x80000004);
    if (count < 0 || entry_size < 0 || table_address < 0) {
        fputs("can't get system device table information\015", stderr);
        return 1;
    }
    fputs("Device table is in kernel memory; showing safe table metadata.\015",
          stdout);
    printf("Entries: %d  Entry size: %d  Table: $%08lx\015", count,
           entry_size, (unsigned long)table_address);
    fputs(" Slot   Driver/descriptor names require a kernel-side copy\015",
          stdout);
    for (index = 0; index < count; ++index)
        printf(" %3d    <kernel device entry>\015", index);
    return 0;
}
