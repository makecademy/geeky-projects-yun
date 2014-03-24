// Sketch for the Arduino Yun board of the robot
// The sketch transmits commands from the Uno board 

// Libraries
#include <Wire.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

// Yun server
YunServer server;

// Sensor pins
int distance_sensor_pin = A0;

void setup()
{

  // Join i2c bus (address optional for master)
  Wire.begin();
  
  // Start bridge
  Bridge.begin();
  
  // Start Yun server
  server.listenOnLocalhost();
  server.begin();
}

void loop()
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

  // Poll every 50ms
  delay(50); 
  
}

// Process called when the REST API is called
void process(YunClient client) {
  
  // Read the command
  String command = client.readStringUntil('/');

  if (command == "robot") {   
     robotCommand(client);
  }

}

// Process robot commands
void robotCommand(YunClient client) {
  
  // Read command and react accordingly
  String command = client.readStringUntil('\r');
  
  if (command == "stop") {
    Wire.beginTransmission(4);
    Wire.write(0);
    Wire.write(",");
    Wire.write(0);
    Wire.write(",");
    Wire.write(0);
    Wire.write(",");
    Wire.write(0);
   Wire.endTransmission();
  }
  
  if (command == "fullfw") {
    Wire.beginTransmission(4); 
    Wire.write(100);
    Wire.write(",");
    Wire.write(0);
    Wire.write(",");
    Wire.write(100);
    Wire.write(",");
    Wire.write(0);
    Wire.endTransmission();
  }
  
  if (command == "turnleft") {
    Wire.beginTransmission(4); 
    Wire.write(75);
    Wire.write(",");
    Wire.write(0);
    Wire.write(",");
    Wire.write(75);
    Wire.write(",");
    Wire.write(1);
    Wire.endTransmission();    
  }
  
  if (command == "turnright") {
    Wire.beginTransmission(4); 
    Wire.write(75);
    Wire.write(",");
    Wire.write(1);
    Wire.write(",");
    Wire.write(75);
    Wire.write(",");
    Wire.write(0);
    Wire.endTransmission(); 
  }
  
   // Return distance measurement
   if (command == "getdistance") {
     client.print("The distance in front of the robot is: ");
     client.print(measure_distance(distance_sensor_pin));
     client.println(" cm.");
  }
  
}

// Measure distance from the ultrasonic sensor
int measure_distance(int pin){
  
  unsigned int Distance=0;
  unsigned long DistanceMeasured=pulseIn(pin,LOW);
  
  if(DistanceMeasured==50000){              // the reading is invalid.
      Serial.print("Invalid");    
   }
    else{
      Distance=DistanceMeasured/50;      // every 50us low level stands for 1cm
   }
   
  return Distance;
}
