#include "Game.hpp"

#include "../Engine/Vector.hpp"

#include <stdexcept>
#include <sstream>

std::string savePath = "saves/world_" + std::to_string(worldSeed) + "/LevelData";

float nextMusicTime = (rand() % 11 + 10) * 60.0f;
float musicTimer = nextMusicTime - 100.0f;

std::string getBlockName(double value) {

    std::ifstream inputFile(ASSETS_PATH"IDs.json");

    if (!inputFile.is_open()) {
        return "Failed to open file!";
    }

    nlohmann::json jsonData;
    inputFile >> jsonData;

    std::ostringstream oss;
    oss << value;
    std::string strValue = oss.str();
    size_t dotPos = strValue.find('.');

    std::string key;
    if (dotPos != std::string::npos) {
        std::string integerPart = strValue.substr(0, dotPos);
        std::string decimalPart = strValue.substr(dotPos + 1);
        key = integerPart + ":" + decimalPart;
    }
    else {
        key = strValue + ":0";
    }

    if (jsonData.contains(key)) {
        return jsonData[key]["Name"];
    }
    else {
        return "Empty";
    }
}

std::unordered_map<std::string, BlockData> blockDataMap;

std::unordered_map<std::string, BlockData> loadBlockData() {
    std::ifstream inputFile(ASSETS_PATH "IDs.json");
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open file!");
    }

    nlohmann::json jsonData;
    inputFile >> jsonData;

    std::unordered_map<std::string, BlockData> blockDataMap;

    for (auto& elem : jsonData.items()) {
        BlockData data;

        data.name = elem.value()["Name"];

        if (!elem.value().contains("Item")) {
            if (!elem.value().contains("durability")) {
                throw std::runtime_error("Field 'durability' is required when 'IsItem' is missing!");
            }
            data.durability = elem.value()["durability"];
        }
        else {
            if (elem.value().contains("durability")) data.durability = elem.value()["durability"];
            else data.durability = 0.0;
        }

        if (elem.value().contains("drops")) data.drops = elem.value()["drops"];

        if (elem.value().contains("sound")) data.sound = elem.value()["sound"];
        if (elem.value().contains("sound3")) data.sound3 = elem.value()["sound3"];

        if (elem.value().contains("harvestTools")) {
            for (auto& tool : elem.value()["harvestTools"].items()) {
                data.harvestTools[tool.key()] = tool.value();
            }
        }

        if (elem.value().contains("requiredTool")) data.requiredTool = elem.value()["requiredTool"];

        if (elem.value().contains("sixTextures") && elem.value()["sixTextures"].is_array()) {
            data.hasSixTextures = true;
            for (const auto& textureId : elem.value()["sixTextures"]) {
                data.sixTextures.push_back(textureId);
            }
        }

        if (elem.value().contains("Grass")) data.grass = elem.value()["Grass"];
        if (elem.value().contains("fuel")) data.fuel = elem.value()["fuel"];
        if (elem.value().contains("meltOut")) data.meltOut = elem.value()["meltOut"];
        if (elem.value().contains("Billboard")) data.billboard = elem.value()["Billboard"];
        if (elem.value().contains("Transparency")) data.transparency = elem.value()["Transparency"];

        if (elem.value().contains("TexturePosition") && elem.value()["TexturePosition"].is_array()) {
            auto texturePosArray = elem.value()["TexturePosition"];
            if (texturePosArray.size() >= 2) {
                data.texturePosition.x = texturePosArray[0];
                data.texturePosition.y = texturePosArray[1];
            }
        }

        if (elem.value().contains("Item")) data.isItem = elem.value()["Item"];

        if (elem.value().contains("Stack")) data.Stack = elem.value()["Stack"];

        blockDataMap[elem.key()] = data;
    }

    return blockDataMap;
}

