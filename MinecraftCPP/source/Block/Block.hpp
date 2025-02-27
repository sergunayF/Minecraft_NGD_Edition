#pragma once

#include "..\Game\Game.hpp"
#include "..\Game\Textures.hpp"
#include "..\Engine\Vector3Hash.hpp"

class Block {
public:

    double id;
    
    bool rendered;
    bool textured;
    bool transparency;

    std::string textureName;
    std::string requiredTool;

    double durability;
    double drops;
    
    std::unordered_map<std::string, double> harvestTools;

    Texture texture;
    Vector3 position;

    Block() : id(0), position({ 0.0f, 0.0f, 0.0f }), textureName("minecraft:air"),
        durability(0), drops(0), requiredTool("none"), textured(false) {
        texture = texturesArray[0][0];
    }

    Block(double blockID, float x, float y, float z)
        : id(blockID), position({ x, y, z }), rendered(false), textured(false) {

        textureName = getTexture(id);

        transparency = blockDataMap[textureName].transparency;
        durability = blockDataMap[textureName].durability;
        drops = blockDataMap[textureName].drops;
        requiredTool = blockDataMap[textureName].requiredTool;
        harvestTools = blockDataMap[textureName].harvestTools;
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


    void Draw(bool isHighlighted, Camera3D& camera) {
        if (!textured) {
            size_t pos = textureName.find(':');
            int a = std::stoi(textureName.substr(0, pos));
            int b = std::stoi(textureName.substr(pos + 1));
            texture = texturesArray[a][b];
            textured = true;
        }

        float blockSize = 1.0f;
        Color tint = isHighlighted ? YELLOW : WHITE;

        if (blockDataMap[textureName].grass) tint = GREEN;

        if (blockDataMap[textureName].billboard) DrawBillboardBlock(texture, position, blockSize, tint);
        else if (id == 2) DrawGrassBlock(position, blockSize, blockSize, blockSize, texturesArray);
        else DrawCubeTexture(texture, position, blockSize, blockSize, blockSize, tint);

        if (isHighlighted) DrawCubeWires(position, blockSize + 0.05f, blockSize + 0.05f, blockSize + 0.05f, BLACK);
    }
};

using BlockMap = std::unordered_map<Vector3, Block, Vector3Hash>;
