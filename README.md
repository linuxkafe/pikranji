# Pikranji — Kanji Picross

A Kanji Picross (Nonogram) game with three frontends:

- **Web** (`index.html`) — HTML5/JS, playable in browser
- **Generator** (`gerar_kanji.py`, `gerador/`) — Creates puzzles from font rendering or manual drawing
- **NDS Port** (`nds/`) — Nintendo DS homebrew using devkitPro (libnds)

## Game Objective

The hidden image on a 15×15 grid is always a **Kanji** character. Fill in the cells following the numerical clues on the sides and top to reveal the character. Its meaning is displayed as a hint.

Solve all rows and columns correctly to win and unlock new puzzles.

## How to Play

| Element | Description |
|---------|-------------|
| **Left clues** | Block sizes of filled cells per row |
| **Top clues** | Block sizes of filled cells per column |
| **Fill tool** | Marks cells black |
| **Mark tool (X)** | Marks cells as empty |
| **Restart** | Clears the grid, keeps the same puzzle |
| **Solve** | Reveals the solution (marks as cheated, no score) |
| **Next** | Loads a random unlocked puzzle |

**Touch/mouse:** Click to toggle · Drag to paint · Long-press as backup tap.

### Control Buttons (NDS)

| Button | Action |
|--------|--------|
| `?` | Hint — reveals a random unsolved line/col (−1 point) |
| `!` | Solve — fills the entire grid, marks as cheated |

---

## Build & Development

### Web

Open `index.html` directly in a browser. No build step required.

### NDS (requires Docker + devkitPro)

```bash
cd nds && ./build.sh        # build and produce pikranji.nds
cd nds && ./build.sh clean  # clean build artifacts
```

### Regenerate puzzles.h

After editing `puzzles.json`:

```bash
cd nds && python3 converter.py
```

### Generate new puzzles from font

```bash
python3 gerar_kanji.py    # outputs novos_puzzles.json
```

---

## Adding New Puzzles

Edit **`puzzles.json`** directly or use the included generator tools.

Each puzzle object:

| Field | Type | Description |
|-------|------|-------------|
| `"kanji"` | String | The Japanese character |
| `"meaning"` | String | Meaning in Portuguese |
| `"meaning_en"` | String | Meaning in English (falls back to `meaning` if missing) |
| `"grid"` | Array 15×15 | `0` = empty, `1` = filled |

Example:

```json
{
    "kanji": "田",
    "meaning": "Rice Field",
    "meaning_en": "Rice Field",
    "grid": [
        [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
        [0,0,1,1,1,1,1,1,1,1,1,1,1,0,0],
        [0,0,1,0,0,0,0,1,0,0,0,0,1,0,0],
        [0,0,1,0,0,0,0,1,0,0,0,0,1,0,0],
        [0,0,1,0,0,0,0,1,0,0,0,0,1,0,0],
        [0,0,1,1,1,1,1,1,1,1,1,1,1,0,0],
        [0,0,1,0,0,0,0,1,0,0,0,0,1,0,0],
        [0,0,1,0,0,0,0,1,0,0,0,0,1,0,0],
        [0,0,1,0,0,0,0,1,0,0,0,0,1,0,0],
        [0,0,1,1,1,1,1,1,1,1,1,1,1,0,0],
        [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
    ]
}
```

---

## Web Generator

Open `gerador/index.html` in a browser to draw puzzles visually:

1. Enter the kanji character and its meaning (PT + EN).
2. Draw on the 15×15 grid by clicking/dragging.
3. Click **Add to Collection**.
4. When done, click **Download JSON** and merge into `puzzles.json`.

---

## NDS Port Architecture

The NDS source is split into logical modules:

| Module | Responsibility |
|--------|----------------|
| `main.c` | Entry point, game loop, state machine |
| `puzzle.c/h` | Clue calculation, win detection, bag/shuffle |
| `render.c/h` | All drawing: grid, clues, UI, particles, backgrounds |
| `input.c/h` | Touch/key handling, drag state machine |
| `audio.c/h` | Background music loop, SFX (PSG + noise channels) |
| `save.c/h` | FAT init, save/load, progressive unlock, language toggle |
| `common.h` | Shared constants, types, extern declarations |

### NDS Constraints

- No dynamic allocation — all arrays are static.
- VRAM: Bank A = text console, Bank B = top screen bitmap, Bank C = bottom screen bitmap.
- `puzzles.h` is **generated** from `puzzles.json` — never edit it manually.

---

## Scoring

| Event | Points |
|-------|--------|
| Base win | +50 |
| Per filled pixel | +2 |
| Each hint used | −1 |
| Solving (cheat) | No points, flag set |

**Progressive unlock:** Every 5 puzzles solved, 10 more are unlocked (capped at total count).

---

## Language Support

Toggle between Portuguese and English by pressing **X** on NDS (or the language button on Web). Some puzzle meanings may fall back to Portuguese if English is not provided.

---

## Repository Structure

```
pikranji/
├── index.html           # Web game (single-file, Tailwind CDN)
├── puzzles.json         # Source of truth for all puzzles
├── gerar_kanji.py       # Python font-based puzzle generator
├── gerador/             # Web puzzle editor
│   ├── index.html
│   └── README.md
├── nds/                 # NDS homebrew
│   ├── Makefile
│   ├── build.sh         # Docker-based build
│   ├── converter.py     # puzzles.json → puzzles.h
│   ├── source/          # Modular C source
│   ├── include/puzzles.h  # Generated — do NOT edit
│   └── assets/gfx/      # Background images (sea1-3.bmp)
├── aes/                 # Project management (kanban, tickets, sprints)
├── docs/                # Documentation (VISION, REQUIREMENTS, etc.)
└── CLAUDE.md            # Operational contract for AI agents
```

---

## License

MIT — see `LICENSE` file.
