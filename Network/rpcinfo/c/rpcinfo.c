/* Q9 rpcinfo: OS-9 compatible RPC portmapper and NULLPROC diagnostics. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SPF/BSD/sys/types.h>
#include <SPF/BSD/netinet/in.h>
#include <SPF/BSD/netdb.h>
#include <SPF/RPC/rpc.h>
#include <SPF/RPC/pmap_clnt.h>
#include <SPF/RPC/pmap_prot.h>

static struct timeval timeout = { 10, 0 };

static void usage(void)
{
    fputs("Syntax: rpcinfo [<opts>]\015", stdout);
    fputs("Function: call RPC server and report results\015", stdout);
    fputs("     -p [host]                 list registered programs\015", stdout);
    fputs("     -t host prog [version]    test program over TCP\015", stdout);
    fputs("     -u host prog [version]    test program over UDP\015", stdout);
    fputs("     -n port                   reserved: fixed-port call not yet enabled\015", stdout);
    fputs("     -?                        display this help\015", stdout);
}

static int address(name, a)
char *name;
struct sockaddr_in *a;
{
    struct hostent *h;
    unsigned long ip;
    memset(a, 0, sizeof *a);
    a->sin_family = AF_INET;
    h = gethostbyname(name);
    if (h) {
        memcpy(&a->sin_addr.s_addr, h->h_addr, 4);
        return 0;
    }
    ip = inet_addr(name);
    if (ip == (unsigned long)-1)
        return 1;
    a->sin_addr.s_addr = ip;
    return 0;
}

static int list_maps(name)
char *name;
{
    struct sockaddr_in a;
    struct pmaplist *l;
    if (address(name, &a)) {
        fprintf(stderr, "rpcinfo: %s is unknown host\015", name);
        return 1;
    }
    l = pmap_getmaps(&a);
    if (!l) {
        fputs("rpcinfo: can't contact portmapper\015", stderr);
        return 1;
    }
    puts("program version protocol port");
    while (l) {
        printf("%lu %lu %s %u\015",
               (unsigned long)l->pml_map.pm_prog,
               (unsigned long)l->pml_map.pm_vers,
               l->pml_map.pm_prot == IPPROTO_TCP ? "tcp" : "udp",
               (unsigned)l->pml_map.pm_port);
        l = l->pml_next;
    }
    return 0;
}

static int test_call(name, prog, vers, proto)
char *name;
unsigned long prog;
unsigned long vers;
char *proto;
{
    CLIENT *c;
    enum clnt_stat s;
    c = clnt_create(name, (u_long)prog, (u_long)vers, proto);
    if (!c) {
        clnt_pcreateerror(name);
        return 1;
    }
    s = clnt_call(c, NULLPROC, xdr_void, (caddr_t)0,
                  xdr_void, (caddr_t)0, timeout);
    if (s != RPC_SUCCESS) {
        clnt_perror(c, name);
        return 1;
    }
    printf("program %lu version %lu ready and waiting\015", prog, vers);
    return 0;
}

main(argc, argv)
int argc;
char **argv;
{
    char *host;
    unsigned long prog, vers;
    int have_version;
    if (argc == 2 && !strcmp(argv[1], "-?")) {
        usage();
        return 0;
    }
    if (argc == 1)
        return list_maps("127.0.0.1");
    if (!strcmp(argv[1], "-p")) {
        host = argc > 2 ? argv[2] : "127.0.0.1";
        if (argc > 3) { usage(); return 1; }
        return list_maps(host);
    }
    if (!strcmp(argv[1], "-n")) {
        fputs("rpcinfo: -n fixed-port calls are not enabled yet\015", stderr);
        return 1;
    }
    if (!strcmp(argv[1], "-t") || !strcmp(argv[1], "-u")) {
        if (argc < 4 || argc > 5) { usage(); return 1; }
        host = argv[2];
        prog = atol(argv[3]);
        have_version = argc == 5;
        vers = have_version ? atol(argv[4]) : 1;
        return test_call(host, prog, vers,
                         !strcmp(argv[1], "-t") ? "tcp" : "udp");
    }
    if (argc == 2)
        return list_maps(argv[1]);
    usage();
    return 1;
}
