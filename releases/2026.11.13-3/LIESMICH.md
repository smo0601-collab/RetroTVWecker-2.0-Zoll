**Firmware 2026.11.13-3** und **RetroCYD 4.0.2** – Browser-Seite bleibt erreichbar (Speicher).

### Änderungen
- Der neue Ton-Bereich der Browser-Seite wird in kleinen Stücken gesendet (vorher ein großer Block, der bei
  knappem Speicher fehlschlagen konnte).
- Flugradar: Bei wenig freiem Speicher wird die dauerhaft offene TLS-Verbindung nach jedem Abruf geschlossen
  (gibt rund 40 KB frei) – die Uhr bleibt im Browser erreichbar.
- Ton-Task wieder mit kleinerem Stack.
- Status: freier Speicher zusätzlich mit dem größten zusammenhängenden Block; „Letzter Neustart“ mit Grund.

### Einspielen
1. Browser der Uhr → *System → Firmware-Update* → `RetroCYD_4.0.2_update.bin`
2. Danach → `WetterUhr_2026.11.13-3_CYD_update.bin` (ESP32-C3: `WetterUhr_2026.11.13-3_update.bin`)

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.13-3_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.2_update.bin` | CYD: Spiele-Update |
| `WetterUhr_2026.11.13-3_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.2 per USB an `0x0` |
| `WetterUhr_2026.11.13-3_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.13-3_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.13-3_komplett.zip`, `RetroCYD_4.0.2_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
