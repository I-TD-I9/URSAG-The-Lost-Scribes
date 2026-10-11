#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

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

// Convert a tile on one side's grid into a postion on the whole board
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
                if (event.key.key == SDLK_ESCAPE) {
                    done = true;
                }
            }
        }

        // ------------------ GAME RENDERING ------------------

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        draw_board(renderer);

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