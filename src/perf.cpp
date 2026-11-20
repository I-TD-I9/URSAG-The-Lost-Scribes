#include "perf.h"
#include "board.h"
#include "hud.h"
#include <cstdio>

// Overlay layout
const float perf_panel_x {1052.0f};
const float perf_panel_y {16.0f};
const float perf_panel_width {216.0f};
const float perf_line_height {14.0f};
const float perf_graph_height {64.0f};
const float perf_bar_width {2.0f};

// Top of the graph is two frame budgets
const float perf_graph_max_ms {frame_budget_ms * 2.0f};

// Converts nanoseconds into milliseconds
float ns_to_ms (Uint64 ns) {
    return ns / 1000000.0f;
}

// Saves one frame's timing, overwriting the oldest once history is full
void record_frame (PerfStats &stats, const FrameTiming &timing) {
    stats.history[stats.next_index] = timing;
    stats.next_index = (stats.next_index + 1) % perf_history_frames;

    if (stats.frame_count < perf_history_frames) {
        stats.frame_count = stats.frame_count + 1;
    }
}

// Gets the i-th oldest frame still in the history
const FrameTiming &frame_at (const PerfStats &stats, int i) {
    int oldest = (stats.next_index - stats.frame_count + perf_history_frames) % perf_history_frames;
    return stats.history[(oldest + i) % perf_history_frames];
}

// Draws one line overlay text
void draw_perf_line (SDL_Renderer *renderer, int line, const char *label, float value, const char *unit) {
    char text[32];
    std::snprintf(text, sizeof(text), "%-7s %6.2f %s", label, value, unit);

    float y = perf_panel_y + 12.0f + line * perf_line_height;
    SDL_RenderDebugText(renderer, perf_panel_x + 12.0f, y, text);
}

// Turns a time into a bar height on graph
float graph_height_for (float ms) {
    float height = (ms / perf_graph_max_ms) * perf_graph_height;

    if (height > perf_graph_height) {
        height = perf_graph_height;
    }

    return height;
}

// Draws frame timing stats and a graph of recent frames
void draw_perf_overlay (SDL_Renderer *renderer, const PerfStats &stats) {
    const float graph_x = perf_panel_x + 12.0f;
    const float graph_y = perf_panel_y + 96.0f;
    const float graph_width = perf_history_frames * perf_bar_width;
    const float panel_height = (graph_y - perf_panel_y) + perf_graph_height + 28.0f;

    draw_panel(renderer, perf_panel_x, perf_panel_y, perf_panel_width, panel_height);

    if (stats.frame_count == 0) {
        return;
    }

    // Averages and the worst frame across history
    float total_update = 0.0f;
    float total_render = 0.0f;
    float total_frame = 0.0f;
    float worst_frame = 0.0f;

    for (int i = 0; i < stats.frame_count; i++) {
        const FrameTiming &timing = frame_at(stats, i);

        total_update = total_update + timing.update_ms;
        total_render = total_render + timing.render_ms;
        total_frame = total_frame + timing.frame_ms;

        if (timing.frame_ms > worst_frame) {
            worst_frame = timing.frame_ms;
        }
    }

    float average_frame = total_frame / stats.frame_count;
    float fps = 0.0f;

    if (average_frame > 0.0f) {
        fps = 1000.0f / average_frame;
    }

    SDL_SetRenderDrawColor(renderer, 200, 200, 210, 255);
    draw_perf_line(renderer, 0, "FPS", fps, "");
    draw_perf_line(renderer, 1, "Frame", average_frame, "ms");
    draw_perf_line(renderer, 2, "Worst", worst_frame, "ms");
    draw_perf_line(renderer, 3, "Update", total_update / stats.frame_count, "ms");
    draw_perf_line(renderer, 4, "Render", total_render / stats.frame_count, "ms");

    // Graph background
    SDL_FRect background {graph_x, graph_y, graph_width, perf_graph_height};
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderFillRect(renderer, &background);

    const float graph_bottom = graph_y + perf_graph_height;

    // One bar per frame, wit oldest on the left. Bright part is time spent working,
    // dim part is time spent waiting for frame cap
    for (int i = 0; i < stats.frame_count; i++) {
        const FrameTiming &timing = frame_at(stats, i);
        float x = graph_x + i * perf_bar_width;

        float frame_height = graph_height_for(timing.frame_ms);
        float work_height = graph_height_for(timing.update_ms + timing.render_ms);

        SDL_FRect frame_bar {x, graph_bottom - frame_height, perf_bar_width, frame_height};
        SDL_FRect work_bar {x, graph_bottom - work_height, perf_bar_width, work_height};

        SDL_SetRenderDrawColor(renderer, 60, 70, 110, 255);
        SDL_RenderFillRect(renderer, &frame_bar);

        // Frames that ran over budget show up red
        if (timing.frame_ms > frame_budget_ms * 1.05f) {
            SDL_SetRenderDrawColor(renderer, 230, 80, 80, 255);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 120, 220, 140, 255);
        }
        SDL_RenderFillRect(renderer, &work_bar);
    }

    // Line marking 60 FPS budget
    float budget_y = graph_bottom - graph_height_for(frame_budget_ms);
    SDL_SetRenderDrawColor(renderer, 255, 220, 100, 255);
    SDL_RenderLine(renderer, graph_x, budget_y, graph_x + graph_width, budget_y);

    // Legend under the graph, each word in its bar's color
    const float legend_y = graph_bottom + 10.0f;

    SDL_SetRenderDrawColor(renderer, 120, 220, 140, 255);
    SDL_RenderDebugText(renderer, graph_x, legend_y, "work");

    SDL_SetRenderDrawColor(renderer, 110, 125, 190, 255);
    SDL_RenderDebugText(renderer, graph_x + 6 * 8.0f, legend_y, "wait");

    SDL_SetRenderDrawColor(renderer, 255, 220, 100, 255);
    SDL_RenderDebugText(renderer, graph_x + 12 * 8.0f, legend_y, "-- 60 FPS");
}
