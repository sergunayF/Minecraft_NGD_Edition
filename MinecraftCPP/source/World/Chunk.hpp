#pragma once

#include "raylib.h"
#include "../Game/Game.hpp"
#include "../Block/Block.hpp"

class Chunk {
public:
    Vector2 worldPos;
    BlockMap blockMap;

    static constexpr int CHUNK_SIZE_X = 16;
    static constexpr int CHUNK_SIZE_Y = 256;
    static constexpr int CHUNK_SIZE_Z = 16;

    Chunk() : worldPos{ 0, 0 } {
        GenerateChunk();
    }

    Chunk(int worldX, int worldZ) {
        worldPos = { static_cast<float>(worldX), static_cast<float>(worldZ) };
        GenerateChunk();
    }

    void GenerateChunk() {
        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                for (int y = 0; y < 128; y++) {
                    Vector3 pos = {
                        worldPos.x * CHUNK_SIZE_X + x,
                        static_cast<float>(y),
                        worldPos.y * CHUNK_SIZE_Z + z
                    };
                    blockMap[pos] = Block(3.1, pos.x, pos.y, pos.z);
                }
            }
        }
    }

    void Draw() {
        for (auto& elem : blockMap) {
            elem.second.Draw(false);
        }
    }
};

using ChunkMap = std::unordered_map<Vector2, Chunk, Vector2Hash>;
