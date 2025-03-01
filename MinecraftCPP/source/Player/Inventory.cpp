#include "Inventory.hpp"

#include "../GUI/Crafting.hpp"

Rectangle texInventoryRect = { 0, 0, 176, 166 };
Rectangle destInventoryRect = { 0, 0, texInventoryRect.width * GUI_SCALE, texInventoryRect.height * GUI_SCALE };

inventorySlots inventorySlotsArray[PLAYER_INITIALIZATION_SLOT];

void inventorySlotsInit() {

	for (int ID = 0; ID < PLAYER_INITIALIZATION_SLOT; ID++) {

		if (ID != 36) inventorySlotsArray[ID] = inventorySlots(ID);

	}

}

void addItemToInventory(double inventory[INVENTORY_SLOTS][INVENTORY_SLOTS_COUNTS], double itemID) {

	int tempSlot = findItemInInventory(inventory, itemID);

	if (tempSlot != -1) {

		inventory[tempSlot][1] += 1;

	}
	else {

		for (int i = 0; i < INVENTORY_SLOTS; i++) {

			if (inventory[i][0] == 0.0) {
				
				inventory[i][0] = itemID;
				inventory[i][1] = 1;
				
				return;

			}
		}
	}
}

void removeItemFromInventory(double inventory[INVENTORY_SLOTS][INVENTORY_SLOTS_COUNTS], int slot) {

	if (inventory[slot][1] <= 1) {

		inventory[slot][0] = 0.0;
		inventory[slot][1] = 0.0;

	}
	else {

		inventory[slot][1] -= 1;

	}

}

int findItemInInventory(double inventory[INVENTORY_SLOTS][INVENTORY_SLOTS_COUNTS], double itemID) {


	for (int i = 0; i < INVENTORY_SLOTS; i++) {

		if (inventory[i][0] == itemID && inventory[i][1] < 64.0) return i;

	}

	return -1;

}

void DrawInventory(double inventory[INVENTORY_SLOTS][INVENTORY_SLOTS_COUNTS], bool isCrafting) {

	DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 150});

	std::vector<double> craft_grid;

	int index = 0;	

	if (!isCrafting) {

		DrawTexturePro(inventoryGUI, texInventoryRect, destInventoryRect, { screenWidth / -2 + texInventoryRect.width, screenHeight / -2 + texInventoryRect.height }, 0.0f, WHITE);

		for (int i = 37; i <= 41; i++) {

			if (i == 39) continue;

			miniBlocks[i].Render(texturesArray);
			inventorySlotsArray[i].ItemID = inventory[i][0];
			inventorySlotsArray[i].ItemCount = inventory[i][1];
			inventorySlotsArray[i].Draw();

			inventorySlotsArray[i].position = {
			static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) * (index % 2)) - (screenWidth / -2 + texInventoryRect.width) + 88 * GUI_SCALE,
			static_cast<float>(screenHeight / 2) - 21 * GUI_SCALE - (SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) * (2 - index / 2)
			};

			index++;

		}

		inventorySlotsArray[RESULT_SLOT].position = { static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) + 38 * GUI_SCALE) - (screenWidth / -2 + texInventoryRect.width) + 88 * GUI_SCALE,
			static_cast<float>(screenHeight / 2) - 21 * GUI_SCALE - (SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) - 8 * GUI_SCALE };

		craft_grid = { inventory[37][0], inventory[38][0], 0.0, inventory[40][0], inventory[41][0], 0.0, 0.0, 0.0, 0.0 };

		craft(craft_grid, inventory);

	}
	else if (isCrafting) {

		DrawTexturePro(craftingGUI, texInventoryRect, destInventoryRect, { screenWidth / -2 + texInventoryRect.width, screenHeight / -2 + texInventoryRect.height }, 0.0f, WHITE);

		for (int i = 37; i < 46; i++) {

			miniBlocks[i].Render(texturesArray);
			inventorySlotsArray[i].ItemID = inventory[i][0];
			inventorySlotsArray[i].ItemCount = inventory[i][1];
			inventorySlotsArray[i].Draw();

			inventorySlotsArray[i].position = {
			static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) * (index % 3)) - (screenWidth / -2 + texInventoryRect.width) + 30 * GUI_SCALE,
			static_cast<float>(screenHeight / 2) - 12 * GUI_SCALE - (SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) * (3 - index / 3)
			};

			index++;

		}

		inventorySlotsArray[RESULT_SLOT].position = { static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) + 18 * GUI_SCALE) - (screenWidth / -2 + texInventoryRect.width) + 88 * GUI_SCALE,
			static_cast<float>(screenHeight / 2) - 21 * GUI_SCALE - (SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) - 9 * GUI_SCALE };

		craft_grid = { inventory[37][0], inventory[38][0], inventory[39][0], inventory[40][0], inventory[41][0], inventory[42][0], inventory[43][0], inventory[44][0], inventory[45][0] };

		craft(craft_grid, inventory);

	}

	for (int i = 0; i < INVENTORY_SLOTS; i++) {

		miniBlocks[i].Render(texturesArray);

		inventorySlotsArray[i].ItemID = inventory[i][0];
		inventorySlotsArray[i].ItemCount = inventory[i][1];

		inventorySlotsArray[i].Draw();

	}

	miniBlocks[RESULT_SLOT].Render(texturesArray);
	inventorySlotsArray[RESULT_SLOT].ItemID = inventory[RESULT_SLOT][0];
	inventorySlotsArray[RESULT_SLOT].ItemCount = inventory[RESULT_SLOT][1];
	inventorySlotsArray[RESULT_SLOT].Draw();

}