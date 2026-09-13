/* Q9 in-memory line sort, compatible with the useful qsort options. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int field_number;
static int separator;

static void usage(void)
{
    fputs("Syntax: qsort [<opts>] {<file> [<opts>]}\015", stdout);
    fputs("Function: sort strings in memory\015", stdout);
    fputs("     -f=<num>  sort on field <num>\015", stdout);
    fputs("     -c=<char> field separation character\015", stdout);
    fputs("     -z[=]path read file names from path/stdin\015", stdout);
    fputs("     -?        display this help\015", stdout);
}

static const char *key(const char *line, char *out, size_t size)
{
    const char *p = line;
    int field = 1;
    size_t n = 0;
    if (field_number <= 0) return line;
    while (*p && field < field_number) {
        if (*p == separator) ++field;
        ++p;
    }
    while (*p && *p != separator && *p != '\015' && *p != '\012' && n + 1 < size)
        out[n++] = *p++;
    out[n] = 0;
    return out;
}

static int compare_lines(const void *left, const void *right)
{
    char a[256], b[256];
    return strcmp(key(*(const char **)left, a, sizeof a),
                  key(*(const char **)right, b, sizeof b));
}

static int read_file(FILE *input, char **lines, int *count, int limit)
{
    char buffer[512];
    char *copy;
    size_t length;
    while (fgets(buffer, sizeof buffer, input) != NULL) {
        if (*count >= limit) return 1;
        length = strlen(buffer);
        copy = (char *)malloc(length + 1);
        if (!copy) return 1;
        strcpy(copy, buffer);
        lines[(*count)++] = copy;
    }
    return ferror(input) ? 1 : 0;
}

static int read_named_list(FILE *list, char **lines, int *count, int limit)
{
    char name[256];
    FILE *input;
    int status = 0;
    while (fgets(name, sizeof name, list) != NULL) {
        char *end = name + strlen(name);
        while (end > name && (end[-1] == '\015' || end[-1] == '\012')) --end;
        *end = 0;
        if (!*name) continue;
        input = fopen(name, "r");
        if (!input) { fprintf(stderr, "can't open '%s'\015", name); status = 1; continue; }
        if (read_file(input, lines, count, limit)) status = 1;
        fclose(input);
    }
    return status;
}

int main(int argc, char **argv)
{
    char *lines[2048];
    FILE *list = NULL, *input;
    const char *name;
    int count = 0, status = 0, i;
    field_number = 0;
    separator = ' ';
    while (--argc > 0) {
        ++argv;
        if (!strcmp(*argv, "-?")) { usage(); return 0; }
        if (!strncmp(*argv, "-f=", 3)) { field_number = atoi(*argv + 3); if (field_number < 1) return 1; continue; }
        if (!strncmp(*argv, "-c=", 3)) { separator = (unsigned char)(*argv)[3]; if (!separator) return 1; continue; }
        if (!strcmp(*argv, "-z")) { list = stdin; continue; }
        if (!strncmp(*argv, "-z=", 3)) { list = fopen(*argv + 3, "r"); if (!list) return 1; continue; }
        if (**argv == '-') { fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1; }
        name = *argv;
        input = fopen(name, "r");
        if (!input) { fprintf(stderr, "can't open '%s'\015", name); status = 1; continue; }
        if (read_file(input, lines, &count, 2048)) status = 1;
        fclose(input);
    }
    if (list) {
        if (read_named_list(list, lines, &count, 2048)) status = 1;
        if (list != stdin) fclose(list);
    }
    if (!list && count == 0 && status == 0) read_file(stdin, lines, &count, 2048);
    qsort(lines, (size_t)count, sizeof lines[0], compare_lines);
    for (i = 0; i < count; ++i) { fputs(lines[i], stdout); free(lines[i]); }
    return status;
}
