#pragma once

#include <SDL3/SDL.h>

// How many recent frames overlay keeps for its averages and graph
const int perf_history_frames {96};

// Time budget for one frame at 60 FPS in milliseconds
const float frame_budget_ms {1000.0f / 60.0f};

// How long the parts of one frame took in milliseconds
struct FrameTiming {
    float update_ms {0.0f};     // update_battle
    float render_ms {0.0f};     // drawing and presenting
    float frame_ms {0.0f};      // the whole frame, including the wait for the frame cap
};

// Timings for most recent frames kept in a ring buffer
struct PerfStats {
    FrameTiming history[perf_history_frames];
    int next_index {0};         // where the next frame gets written
    int frame_count {0};        // frames recorded so far up to perf_history_frames
    bool visible {false};       // toggled with F3
};

float ns_to_ms (Uint64 ns);
void record_frame (PerfStats &stats, const FrameTiming &timing);
void draw_perf_overlay (SDL_Renderer *renderer, const PerfStats &stats);
