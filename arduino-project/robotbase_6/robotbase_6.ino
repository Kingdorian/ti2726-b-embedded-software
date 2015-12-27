#include <ros.h>
#include <ArduinoHardware.h>

#include <SimpleTimer.h>

/*
 * rosserial Publisher Example
 * Prints "hello world!"
 */

#include <std_msgs/String.h>
#include <geometry_msgs/Twist.h>
//class NewHardware : public ArduinoHardware{
  
//public : NewHardware ( ) : ArduinoHardware(&Serial1,
//57600) {};
//};


ros::NodeHandle nh;


// the timer object
SimpleTimer timer;

void messageCb( const geometry_msgs::Twist& robot_controls){
  if(robot_controls.linear.x>1.0){
     moveForward();
     timer.setTimeout(5000, stop_robot);
     timer.run();
   } else if(robot_controls.angular.z < 180 && robot_controls.angular.z > 0){
     turnLeft();
   } else if(robot_controls.angular.z > 180 && robot_controls.angular.z > 0){
     turnRight();
   } else{
     stop_robot();
  }    
}

ros::Subscriber<geometry_msgs::Twist> sub("robo", &messageCb );


void setup()
{
  nh.initNode();
  nh.subscribe(sub);
}

void stop_robot(){
  analogWrite(3, 255);
  analogWrite(7, 255);
}

void moveForward(){
  analogWrite(3, 200);
  analogWrite(7, 200);
}

void turnRight(){
  analogWrite(24, 0);
  analogWrite(25, 0);
  analogWrite(6, 100);
  analogWrite(3, 100);
}

void turnLeft(){   
  analogWrite(24, 0);
  analogWrite(25, 0);
  analogWrite(7, 100);
  analogWrite(2, 100);
}

void loop()
{
  nh.spinOnce();
 
  //timer.setInterval(1000, moveForward);
  //timer.setInterval(4000, stop);
  //timer.run();
  delay(100);
}