std::string getTexture(double value) {
    std::ostringstream oss;
    oss << value;
    std::string strValue = oss.str();

    size_t dotPos = strValue.find('.');

    std::string key;

    if (dotPos != std::string::npos) {
        std::string integerPart = strValue.substr(0, dotPos);
        std::string decimalPart = strValue.substr(dotPos + 1);
        key = integerPart + ":" + decimalPart;
    }
    else {
        key = strValue + ":0";
    }

    return key;
}

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color, bool neighbors[6]) {

    float x = position.x;
    float y = position.y;
    float z = position.z;

    rlSetTexture(texture.id);

    rlBegin(RL_QUADS);

    rlColor4ub(color.r, color.g, color.b, color.a);

    // Front Face (Z+)
    if (!neighbors[4]) {
        rlNormal3f(0.0f, 0.0f, 1.0f);
        rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left
        rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right
        rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right
        rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left
    }

    // Back Face (Z-)
    if (!neighbors[5]) {
        rlNormal3f(0.0f, 0.0f, -1.0f);
        rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Right
        rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Right
        rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Left
        rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Left
    }

    // Top Face (Y+)
    if (!neighbors[2]) {
        rlNormal3f(0.0f, 1.0f, 0.0f);
        rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
        rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Bottom Left
        rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Bottom Right
        rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
    }

    // Bottom Face (Y-)
    if (!neighbors[3]) {
        rlNormal3f(0.0f, -1.0f, 0.0f);
        rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Right
        rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Top Left
        rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left
        rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Right
    }

    // Right Face (X+)
    if (!neighbors[0]) {
        rlNormal3f(1.0f, 0.0f, 0.0f);
        rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right
        rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
        rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Left
        rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left
    }

    // Left Face (X-)
    if (!neighbors[1]) {
        rlNormal3f(-1.0f, 0.0f, 0.0f);
        rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left
        rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Right
        rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Right
        rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
    }

    rlEnd();

    rlSetTexture(0);
}

void DrawCubeSixTexture(Texture2D textures[6], Vector3 position, float width, float height, float length, Color color, double blockID)
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


void DrawGrassBlock(Vector3 position, float width, float height, float length, Texture2D(texturesArray)[TEXTURE_ARRAY_ROWS][TEXTURE_ARRAY_COLS], bool neighbors[6]) {

    DrawCubeTexture(texturesArray[2][1], {position.x, position.y - 0.03f + height / 2, position.z}, width - 0.001f, 0.1f, length - 0.001f, WHITE, neighbors);
    DrawCubeTexture(texturesArray[2][0], position, width, height, length, WHITE, neighbors);
    DrawCubeTexture(texturesArray[2][2], position, width, height, length, WHITE, neighbors);

}

void DrawBillboardBlock(Texture2D texture, Vector3 position, float size, Color color)
{
    float halfSize = size * 0.5f;

    Matrix transform1 = MatrixRotateXYZ({ 0.0f, DEG2RAD * 45.0f, 0.0f });
    Matrix transform2 = MatrixRotateXYZ({ 0.0f, DEG2RAD * (45.0f + 90.0f), 0.0f });

    auto DrawQuad = [&](Matrix transform)
        {
            Vector3 v1 = { -halfSize,  halfSize, 0 };
            Vector3 v2 = { halfSize,  halfSize, 0 };
            Vector3 v3 = { halfSize, -halfSize, 0 };
            Vector3 v4 = { -halfSize, -halfSize, 0 };

            v1 = TransformVectors(v1, transform);
            v2 = TransformVectors(v2, transform);
            v3 = TransformVectors(v3, transform);
            v4 = TransformVectors(v4, transform);

            v1 = AddVectors(v1, position);
            v2 = AddVectors(v2, position);
            v3 = AddVectors(v3, position);
            v4 = AddVectors(v4, position);

            Vector3 v5 = v1;
            Vector3 v6 = v2;
            Vector3 v7 = v3;
            Vector3 v8 = v4;

            rlDisableBackfaceCulling();

            rlSetTexture(texture.id);
            rlBegin(RL_QUADS);
            rlColor4ub(color.r, color.g, color.b, color.a);

            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(v1.x, v1.y, v1.z);
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(v2.x, v2.y, v2.z);
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(v3.x, v3.y, v3.z);
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(v4.x, v4.y, v4.z);

            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(v8.x, v8.y, v8.z);
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(v7.x, v7.y, v7.z);
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(v6.x, v6.y, v6.z);
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(v5.x, v5.y, v5.z);

            rlEnd();
            rlSetTexture(0);

            rlEnableBackfaceCulling();
        };

    DrawQuad(transform1);
    DrawQuad(transform2);
}