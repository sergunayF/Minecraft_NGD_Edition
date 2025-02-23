#pragma once

#include "..\Game\Game.hpp"
#include "..\Game\Textures.hpp"
#include "..\Game\Vector3Hash.hpp"

class Block {
public:
    double id;
    std::string textureName;
    Texture texture;
    Vector3 position;

    Block() : id(0), position({ 0.0f, 0.0f, 0.0f }), textureName("minecraft:air") {
        texture = texturesArray[0][0];
    }

    Block(double blockID, float x, float y, float z) {
        id = blockID;
        position = { x, y, z };

        textureName = getTexture(id);

        size_t pos = textureName.find(':');
        
        //std::cout << "Texture of 1:0: " << textureName << std::endl;

        int a = std::stoi(textureName.substr(0, pos));
        int b = std::stoi(textureName.substr(pos + 1));


        texture = texturesArray[a][b];
    }

    void Draw(bool isHighlighted) {

        int blockSize = 1;

        Color tint = isHighlighted ? YELLOW : WHITE;
        DrawCubeTexture(texture, position, blockSize, blockSize, blockSize, tint);

        if (isHighlighted) {
            DrawCubeWires(position, blockSize + 0.05f, blockSize + 0.05f, blockSize + 0.05f, BLACK);
        }
    }
};

using BlockMap = std::unordered_map<Vector3, Block, Vector3Hash>;