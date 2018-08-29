#include "hough.h"

#define DEG2RAD (M_PI/180.0f)
#define RAD2DEG (180.0f/M_PI)
extern double linesParameter;

namespace zjg {

    Hough::Hough():_accu(0), _accu_w(0), _accu_h(0), _img_w(0), _img_h(0), _thresh(0) {}

    Hough::~Hough() { if(_accu)free(_accu); }

    int Hough::Transform(unsigned char* img_data, cv::Mat & grad_x, cv::Mat & grad_y, int w, int h)
    {
        _img_w = w;
        _img_h = h;

        // build oriented accu
        double hough_h = ((sqrt(2.0) * (double)(h>w?h:w)) / 2.0);
        _accu_h = hough_h * 2.0; // -r -> +r
        _accu_w = 180*2.0; // -180 -> +180

        _accu = (unsigned int*)calloc(_accu_h * _accu_w, sizeof(unsigned int));

        const int dxstep0 = (int)( grad_x.step[0] );
        uchar * dxdata = grad_x.data;

        const int dystep0 = (int)( grad_y.step[0] );
        uchar * dydata = grad_y.data;

        double center_x = w/2;
        double center_y = h/2;

        for(int y=0;y<h;y++)
        {
            const short * const dx_row = (short *)(dxdata + y * dxstep0);
            const short * const dy_row = (short *)(dydata + y * dystep0);
            for(int x=0;x<w;x++)
            {
                if( img_data[ (y*w) + x] > 100 )
                {
                    const float  vx = (float)( dx_row[x] );
                    const float vy = (float)( dy_row[x] );
                    double t = atan2(vy,vx);
                    // center is the (0,0)
                    double r = (((double)x - center_x) * cos(t)) + (((double)y - center_y) * sin(t));
                    _accu[ (int)((round(r + hough_h) * 360)) + int(round(t*RAD2DEG + 180))]++;
                }
            }
        }

        int _max=0;

        for(int r=0;r<_accu_h;r++)
        {
            for (int t = 0; t < _accu_w; t++)
            {
                _max=fmax(_max, int(_accu[(r*_accu_w)+t]));
            }
        }

        _thresh=_max*linesParameter; // lines thresh

        return 0;
    }

    std::vector< std::pair<int, int> > Hough::GetPoints()
    {
        std::vector< std::pair<int, int> > points;
        if (_accu == 0)
            return points;

        for (int r = 0; r < _accu_h; r++) {
            for (int t = 0; t < _accu_w; t++) {
                if ((int) _accu[(r * _accu_w) + t] >= _thresh) {
                    // (r,t) is local maxima (9x9)
                    int maxLocal = _accu[(r * _accu_w) + t];
                    for (int ly = -4; ly <= 4; ly++) {
                        for (int lx = -4; lx <= 4; lx++) {
                            if ((ly + r >= 0 && ly + r < _accu_h) && (lx + t >= 0 && lx + t < _accu_w)) {
                                if ((int) _accu[((r + ly) * _accu_w) + (t + lx)] > maxLocal) {
                                    maxLocal = _accu[((r + ly) * _accu_w) + (t + lx)];
                                    ly = lx = 5;
                                }
                            }
                        }
                    }
                    if (maxLocal > (int) _accu[(r * _accu_w) + t])
                        continue;
                    points.push_back(std::make_pair(r-(_accu_h/2),t-180));

                }
            }
        }
        std::cout << "points: " << points.size() << " " << _thresh << std::endl;
        return points;
    }

    std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > > Hough::GetLines()
    {
        std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > > lines;

        if(_accu == 0)
            return lines;

        for(int r=0;r<_accu_h;r++)
        {
            for(int t=0;t<_accu_w;t++)
            {
                if((int)_accu[(r*_accu_w) + t] >= _thresh)
                {
                    // (r,t) is local maxima (9x9)
                    int maxLocal = _accu[(r*_accu_w) + t];
                    for(int ly=-4;ly<=4;ly++)
                    {
                        for(int lx=-4;lx<=4;lx++)
                        {
                            if( (ly+r>=0 && ly+r<_accu_h) && (lx+t>=0 && lx+t<_accu_w)  )
                            {
                                if( (int)_accu[( (r+ly)*_accu_w) + (t+lx)] > maxLocal )
                                {
                                    maxLocal = _accu[( (r+ly)*_accu_w) + (t+lx)];
                                    ly = lx = 5;
                                }
                            }
                        }
                    }
                    if(maxLocal > (int)_accu[(r*_accu_w) + t])
                        continue;

                    int x1, y1, x2, y2;
                    x1 = y1 = x2 = y2 = 0;

                    // Avoid denominator to be 0
                    if((t-180 >= -45 && t-180 <= 45)||(t-180>135)||(t-180<-135))
                    {
                        //x = (r - y sin(t)) / cos(t);
                        y1 = 0;
                        x1 = ((double)(r-(_accu_h/2)) - ((y1 - (_img_h/2) ) * sin((t-180) * DEG2RAD))) / cos((t-180) * DEG2RAD) + (_img_w / 2);
                        y2 = _img_h - 1;
                        x2 = ((double)(r-(_accu_h/2)) - ((y2 - (_img_h/2) ) * sin((t-180) * DEG2RAD))) / cos((t-180) * DEG2RAD) + (_img_w / 2);
                    }
                    else
                    {
                        //y = (r - x cos(t)) / sin(t)
                        x1 = 0;
                        y1 = (((double)(r-(_accu_h/2))) - ((x1 - (_img_w/2) ) * cos((t-180) * DEG2RAD))) / sin((t-180) * DEG2RAD) + (_img_h / 2);
                        x2 = _img_w - 1;
                        y2 = (((double)(r-(_accu_h/2))) - ((x2 - (_img_w/2) ) * cos((t-180) * DEG2RAD))) / sin((t-180) * DEG2RAD) + (_img_h / 2);
                    }

                    lines.push_back(std::pair< std::pair<int, int>, std::pair<int, int> >(std::pair<int, int>(x1,y1), std::pair<int, int>(x2,y2)));

                }
            }
        }

        std::cout << "lines: " << lines.size() << " " << _thresh << std::endl;
        return lines;
    }

    const unsigned int* Hough::GetAccu(int *w, int *h)
    {
        *w = _accu_w;
        *h = _accu_h;

        return _accu;
    }
}
