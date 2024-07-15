#include "battlefield.h"

#include "define.h"
#include "helper.h"

#include <string>
#include <algorithm>

BattleField::BattleField(int maxRounds): maxRounds_(maxRounds) {}

void BattleField::Reset() {
    this->rounds_ = 0;
    this->actors_[0] = this->actors_[1] = nullptr;
    this->attr.clear();
}

void BattleField::Run() {
    this->BeforeGame_();

    bool ending = false;
    while (!ending) {
        LOG("--------------------------------------------------\n");
        this->StateCheck_();
        LOG("Round", ++this->rounds_);
        LOG('\n');
        std::vector<ActionInfo> actions = this->ChooseActionPhase_();

        if (actions[0].action == Action_T::Escape) {
            LOG("You escape\n");
            return;
        }

        this->PerformActionPhase_(actions);
        this->HandleBeatenSlimesPhase_();
        ending = this->ShowInformationPhase_();
    }
}

void BattleField::SetActor(Actor *a, Actor *b) {
    this->actors_[0] = a; this->actors_[1] = b;
    a->opp = b; b->opp = a;
    a->field = b->field = this;
}

void BattleField::BeforeGame_() {
    // welcome messages
    LOG("Welcome to Battle of Slimes 2!\n");
    std::vector<Slime_T> playerSlimeTs, enemySlimeTs = this->actors_[1]->ChooseStartingSlime();

    // sunny actor
//     LOG("Enemy has Pink, Green and Red, starting with Pink\n");

    // rainy actor
    LOG("Enemy has Green, Blue and Yellow, starting with", this->actors_[1]->slimeOnCourt->slimeName + "\n");

    LOG("You can select three from five (1 for Green, 2 for Red, 3 for Blue, 4 for Yellow, 5 for Pink).\n");

    playerSlimeTs = this->actors_[0]->ChooseStartingSlime();

    LOG("You have", allSlimes.at(playerSlimeTs[0]).slimeName + ',',
        allSlimes.at(playerSlimeTs[1]).slimeName, "and",
        allSlimes.at(playerSlimeTs[2]).slimeName + ", starting with",
        this->actors_[0]->slimeOnCourt->slimeName, "\n");
    LOG(Whose("You"), this->actors_[0]->slimeOnCourt->slimeName + ": HP",
        this->actors_[0]->slimeOnCourt->health, "||",
        Whose("Enemy"), this->actors_[1]->slimeOnCourt->slimeName + ": HP",
        this->actors_[1]->slimeOnCourt->health, "\n");

    // game start messages
    LOG("Battle starts!\n");
}

void BattleField::StateCheck_() {
    // check weather(Sunny, Rainy or Normal)
    switch (Weather_T(this->attr["weather"])) {
        case Weather::Sunny:
        case Weather::Rainy:
            if (!this->attr["weatherCnt"]) {
                LOG("Weather becomes normal.\n");
                this->attr["weather"] = Weather_T::Normal;
            }
            break;
        default:
            break;
    }

    // check faint
    if (this->actors_[0]->slimeOnCourt->attr["faint"]) {
        if (!this->actors_[0]->slimeOnCourt->attr["faintCnt"]) {
            LOG(Whose("You"), this->actors_[0]->slimeOnCourt->slimeName, "wakes up\n");
            this->actors_[0]->slimeOnCourt->attr["faint"] = 0;
        }
    }

    if (this->actors_[1]->slimeOnCourt->attr["faint"]) {
        if (!this->actors_[1]->slimeOnCourt->attr["faintCnt"]) {
            LOG(Whose("Enemy"), this->actors_[1]->slimeOnCourt->slimeName, "wakes up\n");
            this->actors_[1]->slimeOnCourt->attr["faint"] = 0;
        }
    }
}

