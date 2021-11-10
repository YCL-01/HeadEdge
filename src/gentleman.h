//--------< Headers >--------//
#include "common.h"

//--------< Defines >--------//
#define SHM_B_KEY	1112
#define CNT_CYCLE   100000 //usec

//--------< Function >-------//
//void sigGenerator();
void *counter(void *Args);
void gentleman(int shmId_A, int shmId_B);