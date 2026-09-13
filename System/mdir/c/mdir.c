/* Q9 module-directory display utility. */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <module.h>

struct options { int unformatted; int language; int extended; int type; };
static struct mod_dir directory_buffer[256];

static const char *type_word(unsigned int type)
{
    static const char *names[] = { "bas", "obj", "sys", "mult", "data", "cdata", "trap", "data", "data", "subr", "prog", "trap", "data", "fman", "driv", "desc" };
    return type < 16 ? names[type] : "????";
}
static const char *language_word(unsigned int language)
{
    static const char *names[] = { "bas", "obj", "sys", "cobl", "cobl", "cobl", "fort", "pasc" };
    return language < 8 ? names[language] : "????";
}
static void usage(void)
{
    fputs("Syntax: mdir [<opts>] [<mod names>] [<opts>]\015Function: display module directory\015", stdout);
    fputs("     -u        print unformatted listing\015     -a        print language instead of type\015     -e        print extended directory listing\015     -t=<type> list modules only of type <type>\015", stdout);
}
static void print_module(struct mod_dir *entry, struct options *options)
{
    if (entry->md_mptr == NULL) return;
    if (options->unformatted) { printf("$%08lx\015", (unsigned long)entry->md_mptr); return; }
    if (options->extended)
        printf("%08lx %8ld %08lx %04x %04x %5u\015", (unsigned long)entry->md_mptr, (long)entry->md_static, (unsigned long)entry->md_group, entry->md_mchk, entry->md_link, entry->md_link);
    else
        printf("%08lx %8ld %5u\015", (unsigned long)entry->md_mptr, (long)entry->md_static, entry->md_link);
}
int main(int argc, char **argv)
{
    struct options options;
    u_int32 directory_size;
    error_code error;
    time_t now;
    struct tm *current;
    int index;
    options.unformatted = options.language = options.extended = 0; options.type = -1;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) { usage(); return 0; }
        if (strcmp(*argv, "-u") == 0) options.unformatted = 1;
        else if (strcmp(*argv, "-a") == 0) options.language = 1;
        else if (strcmp(*argv, "-e") == 0) options.extended = 1;
        else if (strncmp(*argv, "-t=", 3) == 0) {
            options.type = -1;
            for (index = 0; index < 16; ++index) if (strcmp(*argv + 3, type_word(index)) == 0) options.type = index;
            if (options.type < 0) { fprintf(stderr, "unknown module type - \"%s\".\015", *argv + 3); return 1; }
        } else if (**argv == '-') { fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1; }
        else { fputs("module name filtering requires a kernel memory-copy service.\015", stderr); return 1; }
    }
    if (options.type >= 0) { fputs("module type filtering requires readable module headers.\015", stderr); return 1; }
    directory_size = sizeof directory_buffer;
    error = _os_get_moddir(directory_buffer, &directory_size);
    if (error != 0) { fprintf(stderr, "can't get module directory (error %u).\015", (unsigned int)error); return 1; }
    now = time((time_t *)0); current = localtime(&now);
    if (!options.unformatted && current != NULL) printf("Module Directory at %02d:%02d:%02d\015", current->tm_hour, current->tm_min, current->tm_sec);
    if (options.extended && !options.unformatted) fputs("  Addr     Static   Group      Check Link\015-------- -------- ---------- ---- -----\015", stdout);
    { unsigned int count = directory_size / sizeof directory_buffer[0]; unsigned int item; if (count > 256) count = 256; for (item = 0; item < count; ++item) print_module(&directory_buffer[item], &options); }
    return 0;
}
