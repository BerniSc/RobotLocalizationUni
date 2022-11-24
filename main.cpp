#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

#include "control_window_params.hpp"
#include "image_collection.hpp"
#include "square_detection.hpp"

using namespace cv;
using namespace std;


int blurModeGaussian = 0;
int thresholdModeBool = 0;

const bool testMode = false;
/*
int blurSizeKernel;                 //
int cannyThresholdLow = 30;         //Range 0 to 30
int cannyThresholdHigh = 120;       //Range 0 to 120
int brightness = 0;                 //Range from -100 to 
*/

/**     Descriptors     **/
parameterDescription thresholdMode(0, 1, 0, "Threshold Mode (Binary)");
parameterDescription blurMode(0, 1, 0, "Blur Mode");
parameterDescription blurSize(0, 7, 9, "Bluring Size (1:=3 ,2:=5 etc.)");
parameterDescription cannyLow(0, 60, 12, "Canny Threshold Low");
parameterDescription cannyHigh(0, 180, 35, "Canny Threshold High");
parameterDescription resizeSize(0, 50, 0, "Window Size (in %)");
parameterDescription displayWindowSize(0, 50, 0, "Displaywindow Size (in %)");

/***/
void callback_trackbar_thresholdMode(int mode, void* userData);
void callback_trackbar_BlurMode(int mode, void* userData);
void callback_trackbar_BlurSize(int blurKernelValue, void* userData);
void callback_trackbar_ThresholdLow(int cannyLow, void* userData);
void callback_trackbar_ThresholdHigh(int cannyHigh, void* userData);
void callback_trackbar_WindowSize(int WindowSize, void* userData);
void callback_trackbar_DisplayWindowSize(int WindowSize, void* userData);

void refresh();

Mat getCanny(Mat& original);
Mat getBlurred(Mat& original);

int colsImageStart, rowsImageStart;

Mat image;
Mat imageGray;
Mat imageBlurred;
Mat imageCanny;
//TODO ADD THRESHHOLD SLIDERS
int main(int argc, char** argv) {
    namedWindow("Control Window", WINDOW_AUTOSIZE);
    namedWindow("Normal Image", WINDOW_AUTOSIZE);
    if(testMode) {
        namedWindow("Gray Image", WINDOW_AUTOSIZE);
        namedWindow("Blurred Image", WINDOW_AUTOSIZE);
        namedWindow("Canny Image", WINDOW_AUTOSIZE);
    }
    
    createTrackbar(thresholdMode.name, "Control Window", &thresholdMode.selectedValue, thresholdMode.getMaxValueForSlider(), callback_trackbar_thresholdMode, &image);
    createTrackbar(resizeSize.name, "Control Window", &resizeSize.selectedValue, resizeSize.getMaxValueForSlider(), callback_trackbar_WindowSize, &imageGray);
    createTrackbar(blurMode.name, "Control Window", &blurMode.selectedValue, blurMode.getMaxValueForSlider(), callback_trackbar_BlurMode, &image);
    createTrackbar(blurSize.name, "Control Window", &blurSize.selectedValue, blurSize.getMaxValueForSlider(), callback_trackbar_BlurSize, &image);
    createTrackbar(cannyLow.name, "Control Window", &cannyLow.selectedValue, cannyLow.getMaxValueForSlider(), callback_trackbar_ThresholdLow, &imageBlurred);
    createTrackbar(cannyHigh.name, "Control Window", &cannyHigh.selectedValue, cannyHigh.getMaxValueForSlider(), callback_trackbar_ThresholdHigh, &imageBlurred);
    createTrackbar(displayWindowSize.name, "Control Window", &displayWindowSize.selectedValue, displayWindowSize.getMaxValueForSlider(), callback_trackbar_DisplayWindowSize, &image);
    
    if(testMode) {
        image = imread("Eisbaer.jpg", IMREAD_COLOR);
        colsImageStart = image.cols;
        rowsImageStart = image.rows;
        imageGray = imread("Eisbaer.jpg", IMREAD_GRAYSCALE);

        imageBlurred = getBlurred(imageGray);
        imageCanny = getCanny(imageBlurred);

        if(!image.data && !imageGray.data) {
            cout << "Kann Bild nicht öffnen!" << endl;
            return -1;
        }

        imageBlurred = getBlurred(image);
        imageCanny = getCanny(imageBlurred);
        
        refresh();

        waitKey(0);
    } else {
        vector<vector<Point>> squares;

        VideoCapture cap(-1);

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
            if(thresholdModeBool) threshold(imageGray, imageGray, 80, 255, THRESH_BINARY);
            imageBlurred = getBlurred(imageGray);
            imageCanny = getCanny(imageBlurred);

            end = clock();

            msBetweenFrames = (double(end) - double(start)) / double(CLOCKS_PER_SEC);
            fpsLive = double(numFrames) / double(msBetweenFrames);

            //COLOR changedby Scalar -> Order is B-G-R
            putText(image, "MAX FPS: " + to_string(CAP_PROP_FPS), {25, 50}, FONT_HERSHEY_PLAIN, 2, Scalar(153, 153, 0), 3);
            putText(image, "FPS: " + to_string(fpsLive), {50, image.rows-50}, FONT_HERSHEY_COMPLEX, 1.5, Scalar(153, 153, 0), 2);

            imshow("Normal Image", imageGray);
            imshow("Canny Image", imageCanny);

            waitKey(10);
        }
    }
    return 0;

    //cerr << getBuildInformation();
}

