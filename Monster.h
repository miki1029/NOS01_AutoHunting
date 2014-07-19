#pragma once
#include "Point.h"

class Monster
{
private:
    const Point* _position;
    const unsigned int _huntTime;
    const unsigned int _exp;
public:
    Monster(const Point* p, unsigned int t, unsigned int e);
    ~Monster();
    const Point* GetPosition() const;
    const unsigned int GetHuntTime() const;
    const unsigned int GetExp() const;
};

