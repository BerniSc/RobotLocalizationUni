#ifndef CONTROLL_WINDOW_H
#define CONTROLL_WINDOW_H

#include <string>
#include <vector>

struct parameterDescription {
    public:
        const int minValue;
        const int maxValue;
        const std::string name;

        int selectedValue = -1;
        int startValue;
    
        parameterDescription(int minValue, int maxValue, int startValue, std::string name) : minValue(minValue), maxValue(maxValue), startValue(startValue), name(name) {

        }

        int getValue() {
            if(selectedValue == -1) {
                return startValue;
            }
            return selectedValue - minValue;
        }

        int getMaxValueForSlider() {
            return maxValue + abs(minValue);
        }
}; 

/*  Change Brightness Values    */

/*  Change Mode Bluring */

#endif