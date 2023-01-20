#ifndef CONTROLL_WINDOW_HPP
#define CONTROLL_WINDOW_HPP

#include <string>
#include <vector>
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include "utility.hpp"

struct parameterController;
struct parameterDescription;

struct parameterController {
    private:
        std::string windowName = "Control Window";
    public:
        parameterController() = default;
        parameterController(std::string windowName);

        std::vector<parameterDescription*>  descriptors;

        void addParam(parameterDescription* param);
        
        void createTrackbars();

        void printCurrentConfig();
        void loadConfig(unsigned long configNr);
};

struct parameterDescription {
    private:
        const int minValue;
        const int maxValue;
        
        int startValue;
        
        const std::string name;
        cv::TrackbarCallback callbackFunction;
    public:
        int selectedValue = -1;
    
        parameterDescription(int minValue, int maxValue, int startValue, std::string name, cv::TrackbarCallback callbackFunction);

        std::string getName() const;
        cv::TrackbarCallback getCallbackFunction() const;
        int getValue() const;
        int getMaxValueForSlider() const;
}; 

#endif