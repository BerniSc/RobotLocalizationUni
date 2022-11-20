#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int blurModeGaussian = 0;
int blurSizeKernel;
int cannyThresholdLow = 30;
int cannyThresholdHigh = 120;

Mat imageBlurred;
Mat imageCanny;

/***/
void callback_trackbar_BlurMode(int mode, void* userData);
void callback_trackbar_BlurSize(int blurKernelValue, void* userData);
void callback_trackbar_ThresholdLow(int cannyLow, void* userData);
void callback_trackbar_ThresholdHigh(int cannyHigh, void* userData);

int main(int argc, char** argv) {
    Mat image;
    Mat imageGray;

    image = imread("Eisbaer.jpg", IMREAD_COLOR);
    imageGray = imread("Eisbaer.jpg", IMREAD_GRAYSCALE);

    if(!image.data && !imageGray.data) {
        cout << "Kann Bild nicht öffnen!" << endl;
        return -1;
    }

    namedWindow("Normal Image", WINDOW_AUTOSIZE);
    imshow("Normal Image", image);

    namedWindow("Gray Image", WINDOW_AUTOSIZE);
    imshow("Gray Image", imageGray);

    namedWindow("Blurred Image", WINDOW_AUTOSIZE);
    imageBlurred = image;
    createTrackbar("Blur Mode", "Blurred Image", &blurModeGaussian, 1, callback_trackbar_BlurMode);
    createTrackbar("Blur Size", "Blurred Image", &blurSizeKernel, 6, callback_trackbar_BlurSize, &image);

    namedWindow("Canny Image", WINDOW_AUTOSIZE);
    imageCanny = imageBlurred;
    createTrackbar("Canny Threshold Low", "Canny Image", &cannyThresholdLow, 30, callback_trackbar_ThresholdLow, &imageBlurred);
    createTrackbar("Canny Threshold High", "Canny Image", &cannyThresholdHigh, 120, callback_trackbar_ThresholdHigh, &imageBlurred);
    //cerr << getBuildInformation();
    imshow("Blurred Image", image);

    waitKey(0);
    return 0;
}

void callback_trackbar_BlurMode(int mode, void* userData) {
    blurModeGaussian = !blurModeGaussian;
    cout << "Button Pressed " << blurModeGaussian << endl;
}

void callback_trackbar_BlurSize(int blurKernelValue, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    blurKernelValue = blurKernelValue*2 +1;
    //cout << "valueBlurKernel: " << blurKernelValue << endl;
    if(blurModeGaussian) {
        GaussianBlur(m, imageBlurred, Size(blurKernelValue, blurKernelValue), 0, 0);
    } else {
        medianBlur(m, imageBlurred, blurKernelValue);
    }
    imshow("Blurred Image", imageBlurred);
}

void callback_trackbar_ThresholdLow(int cannyLow, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    Canny(m, imageCanny, cannyLow, cannyThresholdHigh, 3, false);
    imshow("Canny Image", imageCanny);
}

void callback_trackbar_ThresholdHigh(int cannyHigh, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    Canny(m, imageCanny, cannyThresholdLow, cannyHigh, 3, false);
    imshow("Canny Image", imageCanny);
}