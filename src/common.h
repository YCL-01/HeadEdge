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
	char * edgeType;
	// int였던 것을 char 로 수정
}recvArgs;

/* status signal (traffic stat + car stat + ped stat -- ex.223) */
typedef struct statSig{
	char poleStat0[10];
	char pole0Speed[10];
	char poleStat1[10];
	char pole1Speed[10];
}statSig;


/* generated scenario, including car speed, scn code, risk rate */
typedef struct scenario{
	char speed[10];
	char scnCode[10];
	char riskRate[10];
	char pol_num[2];
}scenario;

/* includes generated scenario for two poles. attached to share memory B*/
typedef struct actSig{
	scenario pole0;
	scenario pole1;
}actSig;


/* thread arguments for fetching status from recvSig */
typedef struct cntArgs{
	recvSig *shmA;
	actSig *shmB;
	statSig *orgStat;
	statSig *newStat;
}cntArgs;

typedef struct componentArgs{
	scenario pole0;
	scenario pole1;
	scenario main;
}componentArgs;