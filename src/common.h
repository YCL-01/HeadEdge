//--------< Headers >--------//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sys/ipc.h> 
#include <sys/shm.h>
#include <pthread.h> 

//--------< Structure >--------//
typedef struct signal_A{
	int trafficSignal = 0; // Traffic Signal 0 -> red, 1 -> green, 2-> green blink
	int remainingTime = 0; // represent remaining signal time. works on green/green blink
	char car_0[5]; // SIG + SPEED
	char car_1[5]; 
	char ped_0[5]; // SIG
	char ped_1[5];
}sig_A;

typedef struct signal_B{
	char statcode[256];
}sig_B;

typedef struct threadArgs_A{
	int *sock;
	int shmId;
	int edgeType;
}threadArgs_A;

typedef struct threadArgs_B{
	sig_A *counterSig;
    sig_B *newSigIn;
    sig_B *newSigOut;
}threadArgs_B;
