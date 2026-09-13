#include <stdio.h>
#include <string.h>
#include <events.h>
static void usage(void){fputs("Syntax: events [<opts>]\015Function: display active system events\015     -h            display event values in hex\015     -k[=]name     kill event\015",stdout);}
int main(int argc,char **argv){const char *kill=NULL;int hex=0;while(--argc>0){++argv;if(!strcmp(*argv,"-?")){usage();return 0;}else if(!strcmp(*argv,"-h"))hex=1;else if(!strncmp(*argv,"-k=",3))kill=*argv+3;else if(!strcmp(*argv,"-k")&&argc>0){kill=*(++argv);--argc;}else{fprintf(stderr,"unknown option '%s'\015",*argv);return 1;}}if(kill){error_code e=_os_ev_delete(kill);if(e){fprintf(stderr,"can't delete event '%s' - Error #%03d\015",kill,e);return 1;}return 0;}fputs("Event table enumeration requires the OS-9 event-table query service.\015",stdout);if(hex)fputs("Values: hexadecimal mode requested.\015",stdout);return 0;}
