#include "Chunk.hpp"

std::atomic<bool> isUpdatingChunks = false;

std::mutex fileMutex;

float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

int GetRegionAt(float worldX, float worldZ) {
    double regionNoise = perlin.noise(worldX * 0.01, worldZ * 0.01, 0);
    if (regionNoise < 0.5) return 1; // Регион 1
    return 2; // Регион 2
}

int GetBiomeAt(float worldX, float worldZ) {

    float noiseValue = perlin.noise(worldX * 0.005, worldZ * 0.005, 0);
    int region = GetRegionAt(worldX, worldZ);

    if (region == 1) {
        if (noiseValue < 0.2) return 1; // Пустыня
        if (noiseValue < 0.4) return 2; // Лес
        return 3; // Горы
    }
    else if (region == 2) {

        return 4;

        //if (noiseValue < 0.2) return 4; // Тундра
        //if (noiseValue < 0.4) return 5; // Тайга
        //return 6; // Холмы
    }

    return 1;
}

void Chunk::GenerateChunk() {

    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int z = 0; z < CHUNK_SIZE_Z; z++) {

            float worldX = worldPos.x * CHUNK_SIZE_X + x;
            float worldZ = worldPos.y * CHUNK_SIZE_Z + z;

            int surface_height;

            int biomeID = GetBiomeAt(worldX, worldZ);
            Biome biome(biomeID);

            float base_height = WATER_LEVEL + 3;
            float height_variation = perlin.noise(worldX * 0.02, 0, worldZ * 0.02) * 5;

            if (biomeID == 3) { // ГОРЫ
                float transition = perlin.noise(worldX * 0.01, worldZ * 0.01, 0);
                float blendFactor = transition * transition;

                float mountainHeight = 120;
                float prevBiomeHeight = base_height + height_variation;

                float finalHeight = Lerp(prevBiomeHeight, mountainHeight, blendFactor);

                surface_height = static_cast<int>(finalHeight);
            }
            else {
                surface_height = static_cast<int>(base_height + height_variation);
            }

            for (int y = 0; y < surface_height - 4 + rand() % 3; y++) {
                Vector3 pos = { worldX, static_cast<float>(y), worldZ };
                if (!HasBlockAt(pos)) blockMap[pos] = Block(1, pos.x, pos.y, pos.z); // Камень
            }

            for (int y = surface_height - 4; y < surface_height; y++) {
                Vector3 pos = { worldX, static_cast<float>(y), worldZ };
                if (!HasBlockAt(pos)) blockMap[pos] = Block(biome.underBlock, pos.x, pos.y, pos.z);
            }

            Vector3 surface_pos = { worldX, static_cast<float>(surface_height), worldZ };
            if (!HasBlockAt(surface_pos)) blockMap[surface_pos] = Block(biome.topBlock, surface_pos.x, surface_pos.y, surface_pos.z);

            if (surface_height >= WATER_LEVEL && (rand() % 100) < 4 && biomeID == 2) {
                GenerateTree(x, surface_height + 1, z);
            }

            if (surface_height >= WATER_LEVEL && (rand() % 1000) < 5 && biomeID == 1) {
                for (int cactusHeight = rand() % 3 + 1; cactusHeight != 0; cactusHeight--) {
                    Vector3 cactus_pos = { worldX, surface_height + cactusHeight, worldZ };
                    if (!HasBlockAt(cactus_pos)) blockMap[cactus_pos] = Block(91, cactus_pos.x, cactus_pos.y, cactus_pos.z);
                }
            }

            if (surface_height >= WATER_LEVEL && (rand() % 100) < 2) {
                Vector3 flower_pos = { worldX, static_cast<float>(surface_height + 1), worldZ };
                if (!HasBlockAt(flower_pos) && biomeID == 2) blockMap[flower_pos] = Block(22 + rand() % 2, flower_pos.x, flower_pos.y, flower_pos.z);
                else if (!HasBlockAt(flower_pos) && biomeID == 1 && rand() % 10 < 2) blockMap[flower_pos] = Block(32, flower_pos.x, flower_pos.y, flower_pos.z);
            }
        }
    }

    GenerateCaves();

    GenerateOre(14, 10, 35, 8, 2); // Золото
    GenerateOre(15, 8, 70, 8, 2);  // Железо
    GenerateOre(16, 5, 130, 12, 3); // Уголь
    GenerateOre(13, 5, 130, 64, 4); // Гравий

}

