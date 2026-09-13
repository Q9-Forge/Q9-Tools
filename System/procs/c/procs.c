/* Q9 process-table display utility. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

struct options {
    int alternate;
    int every;
    int both;
    int pid_count;
    process_id pids[16];
};

static void usage(void)
{
    fputs("Syntax: procs [<opts>] [<pids>]\015", stdout);
    fputs("Function: display user processes\015", stdout);
    fputs("     -a    display alternate data\015", stdout);
    fputs("     -e    display every valid process\015", stdout);
    fputs("     -b    display both normal and alternate data\015", stdout);
}

static void print_process(Pr_desc process, struct options *options)
{
    if (process == NULL || process->_id == 0)
        return;
    printf("%3u %3u %3u %5u %5u %04x %3u %3u $%08lx\015",
           process->_id, process->_pid, process->_group, process->_user,
           process->_prior, process->_state, process->_pagcnt,
           process->_age, (unsigned long)process->_pmodul);
    if (options->alternate || options->both)
        printf("       Aging %5u  F$calls %8u  I$calls %8u  Read %8u Written %8u\015",
               process->_age, process->_fcalls, process->_icalls,
               process->_rbytes, process->_wbytes);
}

static int selected_pid(struct options *options, process_id pid)
{
    int index;

    if (options->pid_count == 0)
        return 1;
    for (index = 0; index < options->pid_count; ++index)
        if (options->pids[index] == pid)
            return 1;
    return 0;
}

int main(int argc, char **argv)
{
    struct options options;
    pr_desc process;
    unsigned char table_copy[1024];
    u_int32 table_size;
    error_code error;
    process_id pid;
    unsigned int index;
    unsigned int shown;

    options.alternate = 0;
    options.every = 0;
    options.both = 0;
    options.pid_count = 0;
    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) {
            usage();
            return 0;
        } else if (strcmp(*argv, "-a") == 0) {
            options.alternate = 1;
        } else if (strcmp(*argv, "-e") == 0) {
            options.every = 1;
        } else if (strcmp(*argv, "-b") == 0) {
            options.both = 1;
        } else if (**argv == '-') {
            fprintf(stderr, "unknown option '%c'\015", (*argv)[1]);
            return 1;
        } else {
            if (options.pid_count >= 16) {
                fputs("too many process ids\015", stderr);
                return 1;
            }
            index = (unsigned int)atoi(*argv);
            if (index == 0 || index > 255) {
                fprintf(stderr, "invalid process id \"%s\"\015", *argv);
                return 1;
            }
            options.pids[options.pid_count++] = (process_id)index;
        }
    }

    /* F$GPrDBT copies the process descriptor base table into user memory.
       Its entries are kernel pointers, so they are intentionally not
       followed here.  F$GPrDsc is then used to copy each descriptor. */
    table_size = sizeof table_copy;
    error = _os_get_prtbl(table_copy, &table_size);
    if (error != 0) {
        fprintf(stderr, "can't get process DBT (error %u).\015",
                (unsigned int)error);
        return 1;
    }
    fputs(" PID  PPID Grp.Usr Prior  MemSiz Sig S Module\015", stdout);
    fputs("---- ---- ------- ----- ------ --- - ------------\015", stdout);
    shown = 0;
    for (index = 1; index < 256; ++index) {
        pid = (process_id)index;
        table_size = sizeof process;
        error = _os_gprdsc(pid, &process, &table_size);
        if (error != 0 || table_size < 16 || process._id == 0 ||
            !selected_pid(&options, pid))
            continue;
        print_process(&process, &options);
        ++shown;
    }
    if (shown == 0)
        fputs("no readable process descriptors.\015", stderr);
    return 0;
}
