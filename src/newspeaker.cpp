
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



#include "newspeaker.h"
//speaker functions 

void hexdumpunformatted(void *ptr, int buflen)
{
	unsigned char *buf = (unsigned char *)ptr;
	int i, j;
	for (i = 0; i < buflen; i++)
	{
		printf("%02x ", buf[i]);
		//printf("%c ", buf[i]);
		printf(" ");
	}
}




uint8_t * speaker_tcp_Protocol(string Name, int Value =0)
{
	uint8_t *command = new uint8_t[5];
	uint8_t name;

	command[0] = 0xAA;
	command[1] = 0x00;
	//핑테스트
	if (Name == "ping" && Value == 0)
		name = 0x60;
	//이전곡 재생
	if (Name == "prev")
		name = 0x42;
	// 다음곡 재생
	if (Name == "next"){
		name = 0x42;
		Value = 1;
	}
	// 볼륨 컨트롤
	if (Name == "volume" && (Value > -1 && Value < 101))
		name = 0x50;
	// 정지
	if (Name == "stop")
		name = 0x52;
	// 재생
	if (Name == "play"){
		name = 0x52;
		Value = 1;
	}
	//현재 곡 반복
	if (Name == "single_loop")
		name = 0x54;
	//전체 반복
	if (Name == "whole_loop"){
		name = 0x54;
		Value = 1;
	}
	// 몃번째 곡 재생
	if (Name == "index_play" && (Value > -1 || Value < 101))
		name = 0x56;
	// 파일스캔
	if (Name == "file_scan")
		name = 0x58;
	// 현재 볼륨 확인
	if (Name == "current_volume")
		name = 0x70;
	// 현재 재생파일 확인
	if (Name == "current_file")
		name = 0x72;
	// 현재 루프 확인
	if (Name == "current_loop")
		name = 0x74;
	// 현재 재생 중인지 확인
	if (Name == "current_play_status")
		name = 0x76;
	// 현재 음원 리스트 확인
	if (Name == "current_play_list")
		name = 0x78;
	// 현재 음원 리스트 확인
	if (Name == "current_play_list_line")
		name = 0x80;

	uint8_t checksum = name + Value;
	command[2] = name;
	command[3] = Value;
	command[4] = checksum;
	return command;
}


uint8_t *pantilt_tcp_Protocol_direction(string Name, int speed=0)
{
	uint8_t *command = new uint8_t[8];
	uint8_t name;
    int panspeed, tiltspeed;
	if (Name == "up" )
		name = 0x08;

	if (Name == "down" )
		name = 0x10;

	if (Name == "left" )
		name = 0x04;

	if (Name == "right")
		name = 0x02;

	if (Name == "upleft")
		name = 0x0C;

	if (Name == "upright" )
		name = 0x0A;

	if (Name == "downleft")
		name = 0x14;

	if (Name == "downright")
		name = 0x12;

uint8_t checksum = 0xA0^0x00^0x00^name^panspeed^tiltspeed^0xAF;
	panspeed = speed;
    tiltspeed = speed;
    printf(" panspeed %d, tiltspeed%d\n", panspeed,tiltspeed);
    command[0] = 0xA0;
	command[1] = 0x00;
    command[2] = 0x00;
	command[3] = name;
	command[4] = panspeed;
	command[5] = tiltspeed;
    command[6] = 0xAF;
    command[7] = checksum;

	return command;
}

uint8_t *pantilt_tcp_Protocol_coordinate(string Name, int position=0 )
{
	uint8_t *command = new uint8_t[8];
	uint8_t name;
	int high = 0;
	int low  = 0;

	if (Name == "set_pan_position" && position >=0){
		name = 0x4B;
        high = (position >>8) & 0xff;
        low = position & 0xff;
}

	if (Name == "set_tilt_position" && position >= 0){
		name = 0x4D;
        high = (position >>8) & 0xff;
        low = position & 0xff;
}
    if (Name == "query_pan_position")
        name = 0x51;
    
    if (Name == "query_tilt_position")
        name = 0x53;

	uint8_t checksum = 0xA0^0x00^0x00^name^high^low^0xAF;
	printf("this is position : %d\n",position );
    
    command[0] = 0xA0;
	command[1] = 0x00;
    command[2] = 0x00;
	command[3] = name;
	command[4] = high;
	command[5] = low;
    command[6] = 0xAF;
    command[7] = checksum;

	return command;
}

