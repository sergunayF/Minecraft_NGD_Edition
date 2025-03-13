#pragma once

#include "Game.hpp"

class MiniBlock;

extern Texture texturesArray[TEXTURE_ARRAY_ROWS][TEXTURE_ARRAY_COLS];
extern Image imagesArray[2][2];

extern Texture2D zombie[ZOMBIE_CHUNKS];
extern Texture2D pig[PIG_CHUNKS];

extern Texture cloudTexture;
extern Texture sunTexture;
extern Texture moonTexture;

extern Texture2D gui;
extern Texture2D inventoryGUI;
extern Texture2D craftingGUI;
extern Texture2D furnaceGUI;
extern Texture2D itemsGUI;
extern Texture2D iconsGUI;
extern Texture2D backgroundGUI;

extern Texture2D breakTextures[10];

extern Font minecraftFont;

extern std::vector<MiniBlock> miniBlocks;

void loadTextures();
void unloadTextures();
