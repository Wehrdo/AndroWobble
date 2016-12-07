#define L_IN1 3
#define L_IN2 9
#define R_IN1 10
#define R_IN2 11

#define BEGIN_CHAR 0xBE

#define LEFT_MOTOR_DIR_MASK(x)      (x & 0x01)
#define RIGHT_MOTOR_DIR_MASK(x)      (x & 0x02)

void protectBounds(int * motorVal) {
  if(*motorVal < -255) {
    *motorVal = -255;
  } else if (*motorVal > 255) {
    *motorVal = 255;
  }
}

void setup()
{
  pinMode(L_IN1, OUTPUT);
  pinMode(L_IN2, OUTPUT);
  pinMode(R_IN1, OUTPUT);
  pinMode(R_IN2, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available() > 0) { 
    if(Serial.read() ==  BEGIN_CHAR) {
      int optionsByte = Serial.read();

      int leftMotorDir =  LEFT_MOTOR_DIR_MASK(optionsByte);
      int rightMotorDir =  RIGHT_MOTOR_DIR_MASK(optionsByte);

      int leftMotorVal= Serial.read();
      int rightMotorVal = Serial.read();
      //protectBounds(&leftMotorVal);
      //protectBounds(&rightMotorVal);

      motorL_drive(leftMotorDir, leftMotorVal);
      motorR_drive(rightMotorDir, rightMotorVal);
    }
  }
}


void motorL_drive(int forward, int val) {
  val = 255 - val;
  // Forward == clockwise, current from OUT1 to OUT2
  if (forward) {
    digitalWrite(L_IN1, HIGH);
    analogWrite(L_IN2, val);
  }
  else {
    analogWrite(L_IN1, val);
    digitalWrite(L_IN2, HIGH);
  }
}


void motorR_drive(int forward, int val) {
  val = 255 - val;
  // Forward == clockwise, current from OUT1 to OUT2
  if (forward) {
    digitalWrite(R_IN1, HIGH);
    analogWrite(R_IN2, val);
  }
  else {
    analogWrite(R_IN1, val);
    digitalWrite(R_IN2, HIGH);
  }
}
