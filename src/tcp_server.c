#include "tcp_server.h"
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
        return 1;
    }
    threadSig = (sig_A *)shmAddr;
    
    while(1) {
        recvData = recv(sock, buf, sizeof(buf), 0);
        printf("recv: %s\n", buf);
        switch (edgeType)
        {
        case 0:
            sprintf(threadSig->car_0,"%s ",buf);
            break;
        case 1:
            sprintf(threadSig->ped_0,"%s ",buf);
            break;
        case 2:
            sprintf(threadSig->car_1,"%s ",buf);
            break;
        case 3:
            sprintf(threadSig->ped_1,"%s ",buf);
            break;
        default:
            break;
        }
    }
}

int receiver(int shmId) {

    int sockFd, clientSock;
    struct sockaddr_in address, clientAddr;
    int addlen = sizeof(address);

    if((sockFd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        printf("socket error");
        exit(1);
    }
    
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(SERV_IP);
    address.sin_port = htons(SERV_PORT);

    if(bind(sockFd, (struct sockaddr *)&address, addlen)<0)
    {
        printf("socket bind error");
        exit(1);
    }
 
    if(listen(sockFd, 1) < 0)
    {
        printf("socket listen wrong");
        exit(1);
    }

    printf("Server Activated\n");
    pthread_t newThread[NORM_EDGE];
    threadArgs_A args;
    
    for(int i=0; i<NORM_EDGE; i++){
        if((clientSock = accept(sockFd, (struct sockaddr *)&clientAddr, (socklen_t *)&addlen))< 0) {
            printf("client socket went wrong");
            exit(1);
        }
        args.sock = &clientSock;
        args.shmId = shmId;
        args.edgeType = i; // getEdgeType(ip_data)
        pthread_create(&newThread[i], NULL, workThread, (void *)&args);
    }
    for(int i=0; i<NORM_EDGE; i++){
        pthread_join(newThread[0],NULL);
    }
    return 0;
}