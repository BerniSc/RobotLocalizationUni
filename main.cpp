#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include <geometry_msgs/Pose2D.h>
#include <turtlesim/Pose.h>

#include <iostream>

#include "control_window_params.hpp"
#include "square_detection.hpp"
#include "circle_detection.hpp"
#include "callback_functions.hpp"
#include "utility.hpp"
#include "constants.hpp"

#include <ros/ros.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Point.h>

//#include <opencv2/video.hpp>
//#include "opencv2/imgcodecs.hpp"
//#include "opencv2/videoio.hpp"

using namespace cv;
using namespace std;

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

int colsImageStart, rowsImageStart;

Mat image;
Mat imageGray;
Mat imageBlurred;
Mat imageCanny;

Mat imageBlurredGray;

double angleRobot = 0;

int main(int argc, char** argv) {
    ros::init(argc, argv, "RobotDetection");

    ros::NodeHandle nodeHandle;

    ros::Publisher pub = nodeHandle.advertise<std_msgs::String>("pose_topic_name", 1000);

    std_msgs::String test;
    test.data = "Hi";

    namedWindow("Control Window", WINDOW_AUTOSIZE);
    namedWindow("Normal Image", WINDOW_AUTOSIZE);

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

    #ifdef DEV_MODE
        namedWindow("Dev", WINDOW_GUI_EXPANDED);

        parameterDescription inverseAccumulator(1, 10, 1, "Inverse Accumulator", callback_trackbar_inverseAccumulator);
        parameterDescription minimumDistanceCircles(1, 25, 16, "Divisor of Inputrows for min. dist. circles", callback_trackbar_minDistCircles);
        parameterDescription upperCannyCircles(50, 180, 100, "Upper Canny Threshold for Circles", callback_trackbar_upperCannyCircle);
        parameterDescription centerDetectionThreshold(20, 50, 30, "Threshold for Center detection", callback_trackbar_thresholdCenterDetection);

        parameterController devController("Dev");

        devController.addParam(&inverseAccumulator);
        devController.addParam(&minimumDistanceCircles);
        devController.addParam(&upperCannyCircles);
        devController.addParam(&centerDetectionThreshold);

        devController.createTrackbars();

        ros::Publisher pose_pub = nodeHandle.advertise<turtlesim::Pose>(ros_consts::pose_topic_name, 1000);
        ros::Publisher sim_pub = nodeHandle.advertise<turtlesim::Pose>(ros_consts::sim_topic_name, 1000);

        test.data = "Hi";
    #endif

    /********TESTING WARP***********/
    namedWindow("warped", 0);


    #ifdef DEV_MODE
        //Maybe pass Parameter ref to pub or smthg
        ros::Publisher goal_pub = nodeHandle.advertise<geometry_msgs::Point>(ros_consts::goal_topic_name, 1000);
        setMouseCallback("warped", callback_mouse_doubleclicked, &goal_pub);
    #endif

    Point2f cornersFloat[3];
    
    //Corners are at Top Left, Bottom Left and Bottom Right (Thats the Layout of Detection Bounds)
    Point2f destCorners[3] = {Point(0, 0), Point(0, destHeight), Point(destWidth, destHeight)};

    Mat outputWarped;
    //Warp Matrix for Aplication on the Image
    Mat warpMat;

    //Initialize "Detected Points" in Center that they can be re-arranged
    corners.push_back(Point(inputWidth/2, inputHeight/2));
    corners.push_back(Point(inputWidth/2, inputHeight/2)); 
    corners.push_back(Point(inputWidth/2, inputHeight/2));

    // 2 für USB, -1 für Intern
    VideoCapture cap;
    //Open VideoCapture on chnl 2 at first (USB) if webcam attached
    //if(!cap.open(2) || !cap.isOpened()) {
        //If Opening chnl 2 not possible open chnl -1 (Intern)
        cap.open(-1);
    //}

    cap.set(cv::CAP_PROP_FRAME_WIDTH, inputWidth);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, inputHeight);
       
    //** TODO - Anschauen ob behalten oder wegmachen **//
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(CAP_PROP_FPS, 30);

    cap >> image;
    colsImageStart = image.cols;
    rowsImageStart = image.rows;

    //Start and End Variable for FPS Calculation
    clock_t start, end;

    //Number of Frames Processed in given Time
    int numFrames = 1;
    double msBetweenFrames, fpsLive;

    if(debug_consts::displayBuildInformations) cerr << getBuildInformation();

    while(ros::ok()) {
        //Start Clock for FPS Counter BERNHARD @TODO
        start = clock();
        //Putting current Camera Image from Camerastream into Image Mat @TODO
        cap >> image;
        //Resising Image Mat @TODO
        resize(image, image, cv::Size(), (100-displayWindowSize.getValue())/100.0, (100-displayWindowSize.getValue())/100.0);
        //Processing resized Image i.e. turing it gray etc @TODO
        cvtColor(image, imageGray, COLOR_BGR2GRAY);
        //Applying threshold for save in Processingpower if slider is activated @TODO
        //TODO -> EVTL auch adaptive Threshhold einfügen, -> 3. Param ist wert auf den gesetzt wird
        if(thresholdMode.getValue() && !thresholdAdaptive.getValue()) threshold(imageGray, imageGray, thresholdLow.getValue(), thresholdHigh.getValue(), THRESH_BINARY);
        if(thresholdMode.getValue() && thresholdAdaptive.getValue()) adaptiveThreshold(imageGray, imageGray, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, adaptiveThresholdUpper, adaptiveThresholdLower);

        imageBlurred = getBlurred(imageGray, blurMode, blurSize);
        imageCanny = getCanny(imageBlurred, cannyLow, cannyHigh);

        //Only Blur Gray Image if really needed
        if(!circleDetection.getValue()) imageBlurredGray = getBlurred(imageGray, blurMode, blurSize);

        //When Slider for Calibration Mode is activated Programm tries to find Corners -> Saving them in "corners"
        if(squareDetection.getValue()) findSquares(imageCanny, image, corners);

        //COLOR changed by Scalar -> Order is B-G-R
        //cv::putText(image, "MAX FPS: " + to_string(CAP_PROP_FPS), {25, 50}, FONT_HERSHEY_PLAIN, 2, Scalar(153, 153, 0), 3);
        cv::putText(image, "FPS: " + to_string(fpsLive), {50, image.rows-50}, FONT_HERSHEY_COMPLEX, 1.5, Scalar(153, 153, 0), 2);

        //Setting "cornersFloat" to their equivalent in "corners" -> implicit cast to float
        cornersFloat[0] = corners.at(0);
        cornersFloat[1] = corners.at(1);
        cornersFloat[2] = corners.at(2);
            
        warpMat = getAffineTransform(cornersFloat, destCorners);
        //FOR DISPLAY PUROPOSES @TODO
        //warpAffine(image, outputWarped, warpMat, Size(destWidth, destHeight)); @TODO
        if(circleDetection.getValue()) {
            warpAffine(imageGray, outputWarped, warpMat, Size(destWidth, destHeight));
        } else {
            warpAffine(imageBlurredGray, outputWarped, warpMat, Size(destWidth, destHeight));
        }

        if(!squareDetection.getValue()) {
            std::pair<int, int> innerSize(12, 20);
            std::pair<int, int> outerSize(20, 40);
            vector<Vec3f> circlesSmall = findCircles(outputWarped, outputWarped, innerSize, cv::Scalar(120, 0, 120));
            vector<Vec3f> circlesLarge = findCircles(outputWarped, outputWarped, outerSize, cv::Scalar(0, 130, 0));

            if(circlesLarge.size() == 1 && circlesSmall.size() == 1) {
                angleRobot = getAngleRobot(cv::Point(circlesLarge[0][0], circlesLarge[0][1]), cv::Point(circlesSmall[0][0], circlesSmall[0][1])); 
                cout << angleRobot << endl;
            }

            #ifdef DEV_MODE

                turtlesim::Pose pose_msg;
                if(circlesLarge.size() == 1) {
                    pair<double, double> normalizedPosition = getNormalizedPosition(cv::Point(circlesLarge[0][0], circlesLarge[0][1]), pair<int, int>(220, 220), pair<int, int>(camera_consts::destWidth, camera_consts::destHeight));
                    pose_msg.x = normalizedPosition.first / 100;
                    pose_msg.y = normalizedPosition.second / 100;
                    pose_msg.theta = angleRobot * (M_PI / 180.0);
                    cout << pose_msg.theta << endl;
                    pose_pub.publish(pose_msg);
                    sim_pub.publish(pose_msg);
                    cout << "Current Position and orientation: " << pose_msg.x << " " << pose_msg.y << "    " << pose_msg.theta << endl;
                }
            #endif
        }

        end = clock();
 
        msBetweenFrames = (double(end) - double(start)) / double(CLOCKS_PER_SEC);
        fpsLive = double(numFrames) / double(msBetweenFrames);

        if(displayMode.getValue()) imshow("Normal Image", image); 
        if(displayMode.getValue()) imshow("Canny Image", imageCanny);
        if(displayMode.getValue()) imshow("warped", outputWarped);   

        //Publish Message
        pub.publish(test);

        char option = waitKey(10 * !(debug_consts::singleCameraSteps));
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