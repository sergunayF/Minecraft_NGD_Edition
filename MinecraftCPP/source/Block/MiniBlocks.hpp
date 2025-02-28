#pragma once

#include "../Game/Game.hpp"

class MiniBlock {
private:

    RenderTexture2D renderTexture{};
    Camera3D camera{};
    
    Vector3 position{};
    Vector3 scale{};

public:

    static constexpr int TEXTURE_SIZE = 48;

    double BlockID;
    Texture2D blockTexture{};

    bool rendered = false;

    MiniBlock(double id, Texture2D texture, Vector3 pos, Vector3 size)
        : BlockID(id), blockTexture(std::move(texture)), position(pos), scale(size) {

        renderTexture = LoadRenderTexture(TEXTURE_SIZE, TEXTURE_SIZE);

        camera.position = { 0.0f, 0.0f, 5.0f };
        camera.target = { 0.0f, 0.0f, 0.0f };
        camera.up = { 0.0f, 1.0f, 0.0f };
        camera.fovy = 45.0f;
        camera.projection = CAMERA_PERSPECTIVE;
    }

    MiniBlock(const MiniBlock&) = delete;
    MiniBlock& operator=(const MiniBlock&) = delete;

    MiniBlock(MiniBlock&& other) noexcept
        : renderTexture(other.renderTexture),
        camera(other.camera),
        blockTexture(other.blockTexture),
        position(other.position),
        scale(other.scale) {
        other.renderTexture = {};
        other.blockTexture = {};
    }

    MiniBlock& operator=(MiniBlock&& other) noexcept {
        if (this != &other) {
            UnloadRenderTexture(renderTexture);
            renderTexture = other.renderTexture;
            camera = other.camera;
            blockTexture = other.blockTexture;
            position = other.position;
            scale = other.scale;

            other.renderTexture = {};
            other.blockTexture = {};
        }
        return *this;
    }

    ~MiniBlock() {
        UnloadRenderTexture(renderTexture);
    }

    void Render() {

        if (rendered) return;

        BeginTextureMode(renderTexture);
        ClearBackground(BLANK);

        BeginMode3D(camera);
        rlPushMatrix();

        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(30.0f, 1.0f, 0.0f, 0.0f);
        rlRotatef(-45.0f, 0.0f, 1.0f, 0.0f);

        DrawCubeTexture(blockTexture, { 0.0f, 0.0f, 0.0f }, scale.x, scale.y, scale.z, WHITE);

        rlPopMatrix();
        EndMode3D();
        EndTextureMode();

        rendered = true;

    }

    void Draw(Vector2 screenPos) const {

        if (BlockID != 0.0) DrawTextureRec(renderTexture.texture, { 0, 0, (float)TEXTURE_SIZE, (float)-TEXTURE_SIZE }, screenPos, WHITE);
    
    }
};
