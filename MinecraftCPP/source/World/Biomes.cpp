#include "Biomes.hpp"

Biome::Biome(int BiomeID) {
    this->BiomeID = BiomeID;

    switch (BiomeID) {
    case 1:
        BiomeName = "Desert";
        temperature = 40;
        rainfall = 5;
        Grass = { 237, 201, 175 };
        Water = { 255, 217, 102 };
        Sky = { 255, 204, 153 };
        topBlock = 12;
        underBlock = 12;
        break;

    case 2:
        BiomeName = "Forest";
        temperature = 20;
        rainfall = 70;
        Grass = { 34, 139, 34 };
        Water = { 63, 118, 228 };
        Sky = { 135, 206, 235 };
        topBlock = 2;
        underBlock = 3;
        break;

    case 3:
        BiomeName = "Mountains";
        temperature = 5;
        rainfall = 50;
        Grass = { 100, 100, 100 };
        Water = { 63, 118, 228 };
        Sky = { 200, 200, 255 };
        topBlock = 1;
        underBlock = 1;
        break;

    default:
        BiomeName = "Plains";
        temperature = 25;
        rainfall = 50;
        Grass = { 50, 205, 50 };
        Water = { 63, 118, 228 };
        Sky = { 135, 206, 250 };
        topBlock = 2;
        underBlock = 3;
        break;
    }
}
