#include <Servo.h>

#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SimpleTimer.h>
#include "remote.h"
#include "credential.h"



// Instances
RF24 radio(9, 10);
Servo servo;
SimpleTimer timer;

// Global vars
HeaterStatus_t hActualHeaterStatus;
HeaterStatus_t hAckedHeaterStatus;
HeaterStatus_t bHeaterStatus;
const byte hb_address[6] = RADIO_HB_ADDRESS;
const byte bh_address[6] = RADIO_BH_ADDRESS;
char mySalt[SALT_LENGTH], otherSalt[SALT_LENGTH];
unsigned long mySaltExpTime = 0;




HeaterStatus_t hGetHeaterStatus();
void toggleLED();
void setLEDStatus(LEDStatus_t ledStat);
boolean authDecodePkt(Message_t *msg, Packet_t *pkt);
void printHeaterStatus(HeaterStatus_t stat);
void printMsgInfo(Message_t *msg);
void processPkt(Packet_t pkt);
void processMsg(Message_t *msg);
void genNewMySalt();
void sendMySalt();


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
  servo.write(0);

  // Pin init
  pinMode(LED_PIN, OUTPUT);
  pinMode(PHOTOCELL_PIN, INPUT);

  // Var init
  hAckedHeaterStatus = HEATER_STAT_UNKNOWN;
  hActualHeaterStatus = hGetHeaterStatus();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (radio.available()) {
    Packet_t incomingPkt;
    radio.read(&incomingPkt, sizeof(incomingPkt));
    processPkt(incomingPkt);

    Serial.println(text);
    servo.write(20);
    delay(300);
    servo.write(0);
  }
  
  
}

HeaterStatus_t hGetHeaterStatus() {
  // Photocell D_OUT is high when brightness is below threshold
  if(digitalRead(PHOTOCELL_PIN)) {
    return HEATER_STAT_OFF;
  }
  else {
    return HEATER_STAT_ON;
  }
}

void toggleLED() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

void setLEDStatus(LEDStatus_t ledStat) {
  static int timerId = -1;
  if (timerId > -1) {
    timer.deleteTimer(timerId);
    timerId = -1;
  }
  switch(ledStat) {
    case LED_BLINK_FAST:
      timerId = timer.setInterval(LED_BLINK_FAST_INTVL, toggleLED);
      break;
    case LED_BLINK_SLOW:
      timerId = timer.setInterval(LED_BLINK_SLOW_INTVL, toggleLED);
      break;
    case LED_STAY_ON:
      digitalWrite(LED_PIN, LED_ON);

    case LED_STAY_OFF:
      digitalWrite(LED_PIN, LED_OFF);
      break;
  }
}

boolean authDecodePkt(Message_t *msg, Packet_t *pkt) {
  //TODO
  memcpy(msg, pkt->msg, sizeof(Message_t));  
  return true;
}


void printHeaterStatus(HeaterStatus_t stat) {
  swich(stat) {
    case(HEATER_STAT_OFF):
      Serial.println("Heater Off");
      break;
    case(HEATER_STAT_ON):
      Serial.println("Heater On");
      break;
    case(HEATER_STAT_UNKNOWN):
      Serial.println("Heater Unknown");
      break;
  }
}
void printMsgInfo(Message_t *msg) {
  switch(msg->op) {
    case(OP_NEW_SALT):
      Serial.println("OP_NEW_SALT");
      break;
    case(OP_REQ):
      Serial.println("OP_REQ");
      break;
    case(OP_STATUS):
      Serial.print("OP_STATUS");
      printHeaterStatus(msg->body.status);
      break;
    case(OP_ACK_STATUS):
      Serial.println("OP_ACK_STATUS");
      printHeaterStatus(msg->body.status);
      break;
    case(OP_OPERATE)      
      Serial.println("OP_OPERATE");
      break;
    case(OP_ACK_OPERATE):
      Serial.println("OP_ACK_OPERATE");
      break;
  }
}


void sendPacket(Packet_t *pkt) {
  //TODO auth part
}
void hSendHeaterCurrentStatus() {
  Packet_t pkt;
  memset(pkt, 0, sizeof(pkt));
  hActualHeaterStatus = hGetHeaterStatus();
  pkt.type = PKT_MESSAGE;
  pkt.msg.op = OP_STATUS;
  pkt.msg.body.op_status.status = hAckedHeaterStatus;
  sendPacket(pkt)
}

void processMsg(Message_t *msg) {
  switch(msg->op) {
    case(OP_NEW_SALT):
      memcpy(otherSalt, msg->salt, SALT_LENGTH);
      break;
    case(OP_REQ):
      hSendHeaterCurrentStatus();
      break;
    case(OP_STATUS):
      bHeaterStatus = msg->body.op_status.status;
      bAckStatus();
      break;
    case(OP_ACK_STATUS):
      hAckedHeaterStatus = msg->body.op_ack_status.status;
      break;
    case(OP_OPERATE):
      hAckedHeaterStatus = HEATER_STAT_UNKNOWN; 
      hAckOperate();
      hOperateServo();
      hSendHeaterCurrentStatus();
      break;
    case(OP_ACK_OPERATE):
      bHeaterStatus = HEATER_STAT_UPDATING;
      break;
  } 
}

void genNewMySalt() {
  //TODO
  mySaltExpTime = millis() + TIMEOUT;
}

void sendMySalt() {
  //TODO
}

void processPkt(Packet_t pkt) {
  Message_t msg;
  if (authDecodePkt(msg, incomingPkt)) {
    printMsgInfo(msg);
    processMsg(msg);
    switch(pkt->type) {
      case(PKT_SYN):
        genNewSalt();
        sendNewSalt();
        break;
      case(PKT_MESSAGE):
        processMsg(msg);
        break;
    }
  }
  else {
    Serial.println("Failed to authenticate incoming packet!");
  }
}
