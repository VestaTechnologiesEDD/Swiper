#define NUM_SPARKMAX 8

#define sign(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0))

#include <SerialCommands.h>
#include <Encoder.h>
#include <Servo.h>

// Sparkmax struct
struct Sparkmax {
  Servo servo;
  Encoder* encoder;
  Sparkmax(int pwm, int encA, int encB) {
    servo.attach(pwm, 1000, 2000);
    encoder = new Encoder(encA, encB);
  }
};

// Sparkmax objects
Sparkmax spkmx[] = {
  Sparkmax(2, A0, A1),
  Sparkmax(3, A2, A3),
  Sparkmax(4, A4, A5),
  Sparkmax(5, A6, A7),
  Sparkmax(6, A8, A9),
  Sparkmax(7, A10, A11),
  Sparkmax(8, A12, A13),
  Sparkmax(9, A14, A15),
};

// Set the motor speed
void set_motor_speed(int idx, int spd) {
  int dir = sign(spd);
  int val = 90;
  if (dir > 0) {
    val = map(spd, 0, 127, 90, 180);
  } else if (dir < 0) {
    val = map(spd, -127, 0, 0, 90);
  }
  spkmx[idx].servo.write(val);
}

// 32 byte buffer for the serial commands
char serial_command_buffer_[32];

// Serial command object attached to Serial
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");

// Called when no command matches
void cmd_unrecognized(SerialCommands* sender, const char* cmd) {
  sender->GetSerial()->print("Unrecognized command [");
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println("]");  
}

// Get the device identification
// get_device_name
void cmd_get_device_name(SerialCommands* sender) {
  sender->GetSerial()->println("swiper-cm"); 
}

// Read a motor encoder
// read_encoder <index>
void cmd_read_encoder(SerialCommands* sender) {
    char* idx_str = sender->Next();
  if (idx_str == NULL) {
    sender->GetSerial()->println("ERROR NO_IDX");
    return;
  }
  int idx = atoi(idx_str);

  int value = spkmx[idx].encoder->read();
  sender->GetSerial()->println(value);
}

// Set a motor speed at
// set_speed <index> <speed>
void cmd_set_speed(SerialCommands* sender) {
  char* idx_str = sender->Next();
  if (idx_str == NULL) {
    sender->GetSerial()->println("ERROR NO_IDX");
    return;
  }
  int idx = atoi(idx_str);

  char* spd_str = sender->Next();
  if (spd_str == NULL) {
    sender->GetSerial()->println("ERROR NO_SPD");
    return;
  }
  int spd = atoi(spd_str);

  // Set the motor speed
  set_motor_speed(idx, spd);

  sender->GetSerial()->println("OK");
}

SerialCommand cmd_get_device_name_("get_device_name", cmd_get_device_name);
SerialCommand cmd_read_encoder_("read_encoder", cmd_read_encoder);
SerialCommand cmd_get_set_speed_("set_speed", cmd_set_speed);

void setup() {
  // 115200 baudrate
  Serial.begin(115200);

  // Commands
  serial_commands_.SetDefaultHandler(cmd_unrecognized);
  serial_commands_.AddCommand(&cmd_get_device_name_);
  serial_commands_.AddCommand(&cmd_read_encoder_);
  serial_commands_.AddCommand(&cmd_get_set_speed_);
}

void loop() {
  serial_commands_.ReadSerial();
}
