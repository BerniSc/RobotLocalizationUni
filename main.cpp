#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include <iostream>
#include <math.h>
#include <string.h>
#include <time.h>

using namespace cv;
using namespace std;

// int thresh = 50, N = 5;
int thresh = 40, N = 5;
float sensitivity = 0.02; // Start bei 0.02
const char *wndname = "Square Detection Demo";

const bool testMode = true;

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle(Point pt1, Point pt2, Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
static void findSquares(const Mat &image, vector<vector<Point>> &squares)
{
    squares.clear();

    // blur will enhance edge detection
    Mat imageBlurred(image);
    // medianBlur(image, imageBlurred, 9);
    medianBlur(image, imageBlurred, 11);
    Mat gray0(imageBlurred.size(), CV_8U), gray;

    vector<vector<Point>> contours;

    // find squares in every color plane of the image
    for (int c = 0; c < 3; c++)
    {
           int ch[] = {c, 0};
           mixChannels(&imageBlurred, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for (int l = 0; l < N; l++)
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if (l == 0)
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                // Canny(gray0, gray, 5, thresh, 5);
                Canny(gray0, gray, 35, 90);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1, -1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l + 1) * 255 / N;
            }

            // find contours and store them all as a list
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

            // test each contour
            for (size_t i = 0; i < contours.size(); i++)
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * sensitivity, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if (approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 1000 &&
                    isContourConvex(Mat(approx)))
                {
                    double maxCosine = 0;

                    for (int j = 2; j < 5; j++)
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    //0,3
                    if (maxCosine < 0.3)
                        squares.push_back(approx);
                }
            }
        }
    }
}

bool comparePointsX(const Point &a, const Point &b) {
    return(a.x > b.x);
}

// the function draws all the squares in the image
static void drawSquares(Mat &image, vector<vector<Point>> &squares)
{
    for (size_t i = 0; i < squares.size(); i++)
    {
        Point *p = &squares[i][0];
        Point center(0, 0);

        /*
        sort(squares.begin(), squares.end(), [](vector<Point> const &a, vector<Point> const &b) {
            return (a.at(0).x > b.at(0).x);
        });
        */

        if(abs(squares[i][0].x - squares[i][3].x) > 10) {
            center.x = squares[i][0].x+((squares[i][3].x)-(squares[i][0].x))/2;
        } else {
            center.x = squares[i][0].x+((squares[i][2].x)-(squares[i][0].x))/2;
        }

        if(abs(squares[i][0].y - squares[i][3].y) > 10) {
            center.y = squares[i][0].y+((squares[i][3].y-(squares[i][0].y)))/2;
        } else {
            center.y = squares[i][0].y+((squares[i][2].y)-(squares[i][0].y))/2;
        }

        //cout << squares[i][0] << "  " << squares[i][1]<< "  " <<squares[i][2]  << " " << squares[i][3] << "         " << center << endl;

        int n = (int)squares[i].size();
        // dont detect the border
        if (p->x > 3 && p->y > 3) 
            polylines(image, &p, &n, 1, true, Scalar(0, 255, 0), 3, LINE_AA);

            circle(image, center, 1, Scalar(0, 255, 0), 5);
    }

    imshow(wndname, image);
}



int main(int /*argc*/, char ** /*argv*/)
{
    if (testMode)
    {
        namedWindow(wndname, 1);
        vector<vector<Point>> squares;

        VideoCapture cap(0);

        double fps = cap.get(CAP_PROP_FPS);
        cout << "FPS CAM: " << fps << endl;

        int numFrames = 1;

        clock_t start, end;

        double msBetweenFrames, fpsLive;

        Mat image, imageGray;

        while (true)
        {
            start = clock();

            cap >> image;

            //resize(image, image, Size(image.cols / 2, image.rows / 2));

            //cvtColor(image, imageGray, COLOR_BGR2GRAY);

            findSquares(image, squares);
            drawSquares(image, squares);

            end = clock();

            msBetweenFrames = (double(end) - double(start)) / double(CLOCKS_PER_SEC);

            fpsLive = double(numFrames) / double(msBetweenFrames);

            putText(image, "FPS: " + to_string(fpsLive), {50, 50}, FONT_HERSHEY_TRIPLEX, 1.5, (240, 120, 255));

            imshow("Test", image);

            // imwrite( "out", image );
            //int c = waitKey(500);
            waitKey(10);
        }
    }
    return 0;
}