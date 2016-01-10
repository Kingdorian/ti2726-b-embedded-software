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
#include <opencv2/opencv.hpp>
#include <vector>

class ImageConverter
{
  ros::NodeHandle nh;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  ros::Publisher cmd_vel_pub;
  
public:
  ImageConverter()
    : it_(nh)
  {
     // subscribe to the camera
    image_sub_ = it_.subscribe("/camera/image", 1, &ImageConverter::imageCb, this, 
		image_transport::TransportHints("compressed"));
    image_pub_ = it_.advertise("/image_converter/output_video", 1);


    //publish geometry twist messages on the cmd_vel topic
    cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1000);

  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {	
	//convert the incoming msg to a cv image.
	cv_bridge::CvImagePtr cv_ptr;
  cv::Mat img_rgb; 
    	try
    	{
    	  cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        img_rgb = cv_ptr->image;
    	}
    	catch (cv_bridge::Exception& e)
    	{
     	 ROS_ERROR("cv_bridge exception: %s", e.what());
     	 return;
    	}
  cv::Mat dst, cdst;
	cv::Canny(img_rgb, dst, 50, 200, 3);
  std::vector<cv::Vec4i> lines;

  cv::HoughLinesP(dst, lines, 1, 0.01, 10, 50, 10);
  int dif = 0;
  for(int i = 0; i<lines.size(); i++){
    cv::Vec4i l = lines[i];
    //std::cout << l[0] << " " << l[1] << " " << l[2] << " " << l[3] << " " << std::endl;
    cv::line(cv_ptr->image, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 3, CV_AA);
    dif = dif + ( l[1] - l[3]);
  }

  image_pub_.publish(cv_ptr->toImageMsg());
  if(lines.size() > 0){
    dif = dif / lines.size();
  }
  geometry_msgs::Twist cmd_vel_msg;
  if(dif < 5  && dif > -5){
    cmd_vel_msg.linear.x = 0.5;
  } else if(dif > 0){
    cmd_vel_msg.angular.z = 1;
  } else {
    cmd_vel_msg.angular.z = -1;
  }
  //imshow("detected lines", cdst);
  //cv::waitKey();
	//Send the geometry twist message on the cmd_vel topic
	cmd_vel_pub.publish(cmd_vel_msg);
  }
};

int main(int argc, char** argv) {
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}
