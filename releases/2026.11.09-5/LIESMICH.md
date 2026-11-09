**Firmware 2026.11.09-5** und **RetroCYD 3.5** – Uhr-Update direkt im Browser der Uhr, auch wenn die Spiele installiert sind (CYD 2,8 Zoll).

### Neu
- **Uhr-Update wie gewohnt:** *System → Firmware-Update* → `WetterUhr_…_CYD_update.bin` → *Update installieren*.
  Die Uhr startet dafür kurz RetroCYD im Update-Modus (Anzeige „UHR-UPDATE“), die Seite wartet, bis RetroCYD unter
  derselben Adresse bereit ist, und schickt die Datei automatisch dorthin. RetroCYD schreibt die neue Uhr und startet
  sie. Dauer etwa eine halbe Minute – Seite nicht schließen.
- Kein Umweg mehr über das RetroCYD-Setup und keine Fehlermeldung „Die Spiele belegen den zweiten Programmbereich“.
- RetroCYD 3.5: Update-Modus für die Uhr (nur mit Einmal-Schlüssel von der Uhr, ohne Bluetooth; nach 3 Minuten ohne
  Datei zurück zur Uhr).

### Einmalig so einspielen (deine Uhr kennt den neuen Ablauf noch nicht)
1. Im Browser der Uhr: *System → Firmware-Update* → `RetroCYD_3.5_update.bin` (geht direkt, die Uhr läuft weiter).
2. Uhr-Update **einmal** über das RetroCYD-Setup: Bildschirm lange halten → *SETUP / WLAN* → im Browser die angezeigte
   Adresse öffnen → Firmware-Update → `WetterUhr_2026.11.09-5_CYD_update.bin`. RetroCYD nutzt dafür das WLAN der Uhr.
3. Ab jetzt gehen alle Updates direkt im Browser der Uhr.

Alternativ: `WetterUhr_2026.11.09-5_CYD_mitSpielen_erstinstallation.bin` per USB an `0x0` (löscht Einstellungen).

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.09-5_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_3.5_update.bin` | CYD: Spiele-Update |
| `WetterUhr_2026.11.09-5_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 3.5 per USB an `0x0` |
| `WetterUhr_2026.11.09-5_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.09-5_update.bin` / `_erstinstallation.bin` | ESP32-C3 (nur Versionsnummer neu) |
| `WetterUhr_2026.11.09-5_komplett.zip`, `RetroCYD_3.5_komplett.zip` | Quellcode |

Alle Änderungen: [CHANGELOG](https://github.com/smo0601-collab/RetroTVWecker-2.0-Zoll/blob/main/firmware/WetterUhr/CHANGELOG.md)

_Dieses Release wurde automatisch aus dem Ordner `releases/2026.11.09-5` erstellt._
