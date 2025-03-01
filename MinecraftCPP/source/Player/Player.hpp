#pragma once

#include "..\Game\Game.hpp"
#include "..\Block\Block.hpp"
#include "../World/Chunk.hpp"
#include "Inventory.hpp"

const float gravity = 0.0055f;
const float jumpForce = -0.078f;

class Player;
class Cursor;

extern Cursor cursor;

class Cursor {

public:

    double ItemID;
    int count;

    Cursor() : ItemID(0.0), count(0) {};

    void Withdraw(double inventory[INVENTORY_SLOTS][INVENTORY_SLOTS_COUNTS], int slotID, bool LeftMouseButton);

    void Update(Player& player);

};

class Player {
public:

    Vector3 position;
    Vector3 velocity;

    bool isInventory;
    bool isCrafting;

    bool isGrounded;
    float yaw, pitch;

    Vector3 highlightedBlockPos;

    double inventory[PLAYER_INITIALIZATION_SLOT][INVENTORY_SLOTS_COUNTS];
    int inventorySlot;

    Player(float x, float y, float z);

    void DrawHand(Player& player, Camera3D& camera);

    void Update(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex);
    void Draw();

private:

    bool isRunning = false;
    bool isMovingForward = false;
    double lastWPressTime = 0.0;
    const double doublePressThreshold = 0.3;
    float runSpeedMultiplier = 1.3f;

    float moveSpeed = 0.07f;

    std::string GetHeldTool();

    void BreakBlock(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex);
    void PlaceBlock(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex);

    bool IsFunctionalBlock(double blockID);

    bool CheckCollisionWithChunks(const Vector3& pos, ChunkMap& chunkMap);
    Block* GetBlockAtPosition(const Vector3& pos, ChunkMap& chunkMap);

};