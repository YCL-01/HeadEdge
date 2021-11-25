#include <iostream>
#include <stdio.h>
#include <chrono>
#include <vector>

// Thread
#include "Error.h"
#include "BaseThread.h"
#include "VMS.h"

#include <locale>
#include <codecvt>

// TCP
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include "common.h"
uint8_t ON[] = {0x02, 0x86, 0x0A, 0x00, 0x50, 0x00, 0x4F,
                0x00, 0x57, 0x00, 0x3D, 0x00, 0x31, 0x00, 0xF6, 0x03};

uint8_t OFF[] = {0x02, 0x86, 0x0A, 0x00, 0x50, 0x00, 0x4F,
                 0x00, 0x57, 0x00, 0x3D, 0x00, 0x30, 0x00, 0xF5, 0x03};

const int TIMER = 3;

bool VMS::onEventChanged(VMS_CODE code)
{

    /* For all event, after 3~5 seconds, VMS must return to the default stage. */

    if (code == VE0S0P0)
    {

        printf("Hello World\n");
    }
    else if (code == VE0S0P1)
    {
        printf("Another World\n");
    }
    else if (code == VE1S1P0)
    {
        printf("New World\n");
    }
    else if (code == VE1S1P1)
    {
        printf("No World\n");
    }
    else
    {
        printf("Lotte World\n");
    }

    return true;
};

