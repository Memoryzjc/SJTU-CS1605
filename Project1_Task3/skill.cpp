#include "skill.hpp"
#include <string>

Skill::Skill(std::string & n)
        : damage(20), restraintRate(2.0), restrainedRate(0.5)
{
    if (n == "Tackle") {
        name = "Tackle";
        property = Normal;
    } else if (n == "Leaf") {
        name = "Leaf";
        property = Grass;
    } else if (n == "Flame") {
        name = "Flame";
        property = Fire;
    } else if (n == "Stream") {
        name = "Stream";
        property = Water;
    }
}

Skill::Skill(const char n[])
        : damage(20), restraintRate(2.0), restrainedRate(0.5)
{
    std::string name1(n);
    if (name1 == "Tackle") {
        name = "Tackle";
        property = Normal;
    } else if (name1 == "Leaf") {
        name = "Leaf";
        property = Grass;
    } else if (name1 == "Flame") {
        name = "Flame";
        property = Fire;
    } else if (name1 == "Stream") {
        name = "Stream";
        property = Water;
    }
}

Skill::Skill()
= default;

std::string Skill::getName() const
{
    return name;
}

Property Skill::getProperty() const
{
    return property;
}

double Skill::getDamage() const
{
    return damage;
}

double Skill::getRestraintRate() const
{
    return restraintRate;
}

double Skill::getRestrainedRate() const
{
    return restrainedRate;
}