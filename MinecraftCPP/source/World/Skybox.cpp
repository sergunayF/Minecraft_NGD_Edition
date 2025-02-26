#include "Skybox.hpp"
#include "../Game/Textures.hpp"

const int DAY_START = 0;
const int SUNSET_START = 12000;
const int SUNSET_END = 13000;
const int NIGHT_START = 14000;
const int MID_NIGHT = 15000;
const int NIGHT_END = 23000;
const int SUNRISE_START = 23000;
const int SUNRISE_END = 23500;
const int DAY_TRANSITION_END = 24000;

static float cloudPosition = 0.0f;
static float cloudSpeed = 0.5f;

void DrawSky(int& timeOfDay) {

    timeOfDay = (timeOfDay + 1) % 24000;
    Color skyColor = GetSkyColor(timeOfDay);

    ClearBackground(skyColor);

    Vector3 sunPosition = GetSunPosition(timeOfDay);
    Vector3 moonPosition = GetMoonPosition(timeOfDay);

    cloudPosition -= cloudSpeed * GetFrameTime();
    if (cloudPosition <= -cloudTexture.width - 1280.0f) {
        cloudPosition = 0.0f;
    }

    DrawCubeTexture(cloudTexture, { cloudPosition, 128.0f, 0 }, 1280.0f, 0.01f, 1280.0f, WHITE);
    DrawCubeTexture(cloudTexture, { cloudPosition + cloudTexture.width + 1280.0f, 128.0f, 0 }, 1280.0f, 0.01f, 1280.0f, WHITE);

    //DrawCubeTexture(sunPosition, 50.0f, YELLOW);

    DrawSphere(sunPosition, 50.0f, YELLOW);

    DrawSphere(moonPosition, 40.0f, LIGHTGRAY);
}

Vector3 GetSunPosition(int timeOfDay) {
    float angle = 0.0f;

    if (timeOfDay < SUNSET_START) {

        angle = (float)(timeOfDay - DAY_START) / (SUNSET_START - DAY_START) * PI;
    }
    else if (timeOfDay < SUNSET_END) {

        angle = PI + (float)(timeOfDay - SUNSET_START) / (SUNSET_END - SUNSET_START) * PI / 2;
    }
    else if (timeOfDay < SUNRISE_START) {

        angle = 3 * PI / 2;
    }
    else if (timeOfDay < SUNRISE_END) {

        angle = 3 * PI / 2 + (float)(timeOfDay - SUNRISE_START) / (SUNRISE_END - SUNRISE_START) * PI / 2;
    }
    else {

        angle = 2 * PI;
    }

    float radius = 1000.0f;
    Vector3 sunPosition = {
        radius * cos(angle),
        radius * sin(angle),
        0.0f
    };

    return sunPosition;
}

Vector3 GetMoonPosition(int timeOfDay) {
    float angle = 0.0f;

    if (timeOfDay < NIGHT_START) {

        angle = 3 * PI / 2;
    }
    else if (timeOfDay < MID_NIGHT) {

        angle = 3 * PI / 2 + (float)(timeOfDay - NIGHT_START) / (MID_NIGHT - NIGHT_START) * PI / 2;
    }
    else if (timeOfDay < NIGHT_END) {

        angle = 2 * PI + (float)(timeOfDay - MID_NIGHT) / (NIGHT_END - MID_NIGHT) * PI / 2;
    }
    else {

        angle = 3 * PI / 2;
    }

    float radius = 1000.0f;
    Vector3 moonPosition = {
        radius * cos(angle),
        radius * sin(angle),
        0.0f
    };

    return moonPosition;
}

Color GetSkyColor(int timeOfDay) {

    Vector3 dayColor = { 121, 166, 255 };
    Vector3 sunsetColor = { 255, 100, 50 };
    Vector3 twilightColor = { 50, 50, 100 };
    Vector3 nightColor = { 0, 0, 20 };
    Vector3 sunriseColor = { 255, 150, 50 };

    Vector3 currentColor;

    if (timeOfDay < SUNSET_START) {
        currentColor = dayColor;
    }
    else if (timeOfDay < SUNSET_END) {
        float t = (float)(timeOfDay - SUNSET_START) / (SUNSET_END - SUNSET_START);
        currentColor = {
            dayColor.x * (1 - t) + sunsetColor.x * t,
            dayColor.y * (1 - t) + sunsetColor.y * t,
            dayColor.z * (1 - t) + sunsetColor.z * t
        };
    }
    else if (timeOfDay < NIGHT_START) {
        float t = (float)(timeOfDay - SUNSET_END) / (NIGHT_START - SUNSET_END);
        currentColor = {
            sunsetColor.x * (1 - t) + twilightColor.x * t,
            sunsetColor.y * (1 - t) + twilightColor.y * t,
            sunsetColor.z * (1 - t) + twilightColor.z * t
        };
    }
    else if (timeOfDay < MID_NIGHT) {
        float t = (float)(timeOfDay - NIGHT_START) / (MID_NIGHT - NIGHT_START);
        currentColor = {
            twilightColor.x * (1 - t) + nightColor.x * t,
            twilightColor.y * (1 - t) + nightColor.y * t,
            twilightColor.z * (1 - t) + nightColor.z * t
        };
    }
    else if (timeOfDay < SUNRISE_START) {
        currentColor = nightColor;
    }
    else if (timeOfDay < SUNRISE_END) {
        float t = (float)(timeOfDay - SUNRISE_START) / (SUNRISE_END - SUNRISE_START);
        currentColor = {
            nightColor.x * (1 - t) + sunriseColor.x * t,
            nightColor.y * (1 - t) + sunriseColor.y * t,
            nightColor.z * (1 - t) + sunriseColor.z * t
        };
    }
    else if (timeOfDay < DAY_TRANSITION_END) {
        float t = (float)(timeOfDay - SUNRISE_END) / (DAY_TRANSITION_END - SUNRISE_END);
        currentColor = {
            sunriseColor.x * (1 - t) + dayColor.x * t,
            sunriseColor.y * (1 - t) + dayColor.y * t,
            sunriseColor.z * (1 - t) + dayColor.z * t
        };
    }
    else {
        currentColor = dayColor;
    }

    return { (unsigned char)currentColor.x, (unsigned char)currentColor.y, (unsigned char)currentColor.z, 255 };
}