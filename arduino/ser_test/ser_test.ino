#define L_IN1 3
#define L_IN2 9
#define R_IN1 10
#define R_IN2 11

void setup() {
  Serial.begin(24000);
}

char buf[255];
void loop() {
  if (Serial.available()) {
    unsigned char c = Serial.read();
    motorL_drive(1, c);
    Serial.println((int)c);
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
