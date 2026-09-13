/* Q9-compatible OS-9 hostname utility. */
#include <stdio.h>
#include <string.h>
extern int gethostname(char *, int);
extern int sethostname(char *, int);

int main(int argc, char **argv)
{
    char name[128];
    if (argc > 2 || (argc == 2 && strcmp(argv[1], "-?") == 0)) {
        if (argc == 2 && strcmp(argv[1], "-?") == 0) {
            fputs("Syntax: hostname [name of host]\015Function: display or set (internet) name of host\015", stdout);
            return 0;
        }
        fputs("too many parameters\015", stderr); return 1;
    }
    if (argc == 2) {
        if (sethostname(argv[1], strlen(argv[1])) != 0) { fputs("can't set hostname\015", stderr); return 1; }
        return 0;
    }
    if (gethostname(name, sizeof name) != 0) { fputs("can't get hostname\015", stderr); return 1; }
    printf("%s\015", name);
    return 0;
}
