#include "Linescanner.h"
#include "LateralPIDController.h"
#include "LongitudinalPIDController.h"
#include "math.h"
#include "Servo.h"
#include "CommandLine.h"
#include "GLOBAL.h"

/* -------------------------------
 *  Behavior variables, liable to
 *  change from run to run
 * ------------------------------- */
byte debug    = 1;
byte motorOn  = 0;
byte servoOn  = 0; 
byte lineWidth;
byte _speed   = 0;

Servo servo;
byte angle  = CENTER;

int threshold;

// Bluetooth
byte inByte = 0;
byte ack    = 0;

// Linescanner
const unsigned int expose = 7390;
Linescanner camHigh (CLKHigh, syncHigh, dataHigh, expose);
Linescanner camLow  (CLKLow, syncLow, dataLow, expose);

// PID Controller
LongitudinalPIDController conHigh;
LateralPIDController conLow;

// Command Line
CommandLine cmd;

/* ---------------------------------
 * BEGIN
 * --------------------------------- */
void setup() {
  // prep high Linescanner
  pinMode(syncHigh, OUTPUT);
  pinMode(CLKHigh,  OUTPUT);
  camHigh.setLineWidth();

  // prep low Linescanner
  pinMode(syncLow,  OUTPUT);
  pinMode(CLKLow,   OUTPUT);
  camLow.setLineWidth();

  BTSerial.begin(9600);

  // servo
  servo.attach(SERVO_PIN);
  servo.write(angle);
  
  // commands
  cmd.printCmdList();

  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
}

/* ---------------------------------
 * GET LINE
 * --------------------------------- */
void getline(Linescanner & cam) {
  cam.getline();
  if (debug) cam.printDigital();
}


/* --------------------------------- COMMAND DEFINITIONS --------------------------------- */
void LongPID() {

  float error;
  byte _status = camHigh.checkLongCases(debug);
  
  if ( _status == 0) {
    error = 15;
  } else if (_status == 1) {
    error = 5;
  } else {
    error = camHigh.calcError();
  }
  
  if (error > 10) {
    _speed = 26;
  } else {
    _speed = 51;
  }
  
  analogWrite(MOTOR_PIN, _speed);
}

void LatPID() {
  if (camLow.checkLatCases(angle, servo, debug)) { return; }
  
  float error = camLow.calcError();
  
  angle = CENTER - conLow.pid(error);

  if (debug) {
     camLow.printDigital();
  }

  servo.write(angle);
}

