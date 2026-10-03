# Pikranji — Requirements

## Functional Requirements

### Core Gameplay (All Platforms)
- [ ] 15x15 grid with row/column clues
- [ ] Two tools: Fill (black) and Mark (× for empty)
- [ ] Touch/mouse: tap to toggle, drag to paint, long-press support
- [ ] Auto-complete clue crossing when row/column solved
- [ ] Win detection: all cells match solution
- [ ] Hint system (reveal row/col, costs score)
- [ ] Solve/cheat button (reveals full solution, marks as cheated)
- [ ] Next puzzle button (loads random unlocked puzzle)
- [ ] Restart button (clears current grid)

### Puzzle Content
- [ ] 232+ kanji puzzles (N5 + N4 level)
- [ ] Each puzzle: kanji character, meaning (PT/EN), 15x15 solution grid
- [ ] Puzzles sorted by complexity (filled pixel count)
- [ ] Progressive unlock: solve puzzles to unlock more
- [ ] Score system: +50 base + 2 per filled pixel, -1 per hint

### NDS-Specific
- [ ] Dual screen: top = rotating background art, bottom = game
- [ ] Touch controls on bottom screen
- [ ] Background music (looping) + SFX (tap, mark, win, error)
- [ ] Particle fireworks on win
- [ ] 5 color themes (random on restart)
- [ ] Save/load via libfat (persistent across sessions)
- [ ] Language toggle: Portuguese / English
- [ ] Battery-backed save file (pikranji.sav)

### Generator (Python)
- [ ] Rasterize TTF font to 15x15 grid
- [ ] Read existing puzzles.json to avoid duplicates
- [ ] Output compact JSON format ([0,0,1,...] no spaces)
- [ ] Support N5+N4 kanji master list

### Generator (Web)
- [ ] Visual 15x15 grid editor
- [ ] Click/tap/drag to draw
- [ ] Export to kanji_collection.json

## Non-Functional Requirements

### NDS Port
- [ ] Build with devkitPro (Docker-based)
- [ ] No dynamic allocation (static arrays only)
- [ ] VRAM management via vramSetBank* / bgGetGfxPtr
- [ ] 60 FPS stable
- [ ] ROM size < 4 MB

### Web Port
- [ ] Single HTML file (no build step)
- [ ] Mobile-responsive (touch + mouse)
- [ ] Tailwind via CDN
- [ ] LocalStorage for save persistence

### Documentation
- [ ] CLAUDE.md operational contract
- [ ] C docstrings for all public functions
- [ ] AES project structure (kanban, tickets, sprints)