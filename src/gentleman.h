//--------< Defines >--------//
#define SHM_B_KEY	1112
#define CNT_CYCLE   500000 //usec

//--------< Function >-------//
//void sigGenerator();
void *counter(void *Args);
void gentleman(int shmId_A);//, int shmId_B);