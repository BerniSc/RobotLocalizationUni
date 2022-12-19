#include "callback_functions.hpp"

void callback_trackbar_thresholdMode(int mode, void* userData) {

}

void callback_trackbar_BlurMode(int mode, void* userData) {

}

void callback_trackbar_BlurSize(int blurKernelValue, void* userData) {
    parameterDescription param = *(static_cast<parameterDescription*>(userData));
    std::cout << param.name;
    blurKernelValue = blurKernelValue*2 +1;
    //std::cout << "Hi  " << blurKernelValue << "     " << param.getValue() << std::endl; 
    param.selectedValue = blurKernelValue;
    //std::cout << "Hi  " << blurKernelValue << "     " << param.getValue() << std::endl; 
}

void callback_trackbar_ThresholdCannyLow(int cannyLow, void* userData) {
    parameterDescription param = *(static_cast<parameterDescription*>(userData));
}

void callback_trackbar_ThresholdCannyHigh(int cannyHigh, void* userData) {
    parameterDescription param = *(static_cast<parameterDescription*>(userData));
}

void callback_trackbar_DisplayWindowSize(int WindowSize, void* userData) {
    parameterDescription param = *(static_cast<parameterDescription*>(userData));
}

void callback_trackbar_ThresholdLow(int cannyLow, void* userData) {
    parameterDescription param = *(static_cast<parameterDescription*>(userData));
}

void callback_trackbar_ThresholdHigh(int cannyHigh, void* userData) {
    parameterDescription param = *(static_cast<parameterDescription*>(userData));
}

void callback_trackbar_squareDetection(int mode, void *userData) {
    parameterDescription param = *(static_cast<parameterDescription*>(userData));
}

void callback_trackbar_circleDetection(int mode, void *userData) {
    parameterDescription param = *(static_cast<parameterDescription*>(userData));
}

void callback_trackbar_displayMode(int mode, void *userData) {
    parameterDescription param = *(static_cast<parameterDescription*>(userData));
}

void callback_trackbar_adaptiveMode(int mode, void *userData) {
    parameterDescription param = *(static_cast<parameterDescription*>(userData));
}