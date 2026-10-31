**Firmware 2026.10.31-2** für den RetroTV-Wecker (ESP32-C3 SuperMini, 2,0-Zoll-Display ST7789).

### Neu in dieser Version
- Der **Weihnachtself** kommt zusammen mit Weihnachten vom **14. bis 26.12.** automatisch in den Design-Wechsel.

Feiertage im Wechsel (jeweils 10 Tage vorher bis zum Fest, eigene Liste bleibt unverändert):

| Design | Zeitraum im Wechsel |
|---|---|
| Halloween | 21.10. – 31.10. |
| Weihnachten und Weihnachtself | 14.12. – 26.12. |
| Silvester | 21.12. – 1.1. |
| Valentinstag | 4.2. – 14.2. |
| Ostern | 10 Tage vor Ostersonntag bis Ostermontag (2027: 18.3. – 29.3.) |

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.10.31-2_update.bin` | **Update** – im Browser unter *System → Firmware-Update über WLAN* einspielen (Einstellungen bleiben) |
| `WetterUhr_2026.10.31-2_erstinstallation.bin` | **komplette Firmware für die Erstinstallation per USB** an Adresse `0x0` (löscht Einstellungen) |
| `WetterUhr_2026.10.31-2_komplett.zip` | kompletter Quellcode für die Arduino IDE |

### Update
Browser → *System* → *Firmware-Update über WLAN* → `WetterUhr_2026.10.31-2_update.bin` → *Update installieren*.
Danach steht unter *Status → Firmware* `2026.10.31-2`.

### Erstinstallation
In Chrome oder Edge <https://espressif.github.io/esptool-js/> öffnen, ESP32-C3 per USB verbinden (**Connect**),
*Flash Address* `0x0`, `WetterUhr_2026.10.31-2_erstinstallation.bin` wählen, **Program**. Danach den Access-Point
**Wetteruhr-Setup** (Passwort `wetteruhr1`) öffnen und unter `http://192.168.4.1` einrichten.

### Build
arduino-cli, ESP32-Arduino-Core 3.3.12, *ESP32C3 Dev Module*, *USB CDC On Boot: Enabled*,
*Minimal SPIFFS (1.9MB APP with OTA)*, 4 MB Flash. Programmgröße 1,65 MB (84 % des Update-Bereichs).

Alle Änderungen: [CHANGELOG](https://github.com/smo0601-collab/RetroTVWecker-2.0-Zoll/blob/main/firmware/WetterUhr/CHANGELOG.md)

_Dieses Release wurde automatisch aus dem Ordner `releases/2026.10.31-2` erstellt._
