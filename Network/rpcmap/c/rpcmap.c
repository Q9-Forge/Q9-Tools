#include <stdio.h>
#include <string.h>
#include <SPF/BSD/sys/types.h>
#include <SPF/BSD/netinet/in.h>
#include <SPF/BSD/netdb.h>
#include <SPF/RPC/rpc.h>
#include <SPF/RPC/pmap_clnt.h>
#include <SPF/RPC/pmap_prot.h>
static void usage(void){fputs("Syntax: rpcmap [host]\015Function: list RPC portmapper map entries\015     -?        display this help\015",stdout);}
int main(int n,char**v){struct sockaddr_in a;struct hostent*h;struct pmaplist*l;char*name="127.0.0.1";unsigned long ip;if(n>1&&!strcmp(v[1],"-?")){usage();return 0;}if(n>2){usage();return 1;}if(n==2)name=v[1];memset(&a,0,sizeof a);a.sin_family=AF_INET;h=gethostbyname(name);if(h)memcpy(&a.sin_addr.s_addr,h->h_addr,4);else{ip=inet_addr(name);if(ip==(unsigned long)-1){fputs("unknown host\015",stderr);return 1;}a.sin_addr.s_addr=ip;}l=pmap_getmaps(&a);if(!l){fputs("no portmapper response\015",stderr);return 1;}while(l){printf("%lu %lu %s %lu\015",l->pml_map.pm_prog,l->pml_map.pm_vers,l->pml_map.pm_prot==IPPROTO_TCP?"tcp":"udp",l->pml_map.pm_port);l=l->pml_next;}return 0;}
