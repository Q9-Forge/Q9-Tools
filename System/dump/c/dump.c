#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <modes.h>
#include <module.h>

static void usage(void)
{
    fputs("Syntax: dump [<opts>] <path> [<starting offset>]\015", stdout);
    fputs("Function: formatted display of contents of a device\015", stdout);
    fputs("     -c        don't compress duplicate lines\015", stdout);
    fputs("     -s        interpret offset as sector number\015", stdout);
    fputs("     -m        dump a memory-resident module\015", stdout);
    fputs("     -x        look in execution directory\015", stdout);
    fputs("     -a        (not implemented)\015", stdout);
    fputs("     -k        (not implemented)\015", stdout);
    fputs("     -?        help\015", stdout);
}

static void print_line(unsigned long offset, const unsigned char *buffer, int count)
{
    int i;
    printf("%08lx  ", offset);
    for (i = 0; i < 16; ++i) {
        if (i < count) printf("%02x ", buffer[i]);
        else fputs("   ", stdout);
    }
    fputs(" ", stdout);
    for (i = 0; i < count; ++i)
        putchar(buffer[i] >= 32 && buffer[i] < 127 ? buffer[i] : '.');
    putchar('\015');
}

int main(int argc, char **argv)
{
    const char *path = NULL;
    long offset = 0;
    int sector_mode = 0, no_compress = 0, execution = 0, resident = 0;
    FILE *input;
    unsigned char current[16], previous[16];
    int count, previous_count = -1, repeated = 0;

    while (--argc > 0) {
        ++argv;
        if (!strcmp(*argv, "-?")) { usage(); return 0; }
        if (!strcmp(*argv, "-c")) { no_compress = 1; continue; }
        if (!strcmp(*argv, "-s")) { sector_mode = 1; continue; }
        if (!strcmp(*argv, "-m")) { resident = 1; continue; }
        if (!strcmp(*argv, "-x")) { execution = 1; continue; }
        if (!strcmp(*argv, "-a") || !strcmp(*argv, "-k")) {
            fprintf(stderr, "dump: option %s not implemented\015", *argv);
            return 1;
        }
        if (**argv == '-') { fprintf(stderr, "unknown option '%c'\015", (*argv)[1]); return 1; }
        if (!path) path = *argv;
        else offset = strtol(*argv, NULL, 0);
    }
    if (!path) { fputs("you must specify input path\015", stderr); return 1; }
    if (sector_mode) offset *= 256;
    if (resident) {
        char *module_name = (char *)path;
        mh_com *header;
        void *module_data;
        u_int16 type_lang, attr_rev;
        error_code error = _os_link(&module_name, &header, &module_data,
                                    &type_lang, &attr_rev);
        if (error != 0) { fprintf(stderr, "can't link '%s' - Error #%03d\015", path, error); return 1; }
        if (offset < 0 || offset >= header->_msize) return 1;
        {
            const unsigned char *bytes = (const unsigned char *)header + offset;
            long remaining = (long)header->_msize - offset;
            while (remaining > 0) {
                count = remaining > 16 ? 16 : (int)remaining;
                if (!no_compress && previous_count == count && memcmp(previous, bytes, (size_t)count) == 0) {
                    if (!repeated) { fputs("*\015", stdout); repeated = 1; }
                } else {
                    print_line((unsigned long)offset, bytes, count);
                    memcpy(previous, bytes, (size_t)count); previous_count = count; repeated = 0;
                }
                bytes += count; offset += count; remaining -= count;
            }
        }
        return 0;
    }
    if (execution) {
        path_id input_path;
        u_int32 amount;
        error_code error = _os_open(path, FAM_READ | FAM_EXEC, &input_path);
        if (error != 0) { fprintf(stderr, "can't open '%s' - Error #%03d\015", path, error); return 1; }
        if (offset != 0 && _os_seek(input_path, (u_int32)offset) != 0) { _os_close(input_path); return 1; }
        while ((amount = sizeof current, _os_read(input_path, current, &amount)) == 0 && amount > 0) {
            count = (int)amount;
            if (!no_compress && previous_count == count && memcmp(previous, current, (size_t)count) == 0) {
                if (!repeated) { fputs("*\015", stdout); repeated = 1; }
            } else {
                print_line((unsigned long)offset, current, count);
                memcpy(previous, current, (size_t)count); previous_count = count; repeated = 0;
            }
            offset += count;
        }
        _os_close(input_path);
        return 0;
    }
    input = fopen(path, "rb");
    if (!input) { fprintf(stderr, "can't open '%s'\015", path); return 1; }
    if (fseek(input, offset, 0) != 0) { fclose(input); return 1; }
    while ((count = (int)fread(current, 1, sizeof current, input)) > 0) {
        if (!no_compress && previous_count == count &&
            memcmp(previous, current, (size_t)count) == 0) {
            if (!repeated) { fputs("*\015", stdout); repeated = 1; }
        } else {
            print_line((unsigned long)offset, current, count);
            memcpy(previous, current, (size_t)count);
            previous_count = count;
            repeated = 0;
        }
        offset += count;
    }
    fclose(input);
    return 0;
}
