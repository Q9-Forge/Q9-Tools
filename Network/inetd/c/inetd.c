#include <stdio.h>
#include <string.h>
#include <SPF/BSD/netdb.h>
static void usage(void){puts("Syntax: inetd [-?]\015Function: display NetDB inetd service entries\015");}
main(n,v) int n; char**v; {struct inetdent*e;if(n>1&&!strcmp(v[1],"-?")){usage();return 0;}if(n>1){usage();return 1;}e=getinetdent();while(e){printf("%s %s type=%d wait=%d\015",e->service_name?e->service_name:"",e->protocol?e->protocol:"",e->socket_type,e->wait_status);e=getinetdent();}return 0;}
