/* Q9 tcprecv: receive one file over TCP. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <modes.h>
#include <SPF/BSD/sys/types.h>
#include <SPF/BSD/sys/socket.h>
#include <SPF/BSD/netinet/in.h>
extern int getstat(int,int,void*); extern int setstat(int,int,void*);
int _os_getstat(int p,unsigned long c,void*b){return getstat(p,(int)c,b);} int _os_setstat(int p,unsigned long c,void*b){return setstat(p,(int)c,b);}
static void usage(void){fputs("Syntax: tcprecv file [port]\015Function: receive file over TCP\015     -?        display this help\015",stdout);}
int main(int argc,char **argv){FILE*f;struct sockaddr_in a;int ls,cs,n,port=20001;signed char b[1024];
 if(argc>1&&!strcmp(argv[1],"-?")){usage();return 0;}if(argc<2||argc>3){usage();return 1;}if(argc==3)port=atoi(argv[2]);f=fopen(argv[1],"wb");if(!f){fputs("can't create output\015",stderr);return 1;}memset(&a,0,sizeof a);a.sin_family=AF_INET;a.sin_port=htons((u_short)port);a.sin_addr.s_addr=INADDR_ANY;ls=socket(AF_INET,SOCK_STREAM,0);if(ls<0){fclose(f);return 1;}if(bind(ls,(struct sockaddr*)&a,sizeof a)<0||listen(ls,1)<0){fputs("listen failed\015",stderr);_os_close(ls);fclose(f);return 1;}cs=accept(ls,(struct sockaddr*)0,(int*)0);if(cs<0){fputs("accept failed\015",stderr);_os_close(ls);fclose(f);return 1;}while((n=recv(cs,(char*)b,sizeof b,0))>0)if(fwrite(b,1,(size_t)n,f)!=(size_t)n){fputs("write failed\015",stderr);_os_close(cs);_os_close(ls);fclose(f);return 1;}_os_close(cs);_os_close(ls);fclose(f);return n<0?1:0;}
