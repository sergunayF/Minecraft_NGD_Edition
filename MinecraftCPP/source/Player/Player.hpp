#pragma once

#include "..\Game\Game.hpp"
#include "..\Block\Block.hpp"
#include "../World/Chunk.hpp"

const float gravity = 0.0065f;
const float jumpForce = -0.078f;
const float moveSpeed = 0.07f;

class Player {
public:

    Vector3 position;
    Vector3 velocity;

    bool isGrounded;
    float yaw, pitch;

    Vector3 highlightedBlockPos;

    std::vector<float> inventory;
    int inventorySlot;

    Player(float x, float y, float z);

    void DrawHand(Player& player, Camera3D& camera);

    void Update(ChunkMap& chunkMap);
    void Draw();

private:

    void BreakBlock(ChunkMap& chunkMap);
    void PlaceBlock(ChunkMap& chunkMap);

    bool CheckCollisionWithChunks(const Vector3& pos, ChunkMap& chunkMap);
    Block* GetBlockAtPosition(const Vector3& pos, ChunkMap& chunkMap);

};