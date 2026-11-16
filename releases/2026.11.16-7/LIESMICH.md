**Firmware 2026.11.16-7** (RetroCYD unverändert 4.0.3) – Wetter neben dem Flugradar, ISS-Aussetzer, Selbstheilung.

### Aus dem Protokoll der Uhr
- **Wetter beim Flugradar:** jede Minute `TLS -32512 SSL - Memory allocation failed` – die offene Radar-Verbindung
  belegt rund 40 KB. Jetzt wird sie vor jedem Wetter- und Ferienabruf geschlossen und danach neu aufgebaut.
- **Selbstheilung löste grundlos aus** (14 s nach einem erfolgreichen Abruf): Rechenfehler (Unterlauf) – behoben.
- **ISS-Tracker:** Der Dienst open-notify fällt oft kurz aus. Statt alle 5 s erneut zu fragen und jedes Mal
  „ISS: Fehler“ zu schreiben, fragt die Uhr nach Fehlern seltener (10 s, 20 s, 40 s, dann jede Minute). Im
  Protokoll steht nur der erste Fehler mit HTTP-Code und später „wieder ok“. Bei Aussetzern bis 2 Minuten rechnet
  die Anzeige die Bahn weiter.

### Einspielen
- CYD: `WetterUhr_2026.11.16-7_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.16-7_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.16-7_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.16-7_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.16-7_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.16-7_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.16-7_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
