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

//void sigGenerator(sig_B genSig);

void *counter(void *Args){
    threadArgs_B *args = (threadArgs_B *)Args;
    sig_A *counterSig = args->counterSig;
    
    sig_B *orgSigIn = args->orgSigIn;
    sig_B *orgSigOut = args->orgSigOut;
    
    sig_B *newSigIn = args->newSigIn;
    sig_B *newSigOut = args->newSigOut;
    
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

        printf("OrgIn: %s\nOrgOut: %s\n", orgSigIn->statcode, orgSigOut->statcode);
        printf("car_in: %s\ncar_out: %s\nnewIn: %s\nnewOut: %s\n\n\n", counterSig->car_0, counterSig->car_1, newSigIn->statcode, newSigOut->statcode);
        //printf("orgIn: %s : newIn: %s \norgOut: %s : newIn: %s\n ", orgSigIn->statcode, newSigIn->statcode, orgSigOut->statcode, newSigIn->statcode);
        
        usleep(CNT_CYCLE);
    }
    return (void *)0;
}

void gentleman(int shmId_A){ //, int shmId_B){
    sig_A *counterSig;
    sig_B orgSigIn;
    sig_B orgSigOut;
    sig_B newSigIn;
    sig_B newSigOut;
    threadArgs_B counterArgs;
    //int i = 0;

    void *shmAddr;
    if((shmAddr = shmat(shmId_A, (void *)0, 0)) == (void *)-1) {
        perror("Shmat failed");
        exit(0);
    }else{
        printf("gentleman, shm complete\n");
    }
    
    // Counter //
    counterSig = (sig_A *)shmAddr;
    pthread_t counterThread; 
    counterArgs.counterSig = counterSig;
    counterArgs.newSigIn = &newSigIn;
    counterArgs.newSigOut = &newSigOut;
    counterArgs.orgSigIn = &orgSigIn;
    counterArgs.orgSigOut = &orgSigOut;
    pthread_create(&counterThread, NULL, counter, (void *)&counterArgs);
    pthread_join(counterThread, NULL);
    
    /*
    while(1){
        usleep(CNT_CYCLE);
        if(strlen(counterSig->car_0) > 0){
            printf("counterSig: %s / %s / %s / %s\n", counterSig->car_0, counterSig->ped_0, counterSig->car_1, counterSig->ped_1);
        }
    }
    */
    //

    if ( -1 != shmctl(shmId_A, IPC_RMID, 0)) { 
        printf( "Shared Memory Delete Success!\n");
    }else {
		printf( "Shared Memory Delete Failed..\n");
    }

    exit(0);
}