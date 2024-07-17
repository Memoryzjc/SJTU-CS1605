#include "define.h"
#include "actor.h"
#include "helper.h"
#include "battlefield.h"

#include <cmath>

const std::map<std::pair<Type_T, Type_T>, double> typeRatio{
        {{Type::Normal, Type::Normal}, 1.},
        {{Type::Normal, Type::Grass},  1.},
        {{Type::Normal, Type::Fire},   1.},
        {{Type::Normal, Type::Water},  1.},
        {{Type::Normal, Type::Thunder}, 1.},
        {{Type::Grass,  Type::Normal}, 1.},
        {{Type::Grass,  Type::Grass},  .5},
        {{Type::Grass,  Type::Fire},   .5},
        {{Type::Grass,  Type::Water},  2.},
        {{Type::Grass, Type::Thunder}, 1.},
        {{Type::Fire,   Type::Normal}, 1.},
        {{Type::Fire,   Type::Grass},  2.},
        {{Type::Fire,   Type::Fire},   .5},
        {{Type::Fire,   Type::Water},  .5},
        {{Type::Fire, Type::Thunder}, 1.},
        {{Type::Water,  Type::Normal}, 1.},
        {{Type::Water,  Type::Grass},  .5},
        {{Type::Water,  Type::Fire},   2.},
        {{Type::Water,  Type::Water},  .5},
        {{Type::Water, Type::Thunder}, 1.},
        {{Type::Thunder,  Type::Normal}, 1.},
        {{Type::Thunder,  Type::Grass},  .5},
        {{Type::Thunder,  Type::Fire},   1.},
        {{Type::Thunder,  Type::Water},  2.},
        {{Type::Thunder, Type::Thunder}, .5},
};

// double cannot be passed by template until c++20
DefaultedMap<std::pair<Weather_T, Type_T>, int, 100> weatherRatio{
        {{Weather::Sunny, Type::Fire}, 150},
        {{Weather::Sunny, Type::Water}, 50},
        {{Weather::Rainy, Type::Fire}, 50},
        {{Weather::Rainy, Type::Water}, 150},
};

const std::map<Slime_T, Slime> allSlimes{
        {Slime::G, {Slime::G, Type::Grass,   "Green",  110, 10., 10., 10., {Skill::Tackle, Skill::Leaf,   Skill::Synthesis}}},
        {Slime::R, {Slime::R, Type::Fire,    "Red",    100, 12., 10., 11., {Skill::Claw,   Skill::Flame,  Skill::Sunny}}},
        {Slime::B, {Slime::B, Type::Water,   "Blue",   100, 10., 14., 9.,  {Skill::Tackle, Skill::Stream, Skill::Rainy}}},
        {Slime::Y, {Slime::Y, Type::Thunder, "Yellow", 90,  10., 10., 12., {Skill::Quick,  Skill::Volt,   Skill::Thunder}}},
        {Slime::P, {Slime::P, Type::Normal,  "Pink",   120, 9.,  8.,  7.,  {Skill::Slap,   Skill::Sing,   Skill::Paint}}}
};

const std::map<Potion_T, Potion> allPotions{
        {Potion::Revival, {Potion::Revival, "Revival Potion"}},
        {Potion::Attack,  {Potion::Attack,  "Attack Potion"}},
};

const std::map<Action_T, Action> allActions{
        {Action::Escape, {Action::Escape, "escape", 8}},
        {Action::Change, {Action::Change, "change", 6}},
        {Action::Skill, {Action::Skill, "skill", 0}},
        {Action::Potion, {Action::Potion, "potion", 5}}
};

