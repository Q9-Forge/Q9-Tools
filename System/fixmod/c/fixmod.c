/* Q9 fixmod: conservative OS-9 module-header editor. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <module.h>

static void usage(void)
{
    fputs("Syntax: fixmod [<opts>] {<module>}\015", stdout);
    fputs("Function: fix or update module header information\015", stdout);
    fputs("     -u             update CRC and header parity\015", stdout);
    fputs("     -ur[=]rev      change module revision\015", stdout);
    fputs("     -ue[=]edition  change module edition\015", stdout);
    fputs("     -up[=]perm     change module permissions (hex)\015", stdout);
    fputs("     -us[=]size     change executable stack size\015", stdout);
    fputs("     -?             display this help\015", stdout);
}

static const char *value(const char *arg, const char *key, int *argc, char ***argv)
{
    if (arg[2] == '=') return arg + 3;
    if (arg[2] != 0) return arg + 2;
    if (*argc > 0) { --*argc; return *++*argv; }
    return (const char *)0;
}

static int number(const char *s, unsigned long *out)
{
    char *end;
    *out = strtoul(s, &end, 16);
    return s[0] != 0 && *end == 0;
}

static int edit_one(const char *name, int update, int have_rev, unsigned long rev,
                    int have_ed, unsigned long edition, int have_perm,
                    unsigned long perm, int have_stack, unsigned long stack)
{
    FILE *fp;
    mh_com *h;
    mh_exec *x;
    unsigned long size;
    void *image;
    error_code err;

    fp = fopen(name, "r+b");
    if (!fp) { fprintf(stderr, "can't open \"%s\"\015", name); return 1; }
    /* The Microware target is big-endian, so the module long is native here. */
    if (fseek(fp, 4L, 0) != 0 || fread((char *)&size, 1, 4, fp) != 4) { fclose(fp); return 1; }
    if (size < sizeof(mh_com) || size > 0x1000000UL) {
        fprintf(stderr, "incomplete or invalid module \"%s\"\015", name);
        fclose(fp); return 1;
    }
    image = malloc((size_t)size);
    if (!image) { fclose(fp); return 1; }
    rewind(fp);
    if (fread(image, 1, (size_t)size, fp) != (size_t)size) {
        free(image); fclose(fp); return 1;
    }
    h = (mh_com *)image;
    if (h->_msync != MODSYNC) {
        fprintf(stderr, "module sync wrong \"%s\"\015", name);
        free(image); fclose(fp); return 1;
    }
    x = (mh_exec *)image;
    if (have_rev) h->_mattrev = (h->_mattrev & 0xff00) | (rev & 0xff);
    if (have_ed) h->_medit = (short)edition;
    if (have_perm) h->_maccess = (short)perm;
    if (have_stack) x->_mstack = (long)stack;
    if (update || have_rev || have_ed || have_perm || have_stack) {
        err = _os_setcrc(h);
        if (err) { fprintf(stderr, "can't rewrite CRC for \"%s\"\015", name); free(image); fclose(fp); return 1; }
        rewind(fp);
        if (fwrite(image, 1, (size_t)size, fp) != (size_t)size) {
            free(image); fclose(fp); return 1;
        }
        fflush(fp);
        puts(name);
    }
    free(image);
    fclose(fp);
    return 0;
}

int main(int argc, char **argv)
{
    int update = 0, have_rev = 0, have_ed = 0, have_perm = 0, have_stack = 0;
    int status = 0, names = 0;
    unsigned long rev = 0, edition = 0, perm = 0, stack = 0;
    while (--argc > 0) {
        const char *arg = *++argv, *v;
        if (!strcmp(arg, "-?")) { usage(); return 0; }
        if (!strcmp(arg, "-u")) { update = 1; continue; }
        if (!strncmp(arg, "-ur", 3) || !strncmp(arg, "-ue", 3) ||
            !strncmp(arg, "-up", 3) || !strncmp(arg, "-us", 3)) {
            v = value(arg, arg, &argc, &argv);
            if (!v || !number(v, &rev)) { fprintf(stderr, "invalid option value\015"); return 1; }
            if (arg[1] == 'u' && arg[2] == 'r') { have_rev = 1; }
            else if (arg[2] == 'e') { edition = rev; have_ed = 1; }
            else if (arg[2] == 'p') { perm = rev; have_perm = 1; }
            else { stack = rev; have_stack = 1; }
            continue;
        }
        if (*arg == '-') { fprintf(stderr, "unknown option '%c'\015", arg[1]); return 1; }
        ++names;
        if (edit_one(arg, update, have_rev, rev, have_ed, edition, have_perm, perm, have_stack, stack)) status = 1;
        have_rev = have_ed = have_perm = have_stack = 0;
    }
    if (!names) { fputs("no module names\015", stderr); return 1; }
    return status;
}
