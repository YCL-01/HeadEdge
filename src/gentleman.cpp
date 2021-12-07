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
/* Mysql headers */
#include "/usr/include/mysql/mysql.h"

typedef struct scn{
	char stat[10];
	char code[10];
    char riskRate[10];
}scn;



// compile error 나서 gentleman.h 에 있는 작성되있던 lookuptable, 옮겼습니다.
scn scnLookUpTable[] = {
    { "000", "1", "0" }, 
    { "001", "2", "0" },
    { "002", "3", "0" },  
    { "003", "4", "0" }, 
    { "010", "5", "0" }, 
    { "011", "6", "1" }, 
    { "012", "7", "1" },
    { "013", "8", "1" },  
    { "020", "9", "0" }, 
    { "021", "10", "1" },
    { "022", "11", "1" },
    { "023", "12", "1" }, 
    { "030", "13", "0" },
    { "031", "14", "2" },
    { "032", "15", "2" }, 
    { "033", "16", "2" },  
    { "100", "17", "0" }, 
    { "101", "18", "0" },
    { "102", "19", "0" }, 
    { "103", "20", "0" }, 
    { "110", "21", "0" },  
    { "111", "22", "1" },
    { "112", "23", "1" }, 
    { "113", "24", "1" },
    { "120", "25", "0" }, 
    { "121", "26", "1" },
    { "122", "27", "1" },
    { "123", "28", "1" }, 
    { "130", "29", "0" },  
    { "131", "30", "2" },
    { "132", "31", "2" }, 
    { "133", "32", "2" }, 
    { "200", "33", "0" }, 
    { "201", "34", "0" },  
    { "202", "35", "0" }, 
    { "203", "36", "0" },
    { "210", "37", "0" },
    { "211", "38", "1" }, 
    { "212", "39", "2" },  
    { "213", "40", "2" }, 
    { "220", "41", "0" }, 
    { "221", "42", "1" }, 
    { "222", "43", "2" },  
    { "223", "44", "2" }, 
    { "230", "45", "0" },
    { "231", "46", "2" },
    { "232", "47", "3" },
    { "233", "48", "3" } 
}; 

scn getCode(char stat[10]) {
	int i=0;
    scn hoxy;
    sprintf(hoxy.stat, "%s", stat);
    sprintf(hoxy.code, "1");
    sprintf(hoxy.riskRate, "0");

	for(int j=0; j<48; j++){
		if((strcmp(stat, scnLookUpTable[j].stat)) == 0){
			return scnLookUpTable[j];
            break;
		}
	}
    return hoxy;  
}

scn getStat(char code[10]) {
	int i=0;
    scn hoxy;
    sprintf(hoxy.stat, "%s", code);
    sprintf(hoxy.stat, "999");
    sprintf(hoxy.riskRate, "99");

	for(int j=0; j<48; j++){
		if((strcmp(code, scnLookUpTable[j].code)) == 0){
			return scnLookUpTable[j];
            break;
		}
	}
    return hoxy;  
}


void sigGen( actSig *shmB, statSig *newStat, int scnGen) {
    
    char *stat = (char * )malloc(10);
    switch(scnGen)
    {
    case 0:
        sprintf(shmB->pole0.speed, "%s", newStat->pole0Speed);    
        sprintf(shmB->pole1.speed, "%s", newStat->pole1Speed);   
        usleep(GEN_CYCLE);
        break;
    case 1:
    // switch 문에서 변수 선언할때 중괄호 없으면 컴파일 에러 나서 추가했습니다.
        {
        scn result = getCode(newStat->poleStat0);
        sprintf(shmB->pole0.scnCode, "%s", result.code);
        sprintf(shmB->pole0.riskRate, "%s", result.riskRate);
        sprintf(shmB->pole0.speed, "%s", newStat->pole0Speed);
        sprintf(shmB->pole1.speed, "%s", newStat->pole1Speed);

        //printf("This is code: %s %s\n", result.code, result.riskRate);
        usleep(GEN_CYCLE);
        break;
        }
    case 2:
        {
        scn result = getCode(newStat->poleStat1);
        sprintf(shmB->pole1.scnCode, "%s", result.code);
        sprintf(shmB->pole1.riskRate, "%s", result.riskRate);
        sprintf(shmB->pole0.speed, "%s", newStat->pole0Speed);
        sprintf(shmB->pole1.speed, "%s", newStat->pole1Speed);

        //printf("This is code: %s %s\n", result.code, result.riskRate);
        usleep(GEN_CYCLE);
        break;
        }
    case 3:
        {
        scn result_0 = getCode(newStat->poleStat0);
        scn result_1 = getCode(newStat->poleStat1);
        sprintf(shmB->pole0.scnCode, "%s", result_0.code);
        sprintf(shmB->pole0.riskRate, "%s", result_0.riskRate);
        sprintf(shmB->pole1.scnCode, "%s", result_1.code);
        sprintf(shmB->pole1.riskRate, "%s", result_1.riskRate);
        sprintf(shmB->pole0.speed, "%s", newStat->pole0Speed);
        sprintf(shmB->pole1.speed, "%s", newStat->pole1Speed);

        //printf("This is code0: %s %s\n", result_0.code, result_0.riskRate);
        //printf("This is code1: %s %s\n", result_1.code, result_1.riskRate);
        usleep(GEN_CYCLE);
        break;
        }
    }
}


