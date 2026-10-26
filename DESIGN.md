# URSAG: The Lost Scribes - Battle System Design

## Overview

A turn-based battle system built in C++ with SDL3.

Heroes and enemies stand on two grids that face each other. Attacks and spells target tiles, not units: anyone standing inside a shape gets hit. Spells land in the caster's own row, so where a unit stands decides which rows it can hit and which spells can hit it.

Inspired by Mega Man Battle Network's facing grids, and by Persona's SP system and battle camera.

## Design Pillars

1. **Position is everything.** Lining up with a target's row is how you aim. Staying out of an enemy's row is how you stay safe.
2. **Every AP is a choice.** Moving, attacking and using items share one AP pool, and running out of AP ends the turn.
3. **Readable at a glance.** Threats are always visible. Wind-up spells mark their tiles in advance, and hidden spells still show that something is coming.

## The Board

Two 6x6 grids face each other across a center line.

```
          HEROES                       ENEMIES
 .   .   .   .   .   .     |     .   .   .   .   .   .
 .   .   .   .   .   .     |     .   .   .   E   .   .
 .   .   .   H   .   .     |     -   -   @   -   -   -    <- the hero's row
 .   .   .   .   .   .     |     .   .   .   .   .   .
 .   .   .   .   .   .     |     .   .   .   .   E   .
 .   .   .   .   .   .     |     .   .   .   .   .   .
```

- Units only move inside their own grid, and only one unit can stand on a tile.
- Every battle starts with units on random tiles.
- A spell cast by the hero above is anchored in the hero's row (`@`) and can slide anywhere along it (`-`).

## Turns

- **AP** pays for moving, attacking and items. It refills at the start of every turn.
- **SP** pays for spells. It only comes back through items, so attacks are there for when it runs out.
- A turn ends when a unit attacks, casts, uses an item, passes, or runs out of AP. Moving is the only action that doesn't end it, so a turn is: move into position, then act.
- Heroes act first, then enemies. The battle is won when every enemy is defeated, and lost when every hero is.

## Actions

| Action | Effect |
|--------|--------|
| Attack | Hits one chosen tile anywhere on the enemy grid, instantly |
| Move   | Plan a move with a ghost preview that shows its AP cost, then confirm or cancel |
| Spells | Choose a spell and slide its shape along your row |
| Items  | Use an item |
| Pass   | End the turn without acting |
| Flee   | Leave the battle |

## Spells

- **Shapes.** Every spell has a shape, and anything inside it is hit. Taller shapes reach more rows, so they need less lining up. Parts that fall off the grid are dropped.
- **Speed.** Instant spells are small and land right away. Wind-up spells are bigger and land at the start of the caster's next turn. Their tiles are marked, so the other side gets a chance to step out.
- **Hidden.** Some wind-up spells don't show their tiles, only that something is coming. They cover more ground but hit softly.
- **Cost.** Bigger spells cost more SP. They earn it by forcing targets to spend AP escaping. The biggest can take a whole turn to escape, or can't be escaped at all.
- **Healing** works the same way, on the caster's own grid.

## Enemies

Enemies follow the same rules as heroes. On their turn they dodge any danger they can see, cast a spell if one can reach a hero, line up with a hero's row if not, and otherwise attack.

## Items

Items restore SP. Some appear on tiles during battle and are picked up by stepping on them.

## Camera

The battle is seen from a fixed angle, similar to Persona 1's battle screen, with 2D sprites standing on a flat board. While aiming, the camera pans slightly toward the target grid. It never rotates.

## Feedback

- The aiming cursor shows exactly which tiles will be hit.
- Danger tiles are tinted, and a unit winding up a hidden spell shows a warning.
- Damage numbers, hit flashes and screen shake on big hits.
- Units slide between tiles, and a banner marks the start of each turn.

## Credits

Assets used, with their source and license:

- _(to be filled in)_
