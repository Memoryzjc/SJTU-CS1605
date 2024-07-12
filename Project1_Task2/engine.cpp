#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cassert>
#include "engine.hpp"
#include "skill.hpp"
#include "slime.hpp"


// Function to print a line
void printLine() {
    std::cout << "--------------------------------------------------" << std::endl;
}

// Function to check if the defender's slime is restrained by the attacker's slime
bool isSlimeRestrain(Slime* defenceSlime, Slime* attackSlime) {
    assert(!defenceSlime->isDead() && !attackSlime->isDead());
    if ((defenceSlime->getProperty() == Grass && attackSlime->getProperty() == Fire) ||
        (defenceSlime->getProperty() == Fire && attackSlime->getProperty() == Water) ||
        (defenceSlime->getProperty() == Water && attackSlime->getProperty() == Grass)) {
        return true;
    } else {
        return false;
    }
}

// Function to round a number(different from std::round)
int myRound(double num) {
    double result;
    result = std::round(num);
    if (num - result == 0.5) {
        return static_cast<int>(result) + 1;
    } else {
        return static_cast<int>(result);
    }
}

// Function to carry out attack and return the number of damage
int attack(Slime* attackSlime, Slime* defenceSlime,Skill & attackSkill) {
    assert(!attackSlime->isDead() && !defenceSlime->isDead());
    double rate = 1, damage;

    // Calculate the rate of the attack
    if (attackSkill.getProperty() == Grass) {
        rate = (defenceSlime->getProperty() == Water) ?
                attackSkill.getRestraintRate() : attackSkill.getRestrainedRate();
    } else if (attackSkill.getProperty() == Fire) {
        rate = (defenceSlime->getProperty() == Grass) ?
                attackSkill.getRestraintRate() : attackSkill.getRestrainedRate();
    } else if (attackSkill.getProperty() == Water) {
        rate = (defenceSlime->getProperty() == Fire) ?
                attackSkill.getRestraintRate() : attackSkill.getRestrainedRate();
    }

    // Calculate the damage
    damage = attackSkill.getDamage() * attackSlime->getAttack()
            / defenceSlime->getDefense() * rate;

    // Set the HP of the defence slime
    defenceSlime->setHP(defenceSlime->getHP() - myRound(damage));
    return myRound(damage);
}

// Function to carry out the action
void carryOutAction(Slime* playerSlime, Slime* enemySlime,
                    std::string & playerChoice, std::string & enemyChoice) {
    assert(!playerSlime->isDead() && !enemySlime->isDead());
    // Priority: Change > Skill
    if (playerChoice == "Change" && enemyChoice != "Change") {
        int damage;
        auto enemySkill = Skill(enemyChoice);

        damage = attack(enemySlime, playerSlime, enemySkill);
        std::cout << "Enemy's " << enemySlime->getName()
        << " uses " << enemyChoice << "! Damage: " << damage << std::endl;
    } else if (playerChoice != "Change" && enemyChoice != "Change") {
        int playerDamage, enemyDamage;
        auto playerSkill = Skill(playerChoice), enemySkill = Skill(enemyChoice);
        int playerSpeed = playerSlime->getSpeed(), enemySpeed = enemySlime->getSpeed();

        if (playerSpeed > enemySpeed) {
            playerDamage = attack(playerSlime, enemySlime, playerSkill);
            std::cout << "Your " << playerSlime->getName()
            << " uses " << playerChoice << "! Damage: " << playerDamage << std::endl;
            if (!enemySlime->isDead()) {
                enemyDamage = attack(enemySlime, playerSlime, enemySkill);
                std::cout << "Enemy's " << enemySlime->getName()
                << " uses " << enemyChoice << "! Damage: " << enemyDamage << std::endl;
            }
        } else {
            enemyDamage = attack(enemySlime, playerSlime, enemySkill);
            std::cout << "Enemy's " << enemySlime->getName()
            << " uses " << enemyChoice << "! Damage: " << enemyDamage << std::endl;
            if (!playerSlime->isDead()) {
                playerDamage = attack(playerSlime, enemySlime, playerSkill);
                std::cout << "Your " << playerSlime->getName()
                << " uses " << playerChoice << "! Damage: " << playerDamage << std::endl;
            }
        }
    } else if (playerChoice != "Change" && enemyChoice == "Change") {
        int damage;
        auto playerSkill = Skill(playerChoice);

        damage = attack(playerSlime, enemySlime, playerSkill);
        std::cout << "Your " << playerSlime->getName()
        << " uses " << playerChoice << "! Damage: " << damage << std::endl;
    }
}

