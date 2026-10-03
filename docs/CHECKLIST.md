# Pikranji — Quality Checklist

## Pre-Commit (Quick)
- [ ] NDS build passes (`cd nds && ./build.sh`)
- [ ] No compiler warnings (`-Wall` clean)
- [ ] `pikranji.nds` runs in DeSmuME/MelonDS
- [ ] Touch controls work: tap, drag, long-press
- [ ] Save/load persists across sessions (FAT init)
- [ ] Audio plays (music + SFX)
- [ ] All 232 puzzles load and are solvable

## Pre-Release (Thorough)
- [ ] Web version loads in browser (index.html)
- [ ] Web version saves to localStorage
- [ ] Generator produces valid JSON (`python3 gerar_kanji.py`)
- [ ] Converter produces valid header (`cd nds && python3 converter.py`)
- [ ] No TODO/FIXME in source code
- [ ] Documentation updated (VISION, REQUIREMENTS, ROADMAP)
- [ ] Diffstory written for each ticket

## NDS-Specific
- [ ] No dynamic allocation (malloc/free banned)
- [ ] Static arrays sized for max puzzles (1000)
- [ ] VRAM banks set correctly (A=text, B=top BG, C=bottom BG)
- [ ] Stack usage < 8 KB (EWARM constraint)
- [ ] ROM size < 4 MB
- [ ] Interrupt handlers minimal (VBlank only)

## Generator-Specific
- [ ] Excludes existing kanji from puzzles.json
- [ ] Output JSON format matches spec (compact arrays)
- [ ] Handles missing font gracefully