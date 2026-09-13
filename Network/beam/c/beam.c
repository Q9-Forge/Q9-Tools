/* Q9 beam: send numbered UDP test packets. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <modes.h>
#include <SPF/BSD/sys/types.h>
#include <SPF/BSD/sys/socket.h>
#include <SPF/BSD/netinet/in.h>
#include <SPF/BSD/netdb.h>

extern int getstat(int, int, void *);
extern int setstat(int, int, void *);
int _os_getstat(int p, unsigned long c, void *b) { return getstat(p, (int)c, b); }
int _os_setstat(int p, unsigned long c, void *b) { return setstat(p, (int)c, b); }

#define PACKET_SIZE 1000
#define DEFAULT_PORT 20000

struct beam_packet {
    u_int32 type;
    u_int32 size;
    u_int32 count;
    char data[PACKET_SIZE - 12];
};

static void usage(void)
{
    fputs("Syntax: beam [<opts>] host count [port]\015", stdout);
    fputs("Function: send UDP test packets\015", stdout);
    fputs("     -?        display this help\015", stdout);
}

int main(int argc, char **argv)
{
    const char *host_name;
    struct hostent *host;
    struct sockaddr_in addr;
    struct beam_packet packet;
    int s, count, port = DEFAULT_PORT, i;
    unsigned long numeric;

    if (argc > 1 && !strcmp(argv[1], "-?")) { usage(); return 0; }
    if (argc < 3 || argc > 4) { usage(); return 1; }
    host_name = argv[1];
    count = atoi(argv[2]);
    if (count <= 0) { fputs("count must be positive\015", stderr); return 1; }
    if (argc == 4) { port = atoi(argv[3]); if (port <= 0 || port > 65535) { fputs("illegal port\015", stderr); return 1; } }
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    host = gethostbyname(host_name);
    if (host) memcpy(&addr.sin_addr.s_addr, host->h_addr, 4);
    else {
        numeric = inet_addr(host_name);
        if (numeric == (unsigned long)-1) { fprintf(stderr, "unknown host %s\015", host_name); return 1; }
        addr.sin_addr.s_addr = numeric;
    }
    addr.sin_port = htons((u_short)port);
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) { fputs("socket call failed\015", stderr); return 1; }
    memset(&packet, 0, sizeof packet);
    packet.size = htonl(PACKET_SIZE);
    for (i = 0; i <= count; ++i) {
        packet.type = htonl(i == 0 ? 1 : (i >= count ? 3 : 2));
        packet.count = htonl((u_int32)i);
        if (sendto(s, (char *)&packet, PACKET_SIZE, 0,
                   (struct sockaddr *)&addr, sizeof addr) < 0) {
            fputs("send failed\015", stderr); _os_close(s); return 1;
        }
    }
    _os_close(s);
    printf("sent %d packets to %s:%d\015", count + 1, host_name, port);
    return 0;
}
