#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "board.h"

const int spawn_min_column {2};  // Units always spawn in columns c2 to c5

// A unit on the board
struct Unit {
    Side side {SIDE_HERO};      // Which grid it stands on
    int row {0};
    int column {0};

    int hp {0};
    int max_hp {0};
    int ap {0};
    int max_ap {0};
    int sp {0};
    int max_sp {0};
};

int random_int (int min, int max);
bool is_tile_taken (const std::vector<Unit> &units, Side side, int row, int column);
void spawn_unit (std::vector<Unit> &units, Side side, int max_hp, int max_ap, int max_sp);
void draw_unit (SDL_Renderer *renderer, const Unit &unit);
void draw_ghost (SDL_Renderer *renderer, const Unit &unit);
