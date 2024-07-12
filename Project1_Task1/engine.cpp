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

// Function to check if the player's slime is restrained by the enemy's slime
bool isSlimeRestrain(Slime* defenceSlime, Slime* attackSlime) {
    assert(defenceSlime && attackSlime);
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
    assert(attackSlime && defenceSlime);
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
    assert(playerSlime && enemySlime);
    // Priority: Change > Skill
    if (playerChoice == "Change") {
        int damage;
        auto enemySkill = Skill(enemyChoice);

        damage = attack(enemySlime, playerSlime, enemySkill);
        std::cout << "Enemy's " << enemySlime->getName()
        << " uses " << enemyChoice << "! Damage: " << damage << std::endl;
    } else {
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
    }
}

// Function to change the player's slime
Slime* playerChangeSlime(std::vector<Slime*> & playerSlimes, Slime* playerSlime) {
    // Store the index of the selected slime
    int select = 0;
    // Store the slime that is selected
    Slime *selectSlime;
    // Store the index of the slime that is not selected
    std::vector<int> alternativeSlimeIndex;

    // Find the index of the slime that is not selected
    for (int i = 0; i < playerSlimes.size(); i++) {
        if (playerSlimes[i] && playerSlimes[i] != playerSlime) {
            alternativeSlimeIndex.push_back(i);
        }
    }

    if (alternativeSlimeIndex.empty()) {
        return nullptr;
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

    std::cout << "You send " << playerSlimes[select - 1]->getName() << std::endl;
    return selectSlime;
}

// Function to battle for one turn
std::vector<Slime*> battle(std::vector<Slime*> & playerSlimes,
            Slime* playerSlime, Slime* enemySlime) {
    assert(playerSlime && enemySlime);
    // Stage1: Select action

    // playerAction1: for skill and change(player)
    // playerAction2: for selecting the skill(player)
    // enemyAction: for selecting the skill(enemy)
    int playerAction1, playerAction2, enemyAction;
    std::string playerChoice, enemyChoice;

    int playerSlimeNum = 0;

    // Check the number of player's remaining slime
    for (auto & slime : playerSlimes) {
        if (slime) {
            playerSlimeNum++;
        }
    }

    // Select enemy's action: Only attack
    enemyAction = (isSlimeRestrain(playerSlime, enemySlime)) ? 2 : 1;
    enemyChoice = enemySlime->getSkill()[enemyAction - 1].getName();

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
        if (slime) {
            flag1 = 1;
            break;
        }
    }

    // Check whether all enemy's slimes are dead
    for (auto & slime : enemySlimes) {
        if (slime) {
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

    int playerSlime = 0;
    Slime *playerBattleSlime, *enemyBattleSlime;

    // Player select the slime
    while (true) {
        std::cout << "Select your starting slime (1 for Green, 2 for Red, 3 for Blue): ";
        std::cin >> playerSlime;


        bool valid = true;

        switch (playerSlime) {
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

    assert(playerBattleSlime && enemyBattleSlime);
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
        battleSlimes = battle(playerSlimes, playerBattleSlime, enemyBattleSlime);
        playerBattleSlime = battleSlimes[0];
        enemyBattleSlime = battleSlimes[1];

        assert(playerBattleSlime && enemyBattleSlime);
        if (playerBattleSlime->isDead()) {
            std::cout << "Your " << playerBattleSlime->getName() << " is beaten" << std::endl;
            for (auto & slime : playerSlimes) {
                if (slime == playerBattleSlime) {
                    delete slime;
                    slime = nullptr;
                }
            }

            playerBattleSlime = nullptr;
            if (isEnd(playerSlimes, enemySlimes) == 2) {
                std::cout << "You Lose" << std::endl;
                break;
            } else {
                playerBattleSlime = playerChangeSlime(playerSlimes, playerBattleSlime);
            }
        } else if (enemyBattleSlime->isDead()) {
            std::cout << "Enemy's " << enemyBattleSlime->getName() << " is beaten" << std::endl;
            for (auto & slime : enemySlimes) {
                if (slime == enemyBattleSlime) {
                    delete slime;
                    slime = nullptr;
                }
            }

            enemyBattleSlime = nullptr;

            if (isEnd(playerSlimes, enemySlimes) == 1) {
                std::cout << "You Win" << std::endl;
                break;
            } else {
                for (auto & slime : enemySlimes) {
                    if (slime && isSlimeRestrain(playerBattleSlime, slime)) {
                        enemyBattleSlime = slime;
                    }
                }

                if (!enemyBattleSlime) {
                    Slime *sameSlime, *notSameSlime;
                    for (auto & slime : enemySlimes) {
                        if (slime && playerBattleSlime->getName() == slime->getName()) {
                            sameSlime = slime;
                        } else if (slime && playerBattleSlime->getName() != slime->getName()) {
                            notSameSlime = slime;
                        }
                    }

                    enemyBattleSlime = (sameSlime) ? sameSlime : notSameSlime;
                }

                assert(enemyBattleSlime);
                std::cout << "Enemy sends " << enemyBattleSlime->getName() << std::endl;

            }
        }


        assert(playerBattleSlime && enemyBattleSlime);
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