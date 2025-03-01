// BASE GAME INCLUDES

#include "Game/Game.hpp"
#include "Game/Textures.hpp"
#include "Game/DeBugScreen.hpp"

// GUI INCLUDES

#include "GUI/HotBar.hpp"

// PLAYER INCLUDES

#include "Player/Player.hpp"
#include "Player/Camera.hpp"

#include "GUI/Crafting.hpp"

// WORLD INCLUDES

#include "Block/Block.hpp"
#include "World/Chunk.hpp"
#include "World/Skybox.hpp"

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
    std::shared_lock<std::shared_mutex> lock(chunkMapMutex);

    if (!std::filesystem::exists(savePath)) {
        std::filesystem::create_directories(savePath);
    }

    int playerChunkX = static_cast<int>(std::floor(playerPosition.x / Chunk::CHUNK_SIZE_X));
    int playerChunkZ = static_cast<int>(std::floor(playerPosition.z / Chunk::CHUNK_SIZE_Z));

    std::unordered_set<Vector2, Vector2Hash, Vector2Equal> neededChunks;

    for (int x = playerChunkX - RENDER_DISTANCE; x <= playerChunkX + RENDER_DISTANCE; x++) {
        for (int z = playerChunkZ - RENDER_DISTANCE; z <= playerChunkZ + RENDER_DISTANCE; z++) {
            neededChunks.insert({ static_cast<float>(x), static_cast<float>(z) });
        }
    }

    for (const auto& chunkPos : neededChunks) {
        if (chunkMap.find(chunkPos) == chunkMap.end()) {
            chunkMap[chunkPos] = Chunk(static_cast<int>(chunkPos.x), static_cast<int>(chunkPos.y));
            chunkMap[chunkPos].LoadFromFile(savePath);
            chunkMap[chunkPos].IsLoaded = true;
            chunkMap[chunkPos].Update(chunkMap, chunkMapMutex);
        }
    }

    for (auto it = chunkMap.begin(); it != chunkMap.end();) {
        int chunkDistanceX = abs(static_cast<int>(it->first.x) - playerChunkX);
        int chunkDistanceZ = abs(static_cast<int>(it->first.y) - playerChunkZ);

        if (chunkDistanceX > (RENDER_DISTANCE + 1) || chunkDistanceZ > (RENDER_DISTANCE + 1)) {
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
    InitWindow(screenWidth, screenHeight, "Minecraft: NosovEdition 0.07a"); // Survival Update Part Three | Graphic's && Craft System

    SetTargetFPS(60);
    DisableCursor();
    loadTextures();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    srand(worldSeed);
    blockDataMap = loadBlockData();
    recipes = loadRecipes(ASSETS_PATH"recipes.json");

    Camera3D camera = InitCamera();
    Player player(0, 80, 0);

    InitMiniBlocks(player);
    inventorySlotsInit();

    std::string savePath = "saves/world_" + std::to_string(worldSeed) + "/LevelData";

    ChunkMap chunkMap;
    UpdateChunks(chunkMap, player.position, savePath);

    player.inventory[4][0] = 3.1;
    player.inventory[4][1] = 32.0;

    player.inventory[6][0] = 1.1;
    player.inventory[6][1] = 10.0;

    player.inventory[5][0] = 20.0;
    player.inventory[5][1] = 64.0;

    player.inventory[2][0] = 17.0;
    player.inventory[2][1] = 16.0;

    int currentPlayerChunkX = static_cast<int>(std::floor(player.position.x / Chunk::CHUNK_SIZE_X));
    int currentPlayerChunkZ = static_cast<int>(std::floor(player.position.z / Chunk::CHUNK_SIZE_Z));

    int timeOfDay = 0;
    std::future<void> chunkUpdateTask;

    while (!WindowShouldClose()) {

        if (!player.isInventory) UpdateCameraRotation(player);

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

        std::shared_lock<std::shared_mutex> lock(chunkMapMutex);

        DrawChunks(chunkMap, player.position, player.highlightedBlockPos, camera, chunkMapMutex);

        if (IsKeyPressed(KEY_F3)) {
            std::shared_lock<std::shared_mutex> lock(chunkMapMutex);
            DrawChunks(chunkMap, player.position, player.highlightedBlockPos, camera, chunkMapMutex);
        }

        player.Update(chunkMap, chunkMapMutex);

        player.DrawHand(player, camera);
        
        player.Draw();

        EndMode3D();

        DrawCrosshair(player, chunkMap, chunkMapMutex);
        DrawHotBar(player);

        // DEBUG
        DrawDebug(player, chunkMap, chunkMapMutex);

        // Inventory

        if (player.isInventory) {

            DrawInventory(player.inventory, player.isCrafting);
            cursor.Update(player);

        }

        EndDrawing();
    }

    SaveAllChangedChunks(chunkMap, savePath);

    unloadTextures();
    CloseWindow();

    return 0;

}