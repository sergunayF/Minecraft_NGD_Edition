#pragma once

#include "../Game/Game.hpp"

struct Recipe {

    std::vector<std::vector<double>> pattern;
    
    double result;
    double count;

};

extern std::vector<Recipe> recipes;

std::vector<Recipe> loadRecipes(const std::string& filename);

void craft(std::vector<double>& slots, double inventory[INVENTORY_SLOTS][INVENTORY_SLOTS_COUNTS]);