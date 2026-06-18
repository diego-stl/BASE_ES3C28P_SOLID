#pragma once
// =============================================================================
// TestScreen.h — Pantalla LVGL de pruebas de hardware.
//
// SRP: única responsabilidad — construir y gestionar la UI de pruebas.
//      No toca hardware directamente; llama a IDisplay para el brillo.
// DIP: depende de IDisplay (abstracción), no de DisplayManager concreto.
// =============================================================================
#include "hal/IDisplay.h"
#include <lvgl.h>

class TestScreen {
public:
    // DIP: recibe la abstracción del display
    explicit TestScreen(IDisplay& display);

    // Construye todos los widgets LVGL. Llamar tras lv_init().
    void build();

    // Llamar desde loop() para actualizar coordenadas del canvas táctil.
    void tick();

private:
    IDisplay& _display;

    // Widgets que necesitan actualización en tick()
    lv_obj_t* _touchLabel  = nullptr;   // Label con coordenadas X,Y
    lv_obj_t* _touchCanvas = nullptr;   // Canvas de puntos táctiles

    // Buffer del canvas (320×240 × 2 bytes por pixel)
    static constexpr int kCanvasW = 300;
    static constexpr int kCanvasH = 170;
    lv_color_t _canvasBuf[kCanvasW * kCanvasH];

    // ── Construcción de tabs ──────────────────────────────────────────────
    void _buildBrightnessTab(lv_obj_t* tab);
    void _buildColorTab(lv_obj_t* tab);
    void _buildTouchTab(lv_obj_t* tab);

    // ── Callbacks estáticos ───────────────────────────────────────────────
    static void _onBrightnessChange(lv_event_t* e);
    static void _onColorBtn(lv_event_t* e);

    // Puntero estático para los callbacks que no reciben contexto
    static TestScreen* s_instance;
};
