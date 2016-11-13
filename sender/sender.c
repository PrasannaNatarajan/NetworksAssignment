/* CSD 304 Computer Networks, Fall 2016
   Lab 4, Sender
   Team: 
*/

#include < stdio.h > 
#include < stdlib.h > 
#include < string.h > 
#include < sys / types.h > 
#include < sys / socket.h > 
#include < netinet / in .h > 
#include < netdb.h >

#define MC_PORT 5431
#define BUF_SIZE 4096
#define TCP_PORT 5432
#define MAX_PENDING 5
#define MAX_NUM_OF_STATIONS 10
#define MAX_FILES_IN_A_STATION 1

typedef struct site_info {
  uint8_t type;
  uint8_t site_name_size;
  char * site_name;
  uint8_t site_desc_size;
  char * site_desc;
  uint8_t station_count;
  station_info * station_list;
}
site_info_t;

typedef struct station_info {
  uint8_t station_number;
  uint8_t station_name_size;
  char * station_name;
  uint32_t multicast_address;
  uint16_t data_port;
  uint16_t info_port;
  uint32_t bit_rate;
}
station_info_t;

typedef struct station_not_found {
  uint8_t type;
  uint8_t station_number;
}
station_not_found_t;

typedef struct song_info {
  uint8_t type;
  uint8_t song_name_size;
  char song_name;
  uint16_t remaining_time_in_sec;
  uint8_t next_song_name_size;
  char next_song_name;
}
song_info_t;

/*Global Variables*/
char* all_mcast_addresses[MAX_FILES_IN_A_STATION];

int main(int argc, char * argv[]) {

  int s, s_mcast ,new_s; /* socket descriptor */
  struct sockaddr_in sin; /* socket struct for TCP*/
  struct sockaddr_in sin_mcast; /*socket struct for multi cast*/
  char buf[BUF_SIZE];
  int len;
  pid_t pid;
  /* Multicast specific */
  char * mcast_addr; /* multicast address */
  char * team_multicast_address = "239.192.29.0";

  /* Add code to take port number from user */
  if (argc == 2) {
    mcast_addr = argv[1];
  } else {
    fprintf(stderr, "usage: sender multicast_address\n");
    exit(1);
  }

  /*Filling up the mcast_addresses*/
  int j;
  for (j = 0; j < MAX_NUM_OF_STATIONS; ++j)
  {
  	all_mcast_addresses[j] = malloc(sizeof(char)*15);
  	char ip[3];
  	sprintf(ip,"%d",j);
  	strcat(all_mcast_addresses[j],"239.192.29.");
  	strcat(all_mcast_addresses[j],ip);
  	puts(all_mcast_addresses[j]);
  }

  pid = fork();
  if (pid == 0) {

    /* Send multicast messages */
    /* Warning: This implementation sends strings ONLY */
    /* You need to change it for sending A/V files */
    memset(buf, 0, sizeof(buf));

    /* setup passive open */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      perror("simplex-talk: socket");
      exit(1);
    }
    /* build address data structure */
    memset((char * ) & sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(mcast_addr);
    sin.sin_port = htons(TCP_PORT);

    inet_ntop(AF_INET, & (sin.sin_addr), str, INET_ADDRSTRLEN);
    printf("Server is using address %s and port %d.\n", str, SERVER_PORT);

    if ((bind(s, (struct sockaddr * ) & sin, sizeof(sin))) < 0) {
      perror("simplex-talk: bind");
      exit(1);
    } else
      printf("Server bind done.\n");

    listen(s, MAX_PENDING);
    while (1) {
      if ((new_s = accept(s, (struct sockaddr * ) & sin, & len)) < 0) {
        perror("simplex-talk: accept");
        exit(1);
      }
      
      /*
      check station list and then send current station list and break
		1) find out what are the different stations
		2) make a pointer to site_info struct
		3) fill the struct
		4) serialize the struct
		5) send it to the reciever using syntax send(new_s,buff,strlen(buff)+1,0);
      */  

    }
  } else {
	    int station_number;
	    pid_t pid1;
	    for(station_number = 0;station_number<MAX_NUM_OF_STATIONS;station_number++){
	    	pid1 = fork();
	    	if(pid1==0){
	    		if ((s_mcast = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
	      			perror("server: socket");
	      			exit(1);
    			}

			    /* build address data structure */
			    memset((char * ) & sin, 0, sizeof(sin_mcast));
			    sin_mcast.sin_family = AF_INET;
			    sin_mcast.sin_addr.s_addr = inet_addr(all_mcast_addresses[station_number]); //mcast_addr
			    sin_mcast.sin_port = htons(MC_PORT);

	    	}
	    }
	    

	    char* filenames[MAX_NUM_OF_STATIONS]; // each string will contain all the names of that particular staion
	    int i;

	    for (i = 0; i < MAX_NUM_OF_STATIONS; ++i)
	    {
	    	filenames[i] = malloc(sizeof(char)*200);
	    	char command_ls[20] = "ls channel";
	    	char i_char[33];
	    	sprintf(i_char,"%d/",i);
	    	strcat(command_ls,i_char);
	    	//printf("%s",command_ls);
	    	strcat(command_ls," | grep .mp3");
	    	//printf("%s\n",command_ls);
	    	FILE * temp_fp = popen(command_ls,"r");
	    	char * temp_filename = malloc(sizeof(char)*20);
	    	while(fgets(temp_filename,20,temp_fp)!=NULL){
	    		strcat(filenames[i],temp_filename);
	    		puts(filenames[i]);
	    	}
	    }

	    //use this before sending: system("ffmpeg -i input.mp4 -f mpegts output.mp4"); send output.mp4

	    while (fgets(buf, BUF_SIZE, stdin)) {
	      if ((len = sendto(s_mcast, buf, sizeof(buf), 0,(struct sockaddr * ) & sin,sizeof(sin))) == -1) {
	        perror("sender: sendto");
	        exit(1);
	      }
	      memset(buf, 0, sizeof(buf));
	    }
  }

  close(s);
  return 0;
}