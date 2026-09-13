#include <stdio.h>
#include <string.h>
#include <RPC/rpc.h>
#define SORTPROG 22855
#define SORTVERS 1
#define SORT 1
#define MAXSORTSIZE 64
#define MAXSTRINGLEN 64
typedef char *q9str;
struct q9sortstrings { struct { u_int len; q9str *val; } ss; };
bool_t xdr_q9str(XDR*x,q9str*p){return xdr_string(x,p,MAXSTRINGLEN);}
bool_t xdr_q9sortstrings(XDR*x,struct q9sortstrings*p){return xdr_array(x,(caddr_t*)&p->ss.val,&p->ss.len,MAXSORTSIZE,sizeof(q9str),xdr_q9str);}
static struct timeval timeout={25,0};
main(n,v) int n; char **v; { CLIENT*c; struct q9sortstrings a,r; int i;
 if(n>1&&!strcmp(v[1],"-?")){puts("Syntax: rsort host string ...");return 0;}if(n<3){puts("Syntax: rsort host string ...");return 1;}
 a.ss.len=n-2;a.ss.val=&v[2];r.ss.len=0;r.ss.val=0;c=clnt_create(v[1],SORTPROG,SORTVERS,"tcp");if(!c){clnt_pcreateerror(v[1]);return 1;}
 if(clnt_call(c,SORT,xdr_q9sortstrings,&a,xdr_q9sortstrings,&r,timeout)!=RPC_SUCCESS){clnt_perror(c,v[1]);return 1;}for(i=0;i<r.ss.len;i++)puts(r.ss.val[i]);return 0; }
