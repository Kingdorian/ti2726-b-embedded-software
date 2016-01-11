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
  cv::Mat dst, binary, cropped, cdst;
  cvtColor(img_rgb,binary,CV_RGB2GRAY);
 // imwrite("image_before_tresh.jpg", binary); // UNCOMMENT FOR DEBUGGING
  threshold( binary, binary, 128 , 255,1);
//  imwrite("image_after.jpg", binary); // UNCOMMENT FOR DEBUGGING WARNING SLOW
  std::vector<cv::Vec4i> lines;
  cv::Rect myROI =  cv::Rect(300, 0, 100, binary.size().height);
  binary =  binary( myROI);
cv::Canny(binary, dst, 50, 200, 3);
 cv::HoughLinesP(dst, lines, 1, 0.01, 10, 50, 100);

  int dif = 0;
  if(lines.size()>1){
  cv::Vec4i l = lines[0];
  cv::line(cv_ptr->image, cv::Point(l[0], l[1])+myROI.tl(), cv::Point(l[2], l[3])+myROI.tl(), cv::Scalar(0, 255, 255), 3, CV_AA);
  l = lines[1];
  cv::line(cv_ptr->image, cv::Point(l[0], l[1])+myROI.tl(), cv::Point(l[2], l[3])+myROI.tl(), cv::Scalar(0, 255, 255), 3, CV_AA);

    for(int i = 2; i<lines.size(); i++){
      l = lines[i];
      //std::cout << l[0] << " " << l[1] << " " << l[2] << " " << l[3] << " " << std::endl;
     // cv::line(cv_ptr->image, cv::Point(l[0], l[1])+myROI.tl(), cv::Point(l[2], l[3])+myROI.tl(), cv::Scalar(0, 0, 255), 3, CV_AA);
      dif = dif + ( l[1] - l[3]);
    }
  int highLineY;
  int lowLineY;
  geometry_msgs::Twist cmd_vel_msg;
  if(lines[0][1] + 0.5 * (lines[0][3]-lines[0][1])>lines[1][1] + 0.5 * (lines[1][3]-lines[1][1])){
    highLineY = lines[0][1] + 0.5 * (lines[0][3]-lines[0][1]);
    lowLineY = lines[1][1] + 0.5 * (lines[1][3]-lines[1][1]);
  }else{
    highLineY = lines[1][1] + 0.5 * (lines[1][3]-lines[1][1]);
    lowLineY = lines[0][1] + 0.5 * (lines[0][3]-lines[1][1]);
  }
  if(lowLineY<((binary.size().height*3/10)+myROI.tl().y)){
    cmd_vel_msg.angular.z = 1;
  }else if(highLineY>((binary.size().height*6/10)+myROI.tl().y)){
    cmd_vel_msg.angular.z = -1;
  }else{
    cmd_vel_msg.linear.x = 0.5;
  }
	cmd_vel_pub.publish(cmd_vel_msg);
  cv::line(cv_ptr->image, myROI.tl() + cv::Point(myROI.size().width/2,highLineY), myROI.tl() +  cv::Point(myROI.size().width/2, lowLineY), cv::Scalar(255, 255, 0), 3, CV_AA);
  }
  cv::line(cv_ptr->image, cv::Point(0, binary.size().height*3/10)+myROI.tl(), cv::Point(binary.size().width, binary.size().height*3/10)+myROI.tl(), cv::Scalar(255, 0, 0), 3, CV_AA);
  cv::line(cv_ptr->image, cv::Point(0, binary.size().height/2)+myROI.tl(), cv::Point(binary.size().width, binary.size().height/2)+myROI.tl(), cv::Scalar(255, 0, 0), 3, CV_AA);
  cv::line(cv_ptr->image, cv::Point(0, binary.size().height*7/10)+myROI.tl(), cv::Point(binary.size().width, binary.size().height*7/10)+myROI.tl(), cv::Scalar(255, 0, 0), 3, CV_AA);
  cv::rectangle(cv_ptr->image, myROI, cv::Scalar(0, 255, 0), 3, CV_AA, 0);

  image_pub_.publish(cv_ptr->toImageMsg());
  if(lines.size() > 0){
    dif = dif / lines.size();
  }
	//Send the geometry twist message on the cmd_vel topic
  }
};

int main(int argc, char** argv) {
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}
