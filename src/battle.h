#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "unit.h"
#include "hud.h"

// Everything about the battle that's going on
struct Battle {
    std::vector<Unit> units;                // every unit
    int selected_option {MENU_ATTACK};      // highlighted battle menu option
    bool finished {false};                  // true once the battle is over
};

Battle start_battle ();
void handle_key (Battle &battle, SDL_Keycode key);
void draw_battle (SDL_Renderer *renderer, const Battle &battle);
