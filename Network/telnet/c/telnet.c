/* Q9 telnet: small line-oriented TCP terminal basis. */
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
static void usage(void){fputs("Syntax: telnet [<opts>] host [port]\015Function: line-oriented TCP terminal\015     -n        no local escape processing (default)\015     -o        option display not implemented\015     -d        socket debugging not implemented\015     -?        display this help\015",stdout);}
int main(int argc,char **argv){struct sockaddr_in a;struct hostent*h;char line[256],buf[512];char *host;int s,port=23,n;unsigned long ip;
 host=(char*)0;if(argc>1&&!strcmp(argv[1],"-?")){usage();return 0;}while(--argc>0){++argv;if(!strcmp(*argv,"-n")){continue;}if(!strcmp(*argv,"-o")||!strcmp(*argv,"-d")){fputs("telnet: option not implemented in Q9 line mode\015",stderr);return 1;}if(**argv=='-'){usage();return 1;}if(!host){host=*argv;continue;}if(port==23)port=atoi(*argv);else{usage();return 1;}}if(!host){usage();return 1;}memset(&a,0,sizeof a);a.sin_family=AF_INET;a.sin_port=htons((u_short)port);h=gethostbyname(host);if(h)memcpy(&a.sin_addr.s_addr,h->h_addr,4);else{ip=inet_addr(host);if(ip==(unsigned long)-1){fputs("unknown host\015",stderr);return 1;}a.sin_addr.s_addr=ip;}s=socket(AF_INET,SOCK_STREAM,0);if(s<0)return 1;if(connect(s,(struct sockaddr*)&a,sizeof a)<0){fputs("connect failed\015",stderr);_os_close(s);return 1;}while(fgets(line,sizeof line,stdin)){n=send(s,line,strlen(line),0);if(n<0)break;n=recv(s,buf,sizeof buf-1,0);if(n<=0)break;fwrite(buf,1,(size_t)n,stdout);} _os_close(s);return 0;}
