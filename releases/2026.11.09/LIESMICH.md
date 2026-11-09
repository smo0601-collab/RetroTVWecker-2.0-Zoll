**Firmware 2026.11.09** für den RetroTV-Wecker – auf dem **CYD 2,8 Zoll** jetzt mit den **RetroCYD-Spielen** als zweitem Programm.

### Neu in dieser Version (nur CYD)
- **Uhr und Spiele auf einem Gerät:** RetroCYD 3.2 (16 Spiele, Xbox-Controller per Bluetooth) liegt als zweites
  Programm neben der Uhr.
- **Beim Einschalten** am Bildschirm **Uhr** oder **Spiele** antippen; ohne Auswahl startet nach 6 s die Uhr.
- **Lange halten** in der Uhr: Menü mit *Spiele*, *Standby*, *Zurück*.
  **Lange halten** in RetroCYD (Bildschirm oder BOOT): *ZUR UHR*, *SETUP / WLAN*, *ZURÜCK* – im Spiel
  *ZUR UHR*, *SPIELMENÜ*, *WEITERSPIELEN* (tippen = weiter, halten = wählen, oder Controller).
  Im Spielemenü gibt es außerdem den Punkt *ZUR UHR*.
- **Touch einrichten:** Beim ersten Start mit Spielen drei rote Kreuze antippen; später unter
  *System → Spiele (RetroCYD) → Touch kalibrieren*.
- RetroCYD übernimmt beim ersten Start den Display-Chip und das WLAN der Uhr.
- ESP32-C3: keine Änderung an der Funktion.

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.09_CYD_mitSpielen_erstinstallation.bin` | **CYD: Uhr + Spiele**, per USB an `0x0` (löscht Einstellungen) |
| `WetterUhr_2026.11.09_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_3.2_update.bin` | CYD: Spiele installieren/aktualisieren (im Browser der Uhr) |
| `WetterUhr_2026.11.09_CYD_erstinstallation.bin` | CYD: nur Uhr, per USB an `0x0` |
| `WetterUhr_2026.11.09_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.09_komplett.zip`, `RetroCYD_3.2_komplett.zip` | Quellcode |

### So kommen die Spiele auf deine laufende Uhr
1. Zuerst die Uhr aktualisieren: Browser der Uhr → *System → Firmware-Update* → `WetterUhr_2026.11.09_CYD_update.bin`.
2. Danach dort `RetroCYD_3.2_update.bin` einspielen. Die Uhr läuft weiter; unter *Status* steht „Spiele: installiert“.
3. *System → Spiele starten*, beim nächsten Einschalten die Auswahl am Bildschirm oder lange halten → *Spiele*.

### Spätere Updates (über Kreuz)
- **RetroCYD-Update** im Browser der **Uhr**.
- **Uhr-Update** im **RetroCYD-Setup** (lange halten → SETUP / WLAN → Browser → Firmware-Update).
- Ein Programm kann sich nicht selbst überschreiben; beide Seiten erkennen die Datei und lehnen die falsche ab.

### Hinweise
- Während gespielt wird, läuft die Uhr nicht (kein Wecker).
- Noch nicht auf echter Hardware getestet: Startauswahl, Halte-Menüs, Touch-Kalibrierung und Updates über Kreuz.

### Build
Uhr: arduino-cli, ESP32-Arduino-Core 3.3.12, *ESP32 Dev Module*, *Minimal SPIFFS (1.9MB APP with OTA)*, 1,58 MB (80 %).
RetroCYD: Boardpaket *esp32_bluepad32* 4.1, LovyanGFX, gleiches Partitionsschema, 1,44 MB (73 %).
ESP32-C3: 1,72 MB (87 %).

Alle Änderungen: [CHANGELOG](https://github.com/smo0601-collab/RetroTVWecker-2.0-Zoll/blob/main/firmware/WetterUhr/CHANGELOG.md)

_Dieses Release wurde automatisch aus dem Ordner `releases/2026.11.09` erstellt._
