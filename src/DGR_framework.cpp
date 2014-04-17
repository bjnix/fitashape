//DGR_framework.cpp
#include "dgr_framework/DGR_framework.h"

int slave_listen_port = SLAVE_LISTEN_PORT;

int framesPassed = 0;
bool receivedPacket = false;

std::map<std::string,MapNodePtr *> InputMap;
int s;
int milliseconds;
struct timespec req;

struct sockaddr_in si_me, si_other;
int slen;
pthread_t senderThread, receiverThread;


void error(const char *msg) 
{
    perror(msg);
    exit(1);
}


void * sender(void * data) {
    //packet properties
    int packet_length;
    unsigned char packet_counter;
    //current node properties
    char * node_buf;
    int node_length;
    unsigned char node_counter;
    //buffer
    char * packet_buffer= new char[BUFLEN];
    

    while (true) 
    {
        
        //packet_buffer properties
        packet_length = 2;
        packet_counter = 0;
        //current node properties
        node_length = 0;
        node_counter = 0;

        for(std::map<std::string,MapNodePtr *>::iterator it = InputMap.begin();it != InputMap.end();++it)
        {
        

            node_length = 0;

            MapNodePtr * cur_node = it->second;
            std::string cur_name = cur_node->name;
            char* cur_data = cur_node->getDataString();
            int cur_data_length = cur_node->dataLength;

            node_length = cur_name.length() + sizeof(char) + cur_data_length;

            node_buf = new char[node_length];

            //prepare node buffer
            memcpy(node_buf, cur_name.c_str(), cur_name.length());
            node_buf[cur_name.length()] = '\0';
            memcpy(node_buf + cur_name.length() + 1, cur_data, cur_data_length);

            //message buffer full. send and start a new one.
            if( (packet_length + node_length) > BUFLEN)
            {
                printf("!!! packet split !!!\n");
                if (sendto(s, packet_buffer, packet_length, 0, (struct sockaddr*)&si_other,slen) == -1) 
                    error ("ERROR sendto()");
                packet_counter ++;
                packet_length = 2;
                node_counter = 0;
                it--;

            }
            else
            { // add node to packet buffer
                memcpy(packet_buffer + packet_length, node_buf, node_length);
                packet_length += node_length;
                node_counter ++;
            }

        }
        //prepend some useful data
        packet_buffer[0] = packet_counter;
        packet_buffer[1] = node_counter;
        
        if (sendto(s, packet_buffer, BUFLEN, 0, (struct sockaddr*)&si_other,slen) == -1) 
            error ("ERROR sendto()");

        usleep(32000); /**< 30 fps */
    }
    return 0;
}

// The SLAVES receive state data from teh RELAY via UDP packets and parse the data

    
void * receiver(void * data){

    

    MapNodePtr * cur_node;
    std::string node_name;
    int node_data_length, packet_cursor;
    unsigned char node_counter,packet_counter;
    char * packet_buffer = new char[BUFLEN];

    while (true){
        
        if(recvfrom(s, packet_buffer, BUFLEN, 0, (struct sockaddr *)&si_other,
            &slen) == -1) error("ERROR recvfrom()");
        
        /*//print buffer contents
        
        std::cout << "|";
        for(int i = 0; i < BUFLEN; i++){
            std::cout << packet_buffer[i] <<"|";
        }
        std::cout << std::endl << std::endl;
        */
        
        
        receivedPacket = true;
        framesPassed = 0;
        packet_cursor = 2;

        packet_counter = packet_buffer[0];
        node_counter = packet_buffer[1];
        

        while( (node_counter > 0) && (packet_cursor < BUFLEN) )
        {
                        //extract name
            node_name = "";
            for(int i = packet_cursor; i < BUFLEN; i++) 
            {
                if(packet_buffer[i]) 
                { 
                    node_name.push_back(packet_buffer[i]); 
                    packet_cursor++; 
                }
                else 
                { 
                    packet_cursor++; 
                    break; 
                }
            }
                        //get current node and data length
            cur_node = InputMap.at(node_name);
            node_data_length = cur_node->dataLength;

            char * node_data = new char[node_data_length];
            node_data = packet_buffer + packet_cursor;
                        //set data
            cur_node->setData(node_data);
            packet_cursor += node_data_length;
            node_counter --;
        }
    }
}

void * slave_auto_kill(void * data){

    while(true){
        framesPassed++;
        //printf("frames passed: %d\n",framesPassed);
        if (receivedPacket) 
        {
           //ssprintf("has received Packet\n");
            if (framesPassed > 180) {
                //printf("DGR has revieved a packet and is timing out\n");
                exit(EXIT_SUCCESS);
            }
        } 
        else 
        {
            //printf("has not received Packet\n");
            if (framesPassed > 1500){
            //printf("DGR has not revieved a packet and is timing out\n");
            exit(EXIT_SUCCESS); // If your program takes a very long time to initialize,
                                                    // you can increase this value so the slaves don't prematurely
                                                    // shut themselves off.
            }
        }
        usleep(32000); /**< 30 fps */
    }
}

//constructors

DGR_framework::DGR_framework(char* r_IP){
   	
    InpMap = &InputMap;
	char *RELAY_IP = NULL;
	int so_broadcast = 1;
    RELAY_IP=r_IP;

    slen = sizeof(si_other);
    so_broadcast = 1;

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
        error("ERROR socket");
    
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(RELAY_LISTEN_PORT);

    if (inet_aton(RELAY_IP, &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    if (pthread_create(&senderThread, NULL, &sender, NULL) != 0) 
    {
        perror("Can't start thread, terminating\n");
        exit(1);
    }


}

DGR_framework::DGR_framework(int s_listen_port){
    
    slave_listen_port = s_listen_port;
    slaveInit();

}

DGR_framework::DGR_framework(){
    
    slave_listen_port = SLAVE_LISTEN_PORT;
    slaveInit();
}

DGR_framework::~DGR_framework(){
    close(s);
}

void DGR_framework::slaveInit(){
    InpMap = &InputMap;
    

    slen = sizeof(si_other);
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
        error("ERROR socket");
    
    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(slave_listen_port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) 
        error("ERROR bind");

    // kill slave when necessary
    if (pthread_create(&receiverThread, NULL, &slave_auto_kill, NULL) != 0) 
    {
        perror("Can't start thread, terminating");
        exit(1);
    }

    // listen for updates
    if (pthread_create(&receiverThread, NULL, &receiver, NULL) != 0) 
    {
        perror("Can't start thread, terminating");
        exit(1);
    }

}



