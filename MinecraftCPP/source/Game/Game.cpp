#include "Game.hpp"

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

    std::ifstream inputFile(ASSETS_PATH"IDs.json");
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open file!");
    }

    nlohmann::json jsonData;
    inputFile >> jsonData;

    for (auto& elem : jsonData.items()) {
        BlockData data;
        data.name = elem.value()["Name"];
        data.texture = elem.value()["Texture"];
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
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left Of The Texture and Quad
    // Back Face
    rlNormal3f(0.0f, 0.0f, -1.0f);     // Normal Pointing Away From Viewer
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Left Of The Texture and Quad
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
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
    // Left Face
    rlNormal3f(-1.0f, 0.0f, 0.0f);    // Normal Pointing Left
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
    rlEnd();
    //rlPopMatrix();

    rlSetTexture(0);
}

// Draw cube with texture piece applied to all faces
void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;
    float texWidth = (float)texture.width;
    float texHeight = (float)texture.height;

    // Set desired texture to be enabled while drawing following vertex data
    rlSetTexture(texture.id);

    // We calculate the normalized texture coordinates for the desired texture-source-rectangle
    // It means converting from (tex.width, tex.height) coordinates to [0.0f, 1.0f] equivalent 
    rlBegin(RL_QUADS);
    rlColor4ub(color.r, color.g, color.b, color.a);

    // Front face
    rlNormal3f(0.0f, 0.0f, 1.0f);
    rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
    rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(source.x / texWidth, source.y / texHeight);
    rlVertex3f(x - width / 2, y + height / 2, z + length / 2);

    // Back face
    rlNormal3f(0.0f, 0.0f, -1.0f);
    rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
    rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(source.x / texWidth, source.y / texHeight);
    rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x + width / 2, y - height / 2, z - length / 2);

    // Top face
    rlNormal3f(0.0f, 1.0f, 0.0f);
    rlTexCoord2f(source.x / texWidth, source.y / texHeight);
    rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
    rlVertex3f(x + width / 2, y + height / 2, z - length / 2);

    // Bottom face
    rlNormal3f(0.0f, -1.0f, 0.0f);
    rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
    rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(source.x / texWidth, source.y / texHeight);
    rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x - width / 2, y - height / 2, z + length / 2);

    // Right face
    rlNormal3f(1.0f, 0.0f, 0.0f);
    rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
    rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(source.x / texWidth, source.y / texHeight);
    rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x + width / 2, y - height / 2, z + length / 2);

    // Left face
    rlNormal3f(-1.0f, 0.0f, 0.0f);
    rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
    rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(source.x / texWidth, source.y / texHeight);
    rlVertex3f(x - width / 2, y + height / 2, z - length / 2);

    rlEnd();

    rlSetTexture(0);
}