#include "hud.h"

// Text shown for each menu option
const std::string menu_labels[MENU_OPTION_COUNT] = {
    "Attack", "Move", "Spells", "Items", "Flee"
};

// Draws text at twice its normal size (SDL’s built-in font is only 8×8 pixels)
void draw_text (SDL_Renderer *renderer, float x, float y, const std::string &text) {
    const float scale = 2.0f;

    SDL_SetRenderScale(renderer, scale, scale);
    SDL_RenderDebugText(renderer, x / scale, y / scale, text.c_str());
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
}

// Draws a box with light border
void draw_panel (SDL_Renderer *renderer, float x, float y, float w, float h) {
    SDL_FRect panel {x, y, w, h};

    SDL_SetRenderDrawColor(renderer, 30, 30, 45, 255);
    SDL_RenderFillRect(renderer, &panel);

    SDL_SetRenderDrawColor(renderer, 110, 110, 150, 255);
    SDL_RenderRect(renderer, &panel);
}

// Draws battle menu
void draw_menu (SDL_Renderer *renderer, int selected_option) {
    const float x = 240.0f;
    const float y = 476.0f;
    const float line_height = 30.0f;
    const float panel_width = 220.0f;
    const int option_count = MENU_OPTION_COUNT;
    const float panel_height = 26.0f + option_count * line_height; // So it expands if more options are added

    draw_panel (renderer, x, y, panel_width, panel_height);

    for (int i = 0; i < MENU_OPTION_COUNT; i++) {
        float line_y = y + 16.0f + i * line_height;

        if (i == selected_option) {
            // Highlight bar behind selected option
            SDL_FRect highlight {x + 8.0f, line_y - 6.0f, panel_width - 16.0f, 28.0f};
            SDL_SetRenderDrawColor(renderer, 70, 80, 140, 255);
            SDL_RenderFillRect(renderer, &highlight);

            SDL_SetRenderDrawColor(renderer, 255, 220, 100, 255);
            draw_text(renderer, x + 16.0f, line_y, "> " + menu_labels[i]);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 200, 200, 210, 255);
            draw_text(renderer, x + 16.0f, line_y, "  " + menu_labels[i]);
        }
    }
}

// Draws hero stats on the battle menu
void draw_hero_stats (SDL_Renderer *renderer, const Unit &hero) {
    const float x = 480.0f;
    const float y = 476.0f;
    const float line_height = 30.0f;

    draw_panel(renderer, x, y, 300.0f, 136.0f);

    SDL_SetRenderDrawColor(renderer, 80, 160, 255, 255);
    draw_text(renderer, x + 16.0f, y + 16.0f, "HERO");

    SDL_SetRenderDrawColor(renderer, 200, 200, 210, 255);
    draw_text(renderer, x + 16.0f, y + 16.0f + line_height,
              "HP " + std::to_string(hero.hp) + " / " + std::to_string(hero.max_hp));
    draw_text(renderer, x + 16.0f, y + 16.0f + line_height * 2,
              "AP " + std::to_string(hero.ap) + " / " + std::to_string(hero.max_ap));
    draw_text(renderer, x + 16.0f, y + 16.0f + line_height * 3,
              "SP " + std::to_string(hero.sp) + " / " + std::to_string(hero.max_sp));
}