bool VMS::sendMsg()
{

    std::u16string msg;
    std::vector<int> data;

    // For each scenario, there's a corresponding msg
    if (state == NORMAL)
    {
        if (is_smart_poll)
        {
            // msg = u"EVT=1,RCT=0.0.192.96,USI=015,EFF=090009000900";
            msg = normal_state ? u"EVT=1,RCT=0.0.192.96,USI=012,sEFF=090009000900"
                               : u"EVT=1,RCT=0.0.192.96,USI=013,EFF=090009000900";

            if (normal_count-- <= 0)
            {
                normal_state = !normal_state;
                normal_count = 1;
            }
        }
        else
        {
            //뒤에 Text 지워야함
            msg = normal_state ? u"EVT=1,RCT=0.0.192.96,USI=000,EFF=090009000900"
                               : u"EVT=1,RCT=0.0.192.96,USI=001,EFF=090009000900";

            if (normal_count-- <= 0)
            {
                normal_state = !normal_state;
                normal_count = TIMER;
            }
        }
    }

    else if (state == PERSON)
    {
        if (is_smart_poll)

        {    msg = normal_state ? u"EVT=1,RCT=0.0.192.96,USI=014,EFF=090009000900"
                               : u"EVT=1,RCT=0.0.192.96,USI=015,EFF=090009000900";
            if (normal_count-- <= 0)
            {
                normal_state = !normal_state;
                normal_count = TIMER;
            }
        }
        else
        {
            msg = normal_state ? u"EVT=1,RCT=0.0.192.96,USI=002,EFF=090009000900"
                               : u"EVT=1,RCT=0.0.192.96,USI=003,EFF=090009000900";

            if (normal_count-- <= 0)
            {
                normal_state = !normal_state;
                normal_count = TIMER;
            }
        }
    }
    else if (state == CAR)
    {
        if (is_smart_poll)
        {
            msg = normal_state ? u"EVT=1,RCT=0.0.192.96,USI=012,EFF=090009000900"
                               : u"EVT=1,RCT=0.0.192.96,USI=013,EFF=090009000900";


            if (normal_count-- <= 0)
            {
                normal_state = !normal_state;
                normal_count = TIMER;
            }
        }
        else
        {
            msg = normal_state ? u"EVT=1,RCT=0.0.192.96,USI=004,EFF=090009000900,RCT=65.10.40.40,TXT=$f02$c03<SPEED> "
                               : u"EVT=1,RCT=0.0.192.96,USI=005,EFF=090009000900,RCT=65.10.40.40,TXT=$f02$c03<SPEED> ";

            if (normal_count-- <= 0)
            {
                normal_state = !normal_state;
                normal_count = TIMER;
            }
        }
    }
    else if (state == CARANDPERSON)
    {
        if (is_smart_poll)
        {
           msg = normal_state ? u"EVT=1,RCT=0.0.192.96,USI=014,EFF=090009000900"
                               : u"EVT=1,RCT=0.0.192.96,USI=015,EFF=090009000900";

            if (normal_count-- <= 0)
            {
                normal_state = !normal_state;
                normal_count = TIMER;
            }
        }
        else
        {
            msg = normal_state ? u"EVT=1,RCT=0.0.192.96,USI=006,EFF=090009000900,RCT=65.10.40.40,TXT=$f02$c03<SPEED> "
                               : u"EVT=1,RCT=0.0.192.96,USI=007,EFF=090009000900,RCT=65.10.40.40,TXT=$f02$c03<SPEED> ";

            if (normal_count-- <= 0)
            {
                normal_state = !normal_state;
                normal_count = TIMER;
            }
        }
    }

    else if (state == SPEEDING)
    {
        if (is_smart_poll)
        {
           msg = normal_state ? u"EVT=1,RCT=0.0.192.96,USI=012,EFF=090009000900"
                               : u"EVT=1,RCT=0.0.192.96,USI=013,EFF=090009000900";

            if (normal_count-- <= 0)
            {
                normal_state = !normal_state;
                normal_count = TIMER;
            }
        }
        else
        {
            msg = normal_state ? u"EVT=1,RCT=0.0.192.96,USI=008,EFF=090009000900,RCT=65.10.40.40,TXT=$f02$c03<SPEED> "
                               : u"EVT=1,RCT=0.0.192.96,USI=009,EFF=090009000900,RCT=65.10.40.40,TXT=$f02$c03<SPEED> ";

            if (normal_count-- <= 0)
            {
                normal_state = !normal_state;
                normal_count = TIMER;
            }
        }
    }

    else if (state == SPEEDINGANDPERSON)
    {
        if (is_smart_poll)
        {
            msg = normal_state ? u"EVT=1,RCT=0.0.192.96,USI=014,EFF=090009000900"
                               : u"EVT=1,RCT=0.0.192.96,USI=015,EFF=090009000900";

            if (normal_count-- <= 0)
            {
                normal_state = !normal_state;
                normal_count = TIMER;
            }
        }
        else
        {
            msg = normal_state ? u"EVT=1,RCT=0.0.192.96,USI=010,EFF=090009000900,RCT=65.10.40.40,TXT=$f02$c03<SPEED> "
                               : u"EVT=1,RCT=0.0.192.96,USI=011,EFF=090009000900,RCT=65.10.40.40,TXT=$f02$c03<SPEED> ";

            //strconcat(msg,this->speed);
            if (normal_count-- <= 0)
            {
                normal_state = !normal_state;
                normal_count = TIMER;
            }
        }
    }

    // For states that require speed information,
    if (state == CAR ||
        state == CARANDPERSON ||
        state == SPEEDING ||
        state == SPEEDINGANDPERSON)
    {
        int i = msg.find(u"<SPEED>");

        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        actSig * shmb = (actSig *) this->addr;
        std::u16string num;
        // int -> string -> u16string
        if (!is_smart_poll && i != std::u16string::npos){

        
            if (pol_num == 0){
                num = convert.from_bytes(shmb->pole0.speed);
                // printf("this is vms pol 0 shm memroy speed %s\n", shmb->pole0.speed);

            }

            else {
                num = convert.from_bytes(shmb->pole1.speed);
                // printf("this is vms pol 1 shm memroy speed %s\n", shmb->pole1.speed);

            }
            

            msg.replace(i, 7, num);
        }
    }

    // Each data takes two bytes
    int len = msg.length() * 2;
    int checksum = 0;
    int dataSum = 0;

    // Convert the msg to uint8 byte arr

    for (char16_t const &c : msg)
    {
        //printf("%c", c);
        int num = c;
        int lower, upper;

        // 44032 == '가'
        if (num >= 44032)
        {
            lower = num % 256;
            upper = num / 256;
        }
        else
        {
            lower = num;
            upper = 0;
        }
        data.push_back(lower);
        data.push_back(upper);

        dataSum = dataSum + lower + upper;
    }

    checksum = STX + TYPE_INSERT + len + dataSum;
    checksum %= 256;

    int packet_len = 7 + len;

    unsigned char ret[packet_len];
    int idx = 0;
    ret[idx++] = STX;
    ret[idx++] = TYPE_INSERT;
    ret[idx++] = len % 256;
    ret[idx++] = len / 256;

    for (std::vector<int>::iterator itr = data.begin(); itr != data.end(); ++itr)
        ret[idx++] = *itr;

    ret[idx++] = checksum;
    ret[idx++] = ETX;
    //printf("packet_len:%d", packet_len);
    
    
   

    int result = send(VMS_SOCKET, ret, packet_len, 0);
    // int bytesReceived = 0;
    // std::vector<uint8_t> buffer(20);

    // bytesReceived = recv(VMS_SOCKET, &buffer[0], buffer.size(), 0);
    // printf("%02x %02x %02x %02x %02x %02x %02x\n", buffer.data()[0],buffer.data()[1], buffer.data()[2], buffer.data()[3], buffer.data()[4], buffer.data()[5], buffer.data()[6]);
    //printf("result %d", result);
    if (is_smart_poll){
        
        sleep(1);
    }

    else{
        usleep(500 * 1000);
    }
        

    return true;
}

