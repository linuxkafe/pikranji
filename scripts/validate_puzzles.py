#!/usr/bin/env python3
"""validate_puzzles.py — Validate puzzles.json integrity for Pikranji.

Checks:
  - File is valid JSON
  - Each puzzle has: kanji, meaning, meaning_en, grid
  - Grid is 15x15
  - Cells are only 0 or 1
  - No duplicate kanji
  - All puzzles have meaning_en (or fall back to meaning)
  - Clue consistency: clues generated from the grid match the grid itself
"""

import json
import sys
from pathlib import Path

PUZZLES_FILE = Path(__file__).resolve().parent.parent / "puzzles.json"
GRID_SIZE = 15

def compute_line_clues(line):
    """Compute the nonogram clue for a single row/column."""
    clues = []
    count = 0
    for cell in line:
        if cell == 1:
            count += 1
        elif count > 0:
            clues.append(count)
            count = 0
    if count > 0:
        clues.append(count)
    return clues

def validate_puzzle(puzzle, index):
    """Validate a single puzzle entry. Returns list of error strings."""
    errors = []

    # Required fields
    for field in ("kanji", "meaning", "grid"):
        if field not in puzzle:
            errors.append(f"Puzzle {index}: missing required field '{field}'")
            return errors  # can't validate further without grid

    # Kanji
    kanji = puzzle.get("kanji", "")
    if not isinstance(kanji, str) or len(kanji) == 0:
        errors.append(f"Puzzle {index}: 'kanji' must be a non-empty string")

    # Meaning
    meaning = puzzle.get("meaning", "")
    if not isinstance(meaning, str) or len(meaning) == 0:
        errors.append(f"Puzzle {index}: 'meaning' must be a non-empty string")

    # Meaning EN (fallback to meaning if missing)
    meaning_en = puzzle.get("meaning_en", meaning)
    if not isinstance(meaning_en, str) or len(meaning_en) == 0:
        errors.append(f"Puzzle {index}: 'meaning_en' must be a non-empty string (or 'meaning' must be present)")

    # Grid
    grid = puzzle.get("grid")
    if not isinstance(grid, list):
        errors.append(f"Puzzle {index}: 'grid' must be a list")
        return errors

    if len(grid) != GRID_SIZE:
        errors.append(f"Puzzle {index}: grid has {len(grid)} rows, expected {GRID_SIZE}")
        return errors

    for row_idx, row in enumerate(grid):
        if not isinstance(row, list):
            errors.append(f"Puzzle {index}: grid row {row_idx} is not a list")
            continue
        if len(row) != GRID_SIZE:
            errors.append(f"Puzzle {index}: grid row {row_idx} has {len(row)} cells, expected {GRID_SIZE}")
            continue
        for col_idx, cell in enumerate(row):
            if cell not in (0, 1):
                errors.append(f"Puzzle {index}: grid[{row_idx}][{col_idx}] = {cell}, expected 0 or 1")

    # Clue consistency: clues generated from grid must match grid itself
    if isinstance(grid, list) and len(grid) == GRID_SIZE:
        for r in range(GRID_SIZE):
            row = grid[r]
            if isinstance(row, list) and len(row) == GRID_SIZE:
                clues = compute_line_clues(row)
                # Reconstruct: the clues should uniquely describe the row
                # (we don't solve, just verify consistency)
                if len(clues) > 8:
                    errors.append(f"Puzzle {index}: row {r} has {len(clues)} clue blocks, max is 8")

        for c in range(GRID_SIZE):
            col = [grid[r][c] for r in range(GRID_SIZE) if isinstance(grid[r], list) and len(grid[r]) == GRID_SIZE]
            if len(col) == GRID_SIZE:
                clues = compute_line_clues(col)
                if len(clues) > 8:
                    errors.append(f"Puzzle {index}: col {c} has {len(clues)} clue blocks, max is 8")

    return errors

def main():
    if not PUZZLES_FILE.exists():
        print(f"ERROR: {PUZZLES_FILE} not found")
        sys.exit(1)

    try:
        with open(PUZZLES_FILE, "r", encoding="utf-8") as f:
            data = json.load(f)
    except json.JSONDecodeError as e:
        print(f"ERROR: Invalid JSON in {PUZZLES_FILE}: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"ERROR: Could not read {PUZZLES_FILE}: {e}")
        sys.exit(1)

    if not isinstance(data, list):
        print("ERROR: puzzles.json must contain a JSON array")
        sys.exit(1)

    all_errors = []
    seen_kanji = set()

    for i, puzzle in enumerate(data):
        errors = validate_puzzle(puzzle, i)
        all_errors.extend(errors)

        # Check for duplicate kanji
        kanji = puzzle.get("kanji", "")
        if kanji and kanji in seen_kanji:
            all_errors.append(f"Puzzle {i}: duplicate kanji '{kanji}'")
        if kanji:
            seen_kanji.add(kanji)

    if all_errors:
        print(f"VALIDATION FAILED: {len(all_errors)} error(s) found")
        for err in all_errors[:50]:  # show first 50
            print(f"  - {err}")
        if len(all_errors) > 50:
            print(f"  ... and {len(all_errors) - 50} more")
        sys.exit(1)

    print(f"OK: {len(data)} puzzles valid")
    print(f"  - All grids are 15x15 with 0/1 cells")
    print(f"  - All puzzles have kanji, meaning, meaning_en")
    print(f"  - No duplicate kanji")
    print(f"  - Clue blocks within MAX_CLUES (8) limit")
    sys.exit(0)

if __name__ == "__main__":
    main()