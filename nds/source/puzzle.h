#ifndef PUZZLE_H
#define PUZZLE_H

#include "common.h"

/**
 * @brief Inicializa o sistema de puzzles: calcula complexidade, ordena e prepara o saco.
 * @note Deve ser chamado uma vez no início do jogo, após carregar saveData.
 */
void puzzle_init(void);

/**
 * @brief Libera recursos do módulo puzzle (no-op em NDS, mantido para simetria).
 */
void puzzle_shutdown(void);

/**
 * @brief Calcula as pistas (clues) alvo para o puzzle atual.
 * Preenche rowClues[] e colClues[] baseados em allPuzzles[currentPuzzleIndex].
 */
void puzzle_calculate_target_clues(void);

/**
 * @brief Verifica se uma linha (row ou coluna) corresponde às pistas alvo.
 * @param index Índice da linha/coluna (0-14).
 * @param isRow true para linha, false para coluna.
 * @return true se a linha do jogador corresponde exatamente às pistas.
 */
bool puzzle_check_line_match(int index, bool isRow);

/**
 * @brief Atualiza estados rowDone[] e colDone[] para todas as linhas.
 */
void puzzle_update_clue_states(void);

/**
 * @brief Embaralha o saco de puzzles desbloqueados (Fisher-Yates).
 * @note Usa saveData.unlockedLimit como limite superior.
 */
void puzzle_shuffle_bag(void);

/**
 * @brief Obtém o próximo ID de puzzle do saco; reembaralha se vazio.
 * @return Índice em allPuzzles[] do próximo puzzle.
 */
int puzzle_get_next_id(void);

/**
 * @brief Reseta o estado do jogo atual (grid do jogador, flags, tema aleatório).
 * @note Não muda currentPuzzleIndex.
 */
void puzzle_reset_game(void);

/**
 * @brief Verifica condição de vitória e atualiza saveData se vencido.
 * @note Dispara fogos de artifício, toca som de vitória, salva progresso.
 */
void puzzle_check_win(void);

/**
 * @brief Usa uma dica: revela uma linha/coluna não resolvida (custa 1 ponto).
 * @note Se score == 0, não gasta ponto mas ainda revela.
 */
void puzzle_use_hint(void);

/**
 * @brief Resolve o puzzle atual instantaneamente (marca como batota).
 */
void puzzle_solve_puzzle(void);

#endif /* PUZZLE_H */