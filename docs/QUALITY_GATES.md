# Pikranji — Domain-Specific Quality Gates

## NDS Build Gate
```bash
cd nds && ./build.sh
```
**Pass criteria**: Exit code 0, `pikranji.nds` produced, no compiler errors.

## NDS Runtime Gate
Manual verification in DeSmuME/MelonDS:
1. ROM boots to game screen
2. Touch input registers correctly
3. Background music plays
4. SFX play on actions
5. Save file created on FAT
6. All 232 puzzles accessible

## Puzzle Data Integrity Gate
```bash
cd nds && python3 -c "
import json
with open('../puzzles.json') as f:
    data = json.load(f)
for i, p in enumerate(data):
    assert 'kanji' in p and 'meaning' in p and 'grid' in p
    assert len(p['grid']) == 15
    for row in p['grid']:
        assert len(row) == 15
        for cell in row:
            assert cell in (0, 1)
print(f'OK: {len(data)} puzzles valid')
"
```

## Converter Round-Trip Gate
```bash
cd nds && python3 converter.py && echo "Header generated successfully"
```
**Pass criteria**: `include/puzzles.h` created with PUZZLE_COUNT matching JSON.

## Web Version Gate
Open `index.html` in browser:
- [ ] Grid renders correctly
- [ ] Clues calculate correctly
- [ ] Touch/mouse input works
- [ ] localStorage save/load works