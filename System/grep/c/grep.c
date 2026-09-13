/* Q9-compatible OS-9 line search utility. */
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int silent_output;

static void usage(void)
{
    fputs("Syntax:   grep [<opts>] [<expression>] {[<path>] [<opts>]}\015", stdout);
    fputs("Function: search input files for matching lines\015", stdout);
    fputs("     -c        count matching lines\015     -i        ignore case\015", stdout);
    fputs("     -l        print only filenames with matches\015     -n        number lines\015", stdout);
    fputs("     -v        print non-matching lines\015     -s        silent mode\015", stdout);
    fputs("     -m=<num>  print <num> lines after each matching line\015", stdout);
    fputs("     -e=<expr>  add a search expression\015", stdout);
    fputs("     -f=<file>  read search expressions from file\015", stdout);
    fputs("     -z[=]file  read filenames from file/stdin\015", stdout);
}

static int equal_text(const char *a, const char *b, int insensitive)
{
    while (*b != 0) {
        if (*a == 0) return 0;
        if (insensitive) { if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0; }
        else if (*a != *b) return 0;
        ++a; ++b;
    }
    return 1;
}

static int line_matches(const char *line, const char **patterns, int pattern_count, int insensitive)
{
    int i;
    for (i = 0; i < pattern_count; ++i) {
        const char *place = line;
        while (*place != 0) {
            if (equal_text(place, patterns[i], insensitive)) return 1;
            ++place;
        }
    }
    return 0;
}

static int search_file(const char *path, const char **patterns, int pattern_count, int insensitive, int invert, int count_only, int names_only, int number, int more)
{
    FILE *input; char line[512]; long line_number, matches; int remaining;
    input = fopen(path, "r");
    if (input == NULL) { fprintf(stderr, "can't open '%s'\015", path); return 1; }
    line_number = matches = 0;
    remaining = 0;
    while (fgets(line, sizeof line, input) != NULL) {
        int match = 0;
        ++line_number;
        match = line_matches(line, patterns, pattern_count, insensitive);
        if (invert) match = !match;
        if (match) {
            ++matches;
            if (!silent_output && !count_only && !names_only) { if (number) printf("%ld:", line_number); fputs(line, stdout); }
            remaining = more;
        } else if (remaining > 0) {
            if (!silent_output && !count_only && !names_only) { if (number) printf("%ld:", line_number); fputs(line, stdout); }
            --remaining;
        }
    }
    fclose(input);
    if (!silent_output && count_only) printf("%ld\015", matches);
    if (!silent_output && names_only && matches != 0) printf("%s\015", path);
    return 0;
}

int main(int argc, char **argv)
{
    const char *patterns[64]; int pattern_count = 0; FILE *list = NULL; int insensitive=0, invert=0, count_only=0, names_only=0, number=0, more=0, names=0, status=0;
    silent_output = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv,"-?")==0) { usage(); return 0; }
        else if (strcmp(*argv,"-i")==0) insensitive=1; else if (strcmp(*argv,"-v")==0) invert=1;
        else if (strcmp(*argv,"-c")==0) count_only=1; else if (strcmp(*argv,"-l")==0) names_only=1;
        else if (strcmp(*argv,"-n")==0) number=1; else if (strcmp(*argv,"-s")==0) { silent_output=1; } else if (strncmp(*argv,"-m",2)==0) { const char *v=*argv+2; if (*v=='=') ++v; more=atoi(v); if (more<0) { fputs("illegal value for -m option\015",stderr); return 1; } } else if (strncmp(*argv,"-e=",3)==0) { if (pattern_count >= 64) return 1; patterns[pattern_count++] = *argv + 3; } else if (strncmp(*argv,"-f=",3)==0) { FILE *pf=fopen(*argv+3,"r"); char pbuf[256]; if (!pf) { fprintf(stderr,"can't open '%s'\015",*argv+3); return 1; } while (fgets(pbuf,sizeof pbuf,pf)!=NULL && pattern_count < 64) { char *end=pbuf+strlen(pbuf); char *copy; while (end>pbuf&&(end[-1]=='\015'||end[-1]=='\012')) --end; *end=0; copy=(char *)malloc(strlen(pbuf)+1); if (copy) { strcpy(copy,pbuf); patterns[pattern_count++]=copy; } } fclose(pf); } else if (strcmp(*argv,"-z")==0) list=stdin;
        else if (strncmp(*argv,"-z=",3)==0) list=fopen(*argv+3,"r");
        else if (**argv=='-') { fprintf(stderr,"unknown option '%c'\015",(*argv)[1]); return 1; }
        else if (pattern_count == 0) patterns[pattern_count++] = *argv;
        else { ++names; if (search_file(*argv,patterns,pattern_count,insensitive,invert,count_only,names_only,number,more)!=0) status=1; }
    }
    if (pattern_count == 0) { fputs("you must specify an expression\015",stderr); return 1; }
    if (list!=NULL) { char line[256]; while (fgets(line,sizeof line,list)!=NULL) { char *e=line+strlen(line); while(e>line&&(e[-1]=='\015'||e[-1]=='\012'))--e; *e=0; if(*line!=0) {++names; if(search_file(line,patterns,pattern_count,insensitive,invert,count_only,names_only,number,more)!=0)status=1;}} if(list!=stdin)fclose(list); }
    if (names==0) {
        FILE *input = stdin;
        char line[512];
        long line_number = 0; int remaining = 0;
        while (fgets(line, sizeof line, input) != NULL) {
            int match = 0;
            ++line_number;
            match = line_matches(line, patterns, pattern_count, insensitive);
            if (invert) match = !match;
            if (match) { if (count_only) ++names; else if (!silent_output) { if (number) printf("%ld:", line_number); fputs(line, stdout); } remaining = more; }
            else if (remaining > 0) { if (!count_only && !silent_output) { if (number) printf("%ld:", line_number); fputs(line, stdout); } --remaining; }
        }
    }
    return status;
}