std::vector<ActionInfo> BattleField::ChooseActionPhase_() {
    Action_T enemyAction = this->actors_[1]->ChooseAction(), playerAction;
    std::vector<ActionInfo> actions = {{}, {}};
    std::pair<bool, Skill_T> playerChooseSkill;
    std::pair<bool, Slime_T> playerChooseSlime;

    // set actors
    actions[0].actor = this->actors_[0];
    actions[1].actor = this->actors_[1];

    // enemy choose action
    actions[1].action = enemyAction;
    actions[1].priority = allActions.at(enemyAction).priority;
    switch(enemyAction) {
        case (Action_T::Skill):
            actions[1].u.skill = this->actors_[1]->ChooseSkill().second;
            break;
        case (Action_T::Change):
            actions[1].u.slime = this->actors_[1]->ChooseSlime(true).second;
            break;
        default:
            // use potion
            actions[1].u.potion = this->actors_[1]->ChoosePotion();
            break;
    }

    // player choose action
    bool notBack;
    while (true) {
        // player choose action
        playerAction = this->actors_[0]->ChooseAction();
        actions[0].action = playerAction;
        actions[0].priority = allActions.at(playerAction).priority;

        switch (playerAction) {
            case (Action_T::Skill):
                playerChooseSkill = this->actors_[0]->ChooseSkill();
                notBack = playerChooseSkill.first;
                actions[0].u.skill = playerChooseSkill.second;
                break;
            case (Action_T::Change):
                playerChooseSlime = this->actors_[0]->ChooseSlime(true);
                notBack = playerChooseSlime.first;
                actions[0].u.slime = playerChooseSlime.second;
                break;
            default:
                notBack = true;
                break;
        }

        if (notBack) {
            break;
        }
    }

    return actions;
}

// function to attack each other
// the priority of firstActor's skill is higher
void BattleField::attackEachOther(int firstActor, int secondActor, std::vector<ActionInfo> & actions) {
    ActionInfo firstActorAction = actions[firstActor], secondActorAction = actions[secondActor];

    // the priority of firstActor's skill is higher
    if (this->actors_[firstActor]->slimeOnCourt->attr["faint"]) {
        // firstActor's slime is faint
        LOG(this->Whose(this->actors_[firstActor]->GetName()),
            this->actors_[firstActor]->slimeOnCourt->slimeName, "is sleeping\n");

        if (this->actors_[secondActor]->slimeOnCourt->attr["faint"]) {
            // both faint
            LOG(this->Whose(this->actors_[secondActor]->GetName()),
                this->actors_[secondActor]->slimeOnCourt->slimeName, "is sleeping\n");
        } else {
            // secondActor's slime is not faint
            SkillCbReturn secondActorRet =
                    allSkills.at(secondActorAction.u.skill).cb(this,
                                                                  this->actors_[secondActor],
                                                                  &allSkills.at(secondActorAction.u.skill));
            this->actors_[firstActor]->slimeOnCourt->health =
                    std::max(this->actors_[firstActor]->slimeOnCourt->health - secondActorRet.damage, 0);

            LOG(this->Whose(this->actors_[secondActor]->GetName()),
                this->actors_[secondActor]->slimeOnCourt->slimeName, "uses",
                allSkills.at(secondActorAction.u.skill).skillName + '!', secondActorRet.prompt + '\n');
        }
    } else {
        // firstActor's slime is not faint
        SkillCbReturn firstActorRet =
                allSkills.at(firstActorAction.u.skill).cb(this,
                                                             this->actors_[firstActor],
                                                             &allSkills.at(firstActorAction.u.skill));
        this->actors_[secondActor]->slimeOnCourt->health =
                std::max(this->actors_[secondActor]->slimeOnCourt->health - firstActorRet.damage, 0);
        LOG(this->Whose(this->actors_[firstActor]->GetName()),
            this->actors_[firstActor]->slimeOnCourt->slimeName, "uses",
            allSkills.at(firstActorAction.u.skill).skillName + '!', firstActorRet.prompt + '\n');

        if (!this->actors_[secondActor]->slimeOnCourt->health) {
            return;
        }

        if (this->actors_[secondActor]->slimeOnCourt->attr["faint"]) {
            // secondActor's slime is faint
            LOG(this->Whose(this->actors_[secondActor]->GetName()),
                this->actors_[secondActor]->slimeOnCourt->slimeName, "is sleeping\n");
        } else {
            // secondActor's slime is not faint
            if (secondActorAction.u.skill == Skill_T::Paint) {
                // secondActor's slime uses Paint
                // judge whether the skill is successful
                if (!firstActorRet.success) {
                    SkillCbReturn secondActorRet =
                            allSkills.at(secondActorAction.u.skill).cb(this,
                                                                          this->actors_[secondActor],
                                                                          &allSkills.at(secondActorAction.u.skill));
                    LOG(this->Whose(this->actors_[secondActor]->GetName()),
                        this->actors_[secondActor]->slimeOnCourt->slimeName, "uses",
                        allSkills.at(secondActorAction.u.skill).skillName + '!',
                        secondActorRet.prompt + '\n');
                } else {
                    LOG(this->Whose(this->actors_[secondActor]->GetName()),
                        this->actors_[secondActor]->slimeOnCourt->slimeName, "uses",
                        allSkills.at(secondActorAction.u.skill).skillName + '!', "Failure\n");
                }
            } else {
                SkillCbReturn secondActorRet =
                        allSkills.at(secondActorAction.u.skill).cb(this,
                                                                      this->actors_[secondActor],
                                                                      &allSkills.at(secondActorAction.u.skill));
                this->actors_[firstActor]->slimeOnCourt->health =
                        std::max(this->actors_[firstActor]->slimeOnCourt->health - secondActorRet.damage, 0);

                LOG(this->Whose(this->actors_[secondActor]->GetName()),
                    this->actors_[secondActor]->slimeOnCourt->slimeName, "uses",
                    allSkills.at(secondActorAction.u.skill).skillName + '!',
                    secondActorRet.prompt + '\n');
            }
        }
    }
}

