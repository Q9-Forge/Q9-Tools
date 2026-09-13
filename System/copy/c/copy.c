#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(void)
{
    fputs("Syntax: copy [<opts>] <srcpath> [<dstpath>|<dir>]\015", stdout);
    fputs("Function: copy data from one path to another\015", stdout);
    fputs("     -v          verify integrity of files written\015", stdout);
    fputs("     -p          don't print file names copied\015", stdout);
    fputs("     -a          abort on first error\015", stdout);
    fputs("     -r          rewrite destination\015", stdout);
    fputs("     -n          create a new destination file\015", stdout);
    fputs("     -b[=]<n>    buffer size in K bytes\015", stdout);
    fputs("     -z[=]path   get list of file names from path/stdin\015", stdout);
    fputs("     -a          (not implemented)\015", stdout);
    fputs("     -c          (not implemented)\015", stdout);
    fputs("     -f          (not implemented)\015", stdout);
    fputs("     -w=<dir>    (not implemented)\015", stdout);
    fputs("     -x          (not implemented)\015", stdout);
    fputs("     -?          help\015", stdout);
}

static int one(const char *src, const char *dst, int verify, int quiet, size_t size)
{
    FILE *in, *out;
    unsigned char *buffer, *check;
    size_t n, m;

    in = fopen(src, "rb");
    if (!in) { fprintf(stderr, "can't open '%s'\015", src); return 1; }
    out = fopen(dst, "wb");
    if (!out) { fclose(in); fprintf(stderr, "can't create '%s'\015", dst); return 1; }
    buffer = (unsigned char *)malloc(size);
    check = (unsigned char *)malloc(size);
    if (!buffer || !check) {
        if (buffer) free(buffer); if (check) free(check);
        fclose(in); fclose(out); fputs("can't get memory\015", stderr); return 1;
    }
    while ((n = fread(buffer, 1, size, in)) > 0) {
        if (fwrite(buffer, 1, n, out) != n) {
            free(buffer); free(check); fclose(in); fclose(out);
            fprintf(stderr, "can't write '%s'\015", dst); return 1;
        }
    }
    if (ferror(in)) {
        free(buffer); free(check); fclose(in); fclose(out);
        fprintf(stderr, "can't read '%s'\015", src); return 1;
    }
    fclose(in); fclose(out);
    if (verify) {
        in = fopen(src, "rb"); out = fopen(dst, "rb");
        if (!in || !out) { if (in) fclose(in); if (out) fclose(out); free(buffer); free(check); return 1; }
        while ((n = fread(buffer, 1, size, in)) > 0) {
            m = fread(check, 1, n, out);
            if (m != n || memcmp(buffer, check, n) != 0) {
                fclose(in); fclose(out); free(buffer); free(check);
                fprintf(stderr, "verification failed for '%s'\015", dst); return 1;
            }
        }
        if (fread(check, 1, 1, out) != 0) {
            fclose(in); fclose(out); free(buffer); free(check);
            fprintf(stderr, "verification failed for '%s'\015", dst); return 1;
        }
        fclose(in); fclose(out);
    }
    free(buffer); free(check);
    if (!quiet) printf("%s\015", dst);
    return 0;
}

static const char *base_name(const char *path)
{
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

static int list_copy(FILE *list, const char *directory, int verify, int quiet, int abort_on_error, size_t size)
{
    char line[256], target[256];
    int status = 0;
    while (fgets(line, sizeof line, list) != NULL) {
        char *end = line + strlen(line);
        const char *name;
        while (end > line && (end[-1] == '\015' || end[-1] == '\012')) --end;
        *end = 0;
        if (*line == 0) continue;
        name = base_name(line);
        if (directory[strlen(directory) - 1] == '/')
            sprintf(target, "%s%s", directory, name);
        else
            sprintf(target, "%s/%s", directory, name);
        if (one(line, target, verify, quiet, size) != 0) {
            status = 1;
            if (abort_on_error) break;
        }
    }
    return status;
}

int main(int argc, char **argv)
{
    const char *source = NULL, *destination = NULL;
    FILE *list = NULL;
    int verify = 0, quiet = 0, abort_on_error = 0, status;
    size_t size = 4096;

    while (--argc > 0) {
        ++argv;
        if (!strcmp(*argv, "-?")) { usage(); return 0; }
        if (!strcmp(*argv, "-v")) { verify = 1; continue; }
        if (!strcmp(*argv, "-p")) { quiet = 1; continue; }
        if (!strcmp(*argv, "-a")) { abort_on_error = 1; continue; }
        if (!strcmp(*argv, "-r") || !strcmp(*argv, "-n") || !strcmp(*argv, "-x")) continue;
        if (!strncmp(*argv, "-b", 2)) {
            const char *value = *argv + 2; long kb;
            if (*value == '=') ++value;
            kb = atol(value);
            if (kb < 1 || kb > 64) { fputs("missing or illegal size for -b option\015", stderr); return 1; }
            size = (size_t)kb * 1024; continue;
        }
        if (!strcmp(*argv, "-z")) { list = stdin; continue; }
        if (!strncmp(*argv, "-z=", 3)) {
            list = fopen(*argv + 3, "r");
            if (!list) { fprintf(stderr, "can't open '%s'\015", *argv + 3); return 1; }
            continue;
        }
        if (**argv == '-') { fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1; }
        if (!source && !list) source = *argv;
        else if (!destination) destination = *argv;
        else { fputs("too many paths\015", stderr); return 1; }
    }
    if (list) {
        if (!destination) { fputs("-z option requires a destination directory name\015", stderr); return 1; }
        status = list_copy(list, destination, verify, quiet, abort_on_error, size);
        if (list != stdin) fclose(list);
        return status;
    }
    if (!source || !destination) { fputs("must specify input and output filename\015", stderr); return 1; }
    return one(source, destination, verify, quiet, size);
}
