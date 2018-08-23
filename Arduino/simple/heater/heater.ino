#include <Servo.h>
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

#define SERVO_REST_POS     0
#define SERVO_ACT_POS      90

#define UPDATE_PERIOD      2000


#define LED_ON          LOW
#define LED_OFF         HIGH

RF24 radio(9, 10);
Servo servo;
SimpleTimer timer;

const byte hb_address[6] = RADIO_HB_ADDRESS;
const byte bh_address[6] = RADIO_BH_ADDRESS;

void sendStatus();
void operateServo();

void setup() {
  // Initialize serial port
  Serial.begin(9600);
  
  // Initialize radio
  radio.begin();
  radio.openReadingPipe(0, bh_address);
  radio.openWritingPipe(hb_address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  // Servo init
  servo.attach(SERVO_PIN);
  servo.write(SERVO_REST_POS);

  // Pin init
  pinMode(LED_PIN, OUTPUT);
  pinMode(PHOTOCELL_PIN, INPUT);  

  // Send heater status periodically
  timer.setInterval(UPDATE_PERIOD, sendStatus);
}

void loop() {
  char buf[16];
  if(radio.available()) {
    radio.read(buf, sizeof(buf));
    if(buf[0] == 'g') {
      operateServo();
      sendStatus();
    }
  }
  timer.run();
}

void sendStatus() {
  char stat;
  radio.stopListening();
  // Photocell D_OUT is high when brightness is below threshold
  if(digitalRead(PHOTOCELL_PIN)) {
    // HEATER_STAT_OFF
    stat = '0';
    digitalWrite(LED_PIN, LED_OFF);
  }
  else {
    // HEATER_STAT_ON
    stat = '1';
    digitalWrite(LED_PIN, LED_ON);
  }
  radio.write(&stat, sizeof(stat));
  radio.startListening();
}

void operateServo() {
  servo.write(SERVO_ACT_POS);
  delay(500);
  servo.write(SERVO_REST_POS);
}
