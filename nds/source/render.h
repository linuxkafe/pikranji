#ifndef RENDER_H
#define RENDER_H

#include "common.h"

/**
 * @brief Inicializa o módulo de renderização com ponteiros dos framebuffers.
 * @param topBuf Ponteiro para VRAM do ecrã superior (256x192, 16-bit).
 * @param botBuf Ponteiro para VRAM do ecrã inferior (256x192, 16-bit).
 */
void render_init(u16* topBuf, u16* botBuf);

/**
 * @brief Libera recursos de renderização (no-op em NDS).
 */
void render_shutdown(void);

/**
 * @brief Troca a imagem de fundo do ecrã superior (cicla bgImages[]).
 */
void render_switch_background(void);

/**
 * @brief Desenha um pixel único no ecrã inferior.
 * @param x Coordenada X (0-255).
 * @param y Coordenada Y (0-191).
 * @param color Cor RGB15 com BIT(15) para alpha.
 */
void render_plot(int x, int y, u16 color);

/**
 * @brief Desenha retângulo preenchido no ecrã inferior.
 */
void render_draw_rect(int x, int y, int w, int h, u16 color);

/**
 * @brief Desenha número minúsculo (3x5 pixels) usando miniFont.
 * @param x, y Posição do canto superior esquerdo.
 * @param num Valor 0-99 (recursivo para 2 dígitos).
 * @param color Cor do número.
 */
void render_draw_mini_num(int x, int y, int num, u16 color);

/**
 * @brief Desenha botões de Dica e Resolver no ecrã inferior.
 */
void render_draw_buttons(void);

/**
 * @brief Desenha marcador 'X' numa célula (estado 2 = marcado vazio).
 * @param x, y Posição superior-esquerda da célula em pixels.
 */
void render_draw_x_marker(int x, int y);

/**
 * @brief Desenha cursor ao redor da célula (r, c).
 * @param r Linha 0-14, c Coluna 0-14.
 * @param color Cor do cursor (tema atual).
 */
void render_draw_cursor(int r, int c, u16 color);

/**
 * @brief Renderiza o jogo completo no ecrã inferior.
 * @param cursorR Linha do cursor (-1 se nenhum), cursorC Coluna do cursor.
 * @note Limpa o framebuffer, desenha grid, pistas, células, cursor, botões.
 */
void render_render_game(int cursorR, int cursorC);

/**
 * @brief Inicia animação de fogos de artifício no centro do ecrã inferior.
 */
void render_trigger_explosion(void);

/**
 * @brief Atualiza e desenha partículas de fogos de artifício.
 * @note Deve ser chamado a cada frame enquanto fireworksActive == true.
 */
void render_update_and_draw_fireworks(void);

/**
 * @brief Atualiza textos estáticos no ecrã superior (console).
 * @param lang LANG_PT ou LANG_EN.
 */
void render_refresh_static_text(int lang);

#endif /* RENDER_H */