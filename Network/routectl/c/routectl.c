#include <stdio.h>
#include <string.h>
#include <SPF/BSD/netdb.h>
#include <SPF/BSD/netinet/in.h>
static void usage(void){puts("Syntax: routectl add|delete destination gateway [netmask]\015Function: controlled route table update\015The update requires the literal --apply option.\015");}
static void addr(struct sockaddr*s,char*text){struct sockaddr_in*i=(struct sockaddr_in*)s;memset(s,0,sizeof *s);s->sa_len=sizeof(struct sockaddr_in);s->sa_family=AF_INET;i->sin_addr.s_addr=inet_addr(text);}
main(n,v) int n; char**v; {struct rtreq r;error_code e;if(n>1&&!strcmp(v[1],"-?")){usage();return 0;}if(n<5||n>6){usage();return 1;}if(n!=6||strcmp(v[5],"--apply")){fputs("refusing route change: add --apply\015",stderr);return 1;}memset(&r,0,sizeof r);addr(&r.dst,v[2]);addr(&r.gateway,v[3]);addr(&r.netmask,n==6&&strcmp(v[4],"-")?v[4]:"0.0.0.0");if(!strcmp(v[1],"add")){e=putroutent(&r);}else if(!strcmp(v[1],"delete")){e=delroutent(&r);}else{usage();return 1;}if(e){printf("route operation failed: %d\015",e);return 1;}puts("route operation completed");return 0;}