void *counter(void *Args){
    cntArgs *args = (cntArgs *)Args;
    // original status and new fetched status
    printf("This is counter \n");
    statSig *orgStat = args-> orgStat;
    statSig *newStat = args-> newStat;
    recvSig *shmA = args->shmA;
    actSig *shmB = args->shmB;

    int ped0 = 0;
    int ped1 = 0;
    int ped = 0;
    int i = 0;
    
    while(1){
        ped0 = atoi(shmA->ped_0);
        ped1 = atoi(shmA->ped_1);
        if(ped0 >= ped1) { 
            ped = ped0;
        }else{
            ped = ped1;
        }

        if(i == 0){
            sprintf(newStat->poleStat0, "%s%s%d", shmA->traffic, shmA->car_0, ped);
            sprintf(newStat->pole0Speed, "%s", shmA->car_0_speed);
            sprintf(newStat->poleStat1, "%s%s%d", shmA->traffic, shmA->car_1, ped);
            sprintf(newStat->pole1Speed, "%s", shmA->car_1_speed);
            i++;
            sigGen(args->shmB, newStat, 3);
            sprintf(orgStat->poleStat0, "%s", newStat->poleStat0);
            sprintf(orgStat->pole0Speed, "%s", newStat->pole0Speed);
            sprintf(orgStat->poleStat1, "%s", newStat->poleStat1);
            sprintf(orgStat->pole0Speed, "%s", newStat->pole0Speed);
        }else{
            // Signal compare
            sprintf(newStat->poleStat0, "%s%s%d", shmA->traffic, shmA->car_0, ped);
            sprintf(newStat->pole0Speed, "%s", shmA->car_0_speed);
            sprintf(newStat->poleStat1, "%s%s%d", shmA->traffic, shmA->car_1, ped);
            sprintf(newStat->pole1Speed, "%s", shmA->car_1_speed);

            if(strcmp(newStat->poleStat0, orgStat->poleStat0) == 0){
                if(strcmp(newStat->poleStat1, orgStat->poleStat1) == 0){
                    // pole 0 and pole 1 nothin changed
                    sigGen(shmB, newStat, 0);
                    usleep(CNT_CYCLE);
                }else{
                    // pole 1 changed
                    sigGen(shmB, newStat, 2);
                    sprintf(orgStat->poleStat1, "%s", newStat->poleStat1);
                    usleep(CNT_CYCLE);
                }
            }else{
                if(strcmp(newStat->poleStat1, orgStat->poleStat1) == 0){
                    // pole 0 changed
                    sigGen(shmB, newStat, 1);
                    sprintf(orgStat->poleStat0, "%s", newStat->poleStat0);
                    usleep(CNT_CYCLE);
                }else{
                    // pole 0 and pole 1 changed
                    sigGen(shmB, newStat, 3);
                    sprintf(orgStat->poleStat0, "%s", newStat->poleStat0);
                    sprintf(orgStat->poleStat1, "%s", newStat->poleStat1);
                    usleep(CNT_CYCLE);
                }
            }
        }
    }
    return (void *)0;
}

