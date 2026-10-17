#include "battle.h"

// Sets up a new battle with the hero and two enemies on random tiles
Battle start_battle () {
    Battle battle;

    spawn_unit(battle.units, SIDE_HERO, 400, 150, 200);     // Hero
    spawn_unit(battle.units, SIDE_ENEMY, 250, 150, 120);    // Enemy 1
    spawn_unit(battle.units, SIDE_ENEMY, 150, 150, 120);    // Enemy 2

    return battle;
}

// Reacts to one key press during the battle
void handle_key (Battle &battle, SDL_Keycode key) {
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
    // Choose the highlighted option
    else if (key == SDLK_RETURN) {
        if (battle.selected_option == MENU_FLEE) {
            battle.finished = true;
        }
    }
}

// Draws the board, the units and the HUD
void draw_battle (SDL_Renderer *renderer, const Battle &battle) {
    draw_board(renderer);

    for (const Unit &unit : battle.units) {
        draw_unit(renderer, unit);
    }

    draw_menu(renderer, battle.selected_option);
    draw_hero_stats(renderer, battle.units[0]);
}
