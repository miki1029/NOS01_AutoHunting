#include <iostream>
#include <set>
#include "User.h"
#include "Field.h"
using std::set;

User::AddMonsterInfo::AddMonsterInfo(const Monster* p, const Monster* n, list<const Monster*>::iterator h) : prev(p), next(n), huntMonsterItr(h)
{
}

const Monster* User::AddMonsterInfo::GetPrev() const
{
    return prev;
}
const Monster* User::AddMonsterInfo::GetNext() const
{
    return next;
}

void User::AddMonsterInfo::SetPrev(const Monster* p)
{
    prev = p;
}
void User::AddMonsterInfo::SetNext(const Monster* n)
{
    next = n;
}

const list<const Monster*>::iterator User::AddMonsterInfo::GetHuntMonsterItr() const
{
    return huntMonsterItr;
}

unsigned int User::AddMonsterInfo::GetAddPlayTime() const
{
    unsigned int prevToNextMT = prev->GetPosition()->CalculateDistance(*next->GetPosition());
    unsigned int prevToHuntMT = prev->GetPosition()->CalculateDistance(*(*huntMonsterItr)->GetPosition());
    unsigned int huntToNextMT = (*huntMonsterItr)->GetPosition()->CalculateDistance(*next->GetPosition());
    unsigned int addHuntTime = (*huntMonsterItr)->GetHuntTime();
    return addHuntTime + prevToHuntMT + huntToNextMT - prevToNextMT;
}

unsigned int User::AddMonsterInfo::GetAddExp() const
{
    return (*huntMonsterItr)->GetExp();
}

double User::AddMonsterInfo::GetExpPerTime() const
{
    if (GetAddPlayTime() <= 0) return UINT_MAX;
    return (double)GetAddExp() / GetAddPlayTime();
}

User::User(Point* p, unsigned int t) : _startPosition(p), _position(p), _maxPlayTime(t), _playTime(0), _exp(0)
{
}

User::~User()
{
    // ������ ����
    delete _startPosition;

    // _huntList�� ���� ����
    list<const Monster*>::iterator delItr;
    for (delItr = _huntList.begin(); delItr != _huntList.end(); delItr++)
    {
        delete *delItr;
    }

    // _addMonsterMap�� AddMonsterInfo ����
    hash_map<const Monster*, AddMonsterInfo*>::iterator monMapItr;
    for (monMapItr = _addMonsterMap.begin(); monMapItr != _addMonsterMap.end(); monMapItr++)
    {
        delete (*monMapItr).second;
    }
}

const Point* User::GetPosition() const
{
    return _position;
}
const unsigned int User::GetMaxPlayTime() const
{
    return _maxPlayTime;
}
const unsigned int User::GetPlayTime() const
{
    return _playTime;
}
const list<const Monster*>& User::GetHuntList() const
{
    return _huntList;
}
const unsigned int User::GetExp() const
{
    return _exp;
}

list<const Monster*>::iterator User::FindNextMonster(Field* field, bool extendSearch)
{
    // �ʵ忡�� ������ list�� ������
    list<const Monster*>& monsterList = field->GetMonsterList();
    list<const Monster*>::iterator monsterItr;

    // ��� ��� ���� �ʱ�ȭ(end�� �ʱ�ȭ ��Ű�� �� ���� ������ ������ ����� ���Ͱ� ���� ����)
    MonsterInfo huntMonster;
    huntMonster.pos = monsterList.end();
    huntMonster.moveHuntTime = 0;
    huntMonster.weight = 0;

    // ���� ����Ʈ�� ��ȸ�ϸ� ��� ��� ���� ����
    for (monsterItr = monsterList.begin(); monsterItr != monsterList.end(); monsterItr++)
    {
        // ���� ������ �̵��ð�, ��ɽð�, ����ġ
        unsigned int moveTime = (*monsterItr)->GetPosition()->CalculateDistance(*_position);
        unsigned int huntTime = (*monsterItr)->GetHuntTime();
        unsigned int exp = (*monsterItr)->GetExp();

        // ���� ���� ����ü �ʱ�ȭ
        MonsterInfo curMonster;
        curMonster.pos = monsterItr;
        curMonster.moveHuntTime = moveTime + huntTime;
        curMonster.weight = static_cast<double>(exp) / (moveTime + huntTime); // ����ġ: �ð��� ����ġ

        // ���� �ð� ���� ��� �������� Ȯ��
        if (!extendSearch)
        {
            unsigned int remainTime = _maxPlayTime - _playTime;
            if (curMonster.moveHuntTime > remainTime) continue;
        }

        // ����ġ�� ���Ͽ� ����� ���� ����
        if (curMonster.weight > huntMonster.weight)
        {
            huntMonster = curMonster;
        }
        else if (curMonster.weight == huntMonster.weight)
        {
            // ����ġ�� ������ ��� �ð��� ���� ������
            if (curMonster.moveHuntTime < huntMonster.moveHuntTime)
            {
                huntMonster = curMonster;
            }
        }
    }

    return huntMonster.pos;
}

