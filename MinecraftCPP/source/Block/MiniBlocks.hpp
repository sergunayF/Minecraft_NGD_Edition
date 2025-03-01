#pragma once

#include "../Game/Game.hpp"
#include "../Block/Block.hpp"
#include "../Game/Textures.hpp"

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

    void Render(Texture texturesArray[TEXTURE_ARRAY_ROWS][TEXTURE_ARRAY_COLS]) {

        if (rendered) return;

        std::string textureName = getTexture(BlockID);

        if (blockDataMap[textureName].billboard || blockDataMap[textureName].isItem) {

            size_t pos = textureName.find(':');
            int a = std::stoi(textureName.substr(0, pos));
            int b = std::stoi(textureName.substr(pos + 1));

            BeginTextureMode(renderTexture);
            ClearBackground(BLANK);

            if (blockDataMap[textureName].isItem) {
                Rectangle texItemsRect = { blockDataMap[textureName].texturePosition.x, blockDataMap[textureName].texturePosition.y, 16, 16};
                Rectangle destItemsRect = { 0, 0, texItemsRect.width * GUI_SCALE, texItemsRect.height * GUI_SCALE };

                DrawTexturePro(itemsGUI, texItemsRect, destItemsRect, { -8 * (GUI_SCALE - 1), -8 * (GUI_SCALE - 1) }, 0.0f, WHITE);
                EndTextureMode();

                rendered = true;
                return;
            }

            DrawTextureEx(texturesArray[a][b], { 12 * (GUI_SCALE - 1), 12 * (GUI_SCALE - 1)}, 0.0f, scale.x, WHITE);
            EndTextureMode();

            rendered = true;
            return;

        }

        BeginTextureMode(renderTexture);
        ClearBackground(BLANK);

        BeginMode3D(camera);
        rlPushMatrix();

        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(30.0f, 1.0f, 0.0f, 0.0f);
        rlRotatef(-45.0f, 0.0f, 1.0f, 0.0f);

        Block tempBlock(BlockID, 0.0f, 0.0f, 0.0f);
        tempBlock.blockSize = scale.x;
        tempBlock.Draw(false);

        rlPopMatrix();
        EndMode3D();
        EndTextureMode();

        rendered = true;

    }

    void Draw(Vector2 screenPos) const {

        if (BlockID != 0.0) DrawTextureRec(renderTexture.texture, { 0, 0, (float)TEXTURE_SIZE, (float)-TEXTURE_SIZE }, screenPos, WHITE);
    
    }
};
