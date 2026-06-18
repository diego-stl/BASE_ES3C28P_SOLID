/**
 * @file lv_conf.h
 * Configuration file for v8.3.11
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/* clang-format off */
#if 1 /* Set it to "1" to enable content */

/*====================
   COLOR SETTINGS
 *====================*/

/* Color depth: 1 (1 bit per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888) */
#define LV_COLOR_DEPTH 16

/* Swap the 2 bytes of RGB565 color. Useful if the display has a 8-bit interface (e.g. SPI) */
#define LV_COLOR_16_SWAP 0

/* Enable more complex features if needed */
#define LV_COLOR_CHROMA_KEY lv_color_make(0, 0xFF, 0)

/*=========================
   MEMORY SETTINGS
 *=========================*/

/* 1: Use custom malloc/free, 0: Use the built-in `lv_mem_alloc` and `lv_mem_free` */
#define LV_MEM_CUSTOM 1
#if LV_MEM_CUSTOM
    #define LV_MEM_CUSTOM_INCLUDE <stdlib.h>   /* Header for the generic alloc/free */
    #define LV_MEM_CUSTOM_ALLOC   malloc
    #define LV_MEM_CUSTOM_FREE    free
    #define LV_MEM_CUSTOM_REALLOC realloc
#endif

/*====================
   HAL SETTINGS
 *====================*/

/* Default display refresh period. LVGL will redraw the screen with this period [ms] */
#define LV_DISP_DEF_REFR_PERIOD 30

/* Input device read period in milliseconds */
#define LV_INDEV_DEF_READ_PERIOD 30

/* Use a custom tick source that reads Arduino millis().
 * Esto es ESENCIAL para que LVGL pueda leer el dispositivo táctil
 * (indev task) y procesar los eventos de entrada correctamente. */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE  <Arduino.h>
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif

/*======================
 *  FEATURE CONFIGURATION
 *======================*/

/* Drawing speed up and improvements */
#define LV_DRAW_COMPLEX 1
#define LV_SHADOW_CACHE_SIZE 0
#define LV_IMG_CACHE_NUM 4

/*========================
 *  THEME DEMO AND WIDGETS
 *========================*/

/* Default Theme */
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
    /* 0: Light mode, 1: Dark mode */
    #define LV_THEME_DEFAULT_DARK 1
    /* 1: Enable grow on hover/press */
    #define LV_THEME_DEFAULT_GROW 1
    /* Default transition time in ms */
    #define LV_THEME_DEFAULT_TRANSITION_TIME 150
#endif

/*======================
 *  FONT USAGE
 *======================*/

/* Montserrat fonts */
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 1

/* Always set a default font */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/*======================
 *  TEXT SETTINGS
 *======================*/
#define LV_TXT_ENC LV_TXT_ENC_UTF8
#define LV_TXT_BREAK_CHARS " ,.;:-_"

/*======================
 *  WIDGETS
 *======================*/
#define LV_USE_ARC        1
#define LV_USE_BAR        1
#define LV_USE_BTN        1
#define LV_USE_BTNMATRIX  1
#define LV_USE_CANVAS     1
#define LV_USE_CHECKBOX   1
#define LV_USE_DROPDOWN   1
#define LV_USE_IMG        1
#define LV_USE_LABEL      1
#define LV_USE_LINE       1
#define LV_USE_ROLLER     1
#define LV_USE_SLIDER     1
#define LV_USE_SWITCH     1
#define LV_USE_TEXTAREA   1
#define LV_USE_TABLE      1

#endif /*End of "Content enable"*/
#endif /*LV_CONF_H*/