bool VMS::threadInitialize()
{
    
    // Create and open port
    VMS_SOCKET = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(PORT);
    sockaddr.sin_addr.s_addr = inet_addr(IP);

    // If failed to connect,
    if (connect(VMS_SOCKET, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
    {
        printf(RED "[ERR] Failed to connect a socket!\n" RESET);
        return false;
    }

    printf(GRN "[MSG] VMS succesfully connected!\n" RESET);
    updated = std::chrono::system_clock::now();

    // VMS power on
    send(VMS_SOCKET, ON, 16, 0);

    return true;
}

bool VMS::threadExecute()
{

    std::chrono::system_clock::time_point now;
    now = std::chrono::system_clock::now();


    // How long the last msg has been sent
    std::chrono::duration<double> elapsed;
    elapsed = now-updated;
    if (is_fucked_up){
        if(elapsed.count() > 1800){
            updated = std::chrono::system_clock::now();
            send(VMS_SOCKET, OFF, 16, 0);
            sleep(5);
            send(VMS_SOCKET, ON, 16, 0);
            // sleep(1);
        }
    }
    sendMsg();

    return true;
}

bool VMS::threadShutdown()
{

    send(VMS_SOCKET, OFF, 16, 0);

    if (close(VMS_SOCKET))
    {
        printf(RED "[ERR]   Failed to close socket!\n" RESET);
        return false;
    }

    return true;
}


bool VMS::onNormal()
{
    if (state != NORMAL)
    {
        state = NORMAL;
        
    }

    // this->speed = speed;

    return true;
}



bool VMS::onSpeeding()
{

    //   Speeding state lasts for 3~5 seconds.
    //   If a new car is detected, then the timer restarts.

    if (state != SPEEDING)
    {
        state = SPEEDING;

        updated = std::chrono::system_clock::now();

        normal_state = true;
        normal_count = TIMER;
    }

    // this->speed = speed;

    return true;
}

bool VMS::onCar()
{
    if (state != CAR)
    {
        state = CAR;

        updated = std::chrono::system_clock::now();

        normal_state = true;
        normal_count = TIMER;
    }

    // this->speed = speed;

    return true;
}

bool VMS::onPerson()
{
    if (state != PERSON)
    {
        state = PERSON;

        updated = std::chrono::system_clock::now();

        normal_state = true;
        normal_count = TIMER;
    }

    return true;
}

bool VMS::onCarandPerson()
{
    if (state != CARANDPERSON)
    {
        state = CARANDPERSON;

        updated = std::chrono::system_clock::now();

        normal_state = true;
        normal_count = TIMER;
    }

    // this->speed = speed;

    return true;
}

bool VMS::onSpeedingandPerson()
{
    if (state != SPEEDINGANDPERSON)
    {
        state = SPEEDINGANDPERSON;

        updated = std::chrono::system_clock::now();

        normal_state = true;
        normal_count = TIMER;
    }
    // sharememroy speed
    // this->speed = speed;

    return true;
}

bool VMS::setAddress(void * addr){
    this->addr = addr;
}

VMS::~VMS()
{
}
