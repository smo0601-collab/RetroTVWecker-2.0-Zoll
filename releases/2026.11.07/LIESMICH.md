**Firmware 2026.11.07** für den RetroTV-Wecker – jetzt auch auf dem **CYD 2,8 Zoll** (ESP32-2432S028R, „Cheap Yellow Display“).

### Neu in dieser Version
- Läuft auf dem **CYD 2,8 Zoll** ohne Löten und Verdrahten: Display ILI9341 320 × 240, gleiches Bild wie bisher.
- **Touchscreen** statt Touch-Taster: tippen = nächstes Design, doppelt tippen = Laufschrift, 1 s halten = Standby,
  klingelnder Wecker wird mit jeder Berührung abgestellt.
- **Echte Helligkeitsregelung** über das Backlight (GPIO 21), Standby schaltet das Licht ganz aus.
- **Ton** über den Lautsprecher-Anschluss „SPEAK“ (kleiner 8-Ω-Lautsprecher, optional).
- **RGB-LED** blinkt rot, solange der Wecker klingelt.
- Firmware-Update über WLAN prüft, ob die Datei zum Board passt. Die Status-Seite zeigt die Hardware.
- Der ESP32-C3 SuperMini wird weiter unterstützt (gleicher Quellcode, das Board wird automatisch erkannt).

### Dateien
| Datei | Board | Wofür |
|---|---|---|
| `WetterUhr_2026.11.07_CYD_erstinstallation.bin` | **CYD 2,8 Zoll** | Erstinstallation per USB an Adresse `0x0` (löscht Einstellungen) |
| `WetterUhr_2026.11.07_CYD_update.bin` | **CYD 2,8 Zoll** | Update im Browser unter *System → Firmware-Update über WLAN* |
| `WetterUhr_2026.11.07_erstinstallation.bin` | ESP32-C3 | Erstinstallation per USB an Adresse `0x0` (löscht Einstellungen) |
| `WetterUhr_2026.11.07_update.bin` | ESP32-C3 | Update im Browser |
| `WetterUhr_2026.11.07_komplett.zip` | beide | kompletter Quellcode für die Arduino IDE |

### Erstinstallation CYD
In Chrome oder Edge <https://espressif.github.io/esptool-js/> öffnen, CYD per USB verbinden (**Connect**),
*Flash Address* `0x0`, `WetterUhr_2026.11.07_CYD_erstinstallation.bin` wählen, **Program**. Danach den Access-Point
**Wetteruhr-Setup** (Passwort `wetteruhr1`) öffnen und unter `http://192.168.4.1` einrichten.
Findet der Browser das Board nicht, fehlt eventuell der USB-Treiber für den CH340-Chip.

Bild auf dem Kopf? Im Browser *Anzeige → Darstellung → Ausrichtung* umstellen. Falsche Farben oder weißes Bild
(CYD-Variante mit zwei USB-Buchsen und ST7789): im Quellcode `CYD_ST7789 1` setzen und selbst kompilieren.

### Update ESP32-C3
Browser → *System* → *Firmware-Update über WLAN* → `WetterUhr_2026.11.07_update.bin` → *Update installieren*.

### Build
arduino-cli, ESP32-Arduino-Core 3.3.12, Partition Scheme *Minimal SPIFFS (1.9MB APP with OTA)*, 4 MB Flash.
CYD: *ESP32 Dev Module*, 1,56 MB (79 %). ESP32-C3: *ESP32C3 Dev Module*, *USB CDC On Boot: Enabled*, 1,70 MB (86 %).

Alle Änderungen: [CHANGELOG](https://github.com/smo0601-collab/RetroTVWecker-2.0-Zoll/blob/main/firmware/WetterUhr/CHANGELOG.md)

_Dieses Release wurde automatisch aus dem Ordner `releases/2026.11.07` erstellt._
