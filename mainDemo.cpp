#include <opencv4/opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
    Mat image;
    Mat imageGray, imageBlur, imageCanny;

    //namedWindow("Display window", WINDOW_AUTOSIZE);

    VideoCapture cap(0);

    if(!cap.isOpened()) {
        cout << "cannot open camera";
    }

    while(true) {
        cap >> image;

        resize(image, image, Size(image.cols/8, image.rows/8));

        cvtColor(image, imageGray, COLOR_BGR2GRAY);

        GaussianBlur(imageGray, imageBlur, Size(3, 3), 0);

        Canny(imageBlur, imageCanny, 40, 120);

        imshow("Display window normal", image);
        imshow("Display window Gray", imageGray);
        imshow("Display window Blur", imageBlur);
        imshow("Display window Canny", imageCanny);

        waitKey(1);
    }
    return 0;
}