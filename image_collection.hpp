#pragma once

#include "string"
#include "opencv4/opencv2/core/core.hpp"
#include "opencv4/opencv2/imgproc/imgproc.hpp"

struct ImageCollection {
    cv::Mat image;
    std::string displayWindow; 

    ImageCollection(cv::Mat &image, std::string displayWindow) : image(image), displayWindow(displayWindow) {

    }

    void setImage(cv::Mat &image) {
        this->image = image;
    }
};