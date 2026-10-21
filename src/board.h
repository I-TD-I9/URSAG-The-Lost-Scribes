#pragma once

#include <SDL3/SDL.h>

// Window size
const int window_width {1280};
const int window_height {720};

// Board layout
const int grid_columns {6};
const int grid_rows {6};
const float tile_size {64.0f};
const float center_gap {0.5f};

// Which grid a tile belongs to
enum Side {
    SIDE_HERO,
    SIDE_ENEMY
};

SDL_FPoint grid_to_world (Side side, int row, int column);
SDL_FPoint world_to_screen (float world_x, float world_y);
void draw_tile (SDL_Renderer *renderer, Side side, int row, int column);
void draw_board (SDL_Renderer *renderer);
void draw_tile_tint (SDL_Renderer *renderer, Side side, int row, int column, SDL_Color color);
void draw_tile_outline (SDL_Renderer *renderer, Side side, int row, int column, SDL_Color color);
