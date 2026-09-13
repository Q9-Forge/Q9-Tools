#include <stdio.h>
#include <string.h>
#include <modes.h>
extern void *attach(const char *, int);
static void usage(void){fputs("Syntax: iniz [<opts>] {<devname>}\015Function: attach devices\015     -z[=]file  read device names from file/stdin\015",stdout);}
static int one(const char *name){void *d=attach(name,0);if(!d){fprintf(stderr,"can't attach '%s'\015",name);return 1;}return 0;}
int main(int argc,char **argv){FILE *l=NULL;int n=0,s=0;while(--argc>0){++argv;if(!strcmp(*argv,"-?")){usage();return 0;}else if(!strcmp(*argv,"-z"))l=stdin;else if(!strncmp(*argv,"-z=",3))l=fopen(*argv+3,"r");else if(**argv=='-'){fprintf(stderr,"unknown option '%c'\015",(*argv)[1]);return 1;}else{++n;if(one(*argv))s=1;}}if(l){char b[128];while(fgets(b,sizeof b,l)){char *e=b+strlen(b);while(e>b&&(e[-1]=='\015'||e[-1]=='\012'))--e;*e=0;if(*b){++n;if(one(b))s=1;}}if(l!=stdin)fclose(l);}if(!n){fputs("you must specify -z or device names\015",stderr);return 1;}return s;}
