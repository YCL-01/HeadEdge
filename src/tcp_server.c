#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <pthread.h>
#include <netinet/ip.h>
#include <ifaddrs.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"
#include "tcp_server.h"
#include "trafficSignal.h"

// define traffic signal port and baudspeed info
#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUD_SPEED 38400

//int getEdgeType(){}

void *workThread(void *Args){
    threadArgs_A *args = (threadArgs_A *)Args;
    sig_A *threadSig;
    void *shmAddr;
    int edgeType = args->edgeType;

    int *sockAddr = args->sock;
    int sock = *(sockAddr);
    int recvData;
    char buf[10] = {0, };

    if((shmAddr = shmat(args->shmId, (void *)0, 0)) == (void *)-1) {
        perror("Shmat failed");
        exit(0);
    }
    threadSig = (sig_A *)shmAddr;
    printf("Thread\n");
    while(1) {
        recvData = recv(sock, buf, sizeof(buf), 0);
        //printf("recv: %s\n", buf);
        switch (edgeType)
        {
        case 0:
            sprintf(threadSig->car_0,"%s",buf);
            break;
        case 1:
            sprintf(threadSig->ped_0,"%s",buf);
            break;
        case 2:
            sprintf(threadSig->car_1,"%s",buf);
            break;
        case 3:
            sprintf(threadSig->ped_1,"%s",buf);
            break;
        default:
            break;
        }

        // get Traffic signal
        //threadSig->trafficSignal = getTrafficSignal();
        //if(threadSig->trafficSignal != 0){
        //   threadSig->remainingTime = getRemainingTime();
        //}
    }
}

int receiver(int shmId) {
    int sockFd, clientSock;
    struct sockaddr_in address, clientAddr;
    int addlen = sizeof(address);

    if((sockFd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        printf("socket error\n");
        exit(1);
    }
    
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(SERV_IP);
    address.sin_port = htons(SERV_PORT);

    if(bind(sockFd, (struct sockaddr *)&address, addlen)<0)
    {
        printf("socket bind error\n");
        exit(1);
    }
 
    if(listen(sockFd, 5) < 0)
    {
        printf("socket listen wrong\n");
        exit(1);
    }
    printf("Server Activated\n");
    
    runTrafficSignalThread(SERIAL_PORT, BAUD_SPEED);
    
    pthread_t cliThread[NORM_EDGE];
    threadArgs_A args;
    for(int i=0; i<NORM_EDGE; i++){
        printf("Got into for loop\n");
        if((clientSock = accept(sockFd, (struct sockaddr *)&clientAddr, (socklen_t *)&addlen))< 0) {
            printf("client socket went wrong\n");
            exit(1);
        }
        printf("accepted\n");

        args.sock = &clientSock;
        args.shmId = shmId;
        args.edgeType = i; // getEdgeType(ip_data)

        pthread_create(&cliThread[i], NULL, workThread, (void *)&args);
    }
    for(int i=0; i<NORM_EDGE; i++){
        pthread_join(cliThread[0],NULL);
    }
    return 0;
}