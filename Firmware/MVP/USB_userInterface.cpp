//Copyright 2021-2022(c) John Sullivan
//github.com/doppelhub/Honda_Insight_LiBCM

//Handles user interactions with LiBCM (data query/response)
//see debugUSB.c for data LiBCM automatically generates (and also sends over USB)

#include "libcm.h"

uint8_t line[USER_INPUT_BUFFER_SIZE]; //Stores user text as it's read from serial buffer

/////////////////////////////////////////////////////////////////////////////////////////////

void printStringStoredInArray(const uint8_t *s)
{
  while (*s) { Serial.write(*s++); } //write each character until '0' (EOL character)
}

/////////////////////////////////////////////////////////////////////////////////////////////

void printDebug(void)
{
	Serial.print(F("\nDebug data persists in EEPROM until cleared ('$DEBUG=CLR' to clear)"));

	Serial.print(F("\n -Has LiBCM limited assist since last cleared?: "));
	(EEPROM_hasLibcmDisabledAssist_get() == EEPROM_LICBM_DISABLED_ASSIST) ? Serial.print(F("YES")) : Serial.print(F("NO"));

	Serial.print(F("\n -Has LiBCM limited regen since last cleared?: "));
	(EEPROM_hasLibcmDisabledRegen_get() == EEPROM_LICBM_DISABLED_REGEN) ? Serial.print(F("YES")) : Serial.print(F("NO"));

	Serial.print(F("\n -loopPeriod exceeded since last cleared?: "));
	(EEPROM_hasLibcmFailedTiming_get() == EEPROM_LIBCM_LOOPPERIOD_EXCEEDED) ? Serial.print(F("YES")) : Serial.print(F("NO"));
}

/////////////////////////////////////////////////////////////////////////////////////////////

