/* Q9-compatible OS-9 what-string search utility. */
#include <stdio.h>
#include <string.h>

static void usage(void)
{
    fputs("Syntax:   what [<opts>] <file names> [<opts>]\015", stdout);
    fputs("Function: search files for occurrences of @(#)\015", stdout);
    fputs("     -s   stop after the first occurrence\015", stdout);
}

static int search_file(const char *path, int first_only)
{
    FILE *input;
    int a, b, c, d;
    int found;

    input = fopen(path, "rb");
    if (input == NULL) { fprintf(stderr, "can't open '%s'\015", path); return 1; }
    found = 0;
    a = b = c = d = 0;
    while ((d = fgetc(input)) != EOF) {
        a = b; b = c; c = d;
        if (a != '@' || b != '(' || c != '#') continue;
        {
            char text[256];
            int length = 0;
            while ((d = fgetc(input)) != EOF && d != '"' && d != '>' && d != 0 && length < 255)
                text[length++] = (char)d;
            text[length] = 0;
            printf("%s: %s\015", path, text);
            found = 1;
            if (first_only) break;
        }
    }
    fclose(input);
    return found ? 0 : 0;
}

int main(int argc, char **argv)
{
    int first_only;
    int status;
    int names;

    first_only = 0;
    status = 0;
    names = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) { usage(); return 0; }
        else if (strcmp(*argv, "-s") == 0) first_only = 1;
        else if (**argv == '-') { fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1; }
        else { ++names; if (search_file(*argv, first_only) != 0) status = 1; }
    }
    if (names == 0) { fputs("you must specify file names\015", stderr); return 1; }
    return status;
}
