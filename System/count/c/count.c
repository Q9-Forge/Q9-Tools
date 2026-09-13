/* Q9-compatible OS-9 count utility. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct options { int lines; int words; int chars; int breakdown; long divisor; };

static void usage(void)
{
    fputs("Syntax:   count [<opt>] {<path> [<opts>]}\015", stdout);
    fputs("Function: count characters, lines, and words in a file\015", stdout);
    fputs("     -l        count lines (default)\015", stdout);
    fputs("     -w        count words\015", stdout);
    fputs("     -c        count characters\015", stdout);
    fputs("     -b        count characters with breakdown\015", stdout);
    fputs("     -d[=]<n>  count lines and divide by <n>\015", stdout);
    fputs("     -z=<path> get list of filenames from <path>\015", stdout);
    fputs("     -z        get list of filenames from standard input\015", stdout);
}

static int count_one(const char *path, const struct options *options)
{
    FILE *input;
    long lines, words, chars;
    int value, in_word;

    input = fopen(path, "r");
    if (input == NULL) { fprintf(stderr, "can't open '%s'\015", path); return 1; }
    lines = words = chars = 0;
    in_word = 0;
    while ((value = fgetc(input)) != EOF) {
        ++chars;
        if (value == '\015' || value == '\012') ++lines;
        if (value == ' ' || value == '\t' || value == '\015' || value == '\012') in_word = 0;
        else if (!in_word) { ++words; in_word = 1; }
    }
    fclose(input);
    if (options->breakdown)
        printf("\"%s\": %ld lines, %ld words, %ld characters\015", path, lines, words, chars);
    else if (options->words)
        printf("\"%s\" contains %ld words\015", path, words);
    else if (options->chars)
        printf("\"%s\" contains %ld characters\015", path, chars);
    else {
        long result = options->divisor > 1 ? (lines + options->divisor - 1) / options->divisor : lines;
        if (options->divisor > 1) printf("\"%s\" contains %ld groups of lines\015", path, result);
        else printf("\"%s\" contains %ld lines\015", path, result);
    }
    return 0;
}

static int count_list(FILE *input, const struct options *options)
{
    char line[256];
    int status;
    status = 0;
    while (fgets(line, sizeof line, input) != NULL) {
        char *end = line + strlen(line);
        while (end > line && (end[-1] == '\015' || end[-1] == '\012')) --end;
        *end = 0;
        if (*line != 0 && count_one(line, options) != 0) status = 1;
    }
    return status;
}

int main(int argc, char **argv)
{
    struct options options;
    FILE *list;
    int status, names;
    options.lines = 1; options.words = options.chars = options.breakdown = 0; options.divisor = 1;
    list = NULL; status = names = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) { usage(); return 0; }
        else if (strcmp(*argv, "-l") == 0) { options.lines = 1; options.words = options.chars = 0; }
        else if (strcmp(*argv, "-w") == 0) { options.words = 1; options.lines = options.chars = 0; }
        else if (strcmp(*argv, "-c") == 0) { options.chars = 1; options.lines = options.words = 0; }
        else if (strcmp(*argv, "-b") == 0) {
            options.lines = options.words = options.chars = 1; options.breakdown = 1;
        }
        else if (strcmp(*argv, "-z") == 0) list = stdin;
        else if (strncmp(*argv, "-z=", 3) == 0) list = fopen(*argv + 3, "r");
        else if (strncmp(*argv, "-d", 2) == 0) {
            const char *value = *argv + 2;
            if (*value == '=') ++value;
            options.divisor = atol(value);
            if (options.divisor < 1) { fputs("illegal divisor\015", stderr); return 1; }
        } else if (**argv == '-') { fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1; }
        else { ++names; if (count_one(*argv, &options) != 0) status = 1; }
    }
    if (list != NULL) status |= count_list(list, &options);
    if (list == NULL && names == 0) { fputs("you must specify -z or file names\015", stderr); return 1; }
    if (list != NULL && list != stdin) fclose(list);
    return status;
}
