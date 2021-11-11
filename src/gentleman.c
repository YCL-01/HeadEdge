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

void *counter(void *Args){
    threadArgs_B *args = (threadArgs_B *)Args;
    sig_A *counterSig = args->counterSig;
    sig_B *newSigIn = args->newSigIn;
    sig_B *newSigOut = args->newSigOut;
    
    while(1){
        if(strlen(newSigIn->statcode) == 0){
            continue;
        }else{
            sprintf(newSigIn->statcode, "%s%s", counterSig->car_0, counterSig->ped_0);
            sprintf(newSigOut->statcode, "%s%s", counterSig->car_1, counterSig->ped_1);
            printf("newSigIn: %s\nnewSigOut: %s\n", newSigIn->statcode, newSigOut->statcode);
            usleep(CNT_CYCLE);
        }
    }
}

void gentleman(int shmId_A, int shmId_B){
    sig_A *counterSig;
    sig_B newSigIn;
    sig_B newSigOut;
    threadArgs_B counterArgs;

    void *shmAddr;
    if((shmAddr = shmat(shmId_A, (void *)0, 0)) == (void *)-1) {
        perror("Shmat failed");
        exit(0);
    }

    counterSig = (sig_A *)shmAddr;
    pthread_t counterThread;
    counterArgs.counterSig = counterSig;
    counterArgs.newSigIn = &newSigIn;
    counterArgs.newSigOut = &newSigOut;
    pthread_create(&counterThread, NULL, counter, (void *)&counterArgs);
    pthread_join(counterThread, NULL);   
}