// One time
void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("ARD1: Motor Controller Active");
}


// Repeatedly
void loop() {
  if (Serial.available() > 0) {
    Serial.print("ARD1: Received information: ");
    Serial.println(Serial.readStringUntil(';'));
    Serial.flush();
  }
  delay(30);
}