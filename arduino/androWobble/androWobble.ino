// This is the library for the TB6612 that contains the class Motor and all the
// functions
#include <SparkFun_TB6612.h>

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
// the default pins listed are the ones used on the Redbot (ROB-12097) with
// the exception of STBY which the Redbot controls with a physical switch
#define AIN1 2
#define BIN1 7
#define AIN2 4
#define BIN2 8
#define PWMA 5
#define PWMB 6
#define STBY 9

#define BEGIN_CHAR 0xBE

#define DIRECTION_MASK(x) (x & 0x02)
#define MOTOR_MASK(x)      (x & 0x01)

#define FORWARD 0
#define BACKWARD 1

#define MOTOR_ONE 0
#define MOTOR_TWO 1




// these constants are used to allow you to make your motor configuration 
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

static int motor1Val = 0;
static int motor2Val = 0;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

void protectBounds(int * motorVal) {
  if(*motorVal < -255) {
    *motorVal = -255;
  } else if (*motorVal > 255) {
    *motorVal = 255;
  }
}
void setup()
{
 Serial.begin(9600);
}

void loop()
{
  if (Serial.available() > 0) { 
    if(Serial.read() ==  BEGIN_CHAR) {
      int optionsByte = Serial.read();

      int motorChoice =  MOTOR_MASK(optionsByte);
      int directionChoice = DIRECTION_MASK(optionsByte);

      int motorVal = Serial.read();
      protectBounds(&motorVal);
      
      if(motorChoice == MOTOR_ONE) {
        if(directionChoice == FORWARD) {
          motor1.drive(motorVal);
        } else if (directionChoice == BACKWARD) {
          motor1.drive(-1*motorVal);
        }
      } else if (motorChoice == MOTOR_TWO) {
        if(directionChoice == FORWARD) {
          motor2.drive(motorVal);
        } else if (directionChoice == BACKWARD) {
          motor2.drive(-1*motorVal);
        }     
      }
    }
  }
}
