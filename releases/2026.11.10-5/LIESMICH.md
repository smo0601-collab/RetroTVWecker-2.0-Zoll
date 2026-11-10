**Firmware 2026.11.10-5** und **RetroCYD 3.8.1** – Fortschrittsanzeige beim Update.

### Behoben / neu
- **RetroCYD 3.8.1:** Beim Uhr-Update über RetroCYD (Anzeige „UHR-UPDATE“) blieb der Balken am Display leer und es
  stand keine Prozentzahl da – die Fortschrittsanzeige war in diesem Modus nicht angemeldet. Jetzt laufen Balken und
  Prozent mit; 0 % erscheint sofort.
- **Uhr:** Im Browser zeigt das Firmware-Update jetzt zusätzlich zur Prozentzahl einen grünen Balken.

### Einspielen (Reihenfolge wichtig)
1. Im Browser der Uhr: *System → Firmware-Update* → `RetroCYD_3.8.1_update.bin`
2. Danach: *System → Firmware-Update* → `WetterUhr_2026.11.10-5_CYD_update.bin` – hier ist der Balken am CYD schon neu.

Alternativ: `WetterUhr_2026.11.10-5_CYD_mitSpielen_erstinstallation.bin` per USB an `0x0` (löscht Einstellungen).

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.10-5_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_3.8.1_update.bin` | CYD: Spiele-Update |
| `WetterUhr_2026.11.10-5_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 3.8.1 per USB an `0x0` |
| `WetterUhr_2026.11.10-5_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.10-5_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.10-5_komplett.zip`, `RetroCYD_3.8.1_komplett.zip` | Quellcode |
