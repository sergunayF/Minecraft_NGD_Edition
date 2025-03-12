#pragma once

#include "Player.hpp"


class Mob :public Entity {
public:
    int id = 0;

    Mob() {
        position = { 0, 0, 0 };
        velocity = { 0, 0, 0 };

        isGrounded = false;
    }

    Mob(float x, float y, float z);

//    virtual void DrawHitBox(const Vector3& pos, int WidthX, int Height, int WidthZ) override;
    virtual void Update(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex) override;
    virtual void Draw() override;
    virtual void DrawTexture(Texture2D textures[6], Vector3 position, float width, float height, float length, Color color) override;
    virtual bool CheckCollisionWithChunks(const Vector3& pos, ChunkMap& chunkMap) override;
    virtual Block* GetBlockAtPosition(const Vector3& pos, ChunkMap& chunkMap) override;

private:

};

class Zombie:public Mob {
public:
    Zombie(float x, float y, float z);
    virtual void Update(ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex) override;
    void SetTexture(const Vector3&);

};