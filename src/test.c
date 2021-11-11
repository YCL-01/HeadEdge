#include "tcp_server.h"
#include "gentleman.h"

int process_A(int shmId) {
    receiver(shmId);
    return 0;
}

int process_B(int shmId_A, int shmId_B) {
    gentleman(shmId_A, shmId_B);
    return 0;
}

/*
int processC(int shmId) {
    transceiver(shmId);
    return 0;
}
*/

int main(){
    //share memory
    int shmId[2] = {0, };
    key_t shmSigKey = SHM_A_KEY;
    key_t shmSCNKey = SHM_B_KEY;

    if((shmId[0] = shmget(shmSigKey, sizeof(sig_A), IPC_CREAT|0666)) == -1){
        printf("shmget failed!\n");
        exit(0);
    }
    /*
    if((shmId[1] = shmget(shmSCNKey, sizeof(sig_A), IPC_CREAT|0666)) == -1){
        printf("shmget failed!\n");
        exit(0);
    }
    */

    //multiprocess
    int pid_A, pid_B, pid_C;
    pid_A = fork();
    if(pid_A == 0){
        process_A(shmId[0]);
        sleep(1);
    }else{
        pid_B = fork();
        if(pid_B == 0){
            process_B(shmId[0], shmId[1]);
        }else{
            wait(pid_A);
            wait(pid_B);
            printf("Main processor exit!\n");

            /*
            pid_C = fork();
            if(pid_C == 0){
                test_processC();
            }else{
                wait(pid_A);
                wait(pid_B);
                wait(pid_C);
                printf("this is the parent");
                return 0;
            }
            */
        }
    }
}