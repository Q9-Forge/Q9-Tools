/* Q9 break: enter the OS-9 system debugger. */
#include <stdio.h>
#include <process.h>

int main(int argc, char **argv)
{
    error_code err;
    if (argc != 1) {
        fputs("Syntax: break\015Function: invoke the system level debugger\015", stdout);
        return 1;
    }
    err = _os_sysdbg((void *)0, (void *)0);
    if (err) {
        fprintf(stderr, "system debugger unavailable - Error #%03d\015", err);
        return 1;
    }
    return 0;
}
