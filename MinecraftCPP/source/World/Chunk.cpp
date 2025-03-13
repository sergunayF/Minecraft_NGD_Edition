#include "Chunk.hpp"

#include "../Player/Player.hpp"

std::atomic<bool> isUpdatingChunks = false;

std::mutex fileMutex;

float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

int GetRegionAt(float worldX, float worldZ) {
    double regionNoise = perlin.noise(worldX * 0.01, worldZ * 0.01, 0);
    if (regionNoise < 0.25) return 1; // Регион 1
    return 2; // Регион 2
}

int GetBiomeAt(float worldX, float worldZ) {
    float noiseValue = perlin.noise(worldX * 0.005, worldZ * 0.005, 0);
    int region = GetRegionAt(worldX, worldZ);

    if (region == 1) {
        if (noiseValue < 0.2) return 1; // Пустыня
        if (noiseValue < 0.4) return 2; // Лес
        if (noiseValue < 0.6) return 3; // Горы
        return 7; // Саванна
    }
    else if (region == 2) {
        if (noiseValue < 0.2) return 4; // Тундра
        if (noiseValue < 0.4) return 5; // Тайга
        if (noiseValue < 0.6) return 6; // Холмы
        return 8; // Снежная тундра
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

            if (surface_height >= WATER_LEVEL) {
                if (biomeID == 2 && (rand() % 100) < 4) {
                    GenerateTree(x, surface_height + 1, z);
                }
                else if (biomeID == 5 && (rand() % 100) < 4) {
                    GenerateSpruceTree(x, surface_height + 1, z);
                }
                else if (biomeID == 4 && (rand() % (3 * CHUNK_SIZE_X * CHUNK_SIZE_Z)) < 1) {
                    GenerateTree(x, surface_height + 1, z);
                }
                else if (biomeID == 6 && (rand() % (3 * CHUNK_SIZE_X * CHUNK_SIZE_Z)) < 1) {
                    GenerateTree(x, surface_height + 1, z);
                }
                else if (biomeID == 8 && (rand() % 100) < 4) {
                    GenerateSpruceTree(x, surface_height + 1, z);
                }
            }

            if (surface_height >= WATER_LEVEL) {
                if (biomeID == 2 && (rand() % 100) < 50) {
                    Vector3 grass_pos = { worldX, static_cast<float>(surface_height + 1), worldZ };
                    if (!HasBlockAt(grass_pos)) {
                        blockMap[grass_pos] = Block(31.1f, grass_pos.x, grass_pos.y, grass_pos.z);
                    }
                }
                else if (biomeID == 5) {
                    if ((rand() % 100) < 50) {
                        Vector3 grass_pos = { worldX, static_cast<float>(surface_height + 1), worldZ };
                        if (!HasBlockAt(grass_pos)) {
                            blockMap[grass_pos] = Block(31.1f, grass_pos.x, grass_pos.y, grass_pos.z);
                        }
                    }
                    if ((rand() % 100) < 10) {
                        Vector3 fern_pos = { worldX, static_cast<float>(surface_height + 1), worldZ };
                        if (!HasBlockAt(fern_pos)) {
                            blockMap[fern_pos] = Block(31.2f, fern_pos.x, fern_pos.y, fern_pos.z);
                        }
                    }
                }
            }

            if (surface_height >= WATER_LEVEL && (rand() % 1000) < 5 && biomeID == 1) { // Пустыня
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
        float angleChange = (rand() % 10 - 5) * 3.14159265 / 180.0;
        float pitchChange = (rand() % 6 - 3) * 3.14159265 / 180.0;

        int length = rand() % 150 + 100;

        for (int j = 0; j < length; j++) {
            x += cos(angle) * cos(pitch) * step;
            y += sin(pitch) * step;
            z += sin(angle) * cos(pitch) * step;

            Vector3 center = { static_cast<int>(x), static_cast<int>(y), static_cast<int>(z) };
            int radius = 2 + rand() % 3;

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

            angle += angleChange;
            pitch += pitchChange;
            angleChange += (rand() % 6 - 3) * 3.14159265 / 180.0;
            pitchChange += (rand() % 4 - 2) * 3.14159265 / 180.0;
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

void Chunk::GenerateSpruceTree(int x, int y, int z) {
    int treeHeight = rand() % 4 + 6;

    for (int i = 0; i < treeHeight; i++) {
        Vector3 pos = { worldPos.x * CHUNK_SIZE_X + x, static_cast<float>(y + i), worldPos.y * CHUNK_SIZE_Z + z };
        blockMap[pos] = Block(17, pos.x, pos.y, pos.z);
    }

    for (int dy = 0; dy < 3; dy++) {
        int radius = 2 - dy;
        for (int dx = -radius; dx <= radius; dx++) {
            for (int dz = -radius; dz <= radius; dz++) {
                if (abs(dx) + abs(dz) <= radius + 1) {
                    Vector3 pos = { worldPos.x * CHUNK_SIZE_X + x + dx, static_cast<float>(y + treeHeight - 2 + dy), worldPos.y * CHUNK_SIZE_Z + z + dz };
                    if (!HasBlockAt(pos)) {
                        blockMap[pos] = Block(18, pos.x, pos.y, pos.z);
                    }
                }
            }
        }
    }
}

bool Chunk::HasBlockAt(const Vector3& pos) {
    return blockMap.find(pos) != blockMap.end();
}

Block* Chunk::GetBlockAt(const Vector3& pos, ChunkMap& chunkMap) {
    int localX = static_cast<int>(pos.x) - static_cast<int>(worldPos.x * CHUNK_SIZE_X);
    int localZ = static_cast<int>(pos.z) - static_cast<int>(worldPos.y * CHUNK_SIZE_Z);

    if (localX < 0 || localX >= CHUNK_SIZE_X || localZ < 0 || localZ >= CHUNK_SIZE_Z) {
        int neighborChunkX = worldPos.x + (localX < 0 ? -1 : (localX >= CHUNK_SIZE_X ? 1 : 0));
        int neighborChunkZ = worldPos.y + (localZ < 0 ? -1 : (localZ >= CHUNK_SIZE_Z ? 1 : 0));
        Vector2 neighborPos = { neighborChunkX, neighborChunkZ };

        if (chunkMap.find(neighborPos) != chunkMap.end()) {
            Block* block = chunkMap[neighborPos].GetBlockAt(pos, chunkMap);
            if (block && !blockDataMap[getTexture(block->id)].billboard) {
                return block;
            }
        }
        return nullptr;
    }

    auto it = blockMap.find(pos);
    return (it != blockMap.end() && !blockDataMap[getTexture(it->second.id)].billboard) ? &(it->second) : nullptr;
}


void Chunk::Update(ChunkMap& chunkMap) {

    renderedBlocks.clear();

    for (auto& block : blockMap) {
        Vector3 pos = block.first;

        block.second.neighbors[0] = GetBlockAt({pos.x + 1, pos.y, pos.z}, chunkMap); // right
        block.second.neighbors[1] = GetBlockAt({ pos.x - 1, pos.y, pos.z }, chunkMap); // left
        block.second.neighbors[2] = GetBlockAt({ pos.x, pos.y + 1, pos.z }, chunkMap); // top
        block.second.neighbors[3] = GetBlockAt({ pos.x, pos.y - 1, pos.z }, chunkMap); // bottom
        block.second.neighbors[4] = GetBlockAt({ pos.x, pos.y, pos.z + 1 }, chunkMap); // front
        block.second.neighbors[5] = GetBlockAt({ pos.x, pos.y, pos.z - 1 }, chunkMap); // back

        if (pos.y == 0) {
            block.second.rendered =
                !(block.second.neighbors[0] && block.second.neighbors[1] &&
                    block.second.neighbors[2] && block.second.neighbors[4] &&
                    block.second.neighbors[5]);
        }
        else {
            block.second.rendered =
                !(block.second.neighbors[0] && block.second.neighbors[1] &&
                    block.second.neighbors[2] && block.second.neighbors[3] &&
                    block.second.neighbors[4] && block.second.neighbors[5]);
        }

        if (block.second.rendered) {
            renderedBlocks.push_back(&block.second);
        }
    }
}


void Chunk::UpdateNeighborBlocks(const Vector3& blockPos, ChunkMap& chunkMap, bool isBlockDestroyed) {

    static const Vector3 neighbors[6] = {
        {  1,  0,  0 }, { -1,  0,  0 },  // Вправо, влево
        {  0,  1,  0 }, {  0, -1,  0 },  // Вверх, вниз
        {  0,  0,  1 }, {  0,  0, -1 }   // Вперед, назад
    };

    Vector2 chunkPos = { floor(blockPos.x / CHUNK_SIZE_X), floor(blockPos.z / CHUNK_SIZE_Z) };

    if (!chunkMap.count(chunkPos)) return;

    chunkMap[chunkPos].Update(chunkMap);

    for (int i = 0; i < 6; ++i) {
        Vector3 neighborPos = { blockPos.x + neighbors[i].x, blockPos.y + neighbors[i].y, blockPos.z + neighbors[i].z };

        Vector2 neighborChunkPos = { floor(neighborPos.x / CHUNK_SIZE_X), floor(neighborPos.z / CHUNK_SIZE_Z) };

        if (chunkMap.count(neighborChunkPos)) {
            Chunk& neighborChunk = chunkMap[neighborChunkPos];

            if (isBlockDestroyed) {
                for (Block* neighborBlock : neighborChunk.renderedBlocks) {
                    if (!neighborBlock->rendered) {
                        neighborChunk.Update(chunkMap);
                        break;
                    }
                }
            }
            else {
                for (Block* neighborBlock : neighborChunk.renderedBlocks) {
                    if (neighborBlock->rendered) {
                        neighborChunk.Update(chunkMap);
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
            it->second.Update(chunkMap);
        }
    }
}

void Chunk::SaveToFile(const std::string& savePath) {
    if (!IsChanged) return;

    //printf("\nSave Chunk: %lf, %lf", worldPos.x, worldPos.y);

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

    //printf("\nLoad Chunk: %lf, %lf", worldPos.x, worldPos.y);

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
    IsLoaded = true;

}

void Chunk::Draw(Player& player, Camera3D& camera, ChunkMap& chunkMap) {

    std::vector<Block*> opaqueBlocks;
    std::vector<Block*> transparentBlocks;

    Vector3 camPos = camera.position;

    for (Block* block : renderedBlocks) {

        if (block == nullptr) continue;
        if (reinterpret_cast<uintptr_t>(block) > 0xFFFFFFFFFFFF) continue;

        float dx = std::abs(block->position.x - camPos.x);
        float dy = std::abs(block->position.y - camPos.y);
        float dz = std::abs(block->position.z - camPos.z);

        if (dx > RENDER_DISTANCE * 16 || dy > 8 * FOG_DISTANCE || dz > RENDER_DISTANCE * 16) continue;

        if (block->transparency) transparentBlocks.push_back(block);
        else opaqueBlocks.push_back(block);

    }

    auto sortByDistance = [&](Block* a, Block* b) {
        return DistanceSquared(camPos, a->position) > DistanceSquared(camPos, b->position);
        };

    std::stable_sort(opaqueBlocks.begin(), opaqueBlocks.end(), sortByDistance);
    std::stable_sort(transparentBlocks.begin(), transparentBlocks.end(), sortByDistance);

    for (Block* block : opaqueBlocks) {
        bool isHighlighted = (block->position.x == player.highlightedBlockPos.x &&
            block->position.y == player.highlightedBlockPos.y &&
            block->position.z == player.highlightedBlockPos.z);

        if (block->id == 61 || block->id == 62) static_cast<FurnaceBlock*>(block)->Update(player);

        block->Draw(isHighlighted);
    }


    for (Block* block : transparentBlocks) {
        bool isHighlighted = (block->position.x == player.highlightedBlockPos.x &&
            block->position.y == player.highlightedBlockPos.y &&
            block->position.z == player.highlightedBlockPos.z);
        block->Draw(isHighlighted);
    
    }
}

void DrawChunks(ChunkMap& chunkMap, Player& player, Camera3D& camera, std::shared_mutex& chunkMapMutex) {
    std::shared_lock<std::shared_mutex> lock(chunkMapMutex);

    std::vector<std::pair<Chunk*, float>> sortedChunks;

    for (auto& chunk : chunkMap) {
        if (chunk.second.IsLoaded) {
            float distance = DistanceSquared({ floor(player.position.x / 16), 0, floor(player.position.z / 16) }, { chunk.second.worldPos.x, 0, chunk.second.worldPos.y });
            sortedChunks.emplace_back(&chunk.second, distance);
        }
    }

    auto sortByDistance = [](const std::pair<Chunk*, float>& a, const std::pair<Chunk*, float>& b) {
        return a.second > b.second; 
        };

    std::stable_sort(sortedChunks.begin(), sortedChunks.end(), sortByDistance);

    for (auto& chunk : sortedChunks) {
        if (chunk.first != nullptr) {
            chunk.first->Draw(player, camera, chunkMap);
        }
    }
}