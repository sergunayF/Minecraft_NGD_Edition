#include "Inventory.hpp"


void addItemToInventory(double inventory[SLOTS][SLOTS_COUNTS], double itemID) {

	int tempSlot = findItemInInventory(inventory, itemID);

	if (tempSlot != -1) {

		inventory[tempSlot][1] += 1;

	}
	else {

		for (int i = 0; i < 9; i++) {

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


	for (int i = 0; i < 9; i++) {

		if (inventory[i][0] == itemID && inventory[i][1] < 64.0) return i;

	}

	return -1;

}