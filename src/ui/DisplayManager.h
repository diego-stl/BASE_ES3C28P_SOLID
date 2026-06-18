#pragma once
// =============================================================================
// DisplayManager.h — Inicialización de LVGL + TFT_eSPI + backlight PWM.
//
// SRP: única responsabilidad — ciclo de vida del display y LVGL.
// DIP: implementa IDisplay para desacoplar la UI del hardware físico.
// =============================================================================
#include "hal/IDisplay.h"
#include <TFT_eSPI.h>
#include <lvgl.h>

class DisplayManager : public IDisplay {
public:
    // ── Inicialización ────────────────────────────────────────────────────
    // Debe llamarse en setup(), antes de crear widgets LVGL.
    void init(uint8_t brightnessPercent = 70) override;

    // ── IDisplay ──────────────────────────────────────────────────────────
    void    setBrightness(uint8_t percent) override;
    uint8_t getBrightness()          const override { return _brightness; }
    uint16_t width()                       override { return 320; }   // ILI9341 landscape
    uint16_t height()                      override { return 240; }

    // ── tick(): llamar desde loop() para refrescar LVGL ──────────────────
    void tick();

    // ── Acceso al TFT para el flush callback ─────────────────────────────
    TFT_eSPI& tft() { return _lcd; }

    // ── Constantes de hardware ────────────────────────────────────────────
    static constexpr uint8_t  kBLPin       = 45;
    static constexpr uint32_t kBLFreqHz    = 5000;
    static constexpr uint8_t  kBLResolution = 8;    // 8 bits → 0–255
    static constexpr uint8_t  kBLChannel   = 0;

    // Tamaño del buffer de renderizado LVGL (1/10 de pantalla)
    static constexpr int kBufPixels = 320 * 24;

private:
    TFT_eSPI _lcd;
    uint8_t  _brightness = 70;

    static lv_disp_draw_buf_t  s_drawBuf;
    static lv_color_t          s_buf1[kBufPixels];
    static lv_disp_drv_t       s_dispDrv;
    static lv_indev_drv_t      s_indevDrv;

    // LVGL callbacks (deben ser funciones estáticas o globales)
    static void _flushCb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p);
    static void _touchCb(lv_indev_drv_t* drv, lv_indev_data_t* data);
};
