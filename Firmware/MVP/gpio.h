#ifndef gpio_h
	#define gpio_h

	#define GPIO_CHARGER_INIT 0
	#define GPIO_CHARGER_ON   1
	#define GPIO_CHARGER_OFF  2

	#define GPIO_HEATER_INIT      0
	#define GPIO_HEATER_CONNECTED 1
	#define GPIO_HEATER_ABSENT    2

	void gpio_begin(void);

	bool gpio_keyStateNow(void);

	void gpio_setFanSpeed_OEM(char speed); //don't call directly (use fan_requestSpeed() instead)
	void gpio_setFanSpeed_PCB(char speed); //don't call directly (use fan_requestSpeed() instead)

	void gpio_turnPowerSensors_on( void);
	void gpio_turnPowerSensors_off(void);

	bool gpio_isGridChargerPluggedInNow(void);
	bool gpio_isGridChargerChargingNow(void);

	void gpio_turnGridCharger_on( void);
	void gpio_turnGridCharger_off(void);

	void gpio_setGridCharger_powerLevel(char powerLevel);

	void gpio_turnBuzzer_on_highFreq(void);
	void gpio_turnBuzzer_on_lowFreq(void);
	void gpio_turnBuzzer_off(void);
	void gpio_playSound_firmwareUpdated(void);

	bool gpio_isCoverInstalled(void);
	void gpio_safetyCoverCheck(void);

	void gpio_turnHMI_on(void);
	void gpio_turnHMI_off(void);
	bool gpio_HMIStateNow(void);

	void gpio_turnTemperatureSensors_on(void);
	void gpio_turnTemperatureSensors_off(void);

	void gpio_turnLiBCM_off(void);

	bool gpio1_getState(void);
	bool gpio2_getState(void);
	bool gpio3_getState(void);

	void gpio_turnHeaterPCB_on(void);
	void gpio_turnHeaterPCB_off(void);
	uint8_t gpio_isPackHeaterInstalled(void);

	uint8_t gpio_getHardwareRevision(void);

	#define HW_REV_C 0b00000011
	#define HW_REV_D 0b00000010
	#define HW_REV_E 0b00000001
	#define HW_REV_F 0b00000000

#endif
