#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SimpleTimer.h>


// Pin assignments
#define BUTTON_PIN        3
#define SERVO_PIN         5
#define LED_PIN           8
#define PHOTOCELL_PIN     4


#define RADIO_BH_ADDRESS   "00001"
#define RADIO_HB_ADDRESS   "00002"

#define UPDATE_PERIOD   2000

#define LED_ON          LOW
#define LED_OFF         HIGH

RF24 radio(9, 10);
SimpleTimer timer;

const byte hb_address[6] = RADIO_HB_ADDRESS;
const byte bh_address[6] = RADIO_BH_ADDRESS;
unsigned long lastUpdateTime = 0;
int blinkTimerId = -1;

boolean isButtonPushed(int button_pin);
void toggleLED();

void setup() {
  // Initialize serial port
  Serial.begin(9600);
  
  // Initialize radio
  radio.begin();
  radio.openReadingPipe(0, hb_address);
  radio.openWritingPipe(bh_address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  // Pin init
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);  
}

void loop() {
  char buf[16];
  char cmd = 'g';
  if(radio.available()) {
    radio.read(buf, sizeof(buf));
    if(blinkTimerId > -1) {
      timer.deleteTimer(blinkTimerId);
      blinkTimerId = -1;
    }
    if(buf[0] == '1') 
      digitalWrite(LED_PIN, LED_ON);
    else
      digitalWrite(LED_PIN, LED_OFF);
    lastUpdateTime = millis();
  }
  if(isButtonPushed(BUTTON_PIN)) {
    radio.stopListening();
    radio.write(&cmd, sizeof(cmd));
    radio.startListening();
  }
  if(((millis() - lastUpdateTime) > 3 * UPDATE_PERIOD) && blinkTimerId == -1)
    blinkTimerId = timer.setInterval(500, toggleLED);
  timer.run();
}

boolean isButtonPushed(int button_pin) {
  if (!digitalRead(button_pin)) {
    delay(100);
    if (!digitalRead(button_pin)) {
      while(!digitalRead(button_pin));   // Wait until released.
      return true;
    }
  }
  return false;
}

void toggleLED() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}