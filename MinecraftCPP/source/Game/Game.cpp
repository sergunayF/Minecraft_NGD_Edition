#include "Game.hpp"

#include "../Engine/Vector.hpp"

#include <stdexcept>
#include <sstream>

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

    for (auto& elem : jsonData.items()) {
        BlockData data;
        data.name = elem.value()["Name"];
        data.texture = elem.value()["Texture"];
        data.durability = elem.value()["durability"];

        if (elem.value().contains("drops")) {
            data.drops = elem.value()["drops"];
        }
        else {
            data.drops = 0;
        }

        if (elem.value().contains("harvestTools")) {
            for (auto& tool : elem.value()["harvestTools"].items()) {
                data.harvestTools[tool.key()] = tool.value();
            }
        }

        if (elem.value().contains("requiredTool")) {
            data.requiredTool = elem.value()["requiredTool"];
        }
        else {
            data.requiredTool = "none";
        }

        if (elem.value().contains("Grass")) {
            data.grass = elem.value()["Grass"];
        }
        if (elem.value().contains("Billboard")) {
            data.billboard = elem.value()["Billboard"];
        }
        if (elem.value().contains("Transparency")) {
            data.transparency = elem.value()["Transparency"];
        }

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

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;

    rlSetTexture(texture.id);

    rlBegin(RL_QUADS);

    rlColor4ub(color.r, color.g, color.b, color.a);
    // Front Face
    rlNormal3f(0.0f, 0.0f, 1.0f);       // Normal Pointing Towards Viewer
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left Of The Texture and Quad
    // Back Face
    rlNormal3f(0.0f, 0.0f, -1.0f);     // Normal Pointing Away From Viewer
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Left Of The Texture and Quad
    // Top Face
    rlNormal3f(0.0f, 1.0f, 0.0f);       // Normal Pointing Up
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
    // Bottom Face
    rlNormal3f(0.0f, -1.0f, 0.0f);     // Normal Pointing Down
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
    // Right face
    rlNormal3f(1.0f, 0.0f, 0.0f);       // Normal Pointing Right
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Left Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
    // Left Face
    rlNormal3f(-1.0f, 0.0f, 0.0f);    // Normal Pointing Left
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
    rlEnd();
    //rlPopMatrix();

    rlSetTexture(0);
}


void DrawGrassBlock(Vector3 position, float width, float height, float length, Texture2D(texturesArray)[TEXTURE_ARRAY_ROWS][TEXTURE_ARRAY_COLS]) {

    DrawCubeTexture(texturesArray[2][1], {position.x, position.y - 0.03f + height / 2, position.z}, width - 0.001f, 0.1f, length - 0.001f, WHITE);
    DrawCubeTexture(texturesArray[2][0], position, width, height, length, WHITE);
    DrawCubeTexture(texturesArray[2][2], position, width, height, length, WHITE);

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

