#include "render.h"
#include <stdio.h>

/* Fonte minúscula 3x5 para números 0-9 (cada coluna = 5 bits, LSB = topo) */
static const u8 miniFont[10][3] = {
    {0x1F, 0x11, 0x1F},  /* 0 */
    {0x00, 0x1F, 0x00},  /* 1 */
    {0x1D, 0x15, 0x17},  /* 2 */
    {0x15, 0x15, 0x1F},  /* 3 */
    {0x07, 0x04, 0x1F},  /* 4 */
    {0x17, 0x15, 0x1D},  /* 5 */
    {0x1F, 0x15, 0x1D},  /* 6 */
    {0x01, 0x01, 0x1F},  /* 7 */
    {0x1F, 0x15, 0x1F},  /* 8 */
    {0x17, 0x15, 0x1F}   /* 9 */
};

void render_init(u16* topBuf, u16* botBuf) {
    topScreenBuf = topBuf;
    bottomScreenBuf = botBuf;
    /* Primeira imagem de fundo */
    dmaCopy(bgImages[0], topScreenBuf, 256 * 192 * 2);
}

void render_shutdown(void) { /* no-op */ }

void render_switch_background(void) {
    currentBgIndex = (currentBgIndex + 1) % NUM_BG_IMAGES;
    dmaCopy(bgImages[currentBgIndex], topScreenBuf, 256 * 192 * 2);
}

void render_plot(int x, int y, u16 color) {
    if (x >= 0 && x < 256 && y >= 0 && y < 192)
        bottomScreenBuf[y * 256 + x] = color;
}

void render_draw_rect(int x, int y, int w, int h, u16 color) {
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            render_plot(x + j, y + i, color);
}

void render_draw_mini_num(int x, int y, int num, u16 color) {
    if (num > 9) {
        render_draw_mini_num(x - 4, y, num / 10, color);
        num %= 10;
    }
    for (int col = 0; col < 3; col++) {
        u8 colData = miniFont[num][col];
        for (int row = 0; row < 5; row++)
            if ((colData >> row) & 1)
                render_plot(x + col, y + row, color);
    }
}

void render_draw_buttons(void) {
    /* Botão Dica (?) */
    render_draw_rect(BTN_HINT_X, BTN_HINT_Y, BTN_W, BTN_H, COLOR_BTN);
    render_draw_rect(BTN_HINT_X + 16, BTN_HINT_Y + 6, 4, 2, COLOR_BTN_TXT);
    render_draw_rect(BTN_HINT_X + 20, BTN_HINT_Y + 6, 2, 4, COLOR_BTN_TXT);
    render_draw_rect(BTN_HINT_X + 18, BTN_HINT_Y + 10, 2, 2, COLOR_BTN_TXT);
    render_draw_rect(BTN_HINT_X + 18, BTN_HINT_Y + 14, 2, 2, COLOR_BTN_TXT);
    /* Botão Resolver (!) */
    render_draw_rect(BTN_SOLVE_X, BTN_SOLVE_Y, BTN_W, BTN_H, COLOR_BTN);
    render_draw_rect(BTN_SOLVE_X + 17, BTN_SOLVE_Y + 6, 2, 7, COLOR_BTN_TXT);
    render_draw_rect(BTN_SOLVE_X + 17, BTN_SOLVE_Y + 14, 2, 2, COLOR_BTN_TXT);
}

static void draw_x(int x, int y, int s, u16 c) {
    for (int i = 0; i < s; i++) {
        render_plot(x + i, y + i, c);
        render_plot(x + s - 1 - i, y + i, c);
    }
}

void render_draw_x_marker(int x, int y) {
    draw_x(x + 2, y + 2, CELL_SIZE - 5, COLOR_MARKER);
}

void render_draw_cursor(int r, int c, u16 color) {
    int x = MARGIN_LEFT + (c * CELL_SIZE);
    int y = MARGIN_TOP + (r * CELL_SIZE);
    render_draw_rect(x, y, CELL_SIZE, 1, color);
    render_draw_rect(x, y + CELL_SIZE - 1, CELL_SIZE, 1, color);
    render_draw_rect(x, y, 1, CELL_SIZE, color);
    render_draw_rect(x + CELL_SIZE - 1, y, 1, CELL_SIZE, color);
}

