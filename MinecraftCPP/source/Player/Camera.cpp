#include "Camera.hpp"

Camera3D InitCamera() {
    Camera3D camera = { 0 };
    camera.position = { 0.0f, 10.0f, 20.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    return camera;
}

void UpdateCameraRotation(Player& player) {

    Vector2 mousePosition = GetMousePosition();
    Vector2 center = { screenWidth / 2.0f, screenHeight / 2.0f };
    Vector2 mouseDelta = { mousePosition.x - center.x, mousePosition.y - center.y };

    player.yaw += mouseDelta.x * 0.003f;
    player.pitch -= mouseDelta.y * 0.003f;

    if (player.pitch > 1.4f) player.pitch = 1.4f;
    if (player.pitch < -1.4f) player.pitch = -1.4f;

    SetMousePosition((int)center.x, (int)center.y);

}

void UpdateCameraPosition(Camera3D& camera, const Player& player) {
    Vector3 forward = GetCameraForward(player);
    camera.position = { player.position.x, player.position.y + 1.75f, player.position.z };
    camera.target = { camera.position.x + forward.x, camera.position.y + forward.y, camera.position.z + forward.z };
}

Vector3 GetCameraForward(const Player& player) {
    return { cosf(player.yaw) * cosf(player.pitch), sinf(player.pitch), sinf(player.yaw) * cosf(player.pitch) };
}

Vector3 normalized(float x, float y, float z) {
    float length = std::sqrt(x * x + y * y + z * z);
    return { x / length, y / length, z / length };
}

bool GetBlockLookingAt(Vector3 playerPos, Vector3 forward, ChunkMap& chunkMap, Vector3& outBlockPos, Vector3& outHitNormal, std::shared_mutex& chunkMapMutex) {

    std::shared_lock<std::shared_mutex> lock(chunkMapMutex);

    const float maxDistance = 5.0f;
    const float stepSize = 0.1f;

    Vector3 rayStart = { playerPos.x, playerPos.y + 1.75f, playerPos.z };
    Vector3 currentPos = rayStart;

    for (float t = 0.0f; t < maxDistance; t += stepSize) {
        currentPos = {
            rayStart.x + forward.x * t,
            rayStart.y + forward.y * t,
            rayStart.z + forward.z * t
        };

        Vector3 roundedPos = { floorf(currentPos.x + 0.5f), floorf(currentPos.y + 0.5f), floorf(currentPos.z + 0.5f) };

        Vector2 chunkPos = {
            floor(roundedPos.x / Chunk::CHUNK_SIZE_X),
            floor(roundedPos.z / Chunk::CHUNK_SIZE_Z)
        };

        if (chunkMap.count(chunkPos)) {
            auto& chunk = chunkMap[chunkPos];
            if (chunk.blockMap.count(roundedPos)) {
                outBlockPos = roundedPos;

                Vector3 entryPos = {
                    currentPos.x - forward.x * stepSize,
                    currentPos.y - forward.y * stepSize,
                    currentPos.z - forward.z * stepSize
                };

                outHitNormal = {
                    static_cast<float>(roundedPos.x > floorf(entryPos.x + 0.5f)) - (roundedPos.x < floorf(entryPos.x + 0.5f)),
                    static_cast<float>(roundedPos.y > floorf(entryPos.y + 0.5f)) - (roundedPos.y < floorf(entryPos.y + 0.5f)),
                    static_cast<float>(roundedPos.z > floorf(entryPos.z + 0.5f)) - (roundedPos.z < floorf(entryPos.z + 0.5f))
                };

                return true;
            }
        }
    }

    return false;
}

void DrawCrosshair(Player& player, ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex) {
    int screenCenterX = screenWidth / 2;
    int screenCenterY = screenHeight / 2;
    int crossSize = 7;

    Vector3 blockPos, hitNormal;
    bool canBreak = GetBlockLookingAt(player.position, GetCameraForward(player), chunkMap, blockPos, hitNormal, chunkMapMutex);

    Color crossColor = canBreak ? YELLOW : BLACK;

    DrawLine(screenCenterX - crossSize, screenCenterY, screenCenterX + crossSize, screenCenterY, crossColor);
    DrawLine(screenCenterX, screenCenterY - crossSize, screenCenterX, screenCenterY + crossSize, crossColor);
}