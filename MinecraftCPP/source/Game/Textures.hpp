#pragma once

#include "Game.hpp"

extern Texture texturesArray[TEXTURE_ARRAY_ROWS][TEXTURE_ARRAY_COLS];
extern Image imagesArray[2][2];

extern Texture cloudTexture;
extern Texture sunTexture;
extern Texture moonTexture;

extern Texture2D gui;

extern Texture2D breakTextures[10];

void loadTextures();
void unloadTextures();
