#include "Player.hpp"
#include "Camera.hpp"

Player::Player(float x, float y, float z) {

    position = { x, y, z };
    velocity = { 0, 0, 0 };

    isGrounded = false;
    isInventory = false;

    yaw = 0.0f;
    pitch = 0.0f;

    highlightedBlockPos = { -1, -1, -1 };
    
    for (int i = 0; i < 27 + 9; i++) {
        inventory[i][0] = 0;
        inventory[i][1] = 0;
    }

    inventorySlot = 0;

}

void Player::Update(ChunkMap& chunkMap) {
    velocity.y += gravity;

    Vector3 forward = { cosf(yaw), 0, sinf(yaw) };
    Vector3 right = { sinf(yaw), 0, -cosf(yaw) };

    Vector3 newPos = position;

    if (IsKeyPressed(KEY_W) && !isInventory) {
        double currentTime = GetTime();
        if (currentTime - lastWPressTime < doublePressThreshold) {
            isRunning = true;
        }
        lastWPressTime = currentTime;
    }

    if (IsKeyDown(KEY_W) && !isInventory) {
        if (!isMovingForward) {
            isMovingForward = true;
            if (IsKeyPressed(KEY_LEFT_CONTROL)) isRunning = true;
            if (isRunning) {
                moveSpeed *= runSpeedMultiplier;
            }
        }
    }

    else {
        if (isMovingForward) {
            isMovingForward = false;
            if (isRunning) {
                moveSpeed /= runSpeedMultiplier;
                isRunning = false;
            }
        }
    }

    if (IsKeyDown(KEY_W) && !isInventory) {
        newPos.x += forward.x * moveSpeed;
        newPos.z += forward.z * moveSpeed;
    }
    if (IsKeyDown(KEY_S) && !isInventory) { newPos.x -= forward.x * moveSpeed; newPos.z -= forward.z * moveSpeed; }
    if (IsKeyDown(KEY_A) && !isInventory) { newPos.x += right.x * moveSpeed; newPos.z += right.z * moveSpeed; }
    if (IsKeyDown(KEY_D) && !isInventory) { newPos.x -= right.x * moveSpeed; newPos.z -= right.z * moveSpeed; }

    if (IsKeyPressed(KEY_Q) && !isInventory) removeItemFromInventory(inventory, inventorySlot);

    if (IsKeyPressed(KEY_E)) {
        isInventory = !isInventory;
        isInventory ? EnableCursor() : DisableCursor();
    }

    bool collisionX = false, collisionZ = false, collisionY = false;
    const float playerHeight = 1.8f;

    for (float yOffset = 0; yOffset < playerHeight; yOffset += 1.0f) {
        Vector3 checkX = { round(newPos.x), round(position.y + yOffset), round(position.z) };
        if (CheckCollisionWithChunks(checkX, chunkMap)) collisionX = true;

        Vector3 checkZ = { round(position.x), round(position.y + yOffset), round(newPos.z) };
        if (CheckCollisionWithChunks(checkZ, chunkMap)) collisionZ = true;
    }

    Vector3 checkY = { round(position.x), round(position.y - velocity.y), round(position.z) };
    if (CheckCollisionWithChunks(checkY, chunkMap)) {
        isGrounded = true;
        velocity.y = 0;
        collisionY = true;
        position.y = round(position.y - 0.5f) + 0.5f;
    }
    else {
        position.y -= velocity.y;
    }

    Vector3 checkCeiling = { round(position.x), round(position.y + playerHeight), round(position.z) };
    if (CheckCollisionWithChunks(checkCeiling, chunkMap)) {
        velocity.y = 0;
        isGrounded = false;
        position.y -= 0.02f;
    }

    if (IsKeyDown(KEY_SPACE) && isGrounded && !isInventory) {
        velocity.y = jumpForce;
        isGrounded = false;
        collisionY = false;
    }

    if (!collisionX) position.x = newPos.x;
    if (!collisionZ) position.z = newPos.z;
    if (!collisionY) position.y -= velocity.y;

    Vector3 blockPos, hitNormal;
    if (!GetBlockLookingAt(position, GetCameraForward(*this), chunkMap, blockPos, hitNormal)) {
        highlightedBlockPos = { -1, -1, -1 };
    }
    else {
        highlightedBlockPos = blockPos;
    }

    if (!isInventory) {

        BreakBlock(chunkMap);
        PlaceBlock(chunkMap);

    }

}




std::string Player::GetHeldTool() {

    return getTexture(inventory[inventorySlot][0]);

}


