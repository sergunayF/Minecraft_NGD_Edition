#pragma once

#include "../Game/Game.hpp"
#include "../Block/Block.hpp"
#include "../Engine/Vector3Hash.hpp"

#include "Player.hpp"

Camera3D InitCamera();

void UpdateCameraRotation(Player& player);
void UpdateCameraPosition(Camera3D& camera, const Player& player);

Vector3 GetCameraForward(const Player& player);

Vector3 normalized(float x, float y, float z);

bool GetBlockLookingAt(Vector3 playerPos, Vector3 forward, ChunkMap& chunkMap, Vector3& outBlockPos, Vector3& outHitNormal, std::shared_mutex& chunkMapMutex);

void DrawCrosshair(Player& player, ChunkMap& chunkMap, std::shared_mutex& chunkMapMutex);