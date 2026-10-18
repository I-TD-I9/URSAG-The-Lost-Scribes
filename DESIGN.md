# URSAG: The Lost Scribes - Battle System Design

## Overview

A turn-based battle system built in C++ with SDL3.

The hero and the enemies stand on two grids that face each other. Attacks and spells target tiles, not units: every one has a shape and a range, and anyone standing inside the shape gets hit. Where a unit stands decides both what it can reach and what can reach it.

Inspired by Mega Man Battle Network's facing grids, and by Persona's SP system and battle camera.

## Design Pillars

1. **Position is everything.** Attacks and spells hit tiles, and every one has a limited range. Stepping forward lets a unit reach deeper, but it also brings the unit within the enemy's reach.
2. **Every AP is a choice.** Moving, attacking and using items draw from the same AP pool, and running out of AP ends the turn. AP spent dodging is AP that can't be spent getting in range.
3. **Readable at a glance.** The player can always see what's threatening them. Wind-up spells mark their tiles in advance, and hidden spells still show that something is coming.

## The Board

Two 6x6 grids face each other across a center line. Rows line up across both grids.

```
            HEROES                         ENEMIES
    Back              Front          Front             Back
     c5  c4  c3  c2  c1  c0          c0  c1  c2  c3  c4  c5
r0    .   .   .   .   .   .     |     .   .   .   .   .   .
r1    .   .   .   .   .   .     |     .   .   .  E1   .   .
r2    .   .   .  H1   .   .     |     .   .   .   .   .   .
r3    .   .   .   .   .   .     |     .   .   .   .   .   .
r4    .   .   .   .   .   .     |     .   .   .   .  E2   .
r5    .   .   .   .   .   .     |     .   .   .   .   .   .
                                ^ center line
```

- A unit can only move inside its own grid. Neither side can cross over.
- Only one unit can stand on a tile.
- Columns are counted from the center line outward: c0 is the **Front**, c5 is the **Back**.

### Range

Every attack and spell has a **range**: how far from the caster its target can be. Distance across the center line is:

```
distance = caster's column + target's column + 1 + rows apart
```

The `+ 1` counts the step across the center line, so two units facing each other at the front of the same row are 1 apart.

An attack has a range of 4. From r2 c0, the hero can reach the `o` tiles:

```
     c5  c4  c3  c2  c1  c0          c0  c1  c2  c3  c4  c5
r0    .   .   .   .   .   .     |     o   o   .   .   .   .
r1    .   .   .   .   .   .     |     o   o   o   .   .   .
r2    .   .   .   .   .  H1     |     o   o   o   o   .   .
r3    .   .   .   .   .   .     |     o   o   o   .   .   .
r4    .   .   .   .   .   .     |     o   o   .   .   .   .
r5    .   .   .   .   .   .     |     o   .   .   .   .   .
```

From r2 c3, the same attack only reaches enemy c0 in row 2.

Healing spells target the caster's own grid, so their range is just the rows plus columns between the two tiles.

### Starting positions

Every battle starts with units on random tiles, so no two battles open the same way.
- Units only spawn in their grid's back four columns (c2 to c5).
- No two units can share a tile.

Keeping the two front columns empty means the closest possible start is 5 apart, one more than an attack's range. No one can attack before moving.

## Stats

| Stat | Hero | Enemy 1 | Enemy 2 |
|------|------|---------|--------|
| HP   | 400  | 250     | 150    |
| AP   | 150  | 150     | 150    |
| SP   | 200  | 120     | 120    |

- **AP** pays for moving, attacking and items. It refills at the start of each turn, and leftover AP is lost.
- **SP** pays for spells. It never refills on its own; only items restore it. Attacks don't cost SP, so a unit with no SP can still fight.
- Enemies follow the same rules as the hero, but carry no items.

## Turns

A turn ends when the unit **attacks**, **casts a spell**, **uses an item**, **passes**, or **runs out of AP**. Moving is the only action that doesn't end the turn, so a typical turn is: move into position, then act.

## Battle Menu

| Option | Cost | Effect |
|--------|------|--------|
| Attack | 25 AP | Pick one tile on the enemy grid, within range 4. Hits instantly for 30 damage. Ends the turn |
| Move   | 50 AP per tile | Plan a move with the arrow keys. A ghost of the hero shows the destination, and the AP display shows what the move will cost. Enter locks it in, Esc cancels |
| Spells | SP   | Opens the list of known spells with their SP cost and speed. After choosing one, the player places its shape on the target grid. Casting ends the turn |
| Items  | 25 AP | Opens the list of items and how many are left. Using one ends the turn |
| Pass   | -    | Ends the turn without acting. Useful when nothing is in range, or to save SP |
| Flee   | -    | Leaves the battle |

Every unit's attack works the same way: one tile, range 4, no delay.

## Spells

### Spell speed

| Speed | Name    | Lands | Used for |
|-------|---------|-------|----------|
| 1     | Instant | Immediately | Small spells: 3 tiles or fewer, weak to medium power. Most spells are speed 1 |
| 2     | Wind-up | At the start of the caster's next turn | Larger shapes |

Instant spells are small but certain. Wind-up spells are bigger, but they can be seen coming and dodged.

### Wind-up spells

1. **Cast.** The spell's shape is placed on its target grid. Those tiles become **danger tiles**, visible to everyone unless the spell is hidden.
2. **React.** The other side takes its turn and can spend AP to move off the danger tiles.
3. **Land.** At the start of the caster's next turn, the spell hits every unit still on its danger tiles.

