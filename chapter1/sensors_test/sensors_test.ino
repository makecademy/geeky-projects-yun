// Test sketch for the DHT11 & photocell sensors

// Variables
int lightLevel;
float humidity;
float temperature;

// Include required libraries
#include "DHT.h"

// DHT11 sensor pins
#define DHTPIN 8 
#define DHTTYPE DHT11

// DHT instance
DHT dht(DHTPIN, DHTTYPE);
                                         
void setup(void)
{
 
  // Initialize DHT sensor
  dht.begin();
  
  // Init serial
  Serial.begin(115200);
  
}
  
void loop(void)
{
  
    // Measure the humidity & temperature
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    // Measure light level
    int lightLevel = analogRead(A0);
    
    // Print measurements
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Light level: ");
    Serial.println(lightLevel);
    Serial.println("");
    
    // Repeat 50 ms
    delay(50);
  
}
