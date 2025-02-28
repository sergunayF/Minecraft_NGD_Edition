#include "Player.hpp"

Cursor cursor;

void Cursor::Withdraw(double inventory[SLOTS][SLOTS_COUNTS], int slotID, bool LeftMouseButton) {

    if (ItemID == 0.0) {
        if (inventory[slotID][0] != 0.0) {
            ItemID = inventory[slotID][0];

            if (LeftMouseButton) {
                count = inventory[slotID][1];
                inventory[slotID][0] = 0.0;
                inventory[slotID][1] = 0;
            }
            else {
                count = ceil(inventory[slotID][1] / 2.0);
                inventory[slotID][1] = floor(inventory[slotID][1] / 2.0);

                if (inventory[slotID][1] == 0) {
                    inventory[slotID][0] = 0.0;
                }
            }
        }
    }
    else {
        if (inventory[slotID][0] == 0.0) {
            if (LeftMouseButton) {
                inventory[slotID][0] = ItemID;
                inventory[slotID][1] = count;
                ItemID = 0.0;
                count = 0;
            }
            else {
                inventory[slotID][0] = ItemID;
                inventory[slotID][1] = 1;
                count--;

                if (count == 0) {
                    ItemID = 0.0;
                }
            }
        }
        else if (inventory[slotID][0] == ItemID) {
            int freeSpace = 64 - inventory[slotID][1];

            if (LeftMouseButton) {
                int adding = std::min(count, freeSpace);
                inventory[slotID][1] += adding;
                count -= adding;

                if (count == 0) {
                    ItemID = 0.0;
                }
            }
            else {
                if (inventory[slotID][1] < 64) {
                    inventory[slotID][1]++;
                    count--;

                    if (count == 0) {
                        ItemID = 0.0;
                    }
                }
            }
        }
        else {
            if (LeftMouseButton || (ItemID != inventory[slotID][0])) {
                
                int tmpCount = static_cast<int>(inventory[slotID][1]);

                std::swap(ItemID, inventory[slotID][0]);
                
                inventory[slotID][1] = static_cast<double>(count);
                count = tmpCount;

            }
        }
    }
}


void Cursor::Update(Player& player) {

	Vector2 mousePosition = GetMousePosition();

	for (int i = 0; i < INVENTORY_SLOTS_COUNT; i++) {

		if (CheckCollisionPointRec(mousePosition, {inventorySlotsArray[i].position.x, inventorySlotsArray[i].position.y, SLOT_SIZE * GUI_SCALE, SLOT_SIZE * GUI_SCALE })) {

			inventorySlotsArray[i].Glow = { 255, 255, 255, 155 };

			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
				Withdraw(player.inventory, inventorySlotsArray[i].slotID, (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ? true : false));
			}

		}
		else {

			inventorySlotsArray[i].Glow = { 255, 255, 255, 0 };

		}

	}

	miniBlocks[36].BlockID = ItemID;
	miniBlocks[36].blockTexture = setTexture(getTexture(ItemID));
	miniBlocks[36].rendered = false;
	miniBlocks[36].Render();
	miniBlocks[36].Draw({ mousePosition.x - miniBlocks[36].TEXTURE_SIZE / 2, mousePosition.y - miniBlocks[36].TEXTURE_SIZE / 2 });

}