// 普通伤害型技能的回调函数
// 以该函数实现为例，实现其余具有特殊效果的技能
// 返回值中，
// .success代表是否成功使用技能
// .damage表示对敌方造成的伤害
// .prompt为输出语句，使用技能的完整输出为 "XXX’s X uses XXX! ..."，感叹号之后的输出为.prompt的内容
SkillCallback defaultCb = [](BattleField *field, Actor *me, const Skill *skill) {
    std::stringstream ss;
    Slime *mySlime = me->slimeOnCourt, *oppSlime = me->opp->slimeOnCourt;

    // 伤害计算过程中各个量按照浮点数处理，最终四舍五入后转换成整型
    // 伤害 = 类型倍率 * 天气倍率（这里是百分数所以需要除以100.） * 威力 * 攻击力 / 防御
    int damage = int(round(typeRatio.at({skill->type, oppSlime->type}) *
                           double(weatherRatio[{Weather_T(field->attr["weather"]), skill->type}]) / 100.
                           * skill->power * mySlime->attack / oppSlime->defense));
    PRINT(ss, "Damage:", damage);

    if (oppSlime->attr["faint"]) {
        oppSlime->attr["faint"] = oppSlime->attr["faintCnt"] = 0;
        if (oppSlime->health - damage > 0.) {
            PRINT(ss, "\n");
            PRINT(ss, BattleField::Whose(me->opp->GetName()), oppSlime->slimeName, "wakes up");
        }
    }

    return SkillCbReturn{
            .success = true,
            .damage = damage,
            .prompt = ss.str()
    };
};

// Slap技能回调函数
SkillCallback SlapCb = [](BattleField *field, Actor *me, const Skill *skill) {
    std::stringstream ss;
    Slime *mySlime = me->slimeOnCourt, *oppSlime = me->opp->slimeOnCourt;

    double damage = typeRatio.at({skill->type, oppSlime->type}) *
                           double(weatherRatio[{Weather_T(field->attr["weather"]), skill->type}]) / 100.
                           * skill->power * mySlime->attack / oppSlime->defense;

    if (oppSlime->attr["faint"]) {
        damage *= 2.;
        oppSlime->attr["faint"] = oppSlime->attr["faintCnt"] = 0;
        PRINT(ss, "Damage:", int(round(damage)));
        if (oppSlime->health - int(round(damage)) > 0.) {
            PRINT(ss, "\n");
            PRINT(ss, BattleField::Whose(me->opp->GetName()), oppSlime->slimeName, "wakes up");
        }
    }

    if (ss.str().empty()) {
        PRINT(ss, "Damage:", int(round(damage)));
    }

    return SkillCbReturn {
            .success = true,
            .damage = int(round(damage)),
            .prompt = ss.str()
    };
};

// Sing技能回调函数
SkillCallback SingCb = [](BattleField *field, Actor *me, const Skill *skill) {
    std::stringstream ss;
    Slime *oppSlime = me->opp->slimeOnCourt;

    if (oppSlime->attr["faint"]) {
        PRINT(ss, "Waste");
    } else {
        oppSlime->attr["faint"] = 1;
        oppSlime->attr["faintCnt"] = 3;
        PRINT(ss, BattleField::Whose(me->opp->GetName()), oppSlime->slimeName, "falls asleep");
    }

    return SkillCbReturn {
            .success = true,
            .damage = 0,
            .prompt = ss.str()
    };
};

// Paint技能回调函数
SkillCallback PaintCb = [](BattleField *field, Actor *me, const Skill *skill) {
    std::stringstream ss;
    Slime *oppSlime = me->opp->slimeOnCourt;


    if (!oppSlime->attr["paint"]) {
        oppSlime->defense *= 0.5;
    }

    // 无论是否已经被涂鸦，都会被涂鸦
    oppSlime->attr["paint"] = 1;
    PRINT(ss, BattleField::Whose(me->opp->GetName()), oppSlime->slimeName, "is painted");

    return SkillCbReturn {
            .success = true,
            .damage = 0,
            .prompt = ss.str()
    };
};

// Synthesis技能回调函数
SkillCallback SynthesisCb = [](BattleField *field, Actor *me, const Skill *skill) {
    std::stringstream ss;

    double recoverRatio = .3;
    switch (Weather_T(field->attr["weather"])) {
        case Weather::Sunny:
            recoverRatio = .4;
            break;
        case Weather::Rainy:
            recoverRatio = .2;
            break;
        default:
            break;
    }

    // recover the recoverRatio of the maximum health
    int recoverHealth = static_cast<int>(round((recoverRatio *
            double(allSlimes.at(me->slimeOnCourt->slime).health))));

    // recover health but not exceed the maximum health
    me->slimeOnCourt->health = std::min(me->slimeOnCourt->health + recoverHealth,
                                        allSlimes.at(me->slimeOnCourt->slime).health);
    
    PRINT(ss, "Recovery:" ,recoverHealth);

    return SkillCbReturn {
            .success = true,
            .damage = 0,
            .prompt = ss.str()
    };
};

