/* Q9 ifconfig: read-only interface inspection. */
#include <stdio.h>
#include <string.h>
#include <modes.h>
#include <SPF/BSD/sys/types.h>
#include <SPF/BSD/sys/socket.h>
#include <SPF/BSD/sys/ioctl.h>
#include <SPF/BSD/sys/sockio.h>
#include <SPF/BSD/net/if.h>
#include <SPF/BSD/netinet/in.h>
#include <SPF/BSD/netdb.h>
extern int getstat(int,int,void*); extern int setstat(int,int,void*);
int _os_getstat(int p,unsigned long c,void*b){return getstat(p,(int)c,b);} int _os_setstat(int p,unsigned long c,void*b){return setstat(p,(int)c,b);}
static void usage(void){fputs("Syntax: ifconfig [<interface>]\015Function: display interface information\015     -?        display this help\015Note: Q9 version is read-only.\015",stdout);}
int main(int argc,char **argv){int s,rc;const char *name="en0";struct ifreq r;struct sockaddr_in *a;
 if(argc>1&&!strcmp(argv[1],"-?")){usage();return 0;}if(argc>2){usage();return 1;}if(argc==2)name=argv[1];memset(&r,0,sizeof r);strncpy(r.ifr_name,name,IFNAMSIZ-1);s=socket(AF_INET,SOCK_DGRAM,0);if(s<0){fputs("socket call failed\015",stderr);return 1;}rc=ioctl(s,SIOCGIFADDR,(char*)&r);if(rc<0){fprintf(stderr,"can't read address for %s\015",name);_os_close(s);return 1;}a=(struct sockaddr_in*)&r.ifr_addr;printf("%s address %s\015",name,inet_ntoa(a->sin_addr));if(ioctl(s,SIOCGIFFLAGS,(char*)&r)>=0)printf("%s flags $%04x\015",name,(unsigned short)r.ifr_flags);if(ioctl(s,SIOCGIFMTU,(char*)&r)>=0)printf("%s mtu %lu\015",name,(unsigned long)r.ifr_mtu);_os_close(s);return 0;}
