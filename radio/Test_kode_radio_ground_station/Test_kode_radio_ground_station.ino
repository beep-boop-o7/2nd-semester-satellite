String latestInput = "0;0";
String incommingMsg = "";

// ================= SETUP =================
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  Serial.println("Serial-node klar");
  Serial.println("Indtast: x;y");
}

// ================= LOOP =================
void loop() {

  // ✅ SEND KUN når der kommer nyt input
  if (Serial.available()) {

    latestInput = Serial.readStringUntil('\n');
    latestInput.trim();

    sendData();
  }

  // ✅ optional RX (debug)
  while (Serial1.available()) {
    char c = Serial1.read();
    Serial.write(c);
  }
}

// ================= TX ====================
void sendData() {
  String out = "DATA:" + latestInput;
  Serial1.println(out);

  Serial.print("Sendt: ");
  Serial.println(out);
}