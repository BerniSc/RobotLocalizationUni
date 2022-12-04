#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

#include "control_window_params.hpp"
#include "image_collection.hpp"
#include "square_detection.hpp"
#include "circle_detection.hpp"

#include <opencv2/video.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"

using namespace cv;
using namespace std;

int blurModeGaussian = 0;
int thresholdModeBool = 0;

vector<Point> corners;

const bool testMode = false;

bool warpMode = true;
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
parameterDescription thresholdLow(0,255, 80, "Lower Threshold Value");
parameterDescription thresholdHigh(0, 255, 255, "Higher Threshold Value");
parameterDescription squareDetection(0, 1, 1, "Square Detection (Calib. Mode)");
parameterDescription cricleDetection(0, 1, 1, "Clear Gray Circledetection Mode");
parameterDescription displayMode(0, 1, 1, "Display Mode");

/***/
void callback_trackbar_thresholdMode(int mode, void* userData);
void callback_trackbar_BlurMode(int mode, void* userData);
void callback_trackbar_BlurSize(int blurKernelValue, void* userData);
void callback_trackbar_ThresholdCannyLow(int cannyLow, void* userData);
void callback_trackbar_ThresholdCannyHigh(int cannyHigh, void* userData);
void callback_trackbar_WindowSize(int WindowSize, void* userData);
void callback_trackbar_DisplayWindowSize(int WindowSize, void* userData);
void callback_trackbar_ThresholdLow(int cannyLow, void* userData);
void callback_trackbar_ThresholdHigh(int cannyLow, void* userData);
void callback_trackbar_squareDetection(int mode, void *userData);
void callback_trackbar_circleDetection(int mode, void *userData);
void callback_trackbar_displayMode(int mode, void *userData);

void refresh();

