#include "trafficSignal.h"

// return -> 0, 1, 2
int getTrafficSignal(){
  pthread_mutex_lock(&trafficMutex);
  int temp_trafficSignal = trafficSignal;
  pthread_mutex_unlock(&trafficMutex);
  return temp_trafficSignal;
}

int getRemainingTime(){
  pthread_mutex_lock(&trafficMutex);
  int temp_remainingTime = remainingTime;
  pthread_mutex_unlock(&trafficMutex);
  return temp_remainingTime;
}


int init_uart(char * dev, int baud, int * fd){
    struct termios newtio;
    * fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if ( * fd < 0) {
        printf("%s> uart dev '%s' open fail [%d]n", __func__, dev, * fd);
        return -1;
    }
    memset( & newtio, 0, sizeof(newtio));
    newtio.c_iflag = IGNPAR; // non-parity 
    newtio.c_oflag = OPOST | ONLCR;;
    newtio.c_cflag = CS8 | CLOCAL | CREAD; // NO-rts/cts 
    switch (baud){
    case 115200:
      newtio.c_cflag |= B115200;
      break;
    case 57600:
      newtio.c_cflag |= B57600;
      break;
    case 38400:
      newtio.c_cflag |= B38400;
      break;
    case 19200:
      newtio.c_cflag |= B19200;
      break;
    case 9600:
      newtio.c_cflag |= B9600;
      break;
    case 4800:
      newtio.c_cflag |= B4800;
      break;
    case 2400:
      newtio.c_cflag |= B2400;
      break;
    default:
      newtio.c_cflag |= B115200;
      break;
    }

    newtio.c_lflag = 0;
    //newtio.c_cc[VTIME] = vtime; // timeout 0.1초 단위 
    //newtio.c_cc[VMIN] = vmin; // 최소 n 문자 받을 때까진 대기 
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush( * fd, TCIFLUSH);
    tcsetattr( * fd, TCSANOW, & newtio);
    return 0;
}

void* parseTrafficSignal(void * arg){
    int result;
    char buffer[MAX_BUF_SIZE];
    fd_set reads, temps;

    FD_ZERO( & reads);
    FD_SET(fd, & reads);

    while (1){
        temps = reads;
        result = select(FD_SETSIZE, & temps, NULL, NULL, NULL);

        if (result < 0){
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(fd, & temps)){
            memset(buffer, 0, sizeof(buffer));
            if (read(fd, buffer, MAX_BUF_SIZE) == -1)
                continue;

            // Red -> 0
            if(buffer[4] == 19 && buffer[5] == 0){
              if(getTrafficSignal() != 0){
                pthread_mutex_lock(&trafficMutex);
                trafficSignal = 0;
                pthread_mutex_unlock(&trafficMutex);
              }
            }
            // Green
            else if(buffer[4] == 3 && buffer[5] == 2){
              if(getTrafficSignal() != 1){
                pthread_mutex_lock(&trafficMutex);
                trafficSignal = 1;
                pthread_mutex_unlock(&trafficMutex);
              }
              pthread_mutex_lock(&trafficMutex);
              remainingTime = buffer[12];
              pthread_mutex_unlock(&trafficMutex);
            }
            // Green Blink
            else if(buffer[4] == 11 && buffer[5] == 2){
              if(getTrafficSignal() != 2){
                pthread_mutex_lock(&trafficMutex);
                trafficSignal = 2;
                pthread_mutex_unlock(&trafficMutex);
              }
              pthread_mutex_lock(&trafficMutex);
              remainingTime = buffer[12];
              pthread_mutex_unlock(&trafficMutex);
            }
          
        }
    }
}

void runTrafficSignalThread(char * serialPort, int baudSpeed){
    int result, ret = -1;
    pthread_t p_thread;

    if((ret = init_uart(serialPort, baudSpeed, &fd)) == -1){
      exit(0);
    }
    printf("init uart [%d], [%d]\n", ret, fd);
    pthread_create( &p_thread, NULL, parseTrafficSignal, NULL);

}