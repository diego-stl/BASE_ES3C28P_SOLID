#pragma once
// =============================================================================
// AudioService.h — Inicialización del codec ES8311 y prueba de tono I2S.
//
// SRP: única responsabilidad — audio hardware (codec + I2S).
//      No sabe de UI, SD, ni red.
// DIP: recibe pines vía init(), no los hardcodea internamente.
// =============================================================================
#include <stdint.h>
#include <stdbool.h>

class AudioService {
public:
    // ── Inicialización ────────────────────────────────────────────────────
    // Configura el amplificador, el codec ES8311 y el bus I2S.
    // ampEnablePin: GPIO que activa el amplificador (activo en LOW).
    bool init(uint8_t ampEnablePin,
              uint8_t bck, uint8_t ws, uint8_t dout,
              uint8_t mck, uint32_t sampleRate = 44100);

    bool isReady() const { return _ready; }

    // ── Prueba de tono ────────────────────────────────────────────────────
    // Genera un tono sinusoidal de 1 kHz durante durationMs milisegundos
    // directamente por I2S (sin librería audioI2S).
    void playTestTone(uint16_t freqHz = 1000, uint16_t durationMs = 500);

    // ── Volumen ───────────────────────────────────────────────────────────
    void setVolume(uint8_t percent);   // 0–100%

private:
    bool    _ready       = false;
    uint8_t _ampPin      = 0;
    uint8_t _volumePct   = 70;

    // I2S port index
    static constexpr int kI2SPort = 0;
};
