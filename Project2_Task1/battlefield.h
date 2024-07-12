#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include "actor.h"
#include "helper.h"


class BattleField {
public:
    explicit BattleField(int maxRounds);
    void Reset();
    void Run();
    void SetActor(Actor *, Actor *);
    static std::string Whose(const std::string&); // "Your" or "Enemy's"
    DefaultedMap<std::string, int, 0> attr; // 动态属性，如天气等

private:
    void BeforeGame_();
    void StateCheck_();
    std::vector<ActionInfo> ChooseActionPhase_();
    void PerformActionPhase_(std::vector<ActionInfo> &);
    void HandleBeatenSlimesPhase_();
    bool ShowInformationPhase_();
    void attack(int, int, std::vector<ActionInfo>&);


    int rounds_{};
    int maxRounds_;
    Actor *actors_[2]{};
};

#endif //BATTLEFIELD_H
