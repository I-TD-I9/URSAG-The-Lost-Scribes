#include "board.h"

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
