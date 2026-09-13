#include <stdio.h>
#include <string.h>
#include <sg_codes.h>
extern int _gs_devn(int, char *);
static void usage(void){fputs("Syntax:   pd [<opt>]\015Function: print current path(s)\015     -a        display all directory paths\015     -x        display execution directory path\015     -d        display current data directory\015",stdout);}
int main(int argc,char **argv){int all=0,exec=0,data=1,i;char name[128];while(--argc>0){++argv;if(!strcmp(*argv,"-?")){usage();return 0;}else if(!strcmp(*argv,"-a"))all=1;else if(!strcmp(*argv,"-x"))exec=1;else if(!strcmp(*argv,"-d"))data=1;else if(**argv=='-'){fprintf(stderr,"unknown option '%c'\015",(*argv)[1]);return 1;}else{fputs("unknown parameter\015",stderr);return 1;}}if(exec){if(_gs_devn(2,name)==0)printf("Execution: %s\015",name);else fputs("can't get execution path\015",stderr);return 0;}if(all||data){for(i=0;i<4;++i)if(_gs_devn(i,name)==0)printf("Path %d: %s\015",i,name);}return 0;}
