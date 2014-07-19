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
    // 시작점 해제
    delete _startPosition;

    // _huntList의 몬스터 해제
    list<const Monster*>::iterator delItr;
    for (delItr = _huntList.begin(); delItr != _huntList.end(); delItr++)
    {
        delete *delItr;
    }

    // _addMonsterMap의 AddMonsterInfo 해제
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
    // 필드에서 몬스터의 list를 가져옴
    list<const Monster*>& monsterList = field->GetMonsterList();
    list<const Monster*>::iterator monsterItr;

    // 사냥 대상 몬스터 초기화(end로 초기화 시키고 이 값이 변하지 않으면 사냥할 몬스터가 없는 것임)
    MonsterInfo huntMonster;
    huntMonster.pos = monsterList.end();
    huntMonster.moveHuntTime = 0;
    huntMonster.weight = 0;

    // 몬스터 리스트를 순회하며 사냥 대상 몬스터 선정
    for (monsterItr = monsterList.begin(); monsterItr != monsterList.end(); monsterItr++)
    {
        // 현재 몬스터의 이동시간, 사냥시간, 경험치
        unsigned int moveTime = (*monsterItr)->GetPosition()->CalculateDistance(*_position);
        unsigned int huntTime = (*monsterItr)->GetHuntTime();
        unsigned int exp = (*monsterItr)->GetExp();

        // 현재 몬스터 구조체 초기화
        MonsterInfo curMonster;
        curMonster.pos = monsterItr;
        curMonster.moveHuntTime = moveTime + huntTime;
        curMonster.weight = static_cast<double>(exp) / (moveTime + huntTime); // 가중치: 시간당 경험치

        // 남은 시간 내에 사냥 가능한지 확인
        if (!extendSearch)
        {
            unsigned int remainTime = _maxPlayTime - _playTime;
            if (curMonster.moveHuntTime > remainTime) continue;
        }

        // 가중치를 비교하여 사냥할 몬스터 선정
        if (curMonster.weight > huntMonster.weight)
        {
            huntMonster = curMonster;
        }
        else if (curMonster.weight == huntMonster.weight)
        {
            // 가중치가 같으면 사냥 시간이 적은 것으로
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
    // 초기화

    // 해시맵 초기화
    _addMonsterMap.clear();
    // 남은 플레이 시간
    unsigned int remainTime = _maxPlayTime - _playTime;
    // 필드에서 몬스터의 list를 가져옴
    list<const Monster*>& monsterList = field->GetMonsterList();
    // 유저의 최초 위치로 가상 몬스터 설정(prev의 초기값)
    const Monster* prev = new Monster(_startPosition, 0, 0);

    // 사냥한 몬스터들을 순회(유저의 이동)
    list<const Monster*>::iterator huntMonsterItr;
    for (huntMonsterItr = _huntList.begin(); huntMonsterItr != _huntList.end(); huntMonsterItr++)
    {
        const Monster* next = *huntMonsterItr;
        // 각 사냥한 위치에서 몬스터 리스트를 순회하며 더 추가로 사냥할 몬스터가 있는지 확인
        list<const Monster*>::iterator monsterItr;
        for (monsterItr = monsterList.begin(); monsterItr != monsterList.end(); monsterItr++)
        {
            // 새로운 몬스터 정보
            AddMonsterInfo* newTarget = new AddMonsterInfo(prev, next, monsterItr);

            // 남은 시간 내 잡을 수 있으면
            if (remainTime >= newTarget->GetAddPlayTime())
            {
                // 이미 해시맵에 존재하면
                hash_map<const Monster*, AddMonsterInfo*>::iterator findItr = _addMonsterMap.find(*monsterItr);
                if (findItr != _addMonsterMap.end())
                {
                    unsigned int existPlayTime = findItr->second->GetAddPlayTime();
                    unsigned int newPlayTime = newTarget->GetAddPlayTime();
                    // 현재가 더 짧은 시간이 소요되면 값을 바꾼다.
                    if (newPlayTime < existPlayTime)
                    {
                        // 메모리 해제 및 요소 삭제
                        const AddMonsterInfo* delTarget = (*_addMonsterMap.find(*monsterItr)).second;
                        _addMonsterMap.erase(*monsterItr);
                        delete delTarget;
                        // 새로운 요소 삽입
                        hash_map<const Monster*, AddMonsterInfo*>::value_type val(*monsterItr, newTarget);
                        _addMonsterMap.insert(val);
                    }
                    // 잡지 못하는 몬스터로 판단
                    else
                    {
                        delete newTarget;
                    }
                }
                // 해시맵에 존재하지 않으면 값 추가
                else
                {
                    hash_map<const Monster*, AddMonsterInfo*>::value_type val(*monsterItr, newTarget);
                    _addMonsterMap.insert(val);
                }
            }
            // 잡지 못하는 몬스터로 판단
            else
            {
                delete newTarget;
            }
        }
        prev = next;
    }

    if (_addMonsterMap.size() == 0) return false; // 더 이상 사냥 불가
    else return true;
}

list<const Monster*>::iterator User::HuntRemainMonster()
{
    // 해시맵을 순회하며 시간당 최대 경험치를 갖는 몬스터 선정
    hash_map<const Monster*, AddMonsterInfo*>::iterator monMapItr = _addMonsterMap.begin();
    const AddMonsterInfo* maxTarget = (*(monMapItr++)).second; // 첫번째 요소로 초기화
    for (; monMapItr != _addMonsterMap.end(); monMapItr++)
    {
        const AddMonsterInfo* newTarget = (*monMapItr).second;
        // 시간당 경험치가 더 많으면
        if (newTarget->GetExpPerTime() > maxTarget->GetExpPerTime())
        {
            maxTarget = newTarget;
        }
        // 서로 같으면 시간이 조금 걸리는 것으로
        else if (newTarget->GetExpPerTime() == maxTarget->GetExpPerTime())
        {
            if (newTarget->GetAddPlayTime() < maxTarget->GetAddPlayTime())
            {
                maxTarget = newTarget;
            }
        }
    }

    // 그 몬스터(maxTarget)를 사냥
    // _huntList에서 maxTarget의 위치를 찾음
    list<const Monster*>::iterator insertPosItr;
    for (insertPosItr = _huntList.begin(); insertPosItr != _huntList.end(); insertPosItr++)
    {
        if (*insertPosItr == maxTarget->GetPrev())
        {
            insertPosItr++;
            // prev와 next가 모두 일치하여 그 위치를 찾은 경우
            if (*insertPosItr == maxTarget->GetNext()) break;
            else
            {
                std::cout << "Error!" << std::endl;
            }
        }
    }

    // maxTarget의 몬스터 삽입
    _huntList.insert(insertPosItr, *maxTarget->GetHuntMonsterItr());
    _playTime += maxTarget->GetAddPlayTime();
    _exp += maxTarget->GetAddExp();

    // maxTarget 삽입에 따른 prev와 next가 같았던 요소 수정
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
            // prev->cur->max->next 인 경우
            AddMonsterInfo* prevCurMaxNextCur = new AddMonsterInfo(curPrev, maxMon, curTarget->GetHuntMonsterItr());
            AddMonsterInfo* prevCurMaxNextMax = new AddMonsterInfo(curMon, curNext, maxTarget->GetHuntMonsterItr());
            // 상대적 소요 시간
            int prevCurMaxNextTime = curPrev->GetPosition()->CalculateDistance(*curMon->GetPosition()) + 
                maxMon->GetPosition()->CalculateDistance(*curNext->GetPosition());

            // prev->max->cur->next 인 경우
            AddMonsterInfo* prevMaxCurNextCur = new AddMonsterInfo(maxMon, curNext, curTarget->GetHuntMonsterItr());
            AddMonsterInfo* prevMaxCurNextMax = new AddMonsterInfo(curPrev, curMon, maxTarget->GetHuntMonsterItr());
            // 상대적 소요 시간
            int prevMaxCurNextTime = curPrev->GetPosition()->CalculateDistance(*maxMon->GetPosition()) +
                curMon->GetPosition()->CalculateDistance(*curNext->GetPosition());

            // prev->cur->max->next 의 시간이 더 짧을 경우
            if (prevCurMaxNextTime < prevMaxCurNextTime)
            {
                curTarget->SetNext(maxMon);
            }
            // prev->max->cur->next 의 시간이 더 짧을 경우 혹은 같을 경우
            else
            {
                curTarget->SetPrev(maxMon);
            }
        }
    }

    // _addMonsterMap에서 maxTarget의 몬스터 제거
    _addMonsterMap.erase(*maxTarget->GetHuntMonsterItr());

    // 플레이타임 비교하여 못잡는 몬스터 제거
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
    // 새로 사냥할 몬스터에 대한 증가될 시간과 경험치
    unsigned int extPlayTime = (extMonster->GetHuntTime() + extMonster->GetPosition()->CalculateDistance(*_position));
    unsigned int extExp = extMonster->GetExp();

    // 현재까지 사냥한 몬스터들을 가중치로 정렬하여 huntSet에 저장
    set<MonsterInfo> huntSet;
    
    // huntSet의 첫 요소
    list<const Monster*>::iterator huntItr = _huntList.begin(), prevHuntItr = huntItr;
    unsigned int moveTime = (*huntItr)->GetPosition()->CalculateDistance(*_startPosition);
    unsigned int huntTime = (*huntItr)->GetHuntTime();
    unsigned int exp = (*huntItr)->GetExp();
    MonsterInfo firstMonster;
    firstMonster.pos = huntItr;
    firstMonster.moveHuntTime = moveTime + huntTime;
    firstMonster.weight = static_cast<double>(exp) / (moveTime + huntTime); // 가중치: 시간당 경험치
    huntSet.insert(firstMonster);
    huntItr++;

    // huntSet 요소 추가
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

    // set은 가중치로 정렬되어 있음. 가중치가 낮은 것부터 빼면서 시간을 확보하고
    // 충분한 시간이 되면 extMonster와 비교하여 extMonster가 효율이 좋으면 대체한다.
    unsigned int removePlayTime = 0;
    unsigned int addPlayTime = 0;
    unsigned int removeExp = 0;
    bool huntExt = false;

    // huntSet에서 제외시킬 리스트
    list<MonsterInfo> rmHuntList;

    while (true)
    {
        /*
        // 만약 남은 시간 내 사냥 가능하면 사냥함
        if (remainTime > extPlayTime) {
            huntExt = true;
            break;
        }*/

        // 가장 효율이 안좋은 반복자를 찾음
        set<MonsterInfo>::iterator huntSetItr = huntSet.begin();
        
        // 그 반복자의 몬스터 반복자와 앞 뒤를 찾음
        list<const Monster*>::iterator curMonItr = (*huntSetItr).pos;
        list<const Monster*>::iterator prevMonItr = --curMonItr;
        curMonItr = (*huntSetItr).pos;
        list<const Monster*>::iterator nextMonItr = ++curMonItr;
        curMonItr = (*huntSetItr).pos;
        if (nextMonItr == _huntList.end()) return false;

        // 각 위치의 몬스터를 찾음
        const Monster* curMon = *curMonItr;
        const Monster* prevMon = *prevMonItr;
        const Monster* nextMon = *nextMonItr;

        // 변경될 거리에 대한 계산
        unsigned int prevToNext = prevMon->GetPosition()->CalculateDistance(*nextMon->GetPosition());
        unsigned int curToNext = curMon->GetPosition()->CalculateDistance(*nextMon->GetPosition());

        // 변경될 시간 및 경험치 계산
        removePlayTime += (*huntSetItr).moveHuntTime + curToNext;
        addPlayTime += prevToNext;
        removeExp += curMon->GetExp();

        // 현재 몬스터의 제외 및 제외목록 리스트 유지
        rmHuntList.push_back(*huntSetItr);
        huntSet.erase(huntSetItr);

        // 목록 제외에 따른 nextMon의 이동거리 및 가중치 재계산
        list<const Monster*>::iterator findPos;
        // 먼저 nextMon을 huntSet에서 찾아 제외시킴
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
        // nextMon으로 다시 MonsterInfo를 구성하여 추가
        MonsterInfo nextMonInfo;
        nextMonInfo.pos = findPos;
        nextMonInfo.moveHuntTime = prevToNext + (*nextMonInfo.pos)->GetHuntTime();
        nextMonInfo.weight = static_cast<double>((*nextMonInfo.pos)->GetExp()) / nextMonInfo.moveHuntTime;
        huntSet.insert(nextMonInfo);

        // 충분한 시간 확보했으면 빠져나옴
        if (removePlayTime + remainTime - addPlayTime >= extPlayTime)
        {
            // 새로 잡을 몬스터의 효율이 더 좋으면 사냥함
            if ((double)extExp/extPlayTime > (double)removeExp/(removePlayTime-addPlayTime))
            {
                huntExt = true;
            }
            break;
        }
    }
    
    // 기존 효율 낮은 몬스터를 제거하고 새 몬스터를 잡음
    if (huntExt)
    {
        list<MonsterInfo>::iterator delItr;
        for (delItr = rmHuntList.begin(); delItr != rmHuntList.end(); delItr++)
        {
            // 기존 몬스터 메모리 해제 및 사냥 리스트에서 제외
            const Monster* delMonster = *(*delItr).pos;
            _huntList.erase((*delItr).pos);
            delete delMonster;
        }
        // 리스트 제외에 따른 플레이시간, 경험치 변경
        _playTime -= removePlayTime;
        _playTime += addPlayTime;
        _exp -= removeExp;

        // extMonster 사냥
        HuntMonster(extMonster);
        return true;
    }
    else return false;
}