#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

#include "control_window_params.hpp"
#include "square_detection.hpp"
#include "circle_detection.hpp"
#include "callback_functions.hpp"
#include "utility.hpp"

#include "constants.hpp"

#include <opencv2/video.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"

using namespace cv;
using namespace std;

const int destWidth = 440;//600
const int destHeight = 440;//600

const int inputWidth = 640;//1280
const int inputHeight = 480;//960

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

int main(int argc, char** argv) {
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

    paramController.printCurrentConfig();
    paramController.createTrackbars();

    /********TESTING WARP***********/
    namedWindow("warped", 0);
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
    cap.open(2);
    if(!cap.isOpened()) {
        //If Opening chnl 2 not possible open chnl -1 (Intern)
        cap.open(-1);
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, inputWidth);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, inputHeight);
       
    //** TODO - Anschauen ob behalten oder wegmachen **//
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(CAP_PROP_FPS, 30);

    cap >> image;
    colsImageStart = image.cols;
    rowsImageStart = image.rows;

    clock_t start, end;

    int numFrames = 1;
    double msBetweenFrames, fpsLive;

    while(1) {
        //Start Clock for FPS Counter
        start = clock();
        //Putting current Camera Image from Camerastream into Image Mat
        cap >> image;
        //Resising Image Mat
        resize(image, image, cv::Size(), (100-displayWindowSize.getValue())/100.0, (100-displayWindowSize.getValue())/100.0);
        //Processing resized Image i.e. turing it gray etc 
        cvtColor(image, imageGray, COLOR_BGR2GRAY);
        //Applying threshold for save in Processingpower if slider is activated
        //TODO -> EVTL auch adaptive Threshhold einfügen, -> 3. Param ist wert auf den gesetzt wird
        if(thresholdMode.getValue() && !thresholdAdaptive.getValue()) threshold(imageGray, imageGray, thresholdLow.getValue(), thresholdHigh.getValue(), THRESH_BINARY);
        if(thresholdMode.getValue() && thresholdAdaptive.getValue()) adaptiveThreshold(imageGray, imageGray, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, adaptiveThresholdUpper, adaptiveThresholdLower);

        imageBlurred = getBlurred(imageGray, blurMode, blurSize);
        imageCanny = getCanny(imageBlurred, cannyLow, cannyHigh);

        imageBlurredGray = getBlurred(imageGray, blurMode, blurSize);

        //When Slider for Calibration Mode is activated Programm tries to find Corners
        if(squareDetection.getValue()) findSquares(imageCanny, image, corners);

        //COLOR changed by Scalar -> Order is B-G-R
        cv::putText(image, "MAX FPS: " + to_string(CAP_PROP_FPS), {25, 50}, FONT_HERSHEY_PLAIN, 2, Scalar(153, 153, 0), 3);
        cv::putText(image, "FPS: " + to_string(fpsLive), {50, image.rows-50}, FONT_HERSHEY_COMPLEX, 1.5, Scalar(153, 153, 0), 2);

        cornersFloat[0] = corners.at(0);
        cornersFloat[1] = corners.at(1);
        cornersFloat[2] = corners.at(2);
            
        warpMat = getAffineTransform(cornersFloat, destCorners);
        //FOR DISPLAY PUROPOSES
        //warpAffine(image, outputWarped, warpMat, Size(destWidth, destHeight));
        if(circleDetection.getValue()) {
            warpAffine(imageGray, outputWarped, warpMat, Size(destWidth, destHeight));
        } else {
            warpAffine(imageBlurredGray, outputWarped, warpMat, Size(destWidth, destHeight));
        }

        if(!squareDetection.getValue()) {
            //For Testing -> warped not allways availabe
            //cout << "Trying find Circles" << endl;
            //vector<Vec3f> circles = findCircles(imageBlurredGray, image);
            //If warped is working
            std::pair<int, int> innerSize(10, 18);
            std::pair<int, int> outerSize(25, 60);
            vector<Vec3f> circlesSmall = findCircles(outputWarped, outputWarped, innerSize, cv::Scalar(120, 0, 120));
            vector<Vec3f> circlesLarge = findCircles(outputWarped, outputWarped, outerSize, cv::Scalar(0, 130, 0));

            if(circlesLarge.size() == 1 && circlesSmall.size() == 1) {
                Vec3i cLarge = circlesLarge[0];
                cout << getAngleRobot(cv::Point(circlesLarge[0][0], circlesLarge[0][1]), cv::Point(circlesSmall[0][0], circlesSmall[0][1])) << endl;
            }
        }

        end = clock();
 
        msBetweenFrames = (double(end) - double(start)) / double(CLOCKS_PER_SEC);
        fpsLive = double(numFrames) / double(msBetweenFrames);

        if(displayMode.getValue()) imshow("Normal Image", image);
        if(displayMode.getValue()) imshow("Canny Image", imageCanny);
        if(displayMode.getValue()) imshow("warped", outputWarped);   

        char option = waitKey(10);
        switch(option) {
            case 'q' :
                cout << "Q has been pressed -> Exiting ..." << endl;
                return -1;
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
    }
    return 0;

    //cerr << getBuildInformation();
}