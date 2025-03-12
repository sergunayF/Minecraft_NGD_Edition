#include "Player.hpp"
#include "Camera.hpp"
#include "../Game/Sounds.hpp"

Player::Player(float x, float y, float z) {

    position = { x, y, z };
    velocity = { 0, 0, 0 };

    isGrounded = false;
    isInventory = false;
    isCrafting = false;

    yaw = 0.0f;
    pitch = 0.0f;

    highlightedBlockPos = { -1, -1, -1 };

    for (int i = 0; i < PLAYER_INITIALIZATION_SLOT; i++) {
        inventory[i][0] = 0.0;
        inventory[i][1] = 0.0;
    }

    inventorySlot = 0;

}

void Player::Draw() {
    DrawCube(position, 32, 32, 32, BLUE);
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

bool Player::CheckCollisionWithChunks(const Vector3& pos, ChunkMap& chunkMap) {

    Vector2 chunkPos = {
        floor(pos.x / Chunk::CHUNK_SIZE_X),
        floor(pos.z / Chunk::CHUNK_SIZE_Z)
    };

    if (chunkMap.count(chunkPos)) {
        Chunk& chunk = chunkMap[chunkPos];

        if (chunk.blockMap.count(pos)) {
            Block& block = chunk.blockMap[pos];

            if (blockDataMap[getTexture(block.id)].billboard) {
                return false;
            }

            return true;
        }
    }

    return false;
}

void Player::Update(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex) {
    velocity.y += gravity;

    static float lastY = position.y;

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

    if (IsKeyPressed(KEY_F3) && !isInventory) debug = !debug;

    if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ESCAPE)) {
        if (isCrafting) isCrafting = !isCrafting;
        if (isFurnace) {
            static_cast<FurnaceBlock*>(activeBlock)->SaveState(*this);
            isFurnace = !isFurnace;
        }
        if (isInventory) {
            for (int i = 37; i < PLAYER_INITIALIZATION_SLOT; i++) inventorySlotsArray[i].position = { -256 * GUI_SCALE, -256 * GUI_SCALE };

        }

        if (IsKeyPressed(KEY_E) || (IsKeyPressed(KEY_ESCAPE) && isInventory)) {
            isInventory = !isInventory;
            isInventory ? EnableCursor() : DisableCursor();
        }
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

        if (!isGrounded) {
            float fallDistance = lastY - position.y;
            if (fallDistance > 3.0f) {
                int damage = static_cast<int>(fallDistance - 3);
                HP = std::max(0, HP - damage);
            }
        }

        isGrounded = true;

        velocity.y = 0;
        collisionY = true;
        position.y = round(position.y - 0.5f) + 0.5f;
    }
    else {
        isGrounded = false;
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
    if (!GetBlockLookingAt(position, GetCameraForward(*this), chunkMap, blockPos, hitNormal)) highlightedBlockPos = { -1, -1, -1 };
    else highlightedBlockPos = blockPos;

    if (!isInventory) {
        BreakBlock(chunkMap, chunkMapMutex);
        PlaceBlock(chunkMap);
    }

    if (isGrounded) lastY = position.y;

    Vector3 underPlayerPos = { round(position.x), round(position.y - 1), round(position.z) };
    Block* underBlock = GetBlockAtPosition(underPlayerPos, chunkMap);

    if (underBlock && isGrounded && (IsKeyDown(KEY_S) || IsKeyDown(KEY_D) || IsKeyDown(KEY_A) || IsKeyDown(KEY_W)) && !isInventory) {
        playingSound(underBlock->id, 2);
    }
}


std::string Player::GetHeldTool() {

    return blockDataMap[getTexture(inventory[inventorySlot][0])].name;

}

