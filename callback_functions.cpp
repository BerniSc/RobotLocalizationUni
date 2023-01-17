#include "callback_functions.hpp"

/**
 * Second Parameter is pointer to parameterDescription object
 * It can be used to increase the Options for using the sliders from switching from start->end in single steps to something else 
 * An exemplary implementation of this can be seen in callback_trackbar_BlurSize
*/

void callback_trackbar_thresholdMode(int mode, void* userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}

void callback_trackbar_BlurMode(int mode, void* userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}

void callback_trackbar_BlurSize(int blurKernelValue, void* userData) {
    parameterDescription *param = static_cast<parameterDescription*>(userData);
    //std::cout << param->getName();
    blurKernelValue = blurKernelValue*2 +1;
    param->selectedValue = blurKernelValue;
}

void callback_trackbar_ThresholdCannyLow(int cannyLow, void* userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}

void callback_trackbar_ThresholdCannyHigh(int cannyHigh, void* userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}

void callback_trackbar_DisplayWindowSize(int WindowSize, void* userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}

void callback_trackbar_ThresholdLow(int cannyLow, void* userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}

void callback_trackbar_ThresholdHigh(int cannyHigh, void* userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}

void callback_trackbar_squareDetection(int mode, void *userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}

void callback_trackbar_circleDetection(int mode, void *userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}

void callback_trackbar_displayMode(int mode, void *userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}

void callback_trackbar_adaptiveMode(int mode, void *userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}

void callback_trackbar_inverseAccumulator(int mode, void *userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}
void callback_trackbar_minDistCircles(int mode, void *userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}
void callback_trackbar_upperCannyCircle(int mode, void *userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}
void callback_trackbar_thresholdCenterDetection(int mode, void *userData) {
    //parameterDescription *param = static_cast<parameterDescription*>(userData);
}

void callback_mouse_doubleclicked(int event, int x, int y, int flags, void* userData) {
    if(event == cv::EVENT_LBUTTONDBLCLK) {
        std::cout << "Mouse has been double Clicked over Warped Image -> publishing goto Point " << x << "|" << y << std::endl; 
    }
}