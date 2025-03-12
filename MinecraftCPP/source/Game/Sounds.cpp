#include "Sounds.hpp"

Sound blockSounds[SOUNDS_COUNT][SOUNDS_COUNT];
Sound musicArray[MUSIC_COUNT];

void loadSounds() {

	musicArray[0] = LoadSound(ASSETS_PATH"music/calm1.ogg");
	musicArray[1] = LoadSound(ASSETS_PATH"music/calm2.ogg");
	musicArray[2] = LoadSound(ASSETS_PATH"music/calm3.ogg");

	blockSounds[0][0] = LoadSound(ASSETS_PATH"sound3/step/grass1.ogg");
	blockSounds[0][1] = LoadSound(ASSETS_PATH"sound3/step/grass2.ogg");
	blockSounds[0][2] = LoadSound(ASSETS_PATH"sound3/step/grass3.ogg");
	blockSounds[0][3] = LoadSound(ASSETS_PATH"sound3/step/grass4.ogg");
	blockSounds[0][4] = LoadSound(ASSETS_PATH"sound3/step/grass5.ogg");
	blockSounds[0][5] = LoadSound(ASSETS_PATH"sound3/step/grass6.ogg");

	blockSounds[1][0] = LoadSound(ASSETS_PATH"sound/step/grass1.ogg");
	blockSounds[1][1] = LoadSound(ASSETS_PATH"sound/step/grass2.ogg");
	blockSounds[1][2] = LoadSound(ASSETS_PATH"sound/step/grass3.ogg");
	blockSounds[1][3] = LoadSound(ASSETS_PATH"sound/step/grass4.ogg");

}

void unloadSounds() {

	for (int i = 0; i < SOUNDS_COUNT; i++) {

		for (int j = 0; j < SOUNDS_COUNT; j++) {

			UnloadSound(blockSounds[i][j]);

		}

	}

	for (int i = 0; i < MUSIC_COUNT; i++) UnloadSound(musicArray[i]);

}

void playingSound(double id, int mode) {

	double sound;

	switch(mode) {
	case 0: sound = blockDataMap[getTexture(id)].sound; break;
	case 1: sound = blockDataMap[getTexture(id)].sound3; break;
	default: sound = blockDataMap[getTexture(id)].sound; break;
	}

	int soundIndex = static_cast<int>(sound);
	int randomIndex = rand() % (static_cast<int>((sound - soundIndex) * 10) + 1);

	if (sound != -1.0) {
		if (mode == 2) {
			SetSoundVolume(blockSounds[soundIndex][randomIndex], 0.5f);
		}
		PlaySound(blockSounds[soundIndex][randomIndex]);
	}

}