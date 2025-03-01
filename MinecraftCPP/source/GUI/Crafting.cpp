#include "Crafting.hpp"

std::vector<Recipe> recipes;

std::vector<Recipe> loadRecipes(const std::string& filename) {
    std::vector<Recipe> recipes;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл " << filename << std::endl;
        return recipes;
    }

    json data;
    file >> data;

    for (const auto& recipe : data) {
        Recipe r;
        r.pattern = recipe["pattern"].get<std::vector<std::vector<double>>>();
        r.result = recipe["result"].get<double>();
        r.count = recipe["count"].get<double>();
        recipes.push_back(r);
    }

    return recipes;
}

bool compareGrids(const std::vector<std::vector<double>>& grid, const std::vector<std::vector<double>>& pattern, int offsetX, int offsetY) {

    if (offsetX + pattern.size() > grid.size() || offsetY + pattern[0].size() > grid[0].size()) {
        return false;
    }

    for (size_t i = 0; i < grid.size(); ++i) {
        for (size_t j = 0; j < grid[i].size(); ++j) {

            if (i >= offsetX && i < offsetX + pattern.size() &&
                j >= offsetY && j < offsetY + pattern[0].size()) {

                if (grid[i][j] != pattern[i - offsetX][j - offsetY]) {
                    return false;
                }
            }
            else {

                if (grid[i][j] != 0.0) {
                    return false;
                }
            }
        }
    }
    return true;
}

void craft(std::vector<double>& slots, double inventory[INVENTORY_SLOTS][INVENTORY_SLOTS_COUNTS]) {
    std::vector<std::vector<double>> grid = {
        {slots[0], slots[1], slots[2]},
        {slots[3], slots[4], slots[5]},
        {slots[6], slots[7], slots[8]}
    };

    for (const auto& recipe : recipes) {
        for (int offsetX = 0; offsetX <= grid.size() - recipe.pattern.size(); ++offsetX) {
            for (int offsetY = 0; offsetY <= grid[0].size() - recipe.pattern[0].size(); ++offsetY) {
                if (compareGrids(grid, recipe.pattern, offsetX, offsetY)) {
                    inventory[RESULT_SLOT][0] = recipe.result;
                    inventory[RESULT_SLOT][1] = recipe.count;
                    return;
                }
            }
        }
    }

    inventory[RESULT_SLOT][0] = 0.0;
    inventory[RESULT_SLOT][1] = 0.0;
}