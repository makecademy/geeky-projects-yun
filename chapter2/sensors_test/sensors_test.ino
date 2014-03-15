// Sketch to test the energy meter project

// Include libraries
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

// Create Yun server
YunServer server;

// Define current sensor & relay pins
#define CURRENT_SENSOR A0
#define RELAY_PIN 8

// Define measurement variables
float amplitude_current;
float effective_value;
float effective_voltage = 230; // Set voltage to 230V (Europe) or 110V (US)
float effective_power;
float zero_sensor;

void setup(void) 
{
  
  // Initialize serial connection  
  Serial.begin(115200);
  
  // Start bridge
  Bridge.begin();
  
  // Initialize relay pin
  pinMode(RELAY_PIN,OUTPUT);
  
  // Start server
  server.listenOnLocalhost();
  server.begin();
  
  // Calibrate sensor with null current
  zero_sensor = getSensorValue();
  Serial.print("Zero point sensor: ");
  Serial.println(zero_sensor);
  Serial.println("");
}

void loop(void) 
{
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }
  
  // Perform power measurement
  float sensor_value = getSensorValue();
  Serial.print("Sensor value: ");
  Serial.println(sensor_value);
  
  // Convert to current
  amplitude_current=(float)(sensor_value-zero_sensor)/1024*5/185*1000000;
  effective_value=amplitude_current/1.414;
  
  // Plot data
  Serial.println("Current amplitude (in mA): ");
  Serial.println(amplitude_current,1);
  Serial.println("Current effective value (in mA)");
  Serial.println(effective_value,1);
  Serial.println("Effective power (in W): ");
  Serial.println(abs(effective_value*effective_voltage/1000),1);
  Serial.println("");
  
  // Poll every 50ms
  delay(50);
}

// Process incoming command
void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');

  // is "digital" command?
  if (command == "digital") {
    digitalCommand(client);
  }

  // is "analog" command?
  if (command == "analog") {
    analogCommand(client);
  }

  // is "mode" command?
  if (command == "mode") {
    modeCommand(client);
  }
}

void digitalCommand(YunClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  }
  else {
    value = digitalRead(pin);
  }

  // Send feedback to client
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.println(value);

  // Update datastore key with the current pin value
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));
}

void analogCommand(YunClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/analog/5/120"
  if (client.read() == '/') {
    // Read value and execute command
    value = client.parseInt();
    analogWrite(pin, value);

    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" set to analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  }
  else {
    // Read analog pin
    value = analogRead(pin);

    // Send feedback to client
    client.print(F("Pin A"));
    client.print(pin);
    client.print(F(" reads analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "A";
    key += pin;
    Bridge.put(key, String(value));
  }
}

void modeCommand(YunClient client) {
  int pin;

  // Read pin number
  pin = client.parseInt();

  // If the next character is not a '/' we have a malformed URL
  if (client.read() != '/') {
    client.println(F("error"));
    return;
  }

  String mode = client.readStringUntil('\r');

  if (mode == "input") {
    pinMode(pin, INPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as INPUT!"));
    return;
  }

  if (mode == "output") {
    pinMode(pin, OUTPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as OUTPUT!"));
    return;
  }

  client.print(F("error: invalid mode "));
  client.print(mode);
}

// Get the reading from the current sensor
float getSensorValue()
{
	int sensorValue;
	float avgSensor = 0;
        int nb_measurements = 100;
        for (int i = 0; i < nb_measurements; i++) {
          sensorValue = analogRead(CURRENT_SENSOR);
          avgSensor = avgSensor + float(sensorValue);
        }	  
        avgSensor = avgSensor/float(nb_measurements);
	return avgSensor;
}
