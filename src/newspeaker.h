

#define speaker_ped1 "192.168.0.50"
#define speaker_ped2 "192.168.0.53"
#define speaker_car1 "192.168.0.51"
#define speaker_car2 "192.168.0.54"
#define speaker_movPed1 "192.168.0.52"
#define speaker_movPed2 "192.168.0.55"
#define SPEAKERPORT 9531
#define max_volume 100
#define min_volume 0

void hexdumpunformatted(void *ptr, int buflen);
uint8_t * speaker_tcp_Protocol(string Name, int Value);
uint8_t * pantilt_tcp_Protocol_direction(string Name, int speed);
uint8_t * pantilt_tcp_Protocol_coordinate(string Name, int position);
void speaker_control (const char * speaker_ip, string option, int value);
void speaker_direction (string direction);
void speaker_coordinate (const char * speaker_ip, int pan, int tilt);
void current_speaker_coordinate (const char * speaker_ip) ;


class speaker{
public:
    explicit speaker(const char *ip, int port)  : IP(ip), PORT(port){};
    
    virtual ~speaker();

    virtual void init();
    void control(string option, int value);
    void status (string option);    
    
    private:
        const char *IP;
        int PORT;
        int SPK_SOCKET;
    
        
};

