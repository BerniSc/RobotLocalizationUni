#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

#include "control_window_params.hpp"

using namespace cv;
using namespace std;

int blurModeGaussian = 0;
int blurSizeKernel;                 //
int cannyThresholdLow = 30;         //Range 0 to 30
int cannyThresholdHigh = 120;       //Range 0 to 120
int brightness = 0;                 //Range from -100 to 100

/**     Descriptors     **/
parameterDescription blurMode(0, 1, 0, "Blur Mode");
parameterDescription blurSize(0, 6, 9, "Bluring Size (1:=3 ,2:=5 etc.)");
parameterDescription cannyLow(0, 30, 12, "Canny Threshold Low");
parameterDescription cannyHigh(0, 120, 35, "Canny Threshold High");
parameterDescription resizeSize(-2, 2, 1, "Window Size (-2 := 0,5 -> 2 := 2)");

/***/
void callback_trackbar_BlurMode(int mode, void* userData);
void callback_trackbar_BlurSize(int blurKernelValue, void* userData);
void callback_trackbar_ThresholdLow(int cannyLow, void* userData);
void callback_trackbar_ThresholdHigh(int cannyHigh, void* userData);
void callback_trackbar_WindowSize(int WindowSize, void* userData);

Mat getCanny(Mat& original);
Mat getBlurred(Mat& original);

int main(int argc, char** argv) {
    Mat image = imread("Eisbaer.jpg", IMREAD_COLOR);
    Mat imageGray = imread("Eisbaer.jpg", IMREAD_GRAYSCALE);

    Mat imageBlurred = getBlurred(imageGray);
    Mat imageCanny = getCanny(imageBlurred);

    namedWindow("Control Window");
    namedWindow("Normal Image", WINDOW_AUTOSIZE);
    namedWindow("Gray Image", WINDOW_AUTOSIZE);
    namedWindow("Blurred Image", WINDOW_AUTOSIZE);
    namedWindow("Canny Image", WINDOW_AUTOSIZE);
    
    createTrackbar(resizeSize.name, "Control Window", &resizeSize.selectedValue, resizeSize.getMaxValueForSlider(), callback_trackbar_WindowSize, &image);
    createTrackbar(blurMode.name, "Control Window", &blurMode.selectedValue, blurMode.getMaxValueForSlider(), callback_trackbar_BlurMode, &image);
    createTrackbar(blurSize.name, "Control Window", &blurSize.selectedValue, blurSize.getMaxValueForSlider(), callback_trackbar_BlurSize, &image);
    createTrackbar(cannyLow.name, "Control Window", &cannyLow.selectedValue, cannyLow.getMaxValueForSlider(), callback_trackbar_ThresholdLow, &imageBlurred);
    createTrackbar(cannyHigh.name, "Control Window", &cannyHigh.selectedValue, cannyHigh.getMaxValueForSlider(), callback_trackbar_ThresholdHigh, &imageBlurred);

    if(!image.data && !imageGray.data) {
        cout << "Kann Bild nicht öffnen!" << endl;
        return -1;
    }

    
    imshow("Normal Image", image);
    imshow("Gray Image", imageGray);
    imshow("Blurred Image", getBlurred(image));
    imshow("Canny Image", getCanny(imageBlurred));

    waitKey(0);
    return 0;

    //cerr << getBuildInformation();
}

void callback_trackbar_BlurMode(int mode, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    blurModeGaussian = !blurModeGaussian;
    cout << "Button Pressed " << blurModeGaussian << endl;
    
    Mat blurred = getBlurred(m);
    imshow("Blurred Image", blurred);
    imshow("Canny Image", getCanny(blurred));
}

void callback_trackbar_BlurSize(int blurKernelValue, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    blurKernelValue = blurKernelValue*2 +1;
    blurSize.selectedValue = blurKernelValue;
    //cout << "valueBlurKernel: " << blurKernelValue << " " << blurSize.getValue() <<endl;
    
    Mat blurred = getBlurred(m);
    imshow("Blurred Image", blurred);
    imshow("Canny Image", getCanny(blurred));
}

void callback_trackbar_ThresholdLow(int cannyLow, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    imshow("Canny Image", getCanny(m));
}

void callback_trackbar_ThresholdHigh(int cannyHigh, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    imshow("Canny Image", getCanny(m));
}

void callback_trackbar_WindowSize(int windowSize, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    cout << resizeSize.getValue() << endl;
    if(resizeSize.getValue() == 0) {
        return;
    } else if(resizeSize.getValue() > 0) {
        resize(m, m, cv::Size(), resizeSize.getValue(), resizeSize.getValue());
    } else {
        resize(m, m, cv::Size(), 1.0/(-resizeSize.getValue()), 1.0/(-resizeSize.getValue()));
    }
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