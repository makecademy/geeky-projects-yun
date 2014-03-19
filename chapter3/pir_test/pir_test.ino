// Test of the PIR motion sensor

// Declare PIR pin
const int sensor_pin = 8;

void setup() {
  
  // Start Serial and wait
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  
  // Get sensor value
  boolean sensor_value = digitalRead(sensor_pin);
  
  // Print on serial
  Serial.println(sensor_value);
  delay(100);
}
