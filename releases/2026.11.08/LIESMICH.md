**Firmware 2026.11.08** für den RetroTV-Wecker (CYD 2,8 Zoll und ESP32-C3 SuperMini).

### Neu in dieser Version: Display-Anpassung im Browser
Für CYD-Platinen, bei denen das Bild **um 90° gedreht, spiegelverkehrt oder in falschen Farben** erscheint.

- Der Display-Controller (**ILI9341 oder ST7789**) wird beim Start automatisch erkannt. Viele CYDs haben einen ST7789;
  mit dem bisherigen ILI9341-Treiber war das Bild dann gedreht, gespiegelt und farbig falsch.
- Unter *System → Display-Anpassung* lässt sich alles von Hand einstellen, ohne neu zu kompilieren:
  Controller (automatisch / ILI9341 / ST7789), Achsen tauschen, links/rechts spiegeln, oben/unten spiegeln,
  Rot und Blau tauschen, Farben invertieren. Jede Änderung wirkt sofort nach *Speichern*.
- Unter *Status* stehen der erkannte Controller und seine Kennung.

### Dateien
| Datei | Board | Wofür |
|---|---|---|
| `WetterUhr_2026.11.08_CYD_update.bin` | **CYD 2,8 Zoll** | Update im Browser unter *System → Firmware-Update über WLAN* |
| `WetterUhr_2026.11.08_CYD_erstinstallation.bin` | **CYD 2,8 Zoll** | Erstinstallation per USB an Adresse `0x0` (löscht Einstellungen) |
| `WetterUhr_2026.11.08_update.bin` | ESP32-C3 | Update im Browser |
| `WetterUhr_2026.11.08_erstinstallation.bin` | ESP32-C3 | Erstinstallation per USB an Adresse `0x0` (löscht Einstellungen) |
| `WetterUhr_2026.11.08_komplett.zip` | beide | kompletter Quellcode für die Arduino IDE |

### So stellst du das Bild ein
1. Update einspielen (`…_CYD_update.bin`). Oft stimmt das Bild danach schon, weil der Controller erkannt wird.
2. Sonst im Browser *System → Display-Anpassung*: zuerst den **Controller** umstellen und speichern.
3. Ist es dann noch gedreht/gespiegelt: einzelne Häkchen setzen und jeweils speichern, bis die Uhrzeit richtig lesbar ist.
4. Falsche Farben: *Farben invertieren* bzw. *Rot und Blau tauschen*.

### Build
arduino-cli, ESP32-Arduino-Core 3.3.12, Partition Scheme *Minimal SPIFFS (1.9MB APP with OTA)*.
CYD: *ESP32 Dev Module*, 1,57 MB (79 %). ESP32-C3: *ESP32C3 Dev Module*, *USB CDC On Boot: Enabled*, 1,71 MB (86 %).

Alle Änderungen: [CHANGELOG](https://github.com/smo0601-collab/RetroTVWecker-2.0-Zoll/blob/main/firmware/WetterUhr/CHANGELOG.md)

_Dieses Release wurde automatisch aus dem Ordner `releases/2026.11.08` erstellt._
