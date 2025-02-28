#include "Inventory.hpp"

Rectangle texInventoryRect = { 0, 0, 176, 166 };
Rectangle destInventoryRect = { 0, 0, texInventoryRect.width * GUI_SCALE, texInventoryRect.height * GUI_SCALE };

inventorySlots inventorySlotsArray[36];

void inventorySlotsInit() {

	for (int ID = 0; ID < 36; ID++) {

		inventorySlotsArray[ID] = inventorySlots(ID);

	}

}

void addItemToInventory(double inventory[SLOTS][SLOTS_COUNTS], double itemID) {

	int tempSlot = findItemInInventory(inventory, itemID);

	if (tempSlot != -1) {

		inventory[tempSlot][1] += 1;

	}
	else {

		for (int i = 0; i < 36; i++) {

			if (inventory[i][0] == 0.0) {
				
				inventory[i][0] = itemID;
				inventory[i][1] = 1;
				
				return;

			}
		}
	}
}

void removeItemFromInventory(double inventory[SLOTS][SLOTS_COUNTS], int slot) {

	if (inventory[slot][1] <= 1) {

		inventory[slot][0] = 0.0;
		inventory[slot][1] = 0.0;

	}
	else {

		inventory[slot][1] -= 1;

	}

}

int findItemInInventory(double inventory[SLOTS][SLOTS_COUNTS], double itemID) {


	for (int i = 0; i < 36; i++) {

		if (inventory[i][0] == itemID && inventory[i][1] < 64.0) return i;

	}

	return -1;

}



void DrawInventory(double inventory[SLOTS][SLOTS_COUNTS]) {

	DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 150});

	DrawTexturePro(inventoryGUI, texInventoryRect, destInventoryRect, {screenWidth / -2 + texInventoryRect.width, screenHeight / -2 + texInventoryRect.height}, 0.0f, WHITE);

	for (int i = 0; i < 36; i++) {

		miniBlocks[i].Render();

		inventorySlotsArray[i].ItemID = inventory[i][0];

		inventorySlotsArray[i].Draw();

	}

}