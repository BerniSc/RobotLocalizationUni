#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    Mat image;
    Mat imageGrey;

    image = imread("Eisbaer.jpg", IMREAD_COLOR);
    imageGrey = imread("Eisbaer.jpg", IMREAD_GRAYSCALE);

    if(!image.data && !imageGrey.data) {
        cout << "Kann Bild nicht öffnen!" << endl;
        return -1;
    }

    namedWindow("Normal Image", WINDOW_AUTOSIZE);
    imshow("Normal Image", image);

    waitKey(0);
    return 0;
}

Mat& bluredImage() {
    
}