//--------< Headers >--------//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sys/ipc.h> 
#include <sys/shm.h>
#include <pthread.h>
#include "gentleman.h"
#include "common.h"

// compile error 나서 gentleman.h 에 있는 작성되있던 lookuptable, 옮겼습니다.
const scn scnLookUpTable[] = {
    { "0 0 0", "A1", "0" }, 
    { "0 0 1", "A2", "0" },
    { "0 0 2", "A3", "0" },  
    { "0 0 3", "A4", "0" }, 
    { "0 1 0", "A5", "0" }, 
    { "0 1 1", "A6", "1" }, 
    { "0 1 2", "A7", "1" },
    { "0 1 3", "A8", "1" },  
    { "0 2 0", "A9", "0" }, 
    { "0 2 1", "A10", "1" },
    { "0 2 2", "A11", "1" },
    { "0 2 3", "A12", "1" }, 
    { "0 3 0", "A13", "0" },
    { "0 3 1", "A14", "2" },
    { "0 3 2", "A15", "2" }, 
    { "0 3 3", "A16", "2" },  
    { "1 0 0", "A17", "0" }, 
    { "1 0 1", "A18", "0" },
    { "1 0 2", "A19", "0" }, 
    { "1 0 3", "A20", "0" }, 
    { "1 1 0", "A21", "0" },  
    { "1 1 1", "A22", "1" },
    { "1 1 2", "A23", "1" }, 
    { "1 1 3", "A24", "1" },
    { "1 2 0", "A25", "0" }, 
    { "1 2 1", "A26", "1" },
    { "1 2 2", "A27", "1" },
    { "1 2 3", "A28", "1" }, 
    { "1 3 0", "A29", "0" },  
    { "1 3 1", "A30", "2" },
    { "1 3 2", "A31", "2" }, 
    { "1 3 3", "A32", "2" }, 
    { "2 0 0", "A33", "0" }, 
    { "2 0 1", "A34", "0" },  
    { "2 0 2", "A35", "0" }, 
    { "2 0 3", "A36", "0" },
    { "2 1 0", "A37", "0" },
    { "2 1 1", "A38", "1" }, 
    { "2 1 2", "A49", "2" },  
    { "2 1 3", "A40", "2" }, 
    { "2 2 0", "A41", "0" }, 
    { "2 2 1", "A42", "1" }, 
    { "2 2 2", "A43", "2" },  
    { "2 2 3", "A44", "2" }, 
    { "2 3 0", "A45", "0" },
    { "2 3 1", "A46", "2" },
    { "2 3 2", "A47", "3" },
    { "2 3 3", "A48", "3" } 
}; 


scn getScnCode(char *stat){
	
	int i=0;
	scn result;
	sprintf(result.stat,"%s", stat);
	
	while(1){
		if((strcmp(stat, scnLookUpTable[i].stat)) == 0){
			break;
		}else{
			i++;
		}
	}
	sprintf(result.code, "%s", scnLookUpTable[i].code);
	sprintf(result.riskRate, "%s", scnLookUpTable[i].riskRate);
	return result;
}

void *sigGen(void *Args) {
    genArgs *args = (genArgs *)Args;
    statSig *stat = args->newStat;
    actSig *shmB = args->shmB;
    int *scnGen = args->scnGen;

    while(1) {
        switch(*(scnGen))
        {
        case 0:
            sprintf(shmB->pole0.speed, "%s", stat->pole0Speed);
            sprintf(shmB->pole1.speed, "%s", stat->pole1Speed);
            usleep(GEN_CYCLE);
            break;
        case 1:
        // switch 문에서 변수 선언할때 중괄호 없으면 컴파일 에러 나서 추가했습니다.
            {
            scn result = getScnCode(stat->poleStat0);
            sprintf(shmB->pole0.scnCode, "%s", result.code);
            sprintf(shmB->pole0.riskRate, "%s", result.riskRate);
            sprintf(shmB->pole0.speed, "%s", stat->pole0Speed);
            sprintf(shmB->pole1.speed, "%s", stat->pole1Speed);
            usleep(GEN_CYCLE);
            break;
            }
        case 2:
            {
            scn result = getScnCode(stat->poleStat1);
            sprintf(shmB->pole1.scnCode, "%s", result.code);
            sprintf(shmB->pole1.riskRate, "%s", result.riskRate);
            sprintf(shmB->pole0.speed, "%s", stat->pole0Speed);
            sprintf(shmB->pole1.speed, "%s", stat->pole1Speed);
            usleep(GEN_CYCLE);
            break;
            }
        case 3:
            {
            scn result_0 = getScnCode(stat->poleStat0);
            scn result_1 = getScnCode(stat->poleStat1);
            sprintf(shmB->pole0.scnCode, "%s", result_0.code);
            sprintf(shmB->pole0.riskRate, "%s", result_0.riskRate);
            sprintf(shmB->pole1.scnCode, "%s", result_1.code);
            sprintf(shmB->pole1.riskRate, "%s", result_1.riskRate);
            sprintf(shmB->pole0.speed, "%s", stat->pole0Speed);
            sprintf(shmB->pole1.speed, "%s", stat->pole1Speed);
            usleep(GEN_CYCLE);
            break;
            }
        }
    }
}


