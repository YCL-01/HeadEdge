#include <chrono>

#include "Error.h"
#include "BaseThread.h"

#define STX 0X02
#define ETX 0x03
#define TYPE_INSERT 0x84

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define RESET "\x1B[0m"

#define vms1 "192.168.0.80"
#define vms1_1 "192.168.0.81"
#define vms2 "192.168.0.82"
#define vms_port 100


enum VMS_CODE
{
    VE0S0P0,
    VE0S0P1,
    VE1S1P0,
    VE1S1P1,
    SPEEDING,
    NORMAL,
    CAR,
    PERSON,
    CARANDPERSON,
    SPEEDINGANDPERSON

};

class VMS : public BaseThread
{
public:
    explicit VMS(const char *ip, int port, int is_smart_poll, int pol_num) : IP(ip),
                                                                PORT(port),
                                                                is_smart_poll(is_smart_poll), pol_num(pol_num),normal_count(1), normal_state(true){};
    virtual ~VMS();

    bool onEventChanged(VMS_CODE);
    bool onPerson();
    bool onSpeeding();
    bool onSpeedingandPerson();
    bool onCar();
    bool onCarandPerson();
    bool onNormal();
    bool setAddress(void *);


protected:
    virtual bool threadInitialize();
    virtual bool threadExecute();
    virtual bool threadShutdown();

    // For socket programming
    const char *IP;
    int PORT;
    int CONN;
    int VMS_SOCKET;

    // General info for VMS control
    VMS_CODE state = NORMAL;
    int id;
    int carId;
    int is_smart_poll;
    int pol_num;
    int normal_count;
    bool normal_state;
    void* addr;

    std::chrono::system_clock::time_point updated;

private:
    bool printDefault();
    bool printSpeeding();
    bool printEmergency();
    bool printCrossing();
    bool printChildren();

    bool sendMsg();
    bool txt2uint8();
};
