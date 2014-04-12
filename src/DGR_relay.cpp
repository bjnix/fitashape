// Authors:
// James Walker   jwwalker at mtu dot edu
// Scott A. Kuhl  kuhl at mtu dot edu

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <string>
#include <vector>

#define RELAY_LISTEN_PORT 25885
#define SLAVE_LISTEN_PORT 25884
#define BUFLEN 512


char *RELAY_OUT_IP = NULL;


// network data
int s_R, milliseconds;
struct timespec req;
pthread_t receiverThread;
int so_broadcast = 1;

struct sockaddr_in si_me_R, si_other_R;
socklen_t slen_R;
std::vector<int> s_S;
std::vector<struct sockaddr_in> si_other_S;
socklen_t slen_S;

bool receivedPacket = false;
int framesPassed = 0;

// state data -- ADD YOUR STATE PARAMETERS THAT NEED TO BE PASSED FROM MASTER TO SLAVE HERE.
float rotation;

// Exit with error message
void error(const char *msg) {
  perror(msg);
  exit(1);
}

void closeProgram() {
  exit(0);
}

// Register a callback that is called when the program exits so we can be
// sure to close the ports we're using.
void exitCallback() {
  close(s_R);
  for(int i = 0; i < s_S.size(); i++){
      close(s_S[i]);
    }
  
}

// This function receives incoming packets, repackages them, and then forwards them
// on the network for consumption by the slaves. It does this in an infinite loop.
void * receiver(void *) {
  char buf[BUFLEN];
  std::vector<std::string> splits;
  while (true) {
    // receive data
    if (recvfrom(s_R, buf, BUFLEN, 0, (struct sockaddr*)&si_other_R,
      &slen_R) == -1) error("ERROR recvfrom()");
    receivedPacket = true;
    framesPassed = 0;

    for(int i = 0; i < s_S.size(); i++){
      if (sendto(s_S[i], buf, BUFLEN, 0, (struct sockaddr*)&si_other_S[i],
        slen_S) == -1) error ("ERROR sendto()");
    }
  }
}

// MAIN FUNCTION
int main(int argc, char **argv) {
  if (argc < 2) {
    printf("USAGE: %s ip-to-send-to\n", argv[0]);
    return 1;
  }
  RELAY_OUT_IP=argv[1];
  if (argc > 2) {
    for(int i = 2; i < argc; i++){
            // master init
      struct sockaddr_in _si_other_S;
      int _s_S;
      slen_S=sizeof(_si_other_S);

      if ((_s_S=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) error("ERROR socket");
      //printf("%i--%s--%d--\n",i,argv[i],_s_S);

      setsockopt(_s_S, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));

      memset((char *) &_si_other_S, 0, sizeof(_si_other_S));
      _si_other_S.sin_family = AF_INET;
      _si_other_S.sin_port = htons(atoi(argv[i]));
      if (inet_aton(RELAY_OUT_IP, &_si_other_S.sin_addr) == 0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
      }
      s_S.push_back(_s_S);
      si_other_S.push_back(_si_other_S);
    }
  }
else{
      struct sockaddr_in _si_other_S;
      int _s_S;
      slen_S=sizeof(_si_other_S);

      if ((_s_S=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) error("ERROR socket");
      printf("--%d--\n",_s_S);

      setsockopt(_s_S, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));

      memset((char *) &_si_other_S, 0, sizeof(_si_other_S));
      _si_other_S.sin_family = AF_INET;
      _si_other_S.sin_port = htons(SLAVE_LISTEN_PORT);
      if (inet_aton(RELAY_OUT_IP, &_si_other_S.sin_addr) == 0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
      }
      s_S.push_back(_s_S);
      si_other_S.push_back(_si_other_S);
}
  
  milliseconds = 16;
  req.tv_sec = 0;
  req.tv_nsec = milliseconds * 1000000L;

  // socket stuff
  slen_R=sizeof(si_other_R);
  if ((s_R=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) error("ERROR socket");
  memset((char *) &si_me_R, 0, sizeof(si_me_R));
  si_me_R.sin_family = AF_INET;
  si_me_R.sin_port = htons(RELAY_LISTEN_PORT);
  si_me_R.sin_addr.s_addr = htonl(INADDR_ANY);
  int _bind_result = bind(s_R, (struct sockaddr*)&si_me_R, sizeof(si_me_R));
  if ( _bind_result == -1) error("ERROR bind");

  // listen for updates
  if (pthread_create(&receiverThread, NULL, &receiver, NULL) != 0) {
    perror("Can't start thread, terminating");
    return 1;
  }



  printf("Relay initialization complete\n");

  while (true) {
    usleep(1000000);
    // The relay automatically shuts itself off if it hasn't received any packets
    // within a certain time period (>3 seconds if it has already received a packet,
    // >15 seconds if it hasn't received any packets yet).
    framesPassed++;
    if (receivedPacket) {
      if (framesPassed > 3) exit(EXIT_SUCCESS);
    } else {
      if (framesPassed > 15) exit(EXIT_SUCCESS);
    }
  }

}
