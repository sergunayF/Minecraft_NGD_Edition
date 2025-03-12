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

    case 4:
        BiomeName = "Tundra";
        temperature = -10;
        rainfall = 20;
        Grass = { 200, 200, 200 };
        Water = { 100, 150, 200 };
        Sky = { 150, 200, 255 };
        topBlock = 80;
        underBlock = 3;
        break;

    case 5:
        BiomeName = "Taiga";
        temperature = 0;
        rainfall = 60;
        Grass = { 50, 100, 50 };
        Water = { 63, 118, 228 };
        Sky = { 135, 206, 250 };
        topBlock = 2;
        underBlock = 3;
        break;

    case 6:
        BiomeName = "Hills";
        temperature = 10;
        rainfall = 50;
        Grass = { 100, 150, 100 };
        Water = { 63, 118, 228 };
        Sky = { 135, 206, 250 };
        topBlock = 2;
        underBlock = 1;
        break;

    case 7:
        BiomeName = "Savanna";
        temperature = 30;
        rainfall = 30;
        Grass = { 150, 120, 50 };
        Water = { 255, 217, 102 };
        Sky = { 255, 204, 153 };
        topBlock = 12;
        underBlock = 12;
        break;

    case 8:
        BiomeName = "Snowy Tundra";
        temperature = -20;
        rainfall = 10;
        Grass = { 255, 255, 255 };
        Water = { 200, 200, 255 };
        Sky = { 200, 200, 255 };
        topBlock = 80;
        underBlock = 80;
        break;

    }
}
