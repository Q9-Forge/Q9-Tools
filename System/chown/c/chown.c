#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <modes.h>
extern int chown(const char *,int);
static void usage(void){fputs("Syntax: chown [<opts>] <grp>.<usr> <path>\015Function: change ownership\015     -s        silent\015     -z[=]file  read paths\015",stdout);}
static int one(const char *p,int owner,int silent){if(chown(p,owner)!=0){if(!silent)fprintf(stderr,"can't change owner of '%s'\015",p);return 1;}if(!silent)printf("%s\015",p);return 0;}
int main(int argc,char **argv){const char *owner=NULL;FILE *list=NULL;int silent=0,status=0,names=0;while(--argc>0){++argv;if(!strcmp(*argv,"-?")){usage();return 0;}else if(!strcmp(*argv,"-s"))silent=1;else if(!strcmp(*argv,"-z"))list=stdin;else if(!strncmp(*argv,"-z=",3))list=fopen(*argv+3,"r");else if(**argv=='-'){fprintf(stderr,"unknown option '%c'\015",(*argv)[1]);return 1;}else if(!owner)owner=*argv;else{++names;if(one(*argv,(int)strtol(owner,NULL,0),silent))status=1;}}if(!owner||!names){fputs("you must specify group.user and files\015",stderr);return 1;}if(list){char line[256];while(fgets(line,sizeof line,list)){char *e=line+strlen(line);while(e>line&&(e[-1]=='\015'||e[-1]=='\012'))--e;*e=0;if(*line&&one(line,(int)strtol(owner,NULL,0),silent))status=1;}if(list!=stdin)fclose(list);}return status;}
