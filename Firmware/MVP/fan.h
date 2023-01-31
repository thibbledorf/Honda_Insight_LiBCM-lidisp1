#ifndef fan_h
	#define fan_h

	//Mask to store each subsystem's fan speed request
	//Each subsystem stores 2 bits (OFF/LOW/MED/HIGH)
	#define FAN_REQUESTOR_GRIDCHARGER 0 //0b00000011 //the grid charger subsystem stores its fan request status in the two lowest bits
	#define FAN_REQUESTOR_CELLBALANCE 2 //0b00001100
	#define FAN_REQUESTOR_KEY         4 //0b00110000
	#define FAN_REQUESTOR_TEMPERATURE 6 //0b11000000 //the temperature subsystem stores its fan request status in the two highest bits

	//fan request status bits (for each subsystem)
	#define FAN_OFF  0b00
	#define FAN_LOW  0b01
	#define FAN_HIGH 0b10

	#define FAN_HI_MASK 0b10101010
	#define FAN_LO_MASK 0b01010101

	#define FAN_NOT_REQUESTED 0

	//fan controller number
	#define FAN_PCB 0
	#define FAN_OEM 1
	#define NUM_FAN_CONTROLLERS 2 //MUST equal number of fan controllers

	#define FAN_HYSTERESIS_ms          10000 //period required before fan can turn off
	#define FAN_HYSTERESIS_degC        2 //temperature delta required to change fan speed
	#define AIR_TEMP_DELTA_TO_RUN_FANS 4 //intake air must be this many degrees warmer/cooler than battery to run fans
	#define FAN_HIGH_SPEED_degC        3 //this many additional degrees will enable fan at high speed (instead of low speed)

	#define FORCE_FAN_UPDATE_PERIOD_MINUTES 5
	#define FORCE_FAN_UPDATE_PERIOD_ms (FORCE_FAN_UPDATE_PERIOD_MINUTES * 60000)

	void fan_handler(void);

	void fan_requestSpeed(uint8_t whichFan, uint8_t requestor, char newFanSpeed); //request fan speed
	//Example: fan_requestSpeed(FAN_PCB, FAN_REQUESTOR_GRIDCHARGER, FAN_HIGH);
	char fan_getCurrentSpeed(uint8_t whichFan);

#endif
