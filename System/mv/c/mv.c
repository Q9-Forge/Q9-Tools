#include <stdio.h>
#include <string.h>

static void usage(void)
{
    fputs("Syntax: mv [<opts>] <old> <new>\015", stdout);
    fputs("Function: move files\015", stdout);
    fputs("     -x        execution directory (not implemented)\015", stdout);
    fputs("     -z[=]path read source list from path/stdin\015", stdout);
    fputs("     -?        display this help\015", stdout);
}

/*
 * Cross-device fallback.  rename() is preferred because it preserves the
 * filesystem's normal atomic move semantics.  If it fails, copy the file,
 * verify every copied byte, and only then remove the source.
 */
static int copy_then_remove(const char *source, const char *target)
{
    FILE *in, *out, *check_in, *check_out;
    unsigned char left[512], right[512];
    size_t n, m;
    int failed;

    in = fopen(source, "rb");
    if (!in) {
        fprintf(stderr, "can't read '%s'\015", source);
        return 1;
    }
    out = fopen(target, "wb");
    if (!out) {
        fclose(in);
        fprintf(stderr, "can't create '%s'\015", target);
        return 1;
    }
    failed = 0;
    while ((n = fread(left, 1, sizeof left, in)) > 0) {
        if (fwrite(left, 1, n, out) != n) {
            failed = 1;
            break;
        }
    }
    if (ferror(in)) failed = 1;
    if (fclose(in) != 0) failed = 1;
    if (fclose(out) != 0) failed = 1;
    if (failed) {
        fprintf(stderr, "can't copy '%s' to '%s'\015", source, target);
        return 1;
    }

    /* Re-open both files and compare the complete byte stream. */
    check_in = fopen(source, "rb");
    check_out = fopen(target, "rb");
    if (!check_in || !check_out) {
        if (check_in) fclose(check_in);
        if (check_out) fclose(check_out);
        fprintf(stderr, "can't verify '%s'\015", target);
        return 1;
    }
    failed = 0;
    do {
        n = fread(left, 1, sizeof left, check_in);
        m = fread(right, 1, n, check_out);
        if (m != n || (n && memcmp(left, right, n) != 0)) {
            failed = 1;
            break;
        }
    } while (n != 0);
    if (fread(right, 1, 1, check_out) != 0) failed = 1;
    fclose(check_in);
    fclose(check_out);
    if (failed) {
        fprintf(stderr, "verification failed for '%s'\015", target);
        return 1;
    }
    if (remove(source) != 0) {
        fprintf(stderr, "can't delete '%s' after copy\015", source);
        return 1;
    }
    return 0;
}

static const char *base_name(const char *path)
{
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

static int move_list(FILE *list, const char *directory)
{
    char line[256], target[256];
    int status = 0;
    while (fgets(line, sizeof line, list) != NULL) {
        char *end = line + strlen(line);
        const char *name;
        while (end > line && (end[-1] == '\015' || end[-1] == '\012')) --end;
        *end = 0;
        if (!*line) continue;
        name = base_name(line);
        if (directory[strlen(directory) - 1] == '/') sprintf(target, "%s%s", directory, name);
        else sprintf(target, "%s/%s", directory, name);
        if (rename(line, target) != 0 && copy_then_remove(line, target) != 0) status = 1;
    }
    return status;
}

int main(int argc, char **argv)
{
    const char *source = NULL;
    const char *target = NULL;
    FILE *list = NULL;

    while (--argc > 0) {
        ++argv;
        if (!strcmp(*argv, "-?")) {
            usage();
            return 0;
        }
        if (!strcmp(*argv, "-x")) continue;
        if (!strcmp(*argv, "-z")) { list = stdin; continue; }
        if (!strncmp(*argv, "-z=", 3)) { list = fopen(*argv + 3, "r"); if (!list) return 1; continue; }
        if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        }
        if (!source && !list) source = *argv;
        else if (!target) target = *argv;
        else {
            fputs("too many files\015", stderr);
            return 1;
        }
    }
    if (list) {
        if (!target) { fputs("-z option requires a destination directory name\015", stderr); return 1; }
        source = NULL;
        if (move_list(list, target) != 0) { if (list != stdin) fclose(list); return 1; }
        if (list != stdin) fclose(list);
        return 0;
    }
    if (!source || !target) {
        fputs("two filenames required\015", stderr);
        return 1;
    }
    if (!strcmp(source, target)) {
        fputs("source and target are identical\015", stderr);
        return 1;
    }
    if (rename(source, target) == 0) return 0;
    return copy_then_remove(source, target);
}
