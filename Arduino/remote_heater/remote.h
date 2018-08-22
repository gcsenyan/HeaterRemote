
// Pin assignments
#define BUTTON_PIN      3
#define SERVO_PIN       5
#define LED_PIN         8
#define PHOTOCELL_PIN   4

// Alias
#define LED_ON          LOW
#define LED_OFF         HIGH


// Protocol
#define TIMEOUT         2000

typedef enum {
  OP_REQUEST,
  OP_COS,
  OP_ACK_COS,
  OP_OPERATE,
  OP_ACK_OPERATE
}OpCode_t;

typedef enum {
  HEATER_OFF,
  HEATER_ON,
  HEATER_UNKNOWN
}HeaterStatus_t;

