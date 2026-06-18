// =============================================================================
// SDCardService.cpp
// =============================================================================
#include "SDCardService.h"
#include <Arduino.h>
#include <SD_MMC.h>
#include <FS.h>

bool SDCardService::begin(int maxRetries) {
    if (!SD_MMC.setPins(kSckPin, kCmdPin, kD0Pin, kD1Pin, kD2Pin, kD3Pin)) {
        Serial.println("[SD] ERROR: No se pudieron configurar los pines");
        return false;
    }

    for (int i = 0; i < maxRetries; i++) {
        if (SD_MMC.begin("/sdcard", false)) {  // 4-bit bus
            _mounted = true;
            Serial.printf("[SD] Montada correctamente. Tipo: %d\n", SD_MMC.cardType());
            printCardInfo();
            return true;
        }
        Serial.printf("[SD] Reintento %d/%d...\n", i + 1, maxRetries);
        delay(500);
    }
    Serial.println("[SD] ERROR: No se pudo montar. ¿Tarjeta presente? ¿Formato FAT32?");
    return false;
}

void SDCardService::printCardInfo() {
    if (!_mounted) { Serial.println("[SD] No montada"); return; }
    uint64_t total = SD_MMC.totalBytes() / (1024 * 1024);
    uint64_t used  = SD_MMC.usedBytes()  / (1024 * 1024);
    Serial.printf("[SD] Capacidad: %llu MB  Usado: %llu MB  Libre: %llu MB\n",
                  total, used, total - used);
}

void SDCardService::listRootDirectory() {
    if (!_mounted) { Serial.println("[SD] No montada, no se puede listar"); return; }

    Serial.println("[SD] ── Contenido de / ──────────────────────────");
    File root = SD_MMC.open("/");
    if (!root || !root.isDirectory()) {
        Serial.println("[SD] ERROR: No se puede abrir el directorio raíz");
        return;
    }

    int count = 0;
    File entry = root.openNextFile();
    while (entry) {
        const char* name = entry.name();
        if (entry.isDirectory()) {
            Serial.printf("[SD]   [DIR]  %s\n", name);
        } else {
            Serial.printf("[SD]   [FILE] %-30s  %lu bytes\n", name, (unsigned long)entry.size());
        }
        count++;
        entry.close();
        entry = root.openNextFile();
    }
    root.close();
    Serial.printf("[SD] ── Total: %d entradas ─────────────────────────\n", count);
}
