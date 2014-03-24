// Simple robot test

// Motor pins
int speed_motor1 = 6;  
int speed_motor2 = 5;
int direction_motor1 = 7;
int direction_motor2 = 4;

// Sensor pins
int distance_sensor = A0;

// Speed
int robot_speed;

void setup() {
  
  // Start Serial
  Serial.begin(115200);
  
  // Set pin 4,5,6,7 to output mode
  for(int i=4;i<=7;i++)
  {  
    pinMode(i, OUTPUT);
  }
  // Init speed
  robot_speed = 75;
}

void loop() {
  
  // Send motor commands to the robot
  send_motor_command(speed_motor1,direction_motor1,robot_speed,1);
  send_motor_command(speed_motor2,direction_motor2,robot_speed,1);

  // Read analog data
  Serial.println(measure_distance(A0));
  delay(50);
  
  // Update speed
  robot_speed++;
  if (robot_speed > 255) {robot_speed = 75;}
           
}

// Function to command a given motor of the robot
void send_motor_command(int speed_pin, int direction_pin, int pwm, boolean dir)
{
  analogWrite(speed_pin,pwm); // Set PWM control, 0 for stop, and 255 for maximum speed
  digitalWrite(direction_pin,dir);
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
