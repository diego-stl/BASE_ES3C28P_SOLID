// =============================================================================
// WiFiService.cpp
// =============================================================================
#include "WiFiService.h"
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

void WiFiService::loadCredentials(const char* ssid, const char* pass) {
    if (ssid) { strncpy(_ssid, ssid, sizeof(_ssid) - 1); _ssid[sizeof(_ssid)-1] = '\0'; }
    if (pass) { strncpy(_pass, pass, sizeof(_pass) - 1); _pass[sizeof(_pass)-1] = '\0'; }
    Serial.printf("[WiFi] Credenciales cargadas: SSID=%s\n", _ssid);
}

void WiFiService::connect() {
    if (_ssid[0] == '\0') { _notifyStatus("WiFi: Sin credenciales"); return; }
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(_ssid, _pass);
    _connecting      = true;
    _connectStartMs  = millis();
    _lastReconnectMs = millis();
    _wasConnected    = false;
    _notifyStatus("WiFi: Conectando...");
}

void WiFiService::disconnect() {
    WiFi.disconnect(true, false);
    _connecting   = false;
    _wasConnected = false;
    _notifyStatus("WiFi: Desconectado");
}

bool WiFiService::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

const char* WiFiService::getIPString() const {
    static char buf[20];
    if (isConnected()) {
        strncpy(buf, WiFi.localIP().toString().c_str(), sizeof(buf) - 1);
        buf[sizeof(buf)-1] = '\0';
    } else {
        strncpy(buf, "---.---.---.-", sizeof(buf) - 1);
    }
    return buf;
}

void WiFiService::tick(uint32_t nowMs) {
    // Limitar procesamiento a 1 vez cada 500 ms para no saturar el loop
    if (nowMs - _lastCheckMs < 500) return;
    _lastCheckMs = nowMs;

    if (_connecting) {
        wl_status_t status = WiFi.status();
        if (status == WL_CONNECTED) {
            _connecting   = false;
            _wasConnected = true;
            if (!_ntpConfigured) { _configureNTP(); _ntpConfigured = true; }
            char buf[48];
            snprintf(buf, sizeof(buf), "WiFi: Conectado — IP %s", getIPString());
            _notifyStatus(buf);
            _notify(_onConnected, _onConnectedUD, buf);
        } else if (nowMs - _connectStartMs > kConnectTimeoutMs) {
            _connecting = false;
            _notifyStatus("WiFi: Timeout — sin conexion");
        }
    } else {
        // Detectar desconexión
        if (_wasConnected && !isConnected()) {
            _wasConnected = false;
            _notifyStatus("WiFi: Desconectado");
            _notify(_onDisconnected, _onDisconnectedUD, "WiFi: Desconectado");
        }
        // Auto-reconexión periódica
        if (!isConnected() && !_connecting && _ssid[0] != '\0') {
            if (nowMs - _lastReconnectMs >= kReconnectIntervalMs) {
                _lastReconnectMs = nowMs;
                connect();
            }
        }
    }
}

void WiFiService::scanAndPrint() {
    Serial.println("[WiFi] Iniciando escaneo de redes...");
    WiFi.mode(WIFI_STA);
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("[WiFi] No se encontraron redes");
        return;
    }
    Serial.printf("[WiFi] Redes encontradas: %d\n", n);
    for (int i = 0; i < n; i++) {
        Serial.printf("[WiFi]   %2d) %-32s  RSSI: %4d dBm  %s\n",
                      i + 1,
                      WiFi.SSID(i).c_str(),
                      WiFi.RSSI(i),
                      (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "[Abierta]" : "[Protegida]");
    }
    WiFi.scanDelete();
}

void WiFiService::_configureNTP() {
    configTzTime("COT5", "pool.ntp.org", "time.nist.gov");
    Serial.println("[NTP] Sincronizacion iniciada (COT5 — Bogota)");
}

void WiFiService::_notify(WiFiEventCallback cb, void* ud, const char* msg) {
    if (cb) cb(msg, ud);
}

void WiFiService::_notifyStatus(const char* msg) {
    Serial.printf("[WiFi] %s\n", msg);
    if (_onStatus) _onStatus(msg, _onStatusUD);
}
