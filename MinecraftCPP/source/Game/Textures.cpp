#include "Textures.hpp"

Texture texturesArray[ROWS][COLS];

void loadTextures() {

    texturesArray[0][0] = LoadTexture(BLOCKS_PATH "air.png");

    texturesArray[1][0] = LoadTexture(BLOCKS_PATH "stone.png");

    texturesArray[2][0] = LoadTexture(BLOCKS_PATH "grass_block.png");

    texturesArray[3][0] = LoadTexture(BLOCKS_PATH "dirt.png");
    texturesArray[3][1] = LoadTexture(BLOCKS_PATH "Makovka.png");

    texturesArray[4][0] = LoadTexture(BLOCKS_PATH "cobblestone.png");

    texturesArray[5][0] = LoadTexture(BLOCKS_PATH "planks.png");
}

void unloadTextures() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            UnloadTexture(texturesArray[i][j]);
        }
    }
}
