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

void DrawInventory(Player& player) {

	DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 150});

	std::vector<double> craft_grid;

	int index = 0;	

	if (!player.isCrafting && !player.isFurnace) {

		DrawTexturePro(inventoryGUI, texInventoryRect, destInventoryRect, { screenWidth / -2 + texInventoryRect.width, screenHeight / -2 + texInventoryRect.height }, 0.0f, WHITE);

		for (int i = 37; i <= 41; i++) {

			if (i == 39) continue;

			miniBlocks[i].Render(texturesArray);
			inventorySlotsArray[i].ItemID = player.inventory[i][0];
			inventorySlotsArray[i].ItemCount = player.inventory[i][1];
			inventorySlotsArray[i].Draw();

			inventorySlotsArray[i].position = {
			static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) * (index % 2)) - (screenWidth / -2 + texInventoryRect.width) + 88 * GUI_SCALE,
			static_cast<float>(screenHeight / 2) - 21 * GUI_SCALE - (SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) * (2 - index / 2)
			};

			index++;

		}

		inventorySlotsArray[RESULT_SLOT].position = { static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) + 38 * GUI_SCALE) - (screenWidth / -2 + texInventoryRect.width) + 88 * GUI_SCALE,
			static_cast<float>(screenHeight / 2) - 21 * GUI_SCALE - (SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) - 8 * GUI_SCALE };

		craft_grid = { player.inventory[37][0], player.inventory[38][0], 0.0, player.inventory[40][0], player.inventory[41][0], 0.0, 0.0, 0.0, 0.0 };

		craft(craft_grid, player.inventory);

	}
	else if (player.isCrafting) {

		DrawTexturePro(craftingGUI, texInventoryRect, destInventoryRect, { screenWidth / -2 + texInventoryRect.width, screenHeight / -2 + texInventoryRect.height }, 0.0f, WHITE);

		for (int i = 37; i < 46; i++) {

			miniBlocks[i].Render(texturesArray);
			inventorySlotsArray[i].ItemID = player.inventory[i][0];
			inventorySlotsArray[i].ItemCount = player.inventory[i][1];
			inventorySlotsArray[i].Draw();

			inventorySlotsArray[i].position = {
			static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) * (index % 3)) - (screenWidth / -2 + texInventoryRect.width) + 30 * GUI_SCALE,
			static_cast<float>(screenHeight / 2) - 12 * GUI_SCALE - (SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) * (3 - index / 3)
			};

			index++;

		}

		inventorySlotsArray[RESULT_SLOT].position = { static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) + 18 * GUI_SCALE) - (screenWidth / -2 + texInventoryRect.width) + 88 * GUI_SCALE,
			static_cast<float>(screenHeight / 2) - 21 * GUI_SCALE - (SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) - 9 * GUI_SCALE };

		craft_grid = { player.inventory[37][0], player.inventory[38][0], player.inventory[39][0], player.inventory[40][0], player.inventory[41][0], player.inventory[42][0], player.inventory[43][0], player.inventory[44][0], player.inventory[45][0] };

		craft(craft_grid, player.inventory);

	}
	else if (player.isFurnace) {

		DrawTexturePro(furnaceGUI, texInventoryRect, destInventoryRect, { screenWidth / -2 + texInventoryRect.width, screenHeight / -2 + texInventoryRect.height }, 0.0f, WHITE);

		float burnPixels = 0.0f;
		float coockedPixels = (24.0f * static_cast<float>(player.activeBlock->cookTime) / FURNACE_TIMER);

		if (player.activeBlock->fuelTime != 0) burnPixels = (14.0f * static_cast<float>(player.activeBlock->burnTime) / player.activeBlock->fuelTime);

		DrawTexturePro(furnaceGUI, { 176, 14 - burnPixels, 14, burnPixels }, { 0, 0, 14 * GUI_SCALE, burnPixels * GUI_SCALE }, { screenWidth / -2 + texInventoryRect.width - 56 * GUI_SCALE, burnPixels * GUI_SCALE + screenHeight / -2 + texInventoryRect.height - 50 * GUI_SCALE}, 0.0f, WHITE);

		DrawTexturePro(furnaceGUI, { 176, 14, coockedPixels, 17 }, { 0, 0, coockedPixels * GUI_SCALE, 17 * GUI_SCALE }, { screenWidth / -2 + texInventoryRect.width - 80 * GUI_SCALE, screenHeight / -2 + texInventoryRect.height - 34 * GUI_SCALE}, 0.0f, WHITE);
		
		std::string debugBurn = std::to_string(burnPixels);

		DrawText(debugBurn.c_str(), 0, 0, 20, WHITE);
		
		inventorySlotsArray[37].position = { static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) - 28 * GUI_SCALE) - (screenWidth / -2 + texInventoryRect.width) + 66 * GUI_SCALE,
			static_cast<float>(screenHeight / 2) - 40 * GUI_SCALE - (SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) - 8 * GUI_SCALE };

		inventorySlotsArray[38].position = { static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) - 28 * GUI_SCALE) - (screenWidth / -2 + texInventoryRect.width) + 66 * GUI_SCALE,
			static_cast<float>(screenHeight / 2) - 4 * GUI_SCALE - (SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) - 8 * GUI_SCALE };

		for (int i = 37; i < 39; i++) {

			miniBlocks[i].Render(texturesArray);
			inventorySlotsArray[i].ItemID = player.inventory[i][0];
			inventorySlotsArray[i].ItemCount = player.inventory[i][1];
			inventorySlotsArray[i].Draw();

		}


		inventorySlotsArray[RESULT_SLOT].position = { static_cast<float>((SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) + 18 * GUI_SCALE) - (screenWidth / -2 + texInventoryRect.width) + 80 * GUI_SCALE,
			static_cast<float>(screenHeight / 2) - 21 * GUI_SCALE - (SLOT_SIZE * GUI_SCALE + 2 * GUI_SCALE) - 9 * GUI_SCALE };

	}

	for (int i = 0; i < INVENTORY_SLOTS; i++) {

		miniBlocks[i].Render(texturesArray);

		inventorySlotsArray[i].ItemID = player.inventory[i][0];
		inventorySlotsArray[i].ItemCount = player.inventory[i][1];

		inventorySlotsArray[i].Draw();

	}

	miniBlocks[RESULT_SLOT].Render(texturesArray);
	inventorySlotsArray[RESULT_SLOT].ItemID = player.inventory[RESULT_SLOT][0];
	inventorySlotsArray[RESULT_SLOT].ItemCount = player.inventory[RESULT_SLOT][1];
	inventorySlotsArray[RESULT_SLOT].Draw();

}