void Chunk::GenerateCaves() {

    int numCaves = rand() % 3 + 2;

    for (int i = 0; i < numCaves; i++) {
        float x = worldPos.x * CHUNK_SIZE_X + rand() % CHUNK_SIZE_X;
        float y = rand() % (WATER_LEVEL - 10) + 10;
        float z = worldPos.y * CHUNK_SIZE_Z + rand() % CHUNK_SIZE_Z;

        float angle = (rand() % 360) * 3.14159265 / 180.0;
        float pitch = ((rand() % 60) - 30) * 3.14159265 / 180.0;
        float step = 1.0f;

        int length = rand() % 100 + 100;

        for (int j = 0; j < length; j++) {
            x += cos(angle) * cos(pitch) * step;
            y += sin(pitch) * step;
            z += sin(angle) * cos(pitch) * step;

            Vector3 center = { static_cast<int>(x), static_cast<int>(y), static_cast<int>(z) };

            int radius = 1 + rand() % 2;

            for (int dx = -radius; dx <= radius; dx++) {
                for (int dy = -radius / 2; dy <= radius / 2; dy++) {
                    for (int dz = -radius; dz <= radius; dz++) {
                        if ((dx * dx + dy * dy * 2 + dz * dz) <= radius * radius) {
                            Vector3 pos = { center.x + dx, center.y + dy, center.z + dz };
                            if (blockMap.find(pos) != blockMap.end()) {
                                blockMap.erase(pos);

                            }
                        }
                    }
                }
            }

            angle += (rand() % 15 - 7) * 3.14159265 / 180.0;
            pitch += (rand() % 8 - 4) * 3.14159265 / 180.0;
        }
    }
}

void Chunk::GenerateOre(int oreID, int minY, int maxY, int maxSize, int clusterSize) {

    int oreCount = (CHUNK_SIZE_X * CHUNK_SIZE_Z) / 16;

    for (int i = 0; i < oreCount; i++) {
        int x = rand() % CHUNK_SIZE_X;
        int z = rand() % CHUNK_SIZE_Z;
        int y = rand() % (maxY - minY + 1) + minY;

        int clusterBlocks = rand() % (maxSize - 1) + 1;

        if (oreID == 13) clusterBlocks = rand() % (maxSize - 17) + 16;

        for (int j = 0; j < clusterBlocks; j++) {
            int dx = rand() % clusterSize;
            int dy = rand() % clusterSize;
            int dz = rand() % clusterSize;

            TryPlaceOre(oreID, x + dx, y + dy, z + dz);
        }
    }
}

void Chunk::TryPlaceOre(int oreID, int x, int y, int z) {
    Vector3 pos = { worldPos.x * CHUNK_SIZE_X + x, static_cast<float>(y), worldPos.y * CHUNK_SIZE_Z + z };

    if (blockMap.find(pos) != blockMap.end() && blockMap[pos].id == 1) {
        blockMap[pos] = Block(oreID, pos.x, pos.y, pos.z);
    }
}

void Chunk::GenerateTree(int x, int y, int z) {

    int tree_height = 4 + (rand() % 2);

    for (int i = 0; i < tree_height; i++) {
        Vector3 trunk_pos = { worldPos.x * CHUNK_SIZE_X + x, static_cast<float>(y + i), worldPos.y * CHUNK_SIZE_Z + z };
        if (!HasBlockAt(trunk_pos)) blockMap[trunk_pos] = Block(17, trunk_pos.x, trunk_pos.y, trunk_pos.z);
    }

    int leaves_start = y + tree_height - 2;

    for (int dx = -2; dx <= 2; dx++) {
        for (int dz = -2; dz <= 2; dz++) {
            for (int dy = 0; dy <= 2; dy++) {
                if (abs(dx) + abs(dz) + dy < 4) {
                    Vector3 leaf_pos = { worldPos.x * CHUNK_SIZE_X + x + dx, static_cast<float>(leaves_start + dy), worldPos.y * CHUNK_SIZE_Z + z + dz };
                    if (!HasBlockAt(leaf_pos)) blockMap[leaf_pos] = Block(18, leaf_pos.x, leaf_pos.y, leaf_pos.z);
                }
            }
        }
    }
}

bool Chunk::HasBlockAt(const Vector3& pos) {
    return blockMap.find(pos) != blockMap.end();
}

Block* Chunk::GetBlockAt(const Vector3& pos, ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex) {

    std::shared_lock<std::shared_mutex> lock(chunkMapMutex);

    int localX = static_cast<int>(pos.x) - static_cast<int>(worldPos.x * CHUNK_SIZE_X);
    int localZ = static_cast<int>(pos.z) - static_cast<int>(worldPos.y * CHUNK_SIZE_Z);

    auto it = blockMap.find(pos);
    if (it != blockMap.end()) {

        if (it->second.transparency) return nullptr;

        return &it->second;
    }

    int offsetX = (localX < 0) ? -1 : (localX >= CHUNK_SIZE_X ? 1 : 0);
    int offsetZ = (localZ < 0) ? -1 : (localZ >= CHUNK_SIZE_Z ? 1 : 0);

    if (offsetX == 0 && offsetZ == 0) return nullptr;

    Vector2 neighborPos = { worldPos.x + offsetX, worldPos.y + offsetZ };
    auto itChunk = chunkMap.find(neighborPos);
    if (itChunk == chunkMap.end()) return reinterpret_cast<Block*>(1);

    Vector3 neighborBlockPos = { pos.x, pos.y, pos.z };
    return itChunk->second.GetBlockAt(neighborBlockPos, chunkMap, chunkMapMutex);

}

