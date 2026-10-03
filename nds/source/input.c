#include "input.h"

void input_init(void) {
    /* Nada a inicializar além do que main.c já faz (irqEnable, etc.) */
}

void input_shutdown(void) { /* no-op */ }

static int check_button_hit(touchPosition touch) {
    if (touch.px >= BTN_HINT_X && touch.px <= BTN_HINT_X + BTN_W &&
        touch.py >= BTN_HINT_Y && touch.py <= BTN_HINT_Y + BTN_H) return 1; /* Dica */
    if (touch.px >= BTN_SOLVE_X && touch.px <= BTN_SOLVE_X + BTN_W &&
        touch.py >= BTN_SOLVE_Y && touch.py <= BTN_SOLVE_Y + BTN_H) return 2; /* Resolver */
    return 0;
}

int input_check_buttons(touchPosition touch) {
    return check_button_hit(touch);
}

void input_process(int* cursorR, int* cursorC, bool* forceRender) {
    scanKeys();
    int held = keysHeld();
    int down = keysDown();

    /* Teclas de sistema */
    if (down & KEY_START) { puzzle_reset_game(); *forceRender = true; }
    if (down & KEY_SELECT) {
        currentPuzzleIndex = puzzle_get_next_id();
        puzzle_reset_game();
        *forceRender = true;
    }
    if (down & KEY_X) {
        saveData.language = (saveData.language == LANG_PT) ? LANG_EN : LANG_PT;
        save_save_game();
        render_refresh_static_text(saveData.language);
        *forceRender = true;
    }

    *cursorR = -1; *cursorC = -1;

    if (down & KEY_TOUCH) {
        touchPosition touch; touchRead(&touch);

        /* Botões UI */
        if (!gameWon) {
            int btn = check_button_hit(touch);
            if (btn == 1) { puzzle_use_hint(); *forceRender = true; }
            else if (btn == 2) { puzzle_solve_puzzle(); *forceRender = true; }
        }

        /* Grid */
        int c = (touch.px - MARGIN_LEFT) / CELL_SIZE;
        int r = (touch.py - MARGIN_TOP) / CELL_SIZE;
        if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS) {
            *cursorR = r; *cursorC = c;
            if (!gameWon) {
                isDragging = true;
                if (held & KEY_UP)      dragType = (playerGrid[r][c] == 2) ? 2 : 3;
                else if (held & KEY_DOWN) dragType = (playerGrid[r][c] == 1) ? 2 : 1;
                else                    dragType = 0;
            }
        }
    }
    else if (held & KEY_TOUCH && isDragging) {
        touchPosition touch; touchRead(&touch);
        int c = (touch.px - MARGIN_LEFT) / CELL_SIZE;
        int r = (touch.py - MARGIN_TOP) / CELL_SIZE;
        if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS) {
            *cursorR = r; *cursorC = c;
            if (dragType != 0) {
                int newState = -1;
                if (dragType == 1) newState = 1;      /* Preencher */
                else if (dragType == 2) newState = 0; /* Apagar */
                else if (dragType == 3) newState = 2; /* Marcar X */
                if (playerGrid[r][c] != newState) {
                    playerGrid[r][c] = newState;
                    if (newState == 1) audio_play_sound(0);
                    else if (newState == 2) audio_play_sound(1);
                    else if (newState == 0) audio_play_sound(2);
                    puzzle_check_win();
                    *forceRender = true;
                }
            }
        }
    }
    else {
        isDragging = false;
        dragType = 0;
    }
}