void User::HuntMonster(const Monster* m)
{
    _playTime += (m->GetHuntTime() + m->GetPosition()->CalculateDistance(*_position));
    _exp += m->GetExp();
    _huntList.push_back(m);
    _position = m->GetPosition();
}

bool User::FindRemainMonster(Field* field)
{
    // �ʱ�ȭ

    // �ؽø� �ʱ�ȭ
    _addMonsterMap.clear();
    // ���� �÷��� �ð�
    unsigned int remainTime = _maxPlayTime - _playTime;
    // �ʵ忡�� ������ list�� ������
    list<const Monster*>& monsterList = field->GetMonsterList();
    // ������ ���� ��ġ�� ���� ���� ����(prev�� �ʱⰪ)
    const Monster* prev = new Monster(_startPosition, 0, 0);

    // ����� ���͵��� ��ȸ(������ �̵�)
    list<const Monster*>::iterator huntMonsterItr;
    for (huntMonsterItr = _huntList.begin(); huntMonsterItr != _huntList.end(); huntMonsterItr++)
    {
        const Monster* next = *huntMonsterItr;
        // �� ����� ��ġ���� ���� ����Ʈ�� ��ȸ�ϸ� �� �߰��� ����� ���Ͱ� �ִ��� Ȯ��
        list<const Monster*>::iterator monsterItr;
        for (monsterItr = monsterList.begin(); monsterItr != monsterList.end(); monsterItr++)
        {
            // ���ο� ���� ����
            AddMonsterInfo* newTarget = new AddMonsterInfo(prev, next, monsterItr);

            // ���� �ð� �� ���� �� ������
            if (remainTime >= newTarget->GetAddPlayTime())
            {
                // �̹� �ؽøʿ� �����ϸ�
                hash_map<const Monster*, AddMonsterInfo*>::iterator findItr = _addMonsterMap.find(*monsterItr);
                if (findItr != _addMonsterMap.end())
                {
                    unsigned int existPlayTime = findItr->second->GetAddPlayTime();
                    unsigned int newPlayTime = newTarget->GetAddPlayTime();
                    // ���簡 �� ª�� �ð��� �ҿ�Ǹ� ���� �ٲ۴�.
                    if (newPlayTime < existPlayTime)
                    {
                        // �޸� ���� �� ��� ����
                        const AddMonsterInfo* delTarget = (*_addMonsterMap.find(*monsterItr)).second;
                        _addMonsterMap.erase(*monsterItr);
                        delete delTarget;
                        // ���ο� ��� ����
                        hash_map<const Monster*, AddMonsterInfo*>::value_type val(*monsterItr, newTarget);
                        _addMonsterMap.insert(val);
                    }
                    // ���� ���ϴ� ���ͷ� �Ǵ�
                    else
                    {
                        delete newTarget;
                    }
                }
                // �ؽøʿ� �������� ������ �� �߰�
                else
                {
                    hash_map<const Monster*, AddMonsterInfo*>::value_type val(*monsterItr, newTarget);
                    _addMonsterMap.insert(val);
                }
            }
            // ���� ���ϴ� ���ͷ� �Ǵ�
            else
            {
                delete newTarget;
            }
        }
        prev = next;
    }

    if (_addMonsterMap.size() == 0) return false; // �� �̻� ��� �Ұ�
    else return true;
}

