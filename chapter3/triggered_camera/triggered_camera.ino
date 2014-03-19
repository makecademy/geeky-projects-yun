// Sketch to test the camera & PIR sensor
#include <Bridge.h>
#include <Process.h>

// Picture process
Process picture;

// Filename
String filename;

// Pin
int pir_pin = 8;

// Path
String path = "/mnt/sda1/";

void setup() {
  
  // Bridge
  Bridge.begin();
  
  // Set pin mode
  pinMode(pir_pin,INPUT);
}

void loop(void) 
{
  
  if (digitalRead(pir_pin) == true) {
    
    // Generate filename with timestamp
    filename = "";
    picture.runShellCommand("date +%s");
    while(picture.running());

    while (picture.available()>0) {
      char c = picture.read();
      filename += c;
    } 
    filename.trim();
    filename += ".png";
 
    // Take picture and save it locally
    picture.runShellCommand("fswebcam " + path + filename + " -r 1280x720");
    while(picture.running());
  }
}
