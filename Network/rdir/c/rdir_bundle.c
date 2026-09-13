#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <RPC/rpc.h>
#include <time.h>
#define MAXNAMELEN 255
typedef char *nametype;
typedef struct q9namenode *namelist;
struct q9namenode { nametype name; namelist next; };
struct q9readdir_res { int errno; namelist list; };
bool_t xdr_namelist(XDR*,namelist*);
#define DIRPROG 76
#define DIRVERS 1
#define READDIR 1
bool_t xdr_nametype(XDR*x,nametype*p){return xdr_string(x,p,MAXNAMELEN);}
bool_t xdr_q9namenode(XDR*x,struct q9namenode*p){return xdr_nametype(x,&p->name)&&xdr_namelist(x,&p->next);}
bool_t xdr_namelist(XDR*x,namelist*p){return xdr_pointer(x,(char**)p,sizeof(struct q9namenode),xdr_q9namenode);}
bool_t xdr_q9result(XDR*x,struct q9readdir_res*p){if(!xdr_int(x,&p->errno))return FALSE;if(p->errno==0)return xdr_namelist(x,&p->list);return TRUE;}
static struct timeval timeout={25,0};
struct q9readdir_res *q9readdir_1(nametype *arg,CLIENT*c){static struct q9readdir_res r;bzero(&r,sizeof r);if(clnt_call(c,READDIR,xdr_nametype,arg,xdr_q9result,&r,timeout)!=RPC_SUCCESS)return 0;return &r;}
main(n,v) int n; char **v; { CLIENT*c; struct q9readdir_res*r; char*server; char*dir;
 if(n>1&&!strcmp(v[1],"-?")){puts("Syntax: rdir host directory");return 0;} if(n!=3){puts("Syntax: rdir host directory");return 1;}
 server=v[1];dir=v[2];c=clnt_create(server,DIRPROG,DIRVERS,"tcp");if(!c){clnt_pcreateerror(server);return 1;}
 r=q9readdir_1(&dir,c);if(!r){clnt_perror(c,server);return 1;}if(r->errno){printf("remote error %d\015",r->errno);return 1;}printf("Directory of %s %s\015",server,dir);while(r->list){printf("%s\015",r->list->name);r->list=r->list->next;}return 0; }
