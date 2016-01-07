/**
* Group number : 6
* Student 1: 
* Bryan van Wijk 4363329
* Student 2:
* Dorian de Koning 4348737
*/

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <geometry_msgs/Twist.h>

class ImageConverter
{
  ros::NodeHandle nh;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  ros::Publisher cmd_vel_pub;
  
public:
  ImageConverter()
    : it_(nh)
  {
     // subscribe to the camera
    image_sub_ = it_.subscribe("/camera/image", 1, &ImageConverter::imageCb, this, 
		image_transport::TransportHints("compressed"));

    //publish geometry twist messages on the cmd_vel topic
    cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1000);

  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {	
	//convert the incoming msg to a cv image.
	cv_bridge::CvImagePtr cv_ptr;
    	try
    	{
    	  cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    	}
    	catch (cv_bridge::Exception& e)
    	{
     	 ROS_ERROR("cv_bridge exception: %s", e.what());
     	 return;
    	}
	
	//Send the geometry twist message on the cmd_vel topic
    	geometry_msgs::Twist cmd_vel_msg;
	cmd_vel_msg.linear.x = 1;
	cmd_vel_pub.publish(cmd_vel_msg);
  }
};

int main(int argc, char** argv) {
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}
