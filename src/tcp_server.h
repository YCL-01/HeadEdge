//--------< Headers >--------//
#include "common.h"
#include <netinet/ip.h>
#include <ifaddrs.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <netinet/in.h>

//-----< Share memory >------//
#define SHM_A_KEY		1111

//----< Network Setting >----//
#define SERV_PORT		2555
#define SERV_IP			"192.168.0.10"

//#define VMS_PORT		100
//#define SPK_PORT		9531
#define MAX_BUFFER		256
//#define COMPONETS		4
#define NORM_EDGE		4		

//--------<IP TABLE>---------//
// Should be added before testing in field

//-------< Functions >-------//
int receiver(int shmId);

/* 	Recv SCENARIO LUT signals from 
 	gentleman and tranceive them
 	1 clients(*) - 4 server	 			*/ 
// int transceiver(int shmId);
