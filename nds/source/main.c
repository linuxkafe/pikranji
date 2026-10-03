#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"
#include "puzzle.h"
#include "render.h"
#include "input.h"
#include "audio.h"
#include "save.h"

/* ============================================================================
 * DEFINIÇÕES DE VARIÁVEIS GLOBAIS (instâncias únicas)
 * ============================================================================ */
u16* topScreenBuf = NULL;
u16* bottomScreenBuf = NULL;

const u8* bgImages[NUM_BG_IMAGES];
int currentBgIndex = 0;
int bgTimer = 0;

ColorTheme themes[NUM_THEMES] = {
    { (RGB15(28, 29, 31) | BIT(15)), (RGB15(22, 23, 25) | BIT(15)), (RGB15(31, 10, 5) | BIT(15)), (RGB15(0, 25, 31) | BIT(15)), (RGB15(5, 8, 12) | BIT(15)), (RGB15(20, 20, 20) | BIT(15)) },
    { (RGB15(26, 30, 26) | BIT(15)), (RGB15(20, 24, 20) | BIT(15)), (RGB15(10, 25, 10) | BIT(15)), (RGB15(31, 20, 10) | BIT(15)), (RGB15(5, 15, 5) | BIT(15)), (RGB15(18, 22, 18) | BIT(15)) },
    { (RGB15(25, 28, 30) | BIT(15)), (RGB15(20, 23, 25) | BIT(15)), (RGB15(5, 20, 31) | BIT(15)), (RGB15(31, 25, 10) | BIT(15)), (RGB15(0, 10, 20) | BIT(15)), (RGB15(18, 20, 22) | BIT(15)) },
    { (RGB15(30, 28, 26) | BIT(15)), (RGB15(25, 23, 21) | BIT(15)), (RGB15(25, 12, 5) | BIT(15)), (RGB15(5, 20, 15) | BIT(15)), (RGB15(15, 8, 5) | BIT(15)), (RGB15(24, 22, 20) | BIT(15)) },
    { (RGB15(29, 27, 30) | BIT(15)), (RGB15(24, 22, 25) | BIT(15)), (RGB15(20, 10, 25) | BIT(15)), (RGB15(10, 31, 20) | BIT(15)), (RGB15(12, 5, 15) | BIT(15)), (RGB15(22, 20, 22) | BIT(15)) }
};
int currentThemeIdx = 0;

Particle particles[MAX_PARTICLES];
bool fireworksActive = false;

LineClues rowClues[GRID_ROWS];
LineClues colClues[GRID_COLS];
bool rowDone[GRID_ROWS];
bool colDone[GRID_COLS];

PuzzleEntry sortedPuzzles[MAX_PUZZLES];
int puzzleBag[MAX_PUZZLES];
int bagIndex = 0;

SaveData saveData;
bool fatReady = false;
bool needShuffle = false;

int currentPuzzleIndex = 0;
int playerGrid[GRID_ROWS][GRID_COLS];
bool gameWon = false;
bool cheated = false;
bool isDragging = false;
int dragType = 0;

const char* UI_TEXT[2][6] = {
    {"Score", "Desbloq", "RESOLVIDO", "BATOTA", "Significado", "COMPLETO!"},
    {"Score", "Unlock",  "SOLVED",    "CHEATED", "Meaning",     "COMPLETE!"}
};

/* Dados binários (assembly injection) */
extern const u8 music_bin[];
extern const u32 music_bin_size;
extern const u8 sea1_bin[];
extern const u8 sea2_bin[];
extern const u8 sea3_bin[];

/* ============================================================================
 * MAIN
 * ============================================================================ */
int main(void) {
    /* --- IRQ & Áudio --- */
    irqEnable(IRQ_VBLANK);
    audio_init();

    /* --- VÍDEO & VRAM --- */
    /* Top screen: BG3 em VRAM B (bitmap 16-bit) */
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankB(VRAM_B_MAIN_BG_0x06020000);
    int bgTop = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 8, 0);
    topScreenBuf = bgGetGfxPtr(bgTop);

    /* Console de texto no ecrã superior (BG0 em VRAM A) */
    consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);

    /* Bottom screen: BG3 em VRAM C (bitmap 16-bit) */
    videoSetModeSub(MODE_5_2D);
    vramSetBankC(VRAM_C_SUB_BG);
    int bgBot = bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    bottomScreenBuf = bgGetGfxPtr(bgBot);

    /* --- INICIALIZAÇÃO DOS MÓDULOS --- */
    bgImages[0] = sea1_bin; bgImages[1] = sea2_bin; bgImages[2] = sea3_bin;
    puzzle_init();
    render_init(topScreenBuf, bottomScreenBuf);
    input_init();
    save_init();

    srand(time(NULL));
    puzzle_shuffle_bag();
    currentPuzzleIndex = puzzle_get_next_id();
    puzzle_reset_game();
    render_refresh_static_text(saveData.language);

    int lastR = -1, lastC = -1;
    bool forceRender = true;

    /* ========================================================================
     * GAME LOOP
     * ======================================================================== */
    while (1) {
        swiIntrWait(1, IRQ_VBLANK);

        /* Background rotation (top screen) */
        bgTimer++;
        if (bgTimer >= BG_SWITCH_TIME) {
            bgTimer = 0;
            render_switch_background();
        }

        audio_update();

        int cursorR, cursorC;
        input_process(&cursorR, &cursorC, &forceRender);

        if (forceRender || cursorR != lastR || cursorC != lastC || fireworksActive) {
            const Puzzle* p = &allPuzzles[currentPuzzleIndex];
            int L = saveData.language;

            /* HUD no console (top screen) */
            iprintf("\x1b[2;2H-- PIKRANJI DS --");
            iprintf("\x1b[2;20H%s: %d     ", UI_TEXT[L][0], saveData.score);
            iprintf("\x1b[14;2H%s: %d/%d   ", UI_TEXT[L][1], saveData.unlockedLimit, PUZZLE_COUNT);
            if (fatReady && saveData.solved[currentPuzzleIndex])
                iprintf("\x1b[14;20H\x1b[33m[%s]\x1b[39m", UI_TEXT[L][2]);
            else if (cheated)
                iprintf("\x1b[14;20H\x1b[31m[%s]   \x1b[39m", UI_TEXT[L][3]);
            else
                iprintf("\x1b[14;20H           ");

            iprintf("\x1b[16;2H%s:                                ", UI_TEXT[L][4]);
            const char* meaningStr = (L == LANG_EN) ? p->meaning_en : p->meaning_pt;
            iprintf("\x1b[16;2H%s: \x1b[32m%s\x1b[39m", UI_TEXT[L][4], meaningStr);

            if (gameWon)
                iprintf("\x1b[18;2H** %s ** ", UI_TEXT[L][5]);
            else
                iprintf("\x1b[18;2H                  ");

            render_render_game(cursorR, cursorC);
            if (fireworksActive) render_update_and_draw_fireworks();

            lastR = cursorR; lastC = cursorC; forceRender = false;
        }
    }

    return 0;
}