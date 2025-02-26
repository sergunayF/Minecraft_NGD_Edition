#pragma once

#include "../Game/Game.hpp"

class Biome {
public:

    int BiomeID;
    std::string BiomeName;

    int temperature;
    int rainfall;

    Color Grass;
    Color Water;
    Color Sky;

    int topBlock;
    int underBlock;

    Biome(int BiomeID);
};
