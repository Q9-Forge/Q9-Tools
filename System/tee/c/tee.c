/* Q9-compatible OS-9 tee utility. */
#include <stdio.h>
#include <string.h>

static void usage(void)
{
    fputs("Syntax:   tee {<path>}\015", stdout);
    fputs("Function: copy input to multiple output paths\015", stdout);
}

int main(int argc, char **argv)
{
    FILE *outputs[16];
    char buffer[512];
    int count;
    int index;
    int bytes;
    int status;

    count = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) { usage(); return 0; }
        if (**argv == '-') { fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1; }
        if (count >= 16) { fputs("too many output paths\015", stderr); return 1; }
        outputs[count] = fopen(*argv, "w");
        if (outputs[count] == NULL) { fprintf(stderr, "can't open '%s'\015", *argv); return 1; }
        ++count;
    }
    if (count == 0) { fputs("you must specify output paths\015", stderr); return 1; }
    status = 0;
    while ((bytes = fread(buffer, 1, sizeof buffer, stdin)) > 0) {
        if (fwrite(buffer, 1, bytes, stdout) != bytes) status = 1;
        for (index = 0; index < count; ++index)
            if (fwrite(buffer, 1, bytes, outputs[index]) != bytes) status = 1;
    }
    for (index = 0; index < count; ++index) fclose(outputs[index]);
    return status;
}
