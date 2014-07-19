#include "Monster.h"

Monster::Monster(const Point* p, unsigned int t, unsigned int e) : _position(p), _huntTime(t), _exp(e)
{
}

Monster::~Monster()
{
    delete _position;
}

const Point* Monster::GetPosition() const
{
    return _position;
}

const unsigned int Monster::GetHuntTime() const
{
    return _huntTime;
}

const unsigned int Monster::GetExp() const
{
    return _exp;
}