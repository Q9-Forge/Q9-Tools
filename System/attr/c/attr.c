/* Q9-compatible OS-9 file-attribute utility. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <modes.h>
#include <UNIX/stat.h>

extern int stat(const char *, struct stat *);
extern int chmod(const char *, int);

struct options {
    int quiet;
    int no_print;
    int execution_dir;
};

static void usage(void)
{
    fputs("Syntax:   attr [<opts>] {<path> [<opts>] <permissions>}\015", stdout);
    fputs("Function: display or change file attributes\015", stdout);
    fputs("     -z=<path> get list of file names from <path>\015", stdout);
    fputs("     -z        get list of file names from standard input\015", stdout);
    fputs("     -x        directory to search is execution directory\015", stdout);
    fputs("     -q        quiet mode, suppress can't open file errors\015", stdout);
    fputs("     -a        do not print attributes after changes\015", stdout);
    fputs("     -n        turn the following attributes off\015", stdout);
    fputs("     -         turn the following attributes on\015", stdout);
    fputs("Attributes: d s pe pw pr e w r\015", stdout);
}

static void print_attributes(const char *path, unsigned int mode)
{
    printf("%-24s %c%c%c%c%c%c%c%c\015", path,
           (mode & S_IFDIR) ? 'd' : '-',
           (mode & S_ISHARE) ? 's' : '-',
           (mode & S_IOEXEC) ? 'e' : '-',
           (mode & S_IOWRITE) ? 'w' : '-',
           (mode & S_IOREAD) ? 'r' : '-',
           (mode & S_IEXEC) ? 'e' : '-',
           (mode & S_IWRITE) ? 'w' : '-',
           (mode & S_IREAD) ? 'r' : '-');
}

static int apply_permissions(unsigned int *mode, const char *text)
{
    int enable;
    const char *place;

    enable = 1;
    place = text;
    while (*place != 0) {
        if (*place == '-') {
            enable = 1;
        } else if (*place == 'n') {
            enable = 0;
        } else if (*place == 'd') {
            if (enable) *mode |= S_IFDIR; else *mode &= ~S_IFDIR;
        } else if (*place == 's') {
            if (enable) *mode |= S_ISHARE; else *mode &= ~S_ISHARE;
        } else if (*place == 'p' && place[1] == 'e') {
            if (enable) *mode |= S_IOEXEC; else *mode &= ~S_IOEXEC;
            ++place;
        } else if (*place == 'p' && place[1] == 'w') {
            if (enable) *mode |= S_IOWRITE; else *mode &= ~S_IOWRITE;
            ++place;
        } else if (*place == 'p' && place[1] == 'r') {
            if (enable) *mode |= S_IOREAD; else *mode &= ~S_IOREAD;
            ++place;
        } else if (*place == 'e') {
            if (enable) *mode |= S_IEXEC; else *mode &= ~S_IEXEC;
        } else if (*place == 'w') {
            if (enable) *mode |= S_IWRITE; else *mode &= ~S_IWRITE;
        } else if (*place == 'r') {
            if (enable) *mode |= S_IREAD; else *mode &= ~S_IREAD;
        } else {
            return 1;
        }
        ++place;
    }
    return 0;
}

static int process_path(const char *path, const char *permissions,
                        const struct options *options)
{
    struct stat info;
    unsigned int mode;

    if (stat(path, &info) != 0) {
        if (!options->quiet)
            fprintf(stderr, "can't open file \"%s\"\015", path);
        return 1;
    }
    mode = info.st_mode;
    if (permissions != NULL) {
        if (apply_permissions(&mode, permissions) != 0) {
            fprintf(stderr, "unknown attribute or option '%s'\015",
                    permissions);
            return 1;
        }
        if (chmod(path, (int)mode) != 0) {
            fprintf(stderr, "error writing attributes to disk.\015");
            return 1;
        }
    }
    if (!options->no_print)
        print_attributes(path, mode);
    return 0;
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
        if (*line != 0 && process_path(line, NULL, options) != 0)
            status = 1;
    }
    return status;
}

int main(int argc, char **argv)
{
    struct options options;
    FILE *list;
    const char *path;
    const char *permissions;
    int status;

    options.quiet = 0;
    options.no_print = 0;
    options.execution_dir = 0;
    list = NULL;
    path = NULL;
    permissions = NULL;
    status = 0;

    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) {
            usage();
            return 0;
        } else if (strcmp(*argv, "-q") == 0) {
            options.quiet = 1;
        } else if (strcmp(*argv, "-a") == 0) {
            options.no_print = 1;
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
        } else if (**argv == '-' && path != NULL && permissions == NULL) {
            /* Attribute specifications themselves begin with '-'. */
            permissions = *argv;
        } else if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        } else if (path == NULL) {
            path = *argv;
        } else if (permissions == NULL) {
            permissions = *argv;
        } else {
            fprintf(stderr, "too many parameters\015");
            return 1;
        }
    }

    if (list != NULL)
        status = process_list(list, &options);
    else if (path != NULL)
        status = process_path(path, permissions, &options);
    else {
        fputs("you must specify -z, file, or directory\015", stderr);
        status = 1;
    }
    if (list != stdin && list != NULL)
        fclose(list);
    return status;
}