void Player::BreakBlock(ChunkMap& chunkMap) {
    static double breakProgress = 0.0;
    static Vector3 lastBlockPos = { -1, -1, -1 };

    Vector3 blockPos, hitNormal;

    if (GetBlockLookingAt(position, GetCameraForward(*this), chunkMap, blockPos, hitNormal)) {

        Vector2 chunkPos = {
            floor(blockPos.x / Chunk::CHUNK_SIZE_X),
            floor(blockPos.z / Chunk::CHUNK_SIZE_Z)
        };

        if (chunkMap.count(chunkPos)) {
            auto blockIt = chunkMap[chunkPos].blockMap.find(blockPos);

            if (blockIt != chunkMap[chunkPos].blockMap.end()) {
                Block& block = blockIt->second;

                if (blockPos != lastBlockPos) {
                    breakProgress = 0.0;
                    lastBlockPos = blockPos;
                }

                double breakTime = block.durability;

                std::string heldTool = GetHeldTool();
                bool canDrop = false;

                if (!block.requiredTool.empty() && block.requiredTool != "none") {
                    if (!heldTool.empty()) {
                        if (block.harvestTools.count(heldTool)) {
                            breakTime = block.harvestTools[heldTool];
                            canDrop = true;
                        }
                    }
                }
                else {
                    canDrop = true;
                }

                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    breakProgress += GetFrameTime();

                    int breakStage = static_cast<int>((breakProgress / breakTime) * 10);
                    breakStage = std::min(breakStage, 9);

                    DrawCubeTexture(breakTextures[breakStage], blockPos, 1.1f, 1.1f, 1.1f, WHITE);

                    if (breakProgress >= breakTime) {
                        if (canDrop) {
                            double dropItem = (block.drops > 0) ? block.drops : block.id;
                            addItemToInventory(inventory, dropItem);
                        }

                        chunkMap[chunkPos].blockMap.erase(blockPos);
                        chunkMap[chunkPos].UpdateNeighborBlocks(blockPos, chunkMap, true);
                        chunkMap[chunkPos].IsChanged = true;
                        breakProgress = 0.0;
                    }
                }
                else {
                    breakProgress = 0.0;
                }
            }
        }
    }
    else {
        breakProgress = 0.0;
    }
}





void Player::PlaceBlock(ChunkMap& chunkMap) {
    Vector3 blockPos, hitNormal;

    if (GetBlockLookingAt(position, GetCameraForward(*this), chunkMap, blockPos, hitNormal)) {
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && inventory[inventorySlot][0] != 0.0) {
            Vector3 newBlockPos = {
                round(blockPos.x - hitNormal.x),
                round(blockPos.y - hitNormal.y),
                round(blockPos.z - hitNormal.z)
            };

            Vector2 chunkPos = {
                floor(newBlockPos.x / Chunk::CHUNK_SIZE_X),
                floor(newBlockPos.z / Chunk::CHUNK_SIZE_Z)
            };

            if (chunkMap.count(chunkPos)) {
                if (!chunkMap[chunkPos].blockMap.count(newBlockPos)) {
                    chunkMap[chunkPos].blockMap[newBlockPos] = Block{ inventory[inventorySlot][0], newBlockPos.x, newBlockPos.y, newBlockPos.z};
                    removeItemFromInventory(inventory, inventorySlot);
                    chunkMap[chunkPos].UpdateNeighborBlocks(newBlockPos, chunkMap, true);
                    chunkMap[chunkPos].IsChanged = true;
                }
            }
        }
    }
}


bool Player::CheckCollisionWithChunks(const Vector3& pos, ChunkMap& chunkMap) {
    Vector2 chunkPos = {
        floor(pos.x / Chunk::CHUNK_SIZE_X),
        floor(pos.z / Chunk::CHUNK_SIZE_Z)
    };
    if (chunkMap.count(chunkPos)) {
        return chunkMap[chunkPos].blockMap.count(pos);
    }
    return false;
}

Block* Player::GetBlockAtPosition(const Vector3& pos, ChunkMap& chunkMap) {
    Vector2 chunkPos = {
        floor(pos.x / Chunk::CHUNK_SIZE_X),
        floor(pos.z / Chunk::CHUNK_SIZE_Z)
    };
    if (chunkMap.count(chunkPos)) {
        if (chunkMap[chunkPos].blockMap.count(pos)) {
            return &chunkMap[chunkPos].blockMap[pos];
        }
    }
    return nullptr;
}

void Player::Draw() {
    DrawCube(position, 32, 32, 32, BLUE);
}

void Player::DrawHand(Player& player, Camera3D& camera) {

    float wheelMove = GetMouseWheelMove();
    if (wheelMove != 0 && !isInventory) {
        player.inventorySlot += (wheelMove > 0) ? -1 : 1;
        player.inventorySlot = (player.inventorySlot % 9 + 9) % 9;
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
    if (inventory[inventorySlot][0] != 0.0) DrawCubeTexture(setTexture(getTexture(player.inventory[inventorySlot][0])), stonePos, 0.3f, 0.3f, 0.3f, WHITE);

}