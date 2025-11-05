void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // for (int i = 0; i < 9; i++) {
    pinMode(5, INPUT);
  //}
}

void loop() {
  // put your main code here, to run repeatedly:
  // for (int i = 0; i < 9; i++) {
    Serial.print(pulseIn(5, HIGH, 40000));
    Serial.print(", ");
  // }
  Serial.println();
}