void speaker_control (const char * speaker_ip, string option, int value=0 )
{
	uint8_t * command;
	command = speaker_tcp_Protocol(option, value);
	int sock_cli = socket(AF_INET, SOCK_STREAM, 0);
	/// Define sockaddr_in
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SPEAKERPORT);					   /// Server Port
	servaddr.sin_addr.s_addr = inet_addr(speaker_ip); /// server ip

    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("connect");
		exit(1);
	}

	int n;
    n = send(sock_cli, command, 8, 0);
	
		if (n < 0)
		{
			cerr << "Error writing to socket!" << strerror(errno) << endl;
			close(sock_cli);
			printf("Error");
		}
		
	int bytesReceived = 0;
	std::vector<uint8_t> buffer(100);
	if (option == "volume"){
		
		close(sock_cli);
	}

	else if (option == "current_file"){
		int bytesReceived = 0;
		bytesReceived = recv(sock_cli, &buffer[0], buffer.size(), 0); /// Receiving
		for (int i =3; i<buffer.data()[1]-1;i++){
				printf("%c", buffer.data()[i]);
				printf(" ");
			}
			
			printf("\n");
		close(sock_cli);
	}

	else if (option == "current_play_list"){
		int bytesReceived = 0;
		
		bytesReceived = recv(sock_cli, &buffer[0], buffer.size(), 0); /// Receiving
		printf("this is data protocol %02x %02x %02x %02x %02x\n",buffer.data()[0],buffer.data()[1], buffer.data()[2], buffer.data()[3], buffer.data()[4]);
		do{
			//printf("check");
			command = speaker_tcp_Protocol("current_play_list_line");
			n = send(sock_cli, command, 8, 0);
			if (n < 0)
			{
				cerr << "Error writing to socket!" << strerror(errno) << endl;
				close(sock_cli);
				
				
			}	
			bytesReceived = recv(sock_cli, &buffer[0], buffer.size(), 0); /// Receiving
			//printf("this is data_length %d", buffer.data()[1]);

			// char temp[buffer.data()[1]-1] = "";


			for (int i =3; i<buffer.data()[1]-1;i++){
				printf("%c", buffer.data()[i]);
				printf(" ");
			}
			
			printf("\n");
			
			
		}
		while(buffer.data()[2]!=0x80);
		close(sock_cli);
	}

	else if (option == "current_volume"){
		int bytesReceived = 0;
		std::vector<uint8_t> buffer(20);

		bytesReceived = recv(sock_cli, &buffer[0], buffer.size(), 0); /// Receiving
		printf("bytesReceived : %d\n", bytesReceived);
		//printf("check\n");	
		printf("this is current_volume %d \n",buffer.data()[3]);
		
		close(sock_cli);
	}

	else if (option == "play" || option == "stop" || option == "single_loop" || option == "while_loop" || option == "file_scan" || option == "index_play"){
		close(sock_cli);
		printf("order successfully executed\n");
	}

	else{

		int bytesReceived = 0;
		std::vector<uint8_t> buffer(20);
		bytesReceived = recv(sock_cli, &buffer[0], buffer.size(), 0); /// Receiving
		printf("bytesReceived : %d\n", bytesReceived);
		//printf("check\n");	
		printf("this is data protocol %02x %02x %02x %02x %02x\n",buffer.data()[0],buffer.data()[1], buffer.data()[2], buffer.data()[3], buffer.data()[4]);
		close(sock_cli);
	}
	
}
void speaker_direction (const char * speaker_ip, string direction)
{
	uint8_t * command;
	command = pantilt_tcp_Protocol_direction(direction);
	int sock_cli = socket(AF_INET, SOCK_STREAM, 0);

	/// Define sockaddr_in
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SPEAKERPORT);					   /// Server Port
	servaddr.sin_addr.s_addr = inet_addr(speaker_ip); /// server ip

    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("connect");
		exit(1);
	}

	int n;
    n = send(sock_cli, command, 8, 0);
	
		printf("this is n:%d\n", n);
		if (n < 0)
		{
			cerr << "Error writing to socket!" << strerror(errno) << endl;
			close(sock_cli);
			printf("Error");
		}
		close(sock_cli);
}

