**Firmware 2026.11.09-3** für den RetroTV-Wecker (CYD 2,8 Zoll und ESP32-C3 SuperMini).

### Neu in dieser Version
- **Sechs Designs entfernt:** Labyrinth, Zahnrad-Werk, Blockwelt, Familie, Weihnachtself und Squishy Dumpling.
  Jetzt 27 Designs (2 klassische + 25 Themen).
- Die Nummern ab 4 sind nachgerückt. Gespeicherte Auswahl und Wechsel-Liste rechnet die Uhr beim ersten Start
  selbst um; war ein entferntes Design ausgewählt, kommt ein ähnliches (Labyrinth/Blockwelt → Abenteuer,
  Zahnrad-Werk → Code-Regen, Familie/Squishy Dumpling → Dackelwelpe, Weihnachtself → Weihnachten).
- Weihnachten kommt im Feiertags-Wechsel jetzt allein. Die Kategorie „Heimat & Familie“ entfällt.
- Programm rund 35 KB kleiner.

### Dateien
| Datei | Board | Wofür |
|---|---|---|
| `WetterUhr_2026.11.09-3_CYD_update.bin` | CYD 2,8 Zoll | Uhr-Update (mit installierten Spielen: im **RetroCYD-Setup** einspielen, sonst im Browser der Uhr) |
| `WetterUhr_2026.11.09-3_CYD_mitSpielen_erstinstallation.bin` | CYD 2,8 Zoll | Uhr + RetroCYD 3.3 per USB an `0x0` (löscht Einstellungen) |
| `WetterUhr_2026.11.09-3_CYD_erstinstallation.bin` | CYD 2,8 Zoll | nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.09-3_update.bin` | ESP32-C3 | Update im Browser |
| `WetterUhr_2026.11.09-3_erstinstallation.bin` | ESP32-C3 | Erstinstallation per USB an `0x0` |
| `WetterUhr_2026.11.09-3_komplett.zip` | beide | Quellcode |

RetroCYD (Spiele) bleibt bei Version 3.3, siehe Release v2026.11.09-2.

### Build
arduino-cli, ESP32-Arduino-Core 3.3.12, *Minimal SPIFFS (1.9MB APP with OTA)*.
CYD: *ESP32 Dev Module*, 1,55 MB (78 %). ESP32-C3: *ESP32C3 Dev Module*, *USB CDC On Boot: Enabled*, 1,67 MB (85 %).

Alle Änderungen: [CHANGELOG](https://github.com/smo0601-collab/RetroTVWecker-2.0-Zoll/blob/main/firmware/WetterUhr/CHANGELOG.md)

_Dieses Release wurde automatisch aus dem Ordner `releases/2026.11.09-3` erstellt._
