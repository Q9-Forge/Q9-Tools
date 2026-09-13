/* Q9-compatible OS-9 module unlink utility. */
#include <stdio.h>
#include <string.h>
#include <module.h>

extern error_code _os_link(char **, mh_com **, void **, u_int16 *, u_int16 *);
extern error_code _os_unlink(mh_com *);

static void usage(void)
{
    fputs("Syntax:   unlink [<opts>] {<modnames> [<opts>]}\015", stdout);
    fputs("Function: unlink modules from memory\015", stdout);
    fputs("     -f        force module to go away\015     -z[=]file  read module names from file/stdin\015", stdout);
}

static int unlink_one(const char *text, int force)
{
    char *name = (char *)text; mh_com *header; void *data; u_int16 type=0, attr=0; error_code error;
    error = _os_link(&name, &header, &data, &type, &attr);
    if (error != 0) { fprintf(stderr,"can't link '%s' - Error #%03d\015",text,error); return 1; }
    if (force) attr |= 0x8000;
    error = _os_unlink(header);
    if (error != 0) { fprintf(stderr,"can't unlink '%s' - Error #%03d\015",text,error); return 1; }
    return 0;
}

int main(int argc, char **argv)
{
    FILE *list=NULL; int force=0,status=0,names=0;
    while (--argc>0) { ++argv; if(strcmp(*argv,"-?")==0){usage();return 0;} else if(strcmp(*argv,"-f")==0)force=1; else if(strcmp(*argv,"-z")==0)list=stdin; else if(strncmp(*argv,"-z=",3)==0)list=fopen(*argv+3,"r"); else if(**argv=='-'){fprintf(stderr,"unknown option '%c'\015",(*argv)[1]);return 1;} else {++names;if(unlink_one(*argv,force)!=0)status=1;} }
    if(list!=NULL){char line[256];while(fgets(line,sizeof line,list)!=NULL){char *e=line+strlen(line);while(e>line&&(e[-1]=='\015'||e[-1]=='\012'))--e;*e=0;if(*line!=0){++names;if(unlink_one(line,force)!=0)status=1;}}if(list!=stdin)fclose(list);}
    if(names==0){fputs("you must specify -z or module names\015",stderr);return 1;} return status;
}
