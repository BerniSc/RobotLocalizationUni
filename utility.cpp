#include "utility.hpp"

cv::Mat getCanny(cv::Mat& original, parameterDescription &lowerThreshold, parameterDescription &higherThreshold) {
    cv::Mat imageCanny;
    cv::Canny(original, imageCanny, lowerThreshold.getValue(), higherThreshold.getValue()); 
    return imageCanny;
}

cv::Mat getBlurred(cv::Mat& original, parameterDescription &blurMode, parameterDescription &blurSize) {
    cv::Mat blurred;
    if(blurMode.getValue()) {
        cv::GaussianBlur(original, blurred, cv::Size(blurSize.getValue(), blurSize.getValue()), 0, 0);
    } else {
        cv::medianBlur(original, blurred, blurSize.getValue());
    }
    return blurred;
}