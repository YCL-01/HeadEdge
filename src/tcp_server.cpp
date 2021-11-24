#include "tcp_server.h"
#include "common.h"
#include "component.h"
#include "trafficSignal.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <unistd.h>

// define traffic signal port and baudspeed info
#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUD_SPEED 38400

typedef struct client {
  char ip[15];
  char edgeType[5];
} client;

// compile error 나서 tcp_server.h 에 있는 작성되있던 ip list, 옮겼습니다.
const client edgeIpList[] = {
    {"192.168.0.30", "C0"}, // pole 0 car edge
    {"192.168.0.32", "C1"}, // pole 1 car edge
    {"192.168.0.31", "P0"}, // pole 0 ped edge
    {"192.168.0.33", "P1"}, // pole 1 ped edge
};

const char *getEdgeType(char *ip) {
  int i = 0;
  while (1) {
    if ((strcmp(ip, edgeIpList[i].ip)) == 0) {
      break;
    } else {
      i++;
    }
  }
  return edgeIpList[i].edgeType;
}
void *recvThread(void *Args) {
  recvArgs *args = (recvArgs *)Args;
  recvSig *shmA;
  void *shmAddr;
  char edgeType[3];
  sprintf(edgeType, "%s", args->edgeType);
  printf("Edge: %s\n", edgeType);

  int *sockAddr = args->sock;
  int sock = *(sockAddr);
  int recvData;
  char buf[10];
  char data[10];
  char trafficSig[3] = {
      0,
  };

  if ((shmAddr = shmat(args->shmId, (void *)0, 0)) == (void *)-1) {
    perror("Shmat failed");
    exit(0);
  }
  shmA = (recvSig *)shmAddr;

  while (1) {
    recvData = recv(sock, buf, sizeof(buf), 0);
    sprintf(data, "%s", buf);
    switch (edgeType[0]) {
    case 'C':
      switch (edgeType[1]) {
      case '0':
        sprintf(shmA->car_0, "%c", data[0]);
        sprintf(shmA->car_0_speed, "%c%c", data[1], data[2]);
        usleep(10);
        break;
      case '1':
        sprintf(shmA->car_1, "%c", data[0]);
        sprintf(shmA->car_1_speed, "%c%c", data[1], data[2]);
        usleep(10);
        break;
      }
      break;
    case 'P':
      switch (edgeType[1]) {
      case '0':
        sprintf(shmA->ped_0, "%c", data[0]);
        usleep(10);
        break;
      case '1':
        sprintf(shmA->ped_1, "%c", data[0]);
        usleep(10);
        break;
      }
      break;
    }
    sprintf(shmA->traffic, "%d", getTrafficSignal());
    sprintf(trafficSig, "%d", getTrafficSignal());
    write(sock, trafficSig, sizeof(trafficSig));
  }
}

