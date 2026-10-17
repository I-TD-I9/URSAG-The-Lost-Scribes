#include "unit.h"

#include <random>

// Random whole number from min to max (both included)
int random_int (int min, int max) {
    static std::random_device device;
    static std::mt19937 gen(device());
    std::uniform_int_distribution<int> distribution(min, max);

    return distribution(gen);
}

// Checks if unit is occupying a tile
bool is_tile_taken (const std::vector<Unit> &units, Side side, int row, int column) {
    for (const Unit &unit : units) {
        if (unit.side == side && unit.row == row && unit.column == column) {
            return true;
        }
    }

    return false;
}

// Creates a unit on random free tile
void spawn_unit (std::vector<Unit> &units, Side side, int max_hp, int max_ap, int max_sp) {
    Unit unit;
    unit.side = side;

    // Goes through tiles until it finds a free one
    do {
        unit.row = random_int(0, grid_rows - 1);
        unit.column = random_int(spawn_min_column, grid_columns - 1);
    } while (is_tile_taken(units, side, unit.row, unit.column));

    unit.max_hp = max_hp;
    unit.hp = max_hp;
    unit.max_ap = max_ap;
    unit.ap = max_ap;
    unit.max_sp = max_sp;
    unit.sp = max_sp;

    units.push_back(unit);
}

// Draws a unit inside a tile
void draw_unit (SDL_Renderer *renderer, const Unit &unit) {
    SDL_FPoint world = grid_to_world(unit.side, unit.row, unit.column);

    // Leaves a gap inside the tile square
    const float inset = 0.2f;

    SDL_FPoint top_left = world_to_screen(world.x + inset, world.y + inset);
    SDL_FPoint bottom_right = world_to_screen(world.x + 1.0f - inset, world.y + 1.0f - inset);

    SDL_FRect rect;
    rect.x = top_left.x;
    rect.y = top_left.y;
    rect.w = bottom_right.x - top_left.x;
    rect.h = bottom_right.y - top_left.y;

    // Heroes are blue, and enemies are orange
    if (unit.side == SIDE_HERO) {
        SDL_SetRenderDrawColor(renderer, 80, 160, 255, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 240, 150, 60, 255);
    }

    SDL_RenderFillRect(renderer, &rect);
}
