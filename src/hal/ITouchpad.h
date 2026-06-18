#pragma once
// =============================================================================
// ITouchpad.h — Abstracción mínima del controlador táctil.
//
// DIP: la UI depende de esta interfaz, no de FT6336 directamente.
// ISP: interfaz pequeña y focalizada.
// =============================================================================
#include <stdint.h>
#include <stdbool.h>

class ITouchpad {
public:
    virtual ~ITouchpad() = default;

    // Retorna true si hay un toque activo y rellena x/y con las coordenadas.
    virtual bool read(int16_t& x, int16_t& y) = 0;
};