void speaker_coordinate (const char * speaker_ip, int pan, int tilt){
	
	uint8_t * command;
	command = pantilt_tcp_Protocol_coordinate("set_pan_position", pan);

	
	int sock_cli = socket(AF_INET, SOCK_STREAM, 0);

	/// Define sockaddr_in
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SPEAKERPORT);					   /// Server Port
	servaddr.sin_addr.s_addr = inet_addr(speaker_ip); /// server ip

    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("connect");
		exit(1);
	}

	int n;
    n = send(sock_cli, command, 8, 0);
	
		printf("this is n:%d\n", n);
		if (n < 0)
		{
			cerr << "Error writing to socket!" << strerror(errno) << endl;
			close(sock_cli);
			printf("Error");
		}
		
	command = pantilt_tcp_Protocol_coordinate("set_tilt_position", tilt);

    n = send(sock_cli, command, 8, 0);
	
		printf("this is n:%d\n", n);
		if (n < 0)
		{
			cerr << "Error writing to socket!" << strerror(errno) << endl;
			close(sock_cli);
			printf("Error");
		}
		
	close(sock_cli);
}



void current_speaker_coordinate (const char * speaker_ip) {

	
	int sock_cli = socket(AF_INET, SOCK_STREAM, 0);

	/// Define sockaddr_in
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SPEAKERPORT);					   /// Server Port
	servaddr.sin_addr.s_addr = inet_addr(speaker_ip); /// server ip

    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("connect");
		exit(1);
	}

	int n;

	
	uint8_t * command;
	command = pantilt_tcp_Protocol_coordinate("query_pan_position");
	
    n = send(sock_cli, command, 8, 0);
	
		//printf("this is n:%d\n", n);
		if (n < 0)
		{
			cerr << "Error writing to socket!" << strerror(errno) << endl;
			close(sock_cli);
			printf("Error");
		}
	
	
	std::vector<uint8_t> buffer(20);
	
	int bytesReceived = 0;
	
	bytesReceived = recv(sock_cli, &buffer[0], buffer.size(), 0); /// Receiving
	printf("bytesReceived : %d\n", bytesReceived);
	printf("check\n");
	
	printf("this is current pan coordinate %d\n",buffer.data()[5]|(buffer.data()[4]<<8));

	
	command = pantilt_tcp_Protocol_coordinate("query_tilt_position");

    n = send(sock_cli, command, 8, 0);
	
		//printf("this is n:%d\n", n);
		if (n < 0)
		{
			cerr << "Error writing to socket!" << strerror(errno) << endl;
			close(sock_cli);
			printf("Error");
		}
	
	
	bytesReceived = recv(sock_cli, &buffer[0], buffer.size(), 0); /// Receiving
	printf("bytesReceived : %d\n", bytesReceived);
	
	
	printf("this is current tilt coordinate %d\n",buffer.data()[5]|(buffer.data()[4]<<8));

		
	close(sock_cli);
}

void speaker::init()
{

// Create and open port
    SPK_SOCKET = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(PORT);
    sockaddr.sin_addr.s_addr = inet_addr(IP);

    // If failed to connect,
    if (connect(SPK_SOCKET, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
    {
        printf("[ERR] Failed to connect a socket!\n" );
        
    }

    printf( "[MSG] speaker succesfully connected!\n" );
    uint8_t * command;

    command = speaker_tcp_Protocol("ping", 0);

    // VMS power on
    send(SPK_SOCKET, command, 8, 0);
    int bytesReceived = 0;
    std::vector<uint8_t> buffer(20);
    bytesReceived = recv(SPK_SOCKET, &buffer[0], buffer.size(), 0); /// Receiving
    printf("bytesReceived : %d\n", bytesReceived);
    //printf("check\n");	
    printf("this is PING %02x %02x %02x %02x %02x\n",buffer.data()[0],buffer.data()[1], buffer.data()[2], buffer.data()[3], buffer.data()[4]);
}



void speaker::control(string option, int value)
{
    uint8_t * command;
    command = speaker_tcp_Protocol(option, value);
    int i = 0;
    // VMS power on
    i = send(SPK_SOCKET, command, 8, 0);
    
    // printf("order successfully executed%d\n", i);
}


speaker::~speaker()
{
}