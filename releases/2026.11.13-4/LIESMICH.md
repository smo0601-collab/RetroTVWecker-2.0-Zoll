**Firmware 2026.11.13-4** und **RetroCYD 4.0.3** – weniger Störgeräusche im Lautsprecher (CYD).

### Was war
Der Lautsprecher-Ausgang (GPIO 26) wurde in Ruhe aktiv auf 0 V gehalten. Darüber koppeln Störungen von WLAN und
Display in den Verstärker – als dauerndes Brummen/Zirpen hörbar (in RetroCYD kaum, dort ist das WLAN meist aus).

### Jetzt
- In Ruhe wird der Ausgang abgeschaltet (hochohmig mit schwachem Pull-down) und erst beim nächsten Klang wieder
  eingeschaltet – gleiche Spannung, kein Knacken.
- Zum Vergleichen abschaltbar: *Summer & Wecker → Lautsprecher-Ausgang in Ruhe abschalten*.

### Einspielen
1. Browser der Uhr → *System → Firmware-Update* → `RetroCYD_4.0.3_update.bin`
2. Danach → `WetterUhr_2026.11.13-4_CYD_update.bin` (ESP32-C3: `WetterUhr_2026.11.13-4_update.bin`)

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.13-4_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele-Update |
| `WetterUhr_2026.11.13-4_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.13-4_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.13-4_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.13-4_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
