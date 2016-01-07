/**
* Group number : 6
* Student 1: 
* Bryan van Wijk 4363329
* Student 2:
* Dorian de Koning 4348737
*/

#include <ros.h>
#include <ArduinoHardware.h>
#include <SimpleTimer.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Twist.h>
#include <Ultrasonic.h>

//class NewHardware : public ArduinoHardware{
  
//public : NewHardware ( ) : ArduinoHardware(&Serial1,
//57600) {};
//};


ros::NodeHandle nh;


// the timer object
SimpleTimer timer;
int stopid;

void messageCb( const geometry_msgs::Twist& robot_controls){
  if(robot_controls.linear.x > 0){
     moveForward();
   } else if(robot_controls.angular.z < 0){
     turnRight();
   } else if(robot_controls.angular.z > 0){
     turnLeft();
   } else if(robot_controls.linear.x < 0){
     moveBackward();
   } else{
     stopRobot();
  }    
  
  if(timer.isEnabled(stopid)) {  
    timer.restartTimer(stopid);
  } else{
    stopid = timer.setTimeout(500, stopRobot);
  }
}

ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", &messageCb );

void stopRobot(){
  //disable motors
  analogWrite(24, 0); 
  analogWrite(25, 0);
  
  //Make all the motors stop
  analogWrite(3, 255);
  analogWrite(7, 255);
  analogWrite(6, 255);
  analogWrite(2, 255);
}

void moveForward(){
  //enable both motors
  analogWrite(24, 255);
  analogWrite(25, 255);
  
  //Forward right motor
  analogWrite(7, 200);
  analogWrite(6, 0);
  
  //Forward left motor
  analogWrite(3, 200);
  analogWrite(2, 0);
}

void turnRight(){
  //enable both motors
  analogWrite(24, 255);
  analogWrite(25, 255);
  
  //Backward right motor
  analogWrite(6, 100);
  analogWrite(7, 0);
  
  //Forward left motor
  analogWrite(3, 100);
  analogWrite(2, 0);
}

void turnLeft(){  
  //enable both motors
  analogWrite(24, 255);
  analogWrite(25, 255);
  
  //Forward left motor
  analogWrite(6, 0);
  analogWrite(7, 100);
  
  //Backward right motor
  analogWrite(3, 0);
  analogWrite(2, 100);
}

void  moveBackward(){
  //enable both motors
  analogWrite(24, 255);
  analogWrite(25, 255);
  
  //Backward right motor
  analogWrite(6, 100);
  analogWrite(7, 0);
  
  //Backward right motor
  analogWrite(3, 0);
  analogWrite(2, 100);
}

void setup()
{
  nh.initNode();
  nh.subscribe(sub);
  stopRobot();
  stopid = timer.setTimeout(500, stopRobot);
}


void loop()
{
   nh.spinOnce();
   timer.run();
   // check for obstacles
  //byte wLeft = digitalRead(5);
 // byte wRight = digitalRead(7);
 // if(wLeft == 0 || wRight == 0){
 //    stopRobot();
  //}
}
