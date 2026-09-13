/* Q9 proto: resolve an Internet protocol name. */
#include <stdio.h>
#include <string.h>
#include <SPF/BSD/netdb.h>
static void usage(void){fputs("Syntax: proto name\015Function: resolve network protocol\015     -?        display this help\015",stdout);}
int main(int argc,char **argv){struct protoent*p;if(argc>1&&!strcmp(argv[1],"-?")){usage();return 0;}if(argc!=2){usage();return 1;}p=getprotobyname(argv[1]);if(!p){fputs("protocol not found\015",stderr);return 1;}printf("%s: number %d\015",p->p_name,p->p_proto);return 0;}
