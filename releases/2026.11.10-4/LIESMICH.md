**Firmware 2026.11.10-4** – Fehlerbehebung für das CYD.

### Behoben
- Beim Öffnen des Menüs durch langes Halten (Spiele / Standby / Zurück) blieben Teile des letzten Uhrbilds
  (Sekunden, Animationen) über dem Menü stehen. Die Uhr zeichnet in diesem Moment jetzt nicht mehr.

RetroCYD bleibt bei **3.8** (Dateien zur Vollständigkeit dabei).

### Einspielen
Im Browser der Uhr: *System → Firmware-Update* → `WetterUhr_2026.11.10-4_CYD_update.bin`.

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.10-4_CYD_update.bin` | CYD: Uhr-Update |
| `WetterUhr_2026.11.10-4_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 3.8 per USB an `0x0` |
| `WetterUhr_2026.11.10-4_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.10-4_update.bin` / `_erstinstallation.bin` | ESP32-C3 (nur Versionsnummer neu) |
| `RetroCYD_3.8_update.bin`, `RetroCYD_3.8_komplett.zip` | Spiele (unverändert) |
| `WetterUhr_2026.11.10-4_komplett.zip` | Quellcode der Uhr |