void USB_userInterface_runTestCode(uint8_t testToRun)
{
	//Add whatever code you want to run whenever the user types '$TEST1'/2/3/etc into the Serial Monitor Window
	if     (testToRun == STRING_TERMINATION_CHARACTER) { Serial.print(F("\nError: Test not specified")); }
	else if(testToRun == '1')
	{
		Serial.print(F("\nRunning TEST1: PCB Fan High"));
		fan_requestSpeed(FAN_PCB, FAN_REQUESTOR_KEY, FAN_HIGH);
	}
	else if(testToRun == '2')
	{
		Serial.print(F("\nRunning TEST2: PCB Fan Low"));
		fan_requestSpeed(FAN_PCB, FAN_REQUESTOR_KEY, FAN_LOW);
	}
	else if(testToRun == '3')
	{
		Serial.print(F("\nRunning TEST2: PCB Fan off"));
		fan_requestSpeed(FAN_PCB, FAN_REQUESTOR_KEY, FAN_OFF);
	}
	else if(testToRun == '4')
	{
		Serial.print(F("\nRunning TEST3"));
		Serial.print(F("\nGrid PWM Test"));
		pinMode(PIN_GRID_PWM, OUTPUT);

		static uint8_t pwmValue_grid = 0;

		if     (pwmValue_grid == 255) { pwmValue_grid =   0; }
		else if(pwmValue_grid >= 200) { pwmValue_grid = 255; }
		else                          { pwmValue_grid += 50; }

		Serial.print(F("\nSetting gridPWM to: "));
		Serial.print(pwmValue_grid);
		analogWrite(PIN_GRID_PWM, pwmValue_grid);
	}
	else if(testToRun == '5')
	{
		Serial.print(F("\nRunning TEST5: Is Heater PCB Connected?"));
		gpio_isPackHeaterInstalled();
	}
	else if(testToRun == '6')
	{
		Serial.print(F("\nRunning TEST6: Turn off LiBCM (5V rail).\nLiBCM will stay on if USB's +5V connected."));
		gpio_turnLiBCM_off();
	}
	else if(testToRun == '7')
	{
		Serial.print(F("\nRunning TEST7: Turn off Heater PCB."));
		gpio_turnHeaterPCB_off();
	}
	else if(testToRun == '8')
	{
		Serial.print(F("\nRunning TEST8: Turn on Heater PCB."));
		gpio_turnHeaterPCB_on();
	}
	else if(testToRun == 'T')
	{
		gpio_turnTemperatureSensors_on();
		Serial.print(F("\nTemperatures:"));
		Serial.print(F("\nBLU: "));
		Serial.print(temperature_measureOneSensor_degC(PIN_TEMP_BLU));
		Serial.print(F("\nGRN: "));
		Serial.print(temperature_measureOneSensor_degC(PIN_TEMP_GRN));
		Serial.print(F("\nWHT: "));
		Serial.print(temperature_measureOneSensor_degC(PIN_TEMP_WHT));
		Serial.print(F("\nYEL: "));
		Serial.print(temperature_measureOneSensor_degC(PIN_TEMP_YEL));
		Serial.print(F("\nBAY1: "));
		Serial.print(temperature_measureOneSensor_degC(PIN_TEMP_BAY1));
		Serial.print(F("\nBAY2: "));
		Serial.print(temperature_measureOneSensor_degC(PIN_TEMP_BAY2));
		Serial.print(F("\nBAY3: "));
		Serial.print(temperature_measureOneSensor_degC(PIN_TEMP_BAY3));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

//JTS2doLater: Put this somewhere else
void(* rebootLiBCM) (void) = 0;//declare reset function at address 0

/////////////////////////////////////////////////////////////////////////////////////////////

void printHelp(void)
{
	Serial.print(F("\n\nLiBCM commands:"
		"\n -'$BOOT': restart LiBCM"
		"\n -'$TEST1'/2/3/4: run test code. See 'USB_userInterface_runTestCode()')"
		"\n -'$DEBUG': info stored in EEPROM. 'DEBUG=CLR' to restore defaults"
		"\n -'$LIDISP' 0/1: LiDisplay Debug Mode Off/On."
		"\n -'$KEYms': delay after keyON before LiBCM starts. 'KEYms=___' to set (0 to 254 ms)"
		"\n -'$SoC': battery charge in percent. 'SoC=___' to set (0 to 100%)"
		"\n -'$DISP=PWR'/SCI/CELL/TEMP/DBG/OFF: data to stream (power/BAT&METSCI/Vcell/temperature/none)"
		"\n -'$RATE=___': USB updates per second (1 to 255 Hz)"
		"\n -'$LOOP: LiBCM loop period. '$LOOP=___' to set (1 to 255 ms)"
		"\n -'$SCIms': period between BATTSCI frames. '$SCIms=___' to set (0 to 255 ms)"
		"\n"
		/*
		"\nFuture LiBCM commands (not presently supported"
		"\n -'$DEFAULT' restore all EEPROM values to default"
		"\n -'$FAN' display fan status.  '$FAN=OFF'/LOW/HI to set."
		"\n -'$IHACK' display current hack setting.  '$IHACK=00'/20/40/60 to set."
		"\n -'$VHACK' display voltage hack setting.  '$VHACK=OEM'/ASSISTONLY_VAR/ASSISTONLY_BIN/ALWAYS."
		"\n -'$ASSIST_OFF' disable assist until LiBCM resets."
		"\n -'$ASSIST_ON' enable assist until LiBCM resets."
		"\n -'$REGEN_OFF' disable regen until LiBCM resets."
		"\n -'$REGEN_ON' enable regen until LiBCM resets."
		"\n -'BATTmAh' display battery capacity in mAh.  'BATTmAh=____' to set."
		"\n -'SoC_MAX' display max allowed SoC.  'SoC_MAX=__' to set."
		"\n -'SoC_MIN' display min allowed SoC.  'SoC_MIN=__' to set."
		"\n -'GRIDVMAX' display max grid charger voltage. 'GRIDVMAX=_.___' to set."
		*/
		));
	//When adding new commands, make sure to add cases to the following functions:
		//USB_userInterface_executeUserInput()
		//EEPROM_resetDebugValues() //if debug data is stored in EEPROM
		//EEPROM_verifyDataValid() //if data is stored in EEPROM
}

/////////////////////////////////////////////////////////////////////////////////////////////

//LiBCM's atoi() implementation for uint8_t
uint8_t get_uint8_FromInput(uint8_t digit1, uint8_t digit2, uint8_t digit3)
{
	bool errorOccurred = false;
	uint8_t numDecimalDigits = 3;
	uint8_t decimalValue = 0;

	if     (digit1 == STRING_TERMINATION_CHARACTER) { errorOccurred = true; }
	else if(digit2 == STRING_TERMINATION_CHARACTER) { numDecimalDigits = 1; }
	else if(digit3 == STRING_TERMINATION_CHARACTER) { numDecimalDigits = 2; }

	if(errorOccurred == true) { Serial.print(F("\nInvalid uint8_t Entry")); }
	else
	{
		if     (numDecimalDigits == 1) { decimalValue =                                      (digit1-'0'); }
		else if(numDecimalDigits == 2) { decimalValue =                    (digit1-'0')*10 + (digit2-'0'); }
		else if(numDecimalDigits == 3) { decimalValue = (digit1-'0')*100 + (digit2-'0')*10 + (digit3-'0'); }
	}

	return decimalValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

//determine which command to run
void USB_userInterface_executeUserInput(void)
{
	if(line[0] == '$') //valid commands start with '$'
	{
		//$HELP
		if( (line[1] == 'H') && (line[2] == 'E') && (line[3] == 'L') && (line[4] == 'P') ) { printHelp(); }

		//$BOOT
		else if( (line[1] == 'B') && (line[2] == 'O') && (line[3] == 'O') && (line[4] == 'T') )
		{
			Serial.print(F("\nRebooting LiBCM"));
			delay(50); //give serial buffer time to send
			rebootLiBCM();
		}

		//$TEST
		else if( (line[1] == 'T') && (line[2] == 'E') && (line[3] == 'S') && (line[4] == 'T') ) { USB_userInterface_runTestCode(line[5]); }

		//$DEBUG
		else if( (line[1] == 'D') && (line[2] == 'E') && (line[3] == 'B') && (line[4] == 'U') && (line[5] == 'G') )
		{
			if     ( (line[6] == '=') && (line[7] == 'C') && (line[8] == 'L') && (line[9] == 'R') )
			{
				Serial.print(F("\nRestoring default DEBUG values"));
				EEPROM_resetDebugValues();
			}
			else if(line[6] == STRING_TERMINATION_CHARACTER) { printDebug(); }
		}
/*
		//$LIDISP
		else if( (line[1] == 'L') && (line[2] == 'I') && (line[3] == 'D') && (line[4] == 'I') && (line[5] == 'S') && (line[6] == 'P'))
		{
			LiDisplay_setDebugMode(line[7]);
		} */

		//KEYms
		else if( (line[1] == 'K') && (line[2] == 'E') && (line[3] == 'Y') && (line[4] == 'M') && (line[5] == 'S') )
		{
			if (line[6] == '=')
			{
				uint8_t newKeyOnDelay_ms = get_uint8_FromInput(line[7],line[8],line[9]);
				Serial.print("\nnewKeyOnDelay_ms is " + String(newKeyOnDelay_ms) );
				EEPROM_delayKeyON_ms_set(newKeyOnDelay_ms);
			}
			else if(line[6] == STRING_TERMINATION_CHARACTER)
			{
				Serial.print(F("\n Additional delay before LiBCM responds to keyON event (ms): "));
				Serial.print( EEPROM_delayKeyON_ms_get(), DEC);
			}
		}

		//SoC
		else if( (line[1] == 'S') && (line[2] == 'O') && (line[3] == 'C') )
		{
			if (line[4] == '=')
			{
				uint8_t newSoC_percent = get_uint8_FromInput(line[5],line[6],line[7]);
				Serial.print("\nnewSoC is " + String(newSoC_percent) );
				SoC_setBatteryStateNow_percent(newSoC_percent);
			}
			else if(line[4] == STRING_TERMINATION_CHARACTER)
			{
				Serial.print(F("\nBattery SoC is (%): "));
				Serial.print(SoC_getBatteryStateNow_percent(),DEC);
			}
		}

		//DISP
		//JTS2doLater: Make this function work while grid charging, too
		else if( (line[1] == 'D') && (line[2] == 'I') && (line[3] == 'S') && (line[4] == 'P') && (line[5] == '=') )
		{
			if     ( (line[6] == 'P') && (line[7] == 'W') && (line[8] == 'R') ) { debugUSB_dataTypeToStream_set(DEBUGUSB_STREAM_POWER);      }
			else if( (line[6] == 'S') && (line[7] == 'C') && (line[8] == 'I') ) { debugUSB_dataTypeToStream_set(DEBUGUSB_STREAM_BATTMETSCI); }
			else if( (line[6] == 'C') && (line[7] == 'E') && (line[8] == 'L') ) { debugUSB_dataTypeToStream_set(DEBUGUSB_STREAM_CELL);       }
			else if( (line[6] == 'O') && (line[7] == 'F') && (line[8] == 'F') ) { debugUSB_dataTypeToStream_set(DEBUGUSB_STREAM_NONE);       }
			else if( (line[6] == 'T') && (line[7] == 'E') && (line[8] == 'M') ) { debugUSB_dataTypeToStream_set(DEBUGUSB_STREAM_TEMP);       }
			else if( (line[6] == 'D') && (line[7] == 'B') && (line[8] == 'G') ) { debugUSB_dataTypeToStream_set(DEBUGUSB_STREAM_DEBUG);      }
		}

		//RATE
		else if( (line[1] == 'R') && (line[2] == 'A') && (line[3] == 'T') && (line[4] == 'E') && (line[5] == '=') )
		{
			uint8_t newUpdatesPerSecond = get_uint8_FromInput(line[6],line[7],line[8]);
			debugUSB_dataUpdatePeriod_ms_set( time_hertz_to_milliseconds(newUpdatesPerSecond) );
		}

		//LOOP
		else if( (line[1] == 'L') && (line[2] == 'O') && (line[3] == 'O') && (line[4] == 'P') )
		{
			if(line[5] == '=')
			{
				uint8_t newLooprate_ms = get_uint8_FromInput(line[6],line[7],line[8]);
				time_loopPeriod_ms_set(newLooprate_ms);
			}
			else if(line[5] == STRING_TERMINATION_CHARACTER)
			{
				Serial.print(F("\nLoop period is (ms): "));
				Serial.print(time_loopPeriod_ms_get(),DEC);
			}
		}

		//SCIms
		else if( (line[1] == 'S') && (line[2] == 'C') && (line[3] == 'I') && (line[4] == 'M') && (line[5] == 'S') )
		{
			if(line[6] == '=')
			{
				uint8_t newPeriod_ms = get_uint8_FromInput(line[7],line[8],line[9]);
				BATTSCI_framePeriod_ms_set(newPeriod_ms);
			}
			else if(line[6] == STRING_TERMINATION_CHARACTER)
			{
				Serial.print(F("\nBATTSCI period is (ms): "));
				Serial.print(BATTSCI_framePeriod_ms_get(),DEC);
			}
		}

		//$DEFAULT
		else { Serial.print(F("\nInvalid Entry")); }
	}
	else { Serial.print(F("\nInvalid Entry")); }
}

/////////////////////////////////////////////////////////////////////////////////////////////

//read user-typed input from serial buffer
//user input executes at each newline character
void USB_userInterface_handler(void)
{
	uint8_t latestCharacterRead = 0; //c
	static uint8_t numCharactersReceived = 0; //char_counter
	static uint8_t inputFlags = 0; //stores state as input text is processed (e.g. whether inside a comment or not)

	while( Serial.available() )
	{
		//user-typed characters are waiting in serial buffer

		latestCharacterRead = Serial.read(); //read next character in buffer

		if( (latestCharacterRead == '\n') || (latestCharacterRead == '\r'))
		{
			//line is now complete
			line[numCharactersReceived] = STRING_TERMINATION_CHARACTER;

			Serial.print(F("\necho: "));
			printStringStoredInArray(line); //echo user input

			USB_userInterface_executeUserInput();

			numCharactersReceived = 0; //reset for next line
		}
		else //add (non-EOL) character to array
		{
			if(inputFlags && INPUT_FLAG_INSIDE_COMMENT)
			{
				if(latestCharacterRead == ')') { inputFlags &= ~(INPUT_FLAG_INSIDE_COMMENT); } //end of comment
			}
			else if(numCharactersReceived < (USER_INPUT_BUFFER_SIZE - 1))
			{
				//not presently inside a comment and input buffer not exceeded
				if     (latestCharacterRead == '(') { inputFlags |= INPUT_FLAG_INSIDE_COMMENT;    } //start of comment //ignores all characters until ')'
				else if(latestCharacterRead == ' ') { ;                                           } //throw away whitespaces
				else if( (latestCharacterRead >= 'a') && (latestCharacterRead <= 'z') )
				{
					line[numCharactersReceived++] = latestCharacterRead + ('A' - 'a'); //convert letters to uppercase
				}
				else {line[numCharactersReceived++] = latestCharacterRead; } //store everything else
			}
			else //numCharactersReceived too high
			{
				Serial.print(F("\nError: entry too long"));

				while( Serial.available() )
				{
					//empty serial receive buffer
					uint8_t byte = Serial.read();
					if((byte == '\n') || (byte == '\r')) { break; }
				}
				numCharactersReceived = 0;
			}
		}
	}
}
