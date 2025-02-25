#include "Game/Game.hpp"
#include "Game/Textures.hpp"
#include "Player/Player.hpp"
#include "Block/Block.hpp"
#include "Player/Camera.hpp"
#include "Game/Noise.hpp"
#include <unordered_map>
#include "World/Chunk.hpp"
#include "World/Skybox.hpp"

std::atomic<bool> isUpdatingChunks = false;
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

    InitWindow(screenWidth, screenHeight, "Minecraft: NosovEdition &Artur 0.03a"); // Infinity World Update
    
    SetTargetFPS(60);
    DisableCursor();
    loadTextures();

    srand(worldSeed);
    blockDataMap = loadBlockData();

    Camera3D camera = InitCamera();
    Player player(0, 80, 0);

    ChunkMap chunkMap;
    UpdateChunks(chunkMap, player.position);

    player.inventory.push_back(2.1);
    player.inventory.push_back(2);
    player.inventory.push_back(2.2);
    player.inventory.push_back(7);
    player.inventory.push_back(12);
    player.inventory.push_back(13);
    player.inventory.push_back(14);
    player.inventory.push_back(17);
    player.inventory.push_back(18);
    player.inventory.push_back(19);
    player.inventory.push_back(20);

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
        timeOfDay = (timeOfDay + 10) % 24000;
        Color skyColor = GetSkyColor(timeOfDay);
        ClearBackground(skyColor);

        // Inventory view
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0) {
            player.inventorySlot += (wheelMove > 0) ? 1 : -1;
            if (player.inventorySlot >= player.inventory.size()) player.inventorySlot = 0;
            if (player.inventorySlot < 0) player.inventorySlot = player.inventory.size() - 1;
        }

        Vector3 handOffset = { 0.4f, -0.3f, 0.5f };
        Vector3 forward = GetCameraForward(player);
        Vector3 right = { -sinf(player.yaw), 0, cosf(player.yaw) };

        Vector3 stonePos = {
            camera.position.x + forward.x * handOffset.z + right.x * handOffset.x,
            camera.position.y + handOffset.y,
            camera.position.z + forward.z * handOffset.z + right.z * handOffset.x
        };

        float bobbing = sin(GetTime() * 8) * 0.05f;
        stonePos.y += bobbing;

        float stoneRotation = sin(GetTime() * 5) * 5.0f;
        DrawCubeTexture(setTexture(getTexture(player.inventory[player.inventorySlot])), stonePos, 0.3f, 0.3f, 0.3f, WHITE);

        {
            //std::shared_lock<std::shared_mutex> lock(chunkMapMutex);
            for (auto& chunk : chunkMap) {
                if (chunk.second.IsLoaded) {
                    chunk.second.Draw(player.highlightedBlockPos);
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
        Vector3 blockPos, hitNormal;
        bool blockUnderCrosshair = GetBlockLookingAt(player.position, GetCameraForward(player), chunkMap, blockPos, hitNormal);

        int fps = GetFPS();
        std::string playerPos = "XYZ: " + std::to_string(player.position.x) + " / " + std::to_string(player.position.y) + " / " + std::to_string(player.position.z);

        DrawText("Minecraft &Artur 0.03a (alpha 3/Nosov)", 10, 10, 20, DARKGRAY);
        DrawText(("FPS: " + std::to_string(fps)).c_str(), 10, 40, 20, DARKGRAY);
        DrawText(playerPos.c_str(), 10, 70, 20, DARKGRAY);

        EndDrawing();
    }

    unloadTextures();
    CloseWindow();

    return 0;
}