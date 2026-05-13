// ===== MOTOR PINS =====
const int IN1 = 12;
const int IN2 = 11;
const int ENA = 10;

const int IN3 = 8;
const int IN4 = 7;
const int ENB = 9;

// ===== DATA =====
int receivedX = 0;
int receivedY = 0;
String incommingMsg = "";

float maxScale = 1.5 / 6;

// ================= SETUP =================
void setup() {
  Serial.begin(9600);
  //Serial1.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Serial.println("PWM-node klar");
}

// ================= LOOP =================
void loop() {

  receiveMsg();  // ✅ kør altid

  controlAxis(receivedX, IN1, IN2, ENA);
  controlAxis(receivedY, IN3, IN4, ENB);
}

// ================= RX ====================
void receiveMsg() {

  while (Serial.available()) {

    char c = Serial.read();
    incommingMsg += c;

    if (c == '\n') {

      incommingMsg.trim();

      //Serial.print("RX: ");
      //Serial.println(incommingMsg);

      if (incommingMsg.startsWith("DATA:")) {

        String data = incommingMsg.substring(5);
        int sep = data.indexOf(';');

        if (sep > 0) {
          receivedX = data.substring(0, sep).toInt();
          receivedY = data.substring(sep + 1).toInt();

          Serial.print("Parsed: ");
          Serial.print(receivedX);
          Serial.print(";");
          Serial.println(receivedY);
        }
      }

      incommingMsg = "";  // reset
    }
  }
}

// ================= MOTOR =================
int mapPWM(int percent) {
  int pwm = map(abs(percent), 0, 100, 0, 255);
  pwm *= maxScale;
  return constrain(pwm, 0, 255);
}

void controlAxis(int percent, int in1, int in2, int pwmPin) {
  int pwm = mapPWM(percent);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  if (percent > 0) digitalWrite(in1, HIGH);
  else if (percent < 0) digitalWrite(in2, HIGH);

  if (percent != 0) {
    analogWrite(pwmPin, pwm);
  } else if (percent == 0) {
    digitalWrite(pwmPin, HIGH);
  }
}