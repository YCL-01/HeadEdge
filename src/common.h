//--------< Structure >--------//
typedef struct signal_A{
	int trafficSignal; // Traffic Signal 0 -> red, 1 -> green, 2-> green blink
	int remainingTime; // represent remaining signal time. works on green/green blink
	char car_0[10]; // SIG + SPEED
	char ped_0[10]; 
	char car_1[10]; // SIG
	char ped_1[10];
}sig_A;

typedef struct statSig{
	char statcode[256];
}statSig;

typedef struct genSig{
	char pole_0[10];
	char pole_1[10];
}genSig;

typedef struct recvArgs{
	int *sock;
	int shmId;
	int edgeType;
}recvArgs;

typedef struct tranArgs{
	int *sock;
}tranArgs;

typedef struct cntArgs{
	sig_A *counterSig;
    statSig *newSigIn;
    statSig *newSigOut;
	statSig *orgSigIn;
	statSig *orgSigOut;
	genSig *scnSig;
}cntArgs;
