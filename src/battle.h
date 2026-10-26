#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "unit.h"
#include "hud.h"

// AP cost to move one tile
const int ap_per_tile {50};

// AP cost to attack
const int attack_ap_cost {25};

// Damage an attack deals
const int attack_damage {30};

// How long the enemy turn lasts
const float enemy_turn_seconds {1.0f};

// Order
enum BattleMode {
    MODE_MENU,          // choosing from the battle menu
    MODE_MOVE,          // planning a move
    MODE_ATTACK,        // aiming an attack
    MODE_ENEMY_TURN,    // enemy turn
    MODE_VICTORY,       // all enemies defeated
    MODE_DEFEAT         // the hero is defeated
};

// Everything about the battle
struct Battle {
    std::vector<Unit> units;                // every unit
    int selected_option {MENU_ATTACK};      // highlighted battle menu option
    bool finished {false};                  // true once the battle is over

    BattleMode mode {MODE_MENU};            // what the player is doing
    int ghost_row {0};                      // where a planned move ends
    int ghost_column {0};
    int target_row {0};                     // the tile attack is aimed at
    int target_column {0};

    
    int round {1};                          // goes up each time hero turn starts
    float enemy_turn_timer {0.0f};          // seconds left in the enemy turn
};

Battle start_battle ();
void handle_key (Battle &battle, SDL_Keycode key);
void update_battle (Battle &battle, float delta_seconds);
void draw_battle (SDL_Renderer *renderer, const Battle &battle);
