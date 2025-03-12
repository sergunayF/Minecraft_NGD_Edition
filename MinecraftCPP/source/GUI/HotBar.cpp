#include "HotBar.hpp"

Rectangle texHotBarRect = { 0, 0, 182, 22 };
Rectangle destHotBarRect = { 0, 0, texHotBarRect.width * GUI_SCALE, texHotBarRect.height * GUI_SCALE };

Rectangle texSelectorRect = { 0, 22, 24, 24 };
Rectangle destSelectorRect = { 0, 0, texSelectorRect.width * GUI_SCALE, texSelectorRect.height * GUI_SCALE };

Camera3D uiCamera = { 0 };

Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

void InitMiniBlocks(Player& player) {
    
    miniBlocks.clear();

    Vector3 cubeScale = { 0.75f * GUI_SCALE, 0.75f * GUI_SCALE, 0.75f * GUI_SCALE };
    Vector3 position = { 0.0f, 0.0f, 0.0f };

    for (int i = 0; i < PLAYER_INITIALIZATION_SLOT; i++) {
        miniBlocks.emplace_back(player.inventory[i][0], setTexture(getTexture(player.inventory[i][0])), position, cubeScale);
    }

    miniBlocks.emplace_back(cursor.ItemID, setTexture(getTexture(cursor.ItemID)), position, cubeScale);

}

void UpdateMiniBlocks(Player& player) {

    Vector3 cubeScale = { 0.75f * GUI_SCALE, 0.75f * GUI_SCALE, 0.75f * GUI_SCALE };

    for (int i = 0; i < PLAYER_INITIALIZATION_SLOT; i++) {
        if (miniBlocks[i].BlockID != player.inventory[i][0]) {
            miniBlocks[i].blockTexture = setTexture(getTexture(player.inventory[i][0]));
            miniBlocks[i].BlockID = player.inventory[i][0];
            miniBlocks[i].rendered = false;
        }
    }

}


void DrawHotBar(Player& player) {    

    UpdateMiniBlocks(player);

    DrawTexturePro(gui, texHotBarRect, destHotBarRect, { (screenWidth - destHotBarRect.width) / -2, (screenHeight - destHotBarRect.height) * -1 }, 0.0f, WHITE);

    DrawTexturePro(gui, texSelectorRect, destSelectorRect, { ((screenWidth - destHotBarRect.width) / -2 + GUI_SCALE) - player.inventorySlot * 40, (screenHeight - destHotBarRect.height - GUI_SCALE) * -1 }, 0.0f, WHITE);

    DrawTexturePro(iconsGUI, { 0.0f, 0.0f, 16.0f, 16.0f }, {0.0f, 0.0f, 16.0f * GUI_SCALE, 16.0f * GUI_SCALE}, { screenWidth / -2 + 8.0f * GUI_SCALE / 2, screenHeight / -2 + 8.0f * GUI_SCALE / 2}, 0.0f, WHITE);
    
    DrawTexturePro(iconsGUI, { 0.0f, 64.0f, 182.0f, 5.0f }, {0.0f, 0.0f, 182.0f * GUI_SCALE, 5.0f * GUI_SCALE}, { (screenWidth - destHotBarRect.width) / -2, (screenHeight - destHotBarRect.height) * -1 + 8.0f * GUI_SCALE }, 0.0f, WHITE);

    //HP

    int heartsPerRow = MAX_HEARTS_PER_ROW * 2;
    int rows = (player.MaxHP + heartsPerRow - 1) / heartsPerRow;

    for (int row = 0; row < rows; row++) {
        int rowMaxHearts = (player.MaxHP - row * heartsPerRow) / 2;

        for (int i = 0; i < rowMaxHearts; i++) {
            DrawTexturePro(iconsGUI,
                { 16.0f, 0.0f, 9.0f, 9.0f },
                { 0.0f, 0.0f, 9.0f * GUI_SCALE, 9.0f * GUI_SCALE },
                { (screenWidth - destHotBarRect.width) / -2 - i % 10 * 8.0f * GUI_SCALE,
                  (screenHeight - destHotBarRect.height) * -1 + 18.0f * GUI_SCALE + row * 5.0f * GUI_SCALE },
                0.0f, WHITE);
        }
    }

    for (int row = 0; row < rows; row++) {
        int rowHP = player.HP - row * heartsPerRow;
        if (rowHP <= 0) break;

        int fullHearts = rowHP / 2;
        bool hasHalfHeart = rowHP % 2 != 0;

        for (int i = 0; i < fullHearts; i++) {
            DrawTexturePro(iconsGUI,
                { 52.0f, 0.0f, 9.0f, 9.0f },
                { 0.0f, 0.0f, 9.0f * GUI_SCALE, 9.0f * GUI_SCALE },
                { (screenWidth - destHotBarRect.width) / -2 - i % 10 * 8.0f * GUI_SCALE,
                  (screenHeight - destHotBarRect.height) * -1 + 18.0f * GUI_SCALE + row * 5.0f * GUI_SCALE },
                0.0f, WHITE);
        }

        if (hasHalfHeart) {
            DrawTexturePro(iconsGUI,
                { 61.0f, 0.0f, 9.0f, 9.0f },
                { 0.0f, 0.0f, 9.0f * GUI_SCALE, 9.0f * GUI_SCALE },
                { (screenWidth - destHotBarRect.width) / -2 - fullHearts % 10 * 8.0f * GUI_SCALE,
                  (screenHeight - destHotBarRect.height) * -1 + 18.0f * GUI_SCALE + row * 5.0f * GUI_SCALE },
                0.0f, WHITE);
        }
    }

    //inv

    for (int i = 0; i < 9; i++) {

        miniBlocks[i].Render(texturesArray);
        miniBlocks[i].Draw({ (screenWidth - destHotBarRect.width - 2.0f * GUI_SCALE) / 2 + i * 20.F * GUI_SCALE, screenHeight - destHotBarRect.height - 3});

        if (player.inventory[i][1] > 1.0) {
            std::string countItems = std::to_string(static_cast<int>(player.inventory[i][1]));
            DrawText(countItems.c_str(), static_cast<int>((screenWidth  - destHotBarRect.width - 2) / 2 + i * 40), screenHeight - destHotBarRect.height - 4, 24, WHITE);
        }
    }
}
