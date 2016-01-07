#include <ros.h>
#include <ArduinoHardware.h>
#include <Servo.h>
#include <SimpleTimer.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Twist.h>

//class NewHardware : public ArduinoHardware{
  
//public : NewHardware ( ) : ArduinoHardware(&Serial1,
//57600) {};
//};


ros::NodeHandle nh;

Servo servoLeft;
Servo servoRight;

SimpleTimer timer;
int stopid;

void moveRobot(double x, double z){
  double left = 1500 + 400 * x;
  double right = 1500 - 400 * x;
   left = left - z * 200;
   right = right - z * 200;
 
  servoLeft.writeMicroseconds(left);
  servoRight.writeMicroseconds(right);
}


void messageCb( const geometry_msgs::Twist& robot_controls){
  
  moveRobot(robot_controls.linear.x, robot_controls.angular.z);
  
  if(timer.isEnabled(stopid)) {  
    timer.restartTimer(stopid);
  } else{
    stopid = timer.setTimeout(500, stopRobot);
  }
}



ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", &messageCb );

void setup()
{
  pinMode(7, INPUT);
  pinMode(5, INPUT);
  nh.initNode();
  nh.subscribe(sub);
  servoLeft.attach(13);
  servoRight.attach(12);
  stopRobot(); 
  stopid = timer.setTimeout(500, stopRobot);
}




void stopRobot(){
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
}



void moveForward(){
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1300);
}

void turnRight(){
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1700);
}

void turnLeft(){   
  servoLeft.writeMicroseconds(1300);
  servoRight.writeMicroseconds(1300);
}

void backward(){   
  servoLeft.writeMicroseconds(1300);
  servoRight.writeMicroseconds(1700);
}

void loop()
{
  nh.spinOnce();
  timer.run();
  byte wLeft = digitalRead(5);
  byte wRight = digitalRead(7);
  if(wLeft == 0 || wRight == 0){
     servoRight.writeMicroseconds(1500);
     servoLeft.writeMicroseconds(1500);
  }
}
