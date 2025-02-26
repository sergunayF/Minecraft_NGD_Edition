#include "Player.hpp"
#include "Camera.hpp"

Player::Player(float x, float y, float z) {

    position = { x, y, z };
    velocity = { 0, 0, 0 };

    isGrounded = false;

    yaw = 0.0f;
    pitch = 0.0f;

    highlightedBlockPos = { -1, -1, -1 };
    inventorySlot = 0;
}

void Player::Update(ChunkMap& chunkMap) {
    velocity.y += gravity;

    Vector3 forward = { cosf(yaw), 0, sinf(yaw) };
    Vector3 right = { sinf(yaw), 0, -cosf(yaw) };

    Vector3 newPos = position;

    if (IsKeyDown(KEY_W)) { newPos.x += forward.x * moveSpeed; newPos.z += forward.z * moveSpeed; }
    if (IsKeyDown(KEY_S)) { newPos.x -= forward.x * moveSpeed; newPos.z -= forward.z * moveSpeed; }
    if (IsKeyDown(KEY_A)) { newPos.x += right.x * moveSpeed; newPos.z += right.z * moveSpeed; }
    if (IsKeyDown(KEY_D)) { newPos.x -= right.x * moveSpeed; newPos.z -= right.z * moveSpeed; }

    bool collisionX = false, collisionZ = false, collisionY = false;

    Vector3 checkX = { round(newPos.x), round(position.y) + 1, round(position.z) };
    Vector3 checkXd = { round(newPos.x), round(position.y) + 2, round(position.z) };
    if (CheckCollisionWithChunks(checkX, chunkMap) || CheckCollisionWithChunks(checkXd, chunkMap)) collisionX = true;

    Vector3 checkZ = { round(position.x), round(position.y) + 1, round(newPos.z) };
    Vector3 checkZd = { round(position.x), round(position.y) + 2, round(newPos.z) };
    if (CheckCollisionWithChunks(checkZ, chunkMap) || CheckCollisionWithChunks(checkZd, chunkMap)) collisionZ = true;

    Vector3 checkY = { round(position.x), round(position.y - velocity.y), round(position.z) };
    if (CheckCollisionWithChunks(checkY, chunkMap)) {
        isGrounded = true;
        velocity.y = 0;
        collisionY = true;
    }
    else {
        position.y -= velocity.y;
    }

    if (IsKeyDown(KEY_SPACE) && isGrounded) {
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

    BreakBlock(chunkMap);
    PlaceBlock(chunkMap);
}

void Player::BreakBlock(ChunkMap& chunkMap) {
    Vector3 blockPos, hitNormal;

    if (GetBlockLookingAt(position, GetCameraForward(*this), chunkMap, blockPos, hitNormal)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 chunkPos = {
                floor(blockPos.x / Chunk::CHUNK_SIZE_X),
                floor(blockPos.z / Chunk::CHUNK_SIZE_Z)
            };

            if (chunkMap.count(chunkPos)) {
                chunkMap[chunkPos].blockMap.erase(blockPos);
                chunkMap[chunkPos].UpdateNeighborBlocks(blockPos, chunkMap, true);
            }
        }
    }
}

void Player::PlaceBlock(ChunkMap& chunkMap) {
    Vector3 blockPos, hitNormal;

    if (GetBlockLookingAt(position, GetCameraForward(*this), chunkMap, blockPos, hitNormal)) {
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
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
                    chunkMap[chunkPos].blockMap[newBlockPos] = Block{ inventory[inventorySlot], newBlockPos.x, newBlockPos.y, newBlockPos.z };
                    chunkMap[chunkPos].UpdateNeighborBlocks(newBlockPos, chunkMap, true);
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

}