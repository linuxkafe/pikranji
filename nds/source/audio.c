#include "audio.h"

/* Dados da música (assembly injection) */
extern const u8 music_bin[];
extern const u32 music_bin_size;

#define SAMPLE_RATE 8000
#define DATA_FORMAT SoundFormat_8Bit

static int soundChannel = -1;
static int soundTimer = 0;

void audio_init(void) {
    soundEnable();
    /* Música de fundo em loop no canal 0 (streaming) */
    soundPlaySample(music_bin, DATA_FORMAT, music_bin_size, SAMPLE_RATE, 80, 64, true, 0);
}

void audio_shutdown(void) {
    if (soundChannel != -1) { soundKill(soundChannel); soundChannel = -1; }
    soundTimer = 0;
}

void audio_update(void) {
    if (soundTimer > 0) {
        soundTimer--;
        if (soundTimer == 0 && soundChannel != -1) {
            soundKill(soundChannel);
            soundChannel = -1;
        }
    }
}

void audio_play_sound(int type) {
    if (soundChannel != -1) soundKill(soundChannel);
    switch (type) {
        case 0: /* Tap preencher */
            soundChannel = soundPlayPSG(DutyCycle_50, 400, 60, 64);
            soundTimer = 4;
            break;
        case 1: /* Tap marcar X */
            soundChannel = soundPlayPSG(DutyCycle_12, 2000, 50, 64);
            soundTimer = 3;
            break;
        case 2: /* Apagar */
            soundChannel = soundPlayNoise(1500, 40, 64);
            soundTimer = 5;
            break;
        case 3: /* Vitória */
            soundChannel = soundPlayPSG(DutyCycle_50, 880, 80, 64);
            soundTimer = 30;
            break;
        case 4: /* Erro/negado */
            soundChannel = soundPlayPSG(DutyCycle_25, 150, 60, 64);
            soundTimer = 10;
            break;
        default:
            soundChannel = -1;
            soundTimer = 0;
            break;
    }
}