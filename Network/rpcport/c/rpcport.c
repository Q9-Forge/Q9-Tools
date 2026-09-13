#include <stdio.h>
#include <string.h>
#include <SPF/BSD/sys/types.h>
#include <SPF/BSD/netinet/in.h>
#include <SPF/BSD/netdb.h>
#include <SPF/RPC/rpc.h>
#include <SPF/RPC/pmap_clnt.h>
static void usage(void){fputs("Syntax: rpcport host program version [tcp|udp]\015Function: query RPC portmapper\015     -?        display this help\015",stdout);}
int main(int n,char**v){struct sockaddr_in a;struct hostent*h;u_long p,ver,port,proto=IPPROTO_TCP;char*name;unsigned long ip;if(n>1&&!strcmp(v[1],"-?")){usage();return 0;}if(n<4||n>5){usage();return 1;}name=v[1];p=atol(v[2]);ver=atol(v[3]);if(n==5&&strcmp(v[4],"tcp"))proto=IPPROTO_UDP;memset(&a,0,sizeof a);a.sin_family=AF_INET;h=gethostbyname(name);if(h)memcpy(&a.sin_addr.s_addr,h->h_addr,4);else{ip=inet_addr(name);if(ip==(unsigned long)-1){fputs("unknown host\015",stderr);return 1;}a.sin_addr.s_addr=ip;}port=pmap_getport(&a,p,ver,proto);if(!port){fputs("RPC program is not registered\015",stderr);return 1;}printf("%s %lu/%lu %s port %lu\015",name,p,ver,proto==IPPROTO_TCP?"tcp":"udp",port);return 0;}
