#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "hough.h"
#include "utils.h"

extern std::string imageName; // image name

cv::Mat src, src_gray, grad, dst;
cv::Mat grad_x, grad_y, abs_grad_x, abs_grad_y;

int slobel_lowThreshold = 40;
int scale = 1;
int delta = 0;
int ddepth = CV_16S;

const char* CW_IMG_ORIGINAL = "Result";
const char* CW_IMG_EDGE	= "Canny Edge Detector";
const char* CW_ACCUMULATOR  = "Accumulator";

cv::Mat SobelThreshold()
{
    Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );
    Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );
    addWeighted( abs_grad_x, 1, abs_grad_y, 1, 0, grad );
    threshold(grad,grad,slobel_lowThreshold,255,cv::THRESH_TOZERO);
    return grad;
}

int main()
{
    src = cv::imread(imageName);
    if (src.empty()) { fprintf(stderr, "Can not load image\n"); return -1; }
    GaussianBlur(src, src, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
    dst.create(src.size(), src.type());
    cvtColor(src, src_gray, CV_BGR2GRAY);

    cv::namedWindow(CW_IMG_ORIGINAL, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(CW_IMG_EDGE, 	 cv::WINDOW_AUTOSIZE);
    cv::namedWindow(CW_ACCUMULATOR,	 cv::WINDOW_AUTOSIZE);

    cvMoveWindow(CW_IMG_ORIGINAL, 10, 10);
    cvMoveWindow(CW_IMG_EDGE, 680, 10);
    cvMoveWindow(CW_ACCUMULATOR, 1350, 10);

    int w = src.cols;
    int h = src.rows;

    /// Sobel
    //dst=SobelThreshold();

    /// Canny
    int const canny_threshold = 50;
    Canny( src_gray, dst, MAX(canny_threshold/2,1), canny_threshold, 3 );
    cv::Sobel( src_gray, grad_x, CV_16S, 1, 0, 3 );
    cv::Sobel( src_gray, grad_y, CV_16S, 0, 1, 3 );

    /// hough Transform
    zjg::Hough hough;
    hough.Transform(dst.data, grad_x, grad_y, w, h);

    cv::Mat img_res = src.clone();

    //Search the accumulator
    /// Get Polar Points
    std::vector< std::pair<int, int> > points = hough.GetPoints();

    /// Get Real Lines
    std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > > lines = hough.GetLines();

    /// choose the right quadrangle
    std::vector<Points> v4points = FindQuadrangle(points,w,h);

    /// Affine Transform
    DoAffineTransform(v4points,img_res);

    /// Draw the key points
    for(int i=0;i<v4points.size();i++)
    {
        cv::circle(img_res, cv::Point(v4points[i].x,v4points[i].y), 10, cv::Scalar(0, 255, 0), - 1);
    }

    /// Draw the lines
    std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > >::iterator it;
    for(it=lines.begin();it!=lines.end();it++)
    {
        cv::line(img_res, cv::Point(it->first.first, it->first.second), cv::Point(it->second.first, it->second.second), cv::Scalar( 0, 0, 255), 2, 8);
    }

    cv::imshow(CW_IMG_EDGE, dst);
    cv::imshow(CW_IMG_ORIGINAL, img_res);
    cv::waitKey(0);

    return 0;

//    //Visualize all
//    int aw, ah, maxa;
//    aw = ah = maxa = 0;
//    const unsigned int* accu = hough.GetAccu(&aw, &ah);
//
//    for(int p=0;p<(ah*aw);p++)
//    {
//        if((int)accu[p] > maxa)
//            maxa = accu[p];
//    }
//
//    double contrast = 1.0;
//    double coef = 255.0 / (double)maxa * contrast;
//
//    cv::Mat img_accu(ah, aw, CV_8UC3);
//    for(int p=0;p<(ah*aw);p++)
//    {
//        unsigned char c = (double)accu[p] * coef < 255.0 ? (double)accu[p] * coef : 255.0;
//        img_accu.data[(p*3)+0] = 255;
//        img_accu.data[(p*3)+1] = 255-c;
//        img_accu.data[(p*3)+2] = 255-c;
//    }
//    cv::imshow(CW_ACCUMULATOR, img_accu);
}