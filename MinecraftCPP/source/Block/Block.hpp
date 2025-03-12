#pragma once

#include "..\Game\Game.hpp"
#include "..\Game\Textures.hpp"
#include "..\Engine\Vector3Hash.hpp"

class Player;

class Block {

public:

    double id;

    bool rendered;
    bool textured;
    bool transparency;

    bool sixTextures;

    std::string textureName;
    std::string requiredTool;

    double durability;
    double drops;

    bool isReplaced;

    float blockSize = 1.0f;

    std::unordered_map<std::string, double> harvestTools;

    Texture texture;
    Vector3 position;

    Texture2D sixTextureArray[6];

    bool neighbors[6];
    bool needsUpdate;

    // Furnace {

    double holder[100][2];
    double meltResult;
    int fuelTime;

    int burnTime = 0;
    int cookTime = 0;

    // Furnace }

    Block() : id(0), position({ 0.0f, 0.0f, 0.0f }), textureName("minecraft:air"),
        durability(0), drops(0), requiredTool("none"), textured(false) {
        texture = texturesArray[0][0];
    }

    Block(double blockID, float x, float y, float z)
        : id(blockID), position({ x, y, z }), rendered(false), textured(false) {

        textureName = getTexture(id);

        transparency = blockDataMap[textureName].transparency;
        sixTextures = blockDataMap[textureName].hasSixTextures;
        durability = blockDataMap[textureName].durability;
        drops = blockDataMap[textureName].drops;
        requiredTool = blockDataMap[textureName].requiredTool;
        harvestTools = blockDataMap[textureName].harvestTools;

        memset(holder, 0, sizeof(holder));

    }

    std::string Serialize() const {
        std::ostringstream oss;
        oss << id << ' ' << position.x << ' ' << position.y << ' ' << position.z;
        return oss.str();
    }

    static Block Deserialize(const std::string& data) {
        std::istringstream iss(data);
        double blockID;
        Vector3 pos;

        iss >> blockID >> pos.x >> pos.y >> pos.z;

        return Block(blockID, pos.x, pos.y, pos.z);
    }

    void Draw(bool isHighlighted) {
        
        if (!textured) {
            size_t pos = textureName.find(':');
            int a = std::stoi(textureName.substr(0, pos));
            int b = std::stoi(textureName.substr(pos + 1));

            if (sixTextures) {
                for (int i = 0; i < 6; i++) {
                    sixTextureArray[i] = texturesArray[a][blockDataMap[getTexture(id)].sixTextures[i]];
                }
                textured = true;
            }
            else {
                texture = texturesArray[a][b];
                textured = true;
            }
        }

        Color tint = WHITE;

        if (blockDataMap[textureName].grass) tint = GREEN;

        if (blockDataMap[textureName].billboard) DrawBillboardBlock(texture, position, blockSize, tint);
        else if (id == 2) DrawGrassBlock(position, blockSize, blockSize, blockSize, texturesArray, neighbors);
        else if (!sixTextures) DrawCubeTexture(texture, position, blockSize, blockSize, blockSize, tint, neighbors);
        else DrawCubeSixTexture(sixTextureArray, position, blockSize, blockSize, blockSize, tint, id);

        if (isHighlighted) DrawCubeWires(position, blockSize + 0.05f, blockSize + 0.05f, blockSize + 0.05f, BLACK);

    }

    virtual ~Block() {};

};

class FurnaceBlock : public Block {

public:

    void Update(Player& player);

    void Melting();

    void LoadState(Player& player);
    void SaveState(Player& player);

};

using BlockMap = std::unordered_map<Vector3, Block, Vector3Hash>;
