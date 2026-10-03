#ifndef AUDIO_H
#define AUDIO_H

#include "common.h"

/**
 * @brief Inicializa o subsistema de áudio (PSG + sample de música).
 * @note Inicia reprodução da música de fundo em loop.
 */
void audio_init(void);

/**
 * @brief Libera recursos de áudio (para música e SFX).
 */
void audio_shutdown(void);

/**
 * @brief Atualiza temporizadores de SFX (chamado a cada frame).
 * @note Para canais PSG/Noise após duração programada.
 */
void audio_update(void);

/**
 * @brief Toca um efeito sonoro pré-definido.
 * @param type Tipo de som:
 *   0 = tap preencher (PSG 400Hz)
 *   1 = tap marcar X (PSG 2000Hz)
 *   2 = apagar (Noise)
 *   3 = vitória (PSG 880Hz longo)
 *   4 = erro/negado (PSG 150Hz)
 */
void audio_play_sound(int type);

#endif /* AUDIO_H */