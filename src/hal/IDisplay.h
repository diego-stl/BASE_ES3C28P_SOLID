#pragma once
// =============================================================================
// IDisplay.h — Abstracción mínima del display físico.
//
// OCP / DIP: el resto del código depende de esta interfaz, no de TFT_eSPI.
//            Permite cambiar de driver sin tocar la lógica superior.
// ISP: expone solo lo que los consumidores necesitan.
// =============================================================================
#include <stdint.h>

class IDisplay {
public:
    virtual ~IDisplay() = default;

    // ── Inicialización ────────────────────────────────────────────────────
    // brightnessPercent: nivel inicial de backlight 0–100
    virtual void init(uint8_t brightnessPercent = 70)             = 0;

    // ── Control de brillo (0–100%) ────────────────────────────────────────
    virtual void    setBrightness(uint8_t percent)                = 0;
    virtual uint8_t getBrightness()                         const = 0;

    // ── Dimensiones (retornan valores fijos de la resolución configurada) ──
    virtual uint16_t width()                                      = 0;
    virtual uint16_t height()                                     = 0;
};
