/* Q9 service: resolve a service name and protocol to a port. */
#include <stdio.h>
#include <string.h>
#include <SPF/BSD/netdb.h>
static void usage(void){fputs("Syntax: service name protocol\015Function: resolve network service\015     -?        display this help\015",stdout);}
int main(int argc,char **argv){struct servent*s;if(argc>1&&!strcmp(argv[1],"-?")){usage();return 0;}if(argc!=3){usage();return 1;}s=getservbyname(argv[1],argv[2]);if(!s){fputs("service not found\015",stderr);return 1;}printf("%s/%s: port %d\015",s->s_name,s->s_proto,s->s_port);return 0;}
