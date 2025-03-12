#include "Mobs.hpp"

Mob::Mob(float x, float y, float z) {

    position = { x, y, z };
    velocity = { 0, 0, 0 };

    isGrounded = false;

}

Block* Mob::GetBlockAtPosition(const Vector3& pos, ChunkMap& chunkMap) {
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

void Mob::Update(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex) {
    velocity.y += gravity;

    static float lastY = position.y;


    Vector3 newPos = position;


    bool collisionX = false, collisionZ = false, collisionY = false;
    const float MobHeight = 1.8f;

    for (float yOffset = 0; yOffset < MobHeight; yOffset += 1.0f) {
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

    Vector3 checkCeiling = { round(position.x), round(position.y + MobHeight), round(position.z) };
    if (CheckCollisionWithChunks(checkCeiling, chunkMap)) {
        velocity.y = 0;
        isGrounded = false;
        position.y -= 0.02f;
    }


    if (!collisionX) position.x = newPos.x;
    if (!collisionZ) position.z = newPos.z;
    if (!collisionY) position.y -= velocity.y;

    if (isGrounded) lastY = position.y;

}

bool Mob::CheckCollisionWithChunks(const Vector3& pos, ChunkMap& chunkMap) {
    Vector2 chunkPos = {
        floor(pos.x / Chunk::CHUNK_SIZE_X),
        floor(pos.z / Chunk::CHUNK_SIZE_Z)
    };
    if (chunkMap.count(chunkPos)) {
        return chunkMap[chunkPos].blockMap.count(pos);
    }
    return false;
}

void Mob::Draw() {
    DrawCube(position, 32, 32, 32, BLUE);
}

//void Mob::DrawHitBox(const Vector3& pos, int WidthX, int Height, int WidthZ) {
//    DrawCubeWires(pos, WidthX, Height, WidthZ, {0,0,0,0});
//}

void Mob::DrawTexture(Texture2D textures[6], Vector3 position, float width, float height, float length, Color color)
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

Zombie::Zombie(float x, float y, float z) {

    position = { x, y, z };
    velocity = { 0, 0, 0 };

    isGrounded = false;
}

void Zombie::SetTexture(const Vector3& pos) {

    //head
    Vector3 tempPos = { pos.x,pos.y + 1.8f,pos.z }; 
    Texture2D tmp[6];
    for (int i = 0; i < 6; i++) {
        tmp[i] = zombie[i+1];
    }
    DrawTexture(tmp, tempPos, 0.5f, 0.5f, 0.5f, WHITE);

    //body
    tempPos = { pos.x,pos.y + 1.15f,pos.z };
    for (int i = 0; i < 6; i++) {
        tmp[i] = zombie[7+i];
    }
    DrawTexture(tmp, tempPos, 0.5f, 0.8f, 0.3f, WHITE);

    //legs
    tempPos = { pos.x-0.125f,pos.y + 0.4f,pos.z };
    for (int i = 0; i < 6; i++) {
        tmp[i] = zombie[13 + i];
    }
    DrawTexture(tmp, tempPos, 0.25f, 0.75f, 0.3f, WHITE);
    tempPos = { pos.x+0.125f,pos.y+0.4f,pos.z };
    for (int i = 0; i < 6; i++) {
        tmp[i] = zombie[13 + i];
    }
    DrawTexture(tmp, tempPos, 0.25f, 0.75f, 0.3f, WHITE);

    //arms
    tempPos = { pos.x - 0.375f,pos.y + 1.175f,pos.z };
    for (int i = 0; i < 6; i++) {
        tmp[i] = zombie[19 + i];
    }
    DrawTexture(tmp, tempPos, 0.25f, 0.75f, 0.3f, WHITE);

    tempPos = { pos.x + 0.375f,pos.y + 1.175f,pos.z };
    for (int i = 0; i < 6; i++) {
        tmp[i] = zombie[19 + i];
    }
    DrawTexture(tmp, tempPos, 0.25f, 0.75f, 0.3f, WHITE);


}

void Zombie::Update(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex) {
    velocity.y += gravity;

    static float lastY = position.y;


    Vector3 newPos = position;


    bool collisionX = false, collisionZ = false, collisionY = false;
    const float MobHeight = 1.8f;

    for (float yOffset = 0; yOffset < MobHeight; yOffset += 1.0f) {
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

    Vector3 checkCeiling = { round(position.x), round(position.y + MobHeight), round(position.z) };
    if (CheckCollisionWithChunks(checkCeiling, chunkMap)) {
        velocity.y = 0;
        isGrounded = false;
        position.y -= 0.02f;
    }


    if (!collisionX) position.x = newPos.x;
    if (!collisionZ) position.z = newPos.z;
    if (!collisionY) position.y -= velocity.y;

    if (isGrounded) lastY = position.y;

    SetTexture(position);
}