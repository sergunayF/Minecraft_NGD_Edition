#pragma once

#include "..\Game\Game.hpp"
#include "..\Block\Block.hpp"
#include "../World/Chunk.hpp"
#include "Inventory.hpp"
#include "Camera.hpp"

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

    void Withdraw(double inventory[INVENTORY_SLOTS][INVENTORY_SLOTS_COUNTS], int slotID, bool LeftMouseButton, bool isFurnace);

    void Update(Player& player);

};

class Entity {

public:
    virtual void DrawTexture(Texture2D textures[6], Vector3 position, float width, float height, float length, Color color)=0;
//    virtual void DrawHitBox(const Vector3& pos, int WidthX, int Height, int WidthZ) = 0;
    virtual bool CheckCollisionWithChunks(const Vector3& pos, ChunkMap& chunkMap) = 0;
    virtual Block* GetBlockAtPosition(const Vector3& pos, ChunkMap& chunkMap) = 0;
    virtual void Update(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex) = 0;
    virtual void Draw() = 0;

    Vector3 position;
    Vector3 velocity;

    bool isGrounded;

    int HP = 20;
    int MaxHP = 20;
    int Damage = 1;

    float moveSpeed = 0.07f;

    bool isRunning = false;
    bool isMovingForward = false;
    float runSpeedMultiplier = 1.3f;
};



class Player :public Entity {
public:

    bool debug = false;

    int Level = 0;
    int Hunger = 20;
    int EXP = 0;

    bool isInventory;
    bool isCrafting;
    bool isFurnace;


    float yaw, pitch;

    Block* activeBlock = nullptr;
    Vector3 highlightedBlockPos;

    double inventory[PLAYER_INITIALIZATION_SLOT][INVENTORY_SLOTS_COUNTS];
    int inventorySlot;

    Player(float x, float y, float z);

    void DrawHand(Player& player, Camera3D& camera);
    void DrawTexture(Texture2D textures[6], Vector3 position, float width, float height, float length, Color color) override;

    virtual void Update(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex) override;
    virtual void Draw() override;

    virtual bool CheckCollisionWithChunks(const Vector3& pos, ChunkMap& chunkMap) override;
    virtual Block* GetBlockAtPosition(const Vector3& pos, ChunkMap& chunkMap) override;

private:

    double lastWPressTime = 0.0;
    const double doublePressThreshold = 0.3;

    std::string GetHeldTool();

    void BreakBlock(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex);
    void PlaceBlock(ChunkMap& chunkMap);

};