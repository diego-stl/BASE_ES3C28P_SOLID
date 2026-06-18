#pragma once
#include <stdint.h>

class BluetoothService {
public:
    BluetoothService() = default;
    ~BluetoothService() = default;

    // Inicializa el dispositivo BLE (solo se ejecuta una vez)
    void init();

    // Inicia un escaneo de dispositivos BLE y los imprime en el Serial
    // scanTimeSec: duración del escaneo en segundos
    void scanAndPrint(uint32_t scanTimeSec = 5);

private:
    bool _initialized = false;
};
