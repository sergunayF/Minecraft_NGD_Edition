#pragma once

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>

#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <future>

#include <random>

#include "raylib.h"
#include "rlgl.h"

using json = nlohmann::json;

#define ASSETS_PATH "assets/"
#define BLOCKS_PATH "assets/blocks/"

#define RENDER_DISTANCE 2

#define WATER_LEVEL 63

const int worldSeed = 123;

const int screenWidth = 854;
const int screenHeight = 480;

struct BlockData {
    std::string name;
    std::string texture;
    bool grass = false;
};

std::unordered_map<std::string, BlockData> loadBlockData();

extern std::unordered_map<std::string, BlockData> blockDataMap;

Texture2D setTexture(const std::string& textureName);
std::string getTexture(double id);
std::string getBlockName(double value);

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color);

void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color);

void DrawGrassBlock(Vector3 position, float width, float height, float length, Texture2D(texturesArray)[3][3]);