// Function to change the player's slime
// Does not consider the situation where playerSlime is the only slime left
Slime* playerChangeSlime(std::vector<Slime*> & playerSlimes, Slime* playerSlime) {
    // Store the index of the selected slime
    int select = 0;
    // Store the slime that is selected
    Slime *selectSlime;
    // Store the index of the slime that is not selected
    std::vector<int> alternativeSlimeIndex;

    // Find the index of the slime that is not selected
    for (int i = 0; i < playerSlimes.size(); i++) {
        if (!playerSlimes[i]->isDead() && playerSlimes[i] != playerSlime) {
            alternativeSlimeIndex.push_back(i);
        }
    }

    // Select the slime
    while (true) {
        if (alternativeSlimeIndex.size() == 2) {
            std::cout << "Select your next slime (" << alternativeSlimeIndex[0] + 1
                      << " for " << playerSlimes[alternativeSlimeIndex[0]]->getName() << ", "
                      << alternativeSlimeIndex[1] + 1 << " for "
                      << playerSlimes[alternativeSlimeIndex[1]]->getName() << "): ";
            std::cin >> select;
            if (select == alternativeSlimeIndex[0] + 1 || select == alternativeSlimeIndex[1] + 1) {
                selectSlime = playerSlimes[select - 1];
                break;
            }
        } else if (alternativeSlimeIndex.size() == 1) {
            std::cout << "Select your next slime (" << alternativeSlimeIndex[0] + 1
                      << " for " << playerSlimes[alternativeSlimeIndex[0]]->getName() << "): ";
            std::cin >> select;
            if (select == alternativeSlimeIndex[0] + 1) {
                selectSlime = playerSlimes[select - 1];
                break;
            }
        }
    }

    assert(!selectSlime->isDead());
    std::cout << "You send " << playerSlimes[select - 1]->getName() << std::endl;
    return selectSlime;
}

// Function to find the attacker's slime that can restrain the defender's slime
Slime* findRestrainSlime(std::vector<Slime*> & atkSlimes, Slime* defenceSlime) {
    for (auto & slime : atkSlimes) {
        if (!slime->isDead() && isSlimeRestrain(defenceSlime, slime)) {
            return slime;
        }
    }

    return nullptr;
}


// Function to battle for one turn
std::vector<Slime*> battleStage1_2(std::vector<Slime*> & playerSlimes,
                                   std::vector<Slime*> & enemySlimes,
                                   Slime* playerSlime, Slime* enemySlime) {
    assert(!playerSlime->isDead() && !enemySlime->isDead());

    // Stage1: Select action

    // playerAction1: for skill and change(player)
    // playerAction2: for selecting the skill(player)
    // enemyAction: for selecting the skill(enemy)
    // enemy has two kinds of potion: revival potion and attack potion
    int playerAction1, playerAction2, enemyAction;
    std::string playerChoice, enemyChoice;

    int playerSlimeNum = 0, enemySlimeNum = 0;

    // Check the number of player's remaining slime
    for (auto & slime : playerSlimes) {
        if (!slime->isDead()) {
            playerSlimeNum++;
        }
    }

    // Check the number of enemy's remaining slime
    for (auto & slime : enemySlimes) {
        if (!slime->isDead()) {
            enemySlimeNum++;
        }
    }

    // Select enemy's action: attack(0) or change(6) or use potion(5)
    if (enemySlimeNum == 1) {
        enemyAction = (isSlimeRestrain(playerSlime, enemySlime)) ? 2 : 1;
        enemyChoice = enemySlime->getSkill()[enemyAction - 1].getName();
    } else {
        if (findRestrainSlime(enemySlimes, playerSlime)
            && enemySlime != findRestrainSlime(enemySlimes, playerSlime)) {
            enemySlime = findRestrainSlime(enemySlimes, playerSlime);
            enemyChoice = "Change";
        } else if (isSlimeRestrain(enemySlime, playerSlime)) {
            for (auto & slime : enemySlimes) {
                if (!slime->isDead() && slime->getName() == playerSlime->getName()) {
                    enemySlime = slime;
                    break;
                }
            }

            enemyChoice = "Change";
        } else {
            enemyAction = (isSlimeRestrain(playerSlime, enemySlime)) ? 2 : 1;
            enemyChoice = enemySlime->getSkill()[enemyAction - 1].getName();
        }
    }

    // Select player's action
    while (true) {

        if (playerSlimeNum == 1) {
            std::cout << "Select your action (1 for skill): ";
            std::cin >> playerAction1;

            if (playerAction1 == 1) {
                break;
            }
        } else {
            std::cout << "Select your action (1 for skill, 2 for change): ";
            std::cin >> playerAction1;

            if (playerAction1 == 1 || playerAction1 == 2) {
                break;
            }
        }
    }

    switch (playerAction1) {
        case 1:
            // Select the skill
            while (true) {
                std::cout << "Select the skill (1 for Tackle, 2 for "
                          << playerSlime->getSkill()[1].getName() << "): ";
                std::cin >> playerAction2;

                if (playerAction2 == 1 || playerAction2 == 2) {
                    playerChoice = playerSlime->getSkill()[playerAction2 - 1].getName();
                    break;
                }
            }
            break;
        case 2:
            // Change the player's slime
            playerSlime = playerChangeSlime(playerSlimes, playerSlime);
            playerChoice = "Change";
            break;
        default:
            break;
    }
    // Player's action ends

    if (enemyChoice == "Change") {
        std::cout << "Enemy sends " << enemySlime->getName() << std::endl;
    }

    // Stage2: Carry out action
    carryOutAction(playerSlime, enemySlime, playerChoice, enemyChoice);

    return {playerSlime, enemySlime};
}

// Function to judge whether game is end
// return 0: not end
// return 1: player win
// return 2: player lose
int isEnd(std::vector<Slime*> & playerSlimes, std::vector<Slime*> & enemySlimes) {
    int flag1 = 0, flag2 = 0, result = 0;

    // Check whether all player's slimes are dead
    for (auto & slime : playerSlimes) {
        if (!slime->isDead()) {
            flag1 = 1;
            break;
        }
    }

    // Check whether all enemy's slimes are dead
    for (auto & slime : enemySlimes) {
        if (!slime->isDead()) {
            flag2 = 1;
            break;
        }
    }
    // flag1/flag2 == 0: all dead

    if (flag1 && !flag2) {
        result = 1;
    } else if (!flag1 && flag2) {
        result = 2;
    }

    return result;
}

// Function to play the game
void play(std::vector<Slime*> & playerSlimes, std::vector<Slime*> & enemySlimes) {
    std::cout << "Welcome to Battle of Slimes!" << std::endl
    << "You have Green, Red and Blue. So does Enemy." << std::endl;

    int playerSlimeChoice = 0;
    Slime *playerBattleSlime, *enemyBattleSlime;

    // Player select the slime
    while (true) {
        std::cout << "Select your starting slime (1 for Green, 2 for Red, 3 for Blue): ";
        std::cin >> playerSlimeChoice;


        bool valid = true;

        switch (playerSlimeChoice) {
            case 1:
                playerBattleSlime = playerSlimes[0];
                enemyBattleSlime = enemySlimes[1];
                break;
            case 2:
                playerBattleSlime = playerSlimes[1];
                enemyBattleSlime = enemySlimes[2];
                break;
            case 3:
                playerBattleSlime = playerSlimes[2];
                enemyBattleSlime = enemySlimes[0];
                break;
            default:
                valid = false;
                break;
        }

        if (valid) {
            break;
        }
    }

    std::cout << "You start with " << playerBattleSlime->getName() << std::endl
    << "Enemy starts with " << enemyBattleSlime->getName() << std::endl;

    std::cout << "Your " << playerBattleSlime->getName() << ": HP " << playerBattleSlime->getHP()
    << " || " << "Enemy's " << enemyBattleSlime->getName() << ": HP " << enemyBattleSlime->getHP()
    << std::endl << "Battle starts!" << std::endl;
    printLine();

    // Battle begins
    int round = 1;
    std::vector<Slime*> battleSlimes;
    while (true) {
        std::cout << "Round " << round << std::endl;
        assert(playerBattleSlime && enemyBattleSlime);
        // Stage 1 & 2
        battleSlimes = battleStage1_2(playerSlimes, enemySlimes, playerBattleSlime, enemyBattleSlime);
        playerBattleSlime = battleSlimes[0];
        enemyBattleSlime = battleSlimes[1];


        // Stage 3 & 4
        assert(playerBattleSlime && enemyBattleSlime);
        // Check if the player's battling slime is dead
        if (playerBattleSlime->isDead()) {
            std::cout << "Your " << playerBattleSlime->getName() << " is beaten" << std::endl;
            if (isEnd(playerSlimes, enemySlimes) == 2) {
                std::cout << "You Lose" << std::endl;
                break;
            } else {
                playerBattleSlime = playerChangeSlime(playerSlimes, playerBattleSlime);
            }
        }

        // Check if the enemy's battling slime is dead
        if (enemyBattleSlime->isDead()) {
            std::cout << "Enemy's " << enemyBattleSlime->getName() << " is beaten" << std::endl;

            if (isEnd(playerSlimes, enemySlimes) == 1) {
                std::cout << "You Win" << std::endl;
                break;
            } else {
                // Select the next enemy's battling slime
                if (findRestrainSlime(enemySlimes, playerBattleSlime)) {
                    enemyBattleSlime = findRestrainSlime(enemySlimes, playerBattleSlime);
                } else {
                    for (auto & slime : enemySlimes) {
                        if (!slime->isDead() && slime->getName() == playerBattleSlime->getName()) {
                            enemyBattleSlime = slime;
                            break;
                        }
                    }

                    if (enemyBattleSlime->isDead()) {
                        for (auto & slime : enemySlimes) {
                            if (!slime->isDead()) {
                                enemyBattleSlime = slime;
                                break;
                            }
                        }
                    }
                }
                assert(!enemyBattleSlime->isDead());
                std::cout << "Enemy sends " << enemyBattleSlime->getName() << std::endl;
            }
        }


        assert(!playerBattleSlime->isDead() && !enemyBattleSlime->isDead());
        std::cout << "Your " << playerBattleSlime->getName() << ": HP "
                  << playerBattleSlime->getHP() << " || "
                  << "Enemy's " << enemyBattleSlime->getName() << ": HP "
                  << enemyBattleSlime->getHP() << std::endl;

        if (round == 100) {
            std::cout << "Draw" << std::endl;
            break;
        }
        
        printLine();
        round++;
    }

}