#include <stdio.h>
#include <string.h>
#include <SPF/BSD/netdb.h>
static void usage(void){puts("Syntax: hostconf [-?]\015Function: display NetDB host configuration\015");}
main(n,v) int n; char**v; {struct hostconfent*e;if(n>1&&!strcmp(v[1],"-?")){usage();return 0;}if(n>1){usage();return 1;}e=gethostconfent();while(e){printf("%s=%s\015",e->key?e->key:"",e->value?e->value:"",e=gethostconfent());}endhostconfent();return 0;}
