# BNB-Foraging

**v1.5.1**

Forage the Chernarus wilderness for food. Three foraging actions on vanilla
DayZ world objects - pick fruit from trees, mushrooms from stumps, and berries
from bushes - with a bare-hand injury risk, per-object cooldowns, and full
server-side tuning. No new items: everything it spawns is vanilla DayZ game
data.

**This mod is free for community use, repack or fork - and always will be - with
attribution. Source code is on GitHub. See the Licence section for details.**

## What it adds

- **Search for fruit** - search a fruit tree for apples, plums or pears.
- **Search for mushrooms** - search a tree stump for mushrooms.
- **Search for berries** - search a berry bush (rose hips, elderberries) for
  berries.

Each search has its own find chance and yield range. The player's hands must be
free to search - the first find goes straight into the player's empty hands, and
any extras scatter on the ground at their feet, each placed with a ground check
so nothing spawns inside objects or under the floor.

## Cooldowns

Every tree, stump and bush tracks its own cooldown once searched, so no single
spot can be farmed dry. Cooldowns run on real time and survive server restarts.

## Food quality

Foraged food can come up fresh, dried, or rotten - the odds are
server-configurable. Finds also vary in condition, from slightly worn to
pristine, with the amount of food scaled to match.

## Server setup - turn off the wild fruit and mushrooms

By default DayZ scatters fruit under fruit trees and mushrooms across the forest
floor on its own, so out of the box players would just pick them up off the
ground and never need to forage. To make this mod the real source, disable those
ground spawns in the server's mission `db/events.xml` by setting `active = 0` on
the six Trajectory events:

- TrajectoryApple, TrajectoryPear, TrajectoryPlum (fruit under trees)
- TrajectoryConiferous, TrajectoryDeciduous, TrajectoryHumus (forest mushrooms)

Berries need no change - vanilla already ships those spawns off.

## Configuration - foraging.json

The file is created at `$profile:BNBForaging/foraging.json` on first server start
and re-saved each boot, so new options from an update appear automatically with
their defaults. One top-level option, plus a block per search:

- **debug_log** (default 0) - 1 logs every search to the server RPT (tagged
  BNB_Foraging::Debug) with player, target, yield, hands-vs-ground and cut.
  Config load success or failure is always logged.

Each block - **search_fruit**, **search_mushrooms**, **search_berries** -
carries the same options (defaults shown fruit / mushroom / berry):

- **find chance** - 0..1 chance a search yields. Fruit is per type
  (apple_drop_chance, plum_drop_chance, pear_drop_chance, 0.30 each);
  mushroom_find_chance 0.50; berry_find_chance 0.70.
- **min_count / max_count** - inclusive yield range on a successful search.
  Fruit 1 to 2 each, mushrooms 1 to 3, berries 1 to 4.
- **cooldown minutes** - per-object cooldown before the same tree/stump/bush can
  be searched again (0 = none). 60 (1 hour) by default
  (tree_/stump_/bush_cooldown_minutes).
- **search_duration_seconds** (10.0) - how long the player holds the action.
  Clamped 0.5 to 60.
- **cut_chance_no_gloves** - 0..1 chance a bare-handed or ruined-glove search
  cuts the player. Fruit 0.10, mushrooms 0.05, berries 0.15. 0 turns cuts off.
- **glove_damage** (4.0) - HP an intact glove loses per search while protecting
  from a cut (about 25 searches to ruin a fresh glove); matches vanilla
  stick-gathering wear.
- **food_stage_enable** (1), **food_dried_chance** (0.0), **food_rotten_chance**
  (0.10) - roll a dried or rotten food stage on each foraged item, otherwise it
  spawns fresh.

The small shock on a cut is a fixed part of the mechanic and is not
configurable.

## Compatibility

Server-side driven - clients only need the mod loaded. Spawns vanilla item
classes only, adds no new loot to balance. Built for Chernarus, but fruit trees
are recognised from game data rather than a fixed map list, so other maps' apple,
plum and pear trees should work too; other maps have their own equivalent
Trajectory events for the same wild-spawn cleanup.

## Install

Subscribe on the Steam Workshop

https://steamcommunity.com/sharedfiles/filedetails/?id=3768283328

## Build

Built with [HEMTT](https://github.com/BrettMayson/HEMTT). From the mod root:

```
hemtt release
```

Notes:

- `requiredAddons` are vanilla (`DZ_Data`, `DZ_Scripts`), so no third-party
  subscription is needed to build.
- Signing uses the `Bushy` authority. A clone does not carry that key, so
  build unsigned for local testing or supply your own authority in
  `.hemtt/project.toml`.

## Status

This repository is a one-way, automatically published mirror of the mod
shipped on the Steam Workshop. It does not accept issues or pull requests.
Fork it if you want to build on it, within the licence terms below.

Version history is in [CHANGELOG.md](CHANGELOG.md).

## Licence

Published under Creative Commons Attribution-NonCommercial 4.0 (CC BY-NC 4.0):
modify, repack, fork - non-commercial use only, with attribution. All script
content is Bullets'n'Bandages-authored; the fruit, mushroom and berry classes it
spawns from are vanilla DayZ game data. See [LICENSE](LICENSE).

DAYZ is a registered trademark of Bohemia Interactive a.s. This is an
unofficial modification that is not affiliated or authorized by Bohemia
Interactive a.s.
