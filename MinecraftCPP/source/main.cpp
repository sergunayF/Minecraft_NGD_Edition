// BASE GAME INCLUDES

#include "Game/Game.hpp"
#include "Game/Textures.hpp"
#include "Game/DeBugScreen.hpp"

// PLAYER INCLUDES

#include "Player/Player.hpp"
#include "Player/Camera.hpp"

// WORLD INCLUDES

#include "Block/Block.hpp"
#include "World/Chunk.hpp"
#include "World/Skybox.hpp"

// GUI INCLUDES

#include "GUI/HotBar.hpp"

//

#include <GLFW/glfw3.h>
#include <filesystem>

// MULTYTHREADS

std::shared_mutex chunkMapMutex;

void SaveAllChangedChunks(ChunkMap& chunkMap, const std::string& savePath) {
    std::unique_lock<std::shared_mutex> lock(chunkMapMutex);

    for (auto& chunk : chunkMap) {
        if (chunk.second.IsChanged) {
            chunk.second.SaveToFile(savePath);
        }
    }
}

void UpdateChunks(ChunkMap& chunkMap, const Vector3& playerPosition, const std::string& savePath) {
    std::unique_lock<std::shared_mutex> lock(chunkMapMutex);

    if (!std::filesystem::exists(savePath)) {
        std::filesystem::create_directories(savePath);
    }

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

            chunkMap[chunkPos].LoadFromFile(savePath);
            chunkMap[chunkPos].IsLoaded = true;
            chunkMap[chunkPos].Update(chunkMap);
            //chunkMap[chunkPos].UpdateNeighborChunks(chunkMap, chunkPos);
        }
    }

    for (auto it = chunkMap.begin(); it != chunkMap.end();) {
        if (neededChunks.find(it->first) == neededChunks.end()) {
            if (it->second.IsChanged) {
                it->second.SaveToFile(savePath);
            }

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
    InitWindow(screenWidth, screenHeight, "Minecraft: NosovEdition 0.05a"); // Survival Update Part One | GUI and Blocks
    
    SetTargetFPS(60);
    DisableCursor();
    loadTextures();

    srand(worldSeed);
    blockDataMap = loadBlockData();

    Camera3D camera = InitCamera();
    Player player(0, 80, 0);

    InitMiniBlocks(player);

    std::string savePath = "saves/world_" + std::to_string(worldSeed) + "/LevelData";

    ChunkMap chunkMap;
    UpdateChunks(chunkMap, player.position, savePath);

    player.inventory[4][0] = 1.1;
    player.inventory[4][1] = 1.0;

    int currentPlayerChunkX = static_cast<int>(player.position.x) / Chunk::CHUNK_SIZE_X;
    int currentPlayerChunkZ = static_cast<int>(player.position.z) / Chunk::CHUNK_SIZE_Z;

    int timeOfDay = 0;
    std::future<void> chunkUpdateTask;

    while (!WindowShouldClose()) {

        UpdateCameraRotation(player);
        UpdateCameraPosition(camera, player);

        int newPlayerChunkX = static_cast<int>(player.position.x) / Chunk::CHUNK_SIZE_X;
        int newPlayerChunkZ = static_cast<int>(player.position.z) / Chunk::CHUNK_SIZE_Z;

        if ((newPlayerChunkX != currentPlayerChunkX || newPlayerChunkZ != currentPlayerChunkZ) && !isUpdatingChunks) {
            if (chunkUpdateTask.valid() && chunkUpdateTask.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
                std::cout << "Ожидание завершения обновления чанков..." << std::endl;
                chunkUpdateTask.wait();
            }

            isUpdatingChunks = true;
            chunkUpdateTask = std::async(std::launch::async, UpdateChunks, std::ref(chunkMap), player.position, savePath);
            currentPlayerChunkX = newPlayerChunkX;
            currentPlayerChunkZ = newPlayerChunkZ;
        }


        BeginDrawing();
        BeginMode3D(camera);

        DrawSky(timeOfDay);


        player.Update(chunkMap);
      

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
        DrawHotBar(player);

        // DEBUG
        DrawDebug(player, chunkMap);

        EndDrawing();
    }

    SaveAllChangedChunks(chunkMap, savePath);

    unloadTextures();
    CloseWindow();

    return 0;
}