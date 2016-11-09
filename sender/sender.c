/* CSD 304 Computer Networks, Fall 2016
   Lab 4, Sender
   Team: 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MC_PORT 5432
#define BUF_SIZE 4096

typedef struct site_info
{
	uint8_t type;
	uint8_t site_name_size;
	char *site_name;
	uint8_t site_desc_size;
	char *site_desc;
	uint8_t station_count;
	station_info *station_list;
}site_info_t;

typedef struct station_info
{
	uint8_t station_number;
	uint8_t station_name_size;
	char *station_name;
	uint32_t multicast_address;
	uint16_t data_port;
	uint16_t info_port;
	uint32_t bit_rate;
}station_info_t;

typedef struct station_not_found
{
	uint8_t type ;
	uint8_t station_number;
}station_not_found_t;

typedef struct song_info
{
	uint8_t type;
	uint8_t song_name_size;
	char song_name;
	uint16_t remaining_time_in_sec;
	uint8_t next_song_name_size;
	char next_song_name;
}song_info_t;
int main(int argc, char * argv[]){

  int s; /* socket descriptor */
  struct sockaddr_in sin; /* socket struct */
  char buf[BUF_SIZE];
  int len;
  
  /* Multicast specific */
  char *mcast_addr; /* multicast address */
  

  /* Add code to take port number from user */
  if (argc==2) {
    mcast_addr = argv[1];
  }
  else {
    fprintf(stderr, "usage: sender multicast_address\n");
    exit(1);
  }
   
  /* Create a socket */
  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("server: socket");
    exit(1);
  }
 
  /* build address data structure */
  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr(mcast_addr);
  sin.sin_port = htons(MC_PORT);

 
  /* Send multicast messages */
  /* Warning: This implementation sends strings ONLY */
  /* You need to change it for sending A/V files */
  memset(buf, 0, sizeof(buf));
  
  while (fgets (buf, BUF_SIZE, stdin)) {
    if ((len = sendto(s, buf, sizeof(buf), 0,
		      (struct sockaddr *)&sin, 
		      sizeof(sin))) == -1) {
      perror("sender: sendto");
      exit(1);
    }
    
    memset(buf, 0, sizeof(buf));
  
  }
  
  close(s);  
  return 0;
}