Mat getCanny(Mat& original);
Mat getBlurred(Mat& original);

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
    if(testMode) {
        namedWindow("Gray Image", WINDOW_AUTOSIZE);
        namedWindow("Blurred Image", WINDOW_AUTOSIZE);
        namedWindow("Canny Image", WINDOW_AUTOSIZE);
    }
    
    createTrackbar(thresholdMode.name, "Control Window", &thresholdMode.selectedValue, thresholdMode.getMaxValueForSlider(), callback_trackbar_thresholdMode, &image);
    if(testMode) createTrackbar(resizeSize.name, "Control Window", &resizeSize.selectedValue, resizeSize.getMaxValueForSlider(), callback_trackbar_WindowSize, &imageGray);
    createTrackbar(blurMode.name, "Control Window", &blurMode.selectedValue, blurMode.getMaxValueForSlider(), callback_trackbar_BlurMode, &image);
    createTrackbar(blurSize.name, "Control Window", &blurSize.selectedValue, blurSize.getMaxValueForSlider(), callback_trackbar_BlurSize, &image);
    createTrackbar(cannyLow.name, "Control Window", &cannyLow.selectedValue, cannyLow.getMaxValueForSlider(), callback_trackbar_ThresholdCannyLow, &imageBlurred);
    createTrackbar(cannyHigh.name, "Control Window", &cannyHigh.selectedValue, cannyHigh.getMaxValueForSlider(), callback_trackbar_ThresholdCannyHigh, &imageBlurred);
    createTrackbar(displayWindowSize.name, "Control Window", &displayWindowSize.selectedValue, displayWindowSize.getMaxValueForSlider(), callback_trackbar_DisplayWindowSize, &image);
    createTrackbar(thresholdLow.name, "Control Window", &thresholdLow.selectedValue, thresholdLow.getMaxValueForSlider(), callback_trackbar_ThresholdLow, &image);
    createTrackbar(thresholdHigh.name, "Control Window", &thresholdHigh.selectedValue, thresholdHigh.getMaxValueForSlider(), callback_trackbar_ThresholdHigh, &image);
    createTrackbar(squareDetection.name, "Control Window", &squareDetection.selectedValue, squareDetection.getMaxValueForSlider(), callback_trackbar_squareDetection, &image);
    createTrackbar(displayMode.name, "Control Window", &displayMode.selectedValue, displayMode.getMaxValueForSlider(), callback_trackbar_displayMode, &image);

    /********TESTING WARP***********/
    namedWindow("warped", 0);
    int destWidth = 440;
    int destHeight = 440;
    Point2f cornersFloat[4];
    //PROBLEM -> Festlegung zwischen Affine Transformation (weniger rechenintensiv + beibehaltung Parallelität) oder Perspective Transform (mächtiger)
    //Affine benötigt 3 Punkte -> Falls verdreht evtl Reihnfolge der Punkt tauschen i.E. Punkt 0 an schluss verschieben oder so) -> Reinfolge der Erkennung
    //Evlt auch mit slidern o.ä. regeln
    //Sobald tatsächlichen Kameraaufbau genauere Anpassung der Vergleichspunkte (Startpunkte) für "Corners" und reihnfolge destCorners mgl da wahl der Transformation möglich
    
    //Ansatzpunkt für Michael aus outPut Warped Kreiserkennung ortung möglich -> evlt auch nicht image warpen sondern Canny o.ä. -> Algorithmus bereits gelaufen -> FPS schon bei flüssig min 30
    Point2f destCorners[4] = {Point(0, 0), Point(0, destHeight), Point(destWidth, destHeight), Point(destWidth, 0)};
    Mat outputWarped;
    Mat warpMat;

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
        
        imageBlurredGray = getBlurred(imageGray);

        refresh();

        waitKey(0);
    } else {
        //vector<vector<Point>> squares;
        //FALLS DICKE PUNKTE (Warp Punkte nicht richtig erkannt werden hier Vergleichswerte entsprechend anpassen)
        corners.push_back(Point(300, 120));
        corners.push_back(Point(320, 240)); 
        corners.push_back(Point(400, 120));
        corners.push_back(Point(200, 120));

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
            if(thresholdModeBool) threshold(imageGray, imageGray, thresholdLow.getValue(), thresholdHigh.getValue(), THRESH_BINARY);
            imageBlurred = getBlurred(imageGray);
            imageCanny = getCanny(imageBlurred);

            imageBlurredGray = getBlurred(imageGray);

            if(squareDetection.getValue()) findSquares(imageCanny, image, corners);
            //drawSquares(image, squares);

            //COLOR changedby Scalar -> Order is B-G-R
            putText(image, "MAX FPS: " + to_string(CAP_PROP_FPS), {25, 50}, FONT_HERSHEY_PLAIN, 2, Scalar(153, 153, 0), 3);
            putText(image, "FPS: " + to_string(fpsLive), {50, image.rows-50}, FONT_HERSHEY_COMPLEX, 1.5, Scalar(153, 153, 0), 2);

            //FOR DETECTING AND SHOWING CORNERPOINTS
            
            for(auto i : corners) {
                //cout << i << "      ";
                drawPoint(image, i, 5);
            }
            //cout << "" << endl;
            //*/ 

           if(warpMode) {
            if(corners.at(0).y < corners.at(1).y) {
                cornersFloat[0] = corners.at(0);
                cornersFloat[3] = corners.at(1);
            } else {  
                cornersFloat[3] = corners.at(0);
                cornersFloat[0] = corners.at(1);
            }
            if(corners.at(2).y > corners.at(3).y) {
                cornersFloat[1] = corners.at(2);
                cornersFloat[2] = corners.at(3);
            } else {
                cornersFloat[2] = corners.at(2);
                cornersFloat[1] = corners.at(3);
            }
 
            //cout << cornersFloat[0] << "    " << cornersFloat[1] << "   " << cornersFloat[2] << "   " << cornersFloat[3] << endl;

            //imshow("Image Canny C", imageCanny); 
            //warpMat = getPerspectiveTransform(cornersFloat, destCorners);
            //warpPerspective(image, outputWarped, warpMat, cv::Size(640, 480));
            
            warpMat = getAffineTransform(cornersFloat, destCorners);
            //FOR DISPLAY PUROPOSES
            //warpAffine(image, outputWarped, warpMat, Size(destWidth, destHeight));
            if(cricleDetection.getValue()) {
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


            if(!squareDetection.getValue()) createTrackbar(cricleDetection.name, "Control Window", &cricleDetection.selectedValue, cricleDetection.getMaxValueForSlider(), callback_trackbar_circleDetection, &image);

            waitKey(10);
        }
    }
    return 0;

    //cerr << getBuildInformation();
}

void callback_trackbar_thresholdMode(int mode, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    thresholdModeBool = !thresholdModeBool;
    warpMode = true;
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

void callback_trackbar_ThresholdCannyLow(int cannyLow, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    imageCanny = getCanny(m);
    //imshow("Canny Image", imageCanny);
    refresh();
}

void callback_trackbar_ThresholdCannyHigh(int cannyHigh, void* userData) {
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

void callback_trackbar_ThresholdLow(int cannyLow, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    refresh();
}

void callback_trackbar_ThresholdHigh(int cannyHigh, void* userData) {
    Mat m = *(static_cast<Mat*>(userData));
    refresh();
}

void callback_trackbar_squareDetection(int mode, void *userData) {
    Mat m = *(static_cast<Mat *>(userData));
}

void callback_trackbar_circleDetection(int mode, void *userData) {
    Mat m = *(static_cast<Mat *>(userData));
}

void callback_trackbar_displayMode(int mode, void *userData) {
    Mat m = *(static_cast<Mat *>(userData));
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