/* --------------------------------- BEGIN COMMAND LIST ---------------------------------- */
// Performs BTSerial command based on user input
bool doSerialCmd( byte cmdl ) {
  switch( cmdl ) {
    // SERVO COMMANDS
    case ('w'):
      // center servo
      servo.write(CENTER);
      cmd.printNewCmdLn();
      break;
    case ('d'):
      // right servo
      servo.write(RIGHT);
      cmd.printNewCmdLn();
      break;
    case ('a'):
      // left servo
      servo.write(LEFT);
      cmd.printNewCmdLn();
      break;

    // MOTOR COMMANDS
    case ('1'):
      _speed = 26;
      BTSerial.print("MOTOR AT ");
      BTSerial.println(_speed);
      cmd.printNewCmdLn();
      break;
    // Turn LED LOW
    case ('2'):
      _speed = 51;
      cmd.printNewCmdLn();
      BTSerial.print("MOTOR AT ");
      BTSerial.println(_speed);
      break;
    // Turn LED LOW
    case ('3'):
      _speed = 77;
      BTSerial.print("MOTOR AT ");
      BTSerial.println(_speed);
      cmd.printNewCmdLn();
      break;
    // Turn LED LOW
    case ('4'):
      _speed = 102;
      cmd.printNewCmdLn();
      BTSerial.print("MOTOR AT ");
      BTSerial.println(_speed);
      break;
    // Turn LED LOW
    case ('5'):
      _speed = 128;
      cmd.printNewCmdLn();
      BTSerial.print("MOTOR AT ");
      BTSerial.println(_speed);
      break;
    // Turn LED LOW
    case ('6'):
      _speed = 153;
      cmd.printNewCmdLn();
      BTSerial.print("MOTOR AT ");
      BTSerial.println(_speed);
      break;
    // Turn LED LOW
    case ('7'):
      _speed = 179;
      cmd.printNewCmdLn();
      BTSerial.print("MOTOR AT ");
      BTSerial.println(_speed);
      break;
    // Turn LED LOW
    case ('8'):
      _speed = 204;
      cmd.printNewCmdLn();
      BTSerial.print("MOTOR AT ");
      BTSerial.println(_speed);
      break;
    // Turn LED LOW
    case ('9'):
      _speed = 230;
      cmd.printNewCmdLn();
      BTSerial.print("MOTOR AT ");
      BTSerial.println(_speed);
      break;      
    case ('o'):
      _speed = 255;
      cmd.printNewCmdLn();
      BTSerial.print("MOTOR AT ");
      BTSerial.println(_speed);
      break;      
    case ('0'):
      _speed = 0;
      cmd.printNewCmdLn();
      BTSerial.print("MOTOR AT ");
      BTSerial.println(_speed);
      break;
      
    // PID CONFIG
    case ('l'):
      conLow.incP();
      BTSerial.print("kP AT ");
      BTSerial.println(conLow.getP());
      cmd.printNewCmdLn();
      break;
    case ('k'):
      conLow.decP();
      BTSerial.print("kP AT ");
      BTSerial.println(conLow.getP());
      cmd.printNewCmdLn();
      break;
    case ('m'):
      conLow.incD();
      BTSerial.print("kD AT ");
      BTSerial.println(conLow.getD());
      cmd.printNewCmdLn();
      break;
    case ('n'):
      conLow.decD();
      BTSerial.print("kD AT ");
      BTSerial.println(conLow.getD());
      cmd.printNewCmdLn();
      break;
      
    case ('x'):
      BTSerial.println(NOT_YET_STR);
      cmd.printNewCmdLn();
    case ('z'):
      BTSerial.println(NOT_YET_STR);
      cmd.printNewCmdLn();
      break;

    // MODE COMMANDS 
    case ('h'):
      cmd.printCmdList();
      cmd.printNewCmdLn();
      break;
    case ('r'):
      servoOn = 1;
      motorOn = 1;
      run();
      cmd.printNewCmdLn();
      break;
    case ('t'):
      servoOn = 0;
      motorOn = 1;
      run();
      cmd.printNewCmdLn();
      break;
    case ('s'):
      servoOn = 1;
      motorOn = 0;
      run();
      cmd.printNewCmdLn();
      break;
    case ('c'):
      servoOn = 0;
      motorOn = 0;
      run();
      cmd.printNewCmdLn();
      break;

    // MISC
    case ('g'):
      debug = debug^1;
      BTSerial.print("Debug is ");
      BTSerial.println(debug);
      cmd.printNewCmdLn();

    // RUN INSTRUCTIONS
    case ('e'):
      analogWrite(MOTOR_PIN, 0);
      servo.write(angle = CENTER);
      return true;
    case (NONE):
      return false;
  }
  return false;
}
/* ---------------------------------- END COMMAND LIST ----------------------------------- */

/* ----------------------
 *  LOOP
 * ---------------------- */
void loop() {
  while (!ack) {
    ack = cmd.getAck();    
    delay(3000);
  }

  doSerialCmd(cmd.getSerialCmd());
}

void run() {
  camLow.setLineWidth();
  while(!doSerialCmd(cmd.getSerialCmd())) {      
      if (servoOn)
        LatPID();

      if (motorOn) {
        LongPID();
      }
  
      if (!motorOn && !servoOn) {
        camHigh.getline();
        camLow.getline();
        camHigh.printDigital();
        camLow.printDigital();
      }
    }
  }