void BattleField::PerformActionPhase_(std::vector<ActionInfo> &actions) {
    ActionInfo playerAction = actions[0], enemyAction = actions[1];

    if (playerAction.priority > enemyAction.priority) {
        switch (playerAction.action) {
            case (Action_T::Change):
                LOG("You send", this->actors_[0]->slimeOnCourt->slimeName + '\n');
                break;
            default:
                break;
        }

        switch (enemyAction.action) {
            case (Action_T::Potion):
                this->actors_[1]->attr["revivalPotion"]--;
                this->actors_[1]->availSlimes[Slime_T::B] = this->actors_[1]->grave[Slime_T::B];
                this->actors_[1]->grave.erase(Slime_T::B);
                this->actors_[1]->availSlimes[Slime_T::B].health = int(round(double(allSlimes.at(Slime_T::B).health) / 2.));
                LOG("Enemy uses Revival Potion on Blue\n");
                break;
            case (Action_T::Skill):
                // whether is faint
                if (this->actors_[1]->slimeOnCourt->attr["faint"]) {
					LOG(Whose("Enemy"), this->actors_[1]->slimeOnCourt->slimeName, "is sleeping\n");
                } else {
                    SkillCbReturn ret =
                            allSkills.at(enemyAction.u.skill).cb(this,
                                                                    this->actors_[1],
                                                                    &allSkills.at(enemyAction.u.skill));
                    this->actors_[0]->slimeOnCourt->health =
                        std::max(this->actors_[0]->slimeOnCourt->health - ret.damage, 0);
                    LOG(Whose("Enemy"), this->actors_[1]->slimeOnCourt->slimeName, "uses",
                        allSkills.at(enemyAction.u.skill).skillName + '!', ret.prompt + '\n');
                }

                break;
            default:
                break;
        }
    } else if (playerAction.priority == enemyAction.priority) {
        if (playerAction.action == Action_T::Change) {
            LOG("You send", this->actors_[0]->slimeOnCourt->slimeName + '\n');
            LOG("Enemy sends", this->actors_[1]->slimeOnCourt->slimeName + '\n');
        } else if (playerAction.action == Action_T::Skill) {
            if (allSkills.at(playerAction.u.skill).priority > allSkills.at(playerAction.u.skill).priority) {
                // the priority of player's skill is higher
                this->attackEachOther(0, 1, actions);
            } else if (allSkills.at(playerAction.u.skill).priority < allSkills.at(enemyAction.u.skill).priority) {
                // the priority of enemy's skill is higher
                this->attackEachOther(1, 0, actions);
            } else {
                // the priority of player's skill is equal to the priority of enemy's skill
                // compare the slime's speed
                if (this->actors_[0]->slimeOnCourt->speed > this->actors_[1]->slimeOnCourt->speed) {
                    this->attackEachOther(0, 1, actions);
                } else {
                    this->attackEachOther(1, 0, actions);
                }
            }
        }
    } else {
        switch (enemyAction.action) {
            case (Action_T::Change):
                LOG("Enemy sends", this->actors_[1]->slimeOnCourt->slimeName + '\n');
                break;
            case (Action_T::Potion):
                this->actors_[1]->attr["revivalPotion"]--;
                this->actors_[1]->availSlimes[Slime_T::B] = this->actors_[1]->grave[Slime_T::B];
                this->actors_[1]->grave.erase(Slime_T::B);
                this->actors_[1]->availSlimes[Slime_T::B].health = int(round(double(allSlimes.at(Slime_T::B).health) / 2.));
                LOG("Enemy uses Revival Potion on Blue\n");
                break;
            default:
                break;
        }

        switch (playerAction.action) {
            case (Action_T::Skill):
                // whether is faint
                if (this->actors_[0]->attr["faint"]) {
                    LOG(Whose("You"), this->actors_[0]->slimeOnCourt->slimeName, "is sleeping\n");
                } else {
                    SkillCbReturn ret =
                            allSkills.at(playerAction.u.skill).cb(this,
                                                                     this->actors_[0],
                                                                     &allSkills.at(playerAction.u.skill));
                    this->actors_[1]->slimeOnCourt->health =
                            std::max(this->actors_[1]->slimeOnCourt->health - ret.damage, 0);
                    LOG(Whose("You"), this->actors_[0]->slimeOnCourt->slimeName, "uses",
                        allSkills.at(playerAction.u.skill).skillName + '!', ret.prompt + '\n');
                }

                break;
            default:
                break;
        }
    }
}

