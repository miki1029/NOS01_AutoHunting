#include "Point.h"
#include <cmath>

Point::Point(int x, int y) : _x(x), _y(y)
{
}

Point::~Point()
{
}

const int Point::GetX() const
{
    return _x;
}

const int Point::GetY() const
{
    return _y;
}

const unsigned int Point::CalculateDistance(const Point& p) const
{
    return static_cast<unsigned int>(sqrt(pow(p.GetX() - _x, 2) + pow(p.GetY() - _y, 2)));
}