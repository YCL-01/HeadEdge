//--------< Defines >--------//
#define SHM_B_KEY	1112
#define CNT_CYCLE   100000 //usec
#define GEN_CYCLE   50000

//------<SCN LUT TABLE>------//
typedef struct scn{
	char stat[10];
	char code[10];
    char riskRate[10];
}scn;

//--------< Function >-------//
struct scn getScnCode(char *stat);
void *sigGen(void *Args);
void *counter(void *Args);

void gentleman(int shmId_A, int shmId_B);


