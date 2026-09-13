/* Q9 system-memory information utility. */
#include <stdio.h>
#include <string.h>
#include <types.h>

struct q9_gblkmp_info {
    u_int32 minimum_block;
    u_int32 fragments;
    u_int32 total_ram;
    u_int32 total_free;
};

extern error_code q9_gblkmp(void *, struct q9_gblkmp_info *);

static void usage(void)
{
    fputs("Syntax: mfree [<opt>]\015", stdout);
    fputs("Function: display system memory information\015", stdout);
    fputs("     -e       extended free memory description\015", stdout);
    fputs("     -s       system memory summary information\015", stdout);
}

int main(int argc, char **argv)
{
    int extended;
    int summary;
    u_int32 block_map[256];
    struct q9_gblkmp_info map_info;
    error_code error;
    unsigned int index;

    extended = 0;
    summary = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) {
            usage();
            return 0;
        } else if (strcmp(*argv, "-e") == 0) {
            extended = 1;
        } else if (strcmp(*argv, "-s") == 0) {
            summary = 1;
        } else if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        } else {
            fputs("no parameters recognized\015", stderr);
            return 1;
        }
    }

    error = q9_gblkmp(block_map, &map_info);
    if (error != 0) {
        fprintf(stderr, "can't get block map (error %u)\015",
                (unsigned int)error);
        return 1;
    }
    if (summary || !extended) {
        printf("Current total free RAM:    $%08lx\015",
               (unsigned long)map_info.total_free);
        printf("Total RAM at startup:      $%08lx\015",
               (unsigned long)map_info.total_ram);
        printf("Minimum allocation size:   $%08lx\015",
               (unsigned long)map_info.minimum_block);
    }
    if (extended) {
        fputs("Free memory map:\015", stdout);
        fputs("   -----------------   ------------------------------   -------   ----------\015",
              stdout);
        fputs("    Segment Address          Size of Segment             Color     Priority\015",
              stdout);
        for (index = 0; index < map_info.fragments && index < 128; ++index)
            if (block_map[index * 2 + 1] != 0)
                printf("    $%08lx        $%08lx                 system    --\015",
                       (unsigned long)block_map[index * 2],
                       (unsigned long)block_map[index * 2 + 1]);
    }
    return 0;
}
