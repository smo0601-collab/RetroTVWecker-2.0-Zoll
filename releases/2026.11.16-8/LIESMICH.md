**Firmware 2026.11.16-8** (RetroCYD unverändert 4.0.3) – Korrektur: Task-Watchdog beim Flugradar.

### Gemeldet
Neustart „Task-Watchdog“, Absturz-Spur „Netz: Flugradar“, kurz nach `Radar: HTTP -11` (Zeitlimit beim Lesen).

### Ursache
Beim Warten auf Daten dreht die HTTP-Bibliothek bis zu 8 s in einer Schleife ohne Pause. Seit 2026.11.16-3 lief der
gemeinsame Netz-Task mit Priorität 1 auf Kern 0 und ließ dabei den Leerlauf-Task nicht mehr zum Zug kommen – nach
5 s schlug der Task-Watchdog zu. Jetzt läuft der Netz-Task mit Priorität 0, wie früher der Radar-Task.

### Einspielen
- CYD: `WetterUhr_2026.11.16-8_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.16-8_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.16-8_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.16-8_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.16-8_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.16-8_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.16-8_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
