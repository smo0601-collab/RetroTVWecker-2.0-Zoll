**Firmware 2026.10.30** für den RetroTV-Wecker (ESP32-C3 SuperMini, 2,0-Zoll-Display ST7789).

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.10.30_update.bin` | fertige Firmware – im Browser unter *System → Firmware-Update über WLAN* einspielen |
| `WetterUhr_2026.10.30_komplett.zip` | kompletter Quellcode für die Arduino IDE |

### Neu in dieser Version
- **Firmware-Update über WLAN** im Browser (mit Prüfung der Datei, Neustart nur bei Erfolg) und über den Netzwerk-Port der Arduino IDE.
- Firmware-Version und Bauzeit unter *Status → Firmware*.

### Wichtig beim Umstieg
Lief die Uhr bisher mit dem Partitionsschema *Huge APP (No OTA)*, diese Version **einmal per USB** mit
*Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA)* hochladen (Quellcode aus dem Zip). Danach gehen alle Updates per WLAN.

### Build
Gebaut mit arduino-cli, ESP32-Arduino-Core 3.3.12, Board *ESP32C3 Dev Module*, *USB CDC On Boot: Enabled*,
*Minimal SPIFFS (1.9MB APP with OTA)*. Größe 1,65 MB (84 % des Update-Bereichs).

Alle Änderungen: [CHANGELOG](https://github.com/smo0601-collab/RetroTVWecker-2.0-Zoll/blob/main/firmware/WetterUhr/CHANGELOG.md)
