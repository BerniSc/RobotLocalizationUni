#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include <ros/ros.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Pose2D.h>
#include <turtlesim/Pose.h>

#include <iostream>

#include "control_window_params.hpp"
#include "square_detection.hpp"
#include "circle_detection.hpp"
#include "callback_functions.hpp"
#include "utility.hpp"
#include "constants.hpp"

using namespace cv;
using namespace std;

//Set in "constants.hpp"
const int destWidth = camera_consts::destWidth;
const int destHeight = camera_consts::destHeight;

const int inputWidth = camera_consts::inputWidth;
const int inputHeight = camera_consts::inputHeight;

//Actual Threshold Value for Adaptive Thresholding
int adaptiveThresholdLower = 9;
//Value averaging Pixels around out as "Kernel" -> Uneven
int adaptiveThresholdUpper = 15;

vector<Point> corners;

parameterController paramController;
/**     Descriptors     **/
parameterDescription thresholdMode(0, 1, 0, "Threshold Mode (Binary)", callback_trackbar_thresholdMode);
parameterDescription blurMode(0, 1, 0, "Blur Mode", callback_trackbar_BlurMode);
parameterDescription blurSize(0, 7, 9, "Bluring Size (1:=3 ,2:=5 etc.)", callback_trackbar_BlurSize);
parameterDescription cannyLow(0, 60, 12, "Canny Threshold Low", callback_trackbar_ThresholdCannyLow);
parameterDescription cannyHigh(0, 180, 35, "Canny Threshold High", callback_trackbar_ThresholdCannyHigh);
parameterDescription displayWindowSize(0, 50, 0, "Displaywindow Size (in %)", callback_trackbar_DisplayWindowSize);
parameterDescription thresholdLow(0,255, 80, "Lower Threshold Value", callback_trackbar_ThresholdLow);
parameterDescription thresholdHigh(0, 255, 255, "Higher Threshold Value", callback_trackbar_ThresholdHigh);
parameterDescription squareDetection(0, 1, 1, "Square Detection (Calib. Mode)", callback_trackbar_squareDetection);
parameterDescription circleDetection(0, 1, 1, "Clear Gray Circledetection Mode", callback_trackbar_circleDetection);
parameterDescription displayMode(0, 1, 1, "Display Mode", callback_trackbar_displayMode);
parameterDescription thresholdAdaptive(0, 1, 0, "Binary (0) or Adaptive (1) Threshold", callback_trackbar_adaptiveMode);

//Creating used "Images"
Mat image;
Mat imageGray;
Mat imageBlurred;
Mat imageCanny;
//Optional image -> used if slider acitvated use
Mat imageBlurredGray;

//Initialising angle of Robot to 0
double angleRobot = 0;