void *counter(void *Args){
    cntArgs *args = (cntArgs *)Args;
    // original status and new fetched status
    statSig *orgStat = args-> orgStat;
    statSig *newStat = args-> newStat;
    recvSig *shmA = args->shmA;
    int ped0=0, ped1=0, ped=0, i=0, *scnGen = args->scnGen;
    
    while(1){
        ped0 = atoi(shmA->ped_0);
        ped1 = atoi(shmA->ped_1);
        if(ped0 >= ped1) { 
            ped = ped0;
        }else{
            ped = ped1;
        }

        if(i == 0){
            sprintf(newStat->poleStat0, "%s %s %c", shmA->traffic, shmA->car_0, atoi(ped));
            sprintf(newStat->pole0Speed, "%s", shmA->car_0_speed);
            sprintf(newStat->poleStat1, "%s %s %c", shmA->traffic, shmA->car_1, atoi(ped));
            sprintf(newStat->pole0Speed, "%s", shmA->car_0_speed);
            i++;
            *(scnGen) = 4;
            sprintf(orgStat->poleStat0, "%s", newStat->poleStat0);
            sprintf(orgStat->pole0Speed, "%s", newStat->pole0Speed);
            sprintf(orgStat->poleStat1, "%s", newStat->poleStat1);
            sprintf(orgStat->pole0Speed, "%s", newStat->pole0Speed);
        }else{
            // Signal compare
            sprintf(newStat->poleStat0, "%s %s %c", shmA->traffic, shmA->car_0, atoi(ped));
            sprintf(newStat->pole0Speed, "%s", shmA->car_0_speed);
            sprintf(newStat->poleStat1, "%s %s %c", shmA->traffic, shmA->car_1, atoi(ped));
            sprintf(newStat->pole0Speed, "%s", shmA->car_0_speed);

            if(strcmp(newStat->poleStat0, orgStat->poleStat0) == 0){
                if(strcmp(newStat->poleStat1, orgStat->poleStat1) == 0){
                    // pole 0 and pole 1 nothin changed
                    *(scnGen) = 0;
                    usleep(CNT_CYCLE);
                    continue;
                }else{
                    // pole 1 changed
                    *(scnGen) = 2;
                    sprintf(orgStat->poleStat1, "%s", newStat->poleStat1);
                    usleep(CNT_CYCLE);
                    continue;
                }
            }else{
                if(strcmp(newStat->poleStat1, orgStat->poleStat1) == 0){
                    // pole 0 changed
                    *(scnGen) = 1;
                    sprintf(orgStat->poleStat0, "%s", newStat->poleStat0);
                    usleep(CNT_CYCLE);
                    continue;
                }else{
                    // pole 0 and pole 1 changed
                    *(scnGen) = 3;
                    sprintf(orgStat->poleStat0, "%s", newStat->poleStat0);
                    sprintf(orgStat->poleStat1, "%s", newStat->poleStat1);
                    usleep(CNT_CYCLE);
                    continue;
                }
            }
        }
    }
    return (void *)0;
}

void gentleman(int shmId_A, int shmId_B) {
    
    recvSig *shareMemA;
    actSig *shareMemB;
    statSig *orgStat, *newStat;
    int scnGen;

    // attach share memory A
    void *shmAddrA;
    if((shmAddrA = shmat(shmId_A, (void *)0, 0)) == (void *)-1) {
        perror("Shmat failed");
        exit(0);
    }else{
        printf("gentleman, shm A complete\n");
    }
    shareMemA = (recvSig *)shmAddrA;
    // attach share memory B
    void *shmAddrB;
    if((shmAddrB = shmat(shmId_B, (void *)0, 0)) == (void *)-1) {
        perror("Shmat failed");
        exit(0);
    }else{
        printf("gentleman, shm B complete\n");
    }
    shareMemB = (actSig *)shmAddrB;
    
    // Counter //
    cntArgs counterArgs;
    pthread_t counterThread; 
    counterArgs.shmA = shareMemA;
    counterArgs.orgStat = orgStat;
    counterArgs.newStat = newStat;
    counterArgs.scnGen = &scnGen;
    pthread_create(&counterThread, NULL, counter, (void *)&counterArgs);

    // SigGen //
    genArgs generatorArgs;
    pthread_t generatorThread;
    generatorArgs.shmB = shareMemB;
    generatorArgs.newStat = newStat;
    generatorArgs.scnGen = &scnGen;
    pthread_create(&generatorThread, NULL, sigGen, (void *)&generatorArgs);

    pthread_join(counterThread, NULL);
    pthread_join(generatorThread, NULL);

    if ( -1 != shmctl(shmId_A, IPC_RMID, 0)) { 
        printf( "Shared Memory Delete Success!\n");
    }else {
		printf( "Shared Memory Delete Failed..\n");
    }

    if ( -1 != shmctl(shmId_B, IPC_RMID, 0)) { 
        printf( "Shared Memory Delete Success!\n");
    }else {
		printf( "Shared Memory Delete Failed..\n");
    }

    exit(0);
}
