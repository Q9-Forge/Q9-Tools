#include <stdio.h>
#include <string.h>
#include <SPF/BSD/netdb.h>
static void usage(void){puts("Syntax: intent [-?]\015Function: display network interface intent data\015");}
main(n,v) int n; char**v; {char*s;if(n>1&&!strcmp(v[1],"-?")){usage();return 0;}if(n>1){usage();return 1;}s=getintent();if(!s){puts("no interface intent entries");return 1;}while(s){puts(s);s=getintent();}endintent();return 0;}
