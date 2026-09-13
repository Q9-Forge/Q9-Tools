#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SPF/BSD/sys/types.h>
#include <SPF/BSD/netinet/in.h>
#include <SPF/BSD/netdb.h>
#include <SPF/RPC/rpc.h>
#include <SPF/RPC/pmap_clnt.h>
static void usage(void){puts("Syntax: rpcscan host [first-program last-program]\015Function: scan RPC portmapper program range\015");}
main(n,v) int n; char**v; {struct sockaddr_in a;struct hostent*h;unsigned long ip,p,first=100000,last=100020,port;char*name;if(n>1&&!strcmp(v[1],"-?")){usage();return 0;}if(n<2||n>4){usage();return 1;}name=v[1];if(n>2)first=atol(v[2]);if(n>3)last=atol(v[3]);memset(&a,0,sizeof a);a.sin_family=AF_INET;h=gethostbyname(name);if(h)memcpy(&a.sin_addr.s_addr,h->h_addr,4);else{ip=inet_addr(name);if(ip==(unsigned long)-1){fputs("unknown host\015",stderr);return 1;}a.sin_addr.s_addr=ip;}printf("RPC programs on %s\015",name);for(p=first;p<=last;p++){port=pmap_getport(&a,p,1,IPPROTO_TCP);if(port)printf("%lu/tcp port %lu\015",p,port);port=pmap_getport(&a,p,1,IPPROTO_UDP);if(port)printf("%lu/udp port %lu\015",p,port);}return 0;}
