/* Q9 ping: ICMP echo request/reply using the Microware socket library. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <modes.h>
#include <SPF/BSD/sys/types.h>
#include <SPF/BSD/sys/socket.h>
#include <SPF/BSD/netinet/in.h>
#include <SPF/BSD/netdb.h>

/* The standalone 68000 socket library expects these legacy C entry points.
   The base command link does not export the SPF aliases, so bridge them to
   the classic OS-9 getstat/setstat calls supplied by the system library. */
extern int getstat(int, int, void *);
extern int setstat(int, int, void *);
int _os_getstat(int path, unsigned long code, void *pb) { return getstat(path, (int)code, pb); }
int _os_setstat(int path, unsigned long code, void *pb) { return setstat(path, (int)code, pb); }

struct icmp_echo {
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    unsigned short ident;
    unsigned short sequence;
};

static void usage(void)
{
    fputs("Syntax: ping [<opts>] host\015", stdout);
    fputs("Function: send ICMP echo requests\015", stdout);
    fputs("     -c count       number of requests (default 1)\015", stdout);
    fputs("     -s size        data bytes (default 32)\015", stdout);
    fputs("     -?             display this help\015", stdout);
}

static unsigned short checksum(const unsigned char *p, int n)
{
    unsigned long sum = 0;
    while (n > 1) { sum += ((unsigned)p[0] << 8) | p[1]; p += 2; n -= 2; }
    if (n) sum += (unsigned)p[0] << 8;
    while (sum >> 16) sum = (sum & 0xffff) + (sum >> 16);
    return (unsigned short)~sum;
}

static int resolve_host(const char *name, struct sockaddr_in *addr)
{
    struct hostent *host;
    unsigned long numeric;
    memset(addr, 0, sizeof *addr);
    addr->sin_family = AF_INET;
    host = gethostbyname(name);
    if (host) { memcpy(&addr->sin_addr.s_addr, host->h_addr, 4); return 0; }
    numeric = inet_addr(name);
    if (numeric == (unsigned long)-1) return 1;
    addr->sin_addr.s_addr = numeric;
    return 0;
}

int main(int argc, char **argv)
{
    const char *name = 0;
    struct sockaddr_in addr, from;
    struct icmp_echo *packet;
    unsigned char *raw;
    int count = 1, size = 32, sent = 0, received = 0, s, n, fromlen;
    unsigned short seq;

    while (--argc > 0) {
        const char *arg = *++argv;
        if (!strcmp(arg, "-?")) { usage(); return 0; }
        if (!strcmp(arg, "-c") || !strcmp(arg, "-s")) {
            int *target = arg[1] == 'c' ? &count : &size;
            if (--argc <= 0) { fputs("missing option value\015", stderr); return 1; }
            *target = atoi(*++argv);
            if (*target <= 0 || (arg[1] == 's' && *target > 1400)) { fputs("illegal ping value\015", stderr); return 1; }
            continue;
        }
        if (*arg == '-') { fprintf(stderr, "unknown option %s\015", arg); return 1; }
        if (name) { fputs("too many hosts\015", stderr); return 1; }
        name = arg;
    }
    if (!name) { fputs("you must specify a host\015", stderr); return 1; }
    if (resolve_host(name, &addr)) { fprintf(stderr, "ping: unknown host %s\015", name); return 1; }
    s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (s < 0) { fputs("ping: socket call failed\015", stderr); return 1; }
    raw = (unsigned char *)malloc(sizeof(struct icmp_echo) + (size_t)size);
    if (!raw) { _os_close(s); return 1; }
    packet = (struct icmp_echo *)raw;
    for (seq = 0; seq < (unsigned short)count; ++seq) {
        memset(raw, 0, sizeof(struct icmp_echo) + (size_t)size);
        packet->type = 8;
        packet->ident = 1;
        packet->sequence = seq;
        packet->checksum = checksum(raw, sizeof(struct icmp_echo) + size);
        n = sendto(s, (char *)raw, sizeof(struct icmp_echo) + size, 0,
                   (struct sockaddr *)&addr, sizeof addr);
        ++sent;
        if (n < 0) { fputs("ping: send failed\015", stderr); continue; }
        fromlen = sizeof from;
        n = recvfrom(s, (char *)raw, sizeof(struct icmp_echo) + size + 64, 0,
                     (struct sockaddr *)&from, &fromlen);
        if (n >= (int)sizeof(struct icmp_echo)) {
            ++received;
            printf("reply from %s: %d bytes\015", inet_ntoa(from.sin_addr), n);
        } else fputs("no reply\015", stdout);
    }
    free(raw);
    _os_close(s);
    printf("--- %s ping statistics ---\015%d packets transmitted, %d received\015",
           name, sent, received);
    return received == sent ? 0 : 1;
}
