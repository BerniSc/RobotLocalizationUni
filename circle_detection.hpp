#ifndef CIRCLE_DETECTION_H
#define CIRCLE_DETECTION_H

#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>

#include "utility.hpp"

#include <iostream>
#include <utility>

void drawCircles(cv::Mat &input, const std::vector<cv::Vec3f> &circles, cv::Scalar color = cv::Scalar(255, 0, 255));
std::vector<cv::Vec3f> findCircles(cv::Mat &input, cv::Mat &inputDrawOn, const std::pair<int, int> &detectionSizes, cv::Scalar color = cv::Scalar(255, 0, 255));

#endif