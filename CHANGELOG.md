# Changelog

Notable changes to BNB-Foraging. Versions here follow the Steam Workshop
releases. Source history in this repository starts at v1.4.4, so `v` tags exist
only from that release onward; earlier entries are recorded for reference.

## v1.4.8

- Version bump to stay in step with the Steam Workshop release. No source
  change reached this mirror.

## v1.4.7

- Fix: guard the berry-bush mining suppressor against a null action target,
  which could throw when the game evaluated the action with no target present.
- Performance: cache fruit-tree classification per object class. The per-frame
  action check no longer re-resolves the config and genus lookups every time.
- Fix: the per-object cooldown log is no longer at risk of truncation. The
  write path only falls back to create-mode when the file is genuinely absent,
  so an existing log is never overwritten.
- Removed the `search_requires_free_hands` option. Searching always needs empty
  hands now: the first find goes to a free hand, any extras drop at your feet.
  Making the toggle behave correctly would have needed RPC sync and further
  design, so it was dropped as not worth the effort and risk for the payoff.

## v1.4.6

- Added the `search_requires_free_hands` option (default on): the Search action
  only appears with empty hands. Removed again in v1.4.7.

## v1.4.5

- Default tuning pass: fruit yield lowered to 1 to 2 per search (was 2 to 4),
  berry yield raised to 1 to 4 (was 1 to 3).
- Per-object cooldowns raised from 30 to 60 minutes for trees, stumps and
  bushes.

## v1.4.4

- Bare-hand cuts now apply a small fixed shock (a brief grunt and flinch) on
  top of the bleed.
- First full Steam Workshop page and preview image.
- Two debug-log fixes.

## v1.4.3

- Hard-coded the cut shock as a fixed effect and removed the per-verb
  `cut_shock` config field added in v1.4.2 (overly complex and probably YAGNI).

## v1.4.2

- Added a per-verb `cut_shock` config field. Removed again in v1.4.3.

## v1.4.1

- Bare-hand cutting is now on by default: fruit 0.10, mushroom 0.05 and berry
  0.15 cut chance without gloves.
- Glove wear set to the vanilla bush stick-gathering rate (`glove_damage` 4.0).

## v1.4.0

- Foraging bare-handed, or with ruined gloves, can now cut you: a bleeding
  forearm and a sharp jolt. Intact gloves protect you and take wear instead.
  Adds the six cut config fields; ruined gloves count as bare hands.

## v1.3.0

- Hands-first placement: the first find goes straight into a free hand, any
  extras land on the ground at your feet.

## v1.2.0

- Cleanup and fixes.

## v1.1.1

- Berry-bush mining suppressed so Search is the only option on a berry bush.

## v1.1.0

- First Workshop release of the foraging system: Search for fruit on trees,
  mushrooms on stumps and berries on bushes, on vanilla DayZ world objects.

## v1.0.0

- Initial release.
