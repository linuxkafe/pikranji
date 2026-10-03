#include "save.h"
#include <fat.h>
#include <stdio.h>

void save_init(void) {
    fatReady = fatInitDefault();
    save_load_game();
}

void save_shutdown(void) { /* no-op */ }

void save_save_game(void) {
    if (!fatReady) return;
    saveData.version = 1;  /* always stamp current format version */
    FILE* file = fopen("fat:/pikranji.sav", "wb");
    if (file) {
        fwrite(&saveData, sizeof(SaveData), 1, file);
        fclose(file);
    }
}

void save_load_game(void) {
    /* Defaults */
    saveData.version = 1;
    saveData.score = 0;
    saveData.unlockedLimit = 10;
    saveData.solvedCount = 0;
    saveData.language = LANG_PT;
    for (int i = 0; i < MAX_PUZZLES; i++) saveData.solved[i] = false;

    if (!fatReady) return;
    FILE* file = fopen("fat:/pikranji.sav", "rb");
    if (file) {
        fread(&saveData, sizeof(SaveData), 1, file);
        fclose(file);
        /* Validação de sanidade */
        if (saveData.version < 1) saveData.version = 1;  /* migrate old saves */
        if (saveData.unlockedLimit < 10) saveData.unlockedLimit = 10;
        if (saveData.unlockedLimit > PUZZLE_COUNT) saveData.unlockedLimit = PUZZLE_COUNT;
        if (saveData.language != LANG_PT && saveData.language != LANG_EN) saveData.language = LANG_PT;
    }
}