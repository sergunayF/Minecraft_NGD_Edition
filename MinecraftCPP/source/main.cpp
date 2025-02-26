// BASE GAME INCLUDES

#include "Game/Game.hpp"
#include "Game/Noise.hpp"
#include "Game/Textures.hpp"
#include "Game/DeBugScreen.hpp"

// PLAYER INCLUDES

#include "Player/Player.hpp"
#include "Player/Camera.hpp"

// WORLD INCLUDES

#include "Block/Block.hpp"
#include "World/Chunk.hpp"
#include "World/Skybox.hpp"

//

#include <GLFW/glfw3.h>

// MULTYTHREADS

std::shared_mutex chunkMapMutex;

void UpdateChunks(ChunkMap& chunkMap, const Vector3& playerPosition) {

    std::unique_lock<std::shared_mutex> lock(chunkMapMutex);

    int playerChunkX = static_cast<int>(playerPosition.x) / Chunk::CHUNK_SIZE_X;
    int playerChunkZ = static_cast<int>(playerPosition.z) / Chunk::CHUNK_SIZE_Z;

    std::unordered_set<Vector2, Vector2Hash, Vector2Equal> neededChunks;

    for (int x = playerChunkX - RENDER_DISTANCE; x <= playerChunkX + RENDER_DISTANCE; x++) {
        for (int z = playerChunkZ - RENDER_DISTANCE; z <= playerChunkZ + RENDER_DISTANCE; z++) {
            neededChunks.insert({ static_cast<float>(x), static_cast<float>(z) });
        }
    }

    for (const auto& chunkPos : neededChunks) {
        if (chunkMap.find(chunkPos) == chunkMap.end()) {
            chunkMap[chunkPos] = Chunk(chunkPos.x, chunkPos.y);
            chunkMap[chunkPos].IsLoaded = true;
            chunkMap[chunkPos].Update(chunkMap);
        }
    }

    for (auto it = chunkMap.begin(); it != chunkMap.end();) {
        if (neededChunks.find(it->first) == neededChunks.end()) {
            it->second.IsLoaded = false;
            it->second.renderedBlocks.clear();
            it = chunkMap.erase(it);
        }
        else {
            ++it;
        }
    }

    isUpdatingChunks = false;
}

int main() {

    glfwInitHint(GLFW_ANGLE_PLATFORM_TYPE, GLFW_ANGLE_PLATFORM_TYPE_METAL);
    InitWindow(screenWidth, screenHeight, "Minecraft: NosovEdition 0.04a"); // Caves && Biomes Update
    
    SetTargetFPS(60);
    DisableCursor();
    loadTextures();

    srand(worldSeed);
    blockDataMap = loadBlockData();

    Camera3D camera = InitCamera();
    Player player(0, 80, 0);

    ChunkMap chunkMap;
    UpdateChunks(chunkMap, player.position);

    player.inventory.push_back(1.1);

    int currentPlayerChunkX = static_cast<int>(player.position.x) / Chunk::CHUNK_SIZE_X;
    int currentPlayerChunkZ = static_cast<int>(player.position.z) / Chunk::CHUNK_SIZE_Z;

    int timeOfDay = 0;
    std::future<void> chunkUpdateTask;

    while (!WindowShouldClose()) {

        UpdateCameraRotation(player);

        player.Update(chunkMap);

        UpdateCameraPosition(camera, player);

        int newPlayerChunkX = static_cast<int>(player.position.x) / Chunk::CHUNK_SIZE_X;
        int newPlayerChunkZ = static_cast<int>(player.position.z) / Chunk::CHUNK_SIZE_Z;

        if ((newPlayerChunkX != currentPlayerChunkX || newPlayerChunkZ != currentPlayerChunkZ) && !isUpdatingChunks) {
            
            if (chunkUpdateTask.valid() && chunkUpdateTask.wait_for(std::chrono::seconds(0)) != std::future_status::ready) std::cout << "error multichunk" << std::endl;
            
            else {
                isUpdatingChunks = true;
                chunkUpdateTask = std::async(std::launch::async, UpdateChunks, std::ref(chunkMap), player.position);
                currentPlayerChunkX = newPlayerChunkX;
                currentPlayerChunkZ = newPlayerChunkZ;
            }
        }

        BeginDrawing();
        BeginMode3D(camera);

        // Sky

        DrawSky(timeOfDay);

        player.DrawHand(player, camera);

        {
            //std::shared_lock<std::shared_mutex> lock(chunkMapMutex);
            for (auto& chunk : chunkMap) {
                if (chunk.second.IsLoaded) {
                    chunk.second.Draw(player.highlightedBlockPos, camera);
                }
            }
        }

        if (IsKeyPressed(KEY_F3)) {
            //std::lock_guard<std::mutex> lock(chunkMapMutex);
            for (auto& chunk : chunkMap) {
                chunk.second.Update(chunkMap);
            }
        }

        player.Draw();
        EndMode3D();
        DrawCrosshair(player, chunkMap);

        // DEBUG
        DrawDebug(player, chunkMap);

        EndDrawing();
    }

    unloadTextures();
    CloseWindow();

    return 0;
}