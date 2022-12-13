#pragma once

#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>

#include "utility.hpp"

const float sensitivity = 0.02f;

const int minSizeContour = 1000;
const int maxSizeContour = 3500;

bool sortByXAxis(const cv::Point &a, const cv::Point &b) {
    return a.x < b.x;
}

bool sortByYAxis(const cv::Point &a, const cv::Point &b) {
    return a.y < b.y;
}

double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

void drawSquares(cv::Mat &image, std::vector<cv::Point> &squares) {
    //for(size_t i = 0; i < squares.size(); i++) {
        cv::Point *p = &squares[0];

        int n = (int) squares.size();

        //das Bildrahmen nicht erkannt wird
        if(squares[0].x > 3 && squares[0].y > 3) {
            cv::polylines(image, &p, &n, 1, true, cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
        }
    //}
}

void findSquares(const cv::Mat &imageCanny, cv::Mat &imageToDrawOn, std::vector<cv::Point> &corners) {
    //squares.clear();

    std::vector<std::vector<cv::Point>> contours;

    cv::dilate(imageCanny, imageCanny, cv::Mat(), cv::Point(-1, -1));

    findContours(imageCanny, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Point> approx;

    for(size_t i = 0; i < contours.size(); i++) {
        cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * sensitivity, true);

        //4 Eckpunkte wegen Viereck, Größe um kleine Fehler herauszufiltern, Betrag, weil je nach Orientierung Area negativ möglich und Convex um tatsächliche Rechtecke zu finden 
        if(approx.size() == 4 && fabs(cv::contourArea(cv::Mat(approx))) > minSizeContour && (fabs(cv::contourArea(cv::Mat(approx))) < maxSizeContour || 1) && cv::isContourConvex(cv::Mat(approx))) {
            double maxCosine = 0;

            //std::cout << "Max Size = " << fabs(cv::contourArea(cv::Mat(approx))) << std::endl;

            for(int j = 2; j < 5; j++) {
                double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
                maxCosine = MAX(maxCosine, cosine);
            }

            //Wenn Cos kleiner als 0,3 dann Ecke fast 90°, deswegen Rechteck
            if(maxCosine < 0.3) {
                drawSquares(imageToDrawOn, approx);
                
                sort(approx.begin(), approx.end(), sortByXAxis);

                cv::Point center(0, 0);

                center.x = (approx[3].x - approx[0].x) / 2 + approx[0].x;
                center.y = (approx[1].y - approx[0].y) / 2 + approx[0].y;

                if(center.x < corners[0].x && center.y < corners[0].y) {
                    corners[0] = center;
                } else if(center.x < corners.at(1).x && center.y > corners.at(1).y) {
                    corners.at(1) = center;
                } else if(center.x > corners.at(2).x && center.y > corners.at(2).y) {
                    corners.at(2) = center;
                }

                drawPoint(imageToDrawOn, center, 5);
            }
        }
    }
}