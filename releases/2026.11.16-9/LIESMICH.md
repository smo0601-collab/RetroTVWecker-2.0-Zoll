**Firmware 2026.11.16-9** (RetroCYD unverändert 4.0.3) – Task-Watchdog beim Flugradar: Netz-Task ohne Kernbindung, genauere Spur.

### Gemeldet
Auch mit 2026.11.16-8 nach rund einer Stunde Neustart durch den Task-Watchdog – wieder kurz nachdem das Flugradar
seine Verbindung neu aufgebaut hatte.

### Änderungen
- Der Netz-Task ist nicht mehr fest an Kern 0 gebunden (so lief früher der Radar-Task, ohne Watchdog-Meldungen).
- **Diagnose:** Schlägt der Watchdog zu, merkt sich die Uhr, welche Tasks gerade auf Kern 0 und 1 liefen. Das steht
  danach in *Status → Absturz-Spur* und im Protokoll (`/log`). Die Radar-Spur unterscheidet jetzt
  Verbindung/Anfrage, Daten lesen und Auswerten.

### Einspielen
- CYD: `WetterUhr_2026.11.16-9_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.16-9_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.16-9_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.16-9_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.16-9_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.16-9_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.16-9_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
