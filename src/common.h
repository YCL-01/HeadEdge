//--------< Structure >--------//
typedef struct signal_A{
	int trafficSignal; // Traffic Signal 0 -> red, 1 -> green, 2-> green blink
	int remainingTime; // represent remaining signal time. works on green/green blink
	char car_0[10]; // SIG + SPEED
	char ped_0[10]; 
	char car_1[10]; // SIG
	char ped_1[10];
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
	sig_B *orgSigIn;
	sig_B *orgSigOut;
}threadArgs_B;
