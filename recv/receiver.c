/* CSD 304 Computer Networks, Fall 2016
   Lab 4, multicast receiver
   Team: 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/ioctl.h>

#define TCP_PORT 5432
#define BUF_SIZE 4096
#define MC_PORT 5433
#define DEBUG 1

/*function prototypes*/
void Serialize_station_info_req(station_info_request_t *buf,char *x);

typedef struct station_info_request
{
  uint8_t type;
}station_info_request_t;

typedef struct song_info
{
  uint8_t type;
  uint8_t song_name_size;
  char song_name;
  uint16_t remaining_time_in_sec;
  uint8_t next_song_name_size;
  char next_song_name;
}song_info_t;

/*Helper Functions*/
void Serialize_station_info_req(station_info_request_t *buf,char *x){
  int i;
  char buffer[1024]; 
  memcpy(buffer,&(buf->type),1);
  
  if(DEBUG == 1)
  {
    puts(">>Print memcpy buffer");
    for(i=0;i<1;i++){
      printf("%d",buffer[i]);
    }
  }
  memcpy(x,buffer,sizeof(buffer));
    if(DEBUG == 1)
      puts("End Serialize_fr");
}

int main(int argc, char * argv[]){
  
  int s,s_tcp; /* socket descriptor */
  struct sockaddr_in sin,sin_tcp; /* socket struct */
  char *if_name; /* name of interface */
  struct ifreq ifr; /* interface struct */
  char buf[BUF_SIZE];
  int len;
  /* Multicast specific */
  char *mcast_addr; /* multicast address */
  struct ip_mreq mcast_req;  /* multicast join struct */
  struct sockaddr_in mcast_saddr; /* multicast sender*/
  socklen_t mcast_saddr_len;

  char* tcp_addr; /*tcp ip address for control codes*/
  struct hostent *hp;

  /* Add code to take port number from user */
  if ((argc==2)||(argc == 3)) {
    tcp_addr = argv[1];
  }
  else {
    fprintf(stderr, "usage:(sudo) receiver tcp_address [interface_name (optional)]\n");
    exit(1);
  }

  if(argc == 3) {
    if_name = argv[2];
  }
  else
    if_name = "wlan0";



  /* translate host name into peer's IP address */
  hp = gethostbyname(tcp_addr);
  if (!hp) {
   fprintf(stderr, "simplex-talk: unknown host: %s\n", tcp_addr);
   exit(1);
  }
  else
   printf("Client's remote host: %s\n", argv[1]);
  /* build address data structure */
  bzero((char *)&sin_tcp, sizeof(sin_tcp));
  sin_tcp.sin_family = AF_INET;
  bcopy(hp->h_addr, (char *)&sin_tcp.sin_addr, hp->h_length);
  sin_tcp.sin_port = htons(TCP_PORT);
  /* active open */
  if ((s_tcp = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
   perror("simplex-talk: socket");
   exit(1);
  }
  else
   printf("Client created socket.\n");

  if (connect(s_tcp, (struct sockaddr *)&sin_tcp, sizeof(sin_tcp)) < 0)
    {
      perror("simplex-talk: connect");
      close(s);
      exit(1);
    }
  else
    printf("Client connected.\n");

  station_info_request_t *req;
  req = malloc(sizeof(struct station_info_request)*sizeof(char));
  req->type = 1;
  char buf[1024];
  Serialize_station_info_req(req,buf);
  send(s_tcp, buf, 1024, 0);
  char recv_buf[1024];
  recv(s_tcp,recv_buf,sizeof(recv_buf),0);
  
  /*
  1) Decode the site_info struct
  2) Display recv_buf->station_list[]
  */ 
  char station_list_from_server[]; // buffer to store station_list decoded
  

  

  /* create socket */
  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("receiver: socket");
    exit(1);
  }

  /* build address data structure */
  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(MC_PORT);
  
  
  /*Use the interface specified */ 
  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name , if_name, sizeof(if_name)-1);
  
  if ((setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, 
		  sizeof(ifr))) < 0)
    {
      perror("receiver: setsockopt() error");
      close(s);
      exit(1);
    }

  /* bind the socket */
  if ((bind(s, (struct sockaddr *) &sin, sizeof(sin))) < 0) {
    perror("receiver: bind()");
    exit(1);
  }
  
  /* Multicast specific code follows */
  
  /* build IGMP join message structure */
  mcast_req.imr_multiaddr.s_addr = inet_addr(mcast_addr);
  mcast_req.imr_interface.s_addr = htonl(INADDR_ANY);

  /* send multicast join message */
  if ((setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
		  (void*) &mcast_req, sizeof(mcast_req))) < 0) {
    perror("mcast join receive: setsockopt()");
    exit(1);
  }


  
  /* receive multicast messages */  
  printf("Ready to listen!\n");
   
  while(1) {
    
    /* reset sender struct */
    memset(&mcast_saddr, 0, sizeof(mcast_saddr));
    mcast_saddr_len = sizeof(mcast_saddr);
    
    /* clear buffer and receive */
    memset(buf, 0, sizeof(buf));
    if ((len = recvfrom(s, buf, BUF_SIZE, 0, (struct sockaddr*)&mcast_saddr, 
			&mcast_saddr_len)) < 0) {
      perror("receiver: recvfrom()");
      exit(1);
    }
    fputs(buf, stdout);
    printf("%s",mcast_saddr); fflush(stdout);
    /* Add code to send multicast leave request */
  
  }
  
  close(s);
  return 0;
}
