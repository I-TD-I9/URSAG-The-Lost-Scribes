#include "hud.h"

// Text shown for each menu option
const std::string menu_labels[MENU_OPTION_COUNT] = {
    "Attack", "Move", "Spells", "Items", "Pass", "Flee"
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
void draw_hero_stats (SDL_Renderer *renderer, const Unit &hero, int ap_cost, int sp_cost) {
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
              
    //  Show how much AP will cost while planning a move
    if (ap_cost > 0) {
        SDL_SetRenderDrawColor(renderer, 255, 150, 80, 255);
        draw_text(renderer, x + 224.0f, y + 16.0f + line_height * 2,
            "-" + std::to_string(ap_cost));
        SDL_SetRenderDrawColor(renderer, 200, 200, 210, 255);
    }

    draw_text(renderer, x + 16.0f, y + 16.0f + line_height * 3,
        "SP " + std::to_string(hero.sp) + " / " + std::to_string(hero.max_sp));

    // While aiming a spell show how much SP it costs
    if (sp_cost > 0) {
        SDL_SetRenderDrawColor(renderer, 120, 170, 255, 255);
        draw_text(renderer, x + 224.0f, y + 16.0f + line_height * 3,
            "-" + std::to_string(sp_cost));
    }
}


// Draws line of help text under the HUD
void draw_hint (SDL_Renderer *renderer, const std::string &text) {
    SDL_SetRenderDrawColor(renderer, 150, 150, 170, 255);
    draw_text(renderer, 240.0f, 694.0f, text);
}


// Draws the round number above the board
void draw_round (SDL_Renderer *renderer, int round) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 210, 255);
    draw_text(renderer, 240.0f, 24.0f, "ROUND " + std::to_string(round));
}


// Draws message in the middle of the board
void draw_banner (SDL_Renderer *renderer, const std::string &text) {
    const float text_width = text.size() * 16.0f; 
    const float panel_width = text_width + 64.0f;
    const float panel_height = 56.0f;
    const float x = (window_width - panel_width) / 2.0f;
    const float y = 2.0f;         

    draw_panel(renderer, x, y, panel_width, panel_height);

    SDL_SetRenderDrawColor(renderer, 255, 220, 100, 255);
    draw_text(renderer, x + 32.0f, y + 20.0f, text);
}


// Draws Spells list with each spell's SP cost and speed
void draw_spell_menu (SDL_Renderer *renderer, const std::vector<Spell> &spells, int selected_spell, int sp) {
    const float x = 800.0f;
    const float y = 476.0f;
    const float line_height = 30.0f;
    const float panel_width = 460.0f;
    const int spell_count = spells.size();
    const float panel_height = 26.0f + spell_count * line_height;

    draw_panel(renderer, x, y, panel_width, panel_height);

    for (int i = 0; i < spell_count; i++) {
        const Spell &spell = spells[i];
        float line_y = y + 16.0f + i * line_height;

        // Highlight bar behind selected spell
        if (i == selected_spell) {
            SDL_FRect highlight {x + 8.0f, line_y - 6.0f, panel_width - 16.0f, 28.0f};
            SDL_SetRenderDrawColor(renderer, 70, 80, 140, 255);
            SDL_RenderFillRect(renderer, &highlight);
        }

        // Gray if the hero can't afford it, yellow if selected, white otherwise
        if (spell.sp_cost > sp) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 115, 255);
        }
        else if (i == selected_spell) {
            SDL_SetRenderDrawColor(renderer, 255, 220, 100, 255);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 200, 200, 210, 255);
        }

        std::string marker = "  ";
        if (i == selected_spell) {
            marker = "> ";
        }

        std::string speed_text = "Instant";
        if (spell.hidden) {
            speed_text = "Hidden";
        }
        else if (spell.speed == SPEED_WIND_UP) {
            speed_text = "Wind-up";
        }

        // Three columns (name, cost, speed)
        draw_text(renderer, x + 16.0f, line_y, marker + spell.name);
        draw_text(renderer, x + 216.0f, line_y, std::to_string(spell.sp_cost) + " SP");
        draw_text(renderer, x + 332.0f, line_y, speed_text);
    }
}