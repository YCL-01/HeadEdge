//--------< Structure >--------//

/* Received signals from 4 normal edge and traffic
   signal which is attached to share memory A */
typedef struct recvSig{
	char traffic[2];
	char car_0[10];
	char car_0_speed[10];
	char ped_0[10]; 
	char car_1[10]; 
	char car_1_speed[10];
	char ped_1[10];
}recvSig;

/* thread arguments for receiving */
typedef struct recvArgs{
	int *sock;
	int shmId;
	int edgeType;
}recvArgs;

/* status signal (traffic stat + car stat + ped stat -- ex.223) */
typedef struct statSig{
	char poleStat0[256];
	char pole0Speed[10];
	char poleStat1[256];
	char pole1Speed[10];
}statSig;

/* thread arguments for fetching status from recvSig */
typedef struct cntArgs{
	recvSig *shmA;
	statSig *orgStat;
	statSig *newStat;
	int *scnGen;
}cntArgs;

/* generated scenario, including car speed, scn code, risk rate */
typedef struct scenario{
	char speed[10];
	char scnCode[10];
	char riskRate[10];
}scenario;

/* includes generated scenario for two poles. attached to share memory B*/
typedef struct actSig{
	scenario pole0;
	scenario pole1;
}actSig;

/* thread arguments for Signal generation */
typedef struct genArgs{
	actSig *shmB;
	statSig *newStat;
	int *scnGen;
}genArgs;

/*
typedef struct tranArgs{
	int *sock;
}tranArgs;
*/