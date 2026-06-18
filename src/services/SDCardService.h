#pragma once
// =============================================================================
// SDCardService.h — Montaje y operaciones básicas sobre la tarjeta SD.
//
// SRP: única responsabilidad — I/O de archivos SD (montaje + listado).
//      No sabe de UI, audio, ni red.
// DIP: retorna datos crudos; la capa superior decide qué hacer con ellos.
// =============================================================================
#include <stdint.h>
#include <stdbool.h>

class SDCardService {
public:
    // ── Montaje ───────────────────────────────────────────────────────────
    // Intenta montar la SD con reintentos. Retorna true si tiene éxito.
    bool begin(int maxRetries = 3);

    // true si la SD está montada
    bool isMounted() const { return _mounted; }

    // ── Prueba de listado ─────────────────────────────────────────────────
    // Lista el directorio raíz por el monitor serial.
    void listRootDirectory();

    // ── Info ──────────────────────────────────────────────────────────────
    // Imprime capacidad total y usada por serial.
    void printCardInfo();

private:
    bool _mounted = false;

    // Pines SD_MMC de la placa (4-bit bus)
    static constexpr int kSckPin = 38;
    static constexpr int kCmdPin = 40;
    static constexpr int kD0Pin  = 39;
    static constexpr int kD1Pin  = 41;
    static constexpr int kD2Pin  = 48;
    static constexpr int kD3Pin  = 47;
};
