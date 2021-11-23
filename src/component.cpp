
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <queue>
#include <thread>
#include <mutex>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include<iostream>

using namespace std;
#include "VMS.h"
// #include "speaker.h"
#include "common.h"
#include "newspeaker.h"

VMS* vms_1 = new VMS(vms1, vms_port, 0,0);
VMS* vms_1_1 = new VMS(vms1_1, vms_port, 1,0);
VMS* vms_2 = new VMS(vms2, vms_port, 0,1);
speaker* speaker_ped1_1 = new speaker(speaker_ped1,SPEAKERPORT);
speaker* speaker_ped1_2 = new speaker(speaker_ped2,SPEAKERPORT);
speaker* speaker_car1_1 = new speaker(speaker_car1,SPEAKERPORT);
speaker* speaker_car1_2 = new speaker(speaker_car2,SPEAKERPORT);
speaker* speaker_movPed_1 = new speaker(speaker_movPed1,SPEAKERPORT);
speaker* speaker_movPed_2 = new speaker(speaker_movPed2,SPEAKERPORT);

/* Mysql headers */
//#include "/usr/include/mysql/mysql.h"

typedef struct car_stop_args{
    int stop_sec;
    const char * car_speaker_ip;
} car_stop_args;


typedef struct ped_stop_args{
    int stop_sec;
    int index;
} ped_stop_args;


static int ped_speaker_lookuptable[] = {
    //0은 안트는것 //음원없음은 일단 13으로 통일
    0,
    16,
    16,
    16,
    0,
    16,
    16,
    16,
    0,
    17,
    17,
    17,
    0,
    18,
    18,
    18,
    0,
    19,
    19,
    19,
    0,
    20,
    20,
    20,
    0,
    21,
    21,
    21,
    0,
    22,
    22,
    22,
    0,
    23,
    23,
    23,
    0,
    23,
    24,
    24,
    0,
    25,
    26,
    26,
    0,
    27,
    28,
    28
    };

int car_speaker_lookuptable[] = {
    // 0은 안트는것
    // %16
    0,0,0,0,0,0,14,14,0,0,14,14,15,15,15,15
};

int speaker_stop_lookuptable[] = {
    //0은 안트는것 //음원없음은 일단 13으로 통일, 19번쨰 음원 추가 + % 변경
    // %16
    0,4,4,4,4,3,3,5,6,5,5,5,6,2,2,1,3,4,4,2,3,2,3,3,3,4,4,4,4,3,3,4,4
    };

//2번 부터 스피드 필요
int vms_function_lookuptable[] = {
    0,0,1,1,0,0,1,1,2,2,3,3,4,4,5,5
    };    

void carspeaker_stopper(speaker* speaker){
    
    
    // printf("this is car speaker stopper ip %s\n", car_speaker_ip);
    speaker->control("volume", min_volume);
    // speaker_control(car_speaker_ip, "volume", min_volume);
    
}


