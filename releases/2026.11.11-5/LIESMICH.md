**Firmware 2026.11.11-5** (RetroCYD unverändert 3.9.1) – Korrektur: Drehung um 180 Grad beim ESP32-C3.

### Was war falsch
Seit 2026.11.08 (neue Display-Anpassung) waren auf der Wetteruhr mit ESP32-C3 und 2,0-Zoll-Display
„Normal“ und „180 Grad gedreht“ vertauscht. Die Einstellung schien deshalb nicht zu wirken.
Jetzt ist es wieder wie bis 2026.11.07. Wer zum Ausgleich „180 Grad“ gewählt hatte, stellt wieder auf „Normal“.
Beim CYD ändert sich nichts.

### Einspielen
- ESP32-C3: Browser der Uhr → *System → Firmware-Update* → `WetterUhr_2026.11.11-5_update.bin`
- CYD (nur der Vollständigkeit halber, inhaltlich gleich wie 2026.11.11-4): `WetterUhr_2026.11.11-5_CYD_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.11-5_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.11-5_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_3.9.1_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.11-5_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 3.9.1 per USB an `0x0` |
| `WetterUhr_2026.11.11-5_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.11-5_komplett.zip`, `RetroCYD_3.9.1_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
