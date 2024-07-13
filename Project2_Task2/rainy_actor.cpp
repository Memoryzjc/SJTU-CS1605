#include <cassert>
#include "actor.h"
#include "battlefield.h"


RainyActor::RainyActor() {
    this->attr["revivalPotion"] = 1;
}

std::vector<Slime_T> RainyActor::ChooseStartingSlime() {
    // shuffle the vector of slimes to get the starting slimes
    std::vector<Slime_T> slimes = {Slime_T::G, Slime_T::B, Slime_T::Y};
    ShuffleVector(slimes);

    this->availSlimes = {{Slime_T::G, allSlimes.at(Slime_T::G)},
                         {Slime_T::B, allSlimes.at(Slime_T::B)},
                         {Slime_T::Y, allSlimes.at(Slime_T::Y)}};

    this->slimeOnCourt = &this->availSlimes[slimes[0]];

    return slimes;
}

Action_T RainyActor::ChooseAction() {
    Slime *oppSlime = this->opp->slimeOnCourt, *mySlime = this->slimeOnCourt;

    if (this->grave.contains(Slime_T::B) && this->attr["revivalPotion"]) {
        return Action_T::Potion;
    }

    switch (mySlime->slime) {
        case Slime_T::G:
            if (oppSlime->type == Type::Fire &&
                this->availSlimes.contains(Slime_T::B)) {
                return Action_T::Change;
            }

            return Action_T::Skill;
        case Slime_T::B:
            if ((this->field->attr["weather"] != Weather_T::Sunny && oppSlime->type == Type::Thunder) ||
                (this->field->attr["weather"] == Weather_T::Rainy && oppSlime->type == Type::Grass)) {
                if (this->availSlimes.contains(Slime_T::G)) {
                    return Action_T::Change;
                } else {
                    if (this->field->attr["weather"] == Weather_T::Rainy &&
                        this->availSlimes.contains(Slime_T::Y)) {
                        return Action_T::Change;
                    } else {
                        return Action_T::Skill;
                    }
                }
            }

            return Action_T::Skill;
        default:
            if ((oppSlime->type == Type::Fire && this->availSlimes.contains(Slime_T::B)) ||
                (oppSlime->type != Type::Fire && this->field->attr["weather"] != Weather_T::Rainy && this->availSlimes.contains(Slime_T::G))) {
                return Action_T::Change;
            } else {
                return Action_T::Skill;
            }
    }
}

std::pair<bool, Slime_T> RainyActor::ChooseSlime(bool active) {
    std::pair<bool, Slime_T> ret;
    ret.first = false;

    if (active) {
        switch (this->slimeOnCourt->slime) {
            case Slime_T::G:
                assert(this->availSlimes.contains(Slime_T::B));
                ret.second = Slime_T::B;
                this->slimeOnCourt = &this->availSlimes[Slime_T::B];
                break;
            case Slime_T::B:
                if (this->availSlimes.contains(Slime_T::G)) {
                    ret.second = Slime_T::G;
                    this->slimeOnCourt = &this->availSlimes[Slime_T::G];
                } else {
                    assert(this->availSlimes.contains(Slime_T::Y));
                    ret.second = Slime_T::Y;
                    this->slimeOnCourt = &this->availSlimes[Slime_T::Y];
                }
                break;
            default:
                if (this->opp->slimeOnCourt->type == Type::Fire) {
                    assert(this->availSlimes.contains(Slime_T::B));
                    ret.second = Slime_T::B;
                    this->slimeOnCourt = &this->availSlimes[Slime_T::B];
                } else {
                    assert(this->availSlimes.contains(Slime_T::G));
                    ret.second = Slime_T::G;
                    this->slimeOnCourt = &this->availSlimes[Slime_T::G];
                }
        }
    } else {
        switch (this->slimeOnCourt->slime) {
            case Slime_T::G:
            case Slime_T::Y:
                if (this->availSlimes.contains(Slime_T::B)) {
                    this->slimeOnCourt = &this->availSlimes[Slime_T::B];
                    ret.second = Slime_T::B;
                } else {
                    this->slimeOnCourt = (this->slimeOnCourt->slime == Slime_T::G)
                            ? &this->availSlimes[Slime_T::Y] : &this->availSlimes[Slime_T::G];
                    ret.second = (this->slimeOnCourt->slime == Slime_T::G) ? Slime_T::G : Slime_T::Y;
                }

                break;
            default:
                if (this->availSlimes.contains(Slime_T::G)) {
                    this->slimeOnCourt = &this->availSlimes[Slime_T::G];
                    ret.second = Slime_T::G;
                } else {
                    this->slimeOnCourt = &this->availSlimes[Slime_T::Y];
                    ret.second = Slime_T::Y;
                }
                break;
        }
    }

    return ret;
}

std::pair<bool, Skill_T> RainyActor::ChooseSkill() {
    std::pair<bool, Skill_T> ret;
    ret.first = false;

    switch(this->slimeOnCourt->slime) {
        case Slime_T::G:
            if (this->opp->slimeOnCourt->type == Type::Fire) {
                ret.second = (double(this->slimeOnCourt->health) >= double(allSlimes.at(Slime_T::G).health) * 2. / 3.) ?
                    Skill_T::Tackle : Skill_T::Synthesis;
            } else if (this->opp->slimeOnCourt->type == Type::Grass) {
                ret.second = (double(this->slimeOnCourt->health) >= double(allSlimes.at(Slime_T::G).health) / 2.) ?
                    Skill_T::Tackle : Skill_T::Synthesis;
            } else if (double(this->slimeOnCourt->health) >= double(allSlimes.at(Slime_T::G).health) / 2. ||
                       this->opp->slimeOnCourt->health < allSlimes.at(this->opp->slimeOnCourt->slime).health / 5.) {
                ret.second = Skill_T::Leaf;
            } else {
                ret.second = Skill_T::Synthesis;
            }
            break;
        case Slime_T::B:
            if (this->field->attr["weather"] == Weather_T::Sunny) {
                ret.second = Skill_T::Rainy;
            } else if ((this->field->attr["weather"] != Weather_T::Sunny && this->opp->slimeOnCourt->type == Type::Thunder) ||
                       (this->field->attr["weather"] == Weather_T::Rainy && this->opp->slimeOnCourt->type == Type::Grass)){
                ret.second = Skill_T::Stream;
            } else if (this->field->attr["weather"] != Weather_T::Sunny && this->opp->slimeOnCourt->type == Type::Water) {
                ret.second = Skill_T::Tackle;
            } else {
                ret.second = (this->field->attr["weather"] == Weather_T::Rainy) ? Skill_T::Stream : Skill_T::Rainy;
            }
            break;
        default:
            // only Yellow
            if (double(this->opp->slimeOnCourt->health) < double(allSlimes.at(opp->slimeOnCourt->slime).health) / 5.) {
                ret.second = Skill_T::Quick;
            } else {
                if (this->opp->slimeOnCourt->type == Type::Fire) {
                    ret.second = Skill_T::Volt;
                } else {
                    ret.second = (this->field->attr["weather"] == Weather_T::Rainy)
                            ? Skill_T::Thunder :
                            (this->opp->slimeOnCourt->type == Type::Thunder || this->opp->slimeOnCourt->type == Type::Grass)
                            ? Skill_T::Quick : Skill_T::Volt;
                }
            }
    }

    return ret;
}

PotionInfo RainyActor::ChoosePotion() {
    return {
        .potionT = Potion_T::Revival,
        .slimeT = Slime_T::B
    };
}

std::string RainyActor::GetName() {
    return "Enemy";//"Rainy Team";
}

