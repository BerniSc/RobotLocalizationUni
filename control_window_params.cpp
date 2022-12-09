#include "control_window_params.hpp"

void parameterController::addParam(parameterDescription* param) {
    parameterController::descriptors.push_back(param);
}

void parameterController::printCurrentConfig() {
    std::cout << "-----===== Your Current Config =====-----" << std::endl;
    for(auto descriptor : parameterController::descriptors) {
        std::cout << descriptor->name << "  " << descriptor->getValue() << std::endl;
    }
    std::cout << "_________________________________________" << std::endl;
}

void parameterController::createTrackbars() {
    for(auto descriptor : parameterController::descriptors) {
        cv::createTrackbar(descriptor->name, "Control Window", &descriptor->selectedValue, descriptor->getMaxValueForSlider(), descriptor->callbackFunction, descriptor);
    }
}

int parameterDescription::getValue() {
    if(parameterDescription::selectedValue == -1) {
        return parameterDescription::startValue;
    }
    return parameterDescription::selectedValue - parameterDescription::minValue;
}

int parameterDescription::getMaxValueForSlider() {
    return parameterDescription::maxValue + abs(parameterDescription::minValue);
}