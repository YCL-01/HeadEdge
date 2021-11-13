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
#define SERIAL_PORT "/dev/tty0"
#define BAUD_SPEED 38400
/*
int getEdgeType(){
    ip_table
}*/



void *recvThread(void *Args) {
    recvArgs *args = (recvArgs *)Args;
    sig_A *threadSig;
    void *shmAddr;
    int edgeType = args->edgeType;

    int *sockAddr = args->sock;
    int sock = *(sockAddr);
    int recvData;
    char buf[10];
    char *edge, *data;

    if((shmAddr = shmat(args->shmId, (void *)0, 0)) == (void *)-1) {
        perror("Shmat failed");
        exit(0);
    }
    threadSig = (sig_A *)shmAddr;

    while(1) {
        recvData = recv(sock, buf, sizeof(buf), 0);
        //printf("recv: %s\n", buf);
        edge = strtok(buf, " ");
        //printf("edge: %s\n",edge);
        data = strtok(NULL, ".");
        //printf("data: %s\n",data);
        switch (edge[0])
        {
        case 'c':
            switch (edge[1])
            {
            case '0':
                sprintf(threadSig->car_0,"%s",data);
                break;
            case '1':
                sprintf(threadSig->car_1,"%s",data);
                break;
            }
            break;
        case 'p':
            switch (edge[1])
            {
            case '0':
                sprintf(threadSig->ped_0,"%s",data);
                break;
            case '1':
                sprintf(threadSig->ped_1,"%s",data);
                break;
            }
            break;
        }

        //get Traffic signal
        threadSig->trafficSignal = getTrafficSignal();
        if(threadSig->trafficSignal != 0){
            threadSig->remainingTime = getRemainingTime();
        }
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
    recvArgs args;

    for(int i=0; i<NORM_EDGE; i++){
        if((clientSock = accept(sockFd, (struct sockaddr *)&clientAddr, (socklen_t *)&addlen))< 0) {
            printf("client socket went wrong\n");
            exit(1);
        }
        printf("new client connected %d\n", i);
        args.sock = &clientSock;
        args.shmId = shmId;
        args.edgeType = i; // getEdgeType(ip_data)

        pthread_create(&cliThread[i], NULL, recvThread, (void *)&args);
    }

    for(int i=0; i<NORM_EDGE; i++){
        pthread_join(cliThread[0],NULL);
    }

    return 0;
}

// void *tranThread(void *Args) { }

int transceiver(int shmId) {
    
    genSig *shareMemBSig; // shouldbe scn
    void *shmAddrB;
    if((shmAddrB = shmat(shmId, (void *)0, 0)) == (void *)-1) {
        perror("Shmat failed");
        exit(0);
    }else{
        printf("gentleman, shm B complete\n");
    }
    shareMemBSig = (genSig *)shmAddrB;

    int clientSock;
    struct sockaddr_in servAddr;
    if((clientSock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        printf("client socket error\n");
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(TEST_IP);
    servAddr.sin_port = htons(TEST_PORT);

    for(int i=0; i<5; i++){
        printf("connection to client %d sec left\n", 5-i);
        sleep(1);
    }

    if(connect(clientSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) {
        printf("client connect error\n");
    }
    char message[MAX_BUF_SIZE] = {0, };
    int i = 0;
    char prev_message[MAX_BUF_SIZE] = {0, };
    while(1) {
        if(i == 0) {
            sprintf(message, "pole_0:%s / pole_1:%s", shareMemBSig->pole_0, shareMemBSig->pole_1);
            sprintf(prev_message, "pole_0:%s / pole_1:%s", shareMemBSig->pole_0, shareMemBSig->pole_1);
            printf("%s - %d\n", message, i);
            write(clientSock, message, sizeof(message));
            i++;
        }
        else {
            sprintf(message, "pole_0:%s / pole_1:%s", shareMemBSig->pole_0, shareMemBSig->pole_1);
            if(strcmp(prev_message, message) == 0) {
                continue;
                usleep(10000);
            }else{
                printf("%s - %d\n", message, i);
                write(clientSock, message, sizeof(message));
                sprintf(prev_message, "%s", message);
                i++;
            }
        }
    }
    
    return 0;
}