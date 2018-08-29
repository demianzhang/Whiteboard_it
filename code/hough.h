//
// Created by zjg on 2018/7/24.
//

#ifndef HW_HOUGH_H
#define HW_HOUGH_H

#include <cmath>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#include "config.h"

namespace zjg {
    class Hough {
    public:
        Hough();
        virtual ~Hough();
    public:

        int Transform(unsigned char* img_data, cv::Mat & grad_x, cv::Mat & grad_y, int w, int h);
        std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > > GetLines();
        std::vector< std::pair<int, int> > GetPoints();
        const unsigned int* GetAccu(int *w, int *h);
    private:
        unsigned int* _accu;
        int _accu_w; // t
        int _accu_h; // rho: distance to the (0,0)
        int _img_w;
        int _img_h;
        int _thresh;
    };
}

#endif //HW_HOUGH_H
