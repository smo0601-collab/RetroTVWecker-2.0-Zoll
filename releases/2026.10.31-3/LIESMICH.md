**Firmware 2026.10.31-3** für den RetroTV-Wecker (ESP32-C3 SuperMini, 2,0-Zoll-Display ST7789).

### Neu in dieser Version: fünf Sprachen
- Unter *Anzeige → Darstellung → Sprache*: **Deutsch, English, Français, Español, Italiano**.
- Übersetzt sind das **Display** (alle Designs, Wochentage, Datum, Wettertexte, Feiertags-Grüße und Countdowns,
  Badminton-Ansagen, Flugradar, ISS-Tracker, Sonne und Mond, Update-Anzeige) und die **komplette Browser-Seite**
  einschließlich Design-Namen, Hinweisen und Fehlermeldungen.
- Datum außerhalb von Deutsch als `31/10`; lange Überschriften werden automatisch kleiner, damit sie aufs Display passen.
- ISS-Tracker: „Lade ISS-Daten“ verdeckt nicht mehr das Datum.

Nach dem Update bleibt die Uhr auf Deutsch, bis du eine andere Sprache wählst und speicherst.

![Ein Design in fünf Sprachen](https://raw.githubusercontent.com/smo0601-collab/RetroTVWecker-2.0-Zoll/main/docs/images/languages.png)

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.10.31-3_update.bin` | **Update** – im Browser unter *System → Firmware-Update über WLAN* einspielen (Einstellungen bleiben) |
| `WetterUhr_2026.10.31-3_erstinstallation.bin` | **komplette Firmware für die Erstinstallation per USB** an Adresse `0x0` (löscht Einstellungen) |
| `WetterUhr_2026.10.31-3_komplett.zip` | kompletter Quellcode für die Arduino IDE |

### Update
Browser → *System* → *Firmware-Update über WLAN* → `WetterUhr_2026.10.31-3_update.bin` → *Update installieren*.
Danach steht unter *Status → Firmware* `2026.10.31-3`.

### Erstinstallation
In Chrome oder Edge <https://espressif.github.io/esptool-js/> öffnen, ESP32-C3 per USB verbinden (**Connect**),
*Flash Address* `0x0`, `WetterUhr_2026.10.31-3_erstinstallation.bin` wählen, **Program**. Danach den Access-Point
**Wetteruhr-Setup** (Passwort `wetteruhr1`) öffnen und unter `http://192.168.4.1` einrichten.

### Build
arduino-cli, ESP32-Arduino-Core 3.3.12, *ESP32C3 Dev Module*, *USB CDC On Boot: Enabled*,
*Minimal SPIFFS (1.9MB APP with OTA)*, 4 MB Flash. Programmgröße 1,70 MB (86 % des Update-Bereichs).

Alle Änderungen: [CHANGELOG](https://github.com/smo0601-collab/RetroTVWecker-2.0-Zoll/blob/main/firmware/WetterUhr/CHANGELOG.md)

_Dieses Release wurde automatisch aus dem Ordner `releases/2026.10.31-3` erstellt._