void Chunk::Update(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex) {
    
    std::shared_lock<std::shared_mutex> lock(chunkMapMutex);

    renderedBlocks.clear();

    for (auto& block : blockMap) {

        Vector3 pos = block.first;

        if (pos.y == 0) {
            block.second.rendered =
                !(GetBlockAt({ pos.x + 1, pos.y, pos.z }, chunkMap, chunkMapMutex) &&
                    GetBlockAt({ pos.x - 1, pos.y, pos.z }, chunkMap, chunkMapMutex) &&
                    GetBlockAt({ pos.x, pos.y + 1, pos.z }, chunkMap, chunkMapMutex) &&
                    GetBlockAt({ pos.x, pos.y, pos.z + 1 }, chunkMap, chunkMapMutex) &&
                    GetBlockAt({ pos.x, pos.y, pos.z - 1 }, chunkMap, chunkMapMutex));
        }
        else {
            block.second.rendered =
                !(GetBlockAt({ pos.x + 1, pos.y, pos.z }, chunkMap, chunkMapMutex) &&
                    GetBlockAt({ pos.x - 1, pos.y, pos.z }, chunkMap, chunkMapMutex) &&
                    GetBlockAt({ pos.x, pos.y + 1, pos.z }, chunkMap, chunkMapMutex) &&
                    GetBlockAt({ pos.x, pos.y - 1, pos.z }, chunkMap, chunkMapMutex) &&
                    GetBlockAt({ pos.x, pos.y, pos.z + 1 }, chunkMap, chunkMapMutex) &&
                    GetBlockAt({ pos.x, pos.y, pos.z - 1 }, chunkMap, chunkMapMutex));
        }

        if (block.second.rendered) {
            renderedBlocks.push_back(&block.second);
        }
    }
}


void Chunk::UpdateNeighborBlocks(const Vector3& blockPos, ChunkMap& chunkMap, bool isBlockDestroyed, std::shared_mutex& chunkMapMutex) {

    std::shared_lock<std::shared_mutex> lock(chunkMapMutex);

    static const Vector3 neighbors[6] = {
        {  1,  0,  0 }, { -1,  0,  0 },  // Вправо, влево
        //{  0,  1,  0 }, {  0, -1,  0 },  // Вверх, вниз
        {  0,  0,  1 }, {  0,  0, -1 }   // Вперед, назад
    };

    Vector2 chunkPos = { floor(blockPos.x / CHUNK_SIZE_X), floor(blockPos.z / CHUNK_SIZE_Z) };

    if (!chunkMap.count(chunkPos)) return;

    chunkMap[chunkPos].Update(chunkMap, chunkMapMutex);

    for (const auto& offset : neighbors) {
        Vector3 neighborPos = { blockPos.x + offset.x, blockPos.y + offset.y, blockPos.z + offset.z };

        Vector2 neighborChunkPos = { floor(neighborPos.x / CHUNK_SIZE_X), floor(neighborPos.z / CHUNK_SIZE_Z) };

        if (chunkMap.count(neighborChunkPos)) {
            Chunk& neighborChunk = chunkMap[neighborChunkPos];

            if (isBlockDestroyed) {
                for (Block* neighborBlock : neighborChunk.renderedBlocks) {
                    if (!neighborBlock->rendered) {
                        neighborChunk.Update(chunkMap, chunkMapMutex);
                        break;
                    }
                }
            }
            else {
                for (Block* neighborBlock : neighborChunk.renderedBlocks) {
                    if (neighborBlock->rendered) {
                        neighborChunk.Update(chunkMap, chunkMapMutex);
                        break;
                    }
                }
            }
        }
    }
}

void Chunk::UpdateNeighborChunks(ChunkMap& chunkMap, const Vector2& chunkPos, std::shared_mutex& chunkMapMutex) {

    std::vector<Vector2> neighbors = {
        {chunkPos.x - 1, chunkPos.y}, {chunkPos.x + 1, chunkPos.y},
        {chunkPos.x, chunkPos.y - 1}, {chunkPos.x, chunkPos.y + 1}
    };

    for (const auto& neighborPos : neighbors) {
        auto it = chunkMap.find(neighborPos);
        if (it != chunkMap.end()) {
            it->second.Update(chunkMap, chunkMapMutex);
        }
    }
}

void Chunk::SaveToFile(const std::string& savePath) {
    if (!IsChanged) return;

    std::lock_guard<std::mutex> lock(fileMutex);

    std::string filename = savePath + "/" + std::to_string(static_cast<int>(worldPos.x)) + "_" +
        std::to_string(static_cast<int>(worldPos.y)) + ".dat";
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Ошибка сохранения чанка: " << filename << std::endl;
        return;
    }

    size_t blockCount = blockMap.size();
    file.write(reinterpret_cast<const char*>(&blockCount), sizeof(blockCount));

    for (const auto& block : blockMap) {
        std::string serializedBlock = block.second.Serialize();
        size_t length = serializedBlock.size();
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        file.write(serializedBlock.c_str(), length);
    }

    file.close();
    IsChanged = false;
}

void Chunk::LoadFromFile(const std::string& savePath) {
    std::string filename = savePath + "/" + std::to_string(static_cast<int>(worldPos.x)) + "_" +
        std::to_string(static_cast<int>(worldPos.y)) + ".dat";

    std::lock_guard<std::mutex> lock(fileMutex);

    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        return;
    }

    size_t blockCount;
    file.read(reinterpret_cast<char*>(&blockCount), sizeof(blockCount));

    if (!file) {
        std::cerr << "Ошибка чтения файла чанка: " << filename << std::endl;
        return;
    }

    blockMap.clear();

    for (size_t i = 0; i < blockCount; ++i) {
        size_t length;
        file.read(reinterpret_cast<char*>(&length), sizeof(length));

        std::string serializedBlock(length, '\0');
        file.read(&serializedBlock[0], length);

        if (!file) {
            std::cerr << "Ошибка чтения блока в файле: " << filename << std::endl;
            break;
        }

        Block block = Block::Deserialize(serializedBlock);
        blockMap[block.position] = block;
    }

    file.close();
    IsChanged = false;
}

void Chunk::Draw(const Vector3& highlightedBlockPos, Camera3D& camera, ChunkMap& chunkMap) {

    std::vector<Block*> opaqueBlocks;
    std::vector<Block*> transparentBlocks;

    Vector3 camPos = camera.position;

    for (Block* block : renderedBlocks) {
        if (block == nullptr) continue;
        if (reinterpret_cast<uintptr_t>(block) > 0xFFFFFFFFFFFF) continue;

        if (block->transparency) {
            transparentBlocks.push_back(block);
        }
        else {
            opaqueBlocks.push_back(block);
        }
    }

    auto sortByDistance = [&](Block* a, Block* b) {
        return DistanceSquared(camPos, a->position) > DistanceSquared(camPos, b->position);
        };

    std::stable_sort(opaqueBlocks.begin(), opaqueBlocks.end(), sortByDistance);
    std::stable_sort(transparentBlocks.begin(), transparentBlocks.end(), sortByDistance);

    for (Block* block : opaqueBlocks) {
        bool isHighlighted = (block->position.x == highlightedBlockPos.x &&
            block->position.y == highlightedBlockPos.y &&
            block->position.z == highlightedBlockPos.z);
        block->Draw(isHighlighted);
    }

    for (Block* block : transparentBlocks) {
        bool isHighlighted = (block->position.x == highlightedBlockPos.x &&
            block->position.y == highlightedBlockPos.y &&
            block->position.z == highlightedBlockPos.z);
        block->Draw(isHighlighted);
    }
}

void DrawChunks(ChunkMap& chunkMap, const Vector3& playerPos, const Vector3& highlightedBlockPos, Camera3D& camera, std::shared_mutex& chunkMapMutex) {

    std::shared_lock<std::shared_mutex> lock(chunkMapMutex);

    std::vector<Chunk*> sortedChunks;

    for (auto& chunk : chunkMap) {
        if (chunk.second.IsLoaded) {
            sortedChunks.push_back(&chunk.second);
        }
    }

    auto sortByDistance = [&](Chunk* a, Chunk* b) {
        return DistanceSquared({ floor(playerPos.x / 16), 0, floor(playerPos.z/16) }, { a->worldPos.x, 0, a->worldPos.y }) > DistanceSquared(playerPos, { b->worldPos.x * 16, 0, b->worldPos.y * 16 });
        };

    std::stable_sort(sortedChunks.begin(), sortedChunks.end(), sortByDistance);

    for (Chunk* chunk : sortedChunks) {
        if (chunk != nullptr) {
            chunk->Draw(highlightedBlockPos, camera, chunkMap);
        }
    }
}