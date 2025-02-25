#pragma once

#include "Game.hpp"

#define ROWS 20
#define COLS 3

extern Texture texturesArray[ROWS][COLS];
extern Image imagesArray[2][2];

void loadTextures();
void unloadTextures();
