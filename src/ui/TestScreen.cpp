// =============================================================================
// TestScreen.cpp — Pantalla LVGL de 3 tabs para pruebas de hardware.
//
// Tab 1 — Brillo: slider 0-100% que controla el backlight PWM en tiempo real.
// Tab 2 — Color:  6 botones de color que pintan el fondo.
// Tab 3 — Táctil: canvas donde se dibujan los puntos tocados.
// =============================================================================
#include "TestScreen.h"
#include "touch.h"
#include <Arduino.h>
#include <string.h>

// ── Instancia estática para callbacks ────────────────────────────────────────
TestScreen* TestScreen::s_instance = nullptr;

// ── Paleta de colores para el tab de color ─────────────────────────────────
struct ColorEntry { const char* name; lv_color_t color; };
static const ColorEntry kColors[] = {
    { "Rojo",     LV_COLOR_MAKE(0xE5, 0x39, 0x35) },
    { "Verde",    LV_COLOR_MAKE(0x43, 0xA0, 0x47) },
    { "Azul",     LV_COLOR_MAKE(0x1E, 0x88, 0xE5) },
    { "Naranja",  LV_COLOR_MAKE(0xFB, 0x8C, 0x00) },
    { "Morado",   LV_COLOR_MAKE(0x8E, 0x24, 0xAA) },
    { "Blanco",   LV_COLOR_MAKE(0xFF, 0xFF, 0xFF) },
};
static constexpr int kColorCount = 6;
static lv_obj_t* s_colorScreen = nullptr;  // referencia a la pantalla para cambiar bg

// ── Constructor ───────────────────────────────────────────────────────────────
TestScreen::TestScreen(IDisplay& display) : _display(display) {
    s_instance = this;
    memset(_canvasBuf, 0, sizeof(_canvasBuf));
}

// ── build() ───────────────────────────────────────────────────────────────────
void TestScreen::build() {
    // Pantalla raíz con fondo oscuro
    lv_obj_t* scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_make(0x12, 0x12, 0x20), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    s_colorScreen = scr;

    // ── Título ────────────────────────────────────────────────────────────
    lv_obj_t* title = lv_label_create(scr);
    lv_label_set_text(title, "BASE_ES3C28P — Pruebas de hardware");
    lv_obj_set_style_text_color(title, lv_color_make(0xA0, 0xD0, 0xFF), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 4);

    // ── Contenedor de tabs ────────────────────────────────────────────────
    lv_obj_t* tabview = lv_tabview_create(scr, LV_DIR_TOP, 36);
    lv_obj_set_size(tabview, 320, 216);
    lv_obj_align(tabview, LV_ALIGN_BOTTOM_MID, 0, 0);

    // Estilo tabview
    lv_obj_set_style_bg_color(tabview, lv_color_make(0x1A, 0x1A, 0x2E), 0);
    lv_obj_set_style_bg_opa(tabview, LV_OPA_COVER, 0);

    // Estilo de las pestañas no seleccionadas
    lv_obj_t* tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_bg_color(tab_btns, lv_color_make(0x0F, 0x0F, 0x23), 0);
    lv_obj_set_style_text_color(tab_btns, lv_color_make(0x80, 0x80, 0xA0), 0);

    // Crear los 3 tabs
    lv_obj_t* tabBrillo = lv_tabview_add_tab(tabview, LV_SYMBOL_IMAGE "  Brillo");
    lv_obj_t* tabColor  = lv_tabview_add_tab(tabview, LV_SYMBOL_EYE_OPEN "  Color");
    lv_obj_t* tabTouch  = lv_tabview_add_tab(tabview, LV_SYMBOL_OK "  Tactil");

    _buildBrightnessTab(tabBrillo);
    _buildColorTab(tabColor);
    _buildTouchTab(tabTouch);
}

