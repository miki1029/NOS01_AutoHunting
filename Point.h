#pragma once
class Point
{
private:
    const int _x;
    const int _y;
public:
    Point(int x, int y);
    ~Point();
    const int GetX() const;
    const int GetY() const;
    const unsigned int CalculateDistance(const Point& p) const;
};

