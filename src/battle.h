#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "unit.h"
#include "hud.h"

// AP it costs to move one tile
const int ap_per_tile {50};

// What the player is doing right now
enum BattleMode {
    MODE_MENU,      // choosing from the battle menu
    MODE_MOVE       // planning a move
};

// Everything about the battle that's going on
struct Battle {
    std::vector<Unit> units;                // every unit
    int selected_option {MENU_ATTACK};      // highlighted battle menu option
    bool finished {false};                  // true once the battle is over

    BattleMode mode {MODE_MENU};            // what the player is doing
    int ghost_row {0};                      // where a planned move ends
    int ghost_column {0};
};

Battle start_battle ();
void handle_key (Battle &battle, SDL_Keycode key);
void draw_battle (SDL_Renderer *renderer, const Battle &battle);
