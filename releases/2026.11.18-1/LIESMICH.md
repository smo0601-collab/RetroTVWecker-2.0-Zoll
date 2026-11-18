**Firmware 2026.11.18-1** (RetroCYD unverändert 4.0.3) – Protokoll-Fehler behoben, mehr Spuren für Speicher-Engpässe.

### Anlass: Auswertung des 22-Stunden-Protokolls (CYD, 2026.11.17-6)
Kein Absturz, kein Neustart, Wetter durchgehend ok, WLAN stabil (−38 bis −48 dBm). Gefunden und geändert:

- **Zusammengeklebte Zeilen:** Die Zustandszeile war länger als der Zeilenpuffer (200 Zeichen). Sie wurde samt Zeilenende abgeschnitten, deshalb hing die nächste Zeile direkt dran. Der Puffer ist jetzt größer, und das Zeilenende bleibt immer erhalten.
- **Falsches Datum beim Start („20.05.“):** Bis zum ersten NTP-Abgleich steht jetzt die Zeit seit dem Start im Protokoll.
- **ISS-Ausfallzeiten zu groß** (z. B. „7976 s nicht erreichbar“): Mitgezählt wurden auch die Stunden, in denen das ISS-Design gar nicht lief. Die Zählung beginnt jetzt bei jedem Erscheinen des Designs neu.
- **Speicher-Tiefstand 152 Bytes** (18.11. gegen 11 Uhr, beim Flugradar): Die Ursache steht nicht im Protokoll. Ab dieser Version werden Speicheranforderungen, die scheitern, mit Größe, Task und Netz-Schritt protokolliert, ebenso jeder neue Tiefstand unter 8 KB. Das Flugradar baut außerdem eine neue TLS-Verbindung nur noch auf, wenn ein Speicherblock von mindestens 36 KB frei ist.
- **Bild-Hänger von rund 0,6 s** (etwa stündlich, meist beim Flugradar): Bilder über 0,3 s werden jetzt mit Design, Kacheln und Netz-Schritt protokolliert, um die Ursache einzugrenzen.

### Einspielen
- CYD: `WetterUhr_2026.11.18-1_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.18-1_update.bin` (der C3 lief noch mit 2026.11.17-2)

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.18-1_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.18-1_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.18-1_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.18-1_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.18-1_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
