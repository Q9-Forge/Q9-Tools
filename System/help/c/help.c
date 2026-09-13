#include <stdio.h>
#include <string.h>
static void usage(void){fputs("Syntax: help [<file>] {[<file>]}\015Function: Provide Help for OS-9/68000 Utilities\015",stdout);}
static int one(const char *path){FILE *f=fopen(path,"r");char b[512];int n;if(!f){fprintf(stderr,"can't open '%s'\015",path);return 1;}while((n=fread(b,1,sizeof b,f))>0)fwrite(b,1,n,stdout);fclose(f);return 0;}
int main(int argc,char **argv){int status=0,n=0;while(--argc>0){++argv;if(!strcmp(*argv,"-?")||!strcmp(*argv,"-h")){usage();return 0;}if(**argv=='-'){fprintf(stderr,"unknown option '%c'\015",(*argv)[1]);return 1;}++n;if(one(*argv))status=1;}if(!n){fputs("you must specify help files\015",stderr);return 1;}return status;}
