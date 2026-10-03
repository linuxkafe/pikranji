# Pikranji — Operational Contract

## Project Intent
A Kanji Picross (Nonogram) game with three frontends:
- **Web** (`index.html`) — HTML5/JS, playable in browser
- **Generator** (`gerar_kanji.py`, `gerador/`) — Creates puzzles from font rendering or manual drawing
- **NDS Port** (`nds/`) — Nintendo DS homebrew using devkitPro (libnds)

## Non-Goals
- Multiplayer or network features
- 3D graphics or advanced effects beyond particle fireworks
- Platforms other than Web and NDS (no mobile app, no desktop native)

## Critical Files
| File | Purpose |
|------|---------|
| `puzzles.json` | Source of truth for all puzzles (kanji, meaning, grid) |
| `nds/include/puzzles.h` | Generated C header for NDS build (run `converter.py` to update) |
| `nds/source/main.c` | NDS game logic — **monolithic, needs modularization** |
| `gerar_kanji.py` | Python generator using Pillow to rasterize TTF font |
| `index.html` | Web version — single file, Tailwind via CDN |

## Never-Do List
- **Never edit `nds/include/puzzles.h` manually** — always regenerate via `converter.py`
- **Never commit `nds/build/` artifacts** — gitignored
- **Never add puzzles directly to `puzzles.h`** — add to `puzzles.json` then convert
- **Never use dynamic allocation on NDS** — static arrays only (EWARM constraints)
- **Never assume VRAM layout** — use `vramSetBank*` and `bgGetGfxPtr` as in `main.c`

## Evidence Required
Before declaring any NDS task done:
1. `./build.sh` completes without errors
2. `pikranji.nds` runs in DeSmuME/MelonDS
3. Touch controls work: tap, drag, long-press
4. Save/load persists across sessions (FAT init)
5. Audio plays (music + SFX)
6. All 232 puzzles load and are solvable

## Commands
```bash
# NDS build (requires Docker)
cd nds && ./build.sh

# Regenerate puzzles.h from puzzles.json
cd nds && python3 converter.py

# Generate new kanji puzzles from font
python3 gerar_kanji.py

# Web version — open index.html directly in browser
```