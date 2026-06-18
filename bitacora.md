# Bitácora de Proyecto - BASE_ES3C28P_SOLID

Este subproyecto representa el firmware base limpio y estructurado bajo los principios **SOLID** en C++ para la placa de desarrollo **ESP32-S3 (ES3C28P)** con pantalla de 2.8" IPS y táctil capacitivo. Sirve como plantilla desacoplada para todos los demás proyectos.

## Estructura del Software
*   **HAL (Capa de Abstracción de Hardware)**:
    *   `IDisplay.h`: Interfaz genérica para inicialización y ciclo del display.
    *   `ITouchpad.h`: Interfaz genérica para la lectura táctil.
*   **Servicios (Services)**:
    *   `SDCardService`: Montaje seguro de tarjeta MicroSD vía SDMMC.
    *   `WiFiService`: Conectividad y escaneo Wi-Fi sin bloqueo.
    *   `AudioService`: Driver para códec ES8311 utilizando bus Wire I2C compartido y bus I2S.
    *   `BluetoothService`: Escaneo BLE activo.
*   **UI (Interfaz de Usuario)**:
    *   `DisplayManager`: Gestor de LVGL v8 y control de brillo PWM por hardware.
    *   `TestScreen`: Pantalla interactiva con tres pestañas (Control de brillo, fondo de color y canvas de dibujo táctil).

---

## Registro de Hitos

### [2026-06-07] - Versión 1.0.0
*   Creación inicial del proyecto base desacoplado.
*   Implementación de inversión de dependencias para los componentes de hardware (Display y Touchpad).
*   Integración del menú de control interactivo por puerto serie (USB CDC nativo del ESP32-S3).
*   Fijación de versiones estables en `platformio.ini` (`espressif32 @ 7.0.1`, `lvgl @ 8.4.0`, `TFT_eSPI @ 2.5.43`).