void BattleField::HandleBeatenSlimesPhase_() {
    if (this->actors_[0]->slimeOnCourt->health == 0) {
        LOG(Whose("You"), this->actors_[0]->slimeOnCourt->slimeName, "is beaten\n");

        if (this->actors_[0]->grave.size() != 2) {
            this->actors_[0]->ChooseSlime(false);
            LOG("You send", this->actors_[0]->slimeOnCourt->slimeName + '\n');
        }

        for (auto p = this->actors_[0]->availSlimes.begin(); p != this->actors_[0]->availSlimes.end(); ) {
            if (p->second.health == 0) {
                this->actors_[0]->grave.insert(*p);
                p = this->actors_[0]->availSlimes.erase(p);
            } else {
                ++p;
            }
        }
    }

    if (this->actors_[0]->grave.size() == 3) {
        this->actors_[0]->slimeOnCourt = nullptr;
    }

    if (this->actors_[1]->slimeOnCourt->health == 0) {
        LOG(Whose("Enemy"), this->actors_[1]->slimeOnCourt->slimeName, "is beaten\n");

        if (this->actors_[1]->grave.size() != 2) {
            this->actors_[1]->ChooseSlime(false);
            LOG("Enemy sends", this->actors_[1]->slimeOnCourt->slimeName + '\n');
        }

        for (auto p = this->actors_[1]->availSlimes.begin(); p != this->actors_[1]->availSlimes.end(); ) {
            if (p->second.health == 0) {
                this->actors_[1]->grave.insert(*p);
                p = this->actors_[1]->availSlimes.erase(p);
            } else {
                ++p;
            }
        }
    }

    if (this->actors_[1]->grave.size() == 3) {
        this->actors_[1]->slimeOnCourt = nullptr;
    }
}

bool BattleField::ShowInformationPhase_() {
    // update faintCnt
    if (this->actors_[0]->slimeOnCourt) {
        this->actors_[0]->slimeOnCourt->attr["faintCnt"]--;
    }
    if (this->actors_[1]->slimeOnCourt) {
        this->actors_[1]->slimeOnCourt->attr["faintCnt"]--;
    }

    // update weatherCnt
    this->attr["weatherCnt"] = (this->attr["weatherCnt"]) ? this->attr["weatherCnt"] - 1 : 0;

    if (this->actors_[0]->grave.size() == 3) {
        LOG("You lose\n");
    } else if (this->actors_[1]->grave.size() == 3) {
        LOG("You win\n");
    } else if (this->rounds_ == this->maxRounds_) {
        LOG(Whose("You"), this->actors_[0]->slimeOnCourt->slimeName + ": HP",
            this->actors_[0]->slimeOnCourt->health, "||",
            Whose("Enemy"), this->actors_[1]->slimeOnCourt->slimeName + ": HP",
            this->actors_[1]->slimeOnCourt->health, "\n");
        LOG("Draw\n");
    } else {
        LOG(Whose("You"), this->actors_[0]->slimeOnCourt->slimeName + ": HP",
            this->actors_[0]->slimeOnCourt->health, "||",
            Whose("Enemy"), this->actors_[1]->slimeOnCourt->slimeName + ": HP",
            this->actors_[1]->slimeOnCourt->health, "\n");
        return false;
    }

    return true;
}

std::string BattleField::Whose(const std::string & name) {
    return (name == "You" ? "Your" : name + "'s");
}

