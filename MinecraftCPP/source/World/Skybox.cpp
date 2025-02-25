#include "Skybox.hpp"

Color GetSkyColor(int timeOfDay) {
    const int DAY_START = 0;
    const int SUNSET_START = 12000;
    const int SUNSET_END = 13000;
    const int NIGHT_START = 14000;
    const int SUNRISE_START = 23000;
    const int SUNRISE_END = 24000;

    Vector3 dayColor = { 121, 166, 255 };
    Vector3 sunsetColor = { 255, 100, 50 };
    Vector3 twilightColor = { 50, 50, 100 };
    Vector3 nightColor = { 0, 0, 0 };
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
    else {
        float t = (float)(timeOfDay - SUNRISE_END) / (DAY_START + 24000 - SUNRISE_END);
        currentColor = {
            sunriseColor.x * (1 - t) + dayColor.x * t,
            sunriseColor.y * (1 - t) + dayColor.y * t,
            sunriseColor.z * (1 - t) + dayColor.z * t
        };
    }

    return { (unsigned char)currentColor.x, (unsigned char)currentColor.y, (unsigned char)currentColor.z, 255 };
}
