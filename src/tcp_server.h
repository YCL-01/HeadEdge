#include <stdio.h>
#include <netinet/ip.h>
#include <ifaddrs.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <netinet/in.h>

//-----< Share memory >------//
#define SHM_A_KEY		1111

//----< Network Setting >----//
#define SERV_PORT		2555
#define SERV_IP			"192.168.0.12"

//#define VMS_PORT		100
//#define SPK_PORT		9531
#define MAX_BUFFER		256
//#define COMPONETS		4
#define NORM_EDGE		4	

#define TEST_IP			"192.168.0.10"
#define TEST_PORT		2556	

//--------<IP TABLE>---------//

typedef struct client{
	char ip[15];
	char edgeType[5];
}client;

const client edgeIpList[] = {
    { "192.168.0.30", "C0" }, 	// pole 0 car edge 
    { "192.168.0.32", "C1" }, 	// pole 1 car edge  
    { "192.168.0.31", "P0" }, 	// pole 0 ped edge
    { "192.168.0.33", "P1" }, 	// pole 1 ped edge
}; 



//-------< Functions >-------//
char *getEdgeType(char *ip);
int receiver(int shmId);
void *recvThread(void *Args);

/* 	Recv SCENARIO LUT signals from 
 	gentleman and tranceive them
 	1 clients(*) - 4 server	 			*/ 
int transceiver(int shmId);
