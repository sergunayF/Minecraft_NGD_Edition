#pragma once

#include "Game.hpp"

#define SOUNDS_COUNT 100
#define MUSIC_COUNT 3

extern Sound blockSounds[SOUNDS_COUNT][SOUNDS_COUNT];
extern Sound musicArray[MUSIC_COUNT];

void loadSounds();
void unloadSounds();

void playingSound(double id, int mode);