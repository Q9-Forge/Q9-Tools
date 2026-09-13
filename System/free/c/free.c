#include <stdio.h>
#include <string.h>
#include <modes.h>
#include <sg_codes.h>
static void usage(void){fputs("Syntax: free [<opts>] {<device>}\015Function: report free space on disk\015     -b[=]size  buffer size in K bytes (not implemented)\015     -s        (not implemented)\015",stdout);}
int main(int argc,char **argv){const char *dev=NULL;path_id p;u_int32 free_sectors=0;error_code e;while(--argc>0){++argv;if(!strcmp(*argv,"-?")){usage();return 0;}else if(!strncmp(*argv,"-b",2))continue;else if(**argv=='-'){fprintf(stderr,"unknown option '%c'\015",(*argv)[1]);return 1;}else if(!dev)dev=*argv;else{fputs("too many devices\015",stderr);return 1;}}if(!dev){fputs("you must specify device name\015",stderr);return 1;}e=_os_open(dev,FAM_READ,&p);if(e){fprintf(stderr,"can't open device '%s' - Error #%03d\015",dev,e);return 1;}e=_os9_gs_free(p,&free_sectors);_os_close(p);if(e){fprintf(stderr,"can't get free space - Error #%03d\015",e);return 1;}printf("Free sectors: %lu\015",(unsigned long)free_sectors);return 0;}
