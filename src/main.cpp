#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "board.h"
#include "battle.h"
#include "perf.h"

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

    // Frame timing for performance overlay (F3)
    PerfStats perf;
    
    // Work out how much time has passed
    Uint64 previous_frame_ns = SDL_GetTicksNS();

    // Main Game Loop
    while (!done) {
        SDL_Event event;

        // Record start time of the frame loop
        Uint64 frame_start_ns = SDL_GetTicksNS();

        // Seconds since the last frame 
        float delta_seconds = (frame_start_ns - previous_frame_ns) / 1000000000.0f;
        previous_frame_ns = frame_start_ns;

        // Process all input events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_F3) {
                    perf.visible = !perf.visible;
                }
                else {
                    handle_key(battle, event.key.key);
                }
            }
        }

        // Stop when the battle is over
        if (battle.finished) {
            done = true;
        }

        
        // ------------------ GAME UPDATE ------------------

        Uint64 update_start_ns = SDL_GetTicksNS();

        update_battle(battle, delta_seconds);

        Uint64 update_end_ns = SDL_GetTicksNS();

        // ------------------ GAME RENDERING ------------------

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        draw_battle(renderer, battle);

        if (perf.visible) {
            draw_perf_overlay(renderer, perf);
        }

        SDL_RenderPresent(renderer);

        Uint64 render_end_ns = SDL_GetTicksNS();

        // Calculate how long the frame took
        Uint64 frame_duration_ns = SDL_GetTicksNS() - frame_start_ns;

        // If the frame finished early, delay the remaining time
        if (frame_duration_ns < NS_PER_FRAME) {
            Uint64 delay_ns = NS_PER_FRAME - frame_duration_ns;

            SDL_DelayNS(delay_ns);
        }

        // Record how long each part of this frame took
        FrameTiming timing;
        timing.update_ms = ns_to_ms(update_end_ns - update_start_ns);
        timing.render_ms = ns_to_ms(render_end_ns - update_end_ns);
        timing.frame_ms = ns_to_ms(SDL_GetTicksNS() - frame_start_ns);
        record_frame(perf, timing);
    }
    
    // Clean up graphics context resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Shut down SDL subsystems
    SDL_Quit();
    return 0;
}