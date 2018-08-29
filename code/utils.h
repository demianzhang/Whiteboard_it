//
// Created by zjg on 2018/7/27.
//

#ifndef HW_UTILS_H
#define HW_UTILS_H

#include <vector>
#include <cmath>
#include "config.h"
#include <algorithm>

#define DEG2RAD (M_PI/180.0f)
extern double cParameter;

// sort deg from small to big
bool cmp(std::pair<int, int> & x,std::pair<int, int> & y)
{
    return x.second==y.second?abs(x.first)<abs(y.first):x.second<y.second;
}

struct Points
{
    double x;
    double y;
};

struct Line
{
    Points p1,p2;
    double a,b,c;
};

// including 4 polar points
struct Qradrangle{
    int r1,t1;
    int r2,t2;
    int r3,t3;
    int r4,t4;
    Qradrangle(int _r1, int _t1, int _r2, int _t2, int _r3, int _t3, int _r4, int _t4):
            r1(_r1),t1(_t1),r2(_r2),t2(_t2),r3(_r3),t3(_t3),r4(_r4),t4(_t4){}
};

void GetLinePara(Line *l)
{
    l->a=l->p1.y-l->p2.y;
    l->b=l->p2.x-l->p1.x;
    l->c=l->p1.x*l->p2.y-l->p2.x*l->p1.y;
}

// calculate cross point
Points GetCrossPoint(Line *l1,Line *l2)
{
    GetLinePara(l1);
    GetLinePara(l2);
    double D = l1->a * l2->b - l2->a * l1->b;
    Points p;
    p.x = (l1->b * l2->c - l2->b * l1->c) / D;
    p.y = (l1->c * l2->a - l2->c * l1->a) / D;
    return p;
}

std::vector<Qradrangle> LinesFilter(std::vector<std::pair<int, int> > sorted_lines, int w, int h)
{
    std::vector<Qradrangle> v;
    int length=w>h?h:w;
    for(int i=0;i<=sorted_lines.size()-4;i++)
        for(int j=i+1;j<=sorted_lines.size()-3;j++)
            for(int k=j+1;k<=sorted_lines.size()-2;k++)
                for(int t=k+1;t<=sorted_lines.size()-1;t++)
                {
                    // opposite orientations and distance constraint
                    if(sorted_lines[i].second+150<=sorted_lines[k].second&&sorted_lines[i].second+210>=sorted_lines[k].second
                        && sorted_lines[j].second+150<=sorted_lines[t].second&&sorted_lines[j].second+210>=sorted_lines[t].second
                        && abs(sorted_lines[i].first+sorted_lines[k].first)>=length/2 && abs(sorted_lines[j].first+sorted_lines[t].first)>=length/2)

//                        && (sorted_lines[i].second+60<=sorted_lines[j].second&&sorted_lines[i].second+120>=sorted_lines[j].second)
//                        && (sorted_lines[j].second+60<=sorted_lines[k].second&&sorted_lines[j].second+120>=sorted_lines[k].second)
//                        && (sorted_lines[k].second+60<=sorted_lines[t].second&&sorted_lines[k].second+120>=sorted_lines[t].second)
//                        && ((sorted_lines[t].second+60<=sorted_lines[i].second&&sorted_lines[t].second+120>=sorted_lines[i].second)||(sorted_lines[t].second-60>=sorted_lines[i].second&&sorted_lines[t].second-120<=sorted_lines[i].second)))
                    {
                        v.push_back(Qradrangle(sorted_lines[i].first,sorted_lines[i].second,sorted_lines[j].first,sorted_lines[j].second,sorted_lines[k].first,sorted_lines[k].second,sorted_lines[t].first,sorted_lines[t].second));
                    }
                }
    return v;
}

// calculate perimeter
double perimeter(Points p1, Points p2, Points p3, Points p4)
{
    double sum = 0, dx, dy;
    dx = p1.x - p2.x;
    dy = p1.y - p2.y;
    sum += sqrt(dx * dx + dy * dy);
    dx = p2.x - p3.x;
    dy = p2.y - p3.y;
    sum += sqrt(dx * dx + dy * dy);
    dx = p3.x - p4.x;
    dy = p3.y - p4.y;
    sum += sqrt(dx * dx + dy * dy);
    dx = p4.x - p1.x;
    dy = p4.y - p1.y;
    sum += sqrt(dx * dx + dy * dy);
    return sum;
}

