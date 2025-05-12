int relayControlPin = 13; 
int moistureSensorPin = 0; 
int sensorValue;

void setup() {
  pinMode(relayControlPin, OUTPUT);  // Connect the signal pin of relay to pin 13 of Arduino
  pinMode(moistureSensorPin, INPUT); // Connect the signal pin of sensor to pin 0 of Arduino
}

void loop() { 
  sensorValue = digitalRead(moistureSensorPin); 
  
  if(sensorValue == LOW) {
    digitalWrite(relayControlPin, LOW);  // If sensor gives LOW value, turn relay off
  }
  else {
    digitalWrite(relayControlPin, HIGH); // If sensor gives HIGH value, turn relay on
  }
}
