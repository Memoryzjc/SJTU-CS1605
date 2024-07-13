#include "actor.h"
#include "battlefield.h"


std::vector<Slime_T> SunnyActor::ChooseStartingSlime() {
    // 晴天队有PGR，永远首发P
    availSlimes = {{Slime_T::P, allSlimes.at(Slime_T::P)},
                   {Slime_T::G, allSlimes.at(Slime_T::G)},
                   {Slime_T::R, allSlimes.at(Slime_T::R)}};
    this->slimeOnCourt = &availSlimes.at(Slime_T::P);
    return {Slime_T::P, Slime_T::G, Slime_T::R};
}

Action_T SunnyActor::ChooseAction() {
    switch(this->slimeOnCourt->slime) {
        case Slime::P:
        case Slime::R:
            return Action_T::Skill;
        case Slime::G:
            if (this->slimeOnCourt->health < allSlimes.at(Slime_T::G).health / 2
                && this->grave.size() != 2) {
                return Action_T::Change;
            } else {
                return Action_T::Skill;
            }
        default:
            return Action_T::Escape;
    }
}

std::pair<bool, Slime_T> SunnyActor::ChooseSlime(bool active) {
    std::pair<bool, Slime_T> res;
    res.first = false;
    if (active) {
        this->slimeOnCourt = &availSlimes.at(Slime_T::R);
        res.second = Slime_T::R;
    } else {
        if (this->slimeOnCourt->slime == Slime::P || this->slimeOnCourt->slime == Slime::R) {
            this->slimeOnCourt = &availSlimes.at(Slime_T::G);
            res.second = Slime_T::G;
        } else {
            this->slimeOnCourt = &availSlimes.at(Slime_T::R);
            res.second = Slime_T::R;
        }
    }

    return res;
}

std::pair<bool, Skill_T> SunnyActor::ChooseSkill() {
    std::pair<bool, Skill_T> res;
    res.first = false;
    switch (this->slimeOnCourt->slime) {
        case Slime::P:
            if ((static_cast<double>(opp->slimeOnCourt->health) <
                static_cast<double>(allSlimes.at(opp->slimeOnCourt->slime).health) * 1. / 3.)
                && (opp->slimeOnCourt->attr["faint"] || opp->slimeOnCourt->attr["paint"])) {
                res.second = Skill_T::Slap;
            } else {
                if (!opp->slimeOnCourt->attr["faint"]) {
                    res.second = Skill_T::Sing;
                } else {
                    if (!opp->slimeOnCourt->attr["paint"]) {
                        res.second = Skill_T::Paint;
                    } else {
                        res.second = Skill_T::Slap;
                    }
                }
            }

            break;
        case Slime::G:
            if (this->slimeOnCourt->health >= allSlimes.at(Slime_T::G).health / 2 ||
               (this->slimeOnCourt->health > opp->slimeOnCourt->health && grave.size() == 2)) {
                res.second = (opp->slimeOnCourt->type == Type::Water) ?
                        allSlimes.at(Slime_T::G).skills[1] : allSlimes.at(Slime_T::G).skills[0];
            } else {
                res.second = allSlimes.at(Slime_T::G).skills[2];
            }

            break;
        case Slime::R:
            if ((static_cast<double>(opp->slimeOnCourt->health) <
                static_cast<double>(allSlimes.at(opp->slimeOnCourt->slime).health) * 1. / 5.)
                && (opp->slimeOnCourt->type == Type::Water || opp->slimeOnCourt->type == Type::Fire)) {
                res.second = allSlimes.at(Slime_T::R).skills[0];
            } else {
                res.second = (this->field->attr["weather"] == Weather_T::Sunny) ?
                        allSlimes.at(Slime_T::R).skills[1] : allSlimes.at(Slime_T::R).skills[2];
            }

            break;
        default:
            res.second = allSkills.at(Skill_T::Tackle).skill;
            break;
    }

    return res;
}

std::string SunnyActor::GetName() {
    return "Enemy";//"Sunny Team";
}
