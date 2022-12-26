#include "control_window_params.hpp"

void parameterController::addParam(parameterDescription* param) {
    parameterController::descriptors.push_back(param);
}

void parameterController::printCurrentConfig() {
    std::cout << "-----===== Your Current Config =====-----" << std::endl;
    for(auto descriptor : parameterController::descriptors) {
        std::cout << descriptor->name << " # " << descriptor->getValue() << std::endl;
    }
    std::cout << "_________________________________________" << std::endl;
}

void parameterController::loadConfig(unsigned long configNr) {
    std::cout << "Loading Config " << configNr << " as current Config!" << std::endl;
    std::vector<std::vector<std::string>> configData;
    readInData("param_config.pacf", configData);
    
    if(configNr < 0 || configNr > 9 || (configData.size() < (configNr + 1))) {
        std::cerr << "\n\tAn Error occured during loading of the Config!!\n\t Please Try Again!!" << std::endl; 
        return;
    }

    std::vector<std::string> singleConfig = configData[configNr];

    for(unsigned long i = 1; i < singleConfig.size() - 1; i++) {
        std::vector<std::string> seperatedData = seperateString(singleConfig[i], "#");
        descriptors[i-1]->selectedValue = std::stoi(seperatedData[1]);
    }
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