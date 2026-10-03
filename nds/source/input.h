#ifndef INPUT_H
#define INPUT_H

#include "common.h"

/**
 * @brief Inicializa o subsistema de entrada (teclado + touch).
 */
void input_init(void);

/**
 * @brief Libera recursos de entrada (no-op).
 */
void input_shutdown(void);

/**
 * @brief Processa entrada do frame atual.
 * @param[out] cursorR Ponteiro para linha do cursor (-1 se fora do grid).
 * @param[out] cursorC Ponteiro para coluna do cursor.
 * @param[out] forceRender Ponteiro para flag de redraw forçado.
 * @note Lê keysDown/keysHeld/touch, atualiza estado de drag, trata botões.
 */
void input_process(int* cursorR, int* cursorC, bool* forceRender);

/**
 * @brief Verifica se o toque está dentro dos botões de Dica/Resolver.
 * @param touch Coordenadas de toque.
 * @return 0 = nenhum, 1 = dica, 2 = resolver, -1 = grid.
 */
int input_check_buttons(touchPosition touch);

#endif /* INPUT_H */