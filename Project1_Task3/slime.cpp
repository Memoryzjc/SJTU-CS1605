#include <vector>
#include <string>
#include "slime.hpp"
#include "skill.hpp"

Slime::Slime(Property p, bool atkP)
    : property(p), atkPotionEffect(atkP)
{
    switch (p) {
        case Grass:
            name = "Green";
            HP = 110;
            maxHP = 110;
            attack = 10;
            defense = 10;
            speed = 10;
            skills = {Skill("Tackle"), Skill("Leaf")};
            break;
        case Fire:
            name = "Red";
            HP = 100;
            maxHP = 100;
            attack = 11;
            defense = 10;
            speed = 11;
            skills = {Skill("Tackle"), Skill("Flame")};
            break;
        case Water:
            name = "Blue";
            HP = 100;
            maxHP = 100;
            attack = 10;
            defense = 11;
            speed = 9;
            skills = {Skill("Tackle"), Skill("Stream")};
            break;
        default:
            break;
    }

}


Slime::Slime(Property p)
    : property(p)
{
    switch (p) {
        case Grass:
            name = "Green";
            HP = 110;
            maxHP = 110;
            attack = 10;
            defense = 10;
            speed = 10;
            skills = {Skill("Tackle"), Skill("Leaf")};
            break;
        case Fire:
            name = "Red";
            HP = 100;
            maxHP = 100;
            attack = 11;
            defense = 10;
            speed = 11;
            skills = {Skill("Tackle"), Skill("Flame")};
            break;
        case Water:
            name = "Blue";
            HP = 100;
            maxHP = 100;
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
    HP = std::max(0, healthPoint);
}

int Slime::getMaxHP() const
{
    return maxHP;
}

double Slime::getAttack() const
{
    return attack;
}

void Slime::setAttack(double atk)
{
    attack = atk;
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

bool Slime::getAtkPotionEffect() const
{
    return atkPotionEffect;
}

void Slime::setAtkPotionEffect(bool atkP)
{
    if (atkP && !atkPotionEffect) {
        setAttack(attack * 2.0);
    } else if (atkPotionEffect && !atkP){
        setAttack(attack / 2.0);
    }
    atkPotionEffect = atkP;
}

bool Slime::isDead() const
{
    return HP <= 0;
}