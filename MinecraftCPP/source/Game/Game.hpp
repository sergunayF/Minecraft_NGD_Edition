#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

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

#include <GLFW/glfw3.h>
#include <filesystem>

#include "raylib.h"
#include "rlgl.h"

using json = nlohmann::json;

#define ASSETS_PATH "assets/"
#define BLOCKS_PATH "assets/blocks/"

#define RENDER_DISTANCE 2
#define GUI_SCALE 2

#define WATER_LEVEL 63

#define TEXTURE_ARRAY_ROWS 100
#define TEXTURE_ARRAY_COLS 16

#define SLOTS 36
#define SLOTS_COUNTS 2

const int worldSeed = 123;

const int screenWidth = 854;
const int screenHeight = 480;

struct BlockData {
    std::string name;
    std::string texture;
    bool grass = false;
    bool billboard = false;
    bool transparency = false;

    double durability;
    double drops = 0.0;

    std::unordered_map<std::string, double> harvestTools;
    std::string requiredTool = "";
};


std::unordered_map<std::string, BlockData> loadBlockData();

extern std::unordered_map<std::string, BlockData> blockDataMap;

Texture2D setTexture(const std::string& textureName);

std::string getTexture(double id);
std::string getBlockName(double value);

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color);

void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color);

void DrawGrassBlock(Vector3 position, float width, float height, float length, Texture2D(texturesArray)[TEXTURE_ARRAY_ROWS][TEXTURE_ARRAY_COLS]);

void DrawBillboardBlock(Texture2D texture, Vector3 position, float size, Color color);