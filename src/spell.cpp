#include "spell.h"
#include "board.h"

// Starts a spell with its name, cost, power and speed
Spell make_spell (const std::string &name, int sp_cost, int power, SpellSpeed speed) {
    Spell spell;
    spell.name = name;
    spell.sp_cost = sp_cost;
    spell.power = power;
    spell.speed = speed;

    return spell;
}

// Adds whole rows to spell's shape, from first_row to last_row
void add_full_rows (Spell &spell, int first_row, int last_row) {
    for (int row = first_row; row <= last_row; row++) {
        for (int column = -(grid_columns - 1); column <= grid_columns - 1; column++) {
            spell.shape.push_back(Offset {row, column});
        }
    }
}

// The spells hero knows, in the order the Spells list shows them
std::vector<Spell> hero_spells () {
    std::vector<Spell> spells;

    Spell mend = make_spell("Balatu", 20, 80, SPEED_INSTANT);       // "life"
    mend.heals = true;
    mend.shape = { {0, 0} };
    spells.push_back(mend);

    Spell pillar = make_spell("Dimmu", 20, 35, SPEED_INSTANT);      // "pillar"
    pillar.shape = { {-1, 0}, {0, 0}, {1, 0} };
    spells.push_back(pillar);

    Spell veil = make_spell("Puzru", 20, 20, SPEED_WIND_UP);        // "secret, hiding place"
    veil.hidden = true;
    veil.shape = { {-1, 0}, {0, 0}, {1, 0}, {2, 0} };
    spells.push_back(veil);

    Spell sweep = make_spell("Mehu", 40, 50, SPEED_WIND_UP);        // "storm"
    add_full_rows(sweep, 0, 0);
    spells.push_back(sweep);

    Spell cross = make_spell("Kakkabu", 40, 50, SPEED_WIND_UP);     // "star"
    cross.shape = { {-1, 0}, {0, -1}, {0, 0}, {0, 1}, {1, 0} };
    spells.push_back(cross);

    Spell quake = make_spell("Ribu", 60, 45, SPEED_WIND_UP);        // "earthquake"
    add_full_rows(quake, -1, 1);
    spells.push_back(quake);

    Spell cataclysm = make_spell("Abubu", 100, 40, SPEED_WIND_UP);  // "the Flood"
    add_full_rows(cataclysm, -2, 2);
    spells.push_back(cataclysm);

    return spells;
}


// The tiles a spell covers when its anchor is on
std::vector<Tile> spell_tiles (const Spell &spell, int anchor_row, int anchor_column) {
    std::vector<Tile> tiles;

    for (const Offset &offset : spell.shape) {
        int row = anchor_row + offset.row;
        int column = anchor_column + offset.column;

        if (is_on_grid(row, column)) {
            tiles.push_back(Tile {row, column});
        }
    }

    return tiles;
}