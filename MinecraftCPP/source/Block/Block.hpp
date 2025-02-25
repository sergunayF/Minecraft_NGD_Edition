#pragma once

#include "..\Game\Game.hpp"
#include "..\Game\Textures.hpp"
#include "..\Game\Vector3Hash.hpp"

class Block {
public:

    double id;

    bool rendered;
    bool textured;

    std::string textureName;

    Texture texture;
    Vector3 position;

    Block() : id(0), position({ 0.0f, 0.0f, 0.0f }), textureName("minecraft:air"), textured(false) {
        texture = texturesArray[0][0];
    }

    Block(double blockID, float x, float y, float z)
        : id(blockID), position({ x, y, z }), rendered(false), textured(false) {

        textureName = getTexture(id);
    }

    void Draw(bool isHighlighted) {
        if (!textured) {
            size_t pos = textureName.find(':');
            int a = std::stoi(textureName.substr(0, pos));
            int b = std::stoi(textureName.substr(pos + 1));
            texture = texturesArray[a][b];
            textured = true;
        }

        int blockSize = 1;
        Color tint = isHighlighted ? YELLOW : WHITE;

        // Проверяем, есть ли параметр Grass и равен ли он true
        if (blockDataMap[textureName].grass) {
            // Используем специальные цвета для травы
            tint = GREEN; // Например, зеленый цвет для травы
        }

        if (id == 2) DrawGrassBlock(position, blockSize, blockSize, blockSize, texturesArray);
        else DrawCubeTexture(texture, position, blockSize, blockSize, blockSize, tint);

        if (isHighlighted) {
            DrawCubeWires(position, blockSize + 0.05f, blockSize + 0.05f, blockSize + 0.05f, BLACK);
        }
    }

};

using BlockMap = std::unordered_map<Vector3, Block, Vector3Hash>;
