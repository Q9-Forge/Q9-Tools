/* Q9-compatible OS-9 directory creation utility. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <modes.h>
#include <UNIX/stat.h>

extern int makdir(const char *, int, int, int);
extern int stat(const char *, struct stat *);

struct options {
    int quiet;
    int parents;
    int execution_dir;
};

static void usage(void)
{
    fputs("Syntax:   makdir [<opts>] {<dir name> [<opts>]}\015", stdout);
    fputs("Function: create a directory\015", stdout);
    fputs("     -z=<file>   get list of dir names from <file>\015", stdout);
    fputs("     -z          get list of dir names from standard input\015", stdout);
    fputs("     -x          create directory in execution directory\015", stdout);
    fputs("     -p          create all missing components of specified path\015", stdout);
    fputs("     -q          quiet mode\015", stdout);
}

static int make_one(const char *path, const struct options *options)
{
    struct stat info;

    if (stat(path, &info) == 0) {
        if ((info.st_mode & S_IFDIR) != 0)
            return 0;
        if (!options->quiet)
            fprintf(stderr, "file already exists \"%s\"\015", path);
        return 1;
    }
    if (makdir(path, S_IREAD | S_IWRITE | S_IEXEC |
               S_IOREAD | S_IOWRITE | S_IOEXEC, 0, 0) != 0) {
        if (!options->quiet)
            fprintf(stderr, "can't create directory \"%s\"\015", path);
        return 1;
    }
    return 0;
}

static int make_parents(const char *path, const struct options *options)
{
    char buffer[256];
    char *place;
    int status;

    if (strlen(path) >= sizeof buffer)
        return make_one(path, options);
    strcpy(buffer, path);
    status = 0;
    place = buffer;
    while ((place = strchr(place + 1, '/')) != NULL) {
        *place = 0;
        if (*buffer != 0 && make_one(buffer, options) != 0)
            status = 1;
        *place = '/';
    }
    if (make_one(buffer, options) != 0)
        status = 1;
    return status;
}

static int process_list(FILE *input, const struct options *options)
{
    char line[256];
    int status;

    status = 0;
    while (fgets(line, sizeof line, input) != NULL) {
        char *end;
        end = line + strlen(line);
        while (end > line && (end[-1] == '\015' || end[-1] == '\012'))
            --end;
        *end = 0;
        if (*line != 0 && (options->parents ? make_parents(line, options) :
                           make_one(line, options)) != 0)
            status = 1;
    }
    return status;
}

int main(int argc, char **argv)
{
    struct options options;
    FILE *list;
    int status;
    int names;

    options.quiet = 0;
    options.parents = 0;
    options.execution_dir = 0;
    list = NULL;
    status = 0;
    names = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) {
            usage();
            return 0;
        } else if (strcmp(*argv, "-q") == 0) {
            options.quiet = 1;
        } else if (strcmp(*argv, "-p") == 0) {
            options.parents = 1;
        } else if (strcmp(*argv, "-x") == 0) {
            options.execution_dir = 1;
        } else if (strcmp(*argv, "-z") == 0) {
            list = stdin;
        } else if (strncmp(*argv, "-z=", 3) == 0) {
            list = fopen(*argv + 3, "r");
            if (list == NULL) {
                fprintf(stderr, "can't open \"%s\".\015", *argv + 3);
                return 1;
            }
        } else if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        } else {
            ++names;
            if (options.parents) {
                if (make_parents(*argv, &options) != 0) status = 1;
            } else if (make_one(*argv, &options) != 0) {
                status = 1;
            }
        }
    }
    if (list != NULL)
        status |= process_list(list, &options);
    if (list == NULL && names == 0) {
        fputs("you must specify -z or directory names\015", stderr);
        status = 1;
    }
    if (list != NULL && list != stdin)
        fclose(list);
    return status;
}
