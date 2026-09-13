/* Q9 tcpsend: send one file over a TCP connection. */
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
static void usage(void){fputs("Syntax: tcpsend host file [port]\015Function: send file over TCP\015     -?        display this help\015",stdout);}
int main(int argc,char **argv){FILE*f;struct hostent*h;struct sockaddr_in a;unsigned char b[1024];int s,n,port=20001;unsigned long ip;
 if(argc>1&&!strcmp(argv[1],"-?")){usage();return 0;} if(argc<3||argc>4){usage();return 1;} if(argc==4)port=atoi(argv[3]); f=fopen(argv[2],"rb");if(!f){fputs("can't open input\015",stderr);return 1;} memset(&a,0,sizeof a);a.sin_family=AF_INET;a.sin_port=htons((u_short)port);h=gethostbyname(argv[1]);if(h)memcpy(&a.sin_addr.s_addr,h->h_addr,4);else{ip=inet_addr(argv[1]);if(ip==(unsigned long)-1){fputs("unknown host\015",stderr);fclose(f);return 1;}a.sin_addr.s_addr=ip;} s=socket(AF_INET,SOCK_STREAM,0);if(s<0){fclose(f);return 1;}if(connect(s,(struct sockaddr*)&a,sizeof a)<0){fputs("connect failed\015",stderr);_os_close(s);fclose(f);return 1;}while((n=(int)fread(b,1,sizeof b,f))>0){int sent=0;while(sent<n){int k=send(s,(char*)b+sent,n-sent,0);if(k<=0){fputs("send failed\015",stderr);_os_close(s);fclose(f);return 1;}sent+=k;}}_os_close(s);fclose(f);return 0;}
