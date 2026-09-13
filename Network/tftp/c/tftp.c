/* Q9 tftp: basic RFC-1350 octet-mode get/put client. */
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
static void usage(void){fputs("Syntax: tftp host get remote local\015       tftp host put local remote\015Function: transfer file with TFTP octet mode\015     -?        display this help\015",stdout);}
static void word(unsigned char *p,int v){p[0]=(unsigned char)(v>>8);p[1]=(unsigned char)v;}
static int transfer(int s,struct sockaddr_in *server,int get,const char *first,const char *second){unsigned char pkt[516],data[516];struct sockaddr_in peer;FILE*f;int n,plen,block=1,fromlen;unsigned short op;
 f=fopen(get?second:first,get?"wb":"rb");if(!f){fputs("can't open file\015",stderr);return 1;}word(pkt,get?1:2);strcpy((char*)pkt+2,get?first:second);strcpy((char*)pkt+3+strlen(get?first:second),"octet");plen=4+strlen(get?first:second)+strlen("octet");if(sendto(s,(char*)pkt,plen,0,(struct sockaddr*)server,sizeof *server)<0){fclose(f);return 1;}peer=*server;for(;;){fromlen=sizeof peer;n=recvfrom(s,(char*)data,sizeof data,0,(struct sockaddr*)&peer,&fromlen);if(n<4){fclose(f);return 1;}op=((unsigned)data[0]<<8)|data[1];if(op==5){fputs("TFTP server error\015",stderr);fclose(f);return 1;}if(get){if(op!=3)continue; if((((unsigned)data[2]<<8)|data[3])!=block)continue; if(n>4&&fwrite(data+4,1,(size_t)(n-4),f)!=(size_t)(n-4)){fclose(f);return 1;}word(pkt,4);word(pkt+2,block);sendto(s,(char*)pkt,4,0,(struct sockaddr*)&peer,fromlen);if(n<516)break;block++;}else{if(op!=4||(((unsigned)data[2]<<8)|data[3])!=(unsigned)(block-1))continue;n=(int)fread(pkt+4,1,512,f);word(pkt,3);word(pkt+2,block);if(sendto(s,(char*)pkt,n+4,0,(struct sockaddr*)&peer,fromlen)<0){fclose(f);return 1;}if(n<512)break;block++;}}fclose(f);return 0;}
int main(int argc,char **argv){struct sockaddr_in a;struct hostent*h;int s,port=69;unsigned long ip;int get;if(argc>1&&!strcmp(argv[1],"-?")){usage();return 0;}if(argc!=5||((get=!strcmp(argv[2],"get"))==0&&!strcmp(argv[2],"put")==0)){usage();return 1;}memset(&a,0,sizeof a);a.sin_family=AF_INET;a.sin_port=htons((u_short)port);h=gethostbyname(argv[1]);if(h)memcpy(&a.sin_addr.s_addr,h->h_addr,4);else{ip=inet_addr(argv[1]);if(ip==(unsigned long)-1)return 1;a.sin_addr.s_addr=ip;}s=socket(AF_INET,SOCK_DGRAM,0);if(s<0)return 1;ip=transfer(s,&a,get,argv[3],argv[4]);_os_close(s);return (int)ip;}
