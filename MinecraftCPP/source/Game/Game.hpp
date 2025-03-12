#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <array>
#include <cmath>

#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <queue>

#include <random>

#include <GLFW/glfw3.h>
#include <filesystem>

#include "raylib.h"
#include "rlgl.h"

using json = nlohmann::json;

#define ASSETS_PATH "assets/"
#define BLOCKS_PATH "assets/blocks/"
#define MOB_PATH "assets/mob/"

#define MAX_THREADS 16
#define RENDER_DISTANCE 2
#define FOG_DISTANCE 2

#define GUI_SCALE 2

#define WATER_LEVEL 63

#define TEXTURE_ARRAY_ROWS 100
#define TEXTURE_ARRAY_COLS 20

#define ZOMBIE_CHUNKS 25

#define PLAYER_INITIALIZATION_SLOT 100

#define INVENTORY_SLOTS 36
#define INVENTORY_SLOTS_COUNTS 2

#define CRAFTING_TABLE_SLOTS 10
#define RESULT_SLOT 46

#define MAX_HEARTS_PER_ROW 10

#define FURNACE_TIMER 10

const int worldSeed = 123;

const int screenWidth = 854;
const int screenHeight = 480;

extern std::string savePath;

struct BlockData {

    std::string name;

    bool grass = false;
    bool billboard = false;
    bool transparency = false;

    double durability;
    double drops = 0.0;
    
    int fuel = 0;
    double meltOut = 0.0;

    std::unordered_map<std::string, double> harvestTools;
    std::string requiredTool = "";

    bool hasSixTextures = false;
    std::vector<int> sixTextures;

    bool isItem = false;
    Vector2 texturePosition = { 0, 0 };

    double Stack = 64.0;

};

std::unordered_map<std::string, BlockData> loadBlockData();

extern std::unordered_map<std::string, BlockData> blockDataMap;

Texture2D setTexture(const std::string& textureName);

std::string getTexture(double id);
std::string getBlockName(double value);

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color, bool neighbors[6]);

void DrawCubeSixTexture(Texture2D textures[6], Vector3 position, float width, float height, float length, Color color, double blockID);

void DrawGrassBlock(Vector3 position, float width, float height, float length, Texture2D(texturesArray)[TEXTURE_ARRAY_ROWS][TEXTURE_ARRAY_COLS], bool neighbors[6]);

void DrawBillboardBlock(Texture2D texture, Vector3 position, float size, Color color);