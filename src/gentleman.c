//--------< Headers >--------//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sys/ipc.h> 
#include <sys/shm.h>
#include <pthread.h>
#include "gentleman.h"
#include "common.h"

void sigGenerator(sig_B genSig);

int sigCmp(sig_B *orgSigIn, sig_B *orgSigOut, sig_B *newSigIn, sig_B *newSigOut){

    char *orgSigA = orgSigIn->statcode;
    char *orgSigB = orgSigOut->statcode;
    char *newSigA = newSigIn->statcode;
    char *newSigB = newSigOut->statcode;
    int change = 0;
    
    if(strcmp(orgSigA, newSigA)){
        if(strcmp(orgSigB, newSigB)){
            return 1;
        }else{
            return 2;
        }
    }else{
        if(strcmp(orgSigB, newSigB)){
            return 3;
        }else{
            return 4;
        }
    }
}

void *counter(void *Args){
    threadArgs_B *args = (threadArgs_B *)Args;
    sig_A *counterSig = args->counterSig;
    
    sig_B *orgSigIn = args->orgSigIn;
    sig_B *orgSigOut = args->orgSigOut;
    
    sig_B *newSigIn = args->newSigIn;
    sig_B *newSigOut = args->newSigOut;
    
    int pedIn = 0, pedOut = 0;

    while(1){
        sprintf(orgSigIn->statcode, "%s", newSigIn->statcode);
        sprintf(orgSigOut->statcode, "%s", newSigIn->statcode);
        pedIn = counterSig->ped_0;
        pedOut = counterSig->ped_1;
        //pedIn = ped_0
        if(pedIn>=pedOut){
            sprintf(newSigIn->statcode, "%s%s", counterSig->car_0, counterSig->ped_0);
            sprintf(newSigOut->statcode, "%s%s", counterSig->car_1, counterSig->ped_0);
        }else{
            sprintf(newSigIn->statcode, "%s%s", counterSig->car_0, counterSig->ped_1);
            sprintf(newSigOut->statcode, "%s%s", counterSig->car_1, counterSig->ped_1);
        }
        
        switch(sigCmp(orgSigIn, newSigIn, orgSigOut, newSigOut))
        {
        case 1:
            printf("none changed!\n");
            usleep(CNT_CYCLE);
            continue;
        case 2:
            printf("Pole 2 changed!\n");
            usleep(CNT_CYCLE);
            break;
        case 3:
            printf("Pole 1 changed!\n");
            usleep(CNT_CYCLE);
            break;
        case 4:
            printf("Pole-1 and Pole-2 changed!\n");
            usleep(CNT_CYCLE);
            break;
        }
    }
}

void gentleman(int shmId_A){ //, int shmId_B){
    sig_A *counterSig;
    sig_B orgSigIn;
    sig_B orgSigOut;
    sig_B newSigIn;
    sig_B newSigOut;
    threadArgs_B counterArgs;

    void *shmAddr;
    if((shmAddr = shmat(shmId_A, (void *)0, 0)) == (void *)-1) {
        perror("Shmat failed");
        exit(0);
    }

    // Counter
    // attach shm Sig_A to counterSig
    counterSig = (sig_A *)shmAddr; 
    //pthread_t counterThread; 
    counterArgs.counterSig = counterSig;
    counterArgs.newSigIn = &newSigIn;
    counterArgs.newSigOut = &newSigOut;
    counterArgs.orgSigIn = &orgSigIn;
    counterArgs.orgSigOut = &orgSigOut;
    while(1){
        if(strlen(counterSig->car_0) > 0){
            printf("counterSig car_0: %s\n", counterSig->car_0);
            printf("counterSig car_1: %s\n", counterSig->car_1);
            printf("counterSig ped_0: %s\n", counterSig->ped_0);
            printf("counterSig ped_1: %s\n", counterSig->ped_1);
        }else{
            continue;
        }
    }
    //pthread_create(&counterThread, NULL, counter, (void *)&counterArgs);
    //pthread_join(counterThread, NULL);
}