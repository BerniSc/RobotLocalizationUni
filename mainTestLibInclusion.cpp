#include <opencv4/opencv2/highgui.hpp>
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

    namedWindow("Display window", WINDOW_AUTOSIZE);
    imshow("Display window", image);

    namedWindow("Display window Grey", WINDOW_AUTOSIZE);
    imshow("Display window Grey", imageGrey);

    waitKey(0);

    return 0;
}