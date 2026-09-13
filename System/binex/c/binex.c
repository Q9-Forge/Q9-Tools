/* Q9 binex: binary file to Motorola S-record conversion. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(void)
{
    fputs("Syntax: binex [<opts>] [<inpath>] [<outpath>]\015", stdout);
    fputs("Function: convert file to S record format\015", stdout);
    fputs("     -a[=]hex     load address\015", stdout);
    fputs("     -s[=]n       S-record type 1, 2 or 3\015", stdout);
    fputs("     -r           rewrite destination\015", stdout);
    fputs("     -?           display this help\015", stdout);
}

static int hexnum(const char *s, unsigned long *v)
{
    char *e;
    *v = strtoul(s, &e, 16);
    return *s != 0 && *e == 0;
}

static int address_bytes(int type)
{
    return type == 1 ? 2 : (type == 2 ? 3 : 4);
}

static int record(FILE *out, int type, unsigned long address, const unsigned char *data, int n)
{
    int ab = address_bytes(type), count = ab + n + 1, i;
    unsigned long a = address;
    unsigned int sum = (unsigned int)count;
    fputc('S', out); fputc('0' + type, out);
    fprintf(out, "%02X", count);
    for (i = ab - 1; i >= 0; --i) { unsigned int b = (a >> (i * 8)) & 255; fprintf(out, "%02X", b); sum += b; }
    for (i = 0; i < n; ++i) { fprintf(out, "%02X", data[i]); sum += data[i]; }
    fprintf(out, "%02X\015", (~sum) & 255);
    return ferror(out) ? 1 : 0;
}

static int end_record(FILE *out, int type, unsigned long address)
{
    int endtype = type == 1 ? 9 : (type == 2 ? 8 : 7);
    return record(out, endtype, address, (const unsigned char *)0, 0);
}

int main(int argc, char **argv)
{
    const char *inname = 0, *outname = 0;
    FILE *in, *out;
    unsigned long address = 0, value;
    int type = 1, rewrite = 0, n = 0, status = 0;
    unsigned char buf[16];

    while (--argc > 0) {
        const char *arg = *++argv, *v = 0;
        if (!strcmp(arg, "-?")) { usage(); return 0; }
        if (!strcmp(arg, "-r")) { rewrite = 1; continue; }
        if (!strncmp(arg, "-a", 2) || !strncmp(arg, "-s", 2)) {
            v = arg[2] == '=' ? arg + 3 : arg + 2;
            if (!*v && argc > 0) v = *++argv, --argc;
            if (!v || !hexnum(v, &value)) { fputs("missing or invalid hex constant\015", stderr); return 1; }
            if (arg[1] == 'a') address = value;
            else { type = (int)value; if (type < 1 || type > 3) { fputs("invalid S-record type\015", stderr); return 1; } }
            continue;
        }
        if (*arg == '-') { fprintf(stderr, "unknown option '%c'\015", arg[1]); return 1; }
        if (!inname) inname = arg; else if (!outname) outname = arg; else { fputs("too many paths\015", stderr); return 1; }
    }
    if (!inname) { fputs("no input path\015", stderr); return 1; }
    in = fopen(inname, "rb");
    if (!in) { fprintf(stderr, "can't open '%s'\015", inname); return 1; }
    if (!outname) outname = "stdout";
    if (!strcmp(outname, "stdout")) out = stdout;
    else {
        if (!rewrite && (out = fopen(outname, "rb")) != 0) { fclose(out); fclose(in); fputs("destination exists\015", stderr); return 1; }
        out = fopen(outname, "wb");
        if (!out) { fclose(in); fprintf(stderr, "can't create '%s'\015", outname); return 1; }
    }
    while ((n = (int)fread(buf, 1, sizeof buf, in)) > 0) {
        if (record(out, type, address, buf, n)) { status = 1; break; }
        address += (unsigned long)n;
    }
    if (!status && end_record(out, type, address)) status = 1;
    if (out != stdout) fclose(out);
    fclose(in);
    return status;
}
