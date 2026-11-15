#pragma once

#include <string>
#include <vector>
#include "board.h"

// How fast a spell lands
enum SpellSpeed {
    SPEED_INSTANT,      // lands right away
    SPEED_WIND_UP       // lands at the start of the caster's next turn
};

// One tile of spell's shape
struct Offset {
    int row;
    int column;
};

// Spell specs
struct Spell {
    std::string name;
    int sp_cost {0};
    int power {0};                      // damage dealt, or HP restored
    SpellSpeed speed {SPEED_INSTANT};
    bool hidden {false};                // wind-up only (its tiles aren't shown)
    bool heals {false};                 // healing spells target the caster's own grid
    std::vector<Offset> shape;          // the tiles it covers
};

std::vector<Spell> hero_spells ();
std::vector<Tile> spell_tiles (const Spell &spell, int anchor_row, int anchor_column);