void * sql_connection (void * s){


    MYSQL *connection = NULL, conn;
    MYSQL_RES *sql_result;
    MYSQL_ROW sql_row;
    int query_stat;


    mysql_init(&conn);
    
    // mysql_ssl_set() 사용 하여 인증서 연결
    // int mysql_ssl_set(MYSQL *mysql,
    // const char *key, const char *cert, const char *ca, const char *capath, const char *cipher)
    // key 는 키 파일로의 경로이름이다.
    // cert 는 인증 파일로의 경로이름이다.
    // ca 는 인증 권한 파일로의 경로이름이다.
    // capath 는 pem 포맷에서 SSL CA 인증을 포함하는 디렉토리로의 경로이름이다.
    // cipher 는 SSL 암호화에 사용하기 위해 허용된 암호 리스트이다.
    // 사용되지 않는 SSL 파라미터들은 NULL로 주어질 수 있다.

    mysql_ssl_set(&conn, "/home/nvidia/Desktop/head_edge/csj/HeadEdge/certs/mysql-client-key.pem", "/home/nvidia/Desktop/head_edge/csj/HeadEdge/certs/mysql-client-cert.pem", "/home/nvidia/Desktop/head_edge/csj/HeadEdge/certs/mysql-ca.pem", NULL, NULL);
    my_bool reconnect = 1;
    mysql_options(&conn, MYSQL_OPT_RECONNECT, &reconnect);

    connection = mysql_real_connect(&conn, "globalbridge.iptime.org",
                                    "EDGE", "gba!l0OedgE",
                                    "gbsz100db", 3306,
                                    (char *)NULL, 0);
    if (connection == NULL)
    {
        printf("Mysql connection error : %s\n", mysql_error(&conn));
        exit(103);
    }


    printf("This is sql_connection function \n");
    actSig * current_scnario = (actSig *) s;
    
    while(1){
        char query[3000];
        for (int i =8; i<10; i++){
        int camID = i;
        // printf("camID: %d\n\n", camID);
        if (mysql_ping(&conn) != 0)
        {
            printf("mysql connection failed! Reconnect!");
        }

       
        // printf("before iligal car lock!!!\n");
        
        // start transaction
        mysql_query(connection, "START TRANSACTION;");
        

        // ##### 0. 이벤트 생성 #####
        sprintf(query,  "INSERT INTO events (active_YN, recorded_at) values ('Y', NOW());" );
        query_stat = mysql_query(connection, query);
        if (query_stat != 0){
            mysql_query(connection, "ROLLBACK;");
            printf("Mysql connection error1 : %s\n", mysql_error(&conn));
        }
        else{
                    
            // 위에서 만든 이벤트 아이디
            int event_id = mysql_insert_id(connection);
            // 2 이벤트랑 카메라 연결
            // camid 적어야함
            sprintf(query,  "INSERT INTO events_to_cameras (event_id, cam_id) VALUES (%d, %d)", event_id, camID);
            // printf("this is queyr %s\n", query);

            query_stat = mysql_query(connection, query);

            if (query_stat != 0){
                printf("Mysql query error2 : %s\n", mysql_error(&conn));
                mysql_query(connection, "ROLLBACK;");
            }

            else{
                
                // ##### 3. 알림 이벤트 생성 #####
                
                scn result = (camID == 8) ? getStat(current_scnario->pole0.scnCode): getStat(current_scnario->pole1.scnCode); 
                int crosswalk_signal = (int) result.stat[0];
                switch(crosswalk_signal){
                    case 48:{
                        crosswalk_signal = 51;
                        break;
                    }

                    case 49:{
                        crosswalk_signal = 54;
                        break;
                    }

                    case 50:{
                        crosswalk_signal = 49;
                        break;
                    }
                }
                

                sprintf(query, "INSERT INTO alarm_events (event_id, vehicle_event_type, pedestrian_event_type, crosswalk_signal, risk) VALUES (%d, '%c', '%c', '%c', %d) ", event_id,result.stat[1]+1,result.stat[2]+1,(char) crosswalk_signal,atoi(result.riskRate)); 
                // sprintf(query, "INSERT INTO alarm_events (event_id, vehicle_event_type, pedestrian_event_type, crosswalk_signal, risk) VALUES (%d, '%c', '%c', '%c', %c) ", event_id,result.stat[1]+1,result.stat[2]+1,'6','1'); 

                // printf("this is queyr %s\t%d\n", query,crosswalk_signal);
                query_stat = mysql_query(connection, query);
                if (query_stat != 0){
                                        printf("Mysql query error3 : %s\n", mysql_error(&conn));

                    mysql_query(connection, "ROLLBACK;");
                }
                else{
                    if (atoi(result.riskRate) >=2){
                    // printf("this is atoi %d\n",atoi(result.riskRate));
                        mysql_query(connection, "COMMIT;");
                    }
                    else{
                        mysql_query(connection, "ROLLBACK;");
                    }
                }
                //  event_id : 위에서 생성한 event_id
                //  vehicle_event_type : 1(차량 없음), 2(차량 정지), 3(차량 접근), 4(차량 과속)
                //  pedestrian_event_type : 1(보행자 없음), 2(보행자 대기), 3(보행자 횡단), 4(인도 이탈)
                //  crosswalk_signal : 보행자 신호(1: 적색점등, 3: 녹색점등, 6: 녹색점멸)
                //  risk : 0~3 시나리오 상황에 맞는 위험수치
            }
        }        
    }
    sleep(1);
    }
}