// ── Tab 1: Brillo ─────────────────────────────────────────────────────────────
void TestScreen::_buildBrightnessTab(lv_obj_t* tab) {
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(tab, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(tab, 14, 0);

    // Label de descripción
    lv_obj_t* desc = lv_label_create(tab);
    lv_label_set_text(desc, "Ajusta el brillo de la pantalla");
    lv_obj_set_style_text_color(desc, lv_color_make(0xC0, 0xC0, 0xE0), 0);

    // Label de valor
    lv_obj_t* valLabel = lv_label_create(tab);
    lv_obj_set_style_text_font(valLabel, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(valLabel, lv_color_make(0x64, 0xB5, 0xF6), 0);
    lv_label_set_text_fmt(valLabel, "%d%%", (int)_display.getBrightness());

    // Slider
    lv_obj_t* slider = lv_slider_create(tab);
    lv_obj_set_width(slider, 260);
    lv_slider_set_range(slider, 5, 100);
    lv_slider_set_value(slider, (int)_display.getBrightness(), LV_ANIM_OFF);
    lv_obj_set_style_bg_color(slider, lv_color_make(0x1E, 0x88, 0xE5), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_make(0x64, 0xB5, 0xF6), LV_PART_KNOB);

    // Guardar label en user_data del slider para actualizarlo en el callback
    lv_obj_set_user_data(slider, valLabel);
    lv_obj_add_event_cb(slider, _onBrightnessChange, LV_EVENT_VALUE_CHANGED, nullptr);
}

// ── Tab 2: Color ──────────────────────────────────────────────────────────────
void TestScreen::_buildColorTab(lv_obj_t* tab) {
    lv_obj_t* desc = lv_label_create(tab);
    lv_label_set_text(desc, "Toca un color para pintarlo en el fondo");
    lv_obj_set_style_text_color(desc, lv_color_make(0xC0, 0xC0, 0xE0), 0);
    lv_obj_align(desc, LV_ALIGN_TOP_MID, 0, 8);

    // Grid de botones 3×2
    lv_obj_t* grid = lv_obj_create(tab);
    lv_obj_set_size(grid, 290, 110);
    lv_obj_align(grid, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_color(grid, lv_color_make(0x1A, 0x1A, 0x2E), 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_style_pad_all(grid, 4, 0);

    for (int i = 0; i < kColorCount; i++) {
        lv_obj_t* btn = lv_btn_create(grid);
        lv_obj_set_size(btn, 82, 38);
        lv_obj_set_style_bg_color(btn, kColors[i].color, 0);
        lv_obj_set_style_radius(btn, 6, 0);

        lv_obj_t* lbl = lv_label_create(btn);
        lv_label_set_text(lbl, kColors[i].name);
        lv_obj_center(lbl);
        // Color de texto: blanco excepto para el botón blanco
        lv_color_t txtColor = (i == kColorCount - 1)
                              ? lv_color_make(0x20, 0x20, 0x20)
                              : lv_color_make(0xFF, 0xFF, 0xFF);
        lv_obj_set_style_text_color(lbl, txtColor, 0);

        // Pasar índice como user_data (cast int → pointer)
        lv_obj_set_user_data(btn, (void*)(intptr_t)i);
        lv_obj_add_event_cb(btn, _onColorBtn, LV_EVENT_CLICKED, nullptr);
    }
}

// ── Tab 3: Táctil ─────────────────────────────────────────────────────────────
void TestScreen::_buildTouchTab(lv_obj_t* tab) {
    // Label de coordenadas
    _touchLabel = lv_label_create(tab);
    lv_label_set_text(_touchLabel, "X: ---  Y: ---");
    lv_obj_set_style_text_color(_touchLabel, lv_color_make(0xA0, 0xFF, 0xA0), 0);
    lv_obj_set_style_text_font(_touchLabel, &lv_font_montserrat_16, 0);
    lv_obj_align(_touchLabel, LV_ALIGN_TOP_MID, 0, 2);

    // Canvas de dibujo
    _touchCanvas = lv_canvas_create(tab);
    lv_canvas_set_buffer(_touchCanvas, _canvasBuf, kCanvasW, kCanvasH, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align(_touchCanvas, LV_ALIGN_BOTTOM_MID, 0, -4);
    lv_obj_set_style_border_color(_touchCanvas, lv_color_make(0x40, 0x40, 0x80), 0);
    lv_obj_set_style_border_width(_touchCanvas, 1, 0);

    // Fondo del canvas
    lv_canvas_fill_bg(_touchCanvas, lv_color_make(0x08, 0x08, 0x18), LV_OPA_COVER);

    // Instrucción
    lv_obj_t* hint = lv_label_create(tab);
    lv_label_set_text(hint, "Toca la pantalla para ver los puntos");
    lv_obj_set_style_text_color(hint, lv_color_make(0x60, 0x60, 0x90), 0);
    lv_obj_set_style_text_font(hint, &lv_font_montserrat_12, 0);
    lv_obj_align(hint, LV_ALIGN_CENTER, 0, -10);
}

// ── tick() ────────────────────────────────────────────────────────────────────
// Dibuja en el canvas táctil leyendo directamente del chip FT6336.
// LVGL también lee el táctil en su callback (para mover botones/slider),
// pero cada lectura I2C es independiente y el chip retiene el dato.
void TestScreen::tick() {
    if (!_touchLabel || !_touchCanvas) return;

    if (touch_touched()) {
        int x = touch_last_x;
        int y = touch_last_y;

        // Actualizar label de coordenadas
        lv_label_set_text_fmt(_touchLabel, "X: %3d  Y: %3d", x, y);

        // Mapear coordenadas de pantalla (320×240) al canvas (kCanvasW × kCanvasH)
        int cx = (int)((float)x * kCanvasW / 320.0f);
        int cy = (int)((float)y * kCanvasH / 240.0f);
        cx = cx < 0 ? 0 : (cx >= kCanvasW ? kCanvasW - 1 : cx);
        cy = cy < 0 ? 0 : (cy >= kCanvasH ? kCanvasH - 1 : cy);

        // Dibujar punto circular en el canvas
        lv_draw_rect_dsc_t dsc;
        lv_draw_rect_dsc_init(&dsc);
        dsc.bg_color = lv_color_make(0x00, 0xFF, 0x88);
        dsc.radius   = LV_RADIUS_CIRCLE;
        lv_canvas_draw_rect(_touchCanvas, cx - 3, cy - 3, 7, 7, &dsc);
        lv_obj_invalidate(_touchCanvas);
    }
}

// ── Callbacks estáticos ───────────────────────────────────────────────────────

void TestScreen::_onBrightnessChange(lv_event_t* e) {
    lv_obj_t* slider   = lv_event_get_target(e);
    int        val      = lv_slider_get_value(slider);
    lv_obj_t*  valLabel = (lv_obj_t*)lv_obj_get_user_data(slider);
    if (valLabel) lv_label_set_text_fmt(valLabel, "%d%%", val);
    if (s_instance) s_instance->_display.setBrightness((uint8_t)val);
}

void TestScreen::_onColorBtn(lv_event_t* e) {
    lv_obj_t* btn = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(btn);
    if (idx < 0 || idx >= kColorCount) return;
    if (s_colorScreen) {
        lv_obj_set_style_bg_color(s_colorScreen, kColors[idx].color, 0);
    }
}
