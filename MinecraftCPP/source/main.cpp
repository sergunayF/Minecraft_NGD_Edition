// BASE GAME INCLUDES

#include "Game/Game.hpp"
#include "Game/Textures.hpp"
#include "Game/DeBugScreen.hpp"

// GUI INCLUDES

#include "GUI/HotBar.hpp"

// PLAYER INCLUDES

#include "Player/Player.hpp"
#include "Player/Camera.hpp"
#include "Player/Mobs.hpp"

#include "GUI/Crafting.hpp"



// WORLD INCLUDES

#include "Block/Block.hpp"
#include "World/Chunk.hpp"
#include "World/Skybox.hpp"

// MULTYTHREADS

std::shared_mutex chunkMapMutex;

std::queue<std::packaged_task<void()>> tasks;
std::mutex tasksMutex;
std::condition_variable tasksCV;
std::vector<std::thread> threads;
std::atomic<bool> stopThreads = false;

std::atomic<bool> stopGameThreads = false;

bool worldIsLoaded = false;

void EnsureDirectoryExists(const std::string& path) {

    if (!std::filesystem::exists(path)) std::filesystem::create_directories(path);

}

void SaveAllChangedChunks(ChunkMap& chunkMap, const std::string& savePath) {

    EnsureDirectoryExists(savePath);

    std::unique_lock<std::shared_mutex> lock(chunkMapMutex);

    for (auto& chunk : chunkMap) {
        if (chunk.second.IsChanged) {
            chunk.second.SaveToFile(savePath);
        }
    }

}

