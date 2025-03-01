#include "DeBugScreen.hpp"


void DrawDebug(Player& player, ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex) {

    Vector3 blockPos, hitNormal;
    bool blockUnderCrosshair = GetBlockLookingAt(player.position, GetCameraForward(player), chunkMap, blockPos, hitNormal, chunkMapMutex);

    int fps = GetFPS();
    std::string playerPos = "XYZ: " + std::to_string(player.position.x) + " / " + std::to_string(player.position.y) + " / " + std::to_string(player.position.z);
    std::string chunkPos = "Chunk: " + std::to_string(static_cast<int>(floor(player.position.x / 16))) + " " + std::to_string(static_cast<int>(floor(player.position.z / 16)));

    DrawText("Minecraft 0.07a (alpha 7/Nosov)", 10, 10, 20, WHITE);
    DrawText(("FPS: " + std::to_string(fps)).c_str(), 10, 40, 20, WHITE);
    DrawText(playerPos.c_str(), 10, 70, 20, WHITE);
    DrawText(chunkPos.c_str(), 10, 100, 20, WHITE);

}