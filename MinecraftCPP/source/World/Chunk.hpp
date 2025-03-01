#pragma once

#include "Biomes.hpp"

#include "../Game/Game.hpp"
#include "../Engine/Noise.hpp"
#include "../Engine/Vector.hpp"
#include "../Block/Block.hpp"

#include <sstream>

extern std::atomic<bool> isUpdatingChunks;

class Chunk;
using ChunkMap = std::unordered_map<Vector2, Chunk, Vector2Hash>;

extern std::mutex fileMutex;

class Chunk {

public:

    Vector2 worldPos;
    BlockMap blockMap;
    
    std::vector<Block*> renderedBlocks;

    bool IsLoaded;
    bool IsChanged = false;

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

    //  CHUNK GENERATION

    void GenerateChunk();
    void GenerateTree(int x, int y, int z);
    
    void GenerateCaves();
    
    void GenerateOre(int oreID, int minY, int maxY, int maxSize, int clusterSize);
    void TryPlaceOre(int oreID, int x, int y, int z);

    //

    bool HasBlockAt(const Vector3& pos);
    Block* GetBlockAt(const Vector3& pos, ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex);

    void Update(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex);
    void UpdateNeighborBlocks(const Vector3& blockPos, ChunkMap& chunkMap, bool isBlockDestroyed, std::shared_mutex& chunkMapMutex);
    void UpdateNeighborChunks(ChunkMap& chunkMap, const Vector2& chunkPos, std::shared_mutex& chunkMapMutex);
    
    void SaveToFile(const std::string& savePath);
    void LoadFromFile(const std::string& savePath);

    void Draw(const Vector3& highlightedBlockPos, Camera3D& camera, ChunkMap& chunkMap);

};

void DrawChunks(ChunkMap& chunkMap, const Vector3& playerPos, const Vector3& highlightedBlockPos, Camera3D& camera, std::shared_mutex& chunkMapMutex);