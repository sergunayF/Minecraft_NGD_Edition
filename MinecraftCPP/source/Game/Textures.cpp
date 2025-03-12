#include "Textures.hpp"
#include "../Block/MiniBlocks.hpp"

Texture texturesArray[TEXTURE_ARRAY_ROWS][TEXTURE_ARRAY_COLS];
Image imagesArray[2][2];

Texture cloudTexture;
Texture sunTexture;
Texture moonTexture;

Texture2D gui;
Texture2D inventoryGUI;
Texture2D craftingGUI;
Texture2D furnaceGUI;
Texture2D itemsGUI;
Texture2D iconsGUI;
Texture2D backgroundGUI;

Texture2D breakTextures[10];

Font minecraftFont;

std::vector<MiniBlock> miniBlocks;

void loadTextures() {

    texturesArray[0][0] = LoadTexture(BLOCKS_PATH "air.png");

    texturesArray[1][0] = LoadTexture(BLOCKS_PATH "stone.png");
    texturesArray[1][1] = LoadTexture(BLOCKS_PATH "test.png");

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
    texturesArray[17][17] = LoadTexture(BLOCKS_PATH "oak_log_top.png");

    texturesArray[18][0] = LoadTexture(BLOCKS_PATH "oak_leaves.png");

    texturesArray[19][0] = LoadTexture(BLOCKS_PATH "sponge.png");

    texturesArray[20][0] = LoadTexture(BLOCKS_PATH "glass.png");

    texturesArray[21][0] = LoadTexture(BLOCKS_PATH "white_wool.png");
    texturesArray[21][1] = LoadTexture(BLOCKS_PATH "orange_wool.png");
    texturesArray[21][2] = LoadTexture(BLOCKS_PATH "magenta_wool.png");
    texturesArray[21][3] = LoadTexture(BLOCKS_PATH "light_blue_wool.png");
    texturesArray[21][4] = LoadTexture(BLOCKS_PATH "yellow_wool.png");
    texturesArray[21][5] = LoadTexture(BLOCKS_PATH "lime_wool.png");
    texturesArray[21][6] = LoadTexture(BLOCKS_PATH "pink_wool.png");
    texturesArray[21][7] = LoadTexture(BLOCKS_PATH "gray_wool.png");
    texturesArray[21][8] = LoadTexture(BLOCKS_PATH "light_gray_wool.png");
    texturesArray[21][9] = LoadTexture(BLOCKS_PATH "cyan_wool.png");
    texturesArray[21][10] = LoadTexture(BLOCKS_PATH "purple_wool.png");
    texturesArray[21][11] = LoadTexture(BLOCKS_PATH "blue_wool.png");
    texturesArray[21][12] = LoadTexture(BLOCKS_PATH "brown_wool.png");
    texturesArray[21][13] = LoadTexture(BLOCKS_PATH "green_wool.png");
    texturesArray[21][14] = LoadTexture(BLOCKS_PATH "red_wool.png");
    texturesArray[21][15] = LoadTexture(BLOCKS_PATH "black_wool.png");

    texturesArray[22][0] = LoadTexture(BLOCKS_PATH "dandelion.png");
    texturesArray[23][0] = LoadTexture(BLOCKS_PATH "poppy.png");

    texturesArray[32][0] = LoadTexture(BLOCKS_PATH "dead_bush.png");

    texturesArray[58][0] = LoadTexture(BLOCKS_PATH "crafting_table_front.png");
    texturesArray[58][1] = LoadTexture(BLOCKS_PATH "crafting_table_side.png");
    texturesArray[58][2] = LoadTexture(BLOCKS_PATH "crafting_table_top.png");
    texturesArray[58][3] = LoadTexture(BLOCKS_PATH "planks.png");

    texturesArray[61][0] = LoadTexture(BLOCKS_PATH "furnace_front.png");
    texturesArray[61][1] = LoadTexture(BLOCKS_PATH "furnace_side.png");
    texturesArray[61][2] = LoadTexture(BLOCKS_PATH "furnace_top.png");
    texturesArray[61][3] = LoadTexture(BLOCKS_PATH "furnace_front_on.png");
    texturesArray[62][0] = LoadTexture(BLOCKS_PATH "furnace_front.png");
    texturesArray[62][1] = LoadTexture(BLOCKS_PATH "furnace_side.png");
    texturesArray[62][2] = LoadTexture(BLOCKS_PATH "furnace_top.png");
    texturesArray[62][3] = LoadTexture(BLOCKS_PATH "furnace_front_on.png");

    texturesArray[91][0] = LoadTexture(BLOCKS_PATH "cactus_side.png");
    texturesArray[91][1] = LoadTexture(BLOCKS_PATH "cactus_bottom.png");
    texturesArray[91][2] = LoadTexture(BLOCKS_PATH "cactus_top.png");


    cloudTexture = LoadTexture(ASSETS_PATH"environment/clouds.png");
    sunTexture = LoadTexture(ASSETS_PATH"terrain/sun.png");
    moonTexture = LoadTexture(ASSETS_PATH"terrain/moon.png");

    gui = LoadTexture(ASSETS_PATH"gui/gui.png");
    inventoryGUI = LoadTexture(ASSETS_PATH"gui/inventory.png");
    craftingGUI = LoadTexture(ASSETS_PATH"gui/crafting.png");
    furnaceGUI = LoadTexture(ASSETS_PATH"gui/furnace.png");
    itemsGUI = LoadTexture(ASSETS_PATH"gui/items.png");
    iconsGUI = LoadTexture(ASSETS_PATH"gui/icons.png");
    backgroundGUI = LoadTexture(ASSETS_PATH"gui/background.png");

    for (int i = 0; i < 10; i++) {

        std::string breakName = "destroy_stage_";
        
        std::string breakPath = BLOCKS_PATH + breakName + std::to_string(i) + ".png";

        breakTextures[i] = LoadTexture(breakPath.c_str());

    }

    minecraftFont = LoadFont(ASSETS_PATH"font/minecraft.ttf");

}

void unloadTextures() {
    for (int i = 0; i < TEXTURE_ARRAY_ROWS; i++) {
        for (int j = 0; j < TEXTURE_ARRAY_COLS; j++) {
            UnloadTexture(texturesArray[i][j]);
        }
    }

    UnloadTexture(cloudTexture);
    UnloadTexture(sunTexture);
    UnloadTexture(moonTexture);

    UnloadTexture(gui);
    UnloadTexture(inventoryGUI);
    UnloadTexture(craftingGUI);
    UnloadTexture(furnaceGUI);
    UnloadTexture(itemsGUI);
    UnloadTexture(iconsGUI);
    UnloadTexture(backgroundGUI);

    for (int i = 0; i < 10; i++) {
        UnloadTexture(breakTextures[i]);
    }
    
    UnloadFont(minecraftFont);

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