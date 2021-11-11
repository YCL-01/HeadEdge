#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include <string.h>
#include <sys/types.h>
#include <termios.h>


static pthread_mutex_t trafficMutex;

#define MAX_BUF_SIZE 512
static int fd = 0;

// 0,1,2
static int trafficSignal = 0;

// integer
static int remainingTime;

int getTrafficSignal();

int getRemainingTime();

int init_uart(char * dev, int baud, int * fd);

void* parseTrafficSignal(void * arg);

void runTrafficSignalThread(char * serialPort, int baudSpeed);