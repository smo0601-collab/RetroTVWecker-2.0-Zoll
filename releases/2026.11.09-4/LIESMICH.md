**Firmware 2026.11.09-4** und **RetroCYD 3.4** – WLAN bleibt beim Wechsel zwischen Uhr und Spielen erhalten (CYD 2,8 Zoll).

### Was war los?
Uhr und RetroCYD teilen sich auf dem CYD einen kleinen Einstellungsspeicher (NVS, 20 KB). Läuft er voll oder kann ihn
das andere Programm nicht lesen, löscht der Start ihn komplett – damit waren WLAN und Einstellungen weg.

### Neu
- **Sicherung außerhalb des NVS:** Die Uhr sichert WLAN, alle Einstellungen und die Touch-Kalibrierung zusätzlich in
  einem sonst ungenutzten Flash-Bereich und stellt sie beim nächsten Start von selbst wieder her.
- **Weniger Last auf dem gemeinsamen Speicher:** Der WLAN-Treiber der Uhr schreibt nichts mehr ins NVS.
- **RetroCYD 3.4** nutzt im Setup automatisch das WLAN der Uhr (auch aus der Sicherung), wenn das eigene fehlt
  oder nicht erreichbar ist.
- **Status** zeigt die Belegung des Einstellungsspeichers und ob aus der Sicherung wiederhergestellt wurde.

### Einspielen (CYD mit Spielen)
1. **Uhr-Update** im RetroCYD-Setup: lange halten → *SETUP / WLAN* → Browser → Firmware-Update →
   `WetterUhr_2026.11.09-4_CYD_update.bin`. Danach startet die Uhr.
2. Ist das WLAN der Uhr schon weg: einmal über den Access-Point **Wetteruhr-Setup** neu eintragen und speichern –
   ab jetzt ist es gesichert.
3. **RetroCYD-Update** im Browser der Uhr: *System → Firmware-Update* → `RetroCYD_3.4_update.bin`.

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.09-4_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_3.4_update.bin` | CYD: Spiele-Update (im Browser der Uhr) |
| `WetterUhr_2026.11.09-4_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 3.4 per USB an `0x0` |
| `WetterUhr_2026.11.09-4_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.09-4_update.bin` / `_erstinstallation.bin` | ESP32-C3 (nur Versionsnummer neu) |
| `WetterUhr_2026.11.09-4_komplett.zip`, `RetroCYD_3.4_komplett.zip` | Quellcode |

Alle Änderungen: [CHANGELOG](https://github.com/smo0601-collab/RetroTVWecker-2.0-Zoll/blob/main/firmware/WetterUhr/CHANGELOG.md)

_Dieses Release wurde automatisch aus dem Ordner `releases/2026.11.09-4` erstellt._
