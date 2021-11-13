#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <pthread.h>

#include "tcp_server.h"
#include "gentleman.h"
#include "common.h"

int process_A(int shmId) {
    receiver(shmId);
    return 0;
}

int process_B(int shmId_A, int shmId_B) {
    gentleman(shmId_A, shmId_B);
    return 0;
}


int process_C(int shmId) {
    transceiver(shmId);
    return 0;
}


int main(){
    //share memory
    int shmId[2] = {0, };
    key_t shmSigKey = SHM_A_KEY;
    key_t shmSCNKey = SHM_B_KEY;

    if((shmId[0] = shmget(shmSigKey, sizeof(sig_A), IPC_CREAT|0666)) == -1){
        printf("shmget failed!\n");
        exit(0);
    }
    
    if((shmId[1] = shmget(shmSCNKey, sizeof(genSig), IPC_CREAT|0666)) == -1){
        printf("shmget failed!\n");
        exit(0);
    }
    
   
    //multiprocess
    int pid_A, pid_B, pid_C;
    pid_A = fork();
    if(pid_A == 0){
        printf("process A\n");
        process_A(shmId[0]);
        sleep(1);
    }else{
        pid_B = fork();
        if(pid_B == 0){
            printf("process B\n");
            process_B(shmId[0], shmId[1]);
        }else{
            
            pid_C = fork();
            if(pid_C == 0){
                process_C(shmId[1]);
                printf("process C\n");
            }else{
                wait(pid_A);
                wait(pid_B);
                wait(pid_C);
                printf("this is the parent");
                return 0;
            }
            
        }
    }
}