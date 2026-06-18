#include "BluetoothService.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Callback para imprimir los dispositivos encontrados en tiempo real por Serial
class SerialAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
public:
    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        if (advertisedDevice.haveName()) {
            Serial.printf("  [BLE] Nombre: %-30s | RSSI: %d dBm\n", 
                          advertisedDevice.getName().c_str(), 
                          advertisedDevice.getRSSI());
        } else {
            Serial.printf("  [BLE] MAC: %-33s | RSSI: %d dBm\n", 
                          advertisedDevice.getAddress().toString().c_str(), 
                          advertisedDevice.getRSSI());
        }
    }
};

void BluetoothService::init() {
    if (_initialized) return;
    
    Serial.println("[BLE] Inicializando controlador BLE...");
    // Inicializar dispositivo BLE con un nombre identificativo
    BLEDevice::init("BASE_ES3C28P_BLE");
    _initialized = true;
    Serial.println("[BLE] Inicialización OK");
}

void BluetoothService::scanAndPrint(uint32_t scanTimeSec) {
    init(); // Asegurar que el controlador BLE esté inicializado

    Serial.printf("[BLE] Iniciando escaneo activo de %d segundos...\n", scanTimeSec);
    
    BLEScan* pBLEScan = BLEDevice::getScan();
    
    // Configurar callback estático para procesar dispositivos detectados
    static SerialAdvertisedDeviceCallbacks callbacks;
    pBLEScan->setAdvertisedDeviceCallbacks(&callbacks);
    pBLEScan->setActiveScan(true); // Escaneo activo (más consumo pero mejores resultados)
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);

    // Iniciar escaneo (esta llamada bloquea durante scanTimeSec)
    BLEScanResults foundDevices = pBLEScan->start(scanTimeSec, false);
    
    Serial.printf("\n[BLE] ¡Escaneo terminado! Dispositivos totales encontrados: %d\n", foundDevices.getCount());
    
    // Limpiar resultados para liberar memoria interna del búfer de BLEScan
    pBLEScan->clearResults();
    pBLEScan->setAdvertisedDeviceCallbacks(nullptr);
}
