#include "circle_detection.hpp"

void drawCircles(cv::Mat &input, const std::vector<cv::Vec3f> &circles, cv::Scalar colour) {
   for (unsigned long i = 0; i < circles.size(); i++) //Drawing the circles
    {
        cv::Vec3i c = circles[i];
        cv::Point center = cv::Point(c[0], c[1]);//drawing the centre

        drawPoint(input, center);
        //circle(frame, center, 1, Scalar(0, 100, 100), 3, LINE_AA); //Circle outline
        int radius = c[2];
        circle(input, center, radius, colour, 3, cv::LINE_AA);

        std::cout << "Drew Point at " << center << " with r=" << radius << "    Lenght:" << circles.size() <<std::endl;
    }
}

std::vector<cv::Vec3f> findCircles(cv::Mat &input, cv::Mat &inputDrawOn, const std::pair<int, int> &detectionSizes, cv::Scalar colour) {
    std::vector<cv::Vec3f> circles;

    //cv::HoughCircles(input, circles, cv::HOUGH_GRADIENT, 1, input.rows / 16, 100, 30, diameterStart, diameterEnd);
    cv::HoughCircles(input, circles, cv::HOUGH_GRADIENT, 1, input.rows / 16, 100, 30, detectionSizes.first, detectionSizes.second);

    if(debug_consts::drawDetectedCircles) drawCircles(inputDrawOn, circles, colour);

    return circles;
}