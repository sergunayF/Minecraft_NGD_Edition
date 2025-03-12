#include "Block.hpp"

#include "../Player/Player.hpp"

void FurnaceBlock::Update(Player& player) {

    //printf("ABOBA");

    static double lastUpdateTime = GetTime();

    if (blockDataMap[getTexture(holder[0][0])].meltOut && blockDataMap[getTexture(holder[1][0])].fuel && burnTime == 0) {

        burnTime = blockDataMap[getTexture(holder[1][0])].fuel;
        fuelTime = blockDataMap[getTexture(holder[1][0])].fuel;

        meltResult = blockDataMap[getTexture(holder[0][0])].meltOut;

        holder[1][1] -= 1;
        if (holder[1][1] == 0.0) holder[1][0] = 0.0;

    }

    double oldID = id;
    double curID = id;

    if (burnTime > 0) {
        curID = 62.0;
    }
    else {
        curID = 61.0;
    }

    if (curID != oldID) {

        id = curID;
        textured = false;

    }

    if (GetTime() - lastUpdateTime >= 1.0) Melting();

    if (GetTime() - lastUpdateTime >= 1.0) lastUpdateTime = GetTime();


}

void FurnaceBlock::Melting() {

    if (cookTime >= FURNACE_TIMER) {

        holder[2][0] = meltResult;
        holder[2][1] += 1;

        holder[0][1] -= 1;
        if (holder[0][1] == 1.0) holder[0][0] == 0.0;
        
        //meltResult = 0.0;
        cookTime = 0;
        //fuelTime = 0;

    }

    if (burnTime > 0) {

        burnTime--;
        cookTime++;
    }

}

void FurnaceBlock::LoadState(Player& player) {

    //printf("\nLoad Furnace");

    for (int i = 0; i < 2; i++) {
        player.inventory[37 + i][0] = holder[i][0];
        player.inventory[37 + i][1] = holder[i][1];
    }

    player.inventory[RESULT_SLOT][0] = holder[2][0];
    player.inventory[RESULT_SLOT][1] = holder[2][1];

}

void FurnaceBlock::SaveState(Player& player) {

    for (int i = 0; i < 2; i++) {
        holder[i][0] = player.inventory[37 + i][0];
        holder[i][1] = player.inventory[37 + i][1];
        player.inventory[37 + i][0] = 0.0f;
        player.inventory[37 + i][1] = 0.0f;
    }

    holder[2][0] = player.inventory[RESULT_SLOT][0];
    holder[2][1] = player.inventory[RESULT_SLOT][1];

}