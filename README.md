# Proyecto Base SOLID para ESP32-S3 (Pantalla 2.8" IPS - ES3C28P)

Este es un proyecto base limpio y modular para la placa de desarrollo **ESP32-S3 de 2.8 pulgadas IPS** (referencia del fabricante: **ES3C28P** / **ILI9341V**). Está desarrollado bajo los principios de diseño de software **SOLID**, facilitando un punto de partida estable, ordenado y desacoplado para cualquier desarrollo futuro en este hardware.

---

## ⚠️ Información Crítica del Hardware (USB y Serial)

> [!IMPORTANT]
> **Puerto USB Nativo (CDC)**
> - Este producto **no cuenta con un chip conversor de USB a Serial en la placa** (como los habituales CP2102 o CH340).
> - La interfaz física Type-C se comunica directamente con los pines USB del ESP32-S3 mediante emulación **USB CDC (puerto serie virtual analógico)**.
> - **Compatibilidad del Sistema Operativo**: Se reconoce directamente en sistemas **Windows 10 o superior** sin necesidad de controladores adicionales. En versiones de Windows anteriores, es posible que el sistema no lo reconozca o requiera drivers USB manuales.
> - Para asegurar que la salida serial se redirija correctamente a través de este conector durante el inicio, en el archivo `platformio.ini` se han habilitado las siguientes banderas de compilación:
>   ```ini
>   -D ARDUINO_USB_CDC_ON_BOOT=1
>   -D ARDUINO_USB_MODE=1
>   ```

---

## 💻 Especificaciones Técnicas de la Placa (ES3C28P)

*   **Microcontrolador**: ESP32-S3 (Xtensa® de 32 bits LX7 de doble núcleo a 240 MHz).
*   **Memoria Flash**: 16 MB QIO Flash.
*   **Memoria PSRAM**: 8 MB OPI PSRAM externa (para el correcto funcionamiento de interfaces gráficas complejas como LVGL).
*   **Pantalla**: 
    *   **Tamaño**: 2.8 pulgadas IPS.
    *   **Controlador de Display**: ILI9341V (interfaz SPI a 40 MHz).
    *   **Resolución**: 320 × 240 píxeles.
*   **Controlador Táctil**: FT6336 (capacitivo, interfaz I2C).
*   **Códec de Audio**: ES8311 (DAC de audio de ultra bajo consumo con amplificador de potencia integrado, conectado por I2S e I2C).
*   **Ranura SD**: Ranura para tarjeta MicroSD integrada compatible con bus SD-MMC de hasta 4 bits de ancho.

---

## 📌 Asignación de Pines GPIO (Pinout)

El proyecto base tiene configurado y probado el siguiente mapa de pines:

| Periférico | Pin(es) GPIO | Notas |
| :--- | :--- | :--- |
| **TFT Display (SPI)** | MOSI: `11` \| SCLK: `12` \| MISO: `13` \| CS: `10` \| DC: `46` \| BL (Backlight): `45` | El pin Reset del display se conecta al Reset de hardware de la placa (`RST=-1`). |
| **Touchpad & I2C** | SCL: `15` \| SDA: `16` \| INT: `17` \| RST: `18` | Bus I2C compartido entre el controlador táctil FT6336 y el codec de audio ES8311. |
| **Audio I2S (ES8311)** | BCK: `5` \| WS: `7` \| DOUT: `8` \| MCK: `4` \| AMP Enable: `1` | El amplificador se activa colocando el pin `1` en estado `LOW` (Activo LOW). |
| **Tarjeta SD (MMC)** | SCK: `38` \| CMD: `40` \| D0: `39` \| D1: `41` \| D2: `48` \| D3: `47` | Conexión directa SD-MMC. |

---

## 🔒 Control y Fijación de Versiones (Evitar fallos por actualización)

Para garantizar la estabilidad del proyecto y evitar que futuras actualizaciones automáticas de la plataforma o librerías rompan el código, se han fijado las versiones de manera estricta en el archivo `platformio.ini`:

*   **Plataforma de Desarrollo**: `espressif32 @ 7.0.1` (asegura el uso estable del Core de Arduino 3.x / ESP-IDF).
*   **Librería Gráfica TFT**: `bodmer/TFT_eSPI @ 2.5.43`
*   **Librería LVGL**: `lvgl/lvgl @ 8.4.0`

---

## 🛠️ Estructura del Software (Arquitectura SOLID)

El software sigue principios SOLID estructurados en 3 capas diferenciadas:

1.  **HAL (Hardware Abstraction Layer)**: 
    *   [IDisplay.h](file:///d:/proyectos/esp32s3%20display%20ES3C28P/BASE_ES3C28P_SOLID/src/hal/IDisplay.h) / [ITouchpad.h](file:///d:/proyectos/esp32s3%20display%20ES3C28P/BASE_ES3C28P_SOLID/src/hal/ITouchpad.h): Contratos de interfaz para desacoplar las implementaciones físicas del display y táctil de la interfaz gráfica de usuario.
2.  **Servicios (Services)**:
    *   `SDCardService`: Montaje en bus MMC y lectura de archivos.
    *   `WiFiService`: Escaneo de redes y gestión de la conexión WiFi (con callbacks ligeros).
    *   `AudioService`: Inicialización del codec ES8311 y generación de tonos de prueba sinusoidales (I2S).
    *   `BluetoothService`: Inicialización del stack BLE y escaneo activo de dispositivos.
3.  **UI (User Interface)**:
    *   `DisplayManager`: Inicialización de LVGL, backlight por hardware (PWM) e inyección del driver.
    *   `TestScreen`: Pantalla táctil interactiva LVGL que implementa 3 pestañas:
        *   **Brillo**: Modificación en tiempo real del PWM del Backlight.
        *   **Color**: Pintado del fondo de pantalla con diferentes colores básicos.
        *   **Táctil**: Canvas interactivo para registrar coordenadas de toque en tiempo real.

---

## 📟 Menú de Pruebas por Monitor Serial (115200 baudios)

Al conectar la placa al ordenador e iniciar el monitor serie a **115200 baudios**, se desplegará el menú interactivo para realizar pruebas de hardware de forma aislada:

```text
========================================
  BASE_ES3C28P — Menu de pruebas serial
========================================
  [S] Probar SD Card (montar + listar /)
  [W] Probar WiFi    (escanear redes)
  [A] Probar Audio   (beep 1 kHz, 500 ms)
  [B] Probar Bluetooth (BLE Scan)
  [H] Mostrar este menu
========================================
```

*   **`S` (o `s`)**: Monta la tarjeta MicroSD e imprime en consola los archivos del directorio raíz con su tamaño correspondiente.
*   **`W` (o `w`)**: Realiza un escaneo de redes WiFi en el entorno y muestra sus nombres (SSID), nivel de señal (RSSI) y tipo de encriptación.
*   **`A` (o `a`)**: Activa el códec de audio y el amplificador, reproduciendo un tono sinusoidal limpio a 1 kHz durante 500 ms por la salida de audio.
*   **`B` (o `b`)**: Inicia un escaneo activo de dispositivos Bluetooth de Baja Energía (BLE) durante 5 segundos, listando en tiempo real los nombres de los dispositivos, su MAC y su intensidad de señal (RSSI).
*   **`H` (o `h`)**: Imprime nuevamente el menú de ayuda en el terminal.
