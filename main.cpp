#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

#include "control_window_params.hpp"
#include "square_detection.hpp"
#include "circle_detection.hpp"
#include "callback_functions.hpp"
#include "utility.hpp"

#include <opencv2/video.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"

using namespace cv;
using namespace std;

int blurModeGaussian = 0;
vector<Point> corners;

bool warpMode = true;

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

int colsImageStart, rowsImageStart;

Mat image;
Mat imageGray;
Mat imageBlurred;
Mat imageCanny;

Mat imageBlurredGray;
//TODO ADD THRESHHOLD SLIDERS
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

    paramController.printCurrentConfig();
    paramController.createTrackbars();

    /********TESTING WARP***********/
    namedWindow("warped", 0);
    int destWidth = 440;
    int destHeight = 440;
    Point2f cornersFloat[3];
    
    //Ansatzpunkt für Michael aus outPut Warped Kreiserkennung ortung möglich -> evlt auch nicht image warpen sondern Canny o.ä. -> Algorithmus bereits gelaufen -> FPS schon bei flüssig min 30
    Point2f destCorners[3] = {Point(0, 0), Point(0, destHeight), Point(destWidth, destHeight)};
    Mat outputWarped;
    Mat warpMat;

    //FALLS DICKE PUNKTE (Warp Punkte nicht richtig erkannt werden hier Vergleichswerte entsprechend anpassen)
    corners.push_back(Point(320, 120));
    corners.push_back(Point(320, 210)); 
    corners.push_back(Point(400, 120));

    // 2 für USB, -1 für Intern
    VideoCapture cap(2);

        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        // **** TODO DORT WIEDER HINMACHEN **** //
        //cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
        //cap.set(CAP_PROP_FPS, 60);

        cap >> image;
        colsImageStart = image.cols;
        rowsImageStart = image.rows;

        clock_t start, end;

        int numFrames = 1;
        double msBetweenFrames, fpsLive;

        while(1) {
            start = clock();
            cap >> image;
            resize(image, image, cv::Size(), (100-displayWindowSize.getValue())/100.0, (100-displayWindowSize.getValue())/100.0);
            cvtColor(image, imageGray, COLOR_BGR2GRAY);
            if(thresholdMode.getValue()) threshold(imageGray, imageGray, thresholdLow.getValue(), thresholdHigh.getValue(), THRESH_BINARY);
            imageBlurred = getBlurred(imageGray, blurMode, blurSize);
            imageCanny = getCanny(imageBlurred, cannyLow, cannyHigh);



            imageBlurredGray = getBlurred(imageGray, blurMode, blurSize);

            for(auto i : corners) {
                cout << i << "      ";
                drawPoint(image, i, 5);
            }
            imshow("Image", image);
            waitKey(0);

            if(squareDetection.getValue()) findSquares(imageCanny, image, corners);
            //drawSquares(image, squares);

            //COLOR changedby Scalar -> Order is B-G-R
            putText(image, "MAX FPS: " + to_string(CAP_PROP_FPS), {25, 50}, FONT_HERSHEY_PLAIN, 2, Scalar(153, 153, 0), 3);
            putText(image, "FPS: " + to_string(fpsLive), {50, image.rows-50}, FONT_HERSHEY_COMPLEX, 1.5, Scalar(153, 153, 0), 2);

            //FOR DETECTING AND SHOWING CORNERPOINTS
            
            for(auto i : corners) {
                cout << i << "      ";
                drawPoint(image, i, 5);
            }
            cout << "" << endl;
            //*/ 

           if(warpMode) {
            if(corners.at(0).y < corners.at(1).y) {
                cornersFloat[0] = corners.at(0);
                cornersFloat[1] = corners.at(1);
            } 
            if(corners.at(0).y > corners.at(1).y) {
                cornersFloat[1] = corners.at(0);
                cornersFloat[0] = corners.at(1);
            }
            cornersFloat[2] = corners.at(2);
 
            cout << cornersFloat[0] << "    " << cornersFloat[1] << "   " << cornersFloat[2] << "   " << endl;

            //imshow("Image Canny C", imageCanny); 
            //warpMat = getPerspectiveTransform(cornersFloat, destCorners);
            //warpPerspective(image, outputWarped, warpMat, cv::Size(640, 480));
            
            warpMat = getAffineTransform(cornersFloat, destCorners);
            //FOR DISPLAY PUROPOSES
            //warpAffine(image, outputWarped, warpMat, Size(destWidth, destHeight));
            if(circleDetection.getValue()) {
                warpAffine(imageGray, outputWarped, warpMat, Size(destWidth, destHeight));
            } else {
                warpAffine(imageBlurredGray, outputWarped, warpMat, Size(destWidth, destHeight));
            }

            if(!squareDetection.getValue()) {
                //vector<Vec3f> circles = findCircles(imageCanny, image);
                vector<Vec3f> circles = findCircles(outputWarped, outputWarped);
                //cout << outputWarped.rows - imageCanny.rows << "      " << outputWarped.cols - imageCanny.cols << " " << outputWarped.empty() + imageCanny.empty() << endl;
                //drawCircles(image , circles);
            }

            if(displayMode.getValue()) imshow("warped", outputWarped);   
           }

                       end = clock();
 
            msBetweenFrames = (double(end) - double(start)) / double(CLOCKS_PER_SEC);
            fpsLive = double(numFrames) / double(msBetweenFrames);

            if(displayMode.getValue()) imshow("Normal Image", image);
            if(displayMode.getValue()) imshow("Canny Image", imageCanny);


            if(!squareDetection.getValue()) createTrackbar(circleDetection.name, "Control Window", &circleDetection.selectedValue, circleDetection.getMaxValueForSlider(), callback_trackbar_circleDetection, &image);

            waitKey(0);
            //paramController.printCurrentConfig();
        }
    return 0;

    //cerr << getBuildInformation();
}