int main(int argc, char** argv) {
    ros::init(argc, argv, "RobotDetection");

    ros::NodeHandle nodeHandle;

    ros::Publisher pose_pub = nodeHandle.advertise<turtlesim::Pose>(ros_consts::pose_topic_name, 1000);
    ros::Publisher sim_pub = nodeHandle.advertise<turtlesim::Pose>(ros_consts::sim_topic_name, 1000);
    ros::Publisher goal_pub = nodeHandle.advertise<geometry_msgs::Point>(ros_consts::goal_topic_name, 1000);

    namedWindow("Control Window", WINDOW_AUTOSIZE);
    namedWindow("Normal Image", WINDOW_AUTOSIZE);
    namedWindow("warped", WINDOW_AUTOSIZE);

    paramController.addParam(&thresholdMode);
    paramController.addParam(&blurMode);
    paramController.addParam(&blurSize);
    paramController.addParam(&cannyLow);
    paramController.addParam(&cannyHigh);
    paramController.addParam(&displayWindowSize);
    paramController.addParam(&thresholdLow);
    paramController.addParam(&thresholdHigh);
    paramController.addParam(&squareDetection);
    paramController.addParam(&circleDetection);
    paramController.addParam(&displayMode);
    paramController.addParam(&thresholdAdaptive);

    paramController.createTrackbars();

    //Attach callback function for waypoint Publishing to Warped Img
    setMouseCallback("warped", callback_mouse_doubleclicked, &goal_pub);

    //detected corners
    Point2f cornersFloat[3];
    
    //Corners the detected Corners are mapped to
    //Corners are at Top Left, Bottom Left and Bottom Right (Thats the Layout of Detection Bounds)
    Point2f destCorners[3] = {Point(0, 0), Point(0, destHeight), Point(destWidth, destHeight)};

    Mat outputWarped;
    //Warp Matrix for Aplication on the Image
    Mat warpMat;

    //Initialize "Detected Points" in Center that they can be re-arranged
    corners.push_back(Point(inputWidth/2, inputHeight/2));
    corners.push_back(Point(inputWidth/2, inputHeight/2)); 
    corners.push_back(Point(inputWidth/2, inputHeight/2));

    //Open Webcam
    VideoCapture cap;
    cap.open(-1);
 
    cap.set(cv::CAP_PROP_FRAME_WIDTH, inputWidth);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, inputHeight);
       
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(CAP_PROP_FPS, 30);

    cap >> image;

    //Start and End Variable for FPS Calculation
    clock_t start, end;

    //Number of Frames Processed in given Time
    int numFrames = 1;
    double msBetweenFrames, fpsLive;

    if(debug_consts::displayBuildInformations) cerr << getBuildInformation();

    while(ros::ok()) {
        //Start Clock for FPS Counter
        start = clock();
        //Putting current Camera Image from Camerastream into Image Mat
        cap >> image;
        //Resising Image Mat
        resize(image, image, cv::Size(), (100-displayWindowSize.getValue())/100.0, (100-displayWindowSize.getValue())/100.0);
        //Processing resized Image i.e. turing it gray etc
        cvtColor(image, imageGray, COLOR_BGR2GRAY);
        if(thresholdMode.getValue() && !thresholdAdaptive.getValue()) threshold(imageGray, imageGray, thresholdLow.getValue(), thresholdHigh.getValue(), THRESH_BINARY);
        if(thresholdMode.getValue() && thresholdAdaptive.getValue()) adaptiveThreshold(imageGray, imageGray, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, adaptiveThresholdUpper, adaptiveThresholdLower);

        imageBlurred = getBlurred(imageGray, blurMode, blurSize);
        imageCanny = getCanny(imageBlurred, cannyLow, cannyHigh);

        //Only Blur Gray Image if really needed
        if(!circleDetection.getValue()) imageBlurredGray = getBlurred(imageGray, blurMode, blurSize);

        //When Slider for Calibration Mode is activated Programm tries to find Corners -> Saving them in "corners"
        if(squareDetection.getValue()) findSquares(imageCanny, image, corners);

        //COLOUR changed by Scalar -> Order is B-G-R
        cv::putText(image, "FPS: " + to_string(fpsLive), {50, image.rows-50}, FONT_HERSHEY_COMPLEX, 1.5, Scalar(153, 153, 0), 2);

        //Setting "cornersFloat" to their equivalent in "corners" -> implicit cast to float
        cornersFloat[0] = corners.at(0);
        cornersFloat[1] = corners.at(1);
        cornersFloat[2] = corners.at(2);
            
        warpMat = getAffineTransform(cornersFloat, destCorners);
        //Warping either blurred Image or normal image
        if(circleDetection.getValue()) {
            warpAffine(imageGray, outputWarped, warpMat, Size(destWidth, destHeight));
        } else {
            warpAffine(imageBlurredGray, outputWarped, warpMat, Size(destWidth, destHeight));
        }

        if(!squareDetection.getValue()) {
            vector<Vec3f> circlesSmall = findCircles(outputWarped, outputWarped, camera_consts::innerSize, cv::Scalar(120, 0, 120));
            vector<Vec3f> circlesLarge = findCircles(outputWarped, outputWarped, camera_consts::outerSize, cv::Scalar(0, 130, 0));

            //If more or less than one Circle-Pair detected, angle of robot remains the same 
            if(circlesLarge.size() == 1 && circlesSmall.size() == 1) {
                angleRobot = getAngleRobot(cv::Point(circlesLarge[0][0], circlesLarge[0][1]), cv::Point(circlesSmall[0][0], circlesSmall[0][1]), false); 
            }

            //If more than one Robot detected no values are published
            if(circlesLarge.size() == 1) {
                turtlesim::Pose pose_msg;
                //Getting Position in m
                pair<double, double> normalizedPosition = getNormalizedPosition(cv::Point(circlesLarge[0][0], circlesLarge[0][1]), pair<int, int>(220, 220), pair<int, int>(camera_consts::destWidth, camera_consts::destHeight));
                pose_msg.x = normalizedPosition.first;
                pose_msg.y = normalizedPosition.second;
                pose_msg.theta = angleRobot;

                pose_pub.publish(pose_msg);
                if(ros_consts::sim) sim_pub.publish(pose_msg);
                
                if(debug_consts::printPublishedValues) cout << "Current Position and orientation: " << pose_msg.x << "|" << pose_msg.y << "    " << pose_msg.theta << endl;
            }
        }

        end = clock();
 
        msBetweenFrames = (double(end) - double(start)) / double(CLOCKS_PER_SEC);
        
        fpsLive = double(numFrames) / double(msBetweenFrames);

        if(displayMode.getValue()) imshow("Normal Image", image); 
        if(displayMode.getValue()) imshow("Canny Image", imageCanny);
        if(displayMode.getValue()) imshow("warped", outputWarped);   

        char option = waitKey(5 * !(debug_consts::singleCameraSteps));
        switch(option) {
            case 'q' :
                cout << "Q has been pressed -> Exiting ..." << endl;
                return 10;
                break;
            case 'p' :
                cout << "P has been pressed -> Printing Current Config ...\n\n\n" << endl;
                paramController.printCurrentConfig();
                cout << "\n" << endl;
                break;
            case 'm' :
                cout << "M has been pressed -> Fast Switch between Detection/Calibration Mode ... \n\n" << endl;
                squareDetection.selectedValue = !squareDetection.selectedValue;
                break;
            case 'l' :
                {
                    char loadKey = waitKey(0);
                    cout << "\n\n\nTrying to load Config Nr. " << loadKey << "... \n\n" << endl;
                    paramController.loadConfig((int) loadKey - 48);
                    break;
                }
            case 'c' :
                cout << "Clearing calibrated Corners ... \n\n" << endl;
                corners.clear();
                corners.push_back(Point(inputWidth/2, inputHeight/2));
                corners.push_back(Point(inputWidth/2, inputHeight/2)); 
                corners.push_back(Point(inputWidth/2, inputHeight/2));
                break;
            case '+' :
                cout << "Increasing Adaptive Threshold Values ... \n" << endl;
                adaptiveThresholdLower += 2;
                break; 
            case '-' :
                cout << "Decreasing Adaptive Threshold Values ... \n" << endl;
                adaptiveThresholdLower -= 2;
                break;
        }
       
        //Allows for invoking Callback-Functions
        ros::spinOnce();
    }
    return 0;
}