void pedspeaker_stopper(int spk_index){

    
    // printf("this is ped speaker stopper \n");
    switch(spk_index){
        case 17:
            speaker_ped1_1->control("index_play", 29);
            speaker_ped1_2->control("index_play", 29);
            speaker_movPed_1->control("index_play", 29);
            speaker_movPed_2->control("index_play", 29);
            // speaker_control(speaker_ped1,"index_play",22);
            // speaker_control(speaker_movPed1,"index_play",22);
            // speaker_control(speaker_ped2,"index_play",22);
            // speaker_control(speaker_movPed2,"index_play",22);
            break;
        case 23:
            speaker_ped1_1->control("index_play", 30);
            speaker_ped1_2->control("index_play", 30);
            speaker_movPed_1->control("index_play", 30);
            speaker_movPed_2->control("index_play", 30);
            // speaker_control(speaker_ped1,"index_play",6);
            // speaker_control(speaker_movPed1,"index_play",6);
            // speaker_control(speaker_ped2,"index_play",6);
            // speaker_control(speaker_movPed2,"index_play",6);
            break;
        case 25:
            speaker_ped1_1->control("index_play", 31);
            speaker_ped1_2->control("index_play", 31);
            speaker_movPed_1->control("index_play", 31);
            speaker_movPed_2->control("index_play", 31);
            // speaker_control(speaker_ped1,"index_play",29);
            // speaker_control(speaker_movPed1,"index_play",29);
            // speaker_control(speaker_ped2,"index_play",29);
            // speaker_control(speaker_movPed2,"index_play",29);
            break;
        case 26:
            speaker_ped1_1->control("index_play", 29);
            speaker_ped1_2->control("index_play", 29);
            speaker_movPed_1->control("index_play", 29);
            speaker_movPed_2->control("index_play", 29);
            // speaker_control(speaker_ped1,"index_play",30);
            // speaker_control(speaker_movPed1,"index_play",30);
            // speaker_control(speaker_ped2,"index_play",30);
            // speaker_control(speaker_movPed2,"index_play",30);
            break;
        case 28:
            speaker_ped1_1->control("index_play", 32);
            speaker_ped1_2->control("index_play", 32);
            speaker_movPed_1->control("index_play", 32);
            speaker_movPed_2->control("index_play", 32);
            // speaker_control(speaker_ped1,"index_play",21);
            // speaker_control(speaker_movPed1,"index_play",21);
            // speaker_control(speaker_ped2,"index_play",21);
            // speaker_control(speaker_movPed2,"index_play",21);
            break;
        case 29:
            // speaker_ped1_1->control("index_play", 31);
            // speaker_ped1_2->control("index_play", 31);
            // speaker_movPed_1->control("index_play", 31);
            // speaker_movPed_2->control("index_play", 31);
            // speaker_control(speaker_ped1,"index_play",31);
            // speaker_control(speaker_movPed1,"index_play",31);
            // speaker_control(speaker_ped2,"index_play",31);
            // speaker_control(speaker_movPed2,"index_play",31);
            break;
        
        
        default:
            speaker_ped1_1->control("volume", min_volume);
            speaker_ped1_2->control("volume", min_volume);
            speaker_movPed_1->control("volume", min_volume);
            speaker_movPed_2->control("volume", min_volume);
            // speaker_control(speaker_ped1, "volume", min_volume);
            // speaker_control(speaker_movPed1, "volume", min_volume);
            // speaker_control(speaker_ped2, "volume", min_volume);
            // speaker_control(speaker_movPed2, "volume", min_volume);
            break;
    }
    int ret;
    pthread_exit(&ret);
    }
    


void * ped_control(void * scn){
    
    scenario * current_scnario = (scenario *) scn;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    int scnario_num = atoi(current_scnario->scnCode);
    int ped_spk_index = ped_speaker_lookuptable[scnario_num-1]-1;
    int ped_spekaer_stop_sec = speaker_stop_lookuptable[ped_spk_index];
    printf("ped spk controller %d\n", ped_spk_index);
    if (ped_spk_index>=0){
        
        // printf("This is ped_control if and this is ped_spk_index %d\n", ped_spk_index);
        speaker_ped1_1->control("index_play", ped_spk_index);
        speaker_ped1_2->control("index_play", ped_spk_index);
        speaker_movPed_1->control("index_play", ped_spk_index);
        speaker_movPed_2->control("index_play", ped_spk_index);
   
        speaker_ped1_1->control("volume", max_volume);
        speaker_ped1_2->control("volume", max_volume);        
        speaker_movPed_1->control("volume", max_volume);
        speaker_movPed_2->control("volume", max_volume);

        // speaker_control(speaker_ped1, "volume", max_volume);
        // speaker_control(speaker_movPed1, "volume", max_volume);
        // speaker_control(speaker_ped2, "volume", max_volume);
        // speaker_control(speaker_movPed2, "volume", max_volume);

        // sleep(1);
        
        // speaker_control(speaker_ped1,"index_play",ped_spk_index);
        // speaker_control(speaker_movPed1,"index_play",ped_spk_index);
        // speaker_control(speaker_ped2,"index_play",ped_spk_index);
        // speaker_control(speaker_movPed2,"index_play",ped_spk_index);


        }

    else {
   
        //  printf("This is ped_control else \n");
        speaker_ped1_1->control("volume", min_volume);
        speaker_ped1_2->control("volume", min_volume);        
        speaker_movPed_1->control("volume", min_volume);
        speaker_movPed_2->control("volume", min_volume);
        }
  if (ped_spk_index>=0){
        printf("ped speaker stop stec :%d \n", ped_spekaer_stop_sec);

        sleep(ped_spekaer_stop_sec*2);
        pedspeaker_stopper(ped_spk_index);
    }
    

}

void vms_control(VMS* vms ,int function_num, int car_speed){
    // printf("this is vms function num %d\n", function_num);
    switch (function_num){
        case 0:
            vms->onNormal();
            break;
        case 1:
            vms->onPerson();
            break;
        case 2:
            vms->onCar();
            break;
        case 3:
            vms->onCarandPerson();
            break;
        case 4:
            vms->onSpeeding();
            break;
        case 5:
            vms->onSpeedingandPerson();
            break;
    }
    
}

