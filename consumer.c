#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define MEGA 1000000
#define PORT 12012

static FILE *fp = NULL;

long long _fnInterceptor_entry_() {

  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec * MEGA + tv.tv_usec;
}


void _fnInterceptor_exit_(long long fid, long long t0) {
  struct timeval tv;
  gettimeofday(&tv,NULL);
  long long diff =  (tv.tv_sec * MEGA + tv.tv_usec) - t0;
  if (!fp) {
    fp = fopen("__timing.profile","+w");
  }
  fprintf(fp, "%llx %lld\n", fid, diff);
  return;
}


void _llvm_beacon_enter_(long long fid) {
  // Create a timestamp and send it along with the id
  struct timeval tv;
  gettimeofday(&tv,NULL);
  long long timeStamp = tv.tv_sec * MEGA + tv.tv_usec;
  int sockfd;
  char buffer[16] = "AFSH AG";
  memcpy(buffer, &fid, sizeof(fid));
  memcpy(buffer+sizeof(fid), &timeStamp, sizeof(timeStamp));
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in    servaddr;
  memset( &servaddr, 0, sizeof(servaddr) );
  servaddr.sin_family = AF_INET; 
  servaddr.sin_port = htons(PORT); 
  servaddr.sin_addr.s_addr = htonl( 0x7f000001 );
  sendto(sockfd, buffer, sizeof(buffer), MSG_DONTWAIT ,(const struct sockaddr*) &servaddr, sizeof(servaddr));
  return;
}
