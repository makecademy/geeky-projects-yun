#include <Bridge.h>
#include <Temboo.h>
#include <Process.h>

// Contains Temboo account information
#include "TembooAccount.h" 

// Variables
int lightLevel;
float humidity;
float temperature;
unsigned long time;

// Process to get the measurement time
Process date;

// Your Twitter data
const String TWITTER_ACCESS_TOKEN = "yourTwitterAccessToken";
const String TWITTER_ACCESS_TOKEN_SECRET = "yourTwitterAccessTokenSecret";
const String TWITTER_API_KEY = "yourTwitterAPIKey";
const String TWITTER_API_SECRET = "yourTwitterAPISecret";

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

  // Send data on Twitter
  tweetAlert(lightLevel, temperature, humidity);
      
  // Repeat every minute
  delay(60000);
}

void tweetAlert(int light, float temperature, float humidity) {
  
    if (debug_mode == true){Serial.println("Running SendATweet");}
    
    TembooChoreo StatusesUpdateChoreo;
    
    // Text to tweet
    String tweetText = "Temperature: " + String(temperature) + ", Humidity: " + String(humidity) + ", Light level: " + String(light); 

    // invoke the Temboo client
    // NOTE that the client must be reinvoked, and repopulated with
    // appropriate arguments, each time its run() method is called.
    StatusesUpdateChoreo.begin();
    
    // set Temboo account credentials
    StatusesUpdateChoreo.setAccountName(TEMBOO_ACCOUNT);
    StatusesUpdateChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    StatusesUpdateChoreo.setAppKey(TEMBOO_APP_KEY);

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twitter/Tweets/StatusesUpdate/ 
    // for complete details about the inputs for this Choreo
    
    // identify the Temboo Library choreo to run (Twitter > Tweets > StatusesUpdate)
    StatusesUpdateChoreo.setChoreo("/Library/Twitter/Tweets/StatusesUpdate");
 
    // add the Twitter account information
    StatusesUpdateChoreo.addInput("AccessToken", TWITTER_ACCESS_TOKEN);
    StatusesUpdateChoreo.addInput("AccessTokenSecret", TWITTER_ACCESS_TOKEN_SECRET);
    StatusesUpdateChoreo.addInput("ConsumerKey", TWITTER_API_KEY);  
    StatusesUpdateChoreo.addInput("ConsumerSecret", TWITTER_API_SECRET);
   
    // and the tweet we want to send
    StatusesUpdateChoreo.addInput("StatusUpdate", tweetText);

    // tell the Process to run and wait for the results. The 
    // return code (returnCode) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = StatusesUpdateChoreo.run();

    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
        if (debug_mode == true){Serial.println("Success! Tweet sent!");}
    } else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (StatusesUpdateChoreo.available()) {
        char c = StatusesUpdateChoreo.read();
        if (debug_mode == true){Serial.print(c);}
      }
    } 
    StatusesUpdateChoreo.close();

}
