/* Q9 netstat: read-only route table display via Microware NetDB. */
#include <stdio.h>
#include <string.h>
#include <SPF/BSD/netdb.h>
#include <SPF/BSD/netinet/in.h>

static void usage(void)
{
    fputs("Syntax: netstat [<opts>]\015", stdout);
    fputs("Function: display the IPv4 routing table\015", stdout);
    fputs("     -r        display routes (default)\015", stdout);
    fputs("     -n        display numeric addresses (default)\015", stdout);
    fputs("     -?        display this help\015", stdout);
}

static void address(const struct sockaddr *sa, char *out)
{
    const struct sockaddr_in *in = (const struct sockaddr_in *)sa;
    if (sa->sa_family == AF_INET)
        strcpy(out, inet_ntoa(in->sin_addr));
    else
        strcpy(out, "*");
}

int main(int argc, char **argv)
{
    struct rtreq *route;
    char dst[32], gateway[32], mask[32];
    int show = 1;
    while (--argc > 0) {
        ++argv;
        if (!strcmp(*argv, "-?")) { usage(); return 0; }
        if (!strcmp(*argv, "-r")) { show = 1; continue; }
        if (!strcmp(*argv, "-n")) { continue; }
        fprintf(stderr, "unknown option '%s'\015", *argv);
        return 1;
    }
    if (!show) return 0;
    puts("Destination       Gateway           Netmask           Flags");
    route = getroutent();
    while (route) {
        address(&route->dst, dst);
        address(&route->gateway, gateway);
        address(&route->netmask, mask);
        printf("%-17s %-17s %-17s %04x\015",
               dst, gateway, mask, route->flags);
        route = getroutent();
    }
    endroutent();
    return 0;
}
