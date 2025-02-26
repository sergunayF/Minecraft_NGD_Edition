#include "DeBugScreen.hpp"


void DrawDebug(Player& player, ChunkMap& chunkMap) {

    Vector3 blockPos, hitNormal;
    bool blockUnderCrosshair = GetBlockLookingAt(player.position, GetCameraForward(player), chunkMap, blockPos, hitNormal);

    int fps = GetFPS();
    std::string playerPos = "XYZ: " + std::to_string(player.position.x) + " / " + std::to_string(player.position.y) + " / " + std::to_string(player.position.z);

    DrawText("Minecraft 0.04a (alpha 4/Nosov)", 10, 10, 20, WHITE);
    DrawText(("FPS: " + std::to_string(fps)).c_str(), 10, 40, 20, WHITE);
    DrawText(playerPos.c_str(), 10, 70, 20, WHITE);

}