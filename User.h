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
    // ��� ��� ������ ��ġ(list������) ������ ǥ���ϴ� ����ü
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

    // �ð��� ������ ��� �߰���ų ���Ϳ� �� ��� ���� ������ ǥ���ϴ� Ŭ����
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

    // ���� �ð����� �߰��� �� �ִ� ���͵��� hash_map
    // ��� ������ ���Ͱ� key�� �ǰ� �� ���͸� ��� ���� ������ ���� �ȴ�.
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

    // _addMonsterMap �ʱ�ȭ
    bool FindRemainMonster(Field* field);
    // _addMonsterMap���� �ð��� ����ġ�� ���� ���� ���� ���
    list<const Monster*>::iterator HuntRemainMonster();
    bool IsPossibleHunt() const;

    bool HuntExtMonster(const Monster* m);
};

