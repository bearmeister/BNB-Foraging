# BNB-Foraging

**v1.4.6**

Forage the Chernarus wilderness for food. Three foraging actions on vanilla
DayZ world objects (pick fruit from trees, mushrooms from stumps and berries
from bushes) with a bare-hand injury risk, per-object cooldowns and full
server-side tuning. No new items: everything it spawns is vanilla DayZ game
data.

## What it adds

- **Search for fruit**: search a fruit tree for apples, plums or pears.
- **Search for mushrooms**: search a tree stump for mushrooms.
- **Search for berries**: search a berry bush (rose hips, elderberries) for
  berries.

Each search has its own find chance and yield range. Your hands must be free
to search (server owners can turn this off): the first find goes into your
empty hands, any extras scatter on the ground at your feet.

## Bare hands have a price

Foraging bare-handed, or with ruined gloves, can cut you: a bleeding forearm
and a sharp jolt that makes you wince. Wear an intact pair of gloves and you
are protected, but they take wear with every search and will eventually ruin.
Thorny berry bushes bite more often than fruit trees or stumps. The small
shock on a cut is a fixed part of the mechanic and is not configurable.

## Cooldowns

Every tree, stump and bush tracks its own cooldown after you search it, so you
cannot farm one spot dry. Cooldowns run on real time and survive server
restarts.

## Food quality

Foraged food can come up fresh, dried or rotten. The odds are yours to set.

## Server setup: turn off the wild fruit and mushrooms

By default DayZ scatters fruit under fruit trees and mushrooms across the
forest floor on its own, so out of the box players would just pick them up off
the ground and never need to forage. To make this mod the real source, disable
those ground spawns in your mission's `db/events.xml` by setting `active = 0`
on the six Trajectory events:

- `TrajectoryApple`, `TrajectoryPear`, `TrajectoryPlum` (fruit under trees)
- `TrajectoryConiferous`, `TrajectoryDeciduous`, `TrajectoryHumus` (forest
  mushrooms)

Berries need no change: vanilla already ships those spawns off. Built for
Chernarus; other maps have their own equivalent Trajectory events if you want
the same wild-spawn cleanup.

## Configuration: foraging.json

The file is created at `$profile:BNBForaging/foraging.json` on first server
start and re-saved each boot, so new options from an update appear
automatically with their defaults.

Two top-level options:

- `search_requires_free_hands` (default `1`): `1` shows the Search action only
  with empty hands (the first find goes to a free hand); `0` lets players
  search while holding an item (every find drops to the ground).
- `debug_log` (default `0`): `1` logs every search to the server RPT (tagged
  `BNB_Foraging::Debug`) with player, target, yield, hands-vs-ground and cut.
  Config load success or failure is always logged.

Then a block per search (`search_fruit`, `search_mushrooms`, `search_berries`),
each carrying the same options. Defaults:

| Option | search_fruit | search_mushrooms | search_berries |
|---|---|---|---|
| find chance (`0..1`) | `apple`/`plum`/`pear_drop_chance` `0.30` each | `mushroom_find_chance` `0.50` | `berry_find_chance` `0.70` |
| yield range (inclusive) | `1` to `2` per fruit type | `1` to `3` | `1` to `4` |
| `*_cooldown_minutes` (0 = none) | `tree` `60` | `stump` `60` | `bush` `60` |
| `search_duration_seconds` | `10.0` | `10.0` | `10.0` |
| `cut_chance_no_gloves` (0 = off) | `0.10` | `0.05` | `0.15` |
| `glove_damage` | `4.0` | `4.0` | `4.0` |
| `food_stage_enable` / `food_dried_chance` / `food_rotten_chance` | `1` / `0.0` / `0.10` | `1` / `0.0` / `0.10` | `1` / `0.0` / `0.10` |

Notes:

- `glove_damage` is the HP an intact glove loses per protected search (about 25
  searches to ruin a fresh glove; matches vanilla stick-gathering wear).
- `search_duration_seconds` is clamped to `0.5..60`, cooldowns to `0..10080`
  minutes and counts to `0..50`. Chances are clamped to `0..1`.
- `food_stage_enable` `1` rolls a dried or rotten stage on each foraged item,
  otherwise it spawns fresh.

## Install

Subscribe on the Steam Workshop (recommended for players and most servers):

https://steamcommunity.com/sharedfiles/filedetails/?id=3768283328

For a manual server install, drop the packed `@BNB-Foraging` mod folder into
the server's mod set and load it. The mod is server-side driven; clients only
need it loaded.

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

This repository is a one-way source publication of the mod shipped on the
Steam Workshop. It does not accept issues or pull requests. Fork it if you
want to build on it, within the license terms below.

## Credits

All script content is Bullets'n'Bandages-authored. The fruit, mushroom and
berry classes it spawns from are vanilla DayZ game data; the mod adds no new
loot to balance.

## License

Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0).
Modify, repack and fork for non-commercial use with attribution. See
[LICENSE](LICENSE).
