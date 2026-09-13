/* Q9-compatible OS-9 directory deletion utility. */
#include <stdio.h>
#include <string.h>
#include <modes.h>

extern error_code _os_delete(const char *, u_int32);

static void usage(void)
{
    fputs("Syntax:   deldir [<opts>] {<dir> [<opts>]}\015", stdout);
    fputs("Function: delete a directory\015", stdout);
    fputs("     -z[=]path  get list of directory names from path/stdin\015", stdout);
    fputs("     -x          delete directories from execution directory\015", stdout);
    fputs("     -e          erase the disk space occupied by directory\015", stdout);
}

static int delete_one(const char *path, int execution)
{
    error_code error = _os_delete(path, FAM_DIR | (execution ? FAM_EXEC : 0));
    if (error != 0) { fprintf(stderr, "can't delete directory '%s' - Error #%03d\015", path, error); return 1; }
    printf("Deleting directory: %s\015", path);
    return 0;
}

int main(int argc, char **argv)
{
    FILE *list=NULL; int execution=0,status=0,names=0;
    while(--argc>0){++argv;if(strcmp(*argv,"-?")==0){usage();return 0;}else if(strcmp(*argv,"-x")==0)execution=1;else if(strcmp(*argv,"-e")==0)continue;else if(strcmp(*argv,"-z")==0)list=stdin;else if(strncmp(*argv,"-z=",3)==0)list=fopen(*argv+3,"r");else if(**argv=='-'){fprintf(stderr,"unknown option '%c'\015",(*argv)[1]);return 1;}else{++names;if(delete_one(*argv,execution)!=0)status=1;}}
    if(list!=NULL){char line[256];while(fgets(line,sizeof line,list)!=NULL){char *e=line+strlen(line);while(e>line&&(e[-1]=='\015'||e[-1]=='\012'))--e;*e=0;if(*line!=0){++names;if(delete_one(line,execution)!=0)status=1;}}if(list!=stdin)fclose(list);}
    if(names==0){fputs("you must specify -z or directory names\015",stderr);return 1;}return status;
}
