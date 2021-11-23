
#include <stdio.h>
#include <netinet/ip.h>
#include <ifaddrs.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <netinet/in.h>

//-----< Share memory >------//
#define SHM_A_KEY		1111
#define SHM_B_KEY		1112

//----< Network Setting >----//
#define SERV_PORT		2555
#define SERV_IP			"192.168.0.34"

//#define VMS_PORT		100
//#define SPK_PORT		9531
#define MAX_BUFFER		256
//#define COMPONETS		4
#define NORM_EDGE		4	

#define TEST_IP			"192.168.0.7"
#define TEST_PORT		2556	


//-------< Functions >-------//
const char *getEdgeType(char *ip);
int receiver(int shmId);
void *recvThread(void *Args);

/* 	Recv SCENARIO LUT signals from 
 	gentleman and tranceive them
 	1 clients(*) - 4 server	 			*/ 
int transceiver(int shmId);

