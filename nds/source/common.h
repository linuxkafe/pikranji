#ifndef COMMON_H
#define COMMON_H

#include <nds.h>
#include <stdbool.h>
#include "puzzles.h"

/* ============================================================================
 * CONSTANTES GLOBAIS
 * ============================================================================ */
#define GRID_ROWS        15
#define GRID_COLS        15
#define CELL_SIZE        10
#define MARGIN_LEFT      60
#define MARGIN_TOP       35
#define MAX_CLUES        8
#define MAX_PUZZLES      1000
#define MAX_PARTICLES    150
#define NUM_THEMES       5
#define NUM_BG_IMAGES    3
#define BG_SWITCH_TIME   (60 * 10)
#define BTN_W            36
#define BTN_H            25
#define BTN_HINT_X       216
#define BTN_HINT_Y       60
#define BTN_SOLVE_X      216
#define BTN_SOLVE_Y      100
#define SAMPLE_RATE      8000

/* ============================================================================
 * TIPOS COMPARTILHADOS
 * ============================================================================ */
typedef struct { int count; int values[MAX_CLUES]; } LineClues;

typedef struct { int id; int complexity; } PuzzleEntry;

typedef struct {
    int score;
    int unlockedLimit;
    int solvedCount;
    bool solved[MAX_PUZZLES];
    int language;
} SaveData;

typedef struct { u16 bg; u16 grid; u16 filled; u16 cursor; u16 clue_txt; u16 clue_done; } ColorTheme;

typedef struct { float x, y, vx, vy; int life; u16 color; bool active; } Particle;

#define LANG_PT 0
#define LANG_EN 1

/* ============================================================================
 * ESTADO GLOBAL (DEFINIDO EM main.c, EXPORTADO PARA MÓDULOS)
 * ============================================================================ */
extern u16* topScreenBuf;
extern u16* bottomScreenBuf;
extern const u8* bgImages[];
extern int currentBgIndex;
extern int bgTimer;
extern ColorTheme themes[];
extern int currentThemeIdx;
extern Particle particles[];
extern bool fireworksActive;
extern LineClues rowClues[];
extern LineClues colClues[];
extern bool rowDone[];
extern bool colDone[];
extern PuzzleEntry sortedPuzzles[];
extern int puzzleBag[];
extern int bagIndex;
extern SaveData saveData;
extern bool fatReady;
extern int currentPuzzleIndex;
extern int playerGrid[GRID_ROWS][GRID_COLS];
extern bool gameWon;
extern bool cheated;
extern bool isDragging;
extern int dragType;

/* ============================================================================
 * CORES FIXAS
 * ============================================================================ */
#define COLOR_MARKER  (RGB15(15, 15, 15) | BIT(15))
#define COLOR_WIN_BG  (RGB15(22, 31, 22) | BIT(15))
#define COLOR_BTN     (RGB15(10, 10, 10) | BIT(15))
#define COLOR_BTN_TXT (RGB15(25, 25, 25) | BIT(15))

/* ============================================================================
 * TEXTOS UI (PT/EN)
 * ============================================================================ */
extern const char* UI_TEXT[2][6];

/* ============================================================================
 * PROTÓTIPOS DE INICIALIZAÇÃO POR MÓDULO
 * ============================================================================ */
void puzzle_init(void);
void render_init(u16* topBuf, u16* botBuf);
void input_init(void);
void audio_init(void);
void save_init(void);

void puzzle_shutdown(void);
void render_shutdown(void);
void input_shutdown(void);
void audio_shutdown(void);
void save_shutdown(void);

#endif /* COMMON_H */