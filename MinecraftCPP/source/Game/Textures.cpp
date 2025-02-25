#include "Textures.hpp"

Texture texturesArray[ROWS][COLS];
Image imagesArray[2][2];

void loadTextures() {

    texturesArray[0][0] = LoadTexture(BLOCKS_PATH "air.png");

    texturesArray[1][0] = LoadTexture(BLOCKS_PATH "stone.png");

    imagesArray[0][0] = LoadImage(BLOCKS_PATH "grass_block_top.png");
    imagesArray[1][0] = LoadImage(BLOCKS_PATH "grass_block_side_overlay.png");
    ImageColorTint(&imagesArray[0][0], GREEN);
    ImageColorTint(&imagesArray[1][0], GREEN);
    texturesArray[2][0] = LoadTexture(BLOCKS_PATH "grass_block_side.png");
    texturesArray[2][1] = LoadTextureFromImage(imagesArray[0][0]);
    texturesArray[2][2] = LoadTextureFromImage(imagesArray[1][0]);
    UnloadImage(imagesArray[0][0]);
    UnloadImage(imagesArray[1][0]);

    texturesArray[3][0] = LoadTexture(BLOCKS_PATH "dirt.png");
    texturesArray[3][1] = LoadTexture(BLOCKS_PATH "Makovka.png");

    texturesArray[4][0] = LoadTexture(BLOCKS_PATH "cobblestone.png");

    texturesArray[5][0] = LoadTexture(BLOCKS_PATH "planks.png");

    texturesArray[7][0] = LoadTexture(BLOCKS_PATH "bedrock.png");

    texturesArray[12][0] = LoadTexture(BLOCKS_PATH "sand.png");
    texturesArray[13][0] = LoadTexture(BLOCKS_PATH "gravel.png");

    texturesArray[14][0] = LoadTexture(BLOCKS_PATH "gold_ore.png");
    texturesArray[15][0] = LoadTexture(BLOCKS_PATH "iron_ore.png");
    texturesArray[16][0] = LoadTexture(BLOCKS_PATH "coal_ore.png");

    texturesArray[17][0] = LoadTexture(BLOCKS_PATH "oak_log.png");

    texturesArray[18][0] = LoadTexture(BLOCKS_PATH "oak_leaves.png");

    texturesArray[19][0] = LoadTexture(BLOCKS_PATH "sponge.png");

    texturesArray[20][0] = LoadTexture(BLOCKS_PATH "glass.png");
}

void unloadTextures() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            UnloadTexture(texturesArray[i][j]);
        }
    }
}

Texture2D setTexture(const std::string& textureName) {
    size_t pos = textureName.find(':');

    if (pos == std::string::npos) {
        std::cerr << "Ошибка: некорректный формат имени текстуры: " << textureName << std::endl;
        return texturesArray[0][0];
    }

    int a, b;
    try {
        a = std::stoi(textureName.substr(0, pos));
        b = std::stoi(textureName.substr(pos + 1));
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка при конвертации строки в числа: " << e.what() << std::endl;
        return texturesArray[0][0];
    }

    return texturesArray[a][b];
}