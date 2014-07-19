#include <iostream>
#include <fstream>
#include <list>
#include "Field.h"
using std::ifstream;
using std::ofstream;
using std::list;
using std::cout;
using std::endl;

Field::Field(unsigned int x, unsigned int y, unsigned int m) : _maxX(x), _maxY(y), _maxMonster(m)
{
}

Field::~Field()
{
    delete _user;
    list<const Monster*>::iterator delItr;
    for (delItr = _monsterList.begin(); delItr != _monsterList.end(); delItr++)
    {
        delete *delItr;
    }
}

list<const Monster*>& Field::GetMonsterList()
{
    return _monsterList;
}

void Field::LoadFromFile(const char* fileName)
{
    ifstream inStream;
    inStream.open(fileName);

    // _user 초기화
    unsigned int userX, userY, userPlayTime;
    inStream >> userX >> userY >> userPlayTime;
    Point* userPosition = new Point(userX, userY);
    _user = new User(userPosition, userPlayTime);

    // _monsterList 초기화
    unsigned int numOfMonster;
    inStream >> numOfMonster;
    _monsterList.clear();
    for (unsigned int i = 0; i < numOfMonster; i++)
    {
        unsigned int monX, monY, monHuntTime, monExp;
        inStream >> monX >> monY >> monHuntTime >> monExp;
        Point* monPosition = new Point(monX, monY);
        Monster* monster = new Monster(monPosition, monHuntTime, monExp);
        _monsterList.push_back(monster);
    }

    inStream.close();
}

void Field::AutoHunt()
{
    // 먼저 시간당 최대 경험치를 구함
    while (true)
    {
        list<const Monster*>::iterator huntMonsterItr = _user->FindNextMonster(this);
        if (huntMonsterItr == _monsterList.end()) break;
        _user->HuntMonster(*huntMonsterItr);
        _monsterList.erase(huntMonsterItr);
    }
    /*
    while (true)
    {
        unsigned int prevPlayTime = _user->GetPlayTime();
        unsigned int prevExp = _user->GetExp();*/
    // 마지막 사냥한 몬스터 이후에 경험치 효율이 더 좋은 것이 있는지 확인
    // 기존의 몬스터를 제외하고 이후의 몬스터를 추가하는 것이 더 효율이 좋다면 대체함
    while (true)
    {
        list<const Monster*>::iterator extHuntMonsterItr = _user->FindNextMonster(this, true);
        if (extHuntMonsterItr == _monsterList.end()) break;
        if (!_user->HuntExtMonster(*extHuntMonsterItr)) break;
        _monsterList.erase(extHuntMonsterItr);
    }
        
    // 시간이 남았으면 중간중간 추가할 수 있는 몬스터가 있었는지 확인
    if (_user->GetPlayTime() < _user->GetMaxPlayTime() && _user->FindRemainMonster(this))
    {
        while (_user->IsPossibleHunt())
        {
            list<const Monster*>::iterator huntMonsterItr = _user->HuntRemainMonster();
            _monsterList.erase(huntMonsterItr);
        }
    }
        /*
        unsigned int nextPlayTime = _user->GetPlayTime();
        unsigned int nextExp = _user->GetExp();
        
        if (prevPlayTime == nextPlayTime && prevExp == nextExp) break;
    }*/
}

void Field::SaveToFile(const char* fileName)
{
    ofstream outStream;
    outStream.open(fileName);

    list<const Monster*> monsterList = _user->GetHuntList();

    outStream << _user->GetPlayTime() << endl
        << _user->GetExp() << endl
        << monsterList.size();
    list<const Monster*>::iterator monItr;
    for (monItr = monsterList.begin(); monItr != monsterList.end(); monItr++)
    {
        outStream << endl
            << (*monItr)->GetPosition()->GetX() << " "
            << (*monItr)->GetPosition()->GetY();
    }

    outStream.close();
}