void Player::BreakBlock(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex) {
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

                if (block.requiredTool.empty()) {
                    canDrop = true;
                    if (!heldTool.empty() && block.harvestTools.count(heldTool)) {
                        breakTime = block.harvestTools.at(heldTool);
                    }
                }
                else {
                    if (!heldTool.empty() && block.harvestTools.count(heldTool)) {
                        if (block.harvestTools.at(heldTool) <= block.harvestTools.at(block.requiredTool)) {
                            canDrop = true;
                            breakTime = block.harvestTools.at(heldTool);
                        }
                    }
                }

                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    breakProgress += GetFrameTime();

                    int breakStage = static_cast<int>((breakProgress / breakTime) * 10);
                    breakStage = std::min(breakStage, 9);

                    DrawCubeTexture(breakTextures[breakStage], blockPos, 1.05f, 1.05f, 1.05f, WHITE, chunkMap[chunkPos].blockMap[blockPos].neighbors);

                    playingSound(block.id, 0);

                    if (breakProgress >= breakTime) {
                        if (canDrop) {
                            double dropItem = (block.drops > 0) ? block.drops : block.id;
                            addItemToInventory(inventory, dropItem);
                        }

                        playingSound(block.id, 1);

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

    GetBlockLookingAt(position, GetCameraForward(*this), chunkMap, blockPos, hitNormal);

    Block* tempBlock = GetBlockAtPosition(blockPos, chunkMap);

    if (!IsKeyDown(KEY_LEFT_SHIFT) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && tempBlock != nullptr) {
        if (tempBlock->id == 58.0) {
            isInventory = true;
            isCrafting = true;
            isInventory ? EnableCursor() : DisableCursor();
        }
        else if (tempBlock->id == 61.0 || tempBlock->id == 62.0) {
            static_cast<FurnaceBlock*>(tempBlock)->LoadState(*this);
            activeBlock = tempBlock;
            isInventory = true;
            isFurnace = true;
            isInventory ? EnableCursor() : DisableCursor();
        }
    }

    if (GetBlockLookingAt(position, GetCameraForward(*this), chunkMap, blockPos, hitNormal)) {
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && inventory[inventorySlot][0] != 0.0 && blockDataMap[getTexture(inventory[inventorySlot][0])].isItem == false && isInventory == false) {
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
                    Block newBlock = { inventory[inventorySlot][0], newBlockPos.x, newBlockPos.y, newBlockPos.z };
                    chunkMap[chunkPos].blockMap[newBlockPos] = newBlock;
                    playingSound(newBlock.id, 1);
                    removeItemFromInventory(inventory, inventorySlot);
                    chunkMap[chunkPos].UpdateNeighborBlocks(newBlockPos, chunkMap, true);
                    chunkMap[chunkPos].IsChanged = true;
                }
            }
        }
    }
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

    double bobbing = sin(GetTime() * 8) * 0.05;
    stonePos.y += bobbing;

    double stoneRotation = sin(GetTime() * 5) * 5.0;

    if (inventory[inventorySlot][0] != 0.0 && blockDataMap[getTexture(inventory[inventorySlot][0])].isItem == false) {

        Block tempBlock(inventory[inventorySlot][0], stonePos.x, stonePos.y, stonePos.z);
        tempBlock.blockSize = 0.3f;
        tempBlock.Draw(false);

    }
}


void Player::DrawTexture(Texture2D textures[6], Vector3 position, float width, float height, float length, Color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;

    rlBegin(RL_QUADS);

    // Front Face
    rlSetTexture(textures[0].id);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlNormal3f(0.0f, 0.0f, 1.0f);
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);

    // Back Face
    rlSetTexture(textures[1].id);
    rlNormal3f(0.0f, 0.0f, -1.0f);
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);

    // Top Face
    rlSetTexture(textures[2].id);
    rlNormal3f(0.0f, 1.0f, 0.0f);
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);

    // Bottom Face
    rlSetTexture(textures[3].id);
    rlNormal3f(0.0f, -1.0f, 0.0f);
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);

    // Right Face
    rlSetTexture(textures[4].id);
    rlNormal3f(1.0f, 0.0f, 0.0f);
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);

    // Left Face
    rlSetTexture(textures[5].id);
    rlNormal3f(-1.0f, 0.0f, 0.0f);
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);

    rlEnd();

    rlSetTexture(0);
}
