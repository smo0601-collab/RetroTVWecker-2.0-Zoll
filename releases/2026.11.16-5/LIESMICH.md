**Firmware 2026.11.16-5** (RetroCYD unverändert 4.0.3) – Wetterabruf: Fehler sichtbar, Namensauflösung abgesichert.

### Gemeldet
Es werden keine Wetterdaten geladen; die Browser-Seite ist erreichbar, die Uhrzeit war falsch.

### Neu
- *Status → Wetter* zeigt den **letzten Fehler** des Abrufs und den DNS-Server. Das Protokoll (`/log`) notiert
  zusätzlich Speicher und DNS zum Zeitpunkt des Fehlers.
- **Namensauflösung abgesichert:** Nach dem Verbinden (und nach dem Abschalten des Notfall-Access-Points) trägt die
  Uhr Ersatz-DNS-Server ein (1.1.1.1, 8.8.8.8). Fehlt der DNS-Server des Routers, wird der Router selbst eingetragen.
  Ohne Namensauflösung scheitern Wetter, Zeitabgleich, Radar und ISS gleichzeitig.

### Einspielen
- CYD: `WetterUhr_2026.11.16-5_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.16-5_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.16-5_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.16-5_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.16-5_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.16-5_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.16-5_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
