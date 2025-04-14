#define SW1 4
#define SW2 19

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("SW1: ");
  Serial.print(digitalRead(SW1));
  Serial.print("   SW2: ");
  Serial.println(digitalRead(SW2));
  delay(1000);


}
