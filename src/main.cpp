// =============================================================================
// main.cpp — Composition Root del proyecto BASE_ES3C28P
//
// Única responsabilidad: instanciar objetos, cablearlos e iniciar el loop.
// NO contiene lógica de negocio, UI, audio, ni red.
//
// Principios SOLID aplicados:
//   S — Cada clase tiene una sola responsabilidad.
//   O — Extensible vía interfaces IDisplay / ITouchpad sin modificar código.
//   L — Las implementaciones concretas cumplen sus contratos de interfaz.
//   I — Interfaces pequeñas y focalizadas.
//   D — Todas las dependencias se inyectan aquí (Dependency Injection).
// =============================================================================
#include <Arduino.h>
#include <WiFi.h>

// ── Capas de la arquitectura ──────────────────────────────────────────────────
#include "ui/DisplayManager.h"
#include "ui/TestScreen.h"
#include "services/SDCardService.h"
#include "services/WiFiService.h"
#include "services/AudioService.h"
#include "services/BluetoothService.h"

// ── Definiciones de hardware ──────────────────────────────────────────────────
// Audio
#define AMP_ENABLE_PIN   1   // GPIO activo LOW — enciende el amplificador físico
#define I2S_BCK          5
#define I2S_WS           7
#define I2S_DOUT         8
#define I2S_MCK          4

// WiFi — editar aquí o cargar desde SD (ver loop de prueba 'W')
// Dejar vacío si no se usan credenciales fijas.
static constexpr const char* kWifiSSID = "";   // ej. "MiRed"
static constexpr const char* kWifiPass = "";   // ej. "MiClave123"

// ── Instancias (Composition Root) ─────────────────────────────────────────────
static DisplayManager  s_display;
static TestScreen      s_testScreen(s_display);   // DIP: recibe IDisplay&
static SDCardService   s_sdCard;
static WiFiService     s_wifi;
static AudioService    s_audio;
static BluetoothService s_ble;

// ── Menú de pruebas por monitor serial ───────────────────────────────────────
static void printMenu() {
    Serial.println("\n========================================");
    Serial.println("  BASE_ES3C28P — Menu de pruebas serial");
    Serial.println("========================================");
    Serial.println("  [S] Probar SD Card (montar + listar /)");
    Serial.println("  [W] Probar WiFi    (escanear redes)");
    Serial.println("  [A] Probar Audio   (beep 1 kHz, 500 ms)");
    Serial.println("  [B] Probar Bluetooth (BLE Scan)");
    Serial.println("  [H] Mostrar este menu");
    Serial.println("========================================\n");
}

static void handleSerialCommand(char cmd) {
    switch (cmd) {
        case 'S': case 's':
            Serial.println("\n── PRUEBA SD ───────────────────────────");
            if (!s_sdCard.isMounted()) {
                s_sdCard.begin(3);
            } else {
                Serial.println("[SD] Ya montada. Listando /...");
            }
            if (s_sdCard.isMounted()) {
                s_sdCard.listRootDirectory();
            }
            break;

        case 'W': case 'w':
            Serial.println("\n── PRUEBA WiFi ─────────────────────────");
            s_wifi.scanAndPrint();
            if (s_wifi.hasCredentials()) {
                Serial.println("[WiFi] Credenciales disponibles — iniciando conexion...");
                s_wifi.connect();
            } else {
                Serial.println("[WiFi] Sin credenciales. Edita kWifiSSID/kWifiPass en main.cpp");
            }
            break;

        case 'A': case 'a':
            Serial.println("\n── PRUEBA AUDIO ────────────────────────");
            s_audio.playTestTone(1000, 500);   // 1 kHz, 500 ms
            break;

        case 'B': case 'b':
            Serial.println("\n── PRUEBA BLUETOOTH BLE ────────────────");
            s_ble.scanAndPrint(5); // Escanear durante 5 segundos
            break;

        case 'H': case 'h':
            printMenu();
            break;

        default:
            Serial.printf("[Main] Comando desconocido: '%c'. Pulsa H para el menu.\n", cmd);
            break;
    }
}

// ── setup() ──────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    delay(500);  // Dar tiempo al CDC para conectar
    Serial.println("\n\n╔══════════════════════════════════════╗");
    Serial.println(  "║   BASE_ES3C28P  v1.0                 ║");
    Serial.println(  "║   Proyecto base SOLID — ESP32-S3     ║");
    Serial.println(  "╚══════════════════════════════════════╝\n");

    // 1. Display + LVGL + Táctil (Wire se inicia aquí)
    s_display.init(70);
    Serial.println("[Setup] Display OK");

    // 2. UI de pruebas (construye widgets LVGL)
    s_testScreen.build();
    Serial.println("[Setup] TestScreen construida");

    // 3. Audio: codec ES8311 + I2S
    //    Wire ya iniciado por touch_init() dentro de DisplayManager::init()
    bool audioOk = s_audio.init(AMP_ENABLE_PIN, I2S_BCK, I2S_WS, I2S_DOUT, I2S_MCK);
    if (audioOk) {
        Serial.println("[Setup] Audio OK");
        // Beep corto de bienvenida (400 Hz, 200 ms)
        s_audio.playTestTone(400, 200);
    } else {
        Serial.println("[Setup] AVISO: Audio no disponible");
    }

    // 4. WiFi (opcional — sin bloquear el inicio)
    WiFi.mode(WIFI_STA);
    if (kWifiSSID[0] != '\0') {
        s_wifi.loadCredentials(kWifiSSID, kWifiPass);
        s_wifi.connect();
    } else {
        Serial.println("[Setup] WiFi: sin credenciales configuradas");
    }

    // 5. SD Card (no bloquea si no hay tarjeta)
    bool sdOk = s_sdCard.begin(2);
    if (!sdOk) {
        Serial.println("[Setup] AVISO: SD no disponible (no es obligatoria)");
    }

    // 6. Menú serial
    printMenu();
    Serial.println("[Setup] ✓ LISTO — Iniciando loop()\n");
}

// ── loop() ───────────────────────────────────────────────────────────────────
void loop() {
    uint32_t now = millis();

    // 1. Procesar comandos del monitor serial
    if (Serial.available()) {
        char cmd = (char)Serial.read();
        // Ignorar CR/LF
        if (cmd != '\r' && cmd != '\n') {
            handleSerialCommand(cmd);
        }
    }

    // 2. Tick de servicios
    s_wifi.tick(now);

    // 3. Tick de la UI:
    //    PRIMERO lv_timer_handler() para que LVGL procese el táctil,
    //    LUEGO TestScreen::tick() para leer el estado ya procesado por LVGL.
    s_display.tick();       // → lv_timer_handler() + indev polling
    s_testScreen.tick();    // → lee lv_indev_data y actualiza canvas

    // 4. Ceder CPU al stack WiFi/FreeRTOS
    delay(5);
}
