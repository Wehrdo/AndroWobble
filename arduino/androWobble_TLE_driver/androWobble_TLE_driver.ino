#define L_IN1 3
#define L_IN2 9
#define R_IN1 10
#define R_IN2 11


void setup() {
  pinMode(L_IN1, OUTPUT);
  pinMode(L_IN2, OUTPUT);
  pinMode(R_IN1, OUTPUT);
  pinMode(R_IN2, OUTPUT);
}

void loop() {
  motorL_drive(1, 255);
  delay(1000);
  motorL_drive(0, 255);
  delay(1000);
  motorL_drive(1, 0);
  delay(1000);
  
  motorR_drive(1, 255);
  delay(1000);
  motorR_drive(0, 255);
  delay(1000);
  motorR_drive(1, 0);
  delay(1000);
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
