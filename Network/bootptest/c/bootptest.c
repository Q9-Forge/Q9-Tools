#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SPF/BSD/sys/types.h>
#include <SPF/BSD/sys/socket.h>
#include <SPF/BSD/netinet/in.h>
#include <SPF/BSD/netdb.h>
#include <SPF/BSD/protocols/bootp.h>
static void usage(void){puts("Syntax: bootptest [server]\015Function: send one BOOTP request packet\015     -?        display this help\015");}
main(n,v) int n; char**v; {int s,on=1;struct sockaddr_in a;struct hostent*h;struct bootp p;char*name="255.255.255.255";unsigned long ip;
 if(n>1&&!strcmp(v[1],"-?")){usage();return 0;}if(n>2){usage();return 1;}if(n==2)name=v[1];memset(&p,0,sizeof p);p.bp_op=BOOTREQUEST;p.bp_htype=HTYPE_ETHERNET;p.bp_hlen=6;p.bp_xid=0x51394231L;p.bp_flags=0x8000;memset(&a,0,sizeof a);a.sin_family=AF_INET;a.sin_port=htons(IPPORT_BOOTPS);h=gethostbyname(name);if(h)memcpy(&a.sin_addr.s_addr,h->h_addr,4);else{ip=inet_addr(name);if(ip==(unsigned long)-1){fputs("unknown server\015",stderr);return 1;}a.sin_addr.s_addr=ip;}s=socket(AF_INET,SOCK_DGRAM,0);if(s<0){fputs("socket failed\015",stderr);return 1;}setsockopt(s,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof on);if(sendto(s,(char*)&p,sizeof p,0,(struct sockaddr*)&a,sizeof a)<0){fputs("BOOTP send failed\015",stderr);return 1;}printf("BOOTP request sent to %s\015",name);return 0;}
