/* Q9-compatible OS-9 character translation filter. */
#include <stdio.h>
#include <string.h>

static void usage(void)
{
    fputs("Syntax:   tr [<opts>] <str1> [<str2>] [<path1>] [<path2>] [<opts>]\015", stdout);
    fputs("Function: convert all occurrences of chars in <str1> to <str2>\015", stdout);
    fputs("     -d        delete all characters matching str1\015", stdout);
    fputs("     -c        invert sense of characters in str1\015", stdout);
    fputs("     -v        alias for -c\015", stdout);
    fputs("     -s        squeeze repeated output characters from str2\015", stdout);
    fputs("     -z[=]path read input file names from path/stdin\015", stdout);
}

static int contains(const char *set, int value)
{
    while (*set != 0) if ((unsigned char)*set++ == (unsigned char)value) return 1;
    return 0;
}

static void translate(FILE *input, FILE *output, const char *set1,
                      const char *set2, int delete_mode, int complement,
                      int squeeze)
{
    int value, previous = -1;
    while ((value = fgetc(input)) != EOF) {
        int match = contains(set1, value);
        if (complement) match = !match;
        if (delete_mode && match) continue;
        if (!delete_mode && match) {
            int position = 0;
            const char *place = set1;
            while (*place != 0 && (unsigned char)*place++ != (unsigned char)value) ++position;
            if (set2[position] != 0) value = (unsigned char)set2[position];
            else value = (unsigned char)set2[strlen(set2) - 1];
        }
        if (!(squeeze && !delete_mode && value == previous && contains(set2, value))) {
            fputc(value, output);
            previous = value;
        }
    }
}

int main(int argc, char **argv)
{
    const char *set1;
    const char *set2;
    FILE *input;
    FILE *output;
    FILE *list;
    int delete_mode;
    int complement;
    int index;
    int squeeze;

    set1 = set2 = NULL;
    input = stdin;
    output = stdout;
    list = NULL;
    delete_mode = complement = 0;
    index = 0;
    squeeze = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) { usage(); return 0; }
        else if (strcmp(*argv, "-d") == 0) delete_mode = 1;
        else if (strcmp(*argv, "-c") == 0) complement = 1;
        else if (strcmp(*argv, "-v") == 0) complement = 1;
        else if (strcmp(*argv, "-s") == 0) squeeze = 1;
        else if (strcmp(*argv, "-z") == 0) list = stdin;
        else if (strncmp(*argv, "-z=", 3) == 0) {
            list = fopen(*argv + 3, "r");
            if (list == NULL) { fprintf(stderr, "can't open '%s'\015", *argv + 3); return 1; }
        }
        else if (**argv == '-') { fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1; }
        else if (index++ == 0) set1 = *argv;
        else if (set2 == NULL && !delete_mode) set2 = *argv;
        else if (input == stdin) {
            input = fopen(*argv, "r");
            if (input == NULL) { fprintf(stderr, "can't open '%s'\015", *argv); return 1; }
        } else if (output == stdout) {
            output = fopen(*argv, "w");
            if (output == NULL) { fprintf(stderr, "can't open '%s'\015", *argv); return 1; }
        } else { fputs("too many parameters\015", stderr); return 1; }
    }
    if (set1 == NULL || (!delete_mode && set2 == NULL)) {
        fputs("two character sets must be specified\015", stderr);
        return 1;
    }
    if (list != NULL) {
        char line[256];
        if (input != stdin) { fclose(input); input = stdin; }
        while (fgets(line, sizeof line, list) != NULL) {
            char *end = line + strlen(line);
            FILE *file;
            while (end > line && (end[-1] == '\015' || end[-1] == '\012')) --end;
            *end = 0;
            if (*line == 0) continue;
            file = fopen(line, "r");
            if (file == NULL) { fprintf(stderr, "can't open '%s'\015", line); continue; }
            translate(file, output, set1, set2, delete_mode, complement, squeeze);
            fclose(file);
        }
    } else {
        translate(input, output, set1, set2, delete_mode, complement, squeeze);
    }
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);
    if (list != NULL && list != stdin) fclose(list);
    return 0;
}