void ThreadPoolWorker() {
    while (!stopThreads) {
        std::packaged_task<void()> task;
        {
            std::unique_lock<std::mutex> lock(tasksMutex);
            tasksCV.wait(lock, [] { return !tasks.empty() || stopThreads; });
            if (stopThreads) return;
            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}

void InitThreadPool() {
    for (int i = 0; i < MAX_THREADS; ++i) {
        threads.emplace_back(ThreadPoolWorker);
    }
}

void ShutdownThreadPool() {
    stopThreads = true;
    tasksCV.notify_all();
    for (auto& thread : threads) {
        if (thread.joinable()) thread.join();
    }
}

void LoadOrGenerateChunk(ChunkMap& chunkMap, int x, int z, const std::string& savePath, std::shared_mutex& chunkMapMutex) {
    Vector2 chunkPos = { static_cast<float>(x), static_cast<float>(z) };
    std::shared_lock<std::shared_mutex> lock(chunkMapMutex);

    if (chunkMap.find(chunkPos) != chunkMap.end()) return;

    Chunk newChunk(x, z);
    newChunk.LoadFromFile(savePath);

    if (!newChunk.IsLoaded) {
        newChunk.GenerateChunk();
        newChunk.IsLoaded = true;
    }

    chunkMap[chunkPos] = std::move(newChunk);
    chunkMap[chunkPos].Update(chunkMap);
}

void UpdateChunksAroundPlayer(ChunkMap& chunkMap, Player& player, const std::string& savePath, std::shared_mutex& chunkMapMutex) {
    int playerChunkX = static_cast<int>(std::floor(player.position.x / Chunk::CHUNK_SIZE_X));
    int playerChunkZ = static_cast<int>(std::floor(player.position.z / Chunk::CHUNK_SIZE_Z));

    std::vector<std::future<void>> futures;

    for (int x = playerChunkX - RENDER_DISTANCE; x <= playerChunkX + RENDER_DISTANCE; ++x) {
        for (int z = playerChunkZ - RENDER_DISTANCE; z <= playerChunkZ + RENDER_DISTANCE; ++z) {
            std::packaged_task<void()> task([&chunkMap, x, z, &savePath, &chunkMapMutex] {
                LoadOrGenerateChunk(chunkMap, x, z, savePath, chunkMapMutex);
                });

            std::future<void> result = task.get_future();
            futures.push_back(std::move(result));

            {
                std::lock_guard<std::mutex> lock(tasksMutex);
                tasks.emplace(std::move(task));
            }
            tasksCV.notify_one();
        }
    }

    for (auto& future : futures) future.get();

}


void SaveOutOfRangeChunks(ChunkMap& chunkMap, Player& player, const std::string& savePath, std::shared_mutex& chunkMapMutex) {

    EnsureDirectoryExists(savePath);

    int playerChunkX = static_cast<int>(std::floor(player.position.x / Chunk::CHUNK_SIZE_X));
    int playerChunkZ = static_cast<int>(std::floor(player.position.z / Chunk::CHUNK_SIZE_Z));

    std::vector<Vector2> chunksToRemove;

    {
        std::shared_lock<std::shared_mutex> lock(chunkMapMutex);
        for (auto& chunk : chunkMap) {
            int chunkX = static_cast<int>(chunk.first.x);
            int chunkZ = static_cast<int>(chunk.first.y);

            if (std::abs(chunkX - playerChunkX) > RENDER_DISTANCE || std::abs(chunkZ - playerChunkZ) > RENDER_DISTANCE) {
                if (chunk.second.IsChanged) {
                    chunk.second.SaveToFile(savePath);
                }
                chunksToRemove.push_back(chunk.first);
            }
        }
    }

    {
        std::unique_lock<std::shared_mutex> lock(chunkMapMutex);
        for (auto& chunkPos : chunksToRemove) {
            chunkMap.erase(chunkPos);
        }
    }
}

void UpdateChunksAroundPlayerAsync(ChunkMap& chunkMap, Player& player, const std::string& savePath, std::shared_mutex& chunkMapMutex) {
    std::packaged_task<void()> task([&chunkMap, &player, &savePath, &chunkMapMutex] {
        UpdateChunksAroundPlayer(chunkMap, player, savePath, chunkMapMutex);
        });

    std::future<void> result = task.get_future();

    {
        std::lock_guard<std::mutex> lock(tasksMutex);
        tasks.emplace(std::move(task));
    }
    tasksCV.notify_one();

    result.get();
}

void SaveOutOfRangeChunksAsync(ChunkMap& chunkMap, Player& player, const std::string& savePath, std::shared_mutex& chunkMapMutex) {
    std::packaged_task<void()> task([&chunkMap, &player, &savePath, &chunkMapMutex] {
        SaveOutOfRangeChunks(chunkMap, player, savePath, chunkMapMutex);
        });

    std::future<void> result = task.get_future();

    {
        std::lock_guard<std::mutex> lock(tasksMutex);
        tasks.emplace(std::move(task));
    }
    tasksCV.notify_one();

    result.get();
}

void ChunkUpdateLoop(ChunkMap& chunkMap, Player& player, const std::string& savePath, std::shared_mutex& chunkMapMutex) {
    while (!stopGameThreads) {
        UpdateChunksAroundPlayerAsync(chunkMap, player, savePath, chunkMapMutex);
        SaveOutOfRangeChunksAsync(chunkMap, player, savePath, chunkMapMutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        worldIsLoaded = true;
    }
}


int main() {

    InitWindow(screenWidth, screenHeight, "Minecraft: NosovEdition 0.08a"); // Survival Update Part Four | Chunks && Furnace

    SetTargetFPS(60);
    SetExitKey(0);
    loadTextures();

    BeginDrawing();
    ClearBackground(BLACK);
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            DrawTexturePro(backgroundGUI, { 0.0f, 0.0f, 16.0f, 16.0f }, { 0.0f, 0.0f, 16.0f * GUI_SCALE, 16.0f * GUI_SCALE }, { static_cast<float>(i) * -16.0f * GUI_SCALE, static_cast<float>(j) * -16.0f * GUI_SCALE }, 0.0f, { 255, 255, 255, 150 });
        }
    }
    DrawTextPro(minecraftFont, "Generate Chunks...", { screenWidth / 2 - 64 + GUI_SCALE, screenHeight / 2 + GUI_SCALE }, { 0.0f, 0.0f }, 0.0f, 20.0f, 0.0f, DARKGRAY);
    DrawTextPro(minecraftFont, "Generate Chunks...", { screenWidth / 2 - 64, screenHeight / 2 }, { 0.0f, 0.0f }, 0.0f, 20.0f, 0.0f, WHITE);
    EndDrawing();

    srand(worldSeed);
    blockDataMap = loadBlockData();
    recipes = loadRecipes(ASSETS_PATH"recipes.json");

    Camera3D camera = InitCamera();
    Player player(0, 80, 0);
    Zombie zombe(0, 80, 0);

    InitMiniBlocks(player);
    inventorySlotsInit();

    ChunkMap chunkMap;

    DisableCursor();

    player.inventory[6][0] = 1.1;
    player.inventory[6][1] = 10.0;

    player.inventory[5][0] = 20.0;
    player.inventory[5][1] = 64.0;

    player.inventory[2][0] = 17.0;
    player.inventory[2][1] = 16.0;

    int currentPlayerChunkX = static_cast<int>(std::floor(player.position.x / Chunk::CHUNK_SIZE_X));
    int currentPlayerChunkZ = static_cast<int>(std::floor(player.position.z / Chunk::CHUNK_SIZE_Z));

    int timeOfDay = 0;

    InitThreadPool();

    std::thread chunkThread(ChunkUpdateLoop, std::ref(chunkMap), std::ref(player), std::ref(savePath), std::ref(chunkMapMutex));

    while (!WindowShouldClose()) {

        if (worldIsLoaded == false) continue;

        if (!player.isInventory) UpdateCameraRotation(player);

        UpdateCameraPosition(camera, player);

        BeginDrawing();
        BeginMode3D(camera);

        DrawSky(timeOfDay);

        DrawChunks(chunkMap, player, camera, chunkMapMutex);

        player.Update(chunkMap, chunkMapMutex);

        zombe.Update(chunkMap, chunkMapMutex);

        player.DrawHand(player, camera);

        player.Draw();

        EndMode3D();

        DrawHotBar(player);

        // DEBUG
        if (player.debug) DrawDebug(player, chunkMap, chunkMapMutex);

        // Inventory
        if (player.isInventory) {
            DrawInventory(player);
            cursor.Update(player);
        }

        EndDrawing();
    }

    BeginDrawing();
    ClearBackground(BLACK);
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            DrawTexturePro(backgroundGUI, { 0.0f, 0.0f, 16.0f, 16.0f }, { 0.0f, 0.0f, 16.0f * GUI_SCALE, 16.0f * GUI_SCALE }, { static_cast<float>(i) * -16.0f * GUI_SCALE, static_cast<float>(j) * -16.0f * GUI_SCALE }, 0.0f, { 255, 255, 255, 150 });
        }
    }
    DrawTextPro(minecraftFont, "Saving Level...", { screenWidth / 2 - 64 + GUI_SCALE, screenHeight / 2 + GUI_SCALE }, { 0.0f, 0.0f }, 0.0f, 20.0f, 0.0f, DARKGRAY);
    DrawTextPro(minecraftFont, "Saving Level...", { screenWidth / 2 - 64, screenHeight / 2 }, { 0.0f, 0.0f }, 0.0f, 20.0f, 0.0f, WHITE);
    EndDrawing();

    stopGameThreads = true;
    if (chunkThread.joinable()) chunkThread.join();

    SaveAllChangedChunks(chunkMap, savePath);
    ShutdownThreadPool();
    unloadTextures();
    CloseWindow();

    return 0;
}