void gentleman(int shmId_A, int shmId_B) {
    
    recvSig *shmA;
    actSig *shmB;
    statSig *orgStat, *newStat;
    
    // malloc 추가
    newStat = (statSig * ) malloc(256);
    orgStat = (statSig * ) malloc(256);

    printf("process B intside into Gentleman\n");
    // attach share memory A
    void *shmAddrA;
    if((shmAddrA = shmat(shmId_A, (void *)0, 0)) == (void *)-1) {
        perror("Shmat failed");
        exit(0);
    }else{
        printf("gentleman, shm A complete\n");
    }
    shmA = (recvSig *)shmAddrA;
    
    // attach share memory B
    void *shmAddrB;
    if((shmAddrB = shmat(shmId_B, (void *)0, 0)) == (void *)-1) {
        perror("Shmat failed");
        exit(0);
    }else{
        printf("gentleman, shm B complete\n");
    }
    shmB = (actSig *)shmAddrB;
    

    
    scn old0 = getCode(orgStat->poleStat0);
    scn old1 = getCode(orgStat->poleStat1);

    scn new0 = getCode(newStat->poleStat0);
    scn new1 = getCode(newStat->poleStat1);
    
    int prev_max_rate, current_max_rate;
    current_max_rate = (atoi(old0.riskRate) > atoi(old1.riskRate)) ? atoi(old0.riskRate) : atoi(old1.riskRate);
    prev_max_rate = (atoi(new0.riskRate) > atoi(new1.riskRate)) ? atoi(new0.riskRate) : atoi(new1.riskRate);
    
    printf("this is gentleman before sql error rate %d\t %d\n", current_max_rate,prev_max_rate);
    
    
    
    // Counter //
    cntArgs counterArgs;
    pthread_t counterThread; 
    counterArgs.shmA = shmA;
    counterArgs.shmB = shmB;
    counterArgs.orgStat = orgStat;
    counterArgs.newStat = newStat;
    pthread_create(&counterThread, NULL, counter, (void *)&counterArgs);
    

    pthread_t sql;
    // pthread_create(&sql, NULL, sql_connection, (void *) shmB);
    // pthread_detach(sql); 
    
    
    // pthread_cancel(sql);
    pthread_create(&sql, NULL, sql_connection, (void *) shmB);
    pthread_detach(sql);

    while(1){
        printf("traffic: %s | car_0: %s | ped_0: %s | car_1: %s | ped_1 %s\n", shmA->traffic, shmA->car_0, shmA->ped_0, shmA->car_1, shmA->ped_1);
        printf("pole0Stat: %s | pole1Stat: %s\n", newStat->poleStat0, newStat->poleStat1);
        printf("pole0: %s | pole1: %s\n", shmB->pole0.scnCode, shmB->pole1.scnCode);
        printf("pole0 rate: %s | pole1 rate: %s\n", shmB->pole0.riskRate, shmB->pole1.riskRate);
        printf("pole0 speed: %s | pole1 speed: %s\n\n", shmB->pole0.speed, shmB->pole1.speed);
        sleep(1);
    }
    pthread_join(counterThread, NULL);

    

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