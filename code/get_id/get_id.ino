void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  int id = ESP.getChipId();

  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.print("id = ");
  Serial.print(id);
  Serial.println("");
}

void loop() {
  // put your main code here, to run repeatedly:

}
