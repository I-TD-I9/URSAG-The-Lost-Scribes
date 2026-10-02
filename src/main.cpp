#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int arc, char* argv[]) {
    
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    bool done = false;

    SDL_Init(SDL_INIT_VIDEO);

    // Initializes SDL3 Video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL Initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    
    // Window creation
    if (!SDL_CreateWindowAndRenderer (
        "URSAG",                    //title
        640,                        //width
        480,                        //height
        0,                          //flag
        &window,
        &renderer
    )) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window/renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

     // Main Game Loop
    while (!done) {
        SDL_Event event;

        // Process all input events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }

        // ------------------ GAME RENDERING ------------------

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        // Future render logic

        SDL_RenderPresent(renderer);
    }

    // Clean up graphics context resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Shut down SDL subsystems
    SDL_Quit();
    return 0;
}