#include "graph.h"

GraphSlime::GraphSlime(Actor* left, Actor* right) {
    easy2d::Window::setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    auto background = easy2d::gcnew easy2d::Sprite("Res\\Background.png");
    background->setPos(0, 0);
    this->player = left;
    this->enemy = right;

    this->battle_scene = new easy2d::Scene();
    this->battle_scene->addChild(background);
    this->player_slime = SetSlimeOnPitch(true, SLIME_X_PLAYER, SLIME_Y);
    this->enemy_slime = SetSlimeOnPitch(false, SLIME_X_ENEMY, SLIME_Y);
    
    easy2d::Timer::add([=]() {
        easy2d::Game::quit();  // 将控制权退出图形窗口，返回控制台（图形窗口仍存在）
        });

    easy2d::SceneManager::enter(battle_scene);
    easy2d::Game::start();
}

void GraphSlime::Update() {
    if (this->player->grave.size() == 3 || this->enemy->grave.size() == 3) {
        easy2d::Game::quit();
        return;
	}

    battle_scene->removeAllChildren();
    auto background = easy2d::gcnew easy2d::Sprite("Res\\Background.png");

    this->battle_scene->addChild(background);
    this->player_slime = SetSlimeOnPitch(true, SLIME_X_PLAYER, SLIME_Y);
    this->enemy_slime = SetSlimeOnPitch(false, SLIME_X_ENEMY, SLIME_Y);

    easy2d::Timer::add([=]() {
        easy2d::Game::quit();  // 将控制权退出图形窗口，返回控制台（图形窗口仍存在）
    });

    easy2d::SceneManager::enter(battle_scene);
    easy2d::Game::start();
}

easy2d::Sprite* GraphSlime::SetSlimeOnPitch(bool PorE, float x, float y) {  // 参数PorE表示玩家或敌方
    Actor* actor = (PorE) ? this->player : this->enemy;
    auto slimeImg = easy2d::gcnew easy2d::Sprite(GetSlimeImg(actor->slimeOnCourt, PorE));
    
    slimeImg->setPos(x, y);
    slimeImg->setScale(float(0.3));

    this->battle_scene->addChild(slimeImg);

    float HPcoorX = x + float(80.5), iconCoorX = x + float(10.);
    this->SetHPBar(slimeImg, actor->slimeOnCourt->health, allSlimes.at(actor->slimeOnCourt->slime).health, HPcoorX, y + float(200.));
    this->SetIcons(slimeImg, actor->slimeOnCourt->attr, iconCoorX, y + 90);

    return slimeImg;
}

void GraphSlime::SetHPBar(easy2d::Sprite* slimeImg, int hp, int hpMax, float x, float y) {
    auto hpBar = easy2d::gcnew easy2d::Sprite("Res\\Menu.png");
    auto hpText = easy2d::gcnew easy2d::Text(std::to_string(hp) + "/" + std::to_string(hpMax));

    hpBar->setPos(x, y);
    hpBar->setOpacity(float(.6));
    //hpBar->setScale(.8);

    easy2d::Font font = easy2d::Font();
    font.size = 26;
    font.weight = easy2d::Font::Weight::Bold;

    easy2d::TextStyle style;
    style.alignment = easy2d::TextAlign::Center;
    style.font = font;

    hpText->setTextStyle(style);
    hpText->setAnchor(0.5, 0.5);
    hpText->setPos(x + float(63.5), y + float(24));

    this->battle_scene->addChild(hpBar);
    this->battle_scene->addChild(hpText);
}

void GraphSlime::SetIcons(easy2d::Sprite* slimeImg, DefaultedMap<std::string, int, 0>& attr, float x, float y) {

    auto paintIcon = easy2d::gcnew easy2d::Sprite("Res\\IconPaint.png"),
        faintIcon = easy2d::gcnew easy2d::Sprite("Res\\IconSleep.png");

    //paintIcon->setScale(3.);
    //faintIcon->setScale(3.);

    if (attr["paint"] && attr["faint"]) {
        faintIcon->setPos(x, y);
        paintIcon->setPos(x, y + 32);
        this->battle_scene->addChild(faintIcon);
        this->battle_scene->addChild(paintIcon);
    }
    else if (attr["paint"]) {
        paintIcon->setPos(x, y);
        this->battle_scene->addChild(paintIcon);
    }
    else if (attr["faint"]) {
        faintIcon->setPos(x, y);
        this->battle_scene->addChild(faintIcon);
    }
}

std::string GraphSlime::GetSlimeImg(const Slime* slime, bool PorE) {
    std::string imgPath = "Res\\";
    
    switch (slime->slime) {
    case Slime_T::G:
        imgPath += (PorE) ? "SlimeGrass" : "SlimeGrass_r";
        break;
    case Slime_T::R:
        imgPath += (PorE) ? "SlimeFire" : "SlimeFire_r";
        break;
    case Slime_T::B:
        imgPath += (PorE) ? "SlimeWater" : "SlimeWater_r";
        break;
    case Slime_T::Y:
        imgPath += (PorE) ? "SlimeThunder" : "SlimeThunder_r";
        break;
    default:
        imgPath += (PorE) ? "SlimeNormal" : "SlimeNormal_r";
        break;
    }

    return imgPath + ".png";
}

GraphSlime::~GraphSlime() {
    delete this->battle_scene;
}
