#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include <iostream>
#include <math.h>
#include <string.h>

using namespace cv;
using namespace std;

static void help()
{
    cout << "\nA program using pyramid scaling, Canny, contours, contour simpification and\n"
            "memory storage to find squares in a list of images\n"
            "Returns sequence of squares detected on the image.\n"
            "the sequence is stored in the specified memory storage\n"
            "Call:\n"
            "./squares\n"
            "Using OpenCV version %s\n"
         << CV_VERSION << "\n"
         << endl;
}

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
                    if (maxCosine < 0.3)
                        squares.push_back(approx);
                }
            }
        }
    }
}

// the function draws all the squares in the image
static void drawSquares(Mat &image, const vector<vector<Point>> &squares)
{
    for (size_t i = 0; i < squares.size(); i++)
    {
        const Point *p = &squares[i][0];

        int n = (int)squares[i].size();
        // dont detect the border
        if (p->x > 3 && p->y > 3)
            polylines(image, &p, &n, 1, true, Scalar(0, 255, 0), 3, LINE_AA);
    }

    imshow(wndname, image);
}



int main(int /*argc*/, char ** /*argv*/)
{
    if (testMode)
    {
        // static const char *names[] = {"imgs/2Stickies.jpg", "imgs/manyStickies.jpg", 0};
        help();
        namedWindow(wndname, 1);
        vector<vector<Point>> squares;

        VideoCapture cap(0);
        Mat image;

        while (true)
        {
            cap >> image;

            resize(image, image, Size(image.cols / 4, image.rows / 4));
            imshow("Test", image);
            //findSquares(image, squares);
            //drawSquares(image, squares);
            // imwrite( "out", image );
            //int c = waitKey(500);
        }
    }
    return 0;
}