#ifndef POINT_H
#define POINT_H

struct Point
{
    const double x;
    const double y;

    Point( double _x, double _y ) : x(_x), y(_y) {}
};

#endif