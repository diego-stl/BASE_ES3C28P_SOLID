// =============================================================================
// AudioService.cpp
// =============================================================================
#include "AudioService.h"
#include "es8311.h"
#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>

static constexpr float kPi = 3.14159265358979f;

bool AudioService::init(uint8_t ampEnablePin,
                        uint8_t bck, uint8_t ws, uint8_t dout,
                        uint8_t mck, uint32_t sampleRate) {
    _ampPin = ampEnablePin;

    // 1. Amplificador (activo en LOW)
    pinMode(_ampPin, OUTPUT);
    digitalWrite(_ampPin, LOW);
    Serial.println("[Audio] Amplificador activado (LOW)");

    // 2. Codec ES8311 vía I2C (comparte bus con el táctil — Wire ya iniciado)
    const es8311_clock_config_t clk = {
        .mclk_inverted      = false,
        .sclk_inverted      = false,
        .mclk_from_mclk_pin = true,
        .mclk_frequency     = (int)(sampleRate * 256),
        .sample_frequency   = (int)sampleRate
    };
    esp_err_t err = es8311_init(nullptr, &clk, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16);
    if (err != ESP_OK) {
        Serial.printf("[Audio] ERROR codec ES8311: 0x%X\n", err);
        return false;
    }
    es8311_voice_volume_set(nullptr, 70, nullptr);
    es8311_microphone_config(nullptr, false);  // micrófono desactivado
    Serial.println("[Audio] Codec ES8311 configurado correctamente");

    // 3. Bus I2S
    i2s_config_t i2s_cfg = {
        .mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate          = sampleRate,
        .bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count        = 8,
        .dma_buf_len          = 256,
        .use_apll             = false,
        .tx_desc_auto_clear   = true,
        .fixed_mclk           = 0
    };
    i2s_pin_config_t pin_cfg = {
        .mck_io_num   = (int)mck,
        .bck_io_num   = (int)bck,
        .ws_io_num    = (int)ws,
        .data_out_num = (int)dout,
        .data_in_num  = I2S_PIN_NO_CHANGE
    };

    esp_err_t i2s_err = i2s_driver_install((i2s_port_t)kI2SPort, &i2s_cfg, 0, nullptr);
    if (i2s_err != ESP_OK) {
        Serial.printf("[Audio] ERROR I2S driver: 0x%X\n", i2s_err);
        return false;
    }
    i2s_set_pin((i2s_port_t)kI2SPort, &pin_cfg);
    i2s_zero_dma_buffer((i2s_port_t)kI2SPort);

    Serial.printf("[Audio] I2S iniciado — BCK=%d WS=%d DOUT=%d MCK=%d SR=%lu Hz\n",
                  bck, ws, dout, mck, (unsigned long)sampleRate);
    _ready = true;
    return true;
}

void AudioService::setVolume(uint8_t percent) {
    _volumePct = percent > 100 ? 100 : percent;
    // Mapear 0-100% al rango 0-100 del ES8311
    es8311_voice_volume_set(nullptr, (int)_volumePct, nullptr);
    Serial.printf("[Audio] Volumen: %d%%\n", _volumePct);
}

void AudioService::playTestTone(uint16_t freqHz, uint16_t durationMs) {
    if (!_ready) { Serial.println("[Audio] No iniciado, no se puede reproducir tono"); return; }

    const uint32_t sampleRate  = 44100;
    const uint32_t totalSamples = (sampleRate * durationMs) / 1000;
    // Amplitud: 30% de 32767 para no saturar los parlantes
    const int16_t amplitude = (int16_t)(32767 * 0.30f);

    Serial.printf("[Audio] Tono %d Hz durante %d ms\n", freqHz, durationMs);

    // Buffer de trabajo: 256 frames estéreo (256 * 2 * 2 = 1024 bytes)
    const int kBufFrames = 256;
    int16_t buf[kBufFrames * 2]; // L + R por cada frame

    uint32_t samplesWritten = 0;
    while (samplesWritten < totalSamples) {
        uint32_t chunkFrames = kBufFrames;
        if (samplesWritten + chunkFrames > totalSamples) {
            chunkFrames = totalSamples - samplesWritten;
        }
        for (uint32_t i = 0; i < chunkFrames; i++) {
            float t   = (float)(samplesWritten + i) / (float)sampleRate;
            int16_t s = (int16_t)(amplitude * sinf(2.0f * kPi * freqHz * t));
            buf[i * 2 + 0] = s;  // L
            buf[i * 2 + 1] = s;  // R
        }
        size_t bytesWritten = 0;
        i2s_write((i2s_port_t)kI2SPort, buf, chunkFrames * 4, &bytesWritten, portMAX_DELAY);
        samplesWritten += chunkFrames;
    }
    // Silencio al final
    i2s_zero_dma_buffer((i2s_port_t)kI2SPort);
    Serial.println("[Audio] Tono completado");
}
