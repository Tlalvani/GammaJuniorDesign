int hall_effect = 12;


void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(hall_effect, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  int val = digitalRead(hall_effect);
//  Serial.print(val);
  if (val == 0) {
    Serial.print("Magnet detected");
    
    }
}
