// =============================================================================
// DisplayManager.cpp
// =============================================================================
#include "DisplayManager.h"
#include "touch.h"
#include <Arduino.h>

// ── Definición de miembros estáticos ─────────────────────────────────────────
lv_disp_draw_buf_t  DisplayManager::s_drawBuf;
lv_color_t          DisplayManager::s_buf1[DisplayManager::kBufPixels];
lv_disp_drv_t       DisplayManager::s_dispDrv;
lv_indev_drv_t      DisplayManager::s_indevDrv;

// Puntero global al LCD para los callbacks estáticos
static TFT_eSPI* g_lcd = nullptr;

// ── Callbacks LVGL ────────────────────────────────────────────────────────────

void DisplayManager::_flushCb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p) {
    if (!g_lcd) { lv_disp_flush_ready(drv); return; }
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    g_lcd->startWrite();
    g_lcd->setAddrWindow(area->x1, area->y1, w, h);
    g_lcd->pushColors((uint16_t*)&color_p->full, w * h, true);
    g_lcd->endWrite();
    lv_disp_flush_ready(drv);
}

void DisplayManager::_touchCb(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    if (touch_touched()) {
        data->state   = LV_INDEV_STATE_PR;
        data->point.x = (lv_coord_t)touch_last_x;
        data->point.y = (lv_coord_t)touch_last_y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// ── init() ────────────────────────────────────────────────────────────────────

void DisplayManager::init(uint8_t brightnessPercent) {
    // 1. LCD
    _lcd.init();
    _lcd.invertDisplay(true);   // ILI9341 en esta placa necesita inversión
    _lcd.setRotation(1);        // Landscape
    _lcd.fillScreen(TFT_BLACK);
    g_lcd = &_lcd;
    Serial.println("[Display] LCD inicializado (320×240, rotation=1)");

    // 2. Backlight PWM
    ledcSetup(kBLChannel, kBLFreqHz, kBLResolution);
    ledcAttachPin(kBLPin, kBLChannel);
    setBrightness(brightnessPercent);
    Serial.printf("[Display] Backlight configurado — brillo inicial: %d%%\n", brightnessPercent);

    // 3. Táctil (comparte Wire con ES8311 — init solo una vez)
    touch_init(320, 240, 1);
    Serial.println("[Display] Táctil FT6336 inicializado");

    // 4. LVGL
    lv_init();
    lv_disp_draw_buf_init(&s_drawBuf, s_buf1, nullptr, kBufPixels);

    lv_disp_drv_init(&s_dispDrv);
    s_dispDrv.hor_res  = 320;
    s_dispDrv.ver_res  = 240;
    s_dispDrv.flush_cb = _flushCb;
    s_dispDrv.draw_buf = &s_drawBuf;
    lv_disp_drv_register(&s_dispDrv);

    lv_indev_drv_init(&s_indevDrv);
    s_indevDrv.type    = LV_INDEV_TYPE_POINTER;
    s_indevDrv.read_cb = _touchCb;
    lv_indev_drv_register(&s_indevDrv);

    Serial.println("[Display] LVGL inicializado");
}

// ── setBrightness() ───────────────────────────────────────────────────────────

void DisplayManager::setBrightness(uint8_t percent) {
    _brightness = percent > 100 ? 100 : percent;
    uint32_t duty = (_brightness * 255) / 100;
    ledcWrite(kBLChannel, duty);
}

// ── tick() ────────────────────────────────────────────────────────────────────

void DisplayManager::tick() {
    lv_timer_handler();
}
