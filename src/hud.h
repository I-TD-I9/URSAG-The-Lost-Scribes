#pragma once

#include <SDL3/SDL.h>
#include <string>
#include "unit.h"

// Option for the battle menu
enum MenuOption {
    MENU_ATTACK,
    MENU_MOVE,
    MENU_SPELLS,
    MENU_ITEMS,
    MENU_FLEE,
    MENU_OPTION_COUNT // Counts how many options there are
};

void draw_text (SDL_Renderer *renderer, float x, float y, const std::string &text);
void draw_panel (SDL_Renderer *renderer, float x, float y, float w, float h);
void draw_menu (SDL_Renderer *renderer, int selected_option);
void draw_hero_stats (SDL_Renderer *renderer, const Unit &hero);
