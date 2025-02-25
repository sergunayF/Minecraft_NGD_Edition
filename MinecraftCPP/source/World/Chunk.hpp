#pragma once

#include "../Game/Game.hpp"
#include "../Block/Block.hpp"
#include "Landscape.hpp"
#include "../Game/Noise.hpp"

class Chunk;
using ChunkMap = std::unordered_map<Vector2, Chunk, Vector2Hash>;

class Chunk {

public:
    Vector2 worldPos;
    BlockMap blockMap;
    std::vector<Block*> renderedBlocks;

    bool IsLoaded;

    static constexpr int CHUNK_SIZE_X = 16;
    static constexpr int CHUNK_SIZE_Y = 16;
    static constexpr int CHUNK_SIZE_Z = 16;

    Chunk() : worldPos{ 0, 0 }, IsLoaded(false) {
        GenerateChunk();
    }

    Chunk(int worldX, int worldZ) : IsLoaded(false) {
        worldPos = { static_cast<float>(worldX), static_cast<float>(worldZ) };
        GenerateChunk();
    }

    void GenerateChunk();
    void GenerateTree(int x, int y, int z);

    bool HasBlockAt(const Vector3& pos);
    Block* GetBlockAt(const Vector3& pos, ChunkMap& chunkMap);
    void Update(ChunkMap& chunkMap);
    void UpdateNeighborBlocks(const Vector3& blockPos, ChunkMap& chunkMap, bool isBlockDestroyed);
    void Draw(const Vector3& highlightedBlockPos);
};