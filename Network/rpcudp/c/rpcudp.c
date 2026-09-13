#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SPF/RPC/rpc.h>
static void usage(void){fputs("Syntax: rpcudp host program version\015Function: call RPC NULLPROC over UDP\015     -?        display this help\015",stdout);}
static struct timeval timeout = { 10, 0 };
main(n,v)
int n; char **v;
{
 CLIENT *c; enum clnt_stat s;
 if (n > 1 && !strcmp(v[1], "-?")) { usage(); return 0; }
 if (n != 4) { usage(); return 1; }
 c = clnt_create(v[1], (u_long)atol(v[2]), (u_long)atol(v[3]), "udp");
 if (!c) { clnt_pcreateerror(v[1]); return 1; }
 s = clnt_call(c, NULLPROC, xdr_void, (caddr_t)0, xdr_void, (caddr_t)0, timeout);
 if (s != RPC_SUCCESS) { clnt_perror(c, "UDP NULLPROC"); return 1; }
 puts("UDP RPC NULLPROC succeeded"); return 0;
}
