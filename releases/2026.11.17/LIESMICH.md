**Firmware 2026.11.17** (RetroCYD unverändert 4.0.3) – Ursache des Task-Watchdogs beim Flugradar behoben.

### Aus der Absturz-Spur
`Netz: Flugradar (Daten lesen), Watchdog: Kern 0 'net'` – der Netz-Task hing beim Lesen der Radar-Antwort.

### Ursache
Die Lesefunktionen der Bibliothek, die der JSON-Leser benutzt, warten auf das nächste Byte in einer Schleife ohne
Pause – bis zu 8 Sekunden. Stockt die Verbindung (kurz vorher stand jeweils „Radar: HTTP -1/403“), läuft der Task
ununterbrochen und der Watchdog startet die Uhr nach 5 Sekunden neu.

### Jetzt
- Eigener Leser für Radar, ISS und ISS-Besatzung: wartet mit kurzer Pause, hat eine Gesamtfrist (Radar 10 s, ISS 6 s)
  und entpackt „chunked“-Antworten selbst.
- Wetter, Schulferien und Ortssuche fordern Antworten ohne „chunked“ an (deren Blockköpfe liest die Bibliothek
  ebenfalls ohne Pause).

### Einspielen
- CYD: `WetterUhr_2026.11.17_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.17_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.17_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.17_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.17_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.17_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.17_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
