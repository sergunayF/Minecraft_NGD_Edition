#pragma once

#include "Game.hpp"

extern Texture texturesArray[TEXTURE_ARRAY_ROWS][TEXTURE_ARRAY_COLS];
extern Image imagesArray[2][2];

extern Texture cloudTexture;
extern Texture sunTexture;
extern Texture moonTexture;

void loadTextures();
void unloadTextures();
