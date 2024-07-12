#ifndef PROJECT1_SLIME_HPP
#define PROJECT1_SLIME_HPP

#include <vector>
#include <string>
#include "skill.hpp"

class Slime {
public:
    // Constructor
    Slime (Property p, bool atkPotion);
    explicit Slime(Property p);
    Slime();

    // Get name
    [[nodiscard]] std::string getName() const;

    // Get property
    [[nodiscard]] Property getProperty() const;

    // Get and Set HP
    [[nodiscard]] int getHP() const;
    void setHP(int healthPoint);

    // Get max HP
    [[nodiscard]] int getMaxHP() const;

    // Get and Set attack
    [[nodiscard]] double getAttack() const;
    void setAttack(double atk);

    // Get defense
    [[nodiscard]] double getDefense() const;

    // Get speed
    [[nodiscard]] int getSpeed() const;

    // Get skill
    [[nodiscard]] std::vector<Skill> getSkill() const;

    // Get and set attack potion effect
    [[nodiscard]] bool getAtkPotionEffect() const;
    void setAtkPotionEffect(bool atkPotion);

    // Check if the slime is dead
    [[nodiscard]] bool isDead() const;

private:
    std::string name;
    Property property{};
    int HP{};
    int maxHP{};
    double attack{};
    double defense{};
    int speed{};
    std::vector<Skill> skills;
    bool atkPotionEffect = false;
};


#endif //PROJECT1_SLIME_HPP