// Sunny技能回调函数
SkillCallback SunnyCb = [](BattleField *field, Actor *me, const Skill *skill) {
    std::stringstream ss;

    field->attr["weather"] = Weather::Sunny;
    field->attr["weatherCnt"] = 5;
    PRINT(ss, "Weather becomes sunny");

    return SkillCbReturn {
            .success = true,
            .damage = 0,
            .prompt = ss.str()
    };
};

// Rainy技能回调函数
SkillCallback RainyCb = [](BattleField *field, Actor *me, const Skill *skill) {
    std::stringstream ss;

    field->attr["weather"] = Weather::Rainy;
    field->attr["weatherCnt"] = 5;
    PRINT(ss, "Weather becomes rainy");

    return SkillCbReturn {
            .success = true,
            .damage = 0,
            .prompt = ss.str()
    };
};

// Thunder技能回调函数
SkillCallback ThunderCb = [](BattleField *field, Actor *me, const Skill *skill) {
    std::stringstream ss;
    bool success;
    int damage;

    if (field->attr["weather"] == Weather::Rainy) {
        success = true;
        Slime *mySlime = me->slimeOnCourt, *oppSlime = me->opp->slimeOnCourt;

        damage = int(round(typeRatio.at({skill->type, oppSlime->type}) *
                           double(weatherRatio[{Weather_T(field->attr["weather"]), skill->type}]) / 100.
                           * skill->power * mySlime->attack / oppSlime->defense));
        PRINT(ss, "Damage:", damage);

        if (oppSlime->attr["faint"]) {
            oppSlime->attr["faint"] = oppSlime->attr["faintCnt"] = 0;
            if (oppSlime->health - damage > 0.) {
                PRINT(ss, "\n");
                PRINT(ss, BattleField::Whose(me->opp->GetName()), oppSlime->slimeName, "wakes up");
            }
        }
    } else {
        success = false;
        damage = 0;
        PRINT(ss, "Failure");
    }

    return SkillCbReturn {
            .success = success,
            .damage = damage,
            .prompt = ss.str()
    };
};


const std::map<Skill_T, Skill> allSkills{
        {Skill::Tackle, {Skill::Tackle, "Tackle", Skill::attr::Attack, Type::Normal, 20., 0, defaultCb}},
        {Skill::Claw, {Skill::Claw, "Claw", Skill::attr::Attack, Type::Normal, 15., 0, defaultCb}},
        {Skill::Quick, {Skill::Quick, "Quick", Skill::attr::Attack, Type::Normal, 15., 1, defaultCb}},
        {Skill::Slap, {Skill::Slap, "Slap", Skill::attr::Attack, Type::Normal, 15., 0, SlapCb}},
        {Skill::Sing, {Skill::Sing, "Sing", Skill::attr::Supple, Type::Normal, 0., 0, SingCb}},
        {Skill::Paint, {Skill::Paint, "Paint", Skill::attr::Supple, Type::Normal, 0., 0, PaintCb}},
        {Skill::Leaf, {Skill::Leaf, "Leaf", Skill::attr::Attack, Type::Grass, 20., 0, defaultCb}},
        {Skill::Synthesis, {Skill::Synthesis, "Synthesis", Skill::attr::Supple, Type::Grass, 0., 0, SynthesisCb}},
        {Skill::Flame, {Skill::Flame, "Flame", Skill::attr::Attack, Type::Fire, 20., 0, defaultCb}},
        {Skill::Sunny, {Skill::Sunny, "Sunny", Skill::attr::Supple, Type::Fire, 0., 1, SunnyCb}},
        {Skill::Stream, {Skill::Stream, "Stream", Skill::attr::Attack, Type::Water, 20., 0, defaultCb}},
        {Skill::Rainy, {Skill::Rainy, "Rainy", Skill::attr::Supple, Type::Water, 0., 1, RainyCb}},
        {Skill::Volt, {Skill::Volt, "Volt", Skill::attr::Attack, Type::Thunder, 25., 0, defaultCb}},
        {Skill::Thunder, {Skill::Thunder, "Thunder", Skill::attr::Attack, Type::Thunder, 50., 0, ThunderCb}}
};
