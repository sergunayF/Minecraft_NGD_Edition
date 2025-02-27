#include "HotBar.hpp"

std::vector<MiniBlock> miniBlocks;

Rectangle texHotBarRect = { 0, 0, 182, 22 };
Rectangle destHotBarRect = { 0, 0, texHotBarRect.width * GUI_SCALE, texHotBarRect.height * GUI_SCALE };

Rectangle texSelectorRect = { 0, 22, 24, 24 };
Rectangle destSelectorRect = { 0, 0, texSelectorRect.width * GUI_SCALE, texSelectorRect.height * GUI_SCALE };

Camera3D uiCamera = { 0 };

Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

void InitMiniBlocks(Player& player) {
    miniBlocks.clear();

    Vector3 cubeScale = { 0.75f * GUI_SCALE, 0.75f * GUI_SCALE, 0.75f * GUI_SCALE };

    for (int i = 0; i < 9; i++) {
        Vector3 position = { 0.0f, 0.0f, 0.0f };
        miniBlocks.emplace_back(player.inventory[i][0], setTexture(getTexture(player.inventory[i][0])), position, cubeScale);
    }
}

void UpdateMiniBlocks(Player& player) {
    Vector3 cubeScale = { 0.75f * GUI_SCALE, 0.75f * GUI_SCALE, 0.75f * GUI_SCALE };

    for (int i = 0; i < 9; i++) {
        if (miniBlocks[i].BlockID != player.inventory[i][0]) {
            miniBlocks[i].blockTexture = setTexture(getTexture(player.inventory[i][0]));
            miniBlocks[i].BlockID = player.inventory[i][0];
        }
    }
}


void DrawHotBar(Player& player) {    

    UpdateMiniBlocks(player);

    DrawTexturePro(gui, texHotBarRect, destHotBarRect, { (screenWidth - destHotBarRect.width) / 2 * -1, (screenHeight - destHotBarRect.height) * -1 }, 0.0f, WHITE);

    DrawTexturePro(gui, texSelectorRect, destSelectorRect, { ((screenWidth - destHotBarRect.width) / 2 * -1) - player.inventorySlot * 40, (screenHeight - destHotBarRect.height - 1) * -1 }, 0.0f, WHITE);

    for (int i = 0; i < 9; i++) {

        miniBlocks[i].Render();
        miniBlocks[i].Draw({ (screenWidth - destHotBarRect.width - 2) / 2 + i * 40, screenHeight - destHotBarRect.height - 4});

        if (player.inventory[i][1] != 0.0) {
            std::string countItems = std::to_string(static_cast<int>(player.inventory[i][1]));
            DrawText(countItems.c_str(), static_cast<int>((screenWidth - destHotBarRect.width - 2) / 2 + i * 40), screenHeight - destHotBarRect.height - 4, 24, WHITE);
        }
    }
}
