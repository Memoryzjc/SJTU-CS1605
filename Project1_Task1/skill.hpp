#ifndef PROJECT1_SKILL_HPP
#define PROJECT1_SKILL_HPP

#include <string>

enum Property {
    Normal,
    Grass,
    Fire,
    Water
};

class Skill {
public:
    // Constructor
    explicit Skill(std::string & name);
    explicit Skill(const char name[]);
    Skill();

    // Get name
    [[nodiscard]] std::string getName() const;

    // Get property
    [[nodiscard]] Property getProperty() const;

    // Get damage
    [[nodiscard]] double getDamage() const;

    // Get restraint rate
    [[nodiscard]] double getRestraintRate() const;

    // Get restrained rate
    [[nodiscard]] double getRestrainedRate() const;
private:
    std::string name;
    Property property{};
    double damage{};
    double restraintRate{};
    double restrainedRate{};
};


#endif //PROJECT1_SKILL_HPP