#include <stdio.h>
#include <string.h>
#include <SPF/BSD/netdb.h>
#include <SPF/BSD/netinet/in.h>
static void show(struct sockaddr*s,char*out){struct sockaddr_in*i=(struct sockaddr_in*)s;if(s->sa_family==AF_INET)strcpy(out,inet_ntoa(i->sin_addr));else strcpy(out,"*");}
static void usage(void){puts("Syntax: route [-n] [-?]\015Function: display the IPv4 route table (read-only)\015     -n        keep numeric addresses (default)\015");}
main(n,v) int n; char**v; {struct rtreq*r;char d[32],g[32],m[32];while(--n>0){++v;if(!strcmp(*v,"-?")){usage();return 0;}if(!strcmp(*v,"-n"))continue;usage();return 1;}puts("Destination       Gateway           Netmask           Flags");r=getroutent();while(r){show(&r->dst,d);show(&r->gateway,g);show(&r->netmask,m);printf("%-17s %-17s %-17s %04x\015",d,g,m,r->flags);r=getroutent();}endroutent();return 0;}
