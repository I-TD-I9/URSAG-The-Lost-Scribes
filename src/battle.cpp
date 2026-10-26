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
    if (!is_on_grid(row, column)) {
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


// Ends hero turn and hands it to the enemies
void end_hero_turn (Battle &battle) {
    battle.mode = MODE_ENEMY_TURN;
    battle.enemy_turn_timer = enemy_turn_seconds;
}


// Starts hero next turn: a new round, full AP, and the menu back on attack
void start_hero_turn (Battle &battle) {
    Unit &hero = battle.units[0];
    hero.ap = hero.max_ap;

    battle.round = battle.round + 1;
    battle.selected_option = MENU_ATTACK;
    battle.mode = MODE_MENU;
}


// Reacts to key press while battle menu is open
void handle_menu_key (Battle &battle, SDL_Keycode key) {
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
            if (battle.selected_option == MENU_ATTACK) {
            const Unit &hero = battle.units[0];

                if (hero.ap >= attack_ap_cost) {
                battle.target_row = hero.row;
                battle.target_column = 0;
                battle.mode = MODE_ATTACK;
            }
        }
        else if (battle.selected_option == MENU_MOVE) {
            battle.ghost_row = battle.units[0].row;
            battle.ghost_column = battle.units[0].column;
            battle.mode = MODE_MOVE;
        }
        else if (battle.selected_option == MENU_PASS) {
            
            // End turn without doing anything
            end_hero_turn(battle);
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

        // Running out of AP ends the turn
        if (hero.ap <= 0) {
            end_hero_turn(battle);
        }
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


// Reacts to key press while the player is aiming an attack
void handle_attack_key (Battle &battle, SDL_Keycode key) {
    int row = battle.target_row;
    int column = battle.target_column;

    // The enemy grid counts columns from center line
    if (key == SDLK_UP) {
        row = row - 1;
    }
    else if (key == SDLK_DOWN) {
        row = row + 1;
    }
    else if (key == SDLK_LEFT) {
        column = column - 1;
    }
    else if (key == SDLK_RIGHT) {
        column = column + 1;
    }
    else if (key == SDLK_RETURN) {
        Unit &hero = battle.units[0];
        hero.ap = hero.ap - attack_ap_cost;
        end_hero_turn(battle);
        return;
    }
    else if (key == SDLK_ESCAPE) {
        battle.mode = MODE_MENU;
        return;
    }

    if (is_on_grid(row, column)) {
        battle.target_row = row;
        battle.target_column = column;
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
    else if (battle.mode == MODE_ATTACK) {
        handle_attack_key(battle, key);
    }
}


// Moves battle forward in time once every frame
void update_battle (Battle &battle, float delta_seconds) {
    if (battle.mode == MODE_ENEMY_TURN) {
        battle.enemy_turn_timer = battle.enemy_turn_timer - delta_seconds;

        if (battle.enemy_turn_timer <= 0.0f) {
            start_hero_turn(battle);
        }
    }
}


// Draws the board, the units and the HUD
void draw_battle (SDL_Renderer *renderer, const Battle &battle) {
    draw_board(renderer);

    for (const Unit &unit : battle.units) {
        draw_unit(renderer, unit);
    }

    int planned_cost = 0;

    if (battle.mode == MODE_MENU) {
        draw_hint(renderer, "Up/Down: choose   Enter: select");
    }
    else if (battle.mode == MODE_MOVE) {
        Unit ghost = battle.units[0];       // Shows where hero would end up
        ghost.row = battle.ghost_row;
        ghost.column = battle.ghost_column;
        draw_ghost(renderer, ghost);

        planned_cost = move_cost(battle);
        draw_hint(renderer, "Arrows: plan move   Enter: confirm   Esc: cancel");
    }
    else if (battle.mode == MODE_ATTACK) {
        draw_tile_outline(renderer, SIDE_ENEMY, battle.target_row, battle.target_column, SDL_Color {255, 220, 100, 255});

        planned_cost = attack_ap_cost;
        draw_hint(renderer, "Arrows: aim   Enter: attack   Esc: cancel");
    }

    int highlighted_option = battle.selected_option;

    if (battle.mode == MODE_ENEMY_TURN) {
        highlighted_option = -1;
    }

    draw_round(renderer, battle.round);
    draw_menu(renderer, highlighted_option);
    draw_hero_stats(renderer, battle.units[0], planned_cost);

    if (battle.mode == MODE_ENEMY_TURN) {
        draw_banner(renderer, "ENEMY TURN");
    }
}
