#pragma once

#include "../Game/Game.hpp"
#include "Player.hpp"

void addItemToInventory(double inventory[SLOTS][SLOTS_COUNTS], double itemID);

void removeItemFromInventory(double inventory[SLOTS][SLOTS_COUNTS], int slot);

int findItemInInventory(double inventory[SLOTS][SLOTS_COUNTS], double itemID);