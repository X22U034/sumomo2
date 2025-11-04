void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(7, INPUT);
  pinMode(15, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  Serial.print(pulseIn(7, HIGH, 30000));
  Serial.print(", ");
  Serial.println();
}
