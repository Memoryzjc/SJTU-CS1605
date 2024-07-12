#include <vector>
#include <string>
#include "slime.hpp"
#include "skill.hpp"

Slime::Slime(Property p)
    : property(p)
{
    switch (p) {
        case Grass:
            name = "Green";
            HP = 110;
            attack = 10;
            defense = 10;
            speed = 10;
            skills = {Skill("Tackle"), Skill("Leaf")};
            break;
        case Fire:
            name = "Red";
            HP = 100;
            attack = 11;
            defense = 10;
            speed = 11;
            skills = {Skill("Tackle"), Skill("Flame")};
            break;
        case Water:
            name = "Blue";
            HP = 100;
            attack = 10;
            defense = 11;
            speed = 9;
            skills = {Skill("Tackle"), Skill("Stream")};
            break;
        default:
            break;
    }
}

Slime::Slime()
= default;

std::string Slime::getName() const
{
    return name;
}

Property Slime::getProperty() const
{
    return property;
}

int Slime::getHP() const
{
    return HP;
}

void Slime::setHP(int healthPoint)
{
    HP = healthPoint;
}

double Slime::getAttack() const
{
    return attack;
}

double Slime::getDefense() const
{
    return defense;
}

int Slime::getSpeed() const
{
    return speed;
}

std::vector<Skill> Slime::getSkill() const
{
    return skills;
}


bool Slime::isDead() const
{
    return HP <= 0;
}