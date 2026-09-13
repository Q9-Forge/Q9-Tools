/* Q9-compatible OS-9 paged file display utility. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct options { int title; int formfeed; int page_lines; const char *heading; };

static void usage(void)
{
    fputs("Syntax:   pr [<opts>] {<path> [<opts>]}\015", stdout);
    fputs("Function: display file in specified format\015", stdout);
    fputs("     -t        do not print title\015     -f        do not use form feed\015", stdout);
    fputs("     -p=num    lines printed per page\015     -u=title   use specified title\015", stdout);
    fputs("     -z[=path] read file names from stdin or path\015", stdout);
}

static int print_file(const char *path, const struct options *options)
{
    FILE *input;
    char line[512];
    long line_number = 0;
    int page_line = 0;
    int page = 1;
    input = fopen(path, "r");
    if (input == NULL) { fprintf(stderr, "can't open '%s'\015", path); return 1; }
    if (!options->title) { printf("Page %d\015", page); if (options->heading) printf("%s\015", options->heading); }
    while (fgets(line, sizeof line, input) != NULL) {
        ++line_number;
        if (options->page_lines > 0 && page_line >= options->page_lines) {
            if (!options->formfeed) putchar('\f');
            ++page; page_line = 0;
            if (!options->title) { printf("Page %d\015", page); if (options->heading) printf("%s\015", options->heading); }
        }
        fputs(line, stdout);
        ++page_line;
    }
    fclose(input);
    return 0;
}

int main(int argc, char **argv)
{
    struct options options;
    FILE *list = NULL;
    int names = 0, status = 0;
    options.title = options.formfeed = 0; options.page_lines = 66; options.heading = NULL;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv,"-?")==0) { usage(); return 0; }
        else if (strcmp(*argv,"-t")==0) options.title=1;
        else if (strcmp(*argv,"-f")==0) options.formfeed=1;
        else if (strncmp(*argv,"-p=",3)==0) options.page_lines=atoi(*argv+3);
        else if (strncmp(*argv,"-u=",3)==0) options.heading=*argv+3;
        else if (strcmp(*argv,"-z")==0) list=stdin;
        else if (strncmp(*argv,"-z=",3)==0) list=fopen(*argv+3,"r");
        else if (**argv=='-') { fprintf(stderr,"unknown option '%c'\015",(*argv)[1]); return 1; }
        else { ++names; if(print_file(*argv,&options)!=0) status=1; }
    }
    if (list!=NULL) { char line[256]; while(fgets(line,sizeof line,list)!=NULL){char *e=line+strlen(line);while(e>line&&(e[-1]=='\015'||e[-1]=='\012'))--e;*e=0;if(*line!=0){++names;if(print_file(line,&options)!=0)status=1;}} if(list!=stdin)fclose(list); }
    if(names==0){fputs("you must specify -z or file names\015",stderr);return 1;}
    return status;
}