void component_init(void * addr ){
       
    vms_1->setAddress(addr);
    vms_2->setAddress(addr);
    vms_1_1->setAddress(addr);
    vms_1_1->initialize();
    vms_1->initialize();
    vms_2 ->initialize();
    speaker_ped1_1->init(); 
    speaker_ped1_2->init(); 
    speaker_car1_1->init(); 
    speaker_car1_2->init(); 
    speaker_movPed_1->init();
    speaker_movPed_2->init();
    
    
    speaker_ped1_1->control("play", 0);
    speaker_ped1_2->control("play", 0);
    speaker_movPed_1->control("play", 0);
    speaker_movPed_2->control("play", 0);
    speaker_car1_1->control("play", 0);
    speaker_car1_2->control("play", 0);
    
    speaker_ped1_1->control("volume", min_volume);
    speaker_ped1_2->control("volume", min_volume);
    speaker_movPed_1->control("volume", min_volume);
    speaker_movPed_2->control("volume", min_volume);
    speaker_car1_1->control("volume", min_volume);
    speaker_car1_2->control("volume", min_volume);
    
    
    
    // speaker_control(speaker_ped1, "ping", 0);
    // speaker_control(speaker_movPed1, "ping", 0);
    // speaker_control(speaker_ped2, "ping", 0);
    // speaker_control(speaker_movPed2, "ping", 0);
    // speaker_control(speaker_car1, "ping", 0);
    // speaker_control(speaker_car2, "ping", 0);

    
    
    
    // speaker_control(speaker_ped1, "play", 0);
    // speaker_control(speaker_movPed1, "play", 0);
    // speaker_control(speaker_ped2, "play", 0);
    // speaker_control(speaker_movPed2, "play", 0);
    // speaker_control(speaker_car1, "play", 0);
    // speaker_control(speaker_car2, "play", 0);


}

void * component_controller(void * scn){    
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    scenario * current_scnario = (scenario *) scn;

    int scnario_num = atoi(current_scnario->scnCode);
    int car_speed = atoi(current_scnario->speed);
    int car_spk_index = car_speaker_lookuptable[(scnario_num-1)% 16]-1;
    int vms_index = vms_function_lookuptable[(scnario_num-1)% 16];
    int pol_num = atoi(current_scnario->pol_num);
    // int car_spekaer_stop_sec = speaker_stop_lookuptable[car_spk_index];
    printf("car spk index : %d\n", car_spk_index);
    switch(pol_num){
        
        case 0:{
            // printf("pol num 0, scnario num : %d car spk index : %d vms index : %d, \n", scnario_num ,car_spk_index, vms_index);
            vms_control(vms_1,vms_index,car_speed);
            vms_control(vms_1_1,vms_index,car_speed);
            
            // sleep(1);
            if (car_spk_index >=0){

                speaker_car1_1->control("volume", max_volume);
                speaker_car1_1->control("index_play", car_spk_index);
                
                // speaker_control(speaker_car1, "volume", max_volume);

              
                // speaker_control(speaker_car1, "index_play", car_spk_index);
            }
            else{
                carspeaker_stopper(speaker_car1_1);
            }

            
 
            
            break;
        }
        case 1:{
            // printf("pol num 1, scnario num : %d car spk index : %d vms index : %d, \n", scnario_num ,car_spk_index, vms_index);
            // sleep(1);
            vms_control(vms_2,vms_index,car_speed);
            if (car_spk_index >=0){
                speaker_car1_2->control("volume", max_volume);
                speaker_car1_2->control("index_play", car_spk_index);
            } 
            else{
                    carspeaker_stopper(speaker_car1_2);
            }
         

            break;
        }

    }

    

}

