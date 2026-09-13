/* Q9 host: resolve a hostname through the OS-9 NetDB library. */
#include <stdio.h>
#include <string.h>
#include <SPF/BSD/netdb.h>
static void usage(void){fputs("Syntax: host name\015Function: resolve hostname\015     -?        display this help\015",stdout);}
int main(int argc,char **argv){struct hostent*h;if(argc>1&&!strcmp(argv[1],"-?")){usage();return 0;}if(argc!=2){usage();return 1;}h=gethostbyname(argv[1]);if(!h){fprintf(stderr,"unknown host %s\015",argv[1]);return 1;}printf("%s: %s\015",argv[1],inet_ntoa(*(struct in_addr*)h->h_addr));return 0;}
