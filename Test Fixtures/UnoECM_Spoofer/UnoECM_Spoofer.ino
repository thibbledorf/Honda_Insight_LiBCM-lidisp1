//Spoofs ECM enough to prevent MCM P-Codes

#define PIN_MAMODE1 3
#define PIN_MAMODE2 4
#define PIN_ABSBUSY 2
#define PIN_ENGTRQ 5
#define PIN_CMDPWR 6
#define PIN_UNO_LED 13

void setup()
{
    TCCR2B = TCCR2B & B11111000 | B00000001; // D3/D11 PWM F=31372.55 Hz
  //TCCR2B = TCCR2B & B11111000 | B00000010; // D3/D11 PWM F= 3921.16 Hz
  //TCCR2B = TCCR2B & B11111000 | B00000011; // D3/D11 PWM F=  980.39 Hz
  //TCCR2B = TCCR2B & B11111000 | B00000100; // D3/D11 PWM F=  490.20 Hz (DEFAULT)

  //Timer also controls delay(), etc. 
  //TCCR0B = TCCR0B & B11111000 | B00000001; // D5/D6 PWM F= 62500.00 Hz
  //TCCR0B = TCCR0B & B11111000 | B00000010; // D5/D6 PWM F=  7812.50 Hz
    TCCR0B = TCCR0B & B11111000 | B00000011; // D5/D6 PWM F=   976.56 Hz (DEFAULT)
  //TCCR0B = TCCR0B & B11111000 | B00000100; // D5/D6 PWM F=   244.14 Hz

  //TCCR1B = TCCR1B & B11111000 | B00000001; // D9/D10 PWM F= 31372.55 Hz
  //TCCR1B = TCCR1B & B11111000 | B00000010; // D9/D10 PWM F=  3921.16 Hz
  //TCCR1B = TCCR1B & B11111000 | B00000011; // D9/D10 PWM F=   490.20 Hz (DEFAULT)
  //TCCR1B = TCCR1B & B11111000 | B00000100; // D9/D10 PWM F=   122.55 Hz
  
  
  pinMode(PIN_MAMODE2,OUTPUT);
  digitalWrite(PIN_MAMODE2,HIGH);

  analogWrite(PIN_MAMODE1, 127); //50% PWM
  
  analogWrite(PIN_ENGTRQ,26);  //10% PWM

  analogWrite(PIN_CMDPWR,127); //50% PWM
  
  pinMode(PIN_UNO_LED,OUTPUT);

  delay(1000); //wait to spoof ABS pulse
  pinMode(PIN_ABSBUSY,OUTPUT);
  digitalWrite(PIN_ABSBUSY,HIGH);
  delay(100);
  digitalWrite(PIN_ABSBUSY,LOW);
  pinMode(PIN_ABSBUSY,INPUT);
}

void loop()
{
  digitalWrite(PIN_UNO_LED, !(digitalRead(PIN_UNO_LED)) );
  delay(100);
}
