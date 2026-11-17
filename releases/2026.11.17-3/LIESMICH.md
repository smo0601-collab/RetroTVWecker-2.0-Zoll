**Firmware 2026.11.17-3** (RetroCYD unverändert 4.0.3) – Optimierung: weniger RAM, Flugradar mit konstantem Speicher, Messwerte im Protokoll.

Ausführlicher Bericht mit allen Messwerten: [`docs/Optimierung_2026-11.md`](../../docs/Optimierung_2026-11.md)

### Änderungen
- **Flugradar** liest die Antwort Flugzeug für Flugzeug statt die ganze Liste auf einmal.
  Am PC gemessen (150 Flugzeuge): JSON-Speicherspitze 65 263 B → 2 675 B.
- **Gemeinsamer Arbeitsspeicher der Designs:** 11 Designs teilen sich 8 KB statt eigener Felder.
  Statischer RAM: ESP32-C3 80 992 → 71 560 B, CYD 93 416 → 84 008 B (je rund 9,4 KB mehr freier Speicher).
- **Protokoll:** alle 5 Minuten eine Zeile „Messung“ (Bilder/s, mittlere/maximale Bildzeit, Kacheln je Bild,
  Schleifen/s).

### Einspielen
- CYD: `WetterUhr_2026.11.17-3_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.17-3_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.17-3_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.17-3_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.17-3_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.17-3_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.17-3_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Am PC geprüft (Designs, Flugradar-Leser), nicht auf echter Hardware getestet.
