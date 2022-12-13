#ifndef CONTROLL_WINDOW_H
#define CONTROLL_WINDOW_H

#include <string>
#include <vector>
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include "utility.hpp"

struct parameterController;
struct parameterDescription;

struct parameterController {
    public:
        parameterController() = default;

        std::vector<parameterDescription*>  descriptors;

        void addParam(parameterDescription* param);
        
        void createTrackbars();

        void printCurrentConfig();
        void loadConfig(int configNr);
};

struct parameterDescription {
    public:
        const int minValue;
        const int maxValue;
        const std::string name;

        cv::TrackbarCallback callbackFunction;

        int selectedValue = -1;
        int startValue;
    
        parameterDescription(int minValue, int maxValue, int startValue, std::string name, cv::TrackbarCallback callbackFunction) : minValue(minValue), maxValue(maxValue), startValue(startValue), name(name), callbackFunction(callbackFunction) {

        }

        int getValue();
        int getMaxValueForSlider();
}; 

#endif