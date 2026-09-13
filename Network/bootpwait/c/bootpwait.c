#include <stdio.h>
#include <string.h>
#include <SPF/BSD/sys/types.h>
#include <SPF/BSD/sys/socket.h>
#include <SPF/BSD/netinet/in.h>
#include <SPF/BSD/netdb.h>
#include <SPF/BSD/protocols/bootp.h>
static void usage(void){puts("Syntax: bootpwait [-?]\015Function: wait for one BOOTP reply\015");}
main(n,v) int n; char**v; {int s;struct sockaddr_in a,from;struct bootp p;int flen;struct timeval tv;struct in_addr z;if(n>1&&!strcmp(v[1],"-?")){usage();return 0;}if(n>1){usage();return 1;}s=socket(AF_INET,SOCK_DGRAM,0);if(s<0){fputs("socket failed\015",stderr);return 1;}memset(&a,0,sizeof a);a.sin_family=AF_INET;a.sin_port=htons(IPPORT_BOOTPC);a.sin_addr.s_addr=INADDR_ANY;if(bind(s,(struct sockaddr*)&a,sizeof a)<0){fputs("bind failed\015",stderr);return 1;}tv.tv_sec=10;tv.tv_usec=0;if(setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof tv)<0)puts("warning: receive timeout unavailable");flen=sizeof from;if(recvfrom(s,(char*)&p,sizeof p,0,(struct sockaddr*)&from,&flen)<0){fputs("no BOOTP reply\015",stderr);return 1;}z=p.bp_yiaddr;printf("BOOTP reply from %s, assigned address %s\015",inet_ntoa(from.sin_addr),inet_ntoa(z));if(p.bp_file[0])printf("boot file: %s\015",p.bp_file);return 0;}
