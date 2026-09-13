/* Q9-compatible OS-9 file comparison utility. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(void)
{
    fputs("Syntax:   cmp [<opts>] <path1> <path2> [<opts>]\015", stdout);
    fputs("Function: compare two files\015", stdout);
    fputs("     -t   print only byte totals compared and different\015", stdout);
    fputs("     -s   silent mode, stop at first mismatch\015", stdout);
    fputs("     -b[=]<size> buffer size in K bytes\015", stdout);
    fputs("     -x          (not implemented)\015", stdout);
}

int main(int argc, char **argv)
{
    const char *first;
    const char *second;
    FILE *one;
    FILE *two;
    unsigned long compared;
    unsigned long different;
    int totals;
    int silent;
    unsigned char *buffer_one;
    unsigned char *buffer_two;
    size_t buffer_size;
    size_t count_one;
    size_t count_two;
    size_t index;

    first = second = NULL;
    totals = 0;
    silent = 0;
    buffer_size = 4096;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) { usage(); return 0; }
        else if (strcmp(*argv, "-t") == 0) totals = 1;
        else if (strcmp(*argv, "-s") == 0) silent = 1;
        else if (!strncmp(*argv, "-b", 2)) {
            const char *value = *argv + 2;
            long kb;
            if (*value == '=') ++value;
            kb = atol(value);
            if (kb < 1 || kb > 64) { fputs("missing or illegal size for -b option\015", stderr); return 1; }
            buffer_size = (size_t)kb * 1024;
        }
        else if (**argv == '-') { fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1; }
        else if (first == NULL) first = *argv;
        else if (second == NULL) second = *argv;
        else { fputs("too many file names\015", stderr); return 1; }
    }
    if (first == NULL || second == NULL) { fputs("two files must be specified\015", stderr); return 1; }
    one = fopen(first, "rb");
    two = fopen(second, "rb");
    if (one == NULL || two == NULL) {
        if (one != NULL) fclose(one);
        if (two != NULL) fclose(two);
        fputs("can't open input file\015", stderr);
        return 1;
    }
    buffer_one = (unsigned char *)malloc(buffer_size);
    buffer_two = (unsigned char *)malloc(buffer_size);
    if (buffer_one == NULL || buffer_two == NULL) {
        if (buffer_one) free(buffer_one);
        if (buffer_two) free(buffer_two);
        fclose(one); fclose(two);
        fputs("can't get memory\015", stderr);
        return 1;
    }
    compared = different = 0;
    for (;;) {
        count_one = fread(buffer_one, 1, buffer_size, one);
        count_two = fread(buffer_two, 1, buffer_size, two);
        if (count_one == 0 && count_two == 0) break;
        for (index = 0; index < count_one || index < count_two; ++index) {
            ++compared;
            if (index >= count_one || index >= count_two || buffer_one[index] != buffer_two[index]) {
                ++different;
                if (silent) break;
            }
        }
        if (silent && different) break;
    }
    free(buffer_one);
    free(buffer_two);
    fclose(one);
    fclose(two);
    if (silent) {
        /* Historical -s is intentionally quiet. */
    } else if (totals) {
        printf("Bytes different: %08lx\015", different);
        printf("Bytes compared:  %08lx\015", compared);
    } else if (different == 0) {
        fputs("files are the same\015", stdout);
    } else {
        fputs("files are NOT the same\015", stdout);
        printf("Bytes different: %08lx\015", different);
        printf("Bytes compared:  %08lx\015", compared);
    }
    return different == 0 ? 0 : 1;
}