- Danger tiles stay put, even if the caster moves.
- If the caster is defeated before its spell lands, the spell fizzles.

### Hidden spells

**Hidden** is a trait only wind-up spells can have. A hidden spell's danger tiles aren't shown. The caster shows a warning while it winds up, so the other side knows something is coming, just not where.

Hidden spells cover a lot of tiles but deal little damage. Enemies play by the same rule: the enemy AI only dodges danger tiles it can see.

### Aiming

Attacks and spells are aimed by hand. After choosing Attack or a spell, the player moves a cursor over the target grid. The cursor can only move over tiles within range, and those tiles are highlighted.
- **Attack:** the cursor picks one tile.
- **Spells:** the shape's **anchor tile** (`@`) follows the cursor. Attack spells target the enemy grid, and healing spells target the caster's own grid.

Only the anchor has to be within range. The rest of the shape can reach past it, and any part that falls off the grid is dropped.

### Cost and dodge cost

Spells come in four tiers. The more tiles a spell covers, the more SP it costs:

| Tier   | SP  |
|--------|-----|
| Weak   | 20  |
| Medium | 40  |
| Strong | 60  |
| Grand  | 100 |

**Dodge cost** is the AP a unit needs to escape a visible wind-up spell, at 50 AP per move. This is where big spells earn their cost:
- A small wind-up spell is easy to dodge, usually in a single move.
- A grand spell can take all 150 AP to escape, which ends the target's turn before it can act. Some positions can't escape it at all.

### Spell list

| Spell     | Speed           | SP  | Range | Tiles    | Effect      | Worst-case dodge cost | Known by |
|-----------|-----------------|-----|-------|----------|-------------|-----------------------|----------|
| Mend      | Instant         | 20  | 3     | 1        | Heals 80 HP | -                     | Hero |
| Pillar    | Instant         | 20  | 6     | 3        | 35 damage   | -                     | Hero, Enemy 2 |
| Veil      | Wind-up, hidden | 20  | 8     | 4        | 20 damage   | -                     | Hero, Enemy 2 |
| Sweep     | Wind-up         | 40  | 3     | 6        | 50 damage   | 50 AP                 | Hero, Enemy 1 |
| Cross     | Wind-up         | 40  | 7     | 5        | 50 damage   | 100 AP                | Hero, Enemy 2 |
| Quake     | Wind-up         | 60  | 4     | 18       | 45 damage   | 100 AP                | Hero, Enemy 1 |
| Cataclysm | Wind-up         | 100 | 4     | up to 30 | 40 damage   | Can't be dodged       | Hero |

Enemy 1's spells are short range, so it has to come forward to use them. Enemy 2's are long range, so it can stay back. Cataclysm is the strongest spell, but its short range means the hero has to step forward to cast it.

```
Pillar      Cross       Veil
  X         . X .        X
  @         X @ X        @
  X         . X .        X
                         X
```

- **Mend:** a single tile
- **Sweep:** the anchor's whole row
- **Quake:** the anchor's row, plus the rows directly above and below
- **Cataclysm:** the anchor's row, plus two rows above and two below

Cross aimed at r0, c0. The parts that fall off the grid are dropped:

```
Shape        Enemy grid
. X .        @ X . . . .   <- r0
X @ X        X . . . . .   <- r1
. X .        . . . . . .   <- r2
```

## Enemies

Enemies use the same rules as the hero, aimed at the hero grid. On its turn, an enemy works down this list, repeating until its turn ends:

1. **Dodge:** if it's standing on a visible danger tile, move toward the nearest safe tile.
2. **Cast:** if the hero is within range of one of its spells and it has the SP, cast it centered on the hero.
3. **Attack:** if the hero is within attack range, attack the hero's tile.
4. **Approach:** move one step closer to the hero.
5. **Pass:** if none of the above is possible.

An enemy never steps onto a visible danger tile on purpose.

## Items

| Item     | Cost  | Effect |
|----------|-------|--------|
| SP Tonic | 25 AP | Restores 50 SP |

- The hero starts with 2 SP Tonics.
- Every 3 rounds, another SP Tonic appears on a random empty hero tile. Stepping onto it picks it up.

## Round Structure

1. **Hero turn:** the hero's wind-up spell from last turn lands, AP refills, and the player acts until the turn ends.
2. **Enemy turn:** enemy wind-up spells from last turn land, AP refills, and each enemy acts until its turn ends.

- **Victory:** every enemy is defeated.
- **Defeat:** the hero is defeated.

## Controls

| Key        | Action |
|------------|--------|
| Up / Down  | Choose a menu option |
| Enter      | Confirm |
| Esc        | Back / cancel |
| Arrow keys | Plan a move, or move the target cursor |

## Camera

The battle is seen from a fixed angle, similar to Persona 1's battle screen, with 2D sprites standing on a flat board. While a unit picks the target for an attack or a spell, the camera pans slightly toward the target grid, then eases back once the action is done. The camera never rotates.

## Feedback

- Tiles in range are highlighted while picking a target.
- Danger tiles are tinted red. The hero's own pending spells use a different color.
- A unit winding up a hidden spell shows a warning icon above it.
- Floating damage numbers, hit flashes, and screen shake on big hits.
- Units slide between tiles instead of teleporting.
- A turn banner at the start of each turn.

## Credits

Assets used, with their source and license:

- _(to be filled in)_
