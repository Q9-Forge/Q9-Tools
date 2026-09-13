#include <stdio.h>
#include <string.h>
#include <module.h>
#include <modes.h>

static void usage(void)
{
    fputs("Syntax: save [<opts>] {<modname>}\015", stdout);
    fputs("Function: save given modules to files\015", stdout);
    fputs("     -f=<path>  output file for one module\015", stdout);
    fputs("     -r         rewrite files\015", stdout);
    fputs("     -x         create output in execution directory\015", stdout);
    fputs("     -z[=]path  read module names from file/stdin\015", stdout);
}

static int save_exec(const char *path, const void *image, u_int32 size,
                     int rewrite)
{
    path_id out_path;
    u_int32 count;
    error_code e;

    e = _os_open(path, FAM_READ | FAM_EXEC, &out_path);
    if (e == 0) {
        _os_close(out_path);
        if (!rewrite) {
            fprintf(stderr, "file exists '%s'\015", path);
            return 1;
        }
        e = _os_delete(path, FAM_EXEC);
        if (e != 0) {
            fprintf(stderr, "can't replace '%s' - Error #%03d\015", path, e);
            return 1;
        }
    }
    e = _os_create(path, FAM_READ | FAM_WRITE | FAM_EXEC,
                   &out_path, FAP_READ | FAP_WRITE);
    if (e != 0) {
        fprintf(stderr, "can't create '%s' - Error #%03d\015", path, e);
        return 1;
    }
    count = size;
    e = _os_write(out_path, image, &count);
    _os_close(out_path);
    if (e != 0 || count != size) {
        fprintf(stderr, "can't write '%s' - Error #%03d\015", path, e);
        return 1;
    }
    return 0;
}

static int one(const char *name, const char *out, int rewrite, int execution)
{
    char *module_name;
    mh_com *header;
    void *data;
    u_int16 type_lang = 0, attr_rev = 0;
    error_code e;
    FILE *file;
    char path[128];

    module_name = (char *)name;
    e = _os_link(&module_name, &header, &data, &type_lang, &attr_rev);
    if (e) {
        fprintf(stderr, "can't link '%s' - Error #%03d\015", name, e);
        return 1;
    }
    if (out) strcpy(path, out);
    else strcpy(path, name);
    if (execution) {
        if (save_exec(path, (void *)header, (u_int32)header->_msize, rewrite))
            return 1;
    } else {
        if (!rewrite && (file = fopen(path, "rb")) != NULL) {
            fclose(file);
            fprintf(stderr, "file exists '%s'\015", path);
            return 1;
        }
        file = fopen(path, "wb");
        if (!file) {
            fprintf(stderr, "can't create '%s'\015", path);
            return 1;
        }
        if (fwrite((void *)header, 1, (size_t)header->_msize, file) !=
            (size_t)header->_msize) {
            fclose(file);
            fprintf(stderr, "can't write '%s'\015", path);
            return 1;
        }
        fclose(file);
    }
    printf("saved %s\015", path);
    return 0;
}

int main(int argc, char **argv)
{
    const char *out = NULL;
    const char *name;
    FILE *list = NULL;
    int rewrite = 0, execution = 0, status = 0, names = 0;

    while (--argc > 0) {
        ++argv;
        if (!strcmp(*argv, "-?")) { usage(); return 0; }
        if (!strcmp(*argv, "-r")) { rewrite = 1; continue; }
        if (!strcmp(*argv, "-x")) { execution = 1; continue; }
        if (!strncmp(*argv, "-f=", 3)) { out = *argv + 3; continue; }
        if (!strcmp(*argv, "-z")) { list = stdin; continue; }
        if (!strncmp(*argv, "-z=", 3)) {
            list = fopen(*argv + 3, "r");
            if (!list) { fprintf(stderr, "can't open '%s'\015", *argv + 3); return 1; }
            continue;
        }
        if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        }
        name = *argv;
        ++names;
        if (one(name, out, rewrite, execution)) status = 1;
        out = NULL;
    }
    if (list) {
        char line[128];
        while (fgets(line, sizeof line, list)) {
            char *end = line + strlen(line);
            while (end > line && (end[-1] == '\015' || end[-1] == '\012')) --end;
            *end = 0;
            if (*line) {
                ++names;
                if (one(line, NULL, rewrite, execution)) status = 1;
            }
        }
        if (list != stdin) fclose(list);
    }
    if (!names) {
        fputs("you must specify module names\015", stderr);
        return 1;
    }
    return status;
}
