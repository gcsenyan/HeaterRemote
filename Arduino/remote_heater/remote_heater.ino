#include <Servo.h>

#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include "remote.h"



#define TIMEOUT     2000

//#define LED_OFF     HIGH
//#define LED_ON      LOW

RF24 radio(9, 10);
Servo servo;
const byte address[6] = "00001"; // 5 bytes address

typedef enum {
  HEATER_OFF,
  HEATER_ON,
  HEATER_UNKNOWN
}HeaterStatus_t;

HeaterStatus_t actualHeaterStatus = HEATER_UNKNOWN;
HeaterStatus_t ackedHeaterStatus = HEATER_UNKNOWN;



HeaterStatus_t getHeaterStatus();


void setup() {
 Serial.begin(9600);
  // Initialize radio
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  // Servo
  servo.attach(SERVO_PIN);
  servo.write(0);

  // Pin init
  pinMode(LED_PIN, OUTPUT);
  pinMode(PHOTOCELL_PIN, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    servo.write(20);
    delay(300);
    servo.write(0);
  }
  
  actualHeaterStatus = getHeaterStatus();
  
  
}

HeaterStatus_t getHeaterStatus() {
  // Photocell D_OUT is high when brightness is below threshold
  if(digitalRead(PHOTOCELL_PIN)) {
    digitalWrite(LED_PIN, LED_OFF);
    return HEATER_OFF;
  }
  else {
    digitalWrite(LED_PIN, LED_ON);
    return HEATER_ON;
  }
}

