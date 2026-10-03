#ifndef SAVE_H
#define SAVE_H

#include "common.h"

/**
 * @brief Inicializa o sistema de save (FAT) e carrega saveData do cartão.
 * @note Define fatReady = true se FAT inicializado com sucesso.
 */
void save_init(void);

/**
 * @brief Libera recursos de save (no-op).
 */
void save_shutdown(void);

/**
 * @brief Salva saveData atual no arquivo pikranji.sav no FAT.
 * @note No-op se fatReady == false.
 */
void save_save_game(void);

/**
 * @brief Carrega saveData do arquivo pikranji.sav.
 * @note Se arquivo não existe ou FAT falha, usa defaults.
 * @note Valida limites: unlockedLimit >= 10, <= PUZZLE_COUNT.
 */
void save_load_game(void);

#endif /* SAVE_H */