void render_render_game(int cursorR, int cursorC) {
    ColorTheme* t = &themes[currentThemeIdx];
    u16 bg = gameWon ? COLOR_WIN_BG : t->bg;

    /* Limpa framebuffer inferior */
    for (int i = 0; i < 256 * 192; i++) bottomScreenBuf[i] = bg;

    /* Pistas das colunas (topo) */
    for (int c = 0; c < GRID_COLS; c++) {
        u16 txtColor = colDone[c] ? t->clue_done : t->clue_txt;
        int count = colClues[c].count;
        int x = MARGIN_LEFT + (c * CELL_SIZE) + 3;
        int yBase = MARGIN_TOP - 2;
        for (int i = count - 1; i >= 0; i--)
            render_draw_mini_num(x, yBase - ((count - 1 - i) * 7) - 6,
                                 colClues[c].values[i], txtColor);
    }

    /* Pistas das linhas (esquerda) */
    for (int r = 0; r < GRID_ROWS; r++) {
        u16 txtColor = rowDone[r] ? t->clue_done : t->clue_txt;
        int count = rowClues[r].count;
        int y = MARGIN_TOP + (r * CELL_SIZE) + 3;
        int xBase = MARGIN_LEFT - 2;
        for (int i = count - 1; i >= 0; i--) {
            int val = rowClues[r].values[i];
            int offset = (val > 9) ? 8 : 4;
            render_draw_mini_num(xBase - offset, y, val, txtColor);
            xBase -= (offset + 3);
        }
    }

    /* Grid de células */
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            int px = MARGIN_LEFT + (c * CELL_SIZE);
            int py = MARGIN_TOP + (r * CELL_SIZE);
            u16 color = t->bg;
            if (playerGrid[r][c] == 1) color = t->filled;
            render_draw_rect(px, py, CELL_SIZE - 1, CELL_SIZE - 1, color);
            render_draw_rect(px + CELL_SIZE - 1, py, 1, CELL_SIZE, t->grid);
            render_draw_rect(px, py + CELL_SIZE - 1, CELL_SIZE, 1, t->grid);
            if (playerGrid[r][c] == 2) render_draw_x_marker(px, py);
        }
    }

    /* Moldura do grid */
    render_draw_rect(MARGIN_LEFT - 1, MARGIN_TOP - 1, 1, (GRID_ROWS * CELL_SIZE) + 1, t->clue_txt);
    render_draw_rect(MARGIN_LEFT - 1, MARGIN_TOP - 1, (GRID_COLS * CELL_SIZE) + 1, 1, t->clue_txt);

    /* Cursor e botões */
    if (!gameWon && cursorR >= 0) render_draw_cursor(cursorR, cursorC, t->cursor);
    if (!gameWon) render_draw_buttons();
}

void render_trigger_explosion(void) {
    fireworksActive = true;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = true;
        particles[i].x = 128;
        particles[i].y = 80;
        particles[i].life = 60 + (rand() % 60);
        int r = rand() % 31;
        int g = rand() % 31;
        int b = rand() % 31;
        particles[i].color = RGB15(r, g, b) | BIT(15);
        particles[i].vx = ((rand() % 100) / 20.0f) - 2.5f;
        particles[i].vy = ((rand() % 100) / 20.0f) - 3.5f;
    }
}

void render_update_and_draw_fireworks(void) {
    int activeCount = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) continue;
        activeCount++;
        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        particles[i].vy += 0.08f;
        particles[i].life--;
        if (particles[i].life <= 0 || particles[i].y > 192 ||
            particles[i].x < 0 || particles[i].x > 256) {
            particles[i].active = false;
            continue;
        }
        int px = (int)particles[i].x;
        int py = (int)particles[i].y;
        render_plot(px, py, particles[i].color);
        render_plot(px + 1, py, particles[i].color);
        render_plot(px, py + 1, particles[i].color);
        render_plot(px + 1, py + 1, particles[i].color);
    }
    if (activeCount == 0) fireworksActive = false;
}

void render_refresh_static_text(int lang) {
    iprintf("\x1b[2J");
    if (lang == LANG_PT) {
        iprintf("\x1b[21;1H[?] Ajuda (Custo 1)");
        iprintf("\x1b[21;20H[!] Resolver");
        iprintf("\x1b[22;1H[X] Idioma: PT");
    } else {
        iprintf("\x1b[21;1H[?] Hint (Cost 1)");
        iprintf("\x1b[21;20H[!] Solve");
        iprintf("\x1b[22;1H[X] Lang: EN  ");
    }
}