void stop_controller(char * scnario_stat0, char * scnario_stat1, int main_flag )
{
    int scnario_numA = atoi(scnario_stat0);
    int scnario_numB = atoi(scnario_stat1);
    int car_spk_indexA = car_speaker_lookuptable[(scnario_numA-1)% 16 ];
    int car_spk_indexB = car_speaker_lookuptable[(scnario_numB-1)% 16 ];
    int car_spekaer_stop_secA = speaker_stop_lookuptable[car_spk_indexA];
    int car_spekaer_stop_secB = speaker_stop_lookuptable[car_spk_indexB];
    int ped_spk_index; 

    switch(main_flag){
        case 0:{
            ped_spk_index = ped_speaker_lookuptable[scnario_numA-1];
            break;
        }
        case 1:{
            ped_spk_index = ped_speaker_lookuptable[scnario_numB-1];
            break;
        }
    }

    int ped_spekaer_stop_sec = speaker_stop_lookuptable[ped_spk_index];

    int stop_spk_flag = car_spekaer_stop_secA-car_spekaer_stop_secB;

    if (car_spekaer_stop_secA ==0) carspeaker_stopper(speaker_car1_1);
    if (car_spekaer_stop_secB ==0) carspeaker_stopper(speaker_car1_2);
    if (ped_spekaer_stop_sec>0){
        sleep(ped_spekaer_stop_sec);
    }

    pedspeaker_stopper(ped_spk_index);
    // sleep(0.05);
    // while (ped_spekaer_stop_sec>0) sleep(0.05);
    
    // printf("this is stopper ped_spk_index %d car_spk_inex A: %d car_spk_index b: %d, scnario_Num A %d, scnario_num B %d\n",ped_spk_index,car_spk_indexA, car_spk_indexB, scnario_numA,scnario_numB);

    // printf("stopper time check ped_spekaer_stop_sec : %d car_spekaer_stop_secA : %d car_spekaer_stop_secB : %d\n",ped_spekaer_stop_sec, car_spekaer_stop_secA, car_spekaer_stop_secA);
    // if (stop_spk_flag == 0){
    //     sleep(car_spekaer_stop_secA*2);
    //     if (car_spekaer_stop_secA>0){
            
    //         carspeaker_stopper(speaker_car1_1);
    //         carspeaker_stopper(speaker_car1_2);
    //     }

       
        
    //     sleep((ped_spekaer_stop_sec-car_spekaer_stop_secA)*2);
    //     // if (ped_spekaer_stop_sec >0)
    //     pedspeaker_stopper(ped_spk_index);
    // }

    // else if (stop_spk_flag<0){
    //     sleep(car_spekaer_stop_secA*2);
    //     if (car_spekaer_stop_secA>0){
    //         carspeaker_stopper(speaker_car1_1);
    //     }

    //     sleep((car_spekaer_stop_secB-car_spekaer_stop_secA)*2);
    //     carspeaker_stopper(speaker_car1_2);
    //     sleep((ped_spekaer_stop_sec-car_spekaer_stop_secB)*2);
    //     // if (ped_spekaer_stop_sec >0)
    //     pedspeaker_stopper(ped_spk_index);
    // }

    // else {
    //     printf("car_spekaer stop sec : %d\n", car_spekaer_stop_secB);
    //     sleep(car_spekaer_stop_secB*2);
    //     // if (car_spekaer_stop_secB>0){
    //     carspeaker_stopper(speaker_car1_2);
    //     // }

    //     sleep((car_spekaer_stop_secA-car_spekaer_stop_secB)*2);
    //     carspeaker_stopper(speaker_car1_1);
    
    //     sleep((ped_spekaer_stop_sec-car_spekaer_stop_secA)*2);
    //     // if (ped_spekaer_stop_sec >0)
    //     pedspeaker_stopper(ped_spk_index);
    // }



}


// void * decide_main_function(void * p){
//     actSig * current_scnario = (actSig *) p;
//     pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//     pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    
//     int pol0_riskRate = atoi(current_scnario->pole0.riskRate);
//     int pol1_riskRate = atoi(current_scnario->pole1.riskRate);
//     if (pol0_riskRate>pol1_riskRate){
//         component_controller(current_scnario->pole0.scnCode,current_scnario->pole0.speed,1,0);
//         component_controller(current_scnario->pole1.scnCode,current_scnario->pole1.speed,0,1);
//         stop_controller(current_scnario->pole0.scnCode, current_scnario->pole1.scnCode, 0);
//     }

//     else {
//         component_controller(current_scnario->pole0.scnCode,current_scnario->pole0.speed,0,0);
//         component_controller(current_scnario->pole1.scnCode,current_scnario->pole1.speed,1,1);
//         stop_controller(current_scnario->pole0.scnCode, current_scnario->pole1.scnCode, 1);
//     }

//     return 0;
// }


scenario main_scn (scenario a, scenario b){
    // printf("b.riskRate %s\n", b.riskRate);
    if (atoi(a.riskRate) > atoi(b.riskRate)) return a;
    else return b; 
}