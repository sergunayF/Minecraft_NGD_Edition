#pragma once

#include "../Game/Game.hpp"
#include "../Game/Textures.hpp"
#include "../Block/Miniblocks.hpp"
#include "Player.hpp"

#define SLOT_SIZE 16

extern Rectangle texInventoryRect;
extern Rectangle destInventoryRect;

class inventorySlots;

extern inventorySlots inventorySlotsArray[PLAYER_INITIALIZATION_SLOT];

class inventorySlots {

public:

	int slotID;
	Vector2 position = { -256.0f, -256.0f };

	double ItemID;
	double ItemCount;

	Color GLOW = { 255, 255, 255, 0 };

	inventorySlots(int ID) : slotID(ID), ItemID(-1) {

		if (ID < 9) {

			position = { static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) * ID) - (screenWidth / -2 + texInventoryRect.width) + 8 * GUI_SCALE, (screenHeight / 2 + texInventoryRect.height) - 24 * GUI_SCALE};

		}

		if (ID >= 9 && ID < 36) {

			position = {static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) * (ID % 9)) - (screenWidth / -2 + texInventoryRect.width) + 8 * GUI_SCALE, static_cast<float>(screenHeight / 2) + (static_cast<int>(ID / 9) * (SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE)) - 17 * GUI_SCALE};

		}

	}

	inventorySlots() : slotID(-1), ItemID(-1), position({-64, -64}) {}

	void Draw() {

		DrawRectangle(position.x, position.y, SLOT_SIZE * GUI_SCALE, SLOT_SIZE * GUI_SCALE, GLOW);

		std::string countString = std::to_string(static_cast<int>(ItemCount));

		miniBlocks[slotID].Draw({ position.x - SLOT_SIZE / 4 * GUI_SCALE, position.y - SLOT_SIZE / (GUI_SCALE) });

		if (ItemID != 0.0 && ItemCount > 1.0) DrawText(countString.c_str(), position.x, position.y, 14, WHITE);

	}

};

// 0 - 35 Inventory
// 36 - Cursor
// 37 - 45 - Crafting Table
// 46 = result

void inventorySlotsInit();

void addItemToInventory(double inventory[INVENTORY_SLOTS][INVENTORY_SLOTS_COUNTS], double itemID);

void removeItemFromInventory(double inventory[INVENTORY_SLOTS][INVENTORY_SLOTS_COUNTS], int slot);

int findItemInInventory(double inventory[INVENTORY_SLOTS][INVENTORY_SLOTS_COUNTS], double itemID);

void DrawInventory(double inventory[INVENTORY_SLOTS][INVENTORY_SLOTS_COUNTS], bool isCrafting);