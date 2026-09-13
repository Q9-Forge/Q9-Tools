#include <stdio.h>
#include <string.h>
#include <RPC/rpc.h>
#define MESSAGEPROG 99
#define MESSAGEVERS 1
#define PRINTMESSAGE 1
static struct timeval timeout={25,0};
main(n,v) int n; char **v; { CLIENT*c; int*r; char*m;
 if(n>1&&!strcmp(v[1],"-?")){puts("Syntax: rmsg host message");return 0;} if(n!=3){puts("Syntax: rmsg host message");return 1;}
 c=clnt_create(v[1],MESSAGEPROG,MESSAGEVERS,"tcp");if(!c){clnt_pcreateerror(v[1]);return 1;}m=v[2];r=0;
 if(clnt_call(c,PRINTMESSAGE,xdr_wrapstring,&m,xdr_int,&r,timeout)!=RPC_SUCCESS){clnt_perror(c,v[1]);return 1;}puts("Message delivered");return 0; }
