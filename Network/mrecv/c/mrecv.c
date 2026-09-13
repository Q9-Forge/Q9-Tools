/* Q9 mrecv: receive UDP multicast messages. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <modes.h>
#include <SPF/BSD/sys/types.h>
#include <SPF/BSD/sys/socket.h>
#include <SPF/BSD/netinet/in.h>
#include <SPF/BSD/netdb.h>
extern int getstat(int,int,void*); extern int setstat(int,int,void*);
int _os_getstat(int p,unsigned long c,void*b){return getstat(p,(int)c,b);} int _os_setstat(int p,unsigned long c,void*b){return setstat(p,(int)c,b);}
static void usage(void){fputs("Syntax: mrecv group [port]\015Function: receive multicast messages\015     -?        display this help\015",stdout);}
int main(int argc,char **argv){struct sockaddr_in a;struct ip_mreq req;char b[1500];int s,port=20002,n;unsigned long ip;
 if(argc>1&&!strcmp(argv[1],"-?")){usage();return 0;}if(argc<2||argc>3){usage();return 1;}if(argc==3)port=atoi(argv[2]);ip=inet_addr(argv[1]);if(ip==(unsigned long)-1)return 1;memset(&a,0,sizeof a);a.sin_family=AF_INET;a.sin_port=htons((u_short)port);a.sin_addr.s_addr=INADDR_ANY;s=socket(AF_INET,SOCK_DGRAM,0);if(s<0)return 1;if(bind(s,(struct sockaddr*)&a,sizeof a)<0){fputs("bind failed\015",stderr);_os_close(s);return 1;}req.imr_multiaddr.s_addr=ip;req.imr_interface.s_addr=INADDR_ANY;if(setsockopt(s,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&req,sizeof req)<0){fputs("multicast join failed\015",stderr);_os_close(s);return 1;}n=recv(s,b,sizeof b-1,0);if(n>0){b[n]=0;fputs(b,stdout);fputc('\015',stdout);}_os_close(s);return n>0?0:1;}
