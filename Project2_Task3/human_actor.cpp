#include "actor.h"

#include <sstream>
#include <unordered_set>


HumanActor::HumanActor()
= default;

std::vector<Slime_T> HumanActor::ChooseStartingSlime() {
    int slimeIndex;
    std::vector<std::string> outputIndex = {"1st", "2nd", "3rd"};
    std::vector<int> slimeIndexValue;

    // select human player's 3 slimes
    for (int i = 0; i < 3; ++i) {
        while (true) {
            LOG("Select your", outputIndex[i], "slime: ");
            std::cin >> slimeIndex;
            if (slimeIndex >= 1 && slimeIndex <= 5 &&
                std::find(slimeIndexValue.begin(), slimeIndexValue.end(), slimeIndex) == slimeIndexValue.end()) {
                slimeIndexValue.push_back(slimeIndex);
                this->allSlimeTs.push_back(static_cast<Slime_T>(slimeIndex));
                break;
            }
        }
    }

    // set the available slimes
    for (auto slime_T : this->allSlimeTs) {
        this->availSlimes[slime_T] = allSlimes.at(slime_T);
    }

    // set the first slime on court
    this->slimeOnCourt = &this->availSlimes[this->allSlimeTs[0]];

    return this->allSlimeTs;
}

Action_T HumanActor::ChooseAction() {
    int action;
    if (this->grave.size() != 2) {
        while (true) {
            LOG("Select your action (0 for escape, 1 for skill, 2 for change): ");
            std::cin >> action;
            if (action >= 0 && action <= 2) {
                return static_cast<Action_T>(action);
            }
        }
    } else {
        while(true) {
            LOG("Select your action (0 for escape, 1 for skill): ");
            std::cin >> action;
            if (action >= 0 && action <= 1) {
                return static_cast<Action_T>(action);
            }
        }
    }
}

std::pair<bool, Slime_T> HumanActor::ChooseSlime(bool active) {
    // active: true for active change, false for passive change
    int select;
    std::vector<int> alterSlimeIndex;
    std::pair<bool, Slime_T> res;
    res.first = true;

    for (int i = 0; i < this->allSlimeTs.size(); i++) {
    if (this->allSlimeTs[i] != this->slimeOnCourt->slime &&
        this->availSlimes.find(this->allSlimeTs[i]) != this->availSlimes.end()) {
        alterSlimeIndex.push_back(i);
    }
}

    if (active) {
        while (true) {
            if (alterSlimeIndex.size() == 2) {
                LOG("Select your next slime (0 for back,", alterSlimeIndex[0] + 1, "for",
                    allSlimes.at(this->allSlimeTs[alterSlimeIndex[0]]).slimeName + "-HP");
                LOG(this->availSlimes.at(this->allSlimeTs[alterSlimeIndex[0]]).health);
                LOG(",", alterSlimeIndex[1] + 1, "for", allSlimes.at(this->allSlimeTs[alterSlimeIndex[1]]).slimeName + "-HP");
                LOG(this->availSlimes.at(this->allSlimeTs[alterSlimeIndex[1]]).health); LOG("): ");

                std::cin >> select;
                if (select == alterSlimeIndex[0] + 1 || select == alterSlimeIndex[1] + 1) {
                    this->slimeOnCourt = &this->availSlimes.at(this->allSlimeTs[select - 1]);
                    res.second = this->allSlimeTs[select - 1];
                    break;
                } else if (!select) {
                    // choose back
                    res.first = false;
                    break;
                } else {
                    continue;
                }
            } else if (alterSlimeIndex.size() == 1) {
                LOG("Select your next slime (0 for back,", alterSlimeIndex[0] + 1,
                    "for", allSlimes.at(this->allSlimeTs[alterSlimeIndex[0]]).slimeName + "-HP");
                LOG(this->availSlimes.at(this->allSlimeTs[alterSlimeIndex[0]]).health); LOG("): ");

                std::cin >> select;
                if (select == alterSlimeIndex[0] + 1) {
                    this->slimeOnCourt = &this->availSlimes.at(this->allSlimeTs[select - 1]);
                    res.second = this->allSlimeTs[select - 1];
                    break;
                } else if (!select) {
                    res.first = false;
                    break;
                } else {
                    continue;
                }
            }
        }
    } else {
        while (true) {
            if (this->grave.empty()) {
                LOG("Select your next slime (");
                LOG(alterSlimeIndex[0] + 1, "for", allSlimes.at(this->allSlimeTs[alterSlimeIndex[0]]).slimeName + "-HP");
                LOG(this->availSlimes.at(this->allSlimeTs[alterSlimeIndex[0]]).health);
                LOG(',', alterSlimeIndex[1] + 1, "for", allSlimes.at(this->allSlimeTs[alterSlimeIndex[1]]).slimeName + "-HP");
                LOG(this->availSlimes.at(this->allSlimeTs[alterSlimeIndex[1]]).health); LOG( "): ");

                std::cin >> select;
                if (select == alterSlimeIndex[0] + 1 || select == alterSlimeIndex[1] + 1) {
                    this->slimeOnCourt = &this->availSlimes.at(this->allSlimeTs[select - 1]);
                    res.second = this->allSlimeTs[select - 1];
                    break;
                } else {
                    continue;
                }
            } else if (this->grave.size() == 1) {
                LOG("Select your next slime (");
                LOG(alterSlimeIndex[0] + 1, "for", allSlimes.at(this->allSlimeTs[alterSlimeIndex[0]]).slimeName + "-HP");
                LOG(this->availSlimes.at(this->allSlimeTs[alterSlimeIndex[0]]).health); LOG("): ");

                std::cin >> select;
                if (select == alterSlimeIndex[0] + 1) {
                    this->slimeOnCourt = &this->availSlimes.at(this->allSlimeTs[select - 1]);
                    res.second = this->allSlimeTs[select - 1];
                    break;
                } else {
                    continue;
                }
            }
        }
    }

    return res;
}

std::pair<bool, Skill_T> HumanActor::ChooseSkill() {
    int select;
    std::pair<bool, Skill_T> res;
    res.first = true;
    while (true) {
        LOG("Select the skill (0 for back, 1 for",
            allSkills.at(allSlimes.at(this->slimeOnCourt->slime).skills[0]).skillName);
        LOG(", 2 for", allSkills.at(allSlimes.at(this->slimeOnCourt->slime).skills[1]).skillName);
        LOG(", 3 for", allSkills.at(allSlimes.at(this->slimeOnCourt->slime).skills[2]).skillName + "): ");
        std::cin >> select;

        if (!select) {
            // choose back
            res.first = false;
            break;
        } else if (select >= 1 && select <= 3) {
            res.second = allSlimes.at(this->slimeOnCourt->slime).skills[select - 1];
            break;
        } else {
            continue;
        }
    }
    
    return res;
}

std::string HumanActor::GetName() {
    return "You";
}
