**Firmware 2026.11.11-2** und **RetroCYD 3.9** – beim Firmware-Update werden nur passende Dateien angenommen,
und das richtige Programm spielt sie automatisch ein.

### So funktioniert es jetzt
- Die Datei wird **am Inhalt erkannt** (der Dateiname ist egal), noch bevor etwas übertragen wird.
- **Abgelehnt** (mit Erklärung): Erstinstallations-Dateien mit Bootloader (`*_erstinstallation.bin`, nur per USB),
  Dateien für das andere Board (CYD ↔ ESP32-C3), sehr alte Versionen ohne Kennung, fremde Dateien.
- **Im Browser der Uhr:**
  - Uhr-Datei → Uhr-Update (bei installierten Spielen automatisch über RetroCYD).
  - RetroCYD-Datei → wird neben der Uhr installiert, danach auf Wunsch gleich die Spiele starten.
- **Im Browser von RetroCYD:**
  - Uhr-Datei → Uhr-Update.
  - RetroCYD-Datei → die Seite startet die Uhr, schickt die Datei dorthin und startet danach wieder die Spiele.
- Uhr und RetroCYD prüfen zusätzlich selbst Chip, Programm-Kennung und (am Ende) das richtige Programm.

### Einspielen (Reihenfolge)
1. Browser der Uhr → *System → Firmware-Update* → `RetroCYD_3.9_update.bin`
2. Danach → `WetterUhr_2026.11.11-2_CYD_update.bin` (ESP32-C3: `WetterUhr_2026.11.11-2_update.bin`)

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.11-2_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_3.9_update.bin` | CYD: Spiele-Update |
| `WetterUhr_2026.11.11-2_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 3.9 per USB an `0x0` |
| `WetterUhr_2026.11.11-2_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.11-2_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.11-2_komplett.zip`, `RetroCYD_3.9_komplett.zip` | Quellcode |
