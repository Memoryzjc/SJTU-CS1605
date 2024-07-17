#ifndef ACTOR_H
#define ACTOR_H

#include "define.h"
#include "helper.h"

#include <map>
#include <vector>


class BattleField;


class Actor {
public:
    // destructor
    virtual ~Actor() = default;

    // choose starting slime
    virtual std::vector<Slime_T> ChooseStartingSlime() = 0;

    // choose action
    virtual Action_T ChooseAction() = 0;

    // choose slime
    // parameter:
    // true for active change,
    // false for passive change
    // return a std::pair:
    // first for whether to not back,
    // second for the slime to change
    virtual std::pair<bool, Slime_T> ChooseSlime(bool) = 0;

    // choose skill
    // return a std::pair:
    // first for whether to not back,
    // second for the skill to use
    virtual std::pair<bool, Skill_T> ChooseSkill() = 0;

    // choose potion
    virtual PotionInfo ChoosePotion() { return {}; }

    // get name
    virtual std::string GetName() = 0;

    Actor* opp;                                 // 敌方指针
    BattleField* field;                         // 战场指针
    Slime* slimeOnCourt;                         // 当前场上的宠物
    std::map<Slime_T, Slime> availSlimes;       // 可以换上场的宠物
    std::map<Slime_T, Slime> grave;             // 战败的宠物
    DefaultedMap<std::string, int, 0> attr;     // 动态属性，如剩余药剂数量等
};

class SunnyActor : public Actor {
public:
    ~SunnyActor() override = default;
    std::vector<Slime_T> ChooseStartingSlime() override;
    Action_T ChooseAction() override;
    std::pair<bool, Slime_T> ChooseSlime(bool) override;
    std::pair<bool, Skill_T> ChooseSkill() override;
    std::string GetName() override;
};

class RainyActor : public Actor {
public:
    RainyActor();
    ~RainyActor() override = default;
    std::vector<Slime_T> ChooseStartingSlime() override;
    Action_T ChooseAction() override;
    std::pair<bool, Slime_T> ChooseSlime(bool) override;
    std::pair<bool, Skill_T> ChooseSkill() override;
    PotionInfo ChoosePotion() override;
    std::string GetName() override;
};

class HumanActor : public Actor {
public:
    HumanActor();
    ~HumanActor() override = default;
    std::vector<Slime_T> ChooseStartingSlime() override;
    Action_T ChooseAction() override;
    std::pair<bool, Slime_T> ChooseSlime(bool) override;
    std::pair<bool, Skill_T> ChooseSkill() override;
    std::string GetName() override;

private:
    std::vector<Slime_T> allSlimeTs;
};

#endif //ACTOR_H
