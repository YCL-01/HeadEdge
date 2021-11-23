void carspeaker_stopper( const char * car_speaker_ip);
void pedspeaker_stopper(int);
void * ped_control(void *);
void component_init(void *);
void * component_controller(void * a);
void  * decide_main_function(void * a);
scenario main_scn (scenario a, scenario b);