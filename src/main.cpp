#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <random>
#include <vector>
#include <string>

// Window size
const int window_width {1280};
const int window_height {720};

// Board layout
const int grid_columns {6};
const int grid_rows {6};
const float tile_size {64.0f};
const float center_gap {0.5f};
const int spawn_min_column {2};  // Units always spawn in columns c2 to c5


// Which grid a tile belongs to
enum Side {
    SIDE_HERO,
    SIDE_ENEMY
};

// A unit on the board
struct Unit {
    Side side {SIDE_HERO};      //Which grid it stands on
    int row {0};
    int column {0};

    int hp {0};
    int max_hp {0};
    int ap {0};
    int max_ap {0};
    int sp {0};
    int max_sp {0};
};

// Option for the battle menu
enum MenuOption {
    MENU_ATTACK,
    MENU_MOVE,
    MENU_SPELLS,
    MENU_ITEMS,
    MENU_FLEE,
    MENU_OPTION_COUNT // Counts how many options there are
};

// Text shown for each menu option
const std::string menu_labels[MENU_OPTION_COUNT] = {
    "Attack", "Move", "Spells", "Items", "Flee"
};


// Everything about the battle that's going on
struct Battle {
    std::vector<Unit> units;                // every unit
    int selected_option {MENU_ATTACK};      // highlighted battle menu option
    bool finished {false};                  // true once the battle is over
};

// Convert a tile on one side's grid into a position on the whole board
SDL_FPoint grid_to_world (Side side, int row, int column) {
    SDL_FPoint world;
    world.y = row;

    if (side == SIDE_HERO) {
        world.x = (grid_columns - 1) -  column;
    }
    else {
        world.x = grid_columns + center_gap + column;
    }

    return world;
}

// Converts a position on the board into a position on the screen, in pixels
SDL_FPoint world_to_screen (float world_x, float world_y) {
    const float board_width = (grid_columns * 2 + center_gap) * tile_size;
    const float board_left = (window_width - board_width) / 2.0f;
    const float board_top = 60.0f;

    SDL_FPoint screen;
    screen.x = board_left + world_x * tile_size;
    screen.y = board_top + world_y * tile_size;

    return screen;
}

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

// Draws one tile as a filled square
void draw_tile (SDL_Renderer *renderer, Side side, int row, int column) {
    SDL_FPoint world = grid_to_world (side, row, column);

    //Top-left and bottom-right corners of the tile
    SDL_FPoint top_left = world_to_screen(world.x, world.y);
    SDL_FPoint bottom_right = world_to_screen(world.x + 1.0f, world.y + 1.0f);

    SDL_FRect rect;
    rect.x = top_left.x;
    rect.y = top_left.y;
    rect.w = bottom_right.x - top_left.x;
    rect.h = bottom_right.y - top_left.y;

    // Checkerboard colors
    if ((row + column) % 2 == 0) {
        SDL_SetRenderDrawColor(renderer, 70, 70, 90, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 50, 50, 70, 255);
    }

    SDL_RenderFillRect(renderer, &rect);
}

// Draws both grids
void draw_board (SDL_Renderer *renderer) {
    for (int row = 0; row < grid_rows; row++) {
        for (int column = 0; column < grid_columns; column++) {
            draw_tile(renderer, SIDE_HERO, row, column);
            draw_tile(renderer, SIDE_ENEMY, row, column);
        }
    }
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

//Draws battle menu
void draw_menu (SDL_Renderer *renderer, int selected_option) {
    const float x = 240.0f;
    const float y = 476.0f;
    const float line_height = 30.0f;
    const float panel_width = 220.0f;
    const int option_count = MENU_OPTION_COUNT;
    const float panel_height = 26.0f + option_count * line_height; // So it expands if more option are added

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

int main(int argc, char* argv[]) {
    
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    bool done = false;

    // Initializes SDL3 Video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL Initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    
    // Window creation
    if (!SDL_CreateWindowAndRenderer (
        "URSAG",                    //title
        window_width,               //width
        window_height,              //height
        0,                          //flag
        &window,
        &renderer
    )) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window/renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Target frame rate configurations
    const Uint64 TARGET_FPS = 60;
    const Uint64 NS_PER_FRAME = 1000000000 / TARGET_FPS;

        // Start a new battle
    Battle battle = start_battle();


    // Main Game Loop
    while (!done) {
        SDL_Event event;

        // Record start time of the frame loop
        Uint64 frame_start_ns = SDL_GetTicksNS();

        // Process all input events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                handle_key(battle, event.key.key);
            }
        }

        // Stop when the battle is over
        if (battle.finished) {
            done = true;
        }

        // ------------------ GAME RENDERING ------------------

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        draw_battle(renderer, battle);

        SDL_RenderPresent(renderer);

        // Calculate how long the frame took
        Uint64 frame_duration_ns = SDL_GetTicksNS() - frame_start_ns;

        // If the frame finished early, delay the remaining time
        if (frame_duration_ns < NS_PER_FRAME) {
            Uint64 delay_ns = NS_PER_FRAME - frame_duration_ns;
            
            SDL_DelayNS(delay_ns);
        }
    }
    // Clean up graphics context resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Shut down SDL subsystems
    SDL_Quit();
    return 0;
}