#pragma once
#include "Monster.h"
#include "User.h"
#include <list>
using std::list;

class Field
{
private:
    const unsigned int _maxX;
    const unsigned int _maxY;
    const unsigned int _maxMonster;

    User* _user;
    list<const Monster*> _monsterList;

public:
    Field(unsigned int x, unsigned int y, unsigned int m);
    ~Field();

    list<const Monster*>& GetMonsterList();
    void LoadFromFile(const char* fileName = "Input.txt");
    void AutoHunt();
    void SaveToFile(const char* fileName = "Output.txt");
};