void callback_trackbar_thresholdMode(int mode, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    thresholdModeBool = !thresholdModeBool;
    cout << "Button Pressed " << thresholdModeBool << endl;
}

void callback_trackbar_BlurMode(int mode, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    blurModeGaussian = !blurModeGaussian;
    cout << "Button Pressed " << blurModeGaussian << endl;
    
    imageBlurred = getBlurred(m);
    imageCanny = getCanny(imageBlurred);
    refresh();
}

void callback_trackbar_BlurSize(int blurKernelValue, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    blurKernelValue = blurKernelValue*2 +1;
    blurSize.selectedValue = blurKernelValue;
    //cout << "valueBlurKernel: " << blurKernelValue << " " << blurSize.getValue() <<endl;
    
    image = m;
    imageBlurred = getBlurred(m);
    imageCanny = getCanny(imageBlurred);
    refresh();
}

void callback_trackbar_ThresholdLow(int cannyLow, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    imageCanny = getCanny(m);
    //imshow("Canny Image", imageCanny);
    refresh();
}

void callback_trackbar_ThresholdHigh(int cannyHigh, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    imageCanny = getCanny(m);
    refresh();
    //imshow("Canny Image", imageCanny);
}

void callback_trackbar_WindowSize(int windowSize, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    resize(m, m, cv::Size(), (100-resizeSize.getValue())/100.0, (100-resizeSize.getValue())/100.0);
    if(testMode) imshow("Gray Image", m);
    Mat b = getBlurred(m);
    if(testMode) imshow("Blurred Image", b);
    imshow("Canny Image", getCanny(b));
}

void callback_trackbar_DisplayWindowSize(int WindowSize, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    //cout << displayWindowSize.getValue() << endl;
    resize(m, m, cv::Size(), (100-displayWindowSize.getValue())/100.0, (100-displayWindowSize.getValue())/100.0);
    imshow("Normal Image", m);
}

Mat getCanny(Mat& original) {
    Mat imageCanny;
    Canny(original, imageCanny, cannyLow.getValue(), cannyHigh.getValue(), 3, false);
    return imageCanny;
}

Mat getBlurred(Mat& original) {
    Mat blurred;
    if(blurMode.getValue()) {
        GaussianBlur(original, blurred, Size(blurSize.getValue(), blurSize.getValue()), 0, 0);
    } else {
        medianBlur(original, blurred, blurSize.getValue());
    }
    return blurred;
}

void refresh() {
    resize(image, image, cv::Size(colsImageStart * ((100-displayWindowSize.getValue())/100.0), rowsImageStart * ((100-displayWindowSize.getValue())/100.0)));
    imshow("Normal Image", image);
    if(testMode) {
        resize(imageGray, imageGray, cv::Size(colsImageStart * ((100-resizeSize.getValue())/100.0), rowsImageStart * ((100-resizeSize.getValue())/100.0)));
        imshow("Gray Image", imageGray);
        resize(imageBlurred, imageBlurred, cv::Size(colsImageStart * ((100-resizeSize.getValue())/100.0), rowsImageStart * ((100-resizeSize.getValue())/100.0)));
        imshow("Blurred Image", imageBlurred);
    }
    resize(imageCanny, imageCanny, cv::Size(colsImageStart * ((100-resizeSize.getValue())/100.0), rowsImageStart * ((100-resizeSize.getValue())/100.0)));;
    imshow("Canny Image", imageCanny);
}