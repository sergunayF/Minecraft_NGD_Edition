#pragma once

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include <unordered_map>
#include <vector>
#include <cmath>

#include <thread>
#include <atomic>
#include <mutex>

#include "raylib.h"
#include "rlgl.h"

using json = nlohmann::json;

#define ASSETS_PATH "assets/"
#define BLOCKS_PATH "assets/blocks/"

#define WATER_LEVEL 63

const int screenWidth = 854;
const int screenHeight = 480;

struct BlockData {
    std::string name;
    std::string texture;
};

std::unordered_map<std::string, BlockData> loadBlockData();
std::string getTexture(double id);
std::string getBlockName(double value);

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color);

void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color);