int receiver(int shmId) {
  int sockFd;
  int clientSock[4];
  struct sockaddr_in address;
  struct sockaddr_in clientAddr[4];
  int addlen = sizeof(address);
  int opt = 1;

  // Server (HeadEdge) //
  if ((sockFd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    printf("socket error\n");
    exit(1);
  }
  setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(SERV_IP);
  address.sin_port = htons(SERV_PORT);
  if (bind(sockFd, (struct sockaddr *)&address, addlen) < 0) {
    printf("socket bind error\n");
    exit(1);
  }
  if (listen(sockFd, 5) < 0) {
    printf("socket listen wrong\n");
    exit(1);
  }
  printf("Server Activated\n");

  runTrafficSignalThread(SERIAL_PORT, BAUD_SPEED);
  // new threads for each normal edge
  pthread_t normThread[NORM_EDGE];
  // thread arguments for receiving
  recvArgs Args;
  char *clientIp;

  for (int i = 0; i < NORM_EDGE; i++) {
    if ((clientSock[i] = accept(sockFd, (struct sockaddr *)&clientAddr[i],
                                (socklen_t *)&addlen)) < 0) {
      printf("client socket went wrong\n");
      exit(1);
    }
    Args.sock = &clientSock[i];
    Args.shmId = shmId;
    clientIp = inet_ntoa(clientAddr[i].sin_addr);
    printf("client: %s\n", clientIp);
    sprintf(Args.edgeType, "%s", getEdgeType(clientIp)); // getEdgeType(ip_data)
    printf("EdgeType: %s\n", getEdgeType(clientIp));
    pthread_create(&normThread[i], NULL, recvThread, (void *)&Args);
  }

  for (int i = 0; i < NORM_EDGE; i++) {
    pthread_join(normThread[0], NULL);
  }

  if (-1 != shmctl(shmId, IPC_RMID, 0)) {
    printf("Shared Memory Delete Success!\n");
  } else {
    printf("Shared Memory Delete Failed..\n");
  }

  return 0;
}

// void *tranThread(void *Args) { }
int transceiver(int shmId) {

  actSig *shareMemB; // shouldbe scn
  void *shmAddrB;
  if ((shmAddrB = shmat(shmId, (void *)0, 0)) == (void *)-1) {
    perror("Shmat failed");
    exit(0);
  } else {
    printf("gentleman, shm B complete\n");
  }
  shareMemB = (actSig *)shmAddrB;

  char message[MAX_BUF_SIZE] = {
      0,
  };
  int i = 0;
  char prev_message[MAX_BUF_SIZE] = {
      0,
  };
  char risk_rate_message[MAX_BUF_SIZE] = {
      0,
  };
  char prev_risk_rate_message[MAX_BUF_SIZE] = {
      0,
  };
  componentArgs *current = (componentArgs *)malloc(256);
  componentArgs *prev = (componentArgs *)malloc(256);
  scenario current_main;

  int current_max = 0;
  int prev_max = 0;
  component_init((void *)shareMemB);

  pthread_t pol0, pol1, main;
  pthread_attr_t mainattr;
  pthread_attr_init(&mainattr);
  pthread_attr_setdetachstate(&mainattr, PTHREAD_CREATE_DETACHED);
  while (1) {
    current_main = main_scn(shareMemB->pole0, shareMemB->pole1);

    sprintf(current->pole0.scnCode, "%s", shareMemB->pole0.scnCode);
    sprintf(current->pole0.riskRate, "%s", shareMemB->pole0.riskRate);
    sprintf(current->pole0.speed, "%s", shareMemB->pole0.speed);
    sprintf(current->pole1.scnCode, "%s", shareMemB->pole1.scnCode);
    sprintf(current->pole1.riskRate, "%s", shareMemB->pole1.riskRate);
    sprintf(current->pole1.speed, "%s", shareMemB->pole1.speed);
    sprintf(current->main.scnCode, "%s", current_main.scnCode);
    sprintf(current->main.riskRate, "%s", current_main.riskRate);
    sprintf(current->main.speed, "%s", current_main.speed);

    sprintf(current->pole0.pol_num, "%d", 0);
    sprintf(current->pole1.pol_num, "%d", 1);

    if (i == 0) {

      sprintf(prev->pole0.scnCode, "%s", shareMemB->pole0.scnCode);
      sprintf(prev->pole0.riskRate, "%s", shareMemB->pole0.riskRate);
      sprintf(prev->pole0.speed, "%s", shareMemB->pole0.speed);
      sprintf(prev->pole1.scnCode, "%s", shareMemB->pole1.scnCode);
      sprintf(prev->pole1.riskRate, "%s", shareMemB->pole1.riskRate);
      sprintf(prev->pole1.speed, "%s", shareMemB->pole1.speed);
      sprintf(prev->main.scnCode, "%s", current_main.scnCode);
      sprintf(prev->main.riskRate, "%s", current_main.riskRate);
      sprintf(prev->main.speed, "%s", current_main.speed);

      component_controller((void *)&(current->pole0));
      component_controller((void *)&(current->pole1));
      // pthread_create(&pol0, NULL, component_controller, (void *)
      // &(current->pole0)); pthread_create(&pol1, NULL, component_controller,
      // (void *) &(current->pole1));
      pthread_create(&main, &mainattr, ped_control, (void *)&(current->main));
      // pthread_detach(pol0);
      // pthread_detach(pol1);
      pthread_detach(main);

      i++;
    }

    else {
      // printf("pole1 current scn :%s pole1 prev scn :%s main scn : %s prev
      // main scn: %s\n",current->pole0.scnCode,prev->pole0.scnCode,
      // current->main.scnCode, prev->main.scnCode);

      if (atoi(current->main.riskRate) > atoi(prev->main.riskRate)) {

        printf("Danger increased\n");

        // pthread_cancel(main);
        // pthread_cancel(pol0);
        // pthread_cancel(pol1);

        try {
          if (pthread_kill(main, 0) == 0) {
            pthread_cancel(main);
            printf("Thread cancled\n");
          }
        } catch (int exception) {
          printf("Thread is not exist\n");
        }

        // if (pthread_kill(pol0,0) == 0) pthread_cancel(pol0);
        // if (pthread_kill(pol1,0) == 0) pthread_cancel(pol1);

        printf("New Thread started\n");
        component_controller((void *)&(current->pole0));
        component_controller((void *)&(current->pole1));
        // pthread_create(&pol0, NULL, component_controller, (void *)
        // &(current->pole0)); pthread_create(&pol1, NULL, component_controller,
        // (void *) &(current->pole1));
        pthread_create(&main, &mainattr, ped_control, (void *)&(current->main));

        // pthread_detach(pol0);
        // pthread_detach(pol1);
        pthread_detach(main);

      }
      // Risk not changed
      else {

        if (strcmp(current->pole0.scnCode, prev->pole0.scnCode) == 0) {

          if (strcmp(current->pole1.scnCode, prev->pole1.scnCode) == 0) {
            continue;

          } else {
            printf("if else  num1 %s\n", current->pole1.pol_num);
            component_controller((void *)&(current->pole1));
            // pthread_create(&pol1, NULL, component_controller, (void *)
            // &(current->pole1)); pthread_detach(pol1);
          }
        } else {
          if (strcmp(current->pole1.scnCode, prev->pole1.scnCode) == 0) {
            printf("if else  num2 %s\n", current->pole1.pol_num);
            component_controller((void *)&(current->pole0));
            // pthread_create(&pol0, NULL, component_controller, (void *)
            // &(current->pole0)); pthread_detach(pol0);
          } else {
            printf("if else num3 %s\n", current->pole1.pol_num);
            // component_controller((void *) &(current->pole1));
            // pthread_create(&pol1, NULL, component_controller, (void *)
            // &(current->pole1)); pthread_detach(pol1); pthread_create(&pol0,
            // NULL, component_controller, (void *) &(current->pole0));
            // pthread_detach(pol0);
            component_controller((void *)&(current->pole0));
            component_controller((void *)&(current->pole1));
          }
        }

        // main changed
        if (strcmp(current->main.scnCode, prev->main.scnCode) == 0) {
          usleep(100);
        } else {
          int ret = pthread_kill(main, 0);
          printf("ret value %d\n", ret);
          if (ret != 0) {
            printf("Ped_trhead executues\n");
            // printf("this is tranceiver else, current Thread Does not
            // exist\n");
            pthread_create(&main, &mainattr, ped_control,
                           (void *)&(current->main));
            pthread_detach(main);
          }
        }
      }

      sprintf(prev->pole0.scnCode, "%s", shareMemB->pole0.scnCode);
      sprintf(prev->pole0.riskRate, "%s", shareMemB->pole0.riskRate);
      sprintf(prev->pole0.speed, "%s", shareMemB->pole0.speed);
      sprintf(prev->pole1.scnCode, "%s", shareMemB->pole1.scnCode);
      sprintf(prev->pole1.riskRate, "%s", shareMemB->pole1.riskRate);
      sprintf(prev->pole1.speed, "%s", shareMemB->pole1.speed);
      sprintf(prev->main.scnCode, "%s", current_main.scnCode);
      sprintf(prev->main.riskRate, "%s", current_main.riskRate);
      sprintf(prev->main.speed, "%s", current_main.speed);
      usleep(100);
      // component 계속 실행
      i++;
    }
  }
  return 0;
}
