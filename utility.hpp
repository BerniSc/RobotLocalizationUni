#ifndef UTILITY_H
#define UTILITY_H

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include "control_window_params.hpp"

cv::Mat getBlurred(cv::Mat& original, parameterDescription &blurMode, parameterDescription &blurSize);
cv::Mat getCanny(cv::Mat& original, parameterDescription &lowerThreshold, parameterDescription &higherThreshold);

#endif