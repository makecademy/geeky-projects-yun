// Sketch to log energy data in the cloud

// Include libraries
#include <Bridge.h>
#include <Temboo.h>
#include <Process.h>
#include "TembooAccount.h"
#include <YunServer.h>
#include <YunClient.h>

// Time for the date process
unsigned long time;

// Define current sensor & relay pins
#define CURRENT_SENSOR A0
#define RELAY_PIN 8

// Define measurement variables
float amplitude_current;
float effective_value;
float effective_voltage = 230; // Set voltage to 230V (Europe) or 110V (US)
float effective_power;
float zero_sensor;
int measurements_interval = 0;
int last_measurement = 0;

// Create Yun server
YunServer server;

// Timing for measurements & server updates
int server_poll_time = 50;
int power_measurement_delay = 10000;
int power_measurement_cycles_max = power_measurement_delay/server_poll_time;
int power_measurement_cycles = 0;

// Process to get the measurement time
Process date;

// Your Google Docs data
const String GOOGLE_USERNAME = "yourGoogleUsername";
const String GOOGLE_PASSWORD = "yourGooglePass";
const String SPREADSHEET_TITLE = "Power";

void setup() {
  
  // Start bridge
  Bridge.begin();
  
  // Start serial
  Serial.begin(115200);
  
  // Start date process
  time = millis();
  if (!date.running())  {
    date.begin("date");
    date.addParameter("+%D-%T");
    date.run();
  }
    
  // Initialize relay pin
  pinMode(RELAY_PIN,OUTPUT);
 
  // Start server
  server.listenOnLocalhost();
  server.begin();
  
  // Calibrate sensor with null current
  zero_sensor = getSensorValue();
  
  // Init last measurement
  last_measurement = millis();
}

void loop() {
  
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }  
  
    // Log data at regular intervals
    if (power_measurement_cycles > power_measurement_cycles_max) {
     Serial.println("\nCalling the /Library/Google/Spreadsheets/AppendRow Choreo...");
     
     // Perform power measurement
     float sensor_value = getSensorValue();
   
     // Get measurement interval
     measurements_interval = millis() - last_measurement;
     last_measurement = millis();
     
     // Convert to current
     amplitude_current=(float)(sensor_value-zero_sensor)/1024*5/185*1000000;
     effective_value=amplitude_current/1.414;

     // Calculate power
     float effective_power = abs(effective_value * effective_voltage/1000);
     
     // Log data in Google Docs
     runAppendRow(measurements_interval,effective_power);
     power_measurement_cycles = 0;
    }
    
  // Repeat every 50 ms
  delay(server_poll_time);
  power_measurement_cycles++;
}

void runAppendRow(int measurements_interval, float effectiveValue) {
  TembooChoreo AppendRowChoreo;

  // Invoke the Temboo client
  AppendRowChoreo.begin();

  // Set Temboo account credentials
  AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
  AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);
  
  // Identify the Choreo to run
  AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");

  // your Google username (usually your email address)
  AppendRowChoreo.addInput("Username", GOOGLE_USERNAME);

  // your Google account password
  AppendRowChoreo.addInput("Password", GOOGLE_PASSWORD);
  
  // the title of the spreadsheet you want to append to
  AppendRowChoreo.addInput("SpreadsheetTitle", SPREADSHEET_TITLE);
  
  // restart the date process:
  if (!date.running())  {
    date.begin("date");
    date.addParameter("+%D-%T");
    date.run();
   }
  
  // Get date
  String timeString = date.readString(); 
  
  // Format data
  String data = "";
  data = data + timeString + "," + String(measurements_interval) + "," + String(effectiveValue);

  // Set Choreo inputs
  AppendRowChoreo.addInput("RowData", data);

  // Run the Choreo
  unsigned int returnCode = AppendRowChoreo.run();

  // A return code of zero means everything worked
  if (returnCode == 0) {
    Serial.println("Completed execution of the /Library/Google/Spreadsheets/AppendRow Choreo.\n");
  } else {
    // A non-zero return code means there was an error
    // Read and print the error message
    while (AppendRowChoreo.available()) {
      char c = AppendRowChoreo.read();
      Serial.print(c);
    }
    Serial.println();
  }
  
  AppendRowChoreo.close();
}

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
