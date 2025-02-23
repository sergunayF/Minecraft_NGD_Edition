#pragma once

#include "Game.hpp"

#define ROWS 10
#define COLS 3

extern Texture texturesArray[ROWS][COLS];

void loadTextures();
void unloadTextures();
