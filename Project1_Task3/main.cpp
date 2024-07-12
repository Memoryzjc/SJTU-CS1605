#include <vector>
#include <cassert>
#include "engine.hpp"
#include "slime.hpp"

int main() {
    auto *red = new Slime(Fire),
         *blue = new Slime(Water),
         *green = new Slime(Grass),
         *red1 = new Slime(Fire),
         *blue1 = new Slime(Water),
         *green1 = new Slime(Grass);

    assert(red && blue && green && red1 && blue1 && green1);

    std::vector<Slime*> playerSlime = {green, red, blue}, enemySlime = {green1, red1, blue1};

    play(playerSlime, enemySlime);

    for (auto & slime : playerSlime) {
        delete slime;
    }

    for (auto & slime : enemySlime) {
        delete slime;
    }
    return 0;
}
