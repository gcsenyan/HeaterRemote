
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define BUTTON_PIN  3
#define LED_PIN     8

#define TIMEOUT     2000

#define LED_OFF     HIGH
#define LED_ON      LOW


RF24 radio(9, 10);

const byte address[6] = "00001"; // 5 bytes address

byte led_status = 0;

void setup() {
  // Initialize radio
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();

  // Button and LED
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, led_status);
  

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

void sendCmd() {
  const char str[] = "go";
  radio.write(&str, sizeof(str));
  //delay(100);
}

void loop() {
  if(isButtonPushed(BUTTON_PIN)) {
    sendCmd();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
}
