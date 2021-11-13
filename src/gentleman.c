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

void sigGenerator(genSig *args){
    printf("pole_1: %s\npole_2: %s\n\n\n", args->pole_0, args->pole_1);
    // scenario func
}

void *counter(void *Args){
    threadArgs_B *args = (threadArgs_B *)Args;
    sig_B *orgSigIn = args->orgSigIn;
    sig_B *orgSigOut = args->orgSigOut;
    sig_B *newSigIn = args->newSigIn;
    sig_B *newSigOut = args->newSigOut;
    sig_A *counterSig = args->counterSig;

    genSig *result = args->scnSig;
    
    int pedIn = 0, pedOut = 0;

    //for(i=0; i<20; i++)
    while(1){
        sprintf(orgSigIn->statcode, "%s", newSigIn->statcode);
        sprintf(orgSigOut->statcode, "%s", newSigOut->statcode);
        
        pedIn = atoi(counterSig->ped_0);
        pedOut = atoi(counterSig->ped_1);
        //pedIn = ped_0
        if(pedIn >= pedOut){
            sprintf(newSigIn->statcode, "%s %s", counterSig->car_0, counterSig->ped_0);
            sprintf(newSigOut->statcode, "%s %s", counterSig->car_1, counterSig->ped_0);
        }else{
            sprintf(newSigIn->statcode, "%s %s", counterSig->car_0, counterSig->ped_1);
            sprintf(newSigOut->statcode, "%s %s", counterSig->car_1, counterSig->ped_1);
        }
        // Signal compare
        
        
        //printf("OrgIn: %s\nOrgOut: %s\n", orgSigIn->statcode, orgSigOut->statcode);
        //printf("car_in: %s\ncar_out: %s\nnewIn: %s\nnewOut: %s\n", counterSig->car_0, counterSig->car_1, newSigIn->statcode, newSigOut->statcode);
        //printf("orgIn: %s : newIn: %s \norgOut: %s : newIn: %s\n ", orgSigIn->statcode, newSigIn->statcode, orgSigOut->statcode, newSigIn->statcode);
        
        sprintf(result->pole_0, "%s", newSigIn->statcode);
        sprintf(result->pole_1, "%s", newSigOut->statcode);
        printf("Pole_0: %s -- newIn: %s \nPole_1: %s -- newOut: %s\n\n\n", result->pole_0, newSigIn->statcode, result->pole_1, newSigOut->statcode);
        usleep(CNT_CYCLE);
    }
    return (void *)0;
}

void gentleman(int shmId_A, int shmId_B){
    sig_A *shareMemASig;
    genSig *shareMemBSig;
    
    sig_B orgSigIn;
    sig_B orgSigOut;
    sig_B newSigIn;
    sig_B newSigOut;

    void *shmAddrA;
    if((shmAddrA = shmat(shmId_A, (void *)0, 0)) == (void *)-1) {
        perror("Shmat failed");
        exit(0);
    }else{
        printf("gentleman, shm A complete\n");
    }
    shareMemASig = (sig_A *)shmAddrA;

    void *shmAddrB;
    if((shmAddrB = shmat(shmId_B, (void *)0, 0)) == (void *)-1) {
        perror("Shmat failed");
        exit(0);
    }else{
        printf("gentleman, shm B complete\n");
    }
    shareMemBSig = (genSig *)shmAddrB;
    
    // Counter //
    threadArgs_B counterArgs;
    pthread_t counterThread; 

    counterArgs.counterSig = shareMemASig;
    counterArgs.newSigIn = &newSigIn;
    counterArgs.newSigOut = &newSigOut;
    counterArgs.orgSigIn = &orgSigIn;
    counterArgs.orgSigOut = &orgSigOut;
    counterArgs.scnSig = shareMemBSig;

    pthread_create(&counterThread, NULL, counter, (void *)&counterArgs);
    
    // Signal generator //
    
    while(1){
        usleep(CNT_CYCLE);
        sigGenerator(shareMemBSig);
    }

    pthread_join(counterThread, NULL);
    //

    if ( -1 != shmctl(shmId_A, IPC_RMID, 0)) { 
        printf( "Shared Memory Delete Success!\n");
    }else {
		printf( "Shared Memory Delete Failed..\n");
    }

    if ( -1 != shmctl(shmId_B, IPC_RMID, 0)) { 
        printf( "Shared Memory Delete Success!\n");
    }else {
		printf( "Shared Memory Delete Failed..\n");
    }

    exit(0);
}