list<const Monster*>::iterator User::HuntRemainMonster()
{
    // �ؽø��� ��ȸ�ϸ� �ð��� �ִ� ����ġ�� ���� ���� ����
    hash_map<const Monster*, AddMonsterInfo*>::iterator monMapItr = _addMonsterMap.begin();
    const AddMonsterInfo* maxTarget = (*(monMapItr++)).second; // ù��° ��ҷ� �ʱ�ȭ
    for (; monMapItr != _addMonsterMap.end(); monMapItr++)
    {
        const AddMonsterInfo* newTarget = (*monMapItr).second;
        // �ð��� ����ġ�� �� ������
        if (newTarget->GetExpPerTime() > maxTarget->GetExpPerTime())
        {
            maxTarget = newTarget;
        }
        // ���� ������ �ð��� ���� �ɸ��� ������
        else if (newTarget->GetExpPerTime() == maxTarget->GetExpPerTime())
        {
            if (newTarget->GetAddPlayTime() < maxTarget->GetAddPlayTime())
            {
                maxTarget = newTarget;
            }
        }
    }

    // �� ����(maxTarget)�� ���
    // _huntList���� maxTarget�� ��ġ�� ã��
    list<const Monster*>::iterator insertPosItr;
    for (insertPosItr = _huntList.begin(); insertPosItr != _huntList.end(); insertPosItr++)
    {
        if (*insertPosItr == maxTarget->GetPrev())
        {
            insertPosItr++;
            // prev�� next�� ��� ��ġ�Ͽ� �� ��ġ�� ã�� ���
            if (*insertPosItr == maxTarget->GetNext()) break;
            else
            {
                std::cout << "Error!" << std::endl;
            }
        }
    }

    // maxTarget�� ���� ����
    _huntList.insert(insertPosItr, *maxTarget->GetHuntMonsterItr());
    _playTime += maxTarget->GetAddPlayTime();
    _exp += maxTarget->GetAddExp();

    // maxTarget ���Կ� ���� prev�� next�� ���Ҵ� ��� ����
    hash_map<const Monster*, AddMonsterInfo*>::iterator samePrevElemItr;
    for (samePrevElemItr = _addMonsterMap.begin(); samePrevElemItr != _addMonsterMap.end(); samePrevElemItr++)
    {
        AddMonsterInfo* curTarget = (*samePrevElemItr).second;
        if (curTarget == maxTarget) continue;

        const Monster* curPrev = curTarget->GetPrev();
        const Monster* curMon = *curTarget->GetHuntMonsterItr();
        const Monster* curNext = curTarget->GetNext();

        const Monster* maxPrev = maxTarget->GetPrev();
        const Monster* maxMon = *maxTarget->GetHuntMonsterItr();
        const Monster* maxNext = maxTarget->GetNext();

        if (curPrev == maxPrev && curNext == maxNext)
        {
            // prev->cur->max->next �� ���
            AddMonsterInfo* prevCurMaxNextCur = new AddMonsterInfo(curPrev, maxMon, curTarget->GetHuntMonsterItr());
            AddMonsterInfo* prevCurMaxNextMax = new AddMonsterInfo(curMon, curNext, maxTarget->GetHuntMonsterItr());
            // ����� �ҿ� �ð�
            int prevCurMaxNextTime = curPrev->GetPosition()->CalculateDistance(*curMon->GetPosition()) + 
                maxMon->GetPosition()->CalculateDistance(*curNext->GetPosition());

            // prev->max->cur->next �� ���
            AddMonsterInfo* prevMaxCurNextCur = new AddMonsterInfo(maxMon, curNext, curTarget->GetHuntMonsterItr());
            AddMonsterInfo* prevMaxCurNextMax = new AddMonsterInfo(curPrev, curMon, maxTarget->GetHuntMonsterItr());
            // ����� �ҿ� �ð�
            int prevMaxCurNextTime = curPrev->GetPosition()->CalculateDistance(*maxMon->GetPosition()) +
                curMon->GetPosition()->CalculateDistance(*curNext->GetPosition());

            // prev->cur->max->next �� �ð��� �� ª�� ���
            if (prevCurMaxNextTime < prevMaxCurNextTime)
            {
                curTarget->SetNext(maxMon);
            }
            // prev->max->cur->next �� �ð��� �� ª�� ��� Ȥ�� ���� ���
            else
            {
                curTarget->SetPrev(maxMon);
            }
        }
    }

    // _addMonsterMap���� maxTarget�� ���� ����
    _addMonsterMap.erase(*maxTarget->GetHuntMonsterItr());

    // �÷���Ÿ�� ���Ͽ� ����� ���� ����
    unsigned int remainTime = _maxPlayTime - _playTime;
    list<hash_map<const Monster*, AddMonsterInfo*>::iterator> delItrList;
    for (monMapItr = _addMonsterMap.begin(); monMapItr != _addMonsterMap.end(); monMapItr++)
    {
        if ((*monMapItr).second->GetAddPlayTime() > remainTime)
        {
            delItrList.push_back(monMapItr);
        }
    }
    list<hash_map<const Monster*, AddMonsterInfo*>::iterator>::iterator delItr;
    for (delItr = delItrList.begin(); delItr != delItrList.end(); delItr++)
    {
        const AddMonsterInfo* delTarget = (**delItr).second;
        _addMonsterMap.erase(*delItr);
        delete delTarget;
    }

    list<const Monster*>::iterator huntMonsterItr = maxTarget->GetHuntMonsterItr();
    delete maxTarget;
    return huntMonsterItr;
}

bool User::IsPossibleHunt() const
{
    return !_addMonsterMap.empty();
}

bool User::HuntExtMonster(const Monster* extMonster)
{
    unsigned int remainTime = _maxPlayTime - _playTime;
    // ���� ����� ���Ϳ� ���� ������ �ð��� ����ġ
    unsigned int extPlayTime = (extMonster->GetHuntTime() + extMonster->GetPosition()->CalculateDistance(*_position));
    unsigned int extExp = extMonster->GetExp();

    // ������� ����� ���͵��� ����ġ�� �����Ͽ� huntSet�� ����
    set<MonsterInfo> huntSet;
    
    // huntSet�� ù ���
    list<const Monster*>::iterator huntItr = _huntList.begin(), prevHuntItr = huntItr;
    unsigned int moveTime = (*huntItr)->GetPosition()->CalculateDistance(*_startPosition);
    unsigned int huntTime = (*huntItr)->GetHuntTime();
    unsigned int exp = (*huntItr)->GetExp();
    MonsterInfo firstMonster;
    firstMonster.pos = huntItr;
    firstMonster.moveHuntTime = moveTime + huntTime;
    firstMonster.weight = static_cast<double>(exp) / (moveTime + huntTime); // ����ġ: �ð��� ����ġ
    huntSet.insert(firstMonster);
    huntItr++;

    // huntSet ��� �߰�
    for (; huntItr != _huntList.end(); huntItr++, prevHuntItr++)
    {
        unsigned int moveTime = (*huntItr)->GetPosition()->CalculateDistance(*(*prevHuntItr)->GetPosition());
        unsigned int huntTime = (*huntItr)->GetHuntTime();
        unsigned int exp = (*huntItr)->GetExp();

        MonsterInfo curMonster;
        curMonster.pos = huntItr;
        curMonster.moveHuntTime = moveTime + huntTime;
        curMonster.weight = static_cast<double>(exp) / (moveTime + huntTime);
          
        huntSet.insert(curMonster);
    }

    // set�� ����ġ�� ���ĵǾ� ����. ����ġ�� ���� �ͺ��� ���鼭 �ð��� Ȯ���ϰ�
    // ����� �ð��� �Ǹ� extMonster�� ���Ͽ� extMonster�� ȿ���� ������ ��ü�Ѵ�.
    unsigned int removePlayTime = 0;
    unsigned int addPlayTime = 0;
    unsigned int removeExp = 0;
    bool huntExt = false;

    // huntSet���� ���ܽ�ų ����Ʈ
    list<MonsterInfo> rmHuntList;

    while (true)
    {
        /*
        // ���� ���� �ð� �� ��� �����ϸ� �����
        if (remainTime > extPlayTime) {
            huntExt = true;
            break;
        }*/

        // ���� ȿ���� ������ �ݺ��ڸ� ã��
        set<MonsterInfo>::iterator huntSetItr = huntSet.begin();
        
        // �� �ݺ����� ���� �ݺ��ڿ� �� �ڸ� ã��
        list<const Monster*>::iterator curMonItr = (*huntSetItr).pos;
        list<const Monster*>::iterator prevMonItr = --curMonItr;
        curMonItr = (*huntSetItr).pos;
        list<const Monster*>::iterator nextMonItr = ++curMonItr;
        curMonItr = (*huntSetItr).pos;
        if (nextMonItr == _huntList.end()) return false;

        // �� ��ġ�� ���͸� ã��
        const Monster* curMon = *curMonItr;
        const Monster* prevMon = *prevMonItr;
        const Monster* nextMon = *nextMonItr;

        // ����� �Ÿ��� ���� ���
        unsigned int prevToNext = prevMon->GetPosition()->CalculateDistance(*nextMon->GetPosition());
        unsigned int curToNext = curMon->GetPosition()->CalculateDistance(*nextMon->GetPosition());

        // ����� �ð� �� ����ġ ���
        removePlayTime += (*huntSetItr).moveHuntTime + curToNext;
        addPlayTime += prevToNext;
        removeExp += curMon->GetExp();

        // ���� ������ ���� �� ���ܸ�� ����Ʈ ����
        rmHuntList.push_back(*huntSetItr);
        huntSet.erase(huntSetItr);

        // ��� ���ܿ� ���� nextMon�� �̵��Ÿ� �� ����ġ ����
        list<const Monster*>::iterator findPos;
        // ���� nextMon�� huntSet���� ã�� ���ܽ�Ŵ
        set<MonsterInfo>::iterator nextFindItr;
        for (nextFindItr = huntSet.begin(); nextFindItr != huntSet.end(); nextFindItr++)
        {
            const Monster* findMon = *(*nextFindItr).pos;
            if (findMon == nextMon)
            {
                findPos = (*nextFindItr).pos;
                huntSet.erase(nextFindItr);
                break;
            }
        }
        // nextMon���� �ٽ� MonsterInfo�� �����Ͽ� �߰�
        MonsterInfo nextMonInfo;
        nextMonInfo.pos = findPos;
        nextMonInfo.moveHuntTime = prevToNext + (*nextMonInfo.pos)->GetHuntTime();
        nextMonInfo.weight = static_cast<double>((*nextMonInfo.pos)->GetExp()) / nextMonInfo.moveHuntTime;
        huntSet.insert(nextMonInfo);

        // ����� �ð� Ȯ�������� ��������
        if (removePlayTime + remainTime - addPlayTime >= extPlayTime)
        {
            // ���� ���� ������ ȿ���� �� ������ �����
            if ((double)extExp/extPlayTime > (double)removeExp/(removePlayTime-addPlayTime))
            {
                huntExt = true;
            }
            break;
        }
    }
    
    // ���� ȿ�� ���� ���͸� �����ϰ� �� ���͸� ����
    if (huntExt)
    {
        list<MonsterInfo>::iterator delItr;
        for (delItr = rmHuntList.begin(); delItr != rmHuntList.end(); delItr++)
        {
            // ���� ���� �޸� ���� �� ��� ����Ʈ���� ����
            const Monster* delMonster = *(*delItr).pos;
            _huntList.erase((*delItr).pos);
            delete delMonster;
        }
        // ����Ʈ ���ܿ� ���� �÷��̽ð�, ����ġ ����
        _playTime -= removePlayTime;
        _playTime += addPlayTime;
        _exp -= removeExp;

        // extMonster ���
        HuntMonster(extMonster);
        return true;
    }
    else return false;
}