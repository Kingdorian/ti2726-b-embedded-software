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
#include <math.h>

class NewHardware : public ArduinoHardware{
  
public : NewHardware ( ) : ArduinoHardware(&Serial1,
  57600) {};
};


ros::NodeHandle_<NewHardware> nh;

// the timer object
SimpleTimer timer;
int stopid;
Ultrasonic ultrasonic(23,22);

void messageCb( const geometry_msgs::Twist& robot_controls){
    double vel = fabs(robot_controls.linear.x);
    if(vel < fabs(robot_controls.angular.z)){
      vel = fabs(robot_controls.angular.z);
    }
  
  if(ultrasonic.Ranging(CM) > 10){
   if(robot_controls.linear.x > 0){
      move(255*vel, 255*vel);
     } else if(robot_controls.angular.z < 0){
        move(255*vel, -255*vel);
     } else if(robot_controls.angular.z > 0){
       move(-255*vel, 255*vel);
     }
  }else{
      move(0, 0);
  }
    if(robot_controls.linear.x < 0){
       move(-255*vel, -255*vel);
    }
  if(timer.isEnabled(stopid)) {  
    timer.restartTimer(stopid);
  } else{
    stopid = timer.setTimeout(500, stopRobot);
  }
}

ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", &messageCb );

void move(double leftmotor, double rightmotor){
  //enable both motors
  
    // Enable motors
    analogWrite(24, 255);
    analogWrite(25, 255);
    if(rightmotor > 0){
      //Forward right motor
      analogWrite(7, 0);
      analogWrite(6, rightmotor);
    }else{
       //Backward right motor
      analogWrite(7, fabs(rightmotor));
      analogWrite(6, 0);  
    }
    if(leftmotor > 0){
       //Forward left motor
      analogWrite(2, fabs(leftmotor));
      analogWrite(3, 0);
    }else{
       //Backward right motor
      analogWrite(2, 0);
      analogWrite(3, fabs(leftmotor));      
    }  
   
 
}

void stopRobot(){
    move(0, 0);
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
   delay(10);
}
