#pragma once

#include "../Game/Game.hpp"
#include "../Player/Player.hpp"

void DrawSky(int& timeOfDay);

Vector3 GetSunPosition(int timeOfDay);
Vector3 GetMoonPosition(int timeOfDay);

Color GetSkyColor(int timeOfDay);
