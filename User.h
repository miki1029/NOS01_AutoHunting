#pragma once
#include "Point.h"
#include "Monster.h"
#include <list>
#include <hash_map>
using std::list;
using stdext::hash_map;
class Field;

class User
{
private:
    // 사냥 대상 몬스터의 위치(list에서의) 정보를 표현하는 구조체
    struct MonsterInfo
    {
        list<const Monster*>::iterator pos;
        unsigned int moveHuntTime;
        double weight;

        bool operator==(const struct MonsterInfo &mi) const { return weight == mi.weight; }
        bool operator!=(const struct MonsterInfo &mi) const { return weight != mi.weight; }
        bool operator>(const struct MonsterInfo &mi) const { return weight > mi.weight; }
        bool operator>=(const struct MonsterInfo &mi) const { return weight >= mi.weight; }
        bool operator<(const struct MonsterInfo &mi) const { return weight < mi.weight; }
        bool operator<=(const struct MonsterInfo &mi) const { return weight <= mi.weight; }
    };

    // 시간이 남았을 경우 추가시킬 몬스터와 그 사냥 시점 정보를 표현하는 클래스
    class AddMonsterInfo
    {
    private:
        const Monster* prev;
        const Monster* next;
        list<const Monster*>::iterator huntMonsterItr;

    public:
        AddMonsterInfo(const Monster* p, const Monster* n, list<const Monster*>::iterator h);

        const Monster* GetPrev() const;
        const Monster* GetNext() const;
        void SetPrev(const Monster* p);
        void SetNext(const Monster* n);
        const list<const Monster*>::iterator GetHuntMonsterItr() const;

        unsigned int GetAddPlayTime() const;
        unsigned int GetAddExp() const;
        double GetExpPerTime() const;
    };

    const Point* _startPosition;
    const Point* _position;
    const unsigned int _maxPlayTime;
    unsigned int _playTime;

    list<const Monster*> _huntList;
    unsigned int _exp;

    // 남은 시간동안 추가할 수 있는 몬스터들의 hash_map
    // 사냥 가능한 몬스터가 key가 되고 그 몬스터를 잡는 시점 정보가 값이 된다.
    hash_map<const Monster*, AddMonsterInfo*> _addMonsterMap;

public:
    User(Point* p, unsigned int t);
    ~User();

    const Point* GetPosition() const;
    const unsigned int GetMaxPlayTime() const;
    const unsigned int GetPlayTime() const;
    const list<const Monster*>& GetHuntList() const;
    const unsigned int GetExp() const;

    list<const Monster*>::iterator FindNextMonster(Field* field, bool extendSearch = false);
    void HuntMonster(const Monster* m);

    // _addMonsterMap 초기화
    bool FindRemainMonster(Field* field);
    // _addMonsterMap에서 시간당 경험치가 가장 높은 것을 사냥
    list<const Monster*>::iterator HuntRemainMonster();
    bool IsPossibleHunt() const;

    bool HuntExtMonster(const Monster* m);
};

