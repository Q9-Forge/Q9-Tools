/* Q9 wrapper for loading modules into OS-9 memory. */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <module.h>
#include <memory.h>

struct options {
    int sticky;
    int print_path;
    int data_dir;
    int ignore;
    u_int32 color;
    const char *data_path;
};

static void usage(void)
{
    fputs("Syntax: load [<opts>] {<module> [<opts>]}\015", stdout);
    fputs("Function: load a module into memory\015", stdout);
    fputs("     -s            sticky module load only\015", stdout);
    fputs("     -l            print pathlist of file loaded\015", stdout);
    fputs("     -d            load file from data directory\015", stdout);
    fputs("     -c=<color>    load into colored memory\015", stdout);
    fputs("     -z=<file>     get list of module names from <file>\015", stdout);
    fputs("     -i            ignore errors loading module and keep going\015", stdout);
    fputs("     -z            get list of module names from standard input\015", stdout);
    fputs("     -d=<path>     load file from data path directory\015", stdout);
}

static u_int32 color_value(const char *name)
{
    if (strcmp(name, "PATH") == 0)
        return MEM_ANY;
    if (strcmp(name, "SYSRAM") == 0)
        return SYSRAM;
    if (strcmp(name, "VIDEO1") == 0)
        return VIDEO1;
    if (strcmp(name, "VIDEO2") == 0)
        return VIDEO2;
    return (u_int32)-1;
}

static int load_one(const char *name, struct options *options)
{
    char path[256];
    mh_com *header;
    void *data;
    u_int16 type_lang;
    u_int16 attr_rev;
    error_code error;

    if (options->data_path != NULL)
        sprintf(path, "%s/%s", options->data_path, name);
    else
        strcpy(path, name);
    type_lang = 0;
    attr_rev = 0;
    if (options->sticky)
        attr_rev = (u_int16)(MA_GHOST << 8);
    error = _os_load(path, &header, &data, type_lang, &type_lang,
                     &attr_rev, options->color);
    if (error != 0) {
        fprintf(stderr, "can't load \"%s\" - Error #%03d\015", path, error);
        return 1;
    }
    if (options->print_path)
        printf("%s\015", path);
    else
        printf("loaded \"%s\"\015", path);
    return 0;
}

int main(int argc, char **argv)
{
    struct options options;
    FILE *list;
    char line[256];
    int status;
    int module_count;

    options.sticky = 0;
    options.print_path = 0;
    options.data_dir = 0;
    options.ignore = 0;
    options.color = MEM_ANY;
    options.data_path = NULL;
    list = NULL;
    status = 0;
    module_count = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) {
            usage();
            return 0;
        } else if (strcmp(*argv, "-s") == 0) {
            options.sticky = 1;
        } else if (strcmp(*argv, "-l") == 0) {
            options.print_path = 1;
        } else if (strcmp(*argv, "-d") == 0) {
            options.data_dir = 1;
            options.data_path = "/dd/CMDS/BOOTOBJS";
        } else if (strcmp(*argv, "-i") == 0) {
            options.ignore = 1;
        } else if (strcmp(*argv, "-z") == 0) {
            list = stdin;
        } else if (strncmp(*argv, "-z=", 3) == 0) {
            list = fopen(*argv + 3, "r");
            if (list == NULL) {
                fprintf(stderr, "can't open \"%s\".\015", *argv + 3);
                return 1;
            }
        } else if (strncmp(*argv, "-c=", 3) == 0) {
            options.color = color_value(*argv + 3);
            if (options.color == (u_int32)-1) {
                fprintf(stderr, "unknown color name \"%s\"\015", *argv + 3);
                return 1;
            }
        } else if (strncmp(*argv, "-d=", 3) == 0) {
            options.data_dir = 1;
            options.data_path = *argv + 3;
        } else if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        } else if (load_one(*argv, &options) != 0) {
            ++module_count;
            status = 1;
            if (!options.ignore)
                return status;
        } else {
            ++module_count;
        }
    }
    if (list != NULL) {
        while (fgets(line, sizeof line, list) != NULL) {
            char *end;
            end = line + strlen(line);
            while (end > line && (end[-1] == '\015' || end[-1] == '\012'))
                --end;
            *end = 0;
            if (*line != 0 && load_one(line, &options) != 0) {
                status = 1;
                if (!options.ignore)
                    break;
            }
        }
        if (list != stdin)
            fclose(list);
    }
    if (list == NULL && module_count == 0) {
        fputs("no input files\015", stderr);
        return 1;
    }
    return status;
}
