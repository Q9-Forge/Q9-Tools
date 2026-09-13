/* Q9 msend: send UDP multicast messages. */
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
static void usage(void){fputs("Syntax: msend group message [port]\015Function: send multicast message\015     -?        display this help\015",stdout);}
int main(int argc,char **argv){struct sockaddr_in a;int s,port=20002;unsigned long ip;const char *msg;
 if(argc>1&&!strcmp(argv[1],"-?")){usage();return 0;}if(argc<3||argc>4){usage();return 1;}if(argc==4)port=atoi(argv[3]);ip=inet_addr(argv[1]);if(ip==(unsigned long)-1){fputs("invalid multicast address\015",stderr);return 1;}memset(&a,0,sizeof a);a.sin_family=AF_INET;a.sin_addr.s_addr=ip;a.sin_port=htons((u_short)port);msg=argv[2];s=socket(AF_INET,SOCK_DGRAM,0);if(s<0)return 1;if(sendto(s,(char*)msg,strlen(msg),0,(struct sockaddr*)&a,sizeof a)<0){fputs("multicast send failed\015",stderr);_os_close(s);return 1;}_os_close(s);return 0;}
