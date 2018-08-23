
// Pin assignments
#define BUTTON_PIN      3
#define SERVO_PIN       5
#define LED_PIN         8
#define PHOTOCELL_PIN   4


// Config
#define SERVO_REST_POS          0
#define SERVO_ACT_POS           90


#define LED_BLINK_FAST_INTVL    200
#define LED_BLINK_SLOW_INTVL    500

// Alias
#define LED_ON          LOW
#define LED_OFF         HIGH



typedef enum {
  LED_BLINK_FAST, 
  LED_BLINK_SLOW,
  LED_STAY_ON,
  LED_STAY_OFF
}LEDStatus_t;

// Protocol

// Packet struct (32 Bytes)
/*   PKT_TYPE(2)          SALT(12)               BODY(4)        TAG(14)
 *  [===========|=============================|==========|====================]
 *   0         1 2                          13 14     17
 *              |<================MSG(16)===============>|
 */

#define SALT_LENGTH     12
#define TAG_LENGTH      14
#define PKT_LENGTH      32
#define TIMEOUT         2000

typedef enum {
  PKT_SYN,
  PKT_MESSAGE,
}PacketType_t;

// Operation Code
typedef enum {
  OP_NEW_SALT,          // H <-> B, New salt to use
  OP_REQ,               // H <-  B, request current status from heater
  OP_STATUS,            // H  -> B, report report current heater status
  OP_ACK_STATUS,        // H <-  B, acknowledge the change of heater status 
  OP_OPERATE,           // H <-  B, operate heater switch once
  OP_ACK_OPERATE        // H ->  B, heater acknowledge the "operate" request
}OpCode_t;

typedef enum {
  HEATER_STAT_OFF,
  HEATER_STAT_ON,
  HEATER_STAT_UPDATING,
  HEATER_STAT_UNKNOWN
}HeaterStatus_t;

// Message struct, 4 bytes
typedef struct {
  char salt[SALT_LENGTH];
  OpCode_t op;
  union {
    // OP_REQ
    struct {
      char placeholder[2];
    }op_req;

    // OP_COS
    struct {
      HeaterStatus_t status;
    }op_status;

    // OP_ACK_COS
    struct {
      HeaterStatus_t status;
    }op_ack_status;

    // OP_OPERATE
    struct {
      char placeholder[2];
    }op_operate;

    // OP_ACK_OPERATE
    struct {
      char placeholder[2];
    }op_ack_operate;
  }body;
}Message_t;

// Packet struct, 32 bytes
typedef struct {
  PacketType_t type;
  Message_t msg;
  char tag[TAG_LENGTH];
}Packet_t;