// polar point to line
Line transform(int r, int t, int w, int h)
{
    Line line;
    if((t >= -45 && t <= 45)||(t>135)||(t<-135))
    {
        //x = (r - y sin(t)) / cos(t);
        line.p1.y = 0;
        line.p1.x = ((double)(r) - ((line.p1.y - (h/2) ) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (w / 2);
        line.p2.y = h - 1;
        line.p2.x = ((double)(r) - ((line.p2.y - (h/2) ) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (w / 2);
    }
    else
    {
        //y = (r - x cos(t)) / sin(t)
        line.p1.x = 0;
        line.p1.y = (((double)(r)) - ((line.p1.x - (w/2) ) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (h / 2);
        line.p2.x = w - 1;
        line.p2.y = (((double)(r)) - ((line.p2.x - (w/2) ) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (h / 2);
    }
    return line;
}

// transform to two lines and calculate cross point
Points transformAndCross(int r1, int t1, int r2, int t2, int w,int h)
{
    Line line1 = transform(r1,t1,w,h);
    Line line2 = transform(r2,t2,w,h);
    Points p = GetCrossPoint(&line1,&line2);
    return p;
}

std::vector<Points> PerimeterFilter(std::vector<Qradrangle> qradrangle_set,int w,int h)
{
    Points p1,p2,p3,p4, _p1,_p2,_p3,_p4;
    std::vector<Points> v;
    double _min_c = 99999999.9;
    for(int i=0;i<qradrangle_set.size();i++)
    {
        p1 = transformAndCross(qradrangle_set[i].r1,qradrangle_set[i].t1,qradrangle_set[i].r2,qradrangle_set[i].t2, w, h);
        p2 = transformAndCross(qradrangle_set[i].r2,qradrangle_set[i].t2,qradrangle_set[i].r3,qradrangle_set[i].t3, w, h);
        p3 = transformAndCross(qradrangle_set[i].r3,qradrangle_set[i].t3,qradrangle_set[i].r4,qradrangle_set[i].t4, w, h);
        p4 = transformAndCross(qradrangle_set[i].r4,qradrangle_set[i].t4,qradrangle_set[i].r1,qradrangle_set[i].t1, w, h);
        double tmp_c = perimeter(p1,p2,p3,p4);
        // choose smallest perimeter with constraint
        if(_min_c > tmp_c && tmp_c >= (w+h)*cParameter)
        {
            _min_c = tmp_c;
            _p1=p1;
            _p2=p2;
            _p3=p3;
            _p4=p4;
        }
    }
    v.push_back(_p1);
    v.push_back(_p2);
    v.push_back(_p3);
    v.push_back(_p4);
    return v;
}

std::vector<Points> FindQuadrangle(std::vector< std::pair<int, int> > points,int w, int h)
{
    std::vector<Points> qradrangle4point(4);
    std::vector<Qradrangle> qradrangle_set, qradrangle_set2;
    std::vector<std::pair<int, int> > clockwiseSet, reclockwiseSet;
    std::vector< std::pair<int, int> >::iterator points_it;
    for(points_it=points.begin();points_it!=points.end();points_it++)
    {
        //std::cout<<points_it->first<<" "<<points_it->second<<std::endl;
        if(points_it->first<0)clockwiseSet.push_back(*points_it); // clockwise lines set
        else reclockwiseSet.push_back(*points_it); // counterclockwise lines set
    }
    sort(clockwiseSet.begin(),clockwiseSet.end(),cmp);
    sort(reclockwiseSet.begin(),reclockwiseSet.end(),cmp);

    qradrangle_set = LinesFilter(clockwiseSet, w,h);
    qradrangle_set2 = LinesFilter(reclockwiseSet,w,h);
    qradrangle_set.insert(qradrangle_set.end(),qradrangle_set2.begin(),qradrangle_set2.end());

    std::cout<<"Qradrangle Number Is "<<qradrangle_set.size()<<std::endl;

    //choose the best
    qradrangle4point = PerimeterFilter(qradrangle_set,w,h);

    return qradrangle4point;
}

void DoAffineTransform(std::vector<Points> p, cv::Mat image)
{
    double _min=p[0].x+p[0].y, _max=p[0].x+p[0].y;
    int leftTop=0, rightDown=0;
    for(int i=1;i<4;i++)
    {
        if(_min>p[i].x+p[i].y)
        {
            leftTop=i;
            _min=p[i].x+p[i].y;
        }
        if(_max<p[i].x+p[i].y)
        {
            rightDown=i;
            _max=p[i].x+p[i].y;
        }
    }
    int rightTop=-1,leftDown;
    for(int i=0;i<4;i++)
    {
        if(i==leftTop||i==rightDown)continue;
        if(rightTop==-1)rightTop=i;
        else leftDown=i;
    }
    if(p[rightTop].x<p[leftDown].x)std::swap(rightTop,leftDown);
    double leftTopX = p[leftTop].x;
    double leftTopY = p[leftTop].y;
    double rightTopX = p[rightTop].x;
    double rightTopY = p[rightTop].y;
    double leftDownX = p[leftDown].x;
    double leftDownY = p[leftDown].y;
    double rightDownX = p[rightDown].x;
    double rightDownY = p[rightDown].y;

    std::cout << "Four Points"<<std::endl;

    std::cout << leftTopX << " " << leftTopY << std::endl;
    std::cout << rightTopX << " " << rightTopY << std::endl;
    std::cout << leftDownX << " " << leftDownY << std::endl;
    std::cout << rightDownX << " " << rightDownY << std::endl;

    int newWidth = image.cols-1;
    int newHeight = image.rows-1;

    cv:: Mat dstImage = cv::Mat::zeros(newHeight, newWidth, image.type());
    cv::Point2f srcTriangle[4];
    cv::Point2f dstTriangle[4];
    srcTriangle[0] = cv::Point2f(leftTopX, leftTopY);
    srcTriangle[1] = cv::Point2f(rightTopX, rightTopY);
    srcTriangle[2] = cv::Point2f(leftDownX, leftDownY);
    srcTriangle[3] = cv::Point2f(rightDownX, rightDownY);

    dstTriangle[0] = cv::Point2f(0, 0);
    dstTriangle[1] = cv::Point2f(newWidth, 0);
    dstTriangle[2] = cv::Point2f(0, newHeight);
    dstTriangle[3] = cv::Point2f(newWidth, newHeight);

    cv::Mat warpMat = cv::getPerspectiveTransform(srcTriangle, dstTriangle);
    cv::warpPerspective(image, dstImage, warpMat, image.size());
    cv::namedWindow("Viewdst", CV_WINDOW_NORMAL);
    //cv::resizeWindow("Viewdst", 800, 600);
    cv::imshow("Viewdst", dstImage);
}

#endif //HW_UTILS_H
