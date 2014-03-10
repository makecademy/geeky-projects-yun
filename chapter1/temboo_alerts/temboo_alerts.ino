#include <Bridge.h>
#include <Temboo.h>
#include <Process.h>

// Contains Temboo account information
#include "TembooAccount.h" 

// Variables
int lightLevel;
float humidity;
float temperature;
float temperature_limit;
unsigned long time;

// Process to get the measurement time
Process date;

// Your Google Docs data
const String GOOGLE_USERNAME = "yourEmailAddress";
const String GOOGLE_PASSWORD = "yourPassword";
const String SPREADSHEET_TITLE = "Yun";
const String TO_EMAIL_ADDRESS = "destinationEmail";

// Include required libraries
#include "DHT.h"

// DHT11 sensor pins
#define DHTPIN 8 
#define DHTTYPE DHT11

// DHT instance
DHT dht(DHTPIN, DHTTYPE);

// Debug mode ?
boolean debug_mode = true;

void setup() {
  
  // Start Serial
  if (debug_mode == true){
    Serial.begin(115200);
    delay(4000);
    while(!Serial);
  }
  
  // Initialize DHT sensor
  dht.begin();
  
  // Start bridge
  Bridge.begin();
  
  // Start date process
  time = millis();
  if (!date.running())  {
    date.begin("date");
    date.addParameter("+%D-%T");
    date.run();
  }

  // Set temperature limit
  temperature_limit = 25.0;

  if (debug_mode == true){
    Serial.println("Setup complete. Waiting for sensor input...\n");
  }
}
void loop() {
  
  // Measure the humidity & temperature
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
    
  // Measure light level
  int lightLevel = analogRead(A0);

  if (debug_mode == true){
    Serial.println("\nCalling the /Library/Google/Spreadsheets/AppendRow Choreo...");
  }
  
  // Append data to Google Docs sheet
  runAppendRow(lightLevel, temperature, humidity);

  // Send email alert ?
  if (temperature < temperature_limit) {
     if (debug_mode == true){Serial.println("Sending alert");}
     sendTempAlert("Temperature is too low!");
  }
        
  // Repeat every 10 minutes
  delay(600000);
  
}

// Send email alert
void sendTempAlert(String message) {
  
  if (debug_mode == true){ Serial.println("Running SendAnEmail...");}
  
  TembooChoreo SendEmailChoreo;
   
  SendEmailChoreo.begin();
    
  // set Temboo account credentials
  SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
  SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);

  // identify the Temboo Library choreo to run (Google > Gmail > SendEmail)
  SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail");
    
  // set the required choreo inputs
  // see https://www.temboo.com/library/Library/Google/Gmail/SendEmail/ 
  // for complete details about the inputs for this Choreo

  // the first input is your Gmail email address
  SendEmailChoreo.addInput("Username", GOOGLE_USERNAME);
  // next is your Gmail password.
  SendEmailChoreo.addInput("Password", GOOGLE_PASSWORD);
  // who to send the email to
  SendEmailChoreo.addInput("ToAddress", TO_EMAIL_ADDRESS);
  // then a subject line
  SendEmailChoreo.addInput("Subject", "ALERT: Home Temperature");

  // next comes the message body, the main content of the email   
  SendEmailChoreo.addInput("MessageBody", message);

  // tell the Choreo to run and wait for the results. The 
  // return code (returnCode) will tell us whether the Temboo client 
  // was able to send our request to the Temboo servers
  unsigned int returnCode = SendEmailChoreo.run();

  // a return code of zero (0) means everything worked
  if (returnCode == 0) {
    if (debug_mode == true){Serial.println("Success! Email sent!");}
  } else {
    // a non-zero return code means there was an error
    // read and print the error message
    while (SendEmailChoreo.available()) {
      char c = SendEmailChoreo.read();
      if (debug_mode == true){Serial.print(c);}
    }
  } 
  SendEmailChoreo.close();
}

// Function to add data to Google Docs
void runAppendRow(int lightLevel, float temperature, float humidity) {
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
  
  // Restart the date process:
  if (!date.running())  {
    date.begin("date");
    date.addParameter("+%D-%T");
    date.run();
   }
  
  // Get date
  String timeString = date.readString(); 
  
  // Format data
  String data = "";
  data = data + timeString + "," + String(temperature) + "," + String(humidity) + "," + String(lightLevel);

  // Set Choreo inputs
  AppendRowChoreo.addInput("RowData", data);

  // Run the Choreo
  unsigned int returnCode = AppendRowChoreo.run();

  // A return code of zero means everything worked
  if (returnCode == 0) {
    if (debug_mode == true){
      Serial.println("Completed execution of the /Library/Google/Spreadsheets/AppendRow Choreo.\n");
    }
  } else {
    // A non-zero return code means there was an error
    // Read and print the error message
    while (AppendRowChoreo.available()) {
      char c = AppendRowChoreo.read();
      if (debug_mode == true){ Serial.print(c); }
    }
    if (debug_mode == true){ Serial.println(); }
  }
  AppendRowChoreo.close();
}

