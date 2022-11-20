#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

static void callback_trackbar_bluring(int valueBlurKernel, void *userData) {
    Mat m = *(static_cast<Mat*>(userData));
    Mat dest;
    cout << "valueBlurKernel: " << valueBlurKernel << endl;
    valueBlurKernel = valueBlurKernel*2 +1;
    GaussianBlur(m, dest, Size(valueBlurKernel, valueBlurKernel), 0, 0);
    imshow("Display Trackbar", dest);
}

int main(int argc, char** argv) {
    Mat image;
    Mat imageGrey;

    image = imread("Eisbaer.jpg", IMREAD_COLOR);
    imageGrey = imread("Eisbaer.jpg", IMREAD_GRAYSCALE);

    if(!image.data && !imageGrey.data) {
        cout << "Kann Bild nicht öffnen!" << endl;
        return -1;
    }

    int valueBlurKernel = 3;

    namedWindow("Display window", WINDOW_AUTOSIZE);
    imshow("Display window", image);

    namedWindow("Display Trackbar", 1);
    imshow("Display Trackbar", image);
    createTrackbar("Bluring", "Display Trackbar", &valueBlurKernel, 6, &callback_trackbar_bluring, &image);

    namedWindow("Display window Grey", WINDOW_AUTOSIZE);
    imshow("Display window Grey", imageGrey);

    waitKey(0);

    return 0;
}