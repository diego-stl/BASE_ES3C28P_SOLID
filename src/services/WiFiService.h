#pragma once
// =============================================================================
// WiFiService.h — Conexión WiFi con callbacks desacoplados de la UI.
//
// SRP: única responsabilidad — gestión del estado de red.
// DIP: comunica eventos mediante callbacks (function pointers ligeros).
//      Ninguna dependencia de LVGL ni de TFT.
// =============================================================================
#include <stdint.h>
#include <stdbool.h>

// Callbacks ligeros (evita std::function para reducir overhead en ESP32)
typedef void (*WiFiEventCallback)(const char* msg, void* userData);

class WiFiService {
public:
    // ── Configuración de callbacks ────────────────────────────────────────
    void setOnStatus(WiFiEventCallback cb, void* ud = nullptr)     { _onStatus = cb; _onStatusUD = ud; }
    void setOnConnected(WiFiEventCallback cb, void* ud = nullptr)  { _onConnected = cb; _onConnectedUD = ud; }
    void setOnDisconnected(WiFiEventCallback cb, void* ud = nullptr){ _onDisconnected = cb; _onDisconnectedUD = ud; }

    // ── API de control ────────────────────────────────────────────────────
    void loadCredentials(const char* ssid, const char* pass);
    void connect();
    void disconnect();

    bool isConnected()    const;
    bool isConnecting()   const { return _connecting; }
    bool hasCredentials() const { return _ssid[0] != '\0'; }

    const char* getSSID()     const { return _ssid; }
    const char* getIPString() const;

    // Escanea redes y las imprime por serial (bloqueante ~2 s)
    void scanAndPrint();

    // ── tick(): llamar desde loop() ───────────────────────────────────────
    void tick(uint32_t nowMs);

    static constexpr uint32_t kConnectTimeoutMs    = 15000;
    static constexpr uint32_t kReconnectIntervalMs = 30000;

private:
    char _ssid[32] = "";
    char _pass[64] = "";

    bool     _connecting       = false;
    bool     _wasConnected     = false;
    bool     _ntpConfigured    = false;
    uint32_t _connectStartMs   = 0;
    uint32_t _lastReconnectMs  = 0;
    uint32_t _lastCheckMs      = 0;

    WiFiEventCallback _onStatus       = nullptr;  void* _onStatusUD       = nullptr;
    WiFiEventCallback _onConnected    = nullptr;  void* _onConnectedUD    = nullptr;
    WiFiEventCallback _onDisconnected = nullptr;  void* _onDisconnectedUD = nullptr;

    void _configureNTP();
    void _notify(WiFiEventCallback cb, void* ud, const char* msg);
    void _notifyStatus(const char* msg);
};
