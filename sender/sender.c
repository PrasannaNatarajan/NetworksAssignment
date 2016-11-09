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

#define MC_PORT 5431
#define BUF_SIZE 4096
#define TCP_PORT 5432
#define MAX_PENDING 5

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
  sin.sin_port = htons(TCP_PORT);

 
  /* Send multicast messages */
  /* Warning: This implementation sends strings ONLY */
  /* You need to change it for sending A/V files */
  memset(buf, 0, sizeof(buf));
  
/* setup passive open */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(1);
  }
 
  inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
  printf("Server is using address %s and port %d.\n", str, SERVER_PORT);

  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
    perror("simplex-talk: bind");
    exit(1);
  }
  else
    printf("Server bind done.\n");

  listen(s, MAX_PENDING);

  pid  = fork();
  if(pid == 0){
  	while(1) {
    if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
      perror("simplex-talk: accept");
      exit(1);
    }
    /*check station list and then send current station list*/
    //send(new_s,buff,strlen(buff)+1,0);
    
  	}
  }
  else{
  	while (fgets (buf, BUF_SIZE, stdin)) {
    if ((len = sendto(s, buf, sizeof(buf), 0,
		      (struct sockaddr *)&sin, 
		      sizeof(sin))) == -1) {
      perror("sender: sendto");
      exit(1);
    }
    
    memset(buf, 0, sizeof(buf));
  
  	}
  }
  
  
  close(s);  
  return 0;
}

