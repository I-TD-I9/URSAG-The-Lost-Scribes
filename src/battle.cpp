#include "battle.h"
#include <cstdlib>

// Sets up a new battle with the hero and two enemies on random tiles
Battle start_battle () {
    Battle battle;

    spawn_unit(battle.units, SIDE_HERO, 400, 150, 200);     // Hero
    spawn_unit(battle.units, SIDE_ENEMY, 250, 150, 120);    // Enemy 1
    spawn_unit(battle.units, SIDE_ENEMY, 150, 150, 120);    // Enemy 2

    return battle;
}

// Number of steps between two tiles on the same grid
int grid_distance (int row_a, int column_a, int row_b, int column_b) {
    return std::abs(row_a - row_b) + std::abs(column_a - column_b);
}

// AP the planned move will cost, counted from where the hero stands now
int move_cost (const Battle &battle) {
    const Unit &hero = battle.units[0];

    return grid_distance(hero.row, hero.column, battle.ghost_row, battle.ghost_column) * ap_per_tile;
}

// Checks whether hero's ghost may move onto tile
bool can_ghost_move_to (const Battle &battle, int row, int column) {
    const Unit &hero = battle.units[0];

    // Must stay on hero's own grid
    if (row < 0 || row >= grid_rows || column < 0 || column >= grid_columns) {
        return false;
    }

    // Can't end on another unit
    bool is_hero_tile = (row == hero.row && column == hero.column);
    if (!is_hero_tile && is_tile_taken(battle.units, SIDE_HERO, row, column)) {
        return false;
    }

    // Must be able to afford it
    int cost = grid_distance(hero.row, hero.column, row, column) * ap_per_tile;
    return cost <= hero.ap;
}

// Reacts to key press while battle menu is open
void handle_menu_key (Battle &battle, SDL_Keycode key) {

    // Move the selection up, wrapping around to the bottom
    if (key == SDLK_UP) {
        battle.selected_option = battle.selected_option - 1;
        if (battle.selected_option < 0) {
            battle.selected_option = MENU_OPTION_COUNT - 1;
        }
    }

    // Move the selection down, wrapping around to the top
    else if (key == SDLK_DOWN) {
        battle.selected_option = battle.selected_option + 1;
        if (battle.selected_option >= MENU_OPTION_COUNT) {
            battle.selected_option = 0;
        }
    }

    // Choose highlighted option
    else if (key == SDLK_RETURN) {
        if (battle.selected_option == MENU_MOVE) {

            // Start planning a move from where the hero is standing
            battle.ghost_row = battle.units[0].row;
            battle.ghost_column = battle.units[0].column;
            battle.mode = MODE_MOVE;
        }
        else if (battle.selected_option == MENU_FLEE) {
            battle.finished = true;
        }
    }
}


// Reacts to a key press while the player is planning a move
void handle_move_key (Battle &battle, SDL_Keycode key) {
    int row = battle.ghost_row;
    int column = battle.ghost_column;

    // Hero grid counts columns from the center line, which is on its right,
    // so Right means a lower column number and Left a higher one
    if (key == SDLK_UP) {
        row = row - 1;
    }
    else if (key == SDLK_DOWN) {
        row = row + 1;
    }
    else if (key == SDLK_LEFT) {
        column = column + 1;
    }
    else if (key == SDLK_RIGHT) {
        column = column - 1;
    }
    else if (key == SDLK_RETURN) {

        // Lock in the move. Pay first: the cost is measured from the hero's current tile
        Unit &hero = battle.units[0];
        hero.ap = hero.ap - move_cost(battle);
        hero.row = battle.ghost_row;
        hero.column = battle.ghost_column;

        battle.mode = MODE_MENU;
        return;
    }
    else if (key == SDLK_ESCAPE) {

        // Cancel (go back to menu)
        battle.mode = MODE_MENU;
        return;
    }

    if (can_ghost_move_to(battle, row, column)) {
        battle.ghost_row = row;
        battle.ghost_column = column;
    }
}

// Reacts to one key press during battle
void handle_key (Battle &battle, SDL_Keycode key) {
    if (battle.mode == MODE_MENU) {
        handle_menu_key(battle, key);
    }
    else if (battle.mode == MODE_MOVE) {
        handle_move_key(battle, key);
    }
}

// Draws the board, the units and the HUD
void draw_battle (SDL_Renderer *renderer, const Battle &battle) {
    draw_board(renderer);

    for (const Unit &unit : battle.units) {
        draw_unit(renderer, unit);
    }

        int planned_cost = 0;

    if (battle.mode == MODE_MOVE) {
        // Show where the hero would end up
        Unit ghost = battle.units[0];
        ghost.row = battle.ghost_row;
        ghost.column = battle.ghost_column;
        draw_ghost(renderer, ghost);

        planned_cost = move_cost(battle);
        draw_hint(renderer, "Arrows: plan move   Enter: confirm   Esc: cancel");
    }

    draw_menu(renderer, battle.selected_option);
    draw_hero_stats(renderer, battle.units[0], planned_cost);
}
