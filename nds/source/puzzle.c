#include "puzzle.h"
#include <stdlib.h>
#include <time.h>

/* Comparador para qsort: ordena por complexidade (pixels preenchidos) ascendente */
static int compare_puzzles(const void* a, const void* b) {
    const PuzzleEntry* pa = (const PuzzleEntry*)a;
    const PuzzleEntry* pb = (const PuzzleEntry*)b;
    return (pa->complexity - pb->complexity);
}

void puzzle_init(void) {
    for (int i = 0; i < PUZZLE_COUNT; i++) {
        sortedPuzzles[i].id = i;
        int pixels = 0;
        for (int r = 0; r < GRID_ROWS; r++)
            for (int c = 0; c < GRID_COLS; c++)
                if (allPuzzles[i].grid[r][c] == 1) pixels++;
        sortedPuzzles[i].complexity = pixels;
    }
    qsort(sortedPuzzles, PUZZLE_COUNT, sizeof(PuzzleEntry), compare_puzzles);
}

void puzzle_shutdown(void) { /* no-op: arrays estáticos */ }

void puzzle_calculate_target_clues(void) {
    const Puzzle* p = &allPuzzles[currentPuzzleIndex];
    /* Linhas */
    for (int r = 0; r < GRID_ROWS; r++) {
        int idx = 0, count = 0;
        for (int c = 0; c < GRID_COLS; c++) {
            if (p->grid[r][c] == 1) count++;
            else if (count > 0) {
                if (idx < MAX_CLUES) rowClues[r].values[idx++] = count;
                count = 0;
            }
        }
        if (count > 0 && idx < MAX_CLUES) rowClues[r].values[idx++] = count;
        rowClues[r].count = idx;
    }
    /* Colunas */
    for (int c = 0; c < GRID_COLS; c++) {
        int idx = 0, count = 0;
        for (int r = 0; r < GRID_ROWS; r++) {
            if (p->grid[r][c] == 1) count++;
            else if (count > 0) {
                if (idx < MAX_CLUES) colClues[c].values[idx++] = count;
                count = 0;
            }
        }
        if (count > 0 && idx < MAX_CLUES) colClues[c].values[idx++] = count;
        colClues[c].count = idx;
    }
}

bool puzzle_check_line_match(int index, bool isRow) {
    int currentClues[MAX_CLUES];
    int idx = 0, count = 0;
    for (int i = 0; i < 15; i++) {
        int cell = isRow ? playerGrid[index][i] : playerGrid[i][index];
        if (cell == 1) count++;
        else if (count > 0) {
            if (idx < MAX_CLUES) currentClues[idx++] = count;
            count = 0;
        }
    }
    if (count > 0 && idx < MAX_CLUES) currentClues[idx++] = count;
    LineClues* target = isRow ? &rowClues[index] : &colClues[index];
    if (idx != target->count) return false;
    for (int k = 0; k < idx; k++)
        if (currentClues[k] != target->values[k]) return false;
    return true;
}

void puzzle_update_clue_states(void) {
    for (int r = 0; r < GRID_ROWS; r++) rowDone[r] = puzzle_check_line_match(r, true);
    for (int c = 0; c < GRID_COLS; c++) colDone[c] = puzzle_check_line_match(c, false);
}

void puzzle_shuffle_bag(void) {
    int limit = saveData.unlockedLimit;
    if (limit > PUZZLE_COUNT) limit = PUZZLE_COUNT;
    if (limit < 1) limit = 1;  /* safety: never empty the bag */
    for (int i = 0; i < limit; i++) puzzleBag[i] = sortedPuzzles[i].id;
    for (int i = limit - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = puzzleBag[i];
        puzzleBag[i] = puzzleBag[j];
        puzzleBag[j] = temp;
    }
    bagIndex = 0;
    needShuffle = false;
}

int puzzle_get_next_id(void) {
    int limit = saveData.unlockedLimit;
    if (limit > PUZZLE_COUNT) limit = PUZZLE_COUNT;
    if (limit < 1) limit = 1;
    if (needShuffle || bagIndex >= limit) puzzle_shuffle_bag();
    return puzzleBag[bagIndex++];
}

void puzzle_reset_game(void) {
    for (int i = 0; i < GRID_ROWS; i++)
        for (int j = 0; j < GRID_COLS; j++)
            playerGrid[i][j] = 0;
    gameWon = false;
    cheated = false;
    currentThemeIdx = rand() % NUM_THEMES;
    puzzle_calculate_target_clues();
    puzzle_update_clue_states();
}

void puzzle_check_win(void) {
    if (gameWon) return;
    puzzle_update_clue_states();
    const Puzzle* p = &allPuzzles[currentPuzzleIndex];
    bool match = true;
    int pixelCount = 0;
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (p->grid[r][c] == 1) pixelCount++;
            if ((p->grid[r][c] == 1 && playerGrid[r][c] != 1) ||
                (p->grid[r][c] == 0 && playerGrid[r][c] == 1)) match = false;
        }
    }
    if (!match) return;
    gameWon = true;
    audio_play_sound(3);
    render_trigger_explosion();
    if (!saveData.solved[currentPuzzleIndex] && !cheated) {
        saveData.solved[currentPuzzleIndex] = true;
        saveData.solvedCount++;
        saveData.score += 50 + (pixelCount * 2);
        if (saveData.solvedCount >= (saveData.unlockedLimit - 5)) {
            if (saveData.unlockedLimit < PUZZLE_COUNT) {
                saveData.unlockedLimit += 10;
                needShuffle = true;  /* force re-shuffle on next puzzle_get_next_id() */
            }
        }
        save_save_game();
    }
}

void puzzle_use_hint(void) {
    if (gameWon) return;
    if (saveData.score > 0) { saveData.score--; save_save_game(); }
    const Puzzle* p = &allPuzzles[currentPuzzleIndex];
    int attempts = 0;
    while (attempts < 100) {
        int r = rand() % GRID_ROWS;
        int c = rand() % GRID_COLS;
        if (rand() % 2 == 0) {
            if (!rowDone[r]) {
                for (int i = 0; i < GRID_COLS; i++)
                    playerGrid[r][i] = (p->grid[r][i] == 1) ? 1 : 2;
                audio_play_sound(1);
                puzzle_check_win();
                return;
            }
        } else {
            if (!colDone[c]) {
                for (int i = 0; i < GRID_ROWS; i++)
                    playerGrid[i][c] = (p->grid[i][c] == 1) ? 1 : 2;
                audio_play_sound(1);
                puzzle_check_win();
                return;
            }
        }
        attempts++;
    }
    /* Fallback: primeira linha não resolvida */
    for (int r = 0; r < GRID_ROWS; r++) {
        if (!rowDone[r]) {
            for (int i = 0; i < GRID_COLS; i++)
                playerGrid[r][i] = (p->grid[r][i] == 1) ? 1 : 2;
            audio_play_sound(1);
            puzzle_check_win();
            return;
        }
    }
}

void puzzle_solve_puzzle(void) {
    if (gameWon) return;
    cheated = true;
    const Puzzle* p = &allPuzzles[currentPuzzleIndex];
    for (int r = 0; r < GRID_ROWS; r++)
        for (int c = 0; c < GRID_COLS; c++)
            playerGrid[r][c] = (p->grid[r][c] == 1) ? 1 : 2;
    audio_play_sound(1);
    puzzle_check_win();
}