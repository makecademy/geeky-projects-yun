// Sketch for the Arduino Uno board of the robot
// The sketch receives commands from the Yun board 
// and applies them to the robot

// Libraries
#include <Wire.h>

// Motor pins
int speed_motor1 = 6;  
int speed_motor2 = 5;
int direction_motor1 = 7;
int direction_motor2 = 4;

// Sensor pins
int distance_sensor = A0;

void setup()
{
  // Set pin 4,5,6,7 to output mode
  for(int i=4;i<=7;i++)
  {  
    pinMode(i, OUTPUT);  
  }
  
  // Join i2c bus with address #4 & receive events
  Wire.begin(4);                
  Wire.onReceive(receiveEvent);

  // Start serial for debug purposes
  Serial.begin(9600);           
}

void loop()
{
  delay(100);
}

// Function that executes whenever data is received from the Yun
void receiveEvent(int howMany)
{
  // Read all data for motor commands
  int pwm1 = Wire.read();
  Serial.print(pwm1); 
  char c = Wire.read();
  Serial.print(c);
  
  int dir1 = Wire.read();
  Serial.print(dir1); 
  c = Wire.read();
  Serial.print(c);
  
  int pwm2 = Wire.read();
  Serial.print(pwm2); 
  c = Wire.read();
  Serial.print(c);
  
  int dir2 = Wire.read();
  Serial.println(dir2);
   
  // Apply these commands to the robot motors
  send_motor_command(speed_motor1,direction_motor1,pwm1,dir1);
  send_motor_command(speed_motor2,direction_motor2,pwm2,dir2);
}

// Function to command a given motor of the robot
void send_motor_command(int speed_pin, int direction_pin, int pwm, boolean dir)
{
  analogWrite(speed_pin,pwm); // Set PWM control, 0 for stop, and 255 for maximum speed
  digitalWrite(direction_pin,dir);
}
