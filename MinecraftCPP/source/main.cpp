#include "Game/Game.hpp"
#include "Game/Textures.hpp"
#include "Player/Player.hpp"
#include "Block/Block.hpp"
#include "Player/Camera.hpp"
#include "Game/Noise.hpp"
#include <unordered_map>
#include "World/Chunk.hpp"

Texture2D setTexture(const std::string& textureName) {
    size_t pos = textureName.find(':');

    if (pos == std::string::npos) {
        std::cerr << "Ошибка: некорректный формат имени текстуры: " << textureName << std::endl;
        return texturesArray[0][0];
    }

    int a, b;
    try {
        a = std::stoi(textureName.substr(0, pos));
        b = std::stoi(textureName.substr(pos + 1));
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка при конвертации строки в числа: " << e.what() << std::endl;
        return texturesArray[0][0];
    }

    return texturesArray[a][b];
}

int main() {
    InitWindow(screenWidth, screenHeight, "Minecraft: NosovEdition &Artur 0.03a"); // Infinity World Update

    SetTargetFPS(60);
    DisableCursor();
    loadTextures();

    unsigned int seed = 12345;
    PerlinNoise perlin(seed);

    std::unordered_map<std::string, BlockData> blockDataMap;

    blockDataMap = loadBlockData();

    Camera3D camera = InitCamera();
    Player player(0, 130, 0);

    ChunkMap chunkMap;

    for (int x = -4; x <= 5; x++) {
        for (int z =-4; z <= 5; z++) {
            Vector2 chunkPos = { static_cast<float>(x), static_cast<float>(z) };
            chunkMap[chunkPos] = Chunk(x, z);
        }
    }

    player.inventory.push_back(1);
    player.inventory.push_back(3);
    player.inventory.push_back(3.1);
    player.inventory.push_back(4);
    player.inventory.push_back(5);

    while (!WindowShouldClose()) {
        UpdateCameraRotation(player);
        player.Update(chunkMap);
        UpdateCameraPosition(camera, player);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);

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

        ////


        /*for (auto& chunk : chunkMap) {
            chunk.second.Draw();
        }*/

        player.Draw();
        EndMode3D();

        DrawCrosshair(player, chunkMap);

        // DEBUG
        Vector3 blockPos, hitNormal;
        bool blockUnderCrosshair = GetBlockLookingAt(player.position, GetCameraForward(player), chunkMap, blockPos, hitNormal);

        /*std::string blockInfo = blockUnderCrosshair ?
            "Block: " + getBlockName(chunkMap[blockPos].id) + " [ " +
            std::to_string((int)(blockPos.x / 8)) + " " + std::to_string((int)(blockPos.y / 8)) + " " + std::to_string((int)(blockPos.z / 8)) + "]"
            : "Block: minecraft:air";*/

        int fps = GetFPS();
        std::string playerPos = "XYZ: " + std::to_string(player.position.x) + " / " + std::to_string(player.position.y) + " / " + std::to_string(player.position.z);

        DrawText("Minecraft &Artur 0.03a (alpha 3/Nosov)", 10, 10, 20, DARKGRAY);
        DrawText(("FPS: " + std::to_string(fps)).c_str(), 10, 40, 20, DARKGRAY);
        DrawText(playerPos.c_str(), 10, 70, 20, DARKGRAY);
        //DrawText(blockInfo.c_str(), 10, 100, 20, DARKGRAY);

        EndDrawing();
    }

    unloadTextures();
    CloseWindow();

    return 0;
}