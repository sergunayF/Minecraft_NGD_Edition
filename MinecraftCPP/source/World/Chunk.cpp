#include "Chunk.hpp"

void Chunk::GenerateChunk() {

    for (int x = 0; x < CHUNK_SIZE_X; x++) {

        for (int z = 0; z < CHUNK_SIZE_Z; z++) {

            float height_noise = perlin.noise(
                (x + worldPos.x * CHUNK_SIZE_X) * 0.02,
                0,
                (z + worldPos.y * CHUNK_SIZE_Z) * 0.02
            );

            int base_height = WATER_LEVEL + 3;
            int height_variation = static_cast<int>(height_noise * 5);
            int surface_height = base_height + height_variation;

            for (int y = 0; y < surface_height - 4 + rand() % 3; y++) {
                Vector3 pos = { worldPos.x * CHUNK_SIZE_X + x, static_cast<float>(y), worldPos.y * CHUNK_SIZE_Z + z };
                if (!HasBlockAt(pos)) blockMap[pos] = Block(1, pos.x, pos.y, pos.z); // Камень
            }

            for (int y = surface_height - 4; y < surface_height; y++) {
                Vector3 pos = { worldPos.x * CHUNK_SIZE_X + x, static_cast<float>(y), worldPos.y * CHUNK_SIZE_Z + z };
                if (!HasBlockAt(pos)) blockMap[pos] = Block(3, pos.x, pos.y, pos.z); // Земля
            }

            Vector3 surface_pos = { worldPos.x * CHUNK_SIZE_X + x, static_cast<float>(surface_height), worldPos.y * CHUNK_SIZE_Z + z };
            if (!HasBlockAt(surface_pos)) blockMap[surface_pos] = Block(2, surface_pos.x, surface_pos.y, surface_pos.z); // Трава

            if (surface_height < WATER_LEVEL) {
                for (int y = surface_height + 1; y <= WATER_LEVEL; y++) {
                    Vector3 water_pos = { worldPos.x * CHUNK_SIZE_X + x, static_cast<float>(y), worldPos.y * CHUNK_SIZE_Z + z };
                    if (!HasBlockAt(water_pos)) blockMap[water_pos] = Block(7, water_pos.x, water_pos.y, water_pos.z);
                }
            }

            if (surface_height >= WATER_LEVEL && (rand() % 100) < 5) {
                GenerateTree(x, surface_height + 1, z);
            }
        }
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

Block* Chunk::GetBlockAt(const Vector3& pos, ChunkMap& chunkMap) {
    int localX = static_cast<int>(pos.x) - static_cast<int>(worldPos.x * CHUNK_SIZE_X);
    int localZ = static_cast<int>(pos.z) - static_cast<int>(worldPos.y * CHUNK_SIZE_Z);

    auto it = blockMap.find(pos);
    if (it != blockMap.end()) return &it->second;

    int offsetX = (localX < 0) ? -1 : (localX >= CHUNK_SIZE_X ? 1 : 0);
    int offsetZ = (localZ < 0) ? -1 : (localZ >= CHUNK_SIZE_Z ? 1 : 0);

    if (offsetX == 0 && offsetZ == 0) return nullptr;

    Vector2 neighborPos = { worldPos.x + offsetX, worldPos.y + offsetZ };
    auto itChunk = chunkMap.find(neighborPos);
    if (itChunk == chunkMap.end()) return reinterpret_cast<Block*>(1);

    Vector3 neighborBlockPos = { pos.x, pos.y, pos.z };
    return itChunk->second.GetBlockAt(neighborBlockPos, chunkMap);
}

void Chunk::Update(ChunkMap& chunkMap) {
        renderedBlocks.clear();

        for (auto& block : blockMap) {
            Vector3 pos = block.first;

            if (pos.y == 0) {
                block.second.rendered =
                    !(GetBlockAt({ pos.x + 1, pos.y, pos.z }, chunkMap) &&
                        GetBlockAt({ pos.x - 1, pos.y, pos.z }, chunkMap) &&
                        GetBlockAt({ pos.x, pos.y + 1, pos.z }, chunkMap) &&
                        GetBlockAt({ pos.x, pos.y, pos.z + 1 }, chunkMap) &&
                        GetBlockAt({ pos.x, pos.y, pos.z - 1 }, chunkMap));
            }
            else {
                block.second.rendered =
                    !(GetBlockAt({ pos.x + 1, pos.y, pos.z }, chunkMap) &&
                        GetBlockAt({ pos.x - 1, pos.y, pos.z }, chunkMap) &&
                        GetBlockAt({ pos.x, pos.y + 1, pos.z }, chunkMap) &&
                        GetBlockAt({ pos.x, pos.y - 1, pos.z }, chunkMap) &&
                        GetBlockAt({ pos.x, pos.y, pos.z + 1 }, chunkMap) &&
                        GetBlockAt({ pos.x, pos.y, pos.z - 1 }, chunkMap));
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

    for (const auto& offset : neighbors) {
        Vector3 neighborPos = { blockPos.x + offset.x, blockPos.y + offset.y, blockPos.z + offset.z };

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


void Chunk::Draw(const Vector3& highlightedBlockPos) {

    for (Block* block : renderedBlocks) {

        if (block == nullptr) continue;

        if (reinterpret_cast<uintptr_t>(block) > 0xFFFFFFFFFFFF) continue;

        bool isHighlighted = (block->position.x == highlightedBlockPos.x &&
            block->position.y == highlightedBlockPos.y &&
            block->position.z == highlightedBlockPos.z);

        block->Draw(isHighlighted);
    }
}

