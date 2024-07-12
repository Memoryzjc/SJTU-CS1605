#ifndef PROJECT1_SLIME_HPP
#define PROJECT1_SLIME_HPP

#include <vector>
#include <string>
#include "skill.hpp"

class Slime {
public:
    // Constructor
    explicit Slime(Property p);
    Slime();

    // Get name
    [[nodiscard]] std::string getName() const;

    // Get property
    [[nodiscard]] Property getProperty() const;

    // Get and Set HP
    [[nodiscard]] int getHP() const;
    void setHP(int healthPoint);

    // Get and Set attack
    [[nodiscard]] double getAttack() const;

    // Get defense
    [[nodiscard]] double getDefense() const;

    // Get speed
    [[nodiscard]] int getSpeed() const;

    // Get skill
    [[nodiscard]] std::vector<Skill> getSkill() const;

    // Check if the slime is dead
    [[nodiscard]] bool isDead() const;

private:
    std::string name;
    Property property{};
    int HP{};
    double attack{};
    double defense{};
    int speed{};
    std::vector<Skill> skills;
};


#endif //PROJECT1